#include "scene.h"

#include <unistd.h>

typedef struct {
    i32 time;
    i32 projection;
    i32 view;
} Uniforms;

typedef struct {
    Vec3 position;
    Vec3 speed;
    f32  height;
    Bool can_jump;
} Player;

typedef struct {
    Player player;
    f32    time;
} State;

typedef struct {
    f32 time;
    f32 prev;
    f32 delta;
    f32 fps_time;
    u8  fps_count;
} Frame;

#define MICROSECONDS 1000000.0f

#define FRAME_UPDATE_COUNT 10

static const f32 FRAME_DURATION = (1.0f / 60.0f) * MICROSECONDS;
static const f32 FRAME_UPDATE_STEP = FRAME_DURATION / FRAME_UPDATE_COUNT;

#define RUN      0.004f
#define RUN_MAX  0.125f
#define FRICTION 0.96f
#define DRAG     0.99f

static const f32 RUN_MAX_SQUARED = RUN_MAX * RUN_MAX;

#define JUMP    0.045f
#define GRAVITY 0.00035f

static const Vec3 VIEW_UP = {
    .x = 0.0f, // NOTE: `x`-axis is left/right.
    .y = 1.0f, // NOTE: `y`-axis is up/down.
    .z = 0.0f, // NOTE: `z`-axis is forward/back.
};

static Vec3 VIEW_TARGET = {
    .x = 0.0f,
    .y = 0.0f,
    .z = -1.0f,
};

#define CURSOR_SENSITIVITY 0.1f

static f32 CURSOR_X;
static f32 CURSOR_Y;

static f32 CURSOR_X_DELTA = 0.0f;
static f32 CURSOR_Y_DELTA = 0.0f;

#define VIEW_NEAR 0.1f
#define VIEW_FAR  1000.0f

static f32 VIEW_YAW = -90.0f;
static f32 VIEW_PITCH = 0.0f;

#define PITCH_LIMIT 89.0f

#define EPSILON 0.0001f

static void init_player(State* state) {
    state->player.position.x = 0.0f;
    state->player.position.y = 20.0f;
    state->player.position.z = 0.0f;
    state->player.height = 4.0f;
    state->player.can_jump = TRUE;
}

#define NORM_CROSS(a, b) norm_vec3(cross_vec3(a, b))

#define WITHIN_EPSILON(x) ((-EPSILON < (x)) && ((x) < EPSILON))

static void set_input(GLFWwindow* window, State* state) {
    glfwPollEvents();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        state->player.speed = sub_vec3(
            state->player.speed,
            mul_vec3_f32(NORM_CROSS(cross_vec3(VIEW_TARGET, VIEW_UP), VIEW_UP),
                         RUN));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        state->player.speed =
            add_vec3(state->player.speed,
                     mul_vec3_f32(NORM_CROSS(VIEW_TARGET, VIEW_UP), RUN));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        state->player.speed = add_vec3(
            state->player.speed,
            mul_vec3_f32(NORM_CROSS(cross_vec3(VIEW_TARGET, VIEW_UP), VIEW_UP),
                         RUN));
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        state->player.speed =
            sub_vec3(state->player.speed,
                     mul_vec3_f32(NORM_CROSS(VIEW_TARGET, VIEW_UP), RUN));
    }
    if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) &&
        (state->player.can_jump))
    {
        state->player.speed.y += JUMP;
        state->player.can_jump = FALSE;
    }
    state->player.speed.y -= GRAVITY;
    state->player.position.y += state->player.speed.y;
    {
        if (state->player.position.y < state->player.height) {
            state->player.position.y = state->player.height;
            state->player.speed.y = 0.0f;
            state->player.can_jump = TRUE;
            state->player.speed.x *= FRICTION;
            state->player.speed.z *= FRICTION;
        } else {
            state->player.speed.x *= DRAG;
            state->player.speed.z *= DRAG;
        }
    }
    {
        f32 x = state->player.speed.x;
        f32 z = state->player.speed.z;
        if (RUN_MAX_SQUARED < ((x * x) + (z * z))) {
            f32 radians = atan2f(z, x);
            state->player.speed.x = RUN_MAX * cosf(radians);
            state->player.speed.z = RUN_MAX * sinf(radians);
        }
    }
    if (WITHIN_EPSILON(state->player.speed.x)) {
        state->player.speed.x = 0.0f;
    } else {
        state->player.position.x += state->player.speed.x;
    }
    if (WITHIN_EPSILON(state->player.speed.z)) {
        state->player.speed.z = 0.0f;
    } else {
        state->player.position.z += state->player.speed.z;
    }
}

static void set_uniforms(Uniforms uniforms, const State* state) {
    glUniform1f(uniforms.time, state->time);
    Mat4 projection = perspective_mat4(get_radians(45.0f),
                                       (f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT,
                                       VIEW_NEAR,
                                       VIEW_FAR);
    glUniformMatrix4fv(uniforms.projection, 1, FALSE, &projection.cell[0][0]);
    Mat4 view = look_at_mat4(state->player.position,
                             add_vec3(state->player.position, VIEW_TARGET),
                             VIEW_UP);
    glUniformMatrix4fv(uniforms.view, 1, FALSE, &view.cell[0][0]);
    CHECK_GL_ERROR();
}

static void set_debug(Frame* frame, const State* state) {
    f32 now = (f32)glfwGetTime() * MICROSECONDS;
    f32 elapsed = (now - frame->time);
    if (elapsed < FRAME_DURATION) {
        usleep((u32)(FRAME_DURATION - elapsed));
    }
    if (++frame->fps_count == 30) {
        printf("\033[4A"
               "fps      :%8.2f\n"
               "position :%8.2f%8.2f%8.2f\n"
               "speed    :%8.2f%8.2f%8.2f\n"
               "target   :%8.2f%8.2f%8.2f\n",
               (frame->fps_count / (now - frame->fps_time)) * MICROSECONDS,
               state->player.position.x,
               state->player.position.y,
               state->player.position.z,
               VIEW_TARGET.x,
               VIEW_TARGET.y,
               VIEW_TARGET.z,
               state->player.speed.x,
               state->player.speed.y,
               state->player.speed.z);
        frame->fps_time = frame->time;
        frame->fps_count = 0;
    }
}

static void loop(GLFWwindow* window, u32 program) {
    State state = {0};
    init_player(&state);
    Frame frame = {0};
    set_program(program);
    Uniforms uniforms = {
        .time = glGetUniformLocation(program, "U_TIME"),
        .projection = glGetUniformLocation(program, "U_PROJECTION"),
        .view = glGetUniformLocation(program, "U_VIEW"),
    };
    printf("\n\n\n\n");
    while (!glfwWindowShouldClose(window)) {
        state.time = (f32)glfwGetTime();
        frame.time = state.time * MICROSECONDS;
        frame.delta += frame.time - frame.prev;
        while (FRAME_UPDATE_STEP < frame.delta) {
            set_input(window, &state);
            frame.delta -= FRAME_UPDATE_STEP;
        }
        set_uniforms(uniforms, &state);
        draw(window);
        set_debug(&frame, &state);
        frame.prev = frame.time;
    }
}

static void error_callback(i32 code, const char* error) {
    fprintf(stderr, "%d: %s\n", code, error);
    exit(EXIT_FAILURE);
}

#define CURSOR_CALLBACK(x, y)                                            \
    {                                                                    \
        CURSOR_X = (f32)x;                                               \
        CURSOR_Y = (f32)y;                                               \
        VIEW_YAW += CURSOR_X_DELTA;                                      \
        VIEW_PITCH += CURSOR_Y_DELTA;                                    \
        if (PITCH_LIMIT < VIEW_PITCH) {                                  \
            VIEW_PITCH = PITCH_LIMIT;                                    \
        } else if (VIEW_PITCH < -PITCH_LIMIT) {                          \
            VIEW_PITCH = -PITCH_LIMIT;                                   \
        }                                                                \
        VIEW_TARGET.x =                                                  \
            cosf(get_radians(VIEW_YAW)) * cosf(get_radians(VIEW_PITCH)); \
        VIEW_TARGET.y = sinf(get_radians(VIEW_PITCH));                   \
        VIEW_TARGET.z =                                                  \
            sinf(get_radians(VIEW_YAW)) * cosf(get_radians(VIEW_PITCH)); \
        VIEW_TARGET = norm_vec3(VIEW_TARGET);                            \
    }

static void cursor_callback(GLFWwindow* _, f64 x, f64 y) {
    CURSOR_X_DELTA = ((f32)x - CURSOR_X) * CURSOR_SENSITIVITY;
    CURSOR_Y_DELTA = (CURSOR_Y - (f32)y) * CURSOR_SENSITIVITY;
    CURSOR_CALLBACK(x, y);
}

static void init_cursor_callback(GLFWwindow* window, f64 x, f64 y) {
    CURSOR_CALLBACK(x, y);
    glfwSetCursorPosCallback(window, cursor_callback);
}

i32 main(i32 n, const char** args) {
    if (n < 3) {
        ERROR("Missing args");
    }
    Memory* memory = calloc(1, sizeof(Memory));
    if (!memory) {
        ERROR("`calloc` failed");
    }
    printf("GLFW version: %s\n\n"
           "sizeof(Bool)           : %zu\n"
           "sizeof(Vec3)           : %zu\n"
           "sizeof(Mat4)           : %zu\n"
           "sizeof(Frame)          : %zu\n"
           "sizeof(Uniforms)       : %zu\n"
           "sizeof(Player)         : %zu\n"
           "sizeof(State)          : %zu\n"
           "sizeof(Memory)         : %zu\n"
           "sizeof(memory->buffer) : %zu\n\n",
           glfwGetVersionString(),
           sizeof(Bool),
           sizeof(Vec3),
           sizeof(Mat4),
           sizeof(Player),
           sizeof(Frame),
           sizeof(Uniforms),
           sizeof(State),
           sizeof(Memory),
           sizeof(memory->buffer));
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        ERROR("!glfwInit()");
    }
    GLFWwindow* window = get_window("float");
    glfwSetCursorPosCallback(window, init_cursor_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, TRUE);
    u32 program = get_program(memory,
                              get_shader(memory, args[1], GL_VERTEX_SHADER),
                              get_shader(memory, args[2], GL_FRAGMENT_SHADER));
    set_buffers();
    {
        Native native = {
            .display = glfwGetX11Display(),
            .window = glfwGetX11Window(window),
        };
        hide_cursor(native);
        loop(window, program);
        show_cursor(native);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &IBO);
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);
    glDeleteRenderbuffers(1, &DBO);
    glDeleteProgram(program);
    glfwTerminate();
    free(memory);
    return EXIT_SUCCESS;
}
