---
name: stm32f1-dev
description: Create, modify, and manage STM32F1 microcontroller projects adhering strictly to the general engineering rules. Trigger this skill ALWAYS when the user mentions STM32, STM32F1, Cortex-M3, bare-metal C, embedded systems, Keil, MDK, standard peripheral library, OLED, LED, timer, interrupts, or when they ask to create a new module/project. Use this skill proactively for any embedded C file generation.
---

# STM32F1 单片机开发指南 (AI通用工程规则)

This skill provides comprehensive rules for STM32F1 Microcontroller development, specifically for AI Agents helping users with Electronic Design Contest training or Embedded C programming. Use these rules strictly when writing, refactoring, or generating code, and when managing project files.

## 1. 原则与技术栈 (Core Principles & Tech Stack)
*   **语言约定**: 回复、说明、注释、文档**必须使用简体中文**。代码标识符使用完全纯正英文（禁止拼音及中英混杂），错误信息/日志可使用英文。
*   **技术选型**: **所有 STM32 相关代码必须使用标准库函数 (Standard Peripheral Library)**。**严禁使用 HAL 库或 LL 库**。
*   **外设与驱动**: 为每个外设或功能完全独立创建 `.c` 和 `.h` 文件。
*   **复用原则**: 如果用户已经有了相同功能的代码，直接复用不需重新生成。

## 2. 工程新建与目录结构规范 (Project Structure)
严格遵循以下目录结构：
```text
工程名/                          ← 工程根目录
├── compile_flags.txt            ← clangd 配置（IDE 智能提示用）
├── User/
│   ├── main.c                   ← 用户主程序
│   ├── 工程名.uvprojx           ← Keil 工程文件
│   └── Objects/, Listings/      ← 编译产物目录
├── APP/
│   ├── 模块名/                  ← 每个模块必须独立存放于子文件夹
│   │   ├── 模块名.c
│   │   └── 模块名.h
└── Lib/                         ← STM32 标准库 (Core, STM32F10x_StdPeriph_Driver)
```
### 新建模块必做检查
1. 当需要在 `APP/` 下新建模块时，**必须创建一个以模块名为名的子文件夹**（如 `APP/OLED/oled.c` 和 `oled.h`）。模块文件夹名建议与内部 `.c/.h` 头文件名对应。
2. **必须同步修改 `User/工程名.uvprojx` 和根目录的 `compile_flags.txt`**，将新模块的路径添加进去。
   - **绝对禁止让 AI 直接使用正则或替换去篡改庞大复杂的 `.uvprojx` XML 文本！这会导致大概率报错废毁！**
   - 你必须调用本 Skill 目录下自带的安全注入脚本：执行 `python <这篇规则所在的物理目录>/assets/scripts/add_module.py <Project.uvprojx相对/绝对路径> <..\\APP\\模块名\\对应.c> <..\\APP\\模块名>` ，交由脚本自动完成无损注入。不要使用写死的路径，请利用命令寻找这个 python 脚本。
3. **强制复用官方资产 (Bundled Resources)**：禁止自行生造基础建设。新建工程或添加常见功能时，你必须利用该 Skill 捆绑资源：
   - 将 `assets/Lib/` 完整复刻至项目根目录 `Lib/` 中。
   - 将通用模块 `assets/APP/delay/`、`assets/APP/Usart/`、`assets/APP/OLED/` 等按需完整复刻至项目 `APP/` 内。
   - 并在必要时，修改 `compile_flags.txt` 及 `.uvprojx`。
4. **禁止直接删除模块**：发现不需要的外设驱动或代码时，必须询问用户是否同意删除，严禁擅自删除。

## 3. 编码风格规范 (Coding Style)
### 3.1 格式与排版
*   **缩进与单行长度**: 使用 **4个空格**，严禁使用 Tab。代码长度建议不超过 120 字符。
*   **大括号/空行**: 控制语句大括号独占一行并垂直对齐，或采用 K&R 风格（同行）。独立变量声明后加空行；函数间加 1-2 行空行；空行不应带有空格。
*   **空格规则**:
    - 关键字（如`if`, `for`, `while`）后**必须加空格**。
    - 函数名与左括号 `(` 之间**不得**加空格。
    - 双目/三目运算符 (`>`, `+`, `=` 等) 两侧**必须加空格**。
    - 单目运算符 (`!`, `&`, `++` 等) 与操作数之间**不得**加空格。
    - 自定义类型强制转换后**不得**加空格，如 `(uint8_t)var`。
    - 结构体成员访问符 `.` 和 `->` 前后**不得**加空格。

### 3.2 命名与类型
*   **变量名**: 小写+下划线 (`snake_case`)，如 `sys_tick`。仅循环变量允许用单字母。
*   **函数名**: 大驼峰 (`PascalCase`)，推荐 `模块名_动作` 格式，如 `LED_Init()`。
*   **宏/常量**: 全大写+下划线，如 `MAX_BUFFER_SIZE`。
*   **类型定义**: **强制**使用 ISO C99 `<stdint.h>` 的标准类型，如 `uint8_t`, `uint16_t`, `int32_t`。**严禁直接使用 int, short, char 等定义数值**（除非指代真实字符或简单计数）。
*   **自定义类型**: `struct`, `enum` 建议使用大驼峰并以 `_t` 结尾（如 `Clock_t`）。

### 3.3 注释及文件级别
*   **注释要点**: 注释用于解释 **为什么这样设计**，而不是代码的字面含义；修改代码时必须同步更新注释。
*   **函数注释**: 复杂函数头部必须说明功能、参数及返回值。
*   **防止重复包含**: 头文件必须含有 `#ifndef`, `#define`, `#endif` 保护，宏名建议类似于 `__FILENAME_H__`。
*   **中断服务**: 必须使用汇编启动文件中规定的命名，如 `TIM3_IRQHandler`。中断函数忌复杂逻辑（例如严禁使用 printf、delay 操作）。

## 4. AI 工作流特殊强制要求
### 4.1 `/stm32` 触发指引工作流
当用户通过 `/stm32` 触发使用本 skill 或要求新建项目时，必须严格按以下顺序进行交互：
1. **询问型号与介绍**: 询问用户所需开发的具体芯片型号（例如 STM32F103C8T6 等），并向用户简明扼要地介绍本 Skill 的使用方式及职能。**注意：你必须在最初的介绍中向用户进行“免责或前置声明”：明确告知用户此工程模板默认强依赖并锁定 ARM Compiler 5 (AC5) 编译器内核，提醒使用者确保本地 Keil 中已备有该版本编译器。**
2. **阻断等待**: 必须等待用户回复确认芯片型号。
3. **执行创建与情况反馈**: 用户确认并在完成工程创建后，你必须按以下逻辑创建文件：
   - 对于 Keil 的核心工程文件（`.uvprojx` / `.uvoptx`）和 `compile_flags.txt`：**绝对禁止**你凭空生成这些复杂的 XML/配置 文件！你必须直接从本 Skill 绑定的 `assets/Template/` 下将对应的模板工程原原本本地拷贝过去，然后重命名为用户要求的工程名。
   - 拷入基础库：将 `assets/Lib/` 复制过去。
   - 创建后向用户详细反馈创建状态。
   - 环境配置文件（`compile_flags.txt`）状态
4. **Keil 内部纯人工操作提示**: 告诉用户哪些配置是 AI 无法完成，必须在 Keil 软件中手动进行的（例如：在设 Target 选项卡勾选 Use MicroLIB，在 C/C++ 选项卡添加预编译宏 `STM32F10X_MD,USE_STDPERIPH_DRIVER` 以及将 Debug 替换为实际调试器如 J-Link/ST-Link 并勾选 Reset and Run 等）。

### 4.2 通用规约
- **新建工程前置询问**：任何情境下新建工程，必须先确认工程名称、存放位置及 Keil 工程文件名，不要盲目凭空编造直接写文件。
### 4.3 引脚分配与配置交互约束
涉及到复用或新建带有硬件引脚依赖的代码模块（如 OLED、按键、串口等）时：
- 创建或复用这些代码后的第一时间，**必须**主动向用户询问当前需要的真实硬件引脚。
- 如果该外设的引脚极其固定或可以由用户提供的芯片型号默认得出（例如 USART1 默认使用 PA9/PA10），你需要自动完成配置修改，**但之后必须明确向用户声明**（例如：“我已将 USART1 配置到 PA9/PA10 引脚，如与您的设计冲突请告诉我”）。严格防范用户不知情情况下的引脚资源冲突。
- **关于按键模块的特殊约束**：当用户要求编写或生成按键（KEY）模块代码时，你**必须**使用硬件的外部中断（EXTI）方式来捕获信号（严禁死循环轮询），并且在中断或处理逻辑中**必须**设计有按键消抖（Debounce）处理机制。
- **关于 OLED 模块的特殊约束**：自带的 OLED 资产代码默认是适配标准 `128x64` 分辨率的（如常规的 0.96寸 或 1.3寸 SSD1306）。当你为用户配置 OLED 功能时，必须向用户确认“您的屏幕是常见的 0.96寸 / 1.3寸 还是其他特殊尺寸？”。若尺寸或分辨率不同，你必须主动为其修改底层驱动文件中的 `OLED_GRAM` 大小及 Init 初始化序列参数，不可直接盲目套用。

### 4.4 自动化黑盒编译验证与自我修复机制 (Auto-Compile & Self-Correction)
为了确保不交付存在低级语法错误的半成品，在你为主程序生成代码、修改了现有工程结构或导入了新模块模块文件后，你必须执行“自动编译验证测试”：
1. **调用编译器**：运用你的命令行在工程根目录下执行 Keil 命令行编译指令（如果你不确定路径可优先尝试常见默认路径并把工程名替换对，如 `C:\Keil_v5\UV4\UV4.exe -b User\Project.uvprojx -j0 -o build_log.txt` ）。
2. **读盘排查**：在命令执行完成、收到返回响应后，调用工具去读取生成的编译日志 `build_log.txt`。
3. **闭环修复**：如果在日志中发现任意由你的代码导致的致命报错（如 `cannot open source input file`、`undefined symbol` 等 Error 级情况），**你严禁立刻中止并将报错情况丢给用户**。你必须基于报错行号在后台自动自我修复相关 `.h` / `.c` 和配置问题，然后再次发起 `-b` 编译，如此循环。直到日志里刷出 `0 Errors` 提示，或者尝试穷尽时，才能向用户汇报你所撰写功能的落地完成度！

- **强制口令声明**: 每次回复的最后，你**必须**附加上这句话（一字不差）：**"已按照 stm32f1-dev 生成"**。
