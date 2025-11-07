# 智能实验台监控系统 - 文件功能介绍

## 项目概述

基于STM32F103C8T6的智能实验台监控系统，实现电压电流功率监测、重量检测、人体检测、智能照明控制等功能。

## 文件结构及功能说明

### User/ - 用户应用程序层
- **main.c** - 主程序文件，包含系统初始化、状态更新、OLED显示、按键处理、串口命令处理等核心逻辑

### Drivers/BSP/ - 板级支持包
#### ADC/ - 模数转换器驱动
- **adc.c** - ADC初始化、电压采样、电流采样实现
- **adc.h** - ADC通道定义、函数声明

#### ESP8266/ - WiFi模块驱动
- **esp8266.c** - ESP8266 WiFi模块通信协议实现
- **esp8266.h** - WiFi连接配置、命令定义

#### HC_SR505/ - 人体红外传感器驱动
- **hc_sr505.c** - HC-SR505人体检测传感器控制实现
- **hc_sr505.h** - 传感器引脚定义、检测函数声明

#### HX711/ - 重量传感器驱动
- **HX711.c** - HX711 24位ADC重量传感器数据读取和处理
- **HX711.h** - 重量传感器引脚定义、校准参数、全局变量声明

#### KEY/ - 按键驱动
- **key.c** - 按键扫描、消抖处理实现
- **key.h** - 按键引脚定义、按键状态宏定义

#### LED/ - LED指示灯驱动
- **led.c** - LED控制、闪烁功能实现
- **led.h** - LED引脚定义、控制宏定义

#### MQTT/ - MQTT协议目录
- *(空目录，预留MQTT协议实现)*

#### OLED/ - OLED显示屏驱动
- **oled.c** - OLED显示控制、字符显示、图形绘制实现
- **oled.h** - OLED引脚定义、显示函数声明
- **oled_temp.c** - OLED临时显示功能
- **oledfont.h** - OLED字库定义

#### ONENET/ - OneNET云平台驱动
- **onenet.c** - OneNET物联网平台连接和数据上传
- **onenet.h** - OneNET配置参数、功能点定义
- **onenet_protocol.c** - OneNET MQTT协议实现
- **onenet_protocol.h** - 协议数据结构定义
- **mem_pool.c** - 内存池管理，用于MQTT数据包分配
- **mem_pool.h** - 内存池函数声明
- **base64.c** - Base64编码解码
- **base64.h** - Base64函数声明
- **hmac_sha1.c** - HMAC-SHA1加密算法
- **hmac_sha1.h** - HMAC-SHA1函数声明
- **mqttkit.c** - MQTT协议工具函数
- **mqttkit.h** - MQTT工具函数声明

#### RELAY/ - 继电器控制驱动
- **relay.c** - 照明和电源继电器控制实现
- **relay.h** - 继电器引脚定义、控制宏定义

#### WDG/ - 看门狗驱动
- **wdg.c** - 独立看门狗初始化和喂狗实现
- **wdg.h** - 看门狗函数声明

### Drivers/SYSTEM/ - 系统基础库
#### delay/ - 延时函数库
- **delay.c** - 基于SysTick的精确延时实现
- **delay.h** - 延时函数声明

#### sys/ - 系统配置库
- **sys.c** - 系统时钟、GPIO、中断等基础配置
- **sys.h** - 系统配置函数和宏定义

#### tick/ - 系统计时器
- **tick.c** - 1ms精度的系统滴答计时器实现
- **tick.h** - 计时器函数声明

#### usart/ - 串口通信库
- **usart.c** - USART1串口初始化、收发实现，支持printf重定向
- **usart.h** - 串口配置、缓冲区定义

### Drivers/CMSIS/ - CMSIS标准库
#### Device/ST/STM32F1xx/ - STM32F1系列设备文件
- **Include/** - 芯片头文件目录
  - **stm32f103xe.h** - STM32F103XE芯片寄存器定义
  - **stm32f1xx.h** - STM32F1系列通用定义
  - **system_stm32f1xx.h** - 系统时钟配置
- **Source/Templates/** - 启动文件目录
  - **system_stm32f1xx.c** - 系统初始化实现
  - **arm/startup_stm32f103xe.s** - 启动汇编代码

#### Include/ - CMSIS核心头文件
- **cmsis_armcc.h** - ARMCC编译器支持
- **cmsis_armclang.h** - ARM Clang编译器支持
- **cmsis_compiler.h** - 编译器抽象层
- **cmsis_version.h** - CMSIS版本信息
- **core_cm3.h** - Cortex-M3核心定义
- **mpu_armv7.h** - MPU定义

### Middlewares/ - 中间件层
- **readme.txt** - 中间件说明文档

### Output/ - 编译输出文件
- ***.axf** - 可执行文件
- ***.hex** - 十六进制文件
- ***.lst** - 列表文件
- ***.d** - 依赖文件
- ***.crf** - 交叉引用文件
- **atk_f103.build_log.htm** - 编译日志
- **atk_f103.htm** - 编译报告

### Projects/ - 开发环境配置
#### MDK-ARM/ - Keil MDK项目文件
- **atk_f103.uvprojx** - Keil项目文件
- **atk_f103.uvoptx** - Keil选项文件
- **atk_f103.uvguix.ygh20** - Keil GUI配置
- **EventRecorderStub.scvd** - 事件记录器配置

### 根目录文件
- **README.md** - 项目说明文档
- **引脚分配表.txt** - 硬件引脚分配说明
- **keilkill.bat** - Keil编译清理脚本

## 技术规格

- **MCU**: STM32F103C8T6 (72MHz, 64KB Flash, 20KB RAM)
- **编译环境**: Keil MDK v5.28
- **代码大小**: ~23KB
- **通信接口**: USART1(115200), USART3(ESP8266), I2C(OLED)
- **传感器接口**: ADC(电压电流), HX711(重量), HC-SR505(人体检测)
- **控制接口**: GPIO(继电器控制), 按键输入

## 开发信息

- **版本**: v1.2
- **日期**: 2025-11-07
- **作者**: GitHub Copilot
- **状态**: 稳定可运行
