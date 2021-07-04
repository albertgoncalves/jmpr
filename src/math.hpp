#ifndef __MATH_H__
#define __MATH_H__

#include "prelude.hpp"

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
