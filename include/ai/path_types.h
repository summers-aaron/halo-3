#ifndef AI_PATH_TYPES_H
#define AI_PATH_TYPES_H

#include <stddef.h>

#include "ai/ai_point3d.h"
#include "ai/hint_ref.h"
#include "ai/sector_ref.h"

struct path_input
{
    float radius;
    long unit_index;
    long destination_object_index;
    c_ai_point3d start;
    c_sector_ref start_sector_ref;
    real_point3d attractor;
    long attractor_object_index;
    float attractor_radius;
    union { float attractor_strength; long attractor_type; }; // Attractor strength stored as a float.
    float attractor_weight;
    float maximum_distance;
    float hint_cost;
    bool start_set;
    union { bool allow_broken_sectors; unsigned char pathfinding_type; };
    bool bounded_search;
    char pad47;
    bool use_attractor;
    bool allow_narrow_links;
    bool allow_vehicle_hints;
    char : 8;
};

struct path_step_source
{
    short type;
    unsigned short structure_bsp_index;
    long index;
};

struct path_error_info
{
    short a, b;
    union
    {
        long data[6];
        path_step_source links[3];
    };
};

struct path_step
{
    short type;
    short flags;
    c_sector_ref sector;
    c_hint_ref hint;
    c_ai_point3d point;
};

// Traversal scratch links built per sector by build_path_links_for_sector.
struct path_link
{
    c_sector_ref sector_ref;
    short sector_flags;
    short : 16;
    union { struct {
    short hint_type;
    short structure_bsp_index;
    long index;
    }; path_step_source source; };
    c_ai_point3d point;
    real_vector3d vector;
    bool narrow;
    bool allow_narrow;
    short : 16;
};

typedef char path_link_size[(sizeof(path_link) == 0x30) ? 1 : -1];
typedef char path_input_size[(sizeof(path_input) == 0x4c) ? 1 : -1];
typedef char path_input_unit_offset[(offsetof(path_input, unit_index) == 4) ? 1 : -1];
typedef char path_input_destination_object_offset[(offsetof(path_input, destination_object_index) == 8) ? 1 : -1];
typedef char path_input_attractor_object_offset[(offsetof(path_input, attractor_object_index) == 0x2c) ? 1 : -1];
typedef char path_step_source_size[(sizeof(path_step_source) == 8) ? 1 : -1];
typedef char path_error_info_size[(sizeof(path_error_info) == 0x1c) ? 1 : -1];
typedef char path_step_size[(sizeof(path_step) == 0x1c) ? 1 : -1];

#endif // AI_PATH_TYPES_H
