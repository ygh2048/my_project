/**
 * @file key.c
 * @brief KEY0/KEY1 外部中断驱动
 * @author ygh2048
 * @date 2025-11-08
 */

#include "key.h"
#include "tick.h"

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "misc.h"

#define KEY_DEBOUNCE_MS     20U  /**< 按键去抖时间，单位毫秒 */

static volatile uint8_t key_irq_flags = 0;  /**< 按键中断标志 */
static uint32_t key0_last_tick = 0;         /**< KEY0最后按下时间 */
static uint32_t key1_last_tick = 0;         /**< KEY1最后按下时间 */

/**
 * @brief 初始化按键
 * 配置GPIO和外部中断
 */
void key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 开启GPIO与AFIO时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /* 配置KEY0/KEY1为上拉输入 */
    GPIO_InitStructure.GPIO_Pin = KEY0_GPIO_PIN | KEY1_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStructure);

    tick_init();

    /* 绑定外部中断线 */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);

    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;

    EXTI_InitStructure.EXTI_Line = EXTI_Line12;
    EXTI_Init(&EXTI_InitStructure);

    EXTI_InitStructure.EXTI_Line = EXTI_Line13;
    EXTI_Init(&EXTI_InitStructure);

    /* 配置NVIC */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief 扫描按键状态
 * @param mode 模式（未使用）
 * @return 按键值
 */
uint8_t key_scan(uint8_t mode)
{
    uint8_t keyval = 0;

    sys_intx_disable();
    if (key_irq_flags & KEY0_PRES)
    {
        keyval = KEY0_PRES;
        key_irq_flags &= (uint8_t)~KEY0_PRES;
    }
    else if (key_irq_flags & KEY1_PRES)
    {
        keyval = KEY1_PRES;
        key_irq_flags &= (uint8_t)~KEY1_PRES;
    }
    sys_intx_enable();

    (void)mode;
    return keyval;
}

/**
 * @brief 外部中断处理函数
 * 处理KEY0和KEY1的中断，去抖后设置标志
 */
void EXTI15_10_IRQHandler(void)
{
    uint32_t now = get_tick();

    if (EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
        // KEY0按下，去抖
        if ((now - key0_last_tick) >= KEY_DEBOUNCE_MS)
        {
            key_irq_flags |= KEY0_PRES;
            key0_last_tick = now;
        }
        EXTI_ClearITPendingBit(EXTI_Line12);
    }

    if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
        // KEY1按下，去抖
        if ((now - key1_last_tick) >= KEY_DEBOUNCE_MS)
        {
            key_irq_flags |= KEY1_PRES;
            key1_last_tick = now;
        }
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
}
