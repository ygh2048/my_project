/**
 ****************************************************************************************************
 * @file        led.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2020-05-30
 * @brief       LED ��������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�˾
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� STM32F103������
 * ������Ƶ:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * �����ַ:openedv.taobao.com
 *
 * �޸�˵��
 * V1.0 20200530
 * ��һ�η���
 *
 ****************************************************************************************************
 */

#ifndef __LED_H
#define __LED_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* ���� ���� */

#define LED0_GPIO_PORT                  GPIOC
#define LED0_GPIO_PIN                   SYS_GPIO_PIN13
#define LED0_GPIO_CLK_ENABLE()          do{ RCC->APB2ENR |= 1 << 4; }while(0)   /* PC口时钟使能 */

#define LED1_GPIO_PORT                  GPIOD
#define LED1_GPIO_PIN                   SYS_GPIO_PIN2
#define LED1_GPIO_CLK_ENABLE()          do{ RCC->APB2ENR |= 1 << 5; }while(0)   /* PD口时钟使能 */

/******************************************************************************************/



/* LED�˿ڶ��� */
#define LED0(x)         sys_gpio_pin_set(LED0_GPIO_PORT, LED0_GPIO_PIN, x)      /* LED0 */
#define LED1(x)         sys_gpio_pin_set(LED1_GPIO_PORT, LED1_GPIO_PIN, x)      /* LED1 */

/* LEDȡ������ */
#define LED0_TOGGLE()   do{ LED0_GPIO_PORT->ODR ^= LED0_GPIO_PIN; }while(0)      /* LED0 = !LED0 */
#define LED1_TOGGLE()   do{ LED1_GPIO_PORT->ODR ^= LED1_GPIO_PIN; }while(0)      /* LED1 = !LED1 */


void led_init(void);    /* ��ʼ�� */

#endif

















