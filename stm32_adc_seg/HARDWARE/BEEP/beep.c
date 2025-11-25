#include "beep.h"

// PB8 push-pull output, default off (high level). 低电平驱动模式
// 修改为低电平驱鸣 (LOW=ON, HIGH=OFF)
void BEEP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(BEEP_PERIPH, ENABLE);
    GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BEEP_PORT, &GPIO_InitStructure);

    BEEP_OFF();  // 初始化为高电平（关闭蜂鸣）
}
