#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"

// STM32F103C8T6适配: 使用PB6/PB7 (避开PC13板载LED)
#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)

#define KEY0_PRES 	1	//KEY0 pressed
#define KEY1_PRES	  2	//KEY1 pressed

void KEY_Init(void);
u8 KEY_Scan(u8 mode);	  	//key scan
#endif
