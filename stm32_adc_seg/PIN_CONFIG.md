# STM32F103 引脚配置表

## 按端口分组

### GPIOA (16 pins)
| 引脚 | 功能 | 模式 | 用途 |
|------|------|------|------|
| PA0 | DIGIT-3 | Out_PP | 千位数字位选 |
| PA1 | SEG-a | Out_PP | 7段显示段 a (顶部) |
| PA2 | SEG-f | Out_PP | 7段显示段 f (左上) |
| PA3 | DIGIT-2 | Out_PP | 百位数字位选 |
| PA4 | DIGIT-1 | Out_PP | 十位数字位选 |
| PA5 | SEG-b | Out_PP | 7段显示段 b (右上) |
| PA11 | SEG-g | Out_PP | 7段显示段 g (中间) |
| PA12 | DIGIT-0 | Out_PP | 个位数字位选 |
| PA9 | UART1-TX | AF_PP | 串口发送 |
| PA10 | UART1-RX | IN_FLOATING | 串口接收 |

### GPIOB (16 pins)
| 引脚 | 功能 | 模式 | 用途 |
|------|------|------|------|
| PB0 | ADC | AIN | 模拟电压采集 |
| PB1 | LED1 | Out_PP | 状态灯 1 |
| PB5 | LED0 | Out_PP | 状态灯 0 |
| PB6 | KEY0 | IPU | 按键输入 0 |
| PB7 | KEY1 | IPU | 按键输入 1 |
| PB8 | BEEP | Out_PP | 蜂鸣器 |
| PB12 | SEG-e | Out_PP | 7段显示段 e (左下) |
| PB13 | SEG-d | Out_PP | 7段显示段 d (底部) |
| PB14 | SEG-dp | Out_PP | 7段显示小数点 |
| PB15 | SEG-c | Out_PP | 7段显示段 c (右下) |

### GPIOC (16 pins) - STM32F103C8T6特殊说明
| 引脚 | 功能 | 模式 | 用途 |
|------|------|------|------|
| PC13 | 板载LED | - | 板载状态灯（已占用，不可用） |

---

## 功能模块详解

### 1. ADC 模块 (模拟数据采集)
```
引脚: PB0 (ADC_Channel_8)
采样频率: 50ms 采样一次
分辨率: 12-bit (0~4095)
参考电压: 3.3V
转换公式: 电压(mV) = ADC_Value × 3300 / 4095
采样时间: 239.5 个周期 (高精度)
时钟: 12 MHz (PCLK2/6)
```

### 2. 7段显示模块 (4位数码管) - 共阳极模式
```
混合驱动配置 (非标准接线):
  PA0 = 千位数字位选
  PA1 = a段 (顶部横段)
  PA2 = f段 (左上竖段)
  PA3 = 百位数字位选
  PA4 = 十位数字位选
  PA5 = b段 (右上竖段)
  PA11 = g段 (中间横段)
  PA12 = 个位数字位选
  
  PB12 = e段 (左下竖段)
  PB13 = d段 (底部横段)
  PB14 = dp段 (小数点)  h
  PB15 = c段 (右下竖段)


⚠️ 警告：此硬件接线不符合标准7段数码管设计
  - 段驱动和位选引脚混合在GPIOA和GPIOB
  - 位选：PA0(千位), PA3(百位), PA4(十位), PA12(个位)
  - 段驱动：PA1(a), PA2(f), PA5(b), PA11(g), PB12(e), PB13(d), PB14(dp), PB15(c)

硬件类型: 共阳极 (SEG_COMMON_ANODE已定义)
  - 段驱动: 低电平=点亮，高电平=熄灭
  - 位选驱动: 低电平=激活该位，高电平=关闭该位

显示模式:
  - RAW模式: 显示 0~4095 的 ADC 原始值
  - 电压模式: 显示 X.XXV 格式的电压值

扫描频率: 1ms 刷新一次 (4位动态扫描，帧率 ~250Hz)
  - SEG_Task()必须在主循环中每1ms调用一次
  - 每次调用切换到下一位显示
  - 关键修复：每次刷新前关闭所有位选，防止重影

启动测试序列 (20秒):
  - 第0-5秒: D0位显示0-9 (每数字500ms)
  - 第5-10秒: D1位显示0-9
  - 第10-15秒: D2位显示0-9
  - 第15-20秒: D3位显示0-9
  - 用于诊断位选和段码映射是否正确
```

### 3. 按键输入模块
```
KEY0: PB6 (上拉输入) - STM32F103C8T6适配
KEY1: PB7 (上拉输入) - STM32F103C8T6适配

特性:
  - 内部上拉, 按下时低电平 (0)
  - 防抖延迟: 10ms
  - 返回值: KEY0_PRES=1, KEY1_PRES=2, 无按下=0
  - 注意: 避开PC13板载LED，改用GPIOB引脚
```

### 4. LED 指示灯
```
LED0: PB5 (推挽输出)
  - 按键切换显示模式时无效
  - 报警时闪烁 (500ms 周期)
  - 平时熄灭

LED1: PB1 (推挽输出) - STM32F103C8T6适配
  - 当前无使用 (预留)
  - 注意: 改用PB1避开PC13板载LED
```

### 5. 蜂鸣器驱动 (高电平驱动模式)
```
BEEP: PB8 (推挽输出)
  - 高电平驱动: GPIO_SetBits() 打开蜂鸣器
  - 低电平关闭: GPIO_ResetBits() 关闭蜂鸣器
  - 宏定义:
    * BEEP_ON()  → GPIO_SetBits(GPIOB, GPIO_Pin_8)
    * BEEP_OFF() → GPIO_ResetBits(GPIOB, GPIO_Pin_8)
  - 报警时与 LED0 同步闪烁 (500ms 周期)
```

### 6. UART 串口通信
```
UART1 配置:
  TX: PA9 (复用推挽输出)
  RX: PA10 (浮空输入)
  波特率: 115200 bps
  数据位: 8 bit
  停止位: 1 bit
  校验位: 无

应用:
  - 报警信息输出
  - 调试日志
  - printf 重定向支持
```

---

### 硬件引脚约束

### GPIOA 时钟
- 需要使能: `RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)`
- 用途模块: 7段显示, UART1

### GPIOB 时钟
- 需要使能: `RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE)`
- 用途模块: ADC, LED0, BEEP, 7段显示位选

### GPIOC 时钟 (STM32F103C8T6适配)
- PC13为板载LED，已被占用
- 本项目未使用其他GPIOC引脚

### USART1 时钟
- 需要使能: `RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE)`
- 波特率寄存器设置: 115200 baud

### ADC1 时钟
- 需要使能: `RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE)`
- ADC 时钟分频: `RCC_ADCCLKConfig(RCC_PCLK2_Div6)` (72MHz / 6 = 12MHz)

---

## GPIO 初始化顺序 (main.c 中执行)

```c
1. delay_init()              // 延时初始化 (必须)
2. NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2)  // 中断优先级配置
3. uart_init(115200)         // UART1 初始化 (PA9/PA10)
4. LED_Init()                // LED 初始化 (PB5, PB1)
5. KEY_Init()                // 按键初始化 (PB6, PB7) - STM32F103C8T6适配
6. BEEP_Init()               // 蜂鸣器初始化 (PB8)
7. ADC1_Init()               // ADC 初始化 (PB0)
8. SEG_Init()                // 7段显示初始化 (PA0~7, PB12~15)
```

---

## 电气特性

| 项目 | 规格 |
|------|------|
| 工作电压 | 3.3V |
| GPIO 输出驱动能力 | 25mA (推挽) |
| ADC 参考电压 | 3.3V |
| ADC 采样精度 | 12-bit (1.25mV/LSB) |
| UART 波特率 | 115200 baud |

---


### 扩展建议
- 如需增加传感器: 可使用 ADC 的其他通道 (ADC_Channel_0~17)
- 如需增加按键: 可扩展 KEY 模块复用 PE 的其他引脚
- 如需串行通信: 可启用 UART2 或 UART3

---

