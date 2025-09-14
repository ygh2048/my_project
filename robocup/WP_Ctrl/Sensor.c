/* Copyright (c)  2019-2030 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/
/*----------------------------------------------------------------------------------------------------------------------/
																									开源并不等于免费
																									开源并不等于免费
																									开源并不等于免费
																									重要的事情说三遍
								先驱者的历史已经证明，在当前国内略浮躁+躺平+内卷的大环境下，对于毫无收益的开源项目，单靠坊间飞控爱好者、
								个人情怀式、自发地主动输出去参与开源项目的方式行不通，好的开源项目需要请专职人员做好售后技术服务、配套
								手册和视频教程要覆盖新手入门到进阶阶段，使用过程中对用户反馈问题和需求进行统计、在实践中完成对产品的一
								次次完善与迭代升级。
-----------------------------------------------------------------------------------------------------------------------
*                                                 为什么选择无名创新？
*                                         感动人心价格厚道，最靠谱的开源飞控；
*                                         国内业界良心之作，最精致的售后服务；
*                                         追求极致用户体验，高效进阶学习之路；
*                                         萌新不再孤单求索，合理把握开源尺度；
*                                         响应国家扶贫号召，促进教育体制公平；
*                                         新时代奋斗最出彩，建人类命运共同体。 
-----------------------------------------------------------------------------------------------------------------------
*               生命不息、奋斗不止；前人栽树，后人乘凉！！！
*               开源不易，且学且珍惜，祝早日逆袭、进阶成功！！！
*               学习优秀者，简历可推荐到DJI、ZEROTECH、XAG、AEE、GDU、AUTEL、EWATT、HIGH GREAT等公司就业
*               求职简历请发送：15671678205@163.com，需备注求职意向单位、岗位、待遇等
*               无名创新开源飞控QQ群：2号群465082224、1号群540707961
*               CSDN博客：http://blog.csdn.net/u011992534
*               B站教学视频：https://space.bilibili.com/67803559/#/video				优酷ID：NamelessCotrun无名小哥
*               无名创新国内首款TI开源飞控设计初衷、知乎专栏:https://zhuanlan.zhihu.com/p/54471146
*               客户使用心得、改进意见征集贴：http://www.openedv.com/forum.php?mod=viewthread&tid=234214&extra=page=1
*               淘宝店铺：https://shop348646912.taobao.com/?spm=2013.1.1000126.2.5ce78a88ht1sO2
*               公司官网:www.nameless.tech
*               修改日期:2022/03/01                  
*               版本：躺赢者PRO——CarryPilot_V4.0.3
*               版权所有，盗版必究。
*               Copyright(C) 2019-2030 武汉无名创新科技有限公司 
*               All rights reserved
-----------------------------------------------------------------------------------------------------------------------
*               重要提示：
*               正常淘宝咸鱼转手的飞控、赠送朋友、传给学弟的都可以进售后群学习交流，
*               不得在网上销售无名创新资料，公司开放代码有软件著作权保护版权，他人不得将
*               资料代码传网上供他人下载，不得以谋利为目去销售资料代码，发现有此类操作者，
*               公司会提前告知，请1天内及时处理，否则你的侵权违规行为会被贴出在抖音、
*               今日头条、百家号、公司官网、微信公众平台、技术博客、知乎等平台予以公示曝光
*               此种侵权所为会成为个人终身污点，影响升学、找工作、社会声誉、很快就很在无人机界出名，后果很严重。
*               因此行为给公司造成重大损失者，会以法律途径解决，感谢您的合作，谢谢！！！
----------------------------------------------------------------------------------------------------------------------*/
#include "Headfile.h"
#include "Sensor.h"
#include "Filter.h"

#define  YAW_FUSION_MODE_QUAD  0	//1：偏航角来源于四元数更新，时间长容易偏移，
																	//   针对电赛室内环境磁场干扰大的情况，推荐此方式
																	//0：偏航角来源于偏航角速度与磁力计一阶互补，GPS定点时选择此方式
																	//   推荐采用外置磁力计数据
static float q0=1.0f,q1=0,q2=0,q3=0;																	
float Pitch_Observation,Roll_Observation,Yaw_Observation;//俯仰观测角、横滚观测角、偏航观测角
Vector3f_Body Circle_Angle;
float Gyro_Length=0;//陀螺仪模长
float q_backup[4][10]={0};
float K[3]={1.0,1.0,1.0};//默认标度(量程)误差
float B[3]={0,0,0};//默认零位误差
Sensor_Health Sensor_Flag;
float yaw_angle_deg_enu=0;
float yaw_angle_deg_enu_backups[20]={0};

float invSqrt(float x)
{
  float halfx = 0.5f * x;
  float y = x;
  long i = *(long*)&y;  
  i = 0x5f3759df - (i>>1);
  y = *(float*)&i;
  y = y * (1.5f - (halfx * y * y));
  return y;
}

float sqf(float x) {return ((x)*(x));}
void Compute_Rotation_Matrix_From_Euler(float *rmat_btn)
{
  WP_AHRS.sin_rpy[_PIT]=FastSin(WP_AHRS.Pitch*DEG2RAD);
  WP_AHRS.cos_rpy[_PIT]=FastCos(WP_AHRS.Pitch*DEG2RAD);
  WP_AHRS.sin_rpy[_ROL] =FastSin(WP_AHRS.Roll*DEG2RAD);
  WP_AHRS.cos_rpy[_ROL] =FastCos(WP_AHRS.Roll*DEG2RAD);
  WP_AHRS.sin_rpy[_YAW] =FastSin(WP_AHRS.Yaw *DEG2RAD);
  WP_AHRS.cos_rpy[_YAW] =FastCos(WP_AHRS.Yaw *DEG2RAD);
	
  rmat_btn[0]=WP_AHRS.cos_rpy[_YAW] * WP_AHRS.cos_rpy[_ROL];
  rmat_btn[1]=WP_AHRS.sin_rpy[_PIT] * WP_AHRS.sin_rpy[_ROL]*WP_AHRS.cos_rpy[_YAW]-WP_AHRS.cos_rpy[_PIT] * WP_AHRS.sin_rpy[_YAW];
  rmat_btn[2]=WP_AHRS.sin_rpy[_PIT] * WP_AHRS.sin_rpy[_YAW]+WP_AHRS.cos_rpy[_PIT] * WP_AHRS.sin_rpy[_ROL] * WP_AHRS.cos_rpy[_YAW];  
  rmat_btn[3]=WP_AHRS.sin_rpy[_YAW] * WP_AHRS.cos_rpy[_ROL];
  rmat_btn[4]=WP_AHRS.sin_rpy[_PIT] * WP_AHRS.sin_rpy[_ROL] * WP_AHRS.sin_rpy[_YAW] +WP_AHRS.cos_rpy[_PIT] * WP_AHRS.cos_rpy[_YAW];
  rmat_btn[5]=WP_AHRS.cos_rpy[_PIT] * WP_AHRS.sin_rpy[_ROL] * WP_AHRS.sin_rpy[_YAW] - WP_AHRS.sin_rpy[_PIT] * WP_AHRS.cos_rpy[_YAW];
  rmat_btn[6]=-WP_AHRS.sin_rpy[_ROL];
  rmat_btn[7]= WP_AHRS.sin_rpy[_PIT]* WP_AHRS.cos_rpy[_ROL];
  rmat_btn[8]= WP_AHRS.cos_rpy[_PIT]* WP_AHRS.cos_rpy[_ROL];
}

void Compute_Rotation_Matrix_From_Quad(float *rmat_btn)
{
	float a=WP_AHRS.quaternion[0];
	float b=WP_AHRS.quaternion[1];
	float c=WP_AHRS.quaternion[2];
	float d=WP_AHRS.quaternion[3];
	float bc=b*c;
	float ad=a*d;
	float bd=b*d;
	float ac=a*c;
	float cd=c*d;
  float ab=a*b;
	float a2=a*a;
	float b2=b*b;
	float c2=c*c;
	float d2=d*d;	
	
  rmat_btn[0]=a2+b2-c2-d2;
  rmat_btn[1]=2*(bc-ad);
  rmat_btn[2]=2*(bd+ac);  
  rmat_btn[3]=2*(bc+ad);
  rmat_btn[4]=a2-b2+c2-d2;
  rmat_btn[5]=2*(cd-ab);
  rmat_btn[6]=2*(bd-ac);
  rmat_btn[7]=2*(cd+ab);
  rmat_btn[8]=a2-b2-c2+d2;
	
	WP_AHRS.sin_rpy[_PIT]=FastSin(WP_AHRS.Pitch*DEG2RAD);
  WP_AHRS.cos_rpy[_PIT]=FastCos(WP_AHRS.Pitch*DEG2RAD);
  WP_AHRS.sin_rpy[_ROL] =FastSin(WP_AHRS.Roll*DEG2RAD);
  WP_AHRS.cos_rpy[_ROL] =FastCos(WP_AHRS.Roll*DEG2RAD);
  WP_AHRS.sin_rpy[_YAW] =FastSin(WP_AHRS.Yaw *DEG2RAD);
  WP_AHRS.cos_rpy[_YAW] =FastCos(WP_AHRS.Yaw *DEG2RAD);
}


uint8_t Get_Baro_Offset()
{
  static uint16_t baro_sample_cnt=0;
  if(baro_sample_cnt<=100) 
  {
    baro_sample_cnt++;
  }    
  if(baro_sample_cnt==100) 
  {
    WP_Sensor.baro_presure_offset=WP_Sensor.baro_pressure_raw;
    return 1;
  }
  else if(baro_sample_cnt==101) 
  {
    float Tempbaro=(float)(WP_Sensor.baro_pressure_raw/WP_Sensor.baro_presure_offset)*1.0f;
    WP_Sensor.baro_altitude = 4433000.0f * (1 - FastPow((float)(Tempbaro),0.190295f));
		if(baro_flag==1)
		{
			WP_Sensor.baro_altitude_div=(WP_Sensor.baro_altitude-WP_Sensor.last_baro_altitude)/(Baro_Update_Dt*0.001f);			
		  WP_Sensor.last_baro_altitude=WP_Sensor.baro_altitude;			
			WP_Sensor.baro_altitude_acc=(WP_Sensor.baro_altitude_div-WP_Sensor.last_baro_altitude_div)/(Baro_Update_Dt*0.001f);
			WP_Sensor.last_baro_altitude_div=WP_Sensor.baro_altitude_div;
			baro_flag=0;
		}
    return 1;
  }
  return 0;
}


float Accel_For_Cal[3]={0};
Vector3f gyro,accel,mag;
Vector3f gyro_filter,accel_filter,mag_filter,gyro_filter_bug;
Vector3f gyro_bpf_filter,accel_bpf_filter,accel_bpf_filter_bug;
Vector3f ins_accel_filter;
Butter_BufferData ins_accel_filter_buf[3],gyro_filter_buf_bug[3],gyro_filter_buf[3],accel_filter_buf[3],mag_filter_buf[3],accel_cal_filter_buf[3],accel_for_cal_filter_buf[3];
uint8_t mag_update_flag=0;
Butter_BufferData imu_temperature;
void Sensor_Update(void)
{
  //传感器数据0.5~0.8ms采集完毕
	WP_Sensor.last_temperature=WP_Sensor.temperature;
  ICM20689_Read_Data(&WP_Sensor.gyro_raw,&WP_Sensor.accel_raw,&WP_Sensor.temperature);
  WP_Sensor.imu_updtate_flag=1;
  
	Gyro_Calibration_Check(&WP_Sensor.gyro_raw);
  mag_update_flag=Compass_Read_Data(&WP_Sensor.mag_raw);
	Compass_Fault_Check();
  SPL06_Read_Data(&WP_Sensor.baro_temp_raw,&WP_Sensor.baro_pressure_raw);
  Get_Baro_Offset();
  
  //得到校准后的角速度、加速度、磁力计数据
  gyro.x=WP_Sensor.gyro_raw.x-gyro_offset.x;
  gyro.y=WP_Sensor.gyro_raw.y-gyro_offset.y;
  gyro.z=WP_Sensor.gyro_raw.z-gyro_offset.z;
  
  accel.x=K[0]*WP_Sensor.accel_raw.x-B[0]*G_TO_RAW;
  accel.y=K[1]*WP_Sensor.accel_raw.y-B[1]*G_TO_RAW;
  accel.z=K[2]*WP_Sensor.accel_raw.z-B[2]*G_TO_RAW;
  
  mag.x=WP_Sensor.mag_raw.x-mag_offset.x;
  mag.y=WP_Sensor.mag_raw.y-mag_offset.y;
  mag.z=WP_Sensor.mag_raw.z-mag_offset.z;
  //原始数据滤波处理 
	WP_Sensor.mag_intensity=pythagorous3(mag.x,mag.y,mag.z);	
	
	  //原始数据滤波处理	
  static float last_temperature;
	last_temperature=WP_Sensor._temperature;
  WP_Sensor._temperature=last_temperature+0.1f*(WP_Sensor.temperature-last_temperature);
	
	accel_filter.x=LPButterworth(accel.x,&accel_filter_buf[0],&Accel_Parameter);
	accel_filter.y=LPButterworth(accel.y,&accel_filter_buf[1],&Accel_Parameter);
	accel_filter.z=LPButterworth(accel.z,&accel_filter_buf[2],&Accel_Parameter);	
		
	gyro_filter.x=LPButterworth(gyro.x,&gyro_filter_buf_bug[0],&Gyro_Parameter);
  gyro_filter.y=LPButterworth(gyro.y,&gyro_filter_buf_bug[1],&Gyro_Parameter);
  gyro_filter.z=LPButterworth(gyro.z,&gyro_filter_buf_bug[2],&Gyro_Parameter);	

  ins_accel_filter.x=LPButterworth(accel.x,&ins_accel_filter_buf[0],&Ins_Accel_Parameter);
  ins_accel_filter.y=LPButterworth(accel.y,&ins_accel_filter_buf[1],&Ins_Accel_Parameter);
  ins_accel_filter.z=LPButterworth(accel.z,&ins_accel_filter_buf[2],&Ins_Accel_Parameter);
	
	
  Accel_For_Cal[0]=LPButterworth(WP_Sensor.accel_raw.x,&accel_for_cal_filter_buf[0],&Calibrate_Parameter);
  Accel_For_Cal[1]=LPButterworth(WP_Sensor.accel_raw.y,&accel_for_cal_filter_buf[1],&Calibrate_Parameter);
  Accel_For_Cal[2]=LPButterworth(WP_Sensor.accel_raw.z,&accel_for_cal_filter_buf[2],&Calibrate_Parameter);	  
  WP_Sensor.acce_filter[0]=Accel_For_Cal[0];
  WP_Sensor.acce_filter[1]=Accel_For_Cal[1];
  WP_Sensor.acce_filter[2]=Accel_For_Cal[2];
}




void INS_Sensor_Update(void)
{
  //传感器数据0.5~0.8ms采集完毕
	WP_Sensor.last_temperature=WP_Sensor.temperature;
  ICM20689_Read_Data(&WP_Sensor.gyro_raw,&WP_Sensor.accel_raw,&WP_Sensor.temperature);
  WP_Sensor.imu_updtate_flag=1;
	  
  //得到校准后的角速度、加速度、磁力计数据
  gyro.x=WP_Sensor.gyro_raw.x-gyro_offset.x;
  gyro.y=WP_Sensor.gyro_raw.y-gyro_offset.y;
  gyro.z=WP_Sensor.gyro_raw.z-gyro_offset.z;
  
  accel.x=K[0]*WP_Sensor.accel_raw.x-B[0]*G_TO_RAW;
  accel.y=K[1]*WP_Sensor.accel_raw.y-B[1]*G_TO_RAW;
  accel.z=K[2]*WP_Sensor.accel_raw.z-B[2]*G_TO_RAW;

	//原始数据滤波处理	
  static float last_temperature;
	last_temperature=WP_Sensor._temperature;
  WP_Sensor._temperature=last_temperature+0.1f*(WP_Sensor.temperature-last_temperature);
		
	accel_filter.x=LPButterworth(accel.x,&accel_filter_buf[0],&Accel_Parameter);
	accel_filter.y=LPButterworth(accel.y,&accel_filter_buf[1],&Accel_Parameter);
	accel_filter.z=LPButterworth(accel.z,&accel_filter_buf[2],&Accel_Parameter);	
		
	gyro_filter.x=LPButterworth(gyro.x,&gyro_filter_buf_bug[0],&Gyro_Parameter);
  gyro_filter.y=LPButterworth(gyro.y,&gyro_filter_buf_bug[1],&Gyro_Parameter);
  gyro_filter.z=LPButterworth(gyro.z,&gyro_filter_buf_bug[2],&Gyro_Parameter);	

  ins_accel_filter.x=LPButterworth(accel.x,&ins_accel_filter_buf[0],&Ins_Accel_Parameter);
  ins_accel_filter.y=LPButterworth(accel.y,&ins_accel_filter_buf[1],&Ins_Accel_Parameter);
  ins_accel_filter.z=LPButterworth(accel.z,&ins_accel_filter_buf[2],&Ins_Accel_Parameter);
		
  Accel_For_Cal[0]=LPButterworth(WP_Sensor.accel_raw.x,&accel_for_cal_filter_buf[0],&Calibrate_Parameter);
  Accel_For_Cal[1]=LPButterworth(WP_Sensor.accel_raw.y,&accel_for_cal_filter_buf[1],&Calibrate_Parameter);
  Accel_For_Cal[2]=LPButterworth(WP_Sensor.accel_raw.z,&accel_for_cal_filter_buf[2],&Calibrate_Parameter);
	
  WP_Sensor.acce_filter[0]=Accel_For_Cal[0];
  WP_Sensor.acce_filter[1]=Accel_For_Cal[1];
  WP_Sensor.acce_filter[2]=Accel_For_Cal[2];

  //计算三轴角速度，用于姿态内环控制
  WP_AHRS.Pitch_Gyro=gyro_filter.x*GYRO_CALIBRATION_COFF;
  WP_AHRS.Roll_Gyro =gyro_filter.y*GYRO_CALIBRATION_COFF;
  WP_AHRS.Yaw_Gyro  =gyro_filter.z*GYRO_CALIBRATION_COFF;
	
  WP_AHRS.Pitch_Gyro_Rad=DEG2RAD* WP_AHRS.Pitch_Gyro;
  WP_AHRS.Roll_Gyro_Rad =DEG2RAD* WP_AHRS.Roll_Gyro;
  WP_AHRS.Yaw_Gyro_Rad  =DEG2RAD* WP_AHRS.Yaw_Gyro;
	WP_AHRS.Accel_X_MPSS=accel.x*RAW_TO_G;
	WP_AHRS.Accel_Y_MPSS=accel.y*RAW_TO_G;
	WP_AHRS.Accel_Z_MPSS=accel.z*RAW_TO_G;
	Gyro_Length=FastSqrt(WP_AHRS.Yaw_Gyro*WP_AHRS.Yaw_Gyro+WP_AHRS.Pitch_Gyro*WP_AHRS.Pitch_Gyro+WP_AHRS.Roll_Gyro*WP_AHRS.Roll_Gyro);//单位deg/s
  
  Circle_Angle.Pit+=WP_AHRS.Pitch_Gyro*min_ctrl_dt;
  Circle_Angle.Rol+=WP_AHRS.Roll_Gyro*min_ctrl_dt;
  Circle_Angle.Yaw+=WP_AHRS.Yaw_Gyro*min_ctrl_dt;
  if(Circle_Angle.Pit<0.0f)   Circle_Angle.Pit+=360.0f;
  if(Circle_Angle.Pit>360.0f) Circle_Angle.Pit-=360.0f;
  if(Circle_Angle.Rol<0.0f)   Circle_Angle.Rol+=360.0f;
  if(Circle_Angle.Rol>360.0f) Circle_Angle.Rol-=360.0f;
  if(Circle_Angle.Yaw<0.0f)   Circle_Angle.Yaw+=360.0f;
  if(Circle_Angle.Yaw>360.0f) Circle_Angle.Yaw-=360.0f;
	
  Madgwick_AHRS_Update_IMU(gyro_filter.x,gyro_filter.y,gyro_filter.z
                           ,accel_filter.x,accel_filter.y,accel_filter.z
                             ,mag_filter.x,mag_filter.y,mag_filter.z
                               ,Gyro_Length);	
	
	
	WP_AHRS.Pitch= atan2f(2.0f * q2 * q3 + 2.0f * q0 * q1, -2.0f *q1 *q1 - 2.0f * q2* q2 + 1.0f) * RAD2DEG;		// Pitch
	WP_AHRS.Roll = asinf( 2.0f * q0 * q2 - 2.0f * q1 * q3) * RAD2DEG;																					// Roll

  WP_AHRS.q[0]=q0;
	WP_AHRS.q[1]=q1;
  WP_AHRS.q[2]=q2;
	WP_AHRS.q[3]=q3;
	
	static uint16_t _cnt=0;
	_cnt++;
	if(_cnt>=1)
	{
		SPL06_Read_Data(&WP_Sensor.baro_temp_raw,&WP_Sensor.baro_pressure_raw);
		Get_Baro_Offset();
		_cnt=0;		
	}

	
}


void OBS_Sensor_Update(void)
{
	Gyro_Calibration_Check(&WP_Sensor.gyro_raw);
  mag_update_flag=Compass_Read_Data(&WP_Sensor.mag_raw);
	Compass_Fault_Check();  
  mag.x=WP_Sensor.mag_raw.x-mag_offset.x;
  mag.y=WP_Sensor.mag_raw.y-mag_offset.y;
  mag.z=WP_Sensor.mag_raw.z-mag_offset.z;
  //原始数据滤波处理 
	WP_Sensor.mag_intensity=pythagorous3(mag.x,mag.y,mag.z);

	Observation_Angle_Calculate();//观测量计算
}



void Observation_Angle_Calculate(void)
{
  float ACCE_X_TEMP,ACCE_Y_TEMP,ACCE_Z_TEMP;
  float _magtemp[3]={0};  
  ACCE_X_TEMP=accel.x;
  ACCE_Y_TEMP=accel.y;
  ACCE_Z_TEMP=accel.z;
  Roll_Observation=-57.3*atan(ACCE_X_TEMP*invSqrt(ACCE_Y_TEMP*ACCE_Y_TEMP+ACCE_Z_TEMP*ACCE_Z_TEMP));//横滚角
  Pitch_Observation=57.3*atan(ACCE_Y_TEMP*invSqrt(ACCE_X_TEMP*ACCE_X_TEMP+ACCE_Z_TEMP*ACCE_Z_TEMP));//俯仰角
  /************磁力计倾角补偿*****************/
	if(mag_update_flag==1)
	{
		mag_filter.x=mag.x;
		mag_filter.y=mag.y;
		mag_filter.z=mag.z;			
		_magtemp[0]=mag_filter.x;
		_magtemp[1]=mag_filter.y;
		_magtemp[2]=mag_filter.z;
		

		/***********反正切得到磁力计观测角度*********/	
		magn.y=  _magtemp[1] * WP_AHRS.cos_rpy[_PIT]- _magtemp[2] * WP_AHRS.sin_rpy[_PIT];
		magn.x=  _magtemp[0] * WP_AHRS.cos_rpy[_ROL]
						+_magtemp[1] * WP_AHRS.sin_rpy[_ROL] * WP_AHRS.sin_rpy[_PIT]
					  +_magtemp[2] * WP_AHRS.sin_rpy[_ROL] * WP_AHRS.cos_rpy[_PIT];
		Yaw_Observation=FastAtan2(magn.x,magn.y)*57.296f;	
		if(Yaw_Observation<0) Yaw_Observation=Yaw_Observation+360;
		Yaw_Observation=constrain_float(Yaw_Observation,0,360);
	}
}

float euler_rpy_init[3];
float q_init[4];

void Euler_Angle_Init()
{
  float ACCE_X_TEMP,ACCE_Y_TEMP,ACCE_Z_TEMP;
  float _magtemp[3]={0};    
  ICM20689_Read_Data(&WP_Sensor.gyro_raw,&WP_Sensor.accel_raw,&WP_Sensor.temperature);
  accel.x=K[0]*WP_Sensor.accel_raw.x-B[0]*G_TO_RAW;
  accel.y=K[1]*WP_Sensor.accel_raw.y-B[1]*G_TO_RAW;
  accel.z=K[2]*WP_Sensor.accel_raw.z-B[2]*G_TO_RAW;
  ACCE_X_TEMP=accel.x;
  ACCE_Y_TEMP=accel.y;
  ACCE_Z_TEMP=accel.z;
  Roll_Observation=-57.3*atan(ACCE_X_TEMP*invSqrt(ACCE_Y_TEMP*ACCE_Y_TEMP+ACCE_Z_TEMP*ACCE_Z_TEMP));//横滚角
  Pitch_Observation=57.3*atan(ACCE_Y_TEMP*invSqrt(ACCE_X_TEMP*ACCE_X_TEMP+ACCE_Z_TEMP*ACCE_Z_TEMP));//俯仰角
  
  WP_AHRS.sin_rpy[_PIT]=sin(Pitch_Observation* DEG2RAD);
  WP_AHRS.cos_rpy[_PIT]=cos(Pitch_Observation* DEG2RAD);
  WP_AHRS.sin_rpy[_ROL]=sin(Roll_Observation* DEG2RAD);
  WP_AHRS.cos_rpy[_ROL]=cos(Roll_Observation* DEG2RAD);
  while(mag_update_flag==0)
  {
    delay_ms(5);
    mag_update_flag=Compass_Read_Data(&WP_Sensor.mag_raw);
  }
  /************磁力计倾角补偿*****************/
  _magtemp[0]=WP_Sensor.mag_raw.x-mag_offset.x;
  _magtemp[1]=WP_Sensor.mag_raw.y-mag_offset.y;
  _magtemp[2]=WP_Sensor.mag_raw.z-mag_offset.z;
  
  /***********反正切得到磁力计观测角度*********/
	magn.y=  _magtemp[1] * WP_AHRS.cos_rpy[_PIT]- _magtemp[2] * WP_AHRS.sin_rpy[_PIT];
	magn.x=  _magtemp[0] * WP_AHRS.cos_rpy[_ROL]
					+_magtemp[1] * WP_AHRS.sin_rpy[_ROL] * WP_AHRS.sin_rpy[_PIT]
					+_magtemp[2] * WP_AHRS.sin_rpy[_ROL] * WP_AHRS.cos_rpy[_PIT];
	Yaw_Observation=FastAtan2(magn.x,magn.y)*57.296f;	
	if(Yaw_Observation<0) Yaw_Observation=Yaw_Observation+360;
	Yaw_Observation=constrain_float(Yaw_Observation,0,360);
	yaw_angle_deg_enu=Yaw_Observation;
  euler_rpy_init[0]=Roll_Observation;  //初始化欧拉翻滚角
  euler_rpy_init[1]=Pitch_Observation; //初始化欧拉俯仰角
  euler_rpy_init[2]  =Yaw_Observation ;
}

void Euler_Angle_Init_Again(void)
{
  float ACCE_X_TEMP,ACCE_Y_TEMP,ACCE_Z_TEMP;
  float MagTemp[3]={0};    
  accel.x=K[0]*WP_Sensor.accel_raw.x-B[0]*G_TO_RAW;
  accel.y=K[1]*WP_Sensor.accel_raw.y-B[1]*G_TO_RAW;
  accel.z=K[2]*WP_Sensor.accel_raw.z-B[2]*G_TO_RAW;
  ACCE_X_TEMP=accel.x;
  ACCE_Y_TEMP=accel.y;
  ACCE_Z_TEMP=accel.z;
  Roll_Observation=-57.3*atan(ACCE_X_TEMP*invSqrt(ACCE_Y_TEMP*ACCE_Y_TEMP+ACCE_Z_TEMP*ACCE_Z_TEMP));//横滚角
  Pitch_Observation=57.3*atan(ACCE_Y_TEMP*invSqrt(ACCE_X_TEMP*ACCE_X_TEMP+ACCE_Z_TEMP*ACCE_Z_TEMP));//俯仰角  
  WP_AHRS.sin_rpy[_PIT]=sin(Pitch_Observation* DEG2RAD);
  WP_AHRS.cos_rpy[_PIT]=cos(Pitch_Observation* DEG2RAD);
  WP_AHRS.sin_rpy[_ROL]=sin(Roll_Observation* DEG2RAD);
  WP_AHRS.cos_rpy[_ROL]=cos(Roll_Observation* DEG2RAD);
  /************磁力计倾角补偿*****************/
  MagTemp[0]=WP_Sensor.mag_raw.x-mag_offset.x;
  MagTemp[1]=WP_Sensor.mag_raw.y-mag_offset.y;
  MagTemp[2]=WP_Sensor.mag_raw.z-mag_offset.z;
  magn.x=  MagTemp[0] * WP_AHRS.cos_rpy[_ROL]+ MagTemp[2] * WP_AHRS.sin_rpy[_ROL];
  magn.y=  MagTemp[0] * WP_AHRS.sin_rpy[_PIT]*WP_AHRS.sin_rpy[_ROL]
					+MagTemp[1] * WP_AHRS.cos_rpy[_PIT]
					-MagTemp[2] * WP_AHRS.sin_rpy[_PIT]*WP_AHRS.cos_rpy[_ROL];
  /***********反正切得到磁力计观测角度*********/
  Yaw_Observation=FastAtan2(magn.x,magn.y)*57.296f;
  Yaw_Observation=constrain_float(Yaw_Observation,0,360);
  yaw_angle_deg_enu=Yaw_Observation;	
  euler_rpy_init[0]=Roll_Observation;  //初始化欧拉翻滚角
  euler_rpy_init[1]=Pitch_Observation; //初始化欧拉俯仰角
  euler_rpy_init[2]=Yaw_Observation ;
	quad_from_euler(q_init,euler_rpy_init[0],euler_rpy_init[1],euler_rpy_init[2]);
  q0=q_init[0];
  q1=q_init[1];
  q2=q_init[2];
  q3=q_init[3];
	
	for(int16_t i=9;i>0;i--)
	{ 
	  q_backup[0][i]=q0;
		q_backup[1][i]=q1;
		q_backup[2][i]=q2;
		q_backup[3][i]=q3;
	}
		q_backup[0][0]=q0;
		q_backup[1][0]=q1;
		q_backup[2][0]=q2;
		q_backup[3][0]=q3;
}

void quad_from_euler(float *q, float roll, float pitch, float yaw)
{
	float sPitch2, cPitch2; // sin(phi/2) and cos(phi/2)
	float sRoll2 , cRoll2;  // sin(theta/2) and cos(theta/2)
	float sYaw2  , cYaw2;   // sin(psi/2) and cos(psi/2)
	//calculate sines and cosines
	
	FastSinCos(0.5f * roll*DEG2RAD, &sRoll2, &cRoll2);//roll
	FastSinCos(0.5f * pitch*DEG2RAD,&sPitch2,&cPitch2);//pitch
	FastSinCos(0.5f * yaw*DEG2RAD,  &sYaw2,  &cYaw2);//yaw
	

	// compute the quaternion elements
	q[0] = cPitch2*cRoll2*cYaw2+sPitch2*sRoll2*sYaw2;
	q[1] = sPitch2*cRoll2*cYaw2-cPitch2*sRoll2*sYaw2;
	q[2] = cPitch2*sRoll2*cYaw2+sPitch2*cRoll2*sYaw2;
	q[3] = cPitch2*cRoll2*sYaw2-sPitch2*sRoll2*cYaw2;

  // Normalise quaternion
  float recipNorm = invSqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
  q[0] *= recipNorm;
  q[1] *= recipNorm;
  q[2] *= recipNorm;
  q[3] *= recipNorm;
}


/****************** 根据初始化欧拉角初始化四元数 *****************************/
void Quaternion_Init()
{
  Euler_Angle_Init();
	quad_from_euler(q_init,euler_rpy_init[0],euler_rpy_init[1],euler_rpy_init[2]);
}

void WP_Quad_Init(void)//初始四元数初始化
{
  Quaternion_Init();
  q0=q_init[0];
  q1=q_init[1];
  q2=q_init[2];
  q3=q_init[3];
	
	for(int16_t i=9;i>0;i--)
	{ 
	  q_backup[0][i]=q0;
		q_backup[1][i]=q1;
		q_backup[2][i]=q2;
		q_backup[3][i]=q3;
	}
		q_backup[0][0]=q0;
		q_backup[1][0]=q1;
		q_backup[2][0]=q2;
		q_backup[3][0]=q3;
}
Testime Imu_Delta;
float IMU_Dt=0.0f;
float Yaw_Gyro_Earth_Frame=0;
#define sampleFreq 200
#define AHRS_SYNC_CNT 0
uint8_t ahrs_sync_flag=0;

volatile float beta=0.01f;//0.0175
float yaw_fus_beta=0.025f;//0.025f
float slam_yaw_fus_beta=0.025f;
uint16_t slam_yaw_fus_sync=10;
float accel_mode=0;
float gx_delta=0,gy_delta=0,gz_delta=0;
float kp=0;
void Madgwick_AHRS_Update_IMU(float gx, float gy, float gz, 
                              float ax, float ay, float az,
                              float mx, float my, float mz,
                              float gyro_mold) 
{
  float recipNorm;
  float s0, s1, s2, s3;
  float qDot1, qDot2, qDot3, qDot4;
  float _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2;
  float q0q0, q0q1, q0q2, q0q3, q1q1, q1q2, q1q3, q2q2, q2q3, q3q3;  
  float hx, hy, bx, bz;
  float halfwx, halfwy, halfwz;
  float halfex, halfey, halfez;
  float beta_temp=0;
	static uint16_t ahrs_sync_cnt=0;
  Test_Period(&Imu_Delta);
  IMU_Dt=(float)(Imu_Delta.Time_Delta/1000.0f);
	if(IMU_Dt>1.05f*min_ctrl_dt||IMU_Dt<0.95f*min_ctrl_dt||isnan(IMU_Dt)!=0)   IMU_Dt=min_ctrl_dt;
  gx*=GYRO_CALIBRATION_COFF;
  gy*=GYRO_CALIBRATION_COFF;
  gz*=GYRO_CALIBRATION_COFF;
  //{-sinθ          cosθsin Φ                          cosθcosΦ                   }
  Yaw_Gyro_Earth_Frame=-WP_AHRS.sin_rpy[_ROL]*gx+ WP_AHRS.cos_rpy[_ROL]*WP_AHRS.sin_rpy[_PIT] *gy+ WP_AHRS.cos_rpy[_PIT] * WP_AHRS.cos_rpy[_ROL] *gz;
  
  gx+=gx_delta;
  gy+=gy_delta;
  gz+=gz_delta;
  
  // Rate of change of quaternion from gyroscope
  qDot1 = 0.5f * (-q1 * gx - q2 * gy - q3 * gz)*DEG2RAD;
  qDot2 = 0.5f * ( q0 * gx + q2 * gz - q3 * gy)*DEG2RAD;
  qDot3 = 0.5f * ( q0 * gy - q1 * gz + q3 * gx)*DEG2RAD;
  qDot4 = 0.5f * ( q0 * gz + q1 * gy - q2 * gx)*DEG2RAD;
  
	ahrs_sync_cnt++;
	if(ahrs_sync_cnt>=2)//10ms
	{
		for(int16_t i=9;i>0;i--)
		{ 
			q_backup[0][i]=q_backup[0][i-1];
			q_backup[1][i]=q_backup[1][i-1];
			q_backup[2][i]=q_backup[2][i-1];
			q_backup[3][i]=q_backup[3][i-1];
		}
		ahrs_sync_cnt=0;
		ahrs_sync_flag=1;
	}
		q_backup[0][0]=q0;
		q_backup[1][0]=q1;
		q_backup[2][0]=q2;
		q_backup[3][0]=q3;
		
  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f))&&ahrs_sync_flag==1) 
  {
		accel_mode=GRAVITY_MSS*((safe_sqrt(sq(ax)+sq(ay)+sq(az))/GRAVITY_RAW)-1.0f);
    ahrs_sync_flag=0;		
    // Normalise accelerometer measurement
    recipNorm = invSqrt(ax * ax + ay * ay + az * az);
    ax *= recipNorm;
    ay *= recipNorm;
    az *= recipNorm;     
    // Normalise magnetometer measurement
    recipNorm = invSqrt(mx * mx + my * my + mz * mz);
    mx *= recipNorm;
    my *= recipNorm;
    mz *= recipNorm;		
    
    // Auxiliary variables to avoid repeated arithmetic  
		_2q0=2.0f *q_backup[0][AHRS_SYNC_CNT];//_2q0 = 2.0f * q0;
		_2q1=2.0f *q_backup[1][AHRS_SYNC_CNT];//_2q1 = 2.0f * q1;
		_2q2=2.0f *q_backup[2][AHRS_SYNC_CNT];//_2q2 = 2.0f * q2;
		_2q3=2.0f *q_backup[3][AHRS_SYNC_CNT];//_2q3 = 2.0f * q3;	
		_4q0=4.0f *q_backup[0][AHRS_SYNC_CNT];//_4q0 = 4.0f * q0;
		_4q1=4.0f *q_backup[1][AHRS_SYNC_CNT];//_4q1 = 4.0f * q1;
		_4q2=4.0f *q_backup[2][AHRS_SYNC_CNT];//_4q2 = 4.0f * q2;
		_8q1=8.0f *q_backup[1][AHRS_SYNC_CNT];//_8q1 = 8.0f * q1;
		_8q2=8.0f *q_backup[2][AHRS_SYNC_CNT];//_8q2 = 8.0f * q2;
		q0q0 = q_backup[0][AHRS_SYNC_CNT] * q_backup[0][AHRS_SYNC_CNT];//		q0q0 = q0 * q0;
    q0q1 = q_backup[0][AHRS_SYNC_CNT] * q_backup[1][AHRS_SYNC_CNT];//    q0q1 = q0 * q1;
    q0q2 = q_backup[0][AHRS_SYNC_CNT] * q_backup[2][AHRS_SYNC_CNT];//    q0q2 = q0 * q2;
    q0q3 = q_backup[0][AHRS_SYNC_CNT] * q_backup[3][AHRS_SYNC_CNT];//    q0q3 = q0 * q3;
    q1q1 = q_backup[1][AHRS_SYNC_CNT] * q_backup[1][AHRS_SYNC_CNT];//    q1q1 = q1 * q1;
    q1q2 = q_backup[1][AHRS_SYNC_CNT] * q_backup[2][AHRS_SYNC_CNT];//    q1q2 = q1 * q2;
    q1q3 = q_backup[1][AHRS_SYNC_CNT] * q_backup[3][AHRS_SYNC_CNT];//    q1q3 = q1 * q3;
    q2q2 = q_backup[2][AHRS_SYNC_CNT] * q_backup[2][AHRS_SYNC_CNT];//    q2q2 = q2 * q2;
    q2q3 = q_backup[2][AHRS_SYNC_CNT] * q_backup[3][AHRS_SYNC_CNT];//    q2q3 = q2 * q3;
    q3q3 = q_backup[3][AHRS_SYNC_CNT] * q_backup[3][AHRS_SYNC_CNT];//    q3q3 = q3 * q3;		  
    
    // Reference direction of Earth's magnetic field
    hx = 2.0f * (mx * (0.5f - q2q2 - q3q3) + my * (q1q2 - q0q3) + mz * (q1q3 + q0q2));
    hy = 2.0f * (mx * (q1q2 + q0q3) + my * (0.5f - q1q1 - q3q3) + mz * (q2q3 - q0q1));
    bx = sqrt(hx * hx + hy * hy);
    bz = 2.0f * (mx * (q1q3 - q0q2) + my * (q2q3 + q0q1) + mz * (0.5f - q1q1 - q2q2));
    
    halfwx = bx * (0.5f - q2q2 - q3q3) + bz * (q1q3 - q0q2);
    halfwy = bx * (q1q2 - q0q3) + bz * (q0q1 + q2q3);
    halfwz = bx * (q0q2 + q1q3) + bz * (0.5f - q1q1 - q2q2); 
    // Error is sum of cross product between estimated direction and measured direction of field vectors
    halfex = (my * halfwz - mz * halfwy);
    halfey = (mz * halfwx - mx * halfwz);
    halfez = (mx * halfwy - my * halfwx);
    
    gx_delta=kp*halfex;
    gy_delta=kp*halfey;
    gz_delta=kp*halfez;
    
    // Gradient decent algorithm corrective step
    s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
    s1 = _4q1 * q3q3 - _2q3 * ax + 4.0f * q0q0 * q_backup[1][AHRS_SYNC_CNT] - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
    s2 = 4.0f * q0q0 * q_backup[2][AHRS_SYNC_CNT] + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
    s3 = 4.0f * q1q1 * q_backup[3][AHRS_SYNC_CNT] - _2q1 * ax + 4.0f * q2q2 * q_backup[3][AHRS_SYNC_CNT] - _2q2 * ay;
    
		recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
    s0 *= recipNorm;
    s1 *= recipNorm;
    s2 *= recipNorm;
    s3 *= recipNorm;
    
    beta_temp=beta+0.01f*IMU_Dt*constrain_float(gyro_mold,0,500);//0.0175f  0.025 
		beta_temp-=0.0005f*(accel_mode/GRAVITY_MSS);//0.005
    beta_temp=constrain_float(beta_temp,beta,0.06f);
    
    // Apply feedback step
    qDot1 -= beta_temp * s0;
    qDot2 -= beta_temp * s1;
    qDot3 -= beta_temp * s2;
    qDot4 -= beta_temp * s3;
  }
  
  // Integrate rate of change of quaternion to yield quaternion
  q0 += qDot1 * IMU_Dt;
  q1 += qDot2 * IMU_Dt;
  q2 += qDot3 * IMU_Dt;
  q3 += qDot4 * IMU_Dt;
  
  // Normalise quaternion
  recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 *= recipNorm;
  q1 *= recipNorm;
  q2 *= recipNorm;
  q3 *= recipNorm;
}


Testime yaw_fus_time;
void Get_Status_Feedback(void)
{
	float yaw_obs_err=0;
	float yaw_fus_dt=0;
#if (YAW_FUSION_MODE_QUAD)
	WP_AHRS.Yaw = FastAtan2(2.0f * q1 * q2 + 2.0f * q0 * q3, -2.0f * q3 *q3 - 2.0f * q2 * q2 + 1.0f) * RAD2DEG;		// Yaw
#else
	Test_Period(&yaw_fus_time);
	yaw_fus_dt=(float)(yaw_fus_time.Time_Delta/1000.0f);
  if(yaw_fus_dt>1.05f*WP_Duty_Dt||yaw_fus_dt<0.95f*WP_Duty_Dt||isnan(yaw_fus_dt)!=0)   yaw_fus_dt=WP_Duty_Dt;
	//yaw_fus_dt=0.005f;
	
	if(current_state.rec_update_flag==1
		&&current_state.fault==0//定位异常，遥控器手动操作设定
		&&Optical_Type_Present==3)//设置的定位类型为RPLIDAR/T265
	{		
		//偏航角一阶互补融合
		yaw_angle_deg_enu+=Yaw_Gyro_Earth_Frame*yaw_fus_dt;
		if(yaw_angle_deg_enu<0)   yaw_angle_deg_enu+=360;
		if(yaw_angle_deg_enu>360) yaw_angle_deg_enu-=360;
		
		static uint16_t _cnt=0;
		_cnt++;
		if(_cnt>=2)
		{
			_cnt=0;
			for(uint16_t i=19;i>0;i--)	yaw_angle_deg_enu_backups[i]=yaw_angle_deg_enu_backups[i-1];
		}
		yaw_angle_deg_enu_backups[0]=yaw_angle_deg_enu;
		
		yaw_obs_err=current_state.rpy[2]-yaw_angle_deg_enu_backups[slam_yaw_fus_sync];
		if(current_state.rpy[2]>270&&yaw_angle_deg_enu_backups[slam_yaw_fus_sync]<90)      
			yaw_obs_err=yaw_obs_err-360;//yaw_obs_err=360-yaw_obs_err;
		else if(current_state.rpy[2]<90&&yaw_angle_deg_enu_backups[slam_yaw_fus_sync]>270) 
			yaw_obs_err=360+yaw_obs_err;
				
		if(current_state.rec_head_update_flag==1)
		{
			current_state.rec_head_update_flag=0;
			quad_getangle(current_state.q,current_state.rpy);
			if(current_state.rpy[2]<0.0f)   current_state.rpy[2]+=360.0f;
			if(current_state.rpy[2]>360.0f) current_state.rpy[2]-=360.0f;		
			yaw_angle_deg_enu +=yaw_obs_err* slam_yaw_fus_beta;
			//偏航角数据量化到0~360
		}
		if(yaw_angle_deg_enu<0)   WP_AHRS.Yaw=yaw_angle_deg_enu+360;
		else WP_AHRS.Yaw=yaw_angle_deg_enu;
	}
	else if(Sensor_Flag.Mag_Health==TRUE)
	{		
		//偏航角一阶互补融合
		yaw_angle_deg_enu+=Yaw_Gyro_Earth_Frame*yaw_fus_dt;
		if(yaw_angle_deg_enu<0)   yaw_angle_deg_enu+=360;
		if(yaw_angle_deg_enu>360) yaw_angle_deg_enu-=360;
		
		yaw_obs_err=Yaw_Observation-yaw_angle_deg_enu;
		if(Yaw_Observation>270&&yaw_angle_deg_enu<90)      
			yaw_obs_err=360-yaw_obs_err;
		else if(Yaw_Observation<90&&yaw_angle_deg_enu>270) 
			yaw_obs_err=360+yaw_obs_err;
		
		if(mag_update_flag==1)
		{
			mag_update_flag=0;			
			yaw_angle_deg_enu +=yaw_obs_err* yaw_fus_beta;
			//偏航角数据量化到0~360
	  }
		if(yaw_angle_deg_enu<0)   WP_AHRS.Yaw=yaw_angle_deg_enu+360;
		else WP_AHRS.Yaw=yaw_angle_deg_enu;
		if(GPS_Home_Set==1)  WP_AHRS.Yaw=WP_AHRS.Yaw-Declination;//如果GPS home点已设置，获取当地磁偏角，得到地理真北
	}
	else
	{
		WP_AHRS.Yaw = FastAtan2(2.0f * q1 * q2 + 2.0f * q0 * q3, -2.0f * q3 *q3 - 2.0f * q2 * q2 + 1.0f) * RAD2DEG;		// Yaw
	}
	
  if(WP_AHRS.Yaw<0.0f)   WP_AHRS.Yaw+=360.0f;
  if(WP_AHRS.Yaw>360.0f) WP_AHRS.Yaw-=360.0f;
	if(current_state.last_fault!=current_state.fault)	Total_Controller.Yaw_Angle_Control.Expect=WP_AHRS.Yaw;
#endif
	quad_from_euler(WP_AHRS.quaternion,WP_AHRS.Roll,WP_AHRS.Pitch,WP_AHRS.Yaw);	
	Compute_Rotation_Matrix_From_Quad(WP_AHRS.rMat);//Compute_Rotation_Matrix_From_Euler(WP_AHRS.rMat);
  //更新位置、速度、加速度
  SINS_Prepare();//惯导加速度更新
  Strapdown_INS_High_Kalman();//高度方向卡尔曼滤波估计竖直速度、位置
}























  
//			if((Yaw_Observation>90 && yaw_angle_deg_enu<-90)|| (Yaw_Observation<-90 && yaw_angle_deg_enu>90))
//					 yaw_angle_deg_enu = -yaw_angle_deg_enu * (1-yaw_fus_beta) + Yaw_Observation * yaw_fus_beta;//0.02
//			else yaw_angle_deg_enu =  yaw_angle_deg_enu * (1-yaw_fus_beta) + Yaw_Observation * yaw_fus_beta;
