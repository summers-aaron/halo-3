// ailibxbox/actor_moving_objects.obj: from the records 0x567ec8 0x567f30 0x568098
// @flags /TP /O2 /Oc
#include "units/action_request.h"
#include "memory/tls_globals.h"
#include "memory/data.h"
#include "memory/data_inline.h"
#include "game/game_time_types.h"
#include "objects/object_placement.h"
// @address 0x567f30 ?ignored_obstacle_register@@YAXJJF_N@Z
#include <string.h>
#include "math/real_math.h"
#include "math/matrix_math.h"
#include "motor/action_system.h"
#include "ai/actor_moving.h"
#include "ai/ai_hint_definitions.h"
#include "units/unit_animation.h"
#include "ai/actor_moving_objects.h"
#include "ai/character_definitions.h"
#include "ai/recent_obstacle.h"
#include "game/game_time_inline.h"
#include "ai/special_movement_types.h"


extern "C" {
}

#include "ai/actor_datum.h"
#include "ai/character_properties_types.h"

// March 82f84cf8. Keep the time-helper dependency in the owning unit.
__forceinline void c_recent_obstacle::set(long index, float seconds, bool success)
{
    clear();
    object_index = index;
    ticks = (short)game_seconds_to_ticks_round(seconds);
    flags.set(_recent_obstacle_successful, success);
    flags.set(_recent_obstacle_valid, true);
}



c_recent_obstacle *actor_find_recent_obstacle(long actor_index, long object_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    return c_recent_obstacle::find_recent_obstacle(actor->recent_obstacles, 3, object_index);
}

void ignored_obstacle_register(long actor_index, long object_index, short unused, bool success)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    short selected_index = -1;
    short shortest_ticks = 32767;
    for (short slot_index = 0; slot_index < 3; ++slot_index) {
        c_recent_obstacle *obstacle = &actor->recent_obstacles[slot_index];
        if (obstacle->object_index == object_index) {
            selected_index = slot_index;
            break;
        } else if (!obstacle->valid() && shortest_ticks >= 0) {
            selected_index = slot_index;
            shortest_ticks = -1;
        } else if (obstacle->ticks < shortest_ticks) {
            selected_index = slot_index;
            shortest_ticks = obstacle->ticks;
        }
    }
    if (selected_index != -1) {
        c_recent_obstacle *obstacle = &actor->recent_obstacles[selected_index];
        float seconds = success ? 3.0f : 5.0f;
        obstacle->set(object_index, seconds, success);
    }
}




static inline void actor_moving_objects_action_request_clear(action_request *request)
{
    memset((__int64 *)request, 0, sizeof(*request));
}

static __forceinline void actor_moving_objects_clear_obstacle(actor_datum *actor)
{
    actor->path.obstacle_object_index = -1;
    actor->path.result_value = 0;
}


// @address 0x568098 ?actor_moving_handle_object_hint@@YA_NJJPBFPATreal_vector3d@@1@Z
bool actor_moving_handle_object_hint(long actor_index, long object_index, const short *hint_flags, real_vector3d *avoidance_vector, real_vector3d *throttle)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
    bool handled = false;
    bool can_jump = false;
    float minimum_alignment = 0.5f;
    short hint_type = -1;
    short type = -1;
    if (flag_test((unsigned long)*hint_flags, 3L))
        type = 3;
    else if (flag_test((unsigned long)*hint_flags, 5L))
        type = 5;
    switch (type) {
    case 3:
        if (movement->hoist_flags & 0x3) {
            minimum_alignment = 0.95f;
            hint_type = 3;
            can_jump = true;
        }
        break;
    case 5:
        if (movement->hoist_flags & 0x1800) {
            minimum_alignment = 0.8660254f;
            hint_type = 5;
        }
        break;
    }
    if (hint_type != -1) {
        float radius = movement->pathfinding_radius;
        bool failed = false;
        unsigned char found = false;
        object_marker marker;
        real_point3d point;
        switch (hint_type) {
        case 3:
            found = ai_hint_find_vault_marker(actor_index, object_index, radius, &marker, &point);
            break;
        case 4:
            found = ai_hint_find_mount_marker(actor_index, object_index, radius, &marker, &point);
            break;
        case 5:
            found = ai_hint_find_hoist_marker(actor_index, object_index, radius, &marker, &point);
            break;
        }
        if (found) {
            long animation = -1;
            long fallback_animation = -1;
            bool has_animation = false;
            real_matrix4x3 root_matrix;
            switch (hint_type) {
            case 3:
                animation = 0x117;
                break;
            case 5:
                switch (((long)::ai_hint_get_best_hoist_height(actor->unit_index, point.z - actor->position.body.z))) {
                case 1:
                    animation = 0x1cd;
                    fallback_animation = 0x1cc;
                    break;
                case 2:
                    animation = 0x1ce;
                    fallback_animation = 0x1cc;
                    break;
                default:
                    animation = 0x1cc;
                    break;
                }
                break;
            }
            if (unit_animation_build_root_matrix_by_seat_and_state(actor->unit_index, animation, 84, false, 3, &root_matrix))
                has_animation = true;
            else if (unit_animation_build_root_matrix_by_seat_and_state(actor->unit_index, fallback_animation, 84, false, 3, &root_matrix))
                has_animation = true;
            if (has_animation) {
                real_vector3d direction;
                vector_from_points3d(&actor->position.body, &point, &direction);
                direction.k = 0.0f;
                float distance = normalize3d(&direction);
                if (distance > 0.0f && distance < root_matrix.position.x * -1.3f &&
                    dot_product3d(&actor->facing_vector, &direction) > 0.0f) {
                    if (fabsf(dot_product3d(&marker.matrix.left, &direction)) >= minimum_alignment) {
                        if (dot_product3d(&actor->facing_vector, &direction) > 0.99f) {
                            action_request request;
                            actor_moving_objects_action_request_clear(&request);
                            bool submitted = false;
                            switch (hint_type) {
                            case 3: {
                                special_movement special;
                                actor_special_movement_new(actor_index, &special);
                                if (ai_hint_vault_range_valid(&actor->position.body, &point, &special)) {
                                    request.action = 40;
                                    submitted = true;
                                    request.hint.object_index = object_index;
                                    request.hint.hint = true;
                                    request.hint.direction = direction;
                                    request.hint.point = point;
                                }
                                break;
                            }
                            case 5:
                                request.action = 41;
                                submitted = true;
                                request.hint.object_index = object_index;
                                request.hint.hint = true;
                                request.hint.direction = direction;
                                request.hint.point = point;
                                break;
                            }
                            if (submitted) {
                                submitted = action_submit(actor->unit_index, &request);
                                if (submitted)
                                    actor_moving_objects_clear_obstacle(actor);
                            }
                            ignored_obstacle_register(actor_index, object_index, hint_type, submitted);
                            if (!submitted && can_jump && actor_moving_jump(actor_index, &point, true))
                                actor_moving_objects_clear_obstacle(actor);
                            if (!submitted)
                                failed = true;
                        } else {
                            *avoidance_vector = direction;
                            *throttle = *global_zero_vector3d;
                            actor->flags &= ~1;
                            actor->flags = (actor->flags & ~0x600) | 0x400;
                        }
                        handled = true;
                    } else {
                        failed = true;
                    }
                }
            } else {
                failed = true;
            }
        } else {
            failed = true;
        }
        if (failed) {
            *throttle = *global_zero_vector3d;
            ignored_obstacle_register(actor_index, object_index, hint_type, false);
        }
    }
    return handled;
}
