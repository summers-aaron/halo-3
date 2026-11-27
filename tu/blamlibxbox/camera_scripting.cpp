// @address 0x761454 ?g_observer_default_field_of_view@@3MA
#include "objects/object_name_list.h"
// blamlibxbox/camera_scripting.obj: from the records 0x153458 0x290bf8 0x290d70 0x290ed0 0x290ee0 0x290f80 0x291198 0x291370 0x2913f8 0x291630 0x291770 0x2918d0 0x291948 0x291a38 0x291ab0 0x291b40 0x292930 0x292a18 0x292d28 0x292e38 0x292ee8 0x2930a8 0x2930d8 0x2930e8 0x293278 0x293318 0x293330 0x2b93b0 0x301180 0x301278 0x301330 0x3015a0 0x3016c8 0x3018e0 0x3019f0
#include "math/real_math_constants.h"
#include "interface/chud/chud_dispose_state_types.h"
// @flags /TP /O2
#include "cutscene/cinematic_new_globals_types.h"
#include "camera/observer_globals_types.h"
#include "render/render_texture_camera_globals.h"
#include "math/matrix_math_types.h"
#include "camera/cinematic_scene_types.h"
#include "tag_files/tag_types.h"
#include "animations/animation_id.h"
#include "units/motor_enums.h"
// @address 0x153458 ?scripted_camera_get_first_person_camera_object@@YAJXZ
// @address 0x290ed0 ?__tls_set_g_camera_script_globals_allocator@@YAXPAX@Z
// @address 0x291a38 ?calculate_field_of_view@@YAMM@Z
// @address 0x3018e0 scripted_camera_hide_object
#include <math.h>
#include <string.h>
#include "game/game_time_types.h"
#include "cache/restricted_memory_regions.h"
#include "game/players.h"
#include "game/player_mapping_types.h"
#include "objects/objects.h"
#include "sound/game_sound.h"
#include "camera/camera_scripting.h"
#include "camera/director.h"
#include "camera/first_person_camera.h"
#include "camera/observer.h"
#include "render/render_objects_static_lighting.h"
#include "cutscene/cinematics.h"
#include "game/game_time.h"
#include "motor/action_system.h"
#include "objects/lights.h"
#include "render/camera_fx_settings.h"
#include "hs/hs.h"
#include "hs/hs_library_external.h"
#include "units/unit_scripting.h"
#include "objects/scenery.h"
#include "sound/game_sound_definitions.h"
#include "scenario/scenario_cutscene_flag.h"
#include "camera/cinematic_scene_reference_types.h"
#include "camera/observer_depth_of_field_types.h"
#include "scenario/scenario_cinematic_reference_types.h"
#include "camera/cinematic_types.h"
#include "game/director_perspective_types.h"
#include "cache/cache_file_globals.h"
#include "math/real_math.h"
#include "math/real_math_globals.h"
#include "math/matrix_math.h"

// The object the thread state at TLS 0x8c focuses while it is active in mode
// 4, when that object is live and of type 0, 1 or 12; else -1.
struct real_matrix4x3;

#include "animations/animation_manager.h"
#include "camera/camera_script_globals_types.h"

#include "camera/camera_pan_navigator.h"

#include "objects/object_datum.h"
#include "units/unit_types.h"
#include "memory/data_types.h"

#include "animations/animation_definition_types.h"
#include "animations/animation_search_path_iterator.h"

#include "scenario/scenario_cutscene_camera_point.h"


#include "camera/scripted_camera.h"

#include "scenario/scenario_tag.h"
#include "interface/chud/cortana_effect_definition.h"

#include "game/game_globals.h"
#include "game/game_globals_storage.h"

#include "camera/camera_scripting_globals.h"
#include "camera/director_runtime_symbols.h"
#include "scenario/scenario_runtime_symbols.h"


struct s_object_shot_frame { long flags; s_camera_frame camera; };
struct s_object_shot { long frame_count; TAG_POINTER(s_object_shot_frame) frames; long definition; };
struct s_object_shot_set { long name; long type; long shot_count; TAG_POINTER(s_object_shot) shots; long definition; };
typedef char camera_shot_size[(sizeof(hs_cinematic_shot) == 0xa4) ? 1 : -1];
typedef char camera_shot_frames_offset[(offsetof(hs_cinematic_shot, frames) == 0x9c) ? 1 : -1];

#include "cutscene/cinematic_globals_types.h"
#include "camera/cinematic_scene_music_types.h"



#include "camera/observer_command.h"

#include "memory/tls_globals.h"
#include "math/real_constant_pool.h"
#include "objects/object_node_matrices_inline.h"
#include "scenario/scenario.h"
#include "cache/restricted_memory_registration.h"
#include "scenario/scenario_access.h"
#include "animations/animation_definitions.h"
#include "animations/animation_scalar_node_orientations.h"
#include "math/real_math_functions.h"
#include "interface/gui_model_widget_externs.h"
#include "memory/datum_index.h"
#include "tag_files/tag_groups.h"
#include "game/game_time_inline.h"
namespace {
#include "multithreading/thread_tls.h"
}

long scripted_camera_get_first_person_camera_object(void)
{
  long result = -1;
  s_camera_script_globals *state = g_camera_script_globals;
  if (state->active && state->mode == 4) {
    long object_index = state->object_index;
    if (object_try_and_get_and_verify_type(object_index, 0x1003))
      result = object_index;
  }
  return result;
}


bool c_camera_pan_navigator::set_parameters(float duration, float acceleration_time, float deceleration_time, float initial_scale, float final_scale)
{
    bool result = false;
    short total_ticks = (short)game_seconds_to_ticks_round(duration);
    short acceleration_ticks = (short)game_seconds_to_ticks_round(acceleration_time);
    short deceleration_ticks = (short)game_seconds_to_ticks_round(deceleration_time);
    m_valid = false;
    if (total_ticks > 0) {
        float divisor = (initial_scale - 1.0f) * (float)acceleration_ticks
            + (final_scale - 1.0f) * (float)deceleration_ticks + (float)(total_ticks * 2);
        if (divisor > k_real_one_thousandth_storage) {
            m_velocity = 2.0f / divisor;
            m_initial_velocity = m_velocity * initial_scale;
            if (acceleration_ticks > 0)
                m_acceleration = (m_velocity - m_initial_velocity) / (2.0f * (float)acceleration_ticks);
            else
                m_acceleration = 0.0f;
            if (deceleration_ticks > 0)
                m_deceleration = m_velocity * (final_scale - 1.0f) / (2.0f * (float)deceleration_ticks);
            else
                m_deceleration = 0.0f;
            m_acceleration_ticks = acceleration_ticks;
            m_deceleration_ticks = deceleration_ticks;
            m_total_ticks = total_ticks;
            m_valid = true;
            result = true;
        }
    }
    return result;
}

// @address 0x11df68c g_camera_script_globals
__declspec(thread) s_camera_script_globals *g_camera_script_globals;

void __tls_set_g_camera_script_globals_allocator(void *value)
{
    g_camera_script_globals = (s_camera_script_globals *)value;
}

#define CAMERA_MAX(a,b) ((a)>(b)?(a):(b))
#define CAMERA_MIN(a,b) ((a)<(b)?(a):(b))
#define CAMERA_PIN(a,b,c) CAMERA_MIN(CAMERA_MAX(a,b),c)
static __forceinline float camera_pan_dot_product3d(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + a->j * b->j + a->k * b->k;
}
float c_camera_pan_navigator::evaluate(long ticks)
{
    float result = 1.0f;
    if (m_valid) {
        real_vector3d linear = { m_initial_velocity, m_velocity, m_velocity };
        real_vector3d quadratic = { m_acceleration, 0.0f, m_deceleration };
        real_vector3d time;
        time.i = (float)CAMERA_PIN(ticks, 0, m_acceleration_ticks);
        time.j = (float)CAMERA_PIN(ticks - m_acceleration_ticks, 0, m_total_ticks - m_deceleration_ticks - m_acceleration_ticks);
        time.k = (float)CAMERA_PIN(ticks - (m_total_ticks - m_deceleration_ticks), 0, m_deceleration_ticks);
        real_vector3d squared = { time.i * time.i, time.j * time.j, time.k * time.k };
        result = camera_pan_dot_product3d(&linear, &time) + camera_pan_dot_product3d(&quadratic, &squared);
    }
    return CAMERA_PIN(result, 0.0f, 1.0f);
}

namespace {
static __forceinline const c_model_animation_graph *camera_scripting_get_animation_graph(long graph_index)
{
    return (const c_model_animation_graph *)tag_get('jmad', graph_index);
}
static __forceinline long camera_scripting_get_game_time()
{
    return game_time_globals->time;
}
static __forceinline void camera_scripting_begin_animation()
{
    g_camera_script_globals->mode = 3;
    g_camera_script_globals->animation_changed = true;
}
static __forceinline s_camera_script_globals::s_animation_state *camera_scripting_get_animation_state()
{
    return &g_camera_script_globals->animation;
}
}
// @address 0x291198 ?scripted_camera_set_animation_relative@@YAXJJJFM_NM@Z
void scripted_camera_set_animation_relative(long graph_index, long animation_name, long object_index,
    short flag_index, float playback_scale, bool looping, float start_fraction)
{
    if (graph_index != -1) {
        const c_model_animation_graph *graph = camera_scripting_get_animation_graph(graph_index);
        c_animation_id animation_id = graph->find_animation(animation_name);
        const long &animation_index = animation_id.m_animation_index;
        if (animation_index >= 0) {
            const c_model_animation *animation = graph->get_animation(animation_id);
            if (animation) {
                playback_scale = playback_scale > 0.01f ? playback_scale : 0.01f;
                camera_scripting_begin_animation();
                s_camera_script_globals::s_animation_state *state = camera_scripting_get_animation_state();
                float duration = ((float)animation->get_frame_count() - 1.0f) / 30.0f;
                duration *= playback_scale;
                state->graph_index = graph_index;
                state->animation_name = animation_name;
                state->flag_index = flag_index;
                state->playback_scale = playback_scale;
                state->start_tick = (long)((float)camera_scripting_get_game_time() - (float)game_seconds_to_ticks_round(duration) * start_fraction);
                state->looping = looping;
                g_camera_script_globals->animation_duration = game_seconds_to_ticks_round(duration);
                if (object_index != -1 && object_try_and_get_and_verify_type(object_index, 0x1003)
                    && unit_is_playing_custom_animation(object_index))
                    g_camera_script_globals->object_index = object_index;
                else
                    g_camera_script_globals->object_index = -1;
            }
        }
    }
}

// @address 0x2918d0 ?scripted_camera_get_relative_object_space@@YAXJPAUreal_matrix4x3@@@Z
void scripted_camera_get_relative_object_space(long object_index, real_matrix4x3 *matrix) {
    if (object_try_and_get_and_verify_type(object_index, (unsigned long)-1)) {
        bool transformed = false;
        if (!transformed)
            *matrix = *object_get_node_matrix(object_index, 0);
    } else {
        *matrix = k_real_math_identity_matrix_storage;
    }
}


static __forceinline void camera_scripting_activate_pan()
{
    g_camera_script_globals->mode = 2;
    g_camera_script_globals->animation_changed = true;
}
static __forceinline s_camera_scripting_pan *camera_scripting_get_pan()
{
    return &g_camera_script_globals->pan;
}

// @address 0x291370 ?scripted_camera_set@@YAXFFJ@Z
void scripted_camera_set(short point_index, short ticks, long object_index)
{
    scenario *scenario_view = global_scenario;
    if (point_index >= 0 && point_index < scenario_view->cutscene_camera_points.count) {
        scenario_cutscene_camera_point *point = &((scenario_cutscene_camera_point *)scenario_view->cutscene_camera_points.address)[point_index];
        scripted_camera_set_internal((e_scenario_camera_type)point->type, &point->position, &point->orientation,
            hs_ticks_to_seconds(ticks), point_index, object_index);
    }
}


static __forceinline void camera_scripting_begin_scripted()
{
    g_camera_script_globals->scripted_mode = _camera_script_mode_point;
    g_camera_script_globals->animation_changed = true;
}
static __forceinline s_scripted_camera_state *camera_scripting_get_scripted_state()
{
    // match: 0x291b40: removing the mode assumption swaps the operands of an add in scripted_camera_update
    __assume((e_camera_script_mode)g_camera_script_globals->scripted_mode == _camera_script_mode_point);
    s_scripted_camera_state *result = &g_camera_script_globals->scripted;
    return result;
}
// @address 0x2913f8 ?scripted_camera_set_internal@@YAXW4e_scenario_camera_type@@PBTreal_point3d@@PBTreal_euler_angles3d@@MFJ@Z
void scripted_camera_set_internal(e_scenario_camera_type scenario_camera_type, const real_point3d *point,
    const real_euler_angles3d *euler_angles, float duration, short point_index, long object_index)
{
    camera_scripting_begin_scripted();
    s_scripted_camera_state *scripted = camera_scripting_get_scripted_state();
    scripted->camera_type = scenario_camera_type;
    scripted->point_index = point_index;
    g_camera_script_globals->position = *point;
    vectors3d_from_euler_angles3d(&g_camera_script_globals->forward, &g_camera_script_globals->up, euler_angles);
    g_camera_script_globals->animation_duration = game_seconds_to_ticks_round(duration);
    s_camera_script_globals *state = g_camera_script_globals;
    if (object_index != -1) {
        switch (scripted->camera_type) {
        case _scenario_camera_type_0:
            break;
        case _scenario_camera_type_1: {
            real_matrix4x3 matrix;
            scripted_camera_get_relative_object_space(object_index, &matrix);
            matrix4x3_transform_vector(&matrix, &state->forward, &state->forward);
            matrix4x3_transform_vector(&matrix, &state->up, &state->up);
            matrix4x3_transform_vector(&matrix, &state->position_offset, &state->position_offset);
            break;
        }
        case _scenario_camera_type_3: {
            real_matrix4x3 matrix;
            scripted_camera_get_relative_object_space(object_index, &matrix);
            matrix4x3_transform_point(&matrix, &state->position, &state->position);
            matrix4x3_transform_vector(&matrix, &state->forward, &state->forward);
            matrix4x3_transform_vector(&matrix, &state->up, &state->up);
            object_index = -1;
            break;
        }
        case _scenario_camera_type_2: {
            real_matrix4x3 matrix;
            s_scripted_camera_target *target = &scripted->target;
            scripted_camera_get_relative_object_space(object_index, &matrix);
            matrix4x3_transform_vector(&matrix, &state->forward, &state->forward);
            matrix4x3_transform_vector(&matrix, &state->up, &state->up);
            matrix4x3_transform_vector(&matrix, &state->position_offset, &state->position_offset);
            target->position = matrix.position;
            cross_product3d(&state->forward, &state->up, &target->side);
            normalize3d(&target->side);
            break;
        }
        default:
            // match: 0x2913f8: unreachable hint omits dead branch check
            __assume(0);
            break;
        }
    }
    state->object_index = object_index;
    director_update(0.0f);
    observer_update(0.0001f);
}

// @address 0x291630 ?scripted_camera_set_pan@@YAXFF@Z
void scripted_camera_set_pan(short point_index, short ticks)
{
    if (ticks > 0) {
        scenario_cutscene_camera_point *point = &((scenario_cutscene_camera_point *)global_scenario->cutscene_camera_points.address)[point_index];
        float duration = hs_ticks_to_seconds(ticks);
        camera_scripting_activate_pan();
        s_camera_scripting_pan *pan = camera_scripting_get_pan();
        pan->position = point->position;
        real_matrix4x3 matrix;
        matrix4x3_rotation_from_angles(&matrix, point->orientation.yaw, point->orientation.pitch, point->orientation.roll);
        real_matrix3x3 rotation;
        rotation.forward = matrix.forward;
        rotation.up = matrix.up;
        rotation.left = matrix.left;
        matrix3x3_rotation_to_quaternion(&rotation, &pan->orientation);
        pan->time = camera_scripting_get_game_time();
        pan->navigator.set_parameters(duration, 0.0f, 0.0f, 1.0f, 1.0f);
    } else {
        scripted_camera_set(point_index, ticks, -1);
    }
}

// @address 0x291770 ?scripted_camera_pan@@YAXFFFFMFM@Z
void scripted_camera_pan(short set_index, short point_index, short duration_ticks, short acceleration_ticks,
    float initial_scale, short deceleration_ticks, float final_scale)
{
    scripted_camera_set(set_index, 0, -1);
    camera_scripting_activate_pan();
    if (camera_scripting_get_pan()->navigator.set_parameters(hs_ticks_to_seconds(duration_ticks),
            hs_ticks_to_seconds(acceleration_ticks),
            hs_ticks_to_seconds(deceleration_ticks), initial_scale, final_scale)) {
        scenario_cutscene_camera_point *location = &((scenario_cutscene_camera_point *)global_scenario->cutscene_camera_points.address)[point_index];
        s_camera_scripting_pan *pan = camera_scripting_get_pan();
        camera_scripting_activate_pan();
        pan->position = location->position;
        real_matrix4x3 matrix;
        matrix4x3_rotation_from_angles(&matrix, location->orientation.yaw, location->orientation.pitch, location->orientation.roll);
        matrix3x3_rotation_to_quaternion(&matrix.rotation, &pan->orientation);
        pan->time = camera_scripting_get_game_time();
        return;
    }
    scripted_camera_set_pan(point_index, duration_ticks);
}

float calculate_field_of_view(float distance)
{
  s_game_globals *definition =
      (s_game_globals *)tag_get('matg', global_scenario_game_globals_index);
  s_game_globals_cinematic_camera *element = definition->cinematic_camera.count() ? (s_game_globals_cinematic_camera *)definition->cinematic_camera.address : 0;
  return (float)atan(element->aperture / (distance * 2.0f)) * 2.0f;
}


// @address 0x2930a8 ?get_perspective@c_scripted_camera@@UBA?AW4e_director_perspective@@XZ
e_director_perspective c_scripted_camera::get_perspective() const
{
    return scripted_camera_get_first_person_camera_object() == -1 ? _director_perspective_scripted : _director_perspective_first_person;
}

// @address 0x2930d8 ?update@c_scripted_camera@@UAAXJMPAUs_observer_command@@@Z
void c_scripted_camera::update(long user_index, float dt, s_observer_command *result)
{
    scripted_camera_update(0, 0, result);
}

static __forceinline long camera_scripting_first_active_user()
{
    long result = -1;
    for (long user_index = 0; user_index < 4; ++user_index) {
        if (player_mapping_globals->player_indices[user_index] != -1) {
            result = user_index;
            break;
        }
    }
    return result;
}

// @address 0x292ee8 ?cinematic_camera_set_easing_in@@YAXMMMMMM@Z
// 0x292ee8 (the March dev build names it cinematic_camera_set_easing_in)
void cinematic_camera_set_easing_in(float seconds, float scale, float field_of_view, float i, float j, float k)
{
    s_camera_script_globals::s_camera_track *state = &g_camera_script_globals->camera_track;
    const s_observer_result *camera = (const s_observer_result *)observer_try_and_get_camera(
        camera_scripting_first_active_user());
    if (camera) {
        state->position = camera->position;
        real_matrix4x3 matrix;
        matrix4x3_rotation_from_vectors(&matrix, &camera->forward, &camera->up);
        matrix.position = *global_origin3d;
        matrix3x3_rotation_to_quaternion(&matrix.rotation, &state->orientation);
    } else {
        state->position = *global_origin3d;
        state->orientation = *global_identity_quaternion;
    }
    state->field_of_view = field_of_view;
    state->easing_velocity.i = i;
    state->easing_velocity.j = j;
    state->easing_velocity.k = k;
    state->scale = scale;
    state->duration = (float)game_seconds_to_ticks_round(seconds);
    state->easing = _easing_type_1;
}

// @address 0x2930e8 ?cinematic_camera_set_easing_out@@YAXM@Z
void cinematic_camera_set_easing_out(float seconds)
{
    s_camera_script_globals::s_camera_track *state = &g_camera_script_globals->camera_track;
    const s_observer_result *camera = (const s_observer_result *)observer_try_and_get_camera(
        camera_scripting_first_active_user());
    if (camera) {
        real_matrix4x3 matrix;
        matrix4x3_rotation_from_vectors(&matrix, &camera->forward, &camera->up);
        matrix.position = *global_origin3d;
        matrix3x3_rotation_to_quaternion(&matrix.rotation, &state->previous_orientation);
        state->previous_position = camera->position;
    } else {
        state->previous_orientation = *global_identity_quaternion;
        state->previous_position = *global_origin3d;
    }
    state->duration = (float)game_seconds_to_ticks_round(seconds);
    state->easing = _easing_type_2;
    g_camera_script_globals->camera_track.previous_frame = 0;
    g_camera_script_globals->field_of_view_time = 0.0f;
}

// @address 0x293278 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_camera_script_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_camera_script_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits) {
    // match: 0x293278: value range assumption guides branch lowering
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_register_member(3, name, type, size, alignment_bits,
        __tls_set_g_camera_script_globals_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// 0x293318
template <>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_camera_script_globals_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x293330
template <>
void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_camera_script_globals_allocator>::deallocate(void *memory)
{
    long value = m_member_index;
    g_camera_script_globals = 0;
    restricted_region_free_member(3, value);
}

namespace {

}

// @address 0x290ee0 ?transform_into_flag_space@@YAXPAUreal_matrix4x3@@J@Z
void transform_into_flag_space(struct real_matrix4x3 * matrix, long cutscene_flag_index) {
    if (cutscene_flag_index != -1) {
        scenario_cutscene_flag *flag = (scenario_cutscene_flag *)tag_block_get_element_with_size(
            &global_scenario_get()->cutscene_flags, cutscene_flag_index, sizeof(scenario_cutscene_flag));
        real_vector3d forward;
        real_vector3d up;
        real_matrix4x3 flag_matrix;
        vector3d_from_euler_angles2d(&forward, &flag->facing);
        generate_up_vector3d(&forward, &up);
        matrix4x3_from_point_and_vectors(&flag_matrix, &flag->position, &forward, &up);
        matrix4x3_compute_product(&flag_matrix, matrix, matrix);
    }
}


// @address 0x2b93b0 scripted_camera_set_cinematic_lighting
extern "C" void scripted_camera_set_cinematic_lighting(long item_index)
{
    real_matrix4x3 block;
    lights_reset_cinematic_lighting();
    long handle = cinematic_new_globals->scene_tag_index;
    if (handle != -1) {
        s_cinematic_scene *list = (s_cinematic_scene *)tag_get('cisc', handle);
        if (list != 0 && item_index >= 0 && item_index < list->shots.count) {
            hs_cinematic_shot *item = &((hs_cinematic_shot *)((s_cinematic_scene *)tag_get('cisc', handle))->shots.address)[item_index];
            camera_fx_set_exposure(item->exposure * 8.0f);
            memcpy(&block, &k_real_math_identity_matrix_storage, sizeof(block));
            if (item->frame_count > 0)
                scripted_camera_get_deterministic_cinematic_camera_matrix(item->frames, &block);
            long index = 0;
            if (item->lighting_count > 0) {
                long offset = 0;
                do {
                    s_cinematic_item_element *element = (s_cinematic_item_element *)((char *)item->lighting + offset);
                    long key = element->lighting.index;
                    if (key != -1) {
                        long target = cinematic_object_get_from_scene(element->handle);
                        if (target != -1)
                            lights_set_cinematic_object_lighting(target, element->value, key, &block);
                    }
                    ++index;
                    offset += sizeof(s_cinematic_item_element);
                } while (index < item->lighting_count);
            }
        }
    }
}

// @address 0x301180 scripted_camera_get_scene
s_cinematic_scene *scripted_camera_get_scene(long cinematic_index, long scene_index) {
    s_cinematic_scene *result = 0;
    s_cinematic *cinematic = 0;
    long scene_tag_index = -1;

    if (cinematic_index >= 0) {
        scenario *scen = global_scenario_get();
        if (cinematic_index < scen->cinematics.count) {
            s_scenario_cinematic_reference *cinematic_reference =
                (s_scenario_cinematic_reference *)tag_block_get_element_with_size(&scen->cinematics, cinematic_index, sizeof(s_scenario_cinematic_reference));
            if (cinematic_reference->cinematic.index != -1)
                cinematic = (s_cinematic *)tag_get('cine', cinematic_reference->cinematic.index);
        }
    }

    if (cinematic) {
        if (scene_index >= 0 && scene_index < cinematic->scenes.count) {
            s_cinematic_scene_reference *scene_reference =
                (s_cinematic_scene_reference *)tag_block_get_element_with_size(&cinematic->scenes, scene_index, sizeof(s_cinematic_scene_reference));
            scene_tag_index = scene_reference->scene.index;
        }
    } else if (cinematic_index != -1 && (cinematic_index < 0 || cinematic_index > 1000)
        && tag_get_group_tag(cinematic_index) == 'crte') {
        s_cortana_effect_definition *transition = (s_cortana_effect_definition *)tag_get('crte', cinematic_index);
        scene_tag_index = transition->cinematic_scene.index;
    }

    if (scene_tag_index != -1)
        result = (s_cinematic_scene *)tag_get('cisc', scene_tag_index);
    return result;
}

// @address 0x301278 scripted_camera_is_object_valid_for_players
bool scripted_camera_is_object_valid_for_players(long cinematic_index, long scene_index, long object_index) {
    bool result = false;
    s_cinematic_scene *scene = scripted_camera_get_scene(cinematic_index, scene_index);
    if (scene && object_index >= 0 && object_index < scene->objects.count) {
        s_cinematic_scene_object *object =
            (s_cinematic_scene_object *)tag_block_get_element_with_size(&scene->objects, object_index, sizeof(s_cinematic_scene_object));
        if (object) {
            long player_count = 0;
            if (game_globals->options.game_mode == 1) {
                player_count = players_get_active_and_in_game_count(true);
            }
            result = ((1 << (player_count - 1)) & object->player_flags) == 0;
        }
    }
    return result;
}

namespace {
static __forceinline bool camera_scripting_structure_active(long index) {
    if (index == -1 || (unsigned long)index > 1000) return true;
    return index < 32 && ((1UL << index) & g_active_cinematic_zone_mask) != 0;
}
static __forceinline long camera_scripting_object_index(short index) {
    if (index < 0 || index >= 2048) return -1;
    return ((long *)g_object_name_list)[index];
}
}
void scripted_camera_set_animation_relative(long bsp_index, long camera_index, long object_index, short name_index)
{
    s_cinematic_scene *track = scripted_camera_get_scene(bsp_index, camera_index);
    if (track && camera_scripting_structure_active(bsp_index) && object_index >= 0 && object_index < track->objects.count) {
        s_cinematic_scene_object *object = (s_cinematic_scene_object *)tag_block_get_element_with_size(
            &track->objects, object_index, sizeof(s_cinematic_scene_object));
        if (object) {
            if ((long)object->flags & 8) {
                if (name_index != -1 && camera_scripting_object_index(name_index) == -1)
                    ::object_new_by_name(name_index, 0, 0);
            } else {
                hs_object_create_anew(name_index);
            }
            long index = camera_scripting_object_index(name_index);
            if (index != -1) {
                ::object_set_hidden(index, false);
                ::object_set_always_active(index, 1);
            }
        }
    }
}

// @address 0x292930 ?scripted_camera_get_deterministic_cluster_reference@@YA?AUs_cluster_reference@@XZ
s_cluster_reference scripted_camera_get_deterministic_cluster_reference(void)
{
    s_cluster_reference result;
    result.value() = -1;
    s_camera_script_globals *state = g_camera_script_globals;
    int mode = (short)state->mode - 1;
    real_matrix4x3 matrix;
    real_point3d position;
    real_vector3d forward;
    float time;
    switch (mode) {
    case 0:
        result = scenario_cluster_reference_from_point(&state->position);
        break;
    case 1:
        result = scenario_cluster_reference_from_point(&state->pan.position);
        break;
    case 2:
        if (scripted_camera_get_deterministic_animated_camera_matrix(&matrix, &time))
            result = scenario_cluster_reference_from_point(&matrix.position);
        break;
    case 3: {
        long object_index = scripted_camera_get_first_person_camera_object();
        if (object_index != -1) {
            first_person_camera_deterministic(object_index, &position, &forward);
            result = scenario_cluster_reference_from_point(&position);
        }
        break;
    }
    case 4:
        if (state->cinematic_flag_index != -1) {
            scripted_camera_get_deterministic_cinematic_camera_matrix(0, &matrix);
            result = scenario_cluster_reference_from_point(&matrix.position);
        }
        break;
    }
    return result;
}


// @address 0x292a18 ?scripted_camera_get_deterministic_animated_camera_matrix@@YA_NPAUreal_matrix4x3@@PAM@Z
bool scripted_camera_get_deterministic_animated_camera_matrix(real_matrix4x3 *matrix, float *time)
{
    bool result = false;
    s_camera_script_globals::s_animation_state *animation_state = camera_scripting_get_animation_state();
    const c_model_animation_graph *graph = camera_scripting_get_animation_graph(animation_state->graph_index);
    if (graph) {
        c_animation_id animation_id = graph->find_animation(animation_state->animation_name);
        const long &animation_index = animation_id.m_animation_index;
        if (animation_index >= 0) {
            const c_model_animation *animation = graph->get_animation(animation_id);
            long object_index = g_camera_script_globals->object_index;
            float animation_time;
            if (object_index != -1 && object_try_and_get_and_verify_type(object_index, 0x1003) &&
                action_executing(object_index, _action_custom_animation)) {
                object_datum *object = ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
                const c_animation_manager *animation_manager = (const c_animation_manager *)((char *)object + object->animation.offset);
                animation_time = animation_manager->get_authored_state_time();
            } else {
                animation_time = (float)(camera_scripting_get_game_time() - animation_state->start_tick) * game_time_globals->tick_length *
                    animation_state->playback_scale;
            }
            float last_frame = (float)(animation->get_frame_count() - 1);
            if (animation_state->looping)
                animation_time = (float)fmod(animation_time, last_frame * (1.0f / 30.0f));
            animation_time = CAMERA_PIN(animation_time, 0.0f, animation_time);
            *time = animation_time;
            real_matrix4x3 root_matrix;
            float frame = animation_time * 30.0f;
            last_frame -= 1.0f;
            if (frame >= 1.0f && frame <= last_frame) {
                animation_definitions_get_animation_root_matrix(animation_state->graph_index, animation_id,
                    animation_time, NULL, &root_matrix);
            } else {
                real_orientation orientation;
                {
                    real_orientation last_orientation;
                    real_orientation first_orientation;
                    animation_compute_node_orientations(animation_state->graph_index, animation_id, 1, last_frame * (1.0f / 30.0f) * 30.0f,
                        1.0f, NULL, &last_orientation, 1, false, 0.0f, 0.0f);
                    animation_compute_node_orientations(
                        animation_state->graph_index, animation_id, 1, 1.0f, 1.0f, NULL, &first_orientation, 1, false, 0.0f, 0.0f);
                    float t;
                    if (frame < 1.0f) {
                        t = frame * 0.5f;
                        t += 0.5f;
                    } else {
                        t = (frame - last_frame) * 0.5f;
                    }
                    t = CAMERA_PIN(t, 0.0f, 1.0f);
                    orientation_interpolate(&last_orientation, &first_orientation, t, &orientation);
                }
                matrix4x3_from_point_and_quaternion(&root_matrix, &orientation.translation, &orientation.rotation);
            }
            if (!valid_world_real_point3d(&root_matrix.position))
                root_matrix.position = *global_origin3d;
            const long &flag_index = animation_state->flag_index;
            transform_into_flag_space(&root_matrix, flag_index);
            memcpy(matrix, &root_matrix, sizeof(*matrix));
            result = true;
        }
    }
    return result;
}

// @address 0x292d28 ?scripted_camera_get_deterministic_cinematic_camera_matrix@@YAXPBUs_camera_frame@@PAUreal_matrix4x3@@@Z
void scripted_camera_get_deterministic_cinematic_camera_matrix(const s_camera_frame *frame, real_matrix4x3 *matrix)
{
    if (g_camera_script_globals->mode == 5) {
        s_camera_script_globals::s_camera_track *state = &g_camera_script_globals->camera_track;
        if (!frame) {
            s_cinematic_scene *definition = (s_cinematic_scene *)tag_get('cisc', state->tag);
            hs_cinematic_shot *shot = &((hs_cinematic_shot *)definition->shots.address)[CAMERA_PIN(state->shot_index, 0, definition->shots.count - 1)];
            frame = &shot->frames[CAMERA_PIN(state->frame, 0, shot->frame_count - 1)];
        }
        matrix4x3_from_point_and_vectors(matrix, &frame->position, &frame->forward, &frame->up);
        ::transform_into_flag_space(matrix, state->cutscene_flag_index);
    }
}
inline float observer_suggested_field_of_view() { return g_observer_default_field_of_view; }
// @address 0x292e38 ?scripted_camera_get_field_of_view@@YAMXZ
float scripted_camera_get_field_of_view(void)
{
    float field_of_view = observer_suggested_field_of_view();
    bool in_progress = false;
    if (cinematic_globals)
        in_progress = cinematic_globals->in_progress;
    if (in_progress && g_camera_script_globals->field_of_view_target != 0.0f) {
        if (g_camera_script_globals->field_of_view_time >= g_camera_script_globals->field_of_view_duration) {
            field_of_view = g_camera_script_globals->field_of_view_target;
        } else {
            float t = g_camera_script_globals->field_of_view_time / g_camera_script_globals->field_of_view_duration;
            t = CAMERA_PIN(t, 0.0f, 1.0f);
            field_of_view = interpolate_linear(g_camera_script_globals->field_of_view_start, g_camera_script_globals->field_of_view_target, t);
        }
    }
    return field_of_view;
}

const float k_camera_scripting_degrees_to_radians = 0.017453292f;
static __forceinline bool camera_scripting_valid_field_of_view(float field_of_view)
{
    long bits[1];
    *(float *)bits = field_of_view;
    return (bits[0] & 0x7f800000) != 0x7f800000
        && !(field_of_view < k_observer_field_of_view_minimum) && field_of_view <= k_observer_field_of_view_maximum;
}
// @address 0x291948 ?scripted_camera_set_field_of_view@@YAXMF@Z
void scripted_camera_set_field_of_view(float field_of_view, short ticks)
{
    float suggested = observer_suggested_field_of_view();
    if (g_camera_script_globals->field_of_view_start == 0.0f)
        g_camera_script_globals->field_of_view_start = suggested;
    g_camera_script_globals->field_of_view_start = scripted_camera_get_field_of_view();
    g_camera_script_globals->field_of_view_time = 0.0f;
    g_camera_script_globals->field_of_view_duration = hs_ticks_to_seconds(ticks);
    g_camera_script_globals->field_of_view_target = field_of_view * k_camera_scripting_degrees_to_radians;
    if (!camera_scripting_valid_field_of_view(g_camera_script_globals->field_of_view_target))
        memset(&g_camera_script_globals->field_of_view_time, 0, 4 * sizeof(float));
}

// @address 0x291ab0 ?cinematic_camera_update_texture_camera@@YAXPAUs_camera_frame@@@Z
void cinematic_camera_update_texture_camera(s_camera_frame *frame)
{
    real_matrix4x3 matrix;
    matrix4x3_from_point_and_vectors(&matrix, &frame->position, &frame->forward, &frame->up);
    // match: 0x291ab0: pointer __restrict qualifier prevents alias reload
    s_camera_script_globals::s_camera_track *__restrict flag_space = &g_camera_script_globals->camera_track;
    ::transform_into_flag_space(&matrix, flag_space->cutscene_flag_index);
    render_texture_camera_set_position_and_direction(
        true, &matrix.position, &matrix.forward, &matrix.up, calculate_field_of_view(frame->field_of_view) * 57.29578f);
}


// @address 0x290f80 ?scripted_camera_game_tick@@YAXXZ
void scripted_camera_game_tick(void)
{
    g_camera_script_globals->animation_duration = g_camera_script_globals->animation_duration - 1 >= 0 ? g_camera_script_globals->animation_duration - 1 : 0;
    g_camera_script_globals->field_of_view_time += game_ticks_to_seconds(1.0f);
    if (g_camera_script_globals->mode == 5) {
        ++g_camera_script_globals->camera_track.total_ticks;
        if (g_camera_script_globals->camera_track.easing != 2) {
            long frame = (long)(game_ticks_to_seconds((float)g_camera_script_globals->camera_track.total_ticks) * 30.0f);
            s_camera_script_globals::s_camera_track *track = &g_camera_script_globals->camera_track;
            if (track->tag != -1) {
                s_cinematic_scene *definition = (s_cinematic_scene *)tag_get('cisc', track->tag);
                hs_cinematic_shot *shot = &((hs_cinematic_shot *)definition->shots.address)[track->shot_index];
                if (track->frame >= shot->frame_count && definition->shots.count > track->shot_index + 1)
                    track->frame = shot->frame_count - 1;
                if ((g_chud_cinematic_state->tag != -1 || track->texture) && shot->frame_count > 0
                    && track->frame >= 0 && track->frame < shot->frame_count) {
                    if (track->frame >= 0 && track->frame < shot->frame_count)
                        cinematic_camera_update_texture_camera(&shot->frames[track->frame]);
                }
                track->frame += frame - track->previous_frame;
                track->previous_frame = frame;
            }
            if (track->easing == 1 && (float)track->previous_frame > track->duration)
                track->easing = _easing_type_0;
        } else {
            g_camera_script_globals->camera_track.previous_frame =
                (long)(game_ticks_to_seconds((float)g_camera_script_globals->camera_track.total_ticks) * 30.0f);
            if ((float)g_camera_script_globals->camera_track.previous_frame > g_camera_script_globals->camera_track.duration)
                g_camera_script_globals->camera_track.easing = _easing_type_0;
        }
    }
}

// @address 0x3015a0 scripted_camera_start_object_animation
extern "C" void scripted_camera_start_object_animation(long cinematic_index, long scene_index, long scene_object_index,
    long object_index, long animation_name)
{
    s_cinematic_scene *scene = scripted_camera_get_scene(cinematic_index, scene_index);
    if (scene && scene_object_index >= 0 && scene_object_index < scene->objects.count) {
        s_cinematic_scene_object *entry = (s_cinematic_scene_object *)tag_block_get_element_with_size(
            &scene->objects, scene_object_index, sizeof(s_cinematic_scene_object));
        if (entry && object_index != -1) {
            const object_datum *object =
                ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
            long type_mask = 1 << object->object_type;
            if (((type_mask & 64) != 0 || (type_mask & 0x2000) != 0) && ((long)entry->flags & 8) == 0) {
                long relative_object_index = cinematic_object_get(scene->name);
                scenery_animation_start_private(object_index,
                    entry->animation_graph.index,
                    animation_name, 0, relative_object_index, true, false);
            } else if ((type_mask & 0x1003) != 0 && ((long)entry->flags & 8) == 0) {
                long relative_object_index = cinematic_object_get(scene->name);
                unit_start_user_animation_relative(object_index,
                    entry->animation_graph.index,
                    animation_name, false, relative_object_index);
                unit_scripting_set_component_flag_8(object_index, true);
            }
        }
    }
}
static __forceinline long camera_scripting_object_animation_disabled(const s_cinematic_scene_object *entry)
{
    long flags = (long)entry->flags;
    if ((flags & 8) != 0)
        return flags & 8;
    return 0;
}

// @address 0x3016c8 scripted_camera_show_object
extern "C" void scripted_camera_show_object(long cinematic_index, long scene_index, long scene_object_index,
    short name_index, long object_index, long animation_name, bool flag)
{
    s_cinematic_scene *scene = scripted_camera_get_scene(cinematic_index, scene_index);
    if (!scene)
        return;
    if (!camera_scripting_structure_active(cinematic_index)
        || scene_object_index < 0
        || scene_object_index >= scene->objects.count)
        return;
    s_cinematic_scene_object *entry = (s_cinematic_scene_object *)tag_block_get_element_with_size(
        &scene->objects, scene_object_index, sizeof(s_cinematic_scene_object));
    if (!entry)
        return;
    if (!scripted_camera_is_object_valid_for_players(cinematic_index, scene_index, scene_object_index))
        return;
    if (!flag)
        return;
    long entry_flags = (long)entry->flags;
    if ((entry_flags & 2) == 0) {
        if ((entry_flags & 1) != 0) {
            scripted_camera_set_animation_relative(cinematic_index, scene_index, scene_object_index, name_index);
            long object_name_index = camera_scripting_object_index(name_index);
            object_index = object_name_index;
        }
        else {
            cinematic_activate_scene_object(cinematic_index, scene_index, scene_object_index);
            long entry_object_index = cinematic_object_get((const char *)entry);
            object_index = entry_object_index;
        }
    }
    if (object_index != -1) {
        if (((long)entry->flags & 16) != 0) {
            object_datum *object = ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
            object->flags |= 0x10000;
            object_update_visibility_culling(object_index);
        }
        bool invalidate;
        if (scene->scene_type == 2 || (scene->scene_type == 0 && scene->objects.count < 100))
            invalidate = true;
        else
            invalidate = false;
        if (invalidate)
            render_object_invalidate_render_state(object_index);
    }
    if (!camera_scripting_object_animation_disabled(entry))
        scripted_camera_start_object_animation(cinematic_index, scene_index, scene_object_index, object_index, animation_name);
}
// @address 0x3018e0 scripted_camera_hide_object
extern "C" void scripted_camera_hide_object(long cinematic_index, long scene_index, long scene_object_index, long object_index)
{
    s_cinematic_scene *scene = scripted_camera_get_scene(cinematic_index, scene_index);
    if (scene && scene_object_index >= 0 && scene_object_index < scene->objects.count) {
        s_cinematic_scene_object *entry = (s_cinematic_scene_object *)tag_block_get_element_with_size(
            &scene->objects, scene_object_index, sizeof(s_cinematic_scene_object));
        if (entry && object_index != -1) {
            long entry_flags = (long)entry->flags;
            object_datum *object = ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum;
            if ((entry_flags & 1) == 0 && (entry_flags & 2) == 0) {
                ::object_set_hidden(object_index, true);
                object_set_always_active(object_index, false);
                ((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum->flags &= ~0x200000u;
            }
            long type_bits = 1 << object->object_type;
            if ((type_bits & 0x1003) != 0 && (type_bits & 64) == 0 && (type_bits & 0x2000) == 0) {
                action_submit_event(object_index, 2);
                unit_scripting_set_component_flag_8(object_index, false);
            }
        }
    }
}

// @address 0x3019f0 scripted_camera_start_shot_sound
extern "C" void scripted_camera_start_shot_sound(long cinematic_index, long scene_index, long shot_index, long sound_index)
{
    s_cinematic_scene *scene = scripted_camera_get_scene(cinematic_index, scene_index);
    if (scene && shot_index >= 0 && shot_index < scene->shots.count) {
        hs_cinematic_shot *shot = (hs_cinematic_shot *)tag_block_get_element_with_size(&scene->shots, shot_index, sizeof(hs_cinematic_shot));
        if (shot && sound_index >= 0 && sound_index < shot->music.count) {
            s_cinematic_scene_music *sound = (s_cinematic_scene_music *)tag_block_get_element_with_size(
                &shot->music, sound_index, sizeof(s_cinematic_scene_music));
            if (sound->sound.group_tag == 'snd!') {
                scripted_sound_new_internal(sound->sound.index, -1, 1.0f, k_scripted_sound_default_gain, k_scripted_sound_default_gain, -1, 0);
                return;
            }
            if (sound->sound.group_tag == 'lsnd')
                scripted_looping_sound_start_with_effect(sound->sound.index, -1, 1.0f, 0);
        }
    }
}

static __forceinline void camera_scripting_update_points_interpolate(const real_point3d *a, const real_point3d *b, float t, real_point3d *result)
{
    float inverse = 1.0f - t;
    float product = b->x * t;
    result->x = a->x * inverse + product;
    product = a->y * inverse;
    result->y = b->y * t + product;
    product = a->z * inverse;
    result->z = b->z * t + product;
}
inline void camera_scripting_update_matrix_to_vectors(const real_matrix4x3 *matrix,
    real_point3d *point, real_vector3d *forward, real_vector3d *up)
{
    *forward = matrix->forward;
    *up = matrix->up;
    *point = matrix->position;
}
inline void camera_scripting_update_matrix_to_quaternion(const real_matrix4x3 *matrix, real_quaternion *orientation)
{
    matrix3x3_rotation_to_quaternion(&matrix->rotation, orientation);
}
inline void camera_scripting_update_quaternions_interpolate(const real_quaternion *a,
    const real_quaternion *b, float t, real_quaternion *result)
{
    quaternions_slerp_interpolate(a, b, t, result);
    quaternion_normalize(result);
}
static __forceinline float camera_scripting_update_ticks_real(float seconds)
{
    return seconds * (float)game_time_globals->ticks_per_second;
}
inline real_vector3d *camera_scripting_update_subtract_vectors(const real_point3d *a, const real_point3d *b,
    real_vector3d *result)
{
    result->i = a->x - b->x;
    result->j = a->y - b->y;
    result->k = a->z - b->z;
    return result;
}
static __forceinline real_vector3d *camera_scripting_update_scale_vector(const real_vector3d *vector,
    float scale, real_vector3d *result)
{
    result->i = vector->i * scale;
    result->j = vector->j * scale;
    result->k = vector->k * scale;
    return result;
}
static __forceinline float camera_scripting_update_dot_product(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + a->j * b->j + a->k * b->k;
}
static __forceinline float camera_scripting_update_dot_product(const real_point3d *a, const real_vector3d *b)
{
    return a->i * b->i + a->j * b->j + a->k * b->k;
}
static __forceinline real_plane3d *camera_scripting_update_plane_from_point_and_normal(real_plane3d *plane,
    const real_point3d *point, const real_vector3d *normal)
{
    plane->normal = *normal;
    plane->distance = camera_scripting_update_dot_product(point, &plane->normal);
    return plane;
}
static __forceinline real_point3d *camera_scripting_update_point_from_line(const real_point3d *point,
    const real_vector3d *vector, float t, real_point3d *result)
{
    float product = vector->i * t;
    result->x = point->x + product;
    product = vector->j * t;
    result->y = point->y + product;
    product = vector->k * t;
    result->z = point->z + product;
    return result;
}
inline real_matrix3x3 *camera_scripting_update_matrix_from_vectors(real_matrix3x3 *matrix,
    const real_vector3d *forward, const real_vector3d *up)
{
    matrix->forward = *forward;
    cross_product3d(up, forward, &matrix->left);
    matrix->up = *up;
    return matrix;
}

static __forceinline float camera_scripting_update_arctangent(float y, float x)
{
    return (float)atan2(y, x);
}
static __forceinline float camera_scripting_update_get_speed()
{
    return game_time_globals->rate_scale;
}
static __forceinline long camera_scripting_update_get_time()
{
    return game_time_globals->time;
}
static __forceinline float camera_scripting_update_sine(float angle)
{
    return (float)sin(angle);
}
static __forceinline float camera_scripting_update_cosine(float angle)
{
    return (float)cos(angle);
}

// @address 0x291b40 ?scripted_camera_update@@YAXPAXPBUs_camera_control@@PAUs_observer_command@@@Z
void scripted_camera_update(void *, const s_camera_control *, s_observer_command *command_arg)
{
    s_observer_command *command = command_arg;
    bool valid = true;
    float speed = camera_scripting_update_get_speed();
    command->flags = 0;
    command->field_of_view = scripted_camera_get_field_of_view();
    command->flags |= 8;
    if (game_time_is_paused())
        command->flags |= 128;
    else
        command->flags &= ~128;
    s_camera_script_globals *state = g_camera_script_globals;
    switch ((short)state->mode) {
    case 1: {
        s_scripted_camera_state *scripted = camera_scripting_get_scripted_state();
        long object_index = state->object_index;
        if (object_index != -1) {
            object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, ~0u);
            if (object) {
                scripted_camera_get_relative_object_space(object_index, &command->focus_matrix);
                switch ((e_scenario_camera_type)scripted->camera_type) {
                case _scenario_camera_type_1:
                    command->focus_matrix.forward = *global_forward3d;
                    command->focus_matrix.up = *global_up3d;
                    command->focus_matrix.left = *global_left3d;
                default:
                    command->flags |= 0x100;
                    break;
                case _scenario_camera_type_2: {
                    s_scripted_camera_target *target = &scripted->target;
                    float scale;
                    real_plane3d plane;
                    command->focus_matrix.forward = *global_forward3d;
                    command->focus_matrix.up = *global_up3d;
                    command->focus_matrix.left = *global_left3d;
                    camera_scripting_update_plane_from_point_and_normal(&plane, &command->focus_matrix.position, &target->side);
                    scale = vector_intersect_plane3d(&target->position, &target->side, &plane);
                    camera_scripting_update_point_from_line(&target->position, &target->side, scale, &command->focus_matrix.position);
                    command->flags |= 0x100;
                    break;
                }
                }
            } else {
                valid = false;
            }
        }
        if (valid) {
            float duration = game_ticks_to_seconds((float)state->animation_duration);
            command->timer = speed != 0.0f ? duration / speed : 0.0f;
            command->forward = state->forward;
            command->up = state->up;
            if (state->object_index != -1) {
                float angle = camera_scripting_update_arctangent(command->forward.j, command->forward.i);
                float projected = camera_scripting_update_dot_product(&g_camera_script_globals->position, &command->forward);
                if (projected > 0.0f)
                    projected = 0.0f;
                command->position = *global_origin3d;
                float x = -(command->forward.i * projected);
                command->focus_distance = -projected;
                float y = -(command->forward.j * projected);
                float z = -(command->forward.k * projected);
                float cosine = camera_scripting_update_cosine(angle);
                command->focus_position.x = camera_scripting_update_sine(angle) * y + cosine * x;
                float sine = camera_scripting_update_sine(angle);
                cosine = camera_scripting_update_cosine(angle);
                command->focus_position.z = z;
                command->focus_position.y = sine * x - cosine * y;
                state = g_camera_script_globals;
                command->crosshair_type = state->object_index;
            } else {
                command->crosshair_type = -1;
            }
            command->position = state->position;
            command->flags |= 1;
        }
        break;
    }
    case 2: {
        s_camera_scripting_pan *pan = &state->pan;
        float scale = pan->navigator.evaluate(camera_scripting_update_get_time() - pan->time);
        command->timer = 0.0f;
        scale = CAMERA_PIN(scale, 0.0f, 1.0f);
        real_quaternion orientation;
        {
            real_matrix3x3 rotation;
            camera_scripting_update_matrix_from_vectors(&rotation, &state->forward, &state->up);
            matrix3x3_rotation_to_quaternion(&rotation, &orientation);
        }
        quaternions_slerp_interpolate(&orientation, &pan->orientation, scale, &orientation);
        quaternion_normalize(&orientation);
        real_matrix4x3 matrix;
        matrix4x3_from_point_and_quaternion(&matrix, global_origin3d, &orientation);
        command->forward = matrix.forward;
        command->up = matrix.up;
        camera_scripting_update_points_interpolate(&g_camera_script_globals->position, &pan->position,
            CAMERA_PIN(scale, 0.0f, 1.0f), &command->position);
        command->flags |= 1;
        break;
    }
    case 3: {
        float animation_time = 0.0f;
        real_matrix4x3 matrix;
        if (scripted_camera_get_deterministic_animated_camera_matrix(&matrix, &animation_time)) {
            command->focus_distance = 0.0f;
            command->timer = 0.0f;
            command->forward = matrix.forward;
            command->up = matrix.up;
            command->position = matrix.position;
            command->flags |= 25;
            command->crosshair_type = -1;
        }
        break;
    }
    case 4: {
        long object_index = scripted_camera_get_first_person_camera_object();
        if (object_index != -1)
            first_person_camera_for_unit_and_vector(object_index,
                &((unit_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)].datum)->unit.aiming_vector, command);
        break;
    }
    case 5: {
        s_camera_script_globals::s_camera_track *track = &state->camera_track;
        bool texture_camera = false;
        if (track->tag != -1) {
            s_cinematic_scene *definition = (s_cinematic_scene *)tag_get('cisc', track->tag);
            if (definition->shots.count > 0) {
                hs_cinematic_shot *shot = &((hs_cinematic_shot *)definition->shots.address)[CAMERA_PIN(track->shot_index, 0, definition->shots.count - 1)];
                if (shot->frame_count > 0) {
                    s_camera_frame *frame = &shot->frames[CAMERA_PIN(track->frame, 0, shot->frame_count - 1)];
                    {
                        real_matrix4x3 matrix;
                        scripted_camera_get_deterministic_cinematic_camera_matrix(frame, &matrix);
                        command->timer = 0.0f;
                        command->focus_distance = 0.0f;
                        command->forward = matrix.forward;
                        command->up = matrix.up;
                        command->position = matrix.position;
                    }
                    command->field_of_view = calculate_field_of_view(frame->field_of_view);
                    if (frame->depth_of_field)
                        command->depth_of_field.flags |= 1;
                    else
                        command->depth_of_field.flags &= ~1;
                    command->depth_of_field.values[0] = frame->depth_values[0];
                    command->depth_of_field.values[1] = frame->depth_values[1];
                    command->depth_of_field.values[2] = frame->depth_values[2];
                    command->depth_of_field.values[3] = frame->depth_values[3];
                    short easing = (short)track->easing;
                    if (easing == 1 && camera_scripting_update_ticks_real(hs_ticks_to_seconds(track->previous_frame)) < track->duration) {
                            real_point3d unused_position;
                            float scale = camera_scripting_update_ticks_real(hs_ticks_to_seconds(track->previous_frame)) / track->duration;
                            float weight = ((track->scale - 2.0f) * scale + (3.0f - track->scale)) * scale * scale;
                            command->position.x = (command->position.x - track->position.x) * weight + track->position.x;
                            command->position.y = (command->position.y - track->position.y) * weight + track->position.y;
                            command->position.z = (command->position.z - track->position.z) * weight + track->position.z;
                            real_matrix4x3 matrix;
                            matrix4x3_from_point_and_vectors(&matrix, global_origin3d, &command->forward, &command->up);
                            real_quaternion current_orientation;
                            camera_scripting_update_matrix_to_quaternion(&matrix, &current_orientation);
                            real_quaternion orientation;
                            camera_scripting_update_quaternions_interpolate(&track->orientation, &track->previous_orientation, weight, &orientation);
                            matrix4x3_from_point_and_quaternion(&matrix, global_origin3d, &orientation);
                            camera_scripting_update_matrix_to_vectors(&matrix, &unused_position, &command->forward, &command->up);
                    }
                    else if (easing == 2 &&
                        camera_scripting_update_ticks_real(hs_ticks_to_seconds(track->previous_frame)) < track->duration)
                    {
                            real_point3d unused_position;
                            float scale = camera_scripting_update_ticks_real(hs_ticks_to_seconds(track->previous_frame)) / track->duration;
                            real_vector3d delta;
                            camera_scripting_update_subtract_vectors(&track->position,
                                &track->previous_position, &delta);
                            command->position.x = (track->velocity.i - 2.0f * delta.i) * scale * scale * scale
                                + (3.0f * delta.i - 2.0f * track->velocity.i) * scale * scale
                                + track->velocity.i * scale + track->previous_position.x;
                            command->position.y = (track->velocity.j - 2.0f * delta.j) * scale * scale * scale
                                + (3.0f * delta.j - 2.0f * track->velocity.j) * scale * scale
                                + track->velocity.j * scale + track->previous_position.y;
                            command->position.z = (track->velocity.k - 2.0f * delta.k) * scale * scale * scale
                                + (3.0f * delta.k - 2.0f * track->velocity.k) * scale * scale
                                + track->velocity.k * scale + track->previous_position.z;
                            real_matrix4x3 matrix;
                            real_quaternion orientation;
                            camera_scripting_update_quaternions_interpolate(&track->previous_orientation, &track->orientation, scale, &orientation);
                            matrix4x3_from_point_and_quaternion(&matrix, global_origin3d, &orientation);
                            camera_scripting_update_matrix_to_vectors(&matrix, &unused_position, &command->forward, &command->up);
                    }
                }
                command->crosshair_type = -1;
                command->flags |= 25;
                for (long object_index = 0; object_index < definition->object_shots.count; ++object_index) {
                    s_object_shot_set *objects = (s_object_shot_set *)definition->object_shots.address;
                    s_object_shot_set *object = &objects[object_index];
                    if (object->shot_count > 0) {
                        s_object_shot *shot = &object->shots[CAMERA_PIN(track->shot_index, 0, object->shot_count - 1)];
                        if (shot->frame_count > 0) {
                            s_object_shot_frame *frame = &shot->frames[CAMERA_PIN(track->frame, 0, shot->frame_count - 1)];
                            if (object->type == 0x3cb && frame->flags) {
                                cinematic_camera_update_texture_camera(&frame->camera);
                                texture_camera = texture_camera || (frame->flags & 1) != 0;
                            }
                            if (track->target_name == object->name) {
                                real_matrix4x3 matrix;
                                matrix4x3_from_point_and_vectors(&matrix, &frame->camera.position, &frame->camera.forward, &frame->camera.up);
                                transform_into_flag_space(&matrix, track->cutscene_flag_index);
                                command->timer = 0.0f;
                                command->focus_distance = 0.0f;
                                command->forward = matrix.forward;
                                command->up = matrix.up;
                                command->position = matrix.position;
                                command->field_of_view = calculate_field_of_view(frame->camera.field_of_view);
                                if (frame->camera.depth_of_field)
                                    command->depth_of_field.flags |= 1;
                                else
                                    command->depth_of_field.flags &= ~1;
                                command->depth_of_field.values[0] = frame->camera.depth_values[0];
                                command->depth_of_field.values[1] = frame->camera.depth_values[1];
                                command->depth_of_field.values[2] = frame->camera.depth_values[2];
                                command->depth_of_field.values[3] = frame->camera.depth_values[3];
                            }
                        }
                    }
                }
            }
        }
        if (track->easing != 2) {
            long user_index = camera_scripting_first_active_user();
            if (user_index != -1) {
                const real_point3d *position = &g_observer_globals->blocks[user_index].camera_result.position;
                track->velocity.i = position->x - command->position.x;
                track->velocity.j = position->y - command->position.y;
                track->velocity.k = position->z - command->position.z;
                camera_scripting_update_scale_vector(&track->velocity, 30.0f, &track->velocity);
            } else {
                track->velocity = *global_zero_vector3d;
            }
        }
        if (texture_camera != track->previous_texture) {
            if (texture_camera) {
                g_render_texture_camera_globals->enabled = true;
            } else {
                g_render_texture_camera_globals->enabled = false;
                g_render_texture_camera_globals->dynamic_lights = false;
            }
        }
        break;
    }
    }
    g_camera_script_globals->animation_changed = false;
    command->parameter_timers[4] = 0.0f;
    command->parameter_flags[4] = 3;
    command->flags |= 0x800;
    observer_validate_camera_command(command);
}

t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_camera_script_globals_allocator> g_camera_script_globals_allocator;

