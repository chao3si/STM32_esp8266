#include "stm32f10x.h"
#include "usart.h"
#include "delay.h"


#define MAX_NUM         600
uint8_t rev_buf[MAX_NUM];
uint8_t rev_flag;
uint16_t rev_index; 


uint8_t buf[512];
xQueue Queue;
    

//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 


void uart1_init(uint32_t BoundRate)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;

    USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate            = BoundRate;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);


	NVIC_InitTypeDef NVIC_InitStructure = {
        .NVIC_IRQChannel = USART1_IRQn,
        .NVIC_IRQChannelPreemptionPriority = 0x02,
        .NVIC_IRQChannelSubPriority = 0x02,
        .NVIC_IRQChannelCmd = ENABLE,
    };

    NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

void uart2_init(uint32_t bound)
{
	GPIO_InitTypeDef GPIO_StructInit;
	NVIC_InitTypeDef NVIC_StructInit;
	USART_InitTypeDef USART_StructInit;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //开启两个时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	GPIO_StructInit.GPIO_Pin = GPIO_Pin_2;
	GPIO_StructInit.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_StructInit.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_StructInit);

	GPIO_StructInit.GPIO_Pin = GPIO_Pin_3;
	GPIO_StructInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;

	GPIO_Init(GPIOA, &GPIO_StructInit);

	NVIC_StructInit.NVIC_IRQChannel = USART2_IRQn;
	NVIC_StructInit.NVIC_IRQChannelCmd = ENABLE;
	NVIC_StructInit.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_StructInit.NVIC_IRQChannelSubPriority = 0x02;

	NVIC_Init(&NVIC_StructInit);

	USART_StructInit.USART_BaudRate = bound;
	USART_StructInit.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_StructInit.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_StructInit.USART_Parity = USART_Parity_No;
	USART_StructInit.USART_StopBits = USART_StopBits_1;
	USART_StructInit.USART_WordLength = USART_WordLength_8b;

	USART_DeInit(USART2);
	USART_Init(USART2, &USART_StructInit);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, ENABLE);

	/*初始化循环队列*/
	QueueInit(&Queue, buf, 512);
}

void CleanBuf(uint8_t usart_id)
{
    if(usart_id == 1)
	{
		QueueClean(&Queue);
	}
}

void USART1_IRQHandler(void)
{

}

void USART2_IRQHandler(void)
{
	uint8_t rev_data;

	if(USART_GetFlagStatus(USART2, USART_IT_RXNE) != RESET)
	{
		rev_data = USART_ReceiveData(USART2);
		EnQueue(&Queue, (char)rev_data);		//接收一个字节存储在环形队列中
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}
}

uint16_t Usart2RecieveOneFrameData(uint8_t usart_id, uint8_t *buf, uint16_t timeout) 	//接收数据，超过1ms没数据代表一帧数据接收结束
{
	uint16_t rxlen = 0;
	if(usart_id == 1)
	{
		while(QueueLen(&Queue) == 0)
		{
			timeout--;
			delay_ms(1);
			if(timeout == 0)
			{
				return NULL;
			}
		}

		do
		{
			rxlen = QueueLen(&Queue);
			delay_ms(1);
		} while(rxlen != QueueLen(&Queue));

		for(uint16_t i = 0; i < rxlen; i++)
		{
			buf[i] = DeQueue(&Queue);
			//printf("%02x ", buf[i]);
		}
	}

	return rxlen;
}

void send_byte(uint8_t data)
{
    USART_SendData(USART2, data);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
}

/*直接使用该函数，需要用Microlib，有可能导致程序出问题*/
// int fputc(int ch, FILE *f)
// {
//     USART_SendData(USART1, ch);
//     while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);
//     return ch;
// }

void send_data(uint8_t usart_id, uint8_t *data, uint16_t len)
{
    if(usart_id == 1)
	{
		uint16_t i;
		for (i = 0; i < len; i++)
		{
			send_byte(data[i]);
		}
	}
}

void send_string(char* string)
{
    uint16_t i;
    for (i = 0; string[i] != '\0'; i++)
    {
        send_byte(string[i]);
    }
}

