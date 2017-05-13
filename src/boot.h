#ifndef BOOT_H
#define BOOT_H

//Include libraries needed{{{
#include <avr/io.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "crc16.h" //lib. for crc of 16 calculation
#include "config.h"
//}}}

//Bootloader version definition{{{
#define VERSION         "v1.1-b1"
const uint8_t BootloaderVersion[] PROGMEM = {VERSION};
//}}}

//Definition of Program Addresses{{{
#define NBITS2(n)   ((n&2)?1:0)
#define NBITS4(n)   ((n&(0xC))?(2+NBITS2(n>>2)):(NBITS2(n)))
#define NBITS8(n)   ((n&0xF0)?(4+NBITS4(n>>4)):(NBITS4(n)))
#define NBITS16(n)  ((n&0xFF00)?(8+NBITS8(n>>8)):(NBITS8(n)))
#define NBITS32(n)  ((n&0xFFFF0000)?(16+NBITS16(n>>16)):(NBITS16(n)))
#define NBITS(n)    (n==0?0:NBITS32(n)+1)
#define PAGSIZE     1 //Pagewise address for pages
#define CRCSIZE     2 //16 bits crc
#define SPM_LOG2    (NBITS(SPM_PAGESIZE)-1) //Result of LOG2(SPM_PAGESIZE)
#define BUFFSIZE    (SPM_PAGESIZE + CRCSIZE + PAGSIZE) //Size of the buffer
#define SHORTBUFS   (CRCSIZE + PAGSIZE) //Definition of the size for short buff
#define MAXPAGE     (BOOTSTART / SPM_PAGESIZE) //Max. page of flash app
//}}}

//Definition of transmission constants{{{
#define F_RD      0x72 //Flash Read ('r') --> One page at a time
#define F_WR      0x77 //Flash Write ('w') --> One page at a time
#define F_VR      0x76 //Flash CRC Verification ('v') --> Until spec page
#define F_CL      0x63 //Flash Clear ('c') --> Clear all pages
#define E_RD      0x52 //EEPROM Read ('R') --> One page at a time
#define E_WR      0x57 //EEPROM Write ('W') --> One page at a time
#define E_VR      0x56 //EEPROM CRC Verification ('V') --> Until specified page
#define E_CL      0x43 //EEPROM Clear ('C') --> CLear all pages
#define X_CH      0x58 //eXecute flash changes ('X')
#define Q_BL      0x51 //Quit Bootloader ('Q')
#define ACK       0x06 //Acknowledge byte
#define NACK      0x15 //Negative Acknowledge
//}}}

//Definition crc16 constants{{{
#define INITVAL     0xFFFF //Initial value for crc calculations
#if (SPM_PAGESIZE > 256)
#define MASKBUFF    0xD745
#else
#define MASKBUFF    0xA6BC //CRC-16F/3 (FOR HD=6 when DATA_LENGTH<135)
#endif
#if (BOOTSTART > 0x7FFFUL)
#define MASKFLASH   0xD4D8 //CRC-16F/5 (FOR HD=3 when DATA_LENGTH<65519)
#else
#define MASKFLASH   0xD745 //CRC-16F/4.2 /FOR HD=4 when DATA_LENGTH<32751)
#endif
//}}}

//Macros definitions{{{
#define UART_ENA() (UCSRB |= (1<<TXEN) | (1<<RXEN)) //Enable Uart
#define UART_DIS() (UCSRB &= ~((1<<TXEN) | (1<<RXEN))) //Disable Uart
#define DATA_RX()  (UCSRA & (1<<RXC)) //Check if there is data in rx buffer
#define FREE_TX()  (UCSRA & (1<<UDRE)) //Check if tx buffer is free
#define TIM_ENA()  (TCCRB |= ((1<<CS2) | (1<<CS0))) //Ena with 1024 preesc.
#define TIM_DIS()  (TCCRB &= ~((1<<CS2) | (1<<CS0))) //Disable timer
#define TIM_RST()  (TCNT = 0) //Reset timer count register
#define TIM_OVR()  (TIFR & (1<<TOV)) //Check if timer's register overflowed
#define TFLAGRST() (TIFR |= (1<<TOV)) //Reset timer overflow flag
#define PAGBT      (Buffer[0]) //Page address pagewise
#define PAGWD      ((uint16_t)(PAGBT<<SPM_LOG2)) //Mult page address
                                                 //by SPM_PAGESIZE
//}}}

//Functions prototypes{{{
//device initialization rutine
void __jmain(void) __attribute__ ((naked)) __attribute__ \
         ((section (".init9")));
void main (void) __attribute__ ((noreturn)) __attribute__ ((OS_main));
void uart_tx(uint8_t data) __attribute__ ((noinline));
inline uint8_t uart_rx(void) __attribute__ ((always_inline));
void flash_buff_load(uint8_t *buff, uint8_t PagNm) __attribute__ ((noinline));
#if (BUFFSIZE > 255)
inline void send_block(uint8_t *buff, uint16_t size) __attribute__ \
           ((always_inline));
uint8_t receive_block(uint8_t *buff, uint16_t size) __attribute__ ((noinline));
#else
inline void send_block(uint8_t *buff, uint8_t size) __attribute__ \
           ((always_inline));
uint8_t receive_block(uint8_t *buff, uint8_t) __attribute__ ((noinline));
#endif
typedef void (*fncptr)(void); //jump to application
//}}}

#endif
