#include "spatial_hash.h"

#include <unistd.h>

typedef struct {
    i32 time;
    i32 position;
    i32 projection;
    i32 view;
} Uniforms;

typedef struct {
    Vec3  position;
    Vec3  speed;
    Bool_ can_jump;
    Bool_ jump_key_released;
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

typedef struct {
    BufferMemory buffer;
    GridMemory   grid;
} Memory;

#define INIT_PLAYER_POSITION_X 0.0f
#define INIT_PLAYER_POSITION_Y 15.0f
#define INIT_PLAYER_POSITION_Z 5.0f

#define PLAYER_WIDTH  1.5f
#define PLAYER_HEIGHT 4.0f
#define PLAYER_DEPTH  1.5f

#define PLAYER_WIDTH_HALF (PLAYER_WIDTH / 2.0f)
#define PLAYER_DEPTH_HALF (PLAYER_DEPTH / 2.0f)

#define RUN      0.00325f
#define FRICTION 0.96f
#define DRAG     0.99f

#define SPEED_MAX         0.125f
#define SPEED_EPSILON     0.0001f
#define SPEED_MAX_SQUARED (SPEED_MAX * SPEED_MAX)

#define WORLD_Y_MIN -20.0f

#define JUMP    0.0585f
#define GRAVITY 0.000345f

#define VIEW_UP                                          \
    ((Vec3){                                             \
        .x = 0.0f, /* NOTE: `x`-axis is left/right.   */ \
        .y = 1.0f, /* NOTE: `y`-axis is down/up.      */ \
        .z = 0.0f, /* NOTE: `z`-axis is forward/back. */ \
    })

static Vec3 VIEW_TARGET;

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

#define MICROSECONDS 1000000.0f
#define MILLISECONDS 1000.0f

#define FRAME_UPDATE_COUNT 8.0f
#define FRAME_DURATION     ((1.0f / 60.0f) * MICROSECONDS)
#define FRAME_UPDATE_STEP  (FRAME_DURATION / FRAME_UPDATE_COUNT)

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
        state->player.jump_key_released = FALSE;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
        state->player.jump_key_released = TRUE;
    }
}

static void init_player(State* state) {
    state->player.position.x = INIT_PLAYER_POSITION_X;
    state->player.position.y = INIT_PLAYER_POSITION_Y;
    state->player.position.z = INIT_PLAYER_POSITION_Z;
    state->player.speed.x = 0.0f;
    state->player.speed.y = 0.0f;
    state->player.speed.z = 0.0f;
    state->player.can_jump = FALSE;
    state->player.jump_key_released = FALSE;
    VIEW_TARGET.x = 0.0f;
    VIEW_TARGET.y = 0.0f;
    VIEW_TARGET.z = -1.0f;
    CURSOR_X_DELTA = 0.0f;
    CURSOR_Y_DELTA = 0.0f;
    VIEW_YAW = -90.0f;
    VIEW_PITCH = 0.0f;
}

static Cube get_cube_below(Player player) {
    const f32 bottom = player.position.y - PLAYER_HEIGHT;
    return (Cube){
        .top_right_back =
            {
                .x = player.position.x + PLAYER_WIDTH_HALF,
                .y = bottom,
                .z = player.position.z + PLAYER_DEPTH_HALF,
            },
        .bottom_left_front =
            {
                .x = player.position.x - PLAYER_WIDTH_HALF,
                .y = bottom + player.speed.y,
                .z = player.position.z - PLAYER_DEPTH_HALF,
            },
    };
}

static Cube get_cube_above(Player player) {
    return (Cube){
        .bottom_left_front =
            {
                .x = player.position.x - PLAYER_WIDTH_HALF,
                .y = player.position.y,
                .z = player.position.z - PLAYER_DEPTH_HALF,
            },
        .top_right_back =
            {
                .x = player.position.x + PLAYER_WIDTH_HALF,
                .y = player.position.y + player.speed.y,
                .z = player.position.z + PLAYER_DEPTH_HALF,
            },
    };
}

static Cube get_cube_front(Player player) {
    const Vec3 top_right_back = {
        .x = player.position.x + PLAYER_WIDTH_HALF,
        .y = player.position.y,
        .z = player.position.z - PLAYER_DEPTH_HALF,
    };
    return (Cube){
        .top_right_back = top_right_back,
        .bottom_left_front =
            {
                .x = player.position.x - PLAYER_WIDTH_HALF,
                .y = player.position.y - PLAYER_HEIGHT,
                .z = top_right_back.z + player.speed.z,
            },
    };
}

static Cube get_cube_back(Player player) {
    const Vec3 bottom_left_front = {
        .x = player.position.x - PLAYER_WIDTH_HALF,
        .y = player.position.y - PLAYER_HEIGHT,
        .z = player.position.z + PLAYER_DEPTH_HALF,
    };
    return (Cube){
        .bottom_left_front = bottom_left_front,
        .top_right_back =
            {
                .x = player.position.x + PLAYER_WIDTH_HALF,
                .y = player.position.y,
                .z = bottom_left_front.z + player.speed.z,
            },
    };
}

static Cube get_cube_left(Player player) {
    const Vec3 top_right_back = {
        .x = player.position.x - PLAYER_WIDTH_HALF,
        .y = player.position.y,
        .z = player.position.z + PLAYER_DEPTH_HALF,
    };
    return (Cube){
        .top_right_back = top_right_back,
        .bottom_left_front =
            {
                .x = top_right_back.x + player.speed.x,
                .y = player.position.y - PLAYER_HEIGHT,
                .z = player.position.z - PLAYER_DEPTH_HALF,
            },
    };
}

static Cube get_cube_right(Player player) {
    const Vec3 bottom_left_front = {
        .x = player.position.x + PLAYER_WIDTH_HALF,
        .y = player.position.y - PLAYER_HEIGHT,
        .z = player.position.z - PLAYER_DEPTH_HALF,
    };
    return (Cube){
        .bottom_left_front = bottom_left_front,
        .top_right_back =
            {
                .x = bottom_left_front.x + player.speed.x,
                .y = player.position.y,
                .z = player.position.z + PLAYER_DEPTH_HALF,
            },
    };
}

#define INTERSECT_PLAYER_PLATFORM(player, platform)              \
    ((player.bottom_left_front.x < platform.top_right_back.x) && \
     (platform.bottom_left_front.x < player.top_right_back.x) && \
     (player.bottom_left_front.y < platform.top_right_back.y) && \
     (platform.bottom_left_front.y < player.top_right_back.y) && \
     (player.bottom_left_front.z < platform.top_right_back.z) && \
     (platform.bottom_left_front.z < player.top_right_back.z))

#define WITHIN_SPEED_EPSILON(x) \
    ((-SPEED_EPSILON < (x)) && ((x) < SPEED_EPSILON))

static void set_motion(GridMemory* memory, State* state) {
    if (state->player.position.y < WORLD_Y_MIN) {
        init_player(state);
        return;
    }
    state->player.speed.y -= GRAVITY;
    state->player.can_jump = FALSE;
    f32 x_speed = state->player.speed.x * DRAG;
    f32 z_speed = state->player.speed.z * DRAG;
    if (state->player.speed.y <= 0.0f) {
        const Cube below = get_cube_below(state->player);
        state->player.position.y += state->player.speed.y;
        set_intersects(memory, below);
        for (u8 i = 0; i < memory->len_intersects; ++i) {
            if (INTERSECT_PLAYER_PLATFORM(below, (*memory->intersects[i]))) {
                state->player.position.y =
                    memory->intersects[i]->top_right_back.y + PLAYER_HEIGHT;
                state->player.speed.y = 0.0f;
                x_speed = state->player.speed.x * FRICTION;
                z_speed = state->player.speed.z * FRICTION;
                if (state->player.jump_key_released) {
                    state->player.can_jump = TRUE;
                }
                break;
            }
        }
    } else {
        const Cube above = get_cube_above(state->player);
        state->player.position.y += state->player.speed.y;
        set_intersects(memory, above);
        for (u8 i = 0; i < memory->len_intersects; ++i) {
            if (INTERSECT_PLAYER_PLATFORM(above, (*memory->intersects[i]))) {
                state->player.position.y =
                    memory->intersects[i]->bottom_left_front.y;
                state->player.speed.y = 0.0f;
                break;
            }
        }
    }
    if (SPEED_MAX_SQUARED < ((x_speed * x_speed) + (z_speed * z_speed))) {
        const f32 radians = atan2f(z_speed, x_speed);
        state->player.speed.x = SPEED_MAX * cosf(radians);
        state->player.speed.z = SPEED_MAX * sinf(radians);
    } else {
        state->player.speed.x = x_speed;
        state->player.speed.z = z_speed;
    }
    state->player.position.y += GRAVITY;
    const Cube front_back = state->player.speed.z < 0.0f
                                ? get_cube_front(state->player)
                                : get_cube_back(state->player);
    const Cube left_right = state->player.speed.x < 0.0f
                                ? get_cube_left(state->player)
                                : get_cube_right(state->player);
    state->player.position.y -= GRAVITY;
    if (WITHIN_SPEED_EPSILON(state->player.speed.x)) {
        state->player.speed.x = 0.0f;
    } else {
        state->player.position.x += state->player.speed.x;
    }
    if (WITHIN_SPEED_EPSILON(state->player.speed.z)) {
        state->player.speed.z = 0.0f;
    } else {
        state->player.position.z += state->player.speed.z;
    }
    set_intersects(memory, front_back);
    for (u8 i = 0; i < memory->len_intersects; ++i) {
        if (INTERSECT_PLAYER_PLATFORM(front_back, (*memory->intersects[i]))) {
            state->player.position.z -= state->player.speed.z;
            state->player.speed.z = 0.0f;
        }
    }
    set_intersects(memory, left_right);
    for (u8 i = 0; i < memory->len_intersects; ++i) {
        if (INTERSECT_PLAYER_PLATFORM(left_right, (*memory->intersects[i]))) {
            state->player.position.x -= state->player.speed.x;
            state->player.speed.x = 0.0f;
        }
    }
}

static void set_uniforms(Uniforms uniforms, const State* state) {
    glUniform1f(uniforms.time, state->time);
    glUniform3f(uniforms.position,
                state->player.position.x,
                state->player.position.y,
                state->player.position.z);
    const Mat4 projection =
        perspective_mat4(get_radians(45.0f),
                         (f32)WINDOW_WIDTH / (f32)WINDOW_HEIGHT,
                         VIEW_NEAR,
                         VIEW_FAR);
    glUniformMatrix4fv(uniforms.projection, 1, FALSE, &projection.cell[0][0]);
    const Mat4 view =
        look_at_mat4(state->player.position,
                     add_vec3(state->player.position, VIEW_TARGET),
                     VIEW_UP);
    glUniformMatrix4fv(uniforms.view, 1, FALSE, &view.cell[0][0]);
    CHECK_GL_ERROR();
}

static void set_debug(Frame* frame, const State* state) {
    const f32 now = (f32)glfwGetTime() * MICROSECONDS;
    const f32 elapsed = (now - frame->time);
    if (elapsed < FRAME_DURATION) {
        usleep((u32)(FRAME_DURATION - elapsed));
    }
    if (++frame->fps_count == 60) {
        printf(
            "\033[5A"
            "fps      %8.2f\n"
            "mspf     %8.2f\n"
            "position %8.2f%8.2f%8.2f\n"
            "speed    %8.2f%8.2f%8.2f\n"
            "target   %8.2f%8.2f%8.2f\n",
            (f64)((frame->fps_count / (now - frame->fps_time)) * MICROSECONDS),
            (f64)(((now - frame->fps_time) / frame->fps_count) / MILLISECONDS),
            (f64)state->player.position.x,
            (f64)state->player.position.y,
            (f64)state->player.position.z,
            (f64)state->player.speed.x,
            (f64)state->player.speed.y,
            (f64)state->player.speed.z,
            (f64)VIEW_TARGET.x,
            (f64)VIEW_TARGET.y,
            (f64)VIEW_TARGET.z);
        frame->fps_time = (f32)glfwGetTime() * MICROSECONDS;
        frame->fps_count = 0;
    }
}

static void loop(GLFWwindow* window, GridMemory* memory, u32 program) {
    State state = {0};
    init_player(&state);
    Frame frame = {0};
    set_program(program);
    const Uniforms uniforms = {
        .time = glGetUniformLocation(program, "U_TIME"),
        .position = glGetUniformLocation(program, "U_POSITION"),
        .projection = glGetUniformLocation(program, "U_PROJECTION"),
        .view = glGetUniformLocation(program, "U_VIEW"),
    };
    printf("\n\n\n\n\n");
    set_bounds(memory);
    set_span(memory);
    init_grid(memory);
    while (!glfwWindowShouldClose(window)) {
        state.time = (f32)glfwGetTime();
        frame.time = state.time * MICROSECONDS;
        frame.delta += frame.time - frame.prev;
        while (FRAME_UPDATE_STEP < frame.delta) {
            set_input(window, &state);
            set_motion(memory, &state);
            frame.delta -= FRAME_UPDATE_STEP;
        }
        set_uniforms(uniforms, &state);
        {
            const f32 sin_height =
                sinf(fabsf(state.player.position.y) / 10.0f);
            glClearColor(sin_height, sin_height, sin_height, 1.0f);
        }
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
    (void)_;
    CURSOR_X_DELTA = ((f32)x - CURSOR_X) * CURSOR_SENSITIVITY;
    CURSOR_Y_DELTA = (CURSOR_Y - (f32)y) * CURSOR_SENSITIVITY;
    CURSOR_CALLBACK(x, y);
}

static void init_cursor_callback(GLFWwindow* window, f64 x, f64 y) {
    CURSOR_CALLBACK(x, y);
    glfwSetCursorPosCallback(window, cursor_callback);
}

i32 main(i32 n, const char** args) {
    Memory* memory = calloc(1, sizeof(Memory));
    EXIT_IF(!memory);
    printf("GLFW version : %s\n\n"
           "sizeof(Bool_)          : %zu\n"
           "sizeof(Vec3)           : %zu\n"
           "sizeof(Mat4)           : %zu\n"
           "sizeof(Instance)       : %zu\n"
           "sizeof(Cube)           : %zu\n"
           "sizeof(Native)         : %zu\n"
           "sizeof(BufferMemory)   : %zu\n"
           "sizeof(Index)          : %zu\n"
           "sizeof(Range)          : %zu\n"
           "sizeof(List)           : %zu\n"
           "sizeof(GridMemory)     : %zu\n"
           "sizeof(Player)         : %zu\n"
           "sizeof(Frame)          : %zu\n"
           "sizeof(Uniforms)       : %zu\n"
           "sizeof(State)          : %zu\n"
           "sizeof(Memory)         : %zu\n\n",
           glfwGetVersionString(),
           sizeof(Bool_),
           sizeof(Vec3),
           sizeof(Mat4),
           sizeof(Instance),
           sizeof(Cube),
           sizeof(Native),
           sizeof(BufferMemory),
           sizeof(Index),
           sizeof(Range),
           sizeof(List),
           sizeof(GridMemory),
           sizeof(Player),
           sizeof(Frame),
           sizeof(Uniforms),
           sizeof(State),
           sizeof(Memory));
    EXIT_IF(n < 3);
    glfwSetErrorCallback(error_callback);
    EXIT_IF(!glfwInit());
    GLFWwindow* window = get_window("float");
    glfwSetCursorPosCallback(window, init_cursor_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, TRUE);
    const u32 program =
        get_program(&memory->buffer,
                    get_shader(&memory->buffer, args[1], GL_VERTEX_SHADER),
                    get_shader(&memory->buffer, args[2], GL_FRAGMENT_SHADER));
    set_buffers();
    {
        const Native native = {
            .display = glfwGetX11Display(),
            .window = glfwGetX11Window(window),
        };
        hide_cursor(native);
        loop(window, &memory->grid, program);
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
