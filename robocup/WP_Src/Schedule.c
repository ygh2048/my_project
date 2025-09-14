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
#include "Time.h"
#include "Schedule.h"





void Test_Period(Testime *Time_Lab)
{
  Time_Lab->Last_Time=Time_Lab->Now_Time;
  Time_Lab->Now_Time=micros()/1000.0f;//(10000*TIME_ISR_CNT+TimerValueGet(TIMER1_BASE,TIMER_A)/80.0f)/1000.0f;//单位ms
  Time_Lab->Time_Delta=(Time_Lab->Now_Time-Time_Lab->Last_Time);
  Time_Lab->Time_Delta_INT=(uint16)(Time_Lab->Time_Delta+0.5f);//四舍五入
}


void Get_Systime(systime *sys)
{
  sys->last_time=sys->current_time;
  sys->current_time=micros()/1000.0f;//单位ms
  sys->period=sys->current_time-sys->last_time;
  sys->period_int=(uint16_t)(sys->period+0.5f);//四舍五入
}


//void Schedule_init(void)
//{
//  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);//定时器1使能				
//  TimerConfigure(TIMER1_BASE,TIMER_CFG_PERIODIC_UP);//32位周期定时器
//  TimerLoadSet(TIMER1_BASE,TIMER_A,SysCtlClockGet()/100);//设定装载值,（80M/100）*1/80M=10ms				
//  IntEnable(INT_TIMER1A);//定时器1中断使能				
//  TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT); //中断输出, 设置模式;
//  TimerIntRegister(TIMER1_BASE,TIMER_A,TIMER1A_Handler);//中断函数注册
//  //IntMasterEnable();			
//  TimerEnable(TIMER1_BASE,TIMER_A); //定时器使能开始计数	
//  //IntPriorityGroupingSet(0);
//  IntPrioritySet(INT_TIMER1A,USER_INT0);
//}



////#define Hour         3
////#define Minute       2
////#define Second       1
////#define MicroSecond  0
////uint16 Time_Sys[4]={0};
////uint16 Microsecond_Cnt=0;
////uint32 TIME_ISR_CNT=0,LAST_TIME_ISR_CNT=0;
//void TIMER1A_Handler(void)//中断函数
//{
//  //LAST_TIME_ISR_CNT=TIME_ISR_CNT;
//  //TIME_ISR_CNT++;//每10ms自加
//  //	Microsecond_Cnt++;
//  //	if(Microsecond_Cnt>=100)//1s
//  //	{
//  //		Microsecond_Cnt=0;
//  //		Time_Sys[Second]++;
//  //		if(Time_Sys[Second]>=60)//1min
//  //		{
//  //			Time_Sys[Second]=0;
//  //			Time_Sys[Minute]++;
//  //			if(Time_Sys[Minute]>=60)//1hour
//  //			{
//  //				Time_Sys[Minute]=0;
//  //				Time_Sys[Hour]++;
//  //			}
//  //		}
//  //	}
//  TimerIntClear(TIMER1_BASE,TIMER_TIMA_TIMEOUT);  	
//}

