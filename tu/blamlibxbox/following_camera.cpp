// @address 0x761454 ?g_observer_default_field_of_view@@3MA
// blamlibxbox/following_camera.obj: from the records 0x1ecac8 0x295318 0x295a98 0x295d08 0x295fa8 0x296078
// @compiler xdk6130
#include "units/biped_types.h"
#include "units/biped_definitions.h"
// @flags /TP /O2 /Oc
#include "game/player_control_globals.h"
#include "game/game_globals_storage.h"
#include "units/vehicle_types.h"
#include "units/unit_types.h"
#include <math.h>
#include "math/matrix_math.h"
#include "objects/object_header.h"
#include "game/player_mapping_types.h"
#include "game/game.h"
#include "game/players.h"
#include "objects/objects.h"
#include "camera/following_camera.h"
#include "camera/observer.h"
#include "scenario/wind.h"
#include "units/vehicles.h"
#include "camera/camera_types.h"
#include "interface/chud/chud.h"
#include "tag_files/tag_groups.h"
#include "units/unit_camera.h"

#include "camera/camera_track.h"
#include "tag_files/tag_types.h"
#include "scenario/scenario.h"
#include "game/game_globals.h"
#include "game/player_control_types.h"
#include "game/player_control_camera.h"
#include "saved_games/saved_film_globals.h"
#include "saved_games/saved_film_timer_globals.h"
#include "units/vehicle_definitions.h"
#include "units/vehicle_alien_scout_definitions.h"
#include "motor/mover_inline.h"
#include "math/real_math_trigonometry.h"

extern "C" {
// @address 0x7939c4 k_following_camera_fov_min
float k_following_camera_fov_min = k_degrees_to_radians;
// @address 0x7939c8 k_following_camera_fov_max
float k_following_camera_fov_max = 150.0f * k_degrees_to_radians;
}

// @address 0x94e3b0 g_saved_film_timer_globals


static __forceinline float following_camera_arcsine(float value)
{
    float result = -1.0f;
    if (value > -1.0f)
    {
        if (value < 1.0f)
            result = value;
        else
            result = 1.0f;
    }
    return (float)asin((double)result);
}

// @address 0x1ecac8 following_camera_deterministic
// match: 0x1ecac8: pointer __restrict qualifier prevents alias reload
extern "C" void following_camera_deterministic(const real_matrix4x3 * matrix, real_euler_angles3d *__restrict angles)
{
    float pitch = -following_camera_arcsine(matrix->up.i);
    angles->pitch = pitch;
    float cosine = (float)cos((double)pitch);
    if (cosine > 0.0001)
    {
        float scale = 1.0f / cosine;
        angles->roll = (float)atan2((double)-(matrix->up.j * scale), (double)(matrix->up.k * scale));
        angles->yaw = (float)atan2((double)-(matrix->left.i * scale), (double)(matrix->forward.i * scale));
    }
    else
    {
        angles->roll = 0.0f;
        angles->yaw = (float)atan2((double)matrix->forward.j, (double)matrix->left.j);
    }
}

// @address 0x295a98 ?set_target@c_following_camera@@UAAXJ@Z
void c_following_camera::set_target(long target)
{
    m_target = target;
    m_spare_flag0 = 0;
    m_pitch_offset = 0.0f;
    m_last_unit_index = target;
    m_yaw_offset = 0.0f;
    m_crouching = 0;
    m_focus_scale = 1.0f;
    m_spare_flag1 = 0;
    m_spare_flags = 0;
    m_last_seat_index = -1;
    m_tight_camera_track = 0;
    m_acceleration_offset = *global_zero_vector3d;
}

// @address 0x295d08 ?camera_track_splut@@YAXPBUs_unit_camera@@MPATreal_vector3d@@1_N@Z
// match: 0x295d08: pointer __restrict qualifier prevents alias reload
void camera_track_splut(const s_unit_camera *camera, float time, real_vector3d *__restrict forward, real_vector3d *position, bool alternate)
{
    long track_index = -1;
    long index = alternate ? 1 : 0;
    if (camera->camera_tracks.count()) {
        long last = camera->camera_tracks.count() - 1;
        const s_unit_camera_track *reference = &camera->camera_tracks[index > last ? last : index];
        if (reference) track_index = reference->track.index;
    }
    if (track_index == -1) track_index = global_game_globals->camera.begin()->default_unit_camera_track.index;
    camera_track_definition *track = TAG_GET('trak', camera_track_definition, track_index);
    if (track && track->control_points.count() >= 4) {
        float step = 1.0f / (float)(track->control_points.count() - 1);
        short original = (short)((float)(track->control_points.count() - 1) * time);
        short first = original;
        while (first > 0 && !(first + 4 <= track->control_points.count() && first <= original - 1)) --first;
        if (forward) {
            real_point3d axis = {1.0f, 0.0f, 0.0f};
            real_point3d directions[4];
            for (long index = 0; index < 4; ++index) {
                real_quaternion orientation = track->control_points[first + index].orientation;
                quaternion_transform_point(&orientation, &axis, &directions[index]);
            }
            wind_interpolate_vector(forward->n, directions[0].n, directions[1].n, directions[2].n, directions[3].n, (float)first * step, step, time);
        }
        if (position) {
            wind_interpolate_vector(position->n, track->control_points[first].position.n, track->control_points[first + 1].position.n,
                track->control_points[first + 2].position.n, track->control_points[first + 3].position.n, (float)first *step, step, time);
        }
    } else {
        if (forward) *forward = *global_forward3d;
        if (position) *position = *global_zero_vector3d;
    }
}


// @address 0x295fa8 ?compute_t_from_pitch@@YAMPBUs_unit_camera@@M_N@Z
float compute_t_from_pitch(const s_unit_camera *camera, float pitch, bool tight)
{
    float t;
    float minimum = 0.0f;
    float maximum = 1.0f;
    do
    {
        t = (minimum + maximum) * 0.5f;
        real_vector3d forward;
        camera_track_splut(camera, t, &forward, 0, tight);
        real_euler_angles2d angles;
        float horizontal_y = forward.j;
        float horizontal_x = forward.i;
        angles.yaw = (float)atan2((double)horizontal_y, (double)horizontal_x);
        angles.pitch = (float)atan2((double)forward.k, (double)sqrtf(horizontal_x * horizontal_x + horizontal_y * horizontal_y));
        if (angles.pitch < pitch)
            minimum = t;
        else
            maximum = t;
    } while (maximum - minimum > 0.0001f);
    return t;
}


// @address 0x296078 ?following_camera_update_acceleration@@YAXMPATreal_vector3d@@PBT1@11PBUs_unit_camera_acceleration@@@Z
void following_camera_update_acceleration(float seconds, real_vector3d *result, const real_vector3d *facing,
    const real_vector3d *fallback, const real_vector3d *acceleration, const s_unit_camera_acceleration *settings)
{
    real_vector3d horizontal = *facing;
    horizontal.k = 0.0f;
    if (normalize3d(&horizontal) == 0.0f) {
        horizontal = *fallback;
        horizontal.k = 0.0f;
        if (normalize3d(&horizontal) == 0.0f) horizontal = *global_forward3d;
    }
    real_matrix4x3 matrix;
    matrix4x3_rotation_from_vectors(&matrix, &horizontal, global_up3d);
    real_vector3d local_facing, local_result, local_acceleration, adjusted;
    matrix4x3_inverse_transform_vector(&matrix, facing, &local_facing);
    matrix4x3_inverse_transform_vector(&matrix, result, &local_result);
    matrix4x3_inverse_transform_vector(&matrix, acceleration, &local_acceleration);
    float max_change = settings->rate * seconds;
    for (long i = 0; i < 3; ++i) {
        const s_unit_camera_acceleration_axis *axis = &settings->axes[i];
        float aligned_scale = axis->aligned_scale;
        float unaligned_scale = axis->unaligned_scale;
        float value = local_acceleration.n[i];
        float sign = value > 0.0f ? -1.0f : 1.0f;
        float desired = axis->magnitude * (float)pow(fabs(axis->scale * value), axis->exponent) * sign;
        float lower = desired > -axis->maximum ? desired : -axis->maximum;
        if (lower < axis->maximum) {
            if (desired <= -axis->maximum)
                desired = -axis->maximum;
        } else {
            desired = axis->maximum;
        }
        if (aligned_scale != 0.0f || unaligned_scale != 0.0f) {
            float alignment = ((local_facing.n[i] - 0.70710677f) * 3.4142134f) < 0.0f ? 0.0f : ((local_facing.n[i] - 0.70710677f) * 3.4142134f);
            desired *= (aligned_scale - unaligned_scale) * alignment + unaligned_scale;
        }
        float delta = desired - local_result.n[i];
        float lower_change = delta > -max_change ? delta : -max_change;
        if (lower_change < max_change) {
            if (delta <= -max_change)
                delta = -max_change;
        } else {
            delta = max_change;
        }
        adjusted.n[i] = local_result.n[i] + delta;
    }
    matrix4x3_transform_vector(&matrix, &adjusted, result);
}

static __forceinline s_player_control_state *following_camera_player_state(long user_index)
{
    return &((s_player_control_globals *)player_control_globals)->states[user_index];
}


static __forceinline bool following_camera_vehicle_has_fixed_camera_yaw(long vehicle_index)
{
    bool result = false;
    if (vehicle_get_type(vehicle_index) == 3)
    {
        unit_datum *vehicle = OBJECT_GET(vehicle_index, unit_datum, _object_mask_vehicle);
        const vehicle_type_alien_scout_handling *definition = (const vehicle_type_alien_scout_handling *)
            TAG_GET('vehi', vehicle_definition, vehicle->object.definition_index)->alien_scout.address;
        result = (definition->camera_flags & 1) != 0;
    }
    return result;
}

static __forceinline bool following_camera_unit_desires_tight_camera_track(long unit_index)
{
    bool tight = false;
    unit_datum *unit = (unit_datum *)object_try_and_get_and_verify_type(unit_index, 0x1003);
    if (unit != 0)
        tight = unit->unit.m_unit_flags.test(_unit_flag_prefer_tight_camera_track);
    return tight || game_is_cooperative();
}


static __forceinline float following_camera_sine(float angle)
{
    return (float)sin((double)angle);
}

// @address 0x295318 ?update@c_following_camera@@UAAXJMPAUs_observer_command@@@Z
void c_following_camera::update(long user_index, float seconds, s_observer_command *result)
{
    if (m_target == -1)
        return;
    long controller_unit = -1;
    if (user_index != -1)
        controller_unit = tls_player_mapping_globals()->unit_indices[user_index];
    s_unit_camera_info camera_info;
    real_euler_angles2d angles;
    real_vector3d forward;
    real_vector3d offset;
    if (m_target == controller_unit)
    {
        unit_get_camera_info(following_camera_player_state(user_index)->unit_index, &camera_info);
        angles = following_camera_player_state(user_index)->action.look;
        object_header_datum *header = object_header_get(m_target);
        if (header->object_type == 0)
        {
            biped_datum *biped = OBJECT_GET(m_target, biped_datum, _object_mask_unit);
            float scale =
                TAG_GET('bipd', biped_definition, biped->object.definition_index)->following_camera_scale;
            camera_info.position.x += biped->biped.following_camera_offset.x * scale;
            camera_info.position.y += biped->biped.following_camera_offset.y * scale;
            camera_info.position.z += biped->biped.following_camera_offset.z * scale;
        }
    }
    else
    {
        unit_get_camera_info(m_target, &camera_info);
        forward = OBJECT_GET(camera_info.unit_index, unit_datum, _object_mask_unit)->unit.aiming_vector;
        euler_angles2d_from_vector3d(&angles, &forward);
    }
    bool variable_up = false;
    bool fixed_yaw = false;
    result->field_of_view = g_observer_default_field_of_view;
    result->position = camera_info.position;
    result->timer = 0.0f;
    result->flags = 0;
    result->field_of_view = chud_get_cinematic_fov_scale() * result->field_of_view;
    result->field_of_view = PIN(result->field_of_view, k_following_camera_fov_min, k_following_camera_fov_max);
    result->flags |= 0x800;
    if (camera_info.unit_index != m_last_unit_index || camera_info.seat_index != m_last_seat_index)
    {
        float timer = result->timer;
        if (timer < 0.5f)
            timer = 0.5f;
        result->timer = timer;
    }
    m_last_seat_index = camera_info.seat_index;
    m_last_unit_index = camera_info.unit_index;
    if (camera_info.seat_index != -1)
    {
        long parent = m_target;
        long ultimate = -1;
        if (parent != -1)
        {
            do
            {
                ultimate = parent;
                parent = object_get(parent)->parent_object_index;
            } while (parent != -1);
        }
        result->ignore_object_indices[0] = ultimate;
        result->ignore_object_count = 1;
        result->flags |= 0x20;
    }
    if (camera_info.camera)
    {
        unit_datum *unit = OBJECT_GET(camera_info.unit_index, unit_datum, _object_mask_unit);
        if (mover_has_variable_camera_up(camera_info.unit_index))
            variable_up = true;
        if (TEST_BIT(1 << object_header_get(camera_info.unit_index)->object_type, 1))
            fixed_yaw = following_camera_vehicle_has_fixed_camera_yaw(camera_info.unit_index);
        bool crouching = (unit->unit.control_flags & 1) != 0 || (unit->unit.control_flags & 2) != 0;
        if (crouching != m_crouching)
        {
            float value = result->parameter_timers[1];
            result->parameter_flags[1] = 1;
            if (value < 0.5f)
                value = 0.5f;
            result->parameter_timers[1] = value;
            m_crouching = crouching;
        }
        bool tight = following_camera_unit_desires_tight_camera_track(camera_info.unit_index);
        if (tight != m_tight_camera_track)
        {
            float timer = result->timer;
            float half = 0.5f;
            if (timer >= half)
                half = timer;
            result->timer = half;
            m_tight_camera_track = tight;
        }
        float pitch = PIN(m_pitch_offset + angles.pitch, -1.5707964f, 1.5707964f);
        float t = compute_t_from_pitch(camera_info.camera, pitch, tight);
        camera_track_splut(camera_info.camera, t, &forward, &offset, tight);
        angles.yaw = m_yaw_offset + angles.yaw;
        angles.pitch = pitch;
        if (variable_up)
            player_build_aiming_vector_from_facing(unit->unit.player_index, &angles, &result->forward);
        else
            vector3d_from_euler_angles2d(&result->forward, &angles);
        if (fixed_yaw)
        {
            real_euler_angles2d unit_angles;
            euler_angles2d_from_vector3d(&angles, &result->forward);
            euler_angles2d_from_vector3d(&unit_angles, &unit->object.forward);
            angles.yaw = unit_angles.yaw;
            vector3d_from_euler_angles2d(&result->forward, &angles);
        }
        result->focus_distance = sqrtf(offset.i * offset.i + offset.j * offset.j + offset.k * offset.k);
        result->focus_position.x = (result->focus_distance * cosine(pitch) + offset.i) * m_focus_scale;
        result->focus_position.y = -(m_focus_scale * offset.j);
        result->focus_position.z = (result->focus_distance * following_camera_sine(pitch) + offset.k) * m_focus_scale;
        object_get_velocities(camera_info.unit_index, &result->velocity, 0);
        result->flags |= 1;
        if (variable_up)
        {
            real_vector3d left;
            object_get_orientation(camera_info.unit_index, &forward, &offset);
            cross_product3d(&offset, &result->forward, &left);
            cross_product3d(&result->forward, &left, &result->up);
            normalize3d(&result->up);
            result->flags |= 0x200;
        }
        else
        {
            generate_up_vector3d(&result->forward, &result->up);
        }
    }
    if (camera_info.unit_index != -1 && camera_info.camera && camera_info.camera->camera_acceleration.count())
    {
        real_vector3d *offset_vector = &m_acceleration_offset;
        following_camera_update_acceleration(seconds, offset_vector, &result->forward, &result->up, &result->velocity,
            camera_info.camera->camera_acceleration.begin());
        point_from_line3d(&result->position, offset_vector, 1.0f, &result->position);
    }
    if (camera_info.unit_index != -1)
    {
        long object_index = camera_info.unit_index;
        long ultimate;
        do {
            ultimate = object_index;
            object_index = object_get(object_index)->parent_object_index;
        } while (object_index != -1);
        object_get_center_of_mass(ultimate, &result->parent_position);
        result->flags |= 0x400;
        if (game_is_cooperative() && tls_game_globals()->options.game_mode == 2 && g_saved_film_timer_globals.playback_speed == 1.0f)
        {
            if (m_target != -1 && ((1 << object_header_get(m_target)->object_type) & 0x1003) != 0 && !players_is_object_player_local(m_target))
            {
                result->parameter_flags[5] = 1;
                result->parameter_timers[5] = 0.05f;
                result->timer += 0.05f;
            }
        }
    }
    observer_validate_camera_command(result);
}
