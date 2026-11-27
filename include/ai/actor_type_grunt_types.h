#ifndef INCLUDE_AI_ACTOR_TYPE_GRUNT_TYPES_H
#define INCLUDE_AI_ACTOR_TYPE_GRUNT_TYPES_H

#include <stddef.h>
#include "ai/behavior.h"
#include "ai/ai_point3d.h"
#include "ai/sector_ref.h"
#include "ai/firing_position_ref.h"
#include "math/real_math_types.h"
#include "objects/cluster_reference.h"

struct s_pursuit_position
{
    unsigned short pursuit_type;
    union
    {
        unsigned short cluster_index;
        s_cluster_reference cluster_reference;
        struct { unsigned char cluster_lo, cluster_hi; };
    };
    c_sector_ref sector;
    c_ai_point3d aim_point;
};

typedef char pursuit_position_size[(sizeof(s_pursuit_position) == 0x18) ? 1 : -1];
typedef char pursuit_position_sector_offset[(offsetof(s_pursuit_position, sector) == 4) ? 1 : -1];
typedef char pursuit_position_point_offset[(offsetof(s_pursuit_position, aim_point) == 8) ? 1 : -1];

struct joint_invitation_iterator { long actor_index; short index; short type; bool done; };

struct joint_member { long actor_index; unsigned short status; short desire; float weight; };

struct joint_vehicle_flip_data {
    long vehicle_index;
    short ready_count;
    short actor_count;
    real_vector3d direction;
    short position_count;
};

typedef char joint_vehicle_flip_size[(sizeof(joint_vehicle_flip_data) == 0x18) ? 1 : -1];

struct joint_state_datum {
    union { long header; struct { short identifier; short datum_padding; }; };
    union { joint_member members[10]; joint_member participants[10]; };
    short behavior_index;
    union { short member_count; short participant_count; };
    union
    {
        bool active;
        unsigned char phase;
        struct
        {
            signed char status;
            long joint_type;
            long clump_index;
            short invitation_timer;
        };
    };
    union {
        joint_vehicle_flip_data vehicle_flip;
        struct { long time; short minimum_time, maximum_time; long target; bool complete; } presearch;
        struct { firing_position_ref firing_position; long actor_index; unsigned short unit_type; } huddle;
        struct { bool initialized; short formation; long target, anchor; } formation_data;
        struct {
            union {
                long object_index;
                long scared_actor_index;
                long flush_object_index;
                bool complete;
                bool engage;
                struct { bool initialized; bool failed; short formation; };
            };
            union
            {
                long leader_actor_index;
                long first_point_ref;
                long target;
                long link;
                long second_leader;
                short ready_count;
            };
            union { bool leader_scared; long second_point_ref; long last_update; long second_point; };
            struct
            {
                short timer;
                float position_time;
                union { s_pursuit_position position; unsigned short state; };
                float facing_time;
                real_vector3d facing;
            };
        };
    };
};

typedef char joint_state_datum_size[(sizeof(joint_state_datum) == 204) ? 1 : -1];
typedef char joint_state_members_offset[(offsetof(joint_state_datum, members) == 4) ? 1 : -1];
typedef char joint_state_count_offset[(offsetof(joint_state_datum, member_count) == 0x7e) ? 1 : -1];
typedef char joint_state_payload_offset[(offsetof(joint_state_datum, initialized) == 0x90) ? 1 : -1];

typedef char joint_state_first_point_offset[(offsetof(joint_state_datum, first_point_ref) == 0x94) ? 1 : -1];
typedef char joint_state_second_point_offset[(offsetof(joint_state_datum, second_point_ref) == 0x98) ? 1 : -1];

class c_flee_state_data : public c_behavior_state
{
public:
    firing_position_ref firing_position;
    long target;
    short elapsed_ticks;
    short scream_ticks;
    short position_exposure_ticks;
    unsigned char search_mode;
    bool needs_position;
    bool firing_position_locked;
    bool unable_to_flee;
    bool done_fleeing;
    bool target_resolved;
};

typedef char flee_state_size[(sizeof(c_flee_state_data) == 0x38) ? 1 : -1];
typedef char flee_state_position_offset[(offsetof(c_flee_state_data, firing_position) == 0x24) ? 1 : -1];
typedef char flee_state_target_offset[(offsetof(c_flee_state_data, target) == 0x28) ? 1 : -1];
typedef char flee_state_exposure_offset[(offsetof(c_flee_state_data, position_exposure_ticks) == 0x30) ? 1 : -1];
typedef char flee_state_resolved_offset[(offsetof(c_flee_state_data, target_resolved) == 0x37) ? 1 : -1];

typedef char joint_state_timer_offset[(offsetof(joint_state_datum, timer) == 0x9c) ? 1 : -1];
typedef char joint_state_position_offset[(offsetof(joint_state_datum, position) == 0xa4) ? 1 : -1];
typedef char joint_state_facing_offset[(offsetof(joint_state_datum, facing) == 0xc0) ? 1 : -1];
typedef char joint_member_status_offset[(offsetof(joint_member, status) == 4) ? 1 : -1];
typedef char joint_presearch_target_offset[(offsetof(joint_state_datum, presearch.target) == 0x98) ? 1 : -1];
typedef char joint_huddle_unit_type_offset[(offsetof(joint_state_datum, huddle.unit_type) == 0x98) ? 1 : -1];
typedef char joint_formation_anchor_offset[(offsetof(joint_state_datum, formation_data.anchor) == 0x98) ? 1 : -1];
typedef char joint_member_desire_offset[(offsetof(joint_member, desire) == 0x6) ? 1 : -1];
typedef char joint_member_weight_offset[(offsetof(joint_member, weight) == 0x8) ? 1 : -1];
typedef char joint_state_behavior_offset[(offsetof(joint_state_datum, behavior_index) == 0x7c) ? 1 : -1];
typedef char joint_state_joint_type_offset[(offsetof(joint_state_datum, joint_type) == 0x84) ? 1 : -1];
typedef char joint_state_clump_offset[(offsetof(joint_state_datum, clump_index) == 0x88) ? 1 : -1];
typedef char joint_state_invitation_timer_offset[(offsetof(joint_state_datum, invitation_timer) == 0x8c) ? 1 : -1];
typedef char joint_state_object_offset[(offsetof(joint_state_datum, object_index) == 0x90) ? 1 : -1];
typedef char joint_vehicle_flip_payload_offset[(offsetof(joint_state_datum, vehicle_flip) == 0x90) ? 1 : -1];
typedef char joint_vehicle_flip_direction_offset[(offsetof(joint_state_datum, vehicle_flip.direction) == 0x98) ? 1 : -1];
typedef char joint_vehicle_flip_position_count_offset[(offsetof(joint_state_datum, vehicle_flip.position_count) == 0xa4) ? 1 : -1];

#endif
