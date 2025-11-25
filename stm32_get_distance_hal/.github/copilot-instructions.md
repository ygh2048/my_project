# Copilot Instructions - STM32F103C8T6激光测距报警系统

## 项目核心概览

**项目类型**: STM32 微控制器嵌入式系统  
**开发工具**: Keil MDK-ARM v5.28  
**编译器**: ARMCC V5.06  
**MCU**: STM32F103C8T6 (72MHz, 64KB Flash, 20KB RAM)

### 系统架构

```
┌─────────────────────────────────────────────────────────────┐
│                      STM32F103C8T6                          │
│                       (Main MCU)                            │
├────────────────────────────────┬────────────────────────────┤
│                                │                            │
│  USART2 (921600)   USART3(9600)   USART1(115200)            │
│      ↓                 ↓              ↓                      │
│   STP-23激光         HC-05蓝牙      PC调试串口              │
│  (PA2/PA3)          (PB10/11)      (PA9/10)                 │
│                                                              │
│  GPIO Control (8引脚)                                        │
│  ├─ LCD1602: D4-D7(PA8,PA11,PA12,PB15)                      │
│  ├─        RS/RW/E(PB12,13,14)                             │
│  ├─ Buzzer: PB0                                             │
│  ├─ Keys: PC13-15, PA1, PA4(BT_STATE)                       │
│  └─ Timer: TIM2中断(80ms周期)                              │
└─────────────────────────────────────────────────────────────┘

数据流:
STP-23 (47字节帧) → USART2解析 → distance变量 → LCD显示/蓝牙发送
```

---

## 关键数据结构与全局状态

### 核心全局变量 (`main.c`)

```c
u16 distance = 0;              // 实时距离(mm) - STP-23解析结果
u16 threshold = 500;           // 报警阈值(mm) - 持久存储
uint8_t system_on = 0;         // 系统开/关状态
uint8_t alarm_on = 0;          // 报警激活状态
uint8_t setting_mode = 0;      // 阈值设置模式标志
uint16_t threshold_temp = 0;   // 设置中的临时阈值
uint8_t bt_connected = 0;      // BT连接标志(STATE引脚读取)
```

### UART缓冲区 (`usart.c`)

```c
// USART2 激光接收: 中断驱动，单字节缓冲
uint8_t Uart2_Receive_buf[1];  // 单字节接收,由STP23_ParseByte处理

// USART3 蓝牙: 行缓冲区,回车符('\r'或'\n')结束
#define BT_RX_BUF_SIZE 64
uint8_t bt_rx_buf[BT_RX_BUF_SIZE];
uint16_t bt_rx_index = 0;      // 仅在完整命令后重置
```

---

## 关键工作流程

### 1. 激光测距数据流 (关键: 921600波特率)

**重要**: USART2波特率是**921600**，不是115200。曾因此出现"0mm"故障。

```
USART2 ISR (usart.c)
  ↓
Uart2_Receive_buf[0] = 单字节
  ↓
main.c主循环调用 STP23_ParseByte()
  ↓
状态机解析:
  - 帧同步: 0x54, 0x2C
  - 12个距离值 (每个3字节: L, H, intensity)
  - CRC8校验
  ↓
解析成功 → distance = (H << 8) | L
  ↓
Display_Status() 刷新LCD
Send_Bluetooth_Data() 广播
```

**关键函数**: `STP23_ParseByte()` in `usart.c` (单字节状态机)

### 2. LCD显示更新 (两行16字符)

```
TIM2 IRQ (80ms周期) → Key_Scan() 
  ↓
主循环检查按键标志
  ↓
按键动作 → Display_Status()
  ↓
LCD_SetCursor(line, col) → LCD_WriteString(buf)
```

**显示格式**:
```
行1: "Dist:####mm     " (距离) 或 "System: OFF     " (关机)
行2: "Thr:####mm      " (阈值) 或 "Set:####mm      " (设置中)
```

### 3. 按键处理 (10ms消抖)

```
TIM2 中断 (80ms)
  → Key_Scan() 扫描 PC13/14/15, PA1, PA4(BT_STATE)
  → 按键去抖逻辑 (连续2次采样相同 = 确认)
  → 设置标志: key_flag

主循环
  → 检查 key_flag & 0xF0 (高4位为按键ID)
  → 调用对应处理函数
```

**按键映射** (见`key.h`):
- `KEY_ONOFF` (PC13): 系统开关
- `KEY_OK` (PC14): 进入/退出设置模式
- `KEY_UP` (PC15): 阈值+10mm
- `KEY_DOWN` (PA1): 阈值-10mm
- `BT_STATE` (PA4): 硬件读取,非按键

### 4. 蓝牙命令处理 (行为缓冲)

```
USART3 ISR 单字节接收
  ↓
bt_rx_buf[bt_rx_index++]
  ↓
若遇 '\r' 或 '\n' → bt_rx_index > 0 触发处理
  ↓
Process_Bluetooth_Command()
  - strstr() 搜索命令字符串
  - 格式: "CMD:ON", "SET:THR:500" 等
  ↓
bt_rx_index 重置为0
```

**为何这样做**: 蓝牙串口经常多帧到达,行缓冲避免重复处理

---

## 编译与构建流程

### Build命令 (Keil UV4)

```bash
# 完整重建 (推荐)
UV4.exe -r -j0 STP23.uvprojx

# 清理+构建 (若有链接错误)
UV4.exe -c -r STP23.uvprojx
```

**构建产物**:
```
MDK-ARM/STP23/
  ├── STP23.axf          # ELF二进制(含调试信息)
  ├── STP23.hex          # Intel HEX(烧录格式)
  └── *.d, *.crf, *.o    # 目标文件和依赖
```

**常见编译错误处理**:
1. `identifier undefined`: gpio.h/main.h定义检查 (参见下文)
2. `Undefined symbol`: HAL库缺失,检查STP23.uvprojx的文件列表
3. `Code size exceeded`: Flash超过64KB,需优化或删除代码

### CubeMX配置同步

**重要**: 若修改 `STP23.ioc` (CubeMX工程文件),需:
1. 打开CubeMX → 生成代码
2. 代码覆盖 Core/Inc/*.h, Core/Src/gpio.c, Core/Src/usart.c
3. 重新merge "USER CODE BEGIN/END" 区域
4. 例: BT_STATE引脚 (PA4) 曾在 .ioc 中新增,导致 gpio.h 需更新

---

## 引脚配置约定

### GPIO引脚定义位置 (`Core/Inc/main.h`)

```c
// GPIO宏定义(自动生成或手动)
#define LCD_RS_Pin        GPIO_PIN_12
#define LCD_RS_GPIO_Port  GPIOB
#define LCD_RW_Pin        GPIO_PIN_13
#define LCD_RW_GPIO_Port  GPIOB
// ... 其他引脚

// 蓝牙STATE引脚 (硬件实时读取,非中断)
#define BT_STATE_Pin      GPIO_PIN_4
#define BT_STATE_GPIO_Port GPIOA
```

**约定**:
- 所有GPIO初始化在 `gpio.c` MX_GPIO_Init() 中
- 引脚定义必须在 `main.h` 或 `gpio.h` 的 USER CODE 区
- 若手动改 .ioc,需更新 main.h 定义
- LCD的D0-D3必须接地(4位模式固定使用D4-D7),见PIN_CONFIGURATION.md

---

## 常见陷阱与调试技巧

### 1. USART波特率陷阱

| UART | 用途 | 波特率 | 错误后果 |
|------|------|--------|----------|
| USART1 | 调试printf | 115200 | 乱码,但能恢复 |
| **USART2** | **激光STP-23** | **921600** | **显示0mm,完全失效** ← 曾发生过 |
| USART3 | 蓝牙HC-05 | 9600 | 无蓝牙通讯 |

**检查方法**: `usart.c` MX_USARTx_UART_Init() 中的 huartx.Init.BaudRate

### 2. LCD 4位模式限制

- **必须使用 D4-D7**(高4位),不能用D0-D3
- D0-D3 必须接地,避免浮动干扰
- 一个字节需2次写操作(高4位+低4位)
- 初始化指令 0x28 是固定的

详见: `PIN_CONFIGURATION.md`, `LCD_4BIT_vs_8BIT_MODE.md`

### 3. 蓝牙STATE引脚读取

```c
// 正确: 直接读取GPIO,不依赖中断/查询
uint8_t bt_state = HAL_GPIO_ReadPin(BT_STATE_GPIO_Port, BT_STATE_Pin);
bt_connected = (bt_state == GPIO_PIN_SET) ? 1 : 0;

// 错误: 轮询UART查询"AT+STATE?"(旧方法,现已弃用)
```

### 4. 帧率与更新周期

```
STP-23 帧率:      15 Hz (67ms) → distance更新间隔
LCD 刷新:         100ms (手动调用Display_Status)
按键扫描:         80ms (TIM2中断)
蓝牙发送:         1000ms (Send_Bluetooth_Data)
```

**调试LCD更新不及时**: 检查Display_Status()是否在合适位置调用

### 5. printf调试输出

```c
// 已配置printf重定向到USART1 (usart.c fputc)
printf("[STP23] Distance: %dmm\r\n", distance);
printf("[BT] Connected: %d (STATE=%d)\r\n", bt_connected, BT_Read_State());

// 串口工具连接 PC 的 COM 口,115200 波特率查看
```

---

## 项目文件结构与修改指南

### 核心源文件 (`Core/Src/`)

| 文件 | 用途 | 修改风险 |
|------|------|----------|
| `main.c` | 主程序逻辑,全局变量,显示/蓝牙处理 | 低-高(变量需同步) |
| `usart.c` | UART初始化 + 激光/蓝牙中断处理 | **中**(波特率配置关键) |
| `gpio.c` | GPIO初始化(MX_GPIO_Init) | **中**(若手改.ioc需同步) |
| `tim.c` | TIM2定时器(80ms中断) | 低(配置固定) |

### 硬件驱动 (`Hardware/`)

| 文件 | 功能 | 约定 |
|------|------|------|
| `lcd1602.c/.h` | LCD驱动(4位模式) | 使用 LCD_SetCursor, LCD_WriteString, LCD_WriteNumber |
| `key.c/.h` | 按键扫描(10ms消抖) | key_flag 最低4位为按键ID,bit 7触发标志 |
| `bluetooth.c/.h` | 蓝牙(已弃用,仅AT指令参考) | 改用 Process_Bluetooth_Command() in main.c |

### 文档 (配置与诊断)

| 文件 | 内容 | 用途 |
|------|------|------|
| `PIN_CONFIGURATION.md` | 完整GPIO映射+D0-D3说明 | 硬件接线验证 |
| `LCD_4BIT_vs_8BIT_MODE.md` | 4位/8位模式对比 | 理解LCD工作原理 |
| `LCD_D0D3_GROUNDING_GUIDE.md` | D0-D3接地指南 | 解决LCD不稳定问题 |
| `PINOUT_QUICK_REFERENCE.txt` | 引脚速查表 | 快速查询引脚功能 |

---

## 代码风格与约定

### 命名规范

```c
// 全局变量: 小写 + 下划线
u16 distance;
uint8_t system_on;
uint16_t threshold_temp;

// 宏定义: 大写
#define BT_STATE_Pin GPIO_PIN_4
#define BT_RX_BUF_SIZE 64

// 函数: 小写单词 + 下划线
void Display_Status(void);
void Process_Bluetooth_Command(void);
uint8_t BT_Read_State(void);
```

### 中断与回调

```c
// UART中断在 stm32f1xx_it.c
void USART2_IRQHandler(void) {
    // 单字节接收 → Uart2_Receive_buf[0]
    // usart.c 中有 weak定义,自动调用
}

// 定时器中断
void TIM2_IRQHandler(void) {
    // 80ms周期 → Key_Scan() 调用
}
```

### USER CODE 区域

**重要**: 所有自定义代码在 `/* USER CODE BEGIN/END */` 区内
- CubeMX生成会覆盖这些区域外的代码
- .ioc 重新生成时自动保留

---

## 性能与优化

### 内存使用

```
Flash: 64KB
  ├─ 代码: ~12.3KB (19%)
  ├─ RO-data: ~900B
  └─ 可用: ~50KB

RAM: 20KB
  ├─ 全局变量: ~100B
  ├─ 栈: ~2KB (TIM2/UART ISR用)
  └─ 可用: ~18KB
```

### 中断优先级

```
NVIC优先级配置 (STM32F1 4级, 低优先级=高数值):
  - USART2 (激光): 1 (较高)
  - TIM2 (按键): 2 (中等)
  - USART3 (蓝牙): 2 (中等)
  - USART1 (调试): 不中断
```

**无嵌套中断**: 所有中断优先级 > 0,避免复杂性

---

## 集成与协作约定

### 代码合并

- 避免直接修改 CubeMX 生成代码段(会被覆盖)
- 在 USER CODE 区新增功能
- 若需改 gpio.c 初始化,在 CubeMX 中改 .ioc,重新生成

### 测试与验证

1. **编译**: `UV4.exe -r -j0 STP23.uvprojx` 成功 (0 errors, ~3 warnings)
2. **烧录**: STLink V2 或 ST-Link JTAG 到 PA13/PA14
3. **调试**: 
   - printf输出 on USART1 (115200)
   - 激光数据 on USART2 (921600)
   - 蓝牙命令 on USART3 (9600)

### 常见需求与实现位置

| 需求 | 实现位置 | 优先级 |
|------|----------|--------|
| 修改报警阈值逻辑 | `main.c` alarm_on判断 | 中 |
| 新增蓝牙命令 | `Process_Bluetooth_Command()` in main.c | 中 |
| 改LCD显示格式 | `Display_Status()` + `sprintf()` | 低 |
| 修改按键映射 | `Key_Scan()` 后的 `if(key_flag)` 处理 | 低 |
| 调整扫描周期 | `TIM2` ARR寄存器(usart.c MX_TIM2_Init) | 高 |

---

## 关键参考文档

- **Pin Definitions**: `PIN_CONFIGURATION.md` (引脚速查)
- **Laser Protocol**: `usart.c` STP23_ParseByte 注释
- **LCD Wiring**: `LCD_PINOUT_VERIFICATION.md`
- **Build System**: Keil UV4 IDE, project file: `MDK-ARM/STP23.uvprojx`

---

*最后更新: 2025-11-13*  
*维护者: AI Agent + User*
