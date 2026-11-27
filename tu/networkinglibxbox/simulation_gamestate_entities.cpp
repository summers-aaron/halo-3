// networkinglibxbox/simulation_gamestate_entities.obj: from the records 0x229af0 0x22d1b8 0x22d258 0x3aef88 0x3aefa0 0x3af058 0x3af0d0 0x3af138 0x3af198 0x3af200 0x3af2a0 0x40dda0 0x42d328 0x42d6a0 0x42d9b8 0x42da08
// @flags /TP /O2
#include "physics/breakable_surface_event_data.h"
#include "objects/object_header.h"
#include "game/map_variant.h"
#include "multithreading/thread_tls.h"
#include "memory/data_types.h"
#include "objects/object_datum.h"
#include "memory/data_typed_access.h"
// @address 0x229af0 ?__tls_set_g_simulation_gamestate_entity_data_allocator@@YAXPAX@Z
// @address 0x22d1b8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_simulation_gamestate_entity_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x22d258 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_simulation_gamestate_entity_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x3aefa0 ?simulation_gamestate_entities_build_clear_flags@@YAXPAUs_simulation_queue_gamestate_clear_data@@@Z
// @address 0x3af058 ?simulation_gamestate_entities_notify_simulation_world_reset@@YAXXZ
// @address 0x3af0d0 ?simulation_gamestate_entity_create@@YAJXZ
// @address 0x3af138 ?simulation_gamestate_entity_get_object_index@@YAJJ@Z
// @address 0x3af198 ?simulation_gamestate_entity_try_and_get_object_index@@YAJJK@Z
// @address 0x3af200 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?tls_set_g_gamestate_entity_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x3af2a0 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?tls_set_g_gamestate_entity_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x40dda0 ?apply_game_event@c_simulation_object_refresh_event_definition@@UAA_NJPBJJPBX@Z
// @address 0x42d328 ?gameworld_attachment_valid@c_simulation_map_variant_group_entity_definition@@UAA_NJ@Z
// @address 0x42d6a0 ?delete_game_entity@c_simulation_map_variant_group_entity_definition@@UAA_NJ@Z
// @address 0x42d9b8 ?build_updated_state_data@c_simulation_map_variant_group_entity_definition@@UAA_NPBUs_simulation_entity@@PAKJPAX@Z
// @address 0x42da08 ?update_game_entity@c_simulation_map_variant_group_entity_definition@@UAA_NJKJPBX@Z
#include <string.h>
#include "cache/restricted_memory_regions.h"
#include "simulation/game_interface/simulation_game_entities.h"
#include "simulation/simulation_gamestate_entities.h"
#include "simulation/game_interface/simulation_game_objects.h"
#include "memory/tls_globals.h"
#include "scenario/scenario_map_variant.h"


#include "simulation/simulation_gamestate_entities.h"
#include "game/game_engine_scoring_globals.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_memory_registration.h"
#include "simulation/game_interface/simulation_game_breakable_surfaces.h"
#include "networking/simulation/simulation_entity_database.h"
#include "simulation/game_interface/simulation_game_map_variant.h"







static __forceinline s_simulation_gamestate_entity_datum *simulation_gamestate_entity_datum_get(long index)
{
    return (s_simulation_gamestate_entity_datum *)(tls_simulation_gamestate_entity_data()->data +
        (index & 0xffff) * sizeof(s_simulation_gamestate_entity_datum));
}

void __tls_set_g_simulation_gamestate_entity_data_allocator(void *object)
{
    data_set_new_base_address(&g_breakable_surface_event_data, (s_data_array *)object);
}

static __forceinline long restricted_region_add_simulation_gamestate_entity_member(const char *name, const char *type,
    unsigned int size, long alignment_bits)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, alignment_bits,
        __tls_set_g_simulation_gamestate_entity_data_allocator, 0, 0);
    void *address = restricted_region_get_member_address(3, member_index);
    data_set_new_base_address(&g_breakable_surface_event_data, (s_data_array *)address);
    return member_index;
}
static __forceinline long restricted_region_add_object_member(const char *name, const char *type,
    unsigned int size, long alignment_bits)
{
    long member_index = restricted_memory_register_member(&g_restricted_regions[1], name, type, size, alignment_bits,
        tls_set_g_gamestate_entity_data_allocator, 0, 0);
    void *address = restricted_region_get_member_address(1, member_index);
    data_set_new_base_address(&tls_simulation_gamestate_entity_data_slot(), (s_data_array *)address);
    return member_index;
}


// 0x22d1b8
template <>
void *t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_simulation_gamestate_entity_data_allocator>::allocate(
    unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_simulation_gamestate_entity_member(name, "", size, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// 0x22d258
template <>
void t_restricted_allocation_manager<3,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_simulation_gamestate_entity_data_allocator>::deallocate(void *memory)
{
    g_breakable_surface_event_data = 0;
    restricted_region_free_member(3, m_member_index);
}

// @address 0x3aef88 ?tls_set_g_gamestate_entity_data_allocator@@YAXPAX@Z
void tls_set_g_gamestate_entity_data_allocator(void *object)
{
    data_set_new_base_address(&tls_simulation_gamestate_entity_data_slot(), (s_data_array *)object);
}

void simulation_gamestate_entities_build_clear_flags(s_simulation_queue_gamestate_clear_data *clear_data)
{
    c_data_iterator<s_simulation_gamestate_entity_datum> iterator;
    memset(clear_data, 0, sizeof(*clear_data));
    iterator.begin(tls_simulation_gamestate_entity_data());
    while (iterator.next())
    {
        s_simulation_gamestate_entity_datum *datum = iterator.get_datum();
        if (datum->deletion_queued == 0)
        {
            short absolute_index = iterator.get_absolute_index();
            clear_data->bits[absolute_index >> 5] |= 1UL << (absolute_index & 31);
            datum->deletion_queued = 1;
        }
    }
}

void simulation_gamestate_entities_notify_simulation_world_reset(void)
{
    c_data_iterator<s_simulation_gamestate_entity_datum> iterator;
    iterator.begin(tls_simulation_gamestate_entity_data());
    while (iterator.next())
        iterator.get_datum()->entity_index = -1;
}

long simulation_gamestate_entity_create(void)
{
    long index = datum_new(tls_simulation_gamestate_entity_data());
    if (index != -1) {
        s_simulation_gamestate_entity_datum *datum = simulation_gamestate_entity_datum_get(index);
        datum->object_index = -1;
        datum->entity_index = -1;
        datum->deletion_queued = 0;
    }
    return index;
}

long simulation_gamestate_entity_get_object_index(long entity_index)
{
    long object_index = -1;
    s_simulation_gamestate_entity_datum *datum = 0;
    s_data_array *data = tls_simulation_gamestate_entity_data();
    if (entity_index != -1) {
        int absolute_index = entity_index & 0xffff;
        if (absolute_index < data->first_unallocated) {
            short *entry = (short *)(data->data + data->size * absolute_index);
            int identifier = *entry;
            if (identifier != 0 && identifier == (entity_index >> 16))
                datum = (s_simulation_gamestate_entity_datum *)entry;
        }
    }
    if (datum)
        object_index = datum->object_index;
    return object_index;
}

long simulation_gamestate_entity_try_and_get_object_index(long handle, unsigned long type_mask)
{
    long result = -1;
    if (handle != -1) {
        long object_index = simulation_gamestate_entity_get_object_index(handle);
        if (object_index != -1 && ((1 << object_header_get(object_index)->object_type) & type_mask))
            result = object_index;
    }
    return result;
}

// 0x3af200
template <>
void *t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &tls_set_g_gamestate_entity_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_object_member(name, "", size, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

// 0x3af2a0
template <>
void t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &tls_set_g_gamestate_entity_data_allocator>::deallocate(void *memory)
{
    tls_simulation_gamestate_entity_data_slot() = 0;
    restricted_region_free_member(1, m_member_index);
}




bool c_simulation_object_refresh_event_definition::apply_game_event(long reference_count, const long *references,
    long payload_size, const void *payload)
{
    long handle = references[0];
    bool result = false;
    if (handle != -1) {
        s_data_array *data = tls_simulation_gamestate_entity_data();
        unsigned char exists = (datum_try_and_get(data, handle) != 0) && 1;
        if (exists) {
            s_simulation_gamestate_entity_datum *datum =
                datum_get_typed<s_simulation_gamestate_entity_datum>(data, handle);
            long latch_index = datum->entity_index;
            if (latch_index != -1)
                simulation_entity_latch_update(latch_index, handle, 54);
            result = true;
        }
    }
    return result;
}

bool c_simulation_map_variant_group_entity_definition::gameworld_attachment_valid(long gamestate_index)
{
    bool valid = false;
    long group_index = simulation_gamestate_entity_get_object_index(gamestate_index);
    if (group_index != -1 && breakable_surface_group_get_gamestate_index(group_index) == gamestate_index)
        valid = true;
    return valid;
}

bool c_simulation_map_variant_group_entity_definition::delete_game_entity(long gamestate_index)
{
    bool success = false;
    if (gameworld_attachment_valid(gamestate_index))
    {
        long group_index = simulation_gamestate_entity_get_object_index(gamestate_index);
        breakable_surface_group_set_gamestate_index(group_index, -1);
        simulation_gamestate_entity_datum_get(gamestate_index)->object_index = -1;
        success = true;
    }
    return success;
}

bool c_simulation_map_variant_group_entity_definition::build_updated_state_data(const s_simulation_entity *entity,
    unsigned long *update_mask, long size, void *state)
{
    long object_index = simulation_gamestate_entity_get_object_index(entity->gamestate_index);
    return map_variant_encode_simulation_chunk(&tls_game_engine_globals()->map_info, object_index, update_mask, size,
        static_cast<s_map_variant_object_simulation_state *>(state));
}

bool c_simulation_map_variant_group_entity_definition::update_game_entity(long gamestate_index, unsigned long update_mask,
    long state_size, const void *state_data)
{
    bool result = false;
    long object_index = simulation_gamestate_entity_get_object_index(gamestate_index);
    if (object_index != NONE && gameworld_attachment_valid(gamestate_index))
    {
        // match: 0x42da08: taking the map address directly gives this r30 and shifts the three saved decode arguments down one register.
        c_map_variant *const &map_variant = &tls_game_engine_globals()->map_info;
        result = map_variant_decode_simulation_chunk(map_variant, object_index, update_mask, state_size,
            static_cast<const s_map_variant_object_simulation_decode_state *>(state_data));
    }
    return result;
}

typedef t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &tls_set_g_gamestate_entity_data_allocator> simulation_gamestate_entity_data_allocation_manager;
simulation_gamestate_entity_data_allocation_manager g_simulation_gamestate_entity_data_allocator;

