#include "scene_assets.hpp"

#define COUNT_PLATFORMS \
    (sizeof(PLATFORM_POSITIONS) / sizeof(PLATFORM_POSITIONS[0]))

static Instance INSTANCES[COUNT_PLATFORMS];
static Cube     PLATFORMS[COUNT_PLATFORMS];

static Simd4f32 linear_combine(Simd4f32 a, Mat4 b) {
    Simd4f32 c;
    c = _mm_mul_ps(_mm_shuffle_ps(a, a, 0x00), b.column[0]);
    c = _mm_add_ps(c, _mm_mul_ps(_mm_shuffle_ps(a, a, 0x55), b.column[1]));
    c = _mm_add_ps(c, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xAA), b.column[2]));
    c = _mm_add_ps(c, _mm_mul_ps(_mm_shuffle_ps(a, a, 0xFF), b.column[3]));
    return c;
}

static Vec3& operator*=(Vec3& l, Vec3 r) {
    l.x *= r.x;
    l.y *= r.y;
    l.z *= r.z;
    return l;
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

static Cube scene_get_cube(Mat4 matrix) {
    const f32 width_half = matrix.cell[0][0] / 2.0f;
    const f32 height_half = matrix.cell[1][1] / 2.0f;
    const f32 depth_half = matrix.cell[2][2] / 2.0f;
    return {
        {
            matrix.cell[3][0] - width_half,
            matrix.cell[3][1] - height_half,
            matrix.cell[3][2] - depth_half,
        },
        {
            matrix.cell[3][0] + width_half,
            matrix.cell[3][1] + height_half,
            matrix.cell[3][2] + depth_half,
        },
    };
}

static void scene_set_instances() {
    const Mat4 matrix = scale({10.0f, 0.5f, 10.0f});
    for (u8 i = 0; i < COUNT_PLATFORMS; ++i) {
        INSTANCES[i].matrix = translate(PLATFORM_POSITIONS[i]) * matrix;
        INSTANCES[i].color = {
            cosf(static_cast<f32>(i * 2)),
            sinf(static_cast<f32>(i * 3)),
            (sinf(static_cast<f32>(i * 5)) + cosf(static_cast<f32>(i * 7))) /
                2.0f,
        };
        INSTANCES[i].color *= INSTANCES[i].color;
        PLATFORMS[i] = scene_get_cube(INSTANCES[i].matrix);
    }
}

static void show(Mat4 m) {
    printf("{.cell={");
    for (u8 i = 0; i < 4; ++i) {
        printf("{");
        for (u8 j = 0; j < 3; ++j) {
            printf("%ff,", static_cast<f64>(m.cell[i][j]));
        }
        printf("%ff},", static_cast<f64>(m.cell[i][3]));
    }
    printf("}}");
}

static void show(Vec3 v) {
    printf("{%ff,%ff,%ff}",
           static_cast<f64>(v.x),
           static_cast<f64>(v.y),
           static_cast<f64>(v.z));
}

i32 main() {
    scene_set_instances();
    printf("#ifndef __SCENE_ASSETS_CODEGEN_H__\n"
           "#define __SCENE_ASSETS_CODEGEN_H__\n"
           "#include \"scene_assets.hpp\"\n"
           "#define COUNT_PLATFORMS %zu\n",
           COUNT_PLATFORMS);
    {
        printf("static const Instance INSTANCES[COUNT_PLATFORMS] = {");
        for (u8 i = 0; i < COUNT_PLATFORMS; ++i) {
            printf("{");
            show(INSTANCES[i].matrix);
            printf(",");
            show(INSTANCES[i].color);
            printf(",},");
        }
        printf("};\n");
    }
    {
        printf("static const Cube PLATFORMS[COUNT_PLATFORMS] = {");
        for (u8 i = 0; i < COUNT_PLATFORMS; ++i) {
            printf("{");
            show(PLATFORMS[i].bottom_left_front);
            printf(",");
            show(PLATFORMS[i].top_right_back);
            printf(",},");
        }
        printf("};\n");
    }
    printf("#endif\n");
    return EXIT_SUCCESS;
}
