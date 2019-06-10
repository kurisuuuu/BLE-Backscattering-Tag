/**
 * @file main.c
 */

#include <blebs_base.h>

uint8_t   blePacket1[] = {
                         /* Preamble */
                         0xAA,

                         /* Access Address */
                         0xD6, 0xBE, 0x89, 0x8E,

                         /* Header */
                         0x42, //ADV_NONCONN_IND, random address
                         19, //len byte

                         /* Address */
                         0xEF, 0xFF, 0xC0, 0xAA, 0x18, 0x00,

                         /* Advertising data: Flags AD Type */
                         0x02, //len byte
                         0x01, //AD type = flag
                         0x06, //LE General Discoverable Mode,BR/EDR Not Supported

                         /* Advertising data: Complete Local Name */
                         9, //len byte
                         0x08, //AD type = Shortened local name
                         'I','O','E',' ','c','h','3', '7',

                         /* CRC */
                         0x55, 0x55, 0x55 //initial crc lfsr
};

uint8_t   blePacket2[] = {
                         /* Preamble */
                         0xAA,

                         /* Access Address */
                         0xD6, 0xBE, 0x89, 0x8E,

                         /* Header */
                         0x42, //ADV_NONCONN_IND, random address
                         19, //len byte

                         /* Address */
                         0xEF, 0xFF, 0xC0, 0xAA, 0x18, 0x01,

                         /* Advertising data: Flags AD Type */
                         0x02, //len byte
                         0x01, //AD type = flag
                         0x06, //LE General Discoverable Mode,BR/EDR Not Supported

                         /* Advertising data: Complete Local Name */
                         9, //len byte
                         0x08, //AD type = Shortened local name
                         'I','O','E',' ','c','h','3', '8',
                         //'I', 'L', 'V', 'E', 'E',

                         /* CRC */
                         0x55, 0x55, 0x55 //initial crc lfsr
};

uint8_t blePacket0[] = {
                            0xAA, 0xAA, 0xAA, 0xAA,
                            0xAA, 0xAA, 0xAA, 0xAA,
                            0xAA, 0xAA, 0xAA, 0xAA,
                            0xAA, 0xAA, 0xAA, 0xAA,
                            0xAA, 0xAA, 0xAA, 0xAA,
                            0xAA, 0xAA, 0xAA, 0xAA,
                            0xAA, 0xAA, 0xAA, 0xA5,
        };

uint8_t dmaBuff[8 * sizeof(blePacket1)];

uint8_t*  ptr_blePacket;
uint8_t   len_blePacket;

/**
 * swapbit
 */
uint8_t  swapbits(uint8_t byte)
{
    uint8_t ret = 0;

    if(byte & 0x80) ret |= 0x01;
    if(byte & 0x40) ret |= 0x02;
    if(byte & 0x20) ret |= 0x04;
    if(byte & 0x10) ret |= 0x08;
    if(byte & 0x08) ret |= 0x10;
    if(byte & 0x04) ret |= 0x20;
    if(byte & 0x02) ret |= 0x40;
    if(byte & 0x01) ret |= 0x80;

    return ret;
}

/**
 * bleWhiten
 */
void bleWhiten(uint8_t* data, uint8_t len, uint8_t chan){

    uint8_t  bit, lfsr = swapbits(chan) | 2;

    while(len--)
    {
        for(bit = 0x80; bit; bit >>= 1){

            if(lfsr & 0x80){

                lfsr ^= 0x11;
                (*data) ^= bit;
            }
            lfsr <<= 1;
        }
        data++;
    }
}

/**
 * crc24_gen
 */
void crc24_gen(uint8_t* data, uint8_t len, uint8_t* dst)
{
    uint8_t i, bit, byte;

    while(len--)
    {
        byte = *data++;
        for(i = 0; i < 8; i++, byte <<= 1)
        {
            bit = dst[0] & 0x80;
            dst[0] <<= 1;
            if(dst[1] & 0x80) dst[0] |= 1;
            dst[1] <<= 1;
            if(dst[2] & 0x80) dst[1] |= 1;
            dst[2] <<= 1;

            if(bit != (byte & 0x80))  //Compare MSB of crc & LSB of pdu
            {
                dst[2] ^= 0x5B;
                dst[1] ^= 0x06;
            }
        }
    }
}

/**
 * BlePacketEncode
 */
void BlePacketEncode(uint8_t* packet, uint8_t len, uint8_t chan)
{
    uint8_t i;

    for(i = 0; i < len - 3; i++)
        packet[i] = swapbits(packet[i]);
    crc24_gen(packet + 5, len - 8, packet + len - 3);         //crc24_gen(pduStart, pduLength , crcStart)
    bleWhiten(packet + 5, len - 5, chan);                     //bleWhiten(pduStart, (pdu+crc)Length, channelCoefficient)
}

/**
 * void BLEBS_init(void)
 */
void BLEBS_init(void)
{

    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer

    // Disable FRAM wait cycles to allow clock operation over 8MHz
    FRCTL0 = 0xA500 | ((1) << 4);  //FRCTLPW | NWAITS_1;

    //Init GPIO
    P1OUT = BIT1;                             // Pull-up resistor on P1.1
    P1REN = BIT1;                             // Select pull-up mode for P1.1
    P1DIR = 0xFF ^ BIT1;                      // Set all but P1.1 to output direction
    P1SEL0 |= BIT3 | BIT4;                    // P1.3 and P1.4 options select
    P1IES = BIT1;                             // P1.1 Hi/Lo edge
    P1IFG = 0;                                // Clear all P1 interrupt flags
    P1IE = BIT1;                              // P1.1 interrupt enabled
    P2OUT = 0x00;
    P2DIR = 0xFF;
    //P2SEL1 |= BIT0 | BIT1;                    // USCI_A0 UART operation
    //P2SEL0 &= ~(BIT0 | BIT1);
    P3OUT = 0x00;
    P3DIR = 0xFF;
    //P3SEL1 |= BIT4;                           // P3.4 SMCLK
    P4OUT = 0x00;
    P4DIR = 0xFF;
    PJOUT = 0x00;
    PJDIR = 0xFF;
    //PJSEL0 |= BIT6 | BIT7;                    // For XT2

    // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings.
    PM5CTL0 &= ~LOCKLPM5;       // Lock LPM5.


    CSCTL0_H = 0xA5;
    CSCTL1 = DCORSEL | DCOFSEL_4; //16MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA_0 | DIVS_0 | DIVM_0;
    CSCTL0_H = 0x00;

}

/**
 * gpioSendBle
 */
void gpioSendBle(void)
{
    ptr_blePacket = blePacket1;
    len_blePacket = sizeof(blePacket1);
    sendBLEPacket();
    __delay_cycles(800000);
    ptr_blePacket = blePacket2;
    len_blePacket = sizeof(blePacket2);
    sendBLEPacket();
    __delay_cycles(800000);
}


/**
 * main.c
 */
int main(void)
{
    uint8_t i = 0;


    BLEBS_init();

    BlePacketEncode(blePacket1, sizeof(blePacket1), 37);
    BlePacketEncode(blePacket2, sizeof(blePacket2), 38);

    //flip bit
    for(i=0; i<sizeof(blePacket2); i++)
    {
        blePacket2[i] = ~blePacket2[i];
    }
    while(1)
    {
        //gpioSendBle();
        timerSendBle();
    }
	return 0;
}


