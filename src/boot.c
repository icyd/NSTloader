#include "boot.h"

//main{{{
void main (void){
//Variable definition{{{
    uint8_t Buffer[BUFFSIZE]={SIGNATURE_0, SIGNATURE_1, SIGNATURE_2};
    uint8_t Tcounter = OVFLW;
    uint8_t opt;
    uint16_t crc;
    volatile fncptr runapp = (fncptr)0x0000;
    uint16_t j;
#if (SPM_PAGESIZE > 255)
    uint16_t i;
#else
    uint8_t i;
#endif
//}}}

//Registers and device initialization{{{
    UBRRH = UBRRH_VALUE;
    UBRRL = UBRRL_VALUE;
    UART_ENA();
    send_block(Buffer, 3); //Send signature bytes
    //Send bootloader version info (not enough space)
    /*for(i=7;i>0;i--)*/
        /*uart_tx(pgm_read_byte(BootloaderVersion[7-i]));*/
    TIM_ENA();
//}}}

//Bootloader handshake{{{
    while(Tcounter){ //Wait 1 sec for reply (ACK) of PC
        if(TIM_OVR()){
            TFLAGRST();
            Tcounter--;
        }
        if(DATA_RX() && (UDR == ACK)){
            break;
        }
    }
    TIM_DIS(); //Timer Disable
    TFLAGRST(); //Timer flag reset (to default state)
//}}}

    while(Tcounter){
        opt=uart_rx();
        //Flash and EEPROM read routine{{{
        if((opt==F_RD) || (opt==E_RD)){
            if(receive_block(Buffer, SHORTBUFS)){
                if(opt==F_RD)
                    flash_buff_load(Buffer, PAGBT);
                else{
                     eeprom_read_block(Buffer, (const void*)PAGWD, SPM_PAGESIZE);
                     eeprom_busy_wait();
                }
                crc=crc16_calc(INITVAL, MASKBUFF, SPM_PAGESIZE, Buffer);
                Buffer[SPM_PAGESIZE]=((uint8_t)(crc>>8));
                Buffer[SPM_PAGESIZE+1]=((uint8_t)crc);
                send_block(Buffer, (BUFFSIZE-PAGSIZE));
            }
        }
        //}}}
        //Flash write routine{{{
        else if(opt==F_WR){
            if(receive_block(Buffer, BUFFSIZE)){
                boot_page_erase(PAGWD);
                boot_spm_busy_wait();
                for(i=SPM_PAGESIZE;i>0;i-=2)
                    boot_page_fill(SPM_PAGESIZE-i, ((Buffer[SPM_PAGESIZE-i+3]<<8) | Buffer[SPM_PAGESIZE-i+2]));
                boot_page_write(PAGWD);
                boot_spm_busy_wait();
                uart_tx(ACK);
            }
        }
        //}}}
        //Flash and EEPROM verification routine{{{
        else if((opt==F_VR) || (opt==E_VR)){
            crc=INITVAL;
            if(receive_block(Buffer, SHORTBUFS)){//##
                for(i=PAGBT;i>0;i--){
                    if(opt==F_VR)
                        flash_buff_load(Buffer, PAGBT-i);
                    else{
                         eeprom_read_block(Buffer, (const void*)PAGWD, SPM_PAGESIZE);
                         eeprom_busy_wait();
                    }
                    crc=crc16_calc(crc, MASKFLASH, SPM_PAGESIZE, Buffer);
                }
            }
            uart_tx((uint8_t)(crc>>8));
            uart_tx((uint8_t)crc);
            uart_tx((uint8_t)((~crc)>>8));
            uart_tx((uint8_t)(~crc));
        }
        //}}}
        //Flash clear routine{{{
        else if(opt==F_CL){
            for(i=MAXPAGE;i>0;i--){
                boot_page_erase((uint16_t)((MAXPAGE-i)<<7));
                boot_spm_busy_wait();
            }
                uart_tx(ACK);
        }
        //}}}
        //EEPROM write routine{{{
        else if(opt==E_WR){
            if(receive_block(Buffer, BUFFSIZE)){
                eeprom_update_block(Buffer, (void*)PAGWD, SPM_PAGESIZE);
                eeprom_busy_wait();
                uart_tx(ACK);
            }
        }
        //}}}
        //EEPROM clear routine{{{
        else if(opt==E_CL){
            for(j=(E2END+1);j>0;j--){
                 eeprom_update_byte((uint8_t*)(E2END+1-j),0xFF);
                 eeprom_busy_wait();
            }
            uart_tx(ACK);
        }
        //}}}
        //Write to flash memory, data changes{{{
        else if(opt==X_CH){
            boot_rww_enable();
            uart_tx(ACK);
        }
        //}}}
        //Quit bootloader{{{
        else if(opt==Q_BL){
            uart_tx(ACK);
            Tcounter = 0;
        }
        //}}}
        //Other cases (default){{{
        else{
            uart_tx(NACK);
        }
        //}}}
    }
    UART_DIS(); //UART Disable
    runapp(); //jump to application
    for(;;); //infinite for loop
}
//}}}

//__jmain: device stack initialization{{{
void __jmain(void){
    asm volatile ("clr __zero_reg__");
    asm volatile ("out %0,__zero_reg__" :: "i" (AVR_STATUS_ADDR));
    asm volatile ("ldi r28,lo8(%0)" :: "i" (RAMEND));
    asm volatile ("ldi r29,hi8(%0)" :: "i" (RAMEND));
    asm volatile ("out %0,r29" :: "i" (AVR_STACK_POINTER_HI_ADDR));
    asm volatile ("out %0,r28" :: "i" (AVR_STACK_POINTER_LO_ADDR));
    asm volatile ("rjmp main");
}
//}}}

//uart_tx: data transmission{{{
void uart_tx(uint8_t data){
    while(!FREE_TX());
    UDR = data;
}
//}}}

//uart_rx: data reception{{{
inline uint8_t uart_rx(void){
    while(!DATA_RX());
    return UDR;
}
//}}}

//flash_buff_load: load the buffer with data from flash memory{{{
void flash_buff_load(uint8_t *buff, uint8_t PagNm){
#if (SPM_PAGESIZE > 255)
    uint16_t i;
#else
    uint8_t i;
#endif

    for(i=SPM_PAGESIZE;i>0;i--)
            *(buff+SPM_PAGESIZE-i)=pgm_read_byte((uint16_t)(PagNm<<SPM_LOG2)+SPM_PAGESIZE-i);
}
//}}}

//send_block: transmit a block of data{{{
#if (BUFFSIZE > 255)
inline void send_block(uint8_t *buff, uint16_t size){
    uint16_t i;
#else
inline void send_block(uint8_t *buff, uint8_t size){
    uint8_t i;
#endif

    for(i=size;i>0;i--){
        uart_tx(*(buff+size-i));
    }
}
//}}}

//receive_block: receice a block of data{{{
#if (BUFFSIZE >255)
uint8_t receive_block(uint8_t *buff, uint16_t size){
#else
uint8_t receive_block(uint8_t *buff, uint8_t size){
#endif
    uint16_t i;

    for(i=size;i>0;i--)
        *(buff+size-i)=uart_rx();
    i=crc16_calc(INITVAL, MASKBUFF, (size-CRCSIZE), buff);
    if(i==((*(buff+size-CRCSIZE)<<8) | *(buff+size-CRCSIZE+1)))
       return 1;
    uart_tx(NACK);
    return 0;
}
//}}}

