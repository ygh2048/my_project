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
#include "Developer_Mode.h"





uint8_t SDK1_Mode_Setup=0x00;
uint8_t SDK2_Mode_Setup=0x00;
void Auto_Flight_Ctrl(uint8_t *mode)
{
	static uint16_t openmv_work_mode=0;
	switch(*mode)
	{
    case 0://用户事先指定的SDK开发者模式
		{
			NCQ_SDK_Run();//水内含平控制+高度控制			
			Flight.yaw_ctrl_mode=ROTATE;
			Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		}
		break;		
    case 1://俯视OPENMV视觉追踪色块
		{
			Color_Block_Control_Pilot();//俯视OPENMV视觉水平追踪		
			Flight.yaw_ctrl_mode=ROTATE;
			Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];			
			Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		}
		break;
    case 2://俯视OPENMV视觉追踪AprilTag，控制逻辑与追踪色块一致
		{
			Top_APrilTag_Control_Pilot();//俯视OPENMV视觉水平追踪		
			Flight.yaw_ctrl_mode=ROTATE;
			Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];			
			Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		}
		break;		
    case 3://俯视OPENMV循迹控制，默认黑线，阈值可在openmv程序中调整
		{
			Self_Track_Control_Pilot();//循迹控制内含有水平控制+偏航控制
			Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制			
		}
		break;
    case 4://前视OPENMV视觉追踪	
		{
			Front_AprilTag_Control_Pilot();//前视OPENMV视觉追踪
			
			Flight.yaw_ctrl_mode=ROTATE;
			Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
			Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		}
		break;		
    case 5:
		{
			Front_Surround_Pole_Control_Pilot();
			Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		}
		break;
    case 6:
		{
//			flight_subtask_1();//顺时针转动90度，完成后降落
			robocup_fly(5,10,15,20);
		}
		break;
    case 7:
		{
//			flight_subtask_3();//以10deg/s的角速度顺时针转动10000ms，完成后降落
			if(Auto_Takeoff(Target_Height)==1)//WORK_HEIGHT_CM
			{
				*mode-=1;//到达目标高度后，切换到下一SDK任务
			}	
		}
		break;
    case 8:
		{
		  flight_subtask_5();//机体坐标系下相对位移,正方形轨迹
		}
		break;
    case 9:
		{
			flight_subtask_7();//导航坐标系下，基于初始点的绝对坐标位移,正方形轨迹
		}
		break;
    case 10:
		{
			flight_subtask_8();//航点飞行轨迹圆，半径、航点数量可设置
		}
		break;
    case 11://自动起飞到某一高度
		{
			if(Auto_Takeoff(Target_Height)==1)//WORK_HEIGHT_CM
			{
				*mode+=1;//到达目标高度后，切换到下一SDK任务
				//*mode+=4;//到达目标高度后，切换2022年7月省赛第一部分任务
				//*mode+=5;//到达目标高度后，切换2022年7月省赛第二部分任务
				//*mode+=6;//到达目标高度后，切换2022年7月省赛第三部分任务
			}
		}
		break;
    case 12:
		{
			if(openmv_work_mode==0)//只配置一次
			{
				openmv_work_mode=0x07;
				SDK_DT_Send_Check(openmv_work_mode,UART3_SDK);//起飞完毕之后，将底部OPENMV设置成检测农作物模式
			}
			//2021年电子设计竞赛G题植保无人机
		  Agriculture_UAV_Closeloop();//基础部分
			//Agriculture_UAV_Innovation();//发挥部分
		}
		break;
    case 13:
		{
			ros_flight_support();
		}
		break;
    case 14:
		{
			basic_auto_flight_support();
		}
		break;
		case 15:
		{
			if(openmv_work_mode==0)//只配置一次
			{
				openmv_work_mode=0x10;
				SDK_DT_Send_Check(openmv_work_mode,UART3_SDK);//起飞完毕之后，将底部OPENMV设置成色块、形状检测模式
			}
			//2022年月电子设计竞赛B题送货无人机——第1部分
			Deliver_UAV_Basic();
		}
		break;
		case 16:
		{
			if(openmv_work_mode==0)//只配置一次
			{
				openmv_work_mode=0x10;
				SDK_DT_Send_Check(openmv_work_mode,UART3_SDK);//起飞完毕之后，将底部OPENMV设置成色块、形状检测模式
			}
			//2022年月电子设计竞赛B题送货无人机——第2部分
			Deliver_UAV_Innovation();
		}
		break;		
		case 17:
		{
			//2022年月电子设计竞赛B题送货无人机——第3部分
			Deliver_UAV_Hulahoop();
		}
		break;
		case 18:
		{
			//用户预留任务，编写后注意加上break跳出
			try_uart();
		}		
		case 19:
		{
			//用户预留任务，编写后注意加上break跳出
		}
		case 20:
		{
			//用户预留任务，编写后注意加上break跳出
		}		
		case 21:
		{
			//用户预留任务，编写后注意加上break跳出
		}
		case 22:
		{
			//用户预留任务，编写后注意加上break跳出
		}		
		case 23:
		{
			//用户预留任务，编写后注意加上break跳出
		}
		case 24:
		{
			//用户预留任务，编写后注意加上break跳出
		}		
		case 25:
		{
			//用户预留任务，编写后注意加上break跳出
		}
		case 26:
		{
			//用户预留任务，编写后注意加上break跳出
		}		
		case 27:
		{
			//用户预留任务，编写后注意加上break跳出
		}
		case 28:
		{
			//用户预留任务，编写后注意加上break跳出
		}	
		case 30:
		{
			//用户预留任务，编写后注意加上break跳出
		}			
    case 31://前面预留case不满足情况下执行此情形
		{
			Flight.roll_outer_control_output =RC_Data.rc_rpyt[RC_ROLL];
			Flight.pitch_outer_control_output=RC_Data.rc_rpyt[RC_PITCH];			
			Flight.yaw_ctrl_mode=ROTATE;
			Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
			Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		}
		break;
		case 32://SDK模式中原地降落至地面怠速后停桨,用于任务执行完成后降落
		{
			OpticalFlow_Control(0);		
			Flight.yaw_ctrl_mode=ROTATE;
			Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
		  Flight_Alt_Hold_Control(ALTHOLD_AUTO_VEL_CTRL,NUL,-50);//高度控制
		}
    break;		
	  default:
		{
			Flight.roll_outer_control_output =RC_Data.rc_rpyt[RC_ROLL];
			Flight.pitch_outer_control_output=RC_Data.rc_rpyt[RC_PITCH];	
			Flight.yaw_ctrl_mode=ROTATE;
			Flight.yaw_outer_control_output  =RC_Data.rc_rpyt[RC_YAW];
			Flight_Alt_Hold_Control(ALTHOLD_MANUAL_CTRL,NUL,NUL);//高度控制
		}
	}
}


