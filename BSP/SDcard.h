#ifndef __SDCARD_H
#define __SDCARD_H

#include <stdint.h>
#include <stdio.h>

#define SD_WAIT_RESPONSE                    0xFF
#define SD_WAIT_PROGRAMME                   0xFFFFFF

#define SD_WRITE_ONE_BLOCK_TOKEN            0xFE
#define SD_WRITE_MUTI_BLOCK_TOKEN           0xFC
#define SD_STOP_MUTI_BLOCK_TOKEN            0xFD


#define SD_R1                               0x01
#define SD_R3                               0x03
#define SD_R7                               0x07

#define SD_CMD_ERR                          0x55
#define SD_CMD0_ERR                         0x56
#define SD_CMD8_ERR                         0x57
#define SD_ACMD41_ERR                       0x58
#define SD_CMD58_ERR                        0x59
#define SD_CMD17_ERR                        0x60
#define SD_CMD18_ERR                        0x61
#define SD_CMD24_ERR                        0x62
#define SD_SendBlock_ERR                    0x63
#define SD_CMD25_ERR                        0x64

#define SD_OK                               0
#define SD_ERR                              1

// SD卡指令表  	   
#define CMD0    0       //卡复位
#define CMD1    1
#define CMD8    8       //命令8 ，SEND_IF_COND
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读sector
#define CMD18   18      //命令18，读Multi sector
#define CMD23   23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define CMD41   41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00


uint8_t SD_Init(void);
uint8_t SD_ReadDisk(uint8_t *buf, uint32_t sector, uint32_t count);
uint8_t SD_WriteDisk(uint8_t *buf, uint32_t sector, uint32_t count);
uint32_t SD_GetSectorCount(void);


#endif
