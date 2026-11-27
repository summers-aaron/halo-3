// objectslibxbox/object_placement.obj: from the records 0xd5ef8 0x330920 0x330940 0x330958 0x3309a0 0x3309b8 0x3309d0 0x3309e0 0x330b08 0x330ba8 0x330bc8 0x330bd8 0x330c78 0x330c90 0x330d28 0x330fa0 0x331088 0x3319b8 0x331a98 0x331de0 0x332238 0x3322b0 0x332308 0x332388 0x332408 0x332470 0x3325b0 0x332788 0x332828 0x332840 0x33a848 0x35cb10 0x40db30
#include "cache/restricted_memory.h"
#include "render/water_interaction_tls.h"
#include "objects/object_memory_pool.h"
#include "objects/object_name_list.h"
#include "multithreading/thread_tls.h"
#include "objects/object_bsp_placement_policy_types.h"
#include "objects/object_reference_types.h"
#include "game/game_globals_storage.h"
#include "memory/data_types.h"
#include "memory/tls_globals.h"
#include "math/real_math_types.h"
#include <ppcintrinsics.h>
#include "cseries/cseries.h"
#include "cseries/bit_test.h"
#include "objects/object_iterator.h"
#include "objects/object_placement.h"
#include "cache/restricted_memory_regions.h"
#include "game/game_engine.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_candy_monitor.h"
#include "memory/memory_pool.h"
#include "objects/damage.h"
#include "render/render_water.h"
#include "game/game_engine_sandbox.h"
#include "networking/logic/network_session_interface.h"
#include "memory/data.h"
#include "game/game_globals.h"
#include "game/game_globals_inline.h"
#include "game/game_simulation_inline.h"
#include "tag_files/tag_groups.h"
#include "game/players.h"
#include "scenario/scenario.h"
#include "scenario/scenario_map_variant.h"
#include "cache/cache_files.h"
#include "objects/object_type_definition.h"
#include "objects/object_lifecycle.h"
#include "objects/object_placement_allocators.h"
#include "physics/collision_bsp.h"
#include "physics/collision_bsp_query_types.h"
#include "objects/object_datum.h"
#include "items/item_types.h"
#include "physics/havok.h"
#include "physics/havok_object_bounds.h"
#include "cseries/progress.h"
#include "math/real_math_functions.h"
#include "scenario/scenario_tag.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_region_members.h"
#include <string.h>
#include "scenario/scenario_objects.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "objects/object_placement_globals.h"
#include "scenario/scenario_light_types.h"
#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "structures/structure_bsp_tag.h"
#include "models/model_variant_types.h"
#include "game/game_engine_scoring_globals.h"
#include "physics/havok_monitor_storage.h"
#include "cutscene/cinematic_globals_types.h"
#include "scenario/scenario_resource_registry.h"
#include "scenario/scenario_object_names.h"
#include "scenario/scenario_map_variant_types.h"
#include "scenario/scenario_sky.h"
#include "scenario/scenario_definitions.h"
#include "objects/object_header.h"
// @compiler xdk6130
// @flags /TP /O2 /Oc


struct structure_identifier { unsigned char bsp_index, collision_bsp_index; };

struct surface_identifier { structure_identifier structure; short surface_index; };

namespace {
struct c_object_placement_globals_publisher {
    static __forceinline void publish(void *address) { g_object_placement_globals_address = (s_object_placement_globals *)address; }
};
struct c_object_placement_data_publisher {
    static __forceinline void publish(void *address) { data_set_new_base_address(&g_object_activation_regions, (s_data_array *)address); }
};
}

struct definition_tag_lookup
{
    s_object_definition *value;
};
static __forceinline definition_tag_lookup definition_tag_lookup_get(long definition_index)
{
    definition_tag_lookup result;
    result.value = (s_object_definition *)tag_get(NONE, definition_index);
    return result;
}

// @address 0x11df890 g_object_placement_globals_address
// @address 0x11df848 g_object_activation_regions
__declspec(thread) s_object_placement_globals *g_object_placement_globals_address;
__declspec(thread) s_data_array *g_object_activation_regions;

// @address 0x330940 ?__tls_set_g_object_activation_regions_allocator@@YAXPAX@Z
void __tls_set_g_object_activation_regions_allocator(void *address)
{
    data_set_new_base_address(&g_object_activation_regions, (s_data_array *)address);
}

// @address 0x330958 ?object_activation_regions_initialize@@YAXXZ
void object_activation_regions_initialize(void)
{
    g_object_activation_regions = data_new("object activation regions", 128, 40, 0, (c_allocation_base *)&g_object_activation_regions_allocator);
}

// @address 0x3309a0 ?object_activation_regions_delete_all@@YAXXZ
void object_activation_regions_delete_all(void)
{
    s_data_array *data = g_object_activation_regions;
    data->valid = 1;
    data_delete_all(data);
}

static __forceinline void object_placement_data_make_invalid(s_data_array *data)
{
    data->valid = 0;
}

// @address 0x3309b8 object_activation_regions_invalidate
extern "C" void object_activation_regions_invalidate(void)
{
    object_placement_data_make_invalid(g_object_activation_regions);
}

// @address 0x3309d0 object_activation_regions_dispose
extern "C" void object_activation_regions_dispose(void)
{
    data_dispose(g_object_activation_regions);
}

// @address 0x330bc8 ?__tls_set_g_object_placement_globals_allocator@@YAXPAX@Z
void __tls_set_g_object_placement_globals_allocator(void *value)
{
    g_object_placement_globals_address = (s_object_placement_globals *)value;
}

typedef unsigned int uint;



template<>
// @address 0x330b08 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_object_activation_regions_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_activation_regions_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_member<c_object_placement_data_publisher>(
        1, name, "", size, 0, __tls_set_g_object_activation_regions_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

template<>
// @address 0x330ba8 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_object_activation_regions_allocator@@YAXPAX@Z@@UAAXPAX@Z
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_activation_regions_allocator>::deallocate(void *allocation)
{
    g_object_activation_regions = 0;
    restricted_region_free_member(1, m_member_index);
}

void object_placement_initialize_for_new_map(void)
{
    g_object_placement_globals_address->placed_structure_bsp_mask = 0;
}

static __forceinline long object_type_get_palette_tag_block_offset(e_object_type type)
{
    return object_type_definition_get(type)->scenario_palette_offset;
}
static __forceinline s_tag_block *scenario_get_object_type_scenario_palette(scenario *scenario_data, e_object_type type)
{
    long offset = object_type_get_palette_tag_block_offset(type);
    return (s_tag_block *)((char *)scenario_data + offset);
}

scenario_object_palette_entry *scenario_object_get_palette_entry(scenario *scenario_data, const s_scenario_object *scenario_object)
{
    scenario_object_palette_entry *result = 0;
    if (scenario_object->palette_index != -1) {
        long palette_index = scenario_object->palette_index;
        e_object_type type = scenario_object->object_identifier.get_type();
        if (type >= 0 && type < 14) {
            object_type_definition *definition = object_type_definition_get(type);
            if (definition->scenario_palette_offset != -1) {
                s_tag_block *palette = scenario_get_object_type_scenario_palette(scenario_data, type);
                if (palette_index >= 0 && palette_index < palette->count)
                    result = (scenario_object_palette_entry *)tag_block_get_element_with_size(palette, palette_index, sizeof(scenario_object_palette_entry));
            }
        }
    }
    return result;
}

const s_scenario_object *scenario_object_try_and_get(scenario *scenario, e_object_type type, long index)
{
    const s_scenario_object *result = 0;
    if (type >= 0 && type < 14)
    {
        object_type_definition *definition = object_type_definition_get(type);
        s_tag_block *datums = (s_tag_block *)((char *)scenario + definition->scenario_datums_offset);
        if (index >= 0 && index < datums->count)
        {
            result = (const s_scenario_object *)((char *)datums->address + index * definition->scenario_datum_size);
        }
    }
    return result;
}


bool object_definition_has_persistent_state(e_object_type type, long definition_index)
{
    bool result = ((1 << type) & 0x1d9f) != 0;
    if (!result && ((1 << type) & 64) != 0)
    {
        s_object_definition *definition = (s_object_definition *)((void *)tag_get(0, definition_index));
        if (definition != 0 && definition->model.index != -1)
        {
            s_model_definition *model = (s_model_definition *)((void *)tag_get(0, definition->model.index));
            result = model->damage_info.count > 0;
        }
    }
    return result;
}

e_object_bsp_placement_policy get_runtime_object_placement_policy(const s_scenario_object *scenario_object)
{
    int result = -1;
    switch (scenario_object->bsp_placement_policy) {
    case 0:
        switch ((signed char)scenario_object->object_identifier.m_source) {
        case 0:
        case 1:
            result = ((1 << scenario_object->object_identifier.get_type()) & 0x1c0f) == 0;
            break;
        }
        break;
    case 1:
        result = 0;
        break;
    case 2:
        result = 2;
        break;
    }
    return (e_object_bsp_placement_policy)result;
}


template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_placement_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x332788: assume folds the alignment_bits argument into li r7,0 before the add_member call
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_member<c_object_placement_globals_publisher>(
        1, name, type, size, alignment_bits, __tls_set_g_object_placement_globals_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

// @address 0x7fc320 g_scenario_object_offsets
c_static_array<long, 14> g_scenario_object_offsets;

void object_placement_initialize(void)
{
    g_object_placement_globals_address = (s_object_placement_globals *)g_object_placement_globals_allocator.reserve_memory("object placement globals", 0, 4, 0);
    long offset = 0;
    for (long object_type = 0; object_type < 14; object_type++)
    {
        const object_type_definition *definition = object_type_definition_get((e_object_type)object_type);
        if ((unsigned short)definition->scenario_datums_offset != 0xffff && (unsigned short)definition->scenario_palette_offset != 0xffff)
        {
            g_scenario_object_offsets[object_type] = offset;
            offset += definition->maximum_placement_count;
        }
        else
        {
            g_scenario_object_offsets[object_type] = -1;
        }
    }
}

// @address 0x5bec98 ?object_placement_dispose@@YAXXZ
void object_placement_dispose(void)
{
}

// @address 0x5bec98 ?object_placement_dispose_from_old_map@@YAXXZ
void object_placement_dispose_from_old_map(void)
{
}

// @address 0x5bec98 ?object_placement_dispose_from_old_structure_bsp@@YAXK@Z
void object_placement_dispose_from_old_structure_bsp(unsigned long)
{
}


bool scenario_object_has_persistent_state(scenario *scenario_data, const s_scenario_object *scenario_object)
{
    bool result = false;
    if (scenario_object->name_index != -1)
    {
        result = true;
    }
    else
    {
        e_object_type object_type = scenario_object->object_identifier.get_type();
        scenario_object_palette_entry *palette_entry = scenario_object_get_palette_entry(scenario_data, scenario_object);
        if (palette_entry && palette_entry->object.index != -1)
        {
            long definition_index = palette_entry->object.index;
            result = object_definition_has_persistent_state(object_type, definition_index);
        }
    }
    return result;
}

template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_placement_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}


// @address 0x332840 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_object_placement_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_object_placement_globals_allocator>::deallocate(void *allocation)
{
    long member_index = m_member_index;
    g_object_placement_globals_address = 0;
    restricted_region_free_member(1, member_index);
}


// @address 0x330920 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_water_interaction_event_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_water_interaction_event_allocator>::deallocate(void *allocation)
{
    long member_index = m_member_index;
    g_water_interaction_data_address = 0;
    restricted_region_free_member(1, member_index);
}





static __forceinline structure_identifier object_placement_structure_identifier(long index, long surface)
{
    structure_identifier structure;
    if (surface != -1)
    {
        structure.bsp_index = (unsigned char)index;
        long datum = ((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[index].structure_bsp.index;
        const structure_bsp *bsp = (const structure_bsp *)tag_get(0, datum);
        structure.collision_bsp_index = ((const unsigned char *)bsp->leaves.address)[surface];
    }
    else
        *(short *)&structure = -1;
    return structure;
}

// @address 0x2b24c8 ?collision_bsp_test_sphere@@YA_NPBUcollision_bsp@@V?$c_flags@W4e_collision_bsp_test_sphere_flag@@K$02@@JPBUs_breakable_surface_set@@PBTreal_point3d@@MPAUcollision_bsp_test_sphere_result@@@Z

// @address 0x332470 object_placement_find_surface_from_sphere
extern "C" unsigned char object_placement_find_surface_from_sphere(
    unsigned long active_mask, const real_point3d *point, float radius, surface_identifier *output)
{
    bool result = false;
    unsigned long mask = g_active_structure_bsp_mask;
    long index = 31 - _CountLeadingZeros(-mask & mask);
    collision_bsp_test_sphere_result sphere_result;
    if (index != -1)
    {
        long thread_offset = get_current_thread_index() << 6;
        do
        {
#ifdef HALO3_64BIT
            collision_bsp_test_sphere(((const TAG_POINTER(const collision_bsp) *)&g_collision_bsp_cache)[thread_offset + index],
#else
            collision_bsp_test_sphere(((const collision_bsp **)&g_collision_bsp_cache)[thread_offset + index],
#endif
                c_collision_bsp_test_sphere_flags(5), 0, 0, point, radius, &sphere_result);
            if (sphere_result.surface_count != 0)
            {
                long surface = sphere_result.surface_planes[0];
                output->surface_index = (short)surface;
                output->structure = object_placement_structure_identifier(index, surface);
                result = true;
                break;
            }
            mask = (mask - 1) & mask;
            index = 31 - _CountLeadingZeros(-mask & mask);
        } while (index != -1);
    }
    return result;
}



// @address 0x3309e0 object_activation_regions_update_for_object
extern "C" void object_activation_regions_update_for_object(int activating_object, unsigned char allocate_region)
{
    s_object_header *object_header;
    unsigned char location_bsp;
    int region_timestamp;
    int new_region_index;
    object_placement_activation_region *new_region_entry;
    object_placement_activation_region local;
    object_header = datum_get<s_object_header>(object_header_data, activating_object);
    if (((1 << object_header->object_type) & 0x1cdf) == 0) return;
    if (((object_datum *)object_header->datum)->parent_object_index != -1) return;
    local.filter_info = havok_get_object_bounds_and_group(activating_object, &local.bounds);
    object_get_location(activating_object, &local.location);
    location_bsp = local.location.cluster_reference.bsp_index;
    region_timestamp = game_time_globals->time;
    local.creation_time = region_timestamp;
    if (location_bsp == 255) return;
    if (!allocate_region && g_havok_world) {
        havok_update_bodies_in_bounds(local.filter_info, &local.bounds);
        return;
    }
    new_region_index = datum_new(g_object_activation_regions);
    if (new_region_index != -1) {
        new_region_entry = datum_get<object_placement_activation_region>(g_object_activation_regions, new_region_index);
        new_region_entry->filter_info = local.filter_info;
        new_region_entry->bounds = local.bounds;
        s_location packed_location = local.location;
        new_region_entry->creation_time = region_timestamp;
        new_region_entry->location = packed_location;
    }
}

// match: 0x40db30: forwarding boundary pins argument setup order; inlined, mr r3,r31 sinks below the call setup
static __forceinline long object_new_from_scenario_bypass_simulation_object_placement_test(
    e_object_type type, long index, s_scenario_object *object, s_tag_block *palette, long definition, bool flag)
{
    return object_new_from_scenario_internal(type, (short)index, object, palette, definition, false, false, flag);
}

// @address 0x40db30 object_placement_create_from_simulation
extern "C" long object_placement_create_from_simulation(const s_simulation_object_creation_data *creation)
{
    s_object_definition *object = ((s_object_definition *)tag_get(0, creation->definition_index));
    long type = object->object_type;
    object_type_definition *definition = (object_type_definition *)((object_type_definition **)object_type_definitions)[type];
    long result = -1;
    if ((unsigned short)definition->scenario_datums_offset != 0xffff && (unsigned short)definition->scenario_palette_offset != 0xffff)
    {
        scenario *s = global_scenario;
        long size = 0;
        s_tag_block *datums = scenario_get_object_type_scenario_datums(s, (e_object_type)type, &size);
        if (creation->scenario_datum_index >= 0 && creation->scenario_datum_index < datums->count)
        {
            s_scenario_object *datum = (s_scenario_object *)tag_block_get_element_with_size(
                datums, creation->scenario_datum_index, size);
            s_tag_block *palette = scenario_get_object_type_scenario_palette(s, (e_object_type)type);
            if (datum->palette_index != -1 && datum->palette_index >= 0 && datum->palette_index < palette->count)
                result = object_new_from_scenario_bypass_simulation_object_placement_test(
                    (e_object_type)type, creation->scenario_datum_index, datum, palette, creation->definition_index, false);
        }
    }
    return result;
}


// @address 0x33a848 ?object_placement_deactivate_on_zone_set_switch@@YAXJJ@Z
void object_placement_deactivate_on_zone_set_switch(long object_index, long zone_set_index)
{
    s_model_variant *variant;
    object_datum *object = object_get(object_index);
    long model_index = definition_tag_lookup_get(object->definition_index).value->model.index;
    if (model_index != -1)
    {
        long region_index = 0;
        variant = &((s_model_variant *)TAG_GET(0, s_model_definition, model_index)->variants.address)[object->variant_index];
        long region_count = variant->attachments.count;
        for (; region_index < region_count; region_index++)
        {
            s_model_variant_attachment *entry = (s_model_variant_attachment *)tag_block_get_element_with_size(
                &variant->attachments, region_index, sizeof(s_model_variant_attachment));
            if (entry->parent_marker == zone_set_index && entry->child_object.index != -1)
            {
                long child_index = object->first_child_object_index;
                while (child_index != -1)
                {
                    object_datum *child = object_get(child_index);
                    long next_index = child->next_object_index;
                    if ((bool)((child->flags >> 26) & 1) && child->marker == region_index && !simulation_query_object_is_predicted(child_index))
                        ::object_destroy_internal(child_index, -1, -1, 0, 0);
                    child_index = next_index;
                }
            }
        }
    }
}

extern "C" {
// @address 0x8c4438 g_object_placement_sky_objects_enabled
signed char g_object_placement_sky_objects_enabled;
}

// @address 0x3319b8 object_placement_deactivate_zone_object
extern "C" void object_placement_deactivate_zone_object(int object_index)
{
    object_datum *object = (object_datum *)object_get(object_index);
    bool should_remain = object->object_name_index != -1 ||
        (object->object_identifier.m_source == 4 && g_object_placement_sky_objects_enabled == 0) ||
        object_definition_has_persistent_state((e_object_type)(signed char)object->object_type, object->definition_index);
    int object_index_copy = object_index;
    if (should_remain) {
        object_set_in_limbo(object_index_copy, true);
        object->flags = object->flags | 0x8000000;
        return;
    } else {
        if ((unsigned char)simulation_query_object_is_predicted(object_index_copy) == 0) {
            objects_purge_deleted_object(object_index);
        }
        return;
    }
}


static __forceinline void object_placement_item_flags_set(unsigned char *flags, long bit, bool enable)
{
    if (enable)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}
static __forceinline s_variant_object_datum *object_placement_variant_placement_get(c_map_variant *variants, long index)
{
    s_variant_object_datum *result = 0;
    if ((unsigned long)index <= 639)
        result = &variants->objects[index];
    return result;
}

// @address 0x35cb10 object_placement_update_multiplayer_item_flags
extern "C" void object_placement_update_multiplayer_item_flags(long object_index)
{
    if (game_globals->options.game_mode == 2)
    {
        s_object_header *header = datum_get<s_object_header>(object_header_data, object_index);
        object_datum *object = (object_datum *)header->datum;
        bool result = false;
        if (object->name_index != -1)
        {
            const s_scenario_object *placement = scenario_object_try_and_get(global_scenario, (e_object_type)header->object_type, object->name_index);
            result = scenario_object_get_palette_entry(global_scenario, placement)->object.index != object->definition_index;
        }
        else if (object->map_variant_index != -1)
        {
            c_map_variant *variants = &game_engine_globals->map_info;
            s_variant_object_datum *entry = object_placement_variant_placement_get(variants, object->map_variant_index);
            if (entry && entry->quota_index != -1 && (entry->flags.m_storage & 0x3ff) != 0 && !test_bit(entry->flags.m_storage >> 3, 0))
                result = variants->quotas[entry->quota_index].object_definition_index == object->definition_index;
        }
        if (result)
        {
            object_set_at_rest(object_index, false);
            object_placement_item_flags_set(&((item_datum *)object)->inventory_flags, 2, true);
            object_placement_item_flags_set(&((item_datum *)object)->inventory_flags, 3, true);
        }
    }
}


static __forceinline const s_scenario_object *object_placement_scenario_object_get(long type, long index)
{
    const s_scenario_object *result = 0;
    object_type_definition *definition = (object_type_definition *)((object_type_definition **)object_type_definitions)[type];
    if ((unsigned short)definition->scenario_datums_offset != 65535 && (unsigned short)definition->scenario_palette_offset != 65535)
    {
        scenario *s = global_scenario;
        long size;
        s_tag_block *objects = scenario_get_object_type_scenario_datums(s, (e_object_type)type, &size);
        if (index >= 0 && index < objects->count)
            result = (const s_scenario_object *)((char *)objects->address + index * size);
    }
    return result;
}

static __forceinline bool object_placement_cinematic_suppressing_bsp_object_creation()
{
    return cinematic_globals->suppress_bsp_object_creation;
}

void object_placement_initialize_for_new_structure_bsp(unsigned long activating_structure_bsp_mask)
{
    progress_new("placing objects on bsp");
    ++g_havok_monitor_globals.lock_count_4c;
    bool cinematic_in_progress = false;
    if (cinematic_globals)
        cinematic_in_progress = cinematic_globals->in_progress;
    object_placement_create_active_zone_set_objects(
        cinematic_in_progress && object_placement_cinematic_suppressing_bsp_object_creation() ? 2 : 1);
    --g_havok_monitor_globals.lock_count_4c;
    progress_done();
}

// @address 0x330d28 ?object_placement_prepare_for_new_zone_set@@YAXKK@Z
void object_placement_prepare_for_new_zone_set(unsigned long old_mask, unsigned long new_mask)
{
    ++g_havok_monitor_globals.lock_count_4c;
    if (new_mask != old_mask)
    {
        c_object_iterator<object_datum> iterator;
        iterator.begin(0, 0);
        while (iterator.next())
        {
            object_datum *object = iterator.get_datum();
            if (object->parent_object_index == -1 && object->object_identifier.m_source != -1)
            {
                long index = iterator.get_index();
                const s_scenario_object *scenario_object = ((1L << object->object_identifier.m_source) & 3)
                    ? object_placement_scenario_object_get(object->object_type, object->name_index)
                    : 0;
                bool in_limbo = test_bit(object->flags, 7);
                bool pending = test_bit(object->flags, 27);
                bool valid;
                {
                    bool result = false;
                    object_datum *placement_object = object_get(index);
                    switch (placement_object->bsp_placement_policy)
                    {
                    case 0:
                        result = true;
                        break;
                    case 1:
                    case 2:
                        result = (scenario_object->bsp_mask & new_mask) != 0;
                        break;
                    case -1:
                        result = false;
                        break;
                    default:
                        break;
                    }
                    valid = result;
                }
                if (object->object_identifier.m_source == 4 && g_object_placement_sky_objects_enabled)
                {
                    objects_purge_deleted_object(index);
                }
                else
                {
                    if (object_is_or_contains_player(index))
                        valid = true;
                    if (in_limbo)
                    {
                        if (valid && pending)
                        {
                            object_set_in_limbo(index, false);
                            object->flags &= ~(1UL << _object_data_flag_27);
                        }
                    }
                    else if (!valid)
                        object_placement_deactivate_zone_object(index);
                }
            }
        }
    }
    --g_havok_monitor_globals.lock_count_4c;
}


static __forceinline structure_bsp *object_placement_structure_bsp_definition_get(long bsp_index)
{
    scenario_structure_bsp_reference *reference = &((scenario_structure_bsp_reference *)global_scenario->structure_bsps.address)[bsp_index];
    if (reference && reference->structure_bsp.index != -1)
        return (structure_bsp *)tag_get(0, reference->structure_bsp.index);
    return 0;
}


// @address 0x330fa0 ?object_placement_prepare_for_designer_zone_set_switch@@YAXPBUs_game_non_bsp_zone_set@@0PAVc_scenario_resource_registry@@@Z
void object_placement_prepare_for_designer_zone_set_switch(
    const s_game_non_bsp_zone_set *old_zone_set, const s_game_non_bsp_zone_set *new_zone_set, c_scenario_resource_registry *pending_zone_registry)
{
    c_object_iterator<object_datum> iterator;
    iterator.begin(0, 0);
    ++g_havok_monitor_globals.lock_count_4c;
    while (iterator.next())
    {
        object_datum *object = iterator.get_datum();
        if (!pending_zone_registry->is_tag_available(object->definition_index & 0xffff))
        {
            long object_index = iterator.get_index();
            players_detach_units_from_object(object_index);
            objects_purge_deleted_object(object_index);
        }
    }
    --g_havok_monitor_globals.lock_count_4c;
}

// @address 0xd5ef8 object_activation_regions_update
extern "C" void object_activation_regions_update()
{
    long oldest_region_index = -1;
    long oldest_creation_time = 0x7fffffff;
    c_data_iterator<object_placement_activation_region> iterator;
    iterator.begin(g_object_activation_regions);
    while (iterator.next())
    {
        object_placement_activation_region *region = iterator.get_datum();
        if ((1 << region->location.cluster_reference.bsp_index) & g_active_structure_bsp_mask)
        {
            structure_bsp *structure_bsp = object_placement_structure_bsp_definition_get(region->location.cluster_reference.bsp_index);
            if (region->location.cluster_reference.cluster_index < structure_bsp->clusters.count)
                havok_update_bodies_in_bounds(region->filter_info, &region->bounds);
            datum_delete(g_object_activation_regions, iterator.get_index());
        }
        else if (region->creation_time < oldest_creation_time)
        {
            oldest_region_index = iterator.get_index();
            oldest_creation_time = region->creation_time;
        }
    }
    if (oldest_region_index != -1 && g_object_activation_regions->actual_count > 96)
        datum_delete(g_object_activation_regions, oldest_region_index);
}

// ---------------------------------------------------------------------------
// 0x3325b0 types & callees (appended after all accepted siblings so their
// $M symbol numbering is undisturbed)
// ---------------------------------------------------------------------------

// @address 0x3325b0 object_placement_reinitialize_created_objects
extern "C" void object_placement_reinitialize_created_objects(unsigned long *bitmask, char *scenario_base)
{
    for (long type = 0; type < 14; type++) {
        object_type_definition *definition = (object_type_definition *)((object_type_definition **)object_type_definitions)[type];
        if ((unsigned short)definition->scenario_datums_offset != 0xffff && (unsigned short)definition->scenario_palette_offset != 0xffff) {
            object_type_definition *type_definition = object_type_definition_get((e_object_type)type);
            s_tag_block *block = (s_tag_block *)(scenario_base + type_definition->scenario_datums_offset);
            long stride = type_definition->scenario_datum_size;
            long offset = g_scenario_object_offsets[type];
            for (long l = 0; l < block->count; l++) {
                unsigned long word = (unsigned long)l + (unsigned long)offset;
                if (((1u << (word & 31)) & bitmask[(long)word >> 5]) == 0)
                    continue;
                s_scenario_object *element = (s_scenario_object *)tag_block_get_element_with_size(block, l, stride);
                bool valid = false;
                if ((unsigned short)element->parent_name_index != 0xffff) {
                    short variant = (short)element->parent_name_index;
                    if (variant >= 0 && variant < 0x800) {
                        if (((long *)g_object_name_list)[variant] != -1)
                            valid = true;
                    }
                }
                if (!valid)
                    continue;
                long object_index = element->object_identifier.find_object_index();
                if (object_index != -1) {
                    s_tag_block *palette_block = scenario_get_object_type_scenario_palette((scenario *)scenario_base, (e_object_type)type);
                    object_placement_data placement;
                    if (object_placement_data_new_from_scenario_object(
                            (e_object_type)type, l, (const s_scenario_object *)element, palette_block, false, true, &placement))
                    {
                        if (object_get(object_index)->parent_object_index == -1)
                            object_set_in_limbo(object_index, false);
                        object_type_adjust_placement(&placement);
                        object_type_unplace(object_index);
                        object_reinitialize_from_placement(object_index, &placement, false);
                        object_type_place(object_index, (const s_scenario_object *)element);
                    }
                }
            }
        }
    }
}


static __forceinline game_options *object_placement_options_get()
{
    return &game_globals->options;
}

static __forceinline c_game_variant *object_placement_game_variant_get()
{
    game_options *options = object_placement_options_get();
    return options->game_mode == 2 ? &options->game_variant : 0;
}
static __forceinline bool object_placement_rules_symmetric(game_options *options)
{
    c_game_variant *variant = object_placement_game_variant_get();
    return variant
        ? object_placement_game_variant_get()->get_rules_are_symmetric() : false;
}
static __forceinline bool object_placement_map_variant_valid(const game_options *options)
{
    return map_variant_is_valid((const s_map_variant_validation *)&options->map_variant) && !options->map_variant.m_built_in;
}
static __forceinline void object_placement_filter_engine(bool *allowed, unsigned short flags, long engine)
{
    switch (engine) {
    case 1: *allowed &= (flags & (1 << 0)) != 0; break;
    case 2: *allowed &= (flags & (1 << 1)) != 0; break;
    case 3: *allowed &= (flags & (1 << 2)) != 0; break;
    case 4: *allowed &= (flags & (1 << 3)) != 0; break;
    case 6: *allowed &= (flags & (1 << 7)) != 0; break;
    case 7: *allowed &= (flags & (1 << 4)) != 0; break;
    case 8: *allowed &= (flags & (1 << 5)) != 0; break;
    case 9: *allowed &= (flags & (1 << 6)) != 0; break;
    case _game_engine_infection: *allowed &= (flags & (1 << 8)) != 0; break;
    }
}
static __forceinline void object_placement_pixel_to_color(unsigned long pixel, real_rgb_color *color)
{
    color->red = (float)((pixel >> 16) & 255) * (1.0f / 255.0f);
    color->green = (float)((pixel >> 8) & 255) * (1.0f / 255.0f);
    color->blue = (float)(pixel & 255) * (1.0f / 255.0f);
}

// @address 0x331088
bool object_placement_data_new_from_scenario_object(e_object_type type, long scenario_index, const s_scenario_object *object,
    s_tag_block *palette, bool force, bool cinematic, object_placement_data *placement)
{
    bool result = false;
    bool multiplayer = game_is_multiplayer();
    bool allowed = (object->placement_flags & (1 << 6)) == 0;
    long engine = multiplayer ? object_placement_options_get()->game_variant.get_game_engine_index() : 0;
    object_type_definition *type_definition = object_type_definition_get(type);
    const s_scenario_object_placement *scenario_properties = 0;
    s_scenario_object_placement properties;
    scenario_object_palette_entry *palette_entry = object->palette_index != -1 && object->palette_index >= 0 && object->palette_index < palette->count
        ? (scenario_object_palette_entry *)tag_block_get_element_with_size(palette, object->palette_index, sizeof(scenario_object_palette_entry)) : 0;
    long definition_index = palette_entry ? palette_entry->object.index : -1;
    allowed &= definition_index != -1;
    memset(&properties, 0, sizeof(properties));
    if (allowed) {
      if (multiplayer) {
        switch (type) {
        case _object_type_vehicle: scenario_properties = &((const s_scenario_unit_placement *)object)->multiplayer; break;
        case _object_type_weapon: scenario_properties = &((const s_scenario_unit_placement *)object)->multiplayer; break;
        case _object_type_equipment: scenario_properties = &((const s_scenario_equipment *)object)->multiplayer; break;
        case _object_type_scenery: scenario_properties = &((const s_scenario_scenery_placement *)object)->multiplayer; break;
        case _object_type_crate: scenario_properties = &((const s_scenario_crate_placement *)object)->multiplayer; break;
        }
        if (scenario_properties)
            memcpy(&properties, scenario_properties, sizeof(properties));
      }
        if (multiplayer) {
            switch (type) {
            case _object_type_vehicle: definition_index = game_engine_remap_vehicle_definition_index(definition_index); break;
            case _object_type_weapon: definition_index = game_engine_remap_weapon_definition_index(definition_index); break;
            }
            if (scenario_properties) {
                game_globals_storage *globals = game_globals;
                c_map_variant *map_variant = &globals->options.map_variant;
                if (map_variant_is_valid((const s_map_variant_validation *)map_variant) && !globals->options.map_variant.m_built_in) {
                    long index = map_variant->get_object_index(type, scenario_index);
                    s_variant_object_datum variant_object;
                    map_variant->get_variant_object_datum(index, &variant_object);
                    if (variant_object.flags.test(_variant_object_placement_edited_bit))
                        map_variant_set_placement_properties(&variant_object.properties, &properties);
                }
                bool symmetric = object_placement_rules_symmetric(object_placement_options_get());
                bool asymmetric = game_engine_has_asymmetric_rules();
                if (!game_engine_sandbox_is_active()) {
                    if (properties.placement_mode) {
                        if (symmetric) allowed &= properties.placement_mode == 1;
                        if (asymmetric) allowed &= properties.placement_mode == 2;
                    }
                }
                if (allowed && (properties.engine_flags & 0x3ff))
                    object_placement_filter_engine(&allowed, properties.engine_flags, engine);
            }
            if (allowed && definition_index != -1) {
                const s_object_definition *definition = ((const s_object_definition *)tag_get(0, definition_index));
                long multiplayer_count = definition->multiplayer_object.count();
                bool has_properties = multiplayer_count > 0;
                long grenade_index = game_get_grenade_type_index_from_item_defintion(definition_index);
                if (grenade_index != -1)
                    allowed = (object_placement_game_variant_get()->variant_flags & 1) != 0;
                if (has_properties && multiplayer) {
                    const unsigned short *engine_flags =
                        &((const s_multiplayer_object_properties_definition *)definition->multiplayer_object.address)->game_engine_flags.m_storage;
                    if (*engine_flags & 0x3ff)
                        object_placement_filter_engine(&allowed, *engine_flags, engine);
                }
            }
        }
    }
    if (allowed && type_definition) {
        if (definition_index != -1) {
            object_placement_data_new(placement, definition_index, -1, 0);
            placement->position = object->position;
            float scale = object->scale;
            if (scale <= 0.0f) scale = 1.0f;
            placement->scale = scale;
            placement->multiplayer_cinematic_object = cinematic;
            vectors3d_from_euler_angles3d(&placement->forward, &placement->up, &object->rotation);
            placement->bsp_placement_policy = (char)get_runtime_object_placement_policy(object);
            result = true;
            if (((1 << type) & 0x1447) && (unsigned short)type_definition->scenario_datum_size >= 108) {
                const s_scenario_object_permutation *permutation = (const s_scenario_object_permutation *)object;
                placement->model_variant_index = permutation->model_variant;
                placement->active_change_colors = permutation->active_colors;
                if (!placement->model_variant_index) placement->model_variant_index = 0;
                object_placement_pixel_to_color(permutation->colors[0], &placement->change_colors[0]);
                object_placement_pixel_to_color(permutation->colors[1], &placement->change_colors[1]);
                object_placement_pixel_to_color(permutation->colors[2], &placement->change_colors[2]);
                object_placement_pixel_to_color(permutation->colors[3], &placement->change_colors[3]);
            }
        }
        placement->object_identifier = object->object_identifier;
        if (force && ((1 << object->object_identifier.get_type()) & 0x1cdf))
            placement->flags |= 8;
        else if (test_bit(object->placement_flags, 8))
            placement->flags |= 8;
        else
            placement->flags &= ~8;
        placement->scenario_datum_index = scenario_index;
        if (scenario_properties)
            memcpy(&placement->multiplayer_properties, &properties, sizeof(properties));
        placement->parent_marker = 0;
        placement->child_marker = 0;
        placement->parent_object_index = -1;
        if ((unsigned short)object->parent_name_index != 0xffff && object->parent_name_index >= 0 && object->parent_name_index < 2048) {
            long parent_index = ((long *)g_object_name_list)[object->parent_name_index];
            if (parent_index != -1 && parent_index != object->object_identifier.find_object_index()) {
                placement->parent_object_index = parent_index;
                placement->parent_marker = object->parent_marker;
                placement->child_marker = object->child_marker;
                if (object_try_and_get_and_verify_type(parent_index, -1)) {
                    real_matrix4x3 matrix;
                    object_get_world_matrix(parent_index, &matrix);
                    placement->position = matrix.position;
                }
            }
        }
        if (object_placement_options_get()->game_mode == 2) {
            game_globals_storage *globals = game_globals;
            c_map_variant *map_variant = &globals->options.map_variant;
            if (map_variant_is_valid((const s_map_variant_validation *)map_variant) && !globals->options.map_variant.m_built_in) {
                long index = map_variant->get_object_index(type, scenario_index);
                if (index != -1) {
                    const s_variant_object_datum *variant_object = 0;
                    if (index >= 0 && index < 640)
                        variant_object = &globals->options.map_variant.objects[index];
                    if (variant_object->flags.test(_variant_object_placement_edited_bit) &&
                        !variant_object->flags.test(_variant_object_placement_spawn_from_object_bit))
                    {
                        placement->parent_marker = 0;
                        placement->child_marker = 0;
                        placement->parent_object_index = -1;
                    }
                }
            }
        }
    }
    return result;
}


static __forceinline bool scenario_object_managed_by_script(scenario *scenario_data, const s_scenario_object *scenario_object)
{
    return scenario_object->name_index != NONE;
}
static __forceinline e_object_bsp_placement_policy scenario_object_runtime_placement_policy(const s_scenario_object *scenario_object)
{
    int result = -1;
    switch (scenario_object->bsp_placement_policy) {
    case 0:
        switch ((signed char)scenario_object->object_identifier.m_source) {
        case 0:
            result = ((1 << scenario_object->object_identifier.get_type()) & 0x1c0f) == 0;
            break;
        case 1:
            result = ((1 << scenario_object->object_identifier.get_type()) & 0x1c0f) == 0;
            break;
        }
        break;
    case 1:
        result = 0;
        break;
    case 2:
        result = 2;
        break;
    }
    return (e_object_bsp_placement_policy)result;
}
static __forceinline bool scenario_object_is_placed_globally(scenario *scenario_data, const s_scenario_object *scenario_object)
{
    bool placed_automatically = !test_bit(scenario_object->placement_flags, 0);
    bool placed_globally = test_bit(scenario_object->placement_flags, 11);
    // Evaluate the runtime placement policy before checking global placement flags.
    bool global = scenario_object_managed_by_script(scenario_data, scenario_object) || scenario_object_runtime_placement_policy(scenario_object) != 1;
    bool persistent_or_global = global || placed_globally;
    return placed_automatically && placed_globally && !test_bit(scenario_object->placement_flags, 10);
}
static __forceinline c_map_variant *object_placement_runtime_map_variant()
{
    return (c_map_variant *)&game_engine_globals->map_info;
}

static __forceinline bool object_placement_map_variant_collect_object_type(e_object_type type)
{
    return type == _object_type_scenery || type == _object_type_crate || type == _object_type_vehicle || type == _object_type_weapon ||
        type == _object_type_equipment;
}
// match: 0x331a98: retain raw flag stores to preserve the shared map-variant flag setter.
static __forceinline void object_placement_variant_object_flag_set(s_variant_object_datum *variant_object, e_variant_object_placement_flags bit, bool enable)
{
    unsigned short mask = (unsigned short)(1 << bit);
    if (enable)
        variant_object->flags.m_storage |= mask;
    else
        variant_object->flags.m_storage &= ~mask;
}
__forceinline void c_map_variant::register_scenario_object(e_object_type type, long scenario_index)
{
    if (object_placement_map_variant_collect_object_type(type)) {
        long variant_object_index = object_type_start[type] + scenario_index;
        if (game_is_authoritative()) {
            object_placement_variant_object_flag_set(&objects[variant_object_index], _variant_object_placement_runtime_candidate_bit, true);
            object_placement_variant_object_flag_set(&objects[variant_object_index], _variant_object_placement_registered_bit, true);
        }
    }
}

// @address 0x331a98
void object_placement_create_global_objects(long game_mode, bool only_unplaced)
{
    scenario *scenario_data = global_scenario;
    c_static_flags<5888> created_objects;
    created_objects.clear();
    for (e_object_type type = _object_type_first; type < k_object_type_count; type = (e_object_type)(type + 1)) {
        object_type_definition *definition = (object_type_definition *)((object_type_definition **)object_type_definitions)[type];
        if ((unsigned short)definition->scenario_datums_offset != 0xffff && (unsigned short)definition->scenario_palette_offset != 0xffff) {
            long datum_size;
            s_tag_block *datums = scenario_get_object_type_scenario_datums(scenario_data, type, &datum_size);
            s_tag_block *palette = scenario_get_object_type_scenario_palette(scenario_data, type);
            long offset = g_scenario_object_offsets[type];
            for (long index = 0; index < datums->count; index++) {
                s_scenario_object *scenario_object = (s_scenario_object *)tag_block_get_element_with_size(datums, index, datum_size);
                bool place = scenario_object_is_placed_globally(scenario_data, scenario_object);
                if (only_unplaced && scenario_object->object_identifier.find_object_index() != NONE)
                    place = false;
                if (place) {
                    s_variant_multiplayer_object_properties_definition properties;
                    if (game_is_multiplayer() &&
                        !map_variant_can_create_scenario_object(&object_placement_options_get()->map_variant, type, index, &properties))
                    {
                        if (game_engine_running())
                            map_variant_set_scenario_object_removed(object_placement_runtime_map_variant(), type, index, &properties);
                    } else {
                        long object_index = object_new_from_scenario_internal(type, (short)index,
                            scenario_object, palette, NONE, true, true,
                            false);
                        objects_garbage_collection();
                        if (object_index != NONE)
                            created_objects.set(offset + index, true);
                        if (game_is_multiplayer() && game_engine_running() && object_index == NONE) {
                            c_map_variant *map_variant = object_placement_runtime_map_variant();
                            map_variant->register_scenario_object(type, index);
                        }
                    }
                }
            }
        }
    }
    object_placement_reinitialize_created_objects(created_objects.get_writeable_bits_direct(), (char *)scenario_data);
}


static __forceinline const scenario *object_placement_scenario_tag_get()
{
    return (const scenario *)tag_get(0, global_scenario_index);
}
static __forceinline void object_placement_create_sky_objects(unsigned long active_structure_bsp_mask)
{
    const scenario *scenario_tag = object_placement_scenario_tag_get();
    for (long sky_index = 0; sky_index < scenario_tag->skies.count; sky_index++) {
        const scenario_sky_reference *sky = (const scenario_sky_reference *)tag_block_get_element_with_size(
            &scenario_tag->skies, sky_index, sizeof(scenario_sky_reference));
        if (sky->active_bsps & active_structure_bsp_mask)
            c_scenerio_sky_objects_helper::create_sky_object(sky_index);
    }
}
static __forceinline bool scenario_object_placed_automatically(const s_scenario_object *scenario_object)
{
    return !test_bit(scenario_object->placement_flags, 0);
}
static __forceinline bool scenario_object_placed_globally(unsigned long placement_flags)
{
    const bool &placed_automatically = !test_bit(placement_flags, 0);
    const bool &placed_globally = test_bit(placement_flags, 11);
    return placed_automatically && placed_globally;
}
static __forceinline bool scenario_object_placed_in_structure_bsps(const s_scenario_object *scenario_object, unsigned long structure_bsp_mask)
{
    const bool &in_active_structure_bsp = (scenario_object->bsp_mask & structure_bsp_mask) != 0;
    const bool &placed_automatically = scenario_object_placed_automatically(scenario_object);
    return in_active_structure_bsp && placed_automatically;
}

// @address 0x331de0 ?object_placement_create_active_zone_set_objects@@YAXJ@Z
void object_placement_create_active_zone_set_objects(long mode)
{
    scenario *scenario_data = global_scenario;
    unsigned long active_structure_bsp_mask = g_active_structure_bsp_mask;
    object_placement_create_sky_objects(active_structure_bsp_mask);
    if (mode != 2) {
        unsigned long activating_structure_bsp_mask = active_structure_bsp_mask & ~g_object_placement_globals_address->placed_structure_bsp_mask;
        c_static_flags<5888> existing_objects;
        existing_objects.clear();
        c_static_flags<5888> created_objects;
        created_objects.clear();
        {
            c_object_iterator<object_datum> iterator;
            iterator.begin(0, 0);
            while (iterator.next()) {
                object_datum *object = iterator.get_datum();
                if ((1L << object->object_identifier.m_source) & 3)
                    existing_objects.set(g_scenario_object_offsets[object->object_type] + object->name_index, true);
            }
        }
        for (e_object_type type = _object_type_first; type < k_object_type_count; type = (e_object_type)(type + 1)) {
            object_type_definition *definition = (object_type_definition *)((object_type_definition **)object_type_definitions)[type];
            if ((unsigned short)definition->scenario_datums_offset != 0xffff && (unsigned short)definition->scenario_palette_offset != 0xffff) {
                long datum_size;
                s_tag_block *datums = scenario_get_object_type_scenario_datums(scenario_data, type, &datum_size);
                s_tag_block *palette = scenario_get_object_type_scenario_palette(scenario_data, type);
                long offset = g_scenario_object_offsets[type];
                objects_garbage_collection();
                memory_pool_compact(g_object_memory_pool);
                for (long index = 0; index < datums->count; index++) {
                    long absolute_index = offset + index;
                    if (existing_objects.test(absolute_index))
                        continue;
                    s_scenario_object *scenario_object = (s_scenario_object *)tag_block_get_element_with_size(datums, index, datum_size);
                    if (test_bit(scenario_object->placement_flags, 10) || scenario_object_placed_globally(scenario_object->placement_flags))
                        continue;
                    bool persistent = mode == 1 && scenario_object_has_persistent_state(scenario_data, scenario_object);
                    unsigned long structure_bsp_mask = persistent ? activating_structure_bsp_mask : active_structure_bsp_mask;
                    if (!scenario_object_placed_in_structure_bsps(scenario_object, structure_bsp_mask))
                        continue;
                    bool create = true;
                    if (game_is_multiplayer() && game_engine_running()) {
                        s_variant_multiplayer_object_properties_definition properties;
                        create = map_variant_can_create_scenario_object(&object_placement_options_get()->map_variant, type, index, &properties);
                        if (create)
                            create = map_variant_can_create_scenario_object(object_placement_runtime_map_variant(), type, index, 0);
                        else
                            map_variant_set_scenario_object_removed(object_placement_runtime_map_variant(), type, index, &properties);
                    }
                    if (create) {
                        if (object_new_from_scenario_internal(type, (short)index, scenario_object,
                                palette, NONE, true, true, false) != NONE)
                            created_objects.set(absolute_index, true);
                        objects_garbage_collection();
                    }
                }
            }
        }
        object_placement_reinitialize_created_objects(created_objects.get_writeable_bits_direct(), (char *)scenario_data);
        g_object_placement_globals_address->placed_structure_bsp_mask |= active_structure_bsp_mask;
    }
}


c_object_activation_regions_allocation_manager g_object_activation_regions_allocator;
c_object_placement_globals_allocation_manager g_object_placement_globals_allocator;
