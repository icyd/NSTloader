#ifndef CONFIG_H
#define CONFIG_H

//Definition of F_CPU and BAUD rate{{{
#ifndef F_CPU
#define F_CPU       8000000UL   //CPU Frequency
#endif
#define BAUD        19200UL     // Baud Rate
#include <util/setbaud.h>
//}}}

//Definition of directions of Start App and Start Bootloader{{{
#define PROGSTART   0x0000UL    //Application start address (bytewise)
#define BOOTSTART   0x7C00UL    //Bootloader start address (bytewise)
//}}}

//Definition of the delay to start application
#define OVFLW       ((uint8_t)(F_CPU / 262144UL)) //Value neccesary to obtain around 1 sec overflow

//Register Definition for Customization of device{{{
#define UBRRH       UBRR0H
#define UBRRL       UBRR0L
#define UCSRC       UCSR0C
#define UCSRB       UCSR0B
#define UCSRA       UCSR0A
#define UDRE        UDRE0
#define UPM1        UPM01
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
//}}}
#endif
