// ailibxbox/actor_type_grunt.obj: from the records 0x5752b8 0x575428 0x5755f8 0x575650 0x575770 0x575878 0x5758e8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "ai/ai_direction.h"
#include "ai/actor_type_grunt_types.h"
#include <string.h>
#include "ai/ai_point3d.h"
#include "ai/joint_behavior.h"
#include "ai/joint_behavior_types.h"
// @address 0x5755f8 ?behavior_scared_by_leader_deactivate@@YAXJPAVc_behavior_state@@PAUjoint_state_datum@@@Z
// @address 0x575770 ?behavior_scared_by_leader_control@@YAXJPAVc_behavior_state@@PAUjoint_state_datum@@@Z
// @address 0x575878 ?behavior_scared_by_leader_select_actors@@YAFJJPAVc_behavior_state@@@Z
// @address 0x5758e8 ?behavior_scared_by_leader_flush_actor@@YAXJPAVc_behavior_state@@J@Z

/* Clears both leader references when either refers to the flushed actor. */
#include "memory/tls_slot_aliases.h"
#include "math/real_math.h"
#include <math.h>
#include "ai/actor_moving.h"
#include "ai/pathfinding_utilities.h"
#include "ai/actor_type_grunt.h"

#include "ai/sector_ref.h"
#include "ai/actor_moving.h"
#include "ai/actor_datum.h"
#include "ai/clump_datum.h"
#include "math/real_math_types.h"
#include "ai/clump.h"
#include "ai/clump_iterator.h"
#include "game/game_time_inlines.h"
#include "memory/data.h"

static __forceinline bool actor_type_grunt_behavior_flee_setup(c_flee_state_data *flee_state, long actor_index, long leader_index, long ticks)
{
    flee_state->target_resolved = true;
    flee_state->target = leader_index;
    flee_state->position_exposure_ticks = (short)ticks;
    return true;
}

struct path_state;

// 0x5752b8
short behavior_scared_by_leader_next(long actor_index, c_behavior_state *behavior_state, c_behavior_state *next_state)
{
    c_joint_behavior_data *joint_data = (c_joint_behavior_data *)behavior_state;
    short result = -2;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    joint_state_datum *joint_state = datum_get<joint_state_datum>(tls_joint_state_data(), joint_data->joint_index);
    if (joint_state->scared_actor_index == -1 || joint_state->leader_actor_index == -1) {
        result = -1;
    } else if (joint_data->initiator) {
        if (joint_state->leader_scared) {
            long ticks = game_seconds_integer_to_ticks(3);
            if (actor_type_grunt_behavior_flee_setup((c_flee_state_data *)next_state, actor_index, joint_state->leader_actor_index, ticks))
                result = 84;
            else
                result = -1;
        }
    } else {
        actor_datum *leader = datum_get<actor_datum>(tls_actor_data(), joint_state->scared_actor_index);
        real_vector3d direction;
        vector_from_points3d(&actor->position.body, &leader->position.body, &direction);
        direction.k = 0.0f;
        normalize3d(&direction);
        if (dot_product3d(&actor->facing_vector, &direction) > 0.8 &&
            ::actor_impulse_submit(actor_index, 51, false, &direction)) {
            joint_state->leader_scared = true;
            result = -1;
        }
    }
    return result;
}

void behavior_scared_by_leader_deactivate(long actor_index, c_behavior_state *behavior_state, joint_state_datum *joint_state)
{
    actor_datum *actor = &((actor_datum *)tls_actor_data()->data)[(unsigned short)actor_index];
    long clump_index = actor->clump_index;
    if (clump_index != -1) {
        clump_datum *clump = &((clump_datum *)tls_clump_data()->data)[(unsigned short)clump_index];
        clump->last_scared_time = tls_game_time_globals()->time;
    }
}

void behavior_scared_by_leader_control(long actor_index, c_behavior_state *behavior_state, joint_state_datum *joint_state)
{
    c_joint_behavior_data *state = (c_joint_behavior_data *)behavior_state;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    long leader_actor_index;
    if (state->initiator)
        leader_actor_index = joint_state->leader_actor_index;
    else
        leader_actor_index = joint_state->scared_actor_index;
    actor_datum *leader = (actor_datum *)inl_datum_try_and_get(tls_actor_data(), leader_actor_index);
    if (leader) {
        real_vector3d direction;
        vector_from_points3d(&actor->position.head, &leader->position.head, &direction);
        normalize3d(&direction);
        actor->look_mode = 3;
        actor->look_direction.to_vector(&direction);
    }
}

void behavior_scared_by_leader_flush_actor(long actor_index, c_behavior_state *behavior_state, long flushed_actor_index)
{
    c_joint_behavior_data *joint_data = (c_joint_behavior_data *)behavior_state;
    joint_state_datum *leader_state = datum_get<joint_state_datum>(tls_joint_state_data(), joint_data->joint_index);
    if (leader_state->scared_actor_index == flushed_actor_index || leader_state->leader_actor_index == flushed_actor_index) {
        leader_state->scared_actor_index = -1;
        leader_state->leader_actor_index = -1;
    }
}

// 0x575878
short behavior_scared_by_leader_select_actors(long actor_index, long joint_index, c_behavior_state *behavior_state)
{
    short invited_count = 0;
    joint_state_datum *leader_state = datum_get<joint_state_datum>(tls_joint_state_data(), joint_index);
    if (invite_actor(actor_index, leader_state->leader_actor_index, joint_index, 3, 1.0f))
        invited_count++;
    return invited_count;
}

// @address 0x575650 ?behavior_scared_by_leader_perform@@YA_NJPAVc_behavior_state@@PAUjoint_state_datum@@@Z
bool behavior_scared_by_leader_perform(long actor_index, c_behavior_state *behavior_state, joint_state_datum *joint_state)
{
    c_joint_behavior_data *joint_data = (c_joint_behavior_data *)behavior_state;
    bool result = true;
    long first_leader = joint_state->scared_actor_index;
    if (first_leader != -1) {
        long second_leader = joint_state->leader_actor_index;
        if (second_leader != -1) {
            if (joint_data->initiator) {
                actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
                if (actor->uncovering != 0) {
                    actor_datum *leader = (actor_datum *)datum_try_and_get(tls_actor_data(), second_leader);
                    if (leader != 0) {
                        c_destination_orders orders;
                        memset(&orders.action.direction, 0, sizeof(orders.action.direction));
                        memset(&orders.action, 0, sizeof(orders.action));
                        memset(&orders, 0, sizeof(orders));
                        actor_find_pathfinding_location(second_leader);
                        orders.to_point(&leader->pathfinding_point, &leader->pathfinding_sector);
                        result = actor_move(actor_index, &orders, 0);
                    } else {
                        result = false;
                    }
                }
            } else {
                actor_datum *first_leader_actor = (actor_datum *)datum_try_and_get(tls_actor_data(), first_leader);
                actor_move_halt(actor_index);
                if (first_leader_actor == 0)
                    result = false;
            }
        } else {
            result = false;
        }
    } else {
        result = false;
    }
    return result;
}

// @address 0x575428 ?behavior_scared_by_leader_activate@@YAJJPAVc_behavior_state@@@Z
long behavior_scared_by_leader_activate(long actor_index, c_behavior_state *behavior_state)
{
    long result = -1;
    actor_datum *actor = &((actor_datum *)tls_actor_data()->data)[(unsigned short)actor_index];
    if (actor->type == 2) {
        long clump_index = actor->clump_index;
        if (clump_index != -1) {
            long nearest_index = -1;
            float best_distance = 3.402823466e+38f;
            clump_datum *clump = datum_get<clump_datum>(tls_clump_data(), clump_index);
            clump_actor_iterator iterator;
            clump_actor_iterator_new(&iterator, clump);
            actor_datum *candidate;
            while ((candidate = clump_actor_iterator_next(&iterator)) != 0) {
                if (candidate == actor || candidate->type != 0)
                    continue;
                float distance = distance3d(&actor->position.body, &candidate->position.body);
                if (distance >= 1.0f || distance >= best_distance)
                    continue;
                best_distance = distance;
                nearest_index = iterator.actor_index;
            }
            if (nearest_index != -1) {
                result = joint_new(actor_index, 193, (c_joint_behavior_data *)behavior_state);
                if (result != -1) {
                    joint_state_datum *joint = datum_get<joint_state_datum>(tls_joint_state_data(), result);
                    joint->scared_actor_index = actor_index;
                    joint->leader_actor_index = nearest_index;
                    // match: 0x575428: reading the game time slot directly here (not through tls_game_time_globals()) keeps the
                    // distance3d fmuls/fmadds operand tie in the loop above (one more inline expansion shifts c2's rank keys)
                    clump->last_scared_time = TLS_GAME_TIME_GLOBALS->time;
                }
            }
        }
    } else if (actor->type == 0) {
        joint_invitation_iterator iterator;
        iterator.actor_index = actor_index;
        iterator.index = (short)result;
        iterator.type = 193;
        iterator.done = false;
        invitation_data *invitation = joint_invitation_iterator_next(&iterator);
        if (invitation != 0) {
            do {
                result = joint_accept(actor_index, invitation, (c_joint_behavior_data *)behavior_state);
                if (result != -1)
                    break;
                invitation = joint_invitation_iterator_next(&iterator);
            } while (invitation != 0);
        }
    }
    return result;
}

// @address 0x42d298 ?behavior_scared_by_leader_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
short behavior_scared_by_leader_get_desire(long, const s_behavior_input *, const s_behavior_arguments *)
{
    return 0;
}

// @address 0x7c5c90 ?scared_by_leader_behavior@@3VJoint_primitive_behavior@@A
Joint_primitive_behavior scared_by_leader_behavior = Joint_primitive_behavior(193, 0x3ff00402,
    behavior_scared_by_leader_get_desire, behavior_scared_by_leader_next, behavior_scared_by_leader_activate,
    behavior_scared_by_leader_deactivate, -1, behavior_scared_by_leader_perform,
    behavior_scared_by_leader_control, _joint_type_invitation, behavior_scared_by_leader_select_actors, 2, 2,
    1.0f, behavior_scared_by_leader_flush_actor, 0, 0, 0, 0, 0, 0);
