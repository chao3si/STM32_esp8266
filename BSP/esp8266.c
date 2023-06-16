/*
 * 
 *    ┏┓　　　┏┓
 *  ┏┛┻━━━┛┻┓
 *  ┃　　　　　　　┃
 *  ┃　　　━　　　┃
 *  ┃　＞　　　＜　┃
 *  ┃　　　　　　　┃
 *  ┃...　⌒　...　┃
 *  ┃　　　　　　　┃
 *  ┗━┓　　　   ┏━┛
 *      ┃　　　┃　
 *      ┃　　　┃
 *      ┃　　　┃
 *      ┃　　　┃  神兽保佑
 *      ┃　　　┃  代码无bug　　
 *      ┃　　　┃
 *      ┃　　　┗━━━┓
 *      ┃　　　　　　　┣┓
 *      ┃　　　　　　　┏┛
 *      ┗┓┓┏━┳┓┏┛
 *        ┃┫┫　┃┫┫
 *        ┗┻┛　┗┻┛
 */

#include "stm32f10x.h"
#include <string.h>
#include <stdlib.h>
#include "delay.h"
#include "usart.h"
#include "queue.h"
#include "esp8266.h"

static uint8_t usart_id = 1;

#define DEBUG    


char* Queuestrstr(char *s1, const char *s2, uint16_t len)
{
    s1[len] = '\0';
    return strstr(s1, s2);
}

void Esp8266SendAT(uint8_t usart_id, char *buf)
{
    uint16_t len;
    len = strlen(buf);
    send_data(usart_id, (uint8_t *)buf, len);
}

uint16_t Esp8266RecieveOneFrameData(uint8_t usart_id, uint8_t *buf, uint16_t timeout)
{
    return Usart2RecieveOneFrameData(usart_id, buf, timeout);
}





/**
 * @brief 处理esp8266传回的数据
 * @return {*}
 * @param {uint8_t} usart_id
 * @param {uint16_t} frame_count   接收多少帧
 * @param {uint16_t} timeout       等待帧数据最长时间
 */
uint8_t Esp8266Recieve(uint8_t usart_id, uint16_t frame_count, uint16_t timeout)
{
    uint8_t rxbuf[256] = {0};
    uint16_t rxlen = 0;
    uint16_t framelen = 0;

    uint8_t ret = 255;

    while((frame_count--) > 0)
    {
        CleanBuf(usart_id);
        framelen = Esp8266RecieveOneFrameData(usart_id, rxbuf + rxlen, timeout);
        rxlen += framelen;

        if(framelen == 0)
        {
            break;
        }

        if(strstr((char *)rxbuf, "OK") != NULL)
        {
            #ifdef DEBUG    
            ret = 0;
            #else
            return 0;	
            #endif
        }
        
        if(strstr((char *)rxbuf, ">") != NULL)
        {
            #ifdef DEBUG    
            ret = 1;
            #else
            return 1;	
            #endif
        }

        if(strstr((char *)rxbuf, "ERROR") != NULL)
        {
            #ifdef DEBUG    
            ret = 2;
            #else
            return 2;	
            #endif
        }
    }

    #ifdef DEBUG     	

    for(uint16_t i = 0; i < rxlen; i++)
    {
        printf("%c", rxbuf[i]);
    }
    if(ret != 255)
    {
        return ret;
    }
    #endif
    

    if(rxlen == 0)
    {
        return 3;
    }

    return 4;
}

uint8_t BspEsp8266WifiMode(Esp8266WifiMode mode, uint16_t timeout)
{
	switch(mode)
	{
		case 1:
			Esp8266SendAT(usart_id,"AT+CWMODE_DEF=1\r\n");
			break;
		case 2:
			Esp8266SendAT(usart_id,"AT+CWMODE_DEF=2\r\n");
			break;
		case 3:
			Esp8266SendAT(usart_id,"AT+CWMODE_DEF=3\r\n");
			break;
		default:
			break;
	}

    return Esp8266Recieve(usart_id, 1, timeout);
}

uint8_t Esp8266ConnectWifi(char *wifiName, char *password, uint16_t timeout)
{

	char cmdBuff[100] = {0};
	sprintf(cmdBuff, "AT+CWJAP=\"%s\",\"%s\"\r\n", wifiName, password);
	
	Esp8266SendAT(usart_id, cmdBuff);

    return Esp8266Recieve(usart_id, 10, timeout);
}

uint8_t Esp8266SetChipMode(ESP8266ChipMode mode, uint16_t timeout)
{
    switch(mode)
	{
		case 0:
			Esp8266SendAT(usart_id,"AT+CIPMUX=0\r\n");  //单连接
			break;
		case 1:
			Esp8266SendAT(usart_id,"AT+CIPMUX=1\r\n");  //多连接
			break;
		default:
			break;
	}

    return Esp8266Recieve(usart_id ,1 ,timeout);
}

uint8_t Esp8266ConnectServer(uint8_t cipid, char *server, uint16_t port, uint16_t timeout)
{
	char cmdBuff[100] = {0};
	sprintf(cmdBuff, "AT+CIPSTART=%d,\"TCP\",\"%s\",%d\r\n", cipid, server, port);
	Esp8266SendAT(usart_id,cmdBuff);

    return Esp8266Recieve(usart_id ,5 ,timeout);
}

uint8_t Esp8266SendServerFrameData(uint8_t cipid, uint8_t *data, uint16_t len, uint16_t timeout)
{
	char cmdBuff[100] = {0};
	sprintf(cmdBuff, "AT+CIPSENDEX=%d,%d\r\n", cipid, len);
	Esp8266SendAT(usart_id,cmdBuff);
	
    if(Esp8266Recieve(usart_id, 1 ,timeout) != 1)
    {
        return 5;
    }

    send_data(usart_id, data, len);
	
    return Esp8266Recieve(usart_id, 1 ,timeout);
}

uint8_t *Esp8266RecieveServerFrameData(uint8_t usart_id, uint8_t *buf, uint16_t *rxlen, uint16_t timeout)   //处理上位机传回的数据
{
    uint16_t buflen = 0;
    char *pos1 = NULL;
    char *pos2 = NULL;

    CleanBuf(usart_id);
    buflen = Esp8266RecieveOneFrameData(usart_id, buf, timeout);
	
    if(buflen == 0)
    {
        return NULL;
    }

    pos1 = pos2 = strstr((char *)buf, ":");
    *pos2 = '\0';
    while(*pos1 != ',')
    {
        pos1--;
    }
    *rxlen = atoi(pos1 + 1);

    return (uint8_t *)(pos2 + 1);
}
