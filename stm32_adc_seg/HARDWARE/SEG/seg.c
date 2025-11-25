#include "seg.h"
#include "sys.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

// 引脚映射定义
// 段驱动引脚（分散在GPIOA和GPIOB）
#define SEG_A_PORT   GPIOA
#define SEG_A_PIN    GPIO_Pin_1
#define SEG_B_PORT   GPIOA
#define SEG_B_PIN    GPIO_Pin_5
#define SEG_C_PORT   GPIOB
#define SEG_C_PIN    GPIO_Pin_15
#define SEG_D_PORT   GPIOB
#define SEG_D_PIN    GPIO_Pin_13
#define SEG_E_PORT   GPIOB
#define SEG_E_PIN    GPIO_Pin_12
#define SEG_F_PORT   GPIOA
#define SEG_F_PIN    GPIO_Pin_2
#define SEG_G_PORT   GPIOA
#define SEG_G_PIN    GPIO_Pin_11
#define SEG_DP_PORT  GPIOB
#define SEG_DP_PIN   GPIO_Pin_14

// 位选引脚（分散在GPIOA）
#define DIGIT0_PORT  GPIOA
#define DIGIT0_PIN   GPIO_Pin_12  // 个位
#define DIGIT1_PORT  GPIOA
#define DIGIT1_PIN   GPIO_Pin_4   // 十位
#define DIGIT2_PORT  GPIOA
#define DIGIT2_PIN   GPIO_Pin_3   // 百位
#define DIGIT3_PORT  GPIOA
#define DIGIT3_PIN   GPIO_Pin_0   // 千位

// 7段显示标准编码（a-g+dp）
// bit0=a, bit1=b, bit2=c, bit3=d, bit4=e, bit5=f, bit6=g, bit7=dp
static const uint8_t kDigitCode[16] = {
    0x3F, // 0: a,b,c,d,e,f
    0x06, // 1: b,c
    0x5B, // 2: a,b,d,e,g
    0x4F, // 3: a,b,c,d,g
    0x66, // 4: b,c,f,g
    0x6D, // 5: a,c,d,f,g
    0x7D, // 6: a,c,d,e,f,g
    0x07, // 7: a,b,c
    0x7F, // 8: a,b,c,d,e,f,g
    0x6F, // 9: a,b,c,d,f,g
    0x77, // A: a,b,c,e,f,g
    0x7C, // b: c,d,e,f,g
    0x39, // C: a,d,e,f
    0x5E, // d: b,c,d,e,g
    0x79, // E: a,d,e,f,g
    0x71  // F: a,e,f,g
};

static const uint8_t SEG_CHAR_BLANK = 0x00;
static const uint8_t SEG_CHAR_V = 0x3E;

static uint8_t display_digits[SEG_DIGIT_COUNT] = {0};
static uint8_t display_dots = 0;
static uint8_t scan_index = 0;
static uint8_t test_mode = 0;
static uint8_t test_pattern = 0;

// 控制位选（数码管位的开关）
static void SEG_WriteDigitLines(uint8_t digit_index, uint8_t enable)
{
    GPIO_TypeDef* port;
    uint16_t pin;
    
    // 选择对应的位选引脚
    switch(digit_index)
    {
        case 0:  // 个位
            port = DIGIT0_PORT;
            pin = DIGIT0_PIN;
            break;
        case 1:  // 十位
            port = DIGIT1_PORT;
            pin = DIGIT1_PIN;
            break;
        case 2:  // 百位
            port = DIGIT2_PORT;
            pin = DIGIT2_PIN;
            break;
        case 3:  // 千位
            port = DIGIT3_PORT;
            pin = DIGIT3_PIN;
            break;
        default:
            return;
    }
    
    if (DIGIT_ACTIVE_HIGH)
    {
        // 共阴极：高电平激活位选
        if (enable)
            GPIO_SetBits(port, pin);
        else
            GPIO_ResetBits(port, pin);
    }
    else
    {
        // 共阳极：低电平激活位选
        if (enable)
            GPIO_ResetBits(port, pin);
        else
            GPIO_SetBits(port, pin);
    }
}

// 写段驱动数据（根据段码控制各个段）
static void SEG_WriteSegments(uint8_t pattern)
{
    // pattern的bit位对应：bit0=a, bit1=b, bit2=c, bit3=d, bit4=e, bit5=f, bit6=g, bit7=dp
    
    // 根据共阳极/共阴极模式决定电平逻辑
    uint8_t active_level = SEG_ACTIVE_HIGH ? 1 : 0;  // 共阴极=1，共阳极=0
    
    // a段 (PA1)
    if ((pattern & 0x01) != 0)  // bit0
        active_level ? GPIO_SetBits(SEG_A_PORT, SEG_A_PIN) : GPIO_ResetBits(SEG_A_PORT, SEG_A_PIN);
    else
        active_level ? GPIO_ResetBits(SEG_A_PORT, SEG_A_PIN) : GPIO_SetBits(SEG_A_PORT, SEG_A_PIN);
    
    // b段 (PA5)
    if ((pattern & 0x02) != 0)  // bit1
        active_level ? GPIO_SetBits(SEG_B_PORT, SEG_B_PIN) : GPIO_ResetBits(SEG_B_PORT, SEG_B_PIN);
    else
        active_level ? GPIO_ResetBits(SEG_B_PORT, SEG_B_PIN) : GPIO_SetBits(SEG_B_PORT, SEG_B_PIN);
    
    // c段 (PB15)
    if ((pattern & 0x04) != 0)  // bit2
        active_level ? GPIO_SetBits(SEG_C_PORT, SEG_C_PIN) : GPIO_ResetBits(SEG_C_PORT, SEG_C_PIN);
    else
        active_level ? GPIO_ResetBits(SEG_C_PORT, SEG_C_PIN) : GPIO_SetBits(SEG_C_PORT, SEG_C_PIN);
    
    // d段 (PB13)
    if ((pattern & 0x08) != 0)  // bit3
        active_level ? GPIO_SetBits(SEG_D_PORT, SEG_D_PIN) : GPIO_ResetBits(SEG_D_PORT, SEG_D_PIN);
    else
        active_level ? GPIO_ResetBits(SEG_D_PORT, SEG_D_PIN) : GPIO_SetBits(SEG_D_PORT, SEG_D_PIN);
    
    // e段 (PB12)
    if ((pattern & 0x10) != 0)  // bit4
        active_level ? GPIO_SetBits(SEG_E_PORT, SEG_E_PIN) : GPIO_ResetBits(SEG_E_PORT, SEG_E_PIN);
    else
        active_level ? GPIO_ResetBits(SEG_E_PORT, SEG_E_PIN) : GPIO_SetBits(SEG_E_PORT, SEG_E_PIN);
    
    // f段 (PA2)
    if ((pattern & 0x20) != 0)  // bit5
        active_level ? GPIO_SetBits(SEG_F_PORT, SEG_F_PIN) : GPIO_ResetBits(SEG_F_PORT, SEG_F_PIN);
    else
        active_level ? GPIO_ResetBits(SEG_F_PORT, SEG_F_PIN) : GPIO_SetBits(SEG_F_PORT, SEG_F_PIN);
    
    // g段 (PA11)
    if ((pattern & 0x40) != 0)  // bit6
        active_level ? GPIO_SetBits(SEG_G_PORT, SEG_G_PIN) : GPIO_ResetBits(SEG_G_PORT, SEG_G_PIN);
    else
        active_level ? GPIO_ResetBits(SEG_G_PORT, SEG_G_PIN) : GPIO_SetBits(SEG_G_PORT, SEG_G_PIN);
    
    // dp段 (PB14)
    if ((pattern & 0x80) != 0)  // bit7
        active_level ? GPIO_SetBits(SEG_DP_PORT, SEG_DP_PIN) : GPIO_ResetBits(SEG_DP_PORT, SEG_DP_PIN);
    else
        active_level ? GPIO_ResetBits(SEG_DP_PORT, SEG_DP_PIN) : GPIO_SetBits(SEG_DP_PORT, SEG_DP_PIN);
}

// 根据输入值获取对应的段码
static uint8_t SEG_CodeForChar(uint8_t value)
{
    if (value <= 0x0F)
        return kDigitCode[value];
    if (value == 0xFE)
        return SEG_CHAR_V;
    return SEG_CHAR_BLANK;
}

void SEG_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    uint8_t i;

    // 使能GPIOA和GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    // 初始化GPIOA的段驱动引脚（PA1, PA2, PA5, PA11）
    GPIO_InitStructure.GPIO_Pin = SEG_A_PIN | SEG_F_PIN | SEG_B_PIN | SEG_G_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 初始化GPIOA的位选引脚（PA0, PA3, PA4, PA12）
    GPIO_InitStructure.GPIO_Pin = DIGIT3_PIN | DIGIT2_PIN | DIGIT1_PIN | DIGIT0_PIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 初始化GPIOB的段驱动引脚（PB12, PB13, PB14, PB15）
    GPIO_InitStructure.GPIO_Pin = SEG_E_PIN | SEG_D_PIN | SEG_DP_PIN | SEG_C_PIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 初始化显示数据并关闭所有段
    SEG_WriteSegments(0x00);
    
    // 关闭所有位选
    if (DIGIT_ACTIVE_HIGH)
    {
        // 高电平激活 -> 输出低电平关闭
        GPIO_ResetBits(GPIOA, DIGIT0_PIN | DIGIT1_PIN | DIGIT2_PIN | DIGIT3_PIN);
    }
    else
    {
        // 低电平激活 -> 输出高电平关闭
        GPIO_SetBits(GPIOA, DIGIT0_PIN | DIGIT1_PIN | DIGIT2_PIN | DIGIT3_PIN);
    }
    
    for (i = 0; i < SEG_DIGIT_COUNT; i++)
    {
        display_digits[i] = SEG_CHAR_BLANK;
    }
    display_dots = 0;
    scan_index = 0;
    test_mode = 0;
}

// 多路扫描驱动：每次调用刷新一位
void SEG_Task(void)
{
    uint8_t code;
    uint8_t i;

    // 步骤1：关闭所有位选，防止重影
    for (i = 0; i < SEG_DIGIT_COUNT; i++)
    {
        SEG_WriteDigitLines(i, 0);
    }
    
    // 步骤1.5：短延时，让位选电平稳定（重要！）
    for (i = 0; i < 10; i++) { __NOP(); }

    // 步骤2：取得当前位要显示的段码
    if (test_mode == 1)
    {
        // 测试模式：直接使用测试段码
        code = test_pattern;
    }
    else
    {
        // 正常模式：从查表获取段码
        code = SEG_CodeForChar(display_digits[scan_index]);
        
        // 如果该位需要显示小数点，添加dp段(bit7)
        // 注意：只有当该位有数字时才显示小数点
        if ((display_dots & (1 << scan_index)) && (display_digits[scan_index] != 0xFF))
            code |= 0x80;
    }
    
    // 步骤3：写入段驱动数据
    SEG_WriteSegments(code);

    // 步骤4：打开当前位
    SEG_WriteDigitLines(scan_index, 1);
    
    // 步骤5：扫描进入下一位
    scan_index = (scan_index + 1) % SEG_DIGIT_COUNT;
}

// 显示原始ADC值（0~9999）
void SEG_DisplayRaw(uint16_t value)
{
    uint16_t v = (value > 9999) ? 9999 : value;
    uint8_t digits[SEG_DIGIT_COUNT];
    uint8_t i;
    
    test_mode = 0;
    
    digits[0] = v % 10;
    digits[1] = (v / 10) % 10;
    digits[2] = (v / 100) % 10;
    digits[3] = (v / 1000) % 10;

    if (digits[3] == 0) 
        digits[3] = 0xFF;
    if (digits[3] == 0xFF && digits[2] == 0) 
        digits[2] = 0xFF;
    if (digits[3] == 0xFF && digits[2] == 0xFF && digits[1] == 0) 
        digits[1] = 0xFF;

    for (i = 0; i < SEG_DIGIT_COUNT; i++)
        display_digits[i] = digits[i];
    
    display_dots = 0;
}

// 显示电压值（格式：X.XXV，例如 1.82V 显示为千位=1, 百位=8., 十位=2, 个位=V）
void SEG_DisplayVoltage(uint32_t voltage_mv)
{
    uint32_t centiv;
    
    test_mode = 0;
    
    if (voltage_mv > 3300) 
        voltage_mv = 3300;
    
    // 转换为厘伏（1/100伏），例如1820mV → 182厘伏 → 显示1.82
    centiv = (voltage_mv + 5) / 10;

    // 个位=V, 十位=小数后两位, 百位=小数后一位, 千位=整数位
    display_digits[0] = 0xFE;                    // 个位显示V
    display_digits[1] = centiv % 10;             // 十位显示小数第二位
    display_digits[2] = (centiv / 10) % 10;      // 百位显示小数第一位（带小数点）
    display_digits[3] = (centiv / 100) % 10;     // 千位显示整数位
    
    // 小数点在千位（bit3），即整数位后面
    display_dots = (1 << 3);
}

// 诊断函数：点亮指定的单个段
void SEG_TestSingleSegment(uint8_t segment_bit)
{
    test_mode = 1;
    test_pattern = (1 << segment_bit);
}

// 在指定位置显示指定数字
void SEG_DisplaySingleDigit(uint8_t digit_pos, uint8_t value)
{
    uint8_t i;
    
    test_mode = 0;
    
    for (i = 0; i < SEG_DIGIT_COUNT; i++)
    {
        display_digits[i] = SEG_CHAR_BLANK;
    }
    
    if (digit_pos < SEG_DIGIT_COUNT && value <= 9)
    {
        display_digits[digit_pos] = value;
    }
    
    display_dots = 0;
}
