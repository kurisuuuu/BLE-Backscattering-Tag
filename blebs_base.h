#ifndef BLEBS_BASE_H_
#define BLEBS_BASE_H_


#include <msp430.h>
#include <stdint.h>

#define TX_DELAY    (76)   //304 cycles, 16Mhz, 19us
#define TX_DELAY2   (400)  //1600 cycles, 16Mhz, 100us

void BLEBS_init(void);
void BlePacketEncode(uint8_t* packet, uint8_t len, uint8_t chan);
void crc24_gen(uint8_t* data, uint8_t len, uint8_t* dst);
void bleWhiten(uint8_t* data, uint8_t len, uint8_t whitenCoeff);
void gpioSendBle(void);
void timerSendBle(void);
uint8_t  swapbits(uint8_t byte);

extern uint8_t* ptr_blePacket;
extern uint8_t  len_blePacket;

extern void sendBLEPacket(void);
extern void TA_sendBLEPacket(void);


#endif /* BLEBS_BASE_H_ */

