#ifndef __PROTOCOL_H
#define __PROTOCOL_H

#include <stdint.h>

#define COMMON_PACK_SIZE	8

#define ASK_DAT_SEGMENT 0 //请求类型（请求资源分割：0; 请求资源数据: 1）
#define ASK_DAT_SOURCE 1

#define PACK_HEAD 0x55AA   //头

#define ASK_DAT_SEGMENT_FC 0x0001   //请求资源分割的功能码
#define ASK_DAT_SEGMENT_SC 0x0000    //请求资源分割的状态码

#define ASK_DAT_SOURCE_FC 0x0002   //请求资源数据的功能码
#define ASK_DAT_SOURCE_SC 0x0000    //请求资源数据的状态码

#define ACK_DAT_SEGMENT_FC 0x8001   //请求资源分割的功能码
#define ACK_DAT_SEGMENT_SC 0x0000    //请求资源分割的状态码

#define ACK_DAT_SOURCE_FC 0x8002   //请求资源数据的功能码
#define ACK_DAT_SOURCE_SC 0x0000    //请求资源数据的状态码

//总传输总协议结构体
typedef struct CommonPack
{
    uint16_t head;   //头
	uint16_t crc;    //校验
	uint16_t packId;  //包序号
	uint16_t dataLen;   //数据长度
    uint8_t data_buff[0];   //数据资源
}__attribute__((packed)) CommonPack;

//请求应答功能结构体
typedef struct AckPack{
    uint16_t fc;       //功能码
	uint16_t sc;       //状态码
	uint8_t data[0];   //数据
}__attribute__((packed))AckPack;

//请求资源分割或请求资源数据
typedef struct AskPack{
    uint16_t fc;       //功能码
	uint16_t sc;       //状态码
	uint16_t packdat;   //包信息（请求资源分割：包大小; 请求资源数据: 包序号）
}__attribute__((packed))AskPack;


void AskDataSegment(uint8_t *AskDataSegmentBuf, uint16_t AskFramePackSize, uint16_t *PackSize);
void SendCommenPack(uint8_t *AskDataSegmentBuf, uint16_t PackSize);
uint8_t CheckCommonData(uint8_t *AckData, uint16_t AckDataLen);
uint8_t CheckAckSeg(uint8_t *AckSeg, uint16_t AckSegLen);
void AskData(uint8_t *AskDataBuf, uint16_t PackId, uint16_t *PackSize);
uint8_t CheckAckData(uint8_t *AckSourceData, uint16_t AckSourceDataLen);

#endif
