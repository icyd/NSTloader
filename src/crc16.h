#ifndef CRC16_H
#define CRC16_H 1

#include <stdint.h>

uint16_t crc16_calc(uint16_t initvalue, uint16_t mask, uint16_t length, uint8_t *buff);

#endif
