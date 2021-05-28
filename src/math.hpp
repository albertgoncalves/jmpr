#ifndef __MATH_H__
#define __MATH_H__

#include "prelude.hpp"

#include <immintrin.h>
#include <math.h>

#define PI 3.1415926535897932385f

struct Vec3 {
    f32 x;
    f32 y;
    f32 z;
};

typedef __m128 Simd4f32;

union Mat4 {
    f32      cell[4][4];
    Simd4f32 column[4];
};

#define MIN(l, r) ((l) < (r) ? (l) : (r))
#define MAX(l, r) ((l) < (r) ? (r) : (l))

static f32 get_radians(f32 degrees) {
    return (degrees * PI) / 180.0f;
}

static Vec3 min(Vec3 l, Vec3 r) {
    return {
        MIN(l.x, r.x),
        MIN(l.y, r.y),
        MIN(l.z, r.z),
    };
}

static Vec3 max(Vec3 l, Vec3 r) {
    return {
        MAX(l.x, r.x),
        MAX(l.y, r.y),
        MAX(l.z, r.z),
    };
}

static Vec3 clip(Vec3 x, Vec3 l, Vec3 r) {
    return min(r, max(l, x));
}

static Vec3 operator+(Vec3 l, Vec3 r) {
    return {
        l.x + r.x,
        l.y + r.y,
        l.z + r.z,
    };
}

static Vec3& operator+=(Vec3& l, f32 r) {
    l.x += r;
    l.y += r;
    l.z += r;
    return l;
}

static Vec3 operator-(Vec3 l, Vec3 r) {
    return {
        l.x - r.x,
        l.y - r.y,
        l.z - r.z,
    };
}

static Vec3 operator-(Vec3 l, f32 r) {
    return {
        l.x - r,
        l.y - r,
        l.z - r,
    };
}

static Vec3 operator*(Vec3 l, Vec3 r) {
    return {
        l.x * r.x,
        l.y * r.y,
        l.z * r.z,
    };
}

static Vec3& operator*=(Vec3& l, Vec3 r) {
    l.x *= r.x;
    l.y *= r.y;
    l.z *= r.z;
    return l;
}

static Vec3 operator*(Vec3 l, f32 r) {
    return {
        l.x * r,
        l.y * r,
        l.z * r,
    };
}

static Vec3 operator/(Vec3 l, Vec3 r) {
    return {
        l.x / r.x,
        l.y / r.y,
        l.z / r.z,
    };
}

static Vec3 operator/(Vec3 l, f32 r) {
    return {
        l.x / r,
        l.y / r,
        l.z / r,
    };
}

static Vec3 cross(Vec3 l, Vec3 r) {
    return {
        (l.y * r.z) - (l.z * r.y),
        (l.z * r.x) - (l.x * r.z),
        (l.x * r.y) - (l.y * r.x),
    };
}

static f32 dot(Vec3 l, Vec3 r) {
    return (l.x * r.x) + (l.y * r.y) + (l.z * r.z);
}

static f32 len(Vec3 x) {
    return sqrtf(dot(x, x));
}

static Vec3 norm(Vec3 x) {
    return x / len(x);
}

static Simd4f32 linear_combine(Simd4f32 a, Mat4 b) {
    Simd4f32 c;
    c = _mm_mul_ps(_mm_shuffle_ps(a, a, 0x00), b.column[0]);
    c = _mm_add_ps(c, _mm_mul_ps(_mm_shuffle_ps(a, a, 0x55), b.column[1]));
    c = _mm_add_ps(c, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xAA), b.column[2]));
    c = _mm_add_ps(c, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xFF), b.column[3]));
    return c;
}

static Mat4 operator*(Mat4 l, Mat4 r) {
    return {
        .column[0] = linear_combine(r.column[0], l),
        .column[1] = linear_combine(r.column[1], l),
        .column[2] = linear_combine(r.column[2], l),
        .column[3] = linear_combine(r.column[3], l),
    };
}

static Mat4 diag(f32 x) {
    return {
        .cell[0][0] = x,
        .cell[1][1] = x,
        .cell[2][2] = x,
        .cell[3][3] = x,
    };
}

static Mat4 translate(Vec3 a) {
    Mat4 b = diag(1.0f);
    b.cell[3][0] = a.x;
    b.cell[3][1] = a.y;
    b.cell[3][2] = a.z;
    return b;
}

static Mat4 scale(Vec3 a) {
    Mat4 b = diag(1.0f);
    b.cell[0][0] = a.x;
    b.cell[1][1] = a.y;
    b.cell[2][2] = a.z;
    return b;
}

static Mat4 perspective(f32 fov_radians, f32 aspect_ratio, f32 near, f32 far) {
    const f32 cotangent = 1.0f / tanf(fov_radians / 2.0f);
    return {
        .cell[0][0] = cotangent / aspect_ratio,
        .cell[1][1] = cotangent,
        .cell[2][3] = -1.0f,
        .cell[2][2] = (near + far) / (near - far),
        .cell[3][2] = (near * far * 2.0f) / (near - far),
        .cell[3][3] = 0.0f,
    };
}

static Mat4 look_at(Vec3 eye, Vec3 target, Vec3 up) {
    const Vec3 f = norm(target - eye);
    const Vec3 s = norm(cross(f, up));
    const Vec3 u = cross(s, f);
    return {
        .cell[0][0] = s.x,
        .cell[0][1] = u.x,
        .cell[0][2] = -f.x,
        .cell[0][3] = 0.0f,
        .cell[1][0] = s.y,
        .cell[1][1] = u.y,
        .cell[1][2] = -f.y,
        .cell[1][3] = 0.0f,
        .cell[2][0] = s.z,
        .cell[2][1] = u.z,
        .cell[2][2] = -f.z,
        .cell[2][3] = 0.0f,
        .cell[3][0] = -dot(s, eye),
        .cell[3][1] = -dot(u, eye),
        .cell[3][2] = dot(f, eye),
        .cell[3][3] = 1.0f,
    };
}

#endif
