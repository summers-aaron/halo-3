// ailibxbox/behavior_misc.obj: from the records 0xc2938 0x56fa20 0x56fa58 0x56fcf0 0x56fd38 0x56fe58 0x56fea8 0x56ff70 0x5701b8
// @flags /TP /O2 /Oc
#include "ai/behavior_info.h"
#include "ai/pathfinding_collision.h"
#include "ai/character_behavior_properties.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "ai/sector_ref.h"
#include "ai/ai_point3d.h"
#include "ai/actor_datum.h"
#include "ai/character_properties_types.h"
#include "ai/behavior.h"
#include "cseries/crt_xdk.h"
#include "cseries/cseries.h"
#include "tag_files/tag_groups.h"
#include "units/action_request.h"
// @address 0x56fa20 ?behavior_blind_panic_get_desire@@YAFJPBUs_behavior_input@@PBUs_behavior_arguments@@@Z
// @address 0x56fcf0 ?behavior_blind_panic_deactivate@@YAXJPAVc_behavior_state@@@Z
// @address 0x56fd38 ?behavior_blind_panic_next@@YAFJPAVc_behavior_state@@0@Z
// @address 0x56fe58 ?behavior_blind_panic_control@@YAXJPAVc_behavior_state@@@Z
// @address 0x5701b8 ?behavior_try_to_throw_grenade@@YA_NJ@Z

#include <math.h>
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "units/units.h"
#include "ai/actor_combat.h"
#include "ai/ai_reference_frame.h"
#include "ai/sector.h"
#include "ai/actor_moving_objects.h"
#include "ai/behavior_misc.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "physics/character_physics_component.h"
#include "ai/pathfinding_geometry.h"
#include "math/matrix_math_types.h"
#include "memory/tls_globals.h"
#include "ai/pathfinding_data.h"


#include "units/biped_types.h"
#include "units/biped_definitions.h"
#include "memory/data.h"
#include "objects/object_header.h"
#include "memory/datum_index.h"

struct s_blind_panic_timer_state : public c_behavior_state
{
    short ticks;                     // 36
};
struct s_blind_panic_state : public c_behavior_state
{
    short ticks;                     // 36
    real_point3d panic_point;        // 40
};
struct s_blind_panic_activate_state : public c_behavior_state
{
    short ticks;                     // 36
    real_vector3d direction;         // 40
};

static __forceinline long behavior_misc_seconds_to_ticks_round(float seconds)
{
    float ticks = (float)(int)TLS_GAME_TIME_GLOBALS->ticks_per_second * seconds;
    return (long)(ticks + 0.5f);
}

short behavior_blind_panic_get_desire(long actor_index, const s_behavior_input *, const s_behavior_arguments *)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);

    if (actor->unit_index != -1)
        return 3;
    return 0;
}

void behavior_blind_panic_deactivate(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    unit_datum *unit = OBJECT_GET(actor->unit_index, unit_datum, 0xffffffff);

    unit->unit.unit_flags &= ~0x100000;
}

short behavior_blind_panic_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state)
{
    const float panic_interval = 0.5f;
    s_blind_panic_timer_state *timer = (s_blind_panic_timer_state *)state;
    short result = -2;

    --timer->ticks;
    if (timer->ticks <= 0)
        result = -1;
    else if (timer->ticks > behavior_misc_seconds_to_ticks_round(panic_interval))
    {
        actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
        if (actor->unit_index != -1)
        {
            s_object_header *headers = ((s_object_header *)object_header_data->data);
    unit_datum *unit = (unit_datum *)headers[datum_index_to_absolute_index(actor->unit_index)].datum;
            bool has_equipment = false;
            long child_index = unit->object.first_child_object_index;
            while (child_index != -1)
            {
                unit_datum *child = OBJECT_GET(child_index, unit_datum, 0xffffffff);
                if (child->object.object_type == 5)
                {
                    has_equipment = true;
                    break;
                }
                child_index = child->object.next_object_index;
            }
            if (!has_equipment)
                timer->ticks = (short)behavior_misc_seconds_to_ticks_round(panic_interval);
        }
    }
    return result;
}

void behavior_blind_panic_control(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    s_blind_panic_state *blind_panic_state = (s_blind_panic_state *)state;

    actor->control_point = blind_panic_state->panic_point;
    actor->control_type = 145;
    actor->blind_panic_control_active = true;
}

static __forceinline sector_link *behavior_misc_link_get(pathfinding_data *data, long index)
{
    return &((sector_link *)data->links.address)[index];
}
// @address 0x56fea8 ?smash_obstacle_test_direction@@YA_NJPBTreal_point3d@@PBTreal_vector3d@@Vc_sector_ref@@PAM@Z
bool smash_obstacle_test_direction(long actor_index, const real_point3d *point, const real_vector3d *direction, c_sector_ref sector, float *distance)
{
    path_collision_result collision;
    pathfinding_test_vector(point, sector, g_sector_none, direction, 1.5f, 0, &collision);
    if (collision.collision && collision.link_index != -1 && collision.link_index != 0xffff)
    {
        pathfinding_data *data = pathfinding_data_get(collision.sector.structure_bsp_index);
        sector_link *link = behavior_misc_link_get(data, collision.link_index);
        if (link->flags & 0x100)
        {
            collision.collision = false;
            collision.t = 1.5f;
        }
    }
    if (distance)
        *distance = collision.t;
    return !collision.collision;
}


static __forceinline long behavior_misc_unit_get_motor_program(long unit_index)
{
    unit_datum *unit = OBJECT_GET(unit_index, unit_datum, 0xffffffff);
    return *(long *)((char *)unit + unit->unit.motor_program_offset);
}

// @address 0x56ff70 ?behavior_plan_grenade_throw@@YA_NJPBVc_ai_point3d@@JF@Z
bool behavior_plan_grenade_throw(long actor_index, const c_ai_point3d *target, long target_unit, short flags)
{
    bool success = false;
    long target_object_index = -1;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool used_default_mode = false;
    bool *used_default_mode_reference = 0;
    if (!actor_has_special_equipment(actor_index))
    {
        long unit_index = actor->unit_index;
        if (unit_index != -1)
        {
            if (behavior_misc_unit_get_motor_program(unit_index) == 0 || behavior_misc_unit_get_motor_program(unit_index) == 7)
                used_default_mode_reference = &used_default_mode;
        }
        real_point3d target_position;
        ai_point_get_position(target, &target_position);
        if (motor_animation_validate_animation(unit_index, 226, 34, used_default_mode_reference))
        {
            real_vector3d direction;
            vector_from_points3d(&actor->position.body, &target_position, &direction);
            if (normalize3d(&direction) > 0.0f)
            {
                long forced = actor->flags & (1 << 22);
                if (forced || dot_product3d(&actor->facing_vector, &direction) >= 0.0f)
                {
                    character_grenade_properties *properties = actor_grenade_properties_get(actor_index);
                    if (properties && actor_combat_find_grenade_target(actor_index, &target_position, target_unit, &target_object_index))
                    {
                        bool valid = false;
                        if (forced)
                            valid = true;
                        else
                        {
                            short enemy_count, collateral_count;
                            if (actor_combat_check_collateral_damage(actor_index, properties->throw_velocity, properties->damage_radius,
                                    &target_position, &collateral_count, &enemy_count))
                                valid = enemy_count >= 1 || collateral_count >= properties->collateral_damage_limit;
                        }
                        if (valid &&
                            actor_combat_plan_grenade_trajectory(
                                actor_index, properties->trajectory_type, target, target_unit, target_object_index, flags))
                            success = true;
                    }
                }
            }
        }
    }
    return success;
}

// @address 0x5701b8 ?behavior_try_to_throw_grenade@@YA_NJ@Z
bool behavior_try_to_throw_grenade(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool result = false;
    // match: 0x5701b8: the object header table is read once and kept across the calls below.
    s_object_header *headers = ((s_object_header *)object_header_data->data);
    unit_datum *unit = (unit_datum *)headers[datum_index_to_absolute_index(actor->unit_index)].datum;
    if (!motor_animation_busy(actor->unit_index) && unit->object.current_body_damage == 0.0f) {
        bool should_throw = false;
        long actor_flags = actor->flags;
        if (actor_flags & (1 << 22))
            should_throw = true;
        else {
            real_point3d target_position;
            real_vector3d target_direction;
            ai_point_get_position(&actor->target, &target_position);
            vector_from_points3d(&actor->position.body, &target_position, &target_direction);
            unit_datum *mover_unit = (unit_datum *)headers[datum_index_to_absolute_index(actor->unit_index)].datum;
            long mover_type = *(long *)((char *)mover_unit + mover_unit->unit.motor_program_offset);
            bool variable_camera_up = false;
            if (mover_type == 7 || mover_type == 8)
                variable_camera_up = true;
            const real_vector3d *up = variable_camera_up ? &unit->object.up : &global_identity4x3->up;
            real_vector3d side;
            cross_product3d(up, &target_direction, &side);
            cross_product3d(&side, up, &target_direction);
            real_vector3d side2, facing;
            cross_product3d(up, &unit->unit.aiming_vector, &side2);
            cross_product3d(&side2, up, &facing);
            if (normalize3d(&facing) > 0.0f && normalize3d(&target_direction) > 0.0f) {
                if (dot_product3d(&facing, &target_direction) > 0.97f)
                    should_throw = true;
            }
        }
        if (should_throw) {
            short throw_flags = (actor_flags & (1 << 21)) != 0;
            if (actor_flags & (1 << 22))
                throw_flags |= 2;
            else
                throw_flags &= ~2;
            result = behavior_plan_grenade_throw(actor_index, &actor->target, actor->target_unit, throw_flags);
        }
    }
    return result;
}

static __forceinline c_character_physics_component::e_mode behavior_misc_actor_get_physics_mode(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    long unit_index = actor->unit_index;
    c_character_physics_component::e_mode mode = c_character_physics_component::_mode_none;
    if (unit_index != -1)
    {
        const s_object_header *header = object_header_get(unit_index);
        if (header->object_type == 0)
            mode = ((biped_datum *)header->datum)->biped.physics.get_mode();
    }
    return mode;
}

// @address 0x56fa58 ?behavior_blind_panic_activate@@YA_NJPAVc_behavior_state@@@Z
bool behavior_blind_panic_activate(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool result = false;
    if (actor->unit_index != -1)
    {
        s_blind_panic_activate_state *panic = (s_blind_panic_activate_state *)state;
        panic->ticks = (short)(TLS_GAME_TIME_GLOBALS->ticks_per_second * 10);
        panic->direction = actor->facing_vector;
        panic->direction.k = 0.0f;
        if (normalize3d(&panic->direction) > 0.0f)
        {
            if (object_pathfinding_immediate(actor->unit_index))
            {
                actor_find_pathfinding_location(actor_index);
                if (actor->pathfinding_sector.valid())
                {
                    bool found = false;
                    real_vector3d original;
                    ai_reference_frame_inverse_transform_vector(actor->pathfinding_point.location[0], &actor->facing_vector, &original);
                    float best_score = 0.0f;
                    const float distance_scale = 0.2f;
                    real_vector3d best;
                    for (short index = 0; index < 8; index++)
                    {
                        real_vector3d trial = original;
                        float angle = (float)index * 0.7853982f;
                        float cosine = (float)cos(angle);
                        float sine = (float)sin(angle);
                        rotate_vector_about_axis(&trial, global_up3d, sine, cosine);
                        path_collision_result collision;
                        pathfinding_test_vector(&actor->pathfinding_point.point, actor->pathfinding_sector, g_sector_none, &trial, 5.0f, 0, &collision);
                        float score = (original.i * trial.i + (trial.k * original.k + trial.j * original.j) + 1.0f) * (collision.t * distance_scale);
                        if (score > best_score)
                        {
                            best_score = score;
                            found = true;
                            best = trial;
                        }
                    }
                    if (found)
                        ai_reference_frame_inverse_transform_vector(actor->pathfinding_point.location[0], &best, &panic->direction);
                }
            }
            unit_start_running_blindly(actor->unit_index);
            if (behavior_misc_actor_get_physics_mode(actor_index) != c_character_physics_component::_mode_sentinel)
                actor_exit_posture(actor_index);
            result = true;
        }
    }
    return result;
}


// @address 0xc2938 ?smash_obstacle_get_impulse@@YAFJPAVc_behavior_state@@@Z
short smash_obstacle_get_impulse(long actor_index, c_behavior_state *state)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    actor_path *path = &actor->path;
    if (path->obstacle_object_index == -1 || (_rotl((unsigned short)path->result_value, 30) & 1) == 0)
        return -1;
    if (motor_animation_busy(actor->unit_index) || (actor->flags & 1) == 0)
        return -1;

    unit_datum *obstacle = OBJECT_GET(path->obstacle_object_index, unit_datum, 0xffffffff);

    real_vector3d direction = actor->throttle;
    direction.k = 0.0f;
    real_vector3d facing = actor->facing_vector;
    facing.k = 0.0f;

    float direction_length = normalize3d(&direction);
    if (direction_length <= 0.0f)
        return -1;
    if (normalize3d(&facing) <= 0.0f)
        return -1;

    unit_datum *unit = OBJECT_GET(actor->unit_index, unit_datum, 0x2003);
    character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
    float hint_radius = movement->pathfinding_radius;

    real_vector3d offset;
    vector_from_points3d(&actor->position.body, &obstacle->object.bounding_center, &offset);
    float distance = dot_product3d(&direction, &offset);
    if (distance <= 0.0f)
        return -1;
    if (distance >= obstacle->object.bounding_radius + hint_radius + direction_length)
        return -1;
    if (fabs(unit->object.bounding_center.z - obstacle->object.bounding_center.z) >= obstacle->object.bounding_radius * 0.5f + unit->object.bounding_radius)
        return -1;

    real_vector3d left;
    left.i = -direction.j;
    left.j = direction.i;
    left.k = 0.0f;
    float lateral = dot_product3d(&left, &offset);
    if (fabs(lateral) >= (hint_radius + obstacle->object.bounding_radius) * 0.6f)
        return -1;

    if (unit->object.object_type == 0)
        distance -= ((biped_definition *)tag_get('bipd', unit->object.definition_index))->physics.radius;
    distance -= obstacle->object.bounding_radius;
    if (dot_product3d(&direction, &facing) <= 0.95f)
        return -1;

    bool found = false;
    short mode = 0;
    float distance1 = 0.0f;
    float distance2 = 0.0f;
    c_sector_ref sector;
    c_ai_point3d point;
    object_get_pathfinding_location(path->obstacle_object_index, &sector, &point);
    real_vector3d right;
    scale_vector3d(&left, -1.0f, &right);

    if (smash_obstacle_test_direction(path->obstacle_object_index, &point.point, &left, sector, &distance1) && lateral > 0.0f)
    {
        mode = 1;
        found = true;
    }
    if (!found)
    {
        if (smash_obstacle_test_direction(path->obstacle_object_index, &point.point, &right, sector, &distance2) && lateral < 0.0f)
        {
            mode = 2;
            found = true;
        }
    }
    if (!found)
    {
        float max_distance = distance1 > distance2 ? distance1 : distance2;
        if (max_distance > 0.5f)
        {
            distance1 += lateral;
            distance2 -= lateral;
            if (distance1 > distance2)
                mode = 1;
            else
                mode = 2;
        }
        else
        {
            mode = 0;
        }
        found = true;
    }
    if (found)
    {
        long animation;
        switch (mode)
        {
        case 0:
        case 1:
            animation = 275;
            break;
        case 2:
            animation = 276;
            break;
        }

        float total_frames;
        float key_frame;
        short total_ticks;
        short key_ticks;
        bool in_range;
        if (motor_animation_get_range(actor->unit_index, 226, animation, 0, true, &total_frames, &total_ticks, &key_frame, &key_ticks))
            in_range = distance < key_frame * 1.33f;
        else
            in_range = distance < 0.4f;
        if (in_range)
        {
            action_request request;
            csmemset(&request, 0, sizeof(request));
            request.action = 39;
            request.melee.direction = mode;
            request.melee.target = path->obstacle_object_index;
            request.melee.flags = 0;
            bool submitted = action_submit(actor->unit_index, &request);
            ignored_obstacle_register(actor_index, path->obstacle_object_index, -1, submitted);
            actor->path.result_value = 0;
            actor->path.obstacle_object_index = -1;
        }
    }
    return -1;
}

// @address 0x42d380 ?behavior_blind_panic_perform@@YA_NJPAVc_behavior_state@@@Z
bool behavior_blind_panic_perform(long, c_behavior_state *)
{
    return true;
}

// @address 0x7c5544 ?smash_obstacle_impulse@@3VBehavior_impulse@@A
Behavior_impulse smash_obstacle_impulse = Behavior_impulse(8, 0x3ff007f8, smash_obstacle_get_impulse);

// @address 0x7c54f8 ?blind_panic_behavior@@3VPrimitive_behavior@@A
Primitive_behavior blind_panic_behavior = Primitive_behavior(12, -1, behavior_blind_panic_get_desire,
    behavior_blind_panic_next, behavior_blind_panic_activate, behavior_blind_panic_deactivate, -1,
    behavior_blind_panic_perform, behavior_blind_panic_control);
