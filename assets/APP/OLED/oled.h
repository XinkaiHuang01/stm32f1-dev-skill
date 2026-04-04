#ifndef __OLED_H
#define __OLED_H

#include "stdlib.h"
#include "stm32f10x.h"

//-----------------OLED????----------------

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOE, GPIO_Pin_1) // SCL
#define OLED_SCLK_Set() GPIO_SetBits(GPIOE, GPIO_Pin_1)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOE, GPIO_Pin_2) // DIN
#define OLED_SDIN_Set() GPIO_SetBits(GPIOE, GPIO_Pin_2)

#define OLED_CMD 0  //???
#define OLED_DATA 1 //???

#define u8 unsigned char
#define u32 unsigned int

void OLED_Init(void);                                        // OLED???
void OLED_Clear(void);                                       // OLED??
void OLED_Refresh(void);                                     // OLED??
void OLED_DrawCircle(u8 x, u8 y, u8 r);                      // OLED??
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size1);            // OLED????
void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 size1);         // OLED?????
void OLED_ShowChinese(u8 x, u8 y, u8 num, u8 size1);         // OLED????
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size1);    // OLED????
void Draw_Line_Buffer(int x1, int y1, int x2, int y2);       // OLED??
void OLED_ShowPicture(u8 x0, u8 y0, u8 x1, u8 y1, u8 BMP[]); // OLED????
void Draw_Wave_Buffer(uint16_t *wave_data, uint16_t num_shift, u8 mode);
void OLED_Clear_area(u8 x, u8 y);
void OLED_DrawPoint(u8 x, u8 y);  // OLED??
void OLED_ClearPoint(u8 x, u8 y); // OLED??
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_WaitAck(void);
void Send_Byte(u8 dat);
void OLED_WR_Byte(u8 dat, u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_WR_BP(u8 x, u8 y);
void OLED_ShowChinese16(u8 x, u8 y, u8 index); // 显示16x16中文

#endif
