#include "stm32f10x.h"
#include "MySPI.h"
#include "SDcard.h"

static uint8_t R1;
static uint8_t R3[5];
static uint8_t R7[5];

uint8_t wait_sd_ready()
{
    uint32_t cnt = 0;
    while(cnt < SD_WAIT_PROGRAMME)
    {
        if(MySPI_SwapByte(0xFF) == 0xFF)
        {
            return SD_OK;
        }
        cnt++;
    }
    return SD_ERR;
}

uint8_t SD_Select()
{
    MySPI_SS(0);
    if(wait_sd_ready() == SD_OK) //再次操作sd卡需要等待sd卡编程结束
    {
        return SD_OK;
    }
    MySPI_SS(1);
    return SD_ERR;
}

void SD_DisSelect()
{
    MySPI_SS(1);
    MySPI_SwapByte(0xFF);//不知道为什么
}

void R1_response()
{
    uint8_t retry = SD_WAIT_RESPONSE;
    do
    {
        R1 = MySPI_SwapByte(0xFF);
    } while((R1 & 0x80) && retry--);
}

void R3_response()
{
    uint8_t retry = SD_WAIT_RESPONSE;
    uint8_t R3_R1;
    do
    {
        R3_R1 = MySPI_SwapByte(0xFF);
    } while((R3_R1 & 0x80) && retry--);

    R3[0] = R3_R1;

    for(int i = 1; i <= 4; i++)
    {
        R3[i] = MySPI_SwapByte(0xFF); 
    }
}

void R7_response()
{
    uint8_t retry = SD_WAIT_RESPONSE;
    uint8_t R7_R1;
    do
    {
        R7_R1 = MySPI_SwapByte(0xFF);
    } while((R7_R1 & 0x80) && retry--);

    R7[0] = R7_R1;

    for(int i = 1; i <= 4; i++)
    {
        R7[i] = MySPI_SwapByte(0xFF); 
    }
}

uint8_t SD_GetResponse(uint8_t type)
{
    uint8_t retry = SD_WAIT_RESPONSE;
    
    while(MySPI_SwapByte(0xFF) != type)
    {
        retry--;
        if(retry <= 0)
        {
            return SD_ERR;
        }
    }
    return SD_OK;
}

uint8_t send_command(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t resp_type)
{
	SD_DisSelect();//取消上次片选

	if(SD_Select() == SD_ERR)
    {
        printf("send_command SD_Select err\r\n");
        return SD_ERR;//片选失效 
    }
	//发送
    MySPI_SwapByte(cmd | 0x40);//分别写入命令
    MySPI_SwapByte(arg >> 24);
    MySPI_SwapByte(arg >> 16);
    MySPI_SwapByte(arg >> 8);
    MySPI_SwapByte(arg);	  
    MySPI_SwapByte(crc); 

    switch (resp_type)
    {
    case SD_R1:
        R1_response();
        break;
    case SD_R3:
        R3_response();
        break;
    case SD_R7:
        R7_response();
        break;
    default:
        break;
    }

    return SD_OK;
	//if(cmd==CMD12)MySPI_SwapByte(0xff);//Skip a stuff byte when stop reading
}

uint8_t SD_Init()
{
    uint32_t retry = 0xFF;
    
    MySPI_Init();
    set_spi_speed(SPI_BaudRatePrescaler_256);
    
    for(int i = 0; i < 72; i++)
    {
        MySPI_SwapByte(0xFF);           //协议规定初始化过程，在power on那章有描述
    }
		
    send_command(CMD0, 0, 0x95, SD_R1);      //根据2.0协议，在发送CMD0时片选为0就进入spi模式，进入IDLE状态
    if(R1 != 0x01)
    {
        return SD_CMD0_ERR;
    }

    send_command(CMD8, 0x1AA, 0x87, SD_R7);       //这部分不太懂
    if(R7[3] != 0x01 || R7[4] != 0xAA)
    {
        //printf("r7_2 = %x, r7_3 = %x\r\n", R7[3], R7[4]);
        return SD_CMD8_ERR;
    }
		
    do
    {
        send_command(CMD55, 0, 0X01, SD_R1);
        send_command(CMD41, 0x40000000, 0X01, SD_R3);   //根据协议发送HCS=1表明主机支持大容量卡
    }while((R1 & 0x01) && retry--); //根据协议描述重复发送ACM41直到idle = 0
    if(R1 != 0x00)
    {
        return SD_ACMD41_ERR;
    }

    send_command(CMD58 ,0 , 0X01, SD_R3);   //根据协议描述判断CSS=1，是SD2.0卡
    if((R3[0] & 0x40) != 0)
    {
        return SD_CMD58_ERR;
    }

    SD_DisSelect();
    set_spi_speed(SPI_BaudRatePrescaler_4);

    printf("SD 2.0 card init successfully\r\n");
    return SD_OK;
}

uint8_t SD_RecvData(uint8_t *buf, uint32_t len)
{
    if(SD_GetResponse(0xFE) == SD_ERR)
    {
        return SD_ERR;
    }

    while(len--)        
    {
        *buf=MySPI_SwapByte(0xFF);
        buf++;
    }

    //下面是2个伪CRC（dummy CRC）
    MySPI_SwapByte(0xFF);
    MySPI_SwapByte(0xFF);		

    return SD_OK;
}

uint8_t SD_ReadDisk(uint8_t *buf, uint32_t sector, uint32_t count)
{
    if(count == 1)
    {
        send_command(CMD17, sector, 0x01, SD_R1);
        if(R1 != 0x00)
        {
            return SD_CMD17_ERR;
        }
        uint8_t temp = SD_RecvData(buf, 512);

				return temp;

    }
    else                //多块读取
    {
        send_command(CMD18, sector, 0X01, SD_R1);//连续读命令
        if(R1 != 0x00)
        {
            return SD_CMD18_ERR;
        }
        while(count--)
        {
            SD_RecvData(buf, 512);
            buf += 512;
        }
        send_command(CMD12, 0, 0X01, SD_R1);
    }

	SD_DisSelect();
    
	return SD_OK;
}

uint8_t SD_SendBlock(uint8_t *buf,uint8_t cmd)
{	
	u16 t;		  	  
	if(wait_sd_ready() == SD_ERR)
    {
        return SD_ERR;
    }
    
	MySPI_SwapByte(cmd);
	if(cmd!=0XFD)//不是结束指令
	{
		for(t = 0; t < 512; t++)
        {
            MySPI_SwapByte(buf[t]);//提高速度,减少函数传参时间
        }
	    MySPI_SwapByte(0xFF);//忽略crc
	    MySPI_SwapByte(0xFF);

		t=MySPI_SwapByte(0xFF);//接收响应
		if((t&0x1F)!=0x05)
        {
            return SD_SendBlock_ERR;//响应错误	
        }						  					    
	}						 									  					    
    return SD_OK;//写入成功
}

uint8_t SD_WriteDisk(uint8_t *buf, uint32_t sector, uint32_t count)
{
    if(count == 1)
    {
        send_command(CMD24, sector, 0x01, SD_R1);
        if(R1 != 0x00)
        {
            return SD_CMD24_ERR;
        }
        uint8_t temp = SD_SendBlock(buf, SD_WRITE_ONE_BLOCK_TOKEN);
				return temp;
    }
    else
    {
        send_command(CMD55, 0, 0X01, SD_R1);	
		send_command(CMD23, count, 0X01, SD_R1);//发送指令	
		
 		send_command(CMD25, sector, 0X01, SD_R1);//连续读命令
		if(R1 != 0x00)
		{
            return SD_CMD25_ERR;
		}			
        while(count--)
        {
            SD_SendBlock(buf, SD_WRITE_MUTI_BLOCK_TOKEN);//接收512个字节	 
            buf+=512;
        }
			
        SD_SendBlock(0, SD_STOP_MUTI_BLOCK_TOKEN);//接收512个字节 
    }

    SD_DisSelect();

    return SD_OK;
}

uint8_t SD_GetCSD(u8 *csd_data)
{
    send_command(CMD9, 0, 0x01, SD_R1);//发CMD9命令，读CSD
    if(R1 != 0x00)
	{
    	return SD_ERR;
    }
    
    SD_RecvData(csd_data, 16);//接收16个字节的数据 

	SD_DisSelect();//取消片选
	
    return SD_OK;
}  

uint32_t SD_GetSectorCount(void)
{
    uint8_t csd[16];
    uint32_t Capacity;  
    //uint8_t n;
	uint16_t csize;  					    
	//取CSD信息，如果期间出错，返回0
    if(SD_GetCSD(csd)!=0) return SD_ERR;	    
    //如果为SDHC卡，按照下面方式计算

	csize = csd[9] + ((uint16_t)csd[8] << 8) + 1;
	Capacity = (uint32_t)csize << 10;//得到扇区数	 		   
    
    return Capacity;
}
