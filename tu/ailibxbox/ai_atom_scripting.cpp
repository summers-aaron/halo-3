// ailibxbox/ai_atom_scripting.obj: from the records 0x512328 0x5123b8 0x512530 0x512650 0x512710 0x5127e8 0x512878 0x5129a0 0x512a70 0x512ba8 0x512ce0 0x512e60 0x512ee0 0x512f58 0x513008 0x513080 0x513100 0x513170 0x5131f0 0x513290 0x513330 0x513430 0x513508 0x5135b8 0x513820 0x513890 0x513968 0x513a30
// @flags /TP /O2
#include "units/action_request.h"
#include "math/matrix_math_types.h"
#include "game/game_time_types.h"
#include "game/player_types.h"
#include "objects/object_placement.h"
#include "units/unit_types.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_datum.h"
#include <math.h>
#include <string.h>
#include "memory/data_types.h"
#include "ai/actor_datum.h"
#include "ai/actor_moving.h"
#include "ai/ai_script.h"
#include "ai/cl_engine.h"
#include "motor/action_system.h"
#include "objects/object_scripting.h"
#include "objects/objects.h"
#include "objects/object_reference_types.h"
#include "math/real_math_types.h"
#include "ai/ai_point3d.h"
#include "ai/activities.h"
#include "ai/ai_atom_scripting.h"
#include "ai/ai_atoms.h"
#include "memory/data.h"
#include "sound/game_sound_definitions.h"
#include "sound/game_sound.h"
#include "ai/squad_iterator_types.h"
#include "memory/tls_globals.h"
#include "ai/command_script_data.h"
#include "objects/object_header.h"
#include "math/real_math.h"
#include "ai/ai_reference_frame.h"
#include "ai/cl_engine_commands.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "scenario/scenario_access.h"
#include "ai/cs_scenario_definitions.h"
#include "ai/actor_prop_ref_iterator.h"
#include "ai/behavior_posture_info_types.h"
#include "motor/mover_inline.h"
// @address 0x5123b8 ?cs_find_player_unit@@YAJJ@Z
// @address 0x512650 ?cs_approach@@YAXJJ_NJMMM@Z
// @address 0x512ce0 ?cs_start_custom_animation_internal@@YAXJJ_N0JJ0J0@Z
// @address 0x512878 ?cs_custom_animation@@YAXJJ_NPBVc_ai_direction@@F@Z
// @address 0x512a70 cs_custom_animation_player
// @address 0x512e60 ?cs_look_player@@YAXJJ_N@Z
// @address 0x512ee0 ?cs_aim@@YAXJJ_NJ@Z
// @address 0x512f58 ?cs_aim_player@@YAXJJ_N@Z
// @address 0x513008 ?cs_aim_object@@YAXJJ_NJ@Z
// @address 0x513100 ?cs_face_object@@YAXJJ_NJ@Z
// @address 0x513170 ?cs_face_player@@YAXJJ_N@Z
// @address 0x5131f0 ?cs_shoot_at_target2@@YAXJJ_NJ@Z
// @address 0x513290 ?cs_shoot_at_point@@YAXJJ_NJ@Z
// @address 0x513330 ?cs_vehicle_speed_instantaneous@@YAXJJM@Z
// @address 0x513508 ?cs_posture_exit@@YAXJJ@Z
// @address 0x5135b8 ?cs_teleport@@YAXJJJJ@Z
// @address 0x513820 ?cs_stop_custom_animation@@YAXJJ@Z
// @address 0x513890 ?cs_melee_direction@@YAXJJ_NJ@Z
// @address 0x513968 ?cs_player_melee@@YAXJJJ@Z
// @address 0x513a30 ?cs_smash_direction@@YAXJJ_NJ@Z


namespace {
static __forceinline void cs_look(long actor_index, long script_index, bool aiming, long target_index)
{
  if (script_index != -1) {
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    script->look_at_target = aiming;
    if (aiming) {
      script->look_target.type = 2;
      script->look_target.index = target_index;
    }
  }
}
static __forceinline void cs_look_object(long actor_index, long script_index, bool aiming, long target_index)
{
  if (script_index != -1) {
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    script->look_at_target = aiming;
    if (aiming) {
      script->look_target.type = 1;
      script->look_target.index = target_index;
    }
  }
}
}


void cs_approach(long actor_index, long script_index, bool active, long target_index,
                 float distance, float stop_distance, float max_distance)
{
    if (script_index != -1)
    {
        command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
        cs_initialize_command(script_index, 21, active);
        cs_approach_reset(script_index);
        script->approach_pending = true;
        script->approach_object_index = target_index;
        script->approach_distance_sq = distance * distance;
        script->approach_max_distance_sq = stop_distance * stop_distance;
        script->approach_follow_distance_sq = max_distance * max_distance;
        if (target_index != -1)
            cs_face_object(actor_index, script_index, true, target_index);
    }
}

static __forceinline long ai_atom_scripting_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)tls_game_time_globals()->ticks_per_second + 0.5f);
}


// @address 0x5127e8 cs_pause
void cs_pause(long actor_index, bool active, float seconds)
{
    cs_command *command = cs_initialize_command(actor_index, 0, active);
    command->parameter = (float)ai_atom_scripting_seconds_to_ticks_round(seconds);
}


// @rdata 0x82024008 alias
static const long k_cs_custom_animation_impulses[21] = {
    52, 45, 46, 54, 55, 17, 18, 19, 20, 782, 776, 783, 780, 779, 99, 796, 59, 58, 777, 60, 781
};


static __forceinline void ai_atom_scripting_action_clear(c_ai_action *action)
{
    action->type = 0;
    action->point_valid = false;
    action->b5 = false;
    action->ticks = 0;
    action->impulse_index = 0;
    action->point.set(global_origin3d);
    action->direction.clear();
}

static __forceinline void ai_atom_scripting_action_setup_animation(c_ai_action *action, const c_ai_direction *direction, long animation)
{
    action->type = 5;
    action->point_valid = false;
    if (direction)
        action->direction = *direction;
    else
        action->direction.clear();
    action->impulse_index = animation;
}

void cs_custom_animation(long actor_index, long command_index, bool active, const c_ai_direction *direction, short animation_index)
{
    if (actor_index != -1 && direction->type != -1)
    {
        if (animation_index >= 0 && animation_index < 21)
        {
            long animation = k_cs_custom_animation_impulses[animation_index];
            actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
            ai_atom_scripting_action_clear(&actor->action);
            ai_atom_scripting_action_setup_animation(&actor->action, direction, animation);
            cs_initialize_command(command_index, 13, active);
        }
    }
}


static __forceinline void ai_atom_scripting_action_request_clear(action_request *request)
{
    memset(request, 0, sizeof(*request));
}

// 0x512e60 cs_look_player
void cs_look_player(long actor_index, long script_index, bool looking)
{
    if (script_index != -1 && actor_index != -1)
    {
        command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
        if (looking)
        {
            long player_unit = cs_find_player_unit(actor_index);
            if (player_unit != -1)
            {
                script->look_at_target = 1;
                script->look_target.type = 1;
                script->look_target.index = player_unit;
            }
            else
            {
                script->look_at_target = 0;
            }
        }
        else
        {
            script->look_at_target = 0;
        }
    }
}

void cs_aim(long actor_index, long script_index, bool aiming, long target_index)
{
  if (script_index != -1) {
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    script->aim_at_target = aiming;
    if (aiming) {
      script->aim_target.type = 2;
      script->aim_target.index = target_index;
      script->face_at_target = 0;
      script->face_exactly = 0;
      script->face_lower_weapon = 0;
    }
    cs_look(actor_index, script_index, aiming, target_index);
  }
}

// 0x512f58 cs_aim_player
void cs_aim_player(long actor_index, long script_index, bool aiming)
{
    if (script_index != -1 && actor_index != -1)
    {
        command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
        if (aiming)
        {
            long player_unit = cs_find_player_unit(actor_index);
            if (player_unit != -1)
            {
                script->face_at_target = 0;
                script->face_exactly = 0;
                script->aim_target.index = player_unit;
                script->aim_at_target = 1;
                script->aim_target.type = 1;
                cs_look_object(actor_index, script_index, true, player_unit);
                script->face_lower_weapon = 0;
            }
            else
            {
                script->aim_at_target = 0;
                cs_look_object(actor_index, script_index, false, -1);
            }
        }
        else
        {
            script->aim_at_target = 0;
            cs_look_object(actor_index, script_index, false, -1);
        }
    }
}

void cs_aim_object(long actor_index, long script_index, bool aiming, long target_index)
{
  if (script_index != -1) {
    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    script->aim_at_target = aiming;
    if (aiming) {
      script->look_at_target = 0;
      script->aim_target.type = 1;
      script->aim_target.index = target_index;
      script->face_at_target = 0;
      script->face_exactly = 0;
      script->face_lower_weapon = 0;
    }
    cs_look_object(actor_index, script_index, aiming, target_index);
  }
}

// @address 0x513080 cs_face_point
void cs_face_point(long actor_index, long script_index, bool facing, long target_index)
{
    if (script_index != -1)
    {
        command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
        cs_aim(actor_index, script_index, facing, target_index);
        if (facing)
        {
            script->face_exactly = 1;
            script->aim_at_target = 0;
            script->look_at_target = 0;
            script->face_lower_weapon = 1;
            script->face_at_target = 1;
        }
        else
        {
            script->face_at_target = 0;
        }
    }
}

void cs_face_object(long actor_index, long script_index, bool facing, long target_index)
{
    if (script_index != -1)
    {
        command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
        cs_aim_object(actor_index, script_index, facing, target_index);
        bool face_body;
        if (facing)
        {
            face_body = 1;
            script->face_exactly = 1;
            script->face_lower_weapon = 1;
        }
        else
        {
            face_body = 0;
        }
        script->face_at_target = face_body;
    }
}

// 0x513170 cs_face_player
void cs_face_player(long actor_index, long script_index, bool facing)
{
    if (script_index != -1)
    {
        command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
        cs_aim_player(actor_index, script_index, facing);
        bool face_body;
        if (facing)
        {
            face_body = 1;
            script->face_exactly = 1;
            script->face_lower_weapon = 1;
        }
        else
        {
            face_body = 0;
        }
        script->face_at_target = face_body;
    }
}

void cs_shoot_at_target2(long actor_index, long script_index, bool shooting, long target_index)
{
  command_script_datum *script;
  actor_datum *actor;

  if (script_index != 0xffffffff) {
    script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    script->shoot_at_target = shooting;
    if (shooting != 0 && target_index != -1) {
      actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
      script->aim_at_target = 1;
      script->aim_target.type = 1;
      script->aim_target.index = target_index;
      script->look_at_target = 0;
      if (actor->combat_status < 3) {
        (datum_get<command_script_datum>(tls_command_script_data(), script_index))->forced_combat_status = 3;
      }
    }
  }
}

void cs_shoot_at_point(long actor_index, long script_index, bool shooting, long target_index)
{
  command_script_datum *script;
  actor_datum *actor;

  if (script_index != 0xffffffff) {
    script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
    script->shoot_at_target = shooting;
    if (shooting != 0 && target_index != -1) {
      actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
      script->aim_at_target = 1;
      script->aim_target.index = target_index;
      script->aim_target.type = 2;
      script->look_at_target = 0;
      if (actor->combat_status < 3) {
        (datum_get<command_script_datum>(tls_command_script_data(), script_index))->forced_combat_status = 3;
      }
    }
  }
}


static __forceinline void unit_get_head_position(long unit, real_point3d *out)
{
    object_marker marker;
    internal_object_get_markers_by_string_id(unit, 157, &marker, 1, false);
    *out = marker.matrix.position;
}

// @address 0x5129a0 cs_custom_animation_point
extern "C" void cs_custom_animation_point(long actor_index, long command_index, bool active, long point_ref, short animation_index)
{
    if (actor_index != -1 && command_index != -1 && valid_point_ref(point_ref))
    {
        const cs_point *point = TAG_BLOCK_GET_ELEMENT(&cs_get_point_set((unsigned short)(point_ref >> 16))->points,
                                                      (unsigned short)point_ref, cs_point);
        c_ai_direction direction = {};
        direction.type = 4;
        memcpy(&direction.m_point, &point->position, sizeof(direction.m_point));
        cs_custom_animation(actor_index, command_index, active, &direction, animation_index);
    }
}


void cs_custom_animation_player(long actor_index, long command_index, bool active, short animation_index)
{
    if (actor_index != -1 && command_index != -1)
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        float closest_distance = 3.4028234663852886e38f;
        long closest_unit = -1;
        {
            c_data_iterator<player_datum> players;
            players.begin(tls_player_data());
            if (players.next())
            {
                do
                {
                    player_datum *p = players.get_datum();
                    if (p->unit_index != -1)
                    {
                        real_point3d origin;
                        object_get_origin(p->unit_index, &origin);
                        real_vector3d offset;
                        vector_from_points3d(&actor->position.body, &origin, &offset);
                        // match: 0x5123b8: sums j, then i + k
                        float d = offset.j * offset.j + (offset.i * offset.i + offset.k * offset.k);
                        if (!(d >= closest_distance))
                        {
                            closest_distance = d;
                            closest_unit = p->unit_index;
                        }
                    }
                } while (players.next());
            }
        }
        if (closest_unit != -1)
        {
            c_ai_direction direction = {};
            direction.object_index = closest_unit;
            direction.type = 7;
            cs_custom_animation(actor_index, command_index, active, &direction, animation_index);
        }
    }
}

// 0x5123b8 cs_find_player_unit
long cs_find_player_unit(long actor_index)
{
    float closest_distance = 3.4028234663852886e38f;
    long result = -1;
    actor_prop_ref_iterator refs;
    actor_prop_ref_iterator_new(&refs, actor_index);
    prop_ref_datum *ref;
    while ((ref = actor_prop_ref_iterator_next(&refs)) != 0)
    {
        if (ref->knowledge_state >= 1 &&
            (datum_get<prop_datum>(tls_ai_prop_data(), ref->prop_index))->player &&
            ref->distance < closest_distance)
        {
            result = ref->object_index;
            closest_distance = ref->distance;
        }
    }
    if (result == -1)
    {
        actor_datum *a = datum_get<actor_datum>(tls_actor_data(), actor_index);
        float distance = 3.4028234663852886e38f;
        c_data_iterator<player_datum> players;
        players.begin(tls_player_data());
        while (players.next())
        {
            player_datum *p = players.get_datum();
            if (p->unit_index != -1)
            {
                real_point3d head;
                unit_get_head_position(p->unit_index, &head);
                real_vector3d offset;
                vector_from_points3d(&head, &a->position.head, &offset);
                // match: 0x5129a0: sums j, then i + k
                float d = offset.j * offset.j + (offset.i * offset.i + offset.k * offset.k);
                if (d < distance)
                {
                    result = p->unit_index;
                    distance = d;
                }
            }
        }
    }
    return result;
}

// @address 0x512530 cs_go_to_nearest


// @address 0x512328 cs_running_atom_internal
bool cs_running_atom_internal(long ai_index, short atom_mask)
{
    bool running = false;
    ai_index_actor_iterator iterator;
    ai_index_actor_iterator_new(ai_index, &iterator);
    actor_datum *actor;
    while ((actor = ai_index_actor_iterator_next(&iterator)) != 0)
    {
        if (actor->command_script_index != -1)
        {
            if (::cs_atom_executing(iterator.actor_index, actor->command_script_index, atom_mask))
            {
                running = true;
                break;
            }
        }
    }
    return running;
}

void cs_go_to_nearest(long actor_index, long command_index, bool active, long point_set_id)
{
    if (actor_index != -1) {
        scenario *s = global_scenario_get();
        if (s->script_data.count > 0) {
            actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
            short set_index = (short)(point_set_id >> 16);
            if (set_index >= 0 && set_index < cs_scenario_get_script_data(s)->point_sets.count) {
                cs_point_set *set = cs_get_point_set(set_index);
                float best_distance = 3.4028234663852886e38f;
                short nearest = -1;
                int count = set->points.count;
                short i = 0;
                if (count > 0) {
                    cs_point *points = (cs_point *)set->points.address;
                    do {
                    float distance = ::ai_distance_squared3d(&points[i].position, &actor->position.body);
                    if (distance < best_distance) {
                        best_distance = distance;
                        nearest = i;
                    }
                    ++i;
                    } while (i < count);
                }
                if (nearest != -1 && command_index != -1) {
                    cs_command *command = cs_initialize_command(command_index, 1, active);
                    command->parameter = 0.0f;
                    command->point_ref = (set_index << 16) | (unsigned short)nearest;
                }
            }
        }
    }
}


// 0x513330 cs_vehicle_speed_instantaneous
void cs_vehicle_speed_instantaneous(long actor_index, long script_index, float speed)
{
    if (actor_index != -1 && script_index != -1)
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
        if (actor->driver)
        {
            float direction = 0.0f;
            object_scripting_set_velocity2(actor->root_vehicle_index, speed, 0.0f, 0.0f);
            script->vehicle_speed = speed;
            script->vehicle_speed_valid = 1;
            if ((float)fabs(speed) >= 1.5f)
                direction = speed > 0.0f ? 1.0 : -1.0;
            if (actor->vehicle_driver_type != 3)
            {
                if (actor->vehicle_driver_type != 5)
                    return;
                actor->vehicle_throttle.k = direction;
                return;
            }
            actor->vehicle_speed = direction;
        }
    }
}

// 0x513508 cs_posture_exit
void cs_posture_exit(long actor_index, long script_index)
{
    if (script_index != -1 && actor_index != -1)
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        long unit_index = actor->unit_index;
        if (unit_index != -1 && mover_get_motor_program(unit_index) == 9)
        {
            command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
            ::actor_exit_posture(actor_index);
            script->script_initiated_posture = false;
        }
    }
}


// 0x5135b8 cs_teleport
void cs_teleport(long actor_index, long script_index, long from_point_ref, long facing_point_ref)
{
    if (actor_index != -1 && valid_point_ref(from_point_ref) && valid_point_ref(facing_point_ref))
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        long unit_index = actor->unit_index;
        if (unit_index != -1)
        {
            cs_point_set *point_sets = (cs_point_set *)cs_scenario_get_script_data(global_scenario_get())->point_sets.address;
            cs_point *from_point = TAG_BLOCK_GET_ELEMENT(
                &point_sets[point_reference_get_point_set_index(from_point_ref)].points, point_reference_get_point_index(from_point_ref), cs_point);
            real_point3d from;
            ai_point_get_position(&from_point->position, &from);
            cs_point *facing_point = TAG_BLOCK_GET_ELEMENT(
                &point_sets[point_reference_get_point_set_index(facing_point_ref)].points, point_reference_get_point_index(facing_point_ref), cs_point);
            union
            {
                real_point3d to;
                real_vector3d up;
            };
            ai_point_get_position(&facing_point->position, &to);
            real_vector3d forward;
            forward.i = to.x - from.x;
            forward.j = to.y - from.y;
            forward.k = to.z - from.z;
            if (actor->use_object)
            {
                if (normalize3d(&forward) == 0.0f)
                    ::object_get_orientation(unit_index, &forward, 0);
                generate_up_vector3d(&forward, &up);
            }
            else
            {
                forward.k = 0.0f;
                if (normalize2d(&forward.xy) == 0.0f)
                {
                    ::object_get_orientation(unit_index, &forward, 0);
                    forward.k = 0.0f;
                    if (normalize2d(&forward.xy) == 0.0f)
                        forward = *global_forward3d;
                }
                up = *global_up3d;
            }
            long root_index = -1;
            long parent_index = actor->unit_index;
            if (parent_index != -1)
            {
                do
                {
                    root_index = parent_index;
                    parent_index = datum_get<s_object_header>(tls_object_header_data(), parent_index)->datum->parent;
                } while (parent_index != -1);
            }
            object_set_position_direct(root_index, &from, &forward, &up, 0, false);
            object_reset_after_teleport(root_index);
            actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
            actor->path.valid = 0;
            actor->path.obstacle_object_index = -1;
            actor->path.result_value = 0;
            actor->path_result_time = 0;
            actor->path_result_count = 0;
            actor->path_state = 0;
            actor->destination_orders.clear();
        }
    }
}

// 0x513820 cs_stop_custom_animation
void cs_stop_custom_animation(long actor_index, long script_index)
{
    if (script_index != -1 && actor_index != -1)
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        long unit_index = actor->unit_index;
        if (unit_index != -1)
        {
            if (mover_get_motor_program(unit_index) == 11)
            {
                action_submit_event(unit_index, 2);
            }
        }
    }
}

// 0x513890 cs_melee_direction
void cs_melee_direction(long actor_index, long script_index, bool blocking, long direction)
{
    if (script_index != -1 && actor_index != -1)
    {
        if (direction >= 0 && direction < 6)
        {
            cs_initialize_command(script_index, 25, blocking);
            actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
            if (actor->unit_index != -1)
            {
                action_request request;
                ai_atom_scripting_action_request_clear(&request);
                memset(&request, 0, sizeof(request));
                request.action = 43;
                request.melee.direction = (short)direction;
                action_submit(actor->unit_index, &request);
            }
        }
    }
}

void cs_player_melee(long actor_index, long script_index, long type)
{
    if (actor_index != -1)
    {
        if (type > 0 && type < 4)
        {
            actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
            if (actor->unit_index != -1)
            {
                action_request request;
                ai_atom_scripting_action_request_clear(&request);
                memset(&request, 0, sizeof(request));
                request.action = 60;
                request.melee.direction = (short)((type > 0 ? type : 0) < 3 ? (type > 0 ? type : 0) : 3);
                request.melee.player_target = -1;
                action_submit(actor->unit_index, &request);
            }
        }
    }
}

// 0x513a30 cs_smash_direction
void cs_smash_direction(long actor_index, long script_index, bool blocking, long direction)
{
    if (script_index != -1 && actor_index != -1)
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        if (actor->unit_index != -1)
        {
            cs_initialize_command(script_index, 26, blocking);
            action_request request;
            ai_atom_scripting_action_request_clear(&request);
            memset(&request, 0, sizeof(request));
            request.action = 39;
            request.melee.direction = (short)((direction > 0 ? direction : 0) < 2 ? (direction > 0 ? direction : 0) : 2);
            request.melee.target = -1;
            request.melee.flags = 1;
            action_submit(actor->unit_index, &request);
        }
    }
}

// @address 0x512710 ?cs_move_in_direction@@YAXJ_NMMM@Z
void cs_move_in_direction(long script_index, bool blocking, float angle, float parameter, float direction)
{
    cs_command *command = cs_initialize_command(script_index, 5, blocking);
    command->parameter = parameter;
    command->angle = angle;
    command->object_index = -1;
    if (!(fabs(direction) > 45.0f))
        command->index = 0;
    else if (direction > 0.0f && direction < 135.0f)
        command->index = 1;
    else if (direction < 0.0f && direction > -135.0f)
        command->index = 2;
    else if (fabs(direction) >= 135.0f)
        command->index = 3;
}


// 0x512ce0 cs_start_custom_animation_internal
void cs_start_custom_animation_internal(long actor_index, long script_index, bool blocking, bool no_command,
    long animation, long variant, bool interpolate, long point_ref, bool b30)
{
    if (script_index != -1 && actor_index != -1)
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        if (actor->unit_index != -1)
        {
            action_request request;
            ai_atom_scripting_action_request_clear(&request);
            request.action = 1;
            request.custom_animation.animation = animation;
            request.custom_animation.variant = variant;
            request.custom_animation.interpolate = interpolate;
            request.custom_animation.target = -1;
            request.custom_animation.no_command = no_command;
            request.custom_animation.b30 = b30;
            request.custom_animation.has_facing = false;
            request.custom_animation.b0d = true;
            if (valid_point_ref(point_ref))
            {
                cs_point *point = TAG_BLOCK_GET_ELEMENT(&cs_get_point_set((unsigned short)(point_ref >> 16))->points, (unsigned short)point_ref, cs_point);
                ai_point_get_position(&point->position, &request.custom_animation.position);
                vector3d_from_euler_angles2d(&request.custom_animation.facing, &point->facing);
                request.custom_animation.facing.k = 0.0f;
                if (normalize3d(&request.custom_animation.facing) > 0.0f)
                    request.custom_animation.has_facing = true;
            }
            if (action_submit(actor->unit_index, &request))
            {
                command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
                if (!no_command)
                    cs_initialize_command(script_index, 23, blocking);
                script->script_initiated_custom_animation = true;
            }
        }
    }
}


// @address 0x35f08 k_scripted_sound_default_decibels

static __forceinline void ai_atom_scripting_actor_suppress_shooting(long actor_index, short ticks)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (actor->shooting_suppression_ticks < ticks)
        actor->shooting_suppression_ticks = ticks;
}

// @address 0x512ba8 ?ai_scripting_play_sound_internal@@YAMJJ_NJJMM@Z
float ai_scripting_play_sound_internal(
    long actor_index, long script_index, bool blocking, long sound_index, long playback_label, float duration_scale, float sound_scale)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    float duration;
    {
        float playing_time = 0.0f;
        scripted_sound_new_internal(sound_index, actor->unit_index, sound_scale,
            k_scripted_sound_default_decibels, k_scripted_sound_default_decibels, playback_label, &playing_time);
        duration = playing_time * duration_scale;
    }
    long ticks = (long)((float)tls_game_time_globals()->ticks_per_second * duration + 0.5f);
    if (script_index != -1)
    {
        cs_command *command = cs_initialize_command(script_index, 24, blocking);
        command->parameter = (float)ticks;
    }
    else
    {
        ai_atom_scripting_actor_suppress_shooting(actor_index, (short)ticks);
    }
    return duration;
}


// @address 0x513430 ?cs_posture_set@@YAXJJJ_N@Z
void cs_posture_set(long actor_index, long script_index, long posture, bool enter)
{
    if (script_index != -1 && actor_index != -1)
    {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        if (actor->unit_index != -1)
        {
            behavior_posture_info *info = activities_try_and_get_posture(posture);
            if (info)
            {
                action_request request;
                ai_atom_scripting_action_request_clear(&request);
                request.posture.has_position = false;
                request.posture.posture = posture;
                request.posture.a = enter;
                request.action = 11;
                request.posture.b = info->locked;
                if (action_submit(actor->unit_index, &request))
                {
                    command_script_datum *script = datum_get<command_script_datum>(tls_command_script_data(), script_index);
                    script->script_initiated_posture = true;
                }
            }
        }
    }
}
