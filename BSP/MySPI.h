#ifndef __MYSPI_H
#define __MYSPI_H

#include <stdint.h>

void MySPI_Init(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);
void set_spi_speed(uint8_t perscaler);
void MySPI_SS(uint8_t BitValue);

#endif
