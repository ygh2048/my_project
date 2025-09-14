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
#include "SYSTEM.h"

/***********************************************************
@函数名：HardWave_Init
@入口参数：无
@出口参数：无
功能描述：对系统芯片资源常规硬件资源进行一系列初始化，具体见
各初始化后的注释
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/


void HardWave_Init(void)
{
	WP_Init();									   //飞控芯片初始化
	ConfigureUART1(921600); 			 //串口1初始化，无名创新地面站V1.0.5版本
	ConfigureUART0(256000); 			 //串口0初始化，前向视觉SDK串口
	ADC_Init();										 //ADC初始化，检测外部电池电压
  Butterworth_Parameter_Init();  //滤波器参数初始化
  PPM_Init();                    //PPM解析初始化
  PWM_Init();                    //PWM初始化
  Bling_Init();                  //LED状态指示灯初始化
	Reserved_IO_Init();						 //预留IO初始化——LaunchPad版本飞控时，看下此链接：https://www.bilibili.com/read/cv12740760
	Key_Init();										 //板载按键初始化
  RC_Calibration_Trigger();      //遥控器行程标定检查
  Horizontal_Calibration_Init(); //机架水平标定初始化
  PID_Paramter_Init_With_Flash();//PID参数初始化
	Other_Parameter_Init();        //其它参数初始化
  while(ICM20689_Init());				 //MPU6050陀螺仪、加速度计配置初始化
  Compass_Init();								 //磁力计配置初始化
  SPL06_Init();									 //SPL06气压计配置初始化
  Parameter_Init();							 //加速度计、磁力计校准参数初始化
  WP_Quad_Init();								 //根据观测传感器加速度计、磁力计对姿态四元数初始化
  ConfigureUART3();							 //串口3初始化，OPENMV、树莓派视觉模组、SDK模式串口
	Reserved_Serialport_Init();		 //串口6初始化，前向超声波测距
  ConfigureUART7();							 //串口7初始化.超声波模块/TOF解析串口	
  Uart2_Serialport_Init();			 //串口2初始化，光流模块LC307/激光雷达数据解析
	SDK_Init();										 //SDK模式初始化
	ConfigureUART_Speaker(115200); 	 //串口4/5初始化
	Temperature_Ctrl_Init();			 //温度控制器初始化
  Time_init();									 //飞控调度定时器初始化
  delay_ms(200);								 //初始化延时
}

uint32_t priority[10];
void nvic_priority_get(void)
{
	priority[0]=IntPriorityGet(INT_TIMER2A);
	priority[1]=IntPriorityGet(INT_TIMER1A);
	priority[2]=IntPriorityGet(INT_TIMER0A);
	priority[3]=IntPriorityGet(INT_GPIOC);
	priority[4]=IntPriorityGet(INT_UART0);
	priority[5]=IntPriorityGet(INT_UART1);		
	priority[6]=IntPriorityGet(INT_UART2);
	priority[7]=IntPriorityGet(INT_UART3);
	priority[8]=IntPriorityGet(INT_UART6);
	priority[9]=IntPriorityGet(INT_UART7);
}

