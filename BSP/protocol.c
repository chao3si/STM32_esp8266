#include "protocol.h"
#include <string.h>
#include "usart.h"
#include "esp8266.h"
#include "flash.h"

static uint16_t packid = 0;
static uint8_t usart_id = 1;


void AskDataSegment(uint8_t *AskDataSegmentBuf, uint16_t AskFramePackSize, uint16_t *PackSize)
{
    AskPack *A = (AskPack *)AskDataSegmentBuf;

    A->fc = ASK_DAT_SEGMENT_FC;
    A->sc = ASK_DAT_SEGMENT_SC;
    A->packdat = AskFramePackSize;
    *PackSize = 0x06;
}

void AskData(uint8_t *AskDataBuf, uint16_t PackId, uint16_t *PackSize)
{
    AskPack *A = (AskPack *)AskDataBuf;

    A->fc = ASK_DAT_SOURCE_FC;
    A->sc = ASK_DAT_SOURCE_SC;
    A->packdat = PackId;
    *PackSize = 0x06;
}

void SendCommenPack(uint8_t *AskDataSegmentBuf, uint16_t PackSize)
{
    uint8_t CommenPackBuf[20] = {0};

    CommonPack T;
    memset(&T, 0, sizeof(CommenPackBuf));

    T.head = PACK_HEAD;
    for(uint16_t i = 0; i < PackSize; i++)
    {
        T.crc += AskDataSegmentBuf[i];
    }
    T.packId = packid;
    T.dataLen = PackSize;

    /*没用到柔性数组，malloc没实现*/
    memcpy(CommenPackBuf, (uint8_t*)&T, sizeof(T));
    memcpy(CommenPackBuf + sizeof(T), AskDataSegmentBuf, PackSize);

    Esp8266SendServerFrameData(0, CommenPackBuf, sizeof(T)+PackSize, 300);
}

uint8_t CheckCommonData(uint8_t *AckData, uint16_t AckDataLen)
{
    CommonPack *T = (CommonPack*)AckData;
    
    uint16_t TureCrc = 0;
    
    if(T->head != PACK_HEAD)
    {
        return 1;
    }
    
    if((AckDataLen - sizeof(CommonPack)) != T->dataLen)
    {
        return 2;
    }
    
    for(uint16_t i = 0; i < T->dataLen; i++)
    {
        TureCrc += T->data_buff[i];
    }
    
    if(T->crc != TureCrc)
    {
        return 3;
    }
    
    if(T->packId != packid)
    {
        return 4;
    }
    
    packid++;
    
    return 0;
}

uint8_t CheckAckSeg(uint8_t *AckSeg, uint16_t AckSegLen)
{
    AckPack *A = (AckPack *)AckSeg;

    if(A->sc != ACK_DAT_SEGMENT_SC)
    {
        return 1;
    }

    if(A->fc != ACK_DAT_SEGMENT_FC)
    {
        return 2;
    }


    //请求资源分割应答
    typedef struct AckData{
        uint32_t data_size;      //数据资源大小
        uint16_t pack_sum;        //总包数
    }__attribute__((packed))AckData;

    AckData *data = (AckData *)A->data;
    //printf("%d,%d,%d", AckSegLen,sizeof(AckPack),sizeof(AckData));

    if(AckSegLen - sizeof(AckPack) != sizeof(AckData))
    {
        return 3;
    }

    seg_pack_sum = data->pack_sum;

    return 0;
}

uint8_t CheckAckData(uint8_t *AckSourceData, uint16_t AckSourceDataLen)
{
    AckPack *A = (AckPack *)AckSourceData;

    if(A->sc != ACK_DAT_SOURCE_SC)
    {
        return 1;
    }
    if(A->fc != ACK_DAT_SOURCE_FC)
    {
        return 2;
    }


    //请求资源分割应答
    typedef struct AckData{
        uint16_t pageIndex;        //数据包序号
        uint8_t data_buff[0];   //数据资源(使用柔性数组)
    }__attribute__((packed))AckData;

    AckData *data = (AckData *)A->data;

    // if(data->pageIndex != packid)
    // {
    //     return 3;
    // }
    
    return 0;
}
