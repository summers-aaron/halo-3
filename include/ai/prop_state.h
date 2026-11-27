#ifndef AI_PROP_STATE_H
#define AI_PROP_STATE_H

#include "math/real_math_types.h"
#include "objects/object_reference_types.h"
#include "ai/sector_ref.h"
#include "ai/ai_point3d.h"

// AI prop actor state.
struct prop_state {
    union {
        long flags;
        long index0;
        long unknown;
    };
    union {
        real_point3d position;
        real_point3d body_position;
        real_point3d base_position;
    };
    real_point3d center;
    real_vector3d velocity;
    union
    {
        real_vector3d aiming;
        real_point3d aiming_position;
    };
    s_location location;
    union {
        real_point3d head_position;
        real_point3d target_position;
        struct {
            real_point2d head_position_xy;
            float height;
        };
    };
    union {
        long object_index;
        long index44;
        long vehicle_index;
        long parent;
        long parent_object_index;
    };
    union {
        c_sector_ref pathfinding_sector;
        struct {
            short s48;
            unsigned short s4a;
        };
    };
    union {
        c_ai_point3d pathfinding_point;
        c_ai_point3d path_point;
        struct {
            long target[3];
            short s58;
            short s5a;
        };
    };
    union {
        long inactive_time;
        long index5c;
    };
    union {
        long posture;
        long zero60;
    };
    union { bool inactive; bool orphaned; bool b64; };
    bool b65;
    bool b66;
    bool b67;
    union { bool heard_shooting; bool b68; };
    union { bool dead; bool b69; };
    bool b6a;
    bool b6b;
    union { bool prefer_target; bool b6c; };
    bool b6d;
    union { bool valid; bool b6e; };
    char pad6f;
};

typedef char prop_state_size[(sizeof(prop_state) == 0x70) ? 1 : -1];

#endif // AI_PROP_STATE_H
