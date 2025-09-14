/*
 * bsp_pwm.c
 *
 *  Created on: Jul 20, 2024
 *      Author: ygh20
 */
#include "bsp_pwm.h"
#include "bsp.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;


void pwm3_init(void)
{
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
}
void dj_init(void)
{
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
}

void pwm3_set(int pwm_id,int percent)//输入百分比
{
	enum PWM_id id = pwm_id;
	percent=percent*10;//设置period为1000
	if(percent>100)
	{
		percent=100;
	}
	else if(percent<0)
	{
		percent=0;
	}
	switch(id)
	{
	case pwm_1:
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,percent);
		break;
	case pwm_2:
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,percent);
		break;
	case pwm_3:
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,percent);
		break;
	case pwm_4:
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,percent);
		break;
	}
}

void pwm3_stop(void)
{
	pwm3_set(pwm_1, 0);
	pwm3_set(pwm_2, 0);
	pwm3_set(pwm_3, 0);
	pwm3_set(pwm_4, 0);
}

void dj_set(int dj_id,int angle,int mode)
{
	enum DJ_id id = dj_id;//设置period为2000
	switch(mode)
	{
	case dj180:
		angle = angle+45;
		angle = (angle/45)*50;
		break;
	case dj90:
		angle = angle*2;
		angle = angle+45;
		angle = (angle/45)*50;
		break;
	case bdj180:
		angle = angle+45;
		angle = (angle/45)*50;
		break;
	case bdj90:
		angle = angle*2;
		angle = angle+45;
		angle = (angle/45)*50;
		break;
	}

	switch(id)
	{
	case dj_1:
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,angle);
		break;
	case dj_2:
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,angle);
		break;
	case dj_3:
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,angle);
		break;
	case dj_4:
		__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,angle);
		break;
	}
}




