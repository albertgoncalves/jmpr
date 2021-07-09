#ifndef __PRELUDE_H__
#define __PRELUDE_H__

#include <immintrin.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.1415926535897932385f

typedef uint8_t  u8;
typedef uint32_t u32;
typedef size_t   usize;

typedef int32_t i32;

typedef float  f32;
typedef double f64;

typedef __m128 Simd4f32;

typedef FILE File;

#define null nullptr

struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

union Mat4 {
    f32      cell[4][4];
    Simd4f32 column[4];
};

#define ERROR(x)                                                            \
    {                                                                       \
        fprintf(stderr, "%s:%s:%d\n%s\n", __FILE__, __func__, __LINE__, x); \
        exit(EXIT_FAILURE);                                                 \
    }

#define EXIT_IF(condition) \
    if (condition) {       \
        ERROR(#condition)  \
    }

#endif
