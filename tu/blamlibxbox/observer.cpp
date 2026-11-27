// blamlibxbox/observer.obj: from the records 0x88cd0 0xf67e8 0x12b340 0x12b648 0x12b8f8 0x12bce0 0x12be58 0x12c610 0x147198 0x1d2a40 0x1e5040 0x1fae40 0x202a30 0x202a50 0x202c80 0x202cf8 0x202d78 0x202e38 0x202e88 0x202ed0 0x202fa8 0x203208 0x2032f0 0x203438 0x203708 0x203950 0x203b00 0x203d10 0x2041e8 0x2043f0 0x204478 0x204940 0x204af8 0x204c40 0x204f30 0x205078 0x205490 0x205618 0x205690 0x2058b8 0x205948 0x2059e8 0x205a00 0x205a20 0x205ac0 0x205ad8 0x205af8 0x23e278
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "math/real_math_trigonometry.h"
#include "sound/sound_class_tls.h"
#include "camera/observer_gamestate_tls.h"
#include "physics/collision_test_vector.h"
#include "game/player_active_users.h"
#include "scenario/scenario_objects.h"
#include "objects/object_types.h"
#include <stddef.h>
#include "math/real_math_types.h"
#include "math/matrix_math_types.h"
#include "game/game_time_types.h"
#include "objects/object_reference_types.h"
#include "game/game_globals_storage.h"
#include <ppcintrinsics.h>
#include <math.h>
#include "multithreading/synchronization.h"
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "camera/observer.h"
#include "effects/player_effects.h"
#include "sound/game_sound_player_effects_types.h"
#include "game/game_engine_candy_monitor.h"
#include "physics/collision_features.h"
#include "physics/collisions.h"
#include "render/render_objects.h"
#include "game/game_time.h"
#include "physics/water_physics.h"
#include "render/render_lens_flares.h"
#include "math/matrix_math.h"
#include "scenario/scenario_map_variant.h"
#include "objects/object_placement.h"
#include "physics/collision_feature_types.h"
#include "physics/collision_types.h"
#include "camera/observer_depth_of_field_types.h"
#include "cache/cache_files.h"
#include "rasterizer/rasterizer_globals.h"
#include "physics/collision_result_types.h"
#include "game/player_mapping_types.h"
#include "memory/tls_globals.h"
#include "camera/observer_proximity.h"
#include "cubemaps/cubemap_sample.h"
#include "game/player_mapping_inline.h"
#include "camera/observer_globals_types.h"
#include "camera/director_globals_types.h"
#include "camera/camera_types.h"
#include "scenario/scenario_tag.h"
#include "objects/object_type_definition.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_registration.h"
#include "main/main_time_globals.h"
#include "multithreading/synchronization_inline.h"
#include "cache/restricted_memory_layout.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_regions_inline.h"
#include "math/real_math.h"
#include "camera/observer_command.h"
#include "game/game_simulation_predicates.h"
#include "cutscene/cinematic_globals_types.h"
#include <string.h>
#include "cseries/cseries.h"
#include "interface/first_person_weapon_tls.h"
#include "game/map_variant.h"
#include "objects/object_datum.h"
#include "tag_files/tag_types.h"
#include "game/game_engine_scoring_globals.h"
#include "objects/object_definitions.h"

// @address 0x11df87c g_observer_gamestate_globals
// @address 0x11df8cc ?g_observer_globals@@3PAUs_player_globals@@A
// Thread-local observer gamestate pointer owned by this unit.
extern "C" {
__declspec(thread) void *g_observer_gamestate_globals;
}
__declspec(thread) s_player_globals *g_observer_globals;
struct s_observer_gamestate_globals {
    unsigned long active_structure_bsp_mask;
    s_cluster_reference cluster_references[4];
};

struct s_observer : s_player_block {};

struct collision_line_test_result {
    long type;
    float fraction;
    real_point3d point;
    s_location start_location, location;
    const collision_bsp *bsp;
    c_global_material_type global_material;
    real_plane3d plane;
    long geometry_index, instance_index, object_index;
    short region_index, node_index;
    unsigned long bsp_reference;
    long structure_bsp_index, leaf_index, surface_index;
    unsigned long plane_index;
};
typedef char collision_line_test_result_size[(sizeof(void *) != 4 || ((sizeof(collision_line_test_result) == 0x58))) ? 1 : -1];

struct collision_plane;
// @address 0x1e5040 critical_section_scope_construct

struct s_observer_update {
    bool valid;
    unsigned char flags;
    unsigned char command_id;
    real_point3d position;
    real_vector3d forward;
    __int64 input_time;
};
typedef char observer_update_size[(sizeof(s_observer_update) == 0x28) ? 1 : -1];
typedef char observer_update_input_time_offset[(offsetof(s_observer_update, input_time) == 0x20) ? 1 : -1];
// @address 0x7e7c10 ?global_observer_update_list@@3PAUs_observer_update@@A
s_observer_update global_observer_update_list[4];

struct s_synchronization_globals;
namespace {
// Preserve the unit-local inline critical-section release.
#include "multithreading/critical_section_scope.h"
}

namespace {
static __forceinline s_player_globals *observer_globals_get()
{
    s_player_globals *globals = g_observer_globals;
    return globals;
}
static __forceinline s_player_block *observer_get(long user_index)
{
    s_player_globals *globals = observer_globals_get();
    return &globals->blocks[user_index];
}
static __forceinline __int64 main_time_get_input_collection_time()
{
    return g_main_time_globals->input_collection_milliseconds;
}
static __forceinline void rasterizer_lag_timing_mark_input_adjustment(__int64 input_time)
{
    g_rasterizer_timing_globals.current_record.input_adjustment_time = input_time;
}
static __forceinline long players_first_active_user()
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
}

extern "C" c_critical_section_scope *critical_section_scope_construct(c_critical_section_scope *scope, long critical_section)
{
    scope->m_critical_section = critical_section;
    scope->m_locked = false;
    if (scope->m_critical_section != -1)
        internal_critical_section_enter(scope->m_critical_section);
    scope->m_locked = true;
    return scope;
}

namespace {
static __forceinline long restricted_region_add_observer_member(const char *name, const char *type,
    unsigned int size, long alignment_bits)
{
    long member_index = g_restricted_regions[3].add_member(name, type, size, alignment_bits, __tls_set_g_observer_globals, 0, 0);
    void *address = restricted_region_get_member_address(3, member_index);
    g_observer_globals = (s_player_globals *)address;
    return member_index;
}
static __forceinline long restricted_region_add_observer_gamestate_member(const char *name, const char *type,
    unsigned int size, long alignment_bits)
{
    long member_index = g_restricted_regions[3].add_member(name, type, size, alignment_bits,
        __tls_set_g_render_object_globals_allocator, 0, 0);
    void *address = restricted_region_get_member_address(3, member_index);
    g_observer_gamestate_globals = address;
    return member_index;
}
}

// 0x88cd0
const s_observer_result *observer_try_and_get_camera(long user_index)
{
    const s_observer_result *result = 0;
    if (user_index != -1) {
        bool player_valid = players_user_is_active(user_index);
        if (player_valid) {
            if (g_active_structure_bsp_mask != 0) {
                s_player_block *block = &g_observer_globals->blocks[user_index];
                if (block->camera_result.location.cluster_reference.bsp_index != -1)
                    result = &block->camera_result;
            }
        }
    }
    return result;
}

// 0xf67e8
bool active_user_close_to_point_not_deterministic(const real_point3d *point, float radius, e_adjust_for_fov_scale adjust)
{
    bool result = false;
    float radius_squared = radius * radius;
    for (long user_index = players_first_active_user(); user_index != -1; user_index = players_next_active_user(user_index)) {
        const s_observer_result *camera = &observer_get(user_index)->camera_result;
        const real_point3d *camera_position = &camera->position;
        float distance_squared = distance_squared3d(point, camera_position);
        if (distance_squared < radius_squared) {
            result = true;
            break;
        }
    }
    return result;
}

namespace {
static __forceinline bool valid_world_real(float value)
{
    bool result = valid_real(value) && value >= -50000.0f && value <= 50000.0f;
    return result;
}
}
// @address 0x12b8f8 ?valid_world_real_point3d@@YA_NPATreal_point3d@@@Z
bool valid_world_real_point3d(real_point3d *point)
{
    bool result = valid_world_real(point->x) && valid_world_real(point->y) && valid_world_real(point->z);
    return result;
}

// @address 0x25888 k_observer_default_aspect_ratio
static __forceinline bool observer_finite_value(float value)
{
    union { float real; long bits; } representation;
    representation.real = value;
    return (representation.bits & 0x7f800000) != 0x7f800000;
}
// @address 0x205af8 ?valid_real_point3d@@YA_NPBTreal_point3d@@@Z
// real_math.h inline (COMDAT): out-of-line copy emitted after observer.obj, register-opaque to every caller
// match: 0x12b340: inline keeps observer_validate_camera_command identical; out of line 5 of its words change
#if defined(_XBOX) && !defined(__clang__)
inline bool valid_real_point3d(const real_point3d *point)
#else
bool valid_real_point3d(const real_point3d *point)
#endif
{
    return observer_finite_value(point->x) && observer_finite_value(point->y) && observer_finite_value(point->z);
}
namespace {
static __forceinline bool observer_command_in_range(float value, float low, float high)
{
    int bits[1];
    *(float *)bits = value;
    return !((bits[0] & 0x7f800000) == 0x7f800000) && !(value < low) && value <= high;
}
}
// @address 0x12b648 observer_valid_camera_command
// match: 0x12b648: valid_real_vector3d is folded into valid_real_point3d, so the velocity goes through the point helper.
extern "C" bool observer_valid_camera_command(s_observer_command *command)
{
    return command != 0
        && ((command->flags & 1) == 0
            || (valid_real_vector3d_axes2(&command->forward, &command->up)
                && valid_world_real_point3d(&command->position)
                && valid_world_real_point3d(&command->focus_position)
                && valid_real_point3d(&command->velocity_as_point)
                && observer_command_in_range(command->focus_distance, k_real_zero_storage, k_observer_command_focus_distance_max)
                && observer_command_in_range(command->field_of_view, k_observer_field_of_view_minimum, k_observer_field_of_view_maximum)
                && observer_command_in_range(command->timer, k_real_zero_storage, 3600.0f)));
}

// 0x12c610
void observer_post_global_update_list()
{
    c_critical_section_scope lock(49);
    for (long user_index = 0; user_index < 4; ++user_index) {
        s_player_block *observer = observer_get(user_index);
        global_observer_update_list[user_index].valid = false;
        if (observer) {
            global_observer_update_list[user_index].command_id = observer->command_id;
            global_observer_update_list[user_index].flags = observer->flags;
            global_observer_update_list[user_index].position = observer->position;
            global_observer_update_list[user_index].forward = observer->forward;
            global_observer_update_list[user_index].valid = true;
            global_observer_update_list[user_index].input_time = main_time_get_input_collection_time();
        }
    }
}

// 0x147198
void observer_adopt_global_update_list()
{
    c_critical_section_scope lock(49);
    for (long user_index = 0; user_index < 4; ++user_index) {
        if (global_observer_update_list[user_index].valid) {
            s_player_block *observer = observer_get(user_index);
            if (observer && observer->command_id == global_observer_update_list[user_index].command_id) {
                observer->position = global_observer_update_list[user_index].position;
                observer->forward = global_observer_update_list[user_index].forward;
                rasterizer_lag_timing_mark_input_adjustment(global_observer_update_list[user_index].input_time);
            }
        }
    }
}

// 0x1fae40
void __tls_set_g_observer_gamestate_globals_allocator(void *allocator)
{
    g_sound_class_data = (sound_class_datum *)allocator;
}

// @address 0x202a30 __tls_set_g_observer_globals
void __tls_set_g_observer_globals(void *address)
{
    g_observer_globals = (s_player_globals *)address;
}

// 0x202c80
void observer_clear_global_update_list()
{
    c_critical_section_scope lock(49);
    for (long user_index = 0; user_index < 4; ++user_index)
        global_observer_update_list[user_index].valid = false;
}

// 0x205948
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_observer_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x205948: value range assumption guides branch lowering
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_observer_member(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// 0x205a20
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_observer_gamestate_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x205a20: value range assumption guides branch lowering
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_observer_gamestate_member(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// 0x202cf8
void observer_initialize()
{
    g_observer_globals = (s_player_globals *)g_observer_globals_allocator.reserve_memory("observer globals", 0,
        sizeof(s_player_globals), 0);
    g_observer_gamestate_globals = g_observer_gamestate_globals_allocator.reserve_memory("observer gamestate globals", 0, 12, 0);
    observer_clear_global_update_list();
}

// @address 0x202d78 ?observer_initialize_before_saved_game@@YAXJ@Z
void observer_initialize_before_saved_game(long game_state)
{
    bool before_saved_game = (game_state & 8) == 0;
    if (before_saved_game) {
        s_observer_gamestate_globals *gamestate = (s_observer_gamestate_globals *)g_observer_gamestate_globals;
        gamestate->active_structure_bsp_mask = g_active_structure_bsp_mask;
        for (long user_index = 0; user_index < 4; ++user_index) {
            cluster_reference_invalidate(&gamestate->cluster_references[user_index]);
            if (players_user_is_active(user_index)) {
                s_player_block *observer = observer_get(user_index);
                s_location location;
                scenario_location_from_point(&location, &observer->camera_result.position);
                gamestate->cluster_references[user_index] = location.cluster_reference;
            }
        }
    }
}

namespace {
static __forceinline bool game_is_networked()
{
    bool result = false;
    long simulation = game_simulation::game_simulation_get();
    if (simulation >= 2 && simulation <= 5)
        result = true;
    return result;
}
static __forceinline void observer_block_for_one_frame()
{
    s_player_globals *globals = observer_globals_get();
    globals->block_one_frame = true;
}
}
// 0x202e88
void observer_initialize_after_load_saved_game(long game_state)
{
    if (!game_is_networked())
        observer_block_for_one_frame();
}

// 0x202ed0
void observer_initialize_for_new_structure_bsp(unsigned long structure_bsp_index)
{
    for (long user_index = 0; user_index < 4; ++user_index) {
        if (players_user_is_active(user_index)) {
            s_player_block *observer = observer_get(user_index);
            scenario_location_from_point(&observer->camera_result.location, &observer->camera_result.position);
        }
    }
    bool cinematic_in_progress = false;
    if (cinematic_globals)
        cinematic_in_progress = cinematic_globals->in_progress;
    if (!cinematic_in_progress && game_time_globals->time > 0)
        g_observer_globals->flag = true;
}


// @address 0x2043f0 ?observer_compute_collision_radius@@YAMMM@Z
float observer_compute_collision_radius(float horizontal_fov, float vertical_fov)
{
    float scale = global_z_near;
    float tan_vertical = (float)tan(vertical_fov * 0.5f);
    float tan_horizontal = (float)tan(horizontal_fov * 0.5f);
    return sqrtf(tan_horizontal * tan_horizontal + tan_vertical * tan_vertical + 1.0f) * scale;
}

namespace {
// k-first evaluation order; the shared magnitude_squared3d sums i-first and schedules differently here.
static __forceinline float magnitude_squared3d_k_first(const real_vector3d *v)
{
    return v->k * v->k + (v->i * v->i + v->j * v->j);
}
}
// 0x204af8
// @address 0x204af8 ?observer_fix_focus_position_to_safe_position@@YAXJPATreal_point3d@@JJPBT1@@Z
void observer_fix_focus_position_to_safe_position(
    long object_index, union real_point3d *point, long first_ignore, long second_ignore, union real_point3d const *origin)
{
    s_collision_test_flags flags = observer_get_collision_flags(object_index);
    flags.collision_flags &= ~2;
    long type;
    if (collision_test_point(flags, point, first_ignore, second_ignore, (e_collision_result_type *)&type)) {
#if defined(_XBOX) && !defined(__clang__)
        collision_line_test_result result;
#else
        collision_result result;
#endif
        if (collision_test_line(flags, origin, point, first_ignore, second_ignore, (collision_result *)&result)) {
            real_vector3d vector;
            vector_from_points3d(point, origin, &vector);
            *point = result.point;
            if (magnitude_squared3d_k_first(&vector) > 0.05f * 0.05f && normalize3d(&vector) != 0.0f)
                point_from_line3d(point, &vector, 0.05f, point);
        }
    }
}

// @address 0x205618 observer_test_collision_line
extern "C" bool observer_test_collision_line(long user_index, const real_point3d *point0, const real_point3d *point1,
    long first_ignore_object_index, long second_ignore_object_index, float *fraction, bool underwater)
{
#if defined(_XBOX) && !defined(__clang__)
    long line_collision_storage[26];
    collision_result *line_collision = (collision_result *)line_collision_storage;
#else
    collision_result line_collision_storage;
    collision_result *line_collision = &line_collision_storage;
#endif
    bool result = false;
    line_collision->global_material.m_index = -1;
    s_collision_test_flags flags = observer_get_collision_flags(user_index);
    if (collision_test_line(flags, point0, point1, first_ignore_object_index, second_ignore_object_index, line_collision)) {
        *fraction = line_collision->fraction;
        result = true;
    }
    return result;
}


static __forceinline float observer_arctan(float angle) { return (float)::atan((double)angle); }
static __forceinline float observer_tangent(float angle) { return (float)tan(angle); }

// @address 0x205690 ?observer_result_compute_parameters@@YAXPAUs_observer_result@@@Z
void observer_result_compute_parameters(s_observer_result *result)
{
    float half_tangent = observer_tangent(0.5f * result->horizontal_field_of_view);
    result->vertical_field_of_view = 2.0f * arctangent(half_tangent / result->aspect_ratio, 1.0f);
    result->field_of_view_scale = result->horizontal_field_of_view / g_observer_default_field_of_view;
    float vertical_half = 0.5f * result->vertical_field_of_view;
    float vertical_tangent = observer_tangent(vertical_half);
    float horizontal_angle = observer_arctan(0.0f);
    float vertical_angle = observer_arctan(vertical_tangent * result->view_offset.vertical);
    real_vector3d original_forward = result->forward;
    {
        real_matrix3x3 rotation;
        matrix3x3_from_axis_and_angle(&rotation, &k_real_math_identity_matrix_storage.up, sine(horizontal_angle), cosine(horizontal_angle));
        matrix3x3_transform_vector(&rotation, &result->forward, &result->forward);
        matrix3x3_transform_vector(&rotation, &result->up, &result->up);
    }
    real_vector3d axis;
    cross_product3d(&result->forward, &result->up, &axis);
    {
        real_matrix3x3 rotation;
        matrix3x3_from_axis_and_angle(&rotation, &axis, sine(vertical_angle), cosine(vertical_angle));
        matrix3x3_transform_vector(&rotation, &result->forward, &result->forward);
        matrix3x3_transform_vector(&rotation, &result->up, &result->up);
    }
    float dot = dot_product3d(&original_forward, &result->forward);
    dot = dot > 0.0001f ? dot : 0.0001f;
    scale_vector3d(&original_forward, 1.0f / dot, &original_forward);
    real_vector3d difference;
    subtract_vectors3d(&original_forward, &result->forward, &difference);
    result->direction_error = magnitude3d(&difference);
}

// match: 0x2058b8: optimize pragma preserves function frame layout
#pragma optimize("s", on)
// @address 0x2058b8 ?observer_get_collision_flags@@YA?AUs_collision_test_flags@@H@Z
s_collision_test_flags observer_get_collision_flags(int index)
{
    s_director_storage *entry = &tls_director_globals()->directors[index];
    unsigned bits = 0x2180;
    if (!entry || !&entry->get_director()->m_camera || entry->get_director()->get_camera()->get_type() != _camera_mode_dead)
        bits = 0x2984;
    s_collision_test_flags mask;
    mask.collision_flags = 0x100000;
    mask.object_flags = bits | 1;
    s_collision_test_flags result;
    result.collision_flags = k_collision_test_structure_and_water.collision_flags | mask.collision_flags;
    result.object_flags = k_collision_test_structure_and_water.object_flags | mask.object_flags;
    return result;
}
// match: 0x2058b8: optimize pragma preserves function frame layout
#pragma optimize("", on)

// 0x2059e8
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_observer_globals_allocator>::allocate(unsigned int size,
    const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x205a00
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_observer_globals_allocator>::deallocate(void *address)
{
    g_observer_globals = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x205ac0
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_observer_gamestate_globals_allocator>::allocate(unsigned
    int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x205ad8
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_observer_gamestate_globals_allocator>::deallocate(void *address)
{
    g_observer_gamestate_globals = 0;
    restricted_region_free_member(3, m_member_index);
}

// 0x23e278
void __tls_set_g_observer_globals_allocator(void *allocator)
{
    g_player_effects_globals = (s_player_effects_globals *)allocator;
}

static const long k_observer_state_count = 13;

// @rdata 0x820242d8
static const float k_observer_maximum_parameter_time = 2.0f;

static __forceinline float observer_update_pin_to_maximum(float value)
{
    if (value > k_observer_maximum_parameter_time)
        value = k_observer_maximum_parameter_time;
    return value;
}

static __forceinline real_point3d *observer_update_matrix4x3_transform_point(const real_matrix4x3 *m, const real_point3d *p,
    real_point3d *result)
{
    float x = p->x * m->scale;
    float y = p->y * m->scale;
    float z = p->z * m->scale;
    float rx = x * m->forward.i + (z * m->up.i + y * m->left.i);
    float ry = x * m->forward.j + (z * m->up.j + y * m->left.j);
    float rz = x * m->forward.k + (z * m->up.k + y * m->left.k);
    result->x = rx + m->position.x;
    result->y = ry + m->position.y;
    result->z = rz + m->position.z;
    return result;
}

// @address 0x203438 ?observer_update_command@@YAXJ@Z
void observer_update_command(long observer_index)
{
    s_observer *observer = (s_observer *)observer_get(observer_index);
    float *previous_values = observer->previous.parameter_timers;
    float *values = observer->command->parameter_timers;
    unsigned char *value_flags = observer->command->parameter_flags;
    s_observer_command *command;

    observer_validate_camera_command(observer->command);

    if ((observer->command->flags & 1) != 0) {
        for (short i = 0; i < 6; i++) {
            if ((*value_flags & 1) != 0) {
                if ((*value_flags & 2) == 0 && *values < *previous_values)
                    *values = observer_update_pin_to_maximum(*previous_values);
            } else if (observer->command->timer < *previous_values && (observer->command->flags & 8) == 0) {
                *values = observer_update_pin_to_maximum(*previous_values);
            } else {
                *values = observer->command->timer;
            }
            values++;
            previous_values++;
            value_flags++;
        }

        bool previous_using_matrix = observer->using_matrix;
        if (previous_using_matrix || ((command = observer->command)->flags & 0x100) != 0) {
            if (((observer->command->flags & 0x100) != 0) != previous_using_matrix
                || observer->command->crosshair_type != observer->previous.crosshair_type) {
                real_matrix4x3 matrix;
                if ((observer->command->flags & 0x100) != 0)
                    matrix4x3_inverse(&observer->command->focus_matrix, &matrix);
                else
                    memcpy(&matrix, &k_real_math_identity_matrix_storage, sizeof(matrix));
                if (previous_using_matrix)
                    matrix4x3_compute_product(&matrix, &observer->matrix, &matrix);
                observer_update_matrix4x3_transform_point(&matrix, &observer->focus_point, &observer->focus_point);
                matrix4x3_transform_vector(&matrix, &observer->motion_forward, &observer->motion_forward);
                matrix4x3_transform_vector(&matrix, &observer->motion_up, &observer->motion_up);
                matrix4x3_transform_vector(&matrix, &observer->velocities.linear, &observer->velocities.linear);
                matrix4x3_transform_vector(&matrix, &observer->velocities.angular, &observer->velocities.angular);
                matrix4x3_transform_vector(&matrix, &observer->accelerations.linear, &observer->accelerations.linear);
                matrix4x3_transform_vector(&matrix, &observer->accelerations.angular, &observer->accelerations.angular);
                matrix4x3_transform_vector(&matrix, &observer->displacements.linear, &observer->displacements.linear);
                matrix4x3_transform_vector(&matrix, &observer->displacements.angular, &observer->displacements.angular);
            }
            command = observer->command;
            observer->using_matrix = (command->flags & 0x100) != 0;
            if (observer->using_matrix)
                observer->matrix = command->focus_matrix;
        }
        observer->focus_offset_in_view_space = (command->flags & 0x200) != 0;
        memcpy(&observer->previous, command, sizeof(s_observer_command));
    }
}

// @address 0x745ea4 k_observer_parameter_element_counts
static short k_observer_parameter_element_counts[6] = { 3, 3, 2, 1, 1, 6 };
// @address 0x745eb0 k_observer_parameter_motion_counts
static short k_observer_parameter_motion_counts[6] = { 3, 3, 2, 1, 1, 3 };

static __forceinline void observer_motion_build_matrix(const float *forward, const float *up, real_matrix4x3 *matrix)
{
    matrix->scale = 1.0f;
    memcpy(&matrix->forward, forward, sizeof(real_vector3d));
    matrix->left.i = forward[2] * up[1] - forward[1] * up[2];
    matrix->left.j = up[2] * forward[0] - forward[2] * up[0];
    matrix->left.k = forward[1] * up[0] - up[1] * forward[0];
    memcpy(&matrix->up, up, sizeof(real_vector3d));
    matrix->position.x = 0.0f;
    matrix->position.y = 0.0f;
    matrix->position.z = 0.0f;
}

// @address 0x2041e8 observer_compute_motion_delta
extern "C" void observer_compute_motion_delta(const float *before, const float *after, float *delta)
{
    short index = 0;
    for (; index < 10; ++index)
        *delta++ = *after++ - *before++;
    for (; index < 16; index += 6)
    {
        real_matrix4x3 before_matrix;
        real_matrix4x3 after_matrix;
        real_matrix4x3 inverse;
        real_matrix4x3 relative;
        real_quaternion rotation;
        float angle;
        observer_motion_build_matrix(&before[0], &before[3], &before_matrix);
        observer_motion_build_matrix(&after[0], &after[3], &after_matrix);
        matrix4x3_inverse(&before_matrix, &inverse);
        matrix4x3_compute_product(&after_matrix, &inverse, &relative);
        matrix3x3_rotation_to_quaternion(&relative.rotation, &rotation);
        quaternion_to_angle_and_vector(&rotation, &angle, (real_vector3d *)delta);
        delta[0] *= angle;
        delta[1] *= angle;
        delta[2] *= angle;
        before += 6; after += 6; delta += 3;
    }
}

// @address 0x12be58 ?observer_initialize_for_new_map@@YAXXZ
void observer_initialize_for_new_map()
{
    observer_globals_get()->initialized = true;
    for (long user_index = 0; user_index < 4; ++user_index)
        observer_clear((s_observer *)observer_get(user_index));
    observer_clear_global_update_list();
    s_observer_gamestate_globals *gamestate = (s_observer_gamestate_globals *)g_observer_gamestate_globals;
    gamestate->active_structure_bsp_mask = 0;
    for (long user_index = 0; user_index < 4; ++user_index)
        cluster_reference_invalidate(&gamestate->cluster_references[user_index]);
}

// @address 0x5bec98 ?observer_dispose@@YAXXZ
void observer_dispose(void)
{
}

// @address 0x5bec98 ?observer_dispose_from_old_map@@YAXXZ
void observer_dispose_from_old_map(void)
{
}

// @address 0x5bec98 ?observer_dispose_from_old_structure_bsp@@YAXK@Z
void observer_dispose_from_old_structure_bsp(unsigned long)
{
}
// @address 0x202e38 ?observer_initialize_for_saved_game@@YAXJ@Z
void observer_initialize_for_saved_game(long game_state)
{
    observer_globals_get()->initialized = true;
    for (long user_index = 0; user_index < 4; ++user_index)
        observer_clear((s_observer *)observer_get(user_index));
    observer_clear_global_update_list();
}

// @rdata 0x82035d8c k_observer_world_minimum
static const float k_observer_world_minimum = -50000.0f;
// @rdata 0x82035d90 k_observer_world_maximum
static const float k_observer_world_maximum = 50000.0f;
// @rdata 0x82021694 k_observer_focus_distance_maximum
static const float k_observer_focus_distance_maximum = 3.4028234663852886e+38f;

static __forceinline void observer_validate_point(real_point3d *point)
{
    if (!valid_real_point3d(point))
        *point = *global_origin3d;
}

// @address 0x12b340 ?observer_validate_camera_command@@YAXPAUs_observer_command@@@Z
void observer_validate_camera_command(s_observer_command *command)
{
    bool valid = observer_valid_camera_command(command);
    if (!valid) {
        if (!valid_real_vector3d_axes2(&command->forward, &command->up)) {
            command->forward = global_identity4x3->forward;
            command->up = global_identity4x3->up;
        }
        command->position.x = PIN(command->position.x, k_observer_world_minimum, k_observer_world_maximum);
        command->position.y = PIN(command->position.y, k_observer_world_minimum, k_observer_world_maximum);
        command->position.z = PIN(command->position.z, k_observer_world_minimum, k_observer_world_maximum);
        command->focus_position.x = PIN(command->focus_position.x, k_observer_world_minimum, k_observer_world_maximum);
        command->focus_position.y = PIN(command->focus_position.y, k_observer_world_minimum, k_observer_world_maximum);
        command->focus_position.z = PIN(command->focus_position.z, k_observer_world_minimum, k_observer_world_maximum);
        // match: 0x12b340: valid_real_vector3d is folded into valid_real_point3d, so the velocity goes through the point helper.
        observer_validate_point(&command->velocity_as_point);
        command->field_of_view = PIN(command->field_of_view, k_observer_field_of_view_minimum,
            k_observer_field_of_view_maximum);
        command->focus_distance = PIN(command->focus_distance, 0.0f, k_observer_command_focus_distance_max);
        valid = observer_valid_camera_command(command);
    }
    if (!valid) {
        memset(command, 0, sizeof(*command));
        command->forward = global_identity4x3->forward;
        command->up = global_identity4x3->up;
        command->field_of_view = g_observer_default_field_of_view;
    }
}

// @address 0x203208 ?observer_result_clear@@YAXPAUs_observer_result@@@Z
void observer_result_clear(s_observer_result *observer_result)
{
    s_observer_result_fields *result = (s_observer_result_fields *)observer_result;
    result->position = *global_origin3d;
    long *location = &result->location;
    *location = -1;
    result->velocity = *global_zero_vector3d;
    result->forward = *global_forward3d;
    result->up = *global_up3d;
    result->horizontal_field_of_view = g_observer_default_field_of_view;
    result->aspect_ratio = k_observer_default_aspect_ratio;
    float *horizontal_offset = &result->view_offset.horizontal;
    *horizontal_offset = 0.0f;
    result->view_offset.vertical = 0.0f;
    unsigned long *flags = &result->depth_of_field.flags;
    *flags &= ~1UL;
    observer_result_compute_parameters(observer_result);
}

// @address 0x2032f0 ?observer_clear@@YAXPAUs_observer@@@Z
void observer_clear(s_observer *observer)
{
    observer->motion_forward = *global_forward3d;
    observer->motion_up = *global_up3d;
    observer->field_of_view = g_observer_default_field_of_view;
    memset(observer->velocities.all, 0, k_observer_state_count * sizeof(float));
    memset(observer->accelerations.all, 0, k_observer_state_count * sizeof(float));
    observer->result_valid = false;
    observer_result_clear((s_observer_result *)&observer->result);
    s_observer_command *previous = &observer->previous;
    memset(previous, 0, sizeof(*previous));
    previous->forward = *global_forward3d;
    previous->up = *global_up3d;
    previous->field_of_view = g_observer_default_field_of_view;
    observer->interpolation = 1.0f;
    observer->interpolation_rate = 0.0f;
    observer->trailer_signature = 'rad!';
    observer->header_signature = 'rad!';
    observer->updated = true;
    observer->pending = false;
    observer->using_matrix = false;
    observer->focus_offset_in_view_space = false;
    observer->parent_object_index = -1;
    observer->parent_unit_index = -1;
}

static __forceinline const real_matrix4x3 *observer_camera_effect_get_weapon_matrix(long user_index)
{
    fpw_weapon *state = &tls_first_person_weapon_data()[user_index].weapons[0];
    if (state->hands_node_count != -1 && (state->flags & 16) != 0)
        return &state->camera_adjustment;
    return 0;
}

static __forceinline void observer_camera_effect_matrix_from_point_and_vectors(real_matrix4x3 *matrix,
    const real_point3d *point, const real_vector3d *forward, const real_vector3d *up)
{
    matrix4x3_rotation_from_vectors(matrix, forward, up);
    matrix->position = *point;
}

// @address 0x202a50 ?observer_apply_camera_effect@@YAXJ@Z
void observer_apply_camera_effect(long user_index)
{
    real_matrix4x3 effect_matrix;
    memcpy(&effect_matrix, global_identity4x3, sizeof(effect_matrix));
    s_player_block *observer = (s_player_block *)observer_get(user_index);
    if (user_index != -1) {
        if (tls_director_globals()->slots[user_index].perspective == 0) {
            const real_matrix4x3 *weapon_matrix = observer_camera_effect_get_weapon_matrix(user_index);
            if (weapon_matrix)
                matrix4x3_compute_product(&effect_matrix, weapon_matrix, &effect_matrix);
        }
        if (!game_time_is_paused() && (unsigned long)tls_director_globals()->slots[user_index].perspective < 3)
            player_effect_apply_camera_effect_matrix(user_index, &effect_matrix);
    }
    real_point3d position = observer->result.position;
    real_vector3d forward = observer->result.forward;
    real_vector3d up = observer->result.up;
    real_matrix4x3 camera_matrix;
    observer_camera_effect_matrix_from_point_and_vectors(&camera_matrix, &position, &forward, &up);
    matrix4x3_compute_product(&camera_matrix, &effect_matrix, &camera_matrix);
    position = camera_matrix.position;
    forward = camera_matrix.forward;
    up = camera_matrix.up;
    if (!valid_real_vector3d_axes2(&forward, &up)) {
        normalize3d_with_default(&forward, global_forward3d);
        normalize3d_with_default(&up, global_forward3d);
        if (!valid_real_vector3d_axes2(&forward, &up)) {
            forward = *global_forward3d;
            up = *global_up3d;
        }
    }
    observer->result.position = position;
    observer->result.forward = forward;
    observer->result.up = up;
}

static __forceinline real_point3d *observer_point_add_vector3d(const real_point3d *p, const real_vector3d *v, real_point3d *result)
{
    result->x = p->x + v->i;
    result->y = p->y + v->j;
    result->z = p->z + v->k;
    return result;
}
// @address 0x205490 observer_fix_point_for_collision
extern "C" void observer_fix_point_for_collision(real_point3d *point, const real_vector3d *direction, const real_point3d *start,
    float distance, float radius)
{
    int features[12548];
    float result[12];
    real_point3d end;
    real_vector3d vector;
    union
    {
        s_collision_test_flags flags;
        unsigned __int64 flags_as_bits;
    };

    flags.collision_flags = 0x100001;
    flags.object_flags = 0;
    end.x = direction->i * (distance * -1.0f) + point->x;
    end.y = direction->j * (distance * -1.0f) + point->y;
    end.z = direction->k * (distance * -1.0f) + point->z;
    if (collision_get_features_in_sphere(flags_as_bits, &end, radius, 0.0f, radius, -1, -1, (collision_feature_list
        *)features)) {
        vector_from_points3d(start, &end, &vector);
        if (((unsigned char)::collision_features_test_vector((collision_feature_list const *)features, (real_point3d const *)start,
                (real_vector3d const *)&vector, (collision_plane *)result)))
        {
            real_point3d hit;
            point_from_line3d(start, &vector, result[4], &hit);
            real_vector3d offset;
            vector_from_points3d(&end, &hit, &offset);
            observer_point_add_vector3d(point, &offset, point);
        }
    }
}

static __forceinline float observer_plane_dot_product3d(const real_vector3d *a, const real_vector3d *b)
{
    return a->i * b->i + (a->k * b->k + a->j * b->j);
}
static const float k_observer_interpolation_speed = 2.0f;
static __forceinline short observer_collision_test_planes(const real_point3d *point, const real_vector3d *forward, float scale,
    collision_plane *planes)
{
    s_collision_test_flags flags = k_collision_test_structure;
    flags.collision_flags &= ~2;
    real_point3d start;
    start.x = point->x - forward->i * 0.35f;
    start.y = point->y - forward->j * 0.35f;
    start.z = point->z - forward->k * 0.35f;
    real_vector3d delta;
    delta.i = forward->i * scale;
    delta.j = forward->j * scale;
    delta.k = forward->k * scale;
    real_point3d end_point;
    real_vector3d end_delta;
    return collision_move_point_with_features(flags, &start, &delta, 0.0f, 0.35f, -1, -1, &end_point, &end_delta, 16, planes);
}
// @address 0x204c40 observer_update_collision_distance
extern "C" void observer_update_collision_distance(long user_index, const real_point3d *point, const real_vector3d *direction, float radius,
    // match: 0x204c40: pointer __restrict qualifier prevents alias reload
    float *__restrict distance, real_vector3d *vector, real_point3d *position, float dt)
{
    s_observer *observer = (s_observer *)observer_get(user_index);
    real_vector3d forward = *direction;
    normalize3d(&forward);
    if (TEST_BIT(observer->command->flags, 12))
    {
        collision_plane planes[16];
        if (observer->interpolation * *distance > 0.35f && observer_collision_test_planes(point, &forward, (*distance - 0.35f) *
            -1.0f, planes) > 0)
        {
            const collision_plane *plane = &planes[0];
            float offset = *distance > 0.0f ? 0.315f / *distance : 0.0f;
            float facing = 1.0f - fabsf(observer_plane_dot_product3d(&plane->plane.normal, &forward));
            float target = PIN(plane->t + offset, 0.0f, 1.0f);
            float interpolation = observer->interpolation;
            observer->interpolation = (target - interpolation) * facing * dt * k_observer_interpolation_speed + interpolation;
            observer->interpolation_rate = observer->interpolation - interpolation;
        }
        else if (observer->interpolation < 1.0f)
        {
            observer->interpolation_rate = (dt * k_observer_interpolation_speed - observer->interpolation_rate) * dt +
                observer->interpolation_rate;
            observer->interpolation = PIN(observer->interpolation_rate + observer->interpolation, 0.0f, 1.0f);
        }
        else
        {
            observer->interpolation_rate = 0.0f;
            observer->interpolation = 1.0f;
        }
        *distance *= observer->interpolation;
    }
    else
    {
        observer->interpolation_rate = 0.0f;
        observer->interpolation = 1.0f;
    }
    scale_vector3d(&forward, *distance * -1.0f, vector);
    observer_point_add_vector3d(point, vector, position);
}

static const float k_observer_water_surface_offset = 0.03f;
// @address 0x204f30 observer_clamp_to_water_surface
extern "C" void observer_clamp_to_water_surface(const real_point3d *origin, real_vector3d *direction, real_point3d *position,
    float distance, float radius)
{
    real_point3d point;
    if (direction)
        point_from_line3d(origin, direction, -distance, &point);
    else
        point = *position;
    float z = point.z - k_observer_water_surface_offset;
    point.z = z;
    float water_height = water_physics_get_depth_at_point(&point);
    if (water_height != -3.4028234663852886e+38f)
    {
        float surface_z = water_height + z;
        float offset = 0.0f;
        float bounds[2] = { surface_z + k_observer_water_surface_offset, surface_z - k_observer_water_surface_offset };
        if (z > surface_z)
            offset = bounds[0] - z;
        else if (z > bounds[1])
            offset = bounds[1] - z;
        point.z = offset + z;
        if (direction)
        {
            direction->i = origin->x - point.x;
            direction->j = origin->y - point.y;
            direction->k = origin->z - point.z;
            normalize3d(direction);
        }
        else
        {
            *position = point;
        }
    }
}

static __forceinline bool observer_collision_test_with_t(long user_index, const real_point3d *point0, const real_point3d *point1,
    long first_ignore_object_index, long second_ignore_object_index, float *fraction, bool underwater)
{
#if defined(_XBOX) && !defined(__clang__)
    collision_line_test_result collision;
#else
    collision_result collision;
#endif
    bool result = false;
    s_collision_test_flags flags = observer_get_collision_flags(user_index);
    real_vector3d vector;
    vector_from_points3d(point0, point1, &vector);
    if (collision_test_vector(flags, false, point0, &vector, first_ignore_object_index, second_ignore_object_index, -1,
        (collision_result *)&collision)) {
        *fraction = collision.fraction;
        result = true;
    }
    return result;
}
static __forceinline bool observer_collision_test_offset(long user_index, const real_point3d *point, const real_point3d *origin,
    const real_vector3d *offset, float t, long first_ignore_object_index, long second_ignore_object_index, float *fraction, bool underwater)
{
    real_point3d end;
    point_from_line3d(origin, offset, t, &end);
    return observer_collision_test_with_t(user_index, point, &end, first_ignore_object_index, second_ignore_object_index,
        fraction, underwater);
}
// @address 0x205078 observer_handle_camera_collision
extern "C" void observer_handle_camera_collision(long user_index, real_point3d *point, real_vector3d *direction, const real_vector3d *up,
    const real_point3d *start, float radius, long first_ignore_object_index, long second_ignore_object_index,
    float *distance, float collision_radius, float dt)
{
    float fraction = 1.0f;
    bool underwater = false;
    observer_clamp_to_water_surface(point, direction, NULL, *distance, radius);
    real_point3d position;
    real_vector3d vector;
    observer_update_collision_distance(user_index, point, direction, 0.02f, distance, &vector, &position, dt);
    observer_test_collision_line(user_index, point, &position, first_ignore_object_index, second_ignore_object_index, &fraction,
        underwater);

    float best_fraction = fraction;
    const real_vector3d *best_offset = NULL;
    float scale = 0.174f * *distance;
    real_vector3d offsets[2];
    offsets[0] = *up;
    cross_product3d(up, direction, &offsets[1]);
    offsets[0].i *= scale;
    offsets[0].j *= scale;
    offsets[0].k *= scale;
    offsets[1].i *= scale;
    offsets[1].j *= scale;
    offsets[1].k *= scale;
    float best_sign;
    for (short i = 0; i < 4; i++) {
        float sign = (float)((i & 2) ? 1 : -1);
        float hit_fraction;
        if (observer_collision_test_offset(user_index, point, &position, &offsets[i & 1], sign,
                first_ignore_object_index, second_ignore_object_index, &hit_fraction, underwater) && hit_fraction < best_fraction) {
            best_fraction = hit_fraction;
            best_offset = &offsets[i & 1];
            best_sign = sign;
        }
    }
    if (best_offset) {
        float lower = 0.0f;
        float upper = best_sign;
        float current_fraction = fraction;
        float target_fraction = best_fraction;
        for (short i = 0; i < 10; i++) {
            float middle = 0.5f * (lower + upper);
            float hit_fraction;
            bool hit = observer_collision_test_offset(user_index, point, &position, best_offset, middle,
                first_ignore_object_index, second_ignore_object_index, &hit_fraction, underwater);
            if (hit && fabsf(hit_fraction - target_fraction) < 0.1f) {
                upper = middle;
                target_fraction = hit_fraction;
            } else {
                lower = middle;
                current_fraction = hit ? hit_fraction : 1.0f;
            }
        }
        float blend = (current_fraction < target_fraction ? lower : (upper >= 0.0f))
            ? (current_fraction < target_fraction ? lower : upper)
            : -(current_fraction < target_fraction ? lower : upper);
        fraction = blend * fraction + (1.0f - blend) * best_fraction;
    }
    *distance *= fraction;
    if (start)
        observer_fix_point_for_collision(point, direction, start, *distance, radius);
}

static __forceinline float observer_pin_world(float value)
{
    value = PIN(value, k_observer_world_minimum, k_observer_world_maximum);
    return value;
}
// @address 0x204940 observer_update_camera_position
extern "C" void observer_update_camera_position(long user_index, const real_point3d *point, float distance, const real_vector3d *vector)
{
    s_player_block *observer = observer_get(user_index);
    real_point3d *position = &observer->camera_result.position;
    observer->camera_result.position.x = point->x - vector->i * distance;
    observer->camera_result.position.y = point->y - vector->j * distance;
    observer->camera_result.position.z = point->z - vector->k * distance;
    observer->camera_result.position.x = observer_pin_world(observer->camera_result.position.x);
    observer->camera_result.position.y = observer_pin_world(observer->camera_result.position.y);
    observer->camera_result.position.z = observer_pin_world(observer->camera_result.position.z);
    s_location location;
    scenario_location_from_point(&location, position);
    if (location.cluster_reference.bsp_index != NONE)
        observer->camera_result.location = location;
    observer_compute_collision_radius(observer->camera_result.horizontal_field_of_view,
        observer->camera_result.vertical_field_of_view);
    real_point3d water_point = *position;
    float z = water_point.z - k_observer_water_surface_offset;
    water_point.z = z;
    float water_height = water_physics_get_depth_at_point(&water_point);
    if (water_height != -3.4028234663852886e+38f) {
        float surface_z = water_height + z;
        float offset = 0.0f;
        float bounds[2] = { surface_z + k_observer_water_surface_offset, surface_z - k_observer_water_surface_offset };
        if (z > surface_z)
            offset = bounds[0] - z;
        else if (z > bounds[1])
            offset = bounds[1] - z;
        water_point.z = offset + z;
        *position = water_point;
    }
}

// @address 0x203708 ?observer_update_polynomial@@YAXJ@Z
void observer_update_polynomial(long user_index)
{
    s_player_globals *globals = observer_globals_get();
    s_player_block *block = observer_get(user_index);
    s_player_block *observer = (s_player_block *)block;
    float *accelerations = observer->accelerations.all;
    float *displacements = observer->displacements.all;
    float *velocities = observer->velocities.all;
    float *coefficients_a = observer->coefficients_a;
    float *coefficients_b = observer->coefficients_b;
    float *coefficients_c = observer->coefficients_c;
    float *coefficients_d = observer->coefficients_d;
    float *coefficients_e = observer->coefficients_e;
    float *coefficients_f = observer->coefficients_f;
    float *parameter_timers = observer->previous.parameter_timers;
    observer_validate_camera_command((s_observer_command *)&observer->previous.flags);
    *accelerations = *accelerations;
    short parameter_index = 0;
    while (parameter_index < 6) {
        if ((observer->previous.flags & 1) != 0) {
            float time = *parameter_timers;
            if (time > globals->phase) {
                float t1 = 1.0f / time;
                float t2 = t1 * t1;
                float t3 = t2 * t1;
                float t4 = t3 * t1;
                float t5 = t4 * t1;
                if (k_observer_parameter_motion_counts[parameter_index] > 0) {
                    short component_index = 0;
                    while (component_index < k_observer_parameter_motion_counts[parameter_index]) {
                        // match: 0x203708: value range assumption guides branch lowering
                        __assume(component_index >= 0);
                        coefficients_a[component_index] = displacements[component_index] * t5 * -6.0f +
                            velocities[component_index] * t4 * -3.0f + accelerations[component_index] * t3 * 0.5f;
                        coefficients_b[component_index] = displacements[component_index] * t4 * 15.0f +
                            velocities[component_index] * t3 * 7.0f + accelerations[component_index] * t2 * -1.0f;
                        coefficients_c[component_index] = displacements[component_index] * t3 * -10.0f +
                            velocities[component_index] * t2 * -4.0f + accelerations[component_index] * t1 * 0.5f;
                        coefficients_d[component_index] = 0.0f;
                        coefficients_e[component_index] = 0.0f;
                        coefficients_f[component_index] = displacements[component_index];
                        if (parameter_index == 0 && block->command_id == 0) {
                            float adjust = ((s_observer_command *)&observer->previous.flags)->velocity.n[component_index];
                            coefficients_a[component_index] = coefficients_a[component_index] - adjust * t4 * 3.0f;
                            coefficients_b[component_index] = adjust * t3 * 8.0f + coefficients_b[component_index];
                            coefficients_c[component_index] = coefficients_c[component_index] - adjust * t2 * 6.0f;
                            coefficients_e[component_index] = coefficients_e[component_index] + adjust;
                        }
                        component_index++;
                    }
                }
            }
        }
        accelerations += k_observer_parameter_motion_counts[parameter_index];
        displacements += k_observer_parameter_motion_counts[parameter_index];
        velocities += k_observer_parameter_motion_counts[parameter_index];
        coefficients_a += k_observer_parameter_motion_counts[parameter_index];
        coefficients_b += k_observer_parameter_motion_counts[parameter_index];
        coefficients_c += k_observer_parameter_motion_counts[parameter_index];
        coefficients_d += k_observer_parameter_motion_counts[parameter_index];
        coefficients_e += k_observer_parameter_motion_counts[parameter_index];
        coefficients_f += k_observer_parameter_motion_counts[parameter_index];
        parameter_timers++;
        parameter_index++;
    }
}

// @rdata 0x82035da8..0x82035dc0
static const float k_observer_acceleration_limits[6] = { 1500.0f, 1500.0f, 1500.0f, 100000.0f, 100000.0f, 100000.0f };
// @rdata-end

// @address 0x203950 ?observer_update_accelerations@@YAXJ@Z
void observer_update_accelerations(long user_index)
{
    s_player_globals *globals = (s_player_globals *)observer_globals_get();
    s_player_block *observer = (s_player_block *)observer_get(user_index);
    float *accelerations = observer->accelerations.all;
    float *displacements = observer->displacements.all;
    float *velocities = observer->velocities.all;
    float *a = observer->coefficients_a;
    float *b = observer->coefficients_b;
    float *c = observer->coefficients_c;
    float *d = observer->coefficients_d;
    float *e = observer->coefficients_e;
    float *times = observer->previous.parameter_timers;
    const float *limits = k_observer_acceleration_limits;
    for (long parameter = 0; parameter < 6; ++parameter)
    {
        float time = *times - globals->phase;
        if (time > 0.0f)
        {
            float time2 = time * time;
            float time3 = time2 * time;
            for (short i = 0; i < k_observer_parameter_motion_counts[parameter]; ++i)
            {
                accelerations[i] = 20.0f * a[i] * time3 + 12.0f * b[i] * time2 + 6.0f * c[i] * time + 2.0f * d[i];
                if (accelerations[i] > limits[parameter] || accelerations[i] < -limits[parameter])
                {
                    for (short j = 0; j < 6; ++j)
                    {
                        if (j != parameter && observer->previous.parameter_timers[j] == *times)
                            observer->previous.parameter_timers[j] = 0.0f;
                    }
                    *times = 0.0f;
                }
            }
        }
        else
        {
            memset(accelerations, 0, k_observer_parameter_motion_counts[parameter] * sizeof(float));
        }
        accelerations += k_observer_parameter_motion_counts[parameter];
        displacements += k_observer_parameter_motion_counts[parameter];
        velocities += k_observer_parameter_motion_counts[parameter];
        a += k_observer_parameter_motion_counts[parameter];
        b += k_observer_parameter_motion_counts[parameter];
        c += k_observer_parameter_motion_counts[parameter];
        d += k_observer_parameter_motion_counts[parameter];
        e += k_observer_parameter_motion_counts[parameter];
        ++times;
    }
}

static __forceinline short observer_parameter_count(short parameter)
{
    return k_observer_parameter_motion_counts[parameter];
}
// @address 0x203b00 ?observer_update_velocities@@YAXJ@Z
void observer_update_velocities(long user_index)
{
    s_player_globals *globals = (s_player_globals *)observer_globals_get();
    s_player_block *observer = (s_player_block *)observer_get(user_index);
    float inverse_dtime = (float)(1.0 / globals->phase);
    float *velocities = observer->velocities.all;
    float *displacements = observer->displacements.all;
    float *coefficients_a = observer->coefficients_a;
    float *coefficients_b = observer->coefficients_b;
    float *coefficients_c = observer->coefficients_c;
    float *coefficients_d = observer->coefficients_d;
    float *coefficients_e = observer->coefficients_e;
    float *parameter_times = observer->previous.parameter_timers;
    unsigned char *parameter_flags = observer->previous.parameter_flags;
    for (short parameter = 0; parameter < 6; parameter++) {
        float t = *parameter_times - globals->phase;
        if (t > 0.0f) {
            float t2 = t * t;
            float t3 = t2 * t;
            float t4 = t3 * t;
            for (short index = 0; index < observer_parameter_count(parameter); index++) {
                velocities[index] = 5.0f * coefficients_a[index] * t4 + 4.0f * coefficients_b[index] * t3
                    + 3.0f * coefficients_c[index] * t2 + 2.0f * coefficients_d[index] * t + coefficients_e[index];
            }
        } else if (TEST_BIT(observer->previous.flags, 0) && (TEST_BIT(*parameter_flags, 1) || TEST_BIT(observer->previous.flags, 3))) {
            memset(velocities, 0, observer_parameter_count(parameter) * sizeof(float));
        } else if (TEST_BIT(observer->previous.flags, 0)) {
            for (short index = 0; index < observer_parameter_count(parameter); index++)
                velocities[index] = -(displacements[index] * inverse_dtime);
        }
        parameter_times++;
        parameter_flags++;
        displacements += observer_parameter_count(parameter);
        velocities += observer_parameter_count(parameter);
        coefficients_a += observer_parameter_count(parameter);
        coefficients_b += observer_parameter_count(parameter);
        coefficients_c += observer_parameter_count(parameter);
        coefficients_d += observer_parameter_count(parameter);
        coefficients_e += observer_parameter_count(parameter);
    }
}

// Local normalize3d for the rotation axis below (sums via the shared magnitude_squared3d).
static __forceinline float normalize_rotation_axis(real_vector3d *vector)
{
    float length = sqrtf(magnitude_squared3d(vector));
    if (!(fabsf(length - 0.0f) < k_real_epsilon))
        scale_vector3d(vector, 1.0f / length, vector);
    else
        length = 0.0f;
    return length;
}

// Inline expansion of rotate_vector_about_axis; the out-of-line copy lives in real_math.obj.
static __forceinline void rotate_vector_about_axis_inline(float *vector, const real_vector3d *axis, float
    sine, float cosine)
{
    float scale = (axis->i * vector[0] + axis->j * vector[1] + axis->k * vector[2]) * (1.0f - cosine);
    float cross_i = vector[1] * axis->k - vector[2] * axis->j;
    float cross_j = vector[2] * axis->i - vector[0] * axis->k;
    float cross_k = vector[0] * axis->j - vector[1] * axis->i;
    vector[0] = axis->i * scale + vector[0] * cosine - cross_i * sine;
    vector[1] = axis->j * scale + vector[1] * cosine - cross_j * sine;
    vector[2] = axis->k * scale + vector[2] * cosine - cross_k * sine;
}

static __forceinline void observer_rotate_axes(const float *rotation, float *forward, float *up)
{
    real_vector3d axis;
    memcpy(&axis, rotation, sizeof(axis));
    float angle = normalize_rotation_axis(&axis);
    if (angle != 0.0f) {
        float sine_of_angle = sine(angle);
        float cosine_of_angle = cosine(angle);
        rotate_vector_about_axis_inline(forward, &axis, sine_of_angle, cosine_of_angle);
        rotate_vector_about_axis_inline(up, &axis, sine_of_angle, cosine_of_angle);
    }
}

static __forceinline float observer_normalize_forward_vector(real_vector3d *vector)
{
    float k = 0.0f;
    float i = vector->i;
    float j = vector->j;
    k = vector->k;
    float length = sqrtf((k * k + j * j) + i * i);
    if (!(fabsf(length) < k_real_epsilon))
        scale_vector3d(vector, 1.0f / length, vector);
    else
        length = 0.0f;
    return length;
}

static __forceinline float observer_normalize_up_vector(real_vector3d *vector)
{
    float k = vector->k;
    float i = vector->i;
    float j = vector->j;
    float length = sqrtf((k * k + i * i) + j * j);
    if (!(fabsf(length) < k_real_epsilon)) {
        float inverse_length = 1.0f / length;
        vector->i = inverse_length * i;
        vector->j = inverse_length * j;
        vector->k = inverse_length * k;
    }
    else
        length = 0.0f;
    return length;
}

// @address 0x203d10 observer_update_interpolation
extern "C" void observer_update_interpolation(long user_index)
{
    s_player_globals *globals = observer_globals_get();
    s_player_block *observer = (s_player_block *)observer_get(user_index);
    float deltas[13];
    float *positions = observer->focus_point.n;
    float *delta = deltas;
    float *velocities = observer->velocities.all;
    float *targets = observer->previous.position.n;
    float *a = observer->coefficients_a;
    float *b = observer->coefficients_b;
    float *c = observer->coefficients_c;
    float *d = observer->coefficients_d;
    float *e = observer->coefficients_e;
    float *f = observer->coefficients_f;
    float *times = observer->previous.parameter_timers;
    short parameter;
    for (parameter = 0; parameter < 13; parameter++)
        ;
    for (parameter = 0; parameter < 6; parameter++) {
        float t = *times - globals->phase;
        if (t > 0.0f || !(observer->previous.flags & 1)) {
            short i;
            if (t > 0.0f) {
                float t2 = t * t;
                float t3 = t2 * t;
                float t4 = t3 * t;
                float t5 = t4 * t;
                for (i = 0; i < k_observer_parameter_motion_counts[parameter]; i++)
                    delta[i] = a[i] * t5 + b[i] * t4 + c[i] * t3 + d[i] * t2 + e[i] * t + f[i];
            }
            else {
                for (i = 0; i < k_observer_parameter_motion_counts[parameter]; i++)
                    delta[i] = -velocities[i] * globals->phase;
            }
            if (parameter < 5) {
                for (i = 0; i < k_observer_parameter_motion_counts[parameter]; i++)
                    positions[i] += delta[i];
            }
            else {
                observer_rotate_axes(delta, positions, positions + 3);
            }
        }
        else {
            for (short i = 0; i < k_observer_parameter_element_counts[parameter]; i++)
                positions[i] = targets[i];
        }
        positions += k_observer_parameter_element_counts[parameter];
        targets += k_observer_parameter_element_counts[parameter];
        velocities += k_observer_parameter_motion_counts[parameter];
        delta += k_observer_parameter_motion_counts[parameter];
        a += k_observer_parameter_motion_counts[parameter];
        b += k_observer_parameter_motion_counts[parameter];
        c += k_observer_parameter_motion_counts[parameter];
        d += k_observer_parameter_motion_counts[parameter];
        e += k_observer_parameter_motion_counts[parameter];
        f += k_observer_parameter_motion_counts[parameter];
        times++;
    }
    positions = &observer->focus_point.n[10];
    for (parameter = 5; parameter < 6; parameter++) {
        real_vector3d *forward = (real_vector3d *)positions;
        real_vector3d *up = (real_vector3d *)positions + 1;
        if (!valid_real_vector3d_axes2(forward, up)) {
            real_vector3d left;
            cross_product3d(up, forward, &left);
            cross_product3d(forward, &left, up);
            observer_normalize_forward_vector(forward);
            observer_normalize_up_vector(up);
        }
        positions += k_observer_parameter_element_counts[parameter];
    }
}

namespace {
static __forceinline void observer_validate_axes(real_vector3d *forward, real_vector3d *up)
{
    if (!valid_real_vector3d_axes2(forward, up)) {
        *forward = *global_forward3d;
        *up = *global_up3d;
    }
}
}

// @address 0x204478 observer_update_result
extern "C" void observer_update_result(long user_index, float *effect)
{
    s_player_block *observer = (s_player_block *)observer_get(user_index);
    real_point3d focus_position = observer->focus_point;
    float focus_distance = PIN(observer->focus_distance, 0.0f, k_observer_focus_distance_maximum);
    observer_validate_axes(&observer->motion_forward, &observer->motion_up);
    observer->field_of_view = PIN(observer->field_of_view, k_observer_field_of_view_minimum, k_observer_field_of_view_maximum);
    focus_position.x = PIN(focus_position.x, k_observer_world_minimum, k_observer_world_maximum);
    focus_position.y = PIN(focus_position.y, k_observer_world_minimum, k_observer_world_maximum);
    focus_position.z = PIN(focus_position.z, k_observer_world_minimum, k_observer_world_maximum);
    focus_distance = PIN(focus_distance, 0.0f, k_observer_command_focus_distance_max);
    if (observer->focus_offset_in_view_space) {
        real_vector3d left;
        cross_product3d(&observer->motion_up, &observer->motion_forward, &left);
        point_from_line3d(&focus_position, &observer->motion_forward, observer->focus_offset.i, &focus_position);
        point_from_line3d(&focus_position, &left, observer->focus_offset.j, &focus_position);
        point_from_line3d(&focus_position, &observer->motion_up, observer->focus_offset.k, &focus_position);
    } else {
        real_vector2d forward;
        forward.i = observer->motion_forward.i;
        forward.j = observer->motion_forward.j;
        normalize2d(&forward);
        focus_position.x += observer->focus_offset.i * forward.i + observer->focus_offset.j * forward.j;
        focus_position.y += observer->focus_offset.i * forward.j - observer->focus_offset.j * forward.i;
        focus_position.z += observer->focus_offset.k;
    }
    observer->result.velocity.i = -observer->velocities.linear.i;
    observer->result.velocity.j = -observer->velocities.linear.j;
    observer->result.velocity.k = -observer->velocities.linear.k;
    observer->result.angular_velocity.i = observer->velocities.angular.i;
    observer->result.angular_velocity.j = observer->velocities.angular.j;
    observer->result.angular_velocity.k = observer->velocities.angular.k;
    observer->result.forward = observer->motion_forward;
    observer->result.up = observer->motion_up;
    if ((observer->command->flags & 0x800) == 0) {
        observer->field_of_view = g_observer_default_field_of_view;
        observer->command->field_of_view = g_observer_default_field_of_view;
        observer->result.horizontal_field_of_view = observer->command->field_of_view;
        observer->command->parameter_flags[4] = 0;
    }
    observer->result.horizontal_field_of_view = observer->field_of_view;
    observer->result.view_offset = observer->view_offset;
    observer->result.aspect_ratio = k_observer_default_aspect_ratio;
    observer->result.depth_of_field = observer->command->depth_of_field;
    observer_result_compute_parameters(&observer->camera_result);
    if (observer->using_matrix) {
        matrix4x3_transform_point(&observer->matrix, &focus_position, &focus_position);
        matrix4x3_transform_vector(&observer->matrix, &observer->result.velocity, &observer->result.velocity);
        matrix4x3_transform_vector(&observer->matrix, &observer->result.angular_velocity, &observer->result.angular_velocity);
        matrix4x3_transform_vector(&observer->matrix, &observer->result.forward, &observer->result.forward);
        matrix4x3_transform_vector(&observer->matrix, &observer->result.up, &observer->result.up);
        add_vectors3d(&observer->result.velocity, &observer->previous.velocity, &observer->result.velocity);
    }
    effect[3] = focus_distance;
    memcpy(effect, &focus_position, sizeof(real_point3d));
    observer_update_camera_position(user_index, &focus_position, focus_distance, &observer->result.forward);
    observer->result_valid = true;
}

namespace {
static __forceinline bool observer_result_valid(long user_index)
{
    return ((s_player_block *)observer_get(user_index))->result_valid;
}
}

// @address 0x202fa8 observer_update_collision
extern "C" void observer_update_collision(long user_index, float *effect, float elapsed)
{
    s_player_block *observer = (s_player_block *)observer_get(user_index);
    if (observer_result_valid(user_index) && !TEST_BIT(observer->previous.flags, 4)) {
        float radius = observer_compute_collision_radius(observer->result.horizontal_field_of_view,
            observer->result.vertical_field_of_view);
        const real_point3d *parent_position = TEST_BIT(observer->command->flags, 10) ? &observer->command->parent_position : NULL;
        real_point3d focus_position;
        memcpy(&focus_position, effect, sizeof(real_point3d));
        real_vector3d forward;
        forward.i = focus_position.x - observer->result.position.x;
        forward.j = focus_position.y - observer->result.position.y;
        forward.k = focus_position.z - observer->result.position.z;
        float focus_distance = normalize3d(&forward);
        const s_observer_command *command = observer->command;
        s_player_block *current = (s_player_block *)observer_get(user_index);
        long first_ignore_object_index = NONE;
        long second_ignore_object_index = NONE;
        if (TEST_BIT(command->flags, 6)) {
            first_ignore_object_index = current->parent_object_index;
            second_ignore_object_index = current->parent_unit_index;
        }
        if (TEST_BIT(command->flags, 5)) {
            current->parent_object_index = NONE;
            current->parent_unit_index = NONE;
            if (command->ignore_object_count > 0) {
                first_ignore_object_index = command->ignore_object_indices[0];
                current->parent_object_index = first_ignore_object_index;
            }
            if (command->ignore_object_count > 1) {
                second_ignore_object_index = command->ignore_object_indices[1];
                current->parent_unit_index = second_ignore_object_index;
            }
        }
        bool inside = false;
        if (TEST_BIT(current->previous.flags, 13)) {
            const s_observer_command *capsule = current->command;
            real_point3d camera_position;
            point_from_line3d(&focus_position, &forward, -focus_distance, &camera_position);
            real_vector3d height;
            height.i = capsule->pill_height * 0.0f;
            height.j = capsule->pill_height * 0.0f;
            height.k = capsule->pill_height;
            inside = fast_vector_intersects_sphere(&capsule->pill_base, &height, &camera_position, capsule->pill_width * 0.95f);
        }
        if (!inside) {
            if (TEST_BIT(observer->command->flags, 10))
                observer_fix_focus_position_to_safe_position(
                    user_index, &focus_position, first_ignore_object_index, second_ignore_object_index, parent_position);
            observer_handle_camera_collision(user_index, &focus_position, &forward, &observer->result.up, parent_position, radius,
                first_ignore_object_index, second_ignore_object_index, &focus_distance, 0.02f, elapsed);
            observer_update_camera_position(user_index, &focus_position, focus_distance, &forward);
        }
    }
}

// @address 0x12bce0 ?observer_update@@YAXM@Z
void observer_update(float elapsed)
{
    s_player_globals *globals = observer_globals_get();
    globals->phase = elapsed;
    for (long user_index = 0; user_index < 4; ++user_index) {
        if (players_user_is_active(user_index)) {
            s_player_globals *update_globals = observer_globals_get();
            s_player_globals *block = update_globals;
            block->blocks[user_index].updated = true;
            observer_update_command(user_index);
            if (globals->phase != 0.0f) {
                s_player_block *observer = &block->blocks[user_index];
                float *timer = observer->previous.parameter_timers;
                if ((observer->command->flags & 0x80) == 0) {
                    observer_compute_motion_delta(observer->focus_point.n, observer->previous.position.n, observer->displacements.all);
                    observer_update_polynomial(user_index);
                    observer_update_accelerations(user_index);
                    observer_update_velocities(user_index);
                    observer_update_interpolation(user_index);
                    for (short parameter = 0; parameter < 6; ++parameter, ++timer) {
                        float remaining = *timer - update_globals->phase;
                        *timer = remaining > 0.0f ? remaining : 0.0f;
                    }
                }
            }
            float camera_effect[6];
            observer_update_result(user_index, camera_effect);
            observer_apply_camera_effect(user_index);
            observer_update_collision(user_index, camera_effect, elapsed);
        }
    }
    if (!globals->initialized)
        observer_post_global_update_list();
    globals->initialized = false;
}

// @address 0x1d2a40 map_variant_new_object
extern "C" long map_variant_new_object(c_map_variant *map_variant, long definition_index, long count, long placement_index,
    const real_point3d *position, const real_vector3d *forward, const real_vector3d *up,
    long scenario_index, e_object_type object_type,
    const s_variant_multiplayer_object_properties_definition *properties, short owner_index)
{
    long result = -1;
    bool can_place = false;
    if (placement_index == -1) {
        for (long index = map_variant->m_number_of_scenario_objects; index < 640; ++index) {
            if (map_variant->objects[index].flags.is_empty()) {
                can_place = true;
                break;
            }
        }
    } else if (map_variant->objects[placement_index].object_index == -1) {
        can_place = true;
    }
    if (can_place && definition_index != -1) {
        long quota_index = map_variant_find_quota_index(map_variant, definition_index);
        if (quota_index != -1) {
            s_game_engine_globals *globals = game_engine_globals;
            can_place = game_engine_candy_monitor_count_quota_objects(globals->get_candy_monitor_manager(),
                placement_index, definition_index) < map_variant->quotas[quota_index].maximum_allowed;
        }
    }
    if (definition_index != -1 && can_place) {
        object_placement_data placement;
        if (scenario_index != -1) {
            scenario *current_scenario = global_scenario;
            s_tag_block *palette = (s_tag_block *)((char *)current_scenario +
                object_type_definitions[object_type]->scenario_palette_offset);
            const s_scenario_object *scenario_object = scenario_object_try_and_get(current_scenario, object_type, scenario_index);
            object_placement_data_new_from_scenario_object(object_type, scenario_index, scenario_object, palette, true, false, &placement);
        } else {
            object_placement_data_new(&placement, definition_index, -1, 0);
        }
        placement.position = *position;
        if (forward)
            placement.forward = *forward;
        else
            placement.forward = *global_forward3d;
        if (up)
            placement.up = *up;
        else
            placement.up = *global_up3d;
        if (!valid_real_vector3d_axes2(&placement.forward, &placement.up)) {
            placement.forward = *global_forward3d;
            placement.up = *global_up3d;
        }
        bool has_multiplayer_properties =
            TAG_GET('obje', const s_object_definition, definition_index)
                ->multiplayer_object.count() > 0;
        if (has_multiplayer_properties)
            placement.multiplayer_properties.team_designator = _multiplayer_team_designator_none;
        long object_index = object_new(&placement);
        if (object_index != -1) {
            object_datum *object = object_get(object_index);
            if (placement_index != -1) {
                if (owner_index) {
                    const unsigned long owner_flags = (unsigned short)owner_index;
                    map_variant->objects[placement_index].flags.set_unsafe(map_variant->objects[placement_index].flags.get_unsafe() | owner_flags);
                }
                map_variant->objects[placement_index].flags.set(_variant_object_placement_occupied_slot_bit, false);
            }
            c_map_variant::find_definition_index(object_index, placement_index, count);
            if (object->map_variant_index != -1) {
                s_variant_object_datum *variant_object = &map_variant->objects[object->map_variant_index];
                if (properties) {
                    if (scenario_index == -1) {
                        map_variant->apply_multiplayer_properties(properties, object_index);
                        c_map_variant::setup_object_properties(variant_object);
                    }
                } else {
                    c_map_variant::setup_object_properties(variant_object);
                    map_variant->apply_multiplayer_properties(object->map_variant_index, object_index);
                }
                simulation_action_sandbox_group_update(object->map_variant_index, 4, true);
            }
            game_engine_register_object(object_index);
            simulation_action_object_create(object_index);
        }
        result = object_index;
    }
    return result;
}

#if !defined(_WIN64) && !defined(__LP64__)
typedef char observer_result_offset[(offsetof(s_observer, result) == 0x10c) ? 1 : -1];
typedef char observer_focus_point_offset[(offsetof(s_observer, focus_point) == 380) ? 1 : -1];
typedef char observer_saved_matrix_offset[(offsetof(s_observer, matrix) == 444) ? 1 : -1];
typedef char observer_clear_result_offset[(offsetof(s_player_block, result) == 0x10c) ? 1 : -1];
typedef char observer_parameter_times_offset[(offsetof(s_player_block, previous) + offsetof(s_observer_command, parameter_timers)
    == 0xc8) ? 1 : -1];
#endif

t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_observer_globals_allocator> g_observer_globals_allocator;
t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_observer_gamestate_globals_allocator> g_observer_gamestate_globals_allocator;

// @address 0x761454 ?g_observer_default_field_of_view@@3MA
float g_observer_default_field_of_view = 1.22173047f;

// @address 0x761458 k_observer_field_of_view_minimum
float k_observer_field_of_view_minimum = 0.0174532924f;
// @address 0x76145c k_observer_field_of_view_maximum
float k_observer_field_of_view_maximum = 2.61799383f;
// @address 0x761478 k_observer_command_focus_distance_max
float k_observer_command_focus_distance_max = 50000.0f;
