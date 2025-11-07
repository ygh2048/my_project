/**
 ****************************************************************************************************
 * @file        key.c
 * @author      Smart Lab System
 * @version     V1.0
 * @date        2025-11-04
 * @brief       按键驱动 实现文件 (非阻塞消抖版本)
 * @note        适配STM32F103C8T6, 引脚: KEY0->PB12, KEY1->PB13
 *              使用系统计时器实现 10ms 非阻塞消抖，确保按键响应速度
 ****************************************************************************************************
 */

#include "./BSP/KEY/key.h"
#include "./SYSTEM/tick/tick.h"

/* 按键消抖状态机 */
typedef struct {
    uint8_t state;           /* 消抖状态: 0=等待按下, 1=确认中 */
    uint32_t press_time;     /* 按下时间戳 */
    uint8_t confirmed_key;   /* 确认的按键值 */
} KeyDebounceState;

static KeyDebounceState key0_state = {0, 0, 0};
static KeyDebounceState key1_state = {0, 0, 0};

/**
 * @brief       按键初始化
 * @param       无
 * @retval      无
 */
void key_init(void)
{
    KEY0_GPIO_CLK_ENABLE();
    KEY1_GPIO_CLK_ENABLE();
    
    /* 配置为上拉输入 */
    sys_gpio_set(KEY0_GPIO_PORT, KEY0_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);
    
    sys_gpio_set(KEY1_GPIO_PORT, KEY1_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);
    
    /* 初始化计时器 */
    tick_init();
}

/**
 * @brief       按键非阻塞消抖处理 (状态机)
 * @param       mode: 0-不支持连按, 1-支持连按
 * @retval      按键值: KEY0_PRES, KEY1_PRES, 或 0
 * @note        采用状态机算法:
 *              - 状态0: 等待按下
 *              - 状态1: 消抖中(10ms延时)
 *              - 状态2: 按键保持
 */
uint8_t key_scan(uint8_t mode)
{
    uint8_t keyval = 0;
    uint32_t current_tick = get_tick();
    
    /* ==================== KEY0 消抖状态机 ==================== */
    if (key0_state.state == 0)  /* 等待按下状态 */
    {
        if (!KEY0)  /* 检测到低电平 (按键按下) */
        {
            key0_state.state = 1;
            key0_state.press_time = current_tick;
        }
    }
    else if (key0_state.state == 1)  /* 消抖中 */
    {
        if (!KEY0)  /* 按键仍然按下 */
        {
            if (current_tick - key0_state.press_time >= 10)  /* 消抖时间已到 */
            {
                keyval = KEY0_PRES;
                key0_state.state = 2;  /* 进入按键保持状态 */
            }
        }
        else  /* 噪声 */
        {
            key0_state.state = 0;
        }
    }
    else if (key0_state.state == 2)  /* 按键保持状态 */
    {
        if (KEY0)  /* 按键松开 */
        {
            key0_state.state = 0;
        }
        else if (mode == 1)  /* 支持连按 */
        {
            if (current_tick - key0_state.press_time >= 200)  /* 每200ms连按一次 */
            {
                keyval = KEY0_PRES;
                key0_state.press_time = current_tick;
            }
        }
    }
    
    /* ==================== KEY1 消抖状态机 ==================== */
    if (key1_state.state == 0)  /* 等待按下状态 */
    {
        if (!KEY1)  /* 检测到低电平 (按键按下) */
        {
            key1_state.state = 1;
            key1_state.press_time = current_tick;
        }
    }
    else if (key1_state.state == 1)  /* 消抖中 */
    {
        if (!KEY1)  /* 按键仍然按下 */
        {
            if (current_tick - key1_state.press_time >= 10)  /* 消抖时间已到 */
            {
                keyval = KEY1_PRES;
                key1_state.state = 2;  /* 进入按键保持状态 */
            }
        }
        else  /* 噪声 */
        {
            key1_state.state = 0;
        }
    }
    else if (key1_state.state == 2)  /* 按键保持状态 */
    {
        if (KEY1)  /* 按键松开 */
        {
            key1_state.state = 0;
        }
        else if (mode == 1)  /* 支持连按 */
        {
            if (current_tick - key1_state.press_time >= 200)  /* 每200ms连按一次 */
            {
                keyval = KEY1_PRES;
                key1_state.press_time = current_tick;
            }
        }
    }
    
    return keyval;
}
