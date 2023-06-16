#ifndef __USART_H_
#define __USART_H_

#include <stdio.h>
#include "queue.h"

extern uint8_t rev_buf[];
extern uint16_t rev_index;
extern uint8_t rev_flag;

void uart1_init(uint32_t BoundRate);
void uart2_init(uint32_t bound);
void send_byte(uint8_t data);
void send_data(uint8_t usart_id, uint8_t *data, uint16_t len);
void send_string(char* string);
uint16_t Usart2RecieveOneFrameData(uint8_t usart_id, uint8_t *buf, uint16_t timeout);
void CleanBuf(uint8_t usart_id);

#endif
