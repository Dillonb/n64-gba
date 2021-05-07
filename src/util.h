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
#define PACKED __attribute__((__packed__))

#define WORD_ADDRESS(addr) (addr)
#define HALF_ADDRESS(addr) ((addr) ^ 2)
#define BYTE_ADDRESS(addr) ((addr) ^ 3)

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

INLINE void word_to_byte_array(byte* arr, word index, word value) {
    memcpy(arr + index, &value, sizeof(word));
}

INLINE void half_to_byte_array(byte* arr, word index, half value) {
    memcpy(arr + index, &value, sizeof(half));
}

INLINE word word_from_byte_array(byte* arr, word index) {
    word val;
    memcpy(&val, arr + index, sizeof(word));
    return val;
}

INLINE half half_from_byte_array(byte* arr, word index) {
    half val;
    memcpy(&val, arr + index, sizeof(half));
    return val;
}

#define ASSERTWORD(thing) _Static_assert(sizeof(thing) == 4);
#define ASSERTHALF(thing) _Static_assert(sizeof(thing) == 2);
#define ASSERTBYTE(thing) _Static_assert(sizeof(thing) == 1);

#endif //N64_GBA_UTIL_H
