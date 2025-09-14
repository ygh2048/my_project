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
#include "myiic.h"
#include "ICM2068X.h"
Butter_Parameter Ins_Accel_Parameter;
Butter_Parameter Accel_Parameter;
Butter_Parameter Gyro_Parameter;
Butter_Parameter Calibrate_Parameter;
 

IMU_ID_READ IMU_ID=WHO_AM_I_MPU6050;
uint8_t imu_address=ICM20689_ADRESS;
/***********************************************************
@函数名：ICM20689_Init
@入口参数：无
@出口参数：无
功能描述：MPU6050初始化
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/
uint8_t ICM20689_Init(void)//ICM20689初始化
{
	uint8_t fault=0;
	i2cWrite(imu_address,PWR_MGMT_1, 0x81);//软件强制复位81
	delay_ms(100);	
	IMU_ID=(IMU_ID_READ)(Single_ReadI2C(imu_address,WHO_AM_I));
	if(IMU_ID==0xFF)
	{
		imu_address=ICM20689_ADRESS+1;
		i2cWrite(imu_address,PWR_MGMT_1, 0x81);//软件强制复位81
		delay_ms(100);	
		IMU_ID=(IMU_ID_READ)(Single_ReadI2C(imu_address,WHO_AM_I));
	}
	switch(IMU_ID)
	{
		case WHO_AM_I_MPU6050:
		{
			i2cWrite(imu_address,PWR_MGMT_1  , 0x80);//软件复位
			delay_ms(200);
			i2cWrite(imu_address,SMPLRT_DIV  , 0x00); // sample rate.  Fsample= 1Khz/(<this value>+1) = 1000Hz
			i2cWrite(imu_address,PWR_MGMT_1  , 0x03); //时钟源 PLL with Z Gyro reference
			i2cWrite(imu_address,MPU_CONFIG  , 0x02); //内部低通滤波频率，加速度计184hz，陀螺仪188hz  //默认0x03	  
			i2cWrite(imu_address,GYRO_CONFIG , 0x08); //500deg/s
			i2cWrite(imu_address,ACCEL_CONFIG, 0x08); // Accel scale 4g (8192 LSB/g)			
		}
		break;
		case WHO_AM_I_ICM20689:
		{
			i2cWrite(imu_address,PWR_MGMT_1  , 0x00);//关闭所有中断,解除休眠
			i2cWrite(imu_address,SMPLRT_DIV  , 0x00); // sample rate.  Fsample= 1Khz/(<this value>+1) = 1000Hz	
			delay_ms(100);			
			i2cWrite(imu_address,MPU_CONFIG  , 0x02);//0x00设置陀螺仪、温度内部低通滤波频率范围，陀螺仪250hz，噪声带宽306.6hz，温度4000hz
																														//0x01设置陀螺仪、温度内部低通滤波频率范围，陀螺仪176hz，噪声带宽177hz，温度188hz
																														//0x02设置陀螺仪、温度内部低通滤波频率范围，陀螺仪92hz，噪声带宽108.6hz，温度98hz
																														//0x03设置陀螺仪、温度内部低通滤波频率范围，陀螺仪41hz，噪声带宽59hz，温度42hz		
																														//0x04设置陀螺仪、温度内部低通滤波频率范围，陀螺仪20hz，噪声带宽30.5hz，温度20hz
																														//0x05设置陀螺仪、温度内部低通滤波频率范围，陀螺仪10hz，噪声带宽15.6hz，温度10hz
			i2cWrite(imu_address,GYRO_CONFIG , 0x08);//设置陀螺仪量程，500deg/s
			i2cWrite(imu_address,ACCEL_CONFIG, 0x08);// Accel scale 4g (8192 LSB/g)	
			i2cWrite(imu_address,ACCEL_CONFIG2,0x03);
																										 //0x00设置加速度计内部低通滤波频率范围，加速度218.1hz，噪声带宽235hz		
																										 //0x01设置加速度计内部低通滤波频率范围，加速度218.1hz，噪声带宽235hz
																										 //0x02设置加速度计内部低通滤波频率范围，加速度99.0hz，噪声带宽121.3hz		
																										 //0x03设置加速度计内部低通滤波频率范围，加速度44.8hz，噪声带宽61.5hz
																										 //0x04设置加速度计内部低通滤波频率范围，加速度21.2hz，噪声带宽31.0hz
																										 //0x05设置加速度计内部低通滤波频率范围，加速度10.2hz，噪声带宽15.5hz	
			//Single_WriteI2C(imu_address,ACCEL_CONFIG2,0xC3);//设置加速度计内部低通滤波频率范围，加速度1046.0hz，噪声带宽1100.0hz
			delay_ms(500);		
		}
		break;	
		case WHO_AM_I_ICM20608D:
		case WHO_AM_I_ICM20608G:
		case WHO_AM_I_ICM20602:				
		{
			i2cWrite(imu_address,PWR_MGMT_1,0X80);	//复位ICM20608
			delay_ms(100);
			i2cWrite(imu_address,PWR_MGMT_1, 0X01);	//唤醒ICM20608
			
			i2cWrite(imu_address,0x19, 0x00);   /* 输出速率是内部采样率 */
			i2cWrite(imu_address,0x1A, 0x02);   /* 陀螺仪低通滤波BW=20Hz */
			i2cWrite(imu_address,0x1B, 0x08);   /* 陀螺仪±500dps量程 */
			i2cWrite(imu_address,0x1C, 0x08);   /* 加速度计±4G量程 */
			i2cWrite(imu_address,0x1D, 0x03);   /* 加速度计低通滤波BW=21.2Hz */
			
			i2cWrite(imu_address,0x6C, 0x00);   /* 打开加速度计和陀螺仪所有轴 */
			i2cWrite(imu_address,0x1E, 0x00);   /* 关闭低功耗 */
			i2cWrite(imu_address,0x23, 0x00);   /* 关闭FIFO */ 	
		}
		break;
		default:
		{
			fault=1;
		}			
	}
	
  delay_ms(500);	
  IMU_Calibration();
  Set_Cutoff_Frequency(Imu_Sampling_Freq, 95,&Gyro_Parameter);//姿态角速度反馈滤波参数  95 60 50 Imu_Sampling_Freq
  Set_Cutoff_Frequency(Imu_Sampling_Freq, 30,&Accel_Parameter);//姿态解算加计修正滤波值30  20 30
  Set_Cutoff_Frequency(Imu_Sampling_Freq, 2 ,&Calibrate_Parameter);//传感器校准加计滤波值
  Set_Cutoff_Frequency(Imu_Sampling_Freq, 60,&Ins_Accel_Parameter);//INS加计滤波值
	
	return fault;
}

/***********************************************************
@函数名：ICM20689_Read_Data
@入口参数：vector3f *gyro,vector3f *accel
@出口参数：无
功能描述：MPU6050数据采集
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/
void ICM20689_Read_Data(vector3f *gyro,vector3f *accel,float *temperature)//读取MPU6050数据
{
	uint8_t buf[14];
	int16_t temp;
	i2cReadData(imu_address,ACCEL_XOUT_H,buf,14);
	switch(IMU_ID)
	{
		case WHO_AM_I_MPU6050:
		{
			accel->x=(int16_t)((buf[0]<<8)|buf[1]);
			accel->y=-(int16_t)((buf[2]<<8)|buf[3]);
			accel->z=-(int16_t)((buf[4]<<8)|buf[5]);	
			temp		=(int16_t)((buf[6]<<8)|buf[7]);
			gyro->x	=(int16_t)((buf[8]<<8)|buf[9]);
			gyro->y	=-(int16_t)((buf[10]<<8)|buf[11]);
			gyro->z	=-(int16_t)((buf[12]<<8)|buf[13]);	
			*temperature=36.53f+(float)(temp/340.0f);	
		}
		break;
		case WHO_AM_I_ICM20689:
		{
			accel->x=(int16_t)((buf[0]<<8)|buf[1]);
			accel->y=-(int16_t)((buf[2]<<8)|buf[3]);
			accel->z=-(int16_t)((buf[4]<<8)|buf[5]);	
			temp		=(int16_t)((buf[6]<<8)|buf[7]);
			gyro->x	=(int16_t)((buf[8]<<8)|buf[9]);
			gyro->y	=-(int16_t)((buf[10]<<8)|buf[11]);
			gyro->z	=-(int16_t)((buf[12]<<8)|buf[13]);	
			*temperature=25.0f+(double)((temp-25.0f)/326.8f);	
		}
		break;	
		case WHO_AM_I_ICM20608D:
		case WHO_AM_I_ICM20608G:
		case WHO_AM_I_ICM20602:				
		{
			accel->y=(int16_t)((buf[0]<<8)|buf[1]);
			accel->x=(int16_t)((buf[2]<<8)|buf[3]);
			accel->z=-(int16_t)((buf[4]<<8)|buf[5]);	
			temp		=(int16_t)((buf[6]<<8)|buf[7]);
			gyro->y	=(int16_t)((buf[8]<<8)|buf[9]);
			gyro->x	=(int16_t)((buf[10]<<8)|buf[11]);
			gyro->z	=-(int16_t)((buf[12]<<8)|buf[13]);	
			
			*temperature=25.0f+(double)((temp-25.0f)/326.8f);		
		}
		break;
		default:
		{
			accel->x=(int16_t)((buf[0]<<8)|buf[1]);
			accel->y=-(int16_t)((buf[2]<<8)|buf[3]);
			accel->z=-(int16_t)((buf[4]<<8)|buf[5]);	
			temp		=(int16_t)((buf[6]<<8)|buf[7]);
			gyro->x	=(int16_t)((buf[8]<<8)|buf[9]);
			gyro->y	=-(int16_t)((buf[10]<<8)|buf[11]);
			gyro->z	=-(int16_t)((buf[12]<<8)|buf[13]);	
			*temperature=36.53f+(float)(temp/340.0f);				
		}			
	}
	
}


Vector3f gyro_offset;
s32 g_Gyro_xoffset = 0, g_Gyro_yoffset = 0, g_Gyro_zoffset = 0;
/***********************************************************
@函数名：IMU_Calibration
@入口参数：无
@出口参数：无
功能描述：陀螺仪开机零偏标定
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/
void IMU_Calibration(void)
{
	vector3f gyro_offset_temp;
	ReadFlashParameterOne(GYRO_X_OFFSET,&gyro_offset_temp.x);
	ReadFlashParameterOne(GYRO_Y_OFFSET,&gyro_offset_temp.y);
	ReadFlashParameterOne(GYRO_Z_OFFSET,&gyro_offset_temp.z);	
	if(isnan(gyro_offset_temp.x)==0
		&&isnan(gyro_offset_temp.y)==0
		 &&isnan(gyro_offset_temp.z)==0)//如果之前已经温度校准过，开机时直接用之前校准的数据 
	{
		 gyro_offset.x=gyro_offset_temp.x;
		 gyro_offset.y=gyro_offset_temp.y;
		 gyro_offset.z=gyro_offset_temp.z;
	}
	
	Gyro_Safety_Calibration_Flag=0;//待温度稳定后，自动校准陀螺仪零偏
}











//	0x00,//PWR_MGMT_1    00
//	0x00,//SMPLRT_DIV	   00
//	0x00,//MPU_CONFIG    02
//	0x08,//GYRO_CONFIG   08
//	0x08,//ACCEL_CONFIG  08
//	0x00//ACCEL_CONFIG2  03
