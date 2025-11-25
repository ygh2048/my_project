/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd1602.h"
#include "key.h"
#include "bluetooth.h"
#include "matrix_keypad.h"  // 矩阵键盘驱动
#include <stdio.h>
#include <string.h>
#include <stdlib.h>  // for atoi()
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// 全局变量
u16 receive_cnt = 0;         // 接收成功计数
u16 distance = 0;            // 当前距离 (mm)
u16 threshold = 5000;         // 报警阈值 (mm)，默认5000mm
uint8_t system_on = 0;       // 系统开关状态: 0=关闭, 1=开启
uint8_t alarm_on = 0;        // 报警状态: 0=无报警, 1=报警中
uint8_t setting_mode = 0;    // 设置模式: 0=正常显示, 1=设置阈值
uint16_t threshold_temp = 0; // 阈值临时值(设置时使用)

// 蜂鸣器控制
void Buzzer_On(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
}

void Buzzer_Off(void)
{
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
}

// 读取蓝牙连接状态（通过STATE引脚）
uint8_t BT_Read_State(void)
{
    // STATE引脚: 高电平=已连接, 低电平=未连接
    return (HAL_GPIO_ReadPin(BT_STATE_GPIO_Port, BT_STATE_Pin) == GPIO_PIN_SET) ? 1 : 0;
}

// 显示当前距离和阈值
void Display_Status(void)
{
    char str[17];
    
    // 第一行显示距离
    LCD1602_SetCursor(0, 0);
    if(system_on) {
        sprintf(str, "Dist:%4dmm     ", distance);
    } else {
        sprintf(str, "System: OFF     ");
    }
    LCD1602_WriteString(str);
    
    // 第二行显示阈值
    LCD1602_SetCursor(1, 0);
    if(setting_mode) {
        sprintf(str, "Set:%4dmm      ", threshold_temp);
    } else {
        sprintf(str, "Thr:%4dmm      ", threshold);
    }
    LCD1602_WriteString(str);
}

// 蓝牙初始化：清空HC-05启动时产生的数据
void Initialize_Bluetooth(void)
{
    // 给HC-05启动时间
    HAL_Delay(500);
    
    // 清空接收缓冲区（丢弃HC-05启动时的数据）
    memset(bt_rx_buf, 0, BT_RX_BUF_SIZE);
    bt_rx_index = 0;
    
    printf("[INIT] Bluetooth initialized, waiting for connection...\r\n");
}

// 通过蓝牙发送数据
void Send_Bluetooth_Data(void)
{
    char buf[50];
    sprintf(buf, "Distance:%dmm,Threshold:%dmm,Alarm:%d\r\n", 
            distance, threshold, alarm_on);
    HAL_UART_Transmit(&huart3, (uint8_t*)buf, strlen(buf), 100);
}

// 蓝牙命令处理函数
void Process_Bluetooth_Command(void)
{
    // 检查是否有完整的蓝牙命令接收
    if(bt_rx_index == 0) {
        return;  // 没有数据
    }
    
    char *cmd = (char*)bt_rx_buf;
    
    // ⚠️ 蓝牙模块启动时可能发送AT指令,直接忽略
    if(strstr(cmd, "AT") != NULL) {
        printf("[DEBUG] HC-05 AT command received, ignoring: %s\r\n", cmd);
        memset(bt_rx_buf, 0, BT_RX_BUF_SIZE);
        bt_rx_index = 0;
        return;  // 忽略AT指令
    }
    
    // 连接检测: 蓝牙模块发送"OK"表示连接成功
    if(strstr(cmd, "OK") != NULL || strstr(cmd, "CONNECTED") != NULL) {
        if(!bt_connected) {
            bt_connected = 1;
            system_on = 1;  // 自动开启系统
            Display_Status();
        }
    }
    
    // 命令格式: "CMD:ON" 或 "CMD:OFF" 控制系统开关
    if(strstr(cmd, "CMD:ON") != NULL) {
        if(!system_on) {
            system_on = 1;
            Display_Status();
        }
        // 回复蓝牙
        HAL_UART_Transmit(&huart3, (uint8_t*)"ACK:ON\r\n", 8, 100);
    }
    else if(strstr(cmd, "CMD:OFF") != NULL) {
        if(system_on) {
            system_on = 0;
            Display_Status();
        }
        HAL_UART_Transmit(&huart3, (uint8_t*)"ACK:OFF\r\n", 9, 100);
    }
    
    // 命令格式: "SET:THR:500" 设置阈值为500mm
    if(strstr(cmd, "SET:THR:") != NULL) {
        uint16_t new_threshold;
        if(sscanf(cmd, "SET:THR:%hu", &new_threshold) == 1) {
            if(new_threshold <= 9999) {
                threshold = new_threshold;
                printf("[DEBUG] BT CMD: Threshold set to %dmm\r\n", threshold);
                Display_Status();
                // 回复蓝牙
                char ack[30];
                sprintf(ack, "ACK:THR:%d\r\n", threshold);
                HAL_UART_Transmit(&huart3, (uint8_t*)ack, strlen(ack), 100);
            }
        }
    }
    
    // 命令格式: "GET:STATUS" 获取系统状态
    if(strstr(cmd, "GET:STATUS") != NULL) {
        char status[60];
        sprintf(status, "Status:System=%s,Distance=%dmm,Threshold=%dmm,Alarm=%d\r\n",
                system_on ? "ON" : "OFF", distance, threshold, alarm_on);
        HAL_UART_Transmit(&huart3, (uint8_t*)status, strlen(status), 100);
        printf("[DEBUG] BT CMD: Status sent\r\n");
    }
    
    // 清除接收缓冲区
    memset(bt_rx_buf, 0, BT_RX_BUF_SIZE);
    bt_rx_index = 0;
}

// 检查蓝牙连接状态 (通过STATE引脚实时检测)
void Check_Bluetooth_Connection(void)
{
    static uint8_t last_state = 0;
    uint8_t current_state = BT_Read_State();
    
    // 检测状态变化
    if(current_state != last_state) {
        last_state = current_state;
        
        if(current_state) {
            // 连接成功
            bt_connected = 1;
            system_on = 1;  // 自动开启系统
            printf("[BT] Connected (STATE pin HIGH)\r\n");
            Display_Status();
        } else {
            // 断开连接
            bt_connected = 0;
            printf("[BT] Disconnected (STATE pin LOW)\r\n");
        }
    }
}

// 串口1调试信息输出
void Print_Debug_Info(void)
{
    static uint32_t last_print_time = 0;
    
    // 每秒打印一次调试信息
    if(HAL_GetTick() - last_print_time >= 1000) {
        last_print_time = HAL_GetTick();
        
        printf("\n=== SYSTEM DEBUG INFO ===\r\n");
        printf("Time: %lums\r\n", HAL_GetTick());
        printf("Distance: %dmm\r\n", distance);
        printf("Threshold: %dmm\r\n", threshold);
        printf("System: %s\r\n", system_on ? "ON" : "OFF");
        printf("Alarm: %s\r\n", alarm_on ? "ALARMING" : "NORMAL");
        printf("Setting: %s\r\n", setting_mode ? "YES" : "NO");
        printf("BT Connected: %s (STATE pin: %s)\r\n", 
               bt_connected ? "YES" : "NO",
               BT_Read_State() ? "HIGH" : "LOW");
        printf("LiDAR Frames Received: %d\r\n", receive_cnt);
        printf("========================\r\n");
    }
}

// 处理报警逻辑
void Process_Alarm(void)
{
    if(system_on && distance > 0) {
        if(distance > threshold) {
            alarm_on = 1;
            Buzzer_On();
        } else {
            alarm_on = 0;
            Buzzer_Off();
        }
    } else {
        alarm_on = 0;
        Buzzer_Off();
    }
}

// 按键处理函数
void Process_Keys(void)
{
    uint8_t key = KEY_Scan();
    
    if(key == KEY_NONE) {
        return;
    }
    
    printf("[KEYS] Independent key pressed: %d\r\n", key);
    
    // KEY_ONOFF按键 - 系统开关切换（既可以开机也可以关机）
    if(key == KEY_ONOFF) {
        printf("[KEYS] KEY_ONOFF pressed, system_on=%d\r\n", system_on);
        
        system_on = !system_on;
        
        if (system_on) {
            printf("[KEYS] System ON (toggled)\r\n");
            LCD1602_Clear();
            LCD1602_SetCursor(0, 0);
            LCD1602_WriteString("System: ON");
            HAL_Delay(1000);
        } else {
            alarm_on = 0;
            Buzzer_Off();
            printf("[KEYS] System OFF (toggled)\r\n");
            LCD1602_Clear();
            LCD1602_SetCursor(0, 0);
            LCD1602_WriteString("System: OFF");
            HAL_Delay(1000);
        }
        
        Display_Status();
        return;
    }
    
    // 确认按键 - 进入/退出设置模式
    if(key == KEY_OK) {
        if(setting_mode) {
            // 确认设置，保存阈值
            threshold = threshold_temp;
            setting_mode = 0;
        } else {
            // 进入设置模式
            setting_mode = 1;
            threshold_temp = threshold;
        }
        Display_Status();
        HAL_Delay(200);
        return;
    }
    
    // UP/DOWN键 - 在设置模式下调整阈值
    if(setting_mode) {
        if(key == KEY_UP) {
            threshold_temp += 10; // 每次增加10mm
            if(threshold_temp > 9999) threshold_temp = 9999;
            Display_Status();
            HAL_Delay(100);
        }
        else if(key == KEY_DOWN) {
            if(threshold_temp >= 10) {
                threshold_temp -= 10; // 每次减少10mm
            }
            Display_Status();
            HAL_Delay(100);
        }
    }
}

// 矩阵键盘处理函数
static void Process_Matrix_Keypad(void)
{
    static char input_buffer[5] = {0};     // 输入缓冲区（最多4位数字 + '\0'）
    static uint8_t input_index = 0;        // 当前输入位置
    // 注意: 使用全局变量setting_mode替代局部变量input_mode，确保主循环能正确判断
    
    uint8_t key = Matrix_Keypad_GetKey();
    
    if (key == MATRIX_KEY_NONE) {
        return;
    }
    
    char key_char = Matrix_Keypad_GetChar(key);
    
    // 判断数字键: 明确列举所有数字键，排除A/B/C/D键
    // 数字键: 1=16, 2=15, 3=14, 4=12, 5=11, 6=10, 7=8, 8=7, 9=6, 0=3
    // 特殊键: A=13, B=9, C=5, D=1 (不是数字键)
    uint8_t is_digit_key = (key == MATRIX_KEY_1 || key == MATRIX_KEY_2 || key == MATRIX_KEY_3 ||
                            key == MATRIX_KEY_4 || key == MATRIX_KEY_5 || key == MATRIX_KEY_6 ||
                            key == MATRIX_KEY_7 || key == MATRIX_KEY_8 || key == MATRIX_KEY_9 ||
                            key == MATRIX_KEY_0);
    
    printf("[MATRIX] Key: %d (%c) - is_digit_key=%d, setting_mode=%d\r\n", key, key_char, is_digit_key, setting_mode);
    
    // 调试: 打印按键检测状态和A/B键的值
    printf("[DEBUG] MATRIX_KEY_A=%d, MATRIX_KEY_B=%d, MATRIX_KEY_C=%d, MATRIX_KEY_D=%d\r\n", 
           MATRIX_KEY_A, MATRIX_KEY_B, MATRIX_KEY_C, MATRIX_KEY_D);
    
    // 注意: 不要在这里调用Display_Status()，会覆盖C键设置的界面
    // 每个按键处理函数会在需要时自己调用Display_Status()
    
    // ==================== 数字键处理 (0-9) ====================
    
    if (is_digit_key) {
        if (!setting_mode) {
            // 不在输入模式时，数字键无效，静默忽略（不显示提示）
            printf("[MATRIX] Not in setting mode, digit key ignored\r\n");
            return;
        }
        
        // 在输入模式下，累积数字输入
        if (input_index < 4) {
            input_buffer[input_index++] = key_char;
            input_buffer[input_index] = '\0';
            
            // 调试输出
            printf("[INPUT] Threshold input: %s\r\n", input_buffer);
            
            // 组合完整字符串后一次性写入LCD
            char display_buf[17];
            sprintf(display_buf, "Input: %s mm    ", input_buffer);
            LCD1602_SetCursor(1, 0);
            LCD1602_WriteString(display_buf);
        } else {
            // 已达到最大长度（4位）
            LCD1602_SetCursor(1, 0);
            LCD1602_WriteString("Max 4 digits!   ");
            HAL_Delay(500);
            
            // 恢复显示当前输入
            char display_buf[17];
            sprintf(display_buf, "Input: %s mm    ", input_buffer);
            LCD1602_SetCursor(1, 0);
            LCD1602_WriteString(display_buf);
        }
        return;
    }
    
    // ==================== # 键: 确认输入 ====================
    if (key == MATRIX_KEY_HASH) {
        if (setting_mode && input_index > 0) {
            // 转换输入为整数
            uint16_t new_threshold = atoi(input_buffer);
            
            if (new_threshold >= 10 && new_threshold <= 9999) {
                threshold = new_threshold;
                printf("[MATRIX] Threshold set to: %d mm\r\n", threshold);
                
                // 显示确认信息
                LCD1602_Clear();
                LCD1602_SetCursor(0, 0);
                LCD1602_WriteString("Threshold Set:");
                LCD1602_SetCursor(1, 0);
                char buf[17];
                sprintf(buf, "%d mm", threshold);
                LCD1602_WriteString(buf);
                HAL_Delay(1500);
            } else {
                // 无效输入
                LCD1602_Clear();
                LCD1602_SetCursor(0, 0);
                LCD1602_WriteString("Invalid! (10-9999)");
                HAL_Delay(1500);
            }
            
            // 退出输入模式，返回正常显示
            setting_mode = 0;
            input_index = 0;
            memset(input_buffer, 0, sizeof(input_buffer));
            Display_Status();
        }
        return;
    }
    
    // ==================== * 键: 取消输入 ====================
    if (key == MATRIX_KEY_STAR) {
        if (setting_mode) {
            // 清空输入缓冲区
            setting_mode = 0;
            input_index = 0;
            memset(input_buffer, 0, sizeof(input_buffer));
            
            // 返回正常显示
            Display_Status();
            printf("[MATRIX] Input cancelled\r\n");
        } else {
            // 在非输入模式下，* 键作为清空/复位功能
            LCD1602_Clear();
            LCD1602_SetCursor(0, 0);
            LCD1602_WriteString("Display Cleared");
            HAL_Delay(1000);
            Display_Status();
        }
        return;
    }
    
    // ==================== A 键: 系统开机 ====================
    if (key == MATRIX_KEY_A) {
        printf("[MATRIX] A key pressed, system_on=%d\r\n", system_on);
        
        if (!system_on) {
            system_on = 1;
            printf("[MATRIX] System ON\r\n");
            
            LCD1602_Clear();
            LCD1602_SetCursor(0, 0);
            LCD1602_WriteString("System: ON");
            HAL_Delay(1000);
            Display_Status();
        } else {
            // 系统已经开启，给出提示
            printf("[MATRIX] System already ON\r\n");
        }
        return;
    }
    
    // ==================== B 键: 系统关机 ====================
    if (key == MATRIX_KEY_B) {
        printf("[MATRIX] B key pressed, system_on=%d\r\n", system_on);
        
        if (system_on) {
            system_on = 0;
            alarm_on = 0;
            Buzzer_Off();
            printf("[MATRIX] System OFF\r\n");
            
            LCD1602_Clear();
            LCD1602_SetCursor(0, 0);
            LCD1602_WriteString("System: OFF");
            HAL_Delay(1000);
            Display_Status();
        } else {
            // 系统已经关闭，给出提示
            printf("[MATRIX] System already OFF\r\n");
        }
        return;
    }
    
    // ==================== C 键: 进入阈值设置模式 ====================
    if (key == MATRIX_KEY_C) {
        if (!setting_mode) {
            // 进入阈值设置模式
            setting_mode = 1;
            input_index = 0;
            memset(input_buffer, 0, sizeof(input_buffer));
            
            printf("[MATRIX] Enter threshold setting mode, setting_mode=%d\r\n", setting_mode);
            
            // LCD显示提示
            LCD1602_Clear();
            HAL_Delay(10);
            LCD1602_SetCursor(0, 0);
            LCD1602_WriteString("Set Threshold:");
            LCD1602_SetCursor(1, 0);
            LCD1602_WriteString("Input: _ mm");
            
            printf("[MATRIX] LCD display set completed\r\n");
        } else {
            // 已在输入模式，不做处理
            printf("[MATRIX] Already in setting mode\r\n");
        }
        return;
    }
    
    // ==================== D 键: 查看系统信息 + 蜂鸣提示 ====================
    if (key == MATRIX_KEY_D) {
        printf("[MATRIX] System Info Display with Buzzer\r\n");
        
        // 蜂鸣器提示音（表示进入信息显示模式）
        Buzzer_On();
        HAL_Delay(100);
        Buzzer_Off();
        HAL_Delay(100);
        
        // 循环显示系统信息
        char buf[17];
        
        // 第1屏: 距离和阈值
        LCD1602_Clear();
        LCD1602_SetCursor(0, 0);
        sprintf(buf, "Dist:%4dmm", distance);
        LCD1602_WriteString(buf);
        LCD1602_SetCursor(1, 0);
        sprintf(buf, "Thr:%4dmm", threshold);
        LCD1602_WriteString(buf);
        HAL_Delay(2000);
        
        // 第2屏: 系统状态
        LCD1602_Clear();
        LCD1602_SetCursor(0, 0);
        LCD1602_WriteString(system_on ? "System: ON" : "System: OFF");
        LCD1602_SetCursor(1, 0);
        LCD1602_WriteString(alarm_on ? "Alarm: ACTIVE" : "Alarm: NORMAL");
        HAL_Delay(2000);
        
        // 第3屏: 蓝牙状态
        LCD1602_Clear();
        LCD1602_SetCursor(0, 0);
        LCD1602_WriteString("Bluetooth:");
        LCD1602_SetCursor(1, 0);
        LCD1602_WriteString(bt_connected ? "Connected" : "Disconnected");
        HAL_Delay(2000);
        
        // 蜂鸣器结束提示音
        Buzzer_On();
        HAL_Delay(100);
        Buzzer_Off();
        
        // 返回正常显示
        Display_Status();
        return;
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  
  // 初始化LCD1602
  LCD1602_Init();
  LCD1602_Clear();
  
  // 初始化按键
  KEY_Init();
  
  // 初始化矩阵键盘
  Matrix_Keypad_Init();
  printf("[INIT] Matrix Keypad initialized\r\n");
  
  // 初始化蜂鸣器
  Buzzer_Off();
  
  // 初始化蓝牙模块
  Initialize_Bluetooth();
  
  // 显示欢迎信息
  LCD1602_SetCursor(0, 0);
  LCD1602_WriteString("Laser Distance");
  LCD1602_SetCursor(1, 0);
  LCD1602_WriteString("  Measurement  ");
  HAL_Delay(2000);
  LCD1602_Clear();
  
  // 初始显示
  Display_Status();
  
  printf("Laser Distance Measurement System Started\r\n");
  
  // 启动定时器
  HAL_TIM_Base_Start_IT(&htim2);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  
  uint32_t last_update_time = 0;
  uint32_t last_bt_time = 0;
  uint32_t last_key_scan_time = 0;  // 矩阵键盘扫描时间戳
  
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
    // 处理独立按键
    Process_Keys();
    
    // 处理矩阵键盘 (每2ms扫描一次，提高响应速度)
    if (HAL_GetTick() - last_key_scan_time >= 2) {
        last_key_scan_time = HAL_GetTick();
        Process_Matrix_Keypad();
    }
    
    // 处理蓝牙命令
    Process_Bluetooth_Command();
    
    // 检查蓝牙连接状态
    Check_Bluetooth_Connection();
    
    // 每100ms更新一次显示和报警检测
    if(HAL_GetTick() - last_update_time >= 100) {
        last_update_time = HAL_GetTick();
        
        if(!setting_mode) {
            printf("[MAIN] Calling Display_Status() (setting_mode=%d)\r\n", setting_mode);
            Display_Status();
        } else {
            printf("[MAIN] Skip Display_Status() in setting mode\r\n");
        }
        
        // 处理报警逻辑
        Process_Alarm();
    }
    
    // 每1秒通过蓝牙发送一次数据
    if(HAL_GetTick() - last_bt_time >= 1200) {
        last_bt_time = HAL_GetTick();
        
        if(system_on) {
            Send_Bluetooth_Data();
        }
        
        // 每秒打印一次调试信息
        Print_Debug_Info();
    }
    
    HAL_Delay(1); // 主循环延时
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
