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
#include "VL53L1X.h"
#include "vl53l1_api.h"
#include "VL53L1X_i2c.h"
int32_t distance;
VL53L1_Dev_t VL53;
VL53L1_RangingMeasurementData_t vl52l1x_result_data;
mode_data Mode_data[]=
{
	{(FixPoint1616_t)(16384), //its uint32_t, uint16_.uint16_t, 0.25*65536
	 (FixPoint1616_t)(1179648),	 //18*65536
	 33000,
	 14,
	 10},//default
		
	{(FixPoint1616_t)(16384),	//0.25*65536
	 (FixPoint1616_t)(1179648),		//18*65536
	 200000, 
	 14,
	 10},//high accuracy
		
  {(FixPoint1616_t)(6554),		//0.1*65536
	 (FixPoint1616_t)(3932160),		//60*65536
	 33000,
	 18,
	 14},//long distance
	
  {(FixPoint1616_t)(16384),	//0.25*65536
	 (FixPoint1616_t)(2097152),		//32*65536
	 20000,
	 14,
	 10},//high speed
};

uint16_t di;

VL53L1_Error VL53L1Init(VL53L1_Dev_t* pDev)
{
	
	VL53L1_I2C_Init();
	VL53L1_Error Status = VL53L1_ERROR_NONE;
	pDev->I2cDevAddr=(0x52);//默认地址
	pDev->comms_type=1;//默认通信模式
	pDev->comms_speed_khz = 400;//通信速率（可到400hz）

	VL53L1_RdWord(pDev, 0x010F, &di);
	Status = VL53L1_WaitDeviceBooted(pDev);
  if(Status!=VL53L1_ERROR_NONE)
	{
		//printf("Wait device Boot failed!\r\n");
		return Status;
	}
	
  Delay_Ms(2);
	Status = VL53L1_DataInit(pDev);//device init
	if(Status!=VL53L1_ERROR_NONE) 
	{
		//printf("datainit failed!\r\n");
		return Status;
	}

	Delay_Ms(2);
	Status = VL53L1_StaticInit(pDev);
	if(Status!=VL53L1_ERROR_NONE) 
	{
		//printf("static init failed!\r\n");
		return Status;
	}
	
	Delay_Ms(2);
	Status = VL53L1_SetDistanceMode(pDev, VL53L1_DISTANCEMODE_LONG);	//short,medium,long
	if(Status!=VL53L1_ERROR_NONE) 
	{
		//printf("set discance mode failed!\r\n");
		return Status;
	}
	//定时预算传感器执行一次量程所需要的时间，最小和最大时序预算为 [20 ms，1000 ms]示例：
	VL53L1_SetMeasurementTimingBudgetMicroSeconds(pDev, 35000);//35ms
	VL53L1_SetInterMeasurementPeriodMilliSeconds(pDev, 50);//测量间隔设置成50ms
	VL53L1_StopMeasurement(pDev);
	VL53L1_StartMeasurement(pDev);	
	Delay_Ms(2);
	return Status;
}

/** 
* @brief  传感器进行一个出厂校准
* @param [in] pDev  指定传感器
* @param [in] save  结果存储地址
* @retval  VL53L1_Error类型 
* @par 日志 
*
*/
VL53L1_Error VL53Cali(VL53L1_Dev_t* pDev,void * save)
{
    VL53L1_Error Status = VL53L1_ERROR_NONE;
    Status = VL53L1_StopMeasurement(pDev);
	if(Status!=VL53L1_ERROR_NONE) 
		return Status;
    Status = VL53L1_PerformRefSpadManagement(pDev);//perform ref SPAD management
	if(Status!=VL53L1_ERROR_NONE) 
		return Status;
    /*
    Status = VL53L1_PerformOffsetSimpleCalibration(pDev,140);//14cm的出厂校验值
	if(Status!=VL53L1_ERROR_NONE) 
		return Status;
    */
    Status = VL53L1_GetCalibrationData(pDev,save);
	if(Status!=VL53L1_ERROR_NONE) 
		return Status;
    //全部完成 重新打开测量
    Status = VL53L1_StartMeasurement(pDev);
    return Status;
}

VL53L1_Error VL53InitParam(VL53L1_Dev_t* pDev,uint8_t mode)
{
    VL53L1_Error status = VL53L1_ERROR_NONE;
    //4个限制
    status = VL53L1_SetLimitCheckEnable(pDev,VL53L1_CHECKENABLE_SIGMA_FINAL_RANGE,1);//sigma--standard deviation, enable SIGMA limit check
	if(status!=VL53L1_ERROR_NONE) 
		return status;
    Delay_Ms(2);
	status = VL53L1_SetLimitCheckEnable(pDev,VL53L1_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,1);//signal--amplitude of the signal-
																																												//-reflected. enable signal rate limit check
	if(status!=VL53L1_ERROR_NONE) 
		return status;
	
	
	Delay_Ms(2);
	status = VL53L1_SetLimitCheckValue(pDev,VL53L1_CHECKENABLE_SIGMA_FINAL_RANGE,Mode_data[mode].sigmaLimit);//set SIGMA limit
	if(status!=VL53L1_ERROR_NONE) 
		return status;
	
	
	Delay_Ms(2);
	status = VL53L1_SetLimitCheckValue(pDev,VL53L1_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,Mode_data[mode].signalLimit);//set signal rate limit
	if(status!=VL53L1_ERROR_NONE) 
		return status;
    //4个限制值设置完成
    
    
    status = VL53L1_SetMeasurementTimingBudgetMicroSeconds(pDev,Mode_data[mode].timingBudget);//set the max interval for a whole diatance test
	if(status!=VL53L1_ERROR_NONE) 
		return status;
	Delay_Ms(2);
	status = VL53L1_SetInterMeasurementPeriodMilliSeconds(pDev, 300);
	if(status!=VL53L1_ERROR_NONE) 
	{
		//printf("SetInterMeasurementPeriodMilliSeconds failed!\r\n");
		return status;
	}
	Delay_Ms(2);
	status = VL53L1_StartMeasurement(pDev);
	if(status!=VL53L1_ERROR_NONE) 
	{
		//printf("start measurement failed!\r\n");
		return status;
	}
    return status;
}



//Test_Period(Testime *Time_Lab)
Testime vl;
VL53L1_Error VL53L1X_Get_Distance(VL53L1_Dev_t* pDev)
{
	VL53L1_Error status = VL53L1_ERROR_NONE;
	uint8_t isDataReady=0;
	status = VL53L1_GetMeasurementDataReady(pDev,&isDataReady);
	if(isDataReady)
	{
		Test_Period(&vl);
		status = VL53L1_GetRangingMeasurementData(pDev, &vl52l1x_result_data);
		GD_Distance = vl52l1x_result_data.RangeMilliMeter/10.0;
		status = VL53L1_ClearInterruptAndStartMeasurement(pDev);
		
		WP_Sensor.vl53l1x_updtate_flag=1;
		GD_Distance_Div=1000*(GD_Distance-Last_GD_Distance)/vl.Time_Delta;
		GD_Distance_Acc=1000*(GD_Distance_Div-Last_GD_Distance_Div)/vl.Time_Delta;
		if(GD_Distance<=350&&GD_Distance>0)  Sensor_Flag.Ground_Health=1;
		else  Sensor_Flag.Ground_Health=0; 
	}
	return status;
}


