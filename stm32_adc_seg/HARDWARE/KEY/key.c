#include "stm32f10x.h"
#include "key.h"
#include "sys.h"
#include "delay.h"

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // STM32F103C8T6适配: 使用GPIOB (PB6/PB7), 避开PC13板载LED
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // PB6=KEY0, PB7=KEY1
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

u8 KEY_Scan(u8 mode)
{
    static u8 key_up = 1;
    if (mode) key_up = 1;
    if (key_up && (KEY0 == 0 || KEY1 == 0))
    {
        delay_ms(10);
        key_up = 0;
        if (KEY0 == 0) return KEY0_PRES;
        else if (KEY1 == 0) return KEY1_PRES;
    }
    else if (KEY0 == 1 && KEY1 == 1)
    {
        key_up = 1;
    }
    return 0;
}
