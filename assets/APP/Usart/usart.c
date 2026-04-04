#include "usart.h"
#include "protocol.h"

// 串口1初始化函数
// bound: 波特率 (如 9600)
// 串口通信是嵌入式开发调试和通信的重要手段
void uart1_Init(u32 bound) {

  // 结构体变量声明
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  // 1. 开启时钟
  // USART1挂载在APB2总线上，GPIOA也挂载在APB2总线上
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

  // 2. 配置GPIO引脚
  // USART1_TX (发送端) -> GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode =
      GPIO_Mode_AF_PP; // 复用推挽输出 (交由串口外设控制引脚电平)
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // USART1_RX (接收端) -> GPIOA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode =
      GPIO_Mode_IN_FLOATING; // 浮空输入 (这是STM32F1标准库推荐的RX配置)
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // 3. 配置USART外设参数
  USART_InitStructure.USART_BaudRate = bound;                 // 设置波特率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b; // 8位数据位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;      // 1位停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;         // 无奇偶校验
  USART_InitStructure.USART_HardwareFlowControl =
      USART_HardwareFlowControl_None; // 无流控
  USART_InitStructure.USART_Mode =
      USART_Mode_Rx | USART_Mode_Tx;        // 同时开启接收和发送
  USART_Init(USART1, &USART_InitStructure); // 初始化串口1结构体

  // 4. 开启串口接收中断
  // 当RX寄存器非空(RXNE)时产生中断，表示收到数据
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  // 5. 配置NVIC中断优先级
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =
      3;                                             // 抢占优先级3 (较低)
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; // 子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // 6. 使能串口1 (开启外设)
  USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler(void) // 串口1中断服务程序
{
  // 检查中断标志
  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
    // 读取接收到的字节，同时会自动清除 RXNE 标志
    uint8_t receive_data = USART_ReceiveData(USART1);

    // 将数据交给协议处理模块，判断是否是合法指令
    Protocol_ProcessByte(receive_data);
  }
}

// AC5 重定向
// 重定向 printf 函数到串口1
// 这样使用 printf(...) 时，数据会通过串口1发送出去，方便调试
// 支持 ARM Compiler 5 和 6 以及 MicroLIB

// AC5/AC6 重定向逻辑
#if !defined(__MICROLIB)

#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION < 6000000)
// ARM Compiler 5
#pragma import(__use_no_semihosting)
// 标准库需要的支持函数
struct __FILE {
  int handle;
};

FILE __stdout;
// 定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x) { x = x; }
// 重定义fputc函数
int fputc(int ch, FILE *f) {
  // 等待发送缓冲区为空 (TXE)
  while ((USART1->SR & 0X40) == 0)
    ;
  USART1->DR = (u8)ch; // 写入数据寄存器及发送
  return ch;
}
#else
// ARM Compiler 6 (AC6)
#include <rt_sys.h>
__asm(".global __use_no_semihosting");

void _sys_exit(int x) {
  (void)x;
  while (1)
    ;
}

void _ttywrch(int ch) {
  while ((USART1->SR & 0X40) == 0)
    ;
  USART1->DR = (u8)ch;
}

int fputc(int ch, FILE *f) {
  (void)f;
  while ((USART1->SR & 0X40) == 0)
    ;
  USART1->DR = (u8)ch;
  return ch;
}

FILE __stdout;
FILE __stdin;
FILE __stderr;

int __stdout_putchar(int ch) {
  while ((USART1->SR & 0X40) == 0)
    ;
  USART1->DR = (u8)ch;
  return ch;
}
#endif

#else
// 使用 MicroLIB (Keil中勾选MicroLIB时使用此段)
int fputc(int ch, FILE *f) {
  (void)f;
  while ((USART1->SR & 0X40) == 0)
    ;
  USART1->DR = (u8)ch;
  return ch;
}
#endif
