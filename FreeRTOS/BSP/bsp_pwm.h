/*
 * bsp_pwm.h
 *
 *  Created on: Jul 20, 2024
 *      Author: ygh20
 */

#ifndef BSP_PWM_H_
#define BSP_PWM_H_

#define dj180 1
#define dj90 2
#define bdj180 3
#define bdj90 4

enum PWM_id
{
	pwm_1 = 0,//PA6
	pwm_2,//PA7
	pwm_3,//PB0
	pwm_4,//PB1
};
enum DJ_id
{
	dj_1 = 0,//PA15
	dj_2,//PB3
	dj_3,//PB10
	dj_4,//PB11
};


void pwm3_init(void);
void dj_init(void);
void pwm3_set(int pwm_id,int percent);
void dj_set(int dj_id,int angle,int mode);
void pwm3_stop(void);

#endif /* BSP_PWM_H_ */
