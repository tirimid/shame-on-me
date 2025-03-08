#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;

typedef float f32;
typedef double f64;

void ShowError(char const *Fmt, ...);
u64 GetUnixTimeMs(void);
void BeginTick(void);
void EndTick(void);
f32 Atan2(f32 x, f32 y);

#endif
