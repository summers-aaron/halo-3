// blamlibxbox/effects.obj: from the records 0xf6a10 0xf6d30 0xf6dd8 0xf75e8 0xf76e0 0xf7848 0xf7998 0xf7ad8 0xf7e08 0xf7ef0 0xf8140 0x112048 0x1120e8 0x116060 0x116310 0x116680 0x122888 0x142b10 0x1460c0 0x1461c0 0x1467f0 0x146870 0x14b810 0x1795a8 0x179708 0x1797e0 0x1807b0 0x180890 0x1841a0 0x22e638 0x232c40 0x232cb8 0x232d20 0x232d30 0x232d60 0x233040 0x233230 0x233310 0x233438 0x233588 0x233748 0x233750 0x233b10 0x233c90 0x233d30 0x233e08 0x233f30 0x234020 0x234120 0x234198 0x234268 0x2342d8 0x234360 0x234438 0x2345b0 0x234688 0x234870 0x2349b8 0x234b98 0x234c90 0x234d78 0x234e78 0x234f10 0x234fb0 0x235058 0x2351e0 0x235450 0x2355c0 0x235650 0x235728 0x235da0 0x235f80 0x236068 0x2360e0 0x236150 0x236260 0x236820 0x2369a0 0x236c78 0x236ef8 0x237028 0x2372a0 0x2373f0 0x2374e8 0x237650 0x237768 0x237860 0x2379d8 0x237af0 0x237f70 0x238100 0x2382e8 0x238408 0x238510 0x238618 0x238708 0x2388a0 0x238a60 0x238b20 0x238d10 0x238ed8 0x239048 0x239350 0x239528 0x239590 0x239928 0x239c80 0x239ce8 0x239db0 0x23a398 0x23aeb0 0x23af38 0x23afa0 0x23b178 0x23b1e0 0x23b460 0x23b560 0x23b668 0x23b788 0x23b868 0x23b938 0x23bc40 0x23bd20 0x23bf78 0x23c4e0 0x23c530 0x23c598 0x23c638 0x23c658 0x23c6f8 0x23c718 0x23c7b8 0x23c7d8 0x23c878 0x23c898 0x23c938 0x23c950 0x23c970 0x23ca10 0x23ca28 0x23ca48 0x23cae8 0x23cb00 0x23cb20 0x23cb78 0x23cc18 0x23cc98 0x23cd08 0x23d658 0x2b8600 0x31fa68 0x33dea8 0x33dff8 0x33e100 0x33e1a0 0x4fb848 0x708088 0x708098 0x7080a8 0x7080b8 0x7080c8 0x7080d8 0x7080e8 0x70bbf0 0x70bc08 0x70bc20 0x70bc38 0x70bc50 0x70bc68 0x70bc80
#include "effects/effect_message_queue_tls.h"
// @address 0x11df688 effect_data
#include "effects/effect_data.h"
__declspec(thread) s_data_array *effect_data;
// @address 0x11df864 g_effect_message_queue
extern "C" {
__declspec(thread) void *g_effect_message_queue;
}
#include "effects/particle_array_globals.h"
// @flags /TP /O2 /Oc
#include "game/players_globals_types.h"
// @address 0x11df6ec g_effect_location_data
#include "effects/effect_location_data.h"
__declspec(thread) s_data_array *g_effect_location_data;
#include "effects/effect_array_globals.h"
#include "physics/breakable_surface_event_data.h"
// @address 0x11df7b8 g_effect_lightprobe_slot_data
#include "effects/effect_event_data.h"
#include "effects/effect_lightprobe_memory.h"
#include "effects/effect_lightprobe_slot_data.h"
__declspec(thread) s_data_array *g_effect_lightprobe_slot_data;
// @address 0x11df6c8 g_event_data
__declspec(thread) s_data_array *g_event_data;
#include "memory/data_types.h"
#include "units/unit_types.h"
#include "units/unit_inventory_inline.h"
#include "cutscene/cinematic_globals_types.h"
#include "cutscene/cinematics.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "scenario/cluster_reference_helpers.h"
#include "saved_games/iterator_types.h"
#include "game/player_mapping_types.h"
#include "effects/lens_flare_attachment_types.h"
#include "effects/effect_contrail_location_types.h"
#include "tag_files/tag_index.h"
#include "math/real_math.h"
#include "math/matrix_math_types.h"
#include "cseries/cseries_wrapped.h"
#include "tag_files/tag_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_types.h"
#include "game/player_types.h"
#include "game/game_globals_storage.h"
#include "game/game_globals.h"
#include "game/game_globals_inline.h"
#include "objects/object_placement.h"
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "memory/datum_index_inlines.h"
#include "objects/object_datum.h"
#include "tag_files/string_ids.h"
#include "objects/object_header_access.h"
#include "objects/object_node_matrices_inline.h"
#include "objects/object_attachment_types.h"
#include "physics/collision_types.h"
#include "game/game_time_types.h"
#include "multithreading/synchronized_value.h"
#include "scenario/scenario_tag.h"
#include "render/render_visibility_collection_globals.h"
#include "gpu_particle/shader_slot_types.h"
#include "effects/effect_vector_types.h"
#include "effects/effect_deterministic_types.h"
#include "camera/observer_proximity.h"
#include "physics/water_physics_types.h"
#include "cseries/cseries.h"
// match: 0x325f28: this unit has to declare effect_damage_batch_add_request before object_new (found by image check)
#include "effects/effect_damage_batch.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "sound/game_sound.h"
#include "sound/game_sound_types.h"
#include "sound/game_looping_sound_data.h"
#include "ai/props.h"
#include "camera/observer.h"
#include "effects/contrails.h"
#include "visibility/visible_items.h"
#include "math/real_math_types.h"
#include "cache/restricted_memory.h"
#include "effects/effects.h"
#include "game/player_control.h"
#include "interface/first_person_weapons.h"
#include "items/weapons.h"
#include "math/periodic_functions.h"
#include "math/random_math.h"
#include "math/random_math_range.h"
#include "memory/tls_globals.h"
#include "objects/damage.h"
#include "objects/damage_data.h"
#include "objects/lights.h"
#include "physics/impacts.h"
#include "physics/water_physics.h"
#include "render/render_objects_static_lighting.h"
#include "units/vehicle_type_component.h"
#include "ai/ai.h"
#include "main/main_render.h"
#include "effects/particle_system.h"
#include "effects/particle_emitter.h"
#include "gpu_particle/light_volume_gpu.h"
#include "effects/decals.h"
#include "game/players.h"
#include "physics/collisions.h"
#include "physics/collision_result_types.h"
#include "effects/particle_system_limits_owner_types.h"
#include "effects/effects_particle_system_entry_types.h"
#include "effects/event_flags_types.h"
#include "effects/transparent_sort_layer_types.h"
#include "effects/event_status_types.h"
#include "effects/effects_particle_system_list_types.h"
#include "effects/effect_contrail_profile_types.h"
#include "cache/cache_files.h"
#include "structures/structures.h"
#include "structures/cluster_bit_vectors.h"
#include "effects/effects_globals.h"
// @address 0x11df838 g_light_volume_gpu_row_data
// @address 0x11df71c g_effect_render_globals
// @address 0xb34 k_random_word_normalization
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
const float k_random_word_normalization = 0.000015259022f;
}
#endif
// @address 0x2251c k_real_zero_storage
// @address 0x11df8d8 g_first_person_weapon_data
// @address 0x221fc k_real_constant_pool
// @address 0x11df7dc g_static_lighting_effect_attachments
// @address 0x11df70c g_director_globals
// @address 0x9e7ea0 g_visibility_globals
// @address 0x361a8 k_random_direction_table
// @address 0x8c43a8 g_effect_budget_limits
// @address 0x11df790 g_particle_instance_owner_data
// @address 0x11df794 g_light_volume_system_data
// @address 0x11df7a4 g_particle_gpu_block_data
// @address 0x11df7e4 g_contrail_chain_data
// @address 0x11df7e8 g_light_volume_gpu_data
// @address 0x11df7f4 g_particle_instance_location_data
// @address 0x11df7f8 g_light_volume_location_data
// @address 0x11df814 g_particle_instance_data
// @address 0x11df818 g_light_volume_data
__declspec(thread) s_effect_render_globals *g_effect_render_globals;
__declspec(thread) fpw_first_person *g_first_person_weapon_data;
__declspec(thread) s_data_array *g_static_lighting_effect_attachments;
__declspec(thread) s_data_array *g_light_volume_location_data;
#include "effects/effect_counts.h"
__declspec(thread) s_effect_counts *g_effect_counts;
#include "effects/particle.h"
#include "effects/light_volume_allocator_globals.h"
#include "effects/particle_instance_types.h"
#include "gpu_particle/gpu_particle_globals.h"
#include "gpu_particle/contrail_gpu.h"
#include "math/matrix_math_functions.h"
#include "camera/observer_globals_types.h"
#include "camera/director.h"
#include "camera/camera_fx_settings.h"
#include "geometry/geometry_sampling.h"
#include "cache/restricted_memory_regions_inline.h"
#include "multithreading/thread_tls.h"
// @address 0x142b10 ?effect_get_event@@YAPBUevent_datum@@PBUeffect_datum@@J@Z
// @address 0x23cc98 ?set_bit@?$c_synchronized_bitvector@$0CAA@@@QAAXJ_N@Z
// @address 0x234b98 ?effect_detach@@YAXJ@Z
// @address 0x23aeb0 ?effect_location_find_next_of_type@@YAJPBUeffect_datum@@JF@Z
// @address 0x23c898 ?reserve_memory@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_havok_game_state_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x23c970 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_player_control_globals_deterministic_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
#include "cseries/crt_xdk.h"
#include <ppcintrinsics.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
static const float k_effects_real_max = FLT_MAX;
static const float k_effects_minimum_intensity = 0.0001f;

#include "effects/effect_datum.h"

#include "effects/effect_definitions.h"
#include "effects/effect_event_definitions.h"
#include "models/model_types.h"
#include "items/weapon_definitions.h"


#include "multithreading/synchronized_bitvector.h"
#include "tag_files/tag_groups.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_registration.h"
#include "effects/light_volume_gpu_types.h"
#include "effects/light_volume_location_types.h"
#include "effects/contrail.h"
#include "gpu_particle/contrail_gpu_types.h"
#include "effects/contrail_location.h"
#include "effects/light_volume_types.h"
#include "effects/contrail_system.h"
#include "effects/contrail_states_types.h"
#include "effects/particle_location_runtime_types.h"
#include "effects/particle_system_definitions.h"
#include "effects/effect_message.h"
#include "effects/effect_message_queue.h"
#include "effects/effect_budget_limits.h"

#if !defined(_XBOX) || defined(__clang__)
extern "C" {
s_particle_system_limits_owner *g_effect_budget_limits;
}
#endif

#include "effects/effect_location_types.h"
#include "effects/effect_render_globals.h"
#include "effects/effect_creation_data.h"
#include "geometry/geometry_sampling_types.h"
#include "gpu_particle/constant_entry_types.h"
#include "gpu_particle/light_volume_update_shader_types.h"
#include "gpu_particle/light_volume_render_shader.h"
#include "gpu_particle/particle_block_contrail_vertex_shader_constant_list_types.h"
#include "gpu_particle/gpu_empty_constant_list_types.h"
#include "gpu_particle/vertex_shader.h"
#include "gpu_particle/particle_block_contrail_shader_pair.h"
#include "interface/first_person_weapon_data_types.h"
#include "objects/object_definitions.h"
#include "effects/effect_damage_batch_types.h"
#include "effects/particle_movement_internals.h"
#include "effects/particle_states.h"
#include "effects/particle_runtime.h"
#include "effects/effect_creation_dependencies.h"
#include "effects/effect_node_attachment_types.h"
#include "physics/breakable_surface_event_types.h"
#include "structures/structure_bsp_tag.h"
#include "structures/structure_instanced_geometry.h"
#include "effects/screen_effect_definition_types.h"
#include "objects/cluster_reference_iterator.h"
#include "objects/object_cluster_reference.h"
#include "effects/light_volume_system_creation.h"
#include "effects/lens_flare_definitions.h"
#include "effects/lens_flares.h"
#include "scenario/scenario_definitions.h"
#include "scenario/scenario_lightmap_types.h"
#include "scenario/scenario_lightmap_lighting_types.h"
#include "render/render_lighting_types.h"
#include "render/views/player_view_types.h"
#include "effects/effect_camera.h"
#include "camera/director_storage_types.h"
#include "visibility/visibility_collection.h"
#include "visibility/visibility_projections_and_volumes.h"
#include "gpu_particle/effect_entry_types.h"
#include "effects/effect_pending_pool.h"
#include "effects/light_volumes.h"
#include "multithreading/interlocked_slist_push.h"
#include "rasterizer/rasterizer.h"
#include "objects/damage_effect_definitions.h"
#include "effects/decal_system_types.h"
#include "multithreading/interlocked_slist.h"
#include "models/model_variant_types.h"
#include "objects/crates.h"
#include "render/water_renderer.h"
#include "scenario/scenario_resource_registry.h"


struct s_effect_lightprobe_slot {
    short identifier;
    long effect_index;
    long object_index;
    real_point3d point;
    real_vector3d normal;
    float weight;
};


struct __declspec(align(8)) s_effect_camera_info {
    real_matrix4x3 matrix;
    float near_distance;
    float far_distance;
    float fade_distance;
};

struct s_effects_contrail_spawn_parameters { real_matrix4x3 matrix; };

struct s_effect_marker {
    real_point3d position;
    real_vector3d normal;
    long name;
};

struct effect_marker_list {
    short node_index;
    real_matrix4x3 node_matrix;
    long marker_count;
    const s_effect_marker *markers;
};




struct c_render_batch;
struct s_damage_data;
class c_decal_system;
struct s_particle_system_limits_owner;
struct data_array;
class c_contrail;
class c_contrail_profile;
struct s_particle_spawn_parameters;
struct s_particle_instance;
struct effect_vector_field;

extern "C" {
// @address 0x745cc8 g_effect_last_particle_system_index
long g_effect_last_particle_system_index = -1;
// @address 0x745ccc g_effect_last_contrail_system_index
long g_effect_last_contrail_system_index = -1;
// @address 0x745cd0 g_effect_last_contrail_profile_index
long g_effect_last_contrail_profile_index = -1;
// @address 0x745cd4 g_effect_last_light_volume_system_index
long g_effect_last_light_volume_system_index = -1;
// @address 0x783120 k_effect_non_looping_death_delay
float k_effect_non_looping_death_delay = 0.0333366692066192626953125f;
// @address 0x7e8c58 g_effect_render_locations
s_effect_render_location g_effect_render_locations[128];
// @address 0x7f1a10 g_particle_system_lists
s_effects_particle_system_list g_particle_system_lists[4];
// @address 0x8c29ce g_effect_cull_nonviolent_corpses
bool g_effect_cull_nonviolent_corpses;
// @address 0x8c3fe8 g_effect_damage_batch
c_render_batch *g_effect_damage_batch;
// @address 0x8c3fec g_effect_location_warning_count
int g_effect_location_warning_count;
// @address 0x8c3ff0 g_effect_camera_info
s_effect_camera_info g_effect_camera_info;
// @address 0x8c4030 g_effect_limit_count_index
long g_effect_limit_count_index;
// @address 0x8c4034 g_effect_render_location_count
unsigned long g_effect_render_location_count;
// @address 0x8c4038 g_effect_render_message_count
unsigned int g_effect_render_message_count;
// @address 0x8c46e8 g_effect_camera_active
unsigned char g_effect_camera_active;
// @address 0x8c4878 g_effect_camera_world_info
s_effect_camera_info g_effect_camera_world_info;
// @address 0x119a420 g_effect_limit_reached
bool g_effect_limit_reached[3];
// @address 0x119a424 g_event_limit_reached
bool g_event_limit_reached[3];
// @address 0x119a428 g_effect_location_limit_reached
bool g_effect_location_limit_reached[3];
// @address 0x119a42b g_effect_scenery_limit_reached
bool g_effect_scenery_limit_reached;
// @address 0x119a42c g_effect_lightprobe_limit_reached
bool g_effect_lightprobe_limit_reached[3];
// @address 0x119a448 g_effect_contrail_profile_limit_reached
bool g_effect_contrail_profile_limit_reached[3];
}

// Local c_data_iterator twin: the canonical form flips an fmadds tie in dangerous_effects_near_player.
struct player_datum_iterator {
    __forceinline void begin(s_data_array *data)
    {
        data_iterator_begin(&m_iterator, data);
    }
    __forceinline bool next()
    {
        m_datum = (player_datum *)data_iterator_next(&m_iterator);
        return m_datum != 0;
    }
    __forceinline player_datum *get_datum() const
    {
        return m_datum;
    }
    player_datum *m_datum;
    s_data_iterator m_iterator;
};
static __forceinline long atomic_or(long volatile *destination, long value)
{
    __lwsync();
    return _InterlockedOr(destination, value);
}
static __forceinline long atomic_and(long volatile *destination, long value)
{
    __lwsync();
    return _InterlockedAnd(destination, value);
}






const event_datum *effect_get_event(const effect_datum *effect, long event_count)
{
    event_datum *events = ((event_datum *)g_event_data->data);
    event_datum *event = &events[datum_index_to_absolute_index(effect->event_index)];
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    bool linked = (definition->flags >> 1) & 1;
    if (linked) {
        for (int i = 0; i < event_count; i++)
            event = &events[datum_index_to_absolute_index(event->next_event_index)];
    }
    return event;
}
// @address 0x22e638 ?__tls_set_g_effect_lightprobe_slot_data_allocator@@YAXPAX@Z
void __tls_set_g_effect_lightprobe_slot_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_decal_system_data, (s_data_array *)object);
}

// @address 0x23c530 ?adopt_old_effect_render_message_queue@@YAXPAX@Z
// @address 0x232d20 ?__tls_set_g_effect_message_queue_allocator@@YAXPAX@Z

namespace {
static __forceinline long restricted_region_add_effect_member(const char *name, const char *type, unsigned int size, long alignment)
{
    long member = g_restricted_regions[3].add_member(name, type, size, alignment, __tls_set_g_prop_data_allocator, 0, 0);
    void *address = restricted_region_get_member_address(3, member);
    __tls_set_g_prop_data_allocator(address);
    return member;
}
static __forceinline long restricted_region_add_event_member(const char *name, const char *type, unsigned int size, long alignment)
{
    long member = g_restricted_regions[3].add_member(name, type, size, alignment, __tls_set_g_event_data_allocator, 0, 0);
    void *address = restricted_region_get_member_address(3, member);
    __tls_set_g_event_data_allocator(address);
    return member;
}
static __forceinline long effects_restricted_region_add_effect_location_member(const char *name, const char *type, unsigned int size, long alignment)
{
    long member = g_restricted_regions[3].add_member(name, type, size, alignment, __tls_set_g_impact_data_allocator, 0, 0);
    void *address = restricted_region_get_member_address(3, member);
    data_set_new_base_address((s_data_array **)&g_effect_location_data, (s_data_array *)address);
    return member;
}
static __forceinline long effects_restricted_region_add_contrail_location_member(const char *name, const char *type, unsigned int size, long alignment)
{
    long member = g_restricted_regions[3].add_member(name, type, size, alignment, __tls_set_g_contrail_location_allocator, 0, 0);
    void *address = restricted_region_get_member_address(3, member);
    data_set_new_base_address((s_data_array **)&g_effect_lightprobe_slot_data, (s_data_array *)address);
    return member;
}
static __forceinline long restricted_region_add_message_member(const char *name, const char *type, unsigned int size, long alignment)
{
    long member = g_restricted_regions[3].add_member(name, type, size, alignment, __tls_set_g_effect_message_queue_allocator,
        adopt_old_effect_render_message_queue, 0);
    void *address = restricted_region_get_member_address(3, member);
    g_effect_message_queue = address;
    return member;
}
static __forceinline long effects_restricted_region_add_counts_member(const char *name, const char *type, unsigned int size, long alignment)
{
    long member = g_restricted_regions[2].add_member(name, type, size, alignment, __tls_set_g_havok_game_state_allocator, 0, 0);
    void *address = restricted_region_get_member_address(2, member);
    g_effect_lightprobe_memory = static_cast<s_effect_lightprobe *>(address);
    return member;
}
static __forceinline long effects_restricted_region_add_effect_counts_member(const char *name, const char *type, unsigned int size, long alignment)
{
    long member = g_restricted_regions[1].add_member(name, type, size, alignment, __tls_set_g_player_control_globals_deterministic_allocator, 0, 0);
    void *address = restricted_region_get_member_address(1, member);
    g_effect_counts = (s_effect_counts *)address;
    return member;
}
static __forceinline void *effect_datum_try_and_get(const s_data_array *array, long index)
{
    void *result = 0;
    if (index != -1) {
        int absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(index);
        int identifier = index >> 16;
        if (absolute_index < array->first_unallocated) {
            short *datum = (short *)(array->data + absolute_index * array->size);
            if (*datum != 0 && *datum == identifier)
                result = datum;
        }
    }
    return result;
}
}
__forceinline void c_light_volume_gpu::dispose_from_game() { g_light_volume_gpu_row_data = 0; g_light_volume_gpu_data = 0; }
__forceinline void c_light_volume::dispose_from_game() { g_light_volume_data = 0; }
__forceinline void c_light_volume_location::dispose_from_game() { g_light_volume_location_data = 0; }
__forceinline void c_light_volume_system::dispose_from_game() {
        c_light_volume_gpu::dispose_from_game();
        c_light_volume::dispose_from_game();
        c_light_volume_location::dispose_from_game();
        g_light_volume_system_data = 0;
}
__forceinline void c_contrail::dispose_from_game() { g_contrail_data_array = 0; }
__forceinline void c_contrail_gpu::dispose_from_game()
{
    g_contrail_gpu_link_data = 0;
    g_contrail_chain_data = 0;
    g_particle_instance_data = 0;
    g_particle_instance_location_data = 0;
    g_particle_instance_owner_data = 0;
    g_contrail_gpu_row_data = 0;
    g_contrail_gpu_data = 0;
}
__forceinline void c_contrail_location::dispose_from_game() { g_contrail_location_data = 0; }
__forceinline void c_contrail_system::dispose_from_game() {
    c_contrail_gpu::dispose_from_game();
    g_contrail_profile_data = 0;
    c_contrail::dispose_from_game();
    c_contrail_location::dispose_from_game();
    g_contrail_system_data = 0;
}
// @address 0x8c43b4 k_contrail_default_used_flags

__forceinline void c_particle_location::dispose_from_game() { g_particle_location_data_array = 0; }

namespace {


}


__forceinline void c_particle_emitter::dispose_from_game() { g_particle_emitter_data = 0; }

__forceinline void c_particle_system::dispose_from_game() {
    g_particle_emitter_gpu_data = 0;
    g_particle_gpu_block_data = 0;
    c_particle_location::dispose_from_game();
    c_particle_emitter::dispose_from_game();
    g_particle_data = 0;
    g_particle_system_data_array = 0;
}

static __forceinline void effects_add_render_message(const s_effect_message *message)
{
    unsigned int count = g_effect_render_message_count + 1;
    if (count <= g_effect_budget_limits->table->blocks[4].limits[message->limit_type].maximum_count) {
        g_effect_render_message_count = count;
        ((t_message_queue<s_effect_message, 512> *)g_effect_message_queue)->try_add_message(message);
    }
}

// @address 0x232c40 ?effects_particle_try_and_get@@YAPAXJ@Z
inline void *effects_particle_try_and_get(long particle_index)
{
    return effect_datum_try_and_get((const s_data_array *)g_particle_data, particle_index);
}
// @address 0x232cb8 ?__tls_set_g_event_data_allocator@@YAXPAX@Z
void __tls_set_g_event_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_event_data, (s_data_array *)object);
}
void __tls_set_g_effect_message_queue_allocator(void *address)
{
    g_effect_message_queue = address;
}
// @address 0x232d30 ?effect_is_looping@@YA_NPBUeffect_datum@@@Z
bool effect_is_looping(const effect_datum *effect)
{
    return effect->is_looping();
}


static const real_matrix4x3 effects_gravity_identity = {1.0f, {{ {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} }}, {0.0f, 0.0f, 0.0f}};
static const real_matrix4x3 effects_gravity_negative_identity = {1.0f, {{ {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} }}, {0.0f, 0.0f, 0.0f}};
// @address 0x233c90 ?build_gravity_effect_vectors@@YAXPBTreal_point3d@@QAUs_effect_vector@@@Z
void build_gravity_effect_vectors(const real_point3d *point, s_effect_vector vectors[2])
{
    const real_vector3d *down = global_down3d;
    real_vector3d *down_destination = &vectors[0].vector;
    *down_destination = *down;
    vectors[0].point = *point;
    vectors[0].marker_name = _string_id_gravity;
    const real_vector3d *up = global_up3d;
    real_vector3d *up_destination = &vectors[1].vector;
    *up_destination = *up;
    vectors[1].point = *point;
    vectors[1].marker_name = _string_id_up;
}

// @address 0x233040 ?effects_dispose@@YAXXZ
void effects_dispose()
{
    c_light_volume_system::dispose_from_game();
    c_contrail_system::dispose_from_game();
    c_particle_system::dispose_from_game();
    if ((void *)::effect_data) ::effect_data = 0;
    if (g_event_data) g_event_data = 0;
    if ((void *)g_effect_location_data) g_effect_location_data = 0;
    if ((void *)g_effect_lightprobe_slot_data) g_effect_lightprobe_slot_data = 0;
    if (g_effect_lightprobe_memory) g_effect_lightprobe_memory = 0;
    if ((void *)g_effect_counts) g_effect_counts = 0;
    g_effect_render_message_count = 0;
}
// @address 0x23b178 ?effect_get_valid@@YA_NJ@Z
bool effect_get_valid(long effect_index)
{
    return effect_datum_try_and_get(::effect_data, effect_index) != 0;
}

// @address 0x23c4e0 effect_send_render_message
extern "C" void effect_send_render_message(const s_effect_message *message)
{
    effects_add_render_message(message);
}

// @address 0x23c598 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_effect_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    __tls_set_g_effect_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_effect_member(name, "", size, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}
template<> __forceinline void t_restricted_allocation_manager<3, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    __tls_set_g_effect_data_allocator>::free_memory()
{
    ::effect_data = 0;
    restricted_region_free_member(3, m_member_index);
}
// @address 0x23c638 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_effect_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    __tls_set_g_effect_data_allocator>::deallocate(void *allocation)
{
    free_memory();
}
// @address 0x23c658 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_event_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    __tls_set_g_event_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_event_member(name, "", size, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}
template<> __forceinline void t_restricted_allocation_manager<3, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    __tls_set_g_event_data_allocator>::free_memory()
{
    g_event_data = 0;
    restricted_region_free_member(3, m_member_index);
}
// @address 0x23c6f8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_event_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    __tls_set_g_event_data_allocator>::deallocate(void *allocation)
{
    free_memory();
}
// @address 0x23c718 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_impact_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    &__tls_set_g_impact_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = effects_restricted_region_add_effect_location_member(name, "", size, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}
template<> __forceinline void t_restricted_allocation_manager<3, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    &__tls_set_g_impact_data_allocator>::free_memory()
{
    g_effect_location_data = 0;
    restricted_region_free_member(3, m_member_index);
}
// @address 0x23c7b8 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_impact_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    &__tls_set_g_impact_data_allocator>::deallocate(void *allocation)
{
    free_memory();
}
// @address 0x23c7d8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_contrail_location_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    __tls_set_g_contrail_location_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = effects_restricted_region_add_contrail_location_member(name, "", size, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}
template<> __forceinline void t_restricted_allocation_manager<3, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    __tls_set_g_contrail_location_allocator>::free_memory()
{
    g_effect_lightprobe_slot_data = 0;
    restricted_region_free_member(3, m_member_index);
}
// @address 0x23c878 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_contrail_location_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    __tls_set_g_contrail_location_allocator>::deallocate(void *allocation)
{
    free_memory();
}
// @address 0x23c898 ?reserve_memory@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_havok_game_state_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<> void *t_restricted_allocation_manager<2, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    &__tls_set_g_havok_game_state_allocator>::reserve_memory(const char *name, const char *type, unsigned int size, long alignment)
{
    // match: 0x23c898: value range assumption guides branch lowering
    __assume(alignment == 0);
    m_member_index = effects_restricted_region_add_counts_member(name, type, size, alignment);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(2, m_member_index);
}
// @address 0x23c938 ?allocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_havok_game_state_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<2, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    &__tls_set_g_havok_game_state_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
template<> __forceinline void t_restricted_allocation_manager<2, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    &__tls_set_g_havok_game_state_allocator>::free_memory()
{
    g_effect_lightprobe_memory = 0;
    restricted_region_free_member(2, m_member_index);
}
// @address 0x23c950 ?deallocate@?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_havok_game_state_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<2, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    &__tls_set_g_havok_game_state_allocator>::deallocate(void *allocation)
{
    free_memory();
}
// @address 0x23c970 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_player_control_globals_deterministic_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<1, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    &__tls_set_g_player_control_globals_deterministic_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment)
{
    // match: 0x23c970: value range assumption guides branch lowering
    __assume(alignment == 0);
    m_member_index = effects_restricted_region_add_effect_counts_member(name, type, size, alignment);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}
// @address 0x23ca10 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_player_control_globals_deterministic_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<1, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    &__tls_set_g_player_control_globals_deterministic_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
template<> __forceinline void t_restricted_allocation_manager<1, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    &__tls_set_g_player_control_globals_deterministic_allocator>::free_memory()
{
    g_effect_counts = 0;
    restricted_region_free_member(1, m_member_index);
}
// @address 0x23ca28 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_player_control_globals_deterministic_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<field_proc_t>(0), static_cast<field_proc_t>(0),
    &__tls_set_g_player_control_globals_deterministic_allocator>::deallocate(void *allocation)
{
    free_memory();
}
// @address 0x23ca48 ?reserve_memory@?$t_restricted_allocation_manager@$02$1?adopt_old_effect_render_message_queue@@YAXPAX@Z$0A@$1?__tls_set_g_effect_message_queue_allocator@@YAX0@Z@@QAAPAXPBD0IJ@Z
template<>
void *
t_restricted_allocation_manager<3, adopt_old_effect_render_message_queue, static_cast<field_proc_t>(0),
    __tls_set_g_effect_message_queue_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment)
{
    m_member_index = restricted_region_add_message_member(name, type, size, alignment);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}
// @address 0x23cae8 ?allocate@?$t_restricted_allocation_manager@$02$1?adopt_old_effect_render_message_queue@@YAXPAX@Z$0A@$1?__tls_set_g_effect_message_queue_allocator@@YAX0@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3, adopt_old_effect_render_message_queue, static_cast<field_proc_t>(0),
    __tls_set_g_effect_message_queue_allocator>::allocate(
    unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}
template<>
__forceinline void
t_restricted_allocation_manager<3, adopt_old_effect_render_message_queue, static_cast<field_proc_t>(0),
    __tls_set_g_effect_message_queue_allocator>::free_memory()
{
    long member = m_member_index;
    g_effect_message_queue = 0;
    restricted_region_free_member(3, member);
}
// @address 0x23cb00 ?deallocate@?$t_restricted_allocation_manager@$02$1?adopt_old_effect_render_message_queue@@YAXPAX@Z$0A@$1?__tls_set_g_effect_message_queue_allocator@@YAX0@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3, adopt_old_effect_render_message_queue, static_cast<field_proc_t>(0),
    __tls_set_g_effect_message_queue_allocator>::deallocate(
    void *allocation)
{
    free_memory();
}
// @address 0x23cb20 ?clear@?$t_message_queue@Us_effect_message@@$0CAA@@@QAAXXZ
template<class message, long count>
void t_message_queue<message, count>::clear()
{
    message_count.set(0);
    ((c_synchronized_array<(count + 31) / 32> *)&in_use)->set_array(0);
}
#if !defined(_XBOX) || defined(__clang__)
template void t_message_queue<s_effect_message, 512>::clear();
#endif
template<> __forceinline void t_message_queue<s_effect_message, 512>::add_message(const s_effect_message *new_message)
{
    long index = message_count.increment() - 1;
    in_use.set_bit(index, false);
    memcpy(&messages[index], new_message, sizeof(s_effect_message));
    in_use.set_bit(index, true);
}
// @address 0x23cb78 ?try_add_message@?$t_message_queue@Us_effect_message@@$0CAA@@@QAA_NPBUs_effect_message@@@Z
template<> bool t_message_queue<s_effect_message, 512>::try_add_message(const s_effect_message *new_message)
{
    if (message_count.peek() + 1 < 512) {
        add_message(new_message);
        return true;
    }
    return false;
}
// @address 0x23cc18 ?process_messages@?$t_message_queue@Us_effect_message@@$0CAA@@@QAAXP6AXPAUs_effect_message@@J@ZJ@Z
template<> void t_message_queue<s_effect_message, 512>::process_messages(void (*callback)(s_effect_message *, long), long parameter)
{
    long message_limit = message_count.peek();
    for (long index = 0; index < message_limit; index++) {
        if (in_use.test_bit(index))
            callback(&messages[index], 0);
    }
}
template<long k_bit_count> void c_synchronized_bitvector<k_bit_count>::set_bit(long index, bool set)
{
    long volatile *word = &m_bits.get_member(index >> 5)->value;
    long mask = 1 << (index & 31);
    if (set)
        atomic_or(word, mask);
    else
        atomic_and(word, ~mask);
}
#if !defined(_XBOX) || defined(__clang__)
template void c_synchronized_bitvector<512>::set_bit(long index, bool set);
#endif
// @address 0x4fb848 ?__tls_set_g_effect_data_allocator@@YAXPAX@Z
void __tls_set_g_effect_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&command_script_data, (s_data_array *)object);
}


void effect_detach(long handle)
{
    effect_datum *datum = (effect_datum *)effect_datum_try_and_get(::effect_data, handle);
    if (datum) {
        object_datum *object = (object_datum *)object_try_and_get_and_verify_type(datum->object_index, 0xffffffff);
        if (object) {
            s_object_datum_attachment *attachments = (s_object_datum_attachment *)((char *)object + object->attached_lights.offset);
            int count = object->attached_lights.size / sizeof(s_object_datum_attachment);
            for (int i = 0; i < count; i++) {
                if (attachments[i].datum_index == handle && attachments[i].type == 2) {
                    attachments[i].datum_index = -1;
                    break;
                }
            }
        }
    }
}


static __forceinline void effect_looping_sound_stop(long looping_sound_index)
{
    game_looping_sound_datum *sound = datum_get<game_looping_sound_datum>(game_looping_sound_data, looping_sound_index);
    sound->flags.set(1, true);
}
static __forceinline void effects_stop_looping_sound(effect_datum *effect)
{
    if (effect->looping_sound_index != -1) {
        effect_looping_sound_stop(effect->looping_sound_index);
        effect->looping_sound_index = -1;
    }
}

// @address 0x2345b0 ?effect_stop@@YAXJ_N@Z
void effect_stop(long effect_index, bool detach)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, effect_index);
    if (effect) {
        effects_stop_looping_sound(effect);
        effect->flags.set(_effect_stopping_bit, true);
        if (effect->flags.test(_effect_looping_bit))
            effect->flags.set(_effect_detach_bit, detach);
        else
            effect_delete_internal(effect_index, true);
    }
}

static __forceinline void effects_datum_initialize_isolated(s_data_array *data, s_datum_header *datum)
{
    memset(datum, 0, data->size);
    if ((unsigned short)data->isolated_next_identifier == 0xffff)
        data->isolated_next_identifier = (short)0x8000;
    datum->identifier = data->isolated_next_identifier;
    data->isolated_next_identifier++;
}

// @address 0xf7848 ?datum_new_in_range@@YAJPAUs_data_array@@JJ_N@Z
long datum_new_in_range(s_data_array *data, long minimum_index, long count, bool isolated)
{
    long result = -1;
    long absolute_index = -1;
    long index = minimum_index;
    long maximum_index = minimum_index + count < data->first_unallocated ? minimum_index + count : data->first_unallocated;
    for (; index < maximum_index; index++) {
        if (!(((unsigned long *)data->in_use_bit_vector)[index >> 5] & (1 << (index & 31)))) {
            absolute_index = index;
            break;
        }
    }
    if (absolute_index == -1 && index < minimum_index + count)
        absolute_index = index;
    if (absolute_index != -1) {
        s_datum_header *datum = (s_datum_header *)(data->data + absolute_index * data->size);
        ((unsigned long *)data->in_use_bit_vector)[absolute_index >> 5] |= 1 << (absolute_index & 31);
        data->actual_count++;
        if (data->first_unallocated <= absolute_index)
            data->first_unallocated = absolute_index + 1;
        if (isolated)
            effects_datum_initialize_isolated(data, datum);
        else
            ::datum_initialize(data, (s_datum_header *)datum);
        result = absolute_index | ((unsigned short)datum->identifier << 16);
    }
    return result;
}

// @address 0xf8140 ?effect_delete_internal@@YAXJ_N@Z
void effect_delete_internal(long effect_index, bool detach)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, effect_index);
    if (effect) {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        if (detach)
            effect_detach(effect_index);
        effects_stop_looping_sound(effect);
        effect->flags.set(_effect_marked_for_deletion_bit, true);
        effect->death_delay = effect->object_index == -1 ? definition->death_delay : definition->local_space_death_delay;
    }
}

static __forceinline unsigned long *effect_get_random_seed_address(bool deterministic, const effect_datum *effect)
{
    if (deterministic) {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        if (_rotl(definition->flags, 23) & 1)
            return tls_random_seed();
    }
    return (unsigned long *)&effect->random_seed;
}

// @address 0x234d78 ?effect_update_first_person_user_flags@@YAXJJ_N@Z
void effect_update_first_person_user_flags(long object_index, long user_index, bool first_person)
{
    for (long effect_index = ::data_next_index(effect_data, -1); effect_index != -1;
        effect_index = inl_data_next_index(::effect_data, effect_index))
    {
        effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
        if (first_person) {
            if (effect->object_index == object_index && effect->flags.test(_effect_first_person_bit))
                effect->first_person_user_flags |= 1 << user_index;
        } else if ((effect->first_person_user_flags & (1 << user_index)) && effect->object_index == object_index) {
            effect->first_person_user_flags &= ~(1 << user_index);
        }
    }
}

// @address 0x234e78 ?effect_real_random_range@@YAMPBD_NPBUeffect_datum@@MM@Z
float effect_real_random_range(const char *name, bool deterministic, const effect_datum *effect, float minimum, float maximum)
{
    unsigned long *seed = effect_get_random_seed_address(deterministic, effect);
    return _real_random_range(seed, minimum, maximum);
}

static __forceinline float effects_scale_random_range_value(const effect_datum *effect, float value, unsigned long mask_a, unsigned long mask_b, short index)
{
    if (mask_a & (1 << index))
        value *= effect->scale_a;
    if (mask_b & (1 << index))
        value *= effect->scale_b;
    return value;
}

// @address 0x234f10 ?effect_real_random_range@@YAMPBD_NPBUeffect_datum@@MMKKF@Z
float effect_real_random_range(const char *name, bool deterministic, const effect_datum *effect, float minimum, float maximum,
    unsigned long mask_a, unsigned long mask_b, short index)
{
    float scaled_minimum = effects_scale_random_range_value(effect, minimum, mask_a, mask_b, index);
    float scaled_range = effects_scale_random_range_value(effect, maximum - minimum, mask_a, mask_b, index + 1);
    return effect_real_random_range(name, deterministic, effect, 0.0f, scaled_range) + scaled_minimum;
}



// @address 0x234fb0 ?effect_random_direction3d@@YAXPBD_NPBUeffect_datum@@PATreal_vector3d@@@Z
void effect_random_direction3d(const char *name, bool deterministic, const effect_datum *effect, real_vector3d *direction)
{
    unsigned long *seed = effect_get_random_seed_address(deterministic, effect);
    short index = random_range(seed, 0, 1026);
    *direction = k_random_direction_table[index];
}

// event_allocate (0x235450)
// @address 0x235450 event_allocate
extern "C" long event_allocate(const s_effect_definition *definition)
{
    bool deterministic = definition->flags.test(_effect_definition_first_person_visibility_bit);
    long event_count;
    if (definition->flags.test(_effect_definition_parallel_events_bit))
        event_count = definition->events.count;
    else
        event_count = definition->events.count != 0;
    unsigned char priority = definition->limit_type;
    s_effect_counts *bucket = &g_effect_counts[deterministic];
    long result = -1;
    event_datum *previous = 0;
    long start = deterministic ? 0 : 320;
    if (bucket->event_count + event_count <= g_effect_budget_limits->table->blocks[1].limits[priority].maximum_count) {
        for (long i = 0; i < event_count; i++) {
            long event_index = datum_new_in_range(g_event_data, start, 320, !deterministic);
            bucket->event_count++;
            g_event_limit_reached[priority] = true;
            if (!previous)
                result = event_index;
            else
                previous->next_event_index = event_index;
            previous = datum_get<event_datum>(g_event_data, event_index);
            previous->event_block_index = (unsigned char)i;
            previous->next_event_index = -1;
            previous->event_counter = 0;
        }
    } else {
        g_event_limit_reached[priority] = false;
    }
    return result;
}

// @address 0x2355c0 ?event_destroy@@YAXJE@Z
void event_destroy(long event_index, unsigned char count_index)
{
    if (event_index != -1) {
        long next_event_index = datum_get<event_datum>(g_event_data, event_index)->next_event_index;
        if (next_event_index != -1)
            event_destroy(next_event_index, count_index);
        ::datum_delete(g_event_data, event_index);
        g_effect_counts[count_index].event_count--;
    }
}

// @address 0x235650 ?effect_set_event@@YAXJF@Z
void effect_set_event(long effect_index, short event_block_index)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, effect_index);
    if (effect) {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        if (event_block_index >= 0 && event_block_index < definition->events.count) {
            event_datum *event = datum_get<event_datum>(g_event_data, effect->event_index);
            if (event->event_block_index != event_block_index)
                event->flags.set(_event_stale_bit, false);
            event->event_block_index = (unsigned char)event_block_index;
            event->flags.set(_event_started_bit, true);
            effect_start_event(effect_index, effect->event_index);
        }
    }
}


__forceinline s_effect_message::s_effect_message(long effect_index, long event_index)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    this->effect_index = effect_index;
    definition_index = effect->definition_index;
    this->event_index = event_index;
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, definition_index);
    event_block_index = event->event_block_index;
    limit_type = ((const effect_event_definition *)definition->events.address)[event->event_block_index].limit_type;
    event_counter = event->event_counter;
    location_index = 0;
}


__forceinline s_effect_message::s_effect_message(long effect_index)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const event_datum *event = datum_get<event_datum>(g_event_data, effect->event_index);
    this->effect_index = effect_index;
    definition_index = effect->definition_index;
    event_index = effect->event_index;
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, definition_index);
    limit_type = definition->limit_type;
    event_block_index = event->event_block_index;
    event_counter = event->event_counter;
    location_index = 0;
}

__forceinline s_effect_message::s_effect_message(long effect_index, long event_index, unsigned char limit_type)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    this->effect_index = effect_index;
    definition_index = effect->definition_index;
    this->event_index = event_index;
    event_block_index = event->event_block_index;
    this->limit_type = limit_type;
    event_counter = event->event_counter;
    location_index = 0;
}

static __forceinline bool effect_should_skip_event(const effect_datum *effect, long event_block_index)
{
    bool skip = false;
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    if (event_block_index >= 0 && event_block_index < definition->events.count) {
        const effect_event_definition *event_definition = &((const effect_event_definition *)definition->events.address)[event_block_index];
        if (event_definition->skip_fraction > 0.0f &&
            effect_real_random_range("effect_should_skip_event", true, effect, 0.0f, 1.0f) < event_definition->skip_fraction)
            skip = true;
    }
    return skip;
}

// @address 0x238708 ?effect_get_next_event_index@@YAJPAUeffect_datum@@@Z
long effect_get_next_event_index(effect_datum *effect)
{
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    const event_datum *event = datum_get<event_datum>(g_event_data, effect->event_index);
    long loop_start_event_index = definition->loop_start_event;
    long last_event_index = definition->events.count - 1;
    long event_index = effect->flags.test(_effect_event_started_bit) ? event->event_block_index : -1;
    bool done = false;
    if (effect->is_looping() && loop_start_event_index == -1)
        loop_start_event_index = 0;
    while (!done) {
        done = true;
        if (effect_is_looping(effect) && event_index == last_event_index && loop_start_event_index != -1)
            event_index = loop_start_event_index;
        else
            event_index++;
        if (event_index < definition->events.count && effect_should_skip_event(effect, event_index))
            done = false;
    }
    return event_index;
}


// @address 0x2388a0 ?effect_start_event@@YAXJJ@Z
void effect_start_event(long effect_index, long event_index)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, effect_index);
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    const effect_event_definition *event_definition = &((const effect_event_definition *)definition->events.address)[event->event_block_index];
    event->flags.toggle(_event_started_bit);
    event->flags.set(_event_finished_bit, false);
    event->time = 0.0f;
    if (!event->flags.test(_event_started_bit)) {
        if (event_definition->delay_bounds.lower == event_definition->delay_bounds.upper)
            event->duration = event_definition->delay_bounds.lower;
        else
            event->duration = effect_real_random_range(
                "effect_start_event 2", true, effect, event_definition->delay_bounds.lower, event_definition->delay_bounds.upper);
        if (event->duration == 0.0f)
            event->flags.toggle(_event_started_bit);
    }
    if (event->flags.test(_event_started_bit)) {
        if (event_definition->duration_bounds.lower == event_definition->duration_bounds.upper)
            event->duration = event_definition->duration_bounds.lower;
        else
            event->duration = effect_real_random_range("effect_start_event 1", true, effect,
                event_definition->duration_bounds.lower, event_definition->duration_bounds.upper);
        if (event->flags.test(_event_stale_bit)) {
            event_refresh_particles(effect_index, event_index);
        } else {
            event->event_counter++;
            event_create_particle_systems(effect_index, event_index);
        }
        event_generate_parts(effect_index, event_index, true);
        if (!definition->flags.test(_effect_definition_recreate_systems_bit))
            event->flags.set(_event_stale_bit, true);
    }
}

// @address 0x238a60 ?effect_restart_all_events@@YAXJ@Z
void effect_restart_all_events(long effect_index)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, effect_index);
    long event_index = effect->event_index;
    while (event_index != -1) {
        event_datum *event = datum_get<event_datum>(g_event_data, event_index);
        event->flags.set(_event_started_bit, true);
        effect_start_event(effect_index, event_index);
        event_index = event->next_event_index;
    }
}


static __forceinline effect_location_datum *effects_location_get(long location_index)
{
    return datum_get<effect_location_datum>(g_effect_location_data, location_index);
}

// @address 0x234870 ?effect_delete_locations@@YAXJ@Z
void effect_delete_locations(long effect_index)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, effect_index);
    if (effect) {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        bool deterministic = effect->flags.test(_effect_deterministic_bit);
        for (long location_block_index = 0; location_block_index < definition->locations.count; location_block_index++) {
            long location_index = effect->location_indices[location_block_index];
            while (location_index != -1) {
                effect_location_datum *location = (effect_location_datum *)effect_datum_try_and_get((const s_data_array *)g_effect_location_data,
                    location_index);
                if (!location)
                    break;
                long next_location_index = location->next_location_index;
                ::datum_delete((s_data_array *)g_effect_location_data, location_index);
                g_effect_counts[deterministic].location_count--;
                location_index = next_location_index;
            }
            effect->location_indices[location_block_index] = -1;
        }
    }
}


// @address 0x2382e8 ?effect_add_render_location@@YAXPBUs_effect_message@@@Z
void effect_add_render_location(const s_effect_message *message)
{
    const s_effect_message *location_message = message;
    const s_effect_render_location_data *location_data = &location_message->render_location;
    effect_location_datum *location = (effect_location_datum *)effect_datum_try_and_get((const s_data_array *)g_effect_location_data,
        location_data->location_index);
    if (location) {
        bool first_person = ((unsigned long)location_message->location_index >> 4) & 1;
        if (g_effect_render_location_count < 128) {
            long render_location_index = g_effect_render_location_count++;
            s_effect_render_location *render_location = &g_effect_render_locations[render_location_index];
            long index = location->first_render_location_index;
            if (index != -1) {
                s_effect_render_location *last;
                do {
                    last = &g_effect_render_locations[index];
                    index = last->next_index;
                } while (index != -1);
                last->next_index = (short)render_location_index;
            } else {
                location->first_render_location_index = render_location_index;
            }
            memcpy(render_location->data, location_data->data, sizeof(render_location->data));
            render_location->next_index = -1;
            render_location->flags.set(0, first_person);
        }
    }
}

// @address 0x234c90 ?effect_delete_first_person_locations@@YAXPAUeffect_datum@@@Z
void effect_delete_first_person_locations(effect_datum *effect)
{
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    bool deterministic = effect->flags.test(_effect_deterministic_bit);
    for (long location_block_index = 0; location_block_index < definition->locations.count; location_block_index++) {
        long *location_index = &effect->location_indices[location_block_index];
        while (*location_index != -1) {
            effect_location_datum *location = effects_location_get(*location_index);
            if (location->node_index != 0xffff && (location->node_index & 0x8000)) {
                long next_location_index = location->next_location_index;
                ::datum_delete((s_data_array *)g_effect_location_data, *location_index);
                g_effect_counts[deterministic].location_count--;
                *location_index = next_location_index;
            } else {
                location_index = &location->next_location_index;
            }
        }
    }
}

long effect_location_find_next_of_type(const effect_datum *effect, long location_index, short type)
{
    long result = location_index;
    if (location_index != -1)
    {
        effect_location_datum *location = effects_location_get(location_index);
        bool first_person = location->node_index != 0xffff && (location->node_index & 0x8000);
        bool desired = false;
        switch (type)
        {
        case 1:
            desired = true;
            break;
        case 3:
            desired = first_person;
            break;
        }
        if (first_person != desired)
            result = effect_location_find_next_of_type(effect, location->next_location_index, type);
    }
    return result;
}

// event_update_time (0x238b20)
bool event_update_time(long effect_index, long event_index, float delta_time)
{
    effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    bool finished = event->flags.test(_event_finished_bit);
    while (delta_time >= 0.0f && !finished) {
        float remaining = event->duration - event->time;
        float elapsed = delta_time;
        bool ended;
        if (delta_time >= remaining) {
            ended = true;
            event->time = event->duration;
            delta_time -= remaining;
            elapsed = remaining;
        } else {
            ended = false;
            event->time += delta_time;
            delta_time = -1.0f;
        }
        if (event->flags.test(_event_started_bit)) {
            if (!effect->flags.test(_effect_culled_bit)) {
                effect_apply_accelerations(effect, event_index, elapsed);
                event_generate_parts(effect_index, event_index, false);
            }
            if (ended) {
                if (definition->flags.test(_effect_definition_parallel_events_bit)) {
                    event->flags.set(_event_finished_bit, true);
                    finished = true;
                    break;
                } else {
                    long next_event_index = effect_get_next_event_index(effect);
                    if (next_event_index < definition->events.count) {
                        effect_set_event(effect_index, (short)next_event_index);
                        event = datum_get<event_datum>(g_event_data, effect->event_index);
                    } else {
                        finished = true;
                        break;
                    }
                }
            }
        } else if (ended) {
            effect_start_event(effect_index, event_index);
        }
    }
    return finished;
}

// @address 0x23af38 ?effect_location_get_next_instance@@YAPAUeffect_location_datum@@PBUeffect_datum@@PAJF@Z
effect_location_datum *effect_location_get_next_instance(const effect_datum *effect, long *location_index, short type)
{
    long next_location_index = effect_location_find_next_of_type(effect, *location_index, type);
    effect_location_datum *location = 0;
    *location_index = next_location_index;
    if (next_location_index != -1) {
        location = effects_location_get(next_location_index);
        *location_index = location->next_location_index;
    }
    return location;
}

static __forceinline bool effects_game_clusters_test_reference(const s_game_cluster_bit_vectors *clusters, const s_effect_cluster_reference *cluster_reference)
{
    long bsp_index = cluster_reference->bsp_index;
    long cluster_index = cluster_reference->cluster_index;
    return clusters->bit_vectors[bsp_index].test(cluster_index);
}

static __forceinline bool effects_cluster_visibility_valid(const effect_datum *effect)
{
    return cluster_reference_valid(&effect->cluster_reference) || inl_cinematic_in_progress();
}

// @address 0x239350 ?effect_update_visibility@@YA_NJ@Z
bool effect_update_visibility(long effect_index)
{
    effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    bool culled = true;
    if (effect->flags.test(_effect_first_person_only_bit)) {
        culled = g_effect_render_globals->first_person_hidden == 0;
    } else if (inl_cinematic_in_progress() || definition->flags.test(_effect_definition_always_visible_bit)) {
        culled = false;
    } else if (effects_cluster_visibility_valid(effect)) {
        if (definition->flags.test(_effect_definition_first_person_visibility_bit))
            culled = !effects_game_clusters_test_reference((const s_game_cluster_bit_vectors *)game_globals->cluster_pvs, &effect->cluster_reference);
        else
            culled = !effects_game_clusters_test_reference((const s_game_cluster_bit_vectors *)game_globals->cluster_pvs_local, &effect->cluster_reference);
    }
    if (culled != effect->flags.test(_effect_culled_bit)) {
        effect->flags.set(_effect_culled_bit, culled);
        if (culled && !effect->is_looping()) {
            effect_delete_internal(effect_index, true);
            effect = 0;
        }
    }
    return effect == 0;
}

// @address 0x238d10 ?effect_update_time@@YA_NJM@Z
bool effect_update_time(long effect_index, float delta_time)
{
    effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    if (effect->flags.test(_effect_marked_for_deletion_bit)) {
        if (!(effect->death_delay > 0.0f))
            effect_destroy(effect_index, false);
        else
            effect->death_delay -= delta_time;
    } else if (!effect->flags.test(_effect_stopping_bit)) {
        bool all_finished = true;
        for (long event_index = effect->event_index; event_index != -1; ) {
            if (!event_update_time(effect_index, event_index, delta_time))
                all_finished = false;
            event_datum *event = datum_get<event_datum>(g_event_data, event_index);
            event_index = event->next_event_index;
        }
        if (all_finished) {
            const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
            if (definition->flags.test(_effect_definition_parallel_events_bit) && effect->is_looping())
                effect_restart_all_events(effect_index);
            else {
                effect_delete_internal(effect_index, true);
                effect = 0;
            }
        }
        if (effect) {
            effect->flags.set(_effect_ping_from_impact_bit, false);
            if (effect->flags.test(_effect_object_deleted_bit))
                effect_delete_internal(effect_index, true);
        }
    }
    return effect == 0;
}

// @address 0x239528 ?effect_update@@YAXJM@Z
void effect_update(long effect_index, float time_step)
{
    unsigned char done = effect_update_object(effect_index);
    if (done == 0)
        done = effect_update_visibility(effect_index);
    if (done == 0)
        effect_update_time(effect_index, time_step);
}


// @address 0x23b460 ?effect_set_location_vector@@YAXJPBTreal_vector3d@@@Z
void effect_set_location_vector(long effect_index, const real_vector3d *vector)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, effect_index);
    if (effect) {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        for (long location_index = 0; location_index < definition->locations.count; location_index++) {
            const effect_location_definition *location_definition = &((const effect_location_definition *)definition->locations.address)[location_index];
            if (location_definition->marker_name == _string_id_child && effect->location_indices[location_index] != -1) {
                effect_location_datum *location = effects_location_get(effect->location_indices[location_index]);
                effect->flags.set(_effect_vector_bit, true);
                location->vector = *vector;
            }
        }
    }
}


// @address 0x23b560 ?effect_ping@@YA_NJJMM_N@Z
bool effect_ping(long effect_index, long definition_index, float scale_a, float scale_b, bool force_restart)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, effect_index);
    bool result = false;
    if (effect && effect->definition_index == definition_index) {
        result = true;
        effect->flags.set(_effect_pinged_bit, true);
        if (force_restart || effect->flags.test(_effect_marked_for_deletion_bit) || effect->flags.test(_effect_stopping_bit))
            effect_start(effect_index);
        effect->scale_a = scale_a;
        effect->scale_b = scale_b;
    }
    return result;
}


// @address 0x113cec0 ?render_visibility_collection_globals@@3Us_render_visibility_collection_globals@@A
static __forceinline const s_damage_owner &effects_creation_default_damage_owner()
{
    return *damage_owner_unknown();
}

// @address 0xf6d30 ?effect_creation_data_initialize@@YAXPAUs_effect_creation_data@@@Z
void effect_creation_data_initialize(s_effect_creation_data *creation_data)
{
    memset(creation_data, 0, sizeof(*creation_data));
    creation_data->lightprobe_slot_index = 0;
    float one = 1.0f;
    creation_data->definition_index = -1;
    creation_data->scale_a = one;
    creation_data->scale_b = one;
    creation_data->user_mask = 0;
    creation_data->first_person_user_index = 0;
    creation_data->node_index = -1;
    creation_data->structure_bsp_index = -1;
    creation_data->object_index = -1;
    creation_data->damage_owner = effects_creation_default_damage_owner();
    creation_data->parent_effect_index = 0;
    creation_data->marker_index = -1;
    creation_data->lightprobe_source = 0;
    creation_data->location_index = -1;
    creation_data->particle_system_index = -1;
}
// @address 0x233b10 ?effect_in_player_range@@YA_NPBUs_effect_creation_data@@@Z
bool effect_in_player_range(const s_effect_creation_data *creation_data)
{
    bool in_range = true;
    if (creation_data->definition_index != -1 && creation_data->lightprobe_source &&
        cluster_reference_valid(&creation_data->lightprobe_source->location.cluster_reference) && !inl_cinematic_in_progress()) {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, creation_data->definition_index);
        float minimum_distance = definition->minimum_distance;
        float maximum_distance = definition->maximum_distance;
        if (minimum_distance != 0.0f && maximum_distance != 0.0f) {
            float closest_distance_squared = k_effects_real_max;
            for (long user_index = 0; user_index < 4; user_index++) {
                const s_observer_result *camera = observer_try_and_get_camera(user_index);
                if (camera) {
                    float distance_squared = distance_squared3d(&camera->position, &creation_data->lightprobe_source->point);
                    if (distance_squared < closest_distance_squared)
                        closest_distance_squared = distance_squared;
                }
            }
            if (closest_distance_squared >= minimum_distance * minimum_distance) {
                if (closest_distance_squared > maximum_distance * maximum_distance)
                    in_range = false;
                else
                    in_range = game_clusters_test(
                        &render_visibility_collection_globals.visible_clusters, creation_data->lightprobe_source->location.cluster_reference);
            }
        }
    }
    return in_range;
}


// @address 0x23b788 ?effect_initialize_lightprobe_slot@@YAXJPBUs_effect_creation_data@@@Z
void effect_initialize_lightprobe_slot(long effect_index, const s_effect_creation_data *creation_data)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    s_effect_lightprobe_slot *slot =
        datum_get<s_effect_lightprobe_slot>(g_effect_lightprobe_slot_data, effect->lightprobe_slot_index);
    slot->effect_index = effect_index;
    slot->object_index = creation_data->structure_bsp_index;
    if (creation_data->lightprobe_source) {
        slot->point = creation_data->lightprobe_source->point;
        slot->normal = creation_data->lightprobe_source->plane.n;
    } else {
        slot->normal = (*global_zero_vector3d);
    }
    slot->weight = 0.0f;
}

// @address 0x23b868 ?effect_allocate_lightprobe_slot@@YAXPAUeffect_datum@@PBUs_effect_creation_data@@@Z
void effect_allocate_lightprobe_slot(effect_datum *effect, const s_effect_creation_data *creation_data)
{
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    bool lightprobe = (definition->flags >> _effect_definition_lightprobe_bit) & 1;
    bool lightprobe_always = (definition->flags >> _effect_definition_lightprobe_always_bit) & 1;
    if (lightprobe || lightprobe_always) {
        if ((unsigned long)(((s_data_array *)g_effect_lightprobe_slot_data)->actual_count + 1)
            <= g_effect_budget_limits->table->blocks[3].limits[definition->limit_type].maximum_count) {
            effect->lightprobe_slot_index = datum_new((s_data_array *)g_effect_lightprobe_slot_data);
            g_effect_lightprobe_limit_reached[definition->limit_type] = true;
        } else {
            g_effect_lightprobe_limit_reached[definition->limit_type] = false;
        }
    }
}


static const bool debug_effects_lightprobe_sampling = false;
static __forceinline const s_geometry_sample *effect_get_geometry_sample(long lightprobe_index)
{
    const s_geometry_sample *sample = debug_effects_lightprobe_sampling ? 0 : &c_geometry_sampler::m_default_lightprobe_sample;
    if (lightprobe_index != -1) {
        const s_effect_lightprobe *lightprobe = &g_effect_lightprobe_memory[datum_index_to_absolute_index(lightprobe_index)];
        const s_geometry_sample *lightprobe_sample = debug_effects_lightprobe_sampling ? 0 : &lightprobe->sample;
        if (lightprobe->flags.test(0))
            sample = lightprobe->flags.test(1) ? &lightprobe->sample : lightprobe_sample;
    }
    return sample;
}

// @address 0x23bc40 effect_set_camera_info
extern "C" void effect_set_camera_info(const s_effect_camera_source *source)
{
    g_effect_camera_info.matrix.scale = 1.0f;
    g_effect_camera_info.matrix.forward = *global_down3d;
    g_effect_camera_info.matrix.left = *global_left3d;
    g_effect_camera_info.matrix.up = *global_forward3d;
    g_effect_camera_info.matrix.position = *global_origin3d;
    float distance = ((const c_particle_system_definition *)source)->far_cutoff_distance * 1.5f;
    if (!(g_effect_camera_info.far_distance <= distance))
        distance = g_effect_camera_info.far_distance;
    g_effect_camera_info.near_distance = 10.0f;
    g_effect_camera_info.far_distance = distance;
    g_effect_camera_info.fade_distance = distance;
}


extern "C" {
// @address 0x7bf420 g_contrail_frame_shader_pair
// @address 0x7bf690 g_light_volume_gpu_update_shader

// @address 0x7bf724 g_light_volume_gpu_render_shader

}

// @address 0x7bf4b4 g_effect_shader_cache
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
s_gpu_global_vertex_shader<c_light_volume_gpu::s_render_vertex_shader_constant_list> g_effect_shader_cache =
{
    0, { { 0, g_effect_shader_cache.constants.slots, c_light_volume_gpu::s_render_vertex_shader_constant_list::slot_count, 0, {} } },
    _vertex_type_light_volume
};
}
#endif
// @address 0x233230 ?effects_initialize_for_new_map@@YAXXZ
void effects_initialize_for_new_map(void)
{
    int particle_limits_definition_index;
    s_data_array *effect_data;
    s_data_array *event_data;
    s_data_array *effect_location_data;
    s_data_array *effect_lightprobe_data;
    int reset_count;
    s_effect_counts *effect_counts;
    long *deterministic_location_count_address;
    particle_limits_definition_index = global_game_globals->particle_system_limits.index;
    if (particle_limits_definition_index != -1) {
        g_effect_budget_limits = (s_particle_system_limits_owner *)global_tag_instances
            [datum_index_to_absolute_index(particle_limits_definition_index)].definition;
    } else {
        g_effect_budget_limits = 0;
    }
    effect_data = ::effect_data;
    effect_data->valid = 1;
    data_delete_all(effect_data);
    event_data = g_event_data;
    event_data->valid = 1;
    data_delete_all((s_data_array *)event_data);
    effect_location_data = (s_data_array *)g_effect_location_data;
    effect_location_data->valid = 1;
    data_delete_all((s_data_array *)effect_location_data);
    effect_lightprobe_data = (s_data_array *)g_effect_lightprobe_slot_data;
    effect_lightprobe_data->valid = 1;
    data_delete_all((s_data_array *)effect_lightprobe_data);
    c_particle_system::initialize_for_map();
    c_contrail_system::initialize_for_map();
    particle_initialize_for_new_map();
    c_light_volume_system::initialize_for_map();
    reset_count = 0;
    effect_counts = g_effect_counts;
    g_effect_render_message_count = reset_count;
    effect_counts[1].effect_count = 0;
    effect_counts[0].effect_count = 0;
    effect_counts[1].event_count = 0;
    effect_counts[0].event_count = 0;
    deterministic_location_count_address = &effect_counts[1].location_count;
    *deterministic_location_count_address = 0;
    g_effect_location_warning_count = reset_count;
    effect_counts[0].location_count = 0;
}
// @address 0x233310 ?effects_dispose_from_old_map@@YAXXZ
void effects_dispose_from_old_map(void)
{
 ((s_data_array *)g_light_volume_gpu_row_data)->valid = 0;
 ((s_data_array *)g_light_volume_gpu_data)->valid = 0;
 g_light_volume_gpu_update_shader.vertex_shader.shader_reference = 0;
 g_light_volume_gpu_update_shader.pixel_shader.shader_reference = 0;
 g_light_volume_gpu_render_shader.shader_reference = 0;
 ((s_data_array *)g_light_volume_data)->valid = 0;
 ((s_data_array *)g_light_volume_location_data)->valid = 0;
 ((s_data_array *)g_light_volume_system_data)->valid = 0;
 ((s_data_array *)g_contrail_gpu_link_data)->valid = 0;
 ((s_data_array *)g_contrail_chain_data)->valid = 0;
 g_contrail_frame_shader_pair.vertex_shader.shader_reference = 0;
 g_contrail_frame_shader_pair.pixel_shader.shader_reference = 0;
 g_effect_shader_cache.shader_reference = 0;
 ((s_data_array *)g_particle_instance_data)->valid = 0;
 g_particle_instance_location_data->valid = 0;
 ((s_data_array *)g_particle_instance_owner_data)->valid = 0;
 c_contrail_system::dispose_from_map();
 c_particle_system::dispose_from_map();
 (::effect_data)->valid = 0;
 g_effect_budget_limits = 0;
 (g_event_data)->valid = 0;
 ((s_data_array *)g_effect_location_data)->valid = 0;
 ((s_data_array *)g_effect_lightprobe_slot_data)->valid = 0;
 main_render_purge_effect_messages(true);
 g_effect_render_message_count = 0;
}

static __forceinline float effects_random_fraction(const char *name, bool deterministic, const effect_datum *effect)
{
    unsigned long *seed = effect_get_random_seed_address(deterministic, effect);
    random_seed_next(seed);
    return (float)(double)(*seed >> 16) * k_random_word_normalization;
}
// @address 0x235058 ?effect_random_translational_velocity@@YAXPBD_NPBUeffect_datum@@PBTreal_vector3d@@PAT2@4MMMKK@Z
void effect_random_translational_velocity(const char *name, bool deterministic, const effect_datum *effect,
    const real_vector3d *base_direction, real_vector3d *direction, real_vector3d *velocity, float minimum, float maximum, float angle,
    unsigned long mask_a, unsigned long mask_b)
{
    float speed = effect_real_random_range(name, deterministic, effect, minimum, maximum, mask_a, mask_b, 0);
    float scaled_angle = effects_scale_random_range_value(effect, angle, mask_a, mask_b, 2);
    angle = effects_random_fraction(name, deterministic, effect) * scaled_angle;
    *direction = *base_direction;
    if (angle != 0.0f) {
        real_vector3d axis;
        effect_random_direction3d(name, deterministic, effect, &axis);
        rotate_vector_about_axis(direction, &axis, (float)sin(angle), (float)cos(angle));
    }
    velocity->i = direction->i * speed;
    velocity->j = direction->j * speed;
    velocity->k = direction->k * speed;
}

// @address 0x239c80 ?effect_get_part_to_node@@YAXPBUeffect_part_definition@@PBUreal_matrix4x3@@PAU2@@Z
void effect_get_part_to_node(const effect_part_definition *part, const real_matrix4x3 *node_matrix, real_matrix4x3 *part_to_node)
{
    matrix4x3_rotation_from_angles(part_to_node, part->relative_orientation.yaw, part->relative_orientation.pitch, 0.0f);
    part_to_node->position = part->relative_offset;
    matrix4x3_compute_product(node_matrix, part_to_node, part_to_node);
}

// @address 0x239048 ?effect_update_object@@YA_NJ@Z
bool effect_update_object(long effect_index)
{
    effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    if (effect->flags.test(_effect_marked_for_deletion_bit) && effect->flags.test(_effect_pinged_bit)) {
        effect_refresh_particles(effect_index);
        if (effect->lightprobe_slot_index != -1 && !definition->flags.test(_effect_definition_force_use_own_lightprobe_bit))
            effect_send_sample_lightmap_message(effect_index);
        effect->flags.set(_effect_marked_for_deletion_bit, false);
    }
    effect->flags.set(_effect_pinged_bit, false);
    if (!effect->flags.test(_effect_marked_for_deletion_bit)) {
        if (effect->object_index != -1) {
            if (!object_try_and_get_and_verify_type(effect->object_index, 0xffffffff)) {
                effect_destroy(effect_index, false);
                effect = 0;
            }
            effect_update_location_from_object(effect);
            bool started = effect->flags.test(_effect_looping_bit) || effect->flags.test(_effect_ping_from_impact_bit);
            if (started) {
                long object_index = effect->object_index;
                bool value_valid = false;
                long root_object_index = -1;
                long parent_index = effect->object_index;
                while (parent_index != -1) {
                    root_object_index = parent_index;
                    parent_index = object_get(parent_index)->parent_object_index;
                }
                bool visible = false;
                if ((object_header_get(root_object_index)->flags & 1) != 0 && !object_is_hidden(object_index))
                    visible = true;
                if (visible) {
                    bool first_deterministic = true;
                    bool second_deterministic = true;
                    long first_function = effect->scale_a_function;
                    value_valid = first_function == 0 ||
                        object_get_function_value(
                            object_index, first_function, effect->definition_index, &effect->scale_a, &first_deterministic);
                    long second_function = effect->scale_b_function;
                    if (second_function != 0)
                        object_get_function_value(effect->object_index, second_function, effect->definition_index, &effect->scale_b, &second_deterministic);
                    if (definition->flags.test(_effect_definition_first_person_visibility_bit) && !(first_deterministic && second_deterministic)) {
                        effect_delete_internal(effect_index, true);
                        effect = 0;
                    }
                } else {
                    effect->scale_a = k_real_zero_storage;
                    effect->scale_b = k_real_zero_storage;
                }
                if (!value_valid) {
                    if (definition->flags.test(_effect_definition_delete_immediately_bit)) {
                        effect_delete_internal(effect_index, true);
                        effect = 0;
                    } else if (!effect->flags.test(_effect_stopping_bit)) {
                        effect_stop(effect_index, false);
                    }
                } else if (effect->flags.test(_effect_stopping_bit)) {
                    if (effect->flags.test(_effect_detach_bit)) {
                        effect_delete_internal(effect_index, true);
                        effect = 0;
                    } else {
                        effect_start(effect_index);
                    }
                }
            }
        }
    }
    return effect == 0;
}
static __forceinline void effects_first_person_weapon_get_worldspace_node_matrix(long user_index, long object_index, short node_index, real_matrix4x3 *matrix)
{
    s_first_person_weapon_user_data *first_person = &((s_first_person_weapon_user_data *)g_first_person_weapon_data)[user_index];
    long weapon_slot = first_person_weapon_get_weapon_slot(user_index, object_index);
    matrix4x3_compute_product(&first_person->camera_matrix, &first_person->weapons[weapon_slot].node_matrices[node_index], matrix);
}
static __forceinline short effects_real_node_index(short node_index)
{
    return node_index == -1 ? -1 : node_index & 0x7fff;
}
static __forceinline void effects_accelerate_scale(real_vector3d *vector, float scale)
{
    vector->i *= scale;
    vector->j *= scale;
    vector->k *= scale;
}
static __forceinline float effects_accelerate_normalize(real_vector3d *vector)
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (fabsf(length) >= k_effects_minimum_intensity)
        effects_accelerate_scale(vector, 1.0f / length);
    return length;
}
static __forceinline effect_location_datum *effects_accelerate_next_location(const effect_datum *effect, long *location_index)
{
    long next_location_index = effect_location_find_next_of_type(effect, *location_index, 0);
    effect_location_datum *location = 0;
    *location_index = next_location_index;
    if (next_location_index != -1) {
        location = effects_location_get(next_location_index);
        *location_index = location->next_location_index;
    }
    return location;
}
// @address 0x239928 effect_apply_accelerations
extern "C" void effect_apply_accelerations(effect_datum *effect, long event_index, float elapsed_time)
{
    if (elapsed_time <= 0.0f)
        return;
    long object_index = effect->object_index;
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    const effect_event_definition *event_definition = &((const effect_event_definition *)definition->events.address)[event->event_block_index];
    if (object_index == -1)
        return;
    const object_datum *object = object_get(object_index);
    const s_object_definition *object_definition = TAG_GET('obje', const s_object_definition, object->definition_index);
    for (long acceleration_index = 0; acceleration_index < event_definition->accelerations.count; acceleration_index++) {
        const effect_acceleration_definition *acceleration = &(
            (const effect_acceleration_definition *)event_definition->accelerations.address)[acceleration_index];
        short location_index = acceleration->location_index;
        if (location_index == -1)
            continue;
        long instance_index = effect->location_indices[location_index];
        effect_location_datum *location;
        while ((location = effects_accelerate_next_location(effect, &instance_index)) != 0) {
            if (!definition->flags.test(_effect_definition_first_person_visibility_bit))
                continue;
            real_point3d position;
            real_vector3d direction;
            if (location->node_index != 0xffff) {
                real_matrix4x3 node_matrix;
                effect_get_node_matrix(effect, (short)location->node_index, &node_matrix);
                float offset_x = node_matrix.scale * location->matrix.position.x;
                float offset_y = node_matrix.scale * location->matrix.position.y;
                float offset_z = node_matrix.scale * location->matrix.position.z;
                float forward_x = location->matrix.forward.i;
                float forward_y = location->matrix.forward.j;
                float forward_z = location->matrix.forward.k;
                position.x = (node_matrix.forward.i * offset_x + (node_matrix.up.i * offset_z + node_matrix.left.i * offset_y)) + node_matrix.position.x;
                position.y = (node_matrix.forward.j * offset_x + (node_matrix.up.j * offset_z + node_matrix.left.j * offset_y)) + node_matrix.position.y;
                position.z = (node_matrix.forward.k * offset_x + (node_matrix.up.k * offset_z + node_matrix.left.k * offset_y)) + node_matrix.position.z;
                direction.i = node_matrix.forward.i * forward_x + (node_matrix.up.i * forward_z + node_matrix.left.i * forward_y);
                direction.j = node_matrix.forward.j * forward_x + (node_matrix.up.j * forward_z + node_matrix.left.j * forward_y);
                direction.k = node_matrix.forward.k * forward_x + (node_matrix.up.k * forward_z + node_matrix.left.k * forward_y);
            } else {
                position = location->matrix.position;
                direction = location->matrix.forward;
            }
            real_vector3d velocity;
            ::_random_vector_in_cone3d((unsigned long *)(tls_random_seed()), (char const *)("effect-object-accelerate"),
                (char const *)("..\\effects\\effects.cpp"), 0x17cf, &direction, acceleration->minimum,
                acceleration->maximum, &velocity);
            effects_accelerate_normalize(&velocity);
            float scale = (float)game_time_globals->ticks_per_second * elapsed_time;
            effects_accelerate_scale(&velocity, scale * acceleration->magnitude * object_definition->acceleration_scale);
            object_set_damage_owner(effect->object_index, &effect->damage_owner, false);
            object_apply_acceleration(effect->object_index, -1, &position, &velocity, 0);
        }
    }
}

// @address 0x23afa0 ?effect_get_node_matrix@@YAXPBUeffect_datum@@FPAUreal_matrix4x3@@@Z
void effect_get_node_matrix(const effect_datum *effect, short node_index, real_matrix4x3 *matrix)
{
    if (node_index != -1 && (node_index & 0x8000) && effect->first_person) {
        effects_first_person_weapon_get_worldspace_node_matrix(
            effect->first_person_user_index, effect->object_index, effects_real_node_index(node_index), matrix);
    } else {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        unsigned char position_only = (definition->flags >> 4) & 1;
        if (position_only) {
            matrix->scale = 1.0f;
            matrix->rotation = global_identity4x3->rotation;
            matrix->position = object_get_node_matrix(effect->object_index, effects_real_node_index(node_index))->position;
        } else {
            memcpy(matrix, object_get_node_matrix(effect->object_index, effects_real_node_index(node_index)), sizeof(real_matrix4x3));
        }
    }
}

static __forceinline unsigned short effects_local_random()
{
    unsigned long *seed = get_local_random_seed_address();
    random_seed_next(seed);
    return (unsigned short)(*seed >> 16);
}
static __forceinline long game_looping_sound_new(long definition_index, long effect_index)
{
    long looping_sound_index = datum_new(game_looping_sound_data);
    if (looping_sound_index != -1) {
        game_looping_sound_datum *sound = datum_get<game_looping_sound_datum>(game_looping_sound_data, looping_sound_index);
        sound->type = 3;
        sound->state = 3;
        sound->flags.clear();
        sound->flags.set(9, true);
        sound->scale = 0.0f;
        sound->definition_index = definition_index;
        sound->object_index = -1;
        sound->location = -1;
        sound->timeout = effect_index;
        sound->flags.set(0, true);
    }
    return looping_sound_index;
}
// @address 0x234688 ?effect_start@@YAXJ@Z
void effect_start(long effect_index)
{
    c_render_batch batch;
    g_effect_damage_batch = &batch;
    effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    effect->flags.set(_effect_event_started_bit, false);
    effect->flags.set(_effect_stopping_bit, false);
    if (definition->seed == 0)
        effect->random_seed = effects_local_random();
    else
        effect->random_seed = definition->seed;
    if (definition->flags.test(_effect_definition_parallel_events_bit)) {
        effect_restart_all_events(effect_index);
    } else {
        long next_event_index = effect_get_next_event_index(effect);
        if (next_event_index < definition->events.count)
            effect_set_event(effect_index, (short)next_event_index);
        else
            effect_delete_internal(effect_index, true);
    }
    long sound_definition_index = definition->sound.index;
    if (sound_definition_index != -1 && definition->location != -1 && !effect->flags.test(_effect_no_sound_bit) && effect->looping_sound_index == -1) {
        long location_index = effect->location_indices[definition->location];
        effect_location_datum *location = effect_location_get_next_instance(effect, &location_index, 0);
        if (location)
            effect->looping_sound_index = game_looping_sound_new(sound_definition_index, effect_index);
    }
    g_effect_damage_batch = 0;
    effect->flags.set(_effect_event_started_bit, true);
    effect_damage_batch_flush(&batch);
}

static __forceinline effect_location_datum *effects_next_location_instance(const effect_datum *effect, long *location_index, short type)
{
    *location_index = effect_location_find_next_of_type(effect, *location_index, type);
    effect_location_datum *location = 0;
    if (*location_index != -1) {
        location = effects_location_get(*location_index);
        *location_index = location->next_location_index;
    }
    return location;
}
// @address 0x235f80 effect_get_first_location_matrix
extern "C" const real_matrix4x3 *effect_get_first_location_matrix(const effect_datum *effect, real_matrix4x3 *matrix)
{
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    effect_location_datum *location = 0;
    for (long location_block_index = 0; location_block_index < definition->locations.count; location_block_index++) {
        long location_index = effect->location_indices[location_block_index];
        if (location_index != -1) {
            location = effects_next_location_instance(effect, &location_index, 0);
            break;
        }
    }
    if (location) {
        if (location->node_index != 0xffff && effect->object_index != -1) {
            real_matrix4x3 node_matrix;
            effect_get_node_matrix(effect, (short)location->node_index, &node_matrix);
            matrix4x3_compute_product(&node_matrix, &location->matrix, matrix);
            return matrix;
        }
        return &location->matrix;
    }
    return 0;
}
// @address 0xbab88 ?scenario_location_from_point@@YAXPAUs_location@@PBTreal_point3d@@@Z
// @address 0x236068 ?effect_get_location_from_object@@YA_NPAUeffect_datum@@@Z
bool effect_get_location_from_object(effect_datum *effect)
{
    real_matrix4x3 matrix;
    const real_matrix4x3 *location_matrix = effect_get_first_location_matrix(effect, &matrix);
    if (location_matrix) {
        scenario_location_from_point(&effect->location, &location_matrix->position);
    } else {
        location_invalidate(&effect->location);
    }
    return effect->cluster_reference.bsp_index != -1;
}

// @address 0x2360e0 ?effect_get_transformed_location@@YAPBUreal_matrix4x3@@PBUeffect_datum@@PBUeffect_location_datum@@PAU1@@Z
const real_matrix4x3 *effect_get_transformed_location(const effect_datum *effect, const effect_location_datum *location, real_matrix4x3 *matrix)
{
    if (location->node_index != 0xffff && effect->object_index != -1) {
        real_matrix4x3 node_matrix;
        effect_get_node_matrix(effect, (short)location->node_index, &node_matrix);
        matrix4x3_compute_product(&node_matrix, &location->matrix, matrix);
        return matrix;
    } else {
        return &location->matrix;
    }
}
static __forceinline const real_matrix4x3 *effects_first_person_weapon_get_camera_relative_node_matrix(long user_index, long object_index, short node_index)
{
    long weapon_slot = first_person_weapon_get_weapon_slot(user_index, object_index);
    s_first_person_weapon_user_data *first_person = &((s_first_person_weapon_user_data *)g_first_person_weapon_data)[user_index];
    s_first_person_weapon_data *weapon = &first_person->weapons[weapon_slot];
    return &weapon->node_matrices[node_index];
}

static __forceinline void effects_build_location(const effect_datum *effect, short node_index, real_matrix4x3 *matrix, bool first_person)
{
    if (first_person && node_index != -1 && (node_index & 0x8000) && effect->first_person) {
        memcpy(matrix,
            effects_first_person_weapon_get_camera_relative_node_matrix(
                effect->first_person_user_index, effect->object_index, effects_real_node_index(node_index)),
            sizeof(real_matrix4x3));
    } else {
        effect_get_node_matrix(effect, node_index, matrix);
    }
}
// @address 0x236150 ?effect_get_transformed_location_world_or_local@@YAPBUreal_matrix4x3@@PBUeffect_datum@@PBUeffect_location_datum@@PAU1@_N@Z
const real_matrix4x3 *effect_get_transformed_location_world_or_local(
    const effect_datum *effect, const effect_location_datum *location, real_matrix4x3 *matrix, bool first_person)
{
    const real_matrix4x3 *result = 0;
    bool built = false;
    if (location->node_index != 0xffff && effect->object_index != -1) {
        real_matrix4x3 node_matrix;
        effects_build_location(effect, (short)location->node_index, &node_matrix, first_person);
        matrix4x3_compute_product(&node_matrix, &location->matrix, matrix);
        result = matrix;
        built = true;
    }
    if (!built)
        result = &location->matrix;
    return result;
}


// @address 0x8c43ac g_particle_default_used_states
#if !defined(_XBOX) || defined(__clang__)
extern "C" {
unsigned long g_particle_default_used_states;
}
#endif

// @address 0x2369a0 ?effect_update_particle_locations@@YAXPBUeffect_datum@@PAVc_particle_system@@PBUreal_matrix4x3@@@Z
void effect_update_particle_locations(const effect_datum *effect, c_particle_system *particle_system, const real_matrix4x3 *matrix)
{
    const c_particle_system_definition *definition = particle_system->get_particle_system_definition();
    short camera_mode = definition->camera_mode;
    bool coordinate_system = definition->m_coordinate_system != 0;
    c_particle_state_list state_list;
    state_list.set_particle_system(particle_system);
    long location_block_index = definition->location_index;
    if (location_block_index != -1) {
        long particle_location_index = particle_system->get_first_location_index();
        long previous_location_index = -1;
        long location_index = effect_location_find_next_of_type(effect, effect->location_indices[location_block_index], camera_mode);
        while (location_index != -1) {
            const effect_location_datum *location = effects_location_get(location_index);
            if (particle_location_index == -1)
                particle_location_index = particle_system->add_particle_location();
            if (particle_location_index != -1) {
                ::c_particle_location *particle_location = datum_get<c_particle_location>(g_particle_location_data_array, particle_location_index);
                long render_location_index = location->first_render_location_index;
                while (render_location_index != -1) {
                    const s_effect_render_location *render_location = &g_effect_render_locations[render_location_index];
                    if (TEST_BIT(render_location->flags.get(), 0)) {
                        const real_matrix4x3 *render_matrix = (const real_matrix4x3 *)render_location->data;
                        particle_location->state &= ~0x10;
                        particle_location->set_matrix(render_matrix);
                        state_list.update_particle_location(particle_location);
                        int particle_count = particle_location_update(particle_location, 0.0f, &state_list);
                        particle_system->m_count += particle_count;
                    }
                    render_location_index = render_location->next_index;
                }
                real_matrix4x3 location_matrix;
                particle_location->set_matrix(matrix
                        ? matrix
                        : effect_get_transformed_location_world_or_local(effect, location, &location_matrix, coordinate_system));
                particle_location->set_first_person(location->node_index != 0xffff && (location->node_index & 0x8000));
                if ((_rotl(particle_location->state, 28) & 1) == 0) {
                    state_list.set_particle_location(particle_location);
                    int particle_count = particle_location_update(particle_location, 0.0f, &state_list);
                    particle_system->m_count += particle_count;
                }
                previous_location_index = particle_location_index;
                particle_location_index = particle_location->get_next_location();
            }
            location_index = effect_location_find_next_of_type(effect, location->next_location_index, camera_mode);
        }
        if (particle_location_index != -1)
            particle_system->remove_further_particle_locations(previous_location_index);
    }
}

// @address 0x122888 effect_spawn_impact_effects
extern "C" int effect_spawn_impact_effects(const long *effect_indices, const real_point3d *position, const real_vector3d *normal,
    const real_vector3d *direction, const s_location *location, const real_plane3d *plane)
{
    int result = 0;
    const long *effect = &effect_indices[3];
    real_point3d point;
    point.x = direction->i * 0.01f + position->x;
    point.y = direction->j * 0.01f + position->y;
    point.z = direction->k * 0.01f + position->z;
    int count = 3;
    do {
        long effect_index = *effect;
        if (effect_index != -1 && g_restricted_address[1] != 0) {
            effect_new_unattached_from_point(effect_index, (char *)&point, (char *)normal, (char *)direction, 0, 0, (char *)plane, (char *)location);
            result = 1;
        }
        --count;
        ++effect;
    } while ((unsigned int)count != 0);
    return result;
}
// @address 0x2374e8 ?event_request_light_volume_system_location@@YAXJJPBUeffect_part_definition@@PBUreal_matrix4x3@@_N@Z
void event_request_light_volume_system_location(
    long effect_index, long event_index, const effect_part_definition *part, const real_matrix4x3 *matrix, bool local_space)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    if (!event->flags.test(_event_stale_bit)) {
        real_vector3d velocity;
        real_vector3d direction;
        effect_random_translational_velocity("event-request-light_volume-system", false, effect, &matrix->forward, &direction, &velocity,
            part->minimum_velocity, part->maximum_velocity, part->velocity_angle, part->scale_a_mask, part->scale_b_mask);
        s_effect_message message(effect_index, event_index);
        message.type = _effect_message_add_contrail_profile_location;
        message.limit_type = part->limit_type;
        memcpy(&message.light_volume.matrix, matrix, sizeof(real_matrix4x3));
        message.light_volume.velocity = velocity;
        message.light_volume.random_seed =
            (unsigned long)(effect_real_random_range("event-request-light_volume-system", false, effect, 0.0f, 1.0f) * 4294967296.0f);
        message.location_index = 0;
        effect_send_render_message(&message);
    }
}

// @address 0x23d658 ?effect_attachment_update_position@@YA_NPAUeffects_node_attachment@@@Z
bool effect_attachment_update_position(effects_node_attachment *attachment)
{
    bool result = true;
    if (attachment->type == 1) {
        result = object_try_and_get_and_verify_type(attachment->object_index, 0xffffffff) != 0;
        if (result)
            matrix4x3_transform_point(object_get_node_matrix(attachment->object_index, attachment->node_index),
                &attachment->local_point, &attachment->world_point);
    }
    return result;
}

// @address 0x23cd08 ?try_to_append_message_queue@?$t_message_queue@Us_effect_message@@$0CAA@@@QAAJPAV1@@Z
template<> long t_message_queue<s_effect_message, 512>::try_to_append_message_queue(t_message_queue *source)
{
    long count = source->message_count.peek();
    long failed = 0;
    for (long index = 0; index < count; index++) {
        if (source->in_use.test_bit(index) && !try_add_message(&source->messages[index]))
            failed++;
    }
    return failed;
}

static __forceinline long effects_prepend_message_queue(t_message_queue<s_effect_message, 512> *queue, t_message_queue<s_effect_message, 512> *source)
{
    long count = source->message_count.peek();
    long failed = source->try_to_append_message_queue(queue);
    queue->clear();
    failed += queue->try_to_append_message_queue(source);
    source->message_count.set(count);
    return failed;
}
// @address 0x23c530 ?adopt_old_effect_render_message_queue@@YAXPAX@Z
void adopt_old_effect_render_message_queue(void *allocation)
{
    effects_prepend_message_queue((t_message_queue<s_effect_message, 512> *)g_effect_message_queue, (t_message_queue<s_effect_message, 512> *)allocation);
}

// @address 0x238408 effect_refresh_particles
extern "C" void effect_refresh_particles(long effect_index)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    s_effect_message message(effect_index, effect->event_index, definition->limit_type);
    message.type = _effect_message_refresh_particles;
    effects_add_render_message(&message);
    effect_generate_lens_flares(effect_index, true);
}

// @address 0x238510 event_refresh_particles
extern "C" void event_refresh_particles(long effect_index, long event_index)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    s_effect_message message(effect_index, event_index, ((const effect_event_definition *)definition->events.address)[event->event_block_index].limit_type);
    message.type = _effect_message_refresh_event_particles;
    unsigned long count = g_effect_render_message_count + 1;
    if (count <= g_effect_budget_limits->table->blocks[4].limits[message.limit_type].maximum_count) {
        g_effect_render_message_count = count;
        ((t_message_queue<s_effect_message, 512> *)g_effect_message_queue)->try_add_message(&message);
    }

}

// @address 0x238618 ?effect_send_sample_lightmap_message@@YAXJ@Z
void effect_send_sample_lightmap_message(long effect_index)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    s_effect_message message(effect_index, effect->event_index, definition->limit_type);
    message.type = _effect_message_sample_lightmap;
    effects_add_render_message(&message);
}

// @address 0x23b668 ?effect_update_attached_sound@@YA_NJPATreal_point3d@@PATreal_vector3d@@PAM@Z
bool effect_update_attached_sound(long effect_index, real_point3d *point, real_vector3d *vector, float *scale)
{
    bool result = false;
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    if (effect) {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        if (definition->location != -1) {
            long location_index = effect->location_indices[definition->location];
            effect_location_datum *location = effect_location_get_next_instance(effect, &location_index, 0);
            if (location) {
                real_matrix4x3 matrix;
                const real_matrix4x3 *transform = effect_get_transformed_location(effect, location, &matrix);
                *point = transform->position;
                // match: 0x23b668: pointer __restrict qualifier prevents alias reload
                real_vector3d *__restrict forward = vector;
                *forward = transform->forward;
                if (definition->sound_scale_source != 0xff)
                    *scale = effect->scale_a;
                else
                    *scale = 1.0f;
                result = true;
            }
        }
    }
    return result;
}

static __forceinline effect_location_datum *effects_handle_deleted_next_location(const effect_datum *effect, long *location_index)
{
    long next_location_index = effect_location_find_next_of_type(effect, *location_index, 0);
    effect_location_datum *location = 0;
    *location_index = next_location_index;
    if (next_location_index != -1) {
        location = effects_location_get(next_location_index);
        *location_index = location->next_location_index;
    }
    return location;
}
static __forceinline void effects_handle_deleted_localize_location(const effect_datum *effect, effect_location_datum *location)
{
    if (location->node_index != 0xffff && effect->object_index != -1) {
        real_matrix4x3 node_matrix;
        effect_get_node_matrix(effect, (short)location->node_index, &node_matrix);
        matrix4x3_compute_product(&node_matrix, &location->matrix, &location->matrix);
    }
}
// @address 0xf7998 ?effect_handle_deleted_object@@YAXJ@Z
void effect_handle_deleted_object(long effect_index)
{
    effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    if (effect->flags.test(_effect_stopping_bit)) {
        effect->flags.set(_effect_marked_for_deletion_bit, true);
        effect->death_delay = definition->death_delay;
    } else {
        effect->flags.set(_effect_object_deleted_bit, true);
    }
    for (long index = 0; index < definition->locations.count; index++) {
        long location_index = effect->location_indices[index];
        effect_location_datum *location;
        while ((location = effects_handle_deleted_next_location(effect, &location_index)) != 0) {
            effects_handle_deleted_localize_location(effect, location);
            // match: 0xf7998: short pun keeps li -1 + sth; plain -1/0xffff folds to lis+ori+sth with an RA cascade.
            *(short *)&location->node_index = -1;
        }
    }
    effect_get_location_from_object(effect);
    effect->flags.set(_effect_first_person_bit, false);
    effect->first_person_user_flags = 0;
    effect->first_person = false;
    effect->first_person_user_index = 0;
    effect->object_index = -1;
}

static __forceinline bool effects_status_attachment_is_valid(const effect_datum *effect)
{
    bool valid = effect->attachment_index == -1 || effects_particle_try_and_get(effect->attachment_index) != 0;
    return valid;
}
static __forceinline bool effects_status_flag_test(unsigned long flags, long bit)
{
    bool result = ((flags >> bit) & 1) != 0;
    return result;
}
static __forceinline e_event_status effects_get_event_status_internal(long effect_index, long event_block_index, unsigned short event_counter, float *fraction)
{
    if (effect_index != -1) {
        const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
        unsigned long flags = effect->flags;
        bool enabled =
            (!(effects_status_flag_test(flags, 5) || effects_status_flag_test(flags, 1)) && effects_status_attachment_is_valid(effect)) ||
            effects_status_flag_test(flags, 12);
        const event_datum *event = effect_get_event(effect, event_block_index);
        if (event->event_block_index != event_block_index || event->event_counter != event_counter) {
            *fraction = 1.0f;
            return _event_status_invalid;
        }
        bool may_finish = !effects_status_flag_test(flags, _effect_cannot_be_restarted_bit);
        bool active = event->flags.test(_event_started_bit) && !event->flags.test(_event_finished_bit);
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x146870: the 0.0f is read at +0x320 off the k_real_constant_pool base (lis/addi/lfs); a literal is one lis/lfs and shifts the function
        *fraction = event->flags.test(_event_started_bit) ? event->time / event->duration : k_real_constant_pool[200];
#else
        *fraction = event->flags.test(_event_started_bit) ? event->time / event->duration : 0.0f;
#endif
        if (enabled && active)
            return _event_status_active;
        return may_finish ? _event_status_finished : _event_status_invalid;
    }
    *fraction = 1.0f;
    return _event_status_invalid;
}
// @address 0x146870 ?effect_get_event_status@@YA?AW4e_event_status@@JJGPAM@Z
e_event_status effect_get_event_status(long effect_index, long event_block_index, unsigned short event_counter, float *fraction)
{
    return effects_get_event_status_internal(effect_index, event_block_index, event_counter, fraction);
}

// @address 0x112048 ?effects_handle_deleted_object@@YAXJ@Z
void effects_handle_deleted_object(long object_index)
{
    for (long effect_index = ::data_next_index(effect_data, -1); effect_index != -1;
        effect_index = inl_data_next_index(::effect_data, effect_index))
    {
        effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
        if (effect->object_index == object_index)
            effect_handle_deleted_object(effect_index);
    }
}

// @address 0xf7ef0 effect_update_particle_query_record
extern "C" void effect_update_particle_query_record(c_particle_system *record)
{
    s_data_array *effect_data = ::effect_data;
    long effect_index = record->m_effect_index;
    if (effect_index != -1) {
        unsigned short absolute_index = (unsigned short)effect_index;
        if (absolute_index < effect_data->first_unallocated) {
            effect_datum *effect = (effect_datum *)(effect_data->data + effect_data->size * absolute_index);
            short identifier = effect->identifier;
            if (identifier != 0 && identifier == (effect_index >> 16)) {
                if (!effects_cluster_visibility_valid(effect))
                    return;
                bool has_particle = effect->attachment_index == -1 || effects_particle_try_and_get(effect->attachment_index) != 0;
                if (!has_particle)
                    return;
                long first_person = (signed char)effect->first_person | (signed char)effect->first_person_user_flags;
                record->m_lightprobe_index = effect->lightprobe_slot_index;
                record->m_effect_scale_a = effect->scale_a;
                record->m_effect_scale_b = effect->scale_b;
                record->m_player_mask = (unsigned short)first_person;
                record->m_location = effect->location;
                const void *matrix = 0;
                if (record->m_breakable_surface_event_index != -1) {
                    s_breakable_surfaces_broken_event *attached =
                        (s_breakable_surfaces_broken_event *)datum_try_and_get((const s_data_array *)g_breakable_surface_event_data,
                        record->m_breakable_surface_event_index);
                    if (attached) {
                        short cluster_index = attached->variant_index;
                        if (cluster_index == -1) {
                            matrix = global_identity4x3;
                        } else {
                            signed char bsp_index = attached->bsp_index;
                            s_tag_block *bsps = &global_scenario->structure_bsps;
                            scenario_structure_bsp_reference *bsp = &((scenario_structure_bsp_reference *)bsps->address)[bsp_index];
                            const char *definition = TAG_GET('sbsp', const char, bsp->structure_bsp.index);
                            const structure_instanced_geometry_instance *instances =
                                (const structure_instanced_geometry_instance *)((const structure_bsp *)definition)->instanced_geometry_instances.address;
                            matrix = &instances[cluster_index].transform;
                        }
                    }
                }
                const event_datum *event = effect_get_event(effect, record->m_def_index);
                if (((_rotl(effect->flags, 20) & 1) != 0) || TEST_BIT(event->flags, 3)) {
                    if (event->event_counter == record->m_location_index) {
                        long particle_index = record->m_first_location;
                        if (particle_index != -1) {
                            do {
                                ::c_particle_location *particle =
                                    datum_get<c_particle_location>(g_particle_location_data_array, particle_index);
                                particle_index = particle->next_location_index;
                                particle->state &= ~0x10;
                            } while (particle_index != -1);
                        }
                        record->m_flags = (record->m_flags & 0xFF8D) | 0x70;
                    }
                }
                effect_update_particle_locations(effect, (c_particle_system *)record, (const real_matrix4x3 *)matrix);
                return;
            }
        }
    }
    record->m_effect_index = -1;
}
// @address 0xf75e8 effect_get_reuse_mode
extern "C" long effect_get_reuse_mode(const s_effect_creation_data *creation_data, long effect_index)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    if (creation_data->definition_index == effect->definition_index && effect->object_index == creation_data->structure_bsp_index) {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, creation_data->definition_index);
        real_matrix4x3 matrix;
        const real_matrix4x3 *location = effect_get_first_location_matrix(effect, &matrix);
        if (location) {
            float distance_squared = distance_squared3d(&location->position, creation_data->position);
            if (!(distance_squared >= definition->minimum_separation * definition->minimum_separation))
                return 1;
            if (distance_squared < definition->maximum_separation * definition->maximum_separation)
                return 2;
        }
    }
    return 0;
}

// @address 0x2349b8 ?effect_destroy@@YAXJ_N@Z
void effect_destroy(long effect_index, bool delete_messages)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, effect_index);
    if (effect) {
        bool deterministic = (effect->flags >> 14) & 1;
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        if (delete_messages) {
            t_message_queue<s_effect_message, 512> *queue = (t_message_queue<s_effect_message, 512> *)g_effect_message_queue;
            long message_count = queue->message_count.peek();
            for (long index = 0; index < message_count; index++) {
                if (queue->in_use.test_bit(index) && queue->messages[index].effect_index == effect_index)
                    queue->in_use.set_bit(index, false);
            }
        }
        effect_detach(effect_index);
        if (effect->looping_sound_index != -1) {
            effect_looping_sound_stop(effect->looping_sound_index);
            effect->looping_sound_index = -1;
        }
        ::effect_delete_locations(effect_index);
        if (effect->lightprobe_slot_index != -1) {
            s_effect_lightprobe_slot *slot =
                datum_get<s_effect_lightprobe_slot>(g_effect_lightprobe_slot_data, effect->lightprobe_slot_index);
            slot->effect_index = -1;
            slot->weight = definition->lightprobe_fade_time;
        }
        const effect_datum *current = datum_get<effect_datum>(::effect_data, effect_index);
        event_destroy(current->event_index, (current->flags >> 14) & 1);
        ::datum_delete(effect_data, effect_index);
        g_effect_counts[deterministic].effect_count--;
    }
}


static __forceinline bool effects_allocate_events(long effect_index)
{
    effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    effect->event_index = event_allocate(TAG_GET('effe', const s_effect_definition, effect->definition_index));
    return effect->event_index != -1;
}
// @address 0x2351e0 ?effect_allocate@@YAJPBUs_effect_creation_data@@@Z
long effect_allocate(const s_effect_creation_data *creation_data)
{
    long effect_index = -1;
    if (creation_data->definition_index != -1) {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, creation_data->definition_index);
        if (((bool)(_rotl(creation_data->flags, 30) & 1)) || !definition->flags.test(_effect_definition_first_person_visibility_bit)) {
            if (definition->events.count > 0) {
                bool deterministic = (bool)(_rotl(definition->flags, 23) & 1);
                s_effect_counts *counts = &g_effect_counts[deterministic];
                const s_effect_limit_block *limit_blocks = g_effect_budget_limits->table->blocks;
                if ((unsigned long)(counts->effect_count + 1) <= limit_blocks[g_effect_limit_count_index].limits[definition->limit_type].maximum_count) {
                    effect_index = datum_new_in_range(::effect_data, deterministic ? 0 : 320, 320, !deterministic);
                    counts->effect_count++;
                    g_effect_limit_reached[definition->limit_type] = true;
                    effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
                    datum_get<effect_datum>(::effect_data, effect_index)->definition_index = creation_data->definition_index;
                    effect->damage_owner = creation_data->damage_owner;
                    effect->damage_reporting_type = creation_data->damage_reporting_type;
                    effect->looping_sound_index = -1;
                    effect->object_index = -1;
                    effect->attachment_index = -1;
                    effect->first_person_user_flags = 0;
                    effect->first_person = false;
                    effect->flags.clear();
                    effect->first_person_user_index = -1;
                    effect->flags.set(_effect_deterministic_bit, deterministic);
                    effect->lightprobe_slot_index = -1;
                    effect->impact_coordinates.x = 0.0f;
                    effect->impact_coordinates.y = 0.0f;
                    if (creation_data->plane) {
                        effect->plane = *creation_data->plane;
                        effect->flags.set(_effect_has_plane_bit, true);
                    }
                    for (long index = 0; index < 8; index++) effect->location_indices[index] = -1;
                    effect_allocate_lightprobe_slot(effect, creation_data);
                    if (!effects_allocate_events(effect_index)) {
                        effect_destroy(effect_index, true);
                        effect_index = -1;
                    }
                } else g_effect_limit_reached[definition->limit_type] = false;
            }
        }
    }
    return effect_index;

}

// @address 0x239ce8 effect_get_part_to_world_matrix
extern "C" void effect_get_part_to_world_matrix(
    const effect_datum *effect, const effect_part_definition *part, const effect_location_datum *location, real_matrix4x3 *part_to_world)
{
    unsigned short node_index = location->node_index;
    effect_get_part_to_node(part, &location->matrix, part_to_world);
    if (node_index != 0xffff && effect->object_index != -1) {
        real_matrix4x3 node_matrix;
        effect_get_node_matrix(effect, (short)node_index, &node_matrix);
        matrix4x3_compute_product(&node_matrix, part_to_world, part_to_world);
    }
    if (part->flags & 1) {
        part_to_world->forward = *global_down3d;
        part_to_world->up = *global_forward3d;
    }
}

// @address 0x1460c0 effect_new_contrail_profile
extern "C" long effect_new_contrail_profile(unsigned char limit_type, c_tag_index effect_index, long definition_index,
    unsigned char event_block_index, long event_counter, long unused, unsigned char render_type)
{
    long profile_index = -1;
    s_data_array *data = (s_data_array *)g_particle_instance_owner_data;
    if ((unsigned long)(data->actual_count + 1) <= g_effect_budget_limits->table->blocks[17].limits[limit_type].maximum_count) {
        profile_index = datum_new(data);
        s_effect_contrail_profile *profile = datum_get<s_effect_contrail_profile>(g_particle_instance_owner_data, profile_index);
        profile->age = 0.0f;
        profile->lifespan = 0.0f;
        profile->effect_index = effect_index;
        profile->interpolation = 0.0f;
        profile->limit_type = limit_type;
        profile->definition_index = definition_index;
        profile->event_block_index = event_block_index;
        profile->event_counter = event_counter;
        profile->location_index = -1;
        profile->render_type = render_type;
        profile->active_index = -1;
        profile->references = 0;
        profile->flags = (profile->flags & ~15) | 7;
        g_effect_contrail_profile_limit_reached[limit_type] = true;
    } else {
        g_effect_contrail_profile_limit_reached[limit_type] = false;
    }
    return profile_index;
}
// @address 0x1461c0 effect_contrail_profile_add_location
extern "C" long effect_contrail_profile_add_location(s_effect_contrail_profile *profile, s_effects_contrail_spawn_parameters *spawn, long seed)
{
    long node_index = particle_new_location_from_spawn(profile->limit_type,
        (profile - (s_effect_contrail_profile *)((s_data_array *)g_particle_instance_owner_data)->data) | ((unsigned short)profile->identifier << 16),
        (s_particle_spawn_parameters *)spawn, seed);
    if (node_index != -1) {
        long head_index = profile->location_index;
        if (head_index == -1) {
            profile->location_index = node_index;
            return node_index;
        }
        char *nodes = ((s_data_array *)g_particle_instance_location_data)->data;
        s_effect_contrail_location *head =
            (s_effect_contrail_location *)(nodes + DATUM_INDEX_TO_ABSOLUTE_INDEX(head_index) * sizeof(s_effect_contrail_location));
        long next_index = head->next_index;
        if (next_index == -1) {
            head->next_index = node_index;
        } else {
            particle_location_append(
                (s_effect_contrail_location *)(nodes + DATUM_INDEX_TO_ABSOLUTE_INDEX(next_index) * sizeof(s_effect_contrail_location)), node_index);
        }
    }
    return node_index;
}
// @address 0x232ce8 ?__tls_set_g_contrail_location_allocator@@YAXPAX@Z
// @address 0x232d10 ?__tls_set_g_player_control_globals_deterministic_allocator@@YAXPAX@Z
// @address 0x232ca0 ?__tls_set_g_prop_data_allocator@@YAXPAX@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0x2b46d8 ?dispose_from_map@c_contrail_system@@SAXXZ
// @address 0x2b3fc0 ?dispose_from_map@c_particle_system@@SAXXZ
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x116558 ?matrix4x3_rotation_from_angles@@YAXPAUreal_matrix4x3@@MMM@Z
// @address 0xade58 ?matrix4x3_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z
// @address 0x88cd0 ?observer_try_and_get_camera@@YAPBUs_observer_result@@J@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x9b278 ?rotate_vector_about_axis@@YAPATreal_vector3d@@PAT1@PBT1@MM@Z
// @address 0x473b48 ?first_person_weapon_get_weapon_slot@@YAJJJ@Z
static __forceinline long effects_attachment_next_index(s_data_array *array, long index)
{
    long next_absolute_index = ::data_next_absolute_index(array, (unsigned short)(index + 1));
    long next_index = -1;
    if (next_absolute_index != -1) {
        unsigned short identifier = *(unsigned short *)(array->data + array->size * next_absolute_index);
        next_index = (identifier << 16) | next_absolute_index;
    }
    return next_index;
}
// @address 0x116680 effect_update_attachments
extern "C" void effect_update_attachments(float delta_time)
{
    for (long attachment_index = ::data_next_index(g_static_lighting_effect_attachments, -1); attachment_index != -1;
        attachment_index = effects_attachment_next_index(g_static_lighting_effect_attachments, attachment_index))
    {
        bool keep = false;
        effects_node_attachment *attachment = datum_get<effects_node_attachment>(g_static_lighting_effect_attachments, attachment_index);
        attachment->age += delta_time;
        const render_objects_static_lighting_screen_effect_definition *definition =
            (const render_objects_static_lighting_screen_effect_definition *)global_tag_instances
                [datum_index_to_absolute_index(attachment->definition_index)].definition;
        for (long part_index = 0; part_index < definition->count; part_index++) {
            const render_objects_static_lighting_screen_effect *part = definition->effects + part_index;
            if (part->response.lifespan <= 0.0f || attachment->age <= part->response.lifespan)
                keep = true;
        }
        if (keep)
            keep = effect_attachment_update_position(attachment);
        if (!keep)
            ::datum_delete(g_static_lighting_effect_attachments, attachment_index);
    }
}


// @address 0xc1510 ?game_test_cluster_activation@@YA_NPBUs_cluster_reference@@@Z
// @address 0x238ed8 effect_update_location_from_object
extern "C" void effect_update_location_from_object(effect_datum *effect)
{
    long object_index = effect->object_index;
    long root_object_index = -1;
    while (object_index != -1) {
        root_object_index = object_index;
        object_index = object_get(object_index)->parent_object_index;
    }
    const object_datum *object = ((s_object_header *)object_header_data->data)[(unsigned short)root_object_index].datum;
    if (_rotl(object->flags, 24) & 1) {
        effect->velocity = object->linear_velocity;
        effect->location = object->location;
        if (!game_test_cluster_activation(&effect->cluster_reference)) {
            s_cluster_reference_iterator iterator;
            s_cluster_reference cluster = objects_cluster_reference_get_first(&iterator, root_object_index);
            while (cluster.bsp_index != -1) {
                if (game_test_cluster_activation(&cluster)) {
                    effect->cluster_reference = cluster;
                    break;
                }
                cluster = objects_cluster_reference_get_next(&iterator, root_object_index);
            }
        }
    } else {
        const long invalid_location = -1;
        memcpy(&effect->location, &invalid_location, sizeof(effect->location));
    }
}

typedef char effect_marker_list_matrix_offset[(offsetof(effect_marker_list, node_matrix) == 4) ? 1 : -1];
typedef char effect_marker_list_size[(sizeof(void *) != 4 || ((sizeof(effect_marker_list) == 0x40))) ? 1 : -1];
static __forceinline void effects_inverse_transform_vector(const real_matrix4x3 *matrix, const real_vector3d *vector, real_vector3d *result)
{
    float i = vector->i;
    float j = vector->j;
    float k = vector->k;
    result->i = matrix->forward.i * i + matrix->forward.j * j + matrix->forward.k * k;
    result->j = matrix->left.i * i + matrix->left.j * j + matrix->left.k * k;
    result->k = matrix->up.i * i + matrix->up.j * j + matrix->up.k * k;
}
static __forceinline float effects_point_plane_distance(const real_plane3d *plane, const real_point3d *point)
{
    return point->x * plane->normal.i + point->y * plane->normal.j + point->z * plane->normal.k - plane->d;
}
static __forceinline float effects_intersect_plane(const real_point3d *point, const real_vector3d *vector, const real_plane3d *plane)
{
    float denominator = plane->normal.i * vector->i + plane->normal.j * vector->j + plane->normal.k * vector->k;
    if (denominator != 0.0f)
        return -effects_point_plane_distance(plane, point) / denominator;
    return 0.0f;
}
static __forceinline real_point3d *effects_project_point_onto_plane3d(const real_point3d *point, const real_plane3d *plane, real_point3d *result)
{
    float distance = effects_intersect_plane(point, &plane->normal, plane);
    return point_from_line3d(point, &plane->normal, distance, result);
}
static __forceinline void effects_marker_copy_component(float *destination, const float *source)
{
    float *const &field = destination;
    memcpy(field, source, sizeof(float));
}
static __forceinline void effects_marker_copy_point_words(const real_point3d * source, real_point3d * destination)
{
    effects_marker_copy_component(&destination->x,&source->x);
    effects_marker_copy_component(&destination->y,&source->y);
    effects_marker_copy_component(&destination->z,&source->z);
}
// @address 0x235da0 ?effect_marker_list_get_marker@@YAXPAUobject_marker@@PBUeffect_marker_list@@FPBUreal_plane3d@@@Z
void effect_marker_list_get_marker(object_marker *marker, const effect_marker_list *list, short marker_index, const real_plane3d *plane)
{
    real_point3d position;
    real_vector3d normal;
    real_vector3d up;
    marker->node_index = list->node_index;
    if ((unsigned short)list->node_index != 0xffff) {
        const s_effect_marker *source = &list->markers[marker_index];
        matrix4x3_inverse_transform_point(&list->node_matrix, &source->position, &position);
        effects_inverse_transform_vector(&list->node_matrix, &source->normal, &normal);
        normalize3d(&normal);
    } else {
        const s_effect_marker *source = &list->markers[marker_index];
        position = source->position;
        normal = source->normal;
    }
    perpendicular3d(&normal, &up);
    normalize3d(&up);
    if (plane) {
        if (!(dot_product3d(&position, &plane->normal) - plane->d >= 0.000244140625f)) {
            float distance = vector_intersect_plane3d(&position, &plane->normal, plane);
            point_from_line3d(&position, &plane->normal, distance, &position);
            point_from_line3d(&position, &plane->normal, 0.000244140625f, &position);
        }
    }
    real_matrix4x3 *matrix = &marker->node_matrix;
    matrix4x3_rotation_from_vectors(matrix, &normal, &up);
    effects_marker_copy_point_words(&position, &matrix->position);

}

// @address 0x33dea8 effect_damage_batch_find_or_create_entry
extern "C" long effect_damage_batch_find_or_create_entry(c_render_batch *cache, const s_damage_data *request)
{
    long result = -1;
    long entry_count = cache->count;
    for (long entry_index = 0; entry_index < cache->count; entry_index++) {
        const s_effect_damage_batch_entry *entry = &cache->entries[entry_index];
        if (entry->definition_index == request->effect &&
            memcmp(&entry->owner, &request->owner, sizeof(s_damage_owner)) == 0 &&
            entry->type == request->reporting_info &&
            !(distance_squared3d(&request->origin, &entry->position) >= 2.25f)) {
            long node_index;
            for (node_index = 0; node_index < entry->node_count; node_index++) {
                if (request->location.cluster_index == entry->node_indices[node_index])
                    break;
            }
            if (node_index != entry->node_count) {
                result = entry_index;
                break;
            }
        }
    }
    if (result == -1 && entry_count < 8) {
        result = cache->count++;
        cache->entries[result].reference_count = 0;
    }
    return result;
}

// @address 0x33dff8 effect_damage_batch_add_record
extern "C" void effect_damage_batch_add_record(
    c_render_batch *cache, long record_index, long entry_index, const s_damage_data *request)
{
    s_effect_damage_batch_record *record = &cache->records[record_index];
    s_effect_damage_batch_entry *entry = &cache->entries[entry_index];
    record->position = request->origin;
    record->entry_index = entry_index;
    record->radius = request->scale[0];
    if (entry->reference_count == 0) {
        entry->definition_index = request->effect;
        entry->owner = request->owner;
        entry->type = request->reporting_info;
        entry->position = request->origin;
        entry->location = request->location;
        entry->node_count = structure_clusters_in_sphere(
            entry->location.cluster_reference, &entry->position, 1.5f, 4, (s_cluster_reference *)entry->node_indices, 0);
    }
    entry->reference_count++;
}

static __forceinline long effects_damage_batch_new_record(c_render_batch *cache)
{
    long record_index = cache->record_count;
    if (record_index < 128) {
        cache->record_count = record_index + 1;
        return record_index;
    }
    return -1;
}
// @address 0x33e100 effect_damage_batch_add_request
extern "C" bool effect_damage_batch_add_request(c_render_batch *cache, const s_damage_data *request)
{
    bool success = false;
    long record_index = effects_damage_batch_new_record(cache);
    if (record_index != -1) {
        long entry_index = effect_damage_batch_find_or_create_entry(cache, request);
        if (entry_index != -1) {
            effect_damage_batch_add_record(cache, record_index, entry_index, request);
            success = true;
        } else {
            cache->record_count--;
        }
    }
    if (!success) {
        effect_damage_batch_flush(cache);
        success = effect_damage_batch_add_request(cache, request);
    }
    return success;
}

static __forceinline void effects_request_system(long effect_index, long event_index, const effect_part_definition *part, long part_index, long type)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    if (!event->flags.test(_event_stale_bit)) {
        s_effect_message message(effect_index, event_index, part->limit_type);
        message.type = type;
        message.system.duration = event->duration;
        message.system.scale_a = effect->scale_a;
        message.system.scale_b = effect->scale_b;
        message.system.location = effect->location;
        message.system.object_index = effect->object_index;
        message.system.first_person_flags = effect->first_person | effect->first_person_user_flags;
        message.system.part_index = part_index;
        if (type == 3)
            message.system.lightprobe_slot_index = (float)effect->lightprobe_slot_index;
        effect_send_render_message(&message);
    }
}
// @address 0x236c78 ?event_request_contrail_system@@YAXJJPBUeffect_part_definition@@J@Z
void event_request_contrail_system(long effect_index, long event_index, const effect_part_definition *part, long part_index)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    if (!event->flags.test(_event_stale_bit)) {
        s_effect_message message(effect_index, event_index, part->limit_type);
        message.type = _effect_message_create_contrail_system;
        message.system.duration = event->duration;
        message.system.scale_a = effect->scale_a;
        message.system.scale_b = effect->scale_b;
        message.system.location = effect->location;
        message.system.object_index = effect->object_index;
        message.system.first_person_flags = (signed char)effect->first_person | effect->first_person_user_flags;
        message.system.part_index = part_index;
        message.system.lightprobe_slot_index = (float)effect->lightprobe_slot_index;
        effect_send_render_message(&message);
    }

}
static __forceinline unsigned char effect_cinematic_update_active()
{
    bool active = false;
    s_cinematic_globals_definition *cinematics = cinematic_globals;
    if (cinematics)
        active = cinematics->in_progress;
    return active;
}
// @address 0x2372a0 effect_update_contrail_system
extern "C" void effect_update_contrail_system(s_effect_update_location *record, float value)
{
    s_data_array *array = ::effect_data;
    long datum_index = record->effect_index;
    if (datum_index != -1) {
        unsigned short absolute_index = (unsigned short)datum_index;
        if (absolute_index < array->first_unallocated) {
            effect_datum *effect = (effect_datum *)(array->data + absolute_index * array->size);
            short identifier = effect->identifier;
            if (identifier != 0 && identifier == (datum_index >> 16)) {
                bool update = (unsigned char)effect->location.cluster_reference.bsp_index != 255
                    || effect_cinematic_update_active() != 0;
                if (!update)
                    return;
                bool has_particle = effect->attachment_index == -1
                    || effects_particle_try_and_get(effect->attachment_index) != 0;
                if (!has_particle)
                    return;
                long first_person_flags = (signed char)effect->first_person | effect->first_person_user_flags;
                record->lightprobe_slot_index = effect->lightprobe_slot_index;
                record->effect_scale_a = effect->scale_a;
                record->effect_scale_b = effect->scale_b;
                record->first_person_flags = first_person_flags;
                effect_update_contrail_locations(effect, record, value);
                return;
            }
        }
    }
    record->effect_index = -1;
}
// @address 0x2373f0 ?event_request_light_volume_system@@YAXJJPBUeffect_part_definition@@J@Z
void event_request_light_volume_system(long effect_index, long event_index, const effect_part_definition *part, long part_index)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    if (!event->flags.test(_event_stale_bit)) {
        s_effect_message message(effect_index, event_index, part->limit_type);
        message.type = _effect_message_create_contrail_profile;
        message.system.location = effect->location;
        message.system.duration = event->duration;
        message.system.scale_a = effect->scale_a;
        message.system.scale_b = effect->scale_b;
        message.system.object_index = effect->object_index;
        message.system.first_person_flags = (signed char)effect->first_person | effect->first_person_user_flags;
        message.system.part_index = part_index;
        effect_send_render_message(&message);
    }
}
// @address 0x237768 event_request_beam_system
extern "C" void event_request_beam_system(long effect_index, long event_index, const effect_part_definition *part, long part_index)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    if (!event->flags.test(_event_stale_bit)) {
        s_effect_message message(effect_index, event_index, part->limit_type);
        message.type = _effect_message_create_light_volume_system;
        message.system.location = effect->location;
        message.system.duration = event->duration;
        message.system.scale_a = effect->scale_a;
        message.system.scale_b = effect->scale_b;
        message.system.object_index = effect->object_index;
        message.system.first_person_flags = (signed char)effect->first_person | effect->first_person_user_flags;
        message.system.part_index = part_index;
        effect_send_render_message(&message);
    }
}

// @address 0x236820 ?event_create_particle_system@@YAXPBUs_effect_message@@@Z
void event_create_particle_system(const s_effect_message *message)
{
    const s_effect_message_particle *parameters = &message->particle;
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, message->definition_index);
    const effect_event_definition *event_definition = &((const effect_event_definition *)definition->events.address)[message->event_block_index];
    const c_particle_system_definition *system_definition = &(
        (const c_particle_system_definition *)event_definition->particle_systems.address)[parameters->sub_index];
    long system_index = c_particle_system::create(system_definition, message->effect_index, message->definition_index,
        parameters->sub_index, message->event_block_index, message->event_counter, parameters->random_seed);
    if (system_index != -1) {
        c_particle_system *system = datum_get<c_particle_system>(g_particle_system_data_array, system_index);
        system->m_location = parameters->location;
        system->m_lightprobe_index = (long)message->particle.count;
        system->m_breakable_surface_event_index = message->particle.value;
        system->m_effect_scale_a = message->particle.scale;
        system->m_effect_scale_b = message->particle.rate;
        system->m_object_index = message->particle.owner_index;
        system->m_player_mask = (unsigned short)message->particle.owner_type;
        if ((bool)(_rotl(message->location_index, 30) & 1)) system->m_flags |= 128;
        else system->m_flags &= ~128;
        if ((bool)(_rotl(message->location_index, 29) & 1)) system->m_flags |= 256;
        else system->m_flags &= ~256;
        if (message->location_index & 2) system->m_flags |= 4;
        g_effect_last_particle_system_index = system_index;
    } else g_effect_last_particle_system_index = -1;

}

// @address 0x2b4840 ?create@c_contrail_system@@SAJEJJEJJE@Z
// @address 0x236ef8 event_create_contrail_system
extern "C" void event_create_contrail_system(const s_effect_message *message)
{
    // match: 0x236ef8: pointer __restrict qualifier prevents alias reload
    const s_effect_message *__restrict source = message;
    unsigned short event_block_index = message->event_block_index;
    const s_effect_message_system *parameters = &message->system;
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, message->definition_index);
    const effect_event_definition *event_definition = &((const effect_event_definition *)definition->events.address)[(long)event_block_index];
    const effect_part_definition *part = &((const effect_part_definition *)event_definition->parts.address)[parameters->part_index];
    long effect_index = message->effect_index;
    if (!effect_get_valid(effect_index)) effect_index = -1;
    typedef long (__cdecl *effects_contrail_create_abi)(unsigned char, c_tag_index, long, unsigned char, long, long, unsigned char);
    long system_index = reinterpret_cast<effects_contrail_create_abi>(&c_contrail_system::create)(part->limit_type, part->definition.index,
        effect_index, (unsigned char)event_block_index, message->event_counter, source->definition_index, part->render_type);
    if (system_index != -1) {
        g_effect_last_contrail_system_index = system_index;
        c_contrail_system *system = datum_get<c_contrail_system>(g_contrail_system_data, system_index);
        system->part_block_index = parameters->part_index;
        system->lightprobe_slot_index = (long)parameters->lightprobe_slot_index;
        system->pulse_time = parameters->scale_a;
        system->distance = parameters->scale_b;
        system->bsp_index = (unsigned char)parameters->first_person_flags;
    } else g_effect_last_contrail_system_index = -1;

}

// @address 0x237650 event_create_contrail_profile
extern "C" void event_create_contrail_profile(const s_effect_message *message)
{
    // match: 0x237650: pointer __restrict qualifier prevents alias reload
    const s_effect_message *__restrict source = message;
    unsigned short event_block_index = message->event_block_index;
    const s_effect_message_system *parameters = &message->system;
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, message->definition_index);
    const effect_event_definition *event_definition = &((const effect_event_definition *)definition->events.address)[(long)event_block_index];
    const effect_part_definition *part = &((const effect_part_definition *)event_definition->parts.address)[parameters->part_index];
    long effect_index = message->effect_index;
    if (!effect_get_valid(effect_index)) effect_index = -1;
    long system_index = effect_new_contrail_profile(part->limit_type, part->definition.index, effect_index,
        (unsigned char)event_block_index, message->event_counter, source->definition_index, part->render_type);
    if (system_index != -1) {
        g_effect_last_contrail_profile_index = system_index;
        c_light_volume_system *system = datum_get<c_light_volume_system>(g_particle_instance_owner_data, system_index);
        system->part_index = parameters->part_index;
        system->scale_a = parameters->scale_a;
        system->scale_b = parameters->scale_b;
        system->first_person_flags = (unsigned char)parameters->first_person_flags;
    } else g_effect_last_contrail_profile_index = -1;

}
// @address 0x2379d8 ?event_create_light_volume_system@@YAXPBUs_effect_message@@@Z
void event_create_light_volume_system(const s_effect_message *message)
{
    // match: 0x2379d8: pointer __restrict qualifier prevents alias reload
    const s_effect_message *__restrict source = message;
    unsigned short event_block_index = message->event_block_index;
    const s_effect_message_system *parameters = &message->system;
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, message->definition_index);
    const effect_event_definition *event_definition = &((const effect_event_definition *)definition->events.address)[(long)event_block_index];
    const effect_part_definition *part = &((const effect_part_definition *)event_definition->parts.address)[parameters->part_index];
    long effect_index = message->effect_index;
    if (!effect_get_valid(effect_index)) effect_index = -1;
    long system_index = light_volume_system_new(part->limit_type, part->definition.index, effect_index, (unsigned char)event_block_index,
        message->event_counter, source->definition_index, part->render_type);
    if (system_index != -1) {
        g_effect_last_light_volume_system_index = system_index;
        c_light_volume_system *system = datum_get<c_light_volume_system>(g_light_volume_system_data, system_index);
        system->part_index = parameters->part_index;
        system->scale_a = parameters->scale_a;
        system->scale_b = parameters->scale_b;
        system->first_person_flags = (unsigned char)parameters->first_person_flags;
    } else g_effect_last_light_volume_system_index = -1;

}


static __forceinline void effects_bsp_walk_system(s_data_array *data)
{
    s_iterator it;
    it.iterator.absolute_index = -1;
    it.iterator.index = -1;
    it.iterator.data = data;
    it.datum = (s_datum *)data_iterator_next(&it.iterator);
    while (it.datum != 0) {
        it.datum = (s_datum *)data_iterator_next(&it.iterator);
    }
}
static __forceinline void effects_reset_make_invalid(s_data_array *array)
{
    array->valid = false;
}
// @address 0x233438 ?effects_reset@@YAXXZ
void effects_reset()
{
    s_iterator it;
    it.iterator.data = ::effect_data;
    it.iterator.absolute_index = -1;
    it.iterator.index = -1;
    it.datum = (s_datum *)data_iterator_next(&it.iterator);
    while (it.datum != 0) {
        effect_destroy(it.iterator.index, true);
        it.datum = (s_datum *)data_iterator_next(&it.iterator);
    }
    c_particle_system::dispose_from_map();
    c_particle_system::initialize_for_map();
    c_contrail_system::dispose_from_map();
    c_contrail_system::initialize_for_map();
    effects_reset_make_invalid((s_data_array *)g_contrail_gpu_link_data);
    effects_reset_make_invalid((s_data_array *)g_contrail_chain_data);
    g_contrail_frame_shader_pair.vertex_shader.shader_reference = 0;
    g_contrail_frame_shader_pair.pixel_shader.shader_reference = 0;
    g_effect_shader_cache.shader_reference = 0;
    effects_reset_make_invalid((s_data_array *)g_particle_instance_data);
    effects_reset_make_invalid((s_data_array *)g_particle_instance_location_data);
    effects_reset_make_invalid((s_data_array *)g_particle_instance_owner_data);
    particle_initialize_for_new_map();
    effects_reset_make_invalid((s_data_array *)g_light_volume_gpu_row_data);
    effects_reset_make_invalid((s_data_array *)g_light_volume_gpu_data);
    g_light_volume_gpu_update_shader.vertex_shader.shader_reference = 0;
    g_light_volume_gpu_update_shader.pixel_shader.shader_reference = 0;
    g_light_volume_gpu_render_shader.shader_reference = 0;
    effects_reset_make_invalid((s_data_array *)g_light_volume_data);
    effects_reset_make_invalid((s_data_array *)g_light_volume_location_data);
    effects_reset_make_invalid((s_data_array *)g_light_volume_system_data);
    c_light_volume_system::initialize_for_map();
}
// @address 0x233588 ?effects_initialize_for_new_structure_bsp@@YAXK@Z
void effects_initialize_for_new_structure_bsp(unsigned long structure_bsp_mask)
{
    long index = ::data_next_index(effect_data, -1);
    while (index != -1) {
        effect_datum *effect = datum_get<effect_datum>(::effect_data, index);
        if (effect->object_index != -1) {
            object_datum *object = (object_datum *)object_try_and_get_and_verify_type(effect->object_index, 0xffffffff);
            if (!object) {
                effect->object_index = -1;
                location_invalidate(&effect->location);
                effect_destroy(index, true);
                effect = 0;
            } else {
                effect->location = object->location;
            }
        } else {
            if (!(effect->flag_words[BIG_ENDIAN_VIEW_INDEX(0, 2)] & 1) && !effect_get_location_from_object((effect_datum *)effect)) {
                effect_destroy(index, true);
                effect = 0;
            }
        }
        index = inl_data_next_index(::effect_data, index);
    }
    c_particle_system::initialize_for_new_structure_bsp();
    effects_bsp_walk_system(g_contrail_system_data);
    effects_bsp_walk_system((s_data_array *)g_particle_instance_owner_data);
    effects_bsp_walk_system((s_data_array *)g_light_volume_system_data);
}

// March body is empty; the release image folds it into the shared bare blr at 0x5bec98.
// @address 0x5bec98 ?effects_dispose_from_old_structure_bsp@@YAXK@Z
void effects_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask)
{
}
// @address 0x233748 effects_process_pending_effects
extern "C" void effects_process_pending_effects(unsigned long structure_bsp_mask)
{
    effects_flush_pending_effects();
}
// @address 0x233750 effects_delete_unused_for_new_map
extern "C" void effects_delete_unused_for_new_map(const s_game_non_bsp_zone_set *old_zone_set, const s_game_non_bsp_zone_set *new_zone_set,
    c_scenario_resource_registry *pending_zone_registry)
{
    effects_flush_pending_effects();
    for (long effect_index = ::data_next_index(effect_data, -1); effect_index != -1;
        effect_index = inl_data_next_index(::effect_data, effect_index))
    {
        long definition_index = datum_get<effect_datum>(::effect_data, effect_index)->definition_index & 0xffff;
        if (!pending_zone_registry->is_tag_available(definition_index))
            effect_destroy(effect_index, true);
    }
    s_iterator it;
    it.iterator.absolute_index = -1;
    it.iterator.index = -1;
    it.iterator.data = (s_data_array *)g_particle_system_data_array;
    it.datum = (s_datum *)data_iterator_next(&it.iterator);
    if (it.datum) {
        do {
            c_particle_system *particle = (c_particle_system *)it.datum;
            long definition_index = particle->m_tag_index & 0xffff;
            if (!pending_zone_registry->is_tag_available(definition_index)) {
                long index = it.iterator.index;
                particle = datum_get<c_particle_system>(g_particle_system_data_array, index);
                ((c_particle_system *)particle)->release_particles();
                datum_delete((s_data_array *)g_particle_system_data_array, index);
            }
            it.datum = (s_datum *)data_iterator_next(&it.iterator);
        } while (it.datum);
    }
    contrail_systems_dispose_unused(old_zone_set, new_zone_set, pending_zone_registry);
    it.iterator.absolute_index = -1;
    it.iterator.index = -1;
    it.iterator.data = (s_data_array *)g_particle_instance_owner_data;
    it.datum = (s_datum *)data_iterator_next(&it.iterator);
    if (it.datum) {
        do {
            c_light_volume_system *location = (c_light_volume_system *)it.datum;
            long definition_index = location->m_definition_index & 0xffff;
            if (!pending_zone_registry->is_tag_available(definition_index)) {
                long location_index = it.iterator.index;
                if (location_index != -1) {
                    c_light_volume_system *location_datum =
                        datum_get<c_light_volume_system>(g_particle_instance_owner_data, location_index);
                    long next_location_index = location_datum->m_first_location_index;
                    location_datum->m_first_location_index = -1;
                    if (next_location_index != -1) {
                        s_effect_contrail_location *contrail =
                            datum_get<s_effect_contrail_location>(g_particle_instance_location_data, next_location_index);
                        long leaf_index = contrail->first_instance_index;
                        if (leaf_index != -1) {
                            s_particle_instance *leaf = datum_get<s_particle_instance>(g_particle_instance_data, leaf_index);
                            long gpu_index = leaf->gpu_index;
                            if (gpu_index != -1)
                                contrail_gpu_destroy((unsigned int)gpu_index);
                            long volume_index = leaf->next;
                            if (volume_index != -1)
                                particle_instance_delete(volume_index);
                            datum_delete((s_data_array *)g_particle_instance_data, leaf_index);
                        }
                        long child_index = contrail->next_index;
                        if (child_index != -1)
                            particle_delete_location_chain(child_index);
                        datum_delete((s_data_array *)g_particle_instance_location_data, next_location_index);
                    }
                    datum_delete((s_data_array *)g_particle_instance_owner_data, location_index);
                }
            }
            it.datum = (s_datum *)data_iterator_next(&it.iterator);
        } while (it.datum);
    }
    it.iterator.absolute_index = -1;
    it.iterator.index = -1;
    it.iterator.data = (s_data_array *)g_light_volume_system_data;
    it.datum = (s_datum *)data_iterator_next(&it.iterator);
    if (it.datum) {
        do {
            c_light_volume_system *location = (c_light_volume_system *)it.datum;
            long definition_index = location->m_definition_index & 0xffff;
            if (!pending_zone_registry->is_tag_available(definition_index)) {
                long location_index = it.iterator.index;
                if (location_index != -1) {
                    c_light_volume_system *location_datum =
                        datum_get<c_light_volume_system>(g_light_volume_system_data, location_index);
                    long next_location_index = location_datum->m_first_location_index;
                    location_datum->m_first_location_index = -1;
                    if (next_location_index != -1) {
                        c_light_volume_location *volume =
                            datum_get<c_light_volume_location>(g_light_volume_location_data, next_location_index);
                        long leaf_index = volume->m_light_volume_index;
                        if (leaf_index != -1) {
                            ::c_light_volume *leaf = datum_get<c_light_volume>(g_light_volume_data, leaf_index);
                            long gpu_index = leaf->m_gpu_light_volume_index;
                            if (gpu_index != -1)
                                ::c_light_volume_gpu::destroy(gpu_index);
                            long volume_index = leaf->m_next_light_volume_index;
                            if (volume_index != -1)
                                ::c_light_volume::destroy(volume_index);
                            datum_delete((s_data_array *)g_light_volume_data, leaf_index);
                        }
                        long child_index = volume->m_sibling_index;
                        if (child_index != -1)
                            ::c_light_volume_location::destroy(child_index);
                        datum_delete((s_data_array *)g_light_volume_location_data, next_location_index);
                    }
                    datum_delete((s_data_array *)g_light_volume_system_data, location_index);
                }
            }
            it.datum = (s_datum *)data_iterator_next(&it.iterator);
        } while (it.datum);
    }
}
static __forceinline real_point3d *effects_transform_point(const real_matrix4x3 *matrix, const real_point3d *point, real_point3d *result)
{
    float x = point->x;
    float y = point->y;
    float z = point->z;
    x *= matrix->scale;
    y *= matrix->scale;
    z *= matrix->scale;
    result->x = x * matrix->rotation.forward.i + y * matrix->rotation.left.i + z * matrix->rotation.up.i + matrix->position.x;
    result->y = x * matrix->rotation.forward.j + y * matrix->rotation.left.j + z * matrix->rotation.up.j + matrix->position.y;
    result->z = x * matrix->rotation.forward.k + y * matrix->rotation.left.k + z * matrix->rotation.up.k + matrix->position.z;
    return result;
}

static __forceinline real_vector3d *effects_lens_transform_normal(const real_matrix4x3 *matrix, const real_vector3d *normal, real_vector3d *result)
{
    float i = normal->i;
    float j = normal->j;
    float k = normal->k;
    result->i = i * matrix->forward.i + (k * matrix->up.i + j * matrix->left.i);
    result->j = i * matrix->forward.j + (k * matrix->up.j + j * matrix->left.j);
    result->k = i * matrix->forward.k + (k * matrix->up.k + j * matrix->left.k);
    return result;
}

static __forceinline effect_location_datum *effects_lens_next_location(const effect_datum *effect, long *location_index, short type)
{
    long next_location_index = effect_location_find_next_of_type(effect, *location_index, type);
    effect_location_datum *location = 0;
    *location_index = next_location_index;
    if (next_location_index != -1)
    {
        location = effects_location_get(next_location_index);
        *location_index = location->next_location_index;
    }
    return location;
}

static __forceinline const long &effects_lens_first_location(const effect_datum *effect, long location_block_index)
{
    return effect->location_indices[location_block_index];
}


// @address 0x239590 effect_create_lens_flares
extern "C" void effect_create_lens_flares(long effect_index, long user_index)
{
    effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    long event_index = effect->event_index;
    if (event_index != -1)
    {
        const real_argb_color *white = objects_get_default_argb_white();
        const float &one = 1.0f;
        const float &zero = 0.0f;
        do
        {
            event_datum *event = datum_get<event_datum>(g_event_data, event_index);
            const effect_event_definition *event_definition = &((const effect_event_definition *)definition->events.address)[event->event_block_index];
            if (!event->flags.test(_event_finished_bit))
            {
                for (long part_index = 0; part_index < event_definition->parts.count; part_index++)
                {
                    const effect_part_definition *part = &((const effect_part_definition *)event_definition->parts.address)[part_index];
                    if (part->definition.group_tag == 'lens' && part->definition.index != -1)
                    {
                        short location_block_index = part->location_index;
                        if (location_block_index >= 0 && location_block_index < definition->locations.count)
                        {
                            long location_index = effects_lens_first_location(effect, location_block_index);
                            long location_type = part->render_type;
                            unsigned short instance_key = 0;
                            effect_location_datum *location;
                            while ((location = effects_lens_next_location(effect, &location_index, location_type)) != 0)
                            {
                                real_point3d position = location->matrix.position;
                                real_vector3d direction = location->matrix.forward;
                                float intensity = one;
                                real_rgb_color color = white->rgb;
                                if (part->scale_a_mask & (1 << 5))
                                    intensity *= effect->scale_a;
                                if (part->scale_b_mask & (1 << 5))
                                    intensity *= effect->scale_b;
                                if (location->node_index != 0xffff)
                                {
                                    real_matrix4x3 node_matrix;
                                    effect_get_node_matrix(effect, (short)location->node_index, &node_matrix);
                                    effects_transform_point(&node_matrix, &position, &position);
                                    effects_lens_transform_normal(&node_matrix, &direction, &direction);
                                }
                                if (part->definition.group_tag == 'lens')
                                {
                                    float fraction = event->time;
                                    if (event->duration != zero)
                                    {
                                        fraction /= event->duration;
                                        const s_lens_flare_definition *lens_definition =
                                            TAG_GET('lens', const s_lens_flare_definition, part->definition.index);
                                        intensity *= one - transition_function_evaluate(lens_definition->falloff_function, fraction);
                                    }
                                    const bool &first_person = location->node_index != 0xffff && (location->node_index & 0x8000);
                                    lens_flares_create_internal(part->definition.index, _lens_flare_attachment_type_default, effect_index,
                                        (unsigned short)(part_index * 8 + instance_key), &position, &direction, &color, fraction, intensity,
                                        one, first_person, user_index);
                                }
                                instance_key++;
                            }
                        }
                    }
                }
            }
            event_index = event->next_event_index;
        }
        while (event_index != -1);
    }
}


// @address 0x237860 event_request_beam_system_location
extern "C" void event_request_beam_system_location(
    long effect_index, long event_index, const effect_part_definition *part, const real_matrix4x3 *matrix, unsigned char local_space)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    if (!event->flags.test(_event_stale_bit)) {
        real_vector3d velocity;
        real_vector3d direction;
        effect_random_translational_velocity("event-request-beam-system", false, effect, &matrix->forward, &direction, &velocity,
            part->minimum_velocity, part->maximum_velocity, part->velocity_angle, part->scale_a_mask, part->scale_b_mask);
        s_effect_message message(effect_index, event_index, part->limit_type);
        message.type = _effect_message_add_light_volume_location;
        memcpy(&message.light_volume.matrix, matrix, sizeof(real_matrix4x3));
        message.light_volume.velocity = velocity;
        message.light_volume.random_seed = (unsigned long)(effect_real_random_range("event-request-beam-system", false, effect, 0.0f,
            1.0f) * 4294967296.0f);
        message.location_index = local_space != 0;
        effect_send_render_message(&message);
    }
}

// @address 0x238100 ?effect_generate_lens_flares@@YAXJ_N@Z
void effect_generate_lens_flares(long effect_index, bool local_space)
{
    effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    if (definition->flags.test(_effect_definition_lens_flares_bit)) {
        s_effect_message message(effect_index, effect->event_index, definition->limit_type);
        message.type = _effect_message_add_render_location;
        message.location_index = local_space ? 16 : 0;
        for (long index = 0; index < definition->locations.count; index++) {
            const s_tag_block *const &location_block = &definition->locations;
            const effect_location_definition *locations = (const effect_location_definition *)location_block->address;
            const effect_location_definition *location_definition = &locations[index];
            if ((bool)(_rotl(location_definition->flags, 30) & 1)) {
                long location_index = effect_location_find_next_of_type(effect, effect->location_indices[index], 0);
                while (location_index != -1) {
                    message.lens_flare.location_index = location_index;
                    const effect_location_datum *location = effects_location_get(location_index);
                    real_matrix4x3 node_matrix;
                    real_matrix4x3 world_matrix;
                    const real_matrix4x3 *matrix;
                    if (location->node_index != 0xffff && effect->object_index != -1) {
                        effect_get_node_matrix(effect, (short)location->node_index, &node_matrix);
                        matrix4x3_compute_product(&node_matrix, &location->matrix, &world_matrix);
                        matrix = &world_matrix;
                    } else matrix = &location->matrix;
                    memcpy(&message.lens_flare.matrix, matrix, sizeof(real_matrix4x3));
                    effects_add_render_message(&message);
                    location_index = effect_location_find_next_of_type(effect, location->next_location_index, 0);
                }
            }
        }
    }

}

// @address 0x237f70 ?event_request_decal_system@@YAXJJPBUeffect_part_definition@@PBUreal_matrix4x3@@@Z
void event_request_decal_system(long effect_index, long event_index, const effect_part_definition *part, const real_matrix4x3 *matrix)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    s_effect_message message(effect_index, event_index, part->limit_type);
    message.type = _effect_message_create_decal_system;
    real_vector3d velocity;
    if (part->minimum_velocity == 0.0f && part->maximum_velocity == 0.0f) {
        velocity = matrix->forward;
    } else {
        real_vector3d direction;
        effect_random_translational_velocity("event-request-decal-system", false, effect, &matrix->forward, &direction, &velocity,
            part->minimum_velocity, part->maximum_velocity, part->velocity_angle, part->scale_a_mask, part->scale_b_mask);
    }
    message.decal.position = matrix->position;
    message.decal.velocity = velocity;
    message.decal.object_index = effect->object_index;
    message.decal.definition_index = part->definition.index;
    message.decal.limit_type = part->limit_type;
    effects_add_render_message(&message);
}

// @address 0x23b1e0 ?effect_ping_from_impact@@YA_NJMMPBUreal_matrix4x3@@PBTreal_vector3d@@PBTreal_point2d@@PBUreal_plane3d@@@Z
bool effect_ping_from_impact(long effect_index, float scale_a, float scale_b, const real_matrix4x3 *matrix, const real_vector3d *velocity,
    // match: 0x23b1e0: pointer __restrict qualifier prevents alias reload
    const real_point2d *__restrict coordinates, const real_plane3d *plane)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, effect_index);
    bool result = false;
    if (effect) {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        if (!effect->flags.test(_effect_marked_for_deletion_bit)) effect->flags.set(_effect_ping_from_impact_bit, true);
        if (velocity) effect->velocity = *velocity;
        if (coordinates) effect->impact_coordinates = *coordinates;
        effect->scale_a = scale_a;
        effect->scale_b = scale_b;
        if (matrix) {
            for (long index = 0; index < definition->locations.count; index++) {
                long location_index = effect->location_indices[index];
                while (location_index != -1) {
                    effect_location_datum *location = effects_location_get(location_index);
                    location->matrix = *matrix;
                    location->matrix.scale = 1.0f;
                    location_index = location->next_location_index;
                    if (plane) {
                        if (effects_point_plane_distance(plane, &location->matrix.position) < 0.000244140625f) {
                            effects_project_point_onto_plane3d(&location->matrix.position, plane, &location->matrix.position);
                            point_from_line3d(&location->matrix.position, &plane->normal, 0.000244140625f, &location->matrix.position);
                        }
                    }
                }
            }
        }
        result = true;
    }
    return result;

}

// @rdata 0x8202490c
static const float k_effects_common_epsilon = 0.0001f;
// @address 0x16b908 ?half_to_real@@YAMG@Z
static __forceinline float effects_lightprobe_normalize(real_vector3d *vector, float one, float epsilon)
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (!(fabsf(length) < epsilon)) {
        float inverse = one / length;
        vector->i *= inverse;
        vector->j *= inverse;
        vector->k *= inverse;
        return length;
    }
    return 0.0f;
}
// @address 0x2b8600 effect_sample_lightprobes
extern "C" e_geometry_sample_result effect_sample_lightprobes(const real_point3d *point, s_geometry_sample *result)
{
    e_geometry_sample_result sample_result = _geometry_sample_result_no_lightprobes;
    scenario *scenario_local = global_scenario;
    long lightprobe_definition_index = scenario_local->lightmap.index;
    unsigned long bsp_flags = ((const s_scenario_zone_set *)scenario_local->zone_sets.address)[global_zone_set_index].bsp_active_mask;
    if (lightprobe_definition_index != -1)
    {
        const s_scenario_lightmap_definition *definition =
            (const s_scenario_lightmap_definition *)global_tag_instances[datum_index_to_absolute_index(lightprobe_definition_index)].definition;
        if (definition->airprobes.count > 0)
        {
            const s_tag_block *entries = &definition->airprobes;
            float distances[6];
            float weights[6];
            long indices[6];
            long probe_count = 0;
            long farthest_probe = -1;
            float farthest_distance = -100.0f;
            for (long entry_index = 0; entry_index < entries->count; entry_index++)
            {
                const s_lightmap_airprobe *entry = &((const s_lightmap_airprobe *)entries->address)[entry_index];
                if (entry->bsp_mask & bsp_flags)
                {
                    float distance = sqrtf(distance_squared3d(&entry->position, point));
                    if (distance < 60.0f)
                    {
                        if (probe_count < 6)
                        {
                            distances[probe_count] = distance;
                            indices[probe_count] = entry_index;
                            if (distance > farthest_distance)
                            {
                                farthest_distance = distance;
                                farthest_probe = probe_count;
                            }
                            probe_count++;
                        }
                        else if (distance < farthest_distance)
                        {
                            distances[farthest_probe] = distance;
                            indices[farthest_probe] = entry_index;
                            farthest_distance = -100.0f;
                            farthest_probe = -1;
                            for (long probe_index = 0; probe_index < 6; probe_index++)
                            {
                                if (distances[probe_index] > farthest_distance)
                                {
                                    farthest_distance = distances[probe_index];
                                    farthest_probe = probe_index;
                                }
                            }
                        }
                    }
                }
            }

            if (probe_count > 0)
            {
                float total_weight = 0.0f;
                for (long probe_index = 0; probe_index < probe_count; probe_index++)
                {
                    float distance = MAX(distances[probe_index], 0.01f);
                    weights[probe_index] = 1.0f / ((distance * distance) * distance);
                    total_weight += weights[probe_index];
                }
                for (long probe_index = 0; probe_index < probe_count; probe_index++)
                    weights[probe_index] = ((1.0f / total_weight) * weights[probe_index]);

                memset(result, 0, sizeof(*result));
                result->diffuse.i = 0.0f;
                result->diffuse.j = 0.0f;
                result->diffuse.k = 0.0f;
                result->normal = *global_up3d;
                result->valid = true;
                result->position = *point;
                result->dominant_light_intensity = 10.0f;
                for (long probe_index = 0; probe_index < probe_count; probe_index++)
                {
                    const s_lightmap_airprobe *entry = &((const s_lightmap_airprobe *)entries->address)[indices[probe_index]];
                    float weight = weights[probe_index];
                    result->dominant_light_direction.i += half_to_real(entry->lightprobe.dominant_light_direction[0]) * weight;
                    result->dominant_light_direction.j += half_to_real(entry->lightprobe.dominant_light_direction[1]) * weight;
                    result->dominant_light_direction.k += half_to_real(entry->lightprobe.dominant_light_direction[2]) * weight;
                    result->dominant_light_color.red += half_to_real(entry->lightprobe.dominant_light_intensity[0]) * weight;
                    result->dominant_light_color.green += half_to_real(entry->lightprobe.dominant_light_intensity[1]) * weight;
                    result->dominant_light_color.blue += half_to_real(entry->lightprobe.dominant_light_intensity[2]) * weight;
                    for (long coefficient_index = 0; coefficient_index < 9; coefficient_index++)
                    {
                        result->red[coefficient_index] += half_to_real(entry->lightprobe.channel_coefficients[0][coefficient_index]) * weight;
                        result->green[coefficient_index] += half_to_real(entry->lightprobe.channel_coefficients[1][coefficient_index]) * weight;
                        result->blue[coefficient_index] += half_to_real(entry->lightprobe.channel_coefficients[2][coefficient_index]) * weight;
                    }
                }
                effects_lightprobe_normalize(&result->dominant_light_direction, 1.0f, k_effects_common_epsilon);
                sample_result = _geometry_sample_result_sampled;
            }
        }
    }
    return sample_result;
}

// @rdata 0x8203385c
static const float k_effect_lightprobe_sample_height = 0.2f;
static __forceinline e_geometry_sample_result effect_sample_lighting(const real_point3d *point, const real_vector3d *direction,
    const s_effect_definition *definition, s_geometry_sample *sample)
{
    e_geometry_sample_result result = geometry_sampling_sample_point_lighting(*point, *direction, true, -1,
        definition->flags.test(_effect_definition_lightprobe_bit), true, *sample);
    if (result != _geometry_sample_result_sampled)
        result = effect_sample_lightprobes(point, (s_geometry_sample *)sample);
    return result;
}
// @address 0x23b938 ?effect_update_lighting_samples@@YAXPBUeffect_datum@@@Z
void effect_update_lighting_samples(const effect_datum *effect)
{
    if (effect->lightprobe_slot_index != -1)
    {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        s_effect_lightprobe *lightprobe = &g_effect_lightprobe_memory[datum_index_to_absolute_index(effect->lightprobe_slot_index)];
        s_geometry_sample *sample = &lightprobe->sample;
        const s_effect_lightprobe_slot *slot =
            datum_get<s_effect_lightprobe_slot>(g_effect_lightprobe_slot_data, effect->lightprobe_slot_index);
        bool object_valid = slot->object_index != -1 && object_try_and_get_and_verify_type(slot->object_index, 0xffffffff);
        bool sampled = lightprobe->flags.test(1) && !object_valid;
        if (!sampled && object_valid && !definition->flags.test(_effect_definition_force_use_own_lightprobe_bit))
        {
            render_lighting *lighting = object_update_cached_render_lighting(slot->object_index, 1000.0f);
            if (lighting)
            {
                sampled = true;
                lightprobe->flags.set(0, true);
                memcpy(sample, &lighting->sample, sizeof(*sample));
            }
        }
        if (!sampled)
        {
            if (fabsf(slot->normal.i - global_zero_vector3d->i) >= k_effects_common_epsilon ||
                fabsf(slot->normal.j - global_zero_vector3d->j) >= k_effects_common_epsilon ||
                fabsf(slot->normal.k - global_zero_vector3d->k) >= k_effects_common_epsilon)
            {
                real_point3d point = slot->point;
                real_vector3d direction = slot->normal;
                point.x += 0.01f * direction.i;
                point.y += 0.01f * direction.j;
                point.z += 0.01f * direction.k;
                direction.i = -direction.i;
                direction.j = -direction.j;
                direction.k = -direction.k;
                sampled = _geometry_sample_result_sampled == effect_sample_lighting(&point, &direction, definition, sample);
                lightprobe->flags.set(0, true);
            }
        }
        if (!sampled)
        {
            real_matrix4x3 matrix;
            const real_matrix4x3 *location = effect_get_first_location_matrix(effect, &matrix);
            if (location)
            {
                real_point3d point = location->position;
                point_from_line3d(&point, global_up3d, k_effect_lightprobe_sample_height, &point);
                sampled = _geometry_sample_result_sampled == effect_sample_lighting(&point, global_down3d, definition, sample);
                lightprobe->flags.set(0, true);
            }
        }
        if (sampled)
            lightprobe->flags.set(1, true);
    }
}

typedef char effect_occlusion_view_offset[(sizeof(void *) != 4 || ((offsetof(c_player_view, occlusion_view) == 0x26dc))) ? 1 : -1];

static __forceinline bool effects_build_camera_info(const real_point3d *position)
{
    if (!(g_effect_camera_info.matrix.scale <= 0.0f)) {
        g_effect_camera_world_info = g_effect_camera_info;
        g_effect_camera_world_info.matrix.position.x += position->x;
        g_effect_camera_world_info.matrix.position.y += position->y;
        g_effect_camera_world_info.matrix.position.z += position->z;
        return true;
    }
    return false;
}
static __forceinline unsigned char effects_system_test_render_flag(unsigned short value)
{
    unsigned char result = (value >> 2) & 1;
    return result;
}
// @address 0x1795a8 effect_submit_contrail_systems
extern "C" void effect_submit_contrail_systems(long user_index, long pass)
{
    if (pass == 1) {
        bool valid_user = false;
        if (user_index != -1 && ((s_controllers *)g_director_globals)->slots[user_index].perspective == 0)
            valid_user = true;
        long user = valid_user ? user_index : -1;
        c_data_iterator<c_contrail_system> iterator;
        iterator.begin(g_contrail_system_data);
        while (iterator.next()) {
            c_contrail_system *system = iterator.get_datum();
            if (((const c_flags_no_init<long, unsigned short, 16> *)&system->flags)->test(3)) {
                long first_person_type = system->extra;
                bool visible = true;
                bool user_bit = ((signed char)system->bsp_index & (1 << user)) != 0;
                if (first_person_type == 1 && !user_bit || first_person_type == 2 && user_bit)
                    visible = false;
                if (visible) {
                    for (long location_index = system->first_location; location_index != -1; ) {
                        c_contrail_location *location =
                            c_contrail_location::get(location_index);
                        location->submit();
                        location_index = location->sibling;
                    }
                }
            }
        }
    }
}
// @address 0x180890 effect_queue_contrail_element
extern "C" void effect_queue_contrail_element(s_particle_instance *element)
{
    const real_point3d *center =
        &((const s_effect_contrail_location *)((const s_data_array *)g_particle_instance_location_data)->data)
            [(unsigned short)element->location_index].position;
    if (visibility_volume_test_sphere_planes((const __vector4 *)&g_visibility_globals.collection_input->storage[160], center, element->length) != 0) {
        long location_index =
            datum_get<s_effect_contrail_location>(g_particle_instance_location_data, element->location_index)->owner_index;
        const c_light_volume_system *location =
            datum_get<c_light_volume_system>(g_particle_instance_owner_data, location_index);
        const s_tag_block *block = (const s_tag_block *)global_tag_instances[datum_index_to_absolute_index(location->m_definition_index)].definition;
        const s_effect_entry *entries = (const s_effect_entry *)block->address;
        render_objects_add_transparent_element(center, 0, 0.0f,
            (e_transparent_sort_layer)entries[element->definition_index].shader.sort_layer,
            (void (*)(const void *, long))light_volume_submit_particle_instance, element, -1, 0.0f);
    }
}
// @address 0x1807b0 effect_submit_contrail_volumes
extern "C" void effect_submit_contrail_volumes(c_light_volume_system *system, long user)
{
    long first_person_type = system->first_person_type;
    bool visible = true;
    bool user_bit = ((signed char)system->first_person_flags & (1 << user)) != 0;
    if (first_person_type == 1 && !user_bit || first_person_type == 2 && user_bit)
        visible = false;
    if (visible) {
        for (long volume_index = system->m_first_location_index; volume_index != -1; ) {
            s_effect_contrail_location *volume =
                &((s_effect_contrail_location *)((s_data_array *)g_particle_instance_location_data)->data)[(unsigned short)volume_index];
            for (long element_index = volume->first_instance_index; element_index != -1; ) {
                s_particle_instance *element = &((s_particle_instance *)((s_data_array *)g_particle_instance_data)->data)[(unsigned short)element_index];
                effect_queue_contrail_element(element);
                element_index = element->next;
            }
            volume_index = volume->next_index;
        }
    }
}
// @address 0x179708 effect_submit_contrail_profiles
extern "C" void effect_submit_contrail_profiles(int user_index, int render_pass)
{
    unsigned char viewer_valid;
    c_light_volume_system *contrail_system;
    int submit_user;
    c_data_iterator<c_light_volume_system> contrail_iterator;
    if (render_pass != 1) return;
    viewer_valid = 0;
    if (user_index != -1 && ((s_controllers *)g_director_globals)->slots[user_index].perspective == 0)
        viewer_valid = 1;
    submit_user = ((unsigned char)viewer_valid != 0) ? user_index : -1;
    contrail_iterator.m_iterator.absolute_index = -1;
    contrail_iterator.m_iterator.index = -1;
    contrail_iterator.m_iterator.data = (s_data_array *)g_particle_instance_owner_data;
    contrail_system = (c_light_volume_system *)data_iterator_next(&contrail_iterator.m_iterator);
    contrail_iterator.m_datum = contrail_system;
    if (contrail_system != 0) {
        do {
            if ((unsigned int)effects_system_test_render_flag(contrail_system->m_flags) != 0)
                effect_submit_contrail_volumes((c_light_volume_system *)contrail_system, submit_user);
            contrail_system = (c_light_volume_system *)data_iterator_next(&contrail_iterator.m_iterator);
            contrail_iterator.m_datum = contrail_system;
        } while (contrail_system != 0);
    }
    return;
}
// @address 0x1797e0 effects_render
extern "C" void effects_render(int user_index, int render_pass)
{
    unsigned char valid_effect_user;
    c_light_volume_system *light_volume_system;
    int effective_user;
    c_data_iterator<c_light_volume_system> light_volume_iterator;
    if (render_pass != 1 && render_pass != 3) return;
    valid_effect_user = 0;
    if (user_index != -1 && ((s_controllers *)g_director_globals)->slots[user_index].perspective == 0)
        valid_effect_user = 1;
    effective_user = valid_effect_user ? user_index : -1;
    light_volume_iterator.m_iterator.absolute_index = -1;
    light_volume_iterator.m_iterator.index = -1;
    light_volume_iterator.m_iterator.data = (s_data_array *)g_light_volume_system_data;
    light_volume_system = (c_light_volume_system *)data_iterator_next(&light_volume_iterator.m_iterator);
    light_volume_iterator.m_datum = light_volume_system;
    if (light_volume_system != 0) {
        do {
            if ((unsigned int)effects_system_test_render_flag(light_volume_system->m_flags) != 0)
                light_volume_system_render(light_volume_system, effective_user, render_pass);
            light_volume_system = (c_light_volume_system *)data_iterator_next(&light_volume_iterator.m_iterator);
            light_volume_iterator.m_datum = light_volume_system;
        } while (light_volume_system != 0);
    }
    return;
}
// @address 0x1841a0 effect_update_camera
extern "C" void effect_update_camera(c_player_view *context)
{
    bool active;
    if (g_effect_render_globals->camera_relative && effects_build_camera_info(&context->render_camera_state.position)) {
        context->occlusion_view.render(&context->render_camera_state.position);
        active = true;
    } else {
        active = false;
    }
    g_effect_camera_active = active;

}


static __forceinline bool effects_particle_disposition_test(short disposition, bool violent)
{
    bool allowed;
    if (violent) allowed = disposition != 1;
    else allowed = disposition != 2;
    c_flags_no_init<long, unsigned char, 1> result;
    result.clear();
    result.set(0, allowed);
    unsigned char value = result;
    return value != 0;
}
static __forceinline unsigned char effects_particle_quality_bit(unsigned long flags)
{
    unsigned char result = (flags >> 4) & 1;
    return result;
}


// @address 0x236260 ?event_create_particle_systems@@YAXJJ@Z
void event_create_particle_systems(long effect_index, long event_index)
{
    // match: 0x236260: manual index keeps r9/r10 colors; datum_get renumbers IL and swaps them.
    const effect_datum *effect =
        &((const effect_datum *)::effect_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(effect_index)];
    const event_datum *event = &((event_datum *)g_event_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(event_index)];
    short first_person_flags = (signed char)effect->first_person | effect->first_person_user_flags;
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    const effect_event_definition *event_definition = &((const effect_event_definition *)definition->events.address)[event->event_block_index];
    bool looping = (bool)((event_definition->flags >> 1) & 1);
    bool low_quality = player_mapping_globals->local_player_count > 2 && !inl_cinematic_in_progress() && players_globals->suppressed == 0;
    long system_index = 0;
    if (event_definition->particle_systems.count > 0) {
    const float zero = 0.0f;
    const float one = 1.0f;
    const float seed_scale = 4294967296.0f;
    do {
        const c_particle_system_definition *system_definition = &(
            (const c_particle_system_definition *)event_definition->particle_systems.address)[system_index];
        unsigned short location_type = system_definition->camera_mode;
        bool environment_allowed;
        switch (system_definition->environment) {
        case 0: environment_allowed = true; break;
        case 1: environment_allowed = !effect->flags.test(_effect_underwater_bit); break;
        case 2: environment_allowed = effect->flags.test(_effect_underwater_bit); break;
        case 3: environment_allowed = false; break;
        // match: 0x236260: unreachable hint omits dead branch check
        default: __assume(0);
        }
        if (environment_allowed) {
            bool disposition_allowed = effects_particle_disposition_test(
                                           system_definition->disposition, effect->flags.test(_effect_nonviolent_bit)) &&
                (location_type != 1 || effect->flags.test(_effect_first_person_bit));
            bool quality_allowed = disposition_allowed && !(low_quality && effects_particle_quality_bit(system_definition->flags));
            if (quality_allowed) {
                s_effect_message create_message(effect_index, event_index, system_definition->limit_type);
                create_message.type = _effect_message_create_particle_system;
                create_message.system.duration = event->duration;
                create_message.particle.sub_index = system_index;
                create_message.system.location = effect->location;
                create_message.particle.value = effect->owner_type;
                create_message.particle.random_seed =
                    (unsigned long)(effect_real_random_range("creating-particle-system", false, effect, zero, one) * seed_scale);
                create_message.particle.count = (float)effect->lightprobe_slot_index;
                create_message.particle.scale = effect->scale_a;
                create_message.particle.rate = effect->scale_b;
                create_message.particle.owner_index = effect->object_index;
                create_message.particle.owner_type = first_person_flags;
                create_message.location_index = looping ? 2 : 0;
                if (effect->flags.test(_effect_attached_to_camera_bit)) create_message.location_index |= 4;
                else create_message.location_index &= ~4;
                if (effect->flags.test(_effect_first_person_only_bit)) create_message.location_index |= 8;
                else create_message.location_index &= ~8;
                effects_add_render_message(&create_message);
                long location_index = effect_location_find_next_of_type(effect, effect->location_indices[system_definition->location_index], location_type);
                while (location_index != -1) {
                    const effect_location_datum *location = effects_location_get(location_index);
                    unsigned short first_person = system_definition->m_coordinate_system;
                    real_matrix4x3 temporary_matrix;
                    const real_matrix4x3 *matrix = effect_get_transformed_location_world_or_local(effect, location, &temporary_matrix, first_person);
                    s_effect_message location_message(effect_index, event_index, system_definition->limit_type);
                    location_message.type = _effect_message_add_particle_location;
                    memcpy(&location_message.light_volume.matrix, matrix, sizeof(real_matrix4x3));
                    location_message.light_volume.velocity = effect->velocity;
                    location_message.location_index = location->node_index != 0xffff && (location->node_index & 0x8000);
                    effects_add_render_message(&location_message);
                    location_index = effect_location_find_next_of_type(effect, location->next_location_index, location_type);
                }
                s_effect_message finalize_message(effect_index, event_index, system_definition->limit_type);
                finalize_message.type = _effect_message_finalize_particle_system;
                effects_add_render_message(&finalize_message);
            }
        }
        system_index++;
    } while (system_index < event_definition->particle_systems.count);

    }
}
// @address 0x74b70c ?global_forward3d@@3PBTreal_vector3d@@B
// @address 0x74b720 ?global_down3d@@3PBTreal_vector3d@@B
// @address 0x74b710 ?global_left3d@@3PBTreal_vector3d@@B
// @address 0x74b700 ?global_origin3d@@3PBTreal_point3d@@B

// @address 0x23bd20 ?effect_get_lightmap_tint@@YA?BTreal_argb_color@@J@Z
const real_argb_color effect_get_lightmap_tint(long lightprobe_index)
{
    const s_geometry_sample *sample = effect_get_geometry_sample(lightprobe_index);
    static const float normalization = 1.0f / sqrtf(12.566370964050293f);
    real_argb_color color = { 0.0f, sample->red[0] * normalization,
        sample->green[0] * normalization, sample->blue[0] * normalization };
    static const float log_two = log(2.0f);
    float maximum_component = MAX(MAX(MAX(color.rgb.red, color.rgb.green), color.rgb.blue), k_effects_minimum_intensity);
    color.alpha = PIN(logf(maximum_component) / log_two, 0.0f, 1.0f);

    real_rgb_color *rgb = &color.rgb;
    float scale = exp(-color.alpha * log_two);
    rgb->red = scale * rgb->red;
    rgb->green = scale * rgb->green;
    rgb->blue = scale * rgb->blue;
    return color;
}

static __forceinline bool effects_update_contrail_visible(const effect_datum *effect)
{
    bool visible = effect->location.cluster_reference.bsp_index != -1 || inl_cinematic_in_progress();
    return visible;
}
static __forceinline bool effects_contrail_attachment_valid(const effect_datum *effect)
{
    bool valid = effect->attachment_index == -1 || effects_particle_try_and_get(effect->attachment_index) != 0;
    return valid;
}
// @address 0x14b810 effect_update_contrail_profile_locations
extern "C" void effect_update_contrail_profile_locations(s_effect_contrail_profile *system)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, system->definition_index);
    if (effect && effects_update_contrail_visible(effect) && effects_contrail_attachment_valid(effect)) {
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        const effect_event_definition *event = &((const effect_event_definition *)definition->events.address)[system->event_block_index];
        const effect_part_definition *part = &((const effect_part_definition *)event->parts.address)[system->next_index];
        short render_type = part->render_type;
        short first_person_flags = (signed char)effect->first_person | effect->first_person_user_flags;
        system->lifespan = effect->scale_a;
        system->interpolation = effect->scale_b;
        system->active_index = (signed char)first_person_flags;
        if (part->location_index != -1) {
            long node_index = system->location_index;
            long previous_index = -1;
            long location_index = effect_location_find_next_of_type(effect, effect->location_indices[part->location_index], render_type);
            while (location_index != -1) {
                effect_location_datum *location = effects_location_get(location_index);
                s_effects_contrail_spawn_parameters spawn;
                effect_get_part_to_world_matrix(effect, part, location, &spawn.matrix);
                if (node_index == -1)
                    node_index = effect_contrail_profile_add_location(system, &spawn,
                        (unsigned long)(effect_real_random_range("event-request-light-volume-system", false, effect, 0.0f, 1.0f) *
                            4294967296.0f));
                if (node_index != -1) {
                    s_effect_contrail_location *node =
                        datum_get<s_effect_contrail_location>(g_particle_instance_location_data, node_index);
                    node->position = spawn.matrix.position;
                    node->direction = spawn.matrix.forward;
                    previous_index = node_index;
                    node_index = node->next_index;
                }
                location_index = effect_location_find_next_of_type(effect, location->next_location_index, render_type);
            }
            if (node_index != -1) {
                long next_index;
                if (previous_index != -1) {
                    s_effect_contrail_location *previous =
                        datum_get<s_effect_contrail_location>(g_particle_instance_location_data, previous_index);
                    next_index = previous->next_index;
                    previous->next_index = -1;
                } else {
                    next_index = system->location_index;
                    system->location_index = -1;
                }
                if (next_index != -1)
                    particle_delete_location_chain(next_index);
            }
        }
    }
    if (!effect)
        system->definition_index = -1;
}

static __forceinline bool effects_light_volume_location_attached(const effect_location_datum *location)
{
    bool attached = location->node_index != 0xffff && (location->node_index & 0x8000);
    return attached;
}
// @address 0x237af0 ?effect_update_light_volume_system@@YAXPAVc_light_volume_system@@M@Z
void effect_update_light_volume_system(c_light_volume_system *system, float delta_time)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, system->effect_index);
    if (effect) {
        bool update = (unsigned char)effect->location.cluster_reference.bsp_index != 255
            || effect_cinematic_update_active() != 0;
        if (!update)
            return;
        bool attached = effect->attachment_index == -1
            || effects_particle_try_and_get(effect->attachment_index) != 0;
        if (!attached)
            return;
        float scale_a = effect->scale_a;
        long first_person_flags = (signed char)effect->first_person | effect->first_person_user_flags;
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        const effect_event_definition *event = &((const effect_event_definition *)definition->events.address)[system->event_block_index];
        const effect_part_definition *part = &((const effect_part_definition *)event->parts.address)[system->part_index];
        short render_type = part->render_type;
        system->scale_a = scale_a;
        system->scale_b = effect->scale_b;
        system->first_person_flags = (unsigned char)first_person_flags;
        if (part->location_index == -1)
            return;
        long location_chain = system->location_index;
        long last_visited = -1;
        long valid_index = effect_location_find_next_of_type(effect, effect->location_indices[part->location_index], render_type);
        long secondary_index = -1;
        bool fixed_secondary = false;
        short secondary_type = render_type;
        if (part->secondary_location_index != -1) {
            fixed_secondary = ((const effect_location_definition *)definition->locations.address)
                                  [part->secondary_location_index].marker_name == _string_id_child;
            if (fixed_secondary)
                secondary_type = 0;
            if (!fixed_secondary || effect->flags.test(_effect_vector_bit))
                secondary_index = effect_location_find_next_of_type(effect, effect->location_indices[part->secondary_location_index], secondary_type);
        }
        if (valid_index != -1) {
            do {
                effect_location_datum *location = effects_location_get(valid_index);
                effect_location_datum *secondary_location = 0;
                if (secondary_index != -1)
                    secondary_location = effects_location_get(secondary_index);
                real_matrix4x3 part_to_world;
                effect_get_part_to_world_matrix(effect, part, location, &part_to_world);
                real_matrix4x3 secondary_to_world;
                if (secondary_index != -1)
                    effect_get_part_to_world_matrix(effect, part, secondary_location, &secondary_to_world);
                if (location_chain == -1) {
                    float random = effect_real_random_range("event-request-beam-system", false, effect, 0.0f, 1.0f);
                    location_chain = system->add_light_volume_location(&part_to_world, (unsigned long)(random * 4294967296.0f));
                }
                if (location_chain != -1) {
                    c_light_volume_location *volume =
                        datum_get<c_light_volume_location>(g_light_volume_location_data, location_chain);
                    volume->m_position = part_to_world.position;
                    if (effects_light_volume_location_attached(location))
                        volume->first_person_flags |= 1;
                    else
                        volume->first_person_flags &= ~1;
                    if (secondary_index != -1) {
                        real_vector3d offset;
                        offset.i = secondary_to_world.position.x - part_to_world.position.x;
                        offset.j = secondary_to_world.position.y - part_to_world.position.y;
                        offset.k = secondary_to_world.position.z - part_to_world.position.z;
                        volume->m_length = normalize3d(&offset);
                        volume->m_forward = offset;
                    } else {
                        volume->m_forward = part_to_world.forward;
                    }
                    last_visited = location_chain;
                    location_chain = volume->m_sibling_index;
                }
                valid_index = effect_location_find_next_of_type(effect, location->next_location_index, render_type);
                if (secondary_index != -1 && !fixed_secondary)
                    secondary_index = effect_location_find_next_of_type(effect, secondary_location->next_location_index, secondary_type);
            } while (valid_index != -1);
        }
        if (location_chain == -1)
            return;
        long destroy_index;
        if (last_visited != -1) {
            c_light_volume_location *previous = datum_get<c_light_volume_location>(g_light_volume_location_data, last_visited);
            destroy_index = previous->m_sibling_index;
            previous->m_sibling_index = -1;
        } else {
            destroy_index = system->location_index;
            system->location_index = -1;
        }
        if (destroy_index == -1)
            return;
        ::c_light_volume_location::destroy(destroy_index);
    } else {
        system->effect_index = -1;
    }
}

// @address 0xa31760 g_effect_pending_pool
s_effect_pool g_effect_pending_pool;
// @address 0x232d60 effects_initialize
extern "C" void effects_initialize()
{
    ::effect_data = data_new("effect", 640, sizeof(effect_datum), 0, (c_allocation_base *)&g_effect_data_allocator);
    g_event_data = (s_data_array *)data_new("effect event", 640, sizeof(event_datum), 0, (c_allocation_base *)&g_event_data_allocator);
    g_effect_location_data = data_new("effect location", 1152, sizeof(effect_location_datum), 0,
        (c_allocation_base *)&g_effect_location_data_allocator);
    g_effect_lightprobe_slot_data = (s_data_array *)data_new("effect geometry sample", 128, sizeof(s_effect_lightprobe_slot), 0,
        (c_allocation_base *)&g_effect_lightprobe_slot_data_allocator);
    g_effect_lightprobe_memory = static_cast<s_effect_lightprobe *>(g_effect_lightprobe_allocator.reserve_memory("effect lightprobes", 0, 0xfe00, 0));
    g_effect_counts = (s_effect_counts *)g_effect_counts_allocator.reserve_memory("effect counts", 0, sizeof(s_effect_counts) * 2, 0);
    t_message_queue<s_effect_message, 512> *queue =
        (t_message_queue<s_effect_message, 512> *)g_effect_message_queue_allocator.reserve_memory(
            "effect messaging queue", 0, sizeof(t_message_queue<s_effect_message, 512>), 4);
    g_effect_message_queue = queue;
    queue->clear();
    g_effect_pending_pool.pending.value = 0;
    g_effect_pending_pool.free_list.value = 0;
    s_effect_pool_node *pool_node = g_effect_pending_pool.nodes;
    unsigned long remaining = 128;
    do {
        --remaining;
        pool_node->free = 0;
        pool_node++;
    } while (remaining != 0);
    g_effect_pending_pool.initialized = true;
    g_effect_pending_pool.free_count = 128;
    for (long node_index = 127; node_index >= 0; node_index--) {
        __lwsync();
        InterlockedPushEntrySList(&g_effect_pending_pool.free_list, &g_effect_pending_pool.nodes[node_index]);
    }
    s_effect_counts *counts = g_effect_counts;
    counts[1].effect_count = 0;
    counts[0].effect_count = 0;
    counts[1].event_count = 0;
    counts[0].event_count = 0;
    counts[1].location_count = 0;
    // match: 0x232d60: int pun keeps the zero-store scheduled after the pool stores; a plain store hoists it 4 insns.
    *reinterpret_cast<int *>(&g_effect_render_message_count) = 0;
    counts[0].location_count = 0;
    particle_system_create_data_arrays();
    contrails_initialize();
    g_particle_instance_owner_data = data_new("light_volume_system", 160, 44, 0, (c_allocation_base *)&g_memory_pool_allocator);
    g_particle_instance_location_data = data_new("light_volume", 160, sizeof(s_effect_contrail_location), 0,
        (c_allocation_base *)&g_light_volume_location_allocator);
    g_particle_instance_data = data_new("light_volume", 160, 52, 0, (c_allocation_base *)&g_light_volume_allocator);
    light_volume_gpu_initialize();
    g_light_volume_system_data = data_new("beam_system", 160, 44, 0,
        (c_allocation_base *)&g_light_volume_system_allocator);
    g_light_volume_location_data = data_new("beam", 160, 48, 0,
        (c_allocation_base *)&g_light_volume_location_data_allocator);
    g_light_volume_data = data_new("beam", 160, 52, 0, (c_allocation_base *)&g_light_volume_data_allocator);
    beam_gpu_initialize();
}

// @address 0x1467f0 effect_update_contrail_profile
extern "C" void effect_update_contrail_profile(s_effect_contrail_profile *profile, float delta_time)
{
    if (!(delta_time <= 0.0f))
        effect_update_contrail_profile_locations(profile);
    unsigned long status = effect_get_event_status(profile->definition_index, profile->event_block_index, profile->event_counter, &profile->age);
    if (status >= 1) {
        if (status != 1)
            profile->flags &= ~1;
        else
            profile->flags &= ~6;
    } else {
        profile->flags |= 6;
    }
}

// @address 0x234120 effect_creation_data_initialize_from_vectors
extern "C" void effect_creation_data_initialize_from_vectors(s_effect_creation_data *creation_data, c_tag_index definition_index,
    long vector_count, const s_effect_vector *vectors, const real_plane3d *plane, const s_location *hint,
    e_effect_deterministic deterministic)
{
    effect_creation_data_initialize(creation_data);
    creation_data->definition_index = definition_index;
    creation_data->effect_vector_count = vector_count;
    creation_data->effect_vectors = vectors;
    creation_data->plane = plane;
    creation_data->flags = 0;
    if (deterministic == _effect_deterministic)
        creation_data->flags = 4;
    if (hint)
        creation_data->location_hint = *hint;
}
// @address 0x234268 effect_new_impact_effect
extern "C" long effect_new_impact_effect(c_tag_index definition_index, long, long structure_bsp_index, long,
    long vector_count, const s_effect_vector *vectors, const real_plane3d *plane, const s_location *hint, float scale,
    e_effect_deterministic deterministic)
{
    s_effect_creation_data creation_data;
    creation_data.flags = 0;
    effect_creation_data_initialize_from_vectors(&creation_data, definition_index, vector_count, vectors, plane, hint, deterministic);
    creation_data.scale_a = scale;
    creation_data.structure_bsp_index = structure_bsp_index;
    creation_data.scale_b = scale;
    return effect_new_from_creation_data(&creation_data);
}
// @address 0x2342d8 effect_new_with_damage_owner
extern "C" long effect_new_with_damage_owner(c_tag_index definition_index, const s_damage_owner *damage_owner,
    unsigned char damage_reporting_type, long vector_count, const s_effect_vector *vectors, const real_plane3d *plane,
    const s_location *hint, e_effect_deterministic deterministic)
{
    s_effect_creation_data creation_data;
    creation_data.flags = 0;
    effect_creation_data_initialize_from_vectors(&creation_data, definition_index, vector_count, vectors, plane, hint, deterministic);
    if (damage_owner)
        creation_data.damage_owner = *damage_owner;
    creation_data.damage_reporting_type = damage_reporting_type;
    return effect_new_from_creation_data(&creation_data);
}
// @address 0x234198 effect_create_damage_effect
extern "C" void effect_create_damage_effect(c_tag_index definition_index, const s_damage_owner *damage_owner, long parent_effect_index,
    const real_vector3d *velocity, long vector_count, const s_effect_vector *vectors, const real_plane3d *, const s_location *hint,
    float scale_a, float scale_b, long reserved54, long reserved58, long region_permutation_index, long)
{
    s_effect_creation_data creation_data;
    creation_data.flags = 0;
    effect_creation_data_initialize_from_vectors(&creation_data, definition_index, vector_count, vectors, 0, hint, _effect_deterministic);
    if (damage_owner)
        creation_data.damage_owner = *damage_owner;
    if (velocity)
        creation_data.velocity = *velocity;
    creation_data.scale_a = scale_a;
    creation_data.parent_effect_index = parent_effect_index;
    creation_data.scale_b = scale_b;
    creation_data.reserved54 = reserved54;
    creation_data.reserved58 = reserved58;
    creation_data.region_permutation_index = region_permutation_index;
    effect_new_from_creation_data(&creation_data);
}
static __forceinline effect_location_datum *effects_danger_next_location(const effect_datum *effect, long *location_index)
{
    long next_location_index = effect_location_find_next_of_type(effect, *location_index, 0);
    effect_location_datum *location = 0;
    *location_index = next_location_index;
    if (next_location_index != -1) {
        location = effects_location_get(next_location_index);
        *location_index = location->next_location_index;
    }
    return location;
}
// @address 0xf7ad8 ?dangerous_effects_near_player@@YA_NXZ
bool dangerous_effects_near_player()
{
    bool result = false;
    for (long effect_index = ::data_next_index(effect_data, -1); effect_index != -1;
        effect_index = inl_data_next_index(::effect_data, effect_index))
    {
        effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        if (!effect->flags.test(_effect_stopping_bit) && definition->danger_radius != 0.0f) {
            player_datum_iterator player_iterator;
            player_iterator.begin(player_data);
            while (player_iterator.next()) {
                player_datum *player = player_iterator.get_datum();
                if (player->unit_index != -1) {
                    const object_datum *unit = object_header_get(player->unit_index)->datum;
                    long location_index = effect->location_indices[0];
                    effect_location_datum *location;
                    while ((location = effects_danger_next_location(effect, &location_index)) != 0) {
                        real_point3d point;
                        if (location->node_index != 0xffff) {
                            real_matrix4x3 node_matrix;
                            effect_get_node_matrix(effect, (short)location->node_index, &node_matrix);
                            effects_transform_point(&node_matrix, &location->matrix.position, &point);
                        } else {
                            point = location->matrix.position;
                        }
                        if (point_in_sphere(&unit->bounding_center, &point, unit->bounding_radius + definition->danger_radius)) {
                            result = true;
                            // match: 0xf7ad8: goto keeps result in r21 with one shared epilogue;
                            // an early return drops r21 and emits a second __restgprlr epilogue.
                            goto done;
                        }
                    }
                }
            }
        }
    }
done:
    return result;
}

static __forceinline long effects_vector_find(long marker_name, long vector_count, const s_effect_vector *vectors)
{
    long result = -1;
    for (long vector_index = 0; vector_index < vector_count; vector_index++) {
        if (vectors[vector_index].marker_name == marker_name) {
            result = vector_index;
            break;
        }
    }
    return result;
}
static __forceinline const real_plane3d *effects_effect_get_plane(const effect_datum *effect)
{
    return effect->flags.test(_effect_has_plane_bit) ? &effect->plane : 0;
}
static __forceinline long effects_location_new(effect_datum *effect, short location_block_index)
{
    long location_index = -1;
    long minimum_index = -1;
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    bool deterministic = definition->flags.test(_effect_definition_first_person_visibility_bit);
    if (deterministic)
        minimum_index = 0;
    else
        minimum_index = 576;
    location_index = datum_new_in_range((s_data_array *)g_effect_location_data, minimum_index, 576, !deterministic);
    g_effect_counts[deterministic].location_count++;
    effect_location_datum *location = effects_location_get(location_index);
    location->next_location_index = effect->location_indices[location_block_index];
    effect->location_indices[location_block_index] = location_index;
    return location_index;
}
// @address 0x235728 effect_update_locations
extern "C" void effect_update_locations(long effect_index, long object_index, bool first_person, const s_effect_vector *vectors,
    long vector_count, long marker_index, bool keep_locations)
{
    effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    long default_vector_index = effects_vector_find(_string_id_any, vector_count, vectors);
    const s_effect_definition *definition = (const s_effect_definition *)global_tag_instances[(unsigned short)(long)effect->definition_index].definition;
    if (!keep_locations)
        effect_delete_locations(effect_index);
    effect_marker_list marker_list;
    marker_list.marker_count = vector_count;
    marker_list.markers = (const s_effect_marker *)vectors;
    if (object_index != -1) {
        marker_list.node_index = (short)(marker_index == -1 ? 0 : marker_index);
        if (definition->flags.test(_effect_definition_node_position_only_bit)) {
            marker_list.node_matrix.scale = 1.0f;
            marker_list.node_matrix.rotation = global_identity4x3->rotation;
            marker_list.node_matrix.position = object_get_node_matrix(object_index, marker_list.node_index)->position;
        } else {
            memcpy(&marker_list.node_matrix, object_get_node_matrix(object_index, marker_list.node_index), sizeof(real_matrix4x3));
        }
    } else {
        marker_list.node_index = -1;
    }
    for (long location_block_index = 0; location_block_index < definition->locations.count; location_block_index++) {
        object_marker markers[16];
        long first_person_flags = 0;
        long marker_name = ((const effect_location_definition *)tag_block_get_element_with_size(
                                &definition->locations, location_block_index, sizeof(effect_location_definition)))
                               ->marker_name;
        long marker_count = 0;
        if (marker_name == _string_id_replace)
            marker_name = effect->input_marker_name;
        if (marker_name != 0) {
            if (vector_count > 0) {
                long vector_index = effects_vector_find(marker_name, vector_count, vectors);
                if (vector_index != -1) {
                    effect_marker_list_get_marker(&markers[marker_count], &marker_list, (short)vector_index, effects_effect_get_plane(effect));
                    if (marker_name == _string_id_child)
                        markers[marker_count & 15].node_index = -1;
                    marker_count++;
                }
            }
            if (marker_count == 0 && object_index != -1) {
                marker_count = internal_object_get_markers_by_string_id(object_index, marker_name, markers, 16, false);
                if (marker_count == 0 && marker_name == _string_id_root) {
                    marker_count = 1;
                } else if (marker_count > 0 && effect->explicit_marker_index != 0xff) {
                    markers[0] = markers[effect->explicit_marker_index % marker_count];
                    marker_count = 1;
                }
                if (first_person) {
                    object_marker *first_person_markers = &markers[marker_count];
                    long first_person_count = first_person_weapon_get_markers_by_string_id(
                        object_index, marker_name, first_person_markers, (short)(16 - marker_count));
                    if (first_person_count != 0) {
                        if (definition->flags.test(_effect_definition_first_person_visibility_bit)) {
                            if (g_effect_location_warning_count < 100)
                                g_effect_location_warning_count++;
                        } else {
                            if (effect->explicit_marker_index != 0xff) {
                                *first_person_markers = markers[effect->explicit_marker_index % first_person_count + marker_count];
                                first_person_count = 1;
                            }
                            for (long index = 0; index < first_person_count; index++)
                                first_person_flags |= 1 << (index + marker_count);
                            marker_count += first_person_count;
                        }
                    }
                }
            }
            if (marker_count == 0 && default_vector_index != -1) {
                effect_marker_list_get_marker(&markers[marker_count], &marker_list, (short)default_vector_index, effects_effect_get_plane(effect));
                marker_count++;
            }
        }
        if (!keep_locations) {
            const effect_location_definition *location_definition =
                (const effect_location_definition *)tag_block_get_element_with_size(
                    &definition->locations, location_block_index, sizeof(effect_location_definition));
            bool deterministic = effect->flags.test(_effect_deterministic_bit);
            for (long index = 0; index < marker_count; index++) {
                long limit_type = location_definition->limit_type;
                if ((unsigned long)(g_effect_counts[deterministic].location_count + 1) <=
                    ((const s_effect_limit *)tag_block_get_element_with_size(
                         &g_effect_budget_limits->table->blocks[2].tag_block, limit_type, sizeof(s_effect_limit)))
                        ->maximum_count)
                {
                    g_effect_location_limit_reached[limit_type] = true;
                    effects_location_new(effect, (short)location_block_index);
                } else {
                    g_effect_location_limit_reached[limit_type] = false;
                }
            }
        }
        long location_index = effect->location_indices[location_block_index];
        for (long index = 0; index < marker_count; index++) {
            if (location_index != -1) {
                effect_location_datum *location = effects_location_get(location_index);
                const object_marker *marker = &markers[index];
                location->node_index = (short)marker->node_index == -1
                    ? -1
                    : ((short)marker->node_index & 0x7fff) | ((first_person_flags & (1 << index)) != 0 ? 0x8000 : 0);
                location->matrix = marker->node_matrix;
                location_index = location->next_location_index;
                location->first_render_location_index = -1;
            }
        }
    }
    long looping_sound_index = effect->looping_sound_index;
    if (looping_sound_index != -1) {
        bool stop_sound = false;
        if (definition->location == -1) {
            stop_sound = true;
        } else {
            long location_index = effect->location_indices[definition->location];
            if (!effect_location_get_next_instance(effect, &location_index, 0))
                stop_sound = true;
        }
        if (stop_sound) {
            effect_looping_sound_stop(looping_sound_index);
            effect->looping_sound_index = -1;
        }
    }
}
static __forceinline effect_datum *effects_model_effect_try_and_get(long effect_index)
{
    effect_datum *result = 0;
    if (effect_index != -1) {
        const s_data_array *array = ::effect_data;
        int absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(effect_index);
        int identifier = effect_index >> 16;
        if (absolute_index < array->first_unallocated) {
            short *datum = (short *)(array->data + absolute_index * array->size);
            if (*datum != 0 && *datum == identifier)
                result = (effect_datum *)datum;
        }
    }
    return result;
}
// @address 0x31fa68 ?effects_object_model_state_changed@@YAXJ@Z
void effects_object_model_state_changed(long object_index)
{
    object_datum *object = object_get(object_index);
    const s_object_definition *definition = TAG_GET('obje', const s_object_definition, object->definition_index);
    if (definition->attachments.count > 0) {
        s_object_datum_attachment *attachments = (s_object_datum_attachment *)((char *)object + object->attached_lights.offset);
        for (long attachment_index = 0; attachment_index < definition->attachments.count; attachment_index++) {
            s_object_datum_attachment *attachment = &attachments[attachment_index];
            if (attachment->type == 2) {
                long effect_index = attachment->datum_index;
                effect_datum *effect = effects_model_effect_try_and_get(attachment->datum_index);
                if (effect) {
                    effect->flags.set(_effect_model_state_changed_bit, true);
                    long effect_object_index = effect->object_index;
                    real_point3d origin;
                    s_effect_vector vectors[2];
                    object_get_origin(effect_object_index, &origin);
                    build_gravity_effect_vectors(&origin, vectors);
                    effect_update_locations(effect_index, effect_object_index, effect->first_person_users != 0, vectors, 2, -1, 0);
                }
            }
        }
    }
}

static __forceinline bool effects_notify_was_users(effect_datum *effect)
{
    return (signed char)effect->first_person != 0;
}
static __forceinline bool effects_notify_arg_users(effect_datum *effect)
{
    return (signed char)effect->first_person != 0;
}
static __forceinline long effects_notify_trailing_zeros(long users)
{
    long result = 0;
    while (!(users & 1)) {
        users >>= 1;
        result++;
    }
    return result;
}
static __forceinline long effects_notify_lowest_user(long users)
{
    long result;
    if (users == 0)
        result = -1;
    else
        result = effects_notify_trailing_zeros(users);
    return result;
}
// @address 0x1120e8 ?effects_notify_first_person@@YAXJJ_N@Z
void effects_notify_first_person(long user_index, long object_index, bool first_person)
{
    for (long effect_index = ::data_next_index(effect_data, -1); effect_index != -1;
        effect_index = inl_data_next_index(::effect_data, effect_index))
    {
        effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
        signed char *users = &effect->first_person_users;
        if (first_person) {
            long effect_object_index = effect->object_index;
            if (effect_object_index == object_index && effect->flags.test(_effect_first_person_bit)) {
                bool was_first_person = effects_notify_was_users(effect);
                effect->first_person_users |= 1 << user_index;
                if (!was_first_person)
                    effect_update_locations(effect_index, effect_object_index, effects_notify_arg_users(effect), 0, 0, -1, 0);
            }
        } else if ((*users & (1 << user_index)) && effect->object_index == object_index) {
            *users &= ~(1 << user_index);
            if (!*users)
                effect_delete_first_person_locations(effect);
        }
        effect->first_person_user_index = (signed char)effects_notify_lowest_user(*users);
    }
}

// @address 0x233f30 effect_create_on_object
extern "C" void effect_create_on_object(
    c_tag_index definition_index, const s_damage_owner *, long object_index, float scale_a, float scale_b,
    const real_rgb_color *, const effect_vector_field *, long, long marker_index)
{
    s_effect_creation_data creation_data;
    creation_data.flags = 0;
    effect_creation_data_initialize(&creation_data);
    creation_data.scale_a = scale_a;
    creation_data.flags |= 4;
    creation_data.scale_b = scale_b;
    creation_data.definition_index = definition_index;
    creation_data.flags |= 1;
    creation_data.structure_bsp_index = object_index;
    creation_data.reserved54 = 0;
    creation_data.reserved58 = 0;
    real_point3d origin;
    object_get_origin(object_index, &origin);
    s_effect_vector vectors[3];
    build_gravity_effect_vectors(&origin, vectors);
    vectors[2].marker_name = _string_id_child;
    creation_data.effect_vectors = vectors;
    creation_data.effect_vector_count = 3;
    creation_data.marker_index = marker_index;
    vectors[2].vector = *global_up3d;
    vectors[2].point = *global_origin3d;
    effect_new_from_creation_data(&creation_data);
}



// @address 0xf76e0 effects_render_pass
extern "C" void effects_render_pass(long user_index, long pass)
{
    c_rasterizer::set_z_buffer_mode(pass != 0 ? c_rasterizer::_z_buffer_mode_read_only : c_rasterizer::_z_buffer_mode_normal);
    if (pass == 1) {
        for (long effect_index = ::data_next_index(effect_data, -1); effect_index != -1;
            effect_index = inl_data_next_index(::effect_data, effect_index))
        {
            effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
            if (!effect->flags.test(_effect_stopping_bit) && !effect->flags.test(_effect_culled_bit) && !effect->flags.test(_effect_marked_for_deletion_bit))
                effect_create_lens_flares(effect_index, user_index);
        }
    }
    effect_submit_contrail_systems(user_index, pass);
    effect_submit_contrail_profiles(user_index, pass);
    effects_render(user_index, pass);
    s_effects_particle_system_list *list = &g_particle_system_lists[pass];
    for (long i = 0; i < list->count; i++) {
        s_effects_particle_system_entry *entry = &list->entries[i];
        c_particle_system *system = (c_particle_system *)(((s_data_array *)g_particle_system_data_array)->data +
            DATUM_INDEX_TO_ABSOLUTE_INDEX(entry->particle_system_index) * 88);
        particle_system_render(system, user_index, entry->first_person, pass);
    }
}


// @address 0x33e1a0 effect_damage_batch_flush
extern "C" void effect_damage_batch_flush(c_render_batch *batch)
{
    c_render_batch *cache = batch;
    long index = 0;
    if (batch->count > 0) {
        s_effect_damage_batch_entry *entry = cache->entries;
        do {
            s_damage_data damage;
            damage_data_new(&damage, entry->definition_index);
            damage.scale[0] = 1.0f;
            damage.owner = entry->owner;
            damage.location = entry->location;
            damage.position = entry->position;
            damage.proximity = (damage_proximity_object *)batch;
            damage.origin = entry->position;
            damage.reporting_info = entry->type;
            damage.direction = global_identity4x3->up;
            damage.cone_direction = global_identity4x3->up;
            const s_damage_effect_definition *effect_definition =
                TAG_GET('jpt!', const s_damage_effect_definition, entry->definition_index);
            batch->selected_index = index;
            batch->minimum_distance = effect_definition->radius.lower;
            float maximum_distance = effect_definition->radius.upper;
            batch->maximum_distance = maximum_distance;
            batch->search_radius = maximum_distance + 1.5f;
            area_of_effect_cause_damage(&damage, -1, -1);
            index++;
            entry++;
        } while (index < batch->count);
    }
    batch->count = 0;
    batch->count2 = 0;
}

static __forceinline float effects_common_dot(const real_vector3d *a, const real_vector3d *b)
{
    return (a->i * b->i + a->k * b->k) + a->j * b->j;
}
static __forceinline real_vector3d *effects_common_reflect(const real_vector3d *incident, const real_vector3d *normal, real_vector3d *result)
{
    float scale = 2.0f * effects_common_dot(incident, normal);
    result->i = incident->i - scale * normal->i;
    result->j = incident->j - scale * normal->j;
    result->k = incident->k - scale * normal->k;
    return result;
}
static __forceinline float effects_common_normalize(real_vector3d *vector)
{
    float length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
    if (!(fabsf(length) < k_effects_common_epsilon))
        scale_vector3d(vector, 1.0f / length, vector);
    return length;
}
static __forceinline real_point3d *effects_common_point_from_line(
    const real_point3d *point, const real_vector3d *direction, float distance, real_point3d *result)
{
    result->x = point->x + distance * direction->i;
    result->y = point->y + distance * direction->j;
    result->z = point->z + distance * direction->k;
    return result;
}
// @address 0xf6a10 ?build_common_effect_vectors@@YAXPBTreal_point3d@@PBTreal_vector3d@@1PAUs_effect_vector@@@Z
void build_common_effect_vectors(const real_point3d *point, const real_vector3d *forward, const real_vector3d *up, s_effect_vector *effect_vectors)
{
    s_water_physics_test_sphere_result water_result;
    long marker_names[] = {
        _string_id_normal, _string_id_forward, _string_id_backward, _string_id_reflection,
        _string_id_gravity, _string_id_up, _string_id_water_surface
    };
    for (long i = 0; i < 7; i++) {
        effect_vectors[i].point = *point;
        effect_vectors[i].marker_name = marker_names[i];
    }
    effect_vectors[0].vector = *up;
    effect_vectors[1].vector = *forward;
    scale_vector3d(forward, -1.0f, &effect_vectors[2].vector);
    effects_common_reflect(&effect_vectors[1].vector, &effect_vectors[0].vector, &effect_vectors[3].vector);
    effects_common_normalize(&effect_vectors[3].vector);
    effect_vectors[0].vector = *up;
    effect_vectors[4].vector = *global_down3d;
    effect_vectors[5].vector = *global_up3d;
    if (water_physics_test_sphere(true, point, 0.0f, &water_result)) {
        effects_common_point_from_line(point, global_up3d, water_result.depth, &effect_vectors[6].point);
        effect_vectors[6].vector = water_result.surface_plane.normal;
    } else {
        effect_vectors[6].vector = *global_up3d;
    }
}


// @address 0x234438 effect_update_from_attached_particle
// match: 0x234438: pointer __restrict qualifier prevents alias reload
extern "C" void effect_update_from_attached_particle(effect_datum *__restrict effect)
{
    if (effect->attachment_index != -1)
    {
        const c_particle *particle = (const c_particle *)effects_particle_try_and_get(effect->attachment_index);
        if (particle)
        {
            real_vector3d direction = particle->m_velocity;
            normalize3d_with_default(&direction, global_down3d);
            s_effect_vector vectors[7];
            build_common_effect_vectors(&particle->m_position, &direction, global_up3d, vectors);
            effect_marker_list marker_list;
            marker_list.marker_count = 7;
            marker_list.markers = (const s_effect_marker *)vectors;
            object_marker markers[7];
            const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
            marker_list.node_index = 0xffff;
            object_marker *marker = markers;
            long i = 0;
            {
                for (; i < definition->locations.count; ++marker, ++i)
                {
                    long index = effect->location_indices[i];
                    long marker_name = ((const effect_location_definition *)definition->locations.address)[i].marker_name;
                    if (index != -1)
                    {
                        effect_location_datum *location = effects_location_get(index);
                        long j = 0;
                        const s_effect_vector *v = vectors;
                        do
                        {
                            if (v->marker_name == marker_name)
                                break;
                            ++j;
                            ++v;
                        } while (j < 7);
                        effect_marker_list_get_marker(marker, &marker_list, (short)i, 0);
                        location->matrix = marker->node_matrix;
                    }
                }
            }
            particle_state_list_get_state_values((const c_particle *)particle, 0xef803, effect->particle_scale_a_state,
                effect->particle_scale_b_state, &effect->scale_a, &effect->scale_b);
        }
    }
}


// @address 0xf7e08 ?effects_frame_advance@@YAXM@Z
void effects_frame_advance(float p0)
{
    csmemset(&g_effect_camera_info, 0, sizeof(g_effect_camera_info));
    c_visible_items::clear_all();
    for (long effect_index = data_next_index(::effect_data, -1); effect_index != -1;
        effect_index = inl_data_next_index(::effect_data, effect_index))
    {
        effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
        effect_update_lighting_samples(effect);
        effect_update_from_attached_particle(effect);
    }
    c_particle_system::frame_advance_all(p0);
    c_contrail_system::frame_advance_all(p0);
    contrail_owners_frame_advance_all(p0);
    c_light_volume_system::frame_advance_all(p0);
    c_decal_system::frame_advance_all(p0);
}

// @address 0x233e08 ?effect_new_from_object@@YAJVc_tag_index@@PBUs_damage_owner@@JJMMPBTreal_rgb_color@@PBUeffect_vector_field@@J@Z
long effect_new_from_object(class c_tag_index p0, struct s_damage_owner const *p1, long p2, long p3, float p4, float p5,
    union real_rgb_color const *p6, struct effect_vector_field const *p7, long p8)
{
    real_point3d origin;
    s_effect_creation_data creation_data;
    s_effect_vector effect_vectors[3];

    creation_data.flags = 0;
    effect_creation_data_initialize(&creation_data);
    creation_data.definition_index = p0;
    creation_data.structure_bsp_index = p3;
    if (p8 == 1)
        creation_data.flags |= 4;
    else
        creation_data.flags &= ~4;
    creation_data.scale_a = p4;
    creation_data.scale_b = p5;
    creation_data.reserved54 = 0;
    creation_data.reserved58 = 0;
    creation_data.flags |= 1;

    object_get_origin(p3, &origin);
    build_gravity_effect_vectors(&origin, effect_vectors);
    effect_vectors[2].marker_name = _string_id_child;
    creation_data.effect_vectors = effect_vectors;
    creation_data.effect_vector_count = 3;
    effect_vectors[2].vector = *global_up3d;
    effect_vectors[2].point = *global_origin3d;
    if (p1 != 0)
    {
        creation_data.damage_owner = *p1;
    }
    creation_data.damage_reporting_type = p2;
    return effect_new_from_creation_data(&creation_data);
}

// @address 0x116060 ?effects_update@@YAXXZ
void effects_update()
{
    g_effect_damage_batch = 0;
    effects_flush_pending_effects();
    float delta_time = game_time_globals->tick_length;
    for (long slot_index = ::data_next_index((s_data_array *)g_effect_lightprobe_slot_data, -1); slot_index != -1;
        slot_index = inl_data_next_index((s_data_array *)g_effect_lightprobe_slot_data, slot_index))
    {
        s_effect_lightprobe_slot *slot = datum_get<s_effect_lightprobe_slot>(g_effect_lightprobe_slot_data, slot_index);
        if (slot->effect_index == -1) {
            slot->weight -= delta_time;
            if (slot->weight < 0.0f)
                ::datum_delete((s_data_array *)g_effect_lightprobe_slot_data, slot_index);
        }
    }
    c_render_batch batch1;
    g_effect_damage_batch = &batch1;
    long index = inl_data_next_index(::effect_data, -1);
    for (; index != -1; index = inl_data_next_index(::effect_data, index)) {
        effect_datum *effect = datum_get<effect_datum>(::effect_data, index);
        const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
        if (!definition->flags.test(_effect_definition_first_person_visibility_bit))
            break;
        unsigned char done = effect_update_object(index);
        if (!done)
            done = effect_update_visibility(index);
        if (!done)
            effect_update_time(index, delta_time);
    }
    g_effect_damage_batch = 0;
    effect_damage_batch_flush(&batch1);
    c_render_batch batch2;
    g_effect_damage_batch = &batch2;
    for (; index != -1; index = inl_data_next_index(::effect_data, index)) {
        unsigned char done = effect_update_object(index);
        if (!done)
            done = effect_update_visibility(index);
        if (!done)
            effect_update_time(index, delta_time);
    }
    g_effect_damage_batch = 0;
    effect_damage_batch_flush(&batch2);
    effect_update_attachments(delta_time);
}

// @address 0x3e820 k_effect_creation_random_name
// @address 0x237028 effect_update_contrail_locations
extern "C" void effect_update_contrail_locations(const effect_datum *effect, s_effect_update_location *record, float value)
{
    c_contrail_states states;
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    const effect_event_definition *event_definition = &((const effect_event_definition *)definition->events.address)[record->event_block_index];
    const effect_part_definition *part = &((const effect_part_definition *)event_definition->parts.address)[record->part_block_index];
    long location_index = part->location_index;
    short render_type = part->render_type;
    if (location_index != -1) {
        long previous_location_index = -1;
        long current_location_index = record->first_location;
        long effect_location_index = effect_location_find_next_of_type(effect, effect->location_indices[location_index], render_type);
        while (effect_location_index != -1) {
            const effect_location_datum *location = effects_location_get(effect_location_index);
            real_matrix4x3 part_to_world;
            effect_get_part_to_world_matrix(effect, part, location, &part_to_world);
            if (current_location_index == -1) {
                unsigned long seed = (unsigned long)(effect_real_random_range("event-request-contrail-system", false, effect, 0.0f, 1.0f) * 4294967296.0f);
                current_location_index = record->add_contrail_location(&part_to_world, seed);
            }
            if (current_location_index != -1) {
                c_contrail_location *contrail_location =
                    c_contrail_location::get(current_location_index);
                long render_location_index = location->first_render_location_index;
                while (render_location_index != -1) {
                    const s_effect_render_location *render_location = &g_effect_render_locations[render_location_index];
                    contrail_location_update_matrix(contrail_location, value, (const real_matrix4x3 *)render_location->data);
                    states.set_contrail_system(record);
                    contrail_location_pulse(contrail_location, 0.0f, true, &states);
                    render_location_index = render_location->next_index;
                }
                contrail_location_update_matrix(contrail_location, value, &part_to_world);
                previous_location_index = current_location_index;
                current_location_index = contrail_location->sibling;
            }
            effect_location_index = effect_location_find_next_of_type(effect, location->next_location_index, render_type);
        }
        if (current_location_index != -1) {
            long removed_location_index;
            if (previous_location_index != -1) {
                c_contrail_location *previous_location =
                    c_contrail_location::get(previous_location_index);
                removed_location_index = previous_location->sibling;
                previous_location->sibling = -1;
            } else {
                removed_location_index = record->first_location;
                record->first_location = -1;
            }
            if (removed_location_index != -1)
                c_contrail_location::destroy(removed_location_index);
        }
    }
}


static __forceinline bool effects_creation_flag_test(const s_effect_creation_data *creation_data, long bit)
{
    return ((const c_flags_no_init<long, unsigned long, 5> *)&creation_data->flags)->test(bit);
}
static __forceinline bool effects_weapon_age_external(long weapon_index)
{
    const object_datum *weapon = object_get(weapon_index);
    const struct weapon_definition *weapon_definition =
        TAG_GET('weap', const struct weapon_definition, weapon->definition_index);
    bool depleted = weapon_has_infinite_ammo(weapon_index);
    float age_per_round;
    if (!game_is_campaign() || (age_per_round = weapon_definition->campaign_age_per_round) <= 0.0f)
        age_per_round = weapon_definition->age_per_round;
    if (age_per_round > 0.0f)
        weapon_age(weapon_index, age_per_round);
    return depleted;
}
static __forceinline void effects_find_reusable_instance(const s_effect_creation_data *creation_data, long *out_reuse_mode, long *out_effect_index)
{
    *out_reuse_mode = 0;
    *out_effect_index = -1;
    if (creation_data->position &&
        (TAG_GET('effe', const s_effect_definition, creation_data->definition_index))->maximum_separation > 0.0f)
    {
        for (long candidate_index = ::data_next_index(effect_data, -1); candidate_index != -1;
            candidate_index = ::data_next_index(effect_data, candidate_index))
        {
            *out_reuse_mode = effect_get_reuse_mode(creation_data, candidate_index);
            if (*out_reuse_mode != 0) {
                *out_effect_index = candidate_index;
                break;
            }
        }
    }
}
static __forceinline bool effects_location_valid(const s_location *location)
{
    return location->cluster_reference.bsp_index != -1;
}
static __forceinline long effects_first_person_weapon_next_user(long weapon_index, long previous_user_index)
{
    long result = -1;
    for (long user_index = previous_user_index + 1; user_index < 4; ++user_index) {
        const s_first_person_weapon_user_data *user = &((const s_first_person_weapon_user_data *)g_first_person_weapon_data)[user_index];
        for (long slot = 0; slot < 2; ++slot) {
            if (weapon_index == user->weapons[slot].object_indices[2]) {
                result = user_index;
                break;
            }
        }
    }
    return result;
}
static __forceinline void effects_set_scales(effect_datum *effect, float scale_a, float scale_b)
{
    effect->scale_a = scale_a;
    effect->scale_b = scale_b;
}
static __forceinline bool effects_object_is_nonviolent_corpse(long object_index)
{
    bool nonviolent = false;
    const unit_datum *biped = (const unit_datum *)object_try_and_get_and_verify_type(object_index, _object_mask_biped);
    if (biped && biped->object.m_damage_flags.test(_object_damage_body_depleted_bit))
        nonviolent = biped->unit.type_change_time != -1 && game_time_globals->ticks_per_second + biped->unit.type_change_time < game_time_globals->time;
    return nonviolent;
}
static __forceinline bool effects_test_underwater(const real_point3d *point)
{
    s_water_physics_test_sphere_result water_result;
    return water_physics_test_sphere(true, point, 0.0f, &water_result);
}
// @address 0xf6dd8 ?effect_new_from_creation_data@@YAJPBUs_effect_creation_data@@@Z
long effect_new_from_creation_data(const s_effect_creation_data *creation_data)
{
    long effect_index = -1;
    if (creation_data->definition_index != -1) {
        bool weapon_aged_out = false;
        object_datum *object = 0;
        const s_object_definition *object_definition = 0;
        if (creation_data->structure_bsp_index != -1) {
            // match: 0xf6dd8: manual header index keeps the lwz+rlwinm+lwzx flow; object_get CSEs the index load away.
            object =
                ((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(creation_data->structure_bsp_index)].datum;
            if (object)
                object_definition = TAG_GET('obje', const s_object_definition, object->definition_index);
        }
        if (creation_data->definition_index != -1 &&
            creation_data->structure_bsp_index != -1 &&
            (TAG_GET('effe', const s_effect_definition, creation_data->definition_index))
                ->flags.test(_effect_definition_ages_weapon_bit) &&
            object &&
            ((1 << object->object_identifier.get_type()) & _object_mask_unit)) {
            const unit_datum *unit =
                (const unit_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(creation_data->structure_bsp_index)].datum;
            long weapon_index = unit_inventory_try_get_weapon(creation_data->structure_bsp_index, unit->unit.current_weapon_index);
            if (weapon_index != -1)
                weapon_aged_out = effects_weapon_age_external(weapon_index);
        }
        if (!weapon_aged_out && (effects_creation_flag_test(creation_data, 2) || effect_in_player_range(creation_data))) {
            long reuse_mode;
            effects_find_reusable_instance(creation_data, &reuse_mode, &effect_index);
            if (reuse_mode == 0)
                effect_index = effect_allocate(creation_data);
            if (effect_index != -1) {
                effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
                const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
                effect->owner_type = creation_data->location_index;
                effect->part_counter = 0;
                effect->input_marker_name = creation_data->parent_effect_index;
                effect->explicit_marker_index = (unsigned char)creation_data->marker_index;
                effect->death_delay = 0.0f;
                location_invalidate(&effect->location);
                effect->flags.set(_effect_attached_to_camera_bit, effects_creation_flag_test(creation_data, 3));
                effect->flags.set(_effect_first_person_only_bit, effects_creation_flag_test(creation_data, 4));
                effect->flags.set(_effect_no_sound_bit, creation_data->region_permutation_index & 1);
                if (game_is_multiplayer() && object_definition)
                    effect->flags.set(_effect_no_objects_bit, (object_definition->flags >> 13) & 1);
                if (effects_creation_flag_test(creation_data, 0)) {
                    effect->object_index = creation_data->structure_bsp_index;
                    if (object) {
                        const unit_datum *unit = (const unit_datum *)object_try_and_get_and_verify_type(
                            effect->object_index, _object_mask_unit);
                        if (unit && unit->unit.player_index != -1) {
                            long player_index = unit->unit.player_index;
                            effect->flags.set(_effect_first_person_bit, true);
                            for (long user_index = player_get_first_output_user(player_index); user_index != -1;
                                user_index = (long)director_get_perspective(player_index, user_index))
                                effect->first_person_user_flags |= 1 << user_index;
                        }
                        if (object->object_identifier.get_type() == _object_type_weapon) {
                            effect->flags.set(_effect_first_person_bit, true);
                            long weapon_index = effect->object_index;
                            for (long user_index = first_person_weapon_find_user_index_from_weapon(weapon_index); user_index != -1;
                                user_index = effects_first_person_weapon_next_user(weapon_index, user_index))
                                effect->first_person_users |= 1 << user_index;
                            effect->first_person_user_index = (signed char)effects_notify_lowest_user(effect->first_person_users);
                        }
                    }
                } else {
                    effect->velocity = creation_data->velocity;
                }
                if (effects_creation_flag_test(creation_data, 1)) {
                    effect->flags.set(_effect_looping_bit, true);
                    effect->scale_a_function = creation_data->lightprobe_slot_index;
                    effect->scale_b_function = creation_data->user_mask;
                } else {
                    effects_set_scales(effect, creation_data->scale_a, creation_data->scale_b);
                    effect->flags.set(_effect_cannot_be_restarted_bit,
                        definition->flags.test(_effect_definition_cannot_be_restarted_bit));
                }
                if (effect->object_index != -1 && g_effect_cull_nonviolent_corpses && effects_object_is_nonviolent_corpse(effect->object_index))
                    effect->flags.set(_effect_nonviolent_bit, true);
                effect->attachment_index = creation_data->object_index;
                if (effect->attachment_index != -1) {
                    effect->particle_scale_a_state = creation_data->particle_scale_a_state;
                    effect->particle_scale_b_state = creation_data->particle_scale_b_state;
                }
                effect_update_locations(effect_index, effect->object_index, effect->first_person_users != 0,
                    creation_data->effect_vectors, creation_data->effect_vector_count, creation_data->node_index, reuse_mode != 0);
                if (effect->object_index != -1 && effects_creation_flag_test(creation_data, 0)) {
                    effect_update_location_from_object(effect);
                } else if (effects_location_valid(&creation_data->location_hint)) {
                    effect->location = creation_data->location_hint;
                } else {
                    effect_get_location_from_object(effect);
                }
                if (effect->lightprobe_slot_index != -1) {
                    effect_initialize_lightprobe_slot(effect_index, creation_data);
                    effect_send_sample_lightmap_message(effect_index);
                }
                if (definition->flags.test(_effect_definition_underwater_test_bit)) {
                    real_matrix4x3 matrix;
                    const real_matrix4x3 *location = effect_get_first_location_matrix(effect, &matrix);
                    effect->flags.set(_effect_underwater_bit, location ? effects_test_underwater(&location->position) : false);
                }
                if (creation_data->impact_coordinates)
                    effect->impact_coordinates = *creation_data->impact_coordinates;
                if (creation_data->lightprobe_source
                    && creation_data->lightprobe_source->type != _collision_result_structure
                    && creation_data->lightprobe_source->type != _collision_result_instanced_geometry)
                    effect->flags.set(_effect_no_decals_bit, true);
                if (reuse_mode != 0) {
                    effect_ping(effect_index, creation_data->definition_index, creation_data->scale_a, creation_data->scale_b, reuse_mode == 2);
                    if (reuse_mode == 2)
                        effect_refresh_particles(effect_index);
                } else {
                    effect_start(effect_index);
                    float time_step =
                        (definition->flags.test(_effect_definition_force_looping_bit) && !effect->is_looping())
                        ? k_effect_non_looping_death_delay
                        : 0.0f;
                    effect_update(effect_index, time_step);
                    if (!datum_try_and_get(::effect_data, effect_index))
                        effect_index = -1;
                }
            }
        }
    }
    return effect_index;
}



static __forceinline long effect_new_unattached_from_markers_lite(c_tag_index definition_index, const s_damage_owner *damage_owner,
    unsigned char damage_reporting_type, long vector_count, const s_effect_vector *vectors, const real_plane3d *plane,
    const s_location *hint, e_effect_deterministic deterministic)
{
    s_effect_creation_data creation_data;
    creation_data.flags = 0;
    effect_creation_data_initialize_from_vectors(&creation_data, definition_index, vector_count, vectors, plane, hint, deterministic);
    if (damage_owner)
        creation_data.damage_owner = *damage_owner;
    creation_data.damage_reporting_type = damage_reporting_type;
    return effect_new_from_creation_data(&creation_data);
}

// @address 0x233d30 effect_new_unattached_from_point
extern "C" int effect_new_unattached_from_point(c_tag_index definition_index, char *point_ptr, char *forward_ptr, char *up_ptr,
    int match_all_markers, int deterministic, char *plane_ptr, char *hint_ptr)
{
    // match: 0x233d30: char* params; typed point/plane/location params grow the caller unit_cause_melee_damage_response from 532 to 536 bytes.
    const real_point3d *point = (const real_point3d *)point_ptr;
    const real_vector3d *forward = (const real_vector3d *)forward_ptr;
    const real_vector3d *up = (const real_vector3d *)up_ptr;
    const real_plane3d *plane = (const real_plane3d *)plane_ptr;
    const s_location *hint = (const s_location *)hint_ptr;
    long effect_index = -1;
    if (definition_index != -1 && point) {
        s_effect_vector effect_vectors[8];
        long effect_vector_count = 7;
        build_common_effect_vectors(point, forward, up, effect_vectors);
        if (match_all_markers == 1) {
            effect_vectors[7].point = *point;
            effect_vectors[7].vector = *forward;
            effect_vectors[7].marker_name = _string_id_any;
            effect_vector_count++;
        }
        effect_index = effect_new_unattached_from_markers_lite(
            definition_index, 0, 0, effect_vector_count, effect_vectors, plane, hint, (e_effect_deterministic)deterministic);
    }
    return effect_index;
}

// @address 0x234020 ?effect_new_attached_from_marker_name@@YAJVc_tag_index@@PBUs_damage_owner@@JJFJMMPBTreal_rgb_color@@PBUeffect_vector_field@@JJ@Z
long effect_new_attached_from_marker_name(c_tag_index definition_index, const s_damage_owner *damage_owner, long damage_reporting_type,
    long object_index, short node_index, long marker_name, float scale_a, float scale_b, const real_rgb_color *change_color,
    const effect_vector_field *vector_field, long region_permutation_index, long deterministic)
{
    s_effect_creation_data creation_data;
    object_marker markers[1];
    s_effect_vector vectors[2];

    creation_data.flags = 0;
    effect_creation_data_initialize(&creation_data);

    creation_data.flags |= 1;
    if (deterministic == 1)
        creation_data.flags |= 4;
    else
        creation_data.flags &= ~4;
    creation_data.definition_index = definition_index;
    creation_data.region_permutation_index = region_permutation_index;
    if (damage_owner != 0)
        creation_data.damage_owner = *damage_owner;
    creation_data.damage_reporting_type = (unsigned char)damage_reporting_type;
    creation_data.structure_bsp_index = object_index;
    creation_data.node_index = node_index;
    creation_data.parent_effect_index = marker_name;
    creation_data.scale_a = scale_a;
    creation_data.scale_b = scale_b;
    creation_data.reserved54 = 0;
    creation_data.reserved58 = 0;

    internal_object_get_markers_by_string_id(object_index, marker_name, markers, 1, false);
    build_gravity_effect_vectors(&markers[0].world_matrix.position, vectors);
    creation_data.effect_vectors = vectors;
    creation_data.effect_vector_count = 2;
    return effect_new_from_creation_data(&creation_data);
}


// @address 0x234360 effect_new_from_breakable_surface
extern "C" long effect_new_from_breakable_surface(c_tag_index definition_index, long event_index, float unused_scale, float scale)
{
    s_breakable_surfaces_broken_event *event =
        datum_get<s_breakable_surfaces_broken_event>(g_breakable_surface_event_data, event_index);
    s_effect_vector vector;
    s_effect_creation_data creation_data;
    creation_data.flags = 0;
    effect_creation_data_initialize(&creation_data);
    creation_data.particle_system_index = event->material_index;
    creation_data.definition_index = definition_index;
    creation_data.scale_b = scale;
    creation_data.location_index = event_index;
    creation_data.effect_vectors = &vector;
    creation_data.effect_vector_count = 1;
    creation_data.scale_a = 1.0f;
    vector.vector = event->direction;
    vector.point = *global_origin3d;
    vector.marker_name = _string_id_up;
    creation_data.parent_effect_index = _string_id_up;
    return effect_new_from_creation_data(&creation_data);
}

static __forceinline s_effect_pool_node *effects_async_reverse_list(s_effect_pool_node *list)
{
    s_effect_pool_node *result = list;
    if (list) {
        s_effect_pool_node *previous = 0;
        do {
            s_effect_pool_node *next = list->free;
            list->free = previous;
            previous = list;
            list = next;
        } while (list);
        result = previous;
    }
    return result;
}
// @address 0x116310 effects_flush_pending_effects
extern "C" void effects_flush_pending_effects()
{
    s_effect_pool_node *list = (s_effect_pool_node *)InterlockedFlushSList(&g_effect_pending_pool.pending);
    __lwsync();
    s_effect_pool_node *node = effects_async_reverse_list(list);
    while (node) {
        effects_async_payload *payload = &node->creation;
        s_effect_vector vectors[7];
        build_common_effect_vectors(&payload->point, &payload->forward, &payload->up, vectors);
        c_tag_index definition = node->definition_index;
        s_effect_creation_data creation;
        creation.flags = 0;
        memset(&creation, 0, sizeof(creation));
        s_location location_hint = payload->location_hint;
        long object_index = payload->object_index;
        creation.scale_a = payload->scale_a;
        creation.node_index = -1;
        creation.scale_b = payload->scale_b;
        creation.structure_bsp_index = -1;
        creation.lightprobe_slot_index = 0;
        creation.location_hint = location_hint;
        creation.object_index = object_index;
        creation.user_mask = 0;
        creation.first_person_user_index = 0;
        creation.parent_effect_index = 0;
        creation.damage_owner = effects_creation_default_damage_owner();
        creation.marker_index = -1;
        creation.lightprobe_source = 0;
        creation.location_index = -1;
        creation.flags = 0;
        creation.definition_index = definition;
        creation.effect_vector_count = 7;
        creation.plane = 0;
        creation.effect_vectors = vectors;
        if (creation.object_index != -1) {
            creation.particle_scale_a_state = payload->lightprobe_slot;
            creation.particle_scale_b_state = payload->user_mask;
        }
        effect_new_from_creation_data(&creation);
        s_effect_pool_node *next = node->free;
        __lwsync();
        InterlockedPushEntrySList(&g_effect_pending_pool.free_list, node);
        node = next;
    }
}

long effect_new_weather(c_tag_index definition_index)
{
    s_effect_creation_data creation_data;
    s_effect_vector vectors[2];
    creation_data.flags = 0;
    effect_creation_data_initialize(&creation_data);
    creation_data.flags = 0;
    creation_data.flags |= 1 << 3;
    creation_data.flags |= 1 << 4;
    creation_data.flags |= 1 << 1;
    creation_data.definition_index = definition_index;
    creation_data.effect_vectors = vectors;
    creation_data.effect_vector_count = 2;
    build_gravity_effect_vectors(global_origin3d, vectors);
    return effect_new_from_creation_data(&creation_data);
}

void effect_refresh_location(long effect_index, const s_cluster_reference *cluster_reference)
{
    effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, effect_index);
    if (effect)
    {
        effect->cluster_reference = *cluster_reference;
        effect->leaf_index = 0;
    }
}

void effect_handle_deleted_atmosphere(long effect_index)
{
    effect_destroy(effect_index, true);
}


static __forceinline void effects_create_decal_system(c_tag_index definition_index, const s_effects_message_decal *parameters)
{
    decal_system_create(parameters->limit_type, -1, definition_index,
        parameters->object_index, false, &parameters->position, &parameters->velocity, 0);
}

static __forceinline void effects_handle_create_decal_system(const s_effect_message *message)
{
    const s_effects_message_decal *parameters = &message->decal;
    effects_create_decal_system(c_tag_index(parameters->definition_index), parameters);
}

// @address 0x23bf78 ?handle_effect_render_message@@YAXPAUs_effect_message@@J@Z
void handle_effect_render_message(s_effect_message *message, long parameter)
{
    switch (message->type)
    {
    case _effect_message_create_particle_system:
        event_create_particle_system(message);
        break;
    case _effect_message_add_particle_location:
        if (g_effect_last_particle_system_index != -1)
        {
            c_particle_system *system =
                datum_get<c_particle_system>(g_particle_system_data_array, g_effect_last_particle_system_index);
            long location_index = system->add_particle_location();
            if (location_index != -1)
            {
                ::c_particle_location *location = datum_get<c_particle_location>(g_particle_location_data_array, location_index);
                location->set_matrix(&message->light_volume.matrix);
                location->set_first_person((message->location_index & 1) != 0);
            }
        }
        break;
    case _effect_message_finalize_particle_system:
        if (g_effect_last_particle_system_index != -1)
        {
            c_particle_system *system =
                datum_get<c_particle_system>(g_particle_system_data_array, g_effect_last_particle_system_index);
            effect_update_particle_query_record(system);
        }
        g_effect_last_particle_system_index = -1;
        break;
    case _effect_message_create_contrail_system:
        event_create_contrail_system(message);
        break;
    case _effect_message_add_contrail_location:
        {
            const s_effects_message_location *parameters = &message->light_volume;
            if (g_effect_last_contrail_system_index != -1)
            {
                c_contrail_system *system =
                    datum_get<c_contrail_system>(g_contrail_system_data, g_effect_last_contrail_system_index);
                system->add_contrail_location(&parameters->matrix, parameters->random_seed);
            }
        }
        break;
    case _effect_message_finalize_contrail_system:
        if (g_effect_last_contrail_system_index != -1)
        {
            c_contrail_system *system = datum_get<c_contrail_system>(g_contrail_system_data, g_effect_last_contrail_system_index);
            system->query_effect(0.0f);
            if (system->get_should_pulse())
                system->pulse(0.0f, true);
        }
        g_effect_last_contrail_system_index = -1;
        break;
    case _effect_message_create_contrail_profile:
        event_create_contrail_profile(message);
        break;
    case _effect_message_add_contrail_profile_location:
        {
            s_effects_message_location *parameters = &message->light_volume;
            if (g_effect_last_contrail_profile_index != -1)
            {
                s_effect_contrail_profile *system =
                    datum_get<s_effect_contrail_profile>(g_particle_instance_owner_data, g_effect_last_contrail_profile_index);
                effect_contrail_profile_add_location(system, (s_effects_contrail_spawn_parameters *)parameters, parameters->random_seed);
            }
        }
        break;
    case _effect_message_finalize_contrail_profile:
        if (g_effect_last_contrail_profile_index != -1)
        {
            s_effect_contrail_profile *system =
                datum_get<s_effect_contrail_profile>(g_particle_instance_owner_data, g_effect_last_contrail_profile_index);
            effect_update_contrail_profile(system, 0.0f);
        }
        g_effect_last_contrail_profile_index = -1;
        break;
    case _effect_message_create_light_volume_system:
        event_create_light_volume_system(message);
        break;
    case _effect_message_add_light_volume_location:
        {
            const s_effects_message_location *parameters = &message->light_volume;
            if (g_effect_last_light_volume_system_index != -1)
            {
                c_light_volume_system *system =
                    datum_get<c_light_volume_system>(g_light_volume_system_data, g_effect_last_light_volume_system_index);
                long location_index = system->add_light_volume_location(&parameters->matrix, parameters->random_seed);
                if (location_index != -1)
                {
                    c_light_volume_location *location =
                        datum_get<c_light_volume_location>(g_light_volume_location_data, location_index);
                    location->set_first_person((message->location_index & 1) != 0);
                }
            }
        }
        break;
    case _effect_message_finalize_light_volume_system:
        if (g_effect_last_light_volume_system_index != -1)
        {
            c_light_volume_system *system =
                datum_get<c_light_volume_system>(g_light_volume_system_data, g_effect_last_light_volume_system_index);
            system->query_effect(0.0f);
        }
        g_effect_last_light_volume_system_index = -1;
        break;
    case _effect_message_create_decal_system:
        effects_handle_create_decal_system(message);
        break;
    case _effect_message_effect_xsynced_bit:
        {
            s_effect_definition *definition = TAG_GET('effe', s_effect_definition, message->definition_index);
            definition->flags.m_storage |= 1 << 13;
        }
        break;
    case _effect_message_global_xsynced_bit:
        break;
    case _effect_message_add_render_location:
        effect_add_render_location(message);
        break;
    case _effect_message_refresh_particles:
        {
            effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, message->effect_index);
            if (effect)
                effect->flags.m_storage |= 1 << 12;
        }
        break;
    case _effect_message_refresh_event_particles:
        if (effect_datum_try_and_get(::effect_data, message->effect_index))
        {
            event_datum *event = datum_get<event_datum>(g_event_data, message->event_index);
            event->flags.m_storage |= 1 << 3;
        }
        break;
    case _effect_message_sample_lightmap:
        {
            effect_datum *effect = (effect_datum *)effect_datum_try_and_get(::effect_data, message->effect_index);
            if (effect)
            {
                s_effect_lightprobe *lightprobe = &g_effect_lightprobe_memory
                    [datum_index_to_absolute_index(effect->lightprobe_slot_index)];
                lightprobe->flags.m_storage &= ~3;
                effect_update_lighting_samples(effect);
            }
        }
        break;
    default:
        // match: 0x23bf78: unreachable hint omits dead branch check
        __assume(0);
    }
}

static const float k_effects_water_test_radius = 0.01f;
static const float k_effects_water_event_distance = 40.0f;
static __forceinline bool effects_part_test_flag(const effect_part_definition *part, long bit)
{
    return (part->flags & (1 << bit)) != 0;
}


static __forceinline bool effects_generate_disposition_allowed(bool violent, short disposition)
{
    return violent ? disposition != 1 : disposition != 2;
}

static __forceinline long effects_generate_root_object(long object_index)
{
    while (object_index != -1)
    {
        const object_datum *object = object_get(object_index);
        if (!(bool)((object->flags >> 26) & 1))
            break;
        object_index = object->parent_object_index;
    }
    return object_index;
}

static __forceinline void effects_finish_contrail_system(long effect_index, long event_index, const effect_part_definition *part)
{
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    if (!event->flags.test(_event_stale_bit))
    {
        s_effect_message message(effect_index, event_index, part->limit_type);
        message.type = _effect_message_finalize_contrail_system;
        effect_send_render_message(&message);
    }
}

static __forceinline void effects_finish_light_volume_system(long effect_index, long event_index, const effect_part_definition *part)
{
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    if (!event->flags.test(_event_stale_bit))
    {
        s_effect_message message(effect_index, event_index, part->limit_type);
        message.type = _effect_message_finalize_contrail_profile;
        effect_send_render_message(&message);
    }
}

static __forceinline void effects_finish_beam_system(long effect_index, long event_index, const effect_part_definition *part)
{
    const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
    if (!event->flags.test(_event_stale_bit))
    {
        s_effect_message message(effect_index, event_index, part->limit_type);
        message.type = _effect_message_finalize_light_volume_system;
        effect_send_render_message(&message);
    }
}

// @address 0x239db0 ?event_generate_parts@@YAXJJ_N@Z
void event_generate_parts(long effect_index, long event_index, bool create)
{
    // match: 0x239db0: manual index keeps r9/r10 colors; datum_get renumbers IL and swaps them.
    const effect_datum *effect =
        &((const effect_datum *)::effect_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(effect_index)];
    const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
    const event_datum *event = &((event_datum *)g_event_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(event_index)];
    const effect_event_definition *event_definition = &((const effect_event_definition *)definition->events.address)[event->event_block_index];
    for (short part_index = 0; part_index < event_definition->parts.count; part_index++)
    {
        const effect_part_definition *part = &((const effect_part_definition *)event_definition->parts.address)[part_index];
        if (part->location_index >= 0 && part->location_index < definition->locations.count
            && (create || effects_part_test_flag(part, 5))
            && part->definition.index != -1 && part->location_index != -1)
        {
            bool environment_allowed;
            switch (part->environment)
            {
            case 0: environment_allowed = true; break;
            case 1: environment_allowed = !effect->flags.test(_effect_underwater_bit); break;
            case 2: environment_allowed = effect->flags.test(_effect_underwater_bit); break;
            case 3: environment_allowed = false; break;
            // match: 0x239db0: unreachable hint omits dead branch check
            default: __assume(0);
            }
            if (environment_allowed && effects_generate_disposition_allowed(effect->flags.test(_effect_nonviolent_bit), part->disposition))
            {
                switch (part->type)
                {
                case 'cntl': event_request_contrail_system(effect_index, event_index, part, part_index); break;
                case 'ltvl': event_request_light_volume_system(effect_index, event_index, part, part_index); break;
                case 'beam': event_request_beam_system(effect_index, event_index, part, part_index); break;
                }

                long location_index = effect_location_find_next_of_type(effect, effect->location_indices[part->location_index], part->render_type);
                while (location_index != -1)
                {
                    const effect_location_datum *location = effects_location_get(location_index);
                    bool unattached = effects_part_test_flag(part, 2);
                    real_matrix4x3 part_to_world;
                    effect_get_part_to_world_matrix(effect, part, location, &part_to_world);
                    if (effects_part_test_flag(part, 1))
                    {
                        float radius = effect_real_random_range("effect-generate-part", true, effect, part->minimum_radius, part->maximum_radius);
                        union { s_collision_test_flags test_flags; unsigned __int64 test_flags_value; };
                        test_flags.collision_flags = 0xd809;
                        test_flags.object_flags = 0;
                        long ignore_object_index = effect->object_index;
                        if (ignore_object_index != -1)
                            ignore_object_index = effects_generate_root_object(ignore_object_index);
                        float new_radius;
                        collision_get_unobstructed_point(test_flags_value, &part_to_world.position, radius,
                            ignore_object_index, -1, &part_to_world.position, &new_radius);
                        unattached = true;
                    }

                    float scale = effects_scale_random_range_value(effect, 1.0f, part->scale_a_mask, part->scale_b_mask, 5);
                    bool generate = !(location->node_index != 0xffff && (location->node_index & 0x8000) && part->type != 'beam');
                    if (effect->flags.test(_effect_no_objects_bit))
                    {
                        if (part->type == 'obje' || part->type == 'char')
                            generate = false;
                    }
                    if (generate)
                        event_generate_part(effect_index, event_index, &((effect_datum *)effect)->part_counter, part, location_index,
                            unattached, &part_to_world, scale);

                    bool may_delete_location = part->type == 'jpt!' || part->type == 'obje' || part->type == 'char';
                    if (may_delete_location && !datum_try_and_get((const s_data_array *)g_effect_location_data, location_index))
                        break;
                    location_index = effect_location_find_next_of_type(effect, location->next_location_index, part->render_type);
                }

                switch (part->type)
                {
                case 'cntl': effects_finish_contrail_system(effect_index, event_index, part); break;
                case 'ltvl': effects_finish_light_volume_system(effect_index, event_index, part); break;
                case 'beam': effects_finish_beam_system(effect_index, event_index, part); break;
                }
            }
        }
    }
}

// @address 0x23a398 ?event_generate_part@@YAXJJPAEPBUeffect_part_definition@@J_NPBUreal_matrix4x3@@M@Z
void event_generate_part(long effect_index, long event_index, unsigned char *part_counter, const effect_part_definition *part,
    long location_index, bool unattached, const real_matrix4x3 *matrix, float scale)
{
    const effect_datum *effect = datum_get<effect_datum>(::effect_data, effect_index);
    const effect_location_datum *location = effects_location_get(location_index);
    switch (part->type)
    {
    case 'rwrd':
        {
            real_point3d point = matrix->position;
            point.z -= 0.025f;
            s_water_physics_test_sphere_result result;
            if (water_physics_test_sphere(true, &point, k_effects_water_test_radius, &result))
            {
                real_point3d surface_point;
                point_from_line3d(&point, global_up3d, result.depth, &surface_point);
                if (active_user_close_to_point_not_deterministic(&surface_point, k_effects_water_event_distance, _adjust_for_fov_scale_none))
                    c_water_renderer::game_interation_event_add(part->definition.index, &surface_point, global_zero_vector3d, &result.flow);
            }
        }
        break;
    case 'jpt!':
        {
            s_damage_data damage;
            damage_data_new(&damage, part->definition.index);
            damage.owner = effect->damage_owner;
            damage.reporting_info = effect->damage_reporting_type;
            damage.scale[0] = scale;
            damage.location = effect->location;
            damage.position = matrix->position;
            damage.origin = matrix->position;
            damage.direction = matrix->forward;
            damage.cone_direction = matrix->forward;
            if (g_effect_damage_batch && effects_part_test_flag(part, 7))
                effect_damage_batch_add_request(g_effect_damage_batch, &damage);
            else
                area_of_effect_cause_damage(&damage, -1, -1);
        }
        break;
    case 'obje':
        {
            object_placement_data placement;
            object_placement_data_new(&placement, part->definition.index, effect->object_index, &effect->damage_owner);
            if (simulation_query_object_placement(&placement))
            {
                const s_effect_definition *definition = TAG_GET('effe', const s_effect_definition, effect->definition_index);
                bool distributed = false;
                if (effects_part_test_flag(part, 6))
                {
                    long object_index = effect->object_index;
                    if (object_try_and_get_and_verify_type(object_index, 0xffffffff))
                    {
                        const object_datum *object = object_get(object_index);
                        signed char variant_index = object->variant_index;
                        if (variant_index != -1)
                        {
                            const s_object_definition *object_definition =
                                TAG_GET('obje', const s_object_definition, object->definition_index);
                            if (object_definition->model.index != -1)
                            {
                                const s_model_definition *model =
                                    TAG_GET('hlmt', const s_model_definition, object_definition->model.index);
                                if (variant_index >= 0 && variant_index < model->variants.count)
                                    placement.model_variant_index = ((const s_model_variant *)model->variants.address)[variant_index].name;
                            }
                        }
                    }
                }
                placement.position = matrix->position;
                placement.forward = matrix->forward;
                placement.up = matrix->up;
                if (definition->conical_distribution.count)
                {
                    const s_effect_conical_distribution *distribution = (const s_effect_conical_distribution *)definition->conical_distribution.address;
                    if (distribution->radial_count && distribution->angular_count)
                    {
                        long count = distribution->angular_count * distribution->radial_count;
                        long index = *part_counter % count;
                        long angular_index = index % distribution->angular_count;
                        long radial_index = index / distribution->angular_count;
                        unsigned long *seed = tls_random_seed();
                        _distriubuite_vector_in_cone3d(seed, "effect conical distribution", "..\\effects\\effects.cpp", 6452,
                            &matrix->forward, radial_index, distribution->radial_count, angular_index, distribution->angular_count, 0.0f,
                            distribution->maximum, distribution->exponent, &placement.linear_velocity);
                        scale_vector3d(&placement.linear_velocity,
                            _real_random_range(seed, part->minimum_velocity, part->maximum_velocity), &placement.linear_velocity);
                        *part_counter = effect->part_counter + 1;
                        distributed = true;
                    }
                }
                if (!distributed)
                {
                    real_vector3d base_direction = *vector3d_from_euler_angles2d(&base_direction, &part->velocity_orientation);
                    real_vector3d direction;
                    matrix4x3_transform_vector(matrix, &base_direction, &base_direction);
                    effect_random_translational_velocity("effect-generate-part", true, effect, &base_direction, &direction,
                        &placement.linear_velocity, part->minimum_velocity, part->maximum_velocity, part->velocity_angle,
                        part->scale_a_mask, part->scale_b_mask);
                }
                placement.linear_velocity.i += effect->velocity.i;
                placement.linear_velocity.j += effect->velocity.j;
                placement.linear_velocity.k += effect->velocity.k;
                float angular_speed = effect_real_random_range("effect-generate-part", true, effect, part->minimum_angular_velocity,
                    part->maximum_angular_velocity, part->scale_a_mask, part->scale_b_mask, 3);
                if (angular_speed != 0.0f)
                {
                    effect_random_direction3d("effect-generate-part", true, effect, &placement.angular_velocity);
                    scale_vector3d(&placement.angular_velocity, angular_speed, &placement.angular_velocity);
                }
                else
                    placement.angular_velocity = *global_zero_vector3d;
                long object_index = object_new(&placement);
                if (object_index != -1)
                {
                    const s_object_header *header = object_header_get(object_index);
                    if (!((1 << header->object_type) & (1 << 10)) || ((const crate_datum *)header->datum)->lifetime_ticks == -1)
                        object_set_garbage(object_index, true, 0);
                    simulation_action_object_create(object_index);
                }
            }
        }
        break;
    case 'snd!':
        if (!effect->flags.test(_effect_no_sound_bit))
        {
            if (effect->object_index != -1 && !unattached)
                object_impulse_sound_new(effect->object_index, part->definition.index, effects_real_node_index((short)location->node_index),
                    &location->matrix.position, &location->matrix.forward, scale);
            else
            {
                struct { s_sound_location sound; s_location location; } spatial;
                spatial.sound.position = matrix->position;
                game_sound_location_set_rotation_from_direction(&spatial.sound, &matrix->forward);
                spatial.location = effect->location;
                spatial.sound.velocity = *global_zero_vector3d;
                unattached_impulse_sound_new(part->definition.index, &spatial.sound, scale);
            }
        }
        break;
    case 'ligh':
        if (!datum_get<event_datum>(g_event_data, event_index)->flags.test(_event_stale_bit))
        {
            if (effect->object_index != -1 && !unattached)
            {
                real_matrix4x3 part_to_node;
                effect_get_part_to_node(part, &location->matrix, &part_to_node);
                light_new_unattached(part->definition.index, effect->object_index, effects_real_node_index((short)location->node_index),
                    &part_to_node.position, &part_to_node.forward, &part_to_node.up, scale);
            }
            else
                light_new_unattached(part->definition.index, -1, -1, &matrix->position, &matrix->forward, &matrix->up, scale);
        }
        break;
    case 'decs':
        if (!effect->flags.test(_effect_no_decals_bit) || effects_part_test_flag(part, 2))
            event_request_decal_system(effect_index, event_index, part, matrix);
        break;
    case 'char':
        if (part->definition.index != -1)
        {
            short node_index = (short)location->node_index;
            if (!(node_index != -1 && (location->node_index & 0x8000)))
            {
                const event_datum *event = datum_get<event_datum>(g_event_data, event_index);
                ai_swarm_spawn_from_effect(part->definition.index,
                    (short)(long)((float)game_time_globals->ticks_per_second * event->duration + 0.5f), effect->object_index,
                    effects_real_node_index(node_index), part->minimum_velocity, part->maximum_velocity, &location->matrix);
            }
        }
        break;
    case 'cntl':
        ai_handle_effect_creation(effect_index, event_index, part, matrix, false);
        break;
    case 'ltvl':
        event_request_light_volume_system_location(effect_index, event_index, part, matrix, false);
        break;
    case 'beam':
        event_request_beam_system_location(effect_index, event_index, part, matrix, location->node_index != 0xffff && (location->node_index & 0x8000));
        break;
    case 'lens':
        break;
    case 'sefc':
        if (!datum_get<event_datum>(g_event_data, event_index)->flags.test(_event_stale_bit))
        {
            if (effect->object_index != -1 && !unattached)
            {
                real_matrix4x3 part_to_node;
                effect_get_part_to_node(part, &location->matrix, &part_to_node);
                effects_node_attachment_new(part->definition.index, effect->object_index,
                    effects_real_node_index((short)location->node_index), &part_to_node.position);
            }
            else
            {
                long definition_index = part->definition.index;
                long index = datum_new(g_static_lighting_effect_attachments);
                if (index != -1)
                {
                    effects_node_attachment *attachment = datum_get<effects_node_attachment>(g_static_lighting_effect_attachments, index);
                    attachment->definition_index = definition_index;
                    attachment->type = 0;
                    attachment->world_point = matrix->position;
                    attachment->age = 0.0f;
                }
            }
        }
        break;
    default:
        if (!g_effect_scenery_limit_reached)
            g_effect_scenery_limit_reached = true;
        break;
    }
}

t_effect_data_allocator g_effect_data_allocator;
t_event_data_allocator g_event_data_allocator;
t_effect_location_data_allocator g_effect_location_data_allocator;
t_effect_lightprobe_slot_data_allocator g_effect_lightprobe_slot_data_allocator;
t_effect_lightprobe_allocator g_effect_lightprobe_allocator;
t_effect_counts_allocator g_effect_counts_allocator;
t_effect_message_queue_allocator g_effect_message_queue_allocator;


// @address 0x3e97c ??_7?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_havok_game_state_allocator@@YAXPAX@Z@@6B@
// @address 0x783160 ?g_effect_lightprobe_allocator@@3V?$t_restricted_allocation_manager@$01$0A@$0A@$1?__tls_set_g_havok_game_state_allocator@@YAXPAX@Z@@A
