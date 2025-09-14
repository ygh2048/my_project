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
#include "WP_ADC.h"
#include "hw_adc.h"
#include "adc.h"



void ADC0IntHandler(void);


uint32_t adc_value[8];
float Button_ADCResult;
/***********************************************************
@函数名：ADC_Init
@入口参数：无
@出口参数：无
功能描述：电压测量端口PE3，作为ADC0的通道0初始化，非中断式采集
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/
void ADC_Init(void)//ADC初始化配置   
{    
  SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);// Enable the ADC1 module.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));// Wait for the ADC1 module to be ready.	
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);    
  GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
	
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);    
//  GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);	
//	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_1);
//	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_2);
	GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);
	
  //ADCReferenceSet(ADC0_BASE, ADC_REF_EXT_3V);
  //Enable the first sample sequencer to capture the value of channel 0 when
  //the processor trigger occurs.  
  ADCSequenceConfigure(ADC0_BASE,0,ADC_TRIGGER_PROCESSOR, 0); 
  //ADCHardwareOversampleConfigure(ADC0_BASE, 8);	
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_CH4	|ADC_CTL_END | ADC_CTL_IE);
//	ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_CH5);
//	ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_CH6);
//	ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_CH7 |ADC_CTL_END | ADC_CTL_IE);
		
	ADCIntClear(ADC0_BASE, 0);
	ADCSequenceEnable(ADC0_BASE, 0);    
  ADCIntEnable(ADC0_BASE, 0); 
  //中断触发方式设置ADC_INT_SS0、ADC_INT_DMA_SS0、ADC_INT_DCON_SS0
	ADCIntEnableEx(ADC0_BASE,ADC_INT_SS0);//分别代表普通序列触发、DMA触发和数字比较器触发
	IntEnable(INT_ADC0SS0); //使能ADC采样序列中断	
	ADCIntRegister(ADC0_BASE,0,ADC0IntHandler);		//中断函数注册
	IntPrioritySet(INT_ADC0SS0, USER_INT7);	
} 


float Battery_Voltage;
/***********************************************************
@函数名：Get_Battery_Voltage
@入口参数：无
@出口参数：无
功能描述：测量电池电压，飞控默认分压比为11，故测量电压不要超过
3.3V*11=36.6V，若想测更大的电压，调整板子上分压电阻阻值即可
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/
void Get_Battery_Voltage(void)//ADC获取   
{
//	ADCProcessorTrigger(ADC0_BASE, 0);   
//	while(!ADCIntStatus(ADC0_BASE, 0, false)) {;}
//	ADCIntClear(ADC0_BASE, 0);	
//	ADCSequenceDataGet(ADC0_BASE, 0, value);   
	//value[0] =  HWREG(ADC0_BASE+ ADC_SEQ + (ADC_SEQ_STEP*0) + ADC_SSFIFO);
	static float value_filter;
	value_filter=0.9f*value_filter+0.1f*adc_value[0]*36.3f/4095.0f;	
	Battery_Voltage=value_filter;	
}


void ADC_Sample_Trigger(void)
{
	static uint16_t cnt=0;
	cnt++;
	if(cnt<=2) return;
	cnt=0;
	ADCProcessorTrigger(ADC0_BASE, 0);
}

Testime ADC_Delta;
void ADC0IntHandler(void)
{
	Test_Period(&ADC_Delta);
	ADCIntClear(ADC0_BASE, 0);// 清除ADC中断标志。
	//while(!ADCIntStatus(ADC0_BASE, 0, false));//等待采集结束
	ADCSequenceDataGet(ADC0_BASE, 0, adc_value);// 读取ADC值
}


static uint16_t low_vbat_cnt=0;
void Battery_Voltage_Detection(void)
{
	static uint16_t _cnt=0;
	_cnt++;
	if(_cnt>=200)//每1S检测一次
	{
		_cnt=0;
		if(Battery_Voltage<Safe_Vbat/1000.0f)	 low_vbat_cnt++;
		else low_vbat_cnt/=2;
		if(low_vbat_cnt>=10)//持续10s满足
		{
			buzzer.period=200;//200*5ms
			buzzer.light_on_percent=0.25f;
			buzzer.reset=1;	
			buzzer.times=5;//闪烁5次
			low_vbat_cnt=0;//清0待下一周期检测			
		}			
	}
}

