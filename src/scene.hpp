#ifndef __SCENE_H__
#define __SCENE_H__

#include "init.hpp"
#include "scene_assets_codegen.hpp"

struct Object {
    u32 vertex_array;
    u32 vertex_buffer;
    u32 element_buffer;
    u32 instance_buffer;
    u32 frame_buffer;
    u32 render_buffer_color;
    u32 render_buffer_depth;
};

static Object OBJECT;

#define INDEX_VERTEX   0
#define INDEX_NORMAL   1
#define INDEX_INSTANCE 2

#define VERTEX_OFFSET 0

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

static void scene_set_vertex_attrib(u32         index,
                                    i32         size,
                                    i32         stride,
                                    const void* offset) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, false, stride, offset);
}

template <usize W, usize H>
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
        scene_set_vertex_attrib(INDEX_VERTEX, position_width, stride, null);
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
            scene_set_vertex_attrib(
                index,
                3,
                stride,
                reinterpret_cast<void*>(offsetof(Instance, color)));
            glVertexAttribDivisor(index, 1);
        }
        CHECK_GL_ERROR();
    }
    {
        glGenRenderbuffers(1, &OBJECT.render_buffer_color);
        glBindRenderbuffer(GL_RENDERBUFFER, OBJECT.render_buffer_color);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, W, H);
        CHECK_GL_ERROR();
    }
    {
        glGenRenderbuffers(1, &OBJECT.render_buffer_depth);
        glBindRenderbuffer(GL_RENDERBUFFER, OBJECT.render_buffer_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, W, H);
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

template <usize W, usize H>
static void scene_draw(GLFWwindow* window, i32 width, i32 height) {
    {
        // NOTE: Bind off-screen render target.
        glBindFramebuffer(GL_FRAMEBUFFER, OBJECT.frame_buffer);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glViewport(0, 0, W, H);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    {
        // NOTE: Draw scene.
        glBindVertexArray(OBJECT.vertex_array);
        glDrawElementsInstanced(GL_TRIANGLES,
                                sizeof(INDICES) / sizeof(INDICES[0]),
                                GL_UNSIGNED_INT,
                                reinterpret_cast<void*>(VERTEX_OFFSET),
                                COUNT_PLATFORMS);
    }
    {
        // NOTE: Blit off-screen to on-screen.
        glBindFramebuffer(GL_READ_FRAMEBUFFER, OBJECT.frame_buffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        glBlitFramebuffer(0,
                          0,
                          W,
                          H,
                          0,
                          0,
                          width,
                          height,
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
