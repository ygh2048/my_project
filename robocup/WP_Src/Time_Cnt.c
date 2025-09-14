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
#include "Time_Cnt.h"


void Time0A_init(void)//系统调度定时器初始化
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);							//定时器0使能				
  TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);						//32位周期定时器				
  TimerLoadSet(TIMER0_BASE,TIMER_A,SysCtlClockGet()/200);		//设定装载值,（80M/200）*1/80M=5ms				
  IntEnable(INT_TIMER0A);																		//总中断使能				
  TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT); 					//中断输出, 设置模式;			
  TimerIntRegister(TIMER0_BASE,TIMER_A,TIMER0A_Handler);		//中断函数注册
//  IntMasterEnable();			
  TimerEnable(TIMER0_BASE,TIMER_A); 												//定时器使能开始计数
  IntPrioritySet(INT_TIMER0A,USER_INT6);
}

void Time2A_init(void)//系统调度定时器初始化
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);							//定时器0使能				
  TimerConfigure(TIMER2_BASE,TIMER_CFG_PERIODIC);						//32位周期定时器				
  TimerLoadSet(TIMER2_BASE,TIMER_A,SysCtlClockGet()/100);		//设定装载值,（80M/100）*1/80M=10ms				
  IntEnable(INT_TIMER2A);																		//总中断使能				
  TimerIntEnable(TIMER2_BASE,TIMER_TIMA_TIMEOUT); 					//中断输出, 设置模式;			
  TimerIntRegister(TIMER2_BASE,TIMER_A,TIMER2A_Handler);		//中断函数注册
//  IntMasterEnable();			
  TimerEnable(TIMER2_BASE,TIMER_A); 												//定时器使能开始计数
  IntPrioritySet(INT_TIMER2A,USER_INT7);
}



void Schedule_init(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);//定时器1使能				
  TimerConfigure(TIMER1_BASE,TIMER_CFG_PERIODIC_UP);//32位周期定时器
  TimerLoadSet(TIMER1_BASE,TIMER_A,SysCtlClockGet()/1000);//设定装载值,（80M/400*1/80M=2.5ms				
  IntEnable(INT_TIMER1A);//定时器1中断使能				
  TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT); //中断输出, 设置模式;
  TimerIntRegister(TIMER1_BASE,TIMER_A,TIMER1A_Handler);//中断函数注册
//  IntMasterEnable();			
  TimerEnable(TIMER1_BASE,TIMER_A); //定时器使能开始计数	
  //IntPriorityGroupingSet(0);	
  IntPrioritySet(INT_TIMER1A,USER_INT5);
}

/***********************************************************
@函数名：Time_init
@入口参数：无
@出口参数：无
@功能描述：系统调度定时器初始化
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/
void Time_init(void)//系统调度定时器初始化
{
	IntMasterDisable();
  Time0A_init();
	Time2A_init();
	Schedule_init();//定时器使能
	IntMasterEnable();	
}





Sensor WP_Sensor;
AHRS 	 WP_AHRS;
Testime Time0_Delta,Time0_Delta1;
float time0_max;
/***********************************************************
@函数名：TIMER0A_Handler
@入口参数：无
@出口参数：无
@功能描述：系统调度定时器中断服务函数：主要进行遥控器解析、
传感数据采集、数字滤波、姿态解算、惯性导航、控制等对周期有
严格要求的函数
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/
void TIMER0A_Handler(void)				//系统调度中断函数   4.4ms
{  
	Test_Period(&Time0_Delta);
	Remote_Control();								  //遥控器数据解析
  INS_Sensor_Update();						  //获取姿态数据	
	OBS_Sensor_Update();						  //传感器数据更新	
	Get_Status_Feedback();					  //水平与竖直方向惯导数据
	Optflow_Statemachine();					  //光流状态机，初始化时存在光流外设
  GPS_Data_Prase();								  //GPS数据解析
  KalmanFilter_Horizontal();			  //水平位置GPS双观测量Kalman滤波融合
	Ground_Sensor_Statemachine();     //对地距离传感器状态机更新，超声波/tfmini_plus/VL53L1X
	TOFSense_IIC_Update();					  //Tofsense测距	
	CarryPilot_Control();	            //总控制器
  Calibration_All_Check();					//校准相关检测
	Temperature_State_Check();			  //温控系统检测
  Bling_Working(Bling_Mode);			  //状态指示灯运行
	ADC_Sample_Trigger();						  //ADC电压采集
	Read_Button_State_All();				  //按键状态读取
	ADC_Button_Scan();							  //按键数据处理
	Battery_Voltage_Detection();			//电池低压检测——蜂鸣器报警
	Laser_Light_Work(&laser_light_1); //激光笔/RGB灯/蜂鸣器1驱动
	Laser_Light_Work(&laser_light_2); //激光笔/RGB灯/蜂鸣器2驱动
	Laser_Light_Work(&buzzer);        //电源板蜂鸣器驱动
	NCLink_Send_IMU_Feedback_PC();		//发送无人机状态数据给机载计算机——树莓派4B详情介绍：https://item.taobao.com/item.htm?spm=a230r.1.14.6.1e7575f8M0CgLF&id=669633894747&ns=1&abbucket=12#detail
	Test_Period(&Time0_Delta1);
	float tmp=Time0_Delta1.Now_Time-Time0_Delta.Now_Time;
	if(time0_max<tmp) time0_max=tmp;
	TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
}


Testime Time1_Delta,Time1_Delta1;
float time1_max;
void TIMER1A_Handler(void)//温控中断函数
{
	Test_Period(&Time1_Delta);
	Temperature_Ctrl();	
  Simulation_PWM_Output(Total_Controller.IMU_Temperature_Control.Control_OutPut);
	lsn10_data_prase();
	Test_Period(&Time1_Delta1);
	float tmp=Time1_Delta1.Now_Time-Time1_Delta.Now_Time;
	if(time1_max<tmp) time1_max=tmp;	
	TimerIntClear(TIMER1_BASE,TIMER_TIMA_TIMEOUT);  	
}



Testime Time2_Delta;
void TIMER2A_Handler(void)//地面站数据发送中断函数
{
	Test_Period(&Time2_Delta);
	NCLink_SEND_StateMachine();			//无名创新开源地面站，下载链接http://nameless.tech/download.html
	laser_state_sort();							//雷神N10激光雷达数据解析

	speaker_notify_run(&speaker_mode);	
	TimerIntClear(TIMER2_BASE,TIMER_TIMA_TIMEOUT);
}

