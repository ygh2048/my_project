#ifndef _US_100_H_
#define _US_100_H_

typedef struct
{
	uint32_t us100_start_time;
	uint32_t us100_end_time;
	uint32_t us100_delta;
	uint8_t us100_update_flag;
	float distance;
	float last_distance;
	float pre_last_distance;
	float vel;
	float acc;
	float last_vel;
	uint32_t us100_rssi_cnt;
}us100_data;

extern us100_data us100,us100_front;

//US100
#define User_Height_Max  240//用户使用的最大高度值，单位为cm，需要预留一定裕度给气压计/超声波切换过渡    240



#define US_100_Distance_CMD    0x55
#define US_100_Temperature_CMD 0x50

void US_100_Statemachine(void);
float US_100_Distance(uint8 MSB,uint8 LSB);

extern uint8_t US_100_Update_Flag;
extern float GD_Distance,GD_Distance_Div,GD_Distance_Acc,Last_GD_Distance,Last_GD_Distance_Div;
#endif


