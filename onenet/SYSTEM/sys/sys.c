#include "sys.h"
#include "stm32f10x.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//系统中断分组设置化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/10
//版本：V1.4
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************  

/* System Core Clock Variable - Required by STM32 Standard Library */
uint32_t SystemCoreClock = 72000000;  /* 72MHz default */

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}

//关闭所有中断
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}

//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}

//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}

/**
 * @brief Initialize STM32 system clock
 * @param pll: PLL multiplier (e.g., 9 for 72MHz)
 * @retval None
 */
void sys_stm32_clock_init(u8 pll)
{
    RCC_DeInit();                               /* RCC system reset */
    RCC_HSEConfig(RCC_HSE_ON);                  /* Enable HSE */
    
    if (RCC_WaitForHSEStartUp() == SUCCESS)
    {
        RCC_HCLKConfig(RCC_SYSCLK_Div1);       /* HCLK = SYSCLK */
        RCC_PCLK2Config(RCC_HCLK_Div1);         /* PCLK2 = HCLK */
        RCC_PCLK1Config(RCC_HCLK_Div2);         /* PCLK1 = HCLK/2 */
        
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, pll);  /* Config PLL */
        RCC_PLLCmd(ENABLE);                     /* Enable PLL */
        
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);  /* Wait PLL ready */
        
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);  /* PLL as system clock */
        
        while (RCC_GetSYSCLKSource() != 0x08);  /* Wait confirmed */
    }
}

/**
 * @brief Interrupt control: disable all interrupts
 * @param None
 * @retval None
 */
void sys_intx_disable(void)
{
    INTX_DISABLE();
}

/**
 * @brief Interrupt control: enable all interrupts
 * @param None
 * @retval None
 */
void sys_intx_enable(void)
{
    INTX_ENABLE();
}
