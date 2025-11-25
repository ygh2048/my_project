# 激光测距报警系统 - 项目说明

## 📋 项目概述

本项目是一个基于STM32F103的激光测距报警系统，具有距离测量、阈值设置、超距报警和蓝牙数据传输功能。

### 主要功能
- ✅ 激光距离测量 (实时显示)
- ✅ LCD1602显示距离和阈值
- ✅ 可调阈值设置
- ✅ 超距蜂鸣器报警
- ✅ 蓝牙数据传输
- ✅ 四按键控制

---

## 🔧 硬件配置

### 核心处理器
- **MCU**: STM32F103C8T6
- **主频**: 72MHz
- **Flash**: 64KB
- **RAM**: 20KB

### 外设模块

#### 1. 激光测距模块
- **接口**: USART2 (PA2-TX, PA3-RX)
- **波特率**: 115200
- **协议**: 自定义帧格式，CRC8校验
- **测量范围**: 根据模块规格
- **精度**: 毫米级

#### 2. LCD1602显示模块
- **接口**: GPIO (4位数据模式)
- **引脚配置**:
  - RS: PB12
  - RW: PB13
  - E: PB14
  - D4-D7: PB15, PA8, PA11, PA12
- **显示内容**:
  - 第一行: 系统状态或距离
  - 第二行: 阈值或设置值

#### 3. 蓝牙模块
- **接口**: USART3 (PB10-TX, PB11-RX)
- **波特率**: 921600
- **功能**: 实时发送测量数据和状态

#### 4. 蜂鸣器
- **控制引脚**: PB0
- **触发条件**: 距离 > 阈值

#### 5. 按键模块
| 按键 | 引脚 | 功能 |
|------|------|------|
| KEY_OK | PC13 | 进入/退出设置模式 |
| KEY_UP | PC15 | 增加阈值 (+10mm) |
| KEY_DOWN | PC14 | 减少阈值 (-10mm) |
| KEY_ONOFF | PA1 | 系统开关 |

---

## 💻 软件架构

### 文件结构
```
├── Core/
│   ├── Inc/
│   │   ├── main.h          # 主头文件和引脚定义
│   │   ├── gpio.h          # GPIO配置
│   │   ├── usart.h         # 串口配置和数据结构
│   │   └── tim.h           # 定时器配置
│   └── Src/
│       ├── main.c          # 主程序逻辑
│       ├── gpio.c          # GPIO初始化
│       ├── usart.c         # 串口通信和数据解析
│       └── tim.c           # 定时器配置
├── Hardware/
│   ├── lcd1602.c/.h        # LCD1602驱动
│   └── key.c/.h            # 按键扫描模块
└── Drivers/                # HAL库驱动
```

### 主要模块说明

#### 1. LCD1602驱动 (`lcd1602.c`)
- **功能**: 
  - 4位模式初始化
  - 字符串、数字、浮点数显示
  - 光标定位
  - 清屏功能
- **API**:
  ```c
  void LCD1602_Init(void);
  void LCD1602_Clear(void);
  void LCD1602_SetCursor(uint8_t line, uint8_t column);
  void LCD1602_WriteString(char *str);
  void LCD1602_WriteNumber(uint32_t num);
  ```

#### 2. 按键扫描模块 (`key.c`)
- **功能**:
  - 按键状态检测
  - 软件消抖
  - 支持4个独立按键
- **API**:
  ```c
  uint8_t KEY_Scan(void);
  uint8_t KEY_GetState(uint8_t key);
  ```

#### 3. 串口通信 (`usart.c`)
- **USART1**: 调试输出 (115200)
- **USART2**: 激光测距数据接收
- **USART3**: 蓝牙通信 (921600)
- **数据解析**: 
  - 状态机解析
  - CRC8校验
  - 12点数据平均

#### 4. 主程序逻辑 (`main.c`)
- **系统状态管理**:
  - `system_on`: 系统开关状态
  - `setting_mode`: 设置模式标志
  - `alarm_on`: 报警状态
- **核心函数**:
  ```c
  void Display_Status(void);          // 更新LCD显示
  void Process_Keys(void);            // 处理按键输入
  void Process_Alarm(void);           // 处理报警逻辑
  void Send_Bluetooth_Data(void);     // 发送蓝牙数据
  ```

---

## 🎮 使用说明

### 开机流程
1. 系统上电，LCD显示欢迎信息
2. 2秒后进入正常工作模式
3. 默认状态: 系统关闭

### 基本操作

#### 启动测量
1. 按下 **KEY_ONOFF** 按键
2. LCD第一行显示: `Dist:XXXXmm`
3. LCD第二行显示: `Thr:XXXXmm`

#### 设置阈值
1. 在系统开启状态下，按 **KEY_OK** 进入设置模式
2. LCD第二行显示: `Set:XXXXmm`
3. 按 **KEY_UP** 增加阈值 (+10mm/次)
4. 按 **KEY_DOWN** 减少阈值 (-10mm/次)
5. 再次按 **KEY_OK** 保存设置并退出

#### 关闭系统
1. 按下 **KEY_ONOFF** 按键
2. LCD第一行显示: `System: OFF`
3. 蜂鸣器停止

### 报警功能
- **触发条件**: 测量距离 > 设定阈值
- **报警方式**: 蜂鸣器鸣响
- **解除方式**: 
  - 距离降低到阈值以下
  - 关闭系统

### 蓝牙数据格式
每秒发送一次数据，格式如下:
```
Distance:XXXXmm,Threshold:XXXXmm,Alarm:X\r\n
```
示例:
```
Distance:450mm,Threshold:500mm,Alarm:0
Distance:550mm,Threshold:500mm,Alarm:1
```

---

## 📊 技术参数

| 参数 | 数值 |
|------|------|
| 测量刷新率 | 100ms |
| 显示刷新率 | 100ms |
| 蓝牙发送频率 | 1秒/次 |
| 按键消抖时间 | 10ms |
| 阈值调节步进 | 10mm |
| 阈值范围 | 0-9999mm |
| 默认阈值 | 500mm |

---

## 🔨 编译和烧录

### 编译环境
- **IDE**: Keil MDK-ARM V5.32
- **编译器**: ARM Compiler V5.06 update 6
- **优化等级**: Level 0 (-O0)

### 编译命令
```bash
D:\workapp\keil_v5\UV4\UV4.exe -r f:\ygh20\money\work6_distance\get_distance\MDK-ARM\STP23.uvprojx -j0 -t STP23 -o f:\ygh20\money\work6_distance\get_distance\MDK-ARM\.vscode\uv4.log
```

### 编译结果
```
Program Size: Code=9740 RO-data=584 RW-data=52 ZI-data=2172
0 Error(s), 3 Warning(s)
```

### 烧录方式
1. **ST-Link**: 使用ST-Link V2通过SWD接口烧录
2. **串口**: 使用串口下载工具(需要Bootloader)

---

## 🐛 调试说明

### 串口调试
连接USART1 (PA9-TX, PA10-RX) 到USB-TTL模块:
- **波特率**: 115200
- **数据位**: 8
- **停止位**: 1
- **校验**: 无

### 调试信息输出
```
Laser Distance Measurement System
System Initialized!
Distance: 450mm, Threshold: 500mm, Alarm: 0
Distance: 550mm, Threshold: 500mm, Alarm: 1
```

---

## ⚠️ 注意事项

1. **按键接线**: 按键为低电平有效，内部已配置上拉电阻
2. **LCD对比度**: LCD1602需要外接10K电位器调节对比度(VEE引脚)
3. **激光安全**: 避免激光直射眼睛
4. **蜂鸣器**: 如果蜂鸣器为有源蜂鸣器，直接GPIO控制；无源蜂鸣器需要PWM驱动
5. **蓝牙配对**: 首次使用需先配对蓝牙模块
6. **电源**: 确保5V电源供电稳定，推荐使用1A以上电源

---

## 📝 版本历史

### V1.0 (2025-11-11)
- ✅ 初始版本发布
- ✅ 实现基本测距功能
- ✅ 实现LCD1602显示
- ✅ 实现按键控制
- ✅ 实现蓝牙数据传输
- ✅ 实现阈值报警

---

## 👨‍💻 开发信息

- **开发日期**: 2025年11月11日
- **开发工具**: 
  - STM32CubeMX 6.13.0
  - Keil MDK-ARM V5.32
  - HAL库版本: STM32F1xx HAL Driver V1.8.6
- **测试状态**: ✅ 编译通过

---

## 📄 许可证

本项目遵循 MIT 许可证。

---

## 🔗 相关文档

- [引脚说明.md](./引脚说明.md) - 详细引脚配置说明
- STM32F103C8T6数据手册
- LCD1602使用手册
- 激光测距模块通信协议

---

**项目完成！** 🎉
