#ifndef NN_WIRE_INCLUDED
#define NN_WIRE_INCLUDED

#include <stdint.h>

uint16_t nn_gets(const uint8_t *buf);
void nn_puts(uint8_t *buf, uint16_t val);
uint32_t nn_getl(const uint8_t *buf);
void nn_putl(uint8_t *buf, uint32_t val);
uint64_t nn_getll(const uint8_t *buf);
void nn_putll(uint8_t *buf, uint64_t val);

#endif // !NN_WIRE_INCLUDED
