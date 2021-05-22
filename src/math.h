#ifndef __MATH_H__
#define __MATH_H__

#include "prelude.h"

#include <immintrin.h>
#include <math.h>

#define PI 3.1415926535897932385f

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} Vec3;

typedef __m128 Simd4f32;

typedef union {
    f32      cell[4][4];
    Simd4f32 column[4];
} Mat4;

static f32 get_radians(f32 degrees) {
    return (degrees * PI) / 180.0f;
}

static Vec3 add_vec3(Vec3 l, Vec3 r) {
    return (Vec3){
        .x = l.x + r.x,
        .y = l.y + r.y,
        .z = l.z + r.z,
    };
}

static Vec3 mul_vec3_f32(Vec3 l, f32 r) {
    return (Vec3){
        .x = l.x * r,
        .y = l.y * r,
        .z = l.z * r,
    };
}

static Vec3 sub_vec3(Vec3 l, Vec3 r) {
    return (Vec3){
        .x = l.x - r.x,
        .y = l.y - r.y,
        .z = l.z - r.z,
    };
}

static Vec3 cross_vec3(Vec3 l, Vec3 r) {
    return (Vec3){
        .x = (l.y * r.z) - (l.z * r.y),
        .y = (l.z * r.x) - (l.x * r.z),
        .z = (l.x * r.y) - (l.y * r.x),
    };
}

static f32 dot_vec3(Vec3 l, Vec3 r) {
    return (l.x * r.x) + (l.y * r.y) + (l.z * r.z);
}

static f32 len_vec3(Vec3 v) {
    return sqrtf(dot_vec3(v, v));
}

static Vec3 norm_vec3(Vec3 v) {
    const f32 len = len_vec3(v);
    return (Vec3){
        .x = v.x / len,
        .y = v.y / len,
        .z = v.z / len,
    };
}

static Simd4f32 linear_combine(Simd4f32 l, Mat4 r) {
    Simd4f32 out;
    out = _mm_mul_ps(_mm_shuffle_ps(l, l, 0x00), r.column[0]);
    out = _mm_add_ps(out, _mm_mul_ps(_mm_shuffle_ps(l, l, 0x55), r.column[1]));
    out = _mm_add_ps(out, _mm_mul_ps(_mm_shuffle_ps(l, l, 0xAA), r.column[2]));
    out = _mm_add_ps(out, _mm_mul_ps(_mm_shuffle_ps(l, l, 0xFF), r.column[3]));
    return out;
}

static Mat4 mul_mat4(Mat4 l, Mat4 r) {
    return (Mat4){
        .column[0] = linear_combine(r.column[0], l),
        .column[1] = linear_combine(r.column[1], l),
        .column[2] = linear_combine(r.column[2], l),
        .column[3] = linear_combine(r.column[3], l),
    };
}

static Mat4 diag_mat4(f32 x) {
    return (Mat4){
        .cell[0][0] = x,
        .cell[1][1] = x,
        .cell[2][2] = x,
        .cell[3][3] = x,
    };
}

static Mat4 translate_mat4(Vec3 translation) {
    Mat4 out = diag_mat4(1.0f);
    out.cell[3][0] = translation.x;
    out.cell[3][1] = translation.y;
    out.cell[3][2] = translation.z;
    return out;
}

static Mat4 scale_mat4(Vec3 scale) {
    Mat4 out = diag_mat4(1.0f);
    out.cell[0][0] = scale.x;
    out.cell[1][1] = scale.y;
    out.cell[2][2] = scale.z;
    return out;
}

static Mat4 perspective_mat4(f32 fov_radians,
                             f32 aspect_ratio,
                             f32 near,
                             f32 far) {
    const f32 cotangent = 1.0f / tanf(fov_radians / 2.0f);
    return (Mat4){
        .cell[0][0] = cotangent / aspect_ratio,
        .cell[1][1] = cotangent,
        .cell[2][3] = -1.0f,
        .cell[2][2] = (near + far) / (near - far),
        .cell[3][2] = (near * far * 2.0f) / (near - far),
        .cell[3][3] = 0.0f,
    };
}

static Mat4 look_at_mat4(Vec3 eye, Vec3 target, Vec3 up) {
    const Vec3 f = norm_vec3(sub_vec3(target, eye));
    const Vec3 s = norm_vec3(cross_vec3(f, up));
    const Vec3 u = cross_vec3(s, f);
    return (Mat4){
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
        .cell[3][0] = -dot_vec3(s, eye),
        .cell[3][1] = -dot_vec3(u, eye),
        .cell[3][2] = dot_vec3(f, eye),
        .cell[3][3] = 1.0f,
    };
}

#endif
