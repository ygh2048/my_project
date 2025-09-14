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
#include "Subtask_Demo.h"


uint16_t flight_subtask_cnt[FLIGHT_SUBTASK_NUM]={0};//飞行任务子线程计数器，可以用于控制每个航点子线程的执行
uint32_t flight_global_cnt[FLIGHT_SUBTASK_NUM]={0}; //飞行任务子线全局计数器，可以结合位置偏差用于判断判断航点是否到达
uint32_t execute_time_ms[FLIGHT_SUBTASK_NUM]={0};		//飞行任务子线执行时间，可以用于设置某个子线程的执行时间
uint32_t flight_global_cnt2[FLIGHT_SUBTASK_NUM]={0};//飞行任务子线全局计数器2
#define flight_subtask_delta 5//5ms
Vector3f base_position;//用于记录导航基准原点位置

void flight_subtask_reset(void)
{
	for(uint16_t i=0;i<FLIGHT_SUBTASK_NUM;i++)
	{
		flight_subtask_cnt[i]=0;
		execute_time_ms[i]=0;
		flight_global_cnt[i]=0;
		flight_global_cnt2[i]=0;
	}
}


//基本飞行保障必备子函数——定高、定点
void basic_auto_flight_support(void)
{
	OpticalFlow_Control_Pure(0);//SLAM定点控制
	Flight.yaw_ctrl_mode=ROTATE;//偏航控制为手动模式
	Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
	Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
}


/***********************************************************************************************************/
//顺时针转动90度，完成后降落
void flight_subtask_1(void)
{
	static uint8_t n=0;
	if(flight_subtask_cnt[n]==0)
	{
		Flight.yaw_ctrl_mode=CLOCKWISE;
		Flight.yaw_ctrl_start=1;
		Flight.yaw_outer_control_output  =90;//顺时针90度	
		OpticalFlow_Control_Pure(0);	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		flight_subtask_cnt[n]=1;		
	}
	else if(flight_subtask_cnt[n]==1)
	{
		Flight.yaw_ctrl_mode=CLOCKWISE;
		Flight.yaw_outer_control_output  =0;
		OpticalFlow_Control_Pure(0);	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		
		if(Flight.yaw_ctrl_end==1)  flight_subtask_cnt[n]=2;//执行完毕后，切换到下一阶段	
	}
	else if(flight_subtask_cnt[n]==2)
	{
		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		OpticalFlow_Control_Pure(0);
		Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-30);//高度控制
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}





/***********************************************************************************************************/
//逆时针转动90度，完成后降落
void flight_subtask_2(void)
{
	static uint8_t n=1;
	if(flight_subtask_cnt[n]==0)
	{
		Flight.yaw_ctrl_mode=ANTI_CLOCKWISE;
		Flight.yaw_ctrl_start=1;
		Flight.yaw_outer_control_output  =90;//逆时针90度	
		OpticalFlow_Control_Pure(0);	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		flight_subtask_cnt[n]=1;		
	}
	else if(flight_subtask_cnt[n]==1)
	{
		Flight.yaw_ctrl_mode=ANTI_CLOCKWISE;
		Flight.yaw_outer_control_output  =0;
		OpticalFlow_Control_Pure(0);	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制		
		if(Flight.yaw_ctrl_end==1)  flight_subtask_cnt[n]=2;//执行完毕后，切换到下一阶段		
	}
	else if(flight_subtask_cnt[n]==2)
	{
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		OpticalFlow_Control_Pure(0);
		Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-30);//高度控制
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}





/***********************************************************************************************************/
//以10deg/s的角速度顺时针转动10000ms，完成后降落
void flight_subtask_3(void)
{
	static uint8_t n=2;
	if(flight_subtask_cnt[n]==0)
	{
		
		Flight.yaw_ctrl_mode=CLOCKWISE_TURN;
		Flight.yaw_ctrl_start=1;
		Flight.yaw_outer_control_output  =10;//以10deg/s的角速度顺时针转动10000ms
		Flight.execution_time_ms=10000;//执行时间
		OpticalFlow_Control_Pure(0);	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		flight_subtask_cnt[n]=1;		
	}
	else if(flight_subtask_cnt[n]==1)
	{
		Flight.yaw_ctrl_mode=CLOCKWISE_TURN;
		Flight.yaw_outer_control_output  =0;
		OpticalFlow_Control_Pure(0);	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		
		if(Flight.yaw_ctrl_end==1)  flight_subtask_cnt[n]=2;//执行完毕后，切换到下一阶段		
	}
	else if(flight_subtask_cnt[n]==2)
	{
		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];

		OpticalFlow_Control_Pure(0);
		Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-30);//高度控制
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}





/***********************************************************************************************************/
//以10deg/s的角速度逆时针转动10000ms，完成后降落
void flight_subtask_4(void)
{
	static uint8_t n=3;
	if(flight_subtask_cnt[n]==0)
	{	
		Flight.yaw_ctrl_mode=ANTI_CLOCKWISE_TURN;
		Flight.yaw_ctrl_start=1;
		Flight.yaw_outer_control_output  =10;//以10deg/s的角速度顺时针转动10000ms
		Flight.execution_time_ms=10000;//执行时间
		OpticalFlow_Control_Pure(0);	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		flight_subtask_cnt[n]=1;		
	}
	else if(flight_subtask_cnt[n]==1)
	{
		Flight.yaw_ctrl_mode=ANTI_CLOCKWISE_TURN;
		Flight.yaw_outer_control_output  =0;
		OpticalFlow_Control_Pure(0);	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		
		if(Flight.yaw_ctrl_end==1)  flight_subtask_cnt[n]=2;//执行完毕后，切换到下一阶段		
	}
	else if(flight_subtask_cnt[n]==2)
	{
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];

		OpticalFlow_Control_Pure(0);
		Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-30);//高度控制
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}






//本demo适用于激光雷达SLAM定位条件下，普通光流（LC307、LC302）定位条件下无效
//机体坐标系下相对位移
//右前上分别对应XYZ正方向
void flight_subtask_5(void)
{		
	static uint8_t n=4;
	if(flight_subtask_cnt[n]==0)
	{
		basic_auto_flight_support();//基本飞行支持软件
		flight_subtask_cnt[n]=1;
		execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
		//向前移动100cm
		Horizontal_Navigation(0,100,0,RELATIVE_MODE,BODY_FRAME);
	}
	else if(flight_subtask_cnt[n]==1)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			flight_subtask_cnt[n]=2;
			execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
			//向右移动100cm
			Horizontal_Navigation(100,0,0,RELATIVE_MODE,BODY_FRAME);
		}
	}
	else if(flight_subtask_cnt[n]==2)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			flight_subtask_cnt[n]=3;
			execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
			//向后移动100cm
			Horizontal_Navigation(0,-100,0,RELATIVE_MODE,BODY_FRAME);
		}
	}
	else if(flight_subtask_cnt[n]==3)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			flight_subtask_cnt[n]=4;
			execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
			//向左移动100cm
			Horizontal_Navigation(-100,0,0,RELATIVE_MODE,BODY_FRAME);
		}
	}
	else if(flight_subtask_cnt[n]==4)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			flight_subtask_cnt[n]=5;
			execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
		  
			//以下还可以继续增加子任务，比如向上移动50cm
			//Horizontal_Navigation(0,0,50,RELATIVE_FRAME,MAP_FRAME);
		}
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}

//导航坐标系下相对位移
//东北天分别对应XYZ正方向
void flight_subtask_6(void)
{		
	static uint8_t n=5;
	if(flight_subtask_cnt[n]==0)
	{
		basic_auto_flight_support();//基本飞行支持软件
		flight_subtask_cnt[n]=1;
		execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
		//向前移动100cm
		Horizontal_Navigation(0,100,0,RELATIVE_MODE,MAP_FRAME);
	}
	else if(flight_subtask_cnt[n]==1)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			flight_subtask_cnt[n]=2;
			execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
			//向右移动100cm
			Horizontal_Navigation(100,0,0,RELATIVE_MODE,MAP_FRAME);
		}
	}
	else if(flight_subtask_cnt[n]==2)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			flight_subtask_cnt[n]=3;
			execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
			//向后移动100cm
			Horizontal_Navigation(0,-100,0,RELATIVE_MODE,MAP_FRAME);
		}
	}
	else if(flight_subtask_cnt[n]==3)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			flight_subtask_cnt[n]=4;
			execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
			//向左移动100cm
			Horizontal_Navigation(-100,0,0,RELATIVE_MODE,MAP_FRAME);
		}
	}
	else if(flight_subtask_cnt[n]==4)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			flight_subtask_cnt[n]=5;
			execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
		  
			//以下还可以继续增加子任务，比如向上移动50cm
			//Horizontal_Navigation(0,0,50,RELATIVE_FRAME,MAP_FRAME);
		}
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}


//导航坐标系下，基于初始点的绝对坐标位移
void flight_subtask_7(void)
{		
	static uint8_t n=6;
	Vector3f target_position;
	float x=0,y=0,z=0;
	if(flight_subtask_cnt[n]==0)
	{
		basic_auto_flight_support();//基本飞行支持软件
		//记录下初始起点位置，实际项目中可设置为某一基准原点
		base_position.x=VIO_SINS.Position[_EAST];
		base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=NamelessQuad.Position[_UP];
		
		flight_subtask_cnt[n]=1;
		execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
		//向前移动100cm
		x=0;y=100;z=0;
		target_position.x=base_position.x+x;
		target_position.y=base_position.y+y;
		target_position.z=base_position.z+z;
		Horizontal_Navigation(target_position.x,
												  target_position.y,
												  target_position.z,
												  GLOBAL_MODE,
												  MAP_FRAME);
	}
	else if(flight_subtask_cnt[n]==1)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			flight_subtask_cnt[n]=2;
			execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
			//向右移动100cm
			x=100;y=100;z=0;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=base_position.z+z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
		}
	}
	else if(flight_subtask_cnt[n]==2)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			flight_subtask_cnt[n]=3;
			execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
			//向后移动100cm
			x=100;y=0;z=0;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=base_position.z+z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
		}
	}
	else if(flight_subtask_cnt[n]==3)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			flight_subtask_cnt[n]=4;
			execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
			//向左移动100cm
			x=0;y=0;z=0;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=base_position.z+z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
		}
	}
	else if(flight_subtask_cnt[n]==4)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			flight_subtask_cnt[n]=5;
			execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
		  
			//以下还可以继续增加子任务，比如向上移动50cm
			//Horizontal_Navigation(0,0,50,RELATIVE_FRAME,MAP_FRAME);
		}
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}


//轨迹圆
#define Waypoint_Radius_CM     50 									//轨迹点半径，单位为cm   100
#define Waypoint_Num           60				 						//轨迹点数量						 60
//改变轨迹圆参数，一般情况下只需要调整半径Waypoint_Radius_CM即可
#define Waypoint_Angle_Delta   (360/Waypoint_Num)		//航点角度增量，单位为deg
#define Waypoint_Ideal_Speed	 20										//理想的巡航速度，用于推算航点发布时间，单位为cm/s
#define Waypoint_Time_Gap			 1000*(2*3.14*Waypoint_Radius_CM/Waypoint_Ideal_Speed)/Waypoint_Num		//航点发布时间间隔，单位为ms
float waypoint_x[Waypoint_Num+1],waypoint_y[Waypoint_Num+1];
void Circle_Waypoint_Generate(uint16_t num,float radius)
{
	for(uint16_t i=0;i<num+1;i++)
	{
		waypoint_x[i]=radius-radius*cos(Waypoint_Angle_Delta*i*DEG2RAD);
		waypoint_y[i]=radius*sin(Waypoint_Angle_Delta*i*DEG2RAD);
	}
}


void flight_subtask_8(void)
{		
	static uint8_t n=7;
	Vector3f target_position;
	basic_auto_flight_support();//基本飞行支持软件
	if(flight_subtask_cnt[n]==0)
	{
		Circle_Waypoint_Generate(Waypoint_Num,Waypoint_Radius_CM);
		//记录下初始起点位置，实际项目中可设置为某一基准原点
		base_position.x=VIO_SINS.Position[_EAST];
		base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=NamelessQuad.Position[_UP];
		
		execute_time_ms[n]=Waypoint_Time_Gap/flight_subtask_delta;//子任务执行时间
		target_position.x=base_position.x+waypoint_x[0];
		target_position.y=base_position.y+waypoint_y[0];
		target_position.z=base_position.z;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		flight_subtask_cnt[n]=1;
		laser_light_1.reset=1;
		laser_light_1.times=50000;//闪速1次
		laser_light_1.period=40;
		laser_light_1.light_on_percent=0.5;
	}
	else if(flight_subtask_cnt[n]<Waypoint_Num+1)
	{
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=Waypoint_Time_Gap/flight_subtask_delta;//子任务执行时间
			
			target_position.x=base_position.x+waypoint_x[flight_subtask_cnt[n]];
			target_position.y=base_position.y+waypoint_y[flight_subtask_cnt[n]];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
		}
	}
	else//一直重复圆形轨迹，只运行一次可以注释掉下方赋值行
	{
		flight_subtask_cnt[n]=1;
	}
}



/*****************************************************
@wise 插入的代码,time:2024.5.3.17
******************************************************/

const int16_t wise_waypoints_table_test[2][40]={
{0, 0, 0, 0, 0, 0, 0,-2, -2,-2,0,0,0,0,0,0,0,0,0, 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0, 2, 4, 4, 4,-4,-2,-2, -2,-2,0,0,0,0,0,0,0,0,0, 2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};//选中点前后和本身保持一致，,前前点到投递为止，前点下降（不用，只是空出subtask），后点上升，本身投递（加视觉）end为降落点

#define robocupfixed_cm 6.0f
#define robocupfixed_times  5

//num_one=5,two=10,three=15，end=20
const int16_t wise_waypoints_table_zero[2][10]={0};
const int16_t wise_waypoints_table[2][10]={0};

uint16_t wise_time_gap_test[49]={
3000,4500,4500,4500,4500,4500,4500,4500,4500,4500,
4500,4500,4500,4500,4500,4500,4500,4500,4500,4500,
4500,4500,4500,4500,4500,4500,4500,4500,4500,4500,
4500,4500,4500,4500,4500,4500,4500,4500,4500,4500,
4500,4500,4500,4500,4500,4500,4500,4500,10000
};//0为起降时间
float wise_waypoints[2][40]={0};

static int throw_id=1;//快递序号
static int throw_dropheight=40;//投递下降高度

void Throw(){
	if(throw_id==1){
		Reserved_PWM1_Output(950);
	}
	else
	if(throw_id==2){
		Reserved_PWM1_Output(1250);
	}
	else
	if(throw_id==3){
		Reserved_PWM1_Output(1500);
	}
	else
	{
	 Reserved_PWM1_Output(500);//归位
	}
	throw_id++;
}

void wise_waypoint_generate(void)
{
	for(uint16_t i=0;i<40;i++)
	{
		wise_waypoints[0][i]=25.0f*wise_waypoints_table_test[0][i];
		wise_waypoints[1][i]=25.0f*wise_waypoints_table_test[1][i];
	}
}

void wise_waypoint_generate_robocup(void)
{
	for(uint16_t i=0;i<40;i++)
	{
		wise_waypoints[0][i]=25.0f*wise_waypoints_table[0][i];
		wise_waypoints[1][i]=25.0f*wise_waypoints_table[1][i];
	}
}
/*****************************************************
@wise 插入的代码,time:24.5.10.16
******************************************************/
	
/***************************************************************
  *  @brief    函数航点遍历函数
*  @param     参数   n:		tra_num:航点组表选择 	waypoint_num:到第几航点
  *  @note      备注
  *  @Sample usage:     函数的使用方法 
 **************************************************************/


int traverse(int n,int tra_num,int waypoint_num)
{
	Vector3f target_position;	
	if(flight_subtask_cnt[n]==0)
	{
		basic_auto_flight_support();//基本飞行支持软件
		wise_waypoint_generate();
		base_position.x=VIO_SINS.Position[_EAST];
		base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=NamelessQuad.Position[_UP];
		execute_time_ms[n]=wise_time_gap_test[0]/flight_subtask_delta;//子任务执行时间
		target_position.x=base_position.x+wise_waypoints[0][0];
		target_position.y=base_position.y+wise_waypoints[1][0];
		target_position.z=base_position.z;
		Horizontal_Navigation(target_position.x,
			target_position.y,
			target_position.z,
			GLOBAL_MODE,
			MAP_FRAME);
		flight_subtask_cnt[n]++;	
	}
	else if(flight_subtask_cnt[n]<=waypoint_num)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[flight_subtask_cnt[n]]/flight_subtask_delta;//子任务执行时间
			target_position.x=base_position.x+wise_waypoints[0][flight_subtask_cnt[n]];
		  target_position.y=base_position.y+wise_waypoints[1][flight_subtask_cnt[n]];
		  target_position.z=base_position.z;
	  	Horizontal_Navigation(target_position.x,
			target_position.y,
			target_position.z,
			GLOBAL_MODE,
			MAP_FRAME);
			flight_subtask_cnt[n]++;
		}	
	}
	else if(flight_subtask_cnt[n]==(waypoint_num+1))
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) //重新计算执行子任务时间
		{
			return 1;//任务完成标志
		}
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}


void try_uart(void)
{
		static int n = 25;
	Vector3f target_position;
		if(flight_subtask_cnt[n]==0)
	{
		wise_waypoint_generate();
		base_position.x=VIO_SINS.Position[_EAST];
		base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=NamelessQuad.Position[_UP];
		execute_time_ms[n]=wise_time_gap_test[0]/flight_subtask_delta;//子任务执行时间
		flight_subtask_cnt[n]++;
		speaker_gh(VIO_SINS.Position[_EAST],VIO_SINS.Position[_NORTH],NamelessQuad.Position[_UP],0,0,0,0);
	}
		else if(flight_subtask_cnt[n]<=5)
	{
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[flight_subtask_cnt[n]]/flight_subtask_delta;//子任务执行时间
			flight_subtask_cnt[n]++;
			speaker_gh(VIO_SINS.Position[_EAST],VIO_SINS.Position[_NORTH],NamelessQuad.Position[_UP],0,0,0,0);
		}	
	}
	
}

void robocup_fly(int way_one,int way_two,int way_three,int end)
{
	static int n = 26;
	Vector3f target_position;	
	if(flight_subtask_cnt[n]==0)
	{
		basic_auto_flight_support();//基本飞行支持软件
		wise_waypoint_generate();
		base_position.x=VIO_SINS.Position[_EAST];
		base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=NamelessQuad.Position[_UP];
		execute_time_ms[n]=wise_time_gap_test[0]/flight_subtask_delta;//子任务执行时间
		target_position.x=base_position.x+wise_waypoints[0][0];
		target_position.y=base_position.y+wise_waypoints[1][0];
		target_position.z=base_position.z;
		Horizontal_Navigation(target_position.x,
			target_position.y,
			target_position.z,
			GLOBAL_MODE,
			MAP_FRAME);
		flight_subtask_cnt[n]++;	
	}
	else if(flight_subtask_cnt[n]<(way_one-1))
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[flight_subtask_cnt[n]]/flight_subtask_delta;//子任务执行时间
			target_position.x=base_position.x+wise_waypoints[0][flight_subtask_cnt[n]];
		  target_position.y=base_position.y+wise_waypoints[1][flight_subtask_cnt[n]];
		  target_position.z=base_position.z;
	  	Horizontal_Navigation(target_position.x,
			target_position.y,
			target_position.z,
			GLOBAL_MODE,
			MAP_FRAME);
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
		}	
	}
		else if(flight_subtask_cnt[n]==(way_one-1))
	{
		basic_auto_flight_support();//基本飞行支持软件
		Top_Circle_Control_Pilot();
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output =RC_Data.rc_rpyt[RC_YAW];			
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[0]/flight_subtask_delta;//子任务执行时间
			flight_subtask_cnt[n]++;
			OpticalFlow_Control_Pure(1);//强制刷新悬停位置
			flight_global_cnt[n]=0;
		}	
//		if(execute_time_ms[n]>0) execute_time_ms[n]--;
//		if(execute_time_ms[n]==0) 
//		{
//			execute_time_ms[n]=wise_time_gap_test[flight_subtask_cnt[n]]/flight_subtask_delta;//子任务执行时间
//	  	Horizontal_Navigation(0,0,(-1)*throw_dropheight,RELATIVE_MODE,BODY_FRAME);
//			flight_subtask_cnt[n]++;
//		}	
//		if(flight_global_cnt[n]<robocupfixed_times)//判断是否到达
//		{
//			float robocup_error_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
//			if(robocup_error_cm<=robocupfixed_cm) 
//			{
//				flight_global_cnt[n]++;
//			execute_time_ms[n]=0;
//			}
//			else flight_global_cnt[n]/=2;
//		}	
//		else
//		{
//			execute_time_ms[n]=wise_time_gap_test[flight_subtask_cnt[n]]/flight_subtask_delta;
//			target_position.x=base_position.x+wise_waypoints[0][flight_subtask_cnt[n]];
//		  target_position.y=base_position.y+wise_waypoints[1][flight_subtask_cnt[n]];
//		  target_position.z=base_position.z-throw_dropheight;
//	  	Horizontal_Navigation(target_position.x,
//			target_position.y,
//			target_position.z,
//			GLOBAL_MODE,
//			MAP_FRAME);
//			flight_subtask_cnt[n]++;
//			flight_global_cnt[n]=0;
//		}
	}
	else if(flight_subtask_cnt[n]==way_one)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[flight_subtask_cnt[n]]/flight_subtask_delta;//子任务执行时间
	  	Horizontal_Navigation(0,0,(-1)*throw_dropheight,RELATIVE_MODE,BODY_FRAME);
			flight_subtask_cnt[n]++;
		}	
	}
	else if(flight_subtask_cnt[n]==(way_one+1))
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[0]/flight_subtask_delta;//子任务执行时间
			target_position.x=base_position.x+wise_waypoints[0][flight_subtask_cnt[n]];
		  target_position.y=base_position.y+wise_waypoints[1][flight_subtask_cnt[n]];
		  target_position.z=base_position.z;
	  	Horizontal_Navigation(target_position.x,
			target_position.y,
			target_position.z,
			GLOBAL_MODE,
			MAP_FRAME);
			flight_subtask_cnt[n]++;
			Throw();//投递
		}	
	}
	else if(flight_subtask_cnt[n]<(way_two-1)&&flight_subtask_cnt[n]>(way_one+1))
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[0]/flight_subtask_delta;//子任务执行时间
			target_position.x=base_position.x+wise_waypoints[0][flight_subtask_cnt[n]];
		  target_position.y=base_position.y+wise_waypoints[1][flight_subtask_cnt[n]];
		  target_position.z=base_position.z;
	  	Horizontal_Navigation(target_position.x,
			target_position.y,
			target_position.z,
			GLOBAL_MODE,
			MAP_FRAME);
			flight_subtask_cnt[n]++;
		}
	}
		else if(flight_subtask_cnt[n]==way_two-1)
	{
		basic_auto_flight_support();//基本飞行支持软件
		Top_Circle_Control_Pilot();
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output =RC_Data.rc_rpyt[RC_YAW];			
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[0]/flight_subtask_delta;//子任务执行时间
			flight_subtask_cnt[n]++;
			OpticalFlow_Control_Pure(1);//强制刷新悬停位置
			flight_global_cnt[n]=0;
		}		
	}
		else if(flight_subtask_cnt[n]==way_two)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[flight_subtask_cnt[n]]/flight_subtask_delta;//子任务执行时间
	  	Horizontal_Navigation(0,0,(-1)*throw_dropheight,RELATIVE_MODE,BODY_FRAME);
			flight_subtask_cnt[n]++;
		}	
	}
		else if(flight_subtask_cnt[n]==(way_two+1))
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[0]/flight_subtask_delta;//子任务执行时间
			target_position.x=base_position.x+wise_waypoints[0][flight_subtask_cnt[n]];
		  target_position.y=base_position.y+wise_waypoints[1][flight_subtask_cnt[n]];
		  target_position.z=base_position.z;
	  	Horizontal_Navigation(target_position.x,
			target_position.y,
			target_position.z,
			GLOBAL_MODE,
			MAP_FRAME);
			flight_subtask_cnt[n]++;
			Throw();
		}	
	}
	else if(flight_subtask_cnt[n]<(way_three-1)&&flight_subtask_cnt[n]>(way_two+1))
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[0]/flight_subtask_delta;//子任务执行时间
			target_position.x=base_position.x+wise_waypoints[0][flight_subtask_cnt[n]];
		  target_position.y=base_position.y+wise_waypoints[1][flight_subtask_cnt[n]];
		  target_position.z=base_position.z;
	  	Horizontal_Navigation(target_position.x,
			target_position.y,
			target_position.z,
			GLOBAL_MODE,
			MAP_FRAME);
			flight_subtask_cnt[n]++;
		}
	}
	else if(flight_subtask_cnt[n]==way_three-1)
	{
		basic_auto_flight_support();//基本飞行支持软件
		Top_Circle_Control_Pilot();
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output =RC_Data.rc_rpyt[RC_YAW];			
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[0]/flight_subtask_delta;//子任务执行时间
			flight_subtask_cnt[n]++;
			OpticalFlow_Control_Pure(1);//强制刷新悬停位置
			flight_global_cnt[n]=0;
		}		
	}
			else if(flight_subtask_cnt[n]==way_three)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[flight_subtask_cnt[n]]/flight_subtask_delta;//子任务执行时间
	  	Horizontal_Navigation(0,0,(-1)*throw_dropheight,RELATIVE_MODE,BODY_FRAME);
			flight_subtask_cnt[n]++;
		}	
	}
		else if(flight_subtask_cnt[n]==(way_three+1))
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[0]/flight_subtask_delta;//子任务执行时间
			target_position.x=base_position.x+wise_waypoints[0][flight_subtask_cnt[n]];
		  target_position.y=base_position.y+wise_waypoints[1][flight_subtask_cnt[n]];
		  target_position.z=base_position.z;
	  	Horizontal_Navigation(target_position.x,
			target_position.y,
			target_position.z,
			GLOBAL_MODE,
			MAP_FRAME);
			flight_subtask_cnt[n]++;
			Throw();
		}	
	}
	else if(flight_subtask_cnt[n]<end)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[flight_subtask_cnt[n]]/flight_subtask_delta;//子任务执行时间
			target_position.x=base_position.x+wise_waypoints[0][flight_subtask_cnt[n]];
		  target_position.y=base_position.y+wise_waypoints[1][flight_subtask_cnt[n]];
		  target_position.z=base_position.z;
	  	Horizontal_Navigation(target_position.x,
			target_position.y,
			target_position.z,
			GLOBAL_MODE,
			MAP_FRAME);
			flight_subtask_cnt[n]++;
		}	
	}
	else if(flight_subtask_cnt[n]==end)
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=wise_time_gap_test[0]/flight_subtask_delta;//子任务执行时间
			flight_subtask_cnt[n]++;
		}	
	}
	else if(flight_subtask_cnt[n]==(end+1))//降落
	{
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		OpticalFlow_Control_Pure(0);
		Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-30);//高度控制
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}

/*****************************************************************************************/
const int16_t work_waypoints_table[2][32]={
{0,1,1,2,2,3,3,4,5,6,7,7,6,5,4,4,5,6,7,7,6,5,4,4,5,6,7,7,6,5,4,0},
{0,4,5,5,4,4,5,5,5,5,5,4,4,4,4,3,3,3,3,2,2,2,2,1,1,1,1,0,0,0,0,0}
};
float work_waypoints[2][32]={0};
uint16_t work_time_gap[32]={
3000,
10000,
3000,3000,3000,3000,3000,3000,3000,3000,3000,
3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
10000
};
#define Scale_Param  50.0f
void work_waypoint_generate(void)
{
	for(uint16_t i=0;i<32;i++)
	{
		work_waypoints[0][i]=Scale_Param*work_waypoints_table[0][i];
		work_waypoints[1][i]=Scale_Param*work_waypoints_table[1][i];
	}

}


void Agriculture_UAV_Basic(void)
{		
	static uint8_t n=9;
	Vector3f target_position;	
	if(flight_subtask_cnt[n]==0)//起飞点作为第一个悬停点
	{
		basic_auto_flight_support();//基本飞行支持软件
		work_waypoint_generate();
		//记录下初始起点位置，实际项目中可设置为某一基准原点
		base_position.x=VIO_SINS.Position[_EAST];
		base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=NamelessQuad.Position[_UP];
		
		execute_time_ms[n]=work_time_gap[0]/flight_subtask_delta;//子任务执行时间
		target_position.x=base_position.x+work_waypoints[0][0];
		target_position.y=base_position.y+work_waypoints[1][0];
		target_position.z=base_position.z;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		flight_subtask_cnt[n]=1;		
	}
	else if(flight_subtask_cnt[n]==1)//起飞点悬停完毕后，飞向目标A——第21号作业区域
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=work_time_gap[1]/flight_subtask_delta;//子任务执行时间
			
			target_position.x=base_position.x+work_waypoints[0][1];
			target_position.y=base_position.y+work_waypoints[1][1];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
		}	
	}	
	else if(flight_subtask_cnt[n]<31)//到达A点所在作业点后，遍历所有航点并检测是否打点
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			if(Opv_Top_View_Target.trust_flag==1)//到达目标航电后，判断底部是否为农作物
			{
				laser_light_1.reset=1;
				laser_light_1.times=1;//闪速两次
			}
				
			execute_time_ms[n]=work_time_gap[flight_subtask_cnt[n]]/flight_subtask_delta;//子任务执行时间		
			target_position.x=base_position.x+work_waypoints[0][flight_subtask_cnt[n]];
			target_position.y=base_position.y+work_waypoints[1][flight_subtask_cnt[n]];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
		}
	}
	else if(flight_subtask_cnt[n]==31)//到达最后一个作业点，检测是否打点，最后返回起飞点
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			if(Opv_Top_View_Target.trust_flag==1)//到达目标航点后，判断底部是否为农作物
			{
				laser_light_1.reset=1;
				laser_light_1.times=1;//闪速1次
			}	
			execute_time_ms[n]=work_time_gap[31]/flight_subtask_delta;//子任务执行时间
			target_position.x=base_position.x+work_waypoints[0][31];
			target_position.y=base_position.y+work_waypoints[1][31];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
		}	
	}
	else if(flight_subtask_cnt[n]==32)//到达起飞点
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=2000/flight_subtask_delta;//子任务执行时间
			
			flight_subtask_cnt[n]++;
		}			
	}
	else if(flight_subtask_cnt[n]==33)//原地降落
	{
		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		OpticalFlow_Control_Pure(0);
		Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-30);//高度控制
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}



#define Waypoint_Fix_Cnt1   5 //5
#define Waypoint_Fix_CM1    5//5
//位置kp=400
void Agriculture_UAV_Closeloop(void)
{		
	static uint8_t n=10;
	Vector3f target_position;	
	if(flight_subtask_cnt[n]==0)//起飞点作为第一个悬停点
	{
		basic_auto_flight_support();//基本飞行支持软件
		work_waypoint_generate();
		//记录下初始起点位置，实际项目中可设置为某一基准原点
		//base_position.x=VIO_SINS.Position[_EAST];
		//base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=NamelessQuad.Position[_UP];
		
		execute_time_ms[n]=work_time_gap[0]/flight_subtask_delta;//子任务执行时间
		target_position.x=base_position.x+work_waypoints[0][0];
		target_position.y=base_position.y+work_waypoints[1][0];
		target_position.z=base_position.z;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		flight_subtask_cnt[n]=1;
		flight_global_cnt[n]=0;
	}
	else if(flight_subtask_cnt[n]==1)//检测起飞点悬停完毕后，飞向目标A——第21号作业区域
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][1];
			target_position.y=base_position.y+work_waypoints[1][1];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;		
			
			//flight_subtask_cnt[n]=30;//留作用户测试用阶段调试用，节省时间			
		}
	}	
	else if(flight_subtask_cnt[n]<31)//到达A点所在作业点后，遍历所有航点并检测是否打点
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			execute_time_ms[n]=work_time_gap[flight_subtask_cnt[n]]/flight_subtask_delta;//子任务执行时间		
			target_position.x=base_position.x+work_waypoints[0][flight_subtask_cnt[n]];
			target_position.y=base_position.y+work_waypoints[1][flight_subtask_cnt[n]];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;				
			
			if(Opv_Top_View_Target.trust_flag==1)//到达目标航点后，判断底部是否为农作物
			{
				laser_light_1.reset=1;
				laser_light_1.times=1;//闪烁1次
			}		
		}
	}
	else if(flight_subtask_cnt[n]==31)//到达最后一个航点单独处理
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			execute_time_ms[n]=1000/flight_subtask_delta;//子任务执行时间
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;		
			if(Opv_Top_View_Target.trust_flag==1)//到达目标航点后，判断底部是否为农作物
			{
				laser_light_1.reset=1;
				laser_light_1.times=1;//闪烁1次
			}			
		}			
	}
	else if(flight_subtask_cnt[n]==32)//最后返回起飞点
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			execute_time_ms[n]=work_time_gap[31]/flight_subtask_delta;//子任务执行时间
			target_position.x=base_position.x+work_waypoints[0][31];
			target_position.y=base_position.y+work_waypoints[1][31];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
		}	
	}
	else if(flight_subtask_cnt[n]==33)//到达起飞点
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			
			//以下复位操作的作用：空中到地面不同高度，环境分布陈设变化造成的误差
			//不同高度梯度上陈设变化不大时，以下特殊处理部分可以不去掉
			//特殊处理开始
//			send_check_back=4;//重置slam
//			VIO_SINS.Position[_EAST] = 0;
//			VIO_SINS.Position[_NORTH]= 0;
//		  OpticalFlow_Pos_Ctrl_Expect.x=0;
//		  OpticalFlow_Pos_Ctrl_Expect.y=0;
			//特殊处理结束
		}		
	}
	else if(flight_subtask_cnt[n]==34)//原地降落
	{	
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		OpticalFlow_Control_Pure(0);
		Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-30);//高度控制
		//当巡航高度所在环境与地面环境差异偏大时，建议水平方向只使用速度控制降落，避免位置观测误差造成的降落点偏移
		//OpticalFlow_X_Vel_Control(0);
		//OpticalFlow_Y_Vel_Control(0);
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}



	
	

const int16_t work_waypoints_table_fast[2][32]={
{0,1,7,7,1,4,4,5,5,6,6,7,7},
{0,4,4,5,5,3,0,0,3,3,0,0,3}
};

void work_waypoint_generate_fast(void)
{
	for(uint16_t i=0;i<13;i++)
	{
		work_waypoints[0][i]=Scale_Param*work_waypoints_table_fast[0][i];
		work_waypoints[1][i]=Scale_Param*work_waypoints_table_fast[1][i];
	}

}



#define Waypoint_Fix_Cnt_1   5 //5
#define Waypoint_Fix_CM_1    5//5
uint16_t work_end_flag[30]={0};
uint16_t item=0;
void Agriculture_UAV_Closeloop1(void)
{		
	static uint8_t n=10;
	Vector3f target_position;	
	if(flight_subtask_cnt[n]==0)//起飞点作为第一个悬停点
	{
		basic_auto_flight_support();//基本飞行支持软件
		work_waypoint_generate_fast();
		//记录下初始起点位置，实际项目中可设置为某一基准原点
		//base_position.x=VIO_SINS.Position[_EAST];
		//base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=NamelessQuad.Position[_UP];
		
		execute_time_ms[n]=work_time_gap[0]/flight_subtask_delta;//子任务执行时间
		target_position.x=base_position.x+work_waypoints[0][0];
		target_position.y=base_position.y+work_waypoints[1][0];
		target_position.z=base_position.z;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		flight_subtask_cnt[n]=1;
		flight_global_cnt[n]=0;
	}
	else if(flight_subtask_cnt[n]==1)//检测起飞点悬停完毕后，飞向目标A——第21号作业区域
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt_1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM_1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][1];
			target_position.y=base_position.y+work_waypoints[1][1];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=2;
			flight_global_cnt[n]=0;				
		}
	}	
	else if(flight_subtask_cnt[n]==2)
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt_1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM_1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][2];
			target_position.y=base_position.y+work_waypoints[1][2];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=3;
			flight_global_cnt[n]=0;				
		}	
	}
	else if(flight_subtask_cnt[n]==3)
	{
		basic_auto_flight_support();//基本飞行支持软件
		float dis_cm[7];
		for(uint16_t i=0;i<7;i++)
		{
			dis_cm[i]=ABS(Scale_Param*i-VIO_SINS.Position[_EAST]);
			if(dis_cm[i]<10)
			{
				if(work_end_flag[item+i]==0)
				{
					if(Opv_Top_View_Target.trust_flag==1)//到达目标航点后，判断底部是否为农作物
					{
						laser_light_1.reset=1;
						laser_light_1.times=1;//闪烁1次
					}
					work_end_flag[item+i]=1;
				}
			}
		}
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt_1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM_1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][3];
			target_position.y=base_position.y+work_waypoints[1][3];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=4;
			flight_global_cnt[n]=0;				
		}
	}
	else if(flight_subtask_cnt[n]==4)
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][4];
			target_position.y=base_position.y+work_waypoints[1][4];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=5;
			flight_global_cnt[n]=0;				
		}
		item=7;		
	}
	else if(flight_subtask_cnt[n]==5)
	{
		basic_auto_flight_support();//基本飞行支持软件
		float dis_cm[7];
		for(uint16_t i=0;i<7;i++)
		{
			dis_cm[i]=ABS(Scale_Param*i-VIO_SINS.Position[_EAST]);
			if(dis_cm[i]<10)
			{
				if(work_end_flag[item+i]==0)
				{
					if(Opv_Top_View_Target.trust_flag==1)//到达目标航点后，判断底部是否为农作物
					{
						laser_light_1.reset=1;
						laser_light_1.times=1;//闪烁1次
					}
					work_end_flag[item+i]=1;
				}
			}
		}
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt_1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM_1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][5];
			target_position.y=base_position.y+work_waypoints[1][5];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=6;
			flight_global_cnt[n]=0;				
		}
	}	
	else if(flight_subtask_cnt[n]==6)
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt_1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][6];
			target_position.y=base_position.y+work_waypoints[1][6];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=7;
			flight_global_cnt[n]=0;				
		}
		item=14;		
	}	
	else if(flight_subtask_cnt[n]==7)
	{
		basic_auto_flight_support();//基本飞行支持软件
		float dis_cm[4];
		for(uint16_t i=0;i<4;i++)
		{
			dis_cm[i]=ABS(Scale_Param*i-VIO_SINS.Position[_NORTH]);
			if(dis_cm[i]<10)
			{
				if(work_end_flag[item+i]==0)
				{
					if(Opv_Top_View_Target.trust_flag==1)//到达目标航点后，判断底部是否为农作物
					{
						laser_light_1.reset=1;
						laser_light_1.times=1;//闪烁1次
					}
					work_end_flag[item+i]=1;
				}
			}
		}
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt_1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM_1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][7];
			target_position.y=base_position.y+work_waypoints[1][7];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=8;
			flight_global_cnt[n]=0;				
		}
	}		
	else if(flight_subtask_cnt[n]==8)
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM_1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][8];
			target_position.y=base_position.y+work_waypoints[1][8];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=9;
			flight_global_cnt[n]=0;				
		}
		item=18;		
	}		
	else if(flight_subtask_cnt[n]==9)
	{
		basic_auto_flight_support();//基本飞行支持软件
		float dis_cm[4];
		for(uint16_t i=0;i<4;i++)
		{
			dis_cm[i]=ABS(Scale_Param*i-VIO_SINS.Position[_NORTH]);
			if(dis_cm[i]<10)
			{
				if(work_end_flag[item+i]==0)
				{
					if(Opv_Top_View_Target.trust_flag==1)//到达目标航点后，判断底部是否为农作物
					{
						laser_light_1.reset=1;
						laser_light_1.times=1;//闪烁1次
					}
					work_end_flag[item+i]=1;
				}
			}
		}
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM_1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][9];
			target_position.y=base_position.y+work_waypoints[1][9];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=10;
			flight_global_cnt[n]=0;				
		}
	}
	else if(flight_subtask_cnt[n]==10)
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt_1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM_1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][10];
			target_position.y=base_position.y+work_waypoints[1][10];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=11;
			flight_global_cnt[n]=0;				
		}
		item=22;		
	}	
	else if(flight_subtask_cnt[n]==11)
	{
		basic_auto_flight_support();//基本飞行支持软件
		float dis_cm[4];
		for(uint16_t i=0;i<4;i++)
		{
			dis_cm[i]=ABS(Scale_Param*i-VIO_SINS.Position[_NORTH]);
			if(dis_cm[i]<10)
			{
				if(work_end_flag[item+i]==0)
				{
					if(Opv_Top_View_Target.trust_flag==1)//到达目标航点后，判断底部是否为农作物
					{
						laser_light_1.reset=1;
						laser_light_1.times=1;//闪烁1次
					}
					work_end_flag[item+i]=1;
				}
			}
		}
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt_1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM_1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][11];
			target_position.y=base_position.y+work_waypoints[1][11];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=12;
			flight_global_cnt[n]=0;				
		}
	}	
	else if(flight_subtask_cnt[n]==12)
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt_1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM_1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][12];
			target_position.y=base_position.y+work_waypoints[1][12];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=13;
			flight_global_cnt[n]=0;				
		}
		item=26;		
	}		
	else if(flight_subtask_cnt[n]==13)
	{
		basic_auto_flight_support();//基本飞行支持软件
		float dis_cm[4];
		for(uint16_t i=0;i<4;i++)
		{
			dis_cm[i]=ABS(Scale_Param*i-VIO_SINS.Position[_NORTH]);
			if(dis_cm[i]<10)
			{
				if(work_end_flag[item+i]==0)
				{
					if(Opv_Top_View_Target.trust_flag==1)//到达目标航点后，判断底部是否为农作物
					{
						laser_light_1.reset=1;
						laser_light_1.times=1;//闪烁1次
					}
					work_end_flag[item+i]=1;
				}
			}
		}
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt_1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM_1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][0];
			target_position.y=base_position.y+work_waypoints[1][0];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]=14;
			flight_global_cnt[n]=0;				
		}
	}		
	else if(flight_subtask_cnt[n]==14)//到达起飞点
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt_1)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM_1)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else
		{
			flight_subtask_cnt[n]=15;
			flight_global_cnt[n]=0;
		}		
	}
	else if(flight_subtask_cnt[n]==15)//原地降落
	{
		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		OpticalFlow_Control_Pure(0);
		Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-30);//高度控制
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}



#define Barcode_Height_CM   135//条形码所在高度
#define Waypoint_Fix_Cnt2   5
#define Waypoint_Fix_CM2    5
float min_dis_cm=0,min_dis_angle=0;
float min_dis_cm_backups[Laser_Min_Info_Num],min_dis_angle_backups[Laser_Min_Info_Num];
float target_yaw_err=0;
uint8_t header_fix_flag=1;//机头对准塔杆标志位
uint16_t barcode_id=0;//条码表示的数字信息
uint8_t barcode_flag=0;//条码识别成功标志位
void Agriculture_UAV_Innovation(void)//发挥部分
{		
	if(ABS(min_dis_cm_backups[0]-min_dis_cm_backups[1])>100.0f)//发现距离存在突变，保持上次检测信息
	{
		min_dis_cm_backups[0]		=min_dis_cm_backups[1];//丢弃当前的距离和角度信息
		min_dis_angle_backups[0]=min_dis_angle_backups[1];//丢弃当前的距离和角度信息
	}
	min_dis_cm	 =min_dis_cm_backups[0];
	min_dis_angle=min_dis_angle_backups[0];	
	
	if(min_dis_angle>180) min_dis_angle=min_dis_angle-360;
	target_yaw_err=min_dis_angle;
	
	static uint8_t n=9;
	Vector3f target_position;	
	if(flight_subtask_cnt[n]==0)//起飞点作为第一个悬停点
	{
		basic_auto_flight_support();//基本飞行支持软件
		work_waypoint_generate();
		//记录下初始起点位置，实际项目中可设置为某一基准原点
		base_position.x=VIO_SINS.Position[_EAST];
		base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=NamelessQuad.Position[_UP];
		
		execute_time_ms[n]=work_time_gap[0]/flight_subtask_delta;//子任务执行时间
		target_position.x=base_position.x+work_waypoints[0][0];
		target_position.y=base_position.y+work_waypoints[1][0];
		target_position.z=base_position.z;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		flight_subtask_cnt[n]=1;
		flight_global_cnt[n]=0;
		
		//flight_subtask_cnt[n]=30;//留作用户测试用阶段调试用，节省时间
	}
	else if(flight_subtask_cnt[n]==1)//检测起飞点悬停完毕后，飞向目标A——第21号作业区域
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt2)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM2)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置
		{
			target_position.x=base_position.x+work_waypoints[0][1];
			target_position.y=base_position.y+work_waypoints[1][1];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			//flight_subtask_cnt[n]=30;//单独调试寻杆、靠近、识别条码时可以在此赋值，调过打点步骤
		}
		
	}	
	else if(flight_subtask_cnt[n]<31)//到达A点所在作业点后，遍历所有航点并检测是否打点
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt2)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM2)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置
		{
			execute_time_ms[n]=work_time_gap[flight_subtask_cnt[n]]/flight_subtask_delta;//子任务执行时间		
			target_position.x=base_position.x+work_waypoints[0][flight_subtask_cnt[n]];
			target_position.y=base_position.y+work_waypoints[1][flight_subtask_cnt[n]];
			target_position.z=base_position.z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;				
			
			if(Opv_Top_View_Target.trust_flag==1)//到达目标航点后，判断底部是否为农作物
			{
				laser_light_1.reset=1;
				laser_light_1.times=1;//闪烁1次
			}		
		}
	}
	else if(flight_subtask_cnt[n]==31)//到达最后一个航点单独处理
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt2)//持续4*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM2)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续4*5ms满足，表示到达目标航点位置
		{
			execute_time_ms[n]=1000/flight_subtask_delta;//子任务执行时间
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;		
			if(Opv_Top_View_Target.trust_flag==1)//到达目标航点后，判断底部是否为农作物
			{
				laser_light_1.reset=1;
				laser_light_1.times=1;//闪烁1次
			}			
		}	
	}
	else if(flight_subtask_cnt[n]==32)//首先降低飞行高度到130cm
	{
		basic_auto_flight_support();//基本飞行支持软件
		target_position.x=base_position.x+work_waypoints[0][30];
		target_position.y=base_position.y+work_waypoints[1][30];
		target_position.z=Barcode_Height_CM;//条形码所在高度
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		flight_subtask_cnt[n]++;
		flight_global_cnt[n]=0;		
	}
	else if(flight_subtask_cnt[n]==33)//判断是否到达目标高度
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标高度
		if(flight_global_cnt[n]<100)//持续100*5ms满足
		{
			if(ABS(Total_Controller.Height_Position_Control.Err)<=10.0f)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续100*5ms满足，表示到达目标高度
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
		}
	}
	else if(flight_subtask_cnt[n]==34)//飞到地图中间14号位置，避免现场人员站立太近，激光雷达扫描误以为是距离最近障碍
	{
		basic_auto_flight_support();//基本飞行支持软件
		target_position.x=base_position.x+4*Scale_Param;
		target_position.y=base_position.y+3*Scale_Param;
		target_position.z=Barcode_Height_CM;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt2)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<Waypoint_Fix_CM2)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;		
		}
	}
	else if(flight_subtask_cnt[n]==35)//首先控制偏航运动，使得飞机头部对准塔杆
	{
		float expect_yaw_gyro=Total_Controller.Yaw_Angle_Control.Kp*target_yaw_err;//期望偏航角速度	
		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output=constrain_float(expect_yaw_gyro,-10,10);//以10deg/s的角速度顺时针转动10000ms
		if(ABS(target_yaw_err)<=5.0f)//只有当偏航角度比较小时，才靠近杆
		{
			float dis_err=min_dis_cm-50;//设定杆到无人机中心的距离，非摄像头到杆距离，可根据实际轴距、视觉处理模块安装位置以及识别情况适当调整
			dis_err=constrain_float(dis_err,-20,20);
			OpticalFlow_Y_Vel_Control(Total_Controller.Optical_Position_Control.Kp*dis_err);
			OpticalFlow_X_Vel_Control(0);
			header_fix_flag=1;
		}
		else//否则原地悬停
		{
			OpticalFlow_Control_Pure(header_fix_flag);
			header_fix_flag=0;
		}			
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
	
		if(barcode_flag==1)//只要识别到了二维码，提前结束对准塔杆、靠近任务
		{
			flight_subtask_cnt[n]=36;
			flight_global_cnt[n]=0;
			
			//识别到条形码后，重新回退到14区块，避免直接返航过程中撞到杆
			target_position.x=base_position.x+4*Scale_Param;
			target_position.y=base_position.y+3*Scale_Param;
			target_position.z=Barcode_Height_CM;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
		}
	}
	else if(flight_subtask_cnt[n]==36)
	{
		basic_auto_flight_support();//基本飞行支持软件
		laser_light_1.reset=1;
		laser_light_1.times=barcode_id;//闪烁barcode_id次
		execute_time_ms[n]=(barcode_id*1000+3000)/flight_subtask_delta;
		flight_subtask_cnt[n]++;
		flight_global_cnt[n]=0;
		
		//正常情况不会发生，纯属保险起见
		if(execute_time_ms[n]>=20000/flight_subtask_delta)   execute_time_ms[n]=20000/flight_subtask_delta;//最大不超过20S,避免id信息识别错误，造成长时间耽置
	}
	else if(flight_subtask_cnt[n]==37)//间隔3s后再次闪烁
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0) 
		{
			laser_light_1.reset=1;
			laser_light_1.times=barcode_id;//闪烁barcode_id次
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
		}
	}
	else if(flight_subtask_cnt[n]==38)//退回到4号的下方格子
	{
		basic_auto_flight_support();//基本飞行支持软件
		target_position.x=base_position.x+4*Scale_Param;
		target_position.y=base_position.y-1*Scale_Param;
		target_position.z=Barcode_Height_CM;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt2)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM2)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;				
		}
	}
	else if(flight_subtask_cnt[n]==39)//退回到起飞点的下方格子
	{
		basic_auto_flight_support();//基本飞行支持软件
		target_position.x=base_position.x;
		target_position.y=base_position.y-1*Scale_Param;
		target_position.z=Barcode_Height_CM;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt2)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM2)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;				
		}	
	}
	else if(flight_subtask_cnt[n]==40)//开始对准机头，赋值只执行一次
	{
		Flight.yaw_ctrl_mode=AZIMUTH;				//直接控制航向角模式
		Flight.yaw_ctrl_start=1;						//开始控制使能
		Flight.yaw_outer_control_output  =0;//目标角度，等效正北和初始机头朝向一致
		OpticalFlow_Control_Pure(0);//SLAM定点控制	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		flight_subtask_cnt[n]=41;		
	}
	else if(flight_subtask_cnt[n]==41)//执行对准机头过程，并判断是否对准完毕
	{
		Flight.yaw_ctrl_mode=AZIMUTH;
		Flight.yaw_outer_control_output  =0;
		OpticalFlow_Control_Pure(0);	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		
		if(Flight.yaw_ctrl_end==1)  flight_subtask_cnt[n]=42;//执行完毕后，切换到下一阶段	
	}	
	else if(flight_subtask_cnt[n]==42)//退回到起飞点正下方barcode_id*10处悬停
	{
		basic_auto_flight_support();//基本飞行支持软件
		target_position.x=base_position.x;
		target_position.y=base_position.y-10*barcode_id;
		target_position.z=Barcode_Height_CM;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Waypoint_Fix_Cnt2)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Waypoint_Fix_CM2)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;

			//以下复位操作的作用：空中到地面不同高度，环境分布陈设变化造成的误差
			//不同高度梯度上陈设变化不大时，以下特殊处理部分可以不去掉
			//特殊处理开始
//			send_check_back=4;//重置slam
//			VIO_SINS.Position[_EAST] = 0;
//			VIO_SINS.Position[_NORTH]= 0;
//		  OpticalFlow_Pos_Ctrl_Expect.x=0;
//		  OpticalFlow_Pos_Ctrl_Expect.y=0;
			//特殊处理结束
		}	
	}	
	else if(flight_subtask_cnt[n]==43)//原地降落
	{
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		OpticalFlow_Control_Pure(0);
		Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-30);//高度控制
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}





//2022年TI电赛——7月份省赛赛题送货无人机
#define Deliver_Scale_Param   25
#define First_Working_Height  150
#define Second_Working_Height	80
#define Fixed_CM     5.0f
#define Times_Fixed  5

#define deliver_down_time 6000//单位ms————塔吊下降时间
#define deliver_up_time   6100//单位ms————塔吊上升时间

#define deliver_down_pwm_us 2000
#define deliver_stop_pwm_us 1500
#define deliver_up_pwm_us 	1000


const int16_t deliver_work_waypoints_table[4][12]={
{2 , 8, 11, 14, 14, 11, 5, 5, 2,  8, 8 , 14},//横坐标X
{11, 5, 8 , -1, 11, 2 , 2, 8, 5, -1, 11,  5} //纵坐标Y
};

const uint16_t deliver_work_feature_table[2][12]={
{1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2},//颜色：1红色、2蓝色
{3, 3, 3, 3, 1, 1, 1, 1, 2, 2, 2, 2} //形状：1圆形、2矩形、3三角形
};

uint16_t template_feature[2]={0,0};//特征：颜色+形状
float xtarget[2],ytarget[2];

//特征总共有6组：
//红色三角形、蓝色三角形、红色圆形
//蓝色圆形  、红色矩形  、蓝色矩形
void deliver_work_waypoint_generate(void)
{
	for(uint16_t i=0;i<12;i++)
	{
		work_waypoints[0][i]=Deliver_Scale_Param*deliver_work_waypoints_table[0][i];
		work_waypoints[1][i]=Deliver_Scale_Param*deliver_work_waypoints_table[1][i];
	}

}


//第一部分——手动输入两个目标地点作业
void Deliver_UAV_Basic(void)
{
	static uint8_t n=12;
	Vector3f target_position;
	float x=0,y=0,z=0;	
	if(flight_subtask_cnt[n]==0)//起飞点作为第一个悬停点
	{
		basic_auto_flight_support();//基本飞行支持软件
		deliver_work_waypoint_generate();//备用
			
		//记录下初始起点位置，实际项目中可设置为某一基准原点
		//base_position.x=VIO_SINS.Position[_EAST];
		//base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=First_Working_Height;//第一作业高度
		
		x=base_position.x;
		y=base_position.y;
		z=First_Working_Height;
		target_position.x=x;
		target_position.y=y;
		target_position.z=z;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		flight_subtask_cnt[n]=1;
		flight_global_cnt[n]=0;
		execute_time_ms[n]=5000/flight_subtask_delta;//子任务执行时间
		
		laser_light_1.reset=1;
		laser_light_1.times=50000;//闪速50000次
		laser_light_1.period=200;
		laser_light_1.light_on_percent=0.98;
	}
	else if(flight_subtask_cnt[n]==1)//起飞之后原定悬停5后再执行航点任务
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			x=param_value[0]*param_value[1];
			y=param_value[0]*param_value[2];
			z=First_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=0;
		}		
	}
	else if(flight_subtask_cnt[n]==2)//检测起飞点悬停完毕后，飞向第一个目标点
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Times_Fixed)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Fixed_CM)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置，后降低目标高度
		{
			x=param_value[0]*param_value[1];
			y=param_value[0]*param_value[2];
			z=Second_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
		}
	}
	else if(flight_subtask_cnt[n]==3)
	{
		basic_auto_flight_support();//基本飞行支持软件		
		//判断是否到达第二作业高度
		if(flight_global_cnt[n]<400)//持续400*5ms满足
		{
			if(ABS(Total_Controller.Height_Position_Control.Err)<=10.0f)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}		
		else
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=deliver_down_time/flight_subtask_delta;
		}
	}
	else if(flight_subtask_cnt[n]==4)//执行塔吊下方任务
	{
		//basic_auto_flight_support();//基本飞行支持软件
		Color_Block_Control_Pilot();//俯视OPENMV视觉水平追踪		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output =RC_Data.rc_rpyt[RC_YAW];			
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		
		if(execute_time_ms[n]>0) 
		{
			execute_time_ms[n]--;
			Reserved_PWM1_Output(deliver_down_pwm_us);//放下塔吊
		}
		
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			Reserved_PWM1_Output(deliver_stop_pwm_us);//停止转动
			//不需要刷新位置期望，和上个线程保持一致
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=5000/flight_subtask_delta;
			
			//蜂鸣器语音提示
			laser_light_2.period=200;//200*5ms
			laser_light_2.light_on_percent=0.5f;
			laser_light_2.reset=1;
			laser_light_2.times=4;//闪烁4次	
		}	
	}
	else if(flight_subtask_cnt[n]==5)//原地悬停5s，后恢复到第一巡航高度
	{
		//basic_auto_flight_support();//基本飞行支持软件
		Color_Block_Control_Pilot();//俯视OPENMV视觉水平追踪		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output =RC_Data.rc_rpyt[RC_YAW];			
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			OpticalFlow_Control_Pure(1);//强制刷新悬停位置
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=deliver_up_time/flight_subtask_delta;;
		}		
	}
	else if(flight_subtask_cnt[n]==6)//收起塔吊
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) 
		{
			execute_time_ms[n]--;
			Reserved_PWM1_Output(deliver_up_pwm_us);//收起塔吊
		}
		
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			Reserved_PWM1_Output(deliver_stop_pwm_us);//停止转动
			
			x=param_value[0]*param_value[1];
			y=param_value[0]*param_value[2];
			z=First_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=5000/flight_subtask_delta;;
		}			
	}
	else if(flight_subtask_cnt[n]==7)//第一个航点执行完毕后，恢复到第一飞行高度
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达第一作业高度
		if(flight_global_cnt[n]<200)//持续200*5ms满足
		{
			if(ABS(Total_Controller.Height_Position_Control.Err)<=10.0f)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}		
		else
		{
			//达到第一飞行高度后，继续执行第二个航点任务
			x=param_value[0]*param_value[3];
			y=param_value[0]*param_value[4];
			z=First_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=0;
		}	
	}		
	else if(flight_subtask_cnt[n]==8)//检测起飞点悬停完毕后，飞向第二个目标点
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Times_Fixed)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Fixed_CM)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置，后降低目标高度
		{
			x=param_value[0]*param_value[3];
			y=param_value[0]*param_value[4];
			z=Second_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
		}
	}
	else if(flight_subtask_cnt[n]==9)
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达第二作业高度
		if(flight_global_cnt[n]<200)//持续200*5ms满足
		{
			if(ABS(Total_Controller.Height_Position_Control.Err)<=10.0f)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}		
		else//放出吊仓
		{	
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=deliver_down_time/flight_subtask_delta;
		}
	}
	else if(flight_subtask_cnt[n]==10)//执行塔吊下方任务
	{
		//basic_auto_flight_support();//基本飞行支持软件
		Color_Block_Control_Pilot();//俯视OPENMV视觉水平追踪		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output =RC_Data.rc_rpyt[RC_YAW];			
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		
		if(execute_time_ms[n]>0) 
		{
			execute_time_ms[n]--;
			Reserved_PWM1_Output(deliver_down_pwm_us);//放下塔吊
		}
			
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			Reserved_PWM1_Output(deliver_stop_pwm_us);//停止转动

			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=5000/flight_subtask_delta;
			
			//蜂鸣器语音提示
			laser_light_2.period=200;//200*5ms
			laser_light_2.light_on_percent=0.5f;
			laser_light_2.reset=1;
			laser_light_2.times=4;//闪烁4次		
		}	
	}
	else if(flight_subtask_cnt[n]==11)//原地悬停5s，后恢复到第一巡航高度
	{
		//basic_auto_flight_support();//基本飞行支持软件
		Color_Block_Control_Pilot();//俯视OPENMV视觉水平追踪		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output =RC_Data.rc_rpyt[RC_YAW];			
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			OpticalFlow_Control_Pure(1);//强制刷新悬停位置
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=deliver_up_time/flight_subtask_delta;
		}		
	}
	
	
	
	
	else if(flight_subtask_cnt[n]==12)//收起塔吊
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) 
		{
			execute_time_ms[n]--;
			Reserved_PWM1_Output(deliver_up_pwm_us);//收起塔吊
		}
		
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			Reserved_PWM1_Output(deliver_stop_pwm_us);//停止转动
			
			x=param_value[0]*param_value[3];
			y=param_value[0]*param_value[4];
			z=First_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=0;
		}			
	}
	else if(flight_subtask_cnt[n]==13)//第二个航点执行完毕后，恢复到第一飞行高度
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达第一作业高度
		if(flight_global_cnt[n]<200)//持续400*5ms满足
		{
			if(ABS(Total_Controller.Height_Position_Control.Err)<=10.0f)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}		
		else
		{
			//达到第一飞行高度后，继续执行返航任务航点任务
			target_position.x=base_position.x;
			target_position.y=base_position.y;
			target_position.z=First_Working_Height;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=0;
		}	
	}	
	else if(flight_subtask_cnt[n]==14)//飞向起飞点正上方
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Times_Fixed)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Fixed_CM)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=0;
			/*___________________________________________________________________
			以下复位操作的作用：空中到地面不同高度，环境分布陈设变化造成的误差
			不同高度梯度上陈设变化不大时，以下特殊处理部分可以去掉
			___________________________________________________________________*/
			//特殊处理开始
			send_check_back=4;//重置slam
			VIO_SINS.Position[_EAST] = 0;
			VIO_SINS.Position[_NORTH]= 0;
			OpticalFlow_Pos_Ctrl_Expect.x=0;
			OpticalFlow_Pos_Ctrl_Expect.y=0;
			//特殊处理结束	
		}
	}	
	else if(flight_subtask_cnt[n]==15)//原地下降
	{
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		OpticalFlow_Control_Pure(0);
		Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-30);//高度控制		
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}

/************************************************************************************************/
//第二部分——学习某一特征后，在飞行中找到两个目标地点作业
void Deliver_UAV_Innovation(void)
{
	static uint8_t n=12;
	Vector3f target_position;
	float x=0,y=0,z=0;
	if(flight_subtask_cnt[n]==0)//起飞点作为第一个悬停点
	{
		deliver_work_waypoint_generate();//备用
		template_feature[0]=param_value[7];//加载模板颜色————颜色：1红色、2蓝色
		template_feature[1]=param_value[8];//加载模板形状————形状：1圆形、2矩形、3三角形
		uint16_t k=0;
		for(uint16_t i=0;i<12;i++)
		{
			if(template_feature[0]==deliver_work_feature_table[0][i]
			 &&template_feature[1]==deliver_work_feature_table[1][i])
			{
				//将目标特征所在的位置信息加载到目标数组
				xtarget[k]=work_waypoints[0][i];
				ytarget[k]=work_waypoints[1][i];
				k++;
			}
			if(k>=2) break;//两个航点信息刷新完毕后,提前终止
		}
			
		basic_auto_flight_support();//基本飞行支持软件
		//记录下初始起点位置，实际项目中可设置为某一基准原点
		//base_position.x=VIO_SINS.Position[_EAST];
		//base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=First_Working_Height;//第一作业高度
		
		x=base_position.x;
		y=base_position.y;
		z=First_Working_Height;
		target_position.x=x;
		target_position.y=y;
		target_position.z=z;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		flight_subtask_cnt[n]=1;
		flight_global_cnt[n]=0;
		execute_time_ms[n]=2000/flight_subtask_delta;//子任务执行时间
		
		laser_light_1.reset=1;
		laser_light_1.times=50000;//闪速50000次
		laser_light_1.period=200;
		laser_light_1.light_on_percent=0.75;
	}
	else if(flight_subtask_cnt[n]==1)//起飞之后原定悬停5后再执行航点任务
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			x=xtarget[0];
			y=ytarget[0];
			z=First_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=0;
		}		
	}
	else if(flight_subtask_cnt[n]==2)//检测起飞点悬停完毕后，飞向第一个目标点
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Times_Fixed)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Fixed_CM)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置，后降低目标高度
		{
			x=xtarget[0];
			y=ytarget[0];
			z=Second_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
		}
	}
	else if(flight_subtask_cnt[n]==3)
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达第二作业高度
		if(flight_global_cnt[n]<400)//持续400*5ms满足
		{
			if(ABS(Total_Controller.Height_Position_Control.Err)<=10.0f)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}		
		else
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=deliver_down_time/flight_subtask_delta;
		}
	}
	else if(flight_subtask_cnt[n]==4)//执行塔吊下方任务
	{
		//basic_auto_flight_support();//基本飞行支持软件
		Color_Block_Control_Pilot();//俯视OPENMV视觉水平追踪		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output =RC_Data.rc_rpyt[RC_YAW];			
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		
		
		if(execute_time_ms[n]>0) 
		{
			execute_time_ms[n]--;
			Reserved_PWM1_Output(deliver_down_pwm_us);//放下塔吊
		}
		
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			Reserved_PWM1_Output(deliver_stop_pwm_us);//停止转动
			//不需要刷新位置期望，和上个线程保持一致
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=5000/flight_subtask_delta;
			
			//蜂鸣器语音提示
			laser_light_2.period=200;//200*5ms
			laser_light_2.light_on_percent=0.5f;
			laser_light_2.reset=1;
			laser_light_2.times=4;//闪烁4次	
		}	
	}
	else if(flight_subtask_cnt[n]==5)//原地悬停5s，后恢复到第一巡航高度
	{
		//basic_auto_flight_support();//基本飞行支持软件
		Color_Block_Control_Pilot();//俯视OPENMV视觉水平追踪		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output =RC_Data.rc_rpyt[RC_YAW];			
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制

		
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{		
			OpticalFlow_Control_Pure(1);//强制刷新悬停位置
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=deliver_up_time/flight_subtask_delta;;
		}		
	}
	else if(flight_subtask_cnt[n]==6)//收起塔吊
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) 
		{
			execute_time_ms[n]--;
			Reserved_PWM1_Output(deliver_up_pwm_us);//收起塔吊
		}
		
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			Reserved_PWM1_Output(deliver_stop_pwm_us);//停止转动
			
			x=xtarget[0];
			y=ytarget[0];
			z=First_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=5000/flight_subtask_delta;;
		}			
	}
	else if(flight_subtask_cnt[n]==7)//第一个航点执行完毕后，恢复到第一飞行高度
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达第一作业高度
		if(flight_global_cnt[n]<200)//持续200*5ms满足
		{
			if(ABS(Total_Controller.Height_Position_Control.Err)<=10.0f)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}		
		else
		{
			//达到第一飞行高度后，继续执行第二个航点任务
			x=xtarget[1];
			y=ytarget[1];
			z=First_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=0;
		}	
	}		
	else if(flight_subtask_cnt[n]==8)//检测起飞点悬停完毕后，飞向第二个目标点
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Times_Fixed)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Fixed_CM)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置，后降低目标高度
		{
			x=xtarget[1];
			y=ytarget[1];
			z=Second_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
		}
	}
	else if(flight_subtask_cnt[n]==9)
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达第二作业高度
		if(flight_global_cnt[n]<200)//持续200*5ms满足
		{
			if(ABS(Total_Controller.Height_Position_Control.Err)<=10.0f)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}		
		else//放出吊仓
		{	
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=deliver_down_time/flight_subtask_delta;
		}
	}
	else if(flight_subtask_cnt[n]==10)//执行塔吊下方任务
	{
		//basic_auto_flight_support();//基本飞行支持软件
		Color_Block_Control_Pilot();//俯视OPENMV视觉水平追踪		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output =RC_Data.rc_rpyt[RC_YAW];			
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制

		if(execute_time_ms[n]>0) 
		{
			execute_time_ms[n]--;
			Reserved_PWM1_Output(deliver_down_pwm_us);//放下塔吊
		}
			
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			Reserved_PWM1_Output(deliver_stop_pwm_us);//停止转动

			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=5000/flight_subtask_delta;
			
			//蜂鸣器语音提示
			laser_light_2.period=200;//200*5ms
			laser_light_2.light_on_percent=0.5f;
			laser_light_2.reset=1;
			laser_light_2.times=4;//闪烁4次		
		}	
	}
	else if(flight_subtask_cnt[n]==11)//原地悬停5s，后恢复到第一巡航高度
	{
		//basic_auto_flight_support();//基本飞行支持软件
		Color_Block_Control_Pilot();//俯视OPENMV视觉水平追踪		
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output =RC_Data.rc_rpyt[RC_YAW];			
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
	
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			OpticalFlow_Control_Pure(1);//强制刷新悬停位置
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=deliver_up_time/flight_subtask_delta;
		}		
	}
	else if(flight_subtask_cnt[n]==12)//收起塔吊
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) 
		{
			execute_time_ms[n]--;
			Reserved_PWM1_Output(deliver_up_pwm_us);//收起塔吊
		}
		
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			Reserved_PWM1_Output(deliver_stop_pwm_us);//停止转动
			
			x=xtarget[1];
			y=ytarget[1];
			z=First_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=0;
		}			
	}
	else if(flight_subtask_cnt[n]==13)//第二个航点执行完毕后，恢复到第一飞行高度
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达第一作业高度
		if(flight_global_cnt[n]<200)//持续400*5ms满足
		{
			if(ABS(Total_Controller.Height_Position_Control.Err)<=10.0f)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}		
		else
		{
			//达到第一飞行高度后，继续执行返航任务航点任务
			target_position.x=base_position.x;
			target_position.y=base_position.y;
			target_position.z=First_Working_Height;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=0;
		}	
	}	
	else if(flight_subtask_cnt[n]==14)//飞向起飞点正上方
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Times_Fixed)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Fixed_CM)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=0;
		}
	}	
	else if(flight_subtask_cnt[n]==15)//原地下降
	{
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		OpticalFlow_Control_Pure(0);
		Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-30);//高度控制		
	}
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}

/************************************************************************************************/
//第三部分——穿越圆环
void Deliver_UAV_Hulahoop(void)
{
	static uint8_t n=12;
	static float loop_x_cm=0,loop_y_cm=0,loop_angle=0;
	static float temp_x[2],temp_y[2];
	Vector3f target_position;
	float x=0,y=0,z=0;
	if(flight_subtask_cnt[n]==0)//起飞点作为第一个悬停点
	{
		deliver_work_waypoint_generate();//备用
		loop_x_cm=param_value[9];
		loop_y_cm=param_value[10];
		loop_angle=param_value[11];

		xtarget[0]=loop_x_cm+110*sinf(DEG2RAD*loop_angle);
		ytarget[0]=loop_y_cm-110*cosf(DEG2RAD*loop_angle);
		xtarget[1]=loop_x_cm-110*sinf(DEG2RAD*loop_angle);
		ytarget[1]=loop_y_cm+110*cosf(DEG2RAD*loop_angle);
		//根据圆环的与X轴夹角来决策搜寻对准点
		if(loop_angle<=90)
		{
			temp_x[0]=0;
			temp_y[0]=-50;
			temp_x[1]=350;
			temp_y[1]=-50;
		}
		else
		{
			temp_x[0]=0;
			temp_y[0]=300;
			temp_x[1]=350;
			temp_y[1]=300;		
		}
			
		basic_auto_flight_support();//基本飞行支持软件
		//记录下初始起点位置，实际项目中可设置为某一基准原点
		//base_position.x=VIO_SINS.Position[_EAST];
		//base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=First_Working_Height;//第一作业高度
		
		x=base_position.x;
		y=base_position.y;
		z=First_Working_Height;
		target_position.x=x;
		target_position.y=y;
		target_position.z=z;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		flight_subtask_cnt[n]=1;
		flight_global_cnt[n]=0;
		execute_time_ms[n]=2000/flight_subtask_delta;//子任务执行时间	
	}
	else if(flight_subtask_cnt[n]==1)//起飞之后原定悬停5后再执行航点任务
	{
		basic_auto_flight_support();//基本飞行支持软件
		if(execute_time_ms[n]>0) execute_time_ms[n]--;
		if(execute_time_ms[n]==0)//悬停时间计数器归零，悬停任务执行完毕 
		{
			x=temp_x[0];
			y=temp_y[0];
			z=First_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			execute_time_ms[n]=0;
		}		
	}
	else if(flight_subtask_cnt[n]==2)//检测起飞点悬停完毕后，飞向第一个目标点
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Times_Fixed)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Fixed_CM)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置，后降低目标高度
		{
			x=temp_x[1];
			y=temp_y[1];
			z=First_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
		}
	}
	else if(flight_subtask_cnt[n]==3)//检测起飞点悬停完毕后，飞向第二个目标点
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Times_Fixed)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Fixed_CM)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置，后降低目标高度
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
		}
	}
	else if(flight_subtask_cnt[n]==4)//开始对准机头，赋值只执行一次
	{
		Flight.yaw_ctrl_mode=AZIMUTH;				//直接控制航向角模式
		Flight.yaw_ctrl_start=1;						//开始控制使能
		Flight.yaw_outer_control_output  =loop_angle;//目标角度，等效正北和初始机头朝向一致
		
		OpticalFlow_Control_Pure(0);//SLAM定点控制	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		flight_subtask_cnt[n]++;		
	}
	else if(flight_subtask_cnt[n]==5)//执行对准机头过程，并判断是否对准完毕
	{
		Flight.yaw_ctrl_mode=AZIMUTH;
		Flight.yaw_outer_control_output  =loop_angle;
		OpticalFlow_Control_Pure(0);	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		
		if(Flight.yaw_ctrl_end==1)  flight_subtask_cnt[n]++;//执行完毕后，切换到下一阶段	
	}
	else if(flight_subtask_cnt[n]==6)
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//设置圆环穿越出发点B
		x=xtarget[0];
		y=ytarget[0];
		z=First_Working_Height;
		target_position.x=base_position.x+x;
		target_position.y=base_position.y+y;
		target_position.z=z;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		flight_subtask_cnt[n]++;
		flight_global_cnt[n]=0;
	}	
	else if(flight_subtask_cnt[n]==7)//检测起飞点悬停完毕后，飞向第二个目标点
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Times_Fixed)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Fixed_CM)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置，后降低目标高度
		{
			//设置圆环穿越结束点C
			x=xtarget[1];
			y=ytarget[1];
			z=First_Working_Height;
			target_position.x=base_position.x+x;
			target_position.y=base_position.y+y;
			target_position.z=z;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
		}
	}
	else if(flight_subtask_cnt[n]==8)//检测起飞点悬停完毕后，飞向第二个目标点
	{
		basic_auto_flight_support();//基本飞行支持软件
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Times_Fixed)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Fixed_CM)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置，后降低目标高度
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
		}
	} 
	else if(flight_subtask_cnt[n]==9)//开始对准机头，赋值只执行一次
	{
		Flight.yaw_ctrl_mode=AZIMUTH;				//直接控制航向角模式
		Flight.yaw_ctrl_start=1;						//开始控制使能
		Flight.yaw_outer_control_output  =0;//目标角度，等效正北和初始机头朝向一致
		OpticalFlow_Control_Pure(0);//SLAM定点控制	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		flight_subtask_cnt[n]++;	
	}
	else if(flight_subtask_cnt[n]==10)//执行对准机头过程，并判断是否对准完毕
	{
		Flight.yaw_ctrl_mode=AZIMUTH;
		Flight.yaw_outer_control_output  =0;
		OpticalFlow_Control_Pure(0);	
		Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		
		if(Flight.yaw_ctrl_end==1)  
		{
			flight_subtask_cnt[n]++;//执行完毕后，切换到下一阶段
			flight_global_cnt[n]=0;
			
			target_position.x=base_position.x;
			target_position.y=base_position.y;
			target_position.z=First_Working_Height;
			Horizontal_Navigation(target_position.x,
														target_position.y,
														target_position.z,
														GLOBAL_MODE,
														MAP_FRAME);		
		}			
	}
	else if(flight_subtask_cnt[n]==11)//返回到起点
	{
		basic_auto_flight_support();//基本飞行支持软件
		
		//判断是否到达目标航点位置
		if(flight_global_cnt[n]<Times_Fixed)//持续10*5ms=0.05s满足
		{
			float dis_cm=pythagorous2(OpticalFlow_Pos_Ctrl_Err.x,OpticalFlow_Pos_Ctrl_Err.y);
			if(dis_cm<=Fixed_CM)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
		}
		else//持续10*5ms满足，表示到达目标航点位置，后降低目标高度
		{
			flight_subtask_cnt[n]++;
			flight_global_cnt[n]=0;
			
			//以下复位操作的作用：空中到地面不同高度，环境分布陈设变化造成的误差
			//不同高度梯度上陈设变化不大时，以下特殊处理部分可以不去掉
			//特殊处理开始
			send_check_back=4;//重置slam
			VIO_SINS.Position[_EAST] = 0;
			VIO_SINS.Position[_NORTH]= 0;
		  OpticalFlow_Pos_Ctrl_Expect.x=0;
		  OpticalFlow_Pos_Ctrl_Expect.y=0;
			//特殊处理结束			
		}
	}
	else if(flight_subtask_cnt[n]==12)//原地下降
	{
		Flight.yaw_ctrl_mode=ROTATE;
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		OpticalFlow_Control_Pure(0);
		Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-30);//高度控制		
	}	
	else
	{
		basic_auto_flight_support();//基本飞行支持软件
	}
}




//自动起飞到某一高度
uint8_t Auto_Takeoff(float target)
{
	static uint8_t n=11;
	Vector3f target_position;
	basic_auto_flight_support();//基本飞行支持软件	
	if(flight_subtask_cnt[n]==0)
	{
		//不加此行代码，当后续全程无油门上下动作后，飞机最后自动降落到地面不会自动上锁
		Unwanted_Lock_Flag=0;//允许飞机自动上锁，原理和手动推油起飞类似
			
		//记录下初始起点位置，实际项目中可设置为某一基准原点
		base_position.x=VIO_SINS.Position[_EAST];
		base_position.y=VIO_SINS.Position[_NORTH];
		base_position.z=NamelessQuad.Position[_UP];
		
		//execute_time_ms[n]=10000/flight_subtask_delta;//子任务执行时间
		target_position.x=base_position.x;
		target_position.y=base_position.y;
		target_position.z=base_position.z+target;
		Horizontal_Navigation(target_position.x,
													target_position.y,
													target_position.z,
													GLOBAL_MODE,
													MAP_FRAME);
		flight_subtask_cnt[n]=1;
	  return	0;		
	}
	else if(flight_subtask_cnt[n]==1)
	{
		//判断是否起飞到目标高度
		if(flight_global_cnt[n]<400)//持续400*5ms满足
		{
			if(ABS(Total_Controller.Height_Position_Control.Err)<=10.0f)	flight_global_cnt[n]++;
			else flight_global_cnt[n]/=2;
			return	0;
		}
		else//持续200*5ms满足，表示到达目标高度
		{
			return	1;
		}
	}
	return	0;	
}




//基本飞行保障必备子函数——定高、定点、同时加入了偏航指令控制
void ros_flight_support(void)
{
	Flight.yaw_ctrl_mode=ROTATE;//偏航控制为手动模式
	if(RC_Data.rc_rpyt[RC_YAW]==0)//无水平遥控量给定)
	{
		if(ngs_nav_ctrl.cmd_vel_update==1)	Flight.yaw_outer_control_output  =ngs_nav_ctrl.cmd_vel_angular_z;
		else Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];//作用期过后，控制权限交给遥控器
	}
	else
	{
		Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		ngs_nav_ctrl.cmd_vel_update=0;//只要存在遥控器打杆操作，强制结束本次速度控制
		ngs_nav_ctrl.cmd_vel_during_cnt=0;		
	}		
	OpticalFlow_Control_Pure(0);//SLAM定点控制
	Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
}
