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
#include "TOPSensor.h"

#define GROUND_RANGE_USE_I2C 0

enum TOFSENSE_TYPE
{
	TOFSENSE_UART=0,//量程0.01-5m
	TOFSENSE_P,     //量程0.03-8m
  TOFSENSE_F,     //量程0.05-15m
  TOFSENSE_FP,    //量程0.05-25m
	TOFSENSE_NUM_MAX
};

#define GROUND_RANGE_TOFSENSE  TOFSENSE_UART //对地测距传感器类型 
#define TOFSENSE_DATA_EFFECTIVE_RATE 0.85f   //数据有效率 
uint16_t TOFSENSR_MAX_RANGE[TOFSENSE_NUM_MAX]={500,800,1500,2500}; 




#define FRAME_HEADER 					0x57
#define Frame_MARK_READ_FRAME 0x10

void Check_Front_Tofsense(void);



void NLink_Data_Send(uint8_t *buf, uint32_t cnt)  
{
	USART7_Send(buf,cnt);//用户移植时，重写此串口发送函数
}


void  NLink_TOFSense_Read_Frame(uint8_t id)
{
	uint8_t buf[8];
	uint8_t check_sum=0;
	buf[0]=FRAME_HEADER;
	buf[1]=Frame_MARK_READ_FRAME;
	buf[2]=0xff;
	buf[3]=0xff;
	buf[4]=id;
	buf[5]=0xff;
	buf[6]=0xff;
	check_sum=buf[0]+buf[1]+buf[2]+buf[3]+buf[4]+buf[5]+buf[6];
	buf[7]=check_sum;//0x63;
	NLink_Data_Send(buf,8);
}

Testime tof_t[2];
void TOFSense_Data_Receive_Anl(uint8_t *data_buf,uint8_t num)
{
  uint8_t sum = 0;
	uint8_t id=0;
  for(uint8_t i=0;i<(num-1);i++)  sum+=*(data_buf+i);
  if(!(sum==*(data_buf+num-1))) 	return;//不满足和校验条件
  if(!(*(data_buf)==0x57 && *(data_buf+1)==0x00))return;//不满足帧头条件
	id=data_buf[3];
	
	tofdata[id].pre_last_distance=tofdata[id].last_distance;//上上次高度
	tofdata[id].last_distance=tofdata[id].distance;//上次高度
	tofdata[id].last_vel=tofdata[id].vel;//上次速度
	tofdata[id].last_system_time=tofdata[id].system_time;
	tofdata[id].id=data_buf[3];
	tofdata[id].system_time=data_buf[4]|(data_buf[5]<<8)|(data_buf[6]<<16)|(data_buf[7]<<24);
	tofdata[id].dis=(int32_t)(data_buf[8]<< 8|data_buf[9]<<16|data_buf[10]<<24)/256;
	tofdata[id].dis_status=data_buf[11];
	tofdata[id].signal_strength=data_buf[12]|(data_buf[13]<<8);
	tofdata[id].distance=tofdata[id].dis/10.0f;//cm
	tofdata[id].vel=(tofdata[id].distance-tofdata[id].last_distance)/0.1f; //观测速度
	tofdata[id].acc=(tofdata[id].vel-tofdata[id].last_vel)/0.1f;					  //观测加速度				
}


uint8_t tofsense_buf[20];
void TOF_Sense_Prase(uint8_t data)
{
  static uint8_t data_len = 0,data_cnt = 0;
  static uint8_t state = 0;
  if(state==0&&data==0x57)//判断帧头1
  {
    state=1;
    tofsense_buf[0]=data;
		Test_Period(&tof_t[0]);
  }
  else if(state==1&&data==0x00)//判断帧头2
  {
    state=2;
    tofsense_buf[1]=data;
  }
  else if(state==2&&data==0xff)//功能字节
  {
    state=3;
    tofsense_buf[2]=data;
  }
  else if(state==3&&data<=0xff)//id
  {
    state = 4;
    tofsense_buf[3]=data;
    data_len = 11;
    data_cnt = 0;
  }
  else if(state==4&&data_len>0)//数据接收
  {
    data_len--;
    tofsense_buf[4+data_cnt++]=data;
    if(data_len==0)  state = 5;
  }
  else if(state==5)//和校验
  {
    state = 0;
    tofsense_buf[4+data_cnt++]=data;
		TOFSense_Data_Receive_Anl(tofsense_buf,16);
		Test_Period(&tof_t[1]);
  }
  else state = 0;
}
		
		
TOFSensor_Data tofdata[TOFSENSE_MAX_NUMBER]; 
systime tofsensor;
void TOF_Statemachine(void)
{
	static uint16_t inquire_cnt=0;
	//inquire_cnt++;
	switch(inquire_cnt)
	{
		case 2 :
		{
			NLink_TOFSense_Read_Frame(0x00);
		}
		break;
		case 4 :
		{
			NLink_TOFSense_Read_Frame(0x01);	
		}
		break;
		case 6:
		{
			NLink_TOFSense_Read_Frame(0x02);
		}
		break;
		case 8:
		{
			NLink_TOFSense_Read_Frame(0x03);
		}
		break;
		case 10:
		{
			NLink_TOFSense_Read_Frame(0x04);
		}
		break;
		case 12:
		{
			NLink_TOFSense_Read_Frame(0x05);
		}
		break;
		case 14:
		{
			NLink_TOFSense_Read_Frame(0x06);
		}
		break;
		case 16:
		{
			NLink_TOFSense_Read_Frame(0x07);
		}
		break;
		case 18:
		{
			NLink_TOFSense_Read_Frame(0x08);
		}
		break;
		case 20:
		{
			inquire_cnt=0;
			NLink_TOFSense_Read_Frame(0x09);
		}
		break;
		default:	;
	}
	
	Check_Front_Tofsense();

#if (GROUND_RANGE_USE_I2C==0) 	
  static uint16_t _cnt=0;
	_cnt++;
  if(_cnt>=20)//100ms=10hz
  {
		_cnt=0;					
		GD_Distance=tofdata[0].distance*WP_AHRS.rMat[8];
		GD_Distance_Div=tofdata[0].vel;
		GD_Distance_Acc=tofdata[0].acc;		
		WP_Sensor.tofsensor_updtate_flag=1;	
	}			
	if((GD_Distance<=TOFSENSR_MAX_RANGE[GROUND_RANGE_TOFSENSE]*TOFSENSE_DATA_EFFECTIVE_RATE
		&&GD_Distance>-0.01f)//测得距离在量程内
     &&(tofdata[0].signal_strength!=0))//且信号强度不为0
  	Sensor_Flag.Ground_Health=1;
	else  Sensor_Flag.Ground_Health=0;
#else
  static uint16_t _cnt=0;
	_cnt++;
  if(_cnt>=20)//100ms=10hz
  {
		_cnt=0;					
		GD_Distance=TOFSense_F_Data.distance*WP_AHRS.rMat_Euler[8];
		GD_Distance_Div=TOFSense_F_Data.vel;
		GD_Distance_Acc=TOFSense_F_Data.acc;		
		WP_Sensor.tofsensor_updtate_flag=1;	
	}			
	if((GD_Distance<=TOFSENSR_MAX_RANGE[GROUND_RANGE_TOFSENSE]*TOFSENSE_DATA_EFFECTIVE_RATE
		&&GD_Distance>-0.01f)//测得距离在量程内
     &&(TOFSense_F_Data.signal_strength!=0))//且信号强度不为0
  	Sensor_Flag.Ground_Health=1;
	else  Sensor_Flag.Ground_Health=0;	
#endif
}





float front_tofsense_distance=0;
uint8_t front_tofsense_distance_valid_cnt=0;
uint8_t front_tofsense_distance_valid_flag=0;
void Check_Front_Tofsense()
{
	float _dis[5]={0},tmp=0;
	front_tofsense_distance_valid_cnt=0;
	front_tofsense_distance_valid_flag=0;
	uint8_t i=0;
	for(uint8_t j=1;j<5;j++)
	{
		if(tofdata[j].signal_strength!=0) 
		{
			front_tofsense_distance_valid_cnt++;
			_dis[i++]=tofdata[j].distance;
			front_tofsense_distance_valid_flag=1;
		}
	}
	
	//新增一组超声波测距US100
	if(us100_front.distance>0&&us100_front.distance<200)
	{
		front_tofsense_distance_valid_cnt++;
		_dis[i++]=us100_front.distance;
		front_tofsense_distance_valid_flag=1;
	}
		
	if(front_tofsense_distance_valid_cnt==0) 			front_tofsense_distance=200;//没有扫描到塔杆，默认给200cm
	else if(front_tofsense_distance_valid_cnt==1) front_tofsense_distance=_dis[0];
	else if(front_tofsense_distance_valid_cnt==2) front_tofsense_distance=fminf(_dis[0],_dis[1]);
	else if(front_tofsense_distance_valid_cnt==3) 
	{
		tmp=fminf(_dis[0],_dis[1]);
		tmp=fminf(tmp,_dis[2]);
		front_tofsense_distance=tmp;
	}
	else if(front_tofsense_distance_valid_cnt==4)  
	{
		tmp=fminf(_dis[0],_dis[1]);
		tmp=fminf(tmp,_dis[2]);
		tmp=fminf(tmp,_dis[3]);
		front_tofsense_distance=tmp;
	}
	else if(front_tofsense_distance_valid_cnt==5)  
	{
		tmp=fminf(_dis[0],_dis[1]);
		tmp=fminf(tmp,_dis[2]);
		tmp=fminf(tmp,_dis[3]);
		tmp=fminf(tmp,_dis[4]);
		front_tofsense_distance=tmp;
	}	
	
}







#define TOFSENSE_IIC_ADDRESS    0x08//7位地址
#define PRODUCT_VERSION_REG  		0x00
#define HARDWAVE_VERSION_REG 		0x04
#define FIRMWAVE_VERSION_REG 		0x08

#define INTERFACE_MODE_REG   		0x0C
#define UART_BAUDRATE_REG    		0x10
#define CAN_BAUDRATE_REG     		0x14
#define FOV_REG              		0x18
#define BAND_REG             		0x1C
#define SYSTIME_REG          		0x20
#define DISTANCE_REG         		0x24
#define SIGNAL_STRENGTH_REG  		0x28
#define RANGE_PRECISION_REG  		0x2C


TOFSense_F TOFSense_F_Data;

uint8_t f_buf[20];
void TOFSense_IIC_Update(void)
{
#if (GROUND_RANGE_USE_I2C==1) 
	static uint32_t _cnt=0;
	_cnt++;
	if(_cnt%20==0)//10hz数据刷新,最大50hz
	{
		f_buf[0]=Single_ReadI2C0(TOFSENSE_IIC_ADDRESS,INTERFACE_MODE_REG+1);//id
		i2c0ReadNByte(TOFSENSE_IIC_ADDRESS,SYSTIME_REG,&f_buf[1],13);

		
		TOFSense_F_Data.pre_last_distance=TOFSense_F_Data.last_distance;//上上次高度
		TOFSense_F_Data.last_distance=TOFSense_F_Data.distance;//上次高度
		TOFSense_F_Data.last_vel=TOFSense_F_Data.vel;//上次速度
		TOFSense_F_Data.last_system_time=TOFSense_F_Data.system_time;
		//数据更新
		TOFSense_F_Data.id=f_buf[0];
		TOFSense_F_Data.system_time=f_buf[1]|(f_buf[2]<<8)|(f_buf[3]<<16)|(f_buf[4]<<24);
		TOFSense_F_Data.dis=f_buf[5]|(f_buf[6]<<8)|(f_buf[7]<<16)|(f_buf[8]<<24);
		TOFSense_F_Data.dis_status=f_buf[9];
		TOFSense_F_Data.signal_strength=f_buf[11]|(f_buf[12]<<8);
		TOFSense_F_Data.range_precision=f_buf[13];
		
		TOFSense_F_Data.distance=TOFSense_F_Data.dis/10.0f;//cm
		TOFSense_F_Data.vel=(TOFSense_F_Data.distance-TOFSense_F_Data.last_distance)/0.05f; //观测速度
		TOFSense_F_Data.acc=(TOFSense_F_Data.vel-TOFSense_F_Data.last_vel)/0.05f;					  //观测加速度		
	}
#endif
}
	

























//						uint8_t id=0;
//						
//						id=COM7_Rx_Buf.Ring_Buff[i+3];
//						
//						tofdata[id].pre_last_distance=tofdata[id].last_distance;//上上次高度
//						tofdata[id].last_distance=tofdata[id].distance;//上次高度
//						tofdata[id].last_vel=tofdata[id].vel;//上次速度
//						tofdata[id].last_system_time=tofdata[id].system_time;
//						
//						
//						tofdata[id].id=COM7_Rx_Buf.Ring_Buff[i+3];
//						tofdata[id].system_time=COM7_Rx_Buf.Ring_Buff[i+4]|(COM7_Rx_Buf.Ring_Buff[i+5]<<8)|(COM7_Rx_Buf.Ring_Buff[i+6]<<16)|(COM7_Rx_Buf.Ring_Buff[i+7]<<24);
//					
//  					//if(tofdata.last_system_time==tofdata.system_time)//数据未更新 
//			
//						Get_Systime(&tofsensor);
//						
//						tofdata[id].dis=(int32_t)(COM7_Rx_Buf.Ring_Buff[8]<< 8|COM7_Rx_Buf.Ring_Buff[9]<<16|COM7_Rx_Buf.Ring_Buff[10]<<24)/256;
//						tofdata[id].dis_status=COM7_Rx_Buf.Ring_Buff[i+11];
//						tofdata[id].signal_strength=COM7_Rx_Buf.Ring_Buff[i+12]|(COM7_Rx_Buf.Ring_Buff[i+13]<<8);

//				
//						tofdata[id].distance=tofdata[id].dis/10.0f;//cm
//						tofdata[id].vel=(tofdata[id].distance-tofdata[id].last_distance)/0.1f; //观测速度
//						tofdata[id].acc=(tofdata[id].vel-tofdata[id].last_vel)/0.1f;					  //观测加速度

