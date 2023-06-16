#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "usart.h"
#include "SDcard.h"
#include "esp8266.h"
#include "queue.h"
#include "protocol.h"
#include "flash.h"
/*
用SPI1做SD卡的通讯接口会出现极其莫名其妙的错误
*/

void SD_USART_TEST()
{
    uint8_t buf[512];
    
    for(int i = 0; i < 1; i++)
    {
		for(int j = 0; j < 512; j++)
        {
            buf[i*512+j] = 0xFC;
		}
    }
    uint8_t w_s = SD_WriteDisk(buf, 2, 1);
    printf("w_s:%x \r\n", w_s);

    for(int i = 0; i < 1; i++)
    {
		for(int j = 0; j < 512; j++)
		{
			buf[i*512+j] = 0x01;
		}
    }
		
    uint8_t r_s = SD_ReadDisk(buf, 2, 1);;
    printf("r_s:%x \r\n", r_s);

    for(int i = 0; i < 1; i++)
    {
        printf("\r\nsector =====================\r\n");
        for(int j = 0; j < 512; j++)
        {
             printf("%x ", buf[i*512+j]);
        }
        printf("\r\nsector =====================\r\n");
    }

    uint32_t capcity = SD_GetSectorCount();
    printf("capcity is %d\r\n", capcity);

    uint8_t buf1[4] = {'A', 'T', '\r', '\n'};

    send_data(1,buf1,4);
    while(1)
    {
       if(rev_flag == 1)
       {
           printf("%s", rev_buf);
           rev_flag = 0;
           rev_index = 0;
       }
        delay_ms(500);
    }
}

void queue_test()
{
    uint8_t buf[512];
    xQueue Queue;
    QueueInit(&Queue, buf, 512);

    for(int i = 0; i < 200; i++)
    {
        EnQueue(&Queue, 'A');
    }

    for(int i = 0; i < 200; i++)
    {
        printf("%c", DeQueue(&Queue));
    }
    printf("%c\r\n", DeQueue(&Queue));
    printf("%d\r\n", Queue.rear);
    printf("%d\r\n", Queue.front);
    printf("%d\r\n", QueueLen(&Queue));
    for(int i = 0; i < 400; i++)
    {
        EnQueue(&Queue, 'B');
    }
	printf("%d\r\n", Queue.rear);
    printf("%d\r\n", Queue.front);
    printf("%d\r\n", QueueLen(&Queue));
		for(int i = 0; i < 400; i++)
    {
        printf("%c", DeQueue(&Queue));
    }
		printf("\r\n");
    printf("%d\r\n", Queue.rear);
    printf("%d\r\n", Queue.front);
    
}

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
	uart1_init(115200);
    uart2_init(115200);
    delay_init();
    //SD_Init();
    //queue_test();
    //SD_USART_TEST();


    printf("%d \r\n", Esp8266SetChipMode(eMutipleLink, 2000));
    delay_ms(500);
    // printf("%d \r\n", BspEsp8266WifiMode(eApStationMode, 2000));
    // delay_ms(500);
    // printf("%d \r\n", Esp8266ConnectWifi("CHAOSI", "31415927", 5000));
    // delay_ms(500);
    printf("%d \r\n", Esp8266ConnectServer(0, "192.168.137.1", 8888, 2000));
    delay_ms(500);
    DataDownload();


    while(1)
    {
        delay_ms(500);
    }
}
