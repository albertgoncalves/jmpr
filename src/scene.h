#ifndef __SCENE_H__
#define __SCENE_H__

#include "init.h"
#include "math.h"

typedef struct {
    Vec3 bottom_left_front;
    Vec3 top_right_back;
} Cube;

#define CLEAR_COLOR 0.095f

// clang-format off
static const f32 VERTICES[] = {
    // NOTE: (x,y,z)
    -0.5f, -0.5f, -0.5f, //  0
     0.5f, -0.5f, -0.5f, //  1
     0.5f,  0.5f, -0.5f, //  2
    -0.5f,  0.5f, -0.5f, //  3
    -0.5f, -0.5f,  0.5f, //  4
     0.5f, -0.5f,  0.5f, //  5
     0.5f,  0.5f,  0.5f, //  6
    -0.5f,  0.5f,  0.5f, //  7
};
#define VERTEX_OFFSET 0
static const u32 INDICES[] = {
    0, 1, 2,
    2, 3, 0,
    4, 5, 6,
    6, 7, 4,
    7, 3, 0,
    0, 4, 7,
    6, 2, 1,
    1, 5, 6,
    0, 1, 5,
    5, 4, 0,
    3, 2, 6,
    6, 7, 3,
};
// clang-format on

static const Vec3 PLATFORM_POSITIONS[] = {
    {
        .x = 0.0f,
        .y = 4.0f,
        .z = 0.0f,
    },
    {
        .x = 0.0f,
        .y = 6.0f,
        .z = -10.0f,
    },
    {
        .x = 0.0f,
        .y = 8.0f,
        .z = -20.0f,
    },
    {
        .x = 10.0f,
        .y = 10.0f,
        .z = -20.0f,
    },
    {
        .x = 10.0f,
        .y = 12.0f,
        .z = -10.0f,
    },
    {
        .x = 10.0f,
        .y = 14.0f,
        .z = 0.0f,
    },
    {
        .x = 10.0f,
        .y = 4.0f,
        .z = 0.0f,
    },
    {
        .x = 10.0f,
        .y = 4.0f,
        .z = -10.0f,
    },
    {
        .x = 10.0f,
        .y = 4.0f,
        .z = -20.0f,
    },
};

#define COUNT_PLATFORMS \
    (sizeof(PLATFORM_POSITIONS) / sizeof(PLATFORM_POSITIONS[0]))
#define COUNT_TRANSLATIONS COUNT_PLATFORMS

static Mat4 TRANSLATIONS[COUNT_TRANSLATIONS];
static Cube PLATFORMS[COUNT_PLATFORMS];

static u32 VAO;
static u32 VBO;
static u32 EBO;
static u32 IBO;
static u32 FBO;
static u32 RBO;
static u32 DBO;

static const u32 INDEX_VERTEX = 0;
static const u32 INDEX_TRANSLATE = 1;

static Cube get_cube_mat4(Mat4 m) {
    f32  width_half = m.cell[0][0] / 2.0f;
    f32  height_half = m.cell[1][1] / 2.0f;
    f32  depth_half = m.cell[2][2] / 2.0f;
    Vec3 bottom_left_front = {
        .x = m.cell[3][0] - width_half,
        .y = m.cell[3][1] - height_half,
        .z = m.cell[3][2] - depth_half,
    };
    Vec3 top_right_back = {
        .x = m.cell[3][0] + width_half,
        .y = m.cell[3][1] + height_half,
        .z = m.cell[3][2] + depth_half,
    };
    Cube cube = {
        .bottom_left_front = bottom_left_front,
        .top_right_back = top_right_back,
    };
    return cube;
}

static void set_translations(void) {
    Vec3 scale = {
        .x = 8.0f,
        .y = 0.5f,
        .z = 8.0f,
    };
    for (u8 i = 0; i < COUNT_PLATFORMS; ++i) {
        TRANSLATIONS[i] =
            mul_mat4(translate_mat4(PLATFORM_POSITIONS[i]), scale_mat4(scale));
        PLATFORMS[i] = get_cube_mat4(TRANSLATIONS[i]);
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

static void set_vertex_attrib(u32 index, i32 size, i32 stride, void* offset) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, offset);
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
        i32 position_width = 3;
        i32 stride = ((i32)(sizeof(f32))) * position_width;
        set_vertex_attrib(INDEX_VERTEX,
                          position_width,
                          stride,
                          (void*)VERTEX_OFFSET);
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
        set_translations();
        glGenBuffers(1, &IBO);
        glBindBuffer(GL_ARRAY_BUFFER, IBO);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(TRANSLATIONS),
                     &TRANSLATIONS[0].cell[0][0],
                     GL_STATIC_DRAW);
        i32 stride = sizeof(TRANSLATIONS[0]);
        // NOTE: Instances are limited to `sizeof(f32) * 4`, so `Mat4` data
        // must be constructed in four parts.
        usize offset = sizeof(f32) * 4;
        for (u32 i = 0; i < 4; ++i) {
            u32 index = INDEX_TRANSLATE + i;
            set_vertex_attrib(index, 4, stride, (void*)(i * offset));
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
    glClearColor(CLEAR_COLOR, CLEAR_COLOR, CLEAR_COLOR, 1.0f);
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
                                COUNT_TRANSLATIONS);
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
