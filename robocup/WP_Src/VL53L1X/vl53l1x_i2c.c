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
#include "VL53L1X_i2c.h"

void VL53L1_I2C_Init(void)
{
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	
  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1);
  GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTE_BASE,GPIO_PIN_1,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD);
	GPIOPadConfigSet(GPIO_PORTE_BASE,GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD);
	
  Delay_Ms(10);
  GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_PIN_1);//设置高
  GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_PIN_0);//设置高
}

void VL53L1_SDA_OUT(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTE_BASE,GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD);///GPIO_PIN_TYPE_STD);
}

void VL53L1_SDA_IN(void)
{
  GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTE_BASE,GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}

#define STATUS_OK       0x00
#define STATUS_FAIL     0x01
#define VL_SDA_IN()  VL53L1_SDA_IN()
#define VL_SDA_OUT() VL53L1_SDA_OUT()
#define VL_IIC_SCL_H GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_1,GPIO_PIN_1)//SCL  
#define VL_IIC_SCL_L GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_1,0)//SCL
#define VL_IIC_SDA_H GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,GPIO_PIN_0)//SDA 
#define VL_IIC_SDA_L GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0,0)//SDA 
//#define VL_READ_SDA  GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_0)
#define VL_READ_SDA  (*((volatile uint32_t *)(GPIO_PORTE_BASE + (GPIO_O_DATA + (GPIO_PIN_0 << 2)))))


void DelayUs(uint16_t n)		//need tested arrcoding to system clock, for 30Mhz a slight slow 
{
	uint16_t i;
	while(n)
	{
		i=2;
		while(i)
			i--;
		n--;
	}
}


void VL_IIC_Start(void)
{
	VL_SDA_OUT();//sda线输出
	VL_IIC_SDA_H;	  	  
	VL_IIC_SCL_H;
	//DelayUs(4);
 	VL_IIC_SDA_L;//START:when CLK is high,DATA change form high to low 
	//DelayUs(4);
	VL_IIC_SCL_L;//钳住I2C总线，准备发送或接收数据 
}

//产生IIC停止信号
void VL_IIC_Stop(void)
{
	VL_SDA_OUT();//sda线输出
	VL_IIC_SCL_L;
	VL_IIC_SDA_L;//STOP:when CLK is high DATA change form low to high
 	//DelayUs(4);
	VL_IIC_SCL_H; 
	VL_IIC_SDA_H;//发送I2C总线结束信号
	//DelayUs(4);							   	
}

u8 VL_IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	VL_IIC_SDA_H;
	DelayUs(1);	   
	VL_IIC_SCL_H;
	DelayUs(1);
  VL_SDA_IN();  //SDA设置为输入 	 
	while(VL_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			VL_IIC_Stop();
			return 1;
		}
	}
	VL_IIC_SCL_L;//时钟输出0 	   
	return 0;  
}

//产生ACK应答
void VL_IIC_Ack(void)
{
	VL_IIC_SCL_L;
	VL_SDA_OUT();
	VL_IIC_SDA_L;
	DelayUs(1);//DelayUs(2);
	VL_IIC_SCL_H;
	DelayUs(1);//DelayUs(2);
	VL_IIC_SCL_L;
}

//不产生ACK应答		    
void VL_IIC_NAck(void)
{
	VL_IIC_SCL_L;
	VL_SDA_OUT();
	VL_IIC_SDA_H;
	DelayUs(1);//DelayUs(2);
	VL_IIC_SCL_H;
	DelayUs(1);//DelayUs(2);
	VL_IIC_SCL_L;
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void VL_IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	  VL_SDA_OUT(); 	    
    VL_IIC_SCL_L;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
			if((txd&0x80)>>7)	VL_IIC_SDA_H;
			else	VL_IIC_SDA_L;
			txd<<=1; 	  
			DelayUs(1);//DelayUs(2);  
			VL_IIC_SCL_H;
			DelayUs(1);//DelayUs(2); 
			VL_IIC_SCL_L;	
			DelayUs(1);//DelayUs(2);
    }	 
} 

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 VL_IIC_Read_Byte(void)
{
	unsigned char i,receive=0;
	VL_IIC_SDA_H;
	VL_SDA_IN();//SDA设置为输入
	//DelayUs(4);
	for(i=0;i<8;i++)
	{
		receive<<=1;
		VL_IIC_SCL_L; 
		DelayUs(1);//DelayUs(4);
	  VL_IIC_SCL_H;
		DelayUs(1);//DelayUs(4);
		if(VL_READ_SDA)
			receive |= 0x01;   
	  DelayUs(1);//DelayUs(4);
	}	
  VL_IIC_SCL_L;	
	return receive;
}

//IIC写一个字节数据
u8 VL_IIC_Write_1Byte(u8 SlaveAddress, u16 REG_Address,u8 REG_data)
{
	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress);
	if(VL_IIC_Wait_Ack())
	{
		VL_IIC_Stop();//释放总线
		return 1;//没应答则退出
	}
	VL_IIC_Send_Byte(REG_Address >> 8);
	VL_IIC_Wait_Ack();	
	VL_IIC_Send_Byte(REG_Address & 0x00ff);
	VL_IIC_Wait_Ack();
	//DelayUs(500);
	VL_IIC_Send_Byte(REG_data);
	VL_IIC_Wait_Ack();	
	VL_IIC_Stop();

	return 0;
}

//IIC读一个字节数据
u8 VL_IIC_Read_1Byte(u8 SlaveAddress, u16 REG_Address,u8 *REG_data)
{
	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress);//发写命令
	if(VL_IIC_Wait_Ack())
	{
		 VL_IIC_Stop();//释放总线
		 return 1;//没应答则退出
	}		
	VL_IIC_Send_Byte(REG_Address >> 8);
	VL_IIC_Wait_Ack();
	VL_IIC_Send_Byte((REG_Address & 0x00ff));
	VL_IIC_Wait_Ack();
	//DelayUs(500);
	VL_IIC_Start(); 
	VL_IIC_Send_Byte(SlaveAddress|0x01);//发读命令
	VL_IIC_Wait_Ack();
	*REG_data = VL_IIC_Read_Byte();
	VL_IIC_Stop();

	return 0;
}

//I2C read n bytes
uint8_t VL_I2C_Read_nByte(uint8_t SlaveAddress, uint16_t REG_Address, uint8_t *buf, uint16_t len)
{
	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress);//发写命令
	if(VL_IIC_Wait_Ack()) 
	{
		VL_IIC_Stop();//释放总线
		return 1;//没应答则退出
	}
	VL_IIC_Send_Byte(REG_Address >> 8);
	VL_IIC_Wait_Ack();
	VL_IIC_Send_Byte((REG_Address & 0x00ff));
	VL_IIC_Wait_Ack();
	//DelayUs(500);
	VL_IIC_Start(); 
	VL_IIC_Send_Byte(SlaveAddress|0x01);//发读命令
	VL_IIC_Wait_Ack();
	while(len)
	{
		*buf = VL_IIC_Read_Byte();
		if(1 == len)
		{
			VL_IIC_NAck();
		}
		else
		{
			VL_IIC_Ack();
		}
		buf++;
		len--;
	}
	VL_IIC_Stop();
	return STATUS_OK;
}

//I2C write n bytes
uint8_t VL_I2C_Write_nByte(uint8_t SlaveAddress, uint16_t REG_Address, uint8_t *buf, uint16_t len)
{
	VL_IIC_Start();
	VL_IIC_Send_Byte(SlaveAddress);//发写命令
	if(VL_IIC_Wait_Ack()) 
	{
		VL_IIC_Stop();//释放总线
		return 1;//没应答则退出
	}
	VL_IIC_Send_Byte(REG_Address >> 8);
	VL_IIC_Wait_Ack();
	VL_IIC_Send_Byte(REG_Address & 0x00ff);
	VL_IIC_Wait_Ack();
	while(len--)
	{
		VL_IIC_Send_Byte(*buf++);
		VL_IIC_Wait_Ack();
	}
	VL_IIC_Stop();
	return STATUS_OK;
}
//VL53L1X wrtie multi data
//address: device addr
//index: reg addr
//pdata
//count
uint8_t VL53L1_write_multi(uint8_t address, uint16_t index, uint8_t *pdata, uint16_t count)
{
	uint8_t status = STATUS_OK;
	
	if(VL_I2C_Write_nByte(address,index,pdata,count))
	   status  = STATUS_FAIL;

	return status;
}


//VL53L1X read multi data
//address: dev addr
//index: reg addr
//pdata
//count
uint8_t VL53L1_read_multi(uint8_t address,uint16_t index,uint8_t *pdata,uint16_t count)
{
	uint8_t status = STATUS_OK;

	if(VL_I2C_Read_nByte(address,index,pdata,count))
	  status  = STATUS_FAIL;

	return status;
}

//VL53L1X write 1 byte
//address: dev addr
//index: reg addr
//data (8 bits)
uint8_t VL53L1_write_byte(uint8_t address,uint16_t index,uint8_t data)
{
	uint8_t status = STATUS_OK;

	status = VL53L1_write_multi(address,index,&data,1);

	return status;
}

//VL53L1X write 1 word (2 bytes)
//address: dev addr
//index: reg addr
//data (16 bits)
uint8_t VL53L1_write_word(uint8_t address, uint16_t index, uint16_t data)
{
	uint8_t status = STATUS_OK;
	
	uint8_t buf[2];
	
	buf[0] = (uint8_t)(data >> 8);	//H
	buf[1] = (uint8_t)(data & 0xff);	//L
	
	if(index % 2 == 1)
	{
		status = VL53L1_write_multi(address,index,&buf[0],1);
		status = VL53L1_write_multi(address,index,&buf[0],1);
	}
	status = VL53L1_write_multi(address,index,buf,2);
	
	return status;
}

//VL53L1X a dword(4 bytes)
//address: dev addr
//index: reg addr
//data (32 bits)
uint8_t VL53L1_write_dword(uint8_t address, uint16_t index, uint32_t data)
{
  uint8_t status = STATUS_OK;

  uint8_t buf[4];	
	
	buf[0] = (uint8_t)(data >> 24);
	buf[1] = (uint8_t)((data & 0xff0000) >> 16);
	buf[2] = (uint8_t)((data & 0xff00) >> 8);
	buf[3] = (uint8_t)(data & 0xff);
	
	status = VL53L1_write_multi(address,index,buf,4);
	
	return status;
}


//VL53L1X read 1 byte
//address: dev addr
//index: reg addr
//data (8 bits)
uint8_t VL53L1_read_byte(uint8_t address, uint16_t index, uint8_t *pdata)
{
	uint8_t status = STATUS_OK;
	 
	status = VL53L1_read_multi(address, index, pdata, 1);
	return status;
}

//VL53L1X read a word
//address: dev addr
//index: reg addr
//data (16 bits)
uint8_t VL53L1_read_word(uint8_t address, uint16_t index, uint16_t *pdata)
{
	uint8_t status = STATUS_OK;
	uint8_t buf[2];
	
	status = VL53L1_read_multi(address, index, buf, 2);
	*pdata = ((uint16_t)buf[0] << 8)+(uint16_t)buf[1];
	
	return status;
}

//VL53L1X read a dword
//address: dev addr
//index: reg addr
//data (32 bits)
uint8_t VL53L1_read_dword(uint8_t address, uint16_t index, uint32_t *pdata)
{
	uint8_t status = STATUS_OK;
	
	uint8_t buf[4];
	
	status = VL53L1_read_multi(address,index,buf,4);
	
	*pdata = ((uint32_t)buf[0] << 24)+((uint32_t)buf[1] << 16)+((uint32_t)buf[2] << 8)+((uint32_t)buf[3]);
	
	return status;
}



