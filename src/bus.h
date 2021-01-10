#ifndef N64_GBA_BUS_H
#define N64_GBA_BUS_H

#include "util.h"
#include "arm7tdmi/arm7tdmi.h"

byte gba_read_byte(word address, access_type_t access_type);
half gba_read_half(word address, access_type_t access_type);
word gba_read_word(word address, access_type_t access_type);
void gba_write_byte(word address, byte value, access_type_t access_type);
void gba_write_half(word address, half value, access_type_t access_type);
void gba_write_word(word address, word value, access_type_t access_type);

#endif //N64_GBA_BUS_H
