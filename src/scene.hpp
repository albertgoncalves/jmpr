#ifndef __SCENE_H__
#define __SCENE_H__

#include "init.hpp"
#include "math.hpp"

#define FRAME_BUFFER_SCALE 4

#define FRAME_BUFFER_WIDTH  (INIT_WINDOW_WIDTH / FRAME_BUFFER_SCALE)
#define FRAME_BUFFER_HEIGHT (INIT_WINDOW_HEIGHT / FRAME_BUFFER_SCALE)

struct Object {
    u32 vertex_array;
    u32 vertex_buffer;
    u32 element_buffer;
    u32 instance_buffer;
    u32 frame_buffer;
    u32 render_buffer_color;
    u32 render_buffer_depth;
};

struct Instance {
    Mat4 matrix;
    Vec3 color;
};

struct Cube {
    Vec3 bottom_left_front;
    Vec3 top_right_back;
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

#define INDEX_VERTEX   0
#define INDEX_NORMAL   1
#define INDEX_INSTANCE 2

#define VERTEX_OFFSET 0

#define COUNT_PLATFORMS \
    (sizeof(PLATFORM_POSITIONS) / sizeof(PLATFORM_POSITIONS[0]))
#define COUNT_INSTANCES COUNT_PLATFORMS

static Object   OBJECT;
static Instance INSTANCES[COUNT_INSTANCES];
static Cube     PLATFORMS[COUNT_PLATFORMS];

#define CHECK_GL_ERROR()                               \
    {                                                  \
        switch (glGetError()) {                        \
        case GL_INVALID_ENUM: {                        \
            ERROR("GL_INVALID_ENUM");                  \
        }                                              \
        case GL_INVALID_VALUE: {                       \
            ERROR("GL_INVALID_VALUE");                 \
        }                                              \
        case GL_INVALID_OPERATION: {                   \
            ERROR("GL_INVALID_OPERATION");             \
        }                                              \
        case GL_INVALID_FRAMEBUFFER_OPERATION: {       \
            ERROR("GL_INVALID_FRAMEBUFFER_OPERATION"); \
        }                                              \
        case GL_OUT_OF_MEMORY: {                       \
            ERROR("GL_OUT_OF_MEMORY");                 \
        }                                              \
        case GL_NO_ERROR: {                            \
            break;                                     \
        }                                              \
        }                                              \
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
        INSTANCES[i].color.x = cosf(static_cast<f32>(i * 2));
        INSTANCES[i].color.y = sinf(static_cast<f32>(i * 3));
        INSTANCES[i].color.z =
            (sinf(static_cast<f32>(i * 5)) + cosf(static_cast<f32>(i * 7))) /
            2.0f;
        INSTANCES[i].color *= INSTANCES[i].color;
        PLATFORMS[i] = scene_get_cube(INSTANCES[i].matrix);
    }
}

static void scene_set_vertex_attrib(u32         index,
                                    i32         size,
                                    i32         stride,
                                    const void* offset) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, FALSE, stride, offset);
}

static void scene_set_buffers() {
    glGenVertexArrays(1, &OBJECT.vertex_array);
    glBindVertexArray(OBJECT.vertex_array);
    CHECK_GL_ERROR();
    {
        glGenBuffers(1, &OBJECT.vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, OBJECT.vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(VERTICES),
                     VERTICES,
                     GL_STATIC_DRAW);
        const i32 position_width = 3;
        const i32 normal_width = 3;
        const i32 stride = sizeof(f32) * (position_width + normal_width);
#if VERTEX_OFFSET == 0
        scene_set_vertex_attrib(INDEX_VERTEX, position_width, stride, nullptr);
#else
        set_vertex_attrib(INDEX_VERTEX,
                          position_width,
                          stride,
                          reinterpret_cast<void*>(VERTEX_OFFSET));
#endif
        scene_set_vertex_attrib(
            INDEX_NORMAL,
            normal_width,
            stride,
            reinterpret_cast<void*>(sizeof(f32) * position_width));
        CHECK_GL_ERROR();
    }
    {
        glGenBuffers(1, &OBJECT.element_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, OBJECT.element_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(INDICES),
                     INDICES,
                     GL_STATIC_DRAW);
        CHECK_GL_ERROR();
    }
    {
        scene_set_instances();
        glGenBuffers(1, &OBJECT.instance_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, OBJECT.instance_buffer);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(INSTANCES),
                     &INSTANCES[0].matrix.cell[0][0],
                     GL_STATIC_DRAW);
        const i32 stride = sizeof(INSTANCES[0]);
        // NOTE: Instances are limited to `sizeof(f32) * 4`, so `Instance` must
        // be constructed in multiple layers.
        const usize offset = sizeof(f32) * 4;
        for (u32 i = 0; i < 4; ++i) {
            const u32 index = INDEX_INSTANCE + i;
            scene_set_vertex_attrib(index,
                                    4,
                                    stride,
                                    reinterpret_cast<void*>(i * offset));
            glVertexAttribDivisor(index, 1);
        }
        {
            const u32 index = INDEX_INSTANCE + 4;
            scene_set_vertex_attrib(index,
                                    3,
                                    stride,
                                    reinterpret_cast<void*>(4 * offset));
            glVertexAttribDivisor(index, 1);
        }
        CHECK_GL_ERROR();
    }
    {
        glGenRenderbuffers(1, &OBJECT.render_buffer_color);
        glBindRenderbuffer(GL_RENDERBUFFER, OBJECT.render_buffer_color);
        glRenderbufferStorage(GL_RENDERBUFFER,
                              GL_RGB,
                              FRAME_BUFFER_WIDTH,
                              FRAME_BUFFER_HEIGHT);
        CHECK_GL_ERROR();
    }
    {
        glGenRenderbuffers(1, &OBJECT.render_buffer_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, OBJECT.render_buffer_depth);
        glRenderbufferStorage(GL_RENDERBUFFER,
                              GL_DEPTH_COMPONENT,
                              FRAME_BUFFER_WIDTH,
                              FRAME_BUFFER_HEIGHT);
        CHECK_GL_ERROR();
    }
    {
        glGenFramebuffers(1, &OBJECT.frame_buffer);
        glBindFramebuffer(GL_FRAMEBUFFER, OBJECT.frame_buffer);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT0,
                                  GL_RENDERBUFFER,
                                  OBJECT.render_buffer_color);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  OBJECT.render_buffer_depth);
        CHECK_GL_ERROR();
    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        CHECK_GL_ERROR();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    CHECK_GL_ERROR();
}

static void scene_draw(GLFWwindow* window) {
    {
        // NOTE: Bind off-screen render target.
        glBindFramebuffer(GL_FRAMEBUFFER, OBJECT.frame_buffer);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    {
        // NOTE: Draw scene.
        glBindVertexArray(OBJECT.vertex_array);
        glDrawElementsInstanced(GL_TRIANGLES,
                                sizeof(INDICES) / sizeof(INDICES[0]),
                                GL_UNSIGNED_INT,
                                reinterpret_cast<void*>(VERTEX_OFFSET),
                                COUNT_INSTANCES);
    }
    {
        // NOTE: Blit off-screen to on-screen.
        glBindFramebuffer(GL_READ_FRAMEBUFFER, OBJECT.frame_buffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glBlitFramebuffer(0,
                          0,
                          FRAME_BUFFER_WIDTH,
                          FRAME_BUFFER_HEIGHT,
                          0,
                          0,
                          WINDOW_WIDTH,
                          WINDOW_HEIGHT,
                          GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                          GL_NEAREST);
    }
    glfwSwapBuffers(window);
}

static void scene_delete_buffers() {
    glDeleteVertexArrays(1, &OBJECT.vertex_array);
    glDeleteBuffers(1, &OBJECT.vertex_buffer);
    glDeleteBuffers(1, &OBJECT.element_buffer);
    glDeleteBuffers(1, &OBJECT.instance_buffer);
    glDeleteFramebuffers(1, &OBJECT.frame_buffer);
    glDeleteRenderbuffers(1, &OBJECT.render_buffer_color);
    glDeleteRenderbuffers(1, &OBJECT.render_buffer_depth);
}

#endif
