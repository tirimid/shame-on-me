#ifndef UTIL_H
#define UTIL_H

// use for microsecond profiling.
// compile debug code with -DPROFILE, and release code without.
#ifdef PROFILE
#define NEWTIMER(name) u64 name
#define BEGINTIMER(timer) begintimer(timer)
#define ENDTIMER(timer, name) endtimer(timer, name)
#else
#define NEWTIMER(name)
#define BEGINTIMER(timer)
#define ENDTIMER(timer, name)
#endif

#define OUT

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

void showerr(char const *fmt, ...);
u64 unixus(void);
u64 unixms(void);
void begintick(void);
void endtick(void);
f32 interpangle(f32 a, f32 b, f32 t);
f32 shortestangle(f32 a, f32 b);
void makexform(vec3 pos, vec3 rot, vec3 scale, OUT mat4 out);
void makenorm(mat4 xform, OUT mat3 out);
void begintimer(OUT u64 *timer);
void endtimer(u64 timer, char const *name);
i32 randint(i32 lb, i32 ub);

#endif
