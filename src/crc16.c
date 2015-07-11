#include "crc16.h"

uint16_t crc16_calc (uint16_t initvalue, uint16_t mask, uint16_t length, \
        uint8_t *buff) {
    uint16_t i, crc;
    uint8_t j;

    crc = initvalue;
    for (i = length; i > 0; i--) {
        crc = crc ^ *(buff + length - i);
        for (j = 8; j > 0; j--) {
            if(crc & 1)
                crc = (crc >> 1) ^ mask;
            else
                crc >>= 1;
        }
    }
    return crc;
}
