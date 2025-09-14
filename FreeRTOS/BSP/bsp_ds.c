/*
 * bsp_ds.c
 *
 *  Created on: Jul 30, 2024
 *      Author: ygh20
 */
#include "bsp_ds.h"

typedef struct{
	int16_t position;
	int16_t id;
}dsstruct;

int16_t rec_id=0;
int16_t rec_pos=0;
int16_t route_flag=0;
int16_t s_num=0;
dsstruct ds_array[24];
int s_numflag=0;
char *pos_str;
char *s_position;
extern Usedata mydata;
extern int enable_led;
int last_q3=0;
int last_rec_id=0;


/**
 * @description: 二维码位置选择
 * @return {*}
 */
void data_to_array(void)
{
	if(choose_pos())
	{
	ds_array[rec_id-1].id=rec_id;
	ds_array[rec_id-1].position=rec_pos;
	if(q3 != last_q3)
	{
		enable_led=1;
		last_q3=q3;
	}
	if(last_rec_id != rec_id)
	{
		last_rec_id = rec_id;
		enable_led = 1;
	}
	}
	if(q2!=0)
	{
		s_num=q2;
		choose_s_pos();
		if(ds_array[q2-1].position<=6 && ds_array[q2-1].position>0)
		{
			route_flag=1;
		}
		else if(ds_array[q2-1].position<=12 && ds_array[q2-1].position>6)
		{
			route_flag=2;
		}
		else if(ds_array[q2-1].position<=18 && ds_array[q2-1].position>12)
		{
			route_flag=3;
		}
		else if(ds_array[q2-1].position<=24 && ds_array[q2-1].position>18)
		{
			route_flag=4;
		}
		else
		{
			route_flag=0;
		}
	}

}
void choose_s_pos()
{
	switch(ds_array[q2-1].position)
	{
	case 1:
		s_position="A1";
		break;
	case 2:
		s_position="A2";
		break;
	case 3:
		s_position="A3";
		break;
	case 4:
		s_position="A4";
		break;
	case 5:
		s_position="A5";
		break;
	case 6:
		s_position="A6";
		break;
	case 7:
		s_position="B1";
		break;
	case 8:
		s_position="B2";
		break;
	case 9:
		s_position="B3";
		break;
	case 10:
		s_position="B4";
		break;
	case 11:
		s_position="B5";
		break;
	case 12:
		s_position="B6";
		break;
	case 13:
		s_position="C1";
		break;
	case 14:
		s_position="C2";
		break;
	case 15:
		s_position="C3";
		break;
	case 16:
		s_position="C4";
		break;
	case 17:
		s_position="C5";
		break;
	case 18:
		s_position="C6";
		break;
	case 19:
		s_position="D1";
		break;
	case 20:
		s_position="D2";
		break;
	case 21:
		s_position="D3";
		break;
	case 22:
		s_position="D4";
		break;
	case 23:
		s_position="D5";
		break;
	case 24:
		s_position="D6";
		break;
	default:
		rec_id=0;
		s_position="home";
	}
}
int choose_pos(void)
{
	static int last_id=0;
	if(q0!=last_id)
	{
		last_id=q0;
		rec_pos=q1;
	switch(rec_pos)
	{
	case A1:
		rec_id=q0;
		rec_pos=1;
		pos_str="A1";
		break;
	case A2:
		rec_id=q0;
		rec_pos=2;
		pos_str="A2";
		break;
	case A3:
		rec_id=q0;
		rec_pos=3;
		pos_str="A3";
		break;
	case A4:
		rec_id=q0;
		rec_pos=4;
		pos_str="A4";
		break;
	case A5:
		rec_id=q0;
		rec_pos=5;
		pos_str="A5";
		break;
	case A6:
		rec_id=q0;
		rec_pos=6;
		pos_str="A6";
		break;
	case B1:
		rec_id=q0;
		rec_pos=7;
		pos_str="B1";
		break;
	case B2:
		rec_id=q0;
		rec_pos=8;
		pos_str="B2";
		break;
	case B3:
		rec_id=q0;
		rec_pos=9;
		pos_str="B3";
		break;
	case B4:
		rec_id=q0;
		rec_pos=10;
		pos_str="B4";
		break;
	case B5:
		rec_id=q0;
		rec_pos=11;
		pos_str="B5";
		break;
	case B6:
		rec_id=q0;
		rec_pos=12;
		pos_str="B6";
		break;
	case C1:
		rec_id=q0;
		rec_pos=13;
		pos_str="C1";
		break;
	case C2:
		rec_id=q0;
		rec_pos=14;
		pos_str="C2";
		break;
	case C3:
		rec_id=q0;
		rec_pos=15;
		pos_str="C3";
		break;
	case C4:
		rec_id=q0;
		rec_pos=16;
		pos_str="C4";
		break;
	case C5:
		rec_id=q0;
		rec_pos=17;
		pos_str="C5";
		break;
	case C6:
		rec_id=q0;
		rec_pos=18;
		pos_str="C6";
		break;
	case D1:
		rec_id=q0;
		rec_pos=19;
		pos_str="D1";
		break;
	case D2:
		rec_id=q0;
		rec_pos=20;
		pos_str="D2";
		break;
	case D3:
		rec_id=q0;
		rec_pos=21;
		pos_str="D3";
		break;
	case D4:
		rec_id=q0;
		rec_pos=22;
		pos_str="D4";
		break;
	case D5:
		rec_id=q0;
		rec_pos=23;
		pos_str="D5";
		break;
	case D6:
		rec_id=q0;
		rec_pos=24;
		pos_str="D6";
		break;
	default:
		rec_id=0;
		pos_str="home";
	}
	return true;
	}
	return false;
}

void show(void)
{

	TJCPrintf("now_num.val=%d",(int)rec_id);
//	osDelay(40);
	TJCPrintf("now_position.txt=\"%s\"",(const char *)pos_str);
	TJCPrintf("now_num.val=%d",(int)rec_id);
	TJCPrintf("s_num.val=%d",(int)s_num);
	if(route_flag!=0)
	{
		TJCPrintf("route_flag.val=%d",(int)route_flag);
		TJCPrintf("s_num.val=%d",(int)s_num);
//		TJCPrintf("s_position.val=%s",(const char *)s_position);
	}

//	TJCPrintf("s_num.val=%d",(int)s_num);
//	TJCPrintf("s_position.val=%s",(const char *)s_position);
}

int check_array(void)
{
	static int error_id = 0;
	int check = false;
	for(int i =0;i<24;i++)
	{
		if (ds_array[i].id==0)
		{
			check = true;
			error_id = i;
		}
		check = !check;
	}
	if(check)
	{
		return 0;
	}
	else
	{
		return error_id;
	}

}



