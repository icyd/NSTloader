#include "boot.h"

void main (void){
    uint8_t Buffer[BUFFSIZE];
    uint8_t Boot_On = 0;
    uint8_t Flash_Mod = 0;
    uint8_t Tcounter = OVFLW;
    uint16_t crc;
    uint16_t crcflash = 0;
    uint16_t PagNm = 0; //Bytewise
    uint16_t j;
    volatile fncptr runapp = (fncptr)0x0000;
#if (SPM_PAGESIZE > 255)
    uint16_t i;
#else
    uint8_t i;
#endif

/*UART BAUD rate initialization*/
    UBRRH = UBRRV_H;
    UBRRL = UBRRV_L;
    UARTON();
    uart_tx(SIGNATURE_0);
    uart_tx(SIGNATURE_1);
    uart_tx(SIGNATURE_2);
    TIMER_ON();

/*Bootloader handshake*/
    while(Tcounter){
        if(DataOnRx() && (UDR == ACK)){
            Boot_On = 0xFF;
            break;
        }
        if(TIMER_OVR()){
            TFLAGRST();
            Tcounter--;
        }
    }
    TIMER_OFF();
//Begin data reception
    if(Boot_On){
        Tcounter = OVFLW;
        while(Tcounter){
            if(DataOnRx()){
                Buffer[0] = UDR;
                if(Buffer[0] == FRAME_1){
                    //Populate of buffer
                    for(i=BUFFSIZE;i>0;i--)
                        Buffer[BUFFSIZE-i]=uart_rx();
                    //Check of actual page number
                    if((Buffer[0] == (uint8_t)(PagNm>>8)) && \
                            (Buffer[1] == (uint8_t)PagNm)){
                        crc = crc16_calc(INITVAL, MASKBUFF, (BUFFSIZE-2), Buffer);
                        if((Buffer[BUFFSIZE-2] != (crc>>8) || \
                                    (Buffer[BUFFSIZE-1] != (uint8_t)crc)))
                            uart_tx(NACK);
                        else {
                        /*write to FLASH*/
                            /*Redundacy check !!!*/
                            if(PagNm < BOOTSTART){
                                boot_page_erase(PagNm);
                                boot_spm_busy_wait();
                                for(i=SPM_PAGESIZE;i>0;i-=2)
                                    boot_page_fill((SPM_PAGESIZE-i), \
                                            ((Buffer[SPM_PAGESIZE-i+3]<<8) | \
                                            Buffer[SPM_PAGESIZE-i+2]));
                                boot_spm_busy_wait();
                                boot_page_write(PagNm);
                                boot_spm_busy_wait();
                                if(!Flash_Mod){
                                    Flash_Mod = 0xFF;
                                    eeprom_update_byte((uint8_t *)PFLAGADD, PRGERR);
                                    eeprom_busy_wait();
                                }
                                PagNm += SPM_PAGESIZE;
                            }
                        uart_tx(ACK);
                        }
                    }
                    else
                        uart_tx(NACK);
                }
                else if(Buffer[0] == FRAME_2){
                    Buffer[0] = uart_rx();
                    Buffer[1] = uart_rx();
                    Buffer[2] = uart_rx();
                    Buffer[3] = uart_rx();
                    /*Check if the CRC receive has errors*/
                    if(((Buffer[0] ^ Buffer[2]) == 0xFF) && \
                           ((Buffer[1] ^ Buffer[3]) == 0xFF)){
                        crcflash = ((((uint16_t)Buffer[0]) << 8) | Buffer[1]);
                        uart_tx(ACK);
                    }
                    else
                        uart_tx(NACK);
                }
                else if(Buffer[0] == EOT){
                    TIMER_RST();
                    TIMER_ON(); // waits 1 sec for confirmation of EOT
                    uart_tx(ACK);
                }
                else
                    uart_tx(NACK);
            }

            if(TIMER_OVR()){
                TFLAGRST();
                Tcounter--;
            }
        }
        TIMER_OFF();
        TIMER_RST();
        boot_rww_enable();
        boot_spm_busy_wait();
        crc = INITVAL;
        for(j=PagNm;j>0;j-=SPM_PAGESIZE){
            for(i=SPM_PAGESIZE;i>0;i--)
                Buffer[SPM_PAGESIZE-i]=pgm_read_byte((PagNm-j) + (SPM_PAGESIZE-i));
            crc = crc16_calc(crc, MASKFLASH, SPM_PAGESIZE, Buffer);
        }
        /*Read flash an calculate crc*/
        if(crcflash == crc){
            eeprom_busy_wait();
            eeprom_update_byte((uint8_t *)PFLAGADD, PRGOK);
            eeprom_busy_wait();
            Buffer[0]=PRGOK;
        }
        else{
             Buffer[0]=PRGERR;
        }
        uart_tx(Buffer[0]);
    }
    TFLAGRST();
    /*Buffer[0] = eeprom_read_byte((uint8_t *)PFLAGADD);*/
    /*uart_tx(Buffer[0]);*/
    while(!TxFree()){} //wait for uart to complete transmission
    UARTOFF();
    Buffer[0] = eeprom_read_byte((uint8_t *)PFLAGADD);
    if(Buffer[0] == PRGOK)
        runapp();
    for(;;){}
}

void __jmain(void){
    asm volatile ("clr __zero_reg__");
    asm volatile ("out %0,__zero_reg__" :: "i" (AVR_STATUS_ADDR));
    asm volatile ("ldi r28,lo8(%0)" :: "i" (RAMEND));
    asm volatile ("ldi r29,hi8(%0)" :: "i" (RAMEND));
    asm volatile ("out %0,r29" :: "i" (AVR_STACK_POINTER_HI_ADDR));
    asm volatile ("out %0,r28" :: "i" (AVR_STACK_POINTER_LO_ADDR));
    asm volatile ("rjmp main");
}

void uart_tx(uint8_t data){
    while(!TxFree());
    UDR = data;
}

uint8_t uart_rx(void){
    while(!DataOnRx());
    return UDR;
}
