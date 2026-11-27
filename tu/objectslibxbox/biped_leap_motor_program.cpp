// objectslibxbox/biped_leap_motor_program.obj: from the records 0x3857a8 0x38cd68 0x38cdd8 0x38d000 0x38d130 0x38d610 0x38d6b0 0x38d7f8 0x38daf8 0x38df10 0x38e088 0x38e2e0 0x38e660 0x38e760 0x38e860
// @flags /TP /O2 /Oc
#include "memory/datum_index.h"
#include "physics/character_physics_move_output.h"
#include "motor/biped_leap_tuning.h"
#include "physics/character_physics_input.h"
#include "animations/animation_state_change_settings.h"
#include "math/matrix_math_types.h"
#include "math/real_math.h"
#include "objects/object_datum.h"
#include "units/biped_types.h"
#include "units/biped_definitions.h"
#include "math/real_math_globals.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "units/motor_enums.h"
#include "units/action_request.h"
#include "physics/character_physics_output.h"
#include "units/motor_program_state.h"
#include "units/motor_request.h"
#include "motor/action_system.h"
#include "motor/motor_animation.h"
#include "objects/objects.h"
#include "ai/actor_moving.h"
#include "items/projectiles.h"
#include "motor/motor_system_biped.h"
#include "physics/havok.h"
#include "physics/character_physics_component.h"
#include "motor/biped_leap_motor_program.h"
#include "math/matrix_math.h"
#include "units/bipeds.h"
#include "objects/object_early_movers.h"
#include "animations/animation_id.h"
#include "units/slider_movement_pattern_types.h"
#include "cache/cache_files.h"
#include "units/slider_target_position_types.h"
#include "animations/mixing_board/mixing_board_definition.h"
#include "animations/mixing_board/mixing_board_slider_types.h"
#include <math.h>
#include <string.h>
#include <new>
#include "cseries/cseries.h"
#include "animations/animation_control_data.h"
#include "memory/tls_globals.h"
#include "animations/animation_manager.h"
#include "animations/animation_manager_mixing_board_inline.h"
#include "game/game_time_types.h"
// @address 0x38d6b0 ?compute_biped_leap_completion_ratio@@YAMPBVc_biped_leap_state@@PBUreal_matrix4x3@@M@Z
// @address 0x38d7f8 biped_leap_update_flight_orientation
// @address 0x38e660 ?leaping_biped_ground_plane_fix_transform_to_physics@@YAXJPAUreal_matrix4x3@@@Z
// @address 0x38e760 ?leaping_biped_ground_plane_fix_transform_from_physics@@YAXJPAUreal_matrix4x3@@@Z
// @address 0x38e860 ?leaping_preprocess_node_orientations@@YAXJPBV?$c_static_flags@$0PP@@@JPAUreal_orientation@@@Z
union real_vector3d;
struct real_matrix4x3;
class c_animation_channel_settings;
struct s_state_changes_requested;
struct s_animation_state_change_settings;
struct s_character_physics_update_input_datum;
union real_point3d;
union real_vector3d;
struct real_matrix4x3;

class c_biped_leap_state : public c_motor_program_state
{
public:
    real_quaternion start_rotation;         // 8
    real_quaternion flight_start_rotation;  // 24
    real_quaternion flight_end_rotation;    // 40
    real_quaternion end_rotation;           // 56
    real_orientation orientation;           // 72
    real_vector3d velocity;
    real_matrix4x3 start_matrix;            // 116
    real_matrix4x3 end_matrix;              // 168
    float flight_ratio;                     // 220
    float departure;                        // 224
    float twist;                            // 228
    float arrival;                          // 232
    union { bool valid; bool interpolate_long; };  // 236
    bool active;                            // 237
};
typedef char leap_state_rotation_offset[(offsetof(c_biped_leap_state, start_rotation) == 8) ? 1 : -1];
typedef char leap_state_matrix_offset[(offsetof(c_biped_leap_state, start_matrix) == 116) ? 1 : -1];
typedef char leap_state_active_offset[(offsetof(c_biped_leap_state, active) == 237) ? 1 : -1];

void project_world_matrix_to_up_vector(const real_vector3d *up_vector, const real_matrix4x3 *world_matrix, real_matrix4x3 *ground_matrix)
{
    ground_matrix->position = world_matrix->position;
    ground_matrix->scale = world_matrix->scale;
    real_vector3d_build_axes_from_vectors_using_up(&world_matrix->forward, up_vector, &ground_matrix->forward, &ground_matrix->left, &ground_matrix->up);
}

// @address 0x38d610 ?biped_leap_activate@@YA_NJ@Z
bool biped_leap_activate(long biped_index)
{
    biped_datum *biped = (biped_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(biped_index)].datum;
    c_biped_leap_state *state = (c_biped_leap_state *)((char *)biped + biped->unit.motor_program.offset);
    if (biped->biped.physics.m_mode != c_character_physics_component::_mode_ground)
        biped->biped.physics.set_mode(c_character_physics_component::_mode_ground);
    state->active = false;
    state->valid = false;
    state->orientation = *global_identity_orientation;
    return true;
}

float compute_biped_leap_completion_ratio(const c_biped_leap_state *leap_state, const real_matrix4x3 *object_world_matrix, float offset)
{
    float result = 0.0f;
    if (leap_state->active)
    {
        real_point3d start = leap_state->start_matrix.position;
        real_point3d end = leap_state->end_matrix.position;
        real_point3d current;
        point_from_line3d(&object_world_matrix->position, &object_world_matrix->up, offset, &current);
        real_vector3d direction;
        vector_from_points3d(&start, &end, &direction);
        float length = normalize3d(&direction);
        length = length < 0.0002f ? 0.0002f : length;
        real_vector3d travelled;
        vector_from_points3d(&start, &current, &travelled);
        float projection = dot_product3d(&direction, &travelled);
        result = PIN(projection / length, 0.0f, 1.0f);
    }
    return result;
}

void leaping_preprocess_node_orientations(long object_index, const c_static_flags<255> *node_flags, long node_count, real_orientation *orientations)
{
    biped_datum *unit = (biped_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
    if (unit->object.havok_component_index == -1) return;
    if (unit->biped.physics.m_mode != c_character_physics_component::_mode_ground) return;
    c_biped_leap_state *leap_state = (c_biped_leap_state *)((char *)unit + unit->unit.motor_program.offset);
    if (node_count <= 0) return;
    if (node_flags && !node_flags->test(0)) return;
    orientations_multiply(&leap_state->orientation, orientations, orientations);
}

static __forceinline real_point3d *biped_leap_point_add3(const real_point3d *p, float a, float b, float c, real_point3d *result)
{
    result->x = p->x + a;
    result->y = p->y + b;
    result->z = p->z + c;
    return result;
}

const float biped_leap_flight_path_scale = 1.0f;
const float biped_leap_departure_power = 3.0f;
const float biped_leap_departure_scale = 0.6f;
const float biped_leap_arrival_power = 3.0f;
const float biped_leap_arrival_scale = 1.0f;
const float biped_leap_twist_power = 0.5f;

// @address 0x79d7dc k_mixing_board_slider_targets

// @rdata 0x820221fc
static const float biped_leap_animation_one = 1.0f;
// @rdata 0x82022dc0
static const float biped_leap_animation_half = 0.5f;
// @rdata 0x820242d8
static const float biped_leap_animation_two = 2.0f;

// @address 0x38daf8 ?leap_animation_update@@YAXJPAUs_animation_control_data@@@Z
void leap_animation_update(long object_index, s_animation_control_data *control)
{
    biped_datum *unit = (biped_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
    c_biped_leap_state *state = (c_biped_leap_state *)((char *)unit + unit->unit.motor_program.offset);
    if (state->active)
    {
        real_matrix4x3 world;
        object_get_world_matrix(object_index, &world);
        control->mode = 188;
        control->state = 869;
        control->secondary_state = 870;
        control->secondary_mode = 12;
        float cannonball = state->flight_ratio - biped_leap_animation_half;
        cannonball *= biped_leap_animation_two;
        cannonball = (float)fabs(cannonball);
        cannonball = biped_leap_animation_one - cannonball;
        cannonball = (float)pow(cannonball, 0.333f);
        cannonball = PIN(cannonball * 0.4f, 0.0f, biped_leap_animation_one);
        float idle = state->flight_ratio - biped_leap_animation_half;
        idle *= biped_leap_animation_two;
        idle = PIN(idle, 0.0f, biped_leap_animation_one);
        idle = (float)pow(idle, 3.0f);
        idle = PIN(idle * biped_leap_animation_half, 0.0f, biped_leap_animation_one);
        control->primary_weight = cannonball * (biped_leap_animation_one - idle);
        control->secondary_weight = idle;
        const cache_file_tag_instance *tag_instances = global_tag_instances;
        float offset = ((biped_definition *)tag_instances[datum_index_to_absolute_index(unit->object.definition_index)].definition)->physics.radius;
        real_point3d top;
        point_from_line3d(&world.position, &world.up, offset, &top);
        real_matrix4x3 start = state->start_matrix;
        real_matrix4x3 end = state->end_matrix;
        point_from_line3d(&start.position, &start.up, -offset, &start.position);
        point_from_line3d(&end.position, &end.up, -offset, &end.position);
        real_vector3d direction;
        if (state->flight_ratio < biped_leap_animation_half)
            vector_from_points3d(&top, &start.position, &direction);
        else
            vector_from_points3d(&top, &end.position, &direction);
        float distance = normalize3d(&direction);
        float scale = 1.0f;
        if (offset > 0.0001f)
            scale = PIN(distance, 0.0f, offset) / offset;
        unit->unit.looking_vector = direction;
        unit->unit.desired_looking_vector = unit->unit.looking_vector;
        unit->unit.aiming_vector = direction;
        unit->unit.desired_aiming_vector = unit->unit.aiming_vector;
        unit->unit.facing_vector = unit->object.forward;
        float overlay = state->flight_ratio - biped_leap_animation_half;
        overlay *= biped_leap_animation_two;
        overlay = (float)fabs(overlay);
        overlay = (float)pow(overlay, 2.0f);
        overlay = PIN(overlay * scale, 0.0f, biped_leap_animation_one);
        c_animation_manager *animation = (c_animation_manager *)((char *)unit + unit->object.animation.offset);
        c_mixing_board_slider *slider = animation->get_mixing_board_slider(0);
        c_mixing_board_slider *overlay_slider = animation->get_mixing_board_slider(3);
        overlay_slider->snap_to_target(_slider_target_position_one);
        slider->snap_to_position(overlay);
    }
}

static __forceinline void biped_leap_quaternions_interpolate_and_normalize(
    const real_quaternion *a, const real_quaternion *b, float t, real_quaternion *result)
{
    quaternions_interpolate(a, b, t, result);
    quaternion_normalize(result);
}

static __forceinline float biped_leap_pow(float x, float y)
{
    return (float)pow(x, y);
}

extern "C" bool biped_leap_update_flight_orientation(long biped_index)
{
    biped_datum *biped = (biped_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(biped_index)].datum;
    const cache_file_tag_instance *tag_instances = global_tag_instances;
    biped_definition *definition = ((biped_definition *)tag_instances[datum_index_to_absolute_index(biped->object.definition_index)].definition);
    c_biped_leap_state *leap = (c_biped_leap_state *)((char *)biped + biped->unit.motor_program.offset);
    if (leap->active)
    {
        real_matrix4x3 world_matrix;
        object_get_world_matrix(biped_index, &world_matrix);
        float leap_scale = definition->physics.radius;
        float completion = compute_biped_leap_completion_ratio(leap, &world_matrix, leap_scale);
        float flight_ratio = PIN(completion * biped_leap_flight_path_scale, 0.0f, 1.0f);
        leap->flight_ratio = flight_ratio;
        leap->twist = 0.0f;
        leap->arrival = 0.0f;
        leap->departure = PIN(biped_leap_departure_scale, 0.0f, 1.0f);
        if (flight_ratio > 0.0f)
        {
            float t = (float)fabs((flight_ratio - 0.5f) * 2.0f);
            t = PIN(t, 0.0f, 1.0f);
            if (flight_ratio < 0.5f)
            {
                leap->arrival = 0.0f;
                leap->departure = biped_leap_pow(t, biped_leap_departure_power);
                leap->departure = PIN(leap->departure * biped_leap_departure_scale, 0.0f, 1.0f);
            }
            else
            {
                leap->departure = 0.0f;
                leap->arrival = biped_leap_pow(t, biped_leap_arrival_power);
                leap->arrival = PIN(leap->arrival * biped_leap_arrival_scale, 0.0f, 1.0f);
            }
            leap->twist = biped_leap_pow(flight_ratio, biped_leap_twist_power);
        }
        real_quaternion rotation = global_identity_orientation->rotation;
        if (leap->interpolate_long)
            quaternions_interpolate_long_and_normalize(&leap->flight_start_rotation, &leap->flight_end_rotation, leap->twist, &rotation);
        else
            biped_leap_quaternions_interpolate_and_normalize(&leap->flight_start_rotation, &leap->flight_end_rotation, leap->twist, &rotation);
        if (leap->departure > 0.0f)
            biped_leap_quaternions_interpolate_and_normalize(&rotation, &leap->start_rotation, leap->departure, &rotation);
        if (leap->arrival > 0.0f)
            biped_leap_quaternions_interpolate_and_normalize(&rotation, &leap->end_rotation, leap->arrival, &rotation);
        real_matrix4x3 inverse;
        matrix4x3_inverse(&world_matrix, &inverse);
        real_matrix4x3 local;
        matrix3x3_rotation_from_quaternion((real_matrix3x3 *)&local.forward, &rotation);
        point_from_line3d(&world_matrix.position, &world_matrix.up, leap_scale, &local.position);
        local.scale = world_matrix.scale;
        matrix4x3_compute_product(&inverse, &local, &world_matrix);
        matrix4x3_to_orientation(&world_matrix, &leap->orientation);
    }
    return true;
}

void leaping_biped_ground_plane_fix_transform_to_physics(long object_index, real_matrix4x3 *matrix)
{
    biped_datum *unit = (biped_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
    if (unit->object.havok_component_index == -1) return;
    if (unit->biped.physics.m_mode != c_character_physics_component::_mode_ground) return;
    float offset = ((biped_definition *)global_tag_instances[datum_index_to_absolute_index(unit->object.definition_index)].definition)->physics.radius;
    project_world_matrix_to_up_vector(&global_identity4x3->up, matrix, matrix);
    point_from_line3d(&matrix->position, &unit->object.up, offset, &matrix->position);
    float back = -offset;
    biped_leap_point_add3(&matrix->position, back * 0.0f, back * 0.0f, back, &matrix->position);
}

void leaping_biped_ground_plane_fix_transform_from_physics(long object_index, real_matrix4x3 *matrix)
{
    biped_datum *unit = (biped_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
    if (unit->object.havok_component_index == -1) return;
    if (unit->biped.physics.m_mode != c_character_physics_component::_mode_ground) return;
    const cache_file_tag_instance *tag_instances = global_tag_instances;
    biped_definition *definition = ((biped_definition *)tag_instances[datum_index_to_absolute_index(unit->object.definition_index)].definition);
    float offset = definition->physics.radius;
    real_vector3d *up = &unit->object.up;
    project_world_matrix_to_up_vector(up, matrix, matrix);
    float back = -offset;
    float *pos_x = &matrix->position.x;
    float *pos_y = &matrix->position.y;
    float *pos_z = &matrix->position.z;
    *pos_x = offset * 0.0f + *pos_x;
    *pos_y = *pos_y + offset * 0.0f;
    *pos_z = *pos_z + offset;
    point_from_line3d(&matrix->position, up, back, &matrix->position);
}

// @address 0x38d000 ?leap_execute@@YA_NJPAVc_biped_leap_state@@@Z
bool leap_execute(long object_index, c_biped_leap_state *leap_state)
{
    real_vector3d velocity = leap_state->velocity;
    real_point3d center_of_mass;
    object_get_center_of_mass(object_index, &center_of_mass);
    real_vector3d linear_velocity;
    real_vector3d angular_velocity;
    float z, y, x;
    if (object_get_early_mover_local_space_velocity(object_index, &center_of_mass,
            &linear_velocity, &angular_velocity, false, true))
    {
        velocity.i -= linear_velocity.i;
        velocity.j -= linear_velocity.j;
        velocity.k -= linear_velocity.k;
        z = velocity.k;
        y = velocity.j;
        x = velocity.i;
    }
    else
    {
        z = velocity.k;
        y = velocity.j;
        x = velocity.i;
    }
    object_set_velocities_internal(object_index, &velocity, 0, false);
    havok_object_set_velocities(object_index, &velocity, 0, true);
    const float &vertical_speed_squared = z * z;
    if (!((x * x + vertical_speed_squared) + y * y <= 9.999999e-09f))
    {
        object_set_at_rest(object_index, false);
        object_wake(object_index);
        object_set_requires_motion(object_index);
    }
    return true;
}

static __forceinline real_vector3d *biped_leap_cross_product3d_first(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    float i = a->j * b->k - a->k * b->j;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}

static __forceinline real_vector3d *biped_leap_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float i = a->j * b->k - a->k * b->j;
    float j = a->k * b->i - a->i * b->k;
    float k = a->i * b->j - a->j * b->i;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}

// @address 0x38d130 ?biped_leap_submit@@YA?AW4e_motor_result@@JPBVc_motor_request@@@Z
e_motor_result biped_leap_submit(long object_index, const c_motor_request *motor_request)
{
    e_motor_result result = _motor_result_none;
    biped_datum *biped = (biped_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
    c_motor_program_state *motor_program_state = (c_motor_program_state *)((char *)biped + biped->unit.motor_program.offset);
    if (biped->object.havok_component_index != -1 && biped->biped.physics.m_mode == c_character_physics_component::_mode_ground
        && motor_program_state->set_program(object_index, _motor_program_leap))
    {
        biped_definition *definition = (biped_definition *)global_tag_instances[datum_index_to_absolute_index(biped->object.definition_index)].definition;
        c_biped_leap_state *leap_state = (c_biped_leap_state *)((char *)biped + biped->unit.motor_program.offset);
        real_matrix4x3 world_matrix;
        object_get_world_matrix(object_index, &world_matrix);
        real_matrix4x3 target_matrix;
        {
            real_vector3d forward;
            real_vector3d delta;
            vector_from_points3d(&world_matrix.position, &motor_request->m_land.point, &delta);
            if (normalize3d(biped_leap_cross_product3d_first(&motor_request->m_land.vector, &delta, &forward)) == 0.0f)
            {
                if (normalize3d(biped_leap_cross_product3d(&motor_request->m_land.vector, &world_matrix.forward, &forward)) == 0.0f)
                {
                    normalize3d(biped_leap_cross_product3d(&motor_request->m_land.vector, &world_matrix.left, &forward));
                }
            }
            real_vector3d left;
            biped_leap_cross_product3d(&forward, &motor_request->m_land.vector, &left);
            matrix4x3_rotation_from_vectors(&target_matrix, &left, &motor_request->m_land.vector);
        }
        target_matrix.position = motor_request->m_land.point;
        leap_state->active = false;
        real_vector3d velocity;
        float speed;
        if (leap_build_trajectory(object_index, &target_matrix.position, &target_matrix.up, 0, &velocity, &speed, 0, 0))
        {
            leap_state->interpolate_long = false;
            leap_state->active = true;
            float leap_scale = definition->physics.radius;
            point_from_line3d(&world_matrix.position, &world_matrix.up, leap_scale, &world_matrix.position);
            point_from_line3d(&target_matrix.position, &target_matrix.up, leap_scale, &target_matrix.position);
            normalize3d(&velocity);
            scale_vector3d(&velocity, speed, &leap_state->velocity);
            real_vector3d direction;
            vector_from_points3d(&world_matrix.position, &target_matrix.position, &direction);
            normalize3d(&direction);
            real_matrix3x3 departure_axes;
            real_vector3d_build_axes_from_vectors(&direction, &world_matrix.up, &departure_axes.forward, &departure_axes.left, &departure_axes.up);
            real_matrix3x3 arrival_axes;
            real_vector3d_build_axes_from_vectors(&direction, &target_matrix.forward, &arrival_axes.forward, &arrival_axes.left, &arrival_axes.up);
            if (dot_product3d(&arrival_axes.up, &target_matrix.up) < -0.25f)
            {
                real_vector3d_build_axes_from_vectors(&direction, &target_matrix.up, &arrival_axes.forward, &arrival_axes.left, &arrival_axes.up);
                target_matrix.rotation = arrival_axes;
                project_world_matrix_to_up_vector(&motor_request->m_land.vector, &target_matrix, &target_matrix);
            }
            leap_state->start_matrix = world_matrix;
            leap_state->end_matrix = target_matrix;
            matrix3x3_rotation_to_quaternion(&target_matrix.rotation, &leap_state->end_rotation);
            matrix3x3_rotation_to_quaternion(&world_matrix.rotation, &leap_state->start_rotation);
            matrix3x3_rotation_to_quaternion(&departure_axes, &leap_state->flight_start_rotation);
            matrix3x3_rotation_to_quaternion(&arrival_axes, &leap_state->flight_end_rotation);
            real_quaternion midpoint;
            biped_leap_quaternions_interpolate_and_normalize(&leap_state->flight_start_rotation, &leap_state->flight_end_rotation, 0.5f, &midpoint);
            real_matrix3x3 midpoint_matrix;
            matrix3x3_rotation_from_quaternion(&midpoint_matrix, &midpoint);
            if (dot_product3d(&midpoint_matrix.up, global_up3d) < 0.0f
                && angle_between_vectors3d(&departure_axes.up, &arrival_axes.up) > 2.5132742f)
            {
                real_quaternion long_midpoint;
                quaternions_interpolate_long_and_normalize(&leap_state->flight_start_rotation, &leap_state->flight_end_rotation, 0.5f, &long_midpoint);
                real_matrix3x3 long_midpoint_matrix;
                matrix3x3_rotation_from_quaternion(&long_midpoint_matrix, &long_midpoint);
                if (dot_product3d(&long_midpoint_matrix.up, global_up3d) > 0.0f)
                    leap_state->interpolate_long = true;
            }
            leap_execute(object_index, leap_state);
            result = _motor_result_pending;
        }
        else
        {
            motor_program_state->set_program(object_index, _motor_program_airborne);
        }
    }
    return result;
}

typedef char leap_exit_interpolator_offset[(offsetof(c_animation_manager, m_interpolator) == 0x68) ? 1 : -1];
typedef char leap_exit_flags_offset[(offsetof(c_animation_manager, m_interpolator.m_flags) == 0x72) ? 1 : -1];

void handle_leap_motor_program_exit(long object_index, const real_vector3d *up, float interpolation_time)
{
    biped_datum *unit = (biped_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
    c_biped_leap_state *state = (c_biped_leap_state *)((char *)unit + unit->unit.motor_program.offset);
    real_matrix4x3 world;
    ::object_get_world_matrix(object_index, (::real_matrix4x3 *)&world);
    real_orientation orientation;
    matrix4x3_to_orientation(&world, &orientation);
    orientations_multiply(&orientation, &state->orientation, &orientation);
    real_matrix4x3 transformed;
    matrix3x3_rotation_from_quaternion((real_matrix3x3 *)&transformed.forward, &orientation.rotation);
    transformed.scale = orientation.scale;
    transformed.position = orientation.translation;
    real_matrix4x3 ground;
    project_world_matrix_to_up_vector(up, &transformed, &ground);
    const cache_file_tag_instance *tag_instances = global_tag_instances;
    float offset = ((biped_definition *)tag_instances[datum_index_to_absolute_index(unit->object.definition_index)].definition)->physics.radius;
    float back = -offset;
    ground.position.x = up->i * back + (world.up.i * offset + world.position.x);
    ground.position.y = up->j * back + (world.up.j * offset + world.position.y);
    ground.position.z = up->k * back + (world.up.k * offset + world.position.z);
    real_orientation ground_orientation;
    matrix4x3_to_orientation(&ground, &ground_orientation);
    real_orientation inverse;
    orientation_inverse(&ground_orientation, &inverse);
    orientations_multiply(&inverse, &orientation, &state->orientation);
    object_set_position_direct(object_index, &ground.position, &ground.forward, &ground.up, 0, false);
    unit->unit.looking_vector = unit->object.forward;
    unit->unit.desired_looking_vector = unit->object.forward;
    unit->unit.aiming_vector = unit->object.forward;
    unit->unit.desired_aiming_vector = unit->object.forward;
    unit->unit.facing_vector = unit->object.forward;
    biped_datum *current = (biped_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
    c_animation_manager *animation = (c_animation_manager *)((char *)current + current->object.animation.offset);
    animation->m_interpolator.m_time = 0.0f;
    animation->m_interpolator.m_flags = 0;
    animation->m_interpolator.m_duration = 0.0f;
    object_wake(object_index);
    object_start_interpolation(object_index, interpolation_time);
    state->orientation = *global_identity_orientation;
}

// Runtime leap alignment threshold.
// @address 0x79df90 g_biped_leap_alignment_threshold
float g_biped_leap_alignment_threshold = 0.70710678f;

// @address 0x38e2e0 ?biped_leap_move@@YA?AW4e_motor_program@@JPAVc_motor_request@@@Z
e_motor_program biped_leap_move(long object_index, class c_motor_request *)
{
    e_motor_program result = _motor_program_none;
    biped_datum *unit = (biped_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
    c_biped_leap_state *state = (c_biped_leap_state *)((char *)unit + unit->unit.motor_program.offset);
    if (!state->active)
    {
        handle_leap_motor_program_exit(object_index, global_up3d, 0.26666668f);
        return _motor_program_airborne;
    }
    bool aligned = false;
    bool valid = true;
    real_vector3d offset;
    real_vector3d velocities;
    action_request request;
    real_matrix4x3 product;
    real_matrix4x3 orientation_matrix;
    real_matrix4x3 world;
    object_get_velocities(object_index, &velocities, 0);
    s_character_physics_move_output_datum move_output;
    bool moved = motor_system_biped_move(object_index, true, false, false, true, &move_output);
    object_get_world_matrix(object_index, &world);
    if (moved)
    {
        const cache_file_tag_instance *tag_instances = global_tag_instances;
        float max_distance =
            ((biped_definition *)tag_instances[datum_index_to_absolute_index(unit->object.definition_index)].definition)->physics.radius * 2.0f;
        matrix4x3_from_orientation(&orientation_matrix, &state->orientation);
        matrix4x3_compute_product(&world, &orientation_matrix, &product);
        vector_from_points3d(&product.position, &state->end_matrix.position, &offset);
        float distance = normalize3d(&offset);
        if ((move_output.flags & 1) == 0)
        {
            if (distance < max_distance &&
                ((*unit->biped.physics.get_mode_ground()->get_ground_plane()).i * product.up.i +
                    ((*unit->biped.physics.get_mode_ground()->get_ground_plane()).k * product.up.k +
                        (*unit->biped.physics.get_mode_ground()->get_ground_plane()).j * product.up.j)) > 0.0f)
                aligned = true;
            else
                valid = false;
        }
        if (!aligned && valid)
        {
            float threshold = g_biped_leap_alignment_threshold;
            real_vector3d direction = state->velocity;
            normalize3d(&direction);
            if (distance < max_distance)
            {
                float ratio = distance / max_distance;
                threshold = ratio * (threshold - 0.26f) + 0.26f;
            }
            if ((offset.i * direction.i + offset.j * direction.j + offset.k * direction.k) < threshold)
                valid = false;
        }
    }
    if (valid && moved)
    {
        if (aligned && unit->object.havok_component_index != -1 && unit->biped.physics.m_mode == c_character_physics_component::_mode_ground)
        {
            object_set_velocities(object_index, global_zero_vector3d, 0);
            result = _motor_program_ground;
            const real_vector3d *ground = unit->biped.physics.get_mode_ground()->get_ground_plane();
            if (ground->k <= 0.71f)
                result = _motor_program_climb;
            handle_leap_motor_program_exit(object_index, ground, 0.13333334f);
            memset(&request, 0, sizeof(request));
            request.type = 34;
            request.leap_recovery.velocity = state->velocity;
            request.leap_recovery.normal = *ground;
            action_submit(object_index, &request);
        }
    }
    else
    {
        handle_leap_motor_program_exit(object_index, global_up3d, 0.26666668f);
        result = _motor_program_airborne;
    }
    state->velocity = velocities;
    return result;
}
// @address 0x79df94 g_biped_leap_downward_velocity_threshold
float g_biped_leap_downward_velocity_threshold = 1.5707964f;

static __forceinline void biped_leap_trajectory_offset(float scale, real_point3d *point, const real_vector3d *up)
{
    point->x = point->x + scale * up->i;
    point->y = point->y + up->j * scale;
    point->z = point->z + up->k * scale;
}

// @address 0x38cdd8 ?leap_build_trajectory@@YA_NJPBTreal_point3d@@PBTreal_vector3d@@PBMPAT2@PAMPAF4@Z
bool leap_build_trajectory(long object_index, const real_point3d *point, const real_vector3d *vector, const float *unused_speed,
    real_vector3d *aim_output, float *speed_output, short *unused_ticks, float *unused_time)
{
    bool result = false;
    biped_datum *unit = (biped_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
    const cache_file_tag_instance *tag_instances = global_tag_instances;
    biped_definition *definition = ((biped_definition *)tag_instances[datum_index_to_absolute_index(unit->object.definition_index)].definition);
    float offset = definition->physics.radius;
    float gravity_scale = biped_get_scale(object_index);
    float speed;
    long actor_index = unit->unit.actor_index;
    if (actor_index != -1)
        speed = actor_get_max_jump_speed(actor_index, 0);
    else
        speed = definition->jump_velocity;
    real_point3d origin;
    object_get_origin(object_index, &origin);
    real_point3d target;
    point_from_line3d(point, vector, offset, &target);
    biped_leap_trajectory_offset(offset, &origin, &unit->object.up);
    float maximum_time_scale = definition->maximum_time_scale;
    if (vector->k < -g_biped_leap_downward_velocity_threshold)
        maximum_time_scale = 0.0f;
    float minimum_speed;
    float result_speed;
    float result_time;
    real_vector3d aim_vector;
    if (projectile_aim_ballistic(speed, gravity_scale, &origin, &target, &minimum_speed, &maximum_time_scale,
            0, false, &aim_vector, &result_speed, &result_time, 0, 0, 0)
        || projectile_aim_ballistic(minimum_speed + 0.1f, gravity_scale, &origin, &target, 0, 0, 0, false, &aim_vector, &result_speed, &result_time, 0, 0, 0))
        result = true;
    if (result)
    {
        if (aim_output)
            *aim_output = aim_vector;
        if (speed_output)
            *speed_output = result_speed;
    }
    return result;
}

// @address 0x79df98 g_biped_leap_maximum_interpolation_time
float g_biped_leap_maximum_interpolation_time = 0.06666667f;
static __forceinline object_datum *biped_leap_object_get(long object_index)
{
    return (object_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
}
static __forceinline long biped_leap_object_get_root_object(long object_index)
{
    while (object_index != -1)
    {
        object_datum *object = biped_leap_object_get(object_index);
        if (!(bool)((object->flags >> 26) & 1))
            break;
        object_index = object->parent_object_index;
    }
    return object_index;
}
// @address 0x38df10 ?biped_leap_update@@YA_NJPAUs_animation_control_data@@@Z
bool biped_leap_update(long object_index, s_animation_control_data *control_data)
{
    s_animation_control_data control;
    control.set_defaults();
    object_datum *object = biped_leap_object_get(object_index);
    bool result = false;
    if (object->havok_component_index != -1 && biped_leap_object_get_root_object(object->parent_object_index) == -1)
    {
        s_character_physics_update_input_datum input;
        s_character_physics_update_output_datum output;
        biped_leap_update_flight_orientation(object_index);
        if (motor_system_biped_physics_preupdate_moving(object_index, &input, &output, false, true, 0.0f, false, true))
        {
            leap_animation_update(object_index, &control);
            motor_system_biped_physics_postupdate_moving(object_index, &input, &output, &control.crouching);
        }
        result = true;
    }
    {
        s_animation_state_change_settings settings;
        s_state_changes_requested requested;
        settings.flags.clear();
        requested.goals.clear();
        requested.transition.clear();
        requested.primary.clear();
        motor_animation_build_state_change_settings_from_control_data(object_index, &control, &requested, &settings);
        if (settings.interpolation_time > g_biped_leap_maximum_interpolation_time)
            settings.interpolation_time = g_biped_leap_maximum_interpolation_time;
        result = (0 != motor_task_execute_state_change_settings(object_index, _motor_task_default, _action_none, &settings, 1, control.moved)) | result;
    }
    return result;
}

// @rdata 0x8202490c
static const float biped_leap_orientation_epsilon = 0.0001f;
// @rdata 0x82028fcc
static const float biped_leap_orientation_minimum_alignment = 0.98f;
static __forceinline float biped_leap_orientation_one() { return 1.0f; }
static __forceinline float biped_leap_orientation_dot2d(const real_vector3d *a, const real_vector3d *b)
{
    return a->j * b->j + a->i * b->i;
}
static __forceinline biped_datum *biped_leap_orientation_get(long index)
{
    return (biped_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(index)].datum;
}
static __forceinline bool biped_leap_orientation_has_nodes(long index)
{
    return biped_leap_orientation_get(index)->object.node_orientations.offset != -1;
}
static __forceinline void biped_leap_orientation_normalize2d_with_default(real_vector3d *vector, float magnitude, const real_vector3d *fallback, float zero)
{
    if (fabsf(magnitude) >= biped_leap_orientation_epsilon) {
        float inverse = biped_leap_orientation_one() / magnitude;
        vector->i *= inverse;
        vector->j *= inverse;
        if (magnitude != zero)
            return;
    }
    *vector = *fallback;
}
static __forceinline float biped_leap_orientation_cross2d(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->j - a->j * b->i;
}
static __forceinline real_vector3d *biped_leap_orientation_cross3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    result->i = a->j * b->k - a->k * b->j;
    result->j = a->k * b->i - a->i * b->k;
    result->k = a->i * b->j - a->j * b->i;
    return result;
}
static __forceinline float biped_leap_orientation_dot3d(const real_vector3d *a, const real_vector3d *b)
{
    float b_i = b->i;
    float a_i = a->i;
    return a_i * b_i + (a->k * b->k + a->j * b->j);
}
static __forceinline void biped_leap_orientation_rotate2d(real_vector2d *vector, float sine, float cosine)
{
    float rotated_x = vector->i * cosine - vector->j * sine;
    float rotated_y = vector->j * cosine + vector->i * sine;
    vector->i = rotated_x;
    vector->j = rotated_y;
}
static __forceinline void biped_leap_orientation_scale(const real_vector3d *vector, float scale, real_vector3d *result)
{
    float i = vector->i * scale;
    result->i = i;
    result->j = vector->j * scale;
    result->k = vector->k * scale;
}
static __forceinline void biped_leap_orientation_subtract(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    result->i = a->i - b->i;
    result->j = a->j - b->j;
    result->k = a->k - b->k;
}
// @address 0x3857a8 ?leap_orientation_update@@YA_NJPAUs_animation_control_data@@@Z
bool leap_orientation_update(long object_index, s_animation_control_data *)
{
    biped_datum *biped = biped_leap_orientation_get(object_index);
    biped_definition *definition = (biped_definition *)global_tag_instances[datum_index_to_absolute_index(biped->object.definition_index)].definition;
    real_vector3d desired = biped->unit.facing_vector;
    desired.k = 0.0f;
    float magnitude = sqrtf(desired.j * desired.j + desired.i * desired.i);
    biped_leap_orientation_normalize2d_with_default(&desired, magnitude, &biped->object.forward, 0.0f);
    bool clockwise = biped_leap_orientation_cross2d(&desired, &biped->object.forward) > 0.0f;
    real_vector3d forward = biped->object.forward;
    float turn_angle = TLS_GAME_TIME_GLOBALS->tick_length * definition->moving_turning_speed;
    float cosine = (float)cos(turn_angle);
    float sine = (float)sin(turn_angle);
    if (clockwise)
        sine = -sine;
    bool rotate_about_up = (definition->physics.flags & 8) != 0;
    float crossing;
    if (rotate_about_up) {
        rotate_vector_about_axis(&forward, &biped->object.up, sine, cosine);
        real_vector3d cross;
        biped_leap_orientation_cross3d(&desired, &forward, &cross);
        crossing = biped_leap_orientation_dot3d(&biped->object.up, &cross);
    } else {
        float rotated_x = forward.i * cosine - forward.j * sine;
        float rotated_y = forward.j * cosine + forward.i * sine;
        forward.i = rotated_x;
        forward.j = rotated_y;
        crossing = biped_leap_orientation_cross2d(&desired, &forward);
    }
    if ((clockwise && crossing < 0.0f) || (!clockwise && crossing > 0.0f)) {
        if (rotate_about_up) {
            real_vector3d right;
            biped_leap_orientation_cross3d(&biped->object.up, &desired, &right);
            if (normalize3d(&right) > 0.0f)
                biped_leap_orientation_cross3d(&right, &biped->object.up, &forward);
        } else {
            forward.i = desired.i;
            forward.j = desired.j;
            forward.k = 0.0f;
            biped->object.up = global_identity4x3->up;
        }
        normalize3d(&forward);
    }
    if (biped->unit.player_index != -1 && biped_leap_orientation_has_nodes(object_index)
        && biped_leap_orientation_dot2d(&biped->object.forward, &forward) < biped_leap_orientation_minimum_alignment) {
        real_matrix4x3 rotation;
        real_orientation orientation;
        matrix4x3_rotation_between_vectors(&rotation, &forward, &biped->object.forward);
        matrix4x3_to_orientation(&rotation, &orientation);
        short orientation_size = biped->object.original_node_orientations.size;
        short current_offset = biped->object.original_node_orientations.offset;
        short previous_offset = biped->object.node_orientations.offset;
        long orientation_count = (unsigned long)orientation_size >> 5;
        real_orientation *previous = (real_orientation *)((char *)biped + previous_offset);
        real_orientation *current = (real_orientation *)((char *)biped + current_offset);
        if (orientation_count > 0) {
            orientations_multiply(&orientation, previous, previous);
            orientations_multiply(&orientation, current, current);
        }
    }
    biped->object.forward = forward;
    float projection = biped_leap_orientation_dot3d(&biped->object.forward, &biped->object.up);
    if (fabsf(projection) >= biped_leap_orientation_epsilon) {
        real_vector3d component;
        biped_leap_orientation_scale(&biped->object.forward, projection, &component);
        biped_leap_orientation_subtract(&biped->object.up, &component, &biped->object.up);
        normalize3d(&biped->object.up);
    }
    return true;
}
