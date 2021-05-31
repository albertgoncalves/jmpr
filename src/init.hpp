#ifndef __INIT_H__
#define __INIT_H__

#include "prelude.hpp"

#include <string.h>

#define GL_GLEXT_PROTOTYPES

#include <GLFW/glfw3.h>

// NOTE: This is a hack to hide the mouse cursor; at the moment, seems like
// `glfwSetInputMode(..., GLFW_CURSOR_DISABLED)` doesn't work as intended.
// See `https://github.com/glfw/glfw/issues/1790`.
#define GLFW_EXPOSE_NATIVE_X11

#include <GLFW/glfw3native.h>
#include <X11/extensions/Xfixes.h>

#define INIT_WINDOW_WIDTH  1024
#define INIT_WINDOW_HEIGHT 768

static i32 WINDOW_WIDTH = INIT_WINDOW_WIDTH;
static i32 WINDOW_HEIGHT = INIT_WINDOW_HEIGHT;

struct Native {
    Display* display;
    Window   window;
};

struct BufferMemory {
    char buffer[2 << 9];
};

static void init_set_file(BufferMemory* memory, const char* filename) {
    File* file = fopen(filename, "r");
    EXIT_IF(!file);
    fseek(file, 0, SEEK_END);
    const u32 file_size = static_cast<u32>(ftell(file));
    EXIT_IF(sizeof(memory->buffer) <= file_size);
    rewind(file);
    EXIT_IF(fread(&memory->buffer, sizeof(char), file_size, file) !=
            file_size);
    memory->buffer[file_size] = '\0';
    fclose(file);
}

static void init_hide_cursor(Native native) {
    XFixesHideCursor(native.display, native.window);
    XFlush(native.display);
}

static void init_show_cursor(Native native) {
    XFixesShowCursor(native.display, native.window);
    XFlush(native.display);
}

static void init_framebuffer_size_callback(GLFWwindow* _,
                                           i32         width,
                                           i32         height) {
    (void)_;
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}

static GLFWwindow* init_get_window(const char* name) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(INIT_WINDOW_WIDTH,
                                          INIT_WINDOW_HEIGHT,
                                          name,
                                          nullptr,
                                          nullptr);
    if (!window) {
        glfwTerminate();
        ERROR("!window");
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, init_framebuffer_size_callback);
    glfwSetWindowAspectRatio(window, INIT_WINDOW_WIDTH, INIT_WINDOW_HEIGHT);
    // NOTE: While mouse *does* get locked to center of window, it remains
    // visible. See `https://github.com/glfw/glfw/issues/1790`.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // NOTE: This seems to have no effect. See
    // `https://github.com/glfw/glfw/issues/1559`.
    glfwSwapInterval(1);
    return window;
}

static u32 init_get_shader(BufferMemory* memory,
                           const char*   filename,
                           u32           type) {
    init_set_file(memory, filename);
    const u32   shader = glCreateShader(type);
    const char* source = memory->buffer;
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    i32 status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        memset(memory->buffer, 0, sizeof(memory->buffer));
        glGetShaderInfoLog(shader,
                           sizeof(memory->buffer),
                           nullptr,
                           memory->buffer);
        ERROR(memory->buffer);
    }
    return shader;
}

static u32 init_get_program(BufferMemory* memory,
                            u32           vertex_shader,
                            u32           fragment_shader) {
    const u32 program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    i32 status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        memset(memory->buffer, 0, sizeof(memory->buffer));
        glGetProgramInfoLog(program,
                            sizeof(memory->buffer),
                            nullptr,
                            memory->buffer);
        ERROR(memory->buffer);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program;
}

#endif
