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

#define INIT_PLAYER_POSITION_X 0.0f
#define INIT_PLAYER_POSITION_Y 40.0f
#define INIT_PLAYER_POSITION_Z 0.0f

#define PLAYER_WIDTH  1.0f
#define PLAYER_HEIGHT 4.0f
#define PLAYER_DEPTH  1.0f

static const f32 PLAYER_WIDTH_HALF = PLAYER_WIDTH / 2.0f;
static const f32 PLAYER_DEPTH_HALF = PLAYER_DEPTH / 2.0f;

#define RUN       0.00325f
#define SPEED_MAX 0.125f
#define FRICTION  0.96f
#define DRAG      0.99f

#define WORLD_Y_MIN -20.0f

static const f32 SPEED_MAX_SQUARED = SPEED_MAX * SPEED_MAX;

#define JUMP    0.0585f
#define GRAVITY 0.000345f

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

#define MICROSECONDS 1000000.0f

#define FRAME_UPDATE_COUNT 8

static const f32 FRAME_DURATION = (1.0f / 60.0f) * MICROSECONDS;
static const f32 FRAME_UPDATE_STEP = FRAME_DURATION / FRAME_UPDATE_COUNT;

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
    state->player.position.x = INIT_PLAYER_POSITION_X;
    state->player.position.y = INIT_PLAYER_POSITION_Y;
    state->player.position.z = INIT_PLAYER_POSITION_Z;
}

static Cube get_cube_below(Player player) {
    f32  top = player.position.y - PLAYER_HEIGHT;
    Vec3 bottom_left_front = {
        .x = player.position.x - PLAYER_WIDTH_HALF,
        // NOTE: `player.speed.y` *should* be negative if we're checking
        // _below_ the player!
        .y = top + player.speed.y,
        .z = player.position.z - PLAYER_DEPTH_HALF,
    };
    Vec3 top_right_back = {
        .x = player.position.x + PLAYER_WIDTH_HALF,
        .y = top,
        .z = player.position.z + PLAYER_DEPTH_HALF,
    };
    Cube cube = {
        .bottom_left_front = bottom_left_front,
        .top_right_back = top_right_back,
    };
    return cube;
}

static Cube get_cube_above(Player player) {
    Vec3 bottom_left_front = {
        .x = player.position.x - PLAYER_WIDTH_HALF,
        .y = player.position.y,
        .z = player.position.z - PLAYER_DEPTH_HALF,
    };
    Vec3 top_right_back = {
        .x = player.position.x + PLAYER_WIDTH_HALF,
        .y = player.position.y + player.speed.y,
        .z = player.position.z + PLAYER_DEPTH_HALF,
    };
    Cube cube = {
        .bottom_left_front = bottom_left_front,
        .top_right_back = top_right_back,
    };
    return cube;
}

static Bool intersect_player_platform(Cube player, Cube platform) {
    return (player.bottom_left_front.x <= platform.top_right_back.x) &&
        (platform.bottom_left_front.x <= player.top_right_back.x) &&
        (player.bottom_left_front.y <= platform.top_right_back.y) &&
        (platform.bottom_left_front.y <= player.top_right_back.y) &&
        (player.bottom_left_front.z <= platform.top_right_back.z) &&
        (platform.bottom_left_front.z <= player.top_right_back.z);
}

#define NORM_CROSS(a, b) norm_vec3(cross_vec3(a, b))

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
}

#define WITHIN_EPSILON(x) ((-EPSILON < (x)) && ((x) < EPSILON))

static void set_motion(State* state) {
    if (state->player.position.y < WORLD_Y_MIN) {
        init_player(state);
        return;
    }
    state->player.speed.y -= GRAVITY;
    state->player.position.y += state->player.speed.y;
    f32 x_speed = state->player.speed.x * DRAG;
    f32 z_speed = state->player.speed.z * DRAG;
    if (state->player.speed.y <= 0.0f) {
        Cube below = get_cube_below(state->player);
        for (u8 i = 0; i < COUNT_PLATFORMS; ++i) {
            if (intersect_player_platform(below, PLATFORMS[i])) {
                state->player.position.y =
                    PLATFORMS[i].top_right_back.y + PLAYER_HEIGHT;
                state->player.speed.y = 0.0f;
                state->player.can_jump = TRUE;
                x_speed = state->player.speed.x * FRICTION;
                z_speed = state->player.speed.z * FRICTION;
                break;
            }
        }
    } else {
        Cube above = get_cube_above(state->player);
        for (u8 i = 0; i < COUNT_PLATFORMS; ++i) {
            if (intersect_player_platform(above, PLATFORMS[i])) {
                state->player.position.y = PLATFORMS[i].bottom_left_front.y;
                state->player.speed.y = 0.0f;
                break;
            }
        }
    }
    {
        if (SPEED_MAX_SQUARED < ((x_speed * x_speed) + (z_speed * z_speed))) {
            f32 radians = atan2f(z_speed, x_speed);
            state->player.speed.x = SPEED_MAX * cosf(radians);
            state->player.speed.z = SPEED_MAX * sinf(radians);
        } else {
            state->player.speed.x = x_speed;
            state->player.speed.z = z_speed;
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
               state->player.speed.x,
               state->player.speed.y,
               state->player.speed.z,
               VIEW_TARGET.x,
               VIEW_TARGET.y,
               VIEW_TARGET.z);
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
            set_motion(&state);
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
           "sizeof(Instance)       : %zu\n"
           "sizeof(Cube)           : %zu\n"
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
           sizeof(Instance),
           sizeof(Cube),
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
