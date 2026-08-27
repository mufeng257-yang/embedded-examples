# 天猛星 MSPM0G3507 + ICM42688 SPI 通信注意事项

## 开发遇到的问题

ICM42688 使用 SPI 通信时，能够读取 `WHO_AM_I` 和姿态数据，但 yaw 存在明显误差：

- 转动约 90° 时，yaw 变化量明显偏大。
- 多次转动并回到原位置后，yaw 无法回到原值。
- 改用 I²C 通信后，同一套姿态解算能够正常工作。

## 原因

天猛星 MSPM0G3507 开发板的 SPI1 总线同时连接了板载 W25Q64 Flash：

| 信号 | MSPM0 引脚 | 用途 |
| --- | --- | --- |
| MISO | PB7 | ICM42688 与板载 W25Q64 共用 |
| MOSI | PB8 | ICM42688 与板载 W25Q64 共用 |
| SCLK | PB9 | ICM42688 与板载 W25Q64 共用 |
| ICM CS | PB5 | ICM42688 软件片选 |
| Flash CS | PB6 | 板载 W25Q64 片选 |

原工程只配置了 PB5，没有正确配置 PB6。PB6 未被明确拉高时，板载 W25Q64 可能与 ICM42688 同时参与 SPI 事务，并在共享的 PB7/MISO 上产生总线冲突。

原工程中的 PB5 在 GPIO 初始化后还默认为低电平，导致 ICM42688 在 SPI 初始化完成之前就处于选中状态。这些问题会造成 SPI 数据异常。数据可能仍然连续，`WHO_AM_I` 也可能正确，但陀螺仪采样值会受到影响，最终表现为 yaw 积分误差。

SysConfig 中还保留了 SPI 硬件 CS0 的 PA2 分配，但实际 ICM42688 使用 PB5 软件片选。PA2 在天猛星开发板上未正常引出，不应作为该器件的实际片选。

## 修改方法

### 1. 正确配置两个片选引脚

在 `empty.syscfg` 中进行以下配置：

- PB5 配置为 GPIO 输出，初始值为高电平，用作 ICM42688 CS。
- PB6 配置为 GPIO 输出，初始值为高电平，用于禁止板载 W25Q64。
- PB7、PB8、PB9 继续作为 SPI1 的 MISO、MOSI、SCLK。

生成后的初始化代码应在使能 GPIO 输出前，将 PB5 和 PB6 同时置高：

```c
DL_GPIO_setPins(GPIOB, CS_PIN_PIN | FLASH_CS_FLASH_PIN_PIN);
DL_GPIO_enableOutput(GPIOB, CS_PIN_PIN | FLASH_CS_FLASH_PIN_PIN);
```

### 2. 使用 PB5 手动控制 ICM42688

ICM42688 的每次 SPI 事务都使用 PB5 软件片选：

```c
#define SPI_CS(x) \
    ((x) ? DL_GPIO_setPins(CS_PORT, CS_PIN_PIN) \
         : DL_GPIO_clearPins(CS_PORT, CS_PIN_PIN))
```

事务时序：

```c
SPI_CS(0);
/* 发送寄存器地址并读写数据 */
SPI_CS(1);
```

PB6 在访问 ICM42688 期间必须始终保持高电平。只有需要访问板载 W25Q64 时，才能在确保 PB5 为高电平后拉低 PB6。

### 3. 统一选择 SPI 通信

`ICM42688/icm42688.c` 和 `ICM42688/icm42688.h` 中的通信方式宏应保持一致：

```c
#define ICM_USE_HARD_SPI
//#define ICM_USE_I2C
```

不要在不同文件中同时启用 SPI 和 I²C 宏。

### 4. 重新生成配置并构建

修改 `empty.syscfg` 后，必须使用 SysConfig 重新生成 `ti_msp_dl_config.c` 和 `ti_msp_dl_config.h`，不要手工编辑生成文件。随后重新构建 Keil 工程并下载固件。

## 验证要求

SPI 通信是否正常应至少验证以下内容：

1. ICM42688 已连接并正确供电。
2. `WHO_AM_I` 稳定返回 `0x47`。
3. 静止时三轴陀螺仪数据接近零且没有异常跳变。
4. 静止时加速度模长接近 1 g。
5. 水平旋转后返回原位置，yaw 能够基本回到初始值。

仅看到串口持续输出 YPR 不能证明 SPI 通信正常。传感器未连接、初始化失败或固件未正确更新时，必须先检查 `WHO_AM_I` 和原始六轴数据。

## yaw 使用限制

ICM42688 是六轴 IMU，不包含磁力计。它可以通过陀螺仪积分得到相对 yaw，但无法提供长期稳定的绝对航向。即使 SPI 通信完全正常，长时间运行仍可能产生缓慢漂移；需要绝对航向时，应增加磁力计或其他外部航向参考。
