// ailibxbox/actor_moving_ground.obj: from the records 0xc5240 0x56db00 0x56df90
// @flags /TP /O2 /Oc
#include "ai/pathfinding_collision.h"
#include "ai/character_behavior_properties.h"
#include "ai/actor_type_flood_types.h"
#include "ai/actor_moving_hints.h"
#include "units/weapon_addition_method_types.h"
#include "tag_files/tag_types.h"
#include "tag_files/tag_groups.h"
#include "ai/sector_ref.h"
#include "ai/ai_point3d.h"
// @address 0x56df90 actor_can_advance
#include "math/real_math.h"
#include "ai/actor_datum.h"
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "ai/path_obstacles.h"
#include "ai/prop_dialogue.h"
#include "ai/sector.h"
#include "units/unit_animation.h"
#include "ai/path_obstacle_avoidance.h"
#include "ai/actor_moving_objects.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "objects/object_datum.h"
#include "objects/object_header.h"
#include "ai/clump_datum.h"
#include "ai/prop_ref_datum.h"
#include "game/game_time_inline.h"
#include "units/unit_types.h"
#include "units/biped_types.h"
#include "ai/ai_reference_frame.h"
#include "ai/clump.h"
#include "ai/actor_array_types.h"
#include "cache/cache_files.h"
#include "ai/special_movement_types.h"
#include "ai/path_types.h"
#include "ai/pathfinding_geometry.h"
#include "ai/ai_hint_definitions.h"
#include "ai/ai_information_data.h"
#include "ai/path_obstacle_types.h"
#include "ai/actor_moving_types.h"
#include "ai/interaction_target.h"
#include "ai/ai_direction.h"
#include "objects/object_placement.h"
#include "units/action_request.h"
#include "units/motor_enums.h"
#include "ai/pathfinding_data.h"
#include "units/units.h"
#include "ai/character_properties_types.h"
#include "objects/object_definitions.h"

// 28-byte movement leg at elem + 92 (actor_path_point)

// movement state at actor + 0x72c: header bytes plus position blob indexed by `index * 28`


#include "ai/pathfinding_pill_result.h"


#include "ai/obstacle_profile.h"


#include "objects/object_ai_properties.h"


// 0x56df90
struct profile_source;
enum e_deceleration_obstacle_results;
#include "ai/actor_moving.h"
#include "ai/firing_position_accessors.h"
#include "ai/firing_position_definition.h"
#include "motor/mover_inline.h"
#include "ai/actor_behavior_state.h"

long actor_can_advance(long actor_index, const real_vector3d *direction, float distance)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    character_movement_properties *props = character_definition_movement_properties_get(actor->character_index);
    float default_speed = 0.2f;
    float limit;
    long result;
    if (props != 0) {
        float max_speed = props->destination_tolerance;
        limit = 0.05f;
        if (max_speed > limit)
            limit = max_speed;
    } else {
        limit = default_speed;
    }
    actor_path *movement = &actor->path;
    result = false;
    char index = movement->point_index;
    bool indices_equal = (movement->point_index == movement->point_count - 1);
    if (distance < limit) {
        actor_path *element = (actor_path *)((actor_path_point *)movement + index);
        actor_path_point *leg = 0;
        bool advanced = false;
        result = true;
        actor_path_point *positions = &element->points[0];
        if (!indices_equal)
            leg = &element->points[1];
        float zero = 0.0f;
        if (!indices_equal) {

            if (leg->frame_index == 1 || leg->frame_index == 6) {
                real_vector3d leg_vector;
                ai_vector_from_points3d(&positions->point, &leg->point, &leg_vector);
                if (dot_product3d(direction, &leg_vector) < zero)
                    result = false;
                advanced = true;
            }
        }
        if (!advanced && !indices_equal && (unsigned short)leg->frame_index == 0xffff) {
            s_object_header *header = object_header_get(actor->unit_index);
            unit_datum *object = (unit_datum *)header->datum;
            short field_offset = object->unit.motor_offset;
            if (*(long *)((char *)object + field_offset) == 0) {
                real_vector3d path_vector;
                real_vector3d actor_vector;
                if (index - 1 >= 0) {
                    actor_path *element2 = (actor_path *)((actor_path_point *)movement + movement->point_index);
                    ai_vector_from_points3d(&element2->points[-1].point, &element2->points[0].point, &path_vector);
                } else {
                    ai_vector_from_points3d(&movement->start_point, &element->points[0].point, &path_vector);
                }
                ai_vector_from_points3d(&actor->position.body, &element->points[0].point, &actor_vector);
                normalize3d(&path_vector);
                if (!(dot_product3d(&path_vector, &actor_vector) < limit * 0.5f)) {
                    actor_path_point *target = &element->points[1];
                    result = false;
                    if ((unsigned short)target->frame_index == 0xffff) {
                        actor_find_pathfinding_location(actor_index);
                        if (actor->sector_valid != 0) {
                            if (actor->pathfinding_sector.valid()) {
                                float prop_value;
                                if (props != 0)
                                    prop_value = props->pathfinding_radius;
                                else
                                    prop_value = default_speed;
                                real_vector3d test_vector;
                                real_point3d start;
                                union {
                                    real_point3d end;
                                    pathfinding_test_pill2d_result test_result;
                                };
                                ai_point_get_position(&actor->pathfinding_point, &start);
                                ai_point_get_position(&target->point, &end);
                                test_vector.i = end.x - start.x;
                                test_vector.j = end.y - start.y;
                                test_vector.k = zero;
                                float length = normalize3d(&test_vector);
                                if (!(length <= zero)) {
                                    bool blocked = pathfinding_test_pill2d(false, 0, -1, 2, true, &start, actor->pathfinding_sector,
                                        target->sector, &test_vector, prop_value, length, false, true, false, 0, &test_result);
                                    if (!blocked) {
                                        result = true;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}


// @rdata 0x82022dc0
static const float k_actor_ground_half = 0.5f;
// @rdata 0x8202b128
static const float k_actor_ground_default_deceleration_distance = 1.2f;

static __forceinline long actor_ground_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)TLS_GAME_TIME_GLOBALS->ticks_per_second + k_actor_ground_half);
}
static __forceinline long actor_ground_game_seconds_integer_to_ticks(long seconds)
{
    return seconds * TLS_GAME_TIME_GLOBALS->ticks_per_second;
}

static __forceinline actor_datum *actor_ground_clump_iterator_next(clump_actor_iterator *iterator)
{
    actor_datum *result = 0;
    if (iterator->next_actor_index != -1) {
        result = datum_get<actor_datum>(tls_actor_data(), iterator->next_actor_index);
        iterator->actor_index = iterator->next_actor_index;
        iterator->next_actor_index = result->next_clump_actor_index;
    }
    return result;
}

// @address 0x56db00 ?actor_ground_throttle_control@@YAMJ_NPBTreal_vector3d@@1M@Z
float actor_ground_throttle_control(long actor_index, bool ignore_obstacles, const real_vector3d *movement_direction,
    const real_vector3d *avoidance_direction, float path_distance)
{
    c_flags_no_init<long, short, 8> obstacle_flags;
    long object_index;
    float stop_distance;
    // Typed alias; removing it renumbers inlined code in 0xc5240.
    const real_vector3d *movement_vector = movement_direction;
    special_movement special;
    s_obstacle_profile profile;
    obstacles obstacle_list;

    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    character_movement_properties *movement = (character_movement_properties *)character_definition_movement_properties_get(actor->character_index);
    float attenuation = 1.0f;
    if (!ignore_obstacles)
    {
        long clump = actor->clump_index;
        if (clump != -1 && actor->root_vehicle_index == -1)
        {
            actor_special_movement_new(actor_index, &special);
            obstacle_profile_initialize(&profile, &actor->position.body, 4.0f, 128, (profile_source *)&special,
                &actor->throttle, actor->unit_index, actor->destination_orders.target_index);
            obstacle_profile_set_actor(actor_index, 0, movement_vector, 2, 1, &profile);
            obstacle_list.obstacle_count = 0;
            obstacle_list.disc_count = 0;
            obstacle_list.flag1_count = 0;
            obstacle_list.flag8_count = 0;
            obstacle_list.unused = 0;
            clump_actor_iterator iterator;
            iterator.next_actor_index = datum_get<clump_datum>(tls_clump_data(), clump)->first_actor_index;
            actor_datum *other;
            while ((other = actor_ground_clump_iterator_next(&iterator)) != 0) {
                if (other == actor)
                    continue;
                long unit = other->unit_index;
                if (unit == -1 || unit == actor->movement_obstacle_index)
                    continue;
                path_obstacle_avoidance_add_object_discs(actor_index, unit, &profile, 2, 1, 0, &obstacle_list);
            }
            if (actor->movement_obstacle_index != -1)
                path_obstacle_avoidance_add_object_discs(actor_index, actor->movement_obstacle_index, &profile, 2, 1, 0, &obstacle_list);
            if (obstacle_list.disc_count > 0) {
                obstacle_flags.clear();
                actor_find_pathfinding_location(actor_index);
                if (actor_test_ignorable_obstacles(actor_index, &obstacle_list, 2, true, &actor->pathfinding_point.point, g_sector_none,
                        avoidance_direction, movement->pathfinding_radius, path_distance, &object_index, &obstacle_flags.m_storage) &&
                    obstacle_flags.test(7))
                {
                    stop_distance = k_actor_ground_half;
                    actor->movement_obstacle_index = object_index;
                    actor->movement_obstacle_ticks = (short)actor_ground_seconds_to_ticks_round(1.0f);
                }
            }
        }
        stop_distance = k_actor_ground_half;
        long tag = actor->character_index;
        float *params = 0;
        while (tag != -1) {
            character_definition *definition = TAG_GET('char', character_definition, tag);
            if (definition->flocking_properties.count > 0) {
                params = (float *)definition->flocking_properties.address;
                break;
            }
            tag = definition->parent_character.index;
        }
        float outer = stop_distance;
        if (params != 0 && params[5] > 0.0f)
            outer = params[5];
        if (actor->berserking != 0) {
            actor->movement_object_index = -1;
        } else {
            if (actor->movement_object_index != -1 && game_time_get() - actor->movement_object_index < actor_ground_seconds_to_ticks_round(outer)) {
                actor->flags &= ~1;
                attenuation = 0.0f;
            } else if (actor->movement_obstacle_index != -1) {
                float deceleration_distance = k_actor_ground_default_deceleration_distance;
                float deceleration_velocity = 1.5f;
                float deceleration_stop_distance = k_actor_ground_half;
                float enter_threshold = 0.3f;
                float exit_threshold = stop_distance;
                if (params != 0) {
                    deceleration_distance = params[0];
                    deceleration_velocity = params[1];
                    deceleration_stop_distance = params[2];
                    enter_threshold = params[3];
                    exit_threshold = params[4];
                }
                attenuation = deceleration_obstacle_attenuation(actor_index, actor->unit_index, movement->pathfinding_radius, deceleration_distance,
                    deceleration_velocity, deceleration_stop_distance, &avoidance_direction->xy, 0, 0, 0);
                if (actor->movement_object_index != -1)
                    enter_threshold = exit_threshold;
                if (attenuation < enter_threshold) {
                    actor->flags &= ~1;
                    attenuation = 0.0f;
                    if (actor->movement_object_index == -1) {
                        actor->movement_object_index = game_time_get();
                        stop_distance = k_actor_ground_half;
                    }
                } else {
                    actor->movement_object_index = -1;
                    stop_distance = k_actor_ground_half;
                }
            } else {
                actor->movement_object_index = -1;
            }
        }
    }
    else
    {
        stop_distance = k_actor_ground_half;
    }
    float throttle_scale = 1.0f;
    float limited_attenuation = MIN(attenuation, 1.0f);
    if (actor->speed_override_valid != 0)
        throttle_scale = actor->speed_override;
    else if (actor->throttle_close != 0)
        actor->control_flags |= 0x20000;
    else if (actor->moving_crouch == 0 && (actor->flags & (1 << 28)) != 0 && (unsigned)(actor->output_animation_state - 138) <= 2)
        throttle_scale = stop_distance;
    if (movement->maximum_throttle > 0.0f) {
        if (throttle_scale > movement->maximum_throttle)
            throttle_scale = movement->maximum_throttle;
    }
    return limited_attenuation * throttle_scale;
}


// @rdata 0x820242d8
static const float k_actor_ground_jump_scale = 2.0f;
// @rdata 0x82028bec
static const float k_actor_ground_jump_facing_cosine = 0.99f;



static __forceinline void actor_ground_action_request_new(action_request *request, long type)
{
    memset(request, 0, sizeof(*request));
    request->type = type;
}

static __forceinline actor_datum *actor_ground_calc_actor_get(long actor_index)
{
    return &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index)];
}

static __forceinline float actor_ground_magnitude3d(const real_vector3d *v)
{
    return sqrtf(magnitude_squared3d(v));
}

// match: 0xc6258: yzx ordering and the j reference preserve five words; March 82fa8920 calls dot_product3d.
static __forceinline float actor_ground_dot3d_yzx(const real_vector3d *a, const real_vector3d *b)
{
    const float &j = +a->j;
    return (j * b->j + a->k * b->k) + a->i * b->i;
}

static __forceinline float actor_ground_obstacle_projection(const real_vector3d *a, const real_vector3d *b, float direction_i)
{
    return (a->i * direction_i + a->j * b->j) + a->k * b->k;
}

static __forceinline long actor_get_combat_movement_type(long actor_index)
{
    actor_datum *actor = actor_ground_calc_actor_get(actor_index);
    long result = 138;
    if (actor->throttle_ticks > 0)
        result = 140;
    else if (actor->had_shield)
        result = 139;
    return result;
}

static __forceinline short actor_get_next_hint_step(long actor_index)
{
    short result = -1;
    if (actor_path_executing(actor_index)) {
        actor_datum *actor = actor_ground_calc_actor_get(actor_index);
        actor_path *movement = &actor->path;
        for (short step_index = 0; step_index < movement->point_count; step_index++) {
            actor_path_point *step = &movement->points[step_index];
            if (step->frame_index != -1) {
                result = step_index;
                break;
            }
        }
    }
    return result;
}

static __forceinline void actor_ground_set_leap_control(
    actor_datum *actor, const real_vector2d *direction, float angle1, float angle2, long hint_index)
{
    actor->flags |= (1 << 6);
    actor->jump_aim = *direction;
    actor->jump_horizontal_velocity = angle1;
    actor->jump_vertical_velocity = angle2;
    actor->movement_index = hint_index;
}

// @address 0xc5240 ?actor_move_calculate_ground_movement@@YA?AW4e_movement_result@@JPBTreal_vector3d@@PAT2@1F@Z
e_movement_result actor_move_calculate_ground_movement(
    long actor_index, const real_vector3d *desired_vector, real_vector3d *out_facing_vector, real_vector3d *out_throttle, short unused)
{
    actor_datum *actor = actor_ground_calc_actor_get(actor_index);
    character_movement_properties *movement_props =
        (character_movement_properties *)character_definition_movement_properties_get(actor->character_index);
    actor_path *movement = &actor->path;
    e_movement_result result;
    bool on_final_step = false;
    bool hint_handled = false;
    actor_path_point *current_step = 0;
    bool variable_up = false;

    real_vector3d facing_vector;
    // match: the dead stores to facing_vector, throttle_vector, movement_vector and cross are not in March (0x82fa71a8);
    // removing them moves 17 words of 0xc5240.
    facing_vector.k = 0.0f;
    facing_vector = actor->facing_vector;
    real_vector3d throttle_vector;
    throttle_vector.j = 0.0f;
    throttle_vector.k = 0.0f;
    throttle_vector = *global_zero_vector3d;
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(actor->unit_index, _object_mask_unit);
    const real_vector3d *up_vector;
    if (mover_has_variable_camera_up(actor->unit_index)) {
        up_vector = &unit->object.up;
        variable_up = true;
    } else {
        up_vector = global_up3d;
    }

    if (actor_path_executing(actor_index)) {
        current_step = &movement->points[movement->point_index];
        on_final_step = (movement->point_index == movement->point_count - 1);
    }

    real_vector3d movement_vector;
    movement_vector.k = 0.0f;
    float jump_scale = 0.0f;
    movement_vector.j = 0.0f;
    movement_vector = *desired_vector;
    if (variable_up) {
        real_vector3d cross;
        cross.j = 0.0f;
        cross.k = 0.0f;
        // match: March passes up_vector directly (82fa72a8); the local copy keeps 0xc5240 register order (5 words without).
        real_vector3d up;
        up.j = up_vector->j;
        up.k = up_vector->k;
        up.i = up_vector->i;
        cross_product3d(&up, &movement_vector, &cross);
        cross_product3d(&cross, &up, &movement_vector);
    } else {
        movement_vector.k = 0.0f;
    }

    float distance = normalize3d(&movement_vector);
    if (variable_up && current_step != 0) {
        // match: 0xc5240: March tests current_step->frame_index != 0xffff directly (82fa72f8); without this reference binding the function is 24 bytes short.
        const unsigned long &step_type = (unsigned short)current_step->frame_index;
        if (step_type != 0xffff) {
            distance = actor_ground_magnitude3d(desired_vector);
        }
    }
    if (distance == 0.0f) {
        movement_vector = actor->facing_vector;
    }

    if (actor->blind_panic_control_active) {
        result = _movement_result_none;
    } else if (actor_path_executing(actor_index)) {
        result = (e_movement_result)actor_can_advance(actor_index, &movement_vector, distance);
    } else {
        result = _movement_result_advance;
    }

    process_pending_ground_actions(actor_index);

    if (result == _movement_result_none && movement_props != 0 && !motor_animation_busy(actor->unit_index) && actor_path_executing(actor_index)) {
        bool handled = false;
        long motor_program = mover_get_motor_program(actor->unit_index);
        if (motor_program == 6 || motor_program == 9) {
            actor_exit_posture(actor_index);
            handled = true;
        }
        if (!handled && !actor->destination_orders.ground_destination() && !actor->use_object && TEST_BIT(movement_props->flags, 9)) {
            action_submit_event(actor->unit_index, 27);
            actor->flags &= ~(1 << 0);
            handled = true;
        }
        if (!handled && current_step->frame_index == -1 && motor_program == 5) {
            character_general_properties *general_props = character_definition_general_properties_get(actor->character_index);
            if (!TEST_BIT(movement_props->flags, 9) && (general_props == 0 || !TEST_BIT(general_props->flags, 1))) {
                actor_exit_posture(actor_index);
            } else {
                action_submit_event(actor->unit_index, 27);
            }
        }
    }

    if (!actor->search_active && actor_path_executing(actor_index) && actor->destination_orders.type == 3 && TEST_BIT(actor->flags, 1)) {
        float min_distance = 5.0f;
        bool far_enough = false;
        if (!TEST_BIT(actor->flags, 26)) {
            const c_ai_point3d *fp_point = (const c_ai_point3d *)firing_position_ref_get_definition(
                actor->destination_orders.firing_position);
            float fp_dist = sqrtf(ai_distance_squared3d(fp_point, &actor->position.body));
            if (fp_dist < min_distance) {
                fp_dist = actor_path_distance_remaining(actor_index, -1);
            }
            if (fp_dist >= min_distance) {
                far_enough = true;
            }
        } else {
            far_enough = true;
        }
        if (far_enough && actor->target_prop_index != -1) {
            prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, actor->target_prop_index);
            far_enough = prop_ref->distance > min_distance;
        }
        if (far_enough) {
            actor->flags |= (1 << 26);
            actor->flags &= ~(1 << 1);
            actor->moving_crouch = false;
        } else {
            actor->flags &= ~(1 << 26);
        }
    } else {
        actor->flags &= ~(1 << 26);
    }

    long movement_type = actor_get_movement_type(actor_index);
    if (movement_type == 145 && actor->root_vehicle_index == -1) {
        long anim_mode;
        long anim_state;
        if (motor_animation_get_mode_and_state(actor->unit_index, &anim_mode, &anim_state) && anim_mode != 145) {
            bool has_weapon = unit_animation_test_weapon(actor->unit_index, 145, 226, 226, false);
            if (!has_weapon) {
                unit_datum *unit_obj = (unit_datum *)object_get_and_verify_type(actor->unit_index, _object_mask_unit);
                signed char weapon_slot = unit_obj->unit.current_weapon_index;
                if (weapon_slot != -1) {
                    long weapon_index = unit_obj->unit.weapon_object_indices[weapon_slot];
                    if (weapon_index != -1) {
                        character_retreat_properties *retreat_props = character_definition_retreat_multi_properties_get(actor_index);
                        if (retreat_props != 0 && retreat_props->backup_weapon.index != -1 &&
                            object_get(weapon_index)->definition_index != retreat_props->backup_weapon.index) {
                            object_placement_data placement;
                            object_placement_data_new(&placement, retreat_props->backup_weapon.index, actor->unit_index, 0);
                            long new_weapon_index = object_new(&placement);
                            if (new_weapon_index != -1) {
                                has_weapon = unit_add_weapon_to_inventory(actor->unit_index, new_weapon_index, _weapon_addition_method_swap_for_primary_weapon);
                            }
                        }
                    }
                }
                if (!has_weapon) {
                    movement_type = actor_get_combat_movement_type(actor_index);
                }
            }
        }
    }
    if (movement_type != -1) {
        actor->output_animation_state = movement_type;
    }

    if (result != _movement_result_advance) {
        if (actor->movement_3d) {
            result = _movement_result_done;
            actor->flags &= ~(1 << 0);
            if (actor->type != 15 && actor->controlled_unit_index == -1 && actor->root_vehicle_index != -1) {
                actor->flags &= ~(1 << 9);
            } else {
                actor->flags |= (1 << 9);
            }
        } else if (actor_move_animation_busy(actor_index) && !action_executing(actor->unit_index, _action_smash)) {
            actor->flags &= ~(1 << 0);
            actor->flags &= ~(1 << 9);
        } else {
            s_object_header *header = object_header_get(actor->unit_index);
            if (header->object_type == 0 && ((biped_datum *)header->datum)->biped.physics.m_mode == 4) {
                actor->flags &= ~(1 << 0);
                actor->flags &= ~(1 << 9);
                result = _movement_result_done;
            } else if (actor->airborne && !actor->use_object) {
                actor->flags &= ~(1 << 0);
                actor->flags |= (1 << 9);
            } else if (actor->player_visibility_ticks > 0) {
                actor->flags &= ~(1 << 0);
                result = _movement_result_done;
            }
        }
    }

    if (result == _movement_result_none) {
        if ((actor->output_animation_state != 138 && actor->output_animation_state != 139 && actor->output_animation_state != 140) ||
            (movement_props != 0 && TEST_BIT(movement_props->flags, 1))) {
            actor->flags &= ~(1 << 1);
        }
    }

    if (actor->player_visible) {
        actor->crouch = true;
        actor->moving_crouch = true;
    }
    if (actor->path_aim) {
        actor->flags |= (1 << 10);
    }

    if (result == _movement_result_none && TEST_BIT(actor->flags, 0)) {
        if (!actor->zigzag || distance <= 0.8f) {
            actor->zigzag_timer = 0;
        } else {
            character_retreat_properties *retreat_props = character_definition_retreat_multi_properties_get(actor_index);
            if (retreat_props == 0 || retreat_props->zigzag_period <= 0.0f || retreat_props->zigzag_angle <= 0.0f) {
                actor->zigzag_timer = 0;
            } else {
                actor_find_pathfinding_location(actor_index);
                if (actor->pathfinding_sector.valid()) {
                    float angle = retreat_props->zigzag_angle;
                    real_vector3d rotated_vector = movement_vector;
                    short period_ticks = (short)(long)(retreat_props->zigzag_period * (float)TLS_GAME_TIME_GLOBALS->ticks_per_second + 0.5f);
                    if (distance < 1.6f) {
                        angle = (distance - 0.8f) * angle * 1.25f;
                    }
                    short phase = (short)((actor->zigzag_timer << 1) / period_ticks);
                    if (phase == 1) {
                        angle = -angle;
                    } else if (phase > 1) {
                        actor->zigzag_timer = 0;
                    }
                    float c = (float)cos(angle);
                    float s = (float)sin(angle);
                    rotate_vector_about_axis(&rotated_vector, global_up3d, s, c);
                    real_vector3d transformed_vector;
                    ai_reference_frame_inverse_transform_vector(actor->pathfinding_point.reference_frame(), &rotated_vector, &transformed_vector);
                    path_collision_result collision_result;
                    pathfinding_test_vector(
                        &actor->pathfinding_point.point, actor->pathfinding_sector, g_sector_none, &transformed_vector, 0.5f, 0, &collision_result);
                    if (!collision_result.collision) {
                        movement_vector = rotated_vector;
                        actor->zigzag_timer++;
                    } else {
                        actor->zigzag_timer = 0;
                    }
                } else {
                    actor->zigzag_timer = 0;
                }
            }
        }
        {
            facing_vector = movement_vector;
            throttle_vector = *global_forward3d;
            float throttle_scale = actor_ground_throttle_control(actor_index, variable_up, &facing_vector, &movement_vector, distance);
            scale_vector3d(&throttle_vector, throttle_scale, &throttle_vector);
        }
    }
    actor->flags &= ~(1 << 4);
    *out_facing_vector = facing_vector;
    *out_throttle = throttle_vector;

    if (actor->output_animation_state == 138 || actor->output_animation_state == 139 || actor->output_animation_state == 140) {
        short next_hint_step = actor_get_next_hint_step(actor_index);
        if (next_hint_step != -1) {
            actor_path_point *step = &movement->points[next_hint_step];
            if ((step->frame_index == 1 || step->frame_index == 6 || step->frame_index == 5) &&
                actor_path_distance_remaining(actor_index, next_hint_step) < 1.0f &&
                actor->look_mode < 4) {
                real_vector3d step_vector;
                if (actor_path_step_get_vector(actor_index, next_hint_step, &step_vector) && normalize3d(&step_vector) > 0.0f) {
                    actor->look_mode = 4;
                    actor->look_direction.to_vector(&step_vector);
                    actor->steering_vector = step_vector;
                    actor->flags |= (1 << 1);
                    real_point3d step_point;
                    ai_point_get_position(&step->point, &step_point);
                    actor->aim_mode = 2;
                    actor->aim_direction.to_point(&step_point);
                }
            }
        }
    }

    if (actor_path_executing(actor_index) && !motor_animation_busy(actor->unit_index) && current_step->frame_index != -1) {
        c_hint_ref *hint_ref = &current_step->hint;
        pathfinding_hint *hint_data = 0;
        if (hint_ref->valid()) {
            hint_data = &((pathfinding_hint *)pathfinding_data_get(hint_ref->structure_bsp_index)->hints.address)[hint_ref->hint_index];
        }
        hint_handled = true;
        const real_vector3d *facing = &actor->facing_vector;
        switch (current_step->frame_index) {
        case 1:
        case 6:
            actor_find_pathfinding_location(actor_index);
            *out_facing_vector = movement_vector;
            *out_throttle = *global_zero_vector3d;
            actor->flags |= (1 << 10);
            actor->flags &= ~(1 << 9);
            if (actor->pathfinding_sector == current_step->sector) {
                result = _movement_result_advance;
            } else {
                bool can_jump = false;
                if (movement_props != 0 && TEST_BIT(movement_props->flags, 8) && current_step->frame_index == 6) {
                    can_jump = true;
                } else if (TEST_BIT(actor->flags, 0)) {
                    if (TEST_BIT(actor->flags, 1) && current_step->frame_index != 6) {
                        can_jump = true;
                    } else {
                        can_jump = actor_ground_dot3d_yzx(facing, &movement_vector) > 0.95;
                    }
                }
                if (can_jump && current_step->frame_index == 1 && hint_data != 0 && TEST_BIT(hint_data->raw_data[2], 0)) {
                    can_jump = false;
                }
                if (can_jump) {
                    bool parabolic = (hint_data != 0 && !TEST_BIT(hint_data->raw_data[2], 2) && hint_data->jump_geometry_end != hint_data->jump_geometry_start);
                    char force_velocity = hint_data != 0 ? hint_data->jump_height : 0;
                    if (actor_moving_jump(actor_index, hint_ref, hint_data != 0, force_velocity, &current_step->point, current_step->sector, !parabolic)) {
                        *out_throttle = *global_forward3d;
                    } else if (parabolic) {
                        *out_throttle = *global_zero_vector3d;
                        actor->flags &= ~(1 << 0);
                        result = _movement_result_done;
                    } else {
                        actor->leap_requested = true;
                        actor->leap_target_valid = true;
                        actor->leap_direction = movement_vector.xy;
                        actor->leap_speed = 1.5f;
                        actor->leap_height = 1.5f;
                        actor->leap_prop_index = -1;
                    }
                } else {
                    actor->flags &= ~(1 << 0);
                }
            }
            break;
        case 5:
            if (TEST_BIT(actor->flags, 0) && hint_data != 0) {
                actor_find_pathfinding_location(actor_index);
                *out_facing_vector = movement_vector;
                *out_throttle = *global_zero_vector3d;
                actor->flags |= (1 << 10);
                actor->flags &= ~(1 << 9);
                if (dot_product2d(&movement_vector.xy, &facing->xy) > 0.95 &&
                    !ai_hint_reserved(actor_index, *hint_ref)) {
                    pathfinding_data *pf_data = pathfinding_data_get(hint_ref->structure_bsp_index);
                    real_point3d *vertex = TAG_BLOCK_GET_ELEMENT(&pf_data->vertices, (unsigned short)hint_data->raw_data[1], real_point3d);
                    action_request hoist_request;
                    actor_ground_action_request_new(&hoist_request, 41);
                    hoist_request.hoist.position = *vertex;
                    scale_vector3d(TAG_BLOCK_GET_ELEMENT(&pf_data->vertices, hint_data->alignment_vertex, real_vector3d), -1.0f,
                        &hoist_request.hoist.alignment);
                    ((c_interaction_target *)&hoist_request.hoist.reference)->set_hint(*hint_ref);
                    if (action_submit(actor->unit_index, &hoist_request)) {
                        actor_hint_reserve(actor_index, *hint_ref, true);
                        result = _movement_result_advance;
                    }
                } else {
                    actor->flags &= ~(1 << 0);
                }
            }
            break;
        }
    }

    if (movement_props != 0) {
        if (movement->obstacle_object_index != -1) {
            if (!motor_animation_busy(actor->unit_index) && TEST_BIT(actor->flags, 0)) {
                hint_handled = actor_moving_handle_object_hint(
                    actor_index, movement->obstacle_object_index, &movement->obstacle_flags.m_storage, out_facing_vector, out_throttle);
            }
            if (!hint_handled && movement->obstacle_flags.test(1) && !motor_animation_busy(actor->unit_index) && TEST_BIT(actor->flags, 0)) {
                object_datum *obstacle_object = object_get(movement->obstacle_object_index);
                real_vector3d obstacle_vector;
                obstacle_vector.i = 0.0f;
                obstacle_vector.j = 0.0f;
                vector_from_points3d(&actor->position.body, &obstacle_object->bounding_center, &obstacle_vector);
                float max_dist = obstacle_object->bounding_radius + movement_props->pathfinding_radius * k_actor_ground_jump_scale;
                float obstacle_dist = normalize3d(&obstacle_vector);
                const real_vector3d *obstacle_facing = &actor->facing_vector;
                if (obstacle_dist > 0.0f && obstacle_dist < max_dist) {
                    float projected_dist = actor_ground_obstacle_projection(&obstacle_vector, &movement_vector, movement_vector.i) * obstacle_dist;
                    if (projected_dist > 0.0f) {
                        if (dot_product3d(obstacle_facing, &movement_vector) > k_actor_ground_jump_facing_cosine) {
                            short jump_height = 1;
                            s_object_definition *obj_def =
                                TAG_GET('obje', s_object_definition, obstacle_object->definition_index);
                            if (obj_def->ai_properties.count > 0) {
                                const object_ai_properties *ai_properties = (const object_ai_properties *)obj_def->ai_properties.address;
                                const long &size = ai_properties->leap_jump_speed;
                                if (size > 0) {
                                    short clamped = (size > 6 ? 6 : size) - 1;
                                    if (clamped > jump_height) {
                                        jump_height = clamped;
                                    }
                                }
                            }
                            char force_velocity = (char)(1 << jump_height);
                            real_point3d jump_dest_point;
                            jump_scale = projected_dist * k_actor_ground_jump_scale;
                            point_from_line3d(&actor->position.body, &movement_vector, jump_scale, &jump_dest_point);
                            c_ai_point3d jump_ai_point;
                            jump_ai_point.set(&jump_dest_point);
                            if (actor_moving_jump(actor_index, 0, true, force_velocity, &jump_ai_point, g_sector_none, true)) {
                                ignored_obstacle_register(actor_index, actor->path.obstacle_object_index, 1, false);
                                actor->path.obstacle_object_index = -1;
                                actor->path.obstacle_flags.clear();
                            }
                        } else {
                            *out_facing_vector = movement_vector;
                            *out_throttle = *global_zero_vector3d;
                            actor->flags |= (1 << 10);
                            actor->flags &= ~(1 << 9);
                            actor->flags &= ~(1 << 0);
                        }
                        hint_handled = true;
                    }
                }
            }
            if (!hint_handled && (movement_props->hoist_flags & 0xe0) != 0 && movement->obstacle_flags.test(4) &&
                !motor_animation_busy(actor->unit_index) && TEST_BIT(actor->flags, 0)) {
                object_marker mount_marker;
                real_point3d mount_point;
                if (ai_hint_find_mount_marker(actor_index, movement->obstacle_object_index, movement_props->pathfinding_radius, &mount_marker, &mount_point)) {
                    real_vector3d mount_vector;
                    vector_from_points3d(&actor->position.body, &mount_point, &mount_vector);
                    float mount_dist = actor_ground_magnitude3d(&mount_vector);
                    if (mount_dist > 0.0f) {
                        object_datum *obstacle_object = object_get(movement->obstacle_object_index);
                        float max_dist = obstacle_object->bounding_radius + movement_props->pathfinding_radius * k_actor_ground_jump_scale;
                        if (mount_dist < max_dist && actor_moving_jump(actor_index, &mount_point, true)) {
                            ignored_obstacle_register(actor_index, actor->path.obstacle_object_index, 4, false);
                            actor->path.obstacle_object_index = -1;
                            actor->path.obstacle_flags.clear();
                        }
                    }
                }
            }
        }
        if (!actor->leap_requested && TEST_BIT(actor->flags, 0) &&
            actor->path.obstacle_object_index == -1 && actor->combat_status >= 3 &&
            game_time_get() - actor->last_combat_jump_time > actor_ground_game_seconds_integer_to_ticks(3)) {
            real_point3d jump_point;
            if (TEST_BIT(movement_props->flags, 4) && actor->combat_jump_allowed) {
                ai_point_get_position(&actor->path.destination, &jump_point);
                if (distance_squared3d(&jump_point, &actor->position.body) > 16.0f &&
                    actor_moving_jump(actor_index, &jump_point, false)) {
                    actor->last_combat_jump_time = game_time_get();
                }
            }
            if (TEST_BIT(movement_props->flags, 3) && !actor->leap_requested && on_final_step && distance > k_actor_ground_jump_scale) {
                const real_point3d *body = &actor->position.body;
                point_from_line3d(body, desired_vector, 1.0f, &jump_point);
                actor_moving_jump(actor_index, &jump_point, false);
                actor->last_combat_jump_time = game_time_get();
            }
        }
    }

    if (!actor->airborne && actor->root_vehicle_index == -1) {
        if (actor->leap_requested && !actor_move_animation_busy(actor_index)) {
            bool submitted = false;
            if (actor->leap_active || actor->jump_boosted != 0) {
                action_request request;
                memset(&request, 0, sizeof(request));
                actor_ground_action_request_new(&request, 31);
                request.jump.has_position = true;
                request.jump_flag = actor->jump_boosted;
                if (actor->leap_target_valid) {
                    request.jump.direction = actor->leap_direction;
                } else {
                    request.jump.direction = actor->facing_vector.xy;
                    if (normalize2d(&request.jump.direction) == 0.0f) {
                        request.jump.direction = global_forward3d->xy;
                    }
                }
                submitted = action_submit(actor->unit_index, &request);
            }
            if (submitted) {
                actor_dialogue_event(actor_index, 49, -1, 0);
            } else {
                actor_ground_calc_actor_get(actor_index)->control_flags |= 2;
            }
            if (actor->leap_target_valid) {
                actor_ground_set_leap_control(actor, &actor->leap_direction, actor->leap_speed, actor->leap_height, actor->leap_prop_index);
            }
        }
    } else {
        actor->flags &= ~(1 << 6);
    }

    return result;
}

