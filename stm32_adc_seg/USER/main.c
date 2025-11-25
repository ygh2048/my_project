#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "adc.h"
#include "seg.h"
#include "beep.h"
#include "timer.h"

#define SAMPLE_INTERVAL_MS 50   // ADC采样间隔(ms)
#define BLINK_INTERVAL_MS 500   // 报警闪烁频率(ms)
#define ALARM_LOW_MV 1000       // 异常电压下限(mV)
#define ALARM_HIGH_MV 2500      // 异常电压上限(mV)

// 全局变量，用于定时器中断访问
volatile uint32_t sample_timer = 0;
volatile uint32_t blink_timer = 0;
volatile uint32_t debug_timer = 0;
volatile uint8_t alarm_active = 0;
volatile uint8_t alarm_muted = 0;   // 报警消音标志：仅当前这次报警有效，下一次重新触发自动清除

typedef enum
{
    DISPLAY_RAW = 0,
    DISPLAY_VOLT = 1,
    DISPLAY_TEST = 2
} display_mode_t;

// 测试函数：逐个测试引脚映射
void SEG_TestPinMapping(uint32_t test_timer)
{
    // 每2秒切换一个段，依次点亮PA1,PA2,PA5,PA11,PB12,PB13,PB14,PB15
    uint8_t seg_index = (test_timer / 2000) % 8;
    uint8_t pattern = (1 << seg_index);  // 只点亮一个段
    
    // 直接设置测试模式和测试图案
    SEG_TestSingleSegment(seg_index);
}

int main(void)
{
    // 局部变量定义
    uint16_t adc_raw = 0;
    uint32_t voltage_mv = 0;
    display_mode_t mode = DISPLAY_RAW;
    u8 key = 0;
    uint8_t new_alarm = 0;

    delay_init();                        // 延时函数初始化
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    // 串口初始化
    uart_init(115200);                   // 串口初始化
    printf("\r\n=== STM32F103 ADC Voltage Monitor ===\r\n");
    printf("UART Initialized Successfully\r\n");
    
    LED_Init();                          // LED初始化
    KEY_Init();                          // 按键初始化
    BEEP_Init();                         // 蜂鸣器初始化
    ADC1_Init();                         // ADC初始化
    SEG_Init();                          // 数码管初始化
    
    // 定时器3初始化，1ms中断
    // 72MHz / 7200 = 10kHz计数频率
    // 10kHz / 10 = 1kHz中断频率 (1ms)
    TIM3_Int_Init(9, 7199);

    printf("System Ready\r\n");
    printf("\r\n=== Digit Display Test ===\r\n");
    printf("Each position will display 0-9, 200ms per digit\r\n\r\n");
    
    // 每位依次显示0-9
    {
        uint8_t pos, num;
        const char* pos_names[4] = {"Ones", "Tens", "Hundreds", "Thousands"};
        
        for (pos = 0; pos < 4; pos++)
        {
            printf("Testing position %u (%s):\r\n", pos, pos_names[pos]);
            
            for (num = 0; num <= 9; num++)
            {
                SEG_DisplaySingleDigit(pos, num);
                printf("  %u ", num);
                if (num == 4) printf("\r\n  ");

                // 延迟200ms (定时器中断会在后台刷新数码管)
                delay_ms(200);
            }
            printf("\r\n");
        }
        
        printf("\r\nTest Complete!\r\n");
    }

    // 测试完成后显示1234
    printf("Displaying 1234...\r\n");
    SEG_DisplayRaw(1234);
    LED0 = 1;
    LED1 = 1;

    // 等待5秒观察
    delay_ms(5000);
    
    // 然后显示电压
    printf("Starting normal ADC display...\r\n");
    sample_timer = 0;
    blink_timer = 0;
    debug_timer = 0;
    mode = DISPLAY_VOLT;

    while (1)
    {
        // 数码管刷新在定时器中断中进行，此处不再调用SEG_Task()
        // 也不需要delay_ms(1)来维持循环频率，循环全速运行
        
        key = KEY_Scan(0);  // 按键扫描
        
        // 计时器在中断中自动增加，无需手动增加

        if (sample_timer >= SAMPLE_INTERVAL_MS)
        {
            adc_raw = ADC1_ReadAverage(ADC_CHANNEL_VSENSE, 8);
            voltage_mv = ADC1_ToMilliVolts(adc_raw);
            
            if (mode == DISPLAY_VOLT)
                SEG_DisplayVoltage(voltage_mv);
            else
                SEG_DisplayRaw(adc_raw);
            
            // 报警检测
            new_alarm = (voltage_mv < ALARM_LOW_MV) || (voltage_mv > ALARM_HIGH_MV);
            if (new_alarm && !alarm_active)
            {
                printf("ALARM! Voltage: %u.%02uV, Threshold: 1.00V~2.50V\r\n", 
                       voltage_mv / 1000, (voltage_mv % 1000) / 10);
                alarm_muted = 0; // 新报警触发时取消之前的消音
            }
            alarm_active = new_alarm;
            if (!alarm_active) {
                // 报警条件恢复正常，清除消音标志，为下一次报警做准备
                if (alarm_muted) {
                    printf("Alarm condition cleared; mute reset.\r\n");
                }
                alarm_muted = 0;
            }
            
            sample_timer = 0;
        }

        if (debug_timer >= 1000)
        {
            printf("[ADC] Raw=%u, Voltage=%u.%02uV, Alarm=%s, Muted=%s\r\n",
                   adc_raw, voltage_mv / 1000, (voltage_mv % 1000) / 10,
                   alarm_active ? "ON" : "OFF",
                   alarm_muted ? "YES" : "NO");
            debug_timer = 0;
        }
        
        // 报警动作：LED闪烁和蜂鸣器（仅在未消音时）
        if (alarm_active)
        {
            if (!alarm_muted)
            {
                if (blink_timer >= BLINK_INTERVAL_MS)
                {
                    LED0 = !LED0;
                    BEEP_TOGGLE();
                    blink_timer = 0;
                }
            }
            else
            {
                // 已消音：确保关闭蜂鸣器，LED维持灭
                LED0 = 1;
                BEEP_OFF();
            }
        }
        else
        {
            LED0 = 1; // LED灭
            BEEP_OFF(); // 蜂鸣器关
            blink_timer = 0;
        }
        
        // 按键功能：
        // KEY0：切换显示模式（原始 / 电压）
        // KEY1：在报警激活时消音（仅当前这次报警有效），不影响下一次重新触发
        if (key == KEY0_PRES)
        {
            mode = (mode == DISPLAY_RAW) ? DISPLAY_VOLT : DISPLAY_RAW;
        }
        else if (key == KEY1_PRES)
        {
            if (alarm_active && !alarm_muted)
            {
                alarm_muted = 1;
                LED0 = 1;
                BEEP_OFF();
                printf("Alarm muted by KEY1. Will re-arm on next new alarm event.\r\n");
            }
        }
    }
}
