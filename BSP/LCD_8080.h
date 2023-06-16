#ifndef __LCD_8080_H
#define __LCD_8080_H

#include "stm32f10x.h"

#define LCD_CS                      GPIO_Pin_12
#define LCD_RS                      GPIO_Pin_0
#define LCD_WR                      GPIO_Pin_5
#define LCD_RD                      GPIO_Pin_4
#define LCD_DB0                     GPIO_Pin_14
#define LCD_DB1                     GPIO_Pin_15
#define LCD_DB2                     GPIO_Pin_0
#define LCD_DB3                     GPIO_Pin_1
#define LCD_DB4                     GPIO_Pin_7
#define LCD_DB5                     GPIO_Pin_8
#define LCD_DB6                     GPIO_Pin_9
#define LCD_DB7                     GPIO_Pin_10
#define LCD_DB8                     GPIO_Pin_11
#define LCD_DB9                     GPIO_Pin_12
#define LCD_DB10                    GPIO_Pin_13
#define LCD_DB11                    GPIO_Pin_14
#define LCD_DB12                    GPIO_Pin_15
#define LCD_DB13                    GPIO_Pin_8
#define LCD_DB14                    GPIO_Pin_9
#define LCD_DB15                    GPIO_Pin_10
#define LCD_BL                      GPIO_Pin_0


/*命令*/
#define Column_Address_Set                                  0x2A
#define Page_Address_Set                                    0x2B
#define Memory_Write                                        0x2C
#define Memory_Access_Control                               0x36
#define Read_display_identification_information             0x04
#define Read_ID4                                            0xD3
/*颜色*/
#define WHITE                                               0xFFFF
#define BLACK                                               0x0000
#define BLUE                                                0x001F
#define BRED                                                0XF81F
#define GRED                                                0XFFE0
#define GBLUE                                               0X07FF
#define RED                                                 0xF800
#define MAGENTA                                             0xF81F
#define GREEN                                               0x07E0
#define CYAN                                                0x7FFF
#define YELLOW                                              0xFFE0
#define BROWN                                               0XBC40  
#define BRRED                                               0XFC07  
#define GRAY                                                0X8430  
/*字库参数*/
#define char_width                                          8
#define char_height                                         16
/********/
extern u16  POINT_COLOR;    //画笔颜色
extern u16  BACK_COLOR;     //背景颜色
/*GRAM扫描方向*/
#define L2R_U2D                                             0x00

/*第一种写法*/
typedef struct LCD_Def
{
    volatile u16 LCD_Cmd;
    volatile u16 LCD_Data;
} LCD_TypeDef;

#define LCD_Base                                            ((u32)(0x6C000000 | 0x000007FE))
#define LCD                                                 ((LCD_TypeDef *)LCD_Base)
#define LCD_Width                                           240
#define LCD_Height                                          320


void LCD_FSMC_Init(void);
void LCD_Init(void);
void LCD_WriteCommand(u16 Command);
void LCD_WriteData(u16 Data);
void LCD_SetSandir(u16 dir);
void LCD_SetCursor(u16 x, u16 y);
void LCD_SetWindow(u16 x, u16 y, u16 width, u16 height);
void LCD_SetBL(void);
void LCD_WriteGramPrepare(void);
void LCD_Clear(void);
u16 LCD_ReadData(void);
void LCD_ReadID(u8 *ID);
void LCD_DrawPoint(u16 x, u16 y, u16 PointColor);
void LCD_ShowChar(u16 x, u16 y, u8 character, u16 PointColor);
void LCD_ShowString(u16 x, u16 y, u8 *string, u16 PointColor);
void LCD_ShowNum(u16 x, u16 y, u32 num, u16 PointColor);
void LCD_ShowHexNum(u16 x, u16 y, u32 hexnum, u16 PointColor);

#endif
