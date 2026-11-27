// blamlibxbox/particle_system.obj: from the records 0x142920 0x142f48 0x145528 0x149b38 0x149d40 0x149ed8 0x149ff0 0x14a108 0x14a218 0x14b438 0x152750 0x180980 0x2b3d58 0x2b3df8 0x2b3e18 0x2b3e30 0x2b3f08 0x2b3fc0 0x2b4060 0x2b4148 0x2b41f0 0x2b4290 0x2b63d0 0x300578 0x45f2e0 0x45f4d8 0x45fe40 0x45fec8 0x70a1d0 0x70a1f0 0x70c990 0x70c9c0
#include "effects/particle_array_globals.h"
#include "game/player_control_globals.h"
#include "render/render_objects.h"
#include "cseries/cseries_types.h"
#include "sky_atm/sky_atm_parameters_types.h"
#include "effects/particle_system_globals.h"
#include "math/real_constant_pool_access.h"
#include "memory/tls_globals.h"
#include "gpu_particle/particle_update_shader.h"
#include "gpu_particle/particle_block_queue_types.h"
#include "tag_files/tag_groups.h"
#include "math/real_math_types.h"
#include "tag_files/tag_index.h"
#include "memory/data_types.h"
#include "effects/particle.h"
#include "game/game_globals_storage.h"
#include "cseries/cseries.h"
#include "cseries/bit_operations.h"
#include <stdlib.h>
#include <ppcintrinsics.h>
#include "cache/restricted_memory_regions.h"
#include "game/players.h"
#include "scenario/scenario.h"
#include "hs/object_lists.h"
#include "rasterizer/rasterizer_vertex_definitions.h"
#include "effects/effects.h"
#include "effects/particle_location.h"
#include "effects/contrails.h"
#include "effects/particle_system.h"
#include "tag_files/tag_types.h"
#include "rasterizer/rasterizer_vertex_types.h"
#include "effects/particle_emitter.h"
#include "effects/particle_emitter_allocator.h"
#include "gpu_particle/common_gpu.h"
#include "gpu_particle/gpu_particle_globals.h"
#include "memory/data.h"
#include "effects/particle_location_limits.h"
#include "render/render_lightmap_shadows_globals.h"
#include "render/render.h"
#include "game/particle_location_data_array_types.h"
#include "effects/event_status_types.h"
#include "effects/particle_effect_data_array_types.h"
#include "effects/particle_definitions.h"
#include "effects/particle_system_definitions.h"
#include "effects/effect_definitions.h"
#include "game/player_control_types.h"
#include "camera/saved_film_director_storage.h"
#include "effects/particle_location_runtime_types.h"
#include "game/player_mapping_types.h"
#include "camera/observer_globals_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "effects/effect_datum.h"
#include "effects/particle_movement_internals.h"
#include "gpu_particle/particle_emitter_gpu.h"
#include "effects/particle_states.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_registration.h"
#include "multithreading/thread_tls.h"
#include "cache/restricted_memory_regions_inline.h"
#include "gpu_particle/particle_block_bounds_types.h"
#include "gpu_particle/contrail_target_entry.h"
#include "gpu_particle/contrail_target_bounds_types.h"
#include "math/random_math_range.h"
#include "scenario/cluster_reference_helpers.h"

// @address 0x11df754 g_particle_system_data_array
// @address 0x11df78c g_particle_location_data_array
// @address 0x11df7ac g_particle_emitter_data
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x11df898 g_particle_gpu_storage_address
// @address 0x11df808 g_particle_data
// @address 0x142920 ?create@c_particle_system@@SAJPBVc_particle_system_definition@@JVc_tag_index@@KGGK@Z
// @address 0x145528 ?get_particle_definition@c_particle_system@@QBAPBVc_particle_definition@@XZ
// @address 0x149b38 ?frame_advance_all@c_particle_system@@SAXM@Z
// @address 0x14b438 ?release_particles@c_particle_system@@AAAXXZ
// @address 0x152750 particle_system_compute_visibility
// @address 0x2b3d58 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_object_list_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x2b3df8 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_object_list_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x2b3e18 ?__tls_set_g_particle_system_data_array_allocator@@YAXPAX@Z
// @address 0x2b3e30 particle_system_create_data_arrays
// @address 0x2b3f08 ?initialize_for_map@c_particle_system@@SAXXZ
// @address 0x2b3fc0 ?dispose_from_map@c_particle_system@@SAXXZ
// @address 0x2b4060 ?initialize_for_new_structure_bsp@c_particle_system@@SAXXZ
// @address 0x2b4148 ?remove_further_particle_locations@c_particle_system@@QAAXJ@Z
// @address 0x2b41f0 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_particle_system_data_array_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x2b4290 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_particle_system_data_array_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x2b63d0 ?compute_distance_fade@particle_system_lod@@YAMPBVc_particle_system@@M@Z
// @address 0x300578 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?particle_emitter_set_array_address@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x45f2e0 particle_emitter_gpu_set_storage_address
// @address 0x45f4d8 particle_emitter_gpu_initialize_storage
// @address 0x8c43a8 g_effect_budget_limits
// @address 0x8368d0 g_player_render_tuning
// @address 0x11df7a4 g_particle_gpu_block_data
// @address 0x11df8cc ?g_observer_globals@@3PAUs_player_globals@@A

typedef char particle_system_player_zoom_offset[
    (offsetof(s_player_control_state, action) + offsetof(control, zoom) == 0x2a) ? 1 : -1];
typedef char particle_system_player_states_offset[
    (offsetof(s_player_control_globals, states) == 0x3c0) ? 1 : -1];


// @address 0x7be8a8 g_particle_render_vertex_shader
// @address 0x7be900 g_particle_render_model_vertex_shader
__declspec(thread) s_data_array *g_particle_system_data_array;
__declspec(thread) void *g_particle_gpu_storage_address;
__declspec(thread) s_data_array *g_particle_data;
__declspec(thread) s_data_array *g_particle_gpu_block_data;
#if !defined(_XBOX) || defined(__clang__)
// Port-only definitions: shipped bytes at 0x828368d0 are 0x38 zeros and at 0x82042918 "particle_emitter"
// (console build binds the tags absolute).
extern "C" {
s_player_tuning g_player_render_tuning = {};
char k_particle_emitter_array_name[] = "particle_emitter";
}
#endif

__forceinline void c_particle_emitter_gpu::initialize_for_map() {
    initialize_shaders();
    g_particle_block_queue_system.current->count = 0;
    g_particle_block_queue_system.next->count = 0;
    data_make_valid(g_particle_gpu_block_data);
    data_make_valid((s_data_array *)g_particle_emitter_gpu_data);
}
__forceinline void c_particle_emitter_gpu::dispose_shaders() {
    g_particle_spawn_shaders.dispose();
    g_particle_update_shaders.dispose();
    g_particle_render_vertex_shader.dispose();
    g_particle_render_model_vertex_shader.dispose();
}
__forceinline void c_particle_emitter_gpu::dispose_from_map() {
    ((s_data_array *)g_particle_emitter_gpu_data)->valid = false;
    g_particle_gpu_block_data->valid = false;
    g_particle_block_queue_system.current->count = 0;
    g_particle_block_queue_system.next->count = 0;
    dispose_shaders();
}
__forceinline void c_particle_location::initialize_for_map()
{
    data_make_valid(g_particle_location_data_array);
}
__forceinline void c_particle_location::dispose_from_map()
{
    g_particle_location_data_array->valid = false;
}
__forceinline void c_particle_emitter::initialize_for_map() { data_make_valid(g_particle_emitter_data); }
__forceinline void c_particle_emitter::dispose_from_map() { g_particle_emitter_data->valid = false; }
__forceinline void c_particle::initialize_for_map()
{
    data_make_valid((s_data_array *)g_particle_data);
}
__forceinline void c_particle::dispose_from_map()
{
    g_particle_data->valid = false;
}
// @address 0x119a434 g_particle_system_type_created
extern "C" { bool g_particle_system_type_created[4]; }
extern "C" {
// @address 0x70a1d0 ??__Eg_particle_gpu_data_allocator@@YAXXZ
// @address 0x70c990 ??__Fg_particle_gpu_data_allocator@@YAXXZ
// @address 0x7be770 g_particle_gpu_data_allocator
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_impact_array_data_allocator> g_particle_gpu_data_allocator;
// @address 0x70a1f0 ??__Eg_particle_gpu_storage_allocator@@YAXXZ
// @address 0x70c9c0 ??__Fg_particle_gpu_storage_allocator@@YAXXZ
// @address 0x7be788 g_particle_gpu_storage_allocator
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_emitter_gpu_set_storage_address> g_particle_gpu_storage_allocator;
}
long c_particle_system::create(const c_particle_system_definition *definition, long effect_index, c_tag_index tag_index,
    unsigned long sub_index, unsigned short def_index, unsigned short location_index, unsigned long seed)
{
    long result = -1;
    unsigned char type = definition->get_type();
    if (definition->get_allowed_to_create() && definition->get_valid()) {
        if ((unsigned long)(g_particle_system_data_array->actual_count + 1) <=
            g_effect_budget_limits->table->blocks[_particle_system_budget].limits[type].maximum_count) {
            result = datum_new((s_data_array *)g_particle_system_data_array);
            c_particle_system *system = datum_get<c_particle_system>(g_particle_system_data_array, result);
            system->set_flag(2, false);
            system->set_flag(0x40, true);
            system->set_flag(1, true);
            system->set_flag(0x20, true);
            system->set_flag(8, true);
            system->set_flag(0x10, true);
            if (definition->get_bit12() || definition->get_cinematics())
                system->set_flag(4, true);
            else
                system->set_flag(4, false);
            system->m_random_seed = seed;
            for (long i = 0; i < 4; i++)
                system->m_random_values[i] = _real_random_range(&system->m_random_seed, 0.0f, 1.0f);
            system->m_effect_index = effect_index;
            system->m_tag_index = tag_index;
            system->m_type = definition->get_type();
            system->m_next_system = -1;
            system->m_previous_system = -1;
            system->m_first_location = -1;
            system->m_last_location = -1;
            system->m_sub_index = (unsigned short)sub_index;
            system->m_def_index = (unsigned char)def_index;
            system->m_location_index = location_index;
            system->m_count = 0;
            g_particle_system_type_created[type] = true;
        } else {
            g_particle_system_type_created[type] = false;
        }
    }
    return result;
}
const c_particle_definition *c_particle_system::get_particle_definition() const {
    const c_particle_system_definition *definition = get_particle_system_definition();
    return definition->get_particle_definition();
}
// @address 0x744484 g_particle_inv_render_delta
float g_particle_inv_render_delta = 1.0f;


static __forceinline s_particle_location_data_array *particle_system_location_data()
{
    return (s_particle_location_data_array *)g_particle_location_data_array;
}

static __forceinline bool particle_system_test_long_flag(unsigned long flags, long bit)
{
    return (_rotl(flags, 32 - bit) & 1) != 0;
}
void c_particle_system::frame_advance_all(float dt)
{
    float render_delta = g_player_render_tuning.delta;
    if (!(render_delta >= 0.001f))
        render_delta = 0.001f;
    g_particle_inv_render_delta = 1.0f / render_delta;
    c_data_iterator<c_particle_system> iterator;
    iterator.begin((s_data_array *)g_particle_system_data_array);
    while (iterator.next()) {
        c_particle_system *system = iterator.get_datum();
        bool keep = true;
        const c_particle_definition *particle = system->get_particle_system_definition()->get_particle_definition();
        system->query_effect();
        if (!(dt <= 0.0f))
            system->frame_advance(dt);
        if (bit_test(system->m_flags, 5)) {
            long count = particle_system_pulse_emitters(system, dt);
            if (!(dt <= 0.0f))
                system->m_count = (unsigned short)count;
        }
        if (!(dt <= 0.0f) && system->m_count == 0 && !bit_test(system->m_flags, 3)) {
            long index = iterator.get_index();
            s_data_array *data_array = (s_data_array *)g_particle_system_data_array;
            c_particle_system *datum = datum_get<c_particle_system>(data_array, index);
            datum->release_particles();
            ::datum_delete((s_data_array *)g_particle_system_data_array, index);
            keep = false;
        }
        if (keep) {
            if ((short)system->m_count > 0 && particle_system_test_long_flag(particle->flags2, 12))
                c_render_globals::set_depth_fade_active(true);
            if ((short)system->m_count > 0 && particle_system_test_long_flag(particle->flags2, 13))
                c_render_globals::set_distortion_active(true);
        }
    }
}
static __forceinline long particle_system_first_active_user()
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
// @address 0x14a218 particle_location_compute_fade
extern "C" float particle_location_compute_fade(const s_particle_location_datum *location, long user_index)
{
    c_particle_system *system = datum_get<c_particle_system>(g_particle_system_data_array, location->system_index);
    if (bit_test(system->m_flags, 8)) {
        const c_atmosphere_setting *render_object =
            (const c_atmosphere_setting *)render_objects_get_cluster_element(system->m_location.cluster_reference, 0);
        if (render_object != 0)
            return render_object->cluster_weight;
        return 0.0f;
    }
    if (location->state & 1)
        return 1.0f;
    if (user_index != -1) {
        const real_point3d *position = &location->matrix.position;
        float dy = location->matrix.position.y - g_observer_globals->blocks[user_index].camera_result.position.y;
        float dz = position->z - g_observer_globals->blocks[user_index].camera_result.position.z;
        float dx = position->x - g_observer_globals->blocks[user_index].camera_result.position.x;
        return particle_system_lod::compute_distance_fade(system,
            __fsqrts(dx * dx + (dz * dz + dy * dy)) * g_observer_globals->blocks[user_index].camera_result.field_of_view_scale);
    }
    float result = 0.0f;
    long user = particle_system_first_active_user();
    if (user != -1) {
        float x = location->matrix.position.x;
        float y = location->matrix.position.y;
        float z = location->matrix.position.z;
        const s_player_globals *observers = g_observer_globals;
        do {
            const real_point3d *camera = &observers->blocks[user].camera_result.position;
            float dx = x - camera->x;
            float dz = z - camera->z;
            float dy = y - camera->y;
            float dx_squared = dx * dx;
            float value = particle_system_lod::compute_distance_fade(system,
                __fsqrts(dy * dy + (dz * dz + dx_squared)) * observers->blocks[user].camera_result.field_of_view_scale);
            if (!(result > value))
                result = value;
            user = players_next_active_user(user);
        } while (user != -1);
    }
    return result;
}
// @address 0x14a108 particle_location_update_emitters
extern "C" int particle_location_update_emitters(s_particle_location_datum *location, c_particle_state_list *state_list, float delta)
{
    long count = 0;
    c_particle_system *system = datum_get<c_particle_system>(g_particle_system_data_array, location->system_index);
    location->scale = particle_location_compute_fade(location, -1);
    if ((short)system->m_count > 0 && !(delta <= 0.0f)) {
        long emitter_index = location->first_emitter_index;
        if (emitter_index != -1) {
            do {
                c_particle_emitter *emitter = datum_get<c_particle_emitter>(g_particle_emitter_data, emitter_index);
                count += particle_emitter_update_particles(emitter, state_list, delta);
                emitter_index = emitter->m_next_emitter;
            } while (emitter_index != -1);
        }
    }
    bool cinematic = false;
    if (cinematic_globals != 0)
        cinematic = cinematic_globals->in_progress;
    if (cinematic != 0)
        location->state |= 4;
    else
        location->state &= ~4;
    location->state &= ~8;
    return count;
}
// @address 0x8c43ac g_particle_default_used_states

static __forceinline bool particle_system_effect_should_frame_advance(long effect_index, short particle_count)
{
    const effect_datum *effect = datum_get<effect_datum>(tls_particle_effect_data_array(), effect_index);
    return !particle_system_test_long_flag(effect->flags.get_unsafe(), _effect_stopping_bit) || particle_count > 0;
}
static __forceinline bool particle_system_effect_should_render(long effect_index, bool should_frame_advance)
{
    const effect_datum *effect = datum_get<effect_datum>(tls_particle_effect_data_array(), effect_index);
    return !particle_system_test_long_flag(effect->flags.get_unsafe(), _effect_culled_bit) && should_frame_advance;
}
// @address 0x149d40 ?query_effect@c_particle_system@@QAAXXZ
void c_particle_system::query_effect()
{
    effect_update_particle_query_record(this);
    switch (effect_get_event_status(m_effect_index, m_def_index, m_location_index, &m_age)) {
    case _event_status_invalid:
        set_flag(0x8, false);
        set_flag(0x10, false);
        break;
    case _event_status_finished:
        set_flag(0x10, false);
        break;
    case _event_status_active:
        set_flag(0x10, true);
        break;
    default:
        // match: 0x149d40: unreachable hint omits dead branch check
        __assume(0);
    }
    if (m_effect_index == -1) {
        if (!bit_test(m_flags, 5) || bit_test(m_flags, 1) && bit_test(m_flags, 2)) {
            release_particles();
            m_count = 0;
        }
    } else {
        short particle_count = (short)m_count;
        set_flag(0x20, particle_system_effect_should_frame_advance(m_effect_index, particle_count));
        set_flag(0x40, particle_system_effect_should_render(m_effect_index, bit_test(m_flags, 5)));
    }
}
// @address 0x149ed8 ?frame_advance@c_particle_system@@QAAJM@Z
long c_particle_system::frame_advance(float delta)
{
    c_particle_state_list list;
    list.set_particle_system(this);
    long location_index = m_first_location;
    long particle_count = 0;
    if (location_index != -1) {
        do {
            s_particle_location_datum *location = datum_get<s_particle_location_datum>(g_particle_location_data_array, location_index);
            list.update_particle_location((const c_particle_location *)location);
            int updated_count = particle_location_update(location, delta, &list);
            particle_count += updated_count;
            m_count += updated_count;
            location_index = location->next_location_index;
        } while (location_index != -1);
    }
    return particle_count;
}
// @address 0x149ff0 particle_system_pulse_emitters
extern "C" long particle_system_pulse_emitters(c_particle_system *system, float delta)
{
    c_particle_state_list list;
    list.set_particle_system(system);
    long count = 0;
    long location_index = system->m_first_location;
    if (location_index != -1) {
        do {
            s_particle_location_datum *location = datum_get<s_particle_location_datum>(g_particle_location_data_array, location_index);
            list.update_particle_location((const c_particle_location *)location);
            count += particle_location_update_emitters(location, &list, delta);
            location_index = location->next_location_index;
        } while (location_index != -1);
    }
    system->m_flags |= 2;
    return count;
}
static __forceinline s_particle_location_datum *particle_system_location_get(long location_index)
{
    return (s_particle_location_datum *)particle_system_location_data()->data + (unsigned short)location_index;
}
__forceinline long s_particle_location_datum::get_index() const
{
    return (this - (s_particle_location_datum *)particle_system_location_data()->data) | (identifier << 16);
}
__forceinline void s_particle_location_datum::add_to_list(long *first, long *last)
{
        long index = get_index();
        next_location_index = -1;
        if ((*first) == -1)
            (*first) = index;
        if ((*last) != -1)
            particle_system_location_get((*last))->next_location_index = index;
        (*last) = index;
}
// @address 0x142f48 ?add_particle_location@c_particle_system@@QAAJXZ
long c_particle_system::add_particle_location()
{
    long location_index = particle_location_new(
        m_type, (this - ((c_particle_system *)g_particle_system_data_array->data)) | (m_identifier << 16),
            (long)(_real_random_range(&m_random_seed, 0.0f, 1.0f) * 65535.0f));
    if (location_index != -1) {
        s_particle_location_datum *location = particle_system_location_get(location_index);
        location->add_to_list(&m_first_location, &m_last_location);
    }
    return location_index;
}
void c_particle_system::release_particles()
{
    long location_index = m_first_location;
    m_first_location = -1;
    m_last_location = -1;
    if (location_index != -1) {
        long next_index;
        do {
            next_index = datum_get<s_particle_location_datum>(g_particle_location_data_array, location_index)->next_location_index;
            particle_location_delete(location_index);
            location_index = next_index;
        } while (next_index != -1);
    }
}
extern "C" long particle_system_compute_visibility(const c_particle_system *system, long player_index, bool *out_first_person)
{
    const c_particle_system_definition *definition = system->get_particle_system_definition();
    const c_particle_definition *particle = definition->get_particle_definition();

    bool visible = 1;
    unsigned long flags2 = particle->flags2;
    long result = visible;
    bool in_player_mask = ((short)system->m_player_mask & (visible << player_index)) != 0;

    if (_rotl(flags2, 19) & 1)
        result = 2;
    else if (_rotl(flags2, 16) & 1)
        result = 0;
    else if (in_player_mask)
        result = 3;

    unsigned short appearance = definition->appearance;
    *out_first_person = in_player_mask;
    if (appearance >= 1) {
        if (appearance != 1) {
            if (appearance < 3 && in_player_mask)
                visible = 0;
        } else if (!in_player_mask) {
            visible = 0;
        }
    } else {
        *out_first_person = 0;
    }

    if (tls_game_globals()->options.game_mode == 3)
        in_player_mask = 0;

    bool visible_final = visible;
    if (in_player_mask && visible) {
        short zoom_level = -1;
        if (player_index != -1)
            zoom_level = player_control_globals->states[player_index].action.zoom;
        if (zoom_level != -1)
            visible_final = definition->get_bit8() & 1;
    }

    if (!visible_final)
        return 4;

    return result;
}
static __forceinline bool particle_system_cinematic_active()
{
    bool cinematic = false;
    if (cinematic_globals != 0)
        cinematic = cinematic_globals->in_progress;
    return cinematic;
}
// @address 0x180980 particle_system_render
extern "C" void particle_system_render(c_particle_system *system, long user_index, bool first_person, long pass)
{
    const c_particle_system_definition *definition = system->get_particle_system_definition();
    bool should_render = bit_test(system->m_flags, 6) && (short)system->m_count > 0 &&
        !definition->get_bit7() && (system->m_location.cluster_reference.bsp_index != -1 || particle_system_cinematic_active());
    if (should_render) {
        if (definition->get_bit13())
            effect_set_camera_info((const s_effect_camera_source *)definition);
        long location_index = system->m_first_location;
        if (location_index != -1) {
            do {
                s_particle_location_datum *location = particle_system_location_get(location_index);
                if (first_person == (location->state & 1) && particle_location_compute_fade(location, user_index) > 0.0f) {
                    long emitter_index = location->first_emitter_index;
                    while (emitter_index != -1) {
                        c_particle_emitter *emitter = datum_get<c_particle_emitter>(g_particle_emitter_data, emitter_index);
                        particle_emitter_render(emitter, pass);
                        if (bit_test(emitter->m_flags, 0))
                            location->state |= 4;
                        if (bit_test(emitter->m_flags, 1))
                            location->state |= 8;
                        emitter_index = emitter->m_next_emitter;
                    }
                }
                location_index = location->next_location_index;
            } while (location_index != -1);
        }
    }
}
// @address 0x42918 k_particle_emitter_array_name
namespace {
static __forceinline long particle_system_particle_restricted_region_add_member(const char *name,
    const char *type, unsigned int size, long alignment_bits)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[2], name, type, size,
        alignment_bits, __tls_set_g_object_list_data_allocator, 0, 0);
    void *address = restricted_region_get_member_address(2, member_index);
    data_set_new_base_address((s_data_array **)&g_particle_data, (s_data_array *)address);
    return member_index;
}
}
template<> void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_list_data_allocator>::allocate(unsigned int size,
    const char *name)
{
    m_member_index = particle_system_particle_restricted_region_add_member(name, "", size, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_list_data_allocator>::deallocate(void *address)
{
    g_particle_data = 0;
    restricted_region_free_member(2, m_member_index);
}
void __tls_set_g_particle_system_data_array_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_particle_system_data_array, (s_data_array *)object);
}
extern "C" void particle_system_create_data_arrays(void)
{
    g_particle_system_data_array = (s_data_array *)data_new("particle_system", 0x140, 0x58, 0,
        (c_allocation_base *)&g_particle_system_allocator);
    g_particle_data = data_new("particles", 0x200, 0x7c, 0, (c_allocation_base *)&g_particle_allocator);
    g_particle_emitter_data = (s_data_array *)data_new(k_particle_emitter_array_name, 0x240, 0x90, 0,
        (c_allocation_base *)&g_particle_emitter_allocator);
    g_particle_location_data_array = (s_data_array *)data_new("particle_location", 0x240, 0x60, 0,
        (c_allocation_base *)&g_particle_location_data_array_allocator);
    particle_emitter_gpu_initialize_storage();
}
void c_particle_system::initialize_for_map() {
    data_make_valid(g_particle_system_data_array);
    c_particle::initialize_for_map();
    c_particle_emitter::initialize_for_map();
    c_particle_location::initialize_for_map();
    c_particle_emitter_gpu::initialize_for_map();
}
void c_particle_system::dispose_from_map() {
    c_particle_emitter_gpu::dispose_from_map();
    c_particle_location::dispose_from_map();
    c_particle_emitter::dispose_from_map();
    c_particle::dispose_from_map();
    g_particle_system_data_array->valid = false;
}
void c_particle_system::initialize_for_new_structure_bsp()
{
    c_data_iterator<c_particle_system> iterator;
    iterator.begin((s_data_array *)g_particle_system_data_array);
    while (iterator.next()) {
        c_particle_system *particle_system = iterator.get_datum();
        if (!global_structure_bsp_is_active(particle_system->m_location.cluster_reference.bsp_index)) {
            s_location location;
            long first_location = particle_system->m_first_location;
            if (first_location != -1) {
                s_particle_location_datum *particle_location = particle_system_location_get(first_location);
                scenario_location_from_point(&location, &particle_location->matrix.position);
            } else {
                location_invalidate(&location);
            }
            particle_system->m_location = location;
        }
    }
}
void c_particle_system::remove_further_particle_locations(long location_index)
{
    long next_index;
    if (location_index != -1) {
        next_index = particle_system_location_get(location_index)->next_location_index;
        particle_system_location_get(location_index)->next_location_index = -1;
        this->m_last_location = location_index;
    } else {
        next_index = this->m_first_location;
        this->m_first_location = -1;
        this->m_last_location = -1;
    }
    while (next_index != -1) {
        long next = datum_get<s_particle_location_datum>(g_particle_location_data_array, next_index)->next_location_index;
        particle_location_delete(next_index);
        next_index = next;
    }
}
template<> void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_particle_system_data_array_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_register_member(2, name, "", size, 0, __tls_set_g_particle_system_data_array_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_particle_system_data_array_allocator>::deallocate(void *address)
{
    g_particle_system_data_array = 0;
    restricted_region_free_member(2, m_member_index);
}
inline float particle_system_lod::compute_distance_fade(const c_particle_system *system, float distance)
{
    float scaled = distance;
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x2b63d0: Load zero before the definition so both distance cutoffs return with blelr and bgelr.
    float zero = real_constant_pool_zero();
#else
    float zero = 0.0f;
#endif
    const c_particle_system_definition *definition = system->get_particle_system_definition();
    if (!definition->get_distance_fade_disabled()) {
        if (!definition->get_distance_in_world_units())
            scaled = g_particle_inv_render_delta * scaled;
        if (scaled <= definition->near_cutoff_distance)
            return zero;
        if (scaled >= definition->far_cutoff_distance)
            return zero;
        if (scaled < definition->near_cutoff_distance + definition->near_fade_range)
            return (scaled - definition->near_cutoff_distance) * definition->near_fade_scale;
        if (scaled > definition->far_cutoff_distance - definition->far_fade_range)
            return (definition->far_cutoff_distance - scaled) * definition->far_fade_scale;
    }
#if defined(_XBOX) && !defined(__clang__)
    return real_constant_pool_one();
#else
    return 1.0f;
#endif
}
template<> void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_emitter_set_array_address>::deallocate(void *allocation)
{
    g_particle_emitter_data = 0;
    restricted_region_free_member(2, m_member_index);
}

extern "C" {
// @address 0x7be794 g_particle_gpu_vertex_buffer
}
extern "C" void particle_emitter_gpu_set_storage_address(void *address)
{
    if (address != g_particle_gpu_storage_address && address != 0) {
        g_particle_gpu_storage_address = address;
        common_gpu_vertex_buffer_set_address(&g_particle_gpu_vertex_buffer, address);
        s_gpu_target_table<11> &layout = c_particle_emitter_gpu::x_layout;
        for (unsigned int i = 0; i < 11; i++) {
            s_gpu_storage_interface *entry = layout.entries[i];
            entry->refresh();
            layout.bounds[i].bounds = entry->bounds;
        }
    }
}
static __forceinline unsigned int particle_emitter_gpu_storage_size(unsigned char stride,
    const common_gpu_vertex_buffer &config)
{
    unsigned int size = stride * config.element_count;
    size *= config.element_size;
    return (size + 127) & 0xffffff80;
}
// @address 0x45fe40 ?reserve_memory@?$t_restricted_allocation_manager@$01$0A@$0A@$1?particle_emitter_gpu_set_storage_address@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_emitter_gpu_set_storage_address>::reserve_memory(const char *name, const char *type, unsigned int size,
    long alignment_bits)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[2], name, type, size, alignment_bits,
        particle_emitter_gpu_set_storage_address, 0, 0);
    particle_emitter_gpu_set_storage_address(restricted_region_get_member_address(2, member_index));
    m_member_index = member_index;
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, member_index);
}

// @address 0x45fec8 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?particle_emitter_gpu_set_storage_address@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_emitter_gpu_set_storage_address>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

template<>
void t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &particle_emitter_gpu_set_storage_address>::deallocate(void *)
{
    restricted_region_free_member(2, m_member_index);
}

void particle_emitter_gpu_initialize_storage(void)
{
    g_particle_gpu_block_data = (s_data_array *)data_new("c_particle_emitter_gpu::s_gamestate_proxy", 576, 24, 0,
        (c_allocation_base *)&g_particle_gpu_data_allocator);
    g_particle_gpu_block_data = (s_data_array *)data_new("c_particle_emitter_gpu::s_gamestate_proxy::s_row", 448, 20, 0,
        (c_allocation_base *)&g_particle_emitter_gpu_allocator);
    g_particle_gpu_storage_address = g_particle_gpu_storage_allocator.reserve_memory(
        "c_particle_emitter_gpu::s_gamestate_proxy", "x_gamestate_storage",
        particle_emitter_gpu_storage_size(rasterizer_vertex_get_stride_from_declaration(
            &g_rasterizer_vertex_buffer_declarations[
                g_particle_gpu_vertex_buffer.format]),
            g_particle_gpu_vertex_buffer), 7);
    particle_block_refresh_vertex_buffers((void *)&g_particle_block_queue_system);
}

t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_list_data_allocator> g_particle_allocator;
t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_particle_system_data_array_allocator> g_particle_system_allocator;

