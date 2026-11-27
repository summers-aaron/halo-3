// @address 0x8c42a0 ?g_camera_globals@@3Us_camera_globals@@A
// blamlibxbox/flying_camera.obj: from the records 0x2014c0 0x2969b8 0x2969c0 0x2969c8 0x2969f0 0x296a18 0x296b28 0x297628 0x45fc28
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "camera/debug_director_internals.h"
#include "camera/camera_globals.h"
#include "physics/collision_test_vector.h"
#include "math/matrix_math_types.h"
#include "memory/data_types.h"
#include "shell/shell.h"
#include <string.h>
#include <new>
#include <math.h>
#include "cseries/cseries_wrapped.h"

#include "scenario/scenario.h"
#include "camera/observer.h"
#include "math/periodic_functions.h"
#include "game/player_control.h"
#include "interface/user_interface_controller.h"
#include "camera/camera.h"
#include "objects/object_datum.h"
#include "physics/collision_types.h"
#include "physics/collision_feature_types.h"
#include "input/input_abstraction.h"
#include "input/input_abstraction_types.h"
#include "math/real_math.h"
#include "physics/collision_result_types.h"
#include "physics/collisions.h"
#include "camera/camera_types.h"
#include "camera/director.h"
#include "camera/saved_film_director_storage.h"
#include "camera/director_runtime_symbols.h"
#include "camera/director_globals_types.h"
#include "camera/flying_camera.h"
#include "memory/tls_globals.h"
#include "camera/observer_globals_types.h"
// @address 0x761454 ?g_observer_default_field_of_view@@3MA

#include "cseries/cseries.h"
// @address 0xc1a90 ?euler_angles2d_from_vector3d@@YAPATreal_euler_angles2d@@PAT1@PBTreal_vector3d@@@Z

#include "camera/following_camera.h"

#include "objects/object_type_access.h"

#include "camera/camera_mode_types.h"
#include "game/director_perspective_types.h"
#include "cseries/minimum.h"
#include "cseries/maximum.h"
struct s_observer_command;
// @address 0x86ce0 ?get_target@c_camera@@UBAJXZ
// @address 0x296b28 ?update@c_flying_camera@@UAAXJMPAUs_observer_command@@@Z
// @address 0x297d68 ?set_position@c_flying_camera@@UAAXPBTreal_point3d@@@Z

typedef char flying_camera_game_input_state_size[(sizeof(s_game_input_state) == 0x100) ? 1 : -1];
typedef char flying_camera_look_settings_size[(sizeof(s_controller_look_settings) == 0x34) ? 1 : -1];
typedef char flying_camera_input_globals_size[(sizeof(s_input_abstraction_globals) == 0x4dc) ? 1 : -1];
typedef char flying_camera_dircam_f72_offset[(offsetof(s_camera_globals, flying_camera_field_of_view) == 0x48) ? 1 : -1];
typedef char flying_camera_dircam_f100_offset[(offsetof(s_camera_globals, orbiting_camera_minimum_distance) == 0x64) ? 1 : -1];

// The camera a director holds until it selects a real camera mode.
class c_null_camera : public c_camera
{
public:
    virtual e_camera_mode get_type() const { return (e_camera_mode)NONE; }
    virtual e_director_perspective get_perspective() const { return _director_perspective_authored; }
    virtual void update(long user_index, float elapsed_time, s_observer_command *command) {}
};

extern "C"
{
// @address 0x744608 g_debug_camera_speed

// @address 0x7939dc g_flying_camera_min_field_of_view
float g_flying_camera_min_field_of_view = 0.017453292f;
// @address 0x7939e0 g_flying_camera_max_field_of_view
float g_flying_camera_max_field_of_view = 2.6179938f;
// @address 0x8c4298 g_flying_camera_editor_speed_index
long g_flying_camera_editor_speed_index;
}

// @rdata 0x820221fc
static const float k_flying_camera_one = 1.0f;
// @rdata 0x82024fd4
static const float k_flying_camera_radians_per_degree = 0.017453292f;
// @rdata 0x82032eb4..0x82032ec8
static const long k_flying_camera_editor_speeds[5] = { 1, 5, 20, 40, 100 };
// @rdata-end

static __forceinline void flying_camera_cheat_teleport_to_camera()
{
}

static __forceinline real_vector3d *flying_camera_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float i = a->j * b->k - a->k * b->j;
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = i;
    result->j = j;
    result->k = k;
    return result;
}

static __forceinline real_vector3d *flying_camera_rotate_vector_about_axis(real_vector3d *vector, const real_vector3d *axis, float sine, float cosine)
{
    float scale = (axis->i * vector->i + axis->j * vector->j + axis->k * vector->k) * (k_flying_camera_one - cosine);
    float cross_i = vector->j * axis->k - vector->k * axis->j;
    float cross_j = vector->k * axis->i - vector->i * axis->k;
    const float &cross_k = vector->i * axis->j - vector->j * axis->i;
    vector->i = axis->i * scale + vector->i * cosine - cross_i * sine;
    vector->j = axis->j * scale + vector->j * cosine - cross_j * sine;
    const float &scaled_axis_k = axis->k * scale;
    vector->k = scaled_axis_k + vector->k * cosine - cross_k * sine;
    return vector;
}

// @address 0x3fc598 ?get_type@c_flying_camera@@UBA?AW4e_camera_mode@@XZ
e_camera_mode c_flying_camera::get_type() const
{
    return _camera_mode_flying;
}

__forceinline float c_flying_camera::get_field_of_view() const
{
    return m_flags.test(_zoomed) ? g_camera_globals.flying_camera_field_of_view * k_flying_camera_radians_per_degree : g_observer_default_field_of_view;
}

static __forceinline void flying_camera_get_input_state(e_controller_index controller_index, s_game_input_state **input_state)
{
    *input_state = &input_abstraction_globals.input_states[controller_index];
}

static __forceinline void flying_camera_get_player_look_angular_velocity(e_controller_index controller_index, real_euler_angles2d *angular_velocity)
{
    angular_velocity->yaw = input_abstraction_globals.look_settings[controller_index].preferences.horizontal_look_sensitivity *
        k_flying_camera_radians_per_degree;
    angular_velocity->pitch = input_abstraction_globals.look_settings[controller_index].preferences.vertical_look_sensitivity *
        k_flying_camera_radians_per_degree;
}

// @address 0x296b28 ?update@c_flying_camera@@UAAXJMPAUs_observer_command@@@Z
void c_flying_camera::update(long user_index, float elapsed_time, s_observer_command *result)
{
    bool use_old_controls = player_control_is_old_controls_active() != 0;
    long controller_index = user_interface_controller_get_rumble_enabled(user_index);

    if (controller_index != -1 && !camera_responding_gate((e_controller_index)controller_index))
    {
        float movement_scale = k_flying_camera_one;
        s_game_input_state *input_state;
        flying_camera_get_input_state((e_controller_index)controller_index, &input_state);
        real_euler_angles2d angular_velocity;
        flying_camera_get_player_look_angular_velocity((e_controller_index)controller_index, &angular_velocity);
        s_controller_look_settings settings = input_abstraction_globals.look_settings[controller_index];

        if (m_flags.test(_orientation_enabled))
        {
            m_angles.yaw = input_state->look.i * (m_flags.test(_zoomed) ? g_camera_globals.flying_camera_look_scale : k_flying_camera_one) *
                    g_camera_globals.yaw_scale * angular_velocity.yaw * elapsed_time +
                m_angles.yaw;
            m_angles.pitch = input_state->look.j * (m_flags.test(_zoomed) ? g_camera_globals.flying_camera_look_scale : k_flying_camera_one) *
                    g_camera_globals.pitch_scale * angular_velocity.pitch * elapsed_time +
                m_angles.pitch;
            m_angles.pitch = PIN(m_angles.pitch, -1.4922565f, 1.4922565f);
        }

        if (!use_old_controls)
        {
            float amount = input_state->buttons[0].down_amount();
            if (amount != 0.0f)
            {
                float &boost_time = m_velocity[0];
                float boost = interpolate_linear(k_flying_camera_one, g_camera_globals.boost_scale, amount);
                if (g_camera_globals.boost_time != 0.0f && boost > k_flying_camera_one)
                {
                    float t = transition_function_evaluate(g_camera_globals.boost_function,
                        PIN(boost_time / g_camera_globals.boost_time, 0.0f, k_flying_camera_one));
                    boost = interpolate_linear(k_flying_camera_one, boost, t);
                }
                movement_scale *= MAX(settings.camera_boost_scale * boost, k_flying_camera_one);
                boost_time += elapsed_time;
            }
            else
            {
                m_velocity[0] = 0.0f;
            }

            if (director_get(user_index)->get_type() == 5)
                movement_scale *= (float)k_flying_camera_editor_speeds[g_flying_camera_editor_speed_index] * g_debug_camera_speed;
        }

        movement_scale *= g_debug_camera_speed;
        movement_scale *= settings.camera_speed;

        real_point3d position = m_position;
        real_vector3d forward;
        vector3d_from_euler_angles2d(&forward, &m_angles);
        real_vector3d left;
        flying_camera_cross_product3d(global_up3d, &forward, &left);
        normalize3d(&left);

        short scheme = settings.camera_control_scheme;
        if (scheme == 0)
        {
            m_flags.set(_lock_in_xy_plane_held, input_state->buttons[8].down_amount() == k_flying_camera_one);
        }
        else if (scheme == 1)
        {
            if (input_state->buttons[8].down_frames() == 1)
            {
                if (m_flags.test(_lock_in_xy_plane_held))
                    m_flags.set(_lock_in_xy_plane_held, false);
                else
                    m_flags.set(_lock_in_xy_plane_held, true);
            }
        }
        else
        {
            m_flags.set(_lock_in_xy_plane_held, false);
        }

        if (m_flags.test(_lock_in_xy_plane) || use_old_controls || m_flags.test(_lock_in_xy_plane_held))
        {
            forward.k = 0.0f;
            normalize3d(&forward);
        }

        if (m_flags.test(_movement_enabled))
        {
            point_from_line3d(&position, &forward, input_state->throttle.i * g_camera_globals.forward_scale * movement_scale * elapsed_time, &position);
            point_from_line3d(&position, &left, input_state->throttle.j * g_camera_globals.side_scale * movement_scale * elapsed_time, &position);

            float vertical;
            if (use_old_controls)
                vertical = input_state->buttons[1].down_amount() - input_state->buttons[0].down_amount();
            else
                vertical = input_state->buttons[15].down_amount() - input_state->buttons[14].down_amount();
            vertical = g_camera_globals.up_scale * vertical;

            if (!use_old_controls)
            {
                if (vertical != 0.0f && g_camera_globals.vertical_boost_time > 0.0f)
                {
                    float t = transition_function_evaluate(g_camera_globals.vertical_boost_function,
                        PIN(m_velocity[1] / g_camera_globals.vertical_boost_time, 0.0f, k_flying_camera_one));
                    float boost = interpolate_linear(k_flying_camera_one, g_camera_globals.up_scale, t);
                    long sign = vertical < 0.0f ? -1 : 1;
                    boost = MAX(boost, k_flying_camera_one);
                    m_velocity[1] += elapsed_time;
                    vertical = (float)sign * boost;
                }
                else
                {
                    m_velocity[1] = 0.0f;
                }
                vector3d_from_euler_angles2d(&result->forward, &m_angles);
                generate_up_vector3d(&result->forward, &result->up);
                float up_amount = vertical * movement_scale * elapsed_time;
                point_from_line3d(&position, &result->up, up_amount, &position);
            }
            else
            {
                float up_amount = vertical * movement_scale * elapsed_time;
                const float &horizontal_i = global_up3d->i * up_amount;
                const float &horizontal_j = global_up3d->j * up_amount;
                position.x += horizontal_i;
                position.y += horizontal_j;
                position.z += global_up3d->k * up_amount;
            }
        }

        if (use_old_controls)
        {
            if (input_state->buttons[2].down_frames() != 0)
                g_debug_camera_speed *= 1.03f;
            else if (input_state->buttons[3].down_frames() != 0)
                g_debug_camera_speed *= 0.97087383f;
        }

        if (input_state->buttons[9].down_frames() == 1)
        {
            m_flags.toggle(_zoomed);
            result->parameter_timers[4] = g_camera_globals.flying_camera_parameter_time;
            result->parameter_flags[4] = 1;
        }

        if (m_flags.test(_collision_enabled))
        {
            real_vector3d delta;
            vector_from_points3d(&m_position, &position, &delta);
            s_collision_test_flags test_flags;
            const s_collision_test_flags *base_flags = &k_collision_test_structure_and_water;
            test_flags.collision_flags = base_flags->collision_flags | 0x100004;
            test_flags.object_flags = base_flags->object_flags | 1;
            float distance = (float)fabs((double)sqrtf(delta.i * delta.i + (delta.j * delta.j + delta.k * delta.k)));
            if (distance < 0.0001f)
            {
                s_collision_test_flags point_flags;
                point_flags.collision_flags = 0x100001;
                point_flags.object_flags = 0x7fff;
                e_collision_result_type result_type;
                collision_test_point(point_flags, &m_position, -1, -1, &result_type);
                if ((long)result_type == 4)
                {
                    real_point3d sample = m_position;
                    scale_vector3d(&result->forward, -0.08f, &forward);
                    bool found = false;
                    for (long index = 0; index < 5 && !found; index++)
                    {
                        point_from_line3d(&m_position, &forward, k_flying_camera_one - (float)index * 0.2f, &sample);
                        s_location location;
                        scenario_location_from_point(&location, &sample);
                        found = location.leaf_index != -1;
                    }
                    if (found)
                    {
                        position = sample;
                        m_position = sample;
                    }
                }
            }
            collision_plane planes[3];
            collision_move_point_with_features(test_flags, &m_position, &delta, 0.0f, g_camera_globals.collision_radius,
                -1, -1, &position, &delta, 3, planes);
        }

        if (director_get(user_index)->get_type() == 3 && input_state->buttons[10].down_frames() == 1)
        {
            collision_result collision;
            real_vector3d ray;
            vector3d_from_euler_angles2d(&ray, &m_angles);
            scale_vector3d(&ray, 2000.0f, &ray);
            if (collision_test_vector(k_collision_test_camera, false, &position, &ray, -1, -1, -1, &collision) && valid_world_real_point3d(&collision.point))
            {
                result->timer = MAX(k_flying_camera_one, collision.fraction * 50.0f);
                position.x = collision.point.x - ray.i * 0.0005f;
                position.y = collision.point.y - ray.j * 0.0005f;
                position.z = collision.point.z - ray.k * 0.0005f;
            }
        }
        if (director_get(user_index)->get_type() == 3 && input_state->buttons[11].down_frames() >= 1)
            flying_camera_cheat_teleport_to_camera();

        m_position = position;
    }

    vector3d_from_euler_angles2d(&result->forward, &m_angles);
    generate_up_vector3d(&result->forward, &result->up);
    flying_camera_rotate_vector_about_axis(&result->up, &result->forward, (float)sin(m_roll), (float)cos(m_roll));

    if (use_old_controls && g_camera_globals.flying_camera_transition_time == 0.0f)
        result->timer = MAX(result->timer, 0.3f);
    else
        result->timer = MAX(result->timer, g_camera_globals.flying_camera_transition_time);

    result->position = m_position;
    result->focus_position = *global_origin3d;
    result->focus_distance = 0.0f;
    float field_of_view = get_field_of_view();
    float field_of_view_scale = g_camera_globals.field_of_view_scale;
    field_of_view *= field_of_view_scale;
    result->field_of_view = minimum(maximum(field_of_view, g_flying_camera_min_field_of_view), g_flying_camera_max_field_of_view);
    result->flags = 0x801;
    observer_validate_camera_command(result);
}

// @address 0x2969b8 ?set_forward@c_flying_camera@@UAAXPBTreal_vector3d@@@Z
void c_flying_camera::set_forward(const real_vector3d *forward)
{
    euler_angles2d_from_vector3d(&m_angles, forward);
}

// @address 0x2969c0 ?set_roll@c_flying_camera@@UAAXM@Z
void c_flying_camera::set_roll(float roll)
{
    m_roll = roll;
}

// @address 0x2969c8 ?enable_orientation@c_flying_camera@@UAAX_N@Z
void c_flying_camera::enable_orientation(bool enable)
{
    m_flags.set(_orientation_enabled, enable);
}

// @address 0x2969f0 ?enable_movement@c_flying_camera@@UAAX_N@Z
void c_flying_camera::enable_movement(bool enable)
{
    m_flags.set(_movement_enabled, enable);
}

// @address 0x296a18 ??0c_flying_camera@@QAA@J@Z

// @address 0x88cd0 ?observer_try_and_get_camera@@YAPBUs_observer_result@@J@Z

c_flying_camera::c_flying_camera(long user_index)
{
    m_camera_flags.clear();
    m_target = -1;
    m_camera_state = 0;
    const s_observer_result *camera = observer_try_and_get_camera(user_index);
    if (!camera)
    {
        m_position.z = 0.0f;
        m_position.y = 0.0f;
        m_position.x = 0.0f;
        m_angles.pitch = 0.0f;
        m_angles.yaw = 0.0f;
    }
    else
    {
        m_position = camera->position;
        euler_angles2d_from_vector3d(&m_angles, &((const s_observer_camera_result *)camera)->forward);
    }
    m_roll = 0.0f;
    m_flags.clear();
    m_flags.set(_zoomed, false);
    m_flags.set(_lock_in_xy_plane, false);
    m_flags.set(_collision_enabled, true);
    m_flags.set(_orientation_enabled, true);
    m_flags.set(_movement_enabled, true);
    m_flags.set(_lock_in_xy_plane_held, false);
    m_velocity[0] = 0.0f;
    m_velocity[1] = 0.0f;
}

// @address 0x297628 ?set_target@c_flying_camera@@UAAXJ@Z
void c_flying_camera::set_target(long target)
{
    m_target = target;
    if (target != -1 && ((1 << object_get_type(target)) & 0x1003) != 0)
    {
        real_vector3d forward;
        following_camera_deterministic(target, &m_position, &forward);
        euler_angles2d_from_vector3d(&m_angles, &forward);
    }
}

// @address 0x45fc28 ?get_perspective@c_flying_camera@@UBA?AW4e_director_perspective@@XZ
e_director_perspective c_flying_camera::get_perspective() const
{
    return _director_perspective_authored;
}

// @address 0x3dcdc ??_7c_null_camera@@6B@
// @address 0x1c1258 ?get_type@c_null_camera@@UBA?AW4e_camera_mode@@XZ
// @address 0x45fc28 ?get_perspective@c_null_camera@@UBA?AW4e_director_perspective@@XZ
// @address 0x5bec98 ?update@c_null_camera@@UAAXJMPAUs_observer_command@@@Z
// @address 0x3dca8 ??_7c_director@@6B@
// @address 0x58dcf0 _purecall

// @address 0x2014c0 ??0c_director@@QAA@J@Z
c_director::c_director(long user_index)
{
    memset(&stored_command, 0, sizeof(stored_command));
    m_user_index = user_index;
    blend = 0.0f;
    watched_player_index = -1;
    scoreboard_animation_started = false;
    new (get_camera()) c_null_camera();
}
