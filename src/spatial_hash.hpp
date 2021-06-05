#ifndef __SPATIAL_HASH_H__
#define __SPATIAL_HASH_H__

#include "scene.hpp"

#define GRID_X 10
#define GRID_Y 10
#define GRID_Z 10

#define GRID_VEC3 \
    ((Vec3){      \
        GRID_X,   \
        GRID_Y,   \
        GRID_Z,   \
    })

#define GRID_EPSILON 0.01f

#define CAP_LISTS (2 << 6)

struct Index {
    u8 x;
    u8 y;
    u8 z;
};

struct Range {
    Index bottom;
    Index top;
};

struct List {
    Cube* cube;
    List* next;
    List* last;
};

struct GridMemory {
    List  grid[GRID_X][GRID_Y][GRID_Z];
    Cube  bounds;
    Vec3  span;
    List  lists[CAP_LISTS];
    u8    len_lists;
    Cube* intersects[COUNT_PLATFORMS];
    u8    len_intersects;
};

static List* hash_alloc_list(GridMemory* memory) {
    EXIT_IF(CAP_LISTS <= memory->len_lists);
    List* list = &memory->lists[memory->len_lists++];
    list->cube = nullptr;
    list->next = nullptr;
    list->last = nullptr;
    return list;
}

static void hash_set_bounds(GridMemory* memory) {
    memory->bounds = PLATFORMS[0];
    for (u8 i = 0; i < COUNT_PLATFORMS; ++i) {
        memory->bounds.bottom_left_front =
            min(memory->bounds.bottom_left_front,
                PLATFORMS[i].bottom_left_front);
        memory->bounds.top_right_back =
            max(memory->bounds.top_right_back, PLATFORMS[i].top_right_back);
    }
    memory->bounds.top_right_back += GRID_EPSILON;
    memory->span =
        memory->bounds.top_right_back - memory->bounds.bottom_left_front;
}

static Range hash_get_range(GridMemory* memory, Cube cube) {
    const Vec3 bottom_left_front =
        ((cube.bottom_left_front - memory->bounds.bottom_left_front) /
         memory->span) *
        GRID_VEC3;
    const Vec3 top_right_back =
        ((cube.top_right_back - memory->bounds.bottom_left_front) /
         memory->span) *
        GRID_VEC3;
    return {
        {
            static_cast<u8>(bottom_left_front.x),
            static_cast<u8>(bottom_left_front.y),
            static_cast<u8>(bottom_left_front.z),
        },
        {
            static_cast<u8>(top_right_back.x),
            static_cast<u8>(top_right_back.y),
            static_cast<u8>(top_right_back.z),
        },
    };
}

static void hash_push_grid(GridMemory* memory, Index grid_index, Cube* cube) {
    List* grid = &memory->grid[grid_index.x][grid_index.y][grid_index.z];
    if (!grid->cube) {
        grid->cube = cube;
        grid->next = nullptr;
        grid->last = nullptr;
        return;
    }
    List* list = hash_alloc_list(memory);
    list->cube = cube;
    if (!grid->next) {
        grid->next = list;
        grid->last = list;
        return;
    }
    grid->last->next = list;
    grid->last = list;
}

static void hash_set_grid(GridMemory* memory) {
    memset(memory->grid, 0, sizeof(memory->grid));
    memory->len_lists = 0;
    for (u8 i = 0; i < COUNT_PLATFORMS; ++i) {
        const Range range = hash_get_range(memory, PLATFORMS[i]);
        for (u8 x = range.bottom.x; x <= range.top.x; ++x) {
            for (u8 y = range.bottom.y; y <= range.top.y; ++y) {
                for (u8 z = range.bottom.z; z <= range.top.z; ++z) {
                    hash_push_grid(memory, {x, y, z}, &PLATFORMS[i]);
                }
            }
        }
    }
}

static Cube hash_get_within_bounds(GridMemory* memory, Cube cube) {
    const Vec3 top_right_back = memory->bounds.top_right_back - GRID_EPSILON;
    return {
        clip(cube.bottom_left_front,
             memory->bounds.bottom_left_front,
             top_right_back),
        clip(cube.top_right_back,
             memory->bounds.bottom_left_front,
             top_right_back),
    };
}

static void hash_set_intersects(GridMemory* memory, Cube cube) {
    memory->len_intersects = 0;
    const Range range =
        hash_get_range(memory, hash_get_within_bounds(memory, cube));
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
