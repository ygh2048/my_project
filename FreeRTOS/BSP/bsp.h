#ifndef __BSP_H__
#define __BSP_H__

/* Import HAL related library  导入HAL相关库 */
#include "main.h"
#include "gpio.h"
#include "cmsis_os.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "stm32f1xx_hal.h"
#include "stm32f103xe.h"
#include "stm32f1xx_hal_def.h"

/* Import device driver library  导入设备驱动库 */
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_task.h"
#include "bsp_uart.h"
#include "bsp_pwm.h"
#include "bsp_agreement.h"
#include "bsp_tjc.h"
#include "bsp_ds.h"

#include "nclink.h"

#include "stdio.h"
#include "string.h"
#include "stdint.h"
#include "stdlib.h"
#include <stdarg.h>

/* DEFINE */
#define LED1_ON()         HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET)
#define LED1_OFF()        HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET)
#define LED1_TOGGLE()     HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin)


/* functions */
void Bsp_Init(void);
void Bsp_Loop(void);



#endif /* __BSP_H__ */
