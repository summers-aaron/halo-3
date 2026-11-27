// blamlibxbox/impacts.obj: from the records 0x1071c8 0x112528 0x1131b8 0x1134b0 0x113b00 0x113d78 0x113fd8 0x1145a8 0x1153f8 0x1154a8 0x1154f8 0x115610 0x115740 0x115848 0x115880 0x1159a0 0x1159e0 0x1b9e60 0x1bc9c0 0x223158 0x232cd0 0x247ed0 0x247fe0 0x247ff0 0x2480d8 0x2481a0 0x248478 0x2486c8 0x2488c8 0x248b70 0x248f88 0x249090 0x2490d8 0x249278 0x249388 0x2494b0 0x249588 0x249718 0x249d40 0x249e28 0x249ee0 0x249fe0 0x24a370 0x24a450 0x24a4f0 0x24a5d0 0x24a670 0x45f250 0x45fcc0 0x45fd60 0x708238 0x70bda8
// @flags /TP /O2 /Oc
#include "effects/effect_location_data.h"
#include "physics/physics_model_types.h"
#include "sound/sound_source_types.h"
#include <stddef.h>
#include "cseries/long_sort.h"
#include "physics/physics_model_constraint_types.h"
#include "objects/object_reference_types.h"
#include "tag_files/tag_index.h"
#include "memory/data_types.h"
#include "tag_files/string_ids.h"
#include "tag_files/tag_types.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "game/game_time_types.h"
#include "game/game_time_access.h"
#include "objects/object_iterator.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "phys/phys_dynamics.h"
#include "cseries/cseries.h"
#include "cache/restricted_memory_regions.h"
#include "game/players.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "sound/game_sound.h"
#include "devices/devices.h"
#include "effects/effects.h"
#include "physics/havok_component.h"
#include "physics/impacts.h"
#include "game/game_allegiance_types.h"
#include "cache/restricted_memory.h"
#include "physics/water_physics.h"
#include "physics/havok.h"
#include "physics/physics_model_definitions.h"
#include "sound/sound_manager_platform_playback.h"
#include "math/matrix_math.h"
#include "physics/physics_model_constraint_reference.h"
#include "physics/impact_array_types.h"
#include "cache/cache_files.h"
#include "game/materials.h"
#include "units/bipeds.h"
#include "effects/effect_vector_types.h"
#include "effects/effect_deterministic_types.h"
#include "game/player_mapping_types.h"
#include "physics/water_physics_types.h"
#include "memory/tls_data_arrays.h"
#include "physics/impact.h"
#include "physics/impacts_internal.h"
#include "physics/havok_contact_point.h"
#include "physics/havok_component_class.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_registration.h"
#include "cseries/crt_xdk.h"
#include "physics/impacts_globals_types.h"
#include "sound/game_sound_types.h"
#include "sound/game_looping_sound_data.h"
#include "objects/object_definitions.h"
#include "camera/observer_globals_types.h"
#include "models/model_types.h"
#include "physics/physics_model_constraint_definition.h"
#include "objects/object_header.h"
#include "physics/havok_component_access.h"
#include "memory/data_array_functions.h"
#include "effects/particle_array_globals.h"
#include <math.h>
#include "physics/havok_contact_point_reference.h"
#include "physics/havok_contact_point_iterator.h"
#include "physics/character_physics_ground_helpers.h"
#include "effects/material_effects.h"
#include "physics/havok_constants_types.h"
#include "physics/collision_bsp_types.h"
#include "physics/collision_bsp_cache_types.h"
#include "effects/effect_creation_flags.h"
#include "effects/effect_creation_data.h"
#include "sound/game_sound_impulse_parameters.h"
#include "sound/game_sound_impulse.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "tag_files/tag_groups.h"
enum e_impact_sound_scale_type {
    _impact_sound_scale_character = 0,
    _impact_sound_scale_vehicle = 1,
    _impact_sound_scale_dynamic = 2,
    _impact_sound_scale_none = 3
};
enum e_game_sound_impulse_additional_override_bit { _game_sound_impulse_override_source_type };
// @address 0x1071c8 ?passes_the_character_test@c_impact@@SA_NJJPBTreal_point3d@@PBTreal_vector3d@@@Z
// @address 0x223158 ?__tls_set_g_impact_globals_allocator@@YAXPAX@Z
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0x255d70 ?disconnect_from_impacts@c_havok_component@@QAAXXZ
// @address 0xf8140 ?effect_delete_internal@@YAXJ_N@Z
// @address 0xf25a0 ?matrix4x3_inverse@@YAXPBUreal_matrix4x3@@PAU1@@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0xade58 ?matrix4x3_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0x247fb0 ?__tls_set_g_device_groups_data_allocator@@YAXPAX@Z
// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z
// @address 0xaae70 ?datum_new@@YAJPAUs_data_array@@@Z
 
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z









// @address 0x11df73c g_impacts_globals
extern "C" {
__declspec(thread) impacts_globals_type *g_impacts_globals;
}
// @address 0x11df704 g_impact_data
// @address 0x11df7ec g_impact_array_data
extern "C" {
__declspec(thread) s_data_array *g_impact_data;
__declspec(thread) s_data_array *g_impact_array_data;
}













// match: 0x249fe0: March 8273a2f0 keeps a separate result; objects.cpp owns the shared out-of-line getter.
static __forceinline short impacts_controlling_body_index(const c_havok_component *component)
{
    long result;
    if (component->m_object_postion_controlling_rigid_body_index >= 0 &&
        component->m_object_postion_controlling_rigid_body_index < component->get_rigid_body_count())
        result = component->m_object_postion_controlling_rigid_body_index;
    else
        result = -1;
    return (short)result;
}
struct s_model;


// match: 0x2481a0: retain the typed getter boundary to preserve the accepted frame and register allocation.
static __forceinline s_object_header *impacts_header_datum_get(s_data_array *array, long index)
{
    return datum_get<s_object_header>(array, index);
}
static __forceinline object_datum *impacts_creation_object_get(long index, unsigned long mask)
{
    s_object_header *header = impacts_header_datum_get(object_header_data, index);
    return (object_datum *)header->datum;
}

// match: 0x248478: retain the original datum getter boundary at iterator and deletion sites.
static __forceinline c_havok_component *impacts_contact_component_get(s_data_array *array, long index)
{
    return datum_get<c_havok_component>(array, index);
}



namespace {
static __forceinline s_impact_array *impact_array_get(long array_index)
{
    s_impact_array *result = &((s_impact_array *)TLS_IMPACT_ARRAY_DATA->data)[datum_index_to_absolute_index(array_index)];
    return result;
}
static __forceinline long impact_array_impact_count(long array_index)
{
    s_impact_array *array = impact_array_get(array_index);
    return array->impact_count;
}
static __forceinline void impact_array_remove_impact(long array_index, long impact_index)
{
    s_impact_array *array = impact_array_get(array_index);
    for (long index = 0; index < array->impact_count; ++index) {
        if (array->impact_indices[index] == impact_index) {
            array->impact_indices[index] = array->impact_indices[array->impact_count - 1];
            --array->impact_count;
            break;
        } else {
            continue;
        }
    }
}
static __forceinline void impact_array_delete(long array_index)
{
    ::datum_delete(TLS_IMPACT_ARRAY_DATA, array_index);
}
static __forceinline long impacts_impact_array_new(long object_index)
{
    long array_index = datum_new(TLS_IMPACT_ARRAY_DATA);
    s_impact_array *array = impact_array_get(array_index);
    array->object_index = object_index;
    array->impact_count = 0;
    return array_index;
}
static __forceinline void impacts_impact_array_add_impact(long array_index, long impact_index)
{
    s_impact_array *array = impact_array_get(array_index);
    array->impact_indices[array->impact_count] = impact_index;
    array->impact_count = array->impact_count + 1;
}
}



namespace {
static __forceinline c_impact *impacts_impact_datum_get(long impact_index)
{
    return datum_get<c_impact>(TLS_IMPACT_DATA, impact_index);
}
static __forceinline long impacts_component_impact_datum_index(const c_havok_component *component, long index)
{
    return impact_array_get(component->m_impact_array_index)->impact_indices[index];
}
}

// @address 0x249588 ?calculate_impact_matrix@c_impact@@AAAXJPAUreal_matrix3x3@@@Z
void c_impact::calculate_impact_matrix(long object_index, real_matrix3x3 *matrix)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    matrix->up = object_position;
    float dot = dot_product3d(&object->forward, &object_position);
    bool use_forward = (dot >= 0.0f ? dot : -dot) < 0.9f;
    const real_vector3d *axis = use_forward ? &object->forward : &object->up;
    matrix->forward = *axis;
    real_vector3d projection;
    float proj_dot = dot_product3d(&matrix->forward, &matrix->up);
    scale_vector3d(&matrix->up, proj_dot, &projection);
    subtract_vectors3d(&matrix->forward, &projection, &matrix->forward);
    normalize3d(&matrix->forward);
    cross_product3d(&matrix->up, &matrix->forward, &matrix->left);
    normalize3d(&matrix->left);
}

// @address 0x249d40 ?currently_playing_looping_sound@c_impact@@AAA_NJJ@Z
bool c_impact::currently_playing_looping_sound(long impact_index, long looping_sound_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(get_object_index_a(), -1));
    c_havok_component *component = havok_component_get(object->havok_component);
    bool result = false;
    for (long index = 0; index < component->get_impact_count(); ++index) {
        long other_impact_index = impacts_component_impact_datum_index(component, index);
        if (other_impact_index != impact_index) {
            c_impact *impact = impacts_impact_datum_get(other_impact_index);
            if (impact->looping_sound_a == looping_sound_index || impact->looping_sound_b == looping_sound_index)
                result = true;
        }
    }
    return result;
}

bool c_impact::passes_the_character_test(long first, long second, const real_point3d *point, const real_vector3d *normal)
{
    const unsigned long character_mask = 2049;
    bool first_character; if (first != -1 && ((1UL << object_header_get(first)->object_type) & character_mask)) first_character = true;
        else first_character = false;
    bool second_character; if (second != -1 && ((1UL << object_header_get(second)->object_type) & character_mask)) second_character =
        true; else second_character = false;
    bool first_passes = true;
    bool second_passes = true;
    if (first_character)
        first_passes = havok_component_get(((object_datum *)object_get_and_verify_type(first,
            -1))->havok_component)->is_physics_model_havok_component();
    if (second_character)
        second_passes = havok_component_get(((object_datum *)object_get_and_verify_type(second,
            -1))->havok_component)->is_physics_model_havok_component();
    return first_passes && second_passes;
}

// @address 0x115880 ?update_rigid_body_space_positions@c_impact@@AAAXPBTreal_point3d@@JJ@Z
void c_impact::update_rigid_body_space_positions(const real_point3d *world_space_position, long node_index_a, long node_index_b)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(get_object_index_a(), -1));
    c_havok_component *component = havok_component_get(object->havok_component);
    __declspec(align(16)) real_matrix4x3 transform;
    __declspec(align(16)) real_matrix4x3 inverse;
    component->get_rigid_body_transform(node_index_a, &transform);
    if (rigid_body_index != 0xffff) {
        origin = transform.position;
    } else {
        matrix4x3_inverse(&transform, &inverse);
        matrix4x3_transform_point(&inverse, &origin, &space_position_a);
        if (is_between_pair_of_dynamic_rigid_bodies()) {
            object_datum *other_object = ((object_datum *)object_get_and_verify_type(get_object_index_b(), -1));
            c_havok_component *other_component = havok_component_get(other_object->havok_component);
            other_component->get_rigid_body_transform(node_index_b, &transform);
            matrix4x3_inverse(&transform, &inverse);
            matrix4x3_transform_point(&inverse, &origin, &space_position_b);
        }
    }
}

// @address 0x1159a0 ?get_impact_count@c_havok_component@@QBAJXZ
// match: 0x113fd8: inline lets c_impact::update_state and update_effects (0x1145a8) inline get_impact_count; out of line their saved-register sets change
#if defined(_XBOX) && !defined(__clang__)
inline long c_havok_component::get_impact_count() const
#else
long c_havok_component::get_impact_count() const
#endif
{
    if (m_impact_array_index == -1)
        return 0;
    else
        return impact_array_impact_count(m_impact_array_index);
}

// @address 0x1b9e60 ?add_impact@c_havok_component@@QAAXJ@Z
void c_havok_component::add_impact(long impact_index)
{
    if (m_impact_array_index == -1)
        m_impact_array_index = impacts_impact_array_new(m_object_index);
    impacts_impact_array_add_impact(m_impact_array_index, impact_index);
}

void __tls_set_g_impact_globals_allocator(void *allocator)
{
    tls_game_allegiance_globals_set((s_game_allegiance_globals *)allocator);
}



// @address 0x232cd0 ?__tls_set_g_impact_data_allocator@@YAXPAX@Z
void __tls_set_g_impact_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_effect_location_data, (s_data_array *)object);
}

// @address 0x247ed0 ?remove_impact@c_havok_component@@QAAXJ@Z
void c_havok_component::remove_impact(long impact_index)
{
    if (m_impact_array_index != -1) {
        impact_array_remove_impact(m_impact_array_index, impact_index);
        if (impact_array_impact_count(m_impact_array_index) == 0) {
            impact_array_delete(m_impact_array_index);
            m_impact_array_index = -1;
        }
    }
}


namespace {
 
static __forceinline void impacts_qsort_4byte(void *base, unsigned long count, bool (*compare)(long, long, const void *), const void
    *context)
{
    if (count >= 2)
        templated_sort<long>::qsort((long *)base, count, compare, context);
}
}


// @address 0x112528 ?impacts_notify_object_deleted@@YAXJ@Z
void impacts_notify_object_deleted(long object_index)
{
    if (g_impacts_globals->disconnected_from_physics) {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
        if (havok_object_needs_physics(object_index, object->definition_index)) {
            c_data_iterator<c_impact> iterator;
            iterator.begin(TLS_IMPACT_DATA);
            while (iterator.next()) {
                c_impact *impact = iterator.get_datum();
                if (impact->get_object_index_a() == object_index || impact->get_object_index_b() == object_index)
                    impact_delete(iterator.get_index());
            }
        }
    }
}

// @address 0x247fe0 __tls_set_g_impacts_globals_allocator
extern "C" void __tls_set_g_impacts_globals_allocator(void *address)
{
    g_impacts_globals = (impacts_globals_type *)address;
}

// @address 0x247ff0 ?impacts_disconnect_from_physics@@YAXXZ
void impacts_disconnect_from_physics()
{
    g_impacts_globals->disconnected_from_physics = true;
    c_data_iterator<c_impact> iterator;
    iterator.begin(TLS_IMPACT_DATA);
    while (iterator.next()) {
        c_impact *impact = iterator.get_datum();
        object_datum *object = ((object_datum *)object_get_and_verify_type(impact->get_object_index_a(), -1));
        if (object->havok_component != -1) {
            c_havok_component *component = havok_component_get(object->havok_component);
            component->disconnect_from_impacts();
        }
        impact->disconnect_from_physics();
    }
    data_delete_all(TLS_IMPACT_ARRAY_DATA);
}

// @address 0x2480d8 ?impacts_reconnect_to_physics@@YAXXZ
void impacts_reconnect_to_physics()
{
    g_impacts_globals->disconnected_from_physics = false;
    c_data_iterator<c_impact> iterator;
    iterator.begin(TLS_IMPACT_DATA);
    while (iterator.next()) {
        c_impact *impact = iterator.get_datum();
        object_datum *object = ((object_datum *)object_get_and_verify_type(impact->get_object_index_a(), -1));
        if (object->havok_component != -1) {
            c_havok_component *component = havok_component_get(object->havok_component);
            component->add_impact(iterator.get_index());
        } else {
            impact_delete(iterator.get_index());
        }
    }
}

// @address 0x248f88 ?impacts_quicksort@@YAXXZ
void impacts_quicksort(void)
{
    long sorted_indexes_count = 0;
    c_data_iterator<c_impact> iterator;
    iterator.begin(TLS_IMPACT_DATA);
    while (iterator.next()) {
        g_impacts_globals->sorted_indexes[sorted_indexes_count] = iterator.get_index();
        ++sorted_indexes_count;
    }
    if (sorted_indexes_count > 1)
        impacts_qsort_4byte(g_impacts_globals->sorted_indexes, sorted_indexes_count, (bool (*)(long, long, const void
            *))impacts_score_compare, 0);
    for (long index = 0; index < sorted_indexes_count; ++index) {
        c_impact *datum = datum_get<c_impact>(TLS_IMPACT_DATA, g_impacts_globals->sorted_indexes[index]);
        datum->sorted_index = (signed char)index;
    }
    g_impacts_globals->highest_score_index = sorted_indexes_count > 0 ? sorted_indexes_count - 1 : -1;
    for (long index = 0; index < sorted_indexes_count; ++index) {
    }
}

// @address 0x249090 ?impacts_score_compare@@YA_NJJPAX@Z
bool impacts_score_compare(long first, long second, void *)
{
    return ((c_impact *)(TLS_IMPACT_DATA->data + (unsigned short)first * sizeof(c_impact)))->score >
        ((c_impact *)(TLS_IMPACT_DATA->data + (unsigned short)second * sizeof(c_impact)))->score;
}

// @address 0x45f250 ?__tls_set_g_impact_array_data_allocator@@YAXPAX@Z
void __tls_set_g_impact_array_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_particle_gpu_block_data, (s_data_array *)object);
}

static __forceinline long impacts_seconds_to_ticks_round(float seconds)
{
    return (long)((float)tls_game_time_globals()->ticks_per_second * seconds + 0.5f);
}

// @address 0x249e28 ?queue_collision@c_impact@@QAAXM@Z
void c_impact::queue_collision(float score)
{
    if (score == 0.0f)
        return;
    if (queued_flag != 0 && score <= queued_score)
        return;
    if (queued_tick != -1 && tls_game_time_globals()->time - queued_tick <= impacts_seconds_to_ticks_round(0.2f) && score <=
        queued_score_b * global_lean_pivot_offset)
        return;
    queued_score = score;
    queued_flag = 1;
}


static __forceinline void impacts_effect_delete_managed(long *managed_effect_index)
{
    if (*managed_effect_index != -1) {
        effect_delete_internal(*managed_effect_index, true);
        *managed_effect_index = -1;
    }
}

// @address 0x249ee0 ?halt_managed_effects@c_impact@@AAAX_N0@Z
void c_impact::halt_managed_effects(bool, bool stop_looping_sounds)
{
    if (stop_looping_sounds) {
        if (looping_sound_index_a != -1) {
            datum_get<game_looping_sound_datum>(game_looping_sound_data, looping_sound_index_a)->flags.set(1, true);
            looping_sound_index_a = -1;
            looping_sound_a = -1;
        }
        if (looping_sound_index_b != -1) {
            datum_get<game_looping_sound_datum>(game_looping_sound_data, looping_sound_index_b)->flags.set(1, true);
            looping_sound_index_b = -1;
            looping_sound_b = -1;
        }
    }
    impacts_effect_delete_managed(&managed_effect_index_a);
    impacts_effect_delete_managed(&managed_effect_index_b);
    impacts_effect_delete_managed(&managed_effect_index_c);
}


static __forceinline bool impacts_short_pair_equal(short a, short b, short c, short d)
{
    bool result;
    if ((a == c && b == d) || (b == c && a == d)) result = true;
    else result = false;
    return result;
}

// @address 0x249388 impacts_contact_descriptions_match
extern "C" bool impacts_contact_descriptions_match(int p0, int p1, int p2, int p3, short p4, short p5, short p6, short p7, int p8, int p9,
    unsigned char p10, unsigned char p11, int p12, int p13)
{
    if ((p10 && p11 && impacts_short_pair_equal(p4, p5, p6, p7))
        || (!p10 && !p11 && ((p0 == p2 && p1 == p3) || (p0 == p3 && p1 == p2)) && impacts_short_pair_equal(p4, p5, p6, p7)
            && p12 == p13 && p8 == p9))
        return true;
    return false;
}


static __forceinline real_vector3d *impacts_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *result)
{
    result->i = b->x - a->x;
    result->j = b->y - a->y;
    result->k = b->z - a->z;
    return result;
}
static __forceinline float impacts_magnitude_squared(const real_vector3d *v)
{
    return v->i*v->i + v->j*v->j + v->k*v->k;
}
static __forceinline float impacts_dot_product(const real_vector3d *a, const real_vector3d *b)
{
    return a->i*b->i + a->j*b->j + a->k*b->k;
}
// @address 0x81428 cos
float g_minimum_contact_point_dot_normal_in_impact = (float)cos(45.0);
// @address 0x113d78 ?included_in_impact_space@c_impact@@SA_NPBTreal_point3d@@PBTreal_vector3d@@01PAM@Z
bool c_impact::included_in_impact_space(const real_point3d *position_a, const real_vector3d *normal_a, const real_point3d *position_b,
    const real_vector3d *normal_b, float *distance_squared)
{
    bool result = false;
    real_vector3d displacement;
    impacts_vector_from_points(position_a, position_b, &displacement);
    *distance_squared = impacts_magnitude_squared(&displacement);
    if (*distance_squared < 0.25f)
        result = true;
    else if (*distance_squared < 16.0f) {
        float dot = impacts_dot_product(normal_b, normal_a);
        if (dot > g_minimum_contact_point_dot_normal_in_impact)
            result = true;
    }
    return result;
}
// @address 0x1154a8 ?is_managing_effect@c_impact@@QBA_NXZ
bool c_impact::is_managing_effect() const
{
    return looping_sound_index_a != -1 || looping_sound_index_b != -1 || managed_effect_index_a != -1 || managed_effect_index_b != -1 ||
        managed_effect_index_c != -1;
}
// @address 0x1154f8 impact_resolve_material_effects
extern "C" void impact_resolve_material_effects(
    const c_impact *impact, const real_point3d *position, unsigned short a, unsigned short b, long state, long *effects)
{
    bool swap_materials = false;
    if (impact->is_between_pair_of_dynamic_rigid_bodies()) {
        object_datum *object_a = ((object_datum *)object_get_and_verify_type(impact->object_index_a, -1));
        s_object_definition *definition_a = (s_object_definition *)tag_get('obje', object_a->definition_index);
        if (definition_a->material_effects.index == -1) {
            object_datum *object_b = ((object_datum *)object_get_and_verify_type(impact->object_index_b, -1));
            s_object_definition *definition_b = (s_object_definition *)tag_get('obje', object_b->definition_index);
            if (definition_b->material_effects.index != -1) swap_materials = true;
        }
    }
    unsigned short material_a = swap_materials ? a : b;
    unsigned short material_b = swap_materials ? b : a;
    long object_index = swap_materials ? impact->object_index_b : impact->object_index_a;
    impacts_find_material_effects(object_index, impact->contact_type, &impact->object_position,
        impact->sweetener_size, impact->is_between_pair_of_dynamic_rigid_bodies(), position, material_b, material_a, state,
        effects);
}
// match: 0x115740: the inline score member retains the accepted opaque callee summary.
static __forceinline long impacts_first_active_user()
{
    long result = -1;
    for (long user_index = 0; user_index < 4; ++user_index) {
        if (tls_player_mapping_globals()->player_indices[user_index] != -1) {
            result = user_index;
            break;
        }
    }
    return result;
}

// @address 0x115740 ?get_closest_observer_distance_squared@c_impact@@CAMPBTreal_point3d@@W4e_states@1@@Z
float c_impact::get_closest_observer_distance_squared(const real_point3d *point, e_states state)
{
    float closest = 2.5e+07f;
    long user_index = impacts_first_active_user();
    if (user_index != -1) {
        float origin_x = point->x;
        float origin_y = point->y;
        float origin_z = point->z;
        s_player_globals *observers = observer_get_globals();
        real_vector3d vector;
        vector.j = 0.0f;
        float scale = get_impact_state_score_scale(state);
        do {
            const real_point3d *camera = &observers->blocks[user_index].camera_result.position;
            vector.i = camera->x - origin_x;
            vector.j = camera->y - origin_y;
            vector.k = camera->z - origin_z;
            real_vector3d scaled;
            scale_vector3d(&vector, scale, &scaled);
            float distance_squared = impacts_magnitude_squared(&scaled);
            if (!(distance_squared > closest))
                closest = distance_squared;
            user_index = players_next_active_user(user_index);
        } while (user_index != -1);
    }
    return closest;
}
// match: 0x115848: keep the owned call out of line while suppressing its register-use summary.
// @address 0x115848 ?get_impact_state_score_scale@c_impact@@CAMW4e_states@1@@Z
float c_impact::get_impact_state_score_scale(e_states state)
{
    switch (state) {
    case state2: return 1.0f;
    case state3: return 1.0f;
    case state0: return 1.0f;
    case state1: return 1.0f;
    // match: 0x115848: unreachable hint omits dead branch check
    default: __assume(0);
    }
}
// match: 0x248478: retain the unverified datum-getter boundary with the shared iterator.
static __forceinline c_havok_contact_point *impacts_iterator_contact_point(c_havok_contact_point_iterator *iterator)
{
    object_datum *object = impacts_creation_object_get(iterator->object_index, -1);
    c_havok_component *component = impacts_contact_component_get(g_havok_component_data, object->havok_component);
    return component->get_contact_point(iterator->rigid_body_index, iterator->contact_point_index);
}

struct s_marker_entity;

// @address 0x249fe0 ?update_looping_sound@c_impact@@QAAXPAUs_sound_location@@PAM@Z
void c_impact::update_looping_sound(s_sound_location *location, float *volume)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index_a, -1));
    float strength = collision_type == 3 ? volume_a : volume_b;
    if (use_marker) {
        object_marker marker;
        if (internal_object_get_markers_by_string_id(object_index_a, 0x25e, &marker, 1, false) == 1)
            location->position = marker.matrix.position;
        else object_get_origin(object_index_a, &location->position);
    } else {
        location->position = origin;
        if (physics_reference_count > 0 && object->havok_component != -1) {
            c_havok_component *first = havok_component_get(object->havok_component);
            if (object_index_b != -1) {
                object_datum *other_object = ((object_datum *)object_get_and_verify_type(object_index_b, -1));
                c_havok_component *second = havok_component_get(other_object->havok_component);
                bool first_found = false, second_found = false;
                real_point3d a, b;
                if (first->m_maximum_rigid_body_size <= 3 && impacts_controlling_body_index(first) != -1) {
                    ((c_havok_component *)(s_marker_entity *)first)
                        ->get_rigid_body_center_of_mass(impacts_controlling_body_index(first), &a);
                    location->position = a; first_found = true;
                }
                if (second->m_maximum_rigid_body_size <= 3 && impacts_controlling_body_index(second) != -1) {
                    ((c_havok_component *)(s_marker_entity *)second)
                        ->get_rigid_body_center_of_mass(impacts_controlling_body_index(second), &b);
                    location->position = b; second_found = true;
                }
                if (first_found && second_found) {
                    location->position.x = (a.x + b.x) * 0.5f;
                    location->position.y = (a.y + b.y) * 0.5f;
                    location->position.z = (a.z + b.z) * 0.5f;
                }
            } else if (first->m_maximum_rigid_body_size <= 3 && impacts_controlling_body_index(first) != -1)
                ((c_havok_component *)(s_marker_entity *)first)
                    ->get_rigid_body_center_of_mass(
                        impacts_controlling_body_index(first), &location->position);
        }
    }
    game_sound_location_set_rotation_from_direction(location, &velocity_direction);
    location->velocity = *global_zero_vector3d;
    ((impacts_spatial_location *)location)->location = object->location;
    *volume = (strength > 0.0f) ? ((strength < 1.0f) ? strength : 1.0f) : 0.0f;
}
// @address 0xe7880 ?next@c_havok_contact_point_iterator@@QAAXXZ
// @address 0xe7990 ?valid@c_havok_contact_point_iterator@@QAA_NXZ
// @address 0x247e50 ?get_constraint_rigid_bodies@c_havok_component@@QAAXJPAJ0@Z

// @address 0x113b00 ?calculate_rigid_body_indexes@c_impact@@AAAXJPAJ0@Z
static __forceinline c_havok_component *impacts_object_get_havok_component(s_object_header *headers, char *havok_data, long object_index)
{
    object_datum *object = (object_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
    return (c_havok_component *)(havok_data + (object->havok_component & 0xffff) * sizeof(c_havok_component));
}
// match: 0x113b00: without it the type loads hoist early, the component spills from r3 to r9, and the out stores reorder.
void c_impact::calculate_rigid_body_indexes(long impact_datum_index, long *__restrict rigid_body_index_a, long *__restrict
    rigid_body_index_b)
{
    char *havok_data = g_havok_component_data->data;
    s_object_header *headers = (s_object_header *)object_header_data->data;
    c_havok_component *havok_component = impacts_object_get_havok_component(headers, havok_data, get_object_index_a());
    switch (get_type())
    {
    case type2:
    {
        long matched_constraint_index = -1;
        for (long constraint_index = 0; constraint_index < havok_component->get_constraint_count(); constraint_index++)
        {
            s_physics_model_constraint_reference reference = havok_component->get_physics_model_constraint_reference(constraint_index);
            if (reference.type == constraint_reference.type && reference.index == constraint_reference.index)
                matched_constraint_index = constraint_index;
        }
        havok_component->get_constraint_rigid_bodies(matched_constraint_index, rigid_body_index_a, rigid_body_index_b);
        break;
    }
    case type0:
        *rigid_body_index_a = impacts_controlling_body_index(havok_component);
        *rigid_body_index_b = -1;
        break;
    case type1:
    {
        *rigid_body_index_a = -1;
        *rigid_body_index_b = -1;
        for (c_havok_contact_point_iterator iterator(havok_component->m_object_index, true); iterator.valid(); iterator.next())
        {
            c_havok_contact_point *havok_contact_point =
                impacts_object_get_havok_component(headers, havok_data,
                    iterator.object_index)->get_contact_point(iterator.rigid_body_index, iterator.contact_point_index);
            if (havok_contact_point->get_impact_datum_index() == impact_datum_index)
            {
                if (*rigid_body_index_a == -1 || havok_contact_point->get_rigid_body_index_a() < *rigid_body_index_a)
                    *rigid_body_index_a = havok_contact_point->get_rigid_body_index_a();
                if (havok_contact_point->get_contact_havok_component_index() != -1)
                {
                    if (*rigid_body_index_b == -1 || havok_contact_point->get_rigid_body_index_b() < *rigid_body_index_b)
                        *rigid_body_index_b = havok_contact_point->get_rigid_body_index_b();
                }
            }
        }
        break;
    }
    case type3:
        *rigid_body_index_a = 0;
        *rigid_body_index_b = -1;
        break;
    default:
        // match: 0x113b00: omits the type-switch range check; without it the dispatch emits cmplwi/bgt to default.
        __assume(0);
    }
}

// @address 0x248478 ?impact_delete@@YAXJ@Z
void impact_delete(long impact_index)
{
    // match: 0x248478: handle normalization preserves the constraint-loop evaluation order.
    c_impact *impact = (c_impact *)(TLS_IMPACT_DATA->data + (unsigned short)_rotl(impact_index, 0) * sizeof(c_impact));
    object_datum *object = ((object_datum *)impacts_creation_object_get(impact->get_object_index_a(), -1));
    if (impact->sorted_index != -1)
        g_impacts_globals->sorted_indexes[impact->sorted_index] = -1;
    if (impact->get_reference_count() != 0) {
        if (impact->get_type() == 3 || impact->get_type() == 0) {
            impact->remove_reference(impact_index, true);
        } else {
            c_havok_component *component = impacts_contact_component_get(g_havok_component_data, object->havok_component);
            c_havok_contact_point_iterator iterator(component->m_object_index, true);
            for (; iterator.valid(); iterator.next()) {
                c_havok_contact_point *point = impacts_iterator_contact_point(&iterator);
                if (point->get_impact_datum_index() == impact_index) {
                    impact->remove_reference(impact_index, true);
                    point->set_impact_datum_index(-1);
                }
            }
            for (long index = 0; index < component->get_constraint_count(); ++index) {
                if (component->get_constraint_impact_datum_index(index) == impact_index) {
                    impact->remove_reference(impact_index, true);
                    component->set_constraint_impact_datum_index(index, -1);
                }
            }
        }
    }
    impact->halt_managed_effects(true, true);
    if (object->havok_component != -1) {
        c_havok_component *component = impacts_contact_component_get(g_havok_component_data, object->havok_component);
        component->remove_impact(impact_index);
    }
    ::datum_delete((s_data_array *)TLS_IMPACT_DATA, impact_index);
}
// @address 0x2486c8 ?impacts_create_orphaned_impacts@@YAXXZ
void impacts_create_orphaned_impacts(void)
{
    c_object_iterator<object_datum> iterator;
    long count = 0;
    iterator.m_datum = 0;
    iterator.m_signature = 0x86868686;
    iterator.m_type_mask = 0x1cdf;
    iterator.m_header_mask = 0;
    iterator.m_match_flags = 0;
    long resume_index = g_impacts_globals->orphaned_impacts_resume_index;
    iterator.m_absolute_index = resume_index > 0 ? resume_index : 0;
    iterator.m_index = -1;
    g_impacts_globals->orphaned_impacts_resume_index = -1;
    if (!iterator.next())
        return;
    do {
        object_datum *object = iterator.get_datum();
        if (object->havok_component != -1) {
            c_havok_component *component = havok_component_get(object->havok_component);
            long datum_index = iterator.get_index();
            if (havok_component_has_active_entity(component)) {
                if (component->m_impact_array_index == -1 || impact_array_get(component->m_impact_array_index)->impact_count < 32) {
                    c_havok_contact_point_iterator contact_iterator(datum_index, false);
                    if (contact_iterator.valid()) {
                        do {
                            c_havok_contact_point *point = impacts_iterator_contact_point(&contact_iterator);
                            if (point->get_impact_datum_index() == -1)
                                impacts_create_orphaned_impact(datum_index, point, true);
                            contact_iterator.next();
                        } while (contact_iterator.valid());
                    }
                }
            }
            impacts_create_constraint_impacts(datum_index);
            if (++count > 32) {
                g_impacts_globals->orphaned_impacts_resume_index = (long)(unsigned short)datum_index + 1;
                return;
            }
        }
    } while (iterator.next());
}

// @address 0x2488c8 ?impacts_create_orphaned_impact@@YAXJPAVc_havok_contact_point@@_N@Z
void impacts_create_orphaned_impact(long object_index, c_havok_contact_point *contact_point, bool create)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    c_havok_component *havok_component_a = havok_component_get(object->havok_component);
    c_impact::s_contact_description contact_description;
    c_havok_component *havok_component_b = contact_point->get_contact_havok_component_index() != -1
        ? havok_component_get(contact_point->get_contact_havok_component_index())
        : 0;
    long impact_datum_index = -1;
    bool phantom_only = false;
    long object_index_b = contact_point->get_contact_havok_component_index() != -1 ? contact_point->get_contact_point_object_index() : -1;
    c_impact::initialize_contact_description(&contact_description, c_impact::type1, false,
        object_index, contact_point->get_rigid_body_index_a(), contact_point->calculate_global_material_type_a(),
        object_index_b, contact_point->get_rigid_body_index_b(), contact_point->calculate_global_material_type_b(),
        contact_point->get_position(), contact_point->get_normal(), -1, 0);
    if (havok_component_a->is_physics_model_havok_component()) {
        s_object_definition *object_definition = (s_object_definition *)tag_get('obje', object->definition_index);
        s_model_definition *model_definition = (s_model_definition *)tag_get('hlmt', object_definition->model.index);
        s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model_definition->physics_model_index);
        phantom_only = (physics_model->flags & (1UL << 1)) != 0;
    }
    impact_datum_index = havok_component_a->find_impact_which_includes_contact(&contact_description, true);
    if (impact_datum_index == -1 &&
        (havok_component_b == 0 || havok_component_b->find_impact_which_includes_contact(&contact_description, false) == -1) &&
        !phantom_only &&
        c_impact::passes_the_character_test(object_index, object_index_b, &contact_description.position, &contact_description.normal)) {
        c_havok_component *havok_component = havok_component_get(object->havok_component);
        float score = c_impact::calculate_score(&contact_description.position, c_impact::state1) * 1.1f;
        if (havok_component->make_space_for_new_impact(score, contact_point->get_rigid_body_index_a())) {
            impact_datum_index = impact_new_from_contact(&contact_description, c_impact::state1);
            havok_component->add_impact(impact_datum_index);
        }
    }
    if (impact_datum_index != -1) {
        c_impact *impact = (c_impact *)(TLS_IMPACT_DATA->data + (unsigned short)impact_datum_index * sizeof(c_impact));
        contact_point->set_impact_datum_index(impact_datum_index);
        impact->add_reference(impact_datum_index);
    }
}

// @rdata 0x820224f4
static const float k_constraint_impact_score_scale = 1.1f;
// @address 0x1131b8 impacts_create_constraint_impacts
extern "C" void impacts_create_constraint_impacts(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    if (object->havok_component == -1)
        return;
    c_havok_component *havok_component = havok_component_get(object->havok_component);
    if (!havok_component->is_physics_model_havok_component() || !havok_component_has_active_entity(havok_component))
        return;
    s_object_definition *object_definition = (s_object_definition *)tag_get('obje', object->definition_index);
    s_model_definition *model_definition = (s_model_definition *)tag_get('hlmt', object_definition->model.index);
    s_physics_model_definition *physics_model = (s_physics_model_definition *)tag_get('phmo', model_definition->physics_model_index);
    for (long constraint_index = 0; constraint_index < havok_component->get_constraint_count(); constraint_index++) {
        s_physics_model_constraint_reference reference = havok_component->get_physics_model_constraint_reference(constraint_index);
        if (reference.type == 6 || havok_component->get_constraint_impact_datum_index(constraint_index) != -1)
            continue;
        s_physics_model_constraint_definition *bodies = (s_physics_model_constraint_definition
            *)physics_model_try_and_get_constraint(physics_model, &reference);
        if (bodies->node_edge_index == -1)
            continue;
        s_physics_model_node_constraint_edge *edge = &((s_physics_model_node_constraint_edge
            *)physics_model->node_edges.address)[bodies->node_edge_index];
        bool create = true;
        for (long edge_constraint_index = 0; edge_constraint_index < edge->constraints.count; edge_constraint_index++) {
            s_physics_model_node_constraint_edge_constraint *edge_constraint = &((s_physics_model_node_constraint_edge_constraint
                *)edge->constraints.address)[edge_constraint_index];
            if (edge_constraint->type == reference.type && edge_constraint->index == reference.index) {
                if (edge_constraint->flags & (1 << 1))
                    create = false;
                break;
            }
        }
        if (create) {
            c_impact::s_contact_description contact_description;
            c_havok_component *component = havok_component_get(object->havok_component);
            long rigid_body_a, rigid_body_b;
            real_matrix4x3 transform;
            havok_component->get_constraint_rigid_bodies(constraint_index, &rigid_body_a, &rigid_body_b);
            havok_component->get_rigid_body_transform(rigid_body_a, &transform);
            impact_initialize_contact_description(&contact_description, c_impact::type2, false,
                object_index, rigid_body_a, edge->material_index_a,
                rigid_body_b != -1 ? object_index : -1, rigid_body_b, edge->material_index_b,
                &transform.position, &transform.rotation.forward, -1, &reference);
            long impact_datum_index = havok_component->find_impact_which_includes_contact(&contact_description, false);
            if (impact_datum_index == -1) {
                float score = c_impact::calculate_score(&contact_description.position, c_impact::state1) * k_constraint_impact_score_scale;
                if (component->make_space_for_new_impact(score, -1)) {
                    impact_datum_index = impact_new_from_contact(&contact_description, c_impact::state1);
                    component->add_impact(impact_datum_index);
                }
            }
            if (impact_datum_index != -1) {
                c_impact *impact = impacts_impact_datum_get(impact_datum_index);
                component->set_constraint_impact_datum_index(constraint_index, impact_datum_index);
                impact->add_reference(impact_datum_index);
            }
        }
    }
}

// @address 0x24a370 ?update_vehicle_impact_sound_only@c_impact@@QAAXJ@Z
void c_impact::update_vehicle_impact_sound_only(long impact_datum_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index_a, -1));
    object_position = object->up;
    object_get_origin(object_index_a, &origin);
    vehicle_sound_volume_a = 0.0f;
    vehicle_sound_volume_b = 0.0f;
    velocity_direction = object->linear_velocity;
    if (normalize3d(&velocity_direction) == 0.0f)
        velocity_direction = object->forward;
}

// @address 0x24a450 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_impact_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_impact_data_allocator>::allocate(unsigned int size, const char *name)
{
    int member_index = g_restricted_regions[1].add_member((const char *)name, "", size, 0, __tls_set_g_device_groups_data_allocator, 0, 0);
    unsigned int offset = g_restricted_regions[1].get_member_offset(member_index);
    data_set_new_base_address((s_data_array **)&TLS_IMPACT_DATA, (s_data_array *)(g_restricted_address[1] + offset));
    m_member_index = member_index;
    m_thread_id = g_registered_thread_index;
    return g_restricted_address[1] + g_restricted_regions[1].get_member_offset(member_index);
}
// @address 0x24a4f0 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_impact_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_impact_data_allocator>::deallocate(void *address)
{
    TLS_IMPACT_DATA = 0;
    restricted_region_free_member(1, m_member_index);
}
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_impact_data_allocator> g_impact_data_allocator;
template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *), void
    (*k_set_allocator_function)(void *)>
void *t_restricted_allocation_manager<k_region_index, k_adopt_function, k_post_copy_function, k_set_allocator_function>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x24a5d0: folds the alignment_bits add_member argument to a zero immediate (li r7,0); without it the parameter register passes through.
    __assume(alignment_bits == 0);
    long index = g_restricted_regions[k_region_index].add_member((const char *)name, (const char *)type, size, alignment_bits,
        k_set_allocator_function, k_adopt_function, k_post_copy_function);
    void *address = g_restricted_address[k_region_index] + g_restricted_regions[k_region_index].get_member_offset(index);
    g_impacts_globals = (impacts_globals_type *)address;
    m_member_index = index;
    m_thread_id = g_registered_thread_index;
    return g_restricted_address[k_region_index] + g_restricted_regions[k_region_index].get_member_offset(m_member_index);
}
template void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_impacts_globals_allocator>::reserve_memory(const char *, const char *, unsigned int, long);
// @address 0x24a5d0 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_impacts_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x24a670 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_impacts_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_impacts_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x45fcc0 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_impact_array_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_impact_array_data_allocator>::allocate(unsigned int size, const char *name)
{
    long member_index = g_restricted_regions[2].add_member(name, "", size, 0, __tls_set_g_impact_array_data_allocator, 0, 0);
    void *address = restricted_region_get_member_address(2, member_index);
    data_set_new_base_address(&g_particle_gpu_block_data, (s_data_array *)address);
    m_member_index = member_index;
    m_thread_id = g_registered_thread_index;
    return restricted_region_get_member_address(2, m_member_index);
}

// @address 0x45fd60 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_impact_array_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_impact_array_data_allocator>::deallocate(void *address)
{
    g_particle_gpu_block_data = 0;
    restricted_region_free_member(2, m_member_index);
}

// @address 0x1159e0 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_impacts_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_impacts_globals_allocator>::deallocate(void *address)
{
    long value = m_member_index;
    g_impacts_globals = 0;
    restricted_region_free_member(1, value);
}

// @address 0x2494b0 ?remove_reference@c_impact@@QAAXJ_N@Z
void c_impact::remove_reference(long impact_index, bool deleting)
{
    if (!deleting && physics_reference_count == 1 && get_type() == type1) {
        if (state >= 0 && state <= 1) {
            if (last_reference_time == -1 || tls_game_time_globals()->time - last_reference_time > 2) {
                long rigid_body_index_a;
                long rigid_body_index_b;
                calculate_rigid_body_indexes(impact_index, &rigid_body_index_a, &rigid_body_index_b);
                queue_collision(reference_collision_score);
                update_effects(impact_index, rigid_body_index_a, rigid_body_index_b);
            }
        }
    }
    --physics_reference_count;
}


// @address 0x249278 ?includes_point@c_impact@@QAA_NPBUs_contact_description@1@_N@Z
bool c_impact::includes_point(const s_contact_description *contact_description, bool test_space)
{
    unsigned int contact_type;
    long first;
    long second;
    unsigned int matches;
    bool passes;
    unsigned char distance_storage[16];
    contact_type = contact_description->type;
    if ((int)contact_type == (int)type) {
        second = contact_description->object_b;
        first = contact_description->object_a;
        matches = impacts_contact_descriptions_match(object_index_a, object_index_b, first, second,
            material_a.m_index, material_b.m_index,
            contact_description->material_a.m_index, contact_description->material_b.m_index,
            (int)this->contact_type, contact_description->vehicle_friction_point_index,
            contact_flags, contact_description->disconnected_from_physics,
            (int)constraint_reference.type, (int)contact_description->constraint_type);
        if ((((matches & 0xff) != 0) &&
            ((!test_space ||
              (passes = included_in_impact_space(&origin, &object_position,
                &contact_description->position, &contact_description->normal,
                (float *)distance_storage), passes)))) &&
            ((contact_type == 3U ||
              (passes = passes_the_character_test(first, second,
                &contact_description->position, &contact_description->normal), passes)))) {
            return true;
        }
    }
    return false;
}



static __forceinline const s_object_definition *impacts_get_sweetener_definition(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    return (const s_object_definition *)tag_get('obje', object->definition_index);
}
static __forceinline void impacts_set_dynamic_pair_flag(c_impact *impact, bool enabled)
{
    if (enabled)
        impact->flags |= 2;
    else
        impact->flags &= ~2;
}
static __forceinline void impacts_set_materials(c_impact *impact, const c_impact::s_contact_description *description)
{
    impact->material_a = description->material_a;
    impact->material_b = description->material_b;
}
static __forceinline void impacts_set_contact_normal(c_impact *impact, const real_vector3d &normal)
{
    impact->object_position = normal;
}
static __forceinline bool impacts_second_is_faster(const real_vector3d &first, const real_vector3d &second)
{
    return impacts_magnitude_squared(&second) > impacts_magnitude_squared(&first);
}
// match: 0x2481a0: hoists the description loads above the impact stores; without it loads interleave with stores and fields reload after them.
__forceinline long c_impact::create(const s_contact_description *__restrict description, e_states state)
{
    long index = datum_new((s_data_array *)TLS_IMPACT_DATA);
    c_impact *impact = (c_impact *)(TLS_IMPACT_DATA->data + (unsigned short)index * sizeof(c_impact));
    const s_object_definition *definition_a = impacts_get_sweetener_definition(description->object_a);
    if (description->object_b != -1) {
        unsigned char size_a = definition_a->sweetener_size;
        unsigned char size_b = impacts_get_sweetener_definition(description->object_b)->sweetener_size;
        impact->sweetener_size = size_a <= size_b ? size_a : size_b;
    } else {
        impact->sweetener_size = definition_a->sweetener_size;
    }
    impact->flags = 0;
    impact->type = (e_type)description->type;
    impact->score = get_closest_observer_distance_squared(&description->position, state);
    impact->activation_state = (signed char)state;
    impact->physics_reference_count = 0;
    impact->active = false;
    impact->object_index_a = description->object_a;
    impact->object_index_b = description->object_b;
    impact->contact_type = (signed char)description->vehicle_friction_point_index;
    impact->contact_flags = description->disconnected_from_physics;
    impact->effect_pending = false;
    impacts_set_materials(impact, description);
    impact->looping_sound_index_a = -1;
    impact->looping_sound_a = -1;
    impact->looping_sound_index_b = -1;
    impact->looping_sound_b = -1;
    impact->managed_effect_index_a = -1;
    impact->managed_effect_index_b = -1;
    impact->managed_effect_index_c = -1;
    impact->effect_index_a = -1;
    impact->state = 0;
    impact->last_reference_time = 0;
    impact->queued_flag = 0;
    impact->reference_collision_score = 0.0f;
    impact->queued_score = 0.0f;
    impact->effect_state_a = 127;
    impact->effect_state_b = 127;
    impact->effect_index_b = -1;
    impact->effect_state_c = 127;
    impact->object_position = description->normal;
    normalize3d_with_default(&impact->object_position, global_up3d);
    impact->origin = description->position;
    impact->volume_a = 0.0f;
    impact->volume_b = 0.0f;
    impact->sorted_index = -1;
    impact->effect_elapsed_time = 0.0f;
    impact->has_sound_a = false;
    impact->has_sound_b = false;
    impact->constraint_reference = description->constraint_reference;
    impact->queued_score_b = 0.0f;
    impact->queued_tick = -1;
    bool second_is_faster = false;
    if (description->object_b != -1) {
        object_datum *object_a = ((object_datum *)impacts_creation_object_get(description->object_a, -1));
        object_datum *object_b = ((object_datum *)impacts_creation_object_get(description->object_b, -1));
        second_is_faster = impacts_second_is_faster(object_a->linear_velocity, object_b->linear_velocity);
    }
    impacts_set_dynamic_pair_flag(impact, second_is_faster);
    if (description->vehicle_friction_point_index == -1) {
        ++impact->physics_reference_count;
        impact->update_rigid_body_space_positions(&impact->origin, description->node_index_a, description->node_index_b);
        impact->remove_reference(index, true);
    }
    return index;
}
// @address 0x2481a0 impact_new_from_contact
extern "C" long impact_new_from_contact(const c_impact::s_contact_description *description, c_impact::e_states state)
{
    state = c_impact::state1;
    return c_impact::create(description, state);
}

// @address 0x2490d8 ?get_impact_sound_scale_type@@YA?AW4e_impact_sound_scale_type@@JJJ@Z
e_impact_sound_scale_type get_impact_sound_scale_type(long material_index, long object_index_a, long object_index_b)
{
    e_impact_sound_scale_type result = _impact_sound_scale_none;
    for (;;) {
    if (material_index == -1) {
        long type_mask = 1 << object_header_get(object_index_a)->object_type;
        if (type_mask & 0x801) { result = _impact_sound_scale_character; break; }
        if (type_mask & 2) { result = _impact_sound_scale_vehicle; break; }
    }
    if (object_index_a != -1 && object_index_b != -1) {
        object_datum *object_a = ((object_datum *)object_get_and_verify_type(object_index_a, -1));
        object_datum *object_b = ((object_datum *)object_get_and_verify_type(object_index_b, -1));
        if (object_a->havok_component != -1 && object_b->havok_component != -1) {
            c_havok_component *component_a = havok_component_get(object_a->havok_component);
            c_havok_component *component_b = havok_component_get(object_b->havok_component);
            long body_index_a = impacts_controlling_body_index(component_a);
            long body_index_b = impacts_controlling_body_index(component_b);
            if (body_index_a != -1 && body_index_b != -1) {
                if (!component_a->m_rigid_bodies[body_index_a].m_rigid_body->body_is_fixed() &&
                    !component_a->m_rigid_bodies[body_index_a].m_rigid_body->body_is_keyframed()) {
                    if (!component_b->m_rigid_bodies[body_index_b].m_rigid_body->body_is_fixed() &&
                        !component_b->m_rigid_bodies[body_index_b].m_rigid_body->body_is_keyframed())
                        result = _impact_sound_scale_dynamic;
                }
            }
        }
    }
    break;
    }
    return result;
}


// @address 0x115610 impacts_find_material_effects
extern "C" void impacts_find_material_effects(long p0, long p1, const real_vector3d *vector, long p3, bool p4, const real_point3d *point,
    unsigned short p6, unsigned short p7, long p8, long *p9)
{
    real_point3d adjusted_point;
    s_location location;
    long material_effects_index = -1;
    long effect_type;
    c_global_material_type material_a;
    c_global_material_type material_b;

    adjusted_point = *point;
    if (!p4)
    {
        adjusted_point.x = g_havok_constants.minimum_radius * vector->i + adjusted_point.x;
        adjusted_point.y = vector->j * g_havok_constants.minimum_radius + adjusted_point.y;
        adjusted_point.z = vector->k * g_havok_constants.minimum_radius + adjusted_point.z;
    }

    scenario_location_from_point(&location, &adjusted_point);
    if (p0 != -1 && p1 == -1)
    {
        object_datum *object = (object_datum *)datum_get<s_object_header>(object_header_data, p0)->datum;
        s_object_definition *definition = (s_object_definition *)tag_get('obje', object->definition_index);
        material_effects_index = definition->material_effects.index;
    }

    if (p8 != 0)
        effect_type = (p8 - 2 == 0) + 13;
    else
        effect_type = p3 + 10;

    material_a.m_index = p6;
    material_b.m_index = p7;
    material_effect_find_effects(false, point, material_effects_index, material_a, material_b, false, (enum e_effect_type)effect_type,
        (enum e_sweetener_size)p3, p9);
}

struct s_effect_lightprobe_source;
struct s_sound_tracker;
struct s_sound_tracking_data;
struct s_game_sound_deterministic_permutation;
static inline real_plane3d *impacts_plane_from_designator(const collision_bsp *bsp, long designator, real_plane3d *result)
{
    const real_plane3d *plane = &((const real_plane3d *)bsp->planes.address)[designator & 0x7fff];
    if (designator & 0x8000) {
        result->normal.i = -plane->normal.i;
        result->normal.j = -plane->normal.j;
        result->normal.k = -plane->normal.k;
        result->distance = -plane->distance;
    } else *result = *plane;
    return result;
}
static __forceinline bool impacts_can_interrupt_looping(long state, float scale, float impulse_scale)
{
    bool result = false;
    if (state == 2 && (scale < 0.5f || impulse_scale > 0.25f))
        result = true;
    return result;
}
__forceinline long c_game_sound_impulse_parameters::submit_unattached_sound(const s_sound_location *location,
    s_game_sound_impulse_internal_parameters *storage)
{
    s_game_sound_impulse_internal_parameters &parameters = *storage;
    parameters.additional_override_flags = 0;
    parameters.source.volume_type = 0;
    parameters.source.flags = 0;
    memcpy(&parameters.source.position, location, sizeof(impacts_spatial_location));
    parameters.source.volume = scale;
    parameters.source.spatialization = 17;
    parameters.source.name = 0;
    parameters.object_index = -1;
    parameters.playback_label = game_sound_find_platform_playback_by_label(sound_impulse_index);
    parameters.tracking_data = 0;
    parameters.tracking = 0;
    parameters.permutation = deterministic;
    return game_sound_impulse_new_internal(definition_index, &parameters);
}
static inline void impacts_initialize_unattached_effect(s_effect_creation_data *creation_data, c_tag_index definition, long vector_count,
    const s_effect_vector *vectors, const real_plane3d *plane, const s_location *location, e_effect_deterministic deterministic)
{
    effect_creation_data_initialize(creation_data);
    creation_data->flags = 0;
    creation_data->definition_index = definition;
    creation_data->effect_vector_count = vector_count;
    creation_data->effect_vectors = vectors;
    creation_data->plane = plane;
    if (deterministic == _effect_deterministic)
        creation_data->flags |= 1 << _effect_creation_deterministic_bit;
    if (location)
        creation_data->location_hint = *location;
}
static inline long impacts_new_unattached_effect(c_tag_index definition, float scale, long object_index, long vector_count,
    const s_effect_vector *vectors, const real_plane3d *plane, const s_location *location, e_effect_deterministic deterministic)
{
    s_effect_creation_data creation_data;
    creation_data.flags = 0;
    impacts_initialize_unattached_effect(&creation_data, definition, vector_count, vectors, plane, location, deterministic);
    creation_data.scale_a = scale;
    creation_data.structure_bsp_index = object_index;
    creation_data.scale_b = scale;
    return effect_new_from_creation_data(&creation_data);
}
static __forceinline void impacts_set_looping_alternate(long index, bool alternate)
{
    game_looping_sound_datum *sound = datum_get<game_looping_sound_datum>(game_looping_sound_data, index);
    if (alternate) sound->flags.set(4, true);
    else sound->flags.set(4, false);
}
static __forceinline void impacts_get_body_linear_velocity(c_havok_component *component, long body_index, real_vector3d *velocity)
{
    phys_rigid_body *body = component->m_rigid_bodies[body_index].m_rigid_body;
    if (!body->body_is_fixed()) {
        const phys_vec4 &linear_velocity = body->get_linear_velocity();
        velocity->i = linear_velocity(0);
        velocity->j = linear_velocity(1);
        velocity->k = linear_velocity(2);
    } else *velocity = *global_zero_vector3d;
}
static const float k_impact_loop_state_transition_delay = 0.3f; // @rdata 0x82022518
// @address 0x1145a8 ?update_effects@c_impact@@QAAXJJJ@Z
void c_impact::update_effects(long impact_index, long rigid_body_a, long rigid_body_b)
{
    s_object_header *headers = (s_object_header *)object_header_data->data;
    object_datum *object = (object_datum *)headers[datum_index_to_absolute_index(get_object_index_a())].datum;
    long impulse_effect_a = -1;
    long impulse_effect_b = -1;
    const real_plane3d *plane = 0;
    impacts_sound_context sound_context;
    real_plane3d &contact_plane = sound_context.contact_plane;
    real_point3d point = *get_world_space_position();
    real_vector3d water_normal = global_identity4x3->up;
    if (get_reference_count() != 0 && get_object_index_b() == -1) {
        for (c_havok_contact_point_iterator iterator(get_object_index_a(), false); iterator.valid(); iterator.next()) {
            c_havok_component *component = impacts_object_get_havok_component(headers, g_havok_component_data->data, iterator.object_index);
            c_havok_contact_point *contact = component->get_contact_point(iterator.rigid_body_index, iterator.contact_point_index);
            if (contact->get_impact_datum_index() == impact_index && contact->m_structure_bsp_index != -1 &&
                contact->m_environment_shape_key != -1) {
                unsigned long shape_key = contact->m_environment_shape_key;
                long surface_index = shape_key & 0xffff;
                if ((shape_key & 0xe0000000) != 0x20000000)
                    continue;
                const __int64 &cache_user_index = g_registered_thread_index;
                const collision_bsp *bsp =
                    g_collision_bsp_cache.thread_users[cache_user_index].collision_bsps[contact->m_structure_bsp_index];
                long designator = ((const collision_surface *)bsp->surfaces.address)[surface_index].plane;
                real_plane3d surface_plane;
                impacts_plane_from_designator(bsp, designator, &surface_plane);
                contact_plane = surface_plane;
                plane = &contact_plane;
                break;
            }
        }
    }
    calculate_water_surface_vector(&point, &water_normal);
    s_effect_vector vectors[3];
    vectors[0].marker_name = _string_id_impact;
    vectors[0].point = point;
    vectors[0].vector = object_position;
    vectors[1].marker_name = _string_id_water_surface;
    vectors[1].point = point;
    vectors[1].vector = water_normal;
    vectors[2].marker_name = _string_id_up;
    vectors[2].point = point;
    vectors[2].vector = global_identity4x3->up;
    s_location location;
    scenario_location_from_point(&location, &point);
    if (location.cluster_reference.bsp_index == -1)
        location = object->location;
    impacts_material_effects effects;
    if (queued_flag && g_havok_constants.enable_impact_effects && (signed char)effect_state_a > 2 &&
        (!(effect_index_a >= 2 && effect_index_a <= 3) ||
         impacts_can_interrupt_looping(effect_index_a, effect_index_a == 3 ? volume_a : volume_b, queued_score))) {
        float scale = queued_score;
        scale = PIN(scale, 0.0f, 1.0f);
        impacts_spatial_location spatial;
        s_sound_location &sound_location = spatial.sound;
        sound_location.position = *get_world_space_position();
        last_reference_time = tls_game_time_globals()->time;
        game_sound_location_set_rotation_from_direction(&sound_location, &velocity_direction);
        sound_location.velocity = *global_zero_vector3d;
        spatial.location = object->location;
        impact_resolve_material_effects(this, get_world_space_position(), get_global_material_index_a().m_index,
            get_global_material_index_b().m_index, 0, effects.indices);
        for (long index = 0; index < 3; index++) {
            if (effects.sounds[index] != -1)
            {
                c_game_sound_impulse_parameters(effects.sounds[index], scale).submit_unattached_sound(&sound_location,
                    &sound_context.sound_parameters);
            }
        }
        if (rigid_body_index == 0xffff) {
            for (unsigned long index = 0; index < 3; index++) {
                if (effects.effects[index] != -1) {
                    long effect = impacts_new_unattached_effect(effects.effects[index], scale, -1, 3, vectors, plane, &location,
                        _effect_non_deterministic);
                    if (effect != -1) {
                        if ((long)index == 0) impulse_effect_a = effect;
                        else impulse_effect_b = effect;
                    }
                }
            }
        }
        queued_tick = tls_game_time_globals()->time;
        queued_score_b = scale;
        effect_state_a = 0;
        state = 0;
    } else {
        if (activation_state == 1 && (float)(signed char)effect_state_b * tls_game_time_globals()->tick_length > 0.5f &&
            is_managing_effect()) {
            halt_managed_effects(true, true);
            effect_index_a = -1;
        } else if (activation_state >= 2 && activation_state <= 3 && has_sound_b > 2 &&
                   activation_state != effect_index_a && (float)(signed char)effect_state_b * tls_game_time_globals()->tick_length > 0.5f) {
            bool preserve_looping_sounds = false;
            if (activation_state != effect_index_b) {
                effect_state_c = 0;
                effect_index_b = activation_state;
            }
            if (effect_index_b == activation_state &&
                ((float)(signed char)effect_state_c * tls_game_time_globals()->tick_length > k_impact_loop_state_transition_delay ||
                    !is_managing_effect())) {
                if (is_managing_effect()) {
                    preserve_looping_sounds = effect_index_a >= 2 && effect_index_a <= 3 && effect_index_b >= 2 && effect_index_b <= 3;
                    halt_managed_effects(true, !preserve_looping_sounds);
                }
                effect_index_a = -1;
            }
            if (g_havok_constants.enable_impact_effects && (!is_managing_effect() || preserve_looping_sounds)) {
                bool play_sound;
                if (contact_type == -1 || contact_flags) play_sound = true;
                else play_sound = false;
                bool play_effect;
                if ((contact_type == -1 || !contact_flags) && rigid_body_index == 0xffff) play_effect = true;
                else play_effect = false;
                float scale = activation_state == 3 ? volume_a : volume_b;
                scale = PIN(scale, 0.0f, 1.0f);
                impact_resolve_material_effects(this, get_world_space_position(), get_global_material_index_a().m_index,
                    get_global_material_index_b().m_index, activation_state, effects.indices);
                if (play_sound) {
                    if (preserve_looping_sounds) {
                        if (looping_sound_index_a != -1)
                            impacts_set_looping_alternate(looping_sound_index_a, activation_state != 2);
                        if (looping_sound_index_b != -1)
                            impacts_set_looping_alternate(looping_sound_index_b, activation_state != 2);
                    } else {
                        if (effects.sounds[0] != -1 && !(get_type() == type1 && currently_playing_looping_sound(impact_index,
                            effects.sounds[0]))) {
                            looping_sound_index_a = impact_looping_sound_start(effects.sounds[0], impact_index);
                            if (looping_sound_index_a != -1) {
                                looping_sound_a = effects.sounds[0];
                                impacts_set_looping_alternate(looping_sound_index_a, activation_state != 2);
                            }
                        }
                        if (effects.sounds[1] != -1 && !(get_type() == type1 && currently_playing_looping_sound(impact_index,
                            effects.sounds[1]))) {
                            looping_sound_index_b = impact_looping_sound_start(effects.sounds[1], impact_index);
                            if (looping_sound_index_b != -1) {
                                looping_sound_b = effects.sounds[1];
                                impacts_set_looping_alternate(looping_sound_index_b, activation_state != 2);
                            }
                        }
                    }
                }
                if (play_effect) {
                    if (effects.effects[0] != -1)
                        managed_effect_index_a = effect_new_impact_effect(
                            effects.effects[0], 0, object_index_a, 0, 3, vectors, plane, &location, scale, _effect_non_deterministic);
                    if (effects.effects[2] != -1)
                        managed_effect_index_b = effect_new_impact_effect(
                            effects.effects[2], 0, object_index_a, 0, 3, vectors, plane, &location, scale, _effect_non_deterministic);
                    if (effects.effects[1] != -1)
                        managed_effect_index_c = effect_new_impact_effect(
                            effects.effects[1], 0, object_index_a, 0, 3, vectors, plane, &location, scale, _effect_non_deterministic);
                }
                if (is_managing_effect()) {
                    effect_state_b = 0;
                    effect_index_a = activation_state;
                }
            }
        }
    }
    effect_state_c = MIN((signed char)effect_state_c + 1, 126);
    effect_state_a = MIN((signed char)effect_state_a + 1, 126);
    effect_state_b = MIN((signed char)effect_state_b + 1, 126);
    if (managed_effect_index_a != -1 || managed_effect_index_b != -1 || managed_effect_index_c != -1 || impulse_effect_a != -1 ||
        impulse_effect_b != -1) {
        float impulse_scale = PIN(queued_score, 0.0f, 1.0f);
        float loop_scale = effect_index_a == 3 ? PIN(volume_a, 0.0f, 1.0f) : PIN(volume_b, 0.0f, 1.0f);
        c_havok_component *component = havok_component_get(object->havok_component);
        real_matrix4x3 matrix;
        real_point2d coordinates;
        calculate_effect_parameters(impact_index, rigid_body_a, rigid_body_b, &matrix.position, &matrix.up, &matrix.forward, &matrix.left,
            &coordinates.x, &coordinates.y, false);
        scale_vector3d(&matrix.left, -1.0f, &matrix.left);
        real_vector3d velocity;
        impacts_get_body_linear_velocity(component, rigid_body_a, &velocity);
        if (get_reference_count() != 0 && get_object_index_b() != -1) {
            c_havok_component *other = havok_component_get(((object_datum *)object_get_and_verify_type(get_object_index_b(),
                -1))->havok_component);
            real_vector3d other_velocity;
            other->get_rigid_body_linear_velocity(rigid_body_b, &other_velocity);
            subtract_vectors3d(&velocity, &other_velocity, &velocity);
        }
        if (managed_effect_index_a != -1 &&
            !effect_ping_from_impact(managed_effect_index_a, loop_scale, 0.0f, &matrix, &velocity, &coordinates, plane))
            managed_effect_index_a = -1;
        if (managed_effect_index_b != -1 &&
            !effect_ping_from_impact(managed_effect_index_b, loop_scale, 0.0f, &matrix, &velocity, &coordinates, plane))
            managed_effect_index_b = -1;
        if (managed_effect_index_c != -1 &&
            !effect_ping_from_impact(managed_effect_index_c, loop_scale, 0.0f, &matrix, &velocity, &coordinates, plane))
            managed_effect_index_c = -1;
        if (impulse_effect_a != -1) effect_ping_from_impact(impulse_effect_a, impulse_scale, impulse_scale, &matrix, &velocity,
            &coordinates, plane);
        if (impulse_effect_b != -1) effect_ping_from_impact(impulse_effect_b, impulse_scale, impulse_scale, &matrix, &velocity,
            &coordinates, plane);
    }
    queued_flag = 0;
}

// @address 0x1153f8 ?calculate_water_surface_vector@c_impact@@SAXPATreal_point3d@@PATreal_vector3d@@@Z
// match: 0x1153f8: pointer __restrict qualifier prevents alias reload
void c_impact::calculate_water_surface_vector(real_point3d * __restrict point, real_vector3d *vector)
{
    s_water_physics_test_sphere_result result;
    result.material_type.m_index = -1;
    if (water_physics_test_sphere(true, point, 0.0f, &result))
    {
        real_vector3d up = { 0.0f, 0.0f, 1.0f };
        real_point3d surface_point;
        point_from_line3d(point, &up, result.depth, &surface_point);
        point->z = surface_point.z;
        *vector = result.surface_plane.normal;
        point->x = surface_point.x;
        point->y = surface_point.y;
    }
}

// @address 0x248b70 ?impacts_merge@@YAXXZ
void impacts_merge()
{
    c_data_iterator<c_impact> iterator;
    iterator.begin(TLS_IMPACT_DATA);
    while (iterator.next()) {
        c_impact *impact = iterator.get_datum();
        object_datum *object = ((object_datum *)object_get_and_verify_type(impact->get_object_index_a(), -1));
        bool impact_is_managing = impact->looping_sound_index_a != -1 || impact->looping_sound_index_b != -1 ||
            impact->managed_effect_index_a != -1 || impact->managed_effect_index_b != -1 || impact->managed_effect_index_c != -1;
        long impact_datum_index = iterator.get_index();
        bool impact_updated = false;
        if (object->havok_component != -1) {
            c_havok_component *component = havok_component_get(object->havok_component);
            for (long index = 0; index < component->get_impact_count(); ++index) {
                long other_impact_index = impacts_component_impact_datum_index(component, index);
                if (other_impact_index != impact_datum_index) {
                    c_impact *impact_test = impacts_impact_datum_get(other_impact_index);
                    bool impact_test_is_managing = impact_test->looping_sound_index_a != -1 || impact_test->looping_sound_index_b != -1 ||
                        impact_test->managed_effect_index_a != -1 || impact_test->managed_effect_index_b != -1 ||
                        impact_test->managed_effect_index_c != -1;
                    if (impact_test->get_reference_count() != 0 &&
                        impact->reference_collision_score > impact_test->reference_collision_score &&
                        distance_squared3d(impact->get_world_space_position(), impact_test->get_world_space_position()) < 2.25f &&
                        ((impact_is_managing && !impact_test_is_managing) || (impact_is_managing == impact_test_is_managing))) {
                        bool merged = false;
                        c_havok_contact_point_iterator contact_point_iterator(component->m_object_index, false);
                        for (; contact_point_iterator.valid(); contact_point_iterator.next()) {
                            c_havok_contact_point *contact_point = impacts_iterator_contact_point(&contact_point_iterator);
                            if (contact_point->get_impact_datum_index() == other_impact_index) {
                                c_impact::s_contact_description contact_description;
                                c_impact::initialize_contact_description(&contact_description, c_impact::type1, false,
                                    impact_test->get_object_index_a(), contact_point->get_rigid_body_index_a(), impact_test->material_a,
                                    impact_test->get_object_index_b(), contact_point->get_rigid_body_index_b(), impact_test->material_b,
                                    contact_point->get_position(), contact_point->get_normal(), -1, 0);
                                if (impact->includes_point(&contact_description, true)) {
                                    impact_test->remove_reference(other_impact_index, true);
                                    impact_updated = true;
                                    merged = true;
                                    impact->add_reference(impact_datum_index);
                                    contact_point->set_impact_datum_index(impact_datum_index);
                                }
                            }
                        }
                        if (merged && impact_test->get_reference_count() != 0)
                            impact_test->update(other_impact_index);
                    }
                }
            }
        }
        if (impact_updated)
            impact->update(impact_datum_index);
    }
}

// @address 0x249718 ?calculate_effect_parameters@c_impact@@QAAXJJJPATreal_point3d@@PATreal_vector3d@@11PAM2_N@Z
void c_impact::calculate_effect_parameters(long impact_datum_index, long node_index, long, real_point3d *rectangle_center,
    // match: 0x249718: pointer __restrict qualifier prevents alias reload
    real_vector3d *rectangle_forward, real_vector3d *rectangle_up, real_vector3d *rectangle_left, float *__restrict half_x_dimension,
    float *half_y_dimension, bool)
{
    real_vector3d delta;
    *rectangle_up = object_position;
    *rectangle_center = origin;
    if (rigid_body_index != 0xffff)
    {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index_a, -1));
        c_havok_component *component = havok_component_get(object->havok_component);
        real_matrix4x3 transform;
        component->get_rigid_body_transform(node_index, &transform);
        *rectangle_center = transform.position;
        *rectangle_forward = transform.rotation.forward;
        *rectangle_up = transform.rotation.up;
        *rectangle_left = transform.rotation.left;
        *half_x_dimension = 0.0f;
        *half_y_dimension = 0.0f;
    }
    else if (physics_reference_count <= 1)
    {
        *half_x_dimension = 0.0f;
        *half_y_dimension = 0.0f;
        cross_product3d(rectangle_up, global_up3d, rectangle_forward);
        if (normalize3d(rectangle_forward) < 0.001f)
        {
            cross_product3d(rectangle_up, global_forward3d, rectangle_forward);
            normalize3d(rectangle_forward);
        }
        cross_product3d(rectangle_up, rectangle_forward, rectangle_left);
        normalize3d(rectangle_left);
    }
    else
    {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index_a, -1));
        c_havok_component *component = havok_component_get(object->havok_component);
        bool is_pair = is_between_pair_of_dynamic_rigid_bodies();
        float single_min_x = 3.4028234663852886e+38f;
        float single_max_x = -3.4028234663852886e+38f;
        float single_min_y = 3.4028234663852886e+38f;
        float single_max_y = -3.4028234663852886e+38f;
        float pair_min_x = 3.4028234663852886e+38f;
        float pair_max_x = -3.4028234663852886e+38f;
        float pair_min_y = 3.4028234663852886e+38f;
        float pair_max_y = -3.4028234663852886e+38f;
        real_matrix3x3 single_matrix;
        real_matrix3x3 pair_matrix;
        calculate_impact_matrix(object_index_a, &single_matrix);
        if (is_pair)
            calculate_impact_matrix(object_index_b, &pair_matrix);
        for (c_havok_contact_point_iterator iterator(component->m_object_index, true); iterator.valid(); iterator.next())
        {
            c_havok_contact_point *contact_point = impacts_iterator_contact_point(&iterator);
            if (contact_point->get_impact_datum_index() == impact_datum_index)
            {
                real_vector3d transformed;
                impacts_vector_from_points(rectangle_center, contact_point->get_position(), &delta);
                matrix3x3_inverse_transform_vector(&single_matrix, &delta, &transformed);
                if (single_max_x <= transformed.i)
                    single_max_x = transformed.i;
                if (single_min_x > transformed.i)
                    single_min_x = transformed.i;
                if (single_max_y <= transformed.j)
                    single_max_y = transformed.j;
                if (single_min_y > transformed.j)
                    single_min_y = transformed.j;
                if (is_pair)
                {
                    matrix3x3_transform_vector(&pair_matrix, &delta, &transformed);
                    if (pair_max_x <= transformed.i)
                        pair_max_x = transformed.i;
                    if (pair_min_x > transformed.i)
                        pair_min_x = transformed.i;
                    if (pair_max_y <= transformed.j)
                        pair_max_y = transformed.j;
                    if (pair_min_y > transformed.j)
                        pair_min_y = transformed.j;
                }
            }
        }
        if (is_pair && !((single_max_y - single_min_y) * (single_max_x - single_min_x) > (pair_max_x - pair_min_x) * (pair_max_y -
            pair_min_y)))
        {
            *half_x_dimension = (pair_max_x - pair_min_x) * 0.5f;
            *half_y_dimension = (pair_max_y - pair_min_y) * 0.5f;
            *rectangle_forward = pair_matrix.forward;
            *rectangle_left = pair_matrix.left;
            float x_center = (pair_max_x + pair_min_x) * 0.5f;
            float y_center = (pair_max_y + pair_min_y) * 0.5f;
            point_from_line3d(rectangle_center, rectangle_forward, x_center, rectangle_center);
            point_from_line3d(rectangle_center, rectangle_left, y_center, rectangle_center);
        }
        else
        {
            *half_x_dimension = (single_max_x - single_min_x) * 0.5f;
            *half_y_dimension = (single_max_y - single_min_y) * 0.5f;
            *rectangle_forward = single_matrix.forward;
            *rectangle_left = single_matrix.left;
            float x_center = (single_max_x + single_min_x) * 0.5f;
            float y_center = (single_max_y + single_min_y) * 0.5f;
            point_from_line3d(rectangle_center, rectangle_forward, x_center, rectangle_center);
            point_from_line3d(rectangle_center, rectangle_left, y_center, rectangle_center);
        }
    }
    calculate_water_surface_vector(rectangle_center, &delta);
}

// @rdata 0x82022838
static const float k_impact_minimum_rolling_speed = 0.05f;
// @rdata 0x820397e8
static const float k_impact_vehicle_maximum_rolling_speed = 7.0f;
// @rdata 0x82022dc0
static const float k_impact_maximum_rolling_speed = 0.5f;
// @rdata 0x8202a1a4
static const float k_impact_maximum_sliding_speed = 4.0f;
// @rdata 0x82026480
static const float k_impact_vehicle_rolling_speed_ratio = 8.0f;
// @rdata 0x8202b210
static const float k_impact_rolling_speed_ratio = 1.25f;
static __forceinline float impacts_maximum_collision_magnitude(e_impact_sound_scale_type scale_type)
{
    float result;
    switch (scale_type) {
    case _impact_sound_scale_dynamic: result = k_impact_maximum_rolling_speed; break;
    case _impact_sound_scale_vehicle: result = k_impact_maximum_sliding_speed; break;
    case _impact_sound_scale_character: result = 2.0f; break;
    default: result = 2.0f; break;
    }
    return result;
}
static __forceinline float impacts_minimum_collision_magnitude(e_impact_sound_scale_type scale_type)
{
    float result;
    switch (scale_type) {
    case _impact_sound_scale_dynamic: result = k_impact_minimum_rolling_speed; break;
    case _impact_sound_scale_vehicle: result = 0.6f; break;
    case _impact_sound_scale_character: result = 0.3f; break;
    default: result = 0.3f; break;
    }
    return result;
}
static __forceinline float impacts_minimum_collision_time(e_impact_sound_scale_type scale_type)
{
    return 0.5f;
}
static __forceinline float impacts_minimum_collision_angular_speed(e_impact_sound_scale_type scale_type)
{
    return 1.5707964f;
}
// @address 0x113fd8 ?update_state@c_impact@@QAAXJJJ@Z
void c_impact::update_state(long impact_index, long rigid_body_a, long rigid_body_b)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(get_object_index_a(), -1));
    c_havok_component *havok_component_a = havok_component_get(object->havok_component);
    long object_index_b = get_object_index_b();
    e_impact_sound_scale_type scale_type = get_impact_sound_scale_type(contact_type, get_object_index_a(), object_index_b);
    float maximum_collision_magnitude = impacts_maximum_collision_magnitude(scale_type);
    float minimum_collision_magnitude = impacts_minimum_collision_magnitude(scale_type);
    float minimum_collision_time = impacts_minimum_collision_time(scale_type);
    float minimum_collision_angular_speed = impacts_minimum_collision_angular_speed(scale_type);
    float collision_time = effect_elapsed_time;
    real_vector3d angular_velocity = *havok_component_a->get_rigid_body_pre_simulation_angular_velocity(rigid_body_a);
    float collision_scale = 0.0f;
    if (object_index_b != -1) {
        c_havok_component *havok_component_b = havok_component_get(((object_datum *)object_get_and_verify_type(object_index_b,
            -1))->havok_component);
        subtract_vectors3d(&angular_velocity, havok_component_b->get_rigid_body_pre_simulation_angular_velocity(rigid_body_b),
            &angular_velocity);
    }
    float angular_speed_squared = impacts_magnitude_squared(&angular_velocity);
    activation_state = -1;
    if ((collision_time > minimum_collision_time * minimum_collision_time ||
         angular_speed_squared > minimum_collision_angular_speed * minimum_collision_angular_speed) &&
        vehicle_sound_volume_a > minimum_collision_magnitude) {
        activation_state = 0;
        collision_scale = (MIN(vehicle_sound_volume_a, maximum_collision_magnitude) - minimum_collision_magnitude) /
            (maximum_collision_magnitude - minimum_collision_magnitude);
    } else if (use_marker) {
        long rolling_count = 0;
        float rolling_magnitude = 0.0f;
        long sliding_count = 0;
        float sliding_magnitude = 0.0f;
        for (long index = 0; index < havok_component_a->get_impact_count(); index++) {
            long other_impact_index = impacts_component_impact_datum_index(havok_component_a, index);
            if (other_impact_index != impact_index) {
                c_impact *other = impacts_impact_datum_get(other_impact_index);
                if (!other->use_marker &&
                    get_global_material_index_a() == other->get_global_material_index_a() &&
                    get_global_material_index_b() == other->get_global_material_index_b() &&
                    other->contact_type != -1) {
                    switch (other->activation_state) {
                    case 2:
                        rolling_count++;
                        rolling_magnitude += other->volume_b;
                        break;
                    case 3:
                        sliding_count++;
                        sliding_magnitude += other->volume_a;
                        break;
                    }
                }
            }
        }
        if (sliding_count > 0 && sliding_count >= rolling_count) {
            activation_state = 3;
            volume_a = sliding_magnitude / (float)sliding_count;
        } else if (rolling_count > 0) {
            activation_state = 2;
            volume_b = rolling_magnitude / (float)rolling_count;
        } else {
            activation_state = 1;
        }
    } else if (type == 2 || type == 1 || type == 0) {
        float minimum_rolling_speed = k_impact_minimum_rolling_speed;
        float maximum_rolling_speed = contact_type != -1 ? k_impact_vehicle_maximum_rolling_speed : k_impact_maximum_rolling_speed;
        float minimum_sliding_speed = k_impact_minimum_rolling_speed;
        float maximum_sliding_speed = k_impact_maximum_sliding_speed;
        float rolling_speed_ratio = contact_type != -1 ? k_impact_vehicle_rolling_speed_ratio : k_impact_rolling_speed_ratio;
        real_vector3d linear_velocity;
        havok_component_a->get_rigid_body_linear_velocity(rigid_body_a, &linear_velocity);
        if (object_index_b != -1) {
            c_havok_component *havok_component_b = havok_component_get(((object_datum *)object_get_and_verify_type(object_index_b,
                -1))->havok_component);
            real_vector3d other_linear_velocity;
            havok_component_b->get_rigid_body_linear_velocity(rigid_body_b, &other_linear_velocity);
            subtract_vectors3d(&linear_velocity, &other_linear_velocity, &linear_velocity);
        }
        float speed = magnitude3d(&linear_velocity);
        if (speed > minimum_rolling_speed && !effect_pending &&
            (vehicle_sound_volume_b < 0.001f || speed / vehicle_sound_volume_b > rolling_speed_ratio)) {
            activation_state = 2;
            volume_b = (MIN(speed, maximum_rolling_speed) - minimum_rolling_speed) / (maximum_rolling_speed - minimum_rolling_speed);
        } else if (vehicle_sound_volume_b > minimum_sliding_speed) {
            activation_state = 3;
            volume_a = (MIN(vehicle_sound_volume_b, maximum_sliding_speed) - minimum_sliding_speed) / (maximum_sliding_speed -
                minimum_sliding_speed);
        } else {
            activation_state = 1;
        }
    } else if (type == 3) {
        activation_state = volume_a > volume_b ? 3 : 2;
    }
    if (activation_state == 0) {
        if (state == 0)
            reference_collision_score = 0.0f;
        reference_collision_score = MAX(reference_collision_score, collision_scale);
        if (state == 1)
            queue_collision(reference_collision_score);
        state = MIN(state + 1, 1);
    } else {
        if (state != 0)
            queue_collision(reference_collision_score);
        state = 0;
    }
    has_sound_b = (activation_state >= 2 && activation_state <= 3) ? MIN(has_sound_b + 1, 126) : 0;
}

static __forceinline bool impacts_havok_component_flag_test(const c_havok_component *component, long bit)
{
    return ((component->m_flags >> bit) & 1) != 0;
}
static __forceinline bool impacts_contact_point_invalid(const c_havok_component *component, const c_havok_contact_point *contact_point)
{
    long rigid_body_index_a = contact_point->get_rigid_body_index_a();
    bool invalid = !VALID_INDEX(rigid_body_index_a, component->get_rigid_body_count());
    long object_index_b = contact_point->get_contact_point_object_index();
    if (object_index_b != -1)
    {
        object_datum *object_b = ((object_datum *)object_get_and_verify_type(object_index_b, -1));
        if (object_b->havok_component != -1)
            invalid |= !VALID_INDEX(contact_point->get_rigid_body_index_b(),
                havok_component_get(object_b->havok_component)->get_rigid_body_count());
        else
            invalid = true;
    }
    return invalid;
}
// @address 0x1bc9c0 impacts_post_process_havok_entity
extern "C" void impacts_post_process_havok_entity(const phys_world_object *entity)
{
    long component_index = havok_entity_get_havok_component_index(entity);
    if (component_index == -1)
        return;
    c_havok_component *component = havok_component_get(component_index);
    bool deleted = false;
    bool update_constraint_impacts = (((object_datum *)object_get_and_verify_type(component->m_object_index, -1))->physics_flags & (1UL
        << 9)) != 0;
    for (long rigid_body_index = 0; rigid_body_index < component->get_rigid_body_count(); rigid_body_index++)
    {
        if (component->m_rigid_bodies[rigid_body_index].m_flags & (1 << 3))
        {
            component->m_rigid_bodies[rigid_body_index].m_flags &= ~(1 << 3);
            long object_index = component->m_object_index;
            if (!simulation_query_object_is_predicted(object_index))
            {
                object_delete(object_index);
                deleted = true;
            }
            break;
        }
    }
    if (!deleted && impacts_havok_component_flag_test(component, 3))
    {
        object_set_requires_motion(component->m_object_index);
        if (update_constraint_impacts && havok_component_has_active_entity(component))
            impacts_create_constraint_impacts(component->m_object_index);
    }
    if ((component->m_flags & 1))
    {
        for (c_havok_contact_point_iterator iterator(component->m_object_index, true); iterator.valid(); iterator.next())
        {
            c_havok_contact_point *contact_point = impacts_iterator_contact_point(&iterator);
            long life_cycle_flags = contact_point->m_life_cycle_flags;
            bool contact_destroyed = TEST_BIT(contact_point->m_life_cycle_flags, 1);
            contact_point->m_life_cycle_flags = 0;
            bool contact_created = TEST_BIT(life_cycle_flags, 0);
            if (contact_created)
            {
                c_global_material_type material_a = contact_point->calculate_global_material_type_a();
                c_global_material_type material_b = contact_point->calculate_global_material_type_b();
                if (!g_impacts_globals->disconnected_from_physics)
                {
                    c_impact::s_contact_description contact_description;
                    if (!impacts_contact_point_invalid(component, contact_point))
                    {
                        c_impact::initialize_contact_description(&contact_description, c_impact::type1, true,
                            component->m_object_index, contact_point->get_rigid_body_index_a(), material_a,
                            contact_point->get_contact_point_object_index(), contact_point->get_rigid_body_index_b(), material_b,
                            contact_point->get_position(), contact_point->get_normal(), -1, 0);
                        c_havok_component *component_b = contact_point->get_contact_havok_component_index() != -1
                            ? havok_component_get(contact_point->get_contact_havok_component_index())
                            : 0;
                        bool fixed = true;
                        if (component_b)
                            fixed = component_b->m_rigid_bodies[contact_point->get_rigid_body_index_b()].m_rigid_body->body_is_fixed();
                        long impact_datum_index = component->find_impact_which_includes_contact(&contact_description, true);
                        if (impact_datum_index == -1 &&
                            (component_b == 0 || component_b->find_impact_which_includes_contact(&contact_description, false) == -1) &&
                            (contact_description.object_b != -1 || fixed) &&
                            c_impact::passes_the_character_test(contact_description.object_a, contact_description.object_b,
                                &contact_description.position, &contact_description.normal))
                        {
                            float score = c_impact::calculate_score(&contact_description.position, c_impact::state1);
                            if (component->make_space_for_new_impact(score, contact_description.node_index_a))
                            {
                                impact_datum_index = impact_new_from_contact(&contact_description, c_impact::state1);
                                component->add_impact(impact_datum_index);
                            }
                        }
                        if (impact_datum_index != -1)
                        {
                            c_impact *impact = (c_impact *)(TLS_IMPACT_DATA->data + (unsigned short)impact_datum_index * sizeof(c_impact));
                            contact_point->set_impact_datum_index(impact_datum_index);
                            impact->add_reference(impact_datum_index);
                        }
                    }
                }
            }
            if (contact_destroyed)
            {
                component->m_rigid_bodies[iterator.rigid_body_index].remove_contact_point(iterator.contact_point_index);
                iterator.contact_point_index--;
            }
        }
    }
}

// @address 0x1134b0 ?update@c_impact@@QAAXJ@Z
void c_impact::update(long impact_index)
{
    c_havok_component *havok_component_a = havok_component_get(((object_datum *)object_get_and_verify_type(get_object_index_a(),
        -1))->havok_component);
    long rigid_body_a;
    long rigid_body_b;
    calculate_rigid_body_indexes(impact_index, &rigid_body_a, &rigid_body_b);
    if (contact_type != -1) {
        update_state(impact_index, rigid_body_a, rigid_body_b);
        update_effects(impact_index, rigid_body_a, rigid_body_b);
        return;
    }

    real_matrix4x3 transform;
    havok_component_a->get_rigid_body_transform(rigid_body_a, &transform);
    matrix4x3_transform_point(&transform, &space_position_a, &origin);
    if (is_between_pair_of_dynamic_rigid_bodies()) {
        c_havok_component *havok_component_b = havok_component_get(((object_datum *)object_get_and_verify_type(get_object_index_b(),
            -1))->havok_component);
        real_point3d position_b;
        real_vector3d delta;
        havok_component_b->get_rigid_body_transform(rigid_body_b, &transform);
        matrix4x3_transform_point(&transform, &space_position_b, &position_b);
        impacts_vector_from_points(&origin, &position_b, &delta);
        point_from_line3d(&origin, &delta, 0.5f, &origin);
    }

    vehicle_sound_volume_b = 0.0f;
    vehicle_sound_volume_a = 0.0f;
    if (rigid_body_index == 0xffff) {
        bool impulse_calculated = false;
        for (c_havok_contact_point_iterator iterator(havok_component_a->m_object_index, false); iterator.valid(); iterator.next()) {
            c_havok_contact_point *contact_point = impacts_iterator_contact_point(&iterator);
            if (contact_point->get_impact_datum_index() != impact_index)
                continue;
            float distance_squared;
            if (included_in_impact_space(&origin, &object_position, contact_point->get_position(), contact_point->get_normal(),
                &distance_squared)) {
                if (impulse_calculated)
                    continue;
                real_vector3d velocity;
                havok_component_get_point_velocity(havok_component_a, contact_point->get_rigid_body_index_a(), &velocity,
                    contact_point->get_position());
                long other_rigid_body_index = contact_point->get_rigid_body_index_b();
                const real_vector3d *contact_normal = contact_point->get_normal();
                if (other_rigid_body_index != -1 && contact_point->get_contact_point_object_index() != -1) {
                    real_vector3d other_velocity;
                    c_havok_component *other_component = havok_component_get(((object_datum
                        *)object_get_and_verify_type(contact_point->get_contact_point_object_index(), -1))->havok_component);
                    havok_component_get_point_velocity(other_component, other_rigid_body_index, &other_velocity,
                        contact_point->get_position());
                    subtract_vectors3d(&velocity, &other_velocity, &velocity);
                }
                if (dot_product3d(&velocity, contact_normal) < 0.0f) {
                    float impulse;
                    real_point3d position;
                    real_vector3d normal;
                    havok_component_a->calculate_impulse(contact_point->get_rigid_body_index_a(),
                        contact_point->get_contact_point_object_index(), contact_point->get_rigid_body_index_b(), &impulse, &position,
                        &normal);
                    if (impulse > vehicle_sound_volume_a) {
                        vehicle_sound_volume_a = impulse;
                        object_position = normal;
                        origin = position;
                        update_rigid_body_space_positions(&origin, rigid_body_a, rigid_body_b);
                    }
                    impulse_calculated = true;
                }
            } else {
                remove_reference(impact_index, false);
                contact_point->set_impact_datum_index(-1);
            }
        }
    }

    // match: 0x1134b0: named predicates preserve the late dot-product evaluation order.
    bool has_impact_references = get_reference_count() != 0;
    if (has_impact_references) {
        if (rigid_body_index == 0xffff) {
            real_vector3d velocity;
            havok_component_a->get_rigid_body_linear_velocity_at_point(rigid_body_a, &velocity, &origin);
            bool pair_of_dynamic_bodies = is_between_pair_of_dynamic_rigid_bodies();
            if (pair_of_dynamic_bodies) {
                c_havok_component *havok_component_b = havok_component_get(((object_datum
                    *)object_get_and_verify_type(get_object_index_b(), -1))->havok_component);
                real_vector3d other_velocity;
                havok_component_b->get_rigid_body_linear_velocity_at_point(rigid_body_b, &other_velocity, &origin);
                subtract_vectors3d(&velocity, &other_velocity, &velocity);
            }
            real_vector3d projection;
            scale_vector3d(&object_position, dot_product3d(&velocity, &object_position), &projection);
            subtract_vectors3d(&velocity, &projection, &velocity_direction);
            vehicle_sound_volume_b = normalize3d(&velocity_direction);
            if (vehicle_sound_volume_b == 0.0f)
                velocity_direction = global_identity4x3->up;
        }

        real_vector3d linear_velocity = *havok_component_a->get_rigid_body_pre_simulation_linear_velocity(rigid_body_a);
        if (is_between_pair_of_dynamic_rigid_bodies()) {
            c_havok_component *havok_component_b = havok_component_get(((object_datum *)object_get_and_verify_type(get_object_index_b(),
                -1))->havok_component);
            subtract_vectors3d(&linear_velocity, havok_component_b->get_rigid_body_pre_simulation_linear_velocity(rigid_body_b),
                &linear_velocity);
        }
        float speed_squared = impacts_magnitude_squared(&linear_velocity);
        if (has_sound_a >= 3) {
            effect_elapsed_time = 0.0f;
            has_sound_a = 0;
        }
        if (speed_squared > effect_elapsed_time) {
            effect_elapsed_time = speed_squared;
            has_sound_a = 0;
        }
        has_sound_a++;
        update_state(impact_index, rigid_body_a, rigid_body_b);
        update_effects(impact_index, rigid_body_a, rigid_body_b);
    }
}

#if !defined(_WIN64) && !defined(__LP64__)
typedef char impact_reference_score_offset[(offsetof(c_impact, reference_collision_score) == 0x1c) ? 1 : -1];
typedef char impact_object_a_offset[(offsetof(c_impact, object_index_a) == 0x2c) ? 1 : -1];
typedef char impact_queued_tick_offset[(offsetof(c_impact, queued_tick) == 0xa8) ? 1 : -1];
typedef char impact_contact_object_b_offset[(offsetof(c_impact::s_contact_description, object_b) == 0x10) ? 1 : -1];
typedef char impact_contact_position_offset[(offsetof(c_impact::s_contact_description, position) == 0x1c) ? 1 : -1];
typedef char impact_iterator_object_offset[(offsetof(c_havok_contact_point_iterator, object_index) == 4) ? 1 : -1];
typedef char impact_effect_object_offset[(offsetof(s_effect_creation_data, object_index) == 0x1c) ? 1 : -1];
typedef char impact_effect_scale_offset[(offsetof(s_effect_creation_data, scale_a) == 0x4c) ? 1 : -1];
#endif
