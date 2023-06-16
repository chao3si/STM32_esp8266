#ifndef __ESP8266_H
#define __ESP8266_H

typedef enum {
    eSingleLink = 0,
    eMutipleLink = 1,
} ESP8266ChipMode;

typedef enum Esp8266Mode 
{
    eStationMode = 1,
    eApMode = 2,
    eApStationMode = 3,
}Esp8266WifiMode;


uint8_t BspEsp8266WifiMode(Esp8266WifiMode mode, uint16_t timeout);
uint8_t Esp8266ConnectWifi(char *wifiName,char *password, uint16_t timeout);
uint8_t Esp8266SetChipMode(ESP8266ChipMode mode, uint16_t timeout);
uint8_t Esp8266Recieve(uint8_t usart_id, uint16_t frame_count, uint16_t timeout);
uint8_t Esp8266ConnectServer(uint8_t cipid, char *server, uint16_t port, uint16_t timeout);
uint8_t Esp8266SendServerFrameData(uint8_t cipid, uint8_t *data, uint16_t len, uint16_t timeout);
uint8_t *Esp8266RecieveServerFrameData(uint8_t usart_id, uint8_t *buf, uint16_t *rxlen, uint16_t timeout);

#endif
