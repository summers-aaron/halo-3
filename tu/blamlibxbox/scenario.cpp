// blamlibxbox/scenario.obj: from the records 0x9bb38 0x9bb88 0x9e788 0xb07d8 0xbab88 0xbac68 0xc14a8 0xfba48 0x117f70 0x122d10 0x198a58 0x198ab8 0x198c48 0x198e60 0x198ef8 0x198f78 0x198fd8 0x199178 0x199260 0x199360 0x199730 0x1998f0 0x1999e8 0x199a80 0x199f60 0x19a060 0x240f28 0x50ae30 0x199c08
// @flags /TP /O2 /Oc
#include <xtl.h>
#include <xgraphics.h>
#include "objects/object_reference_types.h"
#include "memory/tls_globals.h"
#include "memory/datum_index.h"
#include "camera/cinematic_scene_types.h"
#include "scenario/scenario_object_name_types.h"
#include "tag_files/tag_groups.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
#include "cseries/cseries.h"
#include "cseries/pointer_utilities.h"
#include <ppcintrinsics.h>
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "scenario/cluster_reference_helpers.h"
#include "game/game_globals_storage.h"
#include "scenario/scenario_access.h"
#include "cache/cache_file_tag_resource_runtime.h"
#include "main/main_time.h"
#include "physics/bsp3d.h"
#include "physics/character_physics_definitions.h"
#include "physics/collision_instanced_geometry_definitions.h"
#include "physics/havok_mopp_code_definitions.h"
#include "physics/havok_vehicle_physics_definitions.h"
#include "physics/physics_model_definitions.h"
#include "structures/structure_seams.h"
#include "structures/structure_seam_allocator.h"
#include "visibility/visibility_portal_activation.h"
#include "cache/restricted_memory_regions.h"
#include "main/main_game.h"
#include "physics/collision_model_definitions.h"
#include "tag_files/tag_resource_cache_external_access.h"
#include "ai/ai_flocks.h"
#include "cache/cache_file_globals.h"
#include "cache/tag_resource_cache_loaded_query.h"
#include "scenario/scenario_pvs.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "camera/cinematic_scene_reference_types.h"
#include "scenario/scenario_cinematic_reference_types.h"
#include "camera/cinematic_types.h"
#include "scenario/scenario_bsp_state_out_types.h"
#include "scenario/scenario_bits_state_types.h"
#include "cache/tag_resource_shortcut_cache_types.h"
#include "scenario/scenario_bits_change_types.h"
#include "physics/collision_instanced_geometry_definition_types.h"
#include "cache/geometry_storage.h"
#include "physics/havok_vehicle_physics.h"
#include "physics/havok_monitor_storage.h"
#include "creatures/creature_definitions.h"
#include "units/biped_definitions.h"
#include "units/vehicle_definitions.h"
#include "scenario/scenario_tag.h"
#include "structures/structure_bsp_tag.h"
#include "structures/structure_runtime_resources.h"
#include "cubemaps/cubemap_sample.h"
#include "structures/structure_background_sound_types.h"
#include "sound/looping_sound_definitions.h"
#include "scenario/scenario_pvs_types.h"
#include "scenario/scenario_definitions.h"
#include "scenario/scenario_runtime_symbols.h"
#include "physics/collision_bsp_cache_types.h"
#include "game/game_system_types.h"
#include "memory/gameplay_tls_globals.h"
#include "cache/tag_resource_page_table.h"
#include "cache/tag_resource_cache_new.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "main/main_halt.h"
#include "scenario/scenario_game_state.h"
#include "camera/cinematic_scene_music_types.h"
#include "ai/flock_instance_types.h"
#include "rasterizer/rasterizer_shader_tag_types.h"
#include "rasterizer/rasterizer_shader_definitions.h"
#include "structures/structure_design_types.h"
#include "rasterizer/rasterizer_compiled_shader_types.h"
#include "scenario/scenario_resource_registry.h"

// @address 0x74b898 g_geometry_globals
// @address 0x74b910 g_havok_monitor_globals
// @address 0x8a1ed8 g_active_cinematic_zone_mask
// @address 0xbab88 ?scenario_location_from_point@@YAXPAUs_location@@PBTreal_point3d@@@Z
// @address 0xfba48 ?scenario_location_deafening@@YA_NPBUs_location@@@Z

static __forceinline const s_cluster_reference &location_cluster_reference(const s_location *location)
{
    return location->cluster_reference;
}
unsigned long g_active_structure_bsp_mask;
// @address 0x7cff08 ?global_scenario@@3PAUscenario@@A
scenario *global_scenario;
// @address 0x74b794 global_scenario_index
long global_scenario_index = -1;
// @address 0x74b798 global_scenario_game_globals_index
long global_scenario_game_globals_index = -1;
// @address 0x74b79c global_zone_set_index
long global_zone_set_index = -1;
// @address 0x7cf604 global_game_globals
s_game_globals *global_game_globals;
// @address 0x8a1ed4 g_active_designer_zone_mask
extern "C" { unsigned long g_active_designer_zone_mask; }
// @address 0x8a1ed0 ?g_touched_structure_bsp_mask@@3KA
unsigned long g_touched_structure_bsp_mask;
// @address 0x8a1edc ?g_touched_cinematic_zone_mask@@3KA
unsigned long g_touched_cinematic_zone_mask;

static __forceinline long global_scenario_index_get() { return global_scenario_index; }
static __forceinline bool scenario_zone_set_pvs_row_test(long scenario_index, const s_scenario_pvs_row *row, s_cluster_reference reference)
{
    return (row->vectors[reference.bsp_index][reference.cluster_index >> 5] & (1 << (reference.cluster_index & 31))) != 0;
}

static __forceinline s_cluster_reference cluster_reference_from_leaf(long structure_bsp_index, long leaf_index)
{
    s_cluster_reference reference;
    if (leaf_index != NONE) {
        unsigned char *cluster_index = (unsigned char *)tag_block_get_element_with_size(
            &global_structure_bsp_get(structure_bsp_index)->leaves, leaf_index, sizeof(unsigned char));
        cluster_reference_set(&reference, structure_bsp_index, *cluster_index);
    } else {
        cluster_reference_invalidate(&reference);
    }
    return reference;
}


static __forceinline long scenario_leaf_index_from_point(long structure_bsp_index, const real_point3d *point)
{
    return bsp3d_test_point(::global_bsp3d_get(structure_bsp_index), 0, point);
}

long scenario_leaf_index_from_point(const real_point3d *point, long *structure_bsp_index)
{
    long leaf_index = NONE;
    for (long index = 0; index < 16; index++)
    {
        if (global_structure_bsp_is_active(index))
        {
            leaf_index = scenario_leaf_index_from_point(index, point);
            if (leaf_index != NONE)
            {
                *structure_bsp_index = index;
                break;
            }
        }
    }
    return leaf_index;
}

s_cluster_reference scenario_cluster_reference_from_leaf_index(long structure_bsp_index, long leaf_index)
{
    return cluster_reference_from_leaf(structure_bsp_index, leaf_index);
}

// @address 0x9bb38 ?scenario_cluster_reference_from_point@@YA?AUs_cluster_reference@@PBTreal_point3d@@@Z
s_cluster_reference scenario_cluster_reference_from_point(const real_point3d *point)
{
    long structure_bsp_index;
    long leaf_index = scenario_leaf_index_from_point(point, &structure_bsp_index);
    if (leaf_index != NONE)
        return scenario_cluster_reference_from_leaf_index(structure_bsp_index, leaf_index);
    s_cluster_reference reference;
    cluster_reference_invalidate(&reference);
    return reference;
}


inline s_cluster_reference *scenario8_cluster_reference_invalidate(s_cluster_reference *cluster_reference)
{
    *(short *)cluster_reference = -1;
    return cluster_reference;
}

// match: 0xc14a8: retain the accepted inline body and callee summary.
inline s_cluster_reference scenario_cluster_reference_from_point_inline(const real_point3d *point)
{
    long structure_bsp_index;
    s_cluster_reference cluster_reference;
    long leaf_index = scenario_leaf_index_from_point(point, &structure_bsp_index);
    if (leaf_index != -1)
        cluster_reference = scenario_cluster_reference_from_leaf_index(structure_bsp_index, leaf_index);
    else
        scenario8_cluster_reference_invalidate(&cluster_reference);
    return cluster_reference;
}

// @address 0xc14a8 ?scenario_structure_index_from_point@@YAFPBTreal_point3d@@@Z
short scenario_structure_index_from_point(const real_point3d *point)
{
    short structure_index = -1;
    s_cluster_reference cluster_reference = scenario_cluster_reference_from_point_inline(point);
    if (scenario_cluster_reference_valid(&cluster_reference))
        structure_index = cluster_reference.bsp_index;
    return structure_index;
}

bool scenario_test_pvs(s_cluster_reference source, s_cluster_reference target)
{
    s_scenario_pvs_row row;
    scenario_zone_set_pvs_get_row(global_scenario_index_get(), &row, scenario_zone_set_index_get(), source, false);
    return scenario_zone_set_pvs_row_test(global_scenario_index_get(), &row, target);
}

void scenario_location_from_point(s_location *location, const real_point3d *point)
{
    location_invalidate(location);
    for (long structure_bsp_index = global_structure_bsp_first_active_index_get(); structure_bsp_index != NONE;
        structure_bsp_index = global_structure_bsp_next_active_index_get(structure_bsp_index))
    {
        long leaf_index = scenario_leaf_index_from_point(structure_bsp_index, point);
        if (leaf_index != NONE)
        {
            location->cluster_reference = cluster_reference_from_leaf(structure_bsp_index, leaf_index);
            location->leaf_index = (short)leaf_index;
            break;
        }
    }
}

void scenario_location_from_leaf(s_location *location, long structure_bsp_index, long leaf_index)
{
    location->leaf_index = (short)leaf_index;
    s_cluster_reference reference = cluster_reference_from_leaf(structure_bsp_index, leaf_index);
    s_cluster_reference *location_reference = &location->cluster_reference;
    *location_reference = reference;
}

bool scenario_location_deafening(const s_location *location)
{
    bool deafening = false;
    if (global_structure_bsp_is_active(location_cluster_reference(location).bsp_index))
    {
        structure_cluster *cluster = (structure_cluster *)tag_block_get_element_with_size(
            &global_structure_bsp_get(location_cluster_reference(location).bsp_index)->clusters,
            location_cluster_reference(location).cluster_index, sizeof(structure_cluster));
        if (cluster->background_sound_index != NONE &&
            cluster->background_sound_index <
                global_structure_bsp_get(location_cluster_reference(location).bsp_index)->background_sound_palette.count)
        {
            structure_background_sound_palette_entry *entry = (structure_background_sound_palette_entry *)tag_block_get_element_with_size(
                &global_structure_bsp_get(location_cluster_reference(location).bsp_index)->background_sound_palette,
                cluster->background_sound_index, sizeof(structure_background_sound_palette_entry));
            if (entry->background_sound.index != NONE)
            {
                looping_sound_definition *looping_sound = (looping_sound_definition *)tag_get('lsnd', entry->background_sound.index);
                deafening = TEST_BIT(looping_sound->flags, 0);
            }
        }
    }
    return deafening;
}


// @address 0x122d10 ?global_bsp3d_get@@YAPAUbsp3d@@J@Z
struct bsp3d * global_bsp3d_get(long structure_bsp_index)
{
    structure_bsp *bsp = global_structure_bsp_get(structure_bsp_index);
    s_structure_bsp_resource *resource = &bsp->resource;
    if (resource->use_resource_items)
        return (bsp3d *)((c_structure_bsp_resource_interface *)&resource->collision_resource_handle)->get_resources()->collision_bsp;
    return (bsp3d *)((s_structure_bsp_resources *)resource->raw_resources.address)->collision_bsp;
}

extern "C" {

}
static __forceinline unsigned long global_designer_zone_active_mask_get()
{
    return g_active_designer_zone_mask;
}
static __forceinline s_scenario_zone_set *scenario_zone_set_get(long zone_set_index)
{
    return (s_scenario_zone_set *)tag_block_get_element_with_size(&global_scenario_get()->zone_sets, zone_set_index,
        sizeof(s_scenario_zone_set));
}
static __forceinline unsigned long scenario_zone_set_bsp_active_mask_get(long zone_set_index)
{
    return zone_set_index != NONE ? scenario_zone_set_get(zone_set_index)->bsp_active_mask : 0;
}
static __forceinline unsigned long scenario_zone_set_designer_zone_required_mask_get(long zone_set_index)
{
    return zone_set_index != NONE ? scenario_zone_set_get(zone_set_index)->designer_zone_required_mask : 0;
}

// @address 0x117f70 scenario_zone_set_is_active
extern "C" bool scenario_zone_set_is_active(long zone_set_index)
{
    bool result = false;
    if (zone_set_index == scenario_zone_set_index_get())
    {
        unsigned long active_structure_bsp_mask = global_structure_bsp_active_mask_get();
        unsigned long active_designer_zone_mask = global_designer_zone_active_mask_get();
        unsigned long structure_bsp_mask = scenario_zone_set_bsp_active_mask_get(zone_set_index);
        unsigned long designer_zone_mask = scenario_zone_set_designer_zone_required_mask_get(zone_set_index);
        result = (structure_bsp_mask & active_structure_bsp_mask) == structure_bsp_mask &&
            (designer_zone_mask & active_designer_zone_mask) == designer_zone_mask;
    }
    return result;
}

s_collision_bsp_cache g_collision_bsp_cache;
static __forceinline void structure_seams_initialize()
{
    g_structure_seam_globals = (s_structure_seam_globals
            *)g_structure_seam_globals_allocator.reserve_memory("s_structure_seam_globals", 0, 34060, 0);
}

void scenario_initialize()
{
    memset(&g_collision_bsp_cache, 0, sizeof(g_collision_bsp_cache));
    structure_seams_initialize();
}


// @address 0x198ab8 ?scenario_initialize_for_new_map@@YAXXZ
void scenario_initialize_for_new_map()
{
    g_touched_structure_bsp_mask = 0;
    g_active_cinematic_zone_mask = 0;
    s_structure_seam_globals *globals = g_structure_seam_globals;
    memset(globals->mappings, 0, sizeof(globals->mappings));
    memset(&globals->active_seam_flags, 0, sizeof(globals->active_seam_flags));
    globals->connected_structure_bsp_mask = 0;
    for (long i = 0; i < 64; i++)
    {
        s_seam_mapping *mapping = &globals->mappings[i];
        mapping->structure_indices[1] = -1;
        mapping->structure_indices[0] = -1;
        for (long j = 0; j < 32; j++)
        {
            mapping->clusters[j].cluster_indices[0] = -1;
            mapping->clusters[j].cluster_indices[1] = -1;
            mapping->clusters[j].cluster_mapping_indices[0] = -1;
            mapping->clusters[j].cluster_mapping_indices[1] = -1;
        }
    }
}

// The March release map folds this empty game-system callback into the shared blr at 0x5bec98.
// @address 0x5bec98 ?scenario_dispose@@YAXXZ
void scenario_dispose()
{
}

// @address 0x5bec98 ?scenario_dispose_from_old_map@@YAXXZ
void scenario_dispose_from_old_map()
{
}

// @address 0x198c48 ?scenario_dispose_from_old_structure_bsp@@YAXK@Z
void scenario_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask)
{
    for (int i = 0; i < 16; i++)
    {
        if (structure_bsp_mask & (1 << i))
        {
            structure_seams_disconnect(global_scenario_index, i);
        }
    }
}

// @address 0x198e60 ?scenario_cluster_reference_from_point@@YA?AUs_cluster_reference@@JPBTreal_point3d@@@Z
s_cluster_reference scenario_cluster_reference_from_point(long bsp_index, const real_point3d *point)
{
    s_cluster_reference result;
    int leaf = bsp3d_test_point(::global_bsp3d_get(bsp_index), 0, point);
    if (leaf != -1)
    {
        result.bsp_index = (signed char)bsp_index;
        result.cluster_index = ((unsigned char *)((structure_bsp *)
                                    global_tag_instances[datum_index_to_absolute_index(((scenario_structure_bsp_reference
                                        *)global_scenario->structure_bsps.address)[bsp_index].structure_bsp.index)]
                                        .definition)
                                   ->leaves.address)[leaf];
        return result;
    }
    cluster_reference_invalidate(&result);
    return result;
}


// @address 0x198ef8 ?scenario_object_name_index_from_string@@YAFPAUscenario@@PBD@Z
short scenario_object_name_index_from_string(scenario *scenario, const char *name)
{
    scenario = global_scenario_get();
    for (short index = 0; index < scenario->object_names.count; index++)
    {
        s_scenario_object_name *object_name = (s_scenario_object_name *)tag_block_get_element_with_size(
            &scenario->object_names, index, sizeof(s_scenario_object_name));
        if (strcmp(object_name->name, name) == 0)
            return index;
    }
    return NONE;
}

// @address 0x198f78 scenario_zone_set_get_data
extern "C" long scenario_zone_set_get_data(void)
{
    long result = 0;
    if (global_scenario_index != NONE)
    {
        scenario *definition = (scenario *)tag_get('scnr', global_scenario_index);
        long zone_set_index = global_zone_set_index;
        if (zone_set_index >= 0 && zone_set_index < definition->zone_sets.count)
            result = *(long *)tag_block_get_element_with_size(&definition->zone_sets, zone_set_index, sizeof(s_scenario_zone_set));
    }
    return result;
}

// @address 0x1998f0 scenario_notify_designer_zone_change
extern "C" bool scenario_notify_designer_zone_change(s_scenario_bits_state *state)
{
    bool changed = state->previous[1] != state->current[1] || state->previous[0] != state->current[0];
    if (changed)
    {
        long lock_token = ::tag_resources_lock_game();
        s_game_non_bsp_zone_set delta;
        delta.designer_zone_mask = state->current[0] & ~state->previous[0];
        game_globals_storage *globals = game_globals;
        delta.cinematic_zone_mask = state->current[1] & ~state->previous[1];
        globals->active_designer_zone_mask = g_active_designer_zone_mask;
        globals->active_cinematic_zone_mask = g_active_cinematic_zone_mask;
        for (unsigned int system_index = 0; system_index < 92; ++system_index)
        {
            if (g_game_systems[system_index].initialize_for_new_non_bsp_zone_set)
                g_game_systems[system_index].initialize_for_new_non_bsp_zone_set(&delta);
        }
        ::tag_resources_unlock_game(lock_token);
    }
    return true;
}

// @address 0x3bebc ??_7c_tag_resource_error_receiver@@6B@


static __forceinline void scenario_commit_resources(c_cache_file_tag_resource_runtime_manager *&storage, bool synchronous,
    c_tag_resource_error_receiver *receiver)
{
    storage->commit_active_zones();
    storage = g_geometry_globals.manager;
    if (storage->m_cache_file_resource_gestalt_index != NONE)
    {
        c_tag_resource_cache_new *cache = storage->get_resource_cache();
        if (cache->m_loaded_query->is_loaded())
            tag_resource_cache_new_update_zones(cache, 0, synchronous, receiver);
        if (!synchronous)
            storage->m_state_valid = true;
        if (storage->m_active_zone_changed)
        {
            tag_resource_cache_new_commit_dirty(cache);
            storage->get_resource_cache()->m_page_table.m_include_unreferenced = true;
            storage->m_active_zone_changed = false;
            storage = g_geometry_globals.manager;
        }
        else
        {
            storage = g_geometry_globals.manager;
        }
    }
}

// @address 0x198fd8 scenario_commit_zone_resources
extern "C" bool scenario_commit_zone_resources(int reserved)
{
    bool result = false;
    c_cache_file_tag_resource_runtime_manager *storage = g_geometry_globals.manager;
    long attempts = 0;
    do
    {
        c_tag_resource_error_receiver receiver;
        if ((unsigned char)reserved)
            scenario_commit_resources(storage, true, &receiver);
        else
            scenario_commit_resources(storage, false, &receiver);
        receiver.finished = true;
        if (receiver.succeeded())
            result = true;
        else if (++attempts >= 3)
            main_halt_and_display_errors();
    }
    while (!result && attempts < 3);
    return result;
}

// @address 0x199730 scenario_dispose_old_zones
extern "C" void scenario_dispose_old_zones(s_scenario_bits_state *state, bool force)
{
    bool changed = state->previous[1] != state->current[1] || state->previous[0] != state->current[0];
    if (changed || force)
    {
        long lock_token = ::tag_resources_lock_game();
        s_game_non_bsp_zone_set previous;
        s_game_non_bsp_zone_set delta;
        s_game_non_bsp_zone_set current;
        c_scenario_resource_registry pending_zone_registry;
        cache_file_tag_resource_compute_scenario_flags(g_geometry_globals.storage,
            g_active_structure_bsp_mask, g_touched_structure_bsp_mask, state->current[0] & state->previous[0],
            state->previous[1] & state->current[1], &pending_zone_registry);
        previous.designer_zone_mask = state->previous[0];
        previous.cinematic_zone_mask = state->previous[1];
        current.designer_zone_mask = state->current[0];
        current.cinematic_zone_mask = state->current[1];
        for (unsigned int system_index = 0; system_index < 92; ++system_index)
        {
            if (g_game_systems[system_index].prepare_for_designer_zone_set_switch)
                g_game_systems[system_index].prepare_for_designer_zone_set_switch(&previous, &current, &pending_zone_registry);
        }
        ::objects_purge_deleted_objects();
        bool cleared = state->previous[1] != state->current[1] || state->previous[0] != state->current[0];
        if (cleared)
        {
            delta.cinematic_zone_mask = state->previous[1] & ~state->current[1];
            delta.designer_zone_mask = state->previous[0] & ~state->current[0];
            (&g_havok_monitor_globals)->lock_count_4c++;
            for (long system_index = NUMBEROF(g_game_systems) - 1; system_index >= 0; system_index--)
            {
                if (g_game_systems[system_index].dispose_from_old_non_bsp_zone_set)
                    g_game_systems[system_index].dispose_from_old_non_bsp_zone_set(&delta);
            }
            ::objects_purge_deleted_objects();
            (&g_havok_monitor_globals)->lock_count_4c--;
            game_globals_storage *globals = game_globals;
            globals->active_designer_zone_mask = 0;
            globals->active_cinematic_zone_mask = 0;
        }
        ::tag_resources_unlock_game(lock_token);
    }
}


// @address 0x199a80 scenario_get_bsp_state
// match: 0x199a80: pointer __restrict qualifier prevents alias reload
extern "C" void scenario_get_bsp_state(scenario_bsp_state_out *__restrict out)
{
    unsigned long a = g_touched_structure_bsp_mask;
    unsigned long b = g_active_designer_zone_mask;
    unsigned long c = g_active_cinematic_zone_mask;
    long index = global_zone_set_index;
    out->valid = true;
    out->touched_structure_bsp_mask = a;
    out->active_designer_zone_mask = b;
    out->active_cinematic_zone_mask = c;
    if (index != NONE)
    {
        s_scenario_zone_set *entry = &((s_scenario_zone_set *)global_scenario->zone_sets.address)[index];
        out->active_structure_bsp_mask = g_active_structure_bsp_mask;
        out->required_designer_zone_mask = entry->designer_zone_required_mask | out->active_designer_zone_mask;
        out->required_cinematic_zone_mask = entry->cinematic_zones & ~g_touched_cinematic_zone_mask;
        out->required_structure_bsp_mask = entry->bsp_active_mask;
    }
    else
    {
        out->active_structure_bsp_mask = 0;
        out->required_designer_zone_mask = 0;
        out->required_cinematic_zone_mask = 0;
        out->required_structure_bsp_mask = 0;
    }
}

// @address 0x19a060 ?scenario_game_states_match@@YA_NPBUs_scenario_game_state@@0@Z
bool scenario_game_states_match(const s_scenario_game_state *a, const s_scenario_game_state *b)
{
    return a->flags == b->flags && a->zone_set_index == b->zone_set_index && a->active_structure_bsp_mask == b->active_structure_bsp_mask &&
        a->active_designer_zone_mask == b->active_designer_zone_mask && a->touched_structure_bsp_mask ==
            b->touched_structure_bsp_mask && a->active_cinematic_zone_mask == b->active_cinematic_zone_mask &&
        a->touched_cinematic_zone_mask == b->touched_cinematic_zone_mask;
}


// @address 0x240f28 cinematic_tag_reference_get_music
extern "C" long cinematic_tag_reference_get_music(long cinematic_index, long scene_index, long shot_index, long entry_index)
{
    long result = NONE;
    s_scenario_cinematic_reference *cinematic_reference = (s_scenario_cinematic_reference *)tag_block_get_element_with_size(
        &global_scenario_get()->cinematics, cinematic_index, sizeof(s_scenario_cinematic_reference));
    if (cinematic_reference->cinematic.index != NONE)
    {
        s_cinematic *cinematic = (s_cinematic *)tag_get('cine', cinematic_reference->cinematic.index);
        if (scene_index >= 0 && scene_index < cinematic->scenes.count)
        {
            s_cinematic_scene_reference *scene_reference = (s_cinematic_scene_reference *)tag_block_get_element_with_size(
                &cinematic->scenes, scene_index, sizeof(s_cinematic_scene_reference));
            if (scene_reference->scene.index != NONE)
            {
                s_cinematic_scene *scene = (s_cinematic_scene *)tag_get('cisc', scene_reference->scene.index);
                if (shot_index >= 0 && shot_index < scene->shots.count)
                {
                    hs_cinematic_shot *shot = (hs_cinematic_shot *)tag_block_get_element_with_size(&scene->shots, shot_index,
                        sizeof(hs_cinematic_shot));
                    if (entry_index >= 0 && entry_index < shot->music.count)
                    {
                        s_cinematic_scene_music *entry = (s_cinematic_scene_music *)tag_block_get_element_with_size(
                            &shot->music, entry_index, sizeof(s_cinematic_scene_music));
                        result = entry->sound.index;
                    }
                }
            }
        }
    }
    return result;
}


// @address 0x50ae30 flock_create_from_key
extern "C" bool flock_create_from_key(long key)
{
    scenario *scenario_definition = global_scenario_get();
    short volume_index = NONE;
    for (short index = 0; index < scenario_definition->flocks.count; index++)
    {
        if (((s_flock_instance *)tag_block_get_element_with_size(&scenario_definition->flocks, index,
            sizeof(s_flock_instance)))->name == key)
        {
            volume_index = index;
            break;
        }
    }
    bool result = false;
    if (volume_index != NONE)
    {
        s_flock_instance *volume = (s_flock_instance *)tag_block_get_element_with_size(&scenario_definition->flocks,
            volume_index, sizeof(s_flock_instance));
        if (global_structure_bsp_is_active(volume->bsp_index))
            result = flock_new_from_scenario(volume_index);
    }
    return result;
}

// @address 0x199360 scenario_apply_zone_set_change
extern "C" bool scenario_apply_zone_set_change(long zone_set_index, unsigned long old_structure_bsp_mask, unsigned long new_structure_bsp_mask,
    long touched_structure_bsp_mask, s_scenario_bits_state *state, long touched_cinematic_zone_mask, bool)
{
    bool result = false;
    if (new_structure_bsp_mask == 0 || (unsigned long)global_scenario_get()->structure_bsps.count >= 32
        || (new_structure_bsp_mask & ~((unsigned long)((1i64 << global_scenario_get()->structure_bsps.count) - 1))) == 0)
    {
        bool force = (new_structure_bsp_mask - old_structure_bsp_mask) != 0;
        scenario_dispose_old_zones(state, force);
        bool changed = false;
        if (old_structure_bsp_mask != new_structure_bsp_mask)
        {
            long lock_token = ::tag_resources_lock_game();
            (&g_havok_monitor_globals)->lock_count_4c++;
            for (unsigned int system_index = 0; system_index < 92; ++system_index)
            {
                if (g_game_systems[system_index].prepare_for_new_zone_set)
                    g_game_systems[system_index].prepare_for_new_zone_set(old_structure_bsp_mask, new_structure_bsp_mask);
            }
            ::objects_purge_deleted_objects();
            (&g_havok_monitor_globals)->lock_count_4c--;
            if (old_structure_bsp_mask != 0)
            {
                main_game_dispose_from_old_structure_bsp(old_structure_bsp_mask & ~new_structure_bsp_mask);
                changed = true;
            }
            ::tag_resources_unlock_game(lock_token);
        }
        if (!changed && global_zone_set_index != NONE)
        {
            (&g_havok_monitor_globals)->lock_count_4c++;
            portal_activation_clear_portals();
            (&g_havok_monitor_globals)->lock_count_4c--;
        }
        cache_file_tag_resource_reset_active_state();
        global_zone_set_index = zone_set_index;
        result = true;
        g_active_designer_zone_mask = state->current[0];
        g_active_structure_bsp_mask = new_structure_bsp_mask;
        g_active_cinematic_zone_mask = state->current[1];
        g_touched_structure_bsp_mask = touched_structure_bsp_mask;
        g_touched_cinematic_zone_mask = touched_cinematic_zone_mask;
        scenario_bsp_state_out out_state;
        scenario_get_bsp_state(&out_state);
        long zone_set_data = scenario_zone_set_get_data();
        cache_file_tag_resource_set_active_state(g_geometry_globals.manager, global_scenario_index, zone_set_data, &out_state);
        if (!scenario_commit_zone_resources(0))
        {
            result = false;
            global_zone_set_index = NONE;
            g_active_structure_bsp_mask = 0;
        }
        bool success = result;
        if (success)
            success = scenario_notify_designer_zone_change(state);
        if (success)
        {
            bool updated = false;
            if (new_structure_bsp_mask != 0 && old_structure_bsp_mask != new_structure_bsp_mask)
            {
                long update_lock_token = ::tag_resources_lock_game();
                game_globals->active_structure_bsp_mask = g_active_structure_bsp_mask;
                for (unsigned int system_index = 0; system_index < 92; ++system_index)
                {
                    if (g_game_systems[system_index].initialize_for_new_structure_bsp)
                        g_game_systems[system_index].initialize_for_new_structure_bsp(new_structure_bsp_mask & ~old_structure_bsp_mask);
                }
                updated = true;
                ::tag_resources_unlock_game(update_lock_token);
            }
            if (!updated && zone_set_index != NONE)
            {
                long portal_lock_token = ::tag_resources_lock_game();
                (&g_havok_monitor_globals)->lock_count_4c++;
                portal_activation_initialize_for_new_structure_bsp(0);
                (&g_havok_monitor_globals)->lock_count_4c--;
                ::tag_resources_unlock_game(portal_lock_token);
            }
        }
        main_time_reset();
        result = success;
    }
    return result;
}

static __forceinline unsigned long scenario_zone_set_designer_zone_forbidden_mask_get(long zone_set_index)
{
    return zone_set_index != NONE ? scenario_zone_set_get(zone_set_index)->designer_zone_forbidden_mask : 0;
}


// @address 0x1999e8 scenario_update_designer_zones
extern "C" void scenario_update_designer_zones(const s_scenario_bits_change *changes)
{
    long zone_set_index = global_zone_set_index;
    unsigned long structure_bsp_mask = game_globals->active_structure_bsp_mask;
    unsigned long designer_zone_mask = g_active_designer_zone_mask;
    unsigned long cinematic_zone_mask = g_active_cinematic_zone_mask;
    unsigned long new_designer_zone_mask = (designer_zone_mask & ~changes[0].clear_mask) | changes[0].set_mask;
    unsigned long new_cinematic_zone_mask = (cinematic_zone_mask & ~changes[1].clear_mask) | changes[1].set_mask;
    unsigned long touched_structure_bsp_mask = g_touched_structure_bsp_mask | structure_bsp_mask;
    unsigned long touched_cinematic_zone_mask = g_touched_cinematic_zone_mask | new_cinematic_zone_mask;
    s_scenario_bits_state state;
    state.previous[0] = designer_zone_mask;
    state.previous[1] = cinematic_zone_mask;
    state.current[0] = new_designer_zone_mask;
    state.current[1] = new_cinematic_zone_mask;
    scenario_apply_zone_set_change(
        zone_set_index, structure_bsp_mask, structure_bsp_mask, touched_structure_bsp_mask, &state, touched_cinematic_zone_mask, false);
}

// @address 0x199260 scenario_switch_zone_set
extern "C" bool scenario_switch_zone_set(long zone_set_index, bool unknown)
{
    unsigned long new_structure_bsp_mask = scenario_zone_set_bsp_active_mask_get(zone_set_index);
    game_globals_storage *globals = game_globals;
    unsigned long old_structure_bsp_mask = globals->active_structure_bsp_mask;
    unsigned long touched_structure_bsp_mask = scenario_zone_set_bsp_active_mask_get(zone_set_index) | g_touched_structure_bsp_mask;
    unsigned long active_designer_zone_mask = globals->active_designer_zone_mask;
    unsigned long new_designer_zone_mask = (scenario_zone_set_designer_zone_required_mask_get(zone_set_index) | active_designer_zone_mask) &
        ~scenario_zone_set_designer_zone_forbidden_mask_get(zone_set_index);
    s_scenario_bits_state state;
    state.previous[0] = active_designer_zone_mask;
    state.previous[1] = globals->active_cinematic_zone_mask;
    state.current[0] = new_designer_zone_mask;
    state.current[1] = globals->active_cinematic_zone_mask;
    return scenario_apply_zone_set_change(zone_set_index, old_structure_bsp_mask, new_structure_bsp_mask, touched_structure_bsp_mask,
        &state, g_touched_structure_bsp_mask, true);
}

static __forceinline unsigned long scenario_game_state_structure_bsp_mask_get(const s_scenario_game_state *s) { return
    s->active_structure_bsp_mask; }

// @address 0x199f60 scenario_set_game_state
extern "C" bool scenario_set_game_state(const s_scenario_game_state *p0)
{
    s_scenario_game_state local;
    bool result = false;
    local.flags = 1;
    local.zone_set_index = global_zone_set_index;
    local.active_structure_bsp_mask = g_active_structure_bsp_mask;
    local.active_designer_zone_mask = g_active_designer_zone_mask;
    local.touched_structure_bsp_mask = g_touched_structure_bsp_mask;
    local.active_cinematic_zone_mask = g_active_cinematic_zone_mask;
    local.touched_cinematic_zone_mask = g_touched_cinematic_zone_mask;
    if (scenario_game_states_match(p0, &local)) {
        result = true;
    } else {
        game_globals_storage *globals = game_globals;
        const unsigned long *zone_masks = &globals->active_designer_zone_mask;
        s_scenario_bits_state state;
        state.current[0] = p0->active_designer_zone_mask;
        state.current[1] = p0->active_cinematic_zone_mask;
        state.previous[0] = zone_masks[0];
        state.previous[1] = zone_masks[1];
        if (scenario_apply_zone_set_change(p0->zone_set_index, globals->active_structure_bsp_mask,
                scenario_game_state_structure_bsp_mask_get(p0), p0->touched_structure_bsp_mask, &state,
                    p0->touched_cinematic_zone_mask, false))
            result = true;
    }
    return result;
}


// @address 0x199178 scenario_activate_zone_set
extern "C" unsigned char scenario_activate_zone_set(long zone_set_index)
{
    game_globals_storage *globals = game_globals;
    unsigned long old_structure_bsp_mask = globals->active_structure_bsp_mask;
    unsigned long new_structure_bsp_mask = scenario_zone_set_bsp_active_mask_get(zone_set_index) & old_structure_bsp_mask;
    unsigned char result;
    if (new_structure_bsp_mask) {
        unsigned long touched_structure_bsp_mask = new_structure_bsp_mask | g_touched_structure_bsp_mask;
        unsigned long active_designer_zone_mask = globals->active_designer_zone_mask;
        unsigned long new_designer_zone_mask = active_designer_zone_mask &
            ~scenario_zone_set_designer_zone_forbidden_mask_get(zone_set_index);
        s_scenario_bits_state state;
        state.previous[0] = active_designer_zone_mask;
        state.previous[1] = globals->active_cinematic_zone_mask;
        state.current[0] = new_designer_zone_mask;
        state.current[1] = globals->active_cinematic_zone_mask;
        result = scenario_apply_zone_set_change(zone_set_index, old_structure_bsp_mask, new_structure_bsp_mask, touched_structure_bsp_mask,
            &state, g_touched_structure_bsp_mask, true);
    } else {
        result = scenario_switch_zone_set(zone_set_index, true);
    }
    return result;
}

// @address 0x199c08 ?scenario_tags_fixup@@YAXXZ

void scenario_tags_fixup()
{
    tag_iterator iterator;
    iterator.next_index = 0;
    iterator.group_tag = NONE;
    long index = tag_iterator_next(&iterator);
    for (; index != NONE; index = tag_iterator_next(&iterator)) {
        unsigned long group = tag_get_group_tag(index);
        if (group <= 'phmo') {
            if (group != 'phmo') {
                if (group <= 'crea') {
                    if (group != 'crea') {
                        if (group != 'bipd') {
                            if (group != 'coll')
                                continue;
                            collision_model_fixup(index);
                            continue;
                        } else {
                            character_physics_definition_fixup(
                                (s_character_physics_definition *)&((biped_definition *)tag_get('bipd', index))->physics);
                        }
                    } else {
                        character_physics_definition_fixup((s_character_physics_definition *)&((creature_definition
                            *)tag_get('crea', index))->physics);
                    }
                } else {
                    if (group - 'glps' != 0) {
                        if (group - 'glps' != 0x600)
                            continue;
                        const s_rasterizer_global_vertex_shader_definition *glvs = (const
                            s_rasterizer_global_vertex_shader_definition *)tag_get('glvs', index);
                        long count = glvs->shaders.count;
                        if (count <= 0)
                            continue;
                        unsigned long offset = 0;
                        unsigned long remaining = (unsigned long)count;
                        do {
                            D3DVertexShader *shader = ((const c_rasterizer_compiled_vertex_shader *)((const char *)glvs->shaders.address
                                + offset))->get_d3d_shader();
                            if (shader != 0)
                                XGRegisterVertexShader(shader, pointer_from_address(shader->Identifier));
                            remaining--;
                            offset += 0x50;
                        } while (remaining != 0);
                    } else {
                        const s_rasterizer_global_pixel_shader_definition *glps = (const
                            s_rasterizer_global_pixel_shader_definition *)tag_get('glps', index);
                        long count = glps->shaders.count;
                        if (count <= 0)
                            continue;
                        unsigned long offset = 0;
                        unsigned long remaining = (unsigned long)count;
                        do {
                            D3DPixelShader *shader = ((const c_rasterizer_compiled_pixel_shader *)((const char *)glps->shaders.address +
                                offset))->get_d3d_shader();
                            if (shader != 0)
                                XGRegisterPixelShader(shader, pointer_from_address(shader->Identifier));
                            remaining--;
                            offset += 0x50;
                        } while (remaining != 0);
                    }
                }
            } else {
                physics_model_fixup(index, false);
            }
        } else if (group <= 'sddt') {
            if (group != 'sddt') {
                if (group != 'pixl') {
                    if (group != 'sbsp')
                        continue;
                    long k = 0;
                    structure_bsp *sbsp = (structure_bsp *)tag_get('sbsp', index);
#ifdef HALO3_64BIT
                    // Redirect the instance physics blocks before the cache pages become read-only.
                    collision_model_unpack_bsp_instances_64(sbsp);
#endif
                    if (sbsp->clusters.count > 0) {
                        do {
                            collision_instanced_geometry_definition_fixup(&((structure_cluster
                                *)sbsp->clusters.address)[k].collision_instanced_geometry, index, k);
                            k++;
                        } while (k < sbsp->clusters.count);
                    }
                    s_structure_physics *mopp = &sbsp->structure_physics;
                    havok_mopp_code_definitions_fixup(&mopp->mopp_code_block);
                    havok_mopp_code_definitions_fixup(&mopp->breakable_surfaces_mopp_code_block);
                    continue;
                } else {
                    const c_rasterizer_pixel_shader *pixl = (const c_rasterizer_pixel_shader *)tag_get('pixl', index);
                    long count = pixl->get_shaders().count;
                    if (count <= 0)
                        continue;
                    unsigned long offset = 0;
                    unsigned long remaining = (unsigned long)count;
                    do {
                        D3DPixelShader *shader = ((const c_rasterizer_compiled_pixel_shader *)((const char *)pixl->get_shaders().address
                            + offset))->get_d3d_shader();
                        if (shader != 0)
                            XGRegisterPixelShader(shader, pointer_from_address(shader->Identifier));
                        remaining--;
                        offset += 0x50;
                    } while (remaining != 0);
                }
            } else {
                s_structure_design *sddt = (s_structure_design *)tag_get('sddt', index);
                havok_mopp_code_definitions_fixup(&sddt->soft_ceiling_mopp_codes);
                havok_mopp_code_definitions_fixup(&sddt->water_mopp_codes);
                continue;
            }
        } else if (group != 'vehi') {
            if (group != 'vtsh')
                continue;
            const c_rasterizer_vertex_shader *vtsh = (const c_rasterizer_vertex_shader *)tag_get('vtsh', index);
            long count = vtsh->get_shader_count();
            if (count <= 0)
                continue;
            unsigned long offset = 0;
            unsigned long remaining = (unsigned long)count;
            do {
                D3DVertexShader *shader = ((const c_rasterizer_compiled_vertex_shader *)((const char *)vtsh->get_shaders() +
                    offset))->get_d3d_shader();
                if (shader != 0)
                    XGRegisterVertexShader(shader, pointer_from_address(shader->Identifier));
                remaining--;
                offset += 0x50;
            } while (remaining != 0);
        } else {
            havok_vehicle_physics_definition_fixup((s_havok_vehicle_physics_definition *)&((vehicle_definition
                *)tag_get('vehi', index))->havok_vehicle_physics);
        }
    }
}
