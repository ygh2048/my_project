#ifndef __SENSOR_H
#define __SENSOR_H

extern float K[3];
extern float B[3];

#define RtA         57.324841
#define AtR    	    0.0174533
#define Acc_G 	    0.0000610351
#define Gyro_G 	    0.0610351
#define Gyro_Gr	    0.0010653
#define DEG2RAD (PI / 180.0f)
#define RAD2DEG (180.0f / PI)
//#define GYRO_CALIBRATION_COFF 0.060976f  //2000
//#define GYRO_CALIBRATION_COFF 0.030488f; //1000
#define GYRO_CALIBRATION_COFF 0.0152672f;  //500



void INS_Sensor_Update(void);
void OBS_Sensor_Update(void);
void quad_from_euler(float *q, float roll, float pitch, float yaw);
void WP_Quad_Init(void);
void Senosr_Update(void);	
void Observation_Angle_Calculate(void);
void Madgwick_AHRS_Update_IMU(float gx, float gy, float gz, 
															float ax, float ay, float az,
															float mx, float my, float mz,
															float gyro_mold);
void Get_Status_Feedback(void);
void Euler_Angle_Init_Again(void);
//extern float Pitch,Roll,Yaw;
//extern float 	Yaw_Gyro,Pitch_Gyro,Roll_Gyro;
extern float Yaw_Gyro_Earth_Frame;
extern float Pitch_Observation,Roll_Observation,Yaw_Observation,Altitude_Observation;
extern Sensor WP_Sensor;
extern Vector3f gyro,accel,mag;
extern Vector3f gyro_filter,accel_filter,mag_filter,ins_accel_filter;
extern Sensor_Health Sensor_Flag;
extern Vector3f_Body Circle_Angle;
extern float Gyro_Length;
extern float IMU_Dt;
extern float yaw_angle_deg_enu; 
extern float Accel_For_Cal[3];
extern float Sensor_Time;

#endif





//  IIR_High_Order_Filter(&Accel_X_Butter_Filter,accel.x);accel_filter.x=Accel_X_Butter_Filter.output[Accel_X_Butter_Filter.N-1];//accel_filter.x=LPButterworth(accel.x,&accel_filter_buf[0],&Accel_Parameter);
//  IIR_High_Order_Filter(&Accel_Y_Butter_Filter,accel.y);accel_filter.y=Accel_Y_Butter_Filter.output[Accel_Y_Butter_Filter.N-1];//accel_filter.y=LPButterworth(accel.y,&accel_filter_buf[1],&Accel_Parameter);
//  IIR_High_Order_Filter(&Accel_Z_Butter_Filter,accel.z);accel_filter.z=Accel_Z_Butter_Filter.output[Accel_Z_Butter_Filter.N-1];//accel_filter.z=LPButterworth(accel.z,&accel_filter_buf[2],&Accel_Parameter);		


//	IIR_High_Order_Filter(&Gyro_X_Butter_Filter,gyro.x);gyro_filter.x=Gyro_X_Butter_Filter.output[Gyro_X_Butter_Filter.N-1];//	gyro_filter.x=LPButterworth(gyro.x,&gyro_filter_buf_bug[0],&Gyro_Parameter);
//  IIR_High_Order_Filter(&Gyro_Y_Butter_Filter,gyro.y);gyro_filter.y=Gyro_Y_Butter_Filter.output[Gyro_Y_Butter_Filter.N-1];//	gyro_filter.y=LPButterworth(gyro.y,&gyro_filter_buf_bug[1],&Gyro_Parameter);
//  IIR_High_Order_Filter(&Gyro_Z_Butter_Filter,gyro.z);gyro_filter.z=Gyro_Z_Butter_Filter.output[Gyro_Z_Butter_Filter.N-1];//	gyro_filter.z=LPButterworth(gyro.z,&gyro_filter_buf_bug[2],&Gyro_Parameter);		
//	
//  IIR_High_Order_Filter(&Accel_X_INS,accel.x);ins_accel_filter.x=Accel_X_INS.output[Accel_X_INS.N-1];//ins_accel_filter.x=LPButterworth(accel.x,&ins_accel_filter_buf[0],&Ins_Accel_Parameter);
//  IIR_High_Order_Filter(&Accel_Y_INS,accel.y);ins_accel_filter.y=Accel_Y_INS.output[Accel_Y_INS.N-1];//ins_accel_filter.y=LPButterworth(accel.y,&ins_accel_filter_buf[1],&Ins_Accel_Parameter);
//  IIR_High_Order_Filter(&Accel_Z_INS,accel.z);ins_accel_filter.z=Accel_Z_INS.output[Accel_Z_INS.N-1];//ins_accel_filter.z=LPButterworth(accel.z,&ins_accel_filter_buf[2],&Ins_Accel_Parameter);
	


