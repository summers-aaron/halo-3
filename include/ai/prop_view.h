#ifndef AI_PROP_VIEW_H
#define AI_PROP_VIEW_H

#include "math/real_math_types.h"
#include "ai/ai_point3d.h"
#include "ai/sector_ref.h"
#include "ai/actor_type_grunt_types.h"
#include "objects/cluster_reference.h"
#include <stddef.h>

struct pursuit_location
{
    unsigned short type;
    s_cluster_reference static_cluster_ref;
    c_sector_ref sector_ref;
    c_ai_point3d point;
};

// Per-actor perception, pursuit and orphan tracking for a prop.
struct prop_view {
    short flags;
    short status;
    short audibility;
    short ineffability;
    short search_state;
    short visible_ticks;
    long perception_time;
    union { long tracking_index; long last_visible_time; long object_index; };
    long acknowledge_time;
    c_ai_point3d pathfinding_point;
    real_vector3d direction;
    union { signed char awareness; signed char category; };
    long unreachable_time;
    long unassailable_until;
    float vitality;
    union { float salience; float scary_rating; };
    union { short sighting_status; short approach_ticks; };
    short search_stage;
    union {
        unsigned short pursuit_type;
        s_pursuit_position saved_position;
        s_pursuit_position pursuit;
        pursuit_location location;
    };
    short sighting_awareness;
    short sighting_search_state;
    short sighting_ticks;
    short sighting_count;
    union { real_vector3d sighting_direction; real_point3d sighting_point; real_vector3d saved_facing; };
    short orphan_ticks;
    long orphaned_time;
    real_point3d sighting_position;
};
typedef char prop_view_size[(sizeof(prop_view) == 0x8c) ? 1 : -1];
typedef char pursuit_location_size[(sizeof(pursuit_location) == 0x18) ? 1 : -1];
typedef char pursuit_location_cluster_offset[(offsetof(pursuit_location, static_cluster_ref) == 2) ? 1 : -1];
typedef char pursuit_location_sector_offset[(offsetof(pursuit_location, sector_ref) == 4) ? 1 : -1];
typedef char pursuit_location_point_offset[(offsetof(pursuit_location, point) == 8) ? 1 : -1];
typedef char prop_view_unreachable_offset[(offsetof(prop_view, unreachable_time) == 0x38) ? 1 : -1];
typedef char prop_view_search_stage_offset[(offsetof(prop_view, search_stage) == 0x4a) ? 1 : -1];
typedef char prop_view_orphaned_time_offset[(offsetof(prop_view, orphaned_time) == 0x7c) ? 1 : -1];
typedef char prop_view_point_offset[(offsetof(prop_view, pathfinding_point) == 0x18) ? 1 : -1];
typedef char prop_view_vitality_offset[(offsetof(prop_view, vitality) == 0x40) ? 1 : -1];
typedef char prop_view_pursuit_offset[(offsetof(prop_view, pursuit) == 0x4c) ? 1 : -1];
typedef char prop_view_sighting_direction_offset[(offsetof(prop_view, sighting_direction) == 0x6c) ? 1 : -1];
typedef char prop_view_sighting_offset[(offsetof(prop_view, sighting_position) == 0x80) ? 1 : -1];

#endif
