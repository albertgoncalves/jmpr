#ifndef __SPATIAL_HASH_H__
#define __SPATIAL_HASH_H__

#include "scene.h"

#define GRID_COUNT_X 10
#define GRID_COUNT_Y 10
#define GRID_COUNT_Z 10

#define GRID_EPSILON 0.01f

#define GRID_COUNT_LISTS 64

typedef struct {
    u8 x;
    u8 y;
    u8 z;
} Index;

typedef struct {
    Index bottom;
    Index top;
} Range;

typedef struct List List;

struct List {
    Cube* cube;
    List* next;
    List* last;
};

typedef struct {
    List  grid[GRID_COUNT_X][GRID_COUNT_Y][GRID_COUNT_Z];
    Cube  bounds;
    Vec3  span;
    List  lists[GRID_COUNT_LISTS];
    u8    len_lists;
    Cube* intersects[COUNT_PLATFORMS];
    u8    len_intersects;
} GridMemory;

static List* alloc_list(GridMemory* memory) {
    EXIT_IF(GRID_COUNT_LISTS <= memory->len_lists);
    List* list = &memory->lists[memory->len_lists++];
    list->cube = NULL;
    list->next = NULL;
    list->last = NULL;
    return list;
}

static void set_bounds(GridMemory* memory) {
    memory->bounds = PLATFORMS[0];
    for (u8 i = 0; i < COUNT_PLATFORMS; ++i) {
        memory->bounds.bottom_left_front.x =
            MIN(memory->bounds.bottom_left_front.x,
                PLATFORMS[i].bottom_left_front.x);
        memory->bounds.bottom_left_front.y =
            MIN(memory->bounds.bottom_left_front.y,
                PLATFORMS[i].bottom_left_front.y);
        memory->bounds.bottom_left_front.z =
            MIN(memory->bounds.bottom_left_front.z,
                PLATFORMS[i].bottom_left_front.z);
        memory->bounds.top_right_back.x = MAX(memory->bounds.top_right_back.x,
                                              PLATFORMS[i].top_right_back.x);
        memory->bounds.top_right_back.y = MAX(memory->bounds.top_right_back.y,
                                              PLATFORMS[i].top_right_back.y);
        memory->bounds.top_right_back.z = MAX(memory->bounds.top_right_back.z,
                                              PLATFORMS[i].top_right_back.z);
    }
    memory->bounds.top_right_back.x += GRID_EPSILON;
    memory->bounds.top_right_back.y += GRID_EPSILON;
    memory->bounds.top_right_back.z += GRID_EPSILON;
}

static void set_span(GridMemory* memory) {
    memory->span = (Vec3){
        .x = memory->bounds.top_right_back.x -
            memory->bounds.bottom_left_front.x,
        .y = memory->bounds.top_right_back.y -
            memory->bounds.bottom_left_front.y,
        .z = memory->bounds.top_right_back.z -
            memory->bounds.bottom_left_front.z,
    };
}

static Range get_range(GridMemory* memory, Cube cube) {
    return (Range){
        .bottom =
            (Index){
                .x = (u8)(((cube.bottom_left_front.x -
                            memory->bounds.bottom_left_front.x) /
                           memory->span.x) *
                          GRID_COUNT_X),
                .y = (u8)(((cube.bottom_left_front.y -
                            memory->bounds.bottom_left_front.y) /
                           memory->span.y) *
                          GRID_COUNT_Y),
                .z = (u8)(((cube.bottom_left_front.z -
                            memory->bounds.bottom_left_front.z) /
                           memory->span.z) *
                          GRID_COUNT_Z),
            },
        .top =
            (Index){
                .x = (u8)(((cube.top_right_back.x -
                            memory->bounds.bottom_left_front.x) /
                           memory->span.x) *
                          GRID_COUNT_X),
                .y = (u8)(((cube.top_right_back.y -
                            memory->bounds.bottom_left_front.y) /
                           memory->span.y) *
                          GRID_COUNT_Y),
                .z = (u8)(((cube.top_right_back.z -
                            memory->bounds.bottom_left_front.z) /
                           memory->span.z) *
                          GRID_COUNT_Z),
            },
    };
}

static void push_grid(GridMemory* memory, Index grid_index, Cube* cube) {
    List* grid = &memory->grid[grid_index.x][grid_index.y][grid_index.z];
    if (!grid->cube) {
        grid->cube = cube;
        grid->next = NULL;
        grid->last = NULL;
        return;
    }
    List* list = alloc_list(memory);
    list->cube = cube;
    if (!grid->next) {
        EXIT_IF(grid->last);
        grid->next = list;
        grid->last = list;
        return;
    }
    grid->last->next = list;
    grid->last = list;
}

static void set_grid(GridMemory* memory) {
    memset(memory->grid, 0, sizeof(memory->grid));
    memory->len_lists = 0;
    for (u8 i = 0; i < COUNT_PLATFORMS; ++i) {
        Range range = get_range(memory, PLATFORMS[i]);
        for (u8 x = range.bottom.x; x <= range.top.x; ++x) {
            for (u8 y = range.bottom.y; y <= range.top.y; ++y) {
                for (u8 z = range.bottom.z; z <= range.top.z; ++z) {
                    push_grid(memory,
                              ((Index){.x = x, .y = y, .z = z}),
                              &PLATFORMS[i]);
                }
            }
        }
    }
}

static Cube get_within_bounds(GridMemory* memory, Cube cube) {
    Vec3 top_right_back = (Vec3){
        .x = memory->bounds.top_right_back.x - GRID_EPSILON,
        .y = memory->bounds.top_right_back.y - GRID_EPSILON,
        .z = memory->bounds.top_right_back.z - GRID_EPSILON,
    };
    return (Cube){
        .bottom_left_front =
            {
                .x = CLIP(cube.bottom_left_front.x,
                          memory->bounds.bottom_left_front.x,
                          top_right_back.x),
                .y = CLIP(cube.bottom_left_front.y,
                          memory->bounds.bottom_left_front.y,
                          top_right_back.y),
                .z = CLIP(cube.bottom_left_front.z,
                          memory->bounds.bottom_left_front.z,
                          top_right_back.z),
            },
        .top_right_back =
            {
                .x = CLIP(cube.top_right_back.x,
                          memory->bounds.bottom_left_front.x,
                          top_right_back.x),
                .y = CLIP(cube.top_right_back.y,
                          memory->bounds.bottom_left_front.y,
                          top_right_back.y),
                .z = CLIP(cube.top_right_back.z,
                          memory->bounds.bottom_left_front.z,
                          top_right_back.z),
            },
    };
}

static void set_intersects(GridMemory* memory, Cube cube) {
    memory->len_intersects = 0;
    Range range = get_range(memory, get_within_bounds(memory, cube));
    for (u8 x = range.bottom.x; x <= range.top.x; ++x) {
        for (u8 y = range.bottom.y; y <= range.top.y; ++y) {
            for (u8 z = range.bottom.z; z <= range.top.z; ++z) {
                List* list = &memory->grid[x][y][z];
                while (list && (list->cube)) {
                    for (u8 i = 0; i < memory->len_intersects; ++i) {
                        if (list->cube == memory->intersects[i]) {
                            goto next;
                        }
                    }
                    memory->intersects[memory->len_intersects++] = list->cube;
                next:
                    list = list->next;
                }
            }
        }
    }
}

#endif
