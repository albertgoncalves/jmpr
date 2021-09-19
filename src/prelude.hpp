#ifndef __PRELUDE_H__
#define __PRELUDE_H__

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <xmmintrin.h>

#define PI 3.1415926535897932385f

typedef uint8_t  u8;
typedef uint32_t u32;
typedef size_t   usize;

typedef int32_t i32;
typedef ssize_t isize;

typedef float  f32;
typedef double f64;

typedef __m128 Simd4f32;

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

struct Cube {
    Vec3 bottom_left_front;
    Vec3 top_right_back;
};

#define EXIT()                                                       \
    {                                                                \
        fprintf(stderr, "%s:%s:%d\n", __FILE__, __func__, __LINE__); \
        _exit(EXIT_FAILURE);                                         \
    }

#define EXIT_WITH(x)                                                        \
    {                                                                       \
        fprintf(stderr, "%s:%s:%d\n%s\n", __FILE__, __func__, __LINE__, x); \
        _exit(EXIT_FAILURE);                                                \
    }

#define EXIT_IF(condition)    \
    if (condition) {          \
        EXIT_WITH(#condition) \
    }

#endif
