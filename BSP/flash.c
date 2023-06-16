#include "flash.h"
#include "protocol.h"
#include "esp8266.h"

uint8_t AskBuf[20];
uint16_t AskPackLen;
uint8_t ServerBuf[1024];
uint16_t ServerBufLen;
uint16_t seg_pack_sum = 0;

void DataDownload()
{
    uint8_t *pServerBuf = (void *)0;

    AskDataSegment(AskBuf, 256, &AskPackLen);
    SendCommenPack(AskBuf, AskPackLen);
    pServerBuf = Esp8266RecieveServerFrameData(1, ServerBuf, &ServerBufLen, 1000);

    
    printf("CheckCommonData code= %d\r\n", CheckCommonData(pServerBuf, ServerBufLen));
    printf("CheckAckSeg code= %d\r\n", CheckAckSeg(pServerBuf + COMMON_PACK_SIZE, ServerBufLen - COMMON_PACK_SIZE));
    
    for(int i = 0; i < ServerBufLen; i++)
    {
        printf("%02x ", pServerBuf[i]);
    }

    for(uint16_t i = 0; i < seg_pack_sum; i++)
    {
        AskData(AskBuf, i, &AskPackLen);
        SendCommenPack(AskBuf, AskPackLen);
        
        pServerBuf = Esp8266RecieveServerFrameData(1, ServerBuf, &ServerBufLen, 1000);
        printf("ServerBufLen= %d\r\n", ServerBufLen);
        printf("CheckCommonData code= %d\r\n", CheckCommonData(pServerBuf, ServerBufLen));
        printf("CheckAckSeg code= %d\r\n", CheckAckData(pServerBuf + COMMON_PACK_SIZE, ServerBufLen - COMMON_PACK_SIZE));
        
        for(int i = 0; i < ServerBufLen; i++)
        {
            printf("%02x ", pServerBuf[i]);
        }
    }




}