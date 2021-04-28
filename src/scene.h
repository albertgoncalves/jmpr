#ifndef __SCENE_H__
#define __SCENE_H__

#include "init.h"
#include "math.h"

typedef struct {
    Mat4 matrix;
    Vec3 color;
} Instance;

typedef struct {
    Vec3 bottom_left_front;
    Vec3 top_right_back;
} Cube;

// clang-format off
static const f32 VERTICES[] = {
    // NOTE: position       // NOTE: normal
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

#define VERTEX_OFFSET 0

static const Vec3 PLATFORM_POSITIONS[] = {
    {.x =   0.0f,  .y =  4.0f,   .z =   0.0f},
    {.x =   0.0f,  .y =  6.0f,   .z = -10.0f},
    {.x =   0.0f,  .y =  8.0f,   .z = -20.0f},
    {.x =  10.0f,  .y = 10.0f,   .z = -20.0f},
    {.x =  10.0f,  .y = 12.0f,   .z = -10.0f},
    {.x =  10.0f,  .y = 14.0f,   .z =   0.0f},
    {.x =  10.0f,  .y =  4.0f,   .z =   0.0f},
    {.x =  10.0f,  .y =  4.0f,   .z = -10.0f},
    {.x =  10.0f,  .y =  4.0f,   .z = -20.0f},
    {.x = -20.0f,  .y =  6.0f,   .z =   0.0f},
    {.x = -10.0f,  .y =  0.0f,   .z = -10.0f},
    {.x = -20.0f,  .y =  2.5f,   .z = -10.0f},
    {.x =  -6.25f, .y =  4.125f, .z = -25.0f},
    {.x =  -2.5f,  .y = 17.25f,  .z =  -7.5f},
    {.x =  -7.5f,  .y = 20.0f,   .z =   7.5f},
    {.x =  -7.5f,  .y = 20.0f,   .z =  17.5f},
    {.x = -17.5f,  .y = 20.0f,   .z =  17.5f},
};
// clang-format on

#define COUNT_PLATFORMS \
    (sizeof(PLATFORM_POSITIONS) / sizeof(PLATFORM_POSITIONS[0]))
#define COUNT_INSTANCES COUNT_PLATFORMS

static Instance INSTANCES[COUNT_INSTANCES];
static Cube     PLATFORMS[COUNT_PLATFORMS];

static u32 VAO;
static u32 VBO;
static u32 EBO;
static u32 IBO;
static u32 FBO;
static u32 RBO;
static u32 DBO;

static const u32 INDEX_VERTEX = 0;
static const u32 INDEX_NORMAL = 1;
static const u32 INDEX_INSTANCE = 2;

static Cube get_cube_mat4(Mat4 m) {
    const f32 width_half = m.cell[0][0] / 2.0f;
    const f32 height_half = m.cell[1][1] / 2.0f;
    const f32 depth_half = m.cell[2][2] / 2.0f;
    return (Cube){
        .bottom_left_front =
            {
                .x = m.cell[3][0] - width_half,
                .y = m.cell[3][1] - height_half,
                .z = m.cell[3][2] - depth_half,
            },
        .top_right_back =
            {
                .x = m.cell[3][0] + width_half,
                .y = m.cell[3][1] + height_half,
                .z = m.cell[3][2] + depth_half,
            },
    };
}

static void set_instances(void) {
    Mat4 scale = scale_mat4((Vec3){
        .x = 10.0f,
        .y = 0.5f,
        .z = 10.0f,
    });
    for (u8 i = 0; i < COUNT_PLATFORMS; ++i) {
        INSTANCES[i].matrix =
            mul_mat4(translate_mat4(PLATFORM_POSITIONS[i]), scale);
        INSTANCES[i].color.x = cosf((f32)(i * 2));
        INSTANCES[i].color.y = sinf((f32)(i * 3));
        INSTANCES[i].color.z =
            (sinf((f32)(i * 5)) + cosf((f32)(i * 7))) / 2.0f;
        INSTANCES[i].color.x *= INSTANCES[i].color.x;
        INSTANCES[i].color.y *= INSTANCES[i].color.y;
        INSTANCES[i].color.z *= INSTANCES[i].color.z;
        PLATFORMS[i] = get_cube_mat4(INSTANCES[i].matrix);
    }
}

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

static void set_vertex_attrib(u32         index,
                              i32         size,
                              i32         stride,
                              const void* offset) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, FALSE, stride, offset);
}

static void set_buffers(void) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    CHECK_GL_ERROR();
    {
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(VERTICES),
                     VERTICES,
                     GL_STATIC_DRAW);
        const i32 position_width = 3;
        const i32 normal_width = 3;
        const i32 stride =
            ((i32)(sizeof(f32))) * (position_width + normal_width);
        set_vertex_attrib(INDEX_VERTEX,
                          position_width,
                          stride,
                          (void*)VERTEX_OFFSET);
        set_vertex_attrib(INDEX_NORMAL,
                          normal_width,
                          stride,
                          (void*)(sizeof(f32) * (usize)position_width));
        CHECK_GL_ERROR();
    }
    {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(INDICES),
                     INDICES,
                     GL_STATIC_DRAW);
        CHECK_GL_ERROR();
    }
    {
        set_instances();
        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ARRAY_BUFFER, IBO);
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
            set_vertex_attrib(index, 4, stride, (void*)(i * offset));
            glVertexAttribDivisor(index, 1);
        }
        {
            const u32 index = INDEX_INSTANCE + 4;
            set_vertex_attrib(index, 3, stride, (void*)(4 * offset));
            glVertexAttribDivisor(index, 1);
        }
        CHECK_GL_ERROR();
    }
    {
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, FBO_WIDTH, FBO_HEIGHT);
        CHECK_GL_ERROR();
    }
    {
        glGenRenderbuffers(1, &DBO);
        glBindRenderbuffer(GL_RENDERBUFFER, DBO);
        glRenderbufferStorage(GL_RENDERBUFFER,
                              GL_DEPTH_COMPONENT,
                              FBO_WIDTH,
                              FBO_HEIGHT);
        CHECK_GL_ERROR();
    }
    {
        glGenFramebuffers(1, &FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_COLOR_ATTACHMENT0,
                                  GL_RENDERBUFFER,
                                  RBO);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                  GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  DBO);
        CHECK_GL_ERROR();
    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        CHECK_GL_ERROR();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    CHECK_GL_ERROR();
}

static void set_program(u32 program) {
    glUseProgram(program);
}

static void draw(GLFWwindow* window) {
    {
        // NOTE: Bind off-screen render target.
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glViewport(0, 0, FBO_WIDTH, FBO_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    {
        // NOTE: Draw scene.
        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_TRIANGLES,
                                sizeof(INDICES) / sizeof(INDICES[0]),
                                GL_UNSIGNED_INT,
                                (void*)VERTEX_OFFSET,
                                COUNT_INSTANCES);
    }
    {
        // NOTE: Blit off-screen to on-screen.
        glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glBlitFramebuffer(0,
                          0,
                          FBO_WIDTH,
                          FBO_HEIGHT,
                          0,
                          0,
                          WINDOW_WIDTH,
                          WINDOW_HEIGHT,
                          GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
                          GL_NEAREST);
    }
    glfwSwapBuffers(window);
}

#endif
