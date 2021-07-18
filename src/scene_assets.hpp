#ifndef __SCENE_ASSETS_H__
#define __SCENE_ASSETS_H__

#include "prelude.hpp"

struct Instance {
    Mat4 matrix;
    Vec3 color;
};

// clang-format off
static const f32 VERTICES[] = {
    // NOTE: Positions.     // NOTE: Normals.
    -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,  //  0
     0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,  //  1
     0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,  //  2
    -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,  //  3
    -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,  //  4
     0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,  //  5
     0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,  //  6
    -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,  //  7
    -0.5f,  0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,  //  8
    -0.5f,  0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,  //  9
    -0.5f, -0.5f, -0.5f,   -1.0f,  0.0f,  0.0f,  // 10
    -0.5f, -0.5f,  0.5f,   -1.0f,  0.0f,  0.0f,  // 11
     0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,  // 12
     0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,  // 13
     0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,  // 14
     0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,  // 15
    -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,  // 16
     0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,  // 17
     0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,  // 18
    -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,  // 19
    -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,  // 20
     0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,  // 21
     0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,  // 22
    -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,  // 23
};
static const u32 INDICES[] = {
     0,  1,  2,
     2,  3,  0,
     4,  5,  6,
     6,  7,  4,
     8,  9, 10,
    10, 11,  8,
    12, 13, 14,
    14, 15, 12,
    16, 17, 18,
    18, 19, 16,
    20, 21, 22,
    22, 23, 20,
};
static const Vec3 PLATFORM_POSITIONS[] = {
    {   0.0f,     4.0f,     0.0f },
    {   0.0f,     6.0f,   -10.0f },
    {   0.0f,     8.0f,   -20.0f },
    {  10.0f,    10.0f,   -20.0f },
    {  10.0f,    12.0f,   -10.0f },
    {  10.0f,    14.0f,     0.0f },
    {  10.0f,     4.0f,     0.0f },
    {  10.0f,     4.0f,   -10.0f },
    {  10.0f,     4.0f,   -20.0f },
    { -20.0f,     6.0f,     0.0f },
    { -10.0f,     0.0f,   -10.0f },
    { -20.0f,     2.5f,   -10.0f },
    {  -6.25f,    4.125f, -25.0f },
    {  -2.5f,    17.25f,   -7.5f },
    {  -7.5f,    20.0f,     7.5f },
    {  -7.5f,    20.0f,    17.5f },
    { -17.5f,    20.0f,    17.5f },
    {   0.0f,     8.0f,   -40.0f },
    {  10.0f,     7.0f,   -35.0f },
    { -20.0f,     9.0f,   -20.0f },
    { -40.0f,     2.5f,   -55.0f },
    { -45.0f,     5.0f,   -40.0f },
    { -25.0f,    30.0f,    -5.0f },
    { -35.0f,    26.5f,     0.0f },
    { -40.0f,    22.5f,     5.0f },
};
// clang-format on

#endif
