// blamlibxbox/visibility_portal_activation.obj: from the records 0x1be878 0x1be888 0x1be8d0 0x1be938 0x1be940 0x1be9d8 0x1beba8 0x1bec48 0x1bec60 0x200d78 0x2026c0
// @flags /TP /O2
// @address 0x11df8b8 g_active_portal_bitvectors
extern "C" { __declspec(thread) unsigned long *g_active_portal_bitvectors; }
#include <stddef.h>
#include <string.h>
#include <ppcintrinsics.h>
#include "visibility/visibility_portal_activation_types.h"
#include "camera/saved_film_director_storage.h"
#include "objects/object_iterator.h"
#include "memory/tls_globals.h"
#include "multithreading/thread_tls.h"
#include "memory/data_types.h"
#include "objects/object_types.h"
#include "objects/object_datum.h"
#include "devices/device_datum.h"
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "objects/object_header_access.h"
#include "memory/datum_index.h"
#include "devices/device_machines.h"
#include "visibility/visibility_portal_activation.h"
#include "scenario/scenario.h"
#include "cache/restricted_memory.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario_definitions.h"
#include "game/game_globals_storage.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cache/restricted_memory_registration.h"
#include "cache/restricted_memory_layout.h"
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x200d78 ?__tls_set_g_director_camera_scripted_flag_allocator@@YAXPAX@Z

// Machine-only portal state follows the shared device prefix. 0x1be940
// and 0x1be9d8 access the three shorts at +0x232, +0x234, +0x236.
struct visibility_portal_machine_datum : device_datum
{
    unsigned long machine_flags;
    long door_open_ticks;
    short pathfinding_policy;
    short portal_index;
    unsigned short portal_flags;
    short portal_structure_bsp_index;
};
typedef char visibility_portal_machine_index_offset[(sizeof(void *) != 4 || (offsetof(visibility_portal_machine_datum, portal_index) == 0x232)) ? 1 : -1];
typedef char visibility_portal_machine_bsp_offset[(sizeof(void *) != 4 || (offsetof(visibility_portal_machine_datum,
    portal_structure_bsp_index) == 0x236)) ? 1 : -1];

void __tls_set_g_director_camera_scripted_flag_allocator(void *allocator)
{
    g_director_camera_scripted = (bool *)allocator;
}

// Registers a member of field layout 1 (see c_restricted_memory::add_member) and publishes its
// per-thread address. One of a family of identical allocators that differ
// only in the layout index, the update callback and the thread-local pointer.

// @address 0x1be878 ?__tls_set_g_active_portal_bitvectors_allocator@@YAXPAX@Z
void __tls_set_g_active_portal_bitvectors_allocator(void *value)
{
    g_active_portal_bitvectors = (unsigned long *)value;
}

// @address 0x1beba8 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_active_portal_bitvectors_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_active_portal_bitvectors_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  (void)alignment_bits;
  int member_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, 0, __tls_set_g_active_portal_bitvectors_allocator, 0, 0);
  unsigned int offset = restricted_region_get_member_offset(3, member_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner = get_current_thread_index();
  m_thread_id = owner;
  m_member_index = member_index;
  g_active_portal_bitvectors = (unsigned long *)address;
  return restricted_region_get_member_address(3, member_index);
}

// @address 0x1bec48 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_active_portal_bitvectors_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_active_portal_bitvectors_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x1bec60 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_active_portal_bitvectors_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_active_portal_bitvectors_allocator>::deallocate(void *memory)
{
    (void)memory;
    int value = m_member_index;
    g_active_portal_bitvectors = 0;
    restricted_region_free_member(3, value);
}

// @address 0x2026c0 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_director_camera_scripted_flag_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_director_camera_scripted_flag_allocator>::deallocate(void *memory)
{
    (void)memory;
    int value = m_member_index;
    g_director_camera_scripted = 0;
    restricted_region_free_member(3, value);
}

// @address 0x1be888 ?portal_activation_initialize@@YAXXZ
void portal_activation_initialize(void)
{
    g_active_portal_bitvectors = (unsigned long *)(&g_active_portal_bitvectors_allocator)
                       ->reserve_memory("visibility active portals", 0, 0x400, 0);
}

// @address 0x5bec98 ?portal_activation_dispose@@YAXXZ
void portal_activation_dispose(void)
{
}

// @address 0x5bec98 ?portal_activation_initialize_for_new_map@@YAXXZ
void portal_activation_initialize_for_new_map(void)
{
}

// @address 0x5bec98 ?portal_activation_dispose_from_old_map@@YAXXZ
void portal_activation_dispose_from_old_map(void)
{
}

// @address 0x1be940 ?portal_activation_clear_portals@@YAXXZ

// @address 0x1be938 ?portal_activation_dispose_from_old_structure_bsp@@YAXK@Z
void portal_activation_dispose_from_old_structure_bsp(unsigned long structure_bsp_mask)
{
    (void)structure_bsp_mask;
    portal_activation_clear_portals();
}

void portal_activation_clear_portals(void)
{
    c_object_iterator<visibility_portal_machine_datum> iterator;
    iterator.begin(128, 0);
    while (iterator.next())
    {
        visibility_portal_machine_datum *device = (visibility_portal_machine_datum *)object_get_and_verify_type(iterator.get_index(), 128);
        device->portal_index = -1;
        device->portal_flags = 0;
        device->portal_structure_bsp_index = -1;
    }
}

// @address 0x1be9d8 ?portal_activation_door_portal_search_for_doors@@YAXXZ
void portal_activation_door_portal_search_for_doors(void)
{
    int zone_set_index = global_zone_set_index;
    s_scenario_zone_set_visibility *visibility =
        &((s_scenario_zone_set_visibility *)global_scenario->zone_set_pvs.address)
        [((s_scenario_zone_set *)global_scenario->zone_sets.address)[zone_set_index].pvs_index];
    portal_activation_clear_portals();
    int active_bsp_mask = (int)g_active_structure_bsp_mask;
    int bsp_index = 31 - _CountLeadingZeros(-active_bsp_mask & active_bsp_mask);
    if (bsp_index != -1) {
        bool door_is_active;
        c_object_iterator<visibility_portal_machine_datum> iterator;
        do {
            int door_index = 0;
            s_scenario_bsp_machine_door_mapping *mapping =
                &((s_scenario_bsp_machine_door_mapping *)visibility->machine_door_mappings.address)[bsp_index];
            if (mapping->machine_door_objects.count > 0) {
                do {
                    s_scenario_machine_door_reference *door_references =
                        (s_scenario_machine_door_reference *)mapping->machine_door_objects.address;
                    s_scenario_machine_door_reference *door_reference = &door_references[door_index];
                    iterator.begin(128, 0);
                    if ((unsigned char)iterator.next() != 0) {
                        s_object_header *headers = (s_object_header *)tls_object_header_data()->data;
                        do {
                            int object_index = iterator.get_index();
                            visibility_portal_machine_datum *machine =
                                (visibility_portal_machine_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
                            if ((unsigned char)door_reference->identifier.is_equal(&machine->object_identifier) != 0)
                            {
                                machine->portal_index = door_reference->portal_index;
                                int portal_flags = door_reference->flags;
                                game_globals_storage *game_globals = tls_game_globals();
                                machine->portal_structure_bsp_index = (unsigned short)bsp_index;
                                machine->portal_flags = (unsigned short)portal_flags;
                                if ((unsigned char)machine_compute_activation(
                                        object_index, (const s_game_cluster_bit_vectors *)game_globals->cluster_activation, &door_is_active) != 0 &&
                                    door_is_active)
                                {
                                    object_activate(object_index);
                                }
                                break;
                            }
                        } while ((unsigned char)iterator.next() != 0);
                    }
                    door_index = door_index + 1;
                } while (door_index < mapping->machine_door_objects.count);
                active_bsp_mask = (int)g_active_structure_bsp_mask;
            }
            int remaining_mask = active_bsp_mask & ~((int)((__int64)1 << (bsp_index + 1)) - 1);
            bsp_index = 31 - _CountLeadingZeros(-remaining_mask & remaining_mask);
        } while (bsp_index != -1);
    }
}

// @address 0x1be8d0 portal_activation_initialize_for_new_structure_bsp
extern "C" void portal_activation_initialize_for_new_structure_bsp(unsigned int structure_bsp_mask)
{
    unsigned int bit_index;
    int byte_offset;

    bit_index = 0;
    byte_offset = 0;
    do {
        if (((1 << bit_index) & structure_bsp_mask) != 0) {
            memset((char *)g_active_portal_bitvectors + byte_offset, -1, 16 * sizeof(unsigned long));
        }
        byte_offset = byte_offset + 0x40;
        bit_index = bit_index + 1;
    } while (byte_offset < 0x400);
    portal_activation_door_portal_search_for_doors();
}


active_portal_bitvectors_allocation_manager g_active_portal_bitvectors_allocator;
