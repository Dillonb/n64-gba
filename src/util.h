#ifndef N64_GBA_UTIL_H
#define N64_GBA_UTIL_H
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef uint8_t byte;
typedef uint16_t half;
typedef uint32_t word;
typedef uint64_t dword;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#define INLINE static inline __attribute__((always_inline))

INLINE int popcnt_16(u16 value) {
    int count = 0;
    for (int i = 0; i < 16; i++) {
        if ((value & 1) == 1) {
            count++;
        }
        value >>= 1;
    }
    return count;
}

INLINE int popcnt_8(u8 value) {
    int count = 0;
    for (int i = 0; i < 8; i++) {
        if ((value & 1) == 1) {
            count++;
        }
        value >>= 1;
    }
    return count;
}

INLINE u32 bswap32(u32 value) {
    return (value & 0x000000FF) << 24
         | (value & 0x0000FF00) << 8
         | (value & 0x00FF0000) >> 8
         | (value & 0xFF000000) >> 24;
}

INLINE u32 bswap16(u16 value) {
    return (value & 0x00FF) << 8
           | (value & 0xFF00) >> 8;
}

INLINE void word_to_byte_array(byte* arr, word index, word value) {
    word w = bswap32(value);
    memcpy(arr + index, &w, sizeof(word));
}

INLINE word word_from_byte_array(byte* arr, word index) {
    word val;
    memcpy(&val, arr + index, sizeof(word));
    return bswap32(val);
}

#define ASSERTWORD(thing) _Static_assert(sizeof(thing) == 4);

#endif //N64_GBA_UTIL_H
