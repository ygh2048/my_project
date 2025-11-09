/**
 ******************************************************************************
 * @file    assert.c
 * @author  Smart Lab System
 * @version V1.0
 * @date    2025-11-07
 * @brief   Assert parameter implementation for STM32F10x standard library
 ******************************************************************************
 */

#include <stdio.h>
#include <stdint.h>

/**
 * @brief  assert_param macro function - empty implementation to avoid linker errors
 *         This is called by the STM32F10x standard library when parameter validation fails
 * @param  expr: condition expression (used for validation)
 * @retval None
 */
void assert_param(int expr)
{
    /* User can add his own implementation to report errors */
    /* For now, this is a no-op function to allow compilation */
}

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    
    /* Infinite loop */
    while (1)
    {
    }
}
