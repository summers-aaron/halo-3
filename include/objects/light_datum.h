#ifndef OBJECTS_LIGHT_DATUM_H
#define OBJECTS_LIGHT_DATUM_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "objects/object_reference_types.h"

struct light_state {
    real_point3d position;
    real_point3d endpoint;
    float offset_distance;
    real_point3d valid_point;
    real_vector3d forward;
    real_vector3d up;
};
typedef light_state s_lights_placement;
typedef char light_state_size_check[(sizeof(light_state) == 0x40) ? 1 : -1];
typedef char light_state_forward_check[(offsetof(light_state, forward) == 0x28) ? 1 : -1];
struct light_datum {
    short identifier;
    unsigned short flags;
    long definition_index;
    long scenario_index;
    long first_cluster_reference_index;
    long creation_time;
    real_point3d bounding_sphere_center;
    float bounding_sphere_radius;
    real_point3d lod_bounding_sphere_center;
    float lod_bounding_sphere_radius;
    real_point3d collision_sphere_center;
    s_location location;
    long attachment_object_index;
    long attachment_flags;
    short attachment_index;
    short change_color;
    long owner_object_index;
    short owner_node_index;
    short pad56;
    real_point3d local_position;
    real_vector3d local_forward;
    real_vector3d local_up;
    union {
        struct {
            real_point3d position;
            real_point3d endpoint;
            float offset_distance;
            real_point3d valid_point;
            real_vector3d forward;
            real_vector3d up;
        };
        s_lights_placement placement;
    };
    float custom_radius;
    // Always 0.0 here and never read back. The March 2007 build computed a
    // parent-object dim factor into this slot, in code dead there and gone here.
    float reservedc0;
    float scale;
    float initial_scale;
    real_rgb_color color;
    float gel_offset_u;
    float gel_offset_v;
    // Always NONE (-1) from creation on; never read back, and no artifact names it.
    long reservede0;
};

typedef char light_datum_size_check[(sizeof(light_datum) == 0xe4) ? 1 : -1];
typedef char light_datum_position_check[(offsetof(light_datum, position) == 0x7c) ? 1 : -1];
typedef char light_datum_color_check[(offsetof(light_datum, color) == 0xcc) ? 1 : -1];
typedef char light_datum_gel_offset_check[(offsetof(light_datum, gel_offset_u) == 0xd8) ? 1 : -1];

#endif
