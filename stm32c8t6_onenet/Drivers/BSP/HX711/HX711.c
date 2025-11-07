/**
 ****************************************************************************************************
 * @file        HX711.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       HX711压力传感器驱动 实现文件
 * @note        适配STM32F103C8T6
 *              引脚: SCK->PB3, DOUT->PB4
 *              参考: 成功的HX711项目(STM32F103_TEST_HX711)
 *              采用STM32F10x标准库函数，不使用寄存器直接操作
 ****************************************************************************************************
 */

#include "./BSP/HX711/HX711.h"
#include "./SYSTEM/delay/delay.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include <stdint.h>
#include <stdio.h>

/* HX711校准参数定义 */
#define HX711_GAP_VALUE     423.0f      /* 校准系数 - 根据实际称重调整 */

/* 全局变量定义 */
uint32_t hx711_buffer = 0;              /* HX711原始数据缓冲 */
uint32_t hx711_maopi = 0;               /* 毛皮(零点)值 */
int32_t hx711_weight = 0;               /* 实际重量(克) */
uint8_t hx711_error_flag = 0;           /* 错误标志 */

/**
 * @brief       HX711初始化函数
 * @param       无
 * @retval      无
 * @note        完全使用STM32F10x标准库函数，不使用寄存器直接操作
 */
void hx711_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 使能GPIOB/AFIO时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    /* PB3/PB4默认被JTAG占用，关闭JTAG仅保留SWD，否则HX711信号无法驱动 */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    /* 配置PB3为推挽输出，50MHz (HX711_SCK) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 配置PB4为上拉输入 (HX711_DOUT) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* SCK初始为低 */
    GPIO_ResetBits(GPIOB, GPIO_Pin_3);
    
    delay_ms(100);  /* 等待HX711启动 */
    
    /* 获取零点(去皮值) */
    hx711_get_maopi();
    
    printf("[HX711] Init: Zero Point = 0x%08lX (%ld)\r\n", hx711_maopi, hx711_maopi);
}

/**
 * @brief       读取HX711原始数据
 * @param       无
 * @retval      HX711原始数据(24位)
 * @note        增益128倍，参考: STM32F103_TEST_HX711项目
 */
uint32_t hx711_read(void)
{
    uint32_t count = 0;
    uint8_t i;
    uint32_t timeout;

    /* 等待DOUT变为高电平(空闲状态) */
    timeout = 1000000;  /* 增加超时时间 */
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == Bit_RESET && timeout--)
    {
        delay_us(1);
    }
    if (timeout == 0)
    {
        printf("[HX711] ERROR: DOUT stuck LOW!\r\n");
        hx711_error_flag = 1;
        return 0;
    }

    /* 拉低SCK开始转换 */
    GPIO_ResetBits(GPIOB, GPIO_Pin_3);
    delay_us(1);

    /* 等待DOUT变为低电平(数据就绪) */
    timeout = 1000000;
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == Bit_SET && timeout--)
    {
        delay_us(1);
    }
    if (timeout == 0)
    {
        printf("[HX711] ERROR: Conversion timeout!\r\n");
        hx711_error_flag = 1;
        return 0;
    }
    
    /* 读取24位数据 */
    for (i = 0; i < 24; i++)
    {
        count = count << 1;            /* 先左移一位 */
        
        /* 产生上升沿 */
        GPIO_SetBits(GPIOB, GPIO_Pin_3);        /* SCK拉高 */
        delay_us(2);                   /* 确保稳定 */
        
        /* 读取数据位 */
        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) == Bit_SET)     /* DOUT为1 */
        {
            count++;
        }
        
        /* 产生下降沿 */
        GPIO_ResetBits(GPIOB, GPIO_Pin_3); /* SCK拉低 */
        delay_us(2);                   /* 确保稳定 */
    }
    
    /* 第25个时钟(设置增益128) */
    GPIO_SetBits(GPIOB, GPIO_Pin_3);            /* SCK拉高 */
    delay_us(2);
    count = count ^ 0x800000;         /* 按照数据手册要求取反 */
    GPIO_ResetBits(GPIOB, GPIO_Pin_3);     /* SCK拉低 */
    delay_us(2);
    
    hx711_error_flag = 0;  /* 清除错误标志 */
    return count;
}

/**
 * @brief       获取毛皮(去皮)
 * @param       无
 * @retval      无
 * @note        在空载时调用,获取零点值
 */
void hx711_get_maopi(void)
{
    hx711_maopi = hx711_read();
}

/**
 * @brief       获取实际重量
 * @param       无
 * @retval      无
 * @note        计算结果存储在全局变量hx711_weight中
 */
void hx711_get_weight(void)
{
    hx711_buffer = hx711_read();
    
    if (hx711_buffer > hx711_maopi)
    {
        /* 先赋值完整的读取值 */
        hx711_weight = hx711_buffer;
        
        /* 减去零点值得到净重 */
        hx711_weight = hx711_weight - hx711_maopi;
        
        /* 转换为实际重量(克) - 注意这里必须先转float再计算 */
        hx711_weight = (int32_t)((float)hx711_weight / HX711_GAP_VALUE);
    }
    else
    {
        hx711_weight = 0;               /* 低于零点时置零 */
    }
}
