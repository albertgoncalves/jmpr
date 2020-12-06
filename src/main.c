#include "prelude.h"

#include <string.h>
#include <unistd.h>

#include "graphics.h"

typedef struct {
    Vec3 position;
    Vec3 speed;
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

#define EPSILON 0.0001f

#define RUN      0.004f
#define RUN_MAX  0.125f
#define FRICTION 0.945f
#define DRAG     0.965f

static const f32 RUN_MAX_SQUARED = RUN_MAX * RUN_MAX;

#define JUMP    0.045f
#define GRAVITY 0.00035f;

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
    {
        state->player.speed.y -= GRAVITY;
        if (state->player.position.y < VIEW_EYE_Y) {
            state->player.position.y = VIEW_EYE_Y;
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
    if (WITHIN_EPSILON(state->player.speed.y)) {
        state->player.speed.y = 0.0f;
    } else {
        state->player.position.y += state->player.speed.y;
    }
    if (WITHIN_EPSILON(state->player.speed.z)) {
        state->player.speed.z = 0.0f;
    } else {
        state->player.position.z += state->player.speed.z;
    }
}

static void set_frame(Frame* frame, const State* state) {
    f32 now = (f32)glfwGetTime() * MICROSECONDS;
    f32 elapsed = (now - frame->time);
    if (elapsed < FRAME_DURATION) {
        usleep((u32)(FRAME_DURATION - elapsed));
    }
    if (++frame->fps_count == 30) {
        printf("\033[5A"
               "fps     :%8.2f\n"
               "eye     :%8.2f%8.2f%8.2f\n"
               "target  :%8.2f%8.2f%8.2f\n"
               "up      :%8.2f%8.2f%8.2f\n"
               "speed   :%8.2f%8.2f%8.2f\n",
               (frame->fps_count / (now - frame->fps_time)) * MICROSECONDS,
               VIEW_EYE.x,
               VIEW_EYE.y,
               VIEW_EYE.z,
               VIEW_TARGET.x,
               VIEW_TARGET.y,
               VIEW_TARGET.z,
               VIEW_UP.x,
               VIEW_UP.y,
               VIEW_UP.z,
               state->player.speed.x,
               state->player.speed.y,
               state->player.speed.z);
        frame->fps_time = frame->time;
        frame->fps_count = 0;
    }
}

static void set_dynamic_uniforms(Uniforms uniforms, const State* state) {
    glUniform1f(uniforms.time, state->time);
    PROJECTION = perspective_mat4(get_radians(45.0f),
                                  (f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT,
                                  VIEW_NEAR,
                                  VIEW_FAR);
    glUniformMatrix4fv(uniforms.projection, 1, FALSE, &PROJECTION.cell[0][0]);
    VIEW = look_at_mat4(VIEW_EYE, add_vec3(VIEW_EYE, VIEW_TARGET), VIEW_UP);
    glUniformMatrix4fv(uniforms.view, 1, FALSE, &VIEW.cell[0][0]);
    CHECK_GL_ERROR();
}

static void loop(GLFWwindow* window, u32 program) {
    State state = {0};
    state.player.position = VIEW_EYE;
    state.player.position.y = 20.0f;
    state.player.can_jump = TRUE;
    Frame    frame = {0};
    Uniforms uniforms = get_uniforms(program);
    glUseProgram(program);
    set_static_uniforms(uniforms);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    printf("\n\n\n\n\n");
    while (!glfwWindowShouldClose(window)) {
        state.time = (f32)glfwGetTime();
        frame.time = state.time * MICROSECONDS;
        frame.delta += frame.time - frame.prev;
        while (FRAME_UPDATE_STEP < frame.delta) {
            set_input(window, &state);
            frame.delta -= FRAME_UPDATE_STEP;
        }
        VIEW_EYE = state.player.position;
        set_dynamic_uniforms(uniforms, &state);
        draw(window);
        set_frame(&frame, &state);
        frame.prev = frame.time;
    }
}

static void init_cursor_callback(GLFWwindow* window, f64 x, f64 y) {
    CURSOR_CALLBACK(x, y);
    glfwSetCursorPosCallback(window, cursor_callback);
}

static void error_callback(i32 code, const char* error) {
    fprintf(stderr, "%d: %s\n", code, error);
    exit(EXIT_FAILURE);
}

i32 main(i32 n, const char** args) {
    printf("GLFW version: %s\n\n", glfwGetVersionString());
    Memory* memory = calloc(1, sizeof(Memory));
    if (!memory) {
        ERROR("`calloc` failed");
    }
    printf("sizeof(Bool)           : %zu\n"
           "sizeof(Vec3)           : %zu\n"
           "sizeof(Mat4)           : %zu\n"
           "sizeof(Frame)          : %zu\n"
           "sizeof(Uniforms)       : %zu\n"
           "sizeof(Player)         : %zu\n"
           "sizeof(State)          : %zu\n"
           "sizeof(Memory)         : %zu\n"
           "sizeof(memory->buffer) : %zu\n\n",
           sizeof(Bool),
           sizeof(Vec3),
           sizeof(Mat4),
           sizeof(Player),
           sizeof(Frame),
           sizeof(Uniforms),
           sizeof(State),
           sizeof(Memory),
           sizeof(memory->buffer));
    if (n < 3) {
        ERROR("Missing args");
    }
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        ERROR("!glfwInit()");
    }
    GLFWwindow* window = get_window("float");
    u32         program = get_program(memory,
                              get_shader(memory, args[1], GL_VERTEX_SHADER),
                              get_shader(memory, args[2], GL_FRAGMENT_SHADER));
    set_objects();
    Native native = {
        .display = glfwGetX11Display(),
        .window = glfwGetX11Window(window),
    };
    hide_cursor(native);
    glfwSetCursorPosCallback(window, init_cursor_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, TRUE);
    loop(window, program);
    show_cursor(native);
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
