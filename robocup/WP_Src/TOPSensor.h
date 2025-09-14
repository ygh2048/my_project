#ifndef _TOPSENSOR_H_
#define _TOPSENSOR_H_

#define TOFSENSE_MAX_NUMBER 	10//最大运行的TOF传感器组
#define TOFSENSE_CURRENT_WORK 2//当前运行的TOF传感器组


typedef struct 
{
	uint8_t reserved1;
	uint8_t id;
	uint32_t system_time;
	int32_t dis;
	uint8_t dis_status;
	uint16_t signal_strength;
	uint8_t reserved2;
	
	uint32_t last_system_time;
	
	float distance;
	float last_distance;
	float pre_last_distance;
	float vel;
	float acc;
	float last_vel;
}TOFSensor_Data;


typedef struct 
{
	uint8_t id;
	uint32_t system_time;
	int32_t dis;
	uint8_t dis_status;
	uint16_t signal_strength;
	uint8_t range_precision;

	
	uint32_t last_system_time;	
	float distance;
	float last_distance;
	float pre_last_distance;
	float vel;
	float acc;
	float last_vel;
	
}TOFSense_F;


extern TOFSense_F TOFSense_F_Data;



void TOF_Statemachine(void);
void TOF_Sense_Prase(uint8_t data);
void TOFSense_IIC_Update(void);


extern TOFSensor_Data tofdata[TOFSENSE_MAX_NUMBER]; 

extern float front_tofsense_distance;
extern uint8_t front_tofsense_distance_valid_cnt,front_tofsense_distance_valid_flag;

#endif







