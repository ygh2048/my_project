#ifndef __BEEP_H
#define __BEEP_H

#include "sys.h"

#define BEEP_PIN    GPIO_Pin_8
#define BEEP_PORT   GPIOB
#define BEEP_PERIPH RCC_APB2Periph_GPIOB

// 修改为低电平驱动 (GPIO_ResetBits=ON, GPIO_SetBits=OFF)
#define BEEP_ON()   GPIO_ResetBits(BEEP_PORT, BEEP_PIN)
#define BEEP_OFF()  GPIO_SetBits(BEEP_PORT, BEEP_PIN)
#define BEEP_TOGGLE() GPIO_WriteBit(BEEP_PORT, BEEP_PIN, (BitAction)!GPIO_ReadOutputDataBit(BEEP_PORT, BEEP_PIN))

void BEEP_Init(void);

#endif
