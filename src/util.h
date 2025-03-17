#ifndef UTIL_H
#define UTIL_H

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
u64 GetUnixTimeUs(void);
u64 GetUnixTimeMs(void);
void BeginTick(void);
void EndTick(void);
f32 InterpolateAngle(f32 a, f32 b, f32 t);
f32 ShortestAngle(f32 a, f32 b);
void MakeXformMat(vec3 Pos, vec3 Rot, vec3 Scale, mat4 Out);
void MakeNormalMat(mat4 Xform, mat3 Out);
void BeginMicroTimer(u64 *OutTimer);
void EndMicroTimer(u64 TimerStart, char const *Name);

#endif
