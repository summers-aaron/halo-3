#ifndef AI_PATH_STATE_TYPES_H
#define AI_PATH_STATE_TYPES_H

#include <stddef.h>
#include "ai/path_types.h"
#include "ai/path_result.h"
#include "ai/path_flying.h"
#include "ai/special_movement_types.h"
#include "ai/reserved_hints.h"

struct path_debug_storage;

// Search records and scratch state, as indexed by path.cpp's heap and hash walkers.
struct path_node
{
    short pad0;
    short parent_node_index;
    c_sector_ref link_sector_ref;
    c_sector_ref sector_ref;
    bool flag_c;
    bool flag_d;
    short pad_e;
    path_step_source source;
    c_ai_point3d point;
    float x28, x2c, x30, x34;
    float total_cost_estimate;
    short quantized_cost_estimate;
    short x3e;
    short heap_location;
    short pad42;
};

struct path_heap_entry { short node_index; short cost_estimate; };

struct path_state
{
    path_input input;
    path_debug_storage *debug;
    c_path_destination goal;
    bool has_goal;
    path_error_info error;
    short best_node_index;
    float best_x98, best_x9c;
    c_ai_point3d best_point;
    bool another_flag;
    char padb1;
    short node_count;
    path_node node_list[2048];
    short heap_count;
    path_heap_entry heap[2049];
    unsigned short hash_buckets[8192];
    special_movement movement;
    actor_moving_reserved_hints cache;
    flight_evaluation_cache flight_cache;
};

typedef char path_node_size[(sizeof(path_node) == 0x44) ? 1 : -1];
typedef char path_node_point_offset[(offsetof(path_node, point) == 0x18) ? 1 : -1];
typedef char path_state_size[(sizeof(void *) != 4 || ((sizeof(path_state) == 0x281dc))) ? 1 : -1];
typedef char path_state_error_offset[(sizeof(void *) != 4 || ((offsetof(path_state, error) == 0x78))) ? 1 : -1];
typedef char path_state_nodes_offset[(sizeof(void *) != 4 || ((offsetof(path_state, node_list) == 0xb4))) ? 1 : -1];
typedef char path_state_cache_offset[(sizeof(void *) != 4 || ((offsetof(path_state, flight_cache) == 0x280ea))) ? 1 : -1];

#endif // AI_PATH_STATE_TYPES_H
