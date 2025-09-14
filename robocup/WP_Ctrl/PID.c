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
#include "Filter.h"
#include "PID.h"

#define NC330_PRODUCT_NUMBER  3//根据不同机型配置，选择默认PID参数


/*
1偏差限幅标志；  2积分限幅标志；3积分分离标志；   4期望；
5反馈            6偏差；        7上次偏差；       8偏差限幅值；
9积分分离偏差值；10积分值       11积分限幅值；    12控制参数Kp；
13控制参数Ki；   14控制参数Kd； 15控制器总输出；  16上次控制器总输出
17总输出限幅度； 18变积分控制时的积分增益
*/
const float Control_Unit[18][20]=
{
  /*                                         Kp        Ki        Kd            */
  /*1  2  3  4  5  6   7  8   9   10  11    12        13        14  15  16  17    18*/
#if (NC330_PRODUCT_NUMBER==1)
	//3S锂电池，好盈乐天20A默认固件，NC330机架，定制电机KV1400，乾丰8045螺旋桨
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 100,  4.00   ,0.00  ,0.00  ,0  ,0 , 500,   1 ,  1 ,  1 },//Pitch_Angle;偏航角度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 , 300,  0.75   ,1.80  ,1.20  ,0  ,0 , 1000,  1 ,  1 ,  1 },//Pitch_Gyro;偏航角速度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 100,  4.00   ,0.00  ,0.00  ,0  ,0 , 500,   1 ,  1 ,  1 },//Roll_Angle;横滚角
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 , 300,  0.75   ,1.80  ,1.20  ,0  ,0 , 1000,  1 ,  1 ,  1 },//Roll_Gyro;横滚角速度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,45  ,0  ,0 , 150 , 4.00   ,0.00  ,0.00  ,0  ,0 , 500,   1 ,  1 ,  1 },//Yaw_Angle;偏航角
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,300 ,0  ,0 , 200,  1.20   ,1.00  ,0.00  ,0  ,0 , 800,   1 ,  1 ,  1 },//Yaw_Gyro;偏航角速度 
#endif

#if (NC330_PRODUCT_NUMBER==2)		
	//3S锂电池，好盈乐天20A默认固件，NC330机架，伟力电机KV950，专用8045螺旋桨
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 80,   4.00   ,0.00  ,0.00  ,0  ,0 , 450,  1 ,  1 ,  1 },//Pitch_Angle;偏航角度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 , 300,  0.90   ,2.40  ,1.25  ,0  ,0 , 500,  1 ,  1 ,  1 },//Pitch_Gyro;偏航角速度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 80,   4.00   ,0.00  ,0.00  ,0  ,0 , 450,  1 ,  1 ,  1 },//Roll_Angle;横滚角
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 , 300,  0.90   ,2.40  ,1.25  ,0  ,0 , 500,  1 ,  1 ,  1 },//Roll_Gyro;横滚角速度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,45  ,0  ,0 , 150 , 4.00   ,0.00  ,0.00  ,0  ,0 , 450,  1 ,  1 ,  1 },//Yaw_Angle;偏航角
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,300 ,0  ,0 , 200,  1.80   ,1.00  ,0.00  ,0  ,0 , 500,  1 ,  1 ,  1 },//Yaw_Gyro;偏航角速度 
#endif

#if (NC330_PRODUCT_NUMBER==3)			
	//4S锂电池，好盈乐天20A默认固件，NC330机架，定制电机KV1400，定制8045螺旋桨	
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 100,  5.00   ,0.00  ,0.00  ,0  ,0 , 500,   1 ,  1 ,  1 },//Pitch_Angle;偏航角度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 , 300,  0.75   ,2.50  ,2.00  ,0  ,0 , 1000,  1 ,  1 ,  1 },//Pitch_Gyro;偏航角速度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 100,  5.00   ,0.00  ,0.00  ,0  ,0 , 500,   1 ,  1 ,  1 },//Roll_Angle;横滚角
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 , 300,  0.75   ,2.50  ,2.00  ,0  ,0 , 1000,  1 ,  1 ,  1 },//Roll_Gyro;横滚角速度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,45  ,0  ,0 , 150 , 5.00   ,0.00  ,0.00  ,0  ,0 , 500,   1 ,  1 ,  1 },//Yaw_Angle;偏航角
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,300 ,0  ,0 , 200,  1.20   ,1.00  ,0.50  ,0  ,0 , 800,   1 ,  1 ,  1 },//Yaw_Gyro;偏航角速度 
#endif

#if (NC330_PRODUCT_NUMBER==4)
	//4S锂电池，好盈乐天20A默认固件，NC360机架，定制电机KV980，9045螺旋桨
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 100,  5.50   ,0.00  ,0.00  ,0  ,0 , 500,   1 ,  1 ,  1 },//Pitch_Angle;偏航角度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 , 300,  1.20   ,3.00  ,6.00  ,0  ,0 , 1000,  1 ,  1 ,  1 },//Pitch_Gyro;偏航角速度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 100,  5.50   ,0.00  ,0.00  ,0  ,0 , 500,   1 ,  1 ,  1 },//Roll_Angle;横滚角
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 , 300,  1.20   ,3.00  ,6.00  ,0  ,0 , 1000,  1 ,  1 ,  1 },//Roll_Gyro;横滚角速度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,45  ,0  ,0 , 150 , 5.50   ,0.00  ,0.00  ,0  ,0 , 500,   1 ,  1 ,  1 },//Yaw_Angle;偏航角
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,300 ,0  ,0 , 200,  2.40   ,1.00  ,2.00  ,0  ,0 , 800,   1 ,  1 ,  1 },//Yaw_Gyro;偏航角速度 
#endif
		
#if (NC330_PRODUCT_NUMBER==5)
	//4S锂电池，好盈乐天20A默认固件，NC360机架，定制电机KV980，9045螺旋桨
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 100,  5.50   ,0.00  ,0.00  ,0  ,0 , 500,   1 ,  1 ,  1 },//Pitch_Angle;偏航角度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 , 300,  1.00   ,3.00  ,5.00  ,0  ,0 , 1000,  1 ,  1 ,  1 },//Pitch_Gyro;偏航角速度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,30  ,0  ,0 , 100,  5.50   ,0.00  ,0.00  ,0  ,0 , 500,   1 ,  1 ,  1 },//Roll_Angle;横滚角
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 , 300,  1.00   ,3.00  ,5.00  ,0  ,0 , 1000,  1 ,  1 ,  1 },//Roll_Gyro;横滚角速度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,45  ,0  ,0 , 150 , 5.50   ,0.00  ,0.00  ,0  ,0 , 500,   1 ,  1 ,  1 },//Yaw_Angle;偏航角
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,300 ,0  ,0 , 200,  1.80   ,1.00  ,1.50  ,0  ,0 , 800,   1 ,  1 ,  1 },//Yaw_Gyro;偏航角速度 
#endif
  //定高参数
  //高度单项比例控制，有偏差限幅、总输出即为最大攀升、下降速度400cm/s
  //Z轴速度比例+积分控制，无偏差限幅
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,200 ,0  ,0 ,100 ,  1.00     ,0.000   ,0    ,0  ,0 ,500,  1 ,  1 ,  1 },//High_Position;海拔高度位置
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,600 ,0  ,0 ,500 ,  5.00     ,0.000   ,0    ,0  ,0 ,1000,  1 ,  1 ,  1 },//High_Speed;海拔攀升速度
  /*
  1偏差限幅标志；  2积分限幅标志；3积分分离标志；    4期望；
  5反馈            6偏差；        7上次偏差；       8偏差限幅值；
  9积分分离偏差值；10积分值       11积分限幅值；    12控制参数Kp；
  13控制参数Ki；   14控制参数Kd； 15控制器总输出；  16上次控制器总输出
  17总输出限幅度
  */
  /*                                       Kp        Ki        Kd            */
  /*1  2  3  4  5  6   7  8   9   10  11    12        13        14  15  16  17*/
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,180 ,0  ,0 ,8,   0.200    ,0.000    ,0    ,0    ,0 ,300,  1 ,  1 ,  1 },//Longitude_Position;水平经度位置
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 ,450, 1.800    ,0.450    ,0    ,0    ,0 ,800,  1 ,  1 ,  1 },//Longitude_Speed;水平经度速度
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,180 ,0  ,0 ,8,   0.200    ,0.000    ,0    ,0    ,0 ,300,  1 ,  1 ,  1 },//Latitude_Position;水平纬度位置
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,500 ,0  ,0 ,450, 1.800    ,0.450    ,0    ,0    ,0 ,800,  1 ,  1 ,  1 },//Latitude_Speed;水平纬度速度
  /*************加速度控制器****************/
  //期望最大加速度1000cm/s^2
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,1500 ,0  ,0 ,750,0.10    ,2.0000    ,0.0  ,0   ,0 ,1000,  1 ,  1 ,  1 },//垂直加速度控制器		0.12  1.5
		
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,100  ,0  ,0 ,3,  0.32    ,0.0000    ,0    ,0   ,0 ,150,  1 ,  1 ,  1 }, //水平经度方向加速度控制器
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,100  ,0  ,0 ,15, 0.45    ,0.0000    ,0.0  ,0   ,0 ,25 ,  1 ,  1 ,  1 }, //水平维度方向加速度控制器
  /*************光流位置、速度控制器****************/
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,100   ,15  ,0 ,15,  0.40  ,0.00   ,0    ,0   ,0 ,30,   1 ,  1 ,  1 },//光流水平位置控制器  0.25
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,100   ,30  ,0 ,200, 3.00  ,0.03  ,0.0  ,0   ,0 ,450 , 1 ,  1 ,  1 },//光流水平速度控制器 4.50  ,0.05
		
  {1  ,1 ,0 ,0 ,0 ,0 , 0 ,50    ,30  ,0 ,100, 0.50  ,0.00   ,2.5 ,0   ,0 ,300  , 1 ,  1 ,  1 },//SDK位置控制器   2.5  150
};



Butter_Parameter Control_Device_Div_LPF_Parameter_For_Gyro;
Butter_Parameter Control_Device_Div_LPF_Parameter;
Butter_Parameter Control_Device_Err_LPF_Parameter;
Butter_Parameter Control_Device_SDK_Err_LPF_Parameter;
AllControler Total_Controller;//系统总控制器
void PID_Init(PID_Controler *Controler,Controler_Label Label)
{
  Controler->Err_Limit_Flag=(uint8)(Control_Unit[Label][0]);//1偏差限幅标志
  Controler->Integrate_Limit_Flag=(uint8)(Control_Unit[Label][1]);//2积分限幅标志
  Controler->Integrate_Separation_Flag=(uint8)(Control_Unit[Label][2]);//3积分分离标志
  Controler->Expect=Control_Unit[Label][3];//4期望
  Controler->FeedBack=Control_Unit[Label][4];//5反馈值
  Controler->Err=Control_Unit[Label][5];//6偏差
  Controler->Last_Err=Control_Unit[Label][6];//7上次偏差
  Controler->Err_Max=Control_Unit[Label][7];//8偏差限幅值
  Controler->Integrate_Separation_Err=Control_Unit[Label][8];//9积分分离偏差值
  Controler->Integrate=Control_Unit[Label][9];//10积分值
  Controler->Integrate_Max=Control_Unit[Label][10];//11积分限幅值
  Controler->Kp=Control_Unit[Label][11];//12控制参数Kp
  Controler->Ki=Control_Unit[Label][12];//13控制参数Ki
  Controler->Kd=Control_Unit[Label][13];//14控制参数Ki
  Controler->Control_OutPut=Control_Unit[Label][14];//15控制器总输出
  Controler->Last_Control_OutPut=Control_Unit[Label][15];//16上次控制器总输出
  Controler->Control_OutPut_Limit=Control_Unit[Label][16];//17上次控制器总输出
  Controler->Scale_Kp=Control_Unit[Label][17];
  Controler->Scale_Ki=Control_Unit[Label][18];
  Controler->Scale_Kd=Control_Unit[Label][19];

}

void Total_PID_Init(void)
{
  PID_Init(&Total_Controller.Pitch_Angle_Control,Pitch_Angle_Controler);
  PID_Init(&Total_Controller.Pitch_Gyro_Control,Pitch_Gyro_Controler);
  PID_Init(&Total_Controller.Roll_Angle_Control,Roll_Angle_Controler);
  PID_Init(&Total_Controller.Roll_Gyro_Control,Roll_Gyro_Controler);
  PID_Init(&Total_Controller.Yaw_Angle_Control,Yaw_Angle_Controler);
  PID_Init(&Total_Controller.Yaw_Gyro_Control,Yaw_Gyro_Controler);
  PID_Init(&Total_Controller.Height_Position_Control,Height_Position_Controler);
  PID_Init(&Total_Controller.Height_Speed_Control,Height_Speed_Controler);
  PID_Init(&Total_Controller.Longitude_Position_Control,Longitude_Position_Controler);
  PID_Init(&Total_Controller.Longitude_Speed_Control,Longitude_Speed_Controler);
  PID_Init(&Total_Controller.Latitude_Position_Control,Latitude_Position_Controler);
  PID_Init(&Total_Controller.Latitude_Speed_Control,Latitude_Speed_Controler);
  
  PID_Init(&Total_Controller.Height_Acce_Control,Height_Acce_Controler);
	
	
  PID_Init(&Total_Controller.Optical_Position_Control_X,Optical_Position_Controler_X);
  PID_Init(&Total_Controller.Optical_Speed_Control_X,Optical_Speed_Controler_X);
  PID_Init(&Total_Controller.Optical_Position_Control,Optical_Position_Controler);
  PID_Init(&Total_Controller.Optical_Speed_Control,Optical_Speed_Controler);
	
	
  PID_Init(&Total_Controller.SDK_Roll_Position_Control,SDK_Roll_Position_Controler);
  PID_Init(&Total_Controller.SDK_Pitch_Position_Control,SDK_Roll_Position_Controler);
  
  Set_Cutoff_Frequency(100, 10 ,&Control_Device_SDK_Err_LPF_Parameter);//100 5
	//Set_Cutoff_Frequency(10, 4 ,&Control_Device_SDK_Err_LPF_Parameter);
	
  Set_Cutoff_Frequency(WP_Duty_Freq, 5 ,&Control_Device_Err_LPF_Parameter);//10  PID控制器偏差低通
  Set_Cutoff_Frequency(WP_Duty_Freq, 20,&Control_Device_Div_LPF_Parameter); //20
  Set_Cutoff_Frequency(Imu_Sampling_Freq, 15,&Control_Device_Div_LPF_Parameter_For_Gyro);//15  30  10
}

float PID_Control(PID_Controler *Controler,float period_second)
{
  float controller_dt=0;

	Get_Systime(&Controler->Systime_t);
	controller_dt=Controler->Systime_t.period/1000.0000f;
	if(controller_dt>1.05f*period_second||controller_dt<0.95f*period_second||isnan(controller_dt)!=0)   controller_dt=period_second;
	if(controller_dt<0.0001f) return 0;
	
  /*******偏差计算*********************/
  Controler->Last_Err=Controler->Err;//保存上次偏差
  Controler->Err=Controler->Expect-Controler->FeedBack;//期望减去反馈得到偏差
  if(Controler->Err_Limit_Flag==1)//偏差限幅度标志位
  {
    if(Controler->Err>=Controler->Err_Max)   Controler->Err= Controler->Err_Max;
    if(Controler->Err<=-Controler->Err_Max)  Controler->Err=-Controler->Err_Max;
  }
  /*******积分计算*********************/
  if(Controler->Integrate_Separation_Flag==1)//积分分离标志位
  {
    if(ABS(Controler->Err)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  /*******积分限幅*********************/
  if(Controler->Integrate_Limit_Flag==1)//积分限制幅度标志
  {
    if(Controler->Integrate>=Controler->Integrate_Max)   Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)  Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******总输出计算*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//输出值递推
  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err//比例
													 +Controler->Integrate//积分
													 +Controler->Kd*(Controler->Err-Controler->Last_Err);//微分
  /*******总输出限幅*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******返回总输出*********************/
  return Controler->Control_OutPut;
}

float PID_Control_Yaw(PID_Controler *Controler,float period_second)
{
  float controller_dt=0;
	Get_Systime(&Controler->Systime_t);
	controller_dt=Controler->Systime_t.period/1000.0000f;
	if(controller_dt>1.05f*period_second||controller_dt<0.95f*period_second||isnan(controller_dt)!=0)   controller_dt=period_second;
	if(controller_dt<0.0001f) return 0;
	
  /*******偏差计算*********************/
  Controler->Last_Err=Controler->Err;//保存上次偏差
  Controler->Err=Controler->Expect-Controler->FeedBack;//期望减去反馈得到偏差
	
	
  /***********************偏航角偏差超过+-180处理*****************************/
	if(Controler->Err<-180)  Controler->Err=Controler->Err+360;
	if(Controler->Err>180)  Controler->Err=Controler->Err-360;
	
  if(Controler->Err_Limit_Flag==1)//偏差限幅度标志位
  {
    if(Controler->Err>=Controler->Err_Max)   Controler->Err= Controler->Err_Max;
    if(Controler->Err<=-Controler->Err_Max)  Controler->Err=-Controler->Err_Max;
  }
  /*******积分计算*********************/
  if(Controler->Integrate_Separation_Flag==1)//积分分离标志位
  {
    if(ABS(Controler->Err)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  /*******积分限幅*********************/
  if(Controler->Integrate_Limit_Flag==1)//积分限制幅度标志
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******总输出计算*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//输出值递推
  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err//比例
														+Controler->Integrate//积分
															+Controler->Kd*(Controler->Err-Controler->Last_Err);//微分
  /*******总输出限幅*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******返回总输出*********************/
  return Controler->Control_OutPut;
}



float Control_Device_LPF(float curr_input,Butter_BufferData *Buffer,Butter_Parameter *Parameter)
{
	if(Buffer->Output_Butter[0]==0&&
		 Buffer->Output_Butter[1]==0&&
		 Buffer->Output_Butter[2]==0&&
		 Buffer->Input_Butter[0]==0&&
		 Buffer->Input_Butter[1]==0&&
		 Buffer->Input_Butter[2]==0)
	{
		Buffer->Output_Butter[0]=curr_input;
		Buffer->Output_Butter[1]=curr_input;
		Buffer->Output_Butter[2]=curr_input;
		Buffer->Input_Butter[0]=curr_input;
		Buffer->Input_Butter[1]=curr_input;
		Buffer->Input_Butter[2]=curr_input;
		return curr_input;
	}
	
  /* 加速度计Butterworth滤波 */
  /* 获取最新x(n) */
  Buffer->Input_Butter[2]=curr_input;
  /* Butterworth滤波 */
  Buffer->Output_Butter[2]=Parameter->b[0] * Buffer->Input_Butter[2]
													+Parameter->b[1] * Buffer->Input_Butter[1]
													+Parameter->b[2] * Buffer->Input_Butter[0]
													-Parameter->a[1] * Buffer->Output_Butter[1]
													-Parameter->a[2] * Buffer->Output_Butter[0];
  /* x(n) 序列保存 */
  Buffer->Input_Butter[0]=Buffer->Input_Butter[1];
  Buffer->Input_Butter[1]=Buffer->Input_Butter[2];
  /* y(n) 序列保存 */
  Buffer->Output_Butter[0]=Buffer->Output_Butter[1];
  Buffer->Output_Butter[1]=Buffer->Output_Butter[2];
	
	for(uint16_t i=0;i<3;i++)
	{
	  if(isnan(Buffer->Output_Butter[i])==1
			||isnan(Buffer->Input_Butter[i])==1)		
			{		
				Buffer->Output_Butter[0]=curr_input;
				Buffer->Output_Butter[1]=curr_input;
				Buffer->Output_Butter[2]=curr_input;
				Buffer->Input_Butter[0]=curr_input;
				Buffer->Input_Butter[1]=curr_input;
				Buffer->Input_Butter[2]=curr_input;
				return curr_input;
			}
	}	
  return Buffer->Output_Butter[2];
}

float PID_Control_Div_LPF(PID_Controler *Controler,float period_second)
{
  int16  i=0;
  float controller_dt=0;
	Get_Systime(&Controler->Systime_t);
	controller_dt=Controler->Systime_t.period/1000.0000f;
	if(controller_dt>1.05f*period_second||controller_dt<0.95f*period_second||isnan(controller_dt)!=0)   controller_dt=period_second;
	if(controller_dt<0.0001f) return 0;
	
  /*******偏差计算*********************/
  Controler->Last_Err=Controler->Err;//保存上次偏差
  Controler->Err=Controler->Expect-Controler->FeedBack;//期望减去反馈得到偏差
  Controler->Dis_Err=Controler->Err-Controler->Last_Err;//原始微分
  
//  /******************************************/
//  //均值滤波，保证得到数据不跳变，避免期望阶跃时，微分输出异常
//	float tempa,tempb,tempc,max,min;//用于防跳变滤波  
//	tempa=Controler->Pre_Last_Dis_Err_LPF;
//  tempb=Controler->Last_Dis_Err_LPF;
//  tempc=Controler->Dis_Err;
//  max = tempa > tempb ? tempa:tempb;
//  max = max > tempc ? max:tempc;
//  min = tempa < tempb ? tempa:tempb;
//  min = min < tempc ? min:tempc;
//  if(tempa > min && tempa < max)    Controler->Dis_Err = tempa;
//  if(tempb > min  && tempb < max )  Controler->Dis_Err = tempb;
//  if(tempc > min  &&  tempc < max)  Controler->Dis_Err = tempc;
//  Controler->Pre_Last_Dis_Err_LPF = Controler->Last_Dis_Err_LPF;
//  Controler->Last_Dis_Err_LPF = Controler->Dis_Err;
//  /*****************************************/
  
  for(i=4;i>0;i--)//数字低通后微分项保存
  {
    Controler->Dis_Error_History[i]=Controler->Dis_Error_History[i-1];
  }
  Controler->Dis_Error_History[0]=Control_Device_LPF(Controler->Dis_Err,
                                                     &Controler->Control_Device_LPF_Buffer,
                                                     &Control_Device_Div_LPF_Parameter);//巴特沃斯低通后得到的微分项,20hz
  
  if(Controler->Err_Limit_Flag==1)//偏差限幅度标志位
  {
    if(Controler->Err>=Controler->Err_Max)   Controler->Err= Controler->Err_Max;
    if(Controler->Err<=-Controler->Err_Max)  Controler->Err=-Controler->Err_Max;
  }
  /*******积分计算*********************/
  if(Controler->Integrate_Separation_Flag==1)//积分分离标志位
  {
    if(ABS(Controler->Err)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  /*******积分限幅*********************/
  if(Controler->Integrate_Limit_Flag==1)//积分限制幅度标志
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******总输出计算*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//输出值递推
  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err//比例
    +Controler->Integrate//积分
      //+Controler->Kd*Controler->Dis_Err;//微分
      +Controler->Kd*Controler->Dis_Error_History[0];//微分项来源于巴特沃斯低通滤波器
  /*******总输出限幅*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******返回总输出*********************/
  return Controler->Control_OutPut;
}


float PID_Control_Err_LPF(PID_Controler *Controler,float period_second)
{
  float controller_dt=0;
	Get_Systime(&Controler->Systime_t);
	controller_dt=Controler->Systime_t.period/1000.0000f;
	if(controller_dt>1.05f*period_second||controller_dt<0.95f*period_second||isnan(controller_dt)!=0)   controller_dt=period_second;
	if(controller_dt<0.0001f) return 0;
	
  /*******偏差计算*********************/
  Controler->Last_Err=Controler->Err;//保存上次偏差
  Controler->Err=Controler->Expect-Controler->FeedBack;//期望减去反馈得到偏差
  Controler->Dis_Err=Controler->Err-Controler->Last_Err;//原始微分
  
  Controler->Last_Err_LPF=Controler->Err_LPF;
  Controler->Err_LPF=Control_Device_LPF(Controler->Err,
                                        &Controler->Control_Device_LPF_Buffer,
                                        &Control_Device_Err_LPF_Parameter);//巴特沃斯低通后得到的微分项,20hz
  
  Controler->Dis_Err_LPF=Controler->Err_LPF-Controler->Last_Err_LPF;//偏差经过低通后的微分量
  
  if(Controler->Err_Limit_Flag==1)//偏差限幅度标志位
  {
    if(Controler->Err_LPF>=Controler->Err_Max)   Controler->Err_LPF= Controler->Err_Max;
    if(Controler->Err_LPF<=-Controler->Err_Max)  Controler->Err_LPF=-Controler->Err_Max;
  }
  /*******积分计算*********************/
  if(Controler->Integrate_Separation_Flag==1)//积分分离标志位
  {
    if(ABS(Controler->Err_LPF)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki
        *Controler->Ki
          *Controler->Err_LPF*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki
      *Controler->Ki
        *Controler->Err_LPF*controller_dt;
  }
  /*******积分限幅*********************/
  if(Controler->Integrate_Limit_Flag==1)//积分限制幅度标志
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******总输出计算*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//输出值递推
  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err_LPF//比例
    +Controler->Integrate//积分
      +Controler->Kd*Controler->Dis_Err_LPF;//已对偏差低通，此处不再对微分项单独低通
  /*******总输出限幅*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******返回总输出*********************/
  return Controler->Control_OutPut;
}


float PID_Control_SDK_Err_LPF(PID_Controler *Controler,uint8_t Differential_Enable_Flag,float period_second)
{
  float controller_dt=0;
	Get_Systime(&Controler->Systime_t);
	controller_dt=Controler->Systime_t.period/1000.0000f;
	if(controller_dt>1.05f*period_second||controller_dt<0.95f*period_second||isnan(controller_dt)!=0)   controller_dt=period_second;
	if(controller_dt<0.0001f) return 0;
	
  /*******偏差计算*********************/
  Controler->Last_Err=Controler->Err;//保存上次偏差
  Controler->Err=Controler->Expect-Controler->FeedBack;//期望减去反馈得到偏差
  
	Controler->Dis_Err=Controler->Err-Controler->Last_Err;//原始微分
  
  Controler->Last_Err_LPF=Controler->Err_LPF;
  Controler->Err_LPF=Control_Device_LPF(Controler->Err,
                                        &Controler->Control_Device_LPF_Buffer,
                                        &Control_Device_SDK_Err_LPF_Parameter);//巴特沃斯低通后得到的微分项,20hz
  if(Differential_Enable_Flag==0)
	{
		Controler->Control_Device_LPF_Buffer.Input_Butter[0]=0;
		Controler->Control_Device_LPF_Buffer.Input_Butter[1]=0;
		Controler->Control_Device_LPF_Buffer.Input_Butter[2]=0;
		Controler->Control_Device_LPF_Buffer.Output_Butter[0]=0;
		Controler->Control_Device_LPF_Buffer.Output_Butter[1]=0;
		Controler->Control_Device_LPF_Buffer.Output_Butter[2]=0;
		Controler->Last_Err_LPF=0;
		Controler->Err_LPF=0;
	}  
  Controler->Dis_Err_LPF=Controler->Err_LPF-Controler->Last_Err_LPF;//偏差经过低通后的微分量

	
  if(Controler->Err_Limit_Flag==1)//偏差限幅度标志位
  {
    if(Controler->Err_LPF>=Controler->Err_Max)   Controler->Err_LPF= Controler->Err_Max;
    if(Controler->Err_LPF<=-Controler->Err_Max)  Controler->Err_LPF=-Controler->Err_Max;
  }
  /*******积分计算*********************/
  if(Controler->Integrate_Separation_Flag==1)//积分分离标志位
  {
    if(ABS(Controler->Err_LPF)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki
        *Controler->Ki
          *Controler->Err_LPF*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki
      *Controler->Ki
        *Controler->Err_LPF*controller_dt;
  }
  /*******积分限幅*********************/
  if(Controler->Integrate_Limit_Flag==1)//积分限制幅度标志
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******总输出计算*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//输出值递推

  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err_LPF//比例
														+Controler->Integrate//积分	
		+													+Controler->Kd*Controler->Dis_Err_LPF;//已对偏差低通，此处不再对微分项单独低通
  /*******总输出限幅*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******返回总输出*********************/
  return Controler->Control_OutPut;
}

void PID_LPF_Reset(PID_Controler *Controler,Controler_Label Label)
{
  Controler->Control_Device_LPF_Buffer.Input_Butter[0]=0;
  Controler->Control_Device_LPF_Buffer.Input_Butter[1]=0;
  Controler->Control_Device_LPF_Buffer.Input_Butter[2]=0;
  Controler->Control_Device_LPF_Buffer.Output_Butter[0]=0;
  Controler->Control_Device_LPF_Buffer.Output_Butter[1]=0;
  Controler->Control_Device_LPF_Buffer.Output_Butter[2]=0;
  Controler->Last_Err_LPF=0;
  Controler->Err_LPF=0;
}


float Differential_Forward_PID_Control_Div_LPF(PID_Controler *Controler,float period_second)//微分先行PID控制器
{
  int16  i=0;
  float tempa,tempb,tempc,max,min;//用于防跳变滤波
  float controller_dt=0;
	Get_Systime(&Controler->Systime_t);
	controller_dt=Controler->Systime_t.period/1000.0000f;
	if(controller_dt>1.05f*period_second||controller_dt<0.95f*period_second||isnan(controller_dt)!=0)   controller_dt=period_second;
	if(controller_dt<0.0001f) return 0;
	
  /*******偏差计算*********************/
  Controler->Last_Err=Controler->Err;//保存上次偏差
  Controler->Err=Controler->Expect-Controler->FeedBack;//期望减去反馈得到偏差
  Controler->Dis_Err=-(Controler->FeedBack-Controler->Last_FeedBack);//只对反馈信号微分
  Controler->Last_FeedBack=Controler->FeedBack;//记录上次反馈值
  /******************************************/
  //均值滤波，保证得到数据不跳变，避免期望阶跃时，微分输出异常
  tempa=Controler->Pre_Last_Dis_Err_LPF;
  tempb=Controler->Last_Dis_Err_LPF;
  tempc=Controler->Dis_Err;
  max = tempa > tempb ? tempa:tempb;
  max = max > tempc ? max:tempc;
  min = tempa < tempb ? tempa:tempb;
  min = min < tempc ? min:tempc;
  if(tempa > min && tempa < max)    Controler->Dis_Err = tempa;
  if(tempb > min  && tempb < max )  Controler->Dis_Err = tempb;
  if(tempc > min  &&  tempc < max)  Controler->Dis_Err = tempc;
  Controler->Pre_Last_Dis_Err_LPF = Controler->Last_Dis_Err_LPF;
  Controler->Last_Dis_Err_LPF = Controler->Dis_Err;
  /*****************************************/  
  for(i=4;i>0;i--)//数字低通后微分项保存
  {
    Controler->Dis_Error_History[i]=Controler->Dis_Error_History[i-1];
  }
  Controler->Dis_Error_History[0]=Control_Device_LPF(Controler->Dis_Err,
                                                     &Controler->Control_Device_LPF_Buffer,
                                                     &Control_Device_Div_LPF_Parameter);//巴特沃斯低通后得到的微分项,20hz 
  if(Controler->Err_Limit_Flag==1)//偏差限幅度标志位
  {
    if(Controler->Err>=Controler->Err_Max)   Controler->Err= Controler->Err_Max;
    if(Controler->Err<=-Controler->Err_Max)  Controler->Err=-Controler->Err_Max;
  }
  /*******积分计算*********************/
  if(Controler->Integrate_Separation_Flag==1)//积分分离标志位
  {
    if(ABS(Controler->Err)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  /*******积分限幅*********************/
  if(Controler->Integrate_Limit_Flag==1)//积分限制幅度标志
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******总输出计算*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//输出值递推
  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err//比例
    +Controler->Integrate//积分
      //+Controler->Kd*Controler->Dis_Err;//微分
      +Controler->Kd*Controler->Dis_Error_History[0];//微分项来源于巴特沃斯低通滤波器
  /*******总输出限幅*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******返回总输出*********************/
  return Controler->Control_OutPut;
}

float PID_Control_Div_LPF_For_Gyro(PID_Controler *Controler,float period_second)
{
  int16  i=0; 
  float controller_dt=0;
	Get_Systime(&Controler->Systime_t);
	controller_dt=Controler->Systime_t.period/1000.0000f;	
	if(controller_dt>1.05f*period_second||controller_dt<0.95f*period_second||isnan(controller_dt)!=0)   controller_dt=period_second;
	if(controller_dt<0.0001f) return 0;
	
  /*******偏差计算*********************/
  Controler->Pre_Last_Err=Controler->Last_Err;//上上次偏差
  Controler->Last_Err=Controler->Err;//保存上次偏差
  Controler->Err=Controler->Expect-Controler->FeedBack;//期望减去反馈得到偏差
	Controler->Dis_Err=(Controler->Err-Controler->Last_Err);
	Controler->Dis_Err=constrain_float(Controler->Dis_Err,-25.0f,25.0f);
 
  for(i=4;i>0;i--)//数字低通后微分项保存
  {
    Controler->Dis_Error_History[i]=Controler->Dis_Error_History[i-1];
  }
  Controler->Dis_Error_History[0]=Control_Device_LPF(Controler->Dis_Err,
                                                     &Controler->Control_Device_LPF_Buffer,
                                                     &Control_Device_Div_LPF_Parameter_For_Gyro);//巴特沃斯低通后得到的微分项,30hz
  Controler->Dis_Error_History[0]=constrain_float(Controler->Dis_Error_History[0],-500,500);//微分项限幅
  //Controler->Adaptable_Kd=Controler->Kd*(1+ABS(Controler->Dis_Error_History[0])/500.0f);//自适应微分参数
  Controler->Adaptable_Kd=Controler->Kd;
	
	
  if(Controler->Err_Limit_Flag==1)//偏差限幅度标志位
  {
    if(Controler->Err>=Controler->Err_Max)   Controler->Err= Controler->Err_Max;
    if(Controler->Err<=-Controler->Err_Max)  Controler->Err=-Controler->Err_Max;
  }
  /*******积分计算*********************/
  if(Controler->Integrate_Separation_Flag==1)//积分分离标志位
  {
    if(ABS(Controler->Err)<=Controler->Integrate_Separation_Err)
      Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  else
  {
    Controler->Integrate+=Controler->Scale_Ki*Controler->Ki*Controler->Err*controller_dt;
  }
  /*******积分限幅*********************/
  if(Controler->Integrate_Limit_Flag==1)//积分限制幅度标志
  {
    if(Controler->Integrate>=Controler->Integrate_Max)
      Controler->Integrate=Controler->Integrate_Max;
    if(Controler->Integrate<=-Controler->Integrate_Max)
      Controler->Integrate=-Controler->Integrate_Max ;
  }
  /*******总输出计算*********************/
  Controler->Last_Control_OutPut=Controler->Control_OutPut;//输出值递推
  Controler->Control_OutPut=Controler->Scale_Kp*Controler->Kp*Controler->Err//比例
    +Controler->Integrate//积分
      //+Controler->Kd*Controler->Dis_Err;//微分
      //+Controler->Kd*Controler->Dis_Error_History[0];//微分项来源于巴特沃斯低通滤波器
      +Controler->Adaptable_Kd*Controler->Dis_Error_History[0];//微分项来源于巴特沃斯低通滤波器
  /*******总输出限幅*********************/
  if(Controler->Control_OutPut>=Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=Controler->Control_OutPut_Limit;
  if(Controler->Control_OutPut<=-Controler->Control_OutPut_Limit)
    Controler->Control_OutPut=-Controler->Control_OutPut_Limit;
  /*******返回总输出*********************/
  return Controler->Control_OutPut;
}


void  PID_Integrate_Reset(PID_Controler *Controler)  {Controler->Integrate=0.0f;}


void East_North_Ctrl_Reset(void)
{
	PID_Integrate_Reset(&Total_Controller.Latitude_Speed_Control);//清空水平速度控制器积分项
	PID_Integrate_Reset(&Total_Controller.Latitude_Position_Control);//清空水平位置控制器积分项
	PID_Integrate_Reset(&Total_Controller.Longitude_Speed_Control);//清空水平速度控制器积分项
	PID_Integrate_Reset(&Total_Controller.Longitude_Position_Control);//清空水平位置控制器积分项
}



void Take_Off_Reset(void)
{
  PID_Integrate_Reset(&Total_Controller.Roll_Gyro_Control);//起飞前屏蔽积分
  PID_Integrate_Reset(&Total_Controller.Pitch_Gyro_Control);
  PID_Integrate_Reset(&Total_Controller.Yaw_Gyro_Control);
  PID_Integrate_Reset(&Total_Controller.Pitch_Angle_Control);
  PID_Integrate_Reset(&Total_Controller.Roll_Angle_Control);
  PID_Integrate_Reset(&Total_Controller.Yaw_Angle_Control);
  
  PID_Integrate_Reset(&Total_Controller.Longitude_Speed_Control);//位置控制速度环积分项
  PID_Integrate_Reset(&Total_Controller.Latitude_Speed_Control);
}

void Throttle_Control_Reset(void)
{
  PID_Integrate_Reset(&Total_Controller.Height_Acce_Control);
  PID_Integrate_Reset(&Total_Controller.Height_Speed_Control);
  PID_Integrate_Reset(&Total_Controller.Height_Position_Control);
	
	PID_LPF_Reset(&Total_Controller.Height_Acce_Control,Height_Acce_Controler);
  Total_Controller.Height_Acce_Control.Expect=0;//4期望
  Total_Controller.Height_Acce_Control.FeedBack=0;//5反馈值
  Total_Controller.Height_Acce_Control.Err=0;//6偏差
  Total_Controller.Height_Acce_Control.Last_Err=0;//7上次偏差
  Total_Controller.Height_Acce_Control.Integrate=0;//10积分值
  Total_Controller.Height_Acce_Control.Control_OutPut=0;//15控制器总输出
  Total_Controller.Height_Acce_Control.Last_Control_OutPut=0;//16上次控制器总输出
}

#define PID_USE_NUM  14
Vector3f_pid PID_Parameter[PID_USE_NUM]={0};
uint8_t Sort_PID_Flag=0;
/***************************************************
函数名: void Save_Or_Reset_PID_Parameter(void)
说明:	PID参数内部Flash读、写操作
入口:	无
出口:	无
备注:	配合地面站使用
注释者：无名小哥
****************************************************/
void Save_Or_Reset_PID_Parameter()
{
  if(Sort_PID_Flag==1)//将地面站设置PID参数写入Flash
  {
    PID_Parameter[0].p=Total_Controller.Pitch_Gyro_Control.Kp;
    PID_Parameter[0].i=Total_Controller.Pitch_Gyro_Control.Ki;
    PID_Parameter[0].d=Total_Controller.Pitch_Gyro_Control.Kd;
    
    PID_Parameter[1].p=Total_Controller.Roll_Gyro_Control.Kp;
    PID_Parameter[1].i=Total_Controller.Roll_Gyro_Control.Ki;
    PID_Parameter[1].d=Total_Controller.Roll_Gyro_Control.Kd;
    
    PID_Parameter[2].p=Total_Controller.Yaw_Gyro_Control.Kp;
    PID_Parameter[2].i=Total_Controller.Yaw_Gyro_Control.Ki;
    PID_Parameter[2].d=Total_Controller.Yaw_Gyro_Control.Kd;
    
    PID_Parameter[3].p=Total_Controller.Pitch_Angle_Control.Kp;
    PID_Parameter[3].i=Total_Controller.Pitch_Angle_Control.Ki;
    PID_Parameter[3].d=Total_Controller.Pitch_Angle_Control.Kd;
    
    PID_Parameter[4].p=Total_Controller.Roll_Angle_Control.Kp;
    PID_Parameter[4].i=Total_Controller.Roll_Angle_Control.Ki;
    PID_Parameter[4].d=Total_Controller.Roll_Angle_Control.Kd;
    
    PID_Parameter[5].p=Total_Controller.Yaw_Angle_Control.Kp;
    PID_Parameter[5].i=Total_Controller.Yaw_Angle_Control.Ki;
    PID_Parameter[5].d=Total_Controller.Yaw_Angle_Control.Kd;
    
    PID_Parameter[6].p=Total_Controller.Height_Position_Control.Kp;
    PID_Parameter[6].i=Total_Controller.Height_Position_Control.Ki;
    PID_Parameter[6].d=Total_Controller.Height_Position_Control.Kd;
    
    PID_Parameter[7].p=Total_Controller.Height_Speed_Control.Kp;
    PID_Parameter[7].i=Total_Controller.Height_Speed_Control.Ki;
    PID_Parameter[7].d=Total_Controller.Height_Speed_Control.Kd;
    
		PID_Parameter[8].p=Total_Controller.Height_Acce_Control.Kp;
    PID_Parameter[8].i=Total_Controller.Height_Acce_Control.Ki;
    PID_Parameter[8].d=Total_Controller.Height_Acce_Control.Kd;
    
    PID_Parameter[9].p=Total_Controller.Latitude_Speed_Control.Kp;
    PID_Parameter[9].i=Total_Controller.Latitude_Speed_Control.Ki;
    PID_Parameter[9].d=Total_Controller.Latitude_Speed_Control.Kd;
    
    PID_Parameter[10].p=Total_Controller.Latitude_Position_Control.Kp;
    PID_Parameter[10].i=Total_Controller.Latitude_Position_Control.Ki;
    PID_Parameter[10].d=Total_Controller.Latitude_Position_Control.Kd;
       
    PID_Parameter[11].p=Total_Controller.Optical_Position_Control.Kp;
    PID_Parameter[11].i=Total_Controller.Optical_Position_Control.Ki;
    PID_Parameter[11].d=Total_Controller.Optical_Position_Control.Kd;
    
    PID_Parameter[12].p=Total_Controller.Optical_Speed_Control.Kp;
    PID_Parameter[12].i=Total_Controller.Optical_Speed_Control.Ki;
    PID_Parameter[12].d=Total_Controller.Optical_Speed_Control.Kd;
    
    PID_Parameter[13].p=Total_Controller.SDK_Roll_Position_Control.Kp;
    PID_Parameter[13].i=Total_Controller.SDK_Roll_Position_Control.Ki;
    PID_Parameter[13].d=Total_Controller.SDK_Roll_Position_Control.Kd;
    Save_PID_Parameter();
    
    Sort_PID_Flag=0;
  }
  else if(Sort_PID_Flag==2)//将复位PID参数，并写入Flash
  {
    Total_PID_Init();//将PID参数重置为参数Control_Unit表里面参数
    PID_Parameter[0].p=Total_Controller.Pitch_Gyro_Control.Kp;
    PID_Parameter[0].i=Total_Controller.Pitch_Gyro_Control.Ki;
    PID_Parameter[0].d=Total_Controller.Pitch_Gyro_Control.Kd;
    
    PID_Parameter[1].p=Total_Controller.Roll_Gyro_Control.Kp;
    PID_Parameter[1].i=Total_Controller.Roll_Gyro_Control.Ki;
    PID_Parameter[1].d=Total_Controller.Roll_Gyro_Control.Kd;
    
    PID_Parameter[2].p=Total_Controller.Yaw_Gyro_Control.Kp;
    PID_Parameter[2].i=Total_Controller.Yaw_Gyro_Control.Ki;
    PID_Parameter[2].d=Total_Controller.Yaw_Gyro_Control.Kd;
    
    PID_Parameter[3].p=Total_Controller.Pitch_Angle_Control.Kp;
    PID_Parameter[3].i=Total_Controller.Pitch_Angle_Control.Ki;
    PID_Parameter[3].d=Total_Controller.Pitch_Angle_Control.Kd;
    
    PID_Parameter[4].p=Total_Controller.Roll_Angle_Control.Kp;
    PID_Parameter[4].i=Total_Controller.Roll_Angle_Control.Ki;
    PID_Parameter[4].d=Total_Controller.Roll_Angle_Control.Kd;
    
    PID_Parameter[5].p=Total_Controller.Yaw_Angle_Control.Kp;
    PID_Parameter[5].i=Total_Controller.Yaw_Angle_Control.Ki;
    PID_Parameter[5].d=Total_Controller.Yaw_Angle_Control.Kd;
    
    PID_Parameter[6].p=Total_Controller.Height_Position_Control.Kp;
    PID_Parameter[6].i=Total_Controller.Height_Position_Control.Ki;
    PID_Parameter[6].d=Total_Controller.Height_Position_Control.Kd;
    
    PID_Parameter[7].p=Total_Controller.Height_Speed_Control.Kp;
    PID_Parameter[7].i=Total_Controller.Height_Speed_Control.Ki;
    PID_Parameter[7].d=Total_Controller.Height_Speed_Control.Kd;
   
	  PID_Parameter[8].p=Total_Controller.Height_Acce_Control.Kp;
    PID_Parameter[8].i=Total_Controller.Height_Acce_Control.Ki;
    PID_Parameter[8].d=Total_Controller.Height_Acce_Control.Kd;  
		
    PID_Parameter[9].p=Total_Controller.Latitude_Speed_Control.Kp;
    PID_Parameter[9].i=Total_Controller.Latitude_Speed_Control.Ki;
    PID_Parameter[9].d=Total_Controller.Latitude_Speed_Control.Kd;
    
    PID_Parameter[10].p=Total_Controller.Latitude_Position_Control.Kp;
    PID_Parameter[10].i=Total_Controller.Latitude_Position_Control.Ki;
    PID_Parameter[10].d=Total_Controller.Latitude_Position_Control.Kd;
    

    
    PID_Parameter[11].p=Total_Controller.Optical_Position_Control.Kp;
    PID_Parameter[11].i=Total_Controller.Optical_Position_Control.Ki;
    PID_Parameter[11].d=Total_Controller.Optical_Position_Control.Kd;
    
    PID_Parameter[12].p=Total_Controller.Optical_Speed_Control.Kp;
    PID_Parameter[12].i=Total_Controller.Optical_Speed_Control.Ki;
    PID_Parameter[12].d=Total_Controller.Optical_Speed_Control.Kd;
    
    PID_Parameter[13].p=Total_Controller.SDK_Roll_Position_Control.Kp;
    PID_Parameter[13].i=Total_Controller.SDK_Roll_Position_Control.Ki;
    PID_Parameter[13].d=Total_Controller.SDK_Roll_Position_Control.Kd;
    Save_PID_Parameter();
    
    Sort_PID_Flag=0;
    
    NCLink_Send_Ask_Flag[0]=1;//回复默认参数后，将更新的数据发送置地面站
    NCLink_Send_Ask_Flag[1]=1;
    NCLink_Send_Ask_Flag[2]=1;
    NCLink_Send_Ask_Flag[3]=1;
    NCLink_Send_Ask_Flag[4]=1;
    NCLink_Send_Ask_Flag[5]=1;
  }
  else if(Sort_PID_Flag==3)//将复位PID参数，并写入Flash
  {
    Total_PID_Init();//将PID参数重置为参数Control_Unit表里面参数
    PID_Parameter[0].p=Total_Controller.Pitch_Gyro_Control.Kp;
    PID_Parameter[0].i=Total_Controller.Pitch_Gyro_Control.Ki;
    PID_Parameter[0].d=Total_Controller.Pitch_Gyro_Control.Kd;
    
    PID_Parameter[1].p=Total_Controller.Roll_Gyro_Control.Kp;
    PID_Parameter[1].i=Total_Controller.Roll_Gyro_Control.Ki;
    PID_Parameter[1].d=Total_Controller.Roll_Gyro_Control.Kd;
    
    PID_Parameter[2].p=Total_Controller.Yaw_Gyro_Control.Kp;
    PID_Parameter[2].i=Total_Controller.Yaw_Gyro_Control.Ki;
    PID_Parameter[2].d=Total_Controller.Yaw_Gyro_Control.Kd;
    
    PID_Parameter[3].p=Total_Controller.Pitch_Angle_Control.Kp;
    PID_Parameter[3].i=Total_Controller.Pitch_Angle_Control.Ki;
    PID_Parameter[3].d=Total_Controller.Pitch_Angle_Control.Kd;
    
    PID_Parameter[4].p=Total_Controller.Roll_Angle_Control.Kp;
    PID_Parameter[4].i=Total_Controller.Roll_Angle_Control.Ki;
    PID_Parameter[4].d=Total_Controller.Roll_Angle_Control.Kd;
    
    PID_Parameter[5].p=Total_Controller.Yaw_Angle_Control.Kp;
    PID_Parameter[5].i=Total_Controller.Yaw_Angle_Control.Ki;
    PID_Parameter[5].d=Total_Controller.Yaw_Angle_Control.Kd;
    
    PID_Parameter[6].p=Total_Controller.Height_Position_Control.Kp;
    PID_Parameter[6].i=Total_Controller.Height_Position_Control.Ki;
    PID_Parameter[6].d=Total_Controller.Height_Position_Control.Kd;
    
    PID_Parameter[7].p=Total_Controller.Height_Speed_Control.Kp;
    PID_Parameter[7].i=Total_Controller.Height_Speed_Control.Ki;
    PID_Parameter[7].d=Total_Controller.Height_Speed_Control.Kd;
    
		PID_Parameter[8].p=Total_Controller.Height_Acce_Control.Kp;
    PID_Parameter[8].i=Total_Controller.Height_Acce_Control.Ki;
    PID_Parameter[8].d=Total_Controller.Height_Acce_Control.Kd;    
    
		PID_Parameter[9].p=Total_Controller.Latitude_Speed_Control.Kp;
    PID_Parameter[9].i=Total_Controller.Latitude_Speed_Control.Ki;
    PID_Parameter[9].d=Total_Controller.Latitude_Speed_Control.Kd;
    
    PID_Parameter[10].p=Total_Controller.Latitude_Position_Control.Kp;
    PID_Parameter[10].i=Total_Controller.Latitude_Position_Control.Ki;
    PID_Parameter[10].d=Total_Controller.Latitude_Position_Control.Kd;
        
    PID_Parameter[11].p=Total_Controller.Optical_Position_Control.Kp;
    PID_Parameter[11].i=Total_Controller.Optical_Position_Control.Ki;
    PID_Parameter[11].d=Total_Controller.Optical_Position_Control.Kd;
    
    PID_Parameter[12].p=Total_Controller.Optical_Speed_Control.Kp;
    PID_Parameter[12].i=Total_Controller.Optical_Speed_Control.Ki;
    PID_Parameter[12].d=Total_Controller.Optical_Speed_Control.Kd;
    
    PID_Parameter[13].p=Total_Controller.SDK_Roll_Position_Control.Kp;
    PID_Parameter[13].i=Total_Controller.SDK_Roll_Position_Control.Ki;
    PID_Parameter[13].d=Total_Controller.SDK_Roll_Position_Control.Kd;
		
	
    Save_PID_Parameter();
    
    Sort_PID_Flag=0;
    
    NCLink_Send_Ask_Flag[0]=1;//回复默认参数后，将更新的数据发送置地面站
    NCLink_Send_Ask_Flag[1]=1;
    NCLink_Send_Ask_Flag[2]=1;
    NCLink_Send_Ask_Flag[3]=1;
    NCLink_Send_Ask_Flag[4]=1;
    NCLink_Send_Ask_Flag[5]=1;
  }
}


typedef struct
{
  uint8_t No_0xFF[PID_USE_NUM];
}PID_Parameter_Flag;
PID_Parameter_Flag PID_Parameter_Read_Flag;
void PID_Paramter_Init_With_Flash()
{
  uint16_t i=0;
  Total_PID_Init();
  for(i=0;i<PID_USE_NUM;i++)
  {
    ReadFlashParameterThree(PID1_PARAMETER_KP+3*i,
                            &PID_Parameter[i].p,
                            &PID_Parameter[i].i,
                            &PID_Parameter[i].d);
    if(isnan(PID_Parameter[i].p)==0
       &&isnan(PID_Parameter[i].i)==0
         &&isnan(PID_Parameter[i].d)==0)
      PID_Parameter_Read_Flag.No_0xFF[i]=0x01;
  }
  if(PID_Parameter_Read_Flag.No_0xFF[0]!=0x00
     &&PID_Parameter_Read_Flag.No_0xFF[1]!=0x00
       &&PID_Parameter_Read_Flag.No_0xFF[2]!=0x00
         &&PID_Parameter_Read_Flag.No_0xFF[3]!=0x00
           &&PID_Parameter_Read_Flag.No_0xFF[4]!=0x00
             &&PID_Parameter_Read_Flag.No_0xFF[5]!=0x00
               &&PID_Parameter_Read_Flag.No_0xFF[6]!=0x00
                 &&PID_Parameter_Read_Flag.No_0xFF[7]!=0x00
                   &&PID_Parameter_Read_Flag.No_0xFF[8]!=0x00
                     &&PID_Parameter_Read_Flag.No_0xFF[9]!=0x00
                       &&PID_Parameter_Read_Flag.No_0xFF[10]!=0x00)//Flash内数据正常，更新PID参数值
  {
    Total_Controller.Pitch_Gyro_Control.Kp=PID_Parameter[0].p;
    Total_Controller.Pitch_Gyro_Control.Ki=PID_Parameter[0].i;
    Total_Controller.Pitch_Gyro_Control.Kd=PID_Parameter[0].d;
    
    Total_Controller.Roll_Gyro_Control.Kp=PID_Parameter[1].p;
    Total_Controller.Roll_Gyro_Control.Ki=PID_Parameter[1].i;
    Total_Controller.Roll_Gyro_Control.Kd=PID_Parameter[1].d;
    
    Total_Controller.Yaw_Gyro_Control.Kp=PID_Parameter[2].p;
    Total_Controller.Yaw_Gyro_Control.Ki=PID_Parameter[2].i;
    Total_Controller.Yaw_Gyro_Control.Kd=PID_Parameter[2].d;
    
    Total_Controller.Pitch_Angle_Control.Kp=PID_Parameter[3].p;
    Total_Controller.Pitch_Angle_Control.Ki=PID_Parameter[3].i;
    Total_Controller.Pitch_Angle_Control.Kd=PID_Parameter[3].d;
    
    Total_Controller.Roll_Angle_Control.Kp=PID_Parameter[4].p;
    Total_Controller.Roll_Angle_Control.Ki=PID_Parameter[4].i;
    Total_Controller.Roll_Angle_Control.Kd=PID_Parameter[4].d;
    
    Total_Controller.Yaw_Angle_Control.Kp=PID_Parameter[5].p;
    Total_Controller.Yaw_Angle_Control.Ki=PID_Parameter[5].i;
    Total_Controller.Yaw_Angle_Control.Kd=PID_Parameter[5].d;
    
    Total_Controller.Height_Position_Control.Kp=PID_Parameter[6].p;
    Total_Controller.Height_Position_Control.Ki=PID_Parameter[6].i;
    Total_Controller.Height_Position_Control.Kd=PID_Parameter[6].d;
    
    Total_Controller.Height_Speed_Control.Kp=PID_Parameter[7].p;
    Total_Controller.Height_Speed_Control.Ki=PID_Parameter[7].i;
    Total_Controller.Height_Speed_Control.Kd=PID_Parameter[7].d;
				
    Total_Controller.Height_Acce_Control.Kp=PID_Parameter[8].p;
    Total_Controller.Height_Acce_Control.Ki=PID_Parameter[8].i;
    Total_Controller.Height_Acce_Control.Kd=PID_Parameter[8].d;		
    
    Total_Controller.Latitude_Speed_Control.Kp=PID_Parameter[9].p;
    Total_Controller.Latitude_Speed_Control.Ki=PID_Parameter[9].i;
    Total_Controller.Latitude_Speed_Control.Kd=PID_Parameter[9].d;
    
    Total_Controller.Latitude_Position_Control.Kp=PID_Parameter[10].p;
    Total_Controller.Latitude_Position_Control.Ki=PID_Parameter[10].i;
    Total_Controller.Latitude_Position_Control.Kd=PID_Parameter[10].d;
    /***********************位置控制：位置、速度参数共用一组PID参数**********************************************************/
    Total_Controller.Longitude_Speed_Control.Kp=PID_Parameter[9].p;
    Total_Controller.Longitude_Speed_Control.Ki=PID_Parameter[9].i;
    Total_Controller.Longitude_Speed_Control.Kd=PID_Parameter[9].d;
    
    Total_Controller.Longitude_Position_Control.Kp=PID_Parameter[10].p;
    Total_Controller.Longitude_Position_Control.Ki=PID_Parameter[10].i;
    Total_Controller.Longitude_Position_Control.Kd=PID_Parameter[10].d;
		
		   
    Total_Controller.Optical_Position_Control.Kp=PID_Parameter[11].p;
    Total_Controller.Optical_Position_Control.Ki=PID_Parameter[11].i;
    Total_Controller.Optical_Position_Control.Kd=PID_Parameter[11].d;
    
    Total_Controller.Optical_Speed_Control.Kp=PID_Parameter[12].p;
    Total_Controller.Optical_Speed_Control.Ki=PID_Parameter[12].i;
    Total_Controller.Optical_Speed_Control.Kd=PID_Parameter[12].d;
    
    Total_Controller.SDK_Roll_Position_Control.Kp=PID_Parameter[13].p;
    Total_Controller.SDK_Roll_Position_Control.Ki=PID_Parameter[13].i;
    Total_Controller.SDK_Roll_Position_Control.Kd=PID_Parameter[13].d;
    
    Total_Controller.SDK_Pitch_Position_Control.Kp=PID_Parameter[13].p;
    Total_Controller.SDK_Pitch_Position_Control.Ki=PID_Parameter[13].i;
    Total_Controller.SDK_Pitch_Position_Control.Kd=PID_Parameter[13].d;
  }
}

void Save_PID_Parameter(void)
{
  int16_t i=0;
  for(i=0;i<PID_USE_NUM;i=i+1)
  {
    WriteFlashParameter_Three(PID1_PARAMETER_KP+i*3,
                              PID_Parameter[i].p,
                              PID_Parameter[i].i,
                              PID_Parameter[i].d,
                              &Flight_Params);
  }
}














//	/******************************************/
//  //均值滤波，保证得到数据不跳变，避免期望阶跃时，微分输出异常
//	float tempa,tempb,tempc,max,min;//用于防跳变滤波  
//	tempa=Controler->Pre_Last_Dis_Err_LPF;
//  tempb=Controler->Last_Dis_Err_LPF;
//  tempc=Controler->Dis_Err;
//  max = tempa > tempb ? tempa:tempb;
//  max = max > tempc ? max:tempc;
//  min = tempa < tempb ? tempa:tempb;
//  min = min < tempc ? min:tempc;
//  if(tempa > min && tempa < max)    Controler->Dis_Err = tempa;
//  if(tempb > min  && tempb < max )  Controler->Dis_Err = tempb;
//  if(tempc > min  &&  tempc < max)  Controler->Dis_Err = tempc;
//  Controler->Pre_Last_Dis_Err_LPF = Controler->Last_Dis_Err_LPF;
//  Controler->Last_Dis_Err_LPF = Controler->Dis_Err;
//  /*****************************************/








