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
#include "Reserved_IO.h"



//LaunchPad版本飞控时，看下此链接：https://www.bilibili.com/read/cv12740760
//TIVA LaunchPad飞控扩展版预留的IO1——PD0、IO2——PD1不能直接用
//需要用到IO1、IO2时，自行取下launchpad上的R9、R10电阻


#define RESERVED_IO_ENABLE 1
//使能前————务必取下R9/R10，否则飞控PWM输出会不正常，会导致炸鸡

_laser_light laser_light_1;
_laser_light laser_light_2;
_laser_light buzzer;
void Reserved_IO_Init(void)
{
#if RESERVED_IO_ENABLE
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2);
	
  GPIOPadConfigSet(GPIO_PORTD_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2,GPIO_STRENGTH_4MA,GPIO_PIN_TYPE_STD);
	
	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_0,0);
	GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_1,0);
  GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_2,0);
#endif
	
	laser_light_1.port=GPIO_PORTD_BASE;
	laser_light_1.pin=GPIO_PIN_0;
	laser_light_1.period=200;//200*5ms
	laser_light_1.light_on_percent=0.2f;
	laser_light_1.reset=0;
	
	
	laser_light_2.port=GPIO_PORTD_BASE;
	laser_light_2.pin=GPIO_PIN_1;
	laser_light_2.period=200;//200*5ms
	laser_light_2.light_on_percent=0.5f;
	
	buzzer.port=GPIO_PORTD_BASE;
	buzzer.pin=GPIO_PIN_2;
	buzzer.period=200;//200*5ms
	buzzer.light_on_percent=0.2f;
	buzzer.reset=0;		
}



void Laser_Light_Work(_laser_light *light)
{
	if(light->reset==1)
	{
		light->reset=0;
		light->cnt=0;
		light->times_cnt=0;//点亮次数计数器
		light->end=0;
	}
	
	if(light->times_cnt==light->times)
	{
		light->end=1;
		return;
	}

	light->cnt++;
	if(light->cnt<=light->period*light->light_on_percent)
	{
		GPIOPinWrite(light->port,light->pin,light->pin);
	}
	else if(light->cnt<light->period)
	{
		GPIOPinWrite(light->port,light->pin,0);
	}
	else//完成点亮一次
	{
		GPIOPinWrite(light->port,light->pin,0);
		light->cnt=0;
		light->times_cnt++;
	}
}



