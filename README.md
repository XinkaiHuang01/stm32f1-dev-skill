<div align="center">

# stm32f1-dev

### AI 写的 STM32 代码跑不起来？这个 Skill 让它一次编译通过。
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

[快速开始](#快速开始) · [工作原理](#工作原理) · [编码规范](#编码规范) · [评估测试](#评估测试)

</div>

---

> [!TIP]
> 这是一个stm32初学者适用的agent skill，能解决大部分培训初期的项目，复杂项目暂未测试

> [!IMPORTANT]
> **安全说明**
>
> | 关注点 | 说明 |
> |--------|------|
> | 会做什么 | 执行 Keil 编译命令、修改 `.uvprojx` / `compile_flags.txt` 配置文件、在项目中创建 `.c` / `.h` 文件和目录 |
> | 不会做什么 | 不发送任何网络请求、不收集遥测数据、不离开本地环境 |
> | 如何撤销 | 删除 Skill 安装目录即可，项目中的代码和配置可手动回滚（git checkout） |

---

## 痛点

如果你用过 AI 工具写嵌入式代码，大概率遇到过这些情况：

- 生成的代码混用 HAL 库和标准库，根本编译不过
- 目录结构混乱，Keil 工程无法正确添加源文件
- 引脚随意指定，多个外设之间产生冲突
- 没有中断命名规范，启动文件对不上

**这不是你的问题。** 大多数 AI 工具对 STM32 裸机开发的理解停留在"能跑就行"，而电子设计竞赛训练和教学场景需要的是**可复现、可维护、一次编译通过**的代码。

这个 Skill 把一套完整的工程规范、标准库模板和自动编译验证机制捆绑在一起，让 AI Agent 在生成 STM32F1 代码时严格遵循约定，交付的不是"参考代码"而是**能直接烧录的工程**。

---

## 快速体验

在支持本 Skill 的 AI Agent 中（OpenCode、Claude Code、Antigravity、Codex 等），直接输入：

```
/stm32
```

Agent 会：
1. 询问你的芯片型号（如 STM32F103C8T6）
2. 介绍 Skill 的职能和使用方式
3. **声明编译器依赖**：此模板默认锁定 ARM Compiler 5 (AC5)，需确保 Keil 中已安装

确认芯片型号后，Agent 会自动创建完整的工程结构，包含标准外设库、目录结构和编译配置。

---

## 安装

将本 Skill 目录放置到对应 Agent 的 skills 路径下：

| Agent | 安装方式 |
|-------|----------|
| OpenCode / Antigravity | 复制到 `~/.gemini/antigravity/skills/stm32f1-dev/` |
| Claude Code | 通过 Claude Code 的 skill 机制安装 |
| Codex | 通过 Codex 的 skill 机制安装 |

安装后，Agent 会在检测到 STM32、Cortex-M3、Keil、OLED、定时器 等关键词时**自动触发**本 Skill。

---

## 快速开始

### 第一步：新建工程

告诉 Agent 你的需求：

```
我要新建一个名为 MyProject 的 STM32F103C8T6 裸机工程
```

Agent 会：
- 从 `assets/Template/` 物理拷贝 Keil 模板和 `compile_flags.txt`
- 复制完整标准外设库 (`assets/Lib/`) 到项目根目录
- 反馈文件创建状态

### 第二步：添加外设模块

```
帮我添加一个 OLED 显示模块
```

Agent 会：
- 询问屏幕尺寸（默认适配 128×64）
- 确认 I²C 引脚分配
- 从 `assets/APP/OLED/` 复制驱动文件
- 通过 `add_module.py` 安全注入 Keil 工程配置

### 第三步：编译验证

修改代码后，Agent 会**自动执行 Keil 命令行编译**，读取日志，如有错误则自我修复，直到 `0 Errors` 才向你汇报。

---

## 工作原理

本 Skill 通过以下机制确保 AI 交付质量：

```
用户请求 → Skill 规则匹配 → 模板拷贝/代码生成 → 安全配置注入 → 自动编译验证 → 交付
```

<details>
<summary><b>详细说明</b> — 核心机制与规范</summary>

### 工程目录结构

```
工程名/
├── compile_flags.txt            ← clangd 配置（IDE 智能提示）
├── User/
│   ├── main.c                   ← 用户主程序
│   ├── 工程名.uvprojx           ← Keil 工程文件
│   └── Objects/, Listings/      ← 编译产物
├── APP/
│   ├── 模块名/                  ← 每个外设独立子文件夹
│   │   ├── 模块名.c
│   │   └── 模块名.h
└── Lib/                         ← STM32 标准库（Core + StdPeriph）
```

### 技术选型

| 项目 | 选择 |
|------|------|
| 语言 | C（标准库函数，**严禁 HAL / LL 库**） |
| 编译器 | ARM Compiler 5 (AC5) — 默认锁定 |
| 类型系统 | 强制 `<stdint.h>`（`uint8_t`, `uint16_t` 等） |
| 命名规范 | 变量 `snake_case`，函数 `PascalCase`，宏 `UPPER_CASE` |
| 缩进 | 4 空格，禁止 Tab |

### 自动编译验证机制

每次代码生成或结构修改后，Agent 必须：

1. 调用 Keil 命令行：`UV4.exe -b User\Project.uvprojx -o build_log.txt`
2. 读取编译日志，检查是否有 Error
3. 如有错误 → 自动修复 → 重新编译 → 循环
4. 直到 `0 Errors` 才向用户汇报

### Keil 手动配置提示

以下内容 AI **无法自动完成**，需在 Keil 中手动设置：

- Target 选项卡：勾选 **Use MicroLIB**
- C/C++ 选项卡：添加预编译宏 `STM32F10X_MD,USE_STDPERIPH_DRIVER`
- Debug 选项卡：将调试器替换为实际的 J-Link / ST-Link，勾选 **Reset and Run**

### 内置模块资产

Skill 捆绑了以下可直接复用的模块：

| 模块 | 路径 | 说明 |
|------|------|------|
| delay | `assets/APP/delay/` | 精确延时函数 |
| Usart | `assets/APP/Usart/` | 串口通信（默认 PA9/PA10） |
| OLED | `assets/APP/OLED/` | SSD1306 驱动（128×64，I²C） |
| 标准库 | `assets/Lib/` | STM32F10x 标准外设驱动全集 |

### 引脚分配规则

涉及硬件引脚的代码生成时，Agent 必须：

1. **主动询问**用户期望的引脚分配
2. 如果引脚可由芯片型号默认得出（如 USART1 → PA9/PA10），自动配置后**明确声明**
3. 防止引脚资源冲突

### 按键模块特殊约束

- 必须使用 **外部中断 (EXTI)** 方式，严禁死循环轮询
- 必须包含**消抖 (Debounce)** 处理机制

</details>

---

## 编码规范

<details>
<summary><b>展开查看</b> — 完整编码风格要求</summary>

### 格式与排版

- 缩进：**4 空格**，严禁 Tab
- 单行长度：建议不超过 120 字符
- 大括号：K&R 风格（与语句同行）或独占一行并垂直对齐
- 空行：函数间 1-2 行，独立变量声明后加空行

### 空格规则

| 场景 | 规则 | 示例 |
|------|------|------|
| 关键字后 | 必须加空格 | `if (x > 0)` |
| 函数名与 `(` 之间 | 不得加空格 | `LED_Init()` |
| 双目/三目运算符两侧 | 必须加空格 | `a + b`, `x = y` |
| 单目运算符与操作数间 | 不得加空格 | `!flag`, `&addr` |
| 强制转换后 | 不得加空格 | `(uint8_t)var` |
| 结构体成员访问符前后 | 不得加空格 | `ptr->field`, `obj.field` |

### 注释要求

- 注释解释 **为什么这样设计**，而非代码字面含义
- 复杂函数头部必须说明：功能、参数、返回值
- 修改代码时同步更新注释

### 头文件保护

```c
#ifndef __OLED_H__
#define __OLED_H__

// ... 内容 ...

#endif /* __OLED_H__ */
```

### 中断服务函数

- 必须使用启动文件中规定的命名（如 `TIM3_IRQHandler`）
- 严禁在中断中使用 printf、delay 等耗时操作

</details>

---

## FAQ

**Q：支持哪些芯片型号？**

A：本 Skill 针对 STM32F1 系列（Cortex-M3 内核），如 STM32F103C8T6、STM32F103RCT6 等。新建工程时 Agent 会询问具体型号。

**Q：为什么强制使用标准库而不是 HAL 库？**

A：标准库 API 更直观、代码更简洁，适合教学和电赛场景。HAL 库抽象层次多，初学者理解成本高。

**Q：Keil 工程文件 `.uvprojx` 是怎么修改的？**

A：AI **不会**直接编辑这个复杂的 XML 文件。而是调用 Skill 自带的 `add_module.py` 安全注入脚本，确保不会损坏工程配置。

---

## 评估测试

本 Skill 包含 5 项评估用例，覆盖核心工作流触发、工程创建、模块生成、资产引入和编译验证等场景。详见 `evals/evals.json`。

---

## 许可证

[MIT License](LICENSE)

Copyright (c) 2026 Kevin Huang
