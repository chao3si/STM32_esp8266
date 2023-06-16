#include "LCD_8080.h"
#include "delay.h"
#include "font.h"

u16  POINT_COLOR = BLACK;
u16  BACK_COLOR = WHITE;  


void LCD_FSMC_Init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOE, ENABLE); 
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_BL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_RD | LCD_WR | LCD_DB0 | LCD_DB1 | LCD_DB2 | LCD_DB3 | LCD_DB13 | LCD_DB14 | LCD_DB15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_RS | LCD_CS;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = LCD_DB4 | LCD_DB5 | LCD_DB6 | LCD_DB7 | LCD_DB8 | LCD_DB9 | LCD_DB10 | LCD_DB11 | LCD_DB12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    


    FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef WriteTiming;
    FSMC_NORSRAMTimingInitTypeDef ReadTiming;

    WriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;
    WriteTiming.FSMC_AddressHoldTime = 0x00;            //没用到
    WriteTiming.FSMC_AddressSetupTime = 0x00;           //设置为15ns，但是性能问题设置为0,100ns
    WriteTiming.FSMC_DataSetupTime = 0x02;              //设置为15ns
    WriteTiming.FSMC_BusTurnAroundDuration = 0x00;      //没用到
    WriteTiming.FSMC_CLKDivision = 0x00;                //没用到
    WriteTiming.FSMC_DataLatency = 0x00;                //没用到


    ReadTiming.FSMC_AccessMode = FSMC_AccessMode_A;
    ReadTiming.FSMC_AddressHoldTime = 0x00;             //没用到
    ReadTiming.FSMC_AddressSetupTime = 0x01;            //本应设置为RD高电平持续时间90ns，但应性能问题设置为200ns
    ReadTiming.FSMC_DataSetupTime = 0x26;               //设置为355ns
    ReadTiming.FSMC_BusTurnAroundDuration = 0x00;       //没用到
    ReadTiming.FSMC_CLKDivision = 0x00;                 //没用到
    ReadTiming.FSMC_DataLatency = 0x00;                 //没用到

    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
    FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &ReadTiming;             //读写时序
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &WriteTiming;                //写时序

    /*下边参数没用到*/
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable; 
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;  
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;   
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable; 

    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

void LCD_Init(void)
{
    LCD_FSMC_Init();

    LCD_WriteCommand(0xCF);
    LCD_WriteData(0x00);
    LCD_WriteData(0xC1);
    LCD_WriteData(0X30);
    LCD_WriteCommand(0xED);
    LCD_WriteData(0x64);
    LCD_WriteData(0x03);
    LCD_WriteData(0X12);
    LCD_WriteData(0X81);
    LCD_WriteCommand(0xE8);
    LCD_WriteData(0x85);
    LCD_WriteData(0x10);
    LCD_WriteData(0x7A);
    LCD_WriteCommand(0xCB);
    LCD_WriteData(0x39);
    LCD_WriteData(0x2C);
    LCD_WriteData(0x00);
    LCD_WriteData(0x34);
    LCD_WriteData(0x02);
    LCD_WriteCommand(0xF7);
    LCD_WriteData(0x20);
    LCD_WriteCommand(0xEA);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteCommand(0xC0);       //Power control
    LCD_WriteData(0x1B);      //VRH[5:0]
    LCD_WriteCommand(0xC1);       //Power control
    LCD_WriteData(0x01);      //SAP[2:0];BT[3:0]
    LCD_WriteCommand(0xC5);       //VCM control
    LCD_WriteData(0x30);      //3F
    LCD_WriteData(0x30);      //3C
    LCD_WriteCommand(0xC7);       //VCM control2
    LCD_WriteData(0XB7);
    LCD_WriteCommand(0x36);       // Memory Access Control
    LCD_WriteData(0x48);
    LCD_WriteCommand(0x3A);
    LCD_WriteData(0x55);
    LCD_WriteCommand(0xB1);
    LCD_WriteData(0x00);
    LCD_WriteData(0x1A);
    LCD_WriteCommand(0xB6);       // Display Function Control
    LCD_WriteData(0x0A);
    LCD_WriteData(0xA2);
    LCD_WriteCommand(0xF2);       // 3Gamma Function Disable
    LCD_WriteData(0x00);
    LCD_WriteCommand(0x26);       //Gamma curve selected
    LCD_WriteData(0x01);
    LCD_WriteCommand(0xE0);       //Set Gamma
    LCD_WriteData(0x0F);
    LCD_WriteData(0x2A);
    LCD_WriteData(0x28);
    LCD_WriteData(0x08);
    LCD_WriteData(0x0E);
    LCD_WriteData(0x08);
    LCD_WriteData(0x54);
    LCD_WriteData(0XA9);
    LCD_WriteData(0x43);
    LCD_WriteData(0x0A);
    LCD_WriteData(0x0F);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteCommand(0XE1);       //Set Gamma
    LCD_WriteData(0x00);
    LCD_WriteData(0x15);
    LCD_WriteData(0x17);
    LCD_WriteData(0x07);
    LCD_WriteData(0x11);
    LCD_WriteData(0x06);
    LCD_WriteData(0x2B);
    LCD_WriteData(0x56);
    LCD_WriteData(0x3C);
    LCD_WriteData(0x05);
    LCD_WriteData(0x10);
    LCD_WriteData(0x0F);
    LCD_WriteData(0x3F);
    LCD_WriteData(0x3F);
    LCD_WriteData(0x0F);
    LCD_WriteCommand(0x2B);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x01);
    LCD_WriteData(0x3f);
    LCD_WriteCommand(0x2A);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0x00);
    LCD_WriteData(0xef);
    LCD_WriteCommand(0x11);       //Exit Sleep
    delay_ms(120);
    LCD_WriteCommand(0x29);       //display on

    LCD_SetBL();
    LCD_SetSandir(L2R_U2D);
    LCD_Clear();
}

void LCD_WriteCommand(u16 Command)
{
    LCD->LCD_Cmd = Command;
}

void LCD_WriteData(u16 Data)
{
    LCD->LCD_Data = Data;
}

u16 LCD_ReadData(void)
{
    volatile u16 ret;
    ret = LCD->LCD_Data;
    return ret;
}

void LCD_SetSandir(u16 dir)
{
    LCD_WriteCommand(Memory_Access_Control);
    LCD_WriteData(dir);
}

void LCD_SetCursor(u16 x, u16 y)
{
    LCD_WriteCommand(Column_Address_Set);
    LCD_WriteData(x >> 8);
    LCD_WriteData(x & 0xFF);
    LCD_WriteCommand(Page_Address_Set);
    LCD_WriteData(y >> 8);
    LCD_WriteData(y & 0xFF);
}

void LCD_SetWindow(u16 x, u16 y, u16 width, u16 height)
{
    u16 x_ = x + width - 1;
    u16 y_ = y + height - 1;
    
    LCD_WriteCommand(Column_Address_Set);
    LCD_WriteData(x >> 8);
    LCD_WriteData(x & 0xFF);
    LCD_WriteData(x_ >> 8);
    LCD_WriteData(x_ & 0xFF);
    LCD_WriteCommand(Page_Address_Set);
    LCD_WriteData(y >> 8);
    LCD_WriteData(y & 0xFF);
    LCD_WriteData(y_ >> 8);
    LCD_WriteData(y_ & 0xFF);
}

void LCD_SetBL(void)
{
    GPIO_SetBits(GPIOB, LCD_BL);
}

void LCD_WriteGramPrepare(void)
{
    LCD_WriteCommand(Memory_Write);
}

void LCD_Clear(void)
{
    u32 total = LCD_Width * LCD_Height;
    u32 i;

    LCD_SetWindow(0, 0, LCD_Width, LCD_Height);
    LCD_WriteGramPrepare();
    for(i = 0; i < total; i++)
    {
        LCD_WriteData(BACK_COLOR);
    }
}

void LCD_ReadID(u8 *ID)
{
    LCD_WriteCommand(Read_ID4);
    LCD_ReadData();
    ID[0] = LCD_ReadData();
    ID[1] = LCD_ReadData();
    ID[2] = LCD_ReadData();
}

void LCD_DrawPoint(u16 x, u16 y, u16 PointColor)
{
    LCD_SetCursor(x, y);
    LCD_WriteGramPrepare();
    LCD_WriteData(PointColor);
}

void LCD_ShowChar(u16 x, u16 y, u8 character, u16 PointColor)
{
    //设置字符显示       20行30列
    //边界条件处理
    x = x * char_width;
    y = y * char_height;
    //LCD_SetCursor(x, y);

    character = character - ' ';
    const u8 *temp = asc2_1608[character];
    u16 temp_y = y; 
    u16 i, j;

    for(i = 0; i < char_width * char_height / 8; i++)
    {
        u8 cur = temp[i];
        for(j = 0; j < char_width; j++)
        {
            if((cur & 0x80) != 0)
            {
                LCD_DrawPoint(x, y, PointColor);
            }else{
                LCD_DrawPoint(x, y, BACK_COLOR);
            }
            cur <<= 1;
            y++;
        }
        if(y - temp_y == char_height)
        {
            x++;
            y = temp_y;
        }
    }
}

void LCD_ShowString(u16 x, u16 y, u8 *string, u16 PointColor)
{
    //LCD_SetCursor(x, y);
    
    while(*string != '\0')
    {
        LCD_ShowChar(y, x, *string, PointColor);
        y++;
        string++;
    }
}

u32 LCD_Pow(u32 x, u32 y)
{
    u32 res = 0x01;
    while(y--)
    {
        res *= x;
    }
    return res;
}

void LCD_ShowNum(u16 x, u16 y, u32 num, u16 PointColor)
{
    u32 num_ = num;
    u8 len = 0;
    if(num == 0) len = 1;
    while(num_ != 0)
    {
        num_ /= 10;
        len++;
    }

    for(u8 i = 0; i < len; i++)
    {
        LCD_ShowChar(y + i, x, num / LCD_Pow(10, len - i - 1) % 10 + '0', PointColor);
    }
}

void LCD_ShowHexNum(u16 x, u16 y, u32 hexnum, u16 PointColor)
{
    u32 hexnum_ = hexnum;
    u8 len = 0;
    if(hexnum == 0) len = 2;
    while(hexnum_ != 0)
    {
        hexnum_ /= 16;
        len++;
    }

    for(u8 i = 0; i < len; i++)
    {
        u8 temp = hexnum / LCD_Pow(16, len - i - 1) % 16;
        if(temp < 10)
        {
            LCD_ShowChar(y + i, x, temp + '0', PointColor);
        }
        else{
            LCD_ShowChar(y + i, x, temp - 10 + 'A', PointColor);
        }
    }
}






