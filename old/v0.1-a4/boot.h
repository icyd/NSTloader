#ifndef BOOT_H
#define BOOT_H 1

//Include
#include <avr/io.h>
#include <avr/boot.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "crc16.h"

//Configuration of Bootloader
//Definition of F_CPU
#ifndef F_CPU
#define F_CPU       8000000UL   //CPU Frequency
#endif
#define BAUD        19200UL     // Baud Rate
#include <util/setbaud.h>
#define PROGSTART   0x0000UL    //Application start address (bytewise)
#define BOOTSTART   0x7C00UL    //Bootloader start address (bytewise)
#define PFLAGADD    E2END       //Address of flag to check the integrity of data in flash

////Definition of Baud Error Calculation
//*** Deprecated, replaced with util/setbaud.h
//#define UBRRVALUE (((F_CPU >> 4) / BAUD) - 1UL)
//#define BAUDERROR (((F_CPU >> 4) / (UBRRVALUE + 1UL) * 100UL) / BAUD)
//#if ((BAUDERROR<98) || (BAUDERROR>102))
//#error "UART error over 2%!"
//#endif
#define UBRRV_L (UBRR_VALUE & 0xFF)
#define UBRRV_H (UBRR_VALUE >> 8)

//Definition of Program Addresses (bytewise)
#define BUFFSIZE    (SPM_PAGESIZE + 4)
#define MAXPAGE     (BOOTSTART / SPM_PAGESIZE)

////Definition of transmission constants
#define FRAME_1     0x01
#define FRAME_2     0x12
#define EOT         0x04
#define ACK         0x06
#define NACK        0x15

//Definition crc16 constants
#define INITVAL     0xFFFF
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

////Definition of program constatns
#define PRGOK       0x55
#define PRGERR      0xFF
#define OVFLW       ((uint8_t)(F_CPU / 262144UL)) // to obtain around 1 sec overflow

//Register Definition for Customization
#define UBRRH       UBRR0H
#define UBRRL       UBRR0L
#define UCSRB       UCSR0B
#define UCSRA       UCSR0A
#define UDRE        UDRE0
#define TXEN        TXEN0
#define RXEN        RXEN0
#define RXC         RXC0
#define UDR         UDR0
#define TCCRB       TCCR0B
#define CS2         CS02
#define CS1         CS01
#define CS0         CS00
#define TCNT        TCNT0
#define TIFR        TIFR0
#define TOV         TOV0

//Macro definition
#define UARTON()    (UCSRB |= (1<<TXEN) | (1<<RXEN))
#define UARTOFF()   (UCSRB &= ~((1<<TXEN) | (1<<RXEN)))
#define DataOnRx()  (UCSRA & (1<<RXC))
#define TxFree()  (UCSRA & (1<<UDRE))
#define TIMER_ON()  (TCCRB |= ((1<<CS2) | (1<<CS0)))//with 1024 preescaler
#define TIMER_OFF() (TCCRB &= ~((1<<CS2) | (1<<CS0)))
#define TIMER_RST() (TCNT = 0)
#define TIMER_OVR() (TIFR & (1<<TOV))
#define TFLAGRST()  (TIFR |= (1<<TOV))

//Types definitions

//Functions prototypes
void __jmain(void) __attribute__ ((naked)) __attribute__ ((section (".init9")));
void main (void) __attribute__ ((noreturn)) __attribute__ ((OS_main));
void uart_tx(uint8_t data) __attribute__ ((noinline));
uint8_t uart_rx(void) __attribute__ ((noinline));
typedef void (*fncptr)(void);

//Bootloader version definition
#define VERSION         "v0.1-a4"
const uint8_t BootloaderVersion[] __attribute__ ((section(".version"),used)) = {VERSION};

#endif
