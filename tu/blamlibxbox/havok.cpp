// blamlibxbox/havok.obj: from the records 0xcc528 0xd1320 0xdf668 0xdf6e0 0xe2c88 0xe34f0 0x1032d8 0x103460 0x106e50 0x1073c8 0x112470 0x112d58 0x1ba8d8 0x1baa60 0x1bab00 0x1bab90 0x1babd8 0x1babe0 0x1baef0 0x1baef8 0x1bb160 0x1bb300 0x1bb678 0x1bb698 0x1bb730 0x1bb850 0x1bb950 0x1bb9c8 0x1bba50 0x1bbb58 0x1bbc08 0x1bbc80 0x1bbd00 0x1bbda0 0x1bbfc8 0x1bc038 0x1bc120 0x1bc558 0x1bc690 0x1bc888 0x1bcec0 0x1bcef0 0x1bcf20 0x1bd010 0x1bd218 0x1bd590 0x1bd5e0 0x1bdc30 0x1bdff0 0x1be128 0x1be1e0 0x1be2b8 0x1be390 0x1be450 0x1be640 0x1be6a8 0x1be7e8 0x232d00 0x247fc8 0x24a510 0x24a5b0 0x260548 0x2605b0 0x260748 0x260b68 0x6b3f30 0x707950
// @address 0x11df870 g_havok_memory_task_data
#include "physics/havok_dependencies.h"
#include "memory/runtime_state_allocation.h"
// @address 0x250570 havok_component_scalar_delete
// @address 0x11df73c g_impacts_globals

#include "physics/physics_model_primitive.h"
#include "physics/havok_memory_allocator_lock.h"
// @address 0x11df824 g_havok_memory_allocator_locked
// @flags /TP /O2
#include "cache/restricted_memory.h"
#include "memory/data_types.h"
#include "game/game_time_types.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "tag_files/tag_types.h"
#include "phys/phys_base_types.h"
#include "game/game_options.h"
#include "game/game_globals_storage.h"
#include "objects/object_datum.h"
#include "objects/object_iterator.h"
#include "units/motor_enums.h"
#include "units/motor_request.h"
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "physics/havok_memory.h"
#include "physics/havok_proxies.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "physics/havok.h"
#include "physics/havok_runtime_storage.h"
#include "physics/havok_collision_filter.h"
#include "physics/havok_component.h"
#include "physics/havok_halo_external.h"
#include "physics/havok_utilities.h"
#include "physics/impacts.h"
#include "memory/data.h"
#include "motor/motor_system.h"
#include "phys/phys_semaphore.h"
#include "tag_files/tag_index.h"
#include "cache/restricted_memory_layout.h"
#include "phys/phys_memory.h"
#include "phys/phys_volume_overlap_listener.h"
#include "physics/havok_runtime_pool_cache_entry_types.h"
#include "physics/havok_runtime_pool_cache_data_types.h"
#include "physics/memory_allocator_types.h"
#include "phys/phys_agent_register_util.h"
#include "cache/cache_files.h"
#include "physics/havok_shapes.h"
#include "physics/havok_environment_mopp_bv_tree_shape.h"
#include "scenario/scenario.h"
#include "phys/phys_world_deletion_listener.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_simulation.h"
#include "phys/phys_island.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_bvh_agent.h"
#include "phys/phys_world_op_util.h"
#include "phys/phys_world_op_queue_types.h"
#include "physics/havok_monitor_storage.h"
// match: 0x1ba8d8: retain the original monitor reset alignment.
struct s_havok_monitor_stream { __int64 first; __int64 second; int third; };
#include "physics/havok_constants_types.h"
#include "physics/havok_memory_globals.h"
#include "physics/havok_proxy_types.h"
#include "physics/havok_contact_point.h"
#include "physics/havok_component_class.h"
#include "memory/tls_globals.h"
#include "scenario/scenario_access.h"
#include "structures/structure_bsp_tag.h"
#include "physics/havok_world_shape_penetration_collector.h"
#include "phys/phys_critical_section.h"
#include "objects/scenery.h"
#include "items/projectile_definitions.h"
#include "phys/phys_error.h"
#include "phys/phys_system.h"
#include "physics/havok_runtime_pool_types.h"
#include "physics/havok_entity_listener.h"
#include "physics/havok_entity_iterator.h"
#include "physics/havok_memory_usage.h"
#include "physics/havok_group_types.h"
#include "game/game_time_inline.h"
#include "game/game_time_access.h"
#include <new>
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "physics/havok_world_constraint.h"
#include "game/game_globals.h"
#include "physics/havok_memory_pool.h"
#include "phys/phys_axis_sweep.h"
#include "physics/havok_contact_point_iterator.h"
#include "objects/object_header_access.h"
#include "tag_files/tag_groups.h"

class phys_world_object;
class phys_island;
class phys_ref_object;
enum phys_memory_class;
class phys_volume;

// @address 0x260748 ??_Gc_broad_phase_border@@UAAPAXI@Z
// @address 0x260b68 ??_Ec_broad_phase_border@@W7AAPAXI@Z
// @address 0x25e758 ?havok_memory_garbage_collect@@YAXXZ
// @address 0x25e8d8 ?havok_memory_garbage_collect_reset_allocated_state@@YAXXZ
// @address 0x82370 _vsnprintf
// @address 0x60a380 ?free_chunk@phys_thread_memory@@QAAXPAXHW4phys_memory_class@@@Z
// @address 0x60c688 ?get_current_thread_id@phys_thread@@SA_KXZ
// @address 0x1be390 ?havok_notify_static_geometry_modfied@@YAXJJJJJ@Z
// @address 0x1be6a8 ?replace_singleton_instance@?$phys_singleton@Vphys_error@@@@SAXPAVphys_error@@@Z
// @address 0x60c530 ?acquire_semaphore@phys_semaphore@@QAAXXZ
// @address 0x60c540 ?release@phys_semaphore@@QAAXH@Z
// @address 0x63d538 ?remove_entity@phys_world@@QAA?AVphys_bool@@PAVphys_entity@@@Z
// @address 0x640368 ?refilter_entity@phys_world@@QAAXPAVphys_entity@@W4phys_entity_filter_update_mode@@W4phys_compound_filter_update_mode@@@Z
// @address 0x1d6da8 ?restricted_region_add_alias@@YAXJ@Z
// @address 0x1d6e40 ?restricted_region_remove_alias@@YAXJ@Z
// @address 0x61ae98 ?set_use_aabb_caching@phys_bvh_agent@@SAXVphys_bool@@@Z
// @address 0x61ed28 ?register_alt_shape_type@phys_dispatcher@@QAAXW4phys_shape_type@@0@Z
// @address 0x61e9e0 ?disable_debugging@phys_dispatcher@@QAAXXZ
// @address 0x61f0f8 ?register_all_agents@phys_agent_register_util@@SAXPAVphys_dispatcher@@@Z
// @address 0x6453c8 ?mark_island_inactive@phys_world_op_util@@SAXPAVphys_world@@PAVphys_island@@@Z
// @address 0x106e50 ?havok_world_remove_entity@@YAXPAVphys_entity@@@Z
// @address 0x1bcef0 ?havok_notify_thread_frame_finished@@YAXJ@Z
// @address 0x1be2b8 ?add_proxy_pair@c_world_shape_penetration_collector@@UAAXABVphys_collision_proxy@@0@Z
// @address 0x3c1f4 ??_7c_world_shape_penetration_collector@@6B@
// @address 0x1ba760 ??_Ec_world_shape_penetration_collector@@UAAPAXI@Z
// @address 0x232d00 ?__tls_set_g_havok_game_state_allocator@@YAXPAX@Z
// @address 0x2605b0 ?enter@phys_critical_section@@QAAXXZ
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z
// @address 0x1ec288 ?data_dispose@@YAXPAUs_data_array@@@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x25e978 ?havok_compute_memory_in_use@@YAXPAUs_havok_memory_usage@@@Z
// @address 0x7246fc ?phys_convex_shape_default_radius@@3MA
// @address 0x247ff0 ?impacts_disconnect_from_physics@@YAXXZ
// @address 0x106d08 ?next@c_havok_entity_iterator@@QAA_NXZ
// @address 0xf1820 ?object_is_or_contains_player@@YA_NJ@Z
// @address 0x60ad20 ?quit_system@phys_system@@YA?AW4phys_result@@XZ
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0xf4228 ?simulation_query_object_is_predicted@@YA_NJ@Z
#include "cseries/crt_xdk.h"
#include <ppcintrinsics.h>
#include <vectorintrinsics.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

struct s_data_iterator;
struct s_data_array;
#include "phys/phys_array_util.h"
#include "phys/phys_base.h"
#include "phys/phys_solver_check.h"

__declspec(thread) long *g_havok_memory_task_data;

// @address 0x396d0 ?PHYS_KEYCODE@@3QBDB
// @rdata 0x820396d0..0x820396ec
const char PHYS_KEYCODE[] = "CLIENT.Ph.Bungie_Next";
// @address 0x396e8 ?PHYS_KEYVALUE@@3IB
const unsigned int PHYS_KEYVALUE = 0x56065dd1;
// @rdata-end

// @address 0x75dd10 ?g_havok_constants@@3Us_havok_constants@@A
s_havok_constants g_havok_constants = {
    0.0328f, 0.00328f, 1, 2, 0.0f, false, 0.02f,
    false, // jumping_beans
    true,  // weld_environment
    true,  // cache_environment_shapes
    false, // shape_cache_debug
    true,  // enable_back_stepping
    true,  // enable_sweep_shapes
    false, // display_stats
    false, // initialize_profiling
    false, // render_impacts
    false, // render_proxies
    false, // render_collision_damage
    true,  // enable_impact_effects
    false, // play_biped_impact_effects
    false, // render_shape_properties
    true,  // cleanup_inactive_agents
    true,  // batch_add_entities_disabled
    60.0f, 0.01f
};

// @address 0x8a1f04 ?g_havok_world@@3PAVphys_world@@A
phys_world *g_havok_world;

class phys_entity;
class phys_rigid_body;
class phys_island;
class phys_constraint;

class phys_volume;

namespace phys_world_op
{

}
class phys_world_op_queue;
class phys_world_desc;
class phys_broadphase_border;
class phys_collision_filter;
class phys_dispatcher;

struct s_havok_simulation_island_cleanup_globals
{
    bool cleanup_required;
};

// @address 0xf02cf0 ?g_havok_simulation_island_cleanup_globals@@3Us_havok_simulation_island_cleanup_globals@@A
s_havok_simulation_island_cleanup_globals g_havok_simulation_island_cleanup_globals;

// @address 0x74b910 g_havok_monitor_globals
s_havok_globals g_havok_monitor_globals = { { NULL }, -1, 0, -1, 0, false, true };

inline bool havok_inside_simulation_step()
{
    return g_havok_monitor_globals.inside_simulation_step;
}
inline bool havok_garbage_collection_locked()
{
    return g_havok_monitor_globals.garbage_collection_locked;
}
inline void havok_garbage_collection_lock(bool lock)
{
    g_havok_monitor_globals.garbage_collection_locked = lock;
}
inline void havok_shapes_lock_environment_null_shapes()
{
    g_havok_shapes_environment_null_shapes_locked = true;
}
inline void havok_shapes_unlock_environment_null_shapes()
{
    g_havok_shapes_environment_null_shapes_locked = false;
}
inline void havok_simulation_island_cleanup_resources()
{
    if (g_havok_simulation_island_cleanup_globals.cleanup_required && !havok_garbage_collection_locked())
    {
        bool cleaned_up_island = false;
        havok_garbage_collection_lock(true);
        for (long island_type = 0; island_type < 2; island_type++)
        {
            const phys_array<phys_island *> &islands = (island_type == 0) ?
                g_havok_world->get_active_islands() : g_havok_world->get_inactive_islands();
            for (long island_index = 0; island_index < islands.getSize(); island_index++)
            {
            }
        }
        havok_garbage_collection_lock(false);
        g_havok_simulation_island_cleanup_globals.cleanup_required = false;
    }
}

extern "C" {
// @address 0x7ce570 ?s_list_head@phys_critical_section_base@@2PAV1@A
// @address 0x7ce578 ?s_list_semaphore@phys_critical_section_base@@2Vphys_semaphore@@A
}
struct s_data_array;

namespace {
static __forceinline void havok_clear_shape_inheritance(phys_dispatcher *dispatcher)
{
    phys_array<phys_dispatcher::shape_inheritance> &inheritance = dispatcher->m_shape_inheritance;
    if ((inheritance.m_capacity_and_flags & (int)0x80000000) == 0) {
        int capacity = inheritance.m_capacity_and_flags & 0x3fffffff;
        phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
        memory->free_chunk(inheritance.m_data, capacity * sizeof(phys_dispatcher::shape_inheritance), PHYS_MEM_ARRAY);
    }
    inheritance.m_data = 0;
    inheritance.m_size = 0;
    inheritance.m_capacity_and_flags = (inheritance.m_capacity_and_flags & 0x40000000) | (int)0x80000000;
}
}

class phys_collision_listener;

namespace {
static __forceinline long havok_initial_component_index(const phys_entity *entity)
{
    return (long)entity->get_world_object_property(0x2001).m_data;
}
static __forceinline bool havok_initial_object_inactive(long component_index)
{
    c_havok_component *component = &(
        (c_havok_component *)((s_data_array *)g_havok_component_data)->data)[(unsigned short)component_index];
    object_datum *object = object_get(component->m_object_index);
    return (_rotl(object->physics_flags, 23) & 1) != 0;
}
}

// @address 0x7ce574 g_havok_thread_monitor_tls_index
// @address 0x11df7a0 g_havok_proxy_data

// @address 0xcc528 ??1phys_critical_section_base@@IAA@XZ
phys_critical_section_base::~phys_critical_section_base()
{
    phys_critical_section_base::s_list_semaphore.acquire_semaphore();
    if (m_prev_link != 0)
        m_prev_link->m_next = m_next;
    else
        phys_critical_section_base::s_list_head = m_next;
    if (m_next != 0)
        m_next->m_prev_link = m_prev_link;
    phys_critical_section_base::s_list_semaphore.release(1);
}

void havok_world_remove_entity(phys_entity *entity)
{
    phys_bool result;

    if (!havok_inside_simulation_step())
    {
        g_havok_monitor_globals.entity_count--;
        havok_shapes_lock_environment_null_shapes();
        result = g_havok_world->remove_entity(entity);
        havok_shapes_unlock_environment_null_shapes();
        havok_simulation_island_cleanup_resources();
    }
}

// @address 0xdf668 havok_component_has_active_entity
unsigned char havok_component_has_active_entity(c_havok_component *component)
{
    bool result = false;
    for (long rigid_body_index = 0; rigid_body_index < component->m_rigid_bodies.m_size; rigid_body_index++) {
        if (component->m_rigid_bodies.m_data[rigid_body_index].m_rigid_body->is_body_active()) {
            result = true;
            break;
        }
    }
    return result;
}

// 0x112470 (no dev name; havok_object_needs_physics until one is found)
// @address 0x112470 havok_object_needs_physics
extern "C" bool havok_object_needs_physics(long object_index, long definition_index)
{
    s_object_definition *definition = TAG_GET('obje', s_object_definition, definition_index);
    long mask = 1 << definition->object_type;
    bool result = false;
    if (mask & (1 << 6)) {
        result = !((((s_scenery_definition *)definition)->scenery_flags & 1) && object_index != -1 &&
                   !(_rotl((object_get(object_index))->physics_flags, 12) & 1));
    } else if (mask & 0x1cdf) {
        result = true;
    } else if (mask & (1 << 5)) {
        result = (((projectile_definition *)definition)->targeting_flags >> 10) & 1;
    }
    return result;
}

// @address 0x1babd8 havok_handle_new_structure_bsp
extern "C" void havok_handle_new_structure_bsp(unsigned long structure_bsp_mask)
{
    havok_initialize_for_new_structure_bsp(structure_bsp_mask);
}

// @address 0x1baef0 havok_handle_old_structure_bsp
extern "C" void havok_handle_old_structure_bsp(unsigned long structure_bsp_mask)
{
    havok_dispose_from_old_structure_bsp(structure_bsp_mask);
}

// @address 0x1bb678 ?havok_save_to_game_state@@YAXJ@Z
void havok_save_to_game_state(long flags)
{
    if (flags & 0x80)
        return;
    havok_dispose_from_old_structure_bsp(g_active_structure_bsp_mask);
}

// @address 0x1bb950 ?havok_object_new_recursive@@YAXJ@Z
void havok_object_new_recursive(long object_index)
{
    havok_memory_garbage_collect_locked();
    havok_object_new_recursive(object_index, object_index);
    havok_simulation_island_cleanup_resources();
    havok_memory_garbage_collect_locked();
}

// @address 0x1bb9c8 ?havok_object_new_recursive@@YAXJJ@Z
// @address 0x250618 ?havok_component_new@@YAJJ@Z
// @address 0xe2ec0 havok_component_activate_rigid_bodies
void havok_object_new_recursive(long root_object_index, long object_index)
{
    object_datum *object = object_get(object_index);
    long child_index = object->first_child_object_index;

    if (object->havok_component_index == -1)
        havok_object_new(object_index);
    while (child_index != -1) {
        object_datum *child = object_get(child_index);
        if (child_index == root_object_index)
            break;
        havok_object_new_recursive(root_object_index, child_index);
        child_index = child->next_object_index;
    }
}

// @address 0x1bbc08 ?havok_object_delete_internal@@YAXJ@Z
void havok_object_delete_internal(long object_index)
{
    object_datum *object = object_get(object_index);
    long owner_index = object->havok_component_index;

    if (owner_index != -1) {
        havok_component_scalar_delete(&((c_havok_component *)((s_data_array *)g_havok_component_data)->data)[(unsigned short)owner_index], 0);
        datum_delete(g_havok_component_data, owner_index);
        object->havok_component_index = -1;
    }
}

// @address 0x1bbb58 ?havok_object_delete@@YAXJ@Z
void havok_object_delete(long object_index)
{
    bool physics = (object_get(object_index)->physics_flags & (1UL << 7)) != 0;

    if (physics)
        havok_memory_garbage_collect_locked();
    havok_object_delete_internal(object_index);
    if (physics) {
        havok_simulation_island_cleanup_resources();
        havok_memory_garbage_collect_locked();
    }
}

// @address 0x1bbc80 ?havok_object_delete_recursive@@YAXJJ@Z
void havok_object_delete_recursive(long stop_object_index, long object_index)
{
    long child_index = object_get(object_index)->first_child_object_index;

    havok_object_delete_internal(object_index);
    while (child_index != -1) {
        object_datum *child = object_get(child_index);
        if (child_index == stop_object_index)
            break;
        havok_object_delete_recursive(stop_object_index, child_index);
        child_index = child->next_object_index;
    }
}

// @address 0x1bbd00 ?havok_component_aquire@@YAXJ_N0@Z
void havok_component_aquire(long object_index, bool recursive, bool force)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *object = header->datum;

    if (!force && !(header->flags & 1))
        return;
    if (havok_object_needs_physics(object_index, object->definition_index))
    {
        if (recursive)
        {
            havok_object_new_recursive(object_index);
            return;
        }
        havok_object_new(object_index);
    }
}

// @address 0x1bbda0 havok_component_release
extern "C" void havok_component_release(long object_index, bool recursive)
{
    object_datum *object = object_get(object_index);

    if (havok_object_needs_physics(-1, object->definition_index))
    {
        if (recursive)
        {
            bool physics = ((object->physics_flags >> 7) & 1) != 0;

            if (physics)
                havok_memory_garbage_collect_locked();
            havok_object_delete_recursive(object_index, object_index);
            if (physics)
            {
                havok_simulation_island_cleanup_resources();
                havok_memory_garbage_collect_locked();
            }
        }
        else
        {
            havok_object_delete(object_index);
        }
    }
}

// @address 0x1bbfc8 ?havok_can_allocate_space_for_instance_of_object_definition@@YA_NJ@Z
bool havok_can_allocate_space_for_instance_of_object_definition(long object_definition_index)
{
    bool result = true;

    if (havok_object_needs_physics(-1, object_definition_index))
        result = ((s_data_array *)g_havok_component_data)->actual_count < ((s_data_array *)g_havok_component_data)->maximum_count;
    return result;
}

// @address 0x1bc558 havok_initialize_collision_dispatcher
extern "C" void havok_initialize_collision_dispatcher(phys_dispatcher *dispatcher)
{
    if (!g_havok_monitor_globals.dispatcher_initialized) {
        dispatcher->register_alt_shape_type(PHYS_SHAPE_USER0, PHYS_SHAPE_CONVEX);
        phys_agent_register_util::register_all_agents(dispatcher);
        havok_clear_shape_inheritance(dispatcher);
        dispatcher->disable_debugging();
        memcpy(g_havok_monitor_globals.cached_dispatcher, dispatcher, sizeof(*dispatcher));
        g_havok_monitor_globals.dispatcher_initialized = true;
    } else {
        havok_clear_shape_inheritance(dispatcher);
        dispatcher->disable_debugging();
        memcpy(dispatcher, g_havok_monitor_globals.cached_dispatcher, sizeof(*dispatcher));
    }
}

// @address 0x1bc690 ?havok_build_environment_surface_mopp@@YAXJ@Z
void havok_build_environment_surface_mopp(long structure_bsp_index)
{
    c_environment_surface_shape *surface_shape = new c_environment_surface_shape(structure_bsp_index, -1);
    c_environment_mopp_bv_tree_shape *mopp_shape = new c_environment_mopp_bv_tree_shape(surface_shape,
        (const phys_bvh_bytecode *)global_structure_bsp_get(surface_shape->m_structure_bsp_index)->structure_physics.mopp_code_block.address);
    mopp_shape->set_user_data(k_environement_body_user_data);
    phys_rigid_body_desc rigid_body_info;
    rigid_body_info.m_proxy_shape = mopp_shape;
    rigid_body_info.m_motion_type = phys_motion::MOTION_FIXED;
    rigid_body_info.m_collision_filter_info = _havok_group_environment;
    phys_rigid_body *rigid_body = new phys_rigid_body(rigid_body_info);
    havok_entity_set_structure_bsp_index(rigid_body, structure_bsp_index);
    g_havok_monitor_globals.rigid_bodies[structure_bsp_index] = rigid_body;
    havok_entity_set_havok_component_index(rigid_body, -1);
    mopp_shape->object_remove_ref();
    surface_shape->object_remove_ref();
}

// @address 0x1bcec0 ?havok_notify_thread_frame_started@@YAXJ@Z
void havok_notify_thread_frame_started(long thread_index)
{
    restricted_region_add_alias(1);
    restricted_region_add_alias(3);
}

// @address 0x1bdff0 ?havok_set_initial_simulation_island_activity@@YAXXZ
void havok_set_initial_simulation_island_activity()
{
    for (long island_index = 0; island_index < g_havok_world->get_active_islands().getSize(); island_index++) {
        phys_island *island = g_havok_world->get_active_islands().m_data[island_index];
        const phys_array<phys_entity *> &entities = island->get_entities();
        long inactive_entity_count = 0;
        for (long entity_index = 0; entity_index < entities.getSize(); entity_index++) {
            long component_index = havok_initial_component_index(entities.m_data[entity_index]);
            if (component_index != 0 && component_index != -1 && havok_initial_object_inactive(component_index))
                inactive_entity_count++;
        }
        if (inactive_entity_count == entities.getSize())
            phys_world_op_util::mark_island_inactive(g_havok_world, island);
    }
}

void c_world_shape_penetration_collector::add_proxy_pair(const phys_collision_proxy &body_a, const phys_collision_proxy &body_b)
{
    const phys_collision_proxy *node = &body_b;
    while (node->m_parent != 0) {
        node = node->m_parent;
    }
    if (((const phys_collider *)node)->get_type_id() == 1) {
        const phys_world_object *owner = (const phys_world_object *)((const phys_collider *)node)->get_collider_owner();
        if (owner != 0) {
            int body_component = havok_entity_get_havok_component_index(owner);
            const phys_shape *shape = body_b.m_proxy_shape;
            bool invalid = false;
            if (shape->get_type_id() != 30) {
                phys_ulong property = shape->m_user_data;
                if (property != 0 && (property < 1 || property > 16)) {
                    invalid = ((const s_physics_model_primitive *)property)->phantom_index != -1;
                }
            }
            if (!invalid && (component_index == -1 || component_index != body_component)) {
                m_early_out = 1;
                found_collision = 1;
            }
        }
    }
}

class phys_thread_memory;

namespace {
static __forceinline bool havok_static_geometry_present()
{
    bool present = false;
    phys_entity **bodies = (phys_entity **)g_havok_monitor_globals.rigid_bodies;
    for (long body_index = 0; body_index < 16; body_index++) {
        if (bodies[body_index] != 0) {
            present = true;
            break;
        }
    }
    return present;
}
}
void havok_notify_static_geometry_modfied(long object_index, long region_index, long old_permutation,
    long new_permutation, long flags)
{
    phys_bvh_agent::set_use_aabb_caching(false);
    if (g_havok_world != 0 && havok_static_geometry_present()) {
        havok_shapes_lock_environment_null_shapes();
        for (long body_index = 0; body_index < 16; body_index++) {
            if (((phys_entity **)g_havok_monitor_globals.rigid_bodies)[body_index] != 0) {
                g_havok_world->refilter_entity(((phys_entity **)g_havok_monitor_globals.rigid_bodies)[body_index],
                    PHYS_UPDATE_FILTER_ON_ENTITY_DISABLE_ENTITY_ENTITY_COLLISIONS_ONLY, PHYS_UPDATE_FILTER_PROCESS_COMPOUND_SHAPES);
            }
        }
        havok_shapes_unlock_environment_null_shapes();
    }
}
// @address 0x1be640 ?havok_initialize_all_allocated_state@@YAX_N@Z
void havok_initialize_all_allocated_state(bool enable)
{
    if (!enable)
        g_havok_monitor_globals.allocated_state_deferred = true;
    havok_initialize_for_new_structure_bsp(g_active_structure_bsp_mask);
    if (!enable)
        g_havok_monitor_globals.allocated_state_deferred = false;
}
class phys_ref_object;

template<>
void phys_singleton<phys_error>::replace_singleton_instance(phys_error *instance)
{
    if (phys_singleton<phys_error>::s_instance != 0)
        phys_singleton<phys_error>::s_instance->object_remove_ref();
    phys_singleton<phys_error>::s_instance = instance;
}

void __tls_set_g_havok_game_state_allocator(void *value)
{
    g_effect_lightprobe_memory = static_cast<s_effect_lightprobe *>(value);
}

typedef void (*field_proc_t)(void *field);
class c_restricted_memory;

// @address 0x1be7e8 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_tasks_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tasks_data_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x74b8f8 ?g_havok_game_state_allocator@@3V?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_tasks_data_allocator@@YAXPAX@Z@@A
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tasks_data_allocator> g_havok_game_state_allocator;

// @address 0x247fc8 ?havok_rebase_impact_array_data@@YAXPAX@Z
void havok_rebase_impact_array_data(void *object)
{
    data_set_new_base_address(&TLS_IMPACT_ARRAY_DATA, (s_data_array *)object);
}

namespace {
static __forceinline void *havok_game_state_get_member_address(long region, long member_index)
{
    char *base_address = g_restricted_address[region];
    unsigned int member_offset = ((s_field_layout *)g_restricted_regions)[region].fields[member_index].offset;
    return base_address + member_offset;
}
static __forceinline long havok_game_state_add_member(long region, const char *name, const char *type,
    unsigned int size, long alignment_bits, field_proc_t set_address)
{
    long member_index = g_restricted_regions[region].add_member(name, type, size, alignment_bits, set_address, 0, 0);
    void *address = havok_game_state_get_member_address(region, member_index);
    set_address(address);
    return member_index;
}
}

// @address 0x24a510 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?havok_rebase_impact_array_data@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &havok_rebase_impact_array_data>::allocate(
    unsigned int size, const char *name)
{
    m_member_index = havok_game_state_add_member(1, name, "", size, 0,
        havok_rebase_impact_array_data);
    m_thread_id = g_registered_thread_index;
    return havok_game_state_get_member_address(1, m_member_index);
}

// @address 0x24a5b0 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?havok_rebase_impact_array_data@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &havok_rebase_impact_array_data>::deallocate(void *address)
{
    TLS_IMPACT_ARRAY_DATA = 0;
    restricted_region_free_member(1, m_member_index);
}

// @address 0x783260 ?g_impact_array_data_allocator@@3V?$t_restricted_allocation_manager@$00$0A@$0A@$1?havok_rebase_impact_array_data@@YAXPAX@Z@@A
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &havok_rebase_impact_array_data> g_impact_array_data_allocator;

// @address 0x260548 ??0phys_critical_section_base@@IAA@H@Z
phys_critical_section_base::phys_critical_section_base(int spin_count)
{
    phys_critical_section_base::s_list_semaphore.acquire_semaphore();
    m_spin_count = spin_count;
    m_prev_link = 0;
    m_next = phys_critical_section_base::s_list_head;
    if (phys_critical_section_base::s_list_head != 0)
        phys_critical_section_base::s_list_head->m_prev_link = this;
    phys_critical_section_base::s_list_head = this;
    phys_critical_section_base::s_list_semaphore.release(1);
}

// @address 0x6b3f30 ??_Gphys_world_desc@@UAAPAXI@Z

struct s_impact_data_array;
struct s_data_array;

static __forceinline void havok_dispose_impacts_dispose()
{
    data_dispose((s_data_array *)TLS_IMPACT_DATA);
    data_dispose(TLS_IMPACT_ARRAY_DATA);
    TLS_IMPACT_DATA = 0;
    TLS_IMPACT_ARRAY_DATA = 0;
}

static __forceinline void havok_dispose_components_dispose_global_allocations()
{
    data_dispose(g_havok_component_data);
    g_havok_component_data = 0;
}

static __forceinline void havok_dispose_real_math_reset_precision()
{
    if (g_shell_floating_point_exceptions_enabled)
        _controlfp(0x100001f, 0xfffff);
}

// @address 0x1baa60 ?havok_dispose@@YAXXZ
void havok_dispose()
{
    g_havok_monitor_globals.lock_count_4c++;
    havok_dispose_impacts_dispose();
    havok_memory_dispose_overflow_pool();
    havok_dispose_components_dispose_global_allocations();
    _clearfp();
    havok_dispose_real_math_reset_precision();
    g_havok_monitor_globals.lock_count_4c--;
}

class c_world_entity_listener;
extern "C" {
// @address 0xa430e0 g_havok_memory_globals

// @address 0x8c4260 g_global_entity_listener
c_entity_listener *g_global_entity_listener;

// @address 0x8c4264 g_world_entity_listener
c_world_entity_listener *g_world_entity_listener;

// @address 0xee94e0 g_havok_runtime_pool_cache
s_havok_runtime_pool_cache g_havok_runtime_pool_cache;

}
__forceinline bool c_entity_listener::is_deactivated_entity_queue_empty()
{
    return deactivated_entity_index + 1 >= deactivated_entities.getSize();
}
__forceinline c_entity_listener *c_entity_listener::get_global_entity_listener()
{
    return g_global_entity_listener;
}

namespace {
static __forceinline void havok_dispose_clear_object_owners()
{
    c_object_iterator<object_datum> object_iterator;
    object_iterator.begin(0, 0);
    while (object_iterator.next())
        object_iterator.get_datum()->havok_component_index = -1;
}
static __forceinline void havok_dispose_proxies_dispose_rigid_bodies_fast()
{
    g_havok_proxy_rigid_bodies_active = false;
    c_data_iterator<s_havok_proxy_datum> iterator;
    iterator.begin((s_data_array *)g_havok_proxy_data);
    while (iterator.next())
    {
        if (iterator.get_datum()->body != 0)
            iterator.get_datum()->body = 0;
    }
}
static __forceinline void havok_dispose_base_system_quit()
{
    phys_system::quit_system();
    ((phys_thread_memory *)&g_havok_monitor_globals.base_system_object)->~phys_thread_memory();
}
static __forceinline void havok_dispose_components_dispose_from_old_map_fast()
{
    data_delete_all((s_data_array *)g_havok_component_data);
    ((s_data_array *)g_havok_component_data)->valid = false;
}
static __forceinline void havok_dispose_memory_allocator_lock() { g_havok_memory_allocator_locked = true; }
static __forceinline void havok_dispose_memory_allocator_unlock() { g_havok_memory_allocator_locked = false; }
static __forceinline void havok_dispose_pool_dispose(phys_memory **pool)
{
    (*pool)->~phys_memory();
    *pool = 0;
}
static __forceinline void havok_dispose_memory_dispose_fast()
{
    if (g_havok_memory_globals.overflow_pool != 0)
        havok_memory_dispose_overflow_pool();
    havok_dispose_pool_dispose(&g_havok_memory_globals.startup_pool);
    havok_dispose_pool_dispose(&g_havok_memory_globals.runtime_pool);
    g_havok_memory_globals.active_allocator = _memory_allocator_system;
    g_havok_memory_globals.initialized = false;
}
}

// @address 0x1bab00 ?havok_initialize_for_new_map@@YAXXZ
void havok_initialize_for_new_map(void)
{
    g_havok_monitor_globals.lock_count_4c++;
    s_data_array *impacts = (s_data_array *)TLS_IMPACT_DATA;
    impacts->valid = 1;
    data_delete_all(impacts);
    s_data_array *other = TLS_IMPACT_ARRAY_DATA;
    other->valid = 1;
    data_delete_all(other);
    impacts_globals_type *g = g_impacts_globals;
    memset(g->sorted_indexes, -1, sizeof(g->sorted_indexes));
    long *p = g_havok_memory_task_data;
    g->highest_score_index = -1;
    g->orphaned_impacts_resume_index = -1;
    p[0] = -1;
    p[1] = -1;
    g_havok_monitor_globals.lock_count_4c--;
}

// @address 0x1bb698 ?havok_load_from_game_state@@YAXJ@Z
void havok_load_from_game_state(long flags)
{
    if (flags & 128)
        return;
    c_data_iterator<c_impact> iterator;
    iterator.begin((s_data_array *)TLS_IMPACT_DATA);
    while (iterator.next()) {
        c_impact *impact = iterator.get_datum();
        if (impact->type != c_impact::type3 && impact->type != c_impact::type0)
            impact->physics_reference_count = 0;
    }
    data_delete_all(TLS_IMPACT_ARRAY_DATA);
    havok_initialize_all_allocated_state((flags & 1) != 0);
}

// @address 0x1bab90 ?havok_dispose_from_old_map@@YAXXZ
void havok_dispose_from_old_map()
{
    g_havok_monitor_globals.lock_count_4c++;
    ((s_data_array *)TLS_IMPACT_DATA)->valid = 0;
    TLS_IMPACT_ARRAY_DATA->valid = 0;
    g_havok_monitor_globals.lock_count_4c--;
}

// @address 0x1baef8 ?havok_dispose_from_old_structure_bsp@@YAXK@Z
void havok_dispose_from_old_structure_bsp(unsigned long structure_bsp_index)
{
    bool garbage_collection_was_locked = havok_garbage_collection_locked();
    g_havok_monitor_globals.lock_count_4c++;
    impacts_disconnect_from_physics();
    if (!garbage_collection_was_locked)
        havok_garbage_collection_lock(true);
    havok_dispose_clear_object_owners();
    g_havok_world = 0;
    g_havok_monitor_globals.entity_count = 0;
    g_havok_monitor_globals.initialized_for_new_map = false;
    g_havok_monitor_globals.structure_bsp_state = NONE;
    g_havok_monitor_globals.monitor_stream_count = 0;
    havok_dispose_base_system_quit();
    havok_dispose_components_dispose_from_old_map_fast();
    havok_dispose_memory_allocator_lock();
    g_world_entity_listener = 0;
    g_global_entity_listener = 0;
    havok_dispose_proxies_dispose_rigid_bodies_fast();
    havok_dispose_memory_dispose_fast();
    phys_rigid_body *no_rigid_body = 0;
    for (long rigid_body_index = 0; rigid_body_index < 16; rigid_body_index++)
        g_havok_monitor_globals.rigid_bodies[rigid_body_index] = no_rigid_body;
    havok_dispose_memory_allocator_unlock();
    if (!garbage_collection_was_locked)
        havok_garbage_collection_lock(false);
    g_havok_simulation_island_cleanup_globals.cleanup_required = false;
    g_havok_runtime_pool_cache.initialized = false;
    g_havok_monitor_globals.lock_count_4c--;
}

void havok_notify_thread_frame_finished(long thread_index)
{
    restricted_region_remove_alias(3);
    restricted_region_remove_alias(1);
}

// @address 0x1bd590 ?havok_display_stats_printf@@YAX_NPBTreal_argb_color@@PBDZZ
void havok_display_stats_printf(bool force, const real_argb_color *color, const char *format, ...)
{
    char message[256];
    va_list arguments;
    va_start(arguments, format);
    _vsnprintf(message, sizeof(message) - 2, format, arguments);
}

// @address 0x8c4268 g_num_entity_iterators_in_use
struct s_havok_entity_owner;
__forceinline c_havok_entity_iterator::c_havok_entity_iterator(phys_world *world)
{
        m_iteration_type = _iterate_active;
        m_world = world;
        m_iterating = false;
    }
__forceinline c_havok_entity_iterator::c_havok_entity_iterator(phys_world *world, e_iteration_type iteration_type)
{
        m_world = world;
        m_iteration_type = iteration_type;
        m_iterating = false;
    }
__forceinline void c_havok_entity_iterator::begin()
{
        m_first_collections_done = false;
        m_collection_index = -1;
        m_entity_index = -1;
        m_iterating = true;
        g_num_entity_iterators_in_use++;
    }
__forceinline phys_entity *c_havok_entity_iterator::get_entity()
{
        if (!m_first_collections_done)
            return (*(m_world->m_active_islands.m_data + m_collection_index))->m_island_entities.m_data[m_entity_index];
        return (*(m_world->m_inactive_islands.m_data + m_collection_index))->m_island_entities.m_data[m_entity_index];
    }
__forceinline void c_havok_entity_iterator::end()
{
        m_iterating = false;
        g_num_entity_iterators_in_use--;
    }
// match: noalias declaration preserves caller live registers
static __forceinline long havok_display_stats_runtime_pool_size() { return 0x384e10; }
static __forceinline long havok_display_stats_startup_pool_size() { return 0x25680; }
static __forceinline long havok_display_stats_overflow_pool_size() { return 2672; }
static __forceinline long havok_display_stats_2nd_overflow_pool_size() { return 2672; }
static __forceinline unsigned long havok_display_stats_get_maximum_memory_allocated()
{
    return havok_display_stats_runtime_pool_size() + havok_display_stats_startup_pool_size() +
        havok_display_stats_overflow_pool_size() + havok_display_stats_2nd_overflow_pool_size();
}

class c_havok_contact_point;

class phys_shape;

extern "C" {
// @address 0x3c1f4 k_penetration_collector_vtable

}
// @address 0x1be1e0 ?havok_world_shape_collides@@YA_NPBVphys_shape@@JPBUreal_matrix4x3@@W4e_havok_group@@@Z
bool havok_world_shape_collides(const phys_shape *shape, long component_index, const real_matrix4x3 *matrix, e_havok_group group)
{
    phys_transform transform;
    c_world_shape_penetration_collector collector(component_index);
    phys_transform_from_matrix4x3(&transform, matrix);
    phys_thread_memory *thread_memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    void *allocation = ((phys_thread_memory *)thread_memory)->allocate_chunk(sizeof(phys_simple_shape_volume), (phys_memory_class)47);
    ((phys_ref_object *)allocation)->m_mem_size_and_flags = sizeof(phys_simple_shape_volume);
    phys_simple_shape_volume *phantom = new (allocation) phys_simple_shape_volume(shape, transform, (unsigned int)group);
    g_havok_world->add_volume((phys_volume *)phantom);
    phantom->get_penetrations(collector);
    bool collided = collector.found_collision;
    g_havok_world->remove_volume((phys_volume *)phantom);
    ((phys_world_object *)phantom)->object_remove_ref();
    return collided;
}

static __forceinline c_havok_component *havok_velocity_component_get(long component_index)
{
    return datum_get<c_havok_component>(g_havok_component_data, component_index);
}
static __forceinline short havok_velocity_position_controlling_rigid_body_index(const c_havok_component *component)
{
    long index = component->m_object_postion_controlling_rigid_body_index;
    return (short)((index >= 0 && index < component->get_rigid_body_count()) ? index : -1);
}
static __forceinline bool havok_velocity_rigid_body_is_fixed(const c_havok_component *component, long rigid_body_index)
{
    return component->m_rigid_bodies.m_data[rigid_body_index].m_rigid_body->m_motion.m_type == 7;
}
static __forceinline bool havok_velocity_rigid_body_is_keyframed(const c_havok_component *component, long rigid_body_index)
{
    return component->m_rigid_bodies.m_data[rigid_body_index].m_rigid_body->m_motion.m_type == 6;
}
// @address 0xe2c88 ?havok_object_set_velocities@@YAXJPBTreal_vector3d@@0_N@Z
void havok_object_set_velocities(long object_index, const real_vector3d *linear_velocity, const real_vector3d *angular_velocity, bool unknown)
{
    object_datum *object = object_get(object_index);
    if (object->havok_component_index == -1)
        return;
    bool physics = ((object->physics_flags >> 9) & 1) != 0;
    if (physics &&
        (linear_velocity == 0 || magnitude_squared3d(linear_velocity) <= 9.999999e-09f) &&
        (angular_velocity == 0 || magnitude_squared3d(angular_velocity) <= 9.999999e-09f))
        return;
    c_havok_component *component = havok_velocity_component_get(object->havok_component_index);
    if (!unknown && !havok_component_has_active_entity(component))
        return;
    if ((1 << object_header_get(object_index)->object_type) & 2) {
        short rigid_body_index = havok_velocity_position_controlling_rigid_body_index(component);
        if (rigid_body_index != -1 &&
            !havok_velocity_rigid_body_is_fixed(component, rigid_body_index) &&
            !havok_velocity_rigid_body_is_keyframed(component, rigid_body_index)) {
            if (linear_velocity != 0)
                component->set_rigid_body_linear_velocity(rigid_body_index, linear_velocity);
            if (angular_velocity != 0)
                component->set_rigid_body_angular_velocity(rigid_body_index, angular_velocity);
        }
    } else {
        for (long rigid_body_index = 0; rigid_body_index < component->get_rigid_body_count(); rigid_body_index++) {
            if (!havok_velocity_rigid_body_is_fixed(component, rigid_body_index) &&
                !havok_velocity_rigid_body_is_keyframed(component, rigid_body_index)) {
                if (linear_velocity != 0)
                    component->set_rigid_body_linear_velocity(rigid_body_index, linear_velocity);
                if (angular_velocity != 0)
                    component->set_rigid_body_angular_velocity(rigid_body_index, angular_velocity);
            }
        }
    }
    bool connected_to_world = ((component->m_flags >> 3) & 1) != 0;
    if (unknown && connected_to_world)
        havok_component_activate_rigid_bodies(component, 0);
}

__forceinline long c_havok_component::calculate_heap_memory_usage(long *contact_point_memory, long *rigid_body_memory, long *constraint_memory,
    long *phantom_memory)
{
    *contact_point_memory = 0;
    *constraint_memory = m_component_constraints.get_sector_capacity() * sizeof(c_constraint);
    const phys_array<s_body_in_phantom> *bodies_in_phantom = m_bodies_in_phantom;
    *phantom_memory = (bodies_in_phantom == 0 ? 0 : bodies_in_phantom->get_sector_capacity()) * sizeof(s_body_in_phantom);
    *rigid_body_memory = m_rigid_bodies.get_sector_capacity() * sizeof(c_rigid_body);
    for (long rigid_body_index = 0; rigid_body_index < get_rigid_body_count(); rigid_body_index++)
    {
        long index_count = get_physics_model_rigid_body_index_count(rigid_body_index);
        if (index_count > 4)
            *rigid_body_memory += ((index_count + 3) >> 2) * sizeof(long);
    }
    return *contact_point_memory + *constraint_memory + *phantom_memory + *rigid_body_memory;
}

// @address 0x1bd5e0 ?havok_display_stats@@YAX_N@Z
void havok_display_stats(bool display_on_screen)
{
    // Stats are always displayed on screen.
    if (g_havok_world && g_havok_world->get_fixed_island())
    {
        long active_simulation_island_count = g_havok_world->get_active_islands().getSize();
        long inactive_simulation_island_count = g_havok_world->get_inactive_islands().getSize();
        long fixed_entity_count = g_havok_world->get_fixed_island()->get_entities().getSize();
        long active_entity_count = 0;
        long inactive_entity_count = 0;
        {
            c_havok_entity_iterator iterator(g_havok_world, c_havok_entity_iterator::_iterate_active);
            iterator.begin();
            while (iterator.next())
                active_entity_count++;
            iterator.end();
        }
        {
            c_havok_entity_iterator iterator(g_havok_world, c_havok_entity_iterator::_iterate_inactive);
            iterator.begin();
            while (iterator.next())
                inactive_entity_count++;
            iterator.end();
        }
        havok_display_stats_printf(true, global_real_argb_green, "Havok Performance Stats");
        havok_display_stats_printf(
            true, 0, "\t simulation islands (active/inactive):   %d/%d", active_simulation_island_count, inactive_simulation_island_count);
        havok_display_stats_printf(true, 0, "\t fixed entities:                         %d", fixed_entity_count);
        havok_display_stats_printf(true, 0, "\t dynamic entities (active/inactive):     %d/%d", active_entity_count, inactive_entity_count);

        s_havok_memory_usage havok_memory_usage;
        havok_compute_memory_in_use(&havok_memory_usage);
        float percent_used = 100.0f * havok_memory_usage.runtime_pool_memory_usage.memory_fraction;
        float debug_memory_kilobytes = (float)(havok_memory_usage.runtime_pool_memory_usage.debug_memory +
            havok_memory_usage.overflow_pool_memory_usage.debug_memory +
            havok_memory_usage.startup_pool_memory_usage.debug_memory) / 1024.0f;
        havok_display_stats_printf(true, global_real_argb_green, "Havok Memory (total %.2f KB, debug %.2f KB, percent used %.3f)",
            (float)havok_display_stats_get_maximum_memory_allocated() / 1024.0f, debug_memory_kilobytes, percent_used);
        havok_display_stats_printf(true, global_real_argb_green, "Startup pool: mem(%.2f KB/%.2f KB), allocs(%d)",
            (float)havok_memory_usage.startup_pool_memory_usage.memory_used / 1024.0f,
            (float)havok_memory_usage.startup_pool_memory_usage.memory_size / 1024.0f,
            havok_memory_usage.startup_pool_memory_usage.allocation_count);
        havok_display_stats_printf(true, global_real_argb_green, "Overflow pool: mem(%.2f KB/%.2f KB), allocs(%d))",
            (float)havok_memory_usage.overflow_pool_memory_usage.memory_used / 1024.0f,
            (float)havok_memory_usage.overflow_pool_memory_usage.memory_size / 1024.0f,
            havok_memory_usage.overflow_pool_memory_usage.allocation_count);
        havok_display_stats_printf(true, global_real_argb_green, "2nd Overflow pool: mem(%.2f KB/%.2f KB), allocs(%d))",
            (float)havok_memory_usage.second_overflow_pool_memory_usage.memory_used / 1024.0f,
            (float)havok_memory_usage.second_overflow_pool_memory_usage.memory_size / 1024.0f,
            havok_memory_usage.second_overflow_pool_memory_usage.allocation_count);
        havok_display_stats_printf(true, global_real_argb_green, "Runtime Pool(%.2f KB): usage==pages(%d/%d)*size(%.2f KB)+last(%.2f KB)",
            (float)(havok_memory_usage.runtime_pool_memory_usage.memory_size - havok_memory_usage.runtime_pool_memory_usage.debug_memory) / 1024.0f,
            havok_memory_usage.runtime_pool_memory_usage.pages_used,
            havok_memory_usage.runtime_pool_memory_usage.page_count,
            (float)havok_memory_usage.runtime_pool_memory_usage.page_size / 1024.0f,
            (float)(havok_memory_usage.runtime_pool_memory_usage.page_size - havok_memory_usage.runtime_pool_memory_usage.last_page_free) / 1024.0f);
        havok_display_stats_printf(true, global_real_argb_green, "\t\tpage_fraction %.2f, memory_fraction %.2f",
            (float)havok_memory_usage.runtime_pool_memory_usage.pages_used / (float)havok_memory_usage.runtime_pool_memory_usage.page_count,
            havok_memory_usage.runtime_pool_memory_usage.memory_fraction);
        for (long bucket_index = 0; bucket_index < havok_memory_usage.runtime_pool_memory_usage.bucket_count; bucket_index++)
        {
            havok_display_stats_printf(true, global_real_argb_blue, "\t b%d(%d) (%d / %d)", bucket_index,
                havok_memory_usage.runtime_pool_memory_usage.buckets[bucket_index].page_count,
                havok_memory_usage.runtime_pool_memory_usage.buckets[bucket_index].allocation_size,
                havok_memory_usage.runtime_pool_memory_usage.buckets[bucket_index].allocation_count);
        }

        long total_contact_point_memory = 0;
        long total_rigid_body_memory = 0;
        long total_phantom_memory = 0;
        long total_constraint_memory = 0;
        long maximum_contact_point_memory = 0;
        long maximum_rigid_body_memory = 0;
        long maximum_phantom_memory = 0;
        long maximum_constraint_memory = 0;
        c_data_iterator<c_havok_component> component_iterator;
        component_iterator.begin(g_havok_component_data);
        while (component_iterator.next())
        {
            long contact_point_memory;
            long rigid_body_memory;
            long phantom_memory;
            long constraint_memory;
            component_iterator.get_datum()->calculate_heap_memory_usage(&contact_point_memory, &rigid_body_memory, &constraint_memory, &phantom_memory);
            total_contact_point_memory += contact_point_memory;
            total_rigid_body_memory += rigid_body_memory;
            total_phantom_memory += phantom_memory;
            total_constraint_memory += constraint_memory;
            maximum_contact_point_memory = maximum_contact_point_memory > contact_point_memory ? maximum_contact_point_memory : contact_point_memory;
            maximum_rigid_body_memory = maximum_rigid_body_memory > rigid_body_memory ? maximum_rigid_body_memory : rigid_body_memory;
            maximum_phantom_memory = maximum_phantom_memory > phantom_memory ? maximum_phantom_memory : phantom_memory;
            maximum_constraint_memory = maximum_constraint_memory > constraint_memory ? maximum_constraint_memory : constraint_memory;
        }
        havok_display_stats_printf(true, global_real_argb_green, "Havok Component Meta Information Resource Stats");
        havok_display_stats_printf(true, 0, "HEAP meta contact point memory %.2f KB (max %d), meta rigid body memory %.2f KB (max %d)",
            (float)total_contact_point_memory / 1024.0f, maximum_contact_point_memory,
            (float)total_rigid_body_memory / 1024.0f, maximum_rigid_body_memory);
        havok_display_stats_printf(true, 0, "HEAP meta phantom memory %.2f KB (max %d), meta constaint memory %.2f KB (max %d)",
            (float)total_phantom_memory / 1024.0f, maximum_phantom_memory,
            (float)total_constraint_memory / 1024.0f, maximum_constraint_memory);
        havok_display_stats_printf(true, 0, "TOAL HEAP meta memory %.2f KB",
            (float)(total_contact_point_memory + total_rigid_body_memory + total_phantom_memory + total_constraint_memory) / 1024.0f);
    }
}

// @address 0x1bcf20 ?havok_all_entities_in_island_contain_players@@YA_NPBVphys_island@@@Z
bool havok_all_entities_in_island_contain_players(const phys_island *island)
{
    bool result = false;
    long entity_index = 0;
    long entity_count = island->m_island_entities.m_size;
    if (entity_count > 0) {
        phys_entity *const *entity = island->m_island_entities.m_data;
        result = true;
        s_data_array *components = g_havok_component_data;
        do {
            long component_index = (long)(*entity)->get_world_object_property(0x2001).m_data;
            if (component_index == 0)
                component_index = -1;
            bool contains_player = false;
            if (component_index != -1)
                contains_player = object_is_or_contains_player(datum_get<c_havok_component>(components, component_index)->m_object_index);
            if (contains_player) {
                entity_index++;
                entity++;
            } else {
                result = false;
                break;
            }
        } while (entity_index < entity_count);
    }
    return result;
}

namespace {
static __forceinline long havok_object_ultimate_parent(long object_index)
{
    long result = -1;
    while (object_index != -1)
    {
        result = object_index;
        object_index = object_get(object_index)->parent_object_index;
    }
    return result;
}
}

// the object a reclaim pass may delete for this rigid body at this level, or -1
// @address 0x1bd010 havok_cull_objects_filter_rigid_body
extern "C" long havok_cull_objects_filter_rigid_body(long level, phys_entity *entity, bool over_budget, bool include_inactive,
    bool island_contains_players, long protected_object_index)
{
    long component_index = havok_entity_get_havok_component_index(entity);
    long result = -1;
    if (component_index != -1) {
        long object_index = datum_get<c_havok_component>(g_havok_component_data, component_index)->m_object_index;
        if (level == 0) {
            if (over_budget) {
                if (!object_is_or_contains_player(object_index) || island_contains_players)
                    result = object_index;
            }
        } else if (havok_object_ultimate_parent(object_index) != protected_object_index &&
            !simulation_query_object_is_predicted(object_index)) {
            s_object_header *header = object_header_get(object_index);
            bool recycling = _rotl(header->datum->recycling_flags, 31) & 1;
            unsigned char multiplayer_object = game_globals->options.game_mode == 2 &&
                object_header_get(object_index)->datum->multiplayer_data_offset != -1;
            if (include_inactive || (header->flags & 1) != 0) {
                if (!recycling) {
                    if (multiplayer_object) {
                        if (level >= 5)
                            result = object_index;
                    } else {
                        long type_mask = 1 << header->object_type;
                        if ((type_mask & _object_mask_unit) != 0) {
                            if (level >= 6 && !object_is_or_contains_player(object_index))
                                result = object_index;
                        } else if ((type_mask & _object_mask_crate) != 0) {
                            if (level >= 2)
                                result = object_index;
                        } else if ((type_mask & _object_mask_item) != 0) {
                            if (level >= 3)
                                result = object_index;
                        } else if ((type_mask & _object_mask_creature) != 0) {
                            if (level >= 4)
                                result = object_index;
                        }
                    }
                } else {
                    result = object_index;
                }
            }
        }
    }
    return result;
}

// @address 0x707950 havok_initialize_collision_tolerance
extern "C" void havok_initialize_collision_tolerance(void)
{
    g_havok_constants.collision_tolerance = phys_convex_shape_default_radius * 0.328f;
}

// @address 0x1bb160 ?havok_initialize_object_rigid_bodies@@YAXXZ
void havok_initialize_object_rigid_bodies()
{
    {
        c_object_iterator<object_datum> iterator;
        iterator.begin(0, 0);
        while (iterator.next())
            object_get(iterator.get_index())->havok_component_index = -1;
    }
    {
        c_object_iterator<object_datum> iterator;
        iterator.begin(0, 1);
        while (iterator.next())
        {
            long object_index = iterator.get_index();
            if (object_get(object_index)->havok_component_index == -1)
                havok_object_new_recursive(object_index);
        }
    }
    havok_set_initial_simulation_island_activity();
    g_havok_proxy_rigid_bodies_active = true;
    g_havok_monitor_globals.initialize_time = game_time_get();
    c_data_iterator<s_havok_proxy_datum> iterator;
    iterator.begin((s_data_array *)g_havok_proxy_data);
    while (iterator.next())
    {
        iterator.get_datum()->body = 0;
        if (!havok_proxy_aquire_internal(iterator.get_datum()->object_index, false))
            havok_proxy_release_internal(iterator.get_datum()->object_index, true);
    }
}

// @address 0x1bb730 ?havok_object_new@@YAXJ@Z
void havok_object_new(long object_index)
{
    object_datum *object = object_get(object_index);

    havok_memory_garbage_collect_locked();
    if (object->havok_component_index == -1 && havok_object_needs_physics(object_index, object->definition_index))
    {
        object->m_physics_flags.set(_object_physics_phantom_active_bit, false);
        if (g_havok_monitor_globals.initialized_for_new_map)
        {
            if (!havok_can_allocate_space_for_instance_of_object_definition(object->definition_index))
                havok_make_space_for_connected_rigid_bodies(havok_object_ultimate_parent(object_index));
            object->havok_component_index = havok_component_new(object_index);
            havok_component_construct(object->havok_component_index);
            havok_object_update_component_state(object_index, object->m_physics_flags.test(_object_physics_connected_bit));
        }
        havok_simulation_island_cleanup_resources();
        havok_memory_garbage_collect_locked();
    }
}

class c_runtime_pool;

// @address 0x103460 havok_memory_garbage_collect_locked
extern "C" void havok_memory_garbage_collect_locked(void)
{
    if (havok_garbage_collection_locked())
        return;
    havok_garbage_collection_lock(true);
    unsigned char collected;
    long attempt = 0;
    do
    {
        havok_memory_garbage_collect();
        if (havok_memory_is_runtime_pool_full((const c_runtime_pool *)g_havok_memory_globals.runtime_pool))
        {
            havok_memory_garbage_collect_reset_allocated_state();
            collected = 1;
        }
        else
        {
            collected = 0;
        }
        attempt++;
    } while (attempt < 4 && collected != 0);
    havok_garbage_collection_lock(false);
}

// @address 0x1bba50 ?havok_object_rebuild@@YAXJJ@Z
void havok_object_rebuild(long object_index, long unknown)
{
    object_datum *object = object_get(object_index);
    long attached = object->havok_component_index;
    if (attached != -1) {
        c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, attached);
        bool was_set = ((component->m_flags >> 3) & 1) != 0;
        if (was_set)
            component->disconnect_from_havok_world();
        component->clear_rigid_bodies();
        component->m_flags = unknown;
        havok_component_construct(object->havok_component_index);
        havok_object_update_component_state(object_index, was_set);
    }
    s_object_header *header = object_header_get(object_index);
    unsigned long object_flags = header->datum->physics_flags;
    if ((_rotl(object_flags, 18) & 1) != 0) {
        if (((header->flags & 1) != 0) && ((_rotl(object_flags, 25) & 1) != 0)) {
            havok_proxy_release_internal(object_index, false);
            havok_proxy_aquire_internal(object_index, false);
        } else {
            havok_proxy_release_internal(object_index, true);
        }
    }
}

// @address 0x1bb850 havok_object_update_component_state
extern "C" void havok_object_update_component_state(long object_index, bool was_set)
{
    object_datum *object = object_get(object_index);
    c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, object->havok_component_index);

    if (was_set)
        component->connect_to_havok_world();
    unsigned long flags = component->m_flags;
    bool connected = ((_rotl(flags, 23) & 1) != 0) || ((_rotl(flags, 18) & 1) != 0);
    if (connected)
        object->physics_flags |= 0x2000;
    else
        object->physics_flags &= ~0x2000;
    if (((1 << object_header_get(object_index)->object_type) & 1) != 0) {
        c_motor_request request;
        request.setup_notify_physics_built();
        motor_system_submit(object_index, &request);
    }
}

namespace {
static __forceinline unsigned char havok_sleep_flag_test(unsigned long flags, long bit) {
    return (unsigned char)((flags >> bit) & 1);
}
static __forceinline bool havok_sleep_body_can_sleep(phys_rigid_body *body) {
    return body->m_motion.m_type != 7 && body->m_motion.m_type != 6;
}
}

// @address 0xdf6e0 havok_crate_needs_physics_update
extern "C" bool havok_crate_needs_physics_update(long index)
{
    bool result = false;
    object_datum *object = object_get(index);
    if (object->simulation_entity_index != -1) {
        long component_index = object->havok_component_index;
        if (component_index != -1 && object->object_type == 10) {
            long datum_index = component_index & 0xffff;
            bool all_clear = true;
            c_havok_component *component = &((c_havok_component *)((s_data_array *)g_havok_component_data)->data)[datum_index];
            long count = component->m_rigid_bodies.m_size;
            if (count > 0) {
                for (long body_index = 0; body_index < count; ++body_index) {
                    all_clear = all_clear && havok_sleep_body_can_sleep(component->m_rigid_bodies.m_data[body_index].m_rigid_body);
                }
            }
            if (havok_sleep_flag_test(component->m_flags, 7) != 0 || !all_clear)
                result = true;
        }
    }
    return result;
}

// @address 0x1bd218 havok_cleanup_large_simulation_islands
extern "C" bool havok_cleanup_large_simulation_islands(long level, bool active, bool flag, long object_index)
{
    long best_count = 0, best_object = 0, best_usage = 0, best_usage_count = 0, best_usage_object = 0;
    phys_world *world;
    phys_array<phys_island *> *islands;
#if defined(_MSC_VER) && !defined(__clang__)
    // match: 0x1bd218: the address-building read of havok_cleanup::g_havok_world (physics/havok_dependencies.h).
    if (active) {
        world = *(phys_world **)havok_cleanup::g_havok_world;
        islands = &world->m_active_islands;
    } else {
        world = *(phys_world **)havok_cleanup::g_havok_world;
        islands = &world->m_inactive_islands;
    }
#else
    world = g_havok_world;
    islands = active ? &world->m_active_islands : &world->m_inactive_islands;
#endif
    best_count = (long)0x80000000;
    best_object = -1;
    long island_index;
    if (!active) {
        // match: 0x1bd218: value range assumption guides branch lowering
        __assume(!active);
        phys_island *fixed_island = world->m_fixed_island;
        bool contains_players = havok_all_entities_in_island_contain_players(fixed_island);
        for (long i = 0; i < fixed_island->m_island_entities.getSize(); i++) {
            // match: 0x1bd218: value range assumption guides branch lowering
            __assume(i >= 0);
            phys_entity *entity = fixed_island->m_island_entities.m_data[i];
            long count = entity->get_chain_constraint_count();
            if (count > best_count) {
                long candidate = havok_cull_objects_filter_rigid_body(level, entity, 0, flag, contains_players, object_index);
                if (candidate != -1) {
                    best_count = count;
                    best_object = candidate;
                }
            }
        }
    }
    best_usage = 0;
    best_usage_count = (long)0x80000000;
    best_usage_object = -1;
    for (island_index = 0; island_index < islands->m_size; island_index++) {
        phys_island *island = islands->m_data[island_index];
        bool contains_players = havok_all_entities_in_island_contain_players(island);
        bool over_budget = (float)island->get_integration_mem_usage() > 4096.0f;
        for (long entity_index = 0; entity_index < island->m_island_entities.getSize(); entity_index++) {
            phys_entity *entity = island->m_island_entities.m_data[entity_index];
            long count = entity->get_chain_constraint_count();
            if (over_budget && level == 0) {
                long usage = island->get_integration_mem_usage();
                if (usage < best_usage)
                    continue;
                if (count <= best_usage_count)
                    continue;
                long candidate = havok_cull_objects_filter_rigid_body(0, entity, over_budget, flag, contains_players, object_index);
                if (candidate == -1)
                    continue;
                best_usage = usage;
                best_usage_count = count;
                best_usage_object = candidate;
                continue;
            }
            if (count > best_count) {
                long candidate = havok_cull_objects_filter_rigid_body(level, entity, 0, flag, contains_players, object_index);
                if (candidate != -1) {
                    best_count = count;
                    best_object = candidate;
                }
            }
        }
    }
    if (best_usage_object != -1)
        best_object = best_usage_object;
    if (best_object != -1) {
        bool needs_rebuild = false;
        bool simulated = false;
        if (game_globals->options.game_simulation == 4)
            simulated = true;
        if (simulated != 0) {
            s_object_header *header = object_header_get(best_object);
            needs_rebuild = (header->datum->simulation_entity_index + 1) != 0;
        }
        if (needs_rebuild != 0) {
            const s_object_header *header = object_header_get(best_object);
            header->datum->physics_flags |= 64;
            havok_object_rebuild(best_object, 0);
            return true;
        }
        s_object_header *header = object_header_get(best_object);
        object_activation_regions_update_for_object(best_object, true);
        object_disconnect_from_physics(best_object);
        havok_object_delete(best_object);
        if ((header->flags & 16) == 0)
            object_delete(best_object);
        return true;
    }
    return false;
}

// @address 0xd1320 ??0phys_critical_section@@QAA@H@Z
#if defined(_XBOX) && !defined(__clang__)
__forceinline phys_critical_section::phys_critical_section(int spin_count) : phys_critical_section_base(spin_count)
#else
phys_critical_section::phys_critical_section(int spin_count) : phys_critical_section_base(spin_count)
#endif
{
    RtlInitializeCriticalSectionAndSpinCount(&m_section, spin_count);
    m_current_thread = -1;
}

// @address 0x1be128 ?havok_make_space_for_connected_rigid_bodies@@YAXJ@Z
void havok_make_space_for_connected_rigid_bodies(long object_index)
{
    bool active = false;
    long level = 1;
    if (g_havok_monitor_globals.entity_count > 1696 ||
        ((s_data_array *)g_havok_component_data)->actual_count ==
            ((s_data_array *)g_havok_component_data)->maximum_count)
    {
        while (g_havok_monitor_globals.entity_count > 1696 ||
            ((s_data_array *)g_havok_component_data)->actual_count ==
                ((s_data_array *)g_havok_component_data)->maximum_count)
        {
            if (!havok_cleanup_large_simulation_islands(level, active, true, object_index)) {
                if (level < 6) {
                    level++;
                } else if (!active) {
                    active = true;
                    level = 1;
                }
            }
        }
    }
}


// @address 0x63d3b0 ?add_chain_entity@phys_world@@QAAPAVphys_entity@@PAV2@W4phys_entity_activation@@@Z
// @address 0x1bc038 ?havok_world_add_entity@@YAXPAVphys_entity@@_N@Z
void havok_world_add_entity(phys_entity *entity, bool active)
{
    if (!g_havok_monitor_globals.inside_simulation_step) {
        if (g_havok_monitor_globals.entity_count >= 1700) {
            long component_index = havok_entity_get_havok_component_index(entity);
            long object_index = component_index != -1
                ? datum_get<c_havok_component>(g_havok_component_data, component_index)->get_object_index()
                : -1;
            havok_make_space_for_connected_rigid_bodies(object_index);
        }
        g_havok_monitor_globals.entity_count++;
        havok_shapes_lock_environment_null_shapes();
        phys_entity *result = g_havok_world->add_chain_entity(entity, active ? (phys_entity_activation)0 : (phys_entity_activation)1);
        havok_shapes_unlock_environment_null_shapes();
        havok_simulation_island_cleanup_resources();
    }
}

// @address 0xe34f0 havok_object_set_position
extern "C" void havok_object_set_position(long object_index, bool ignore_infinite_inertia, bool force, bool keep_rotation)
{
    s_object_header *header = object_header_get(object_index);
    object_datum *object = header->datum;
    long component_index = object->havok_component_index;
    if (component_index == -1)
        return;
    c_havok_component *component = datum_get<c_havok_component>(g_havok_component_data, component_index);
    long controlling_body = component->m_object_postion_controlling_rigid_body_index;
    if (controlling_body < 0 || controlling_body >= component->get_rigid_body_count())
        controlling_body = -1;
    short rigid_body_index = (short)controlling_body;
    unsigned char infinite_inertia;
    if (rigid_body_index == -1) {
        if (!force)
            return;
        infinite_inertia = 0;
    } else {
        infinite_inertia = (component->m_rigid_bodies.m_data[rigid_body_index].m_flags & 2) != 0 &&
                           ((1 << header->object_type) & 1) != 0;
    }
    if (infinite_inertia && ignore_infinite_inertia)
        return;
    unsigned long flags = component->m_flags;
    unsigned char keyframed = ((flags >> 5) & 1) != 0;
    unsigned char sentinal = ((flags >> 4) & 1) != 0;
    unsigned char skip = keyframed || sentinal;
    if (skip) {
        if (!force)
            return;
        if (keyframed)
            return;
    }
    havok_component_update_body_transforms(component, component_index, force, keep_rotation);
}

class c_havok_memory;

class c_allocation_base;

// @address 0x78326c ?g_impacts_globals_allocator@@3V?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_impacts_globals_allocator@@YAXPAX@Z@@A
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_impacts_globals_allocator> g_impacts_globals_allocator;

// @address 0x1ba8d8 ?havok_initialize@@YAXXZ
void havok_initialize(void)
{
    g_havok_monitor_globals.lock_count_4c++;
    g_havok_monitor_globals.monitor_stream_count = 0;
    g_havok_memory_task_data = (long *)g_havok_game_state_allocator.reserve_memory("havok gamestate", 0, 8, 0);
    g_havok_component_data = data_new("havok components", 1500, sizeof(c_havok_component), 4, (c_allocation_base *)&g_runtime_state_allocation);
    ::havok_memory_update_initialize();
    TLS_IMPACT_DATA = (s_data_array *)data_new("impacts", 32, 180, 0, (c_allocation_base *)&g_impact_data_allocator);
    TLS_IMPACT_ARRAY_DATA = data_new("impact arrarys", 32, 136, 0, (c_allocation_base *)&g_impact_array_data_allocator);
    impacts_globals_type *impacts = (impacts_globals_type *)g_impacts_globals_allocator.reserve_memory("impact globals", 0, 140, 0);
    impacts->disconnected_from_physics = false;
    g_impacts_globals = impacts;
    void *memory = g_havok_monitor_globals.thread_memory;
    memset(memory, 0, sizeof(phys_thread_memory));
    memset(g_havok_monitor_globals.scratch, 0, sizeof(g_havok_monitor_globals.scratch));
    new (memory) phys_thread_memory((phys_memory *)(&g_havok_memory), 0);
    ((phys_thread_memory *)memory)->set_stack_area(g_havok_monitor_globals.scratch, sizeof(g_havok_monitor_globals.scratch));
    memset((s_havok_monitor_stream *)g_havok_monitor_globals.monitor_stream, 0, sizeof(g_havok_monitor_globals.monitor_stream));
    g_havok_monitor_globals.lock_count_4c--;
}

// @address 0x1be450 ?havok_notify_object_changed_region_permutation@@YAXJJPBD@Z
void havok_notify_object_changed_region_permutation(long object_index, long unused, const char *old_permutations)
{
    object_datum *object = object_get(object_index);
    if ((unsigned char)((object->physics_flags >> 17) & 1) && object->havok_component_index != -1) {
        long buffer[6];
        long count = havok_get_object_bounds_and_group(object_index, buffer);
        havok_update_bodies_in_bounds(count, buffer);
    }
    if (object->havok_component_index == -1)
        return;
    c_havok_component *component =
        datum_get<c_havok_component>(g_havok_component_data, object->havok_component_index);
    unsigned char physics_model_component = ((component->m_flags >> 8) & 1) != 0;
    if (!physics_model_component)
        return;
    bool rebuild = false;
    s_object_definition *definition =
        TAG_GET('obje', s_object_definition, object->definition_index);
    s_model_definition *model =
        TAG_GET('hlmt', s_model_definition, definition->model.index);
    object_datum *region_datum = object_get(object_index);
    long size = region_datum->region_information.size;
    char *base = (char *)region_datum + region_datum->region_information.offset;
    if ((unsigned char)((region_datum->flags >> 11) & 1))
        size -= 16;
    long region_count = size / 10;
    char *permutations = base + region_count * 8;
    for (long region_index = 0; region_index < model->regions.count; ++region_index) {
        s_model_region *region = (s_model_region *)model->regions.address + region_index;
        if (old_permutations[region_index] == permutations[region_index])
            continue;
        if (old_permutations[region_index] == -1 || permutations[region_index] == -1) {
            long permutation_index = old_permutations[region_index] != -1 ? old_permutations[region_index] : permutations[region_index];
            s_model_region_permutation *permutation =
                (s_model_region_permutation *)region->permutations.address + permutation_index;
            if (permutation->physics_permutation_index != -1) {
                rebuild = true;
                break;
            }
        } else {
            s_model_region_permutation *old_permutation =
                (s_model_region_permutation *)region->permutations.address + old_permutations[region_index];
            s_model_region_permutation *permutation =
                (s_model_region_permutation *)region->permutations.address + permutations[region_index];
            if (old_permutation->physics_permutation_index != permutation->physics_permutation_index) {
                rebuild = true;
                break;
            }
        }
    }
    if (rebuild)
        havok_object_rebuild(object_index, 0);
}

static __forceinline void havok_world_add_entity_batch(phys_entity *const *entities, long count, phys_entity_activation activation)
{
    g_havok_world->add_entity_batch(entities, count, activation);
}
static __forceinline void havok_world_split_simulation_islands()
{
    const phys_array<phys_island *> &inactive_islands = g_havok_world->get_inactive_islands();
    const phys_array<phys_island *> &active_islands = g_havok_world->get_active_islands();
    for (long island_index = inactive_islands.getSize() - 1; island_index >= 0; island_index--)
    {
        phys_island *island = inactive_islands[island_index];
        phys_world_op_util::split_island(g_havok_world, island);
    }
    for (long island_index = active_islands.getSize() - 1; island_index >= 0; island_index--)
        phys_world_op_util::split_island(g_havok_world, active_islands[island_index]);
}

// every rigid body goes back into the world in batches (the static bodies, proxies and object bodies), the simulation
// islands are split again, and the objects' constraints are added back
// @address 0x1bc120 havok_world_rebuild_all_entities
extern "C" void havok_world_rebuild_all_entities()
{
    phys_entity *activated_entities[512];
    phys_entity *deactivated_entities[512];
    long deactivated_count = 0;
    long activated_count = 0;

    for (long body_index = 0; body_index < 16; body_index++)
    {
        if (g_havok_monitor_globals.rigid_bodies[body_index])
            activated_entities[activated_count++] = (phys_entity *)g_havok_monitor_globals.rigid_bodies[body_index];
    }

    {
        phys_entity *proxy_entities[16];
        long proxy_count = 0;
        c_data_iterator<s_havok_proxy_datum> proxy_iterator;
        proxy_iterator.begin((s_data_array *)g_havok_proxy_data);
        while (proxy_iterator.next())
        {
            if (proxy_iterator.get_datum()->body)
                proxy_entities[proxy_count++] = (phys_entity *)proxy_iterator.get_datum()->body;
        }
        for (long proxy_index = 0; proxy_index < proxy_count; proxy_index++)
            deactivated_entities[deactivated_count++] = proxy_entities[proxy_index];
    }

    {
        c_object_iterator<object_datum> iterator;
        iterator.begin(0, 0);
        while (iterator.next())
        {
            object_datum *object = object_get(iterator.get_index());
            bool connected = (object->physics_flags >> 7) & 1;
            if (connected && object->havok_component_index != -1)
            {
                bool fixed = (object->physics_flags >> 9) & 1;
                c_havok_component *component = havok_velocity_component_get(object->havok_component_index);
                if (fixed)
                {
                    for (long rigid_body_index = 0; rigid_body_index < component->get_rigid_body_count(); rigid_body_index++)
                        activated_entities[activated_count++] = (phys_entity *)component->m_rigid_bodies.m_data[rigid_body_index].m_rigid_body;
                    if (activated_count >= 448)
                    {
                        havok_world_add_entity_batch(activated_entities, activated_count, (phys_entity_activation)0);
                        activated_count = 0;
                    }
                }
                else
                {
                    for (long rigid_body_index = 0; rigid_body_index < component->get_rigid_body_count(); rigid_body_index++)
                        deactivated_entities[deactivated_count++] = (phys_entity *)component->m_rigid_bodies.m_data[rigid_body_index].m_rigid_body;
                    if (deactivated_count >= 448)
                    {
                        havok_world_add_entity_batch(deactivated_entities, deactivated_count, (phys_entity_activation)1);
                        deactivated_count = 0;
                    }
                }
            }
        }
    }
    if (activated_count > 0)
        havok_world_add_entity_batch(activated_entities, activated_count, (phys_entity_activation)0);
    if (deactivated_count > 0)
        havok_world_add_entity_batch(deactivated_entities, deactivated_count, (phys_entity_activation)1);

    havok_world_split_simulation_islands();

    {
        c_object_iterator<object_datum> iterator;
        iterator.begin(0, 0);
        while (iterator.next())
        {
            object_datum *object = object_get(iterator.get_index());
            bool connected = (object->physics_flags >> 7) & 1;
            if (connected && object->havok_component_index != -1)
            {
                c_havok_component *component = havok_velocity_component_get(object->havok_component_index);
                for (long constraint_index = 0; constraint_index < component->m_component_constraints.getSize(); constraint_index++)
                    havok_world_add_constraint(component->m_component_constraints.m_data[constraint_index].m_contact_constraint);
            }
        }
    }
}

class phys_mat3;
class phys_quat;
class phys_statistics_collector;

static __forceinline long havok_acceleration_ultimate_parent(long object_index)
{
    long parent = -1;
    while (object_index != -1)
    {
        parent = object_index;
        object_index = object_get(object_index)->parent_object_index;
    }
    return parent;
}
static __forceinline void havok_acceleration_scale(real_vector3d *v, float scale)
{
    v->i *= scale; v->j *= scale; v->k *= scale;
}
static __forceinline void havok_acceleration_angular(c_havok_component *component, long index, const real_vector3d *acceleration)
{
    phys_rigid_body *body = (phys_rigid_body *)component->m_rigid_bodies.m_data[index].m_rigid_body;
    phys_vec4 torque;
    torque.set(acceleration->i, acceleration->j, acceleration->k, 0.0f);
    torque.mul4(body->m_motion.get_mass());
    ((phys_entity *)body)->activate();
#if !defined(_XBOX) || defined(__clang__)
    body->get_rigid_motion()->apply_torque(1.0f, torque);
#else
    body->m_motion.apply_torque(1.0f, torque);
#endif
}
// @address 0x1bdc30 ?havok_object_apply_acceleration@@YA_NJJPBTreal_point3d@@PBTreal_vector3d@@1PAJPAT2@3@Z
bool havok_object_apply_acceleration(long object_index, long node_index, const real_point3d *point,
    const real_vector3d *translational_acceleration, const real_vector3d *angular_acceleration,
    long *out_accelerated_object_index, real_vector3d *out_translational_velocity, real_vector3d *out_angular_velocity)
{
    object_datum *object = object_get(object_index);
    bool result = false;
    if (object->havok_component_index != -1)
    {
        long parent_index = havok_acceleration_ultimate_parent(object_index);
        object_datum *parent = object_get(parent_index);
        real_vector3d modified_translational_acceleration;
        real_vector3d modified_angular_acceleration;
        if (translational_acceleration)
            modified_translational_acceleration = *translational_acceleration;
        if (angular_acceleration)
            modified_angular_acceleration = *angular_acceleration;
        if (parent->havok_component_index != -1)
        {
            c_havok_component *component = havok_velocity_component_get(parent->havok_component_index);
            if (parent_index != object_index)
            {
                const s_object_definition *definition =
                    TAG_GET('obje', const s_object_definition, object->definition_index);
                const s_object_definition *parent_definition =
                    TAG_GET('obje', const s_object_definition, parent->definition_index);
                if (definition->acceleration_scale > 0.001f)
                {
                    float scale = parent_definition->acceleration_scale / definition->acceleration_scale;
                    if (translational_acceleration)
                        havok_acceleration_scale(&modified_translational_acceleration, scale);
                    if (angular_acceleration)
                        havok_acceleration_scale(&modified_angular_acceleration, scale);
                }
            }
            if (!havok_component_has_active_entity(component))
                havok_component_activate_rigid_bodies(component, 0);
            long rigid_body_index = node_index == -1 || parent_index != object_index ? -1 :
                havok_object_compute_rigid_body_index_of_node(parent_index, node_index);
            long rigid_body_count = 0;
            long rigid_body_indices[64];
            if (rigid_body_index != -1)
            {
                rigid_body_count = 1;
                rigid_body_indices[0] = rigid_body_index;
            }
            else
            {
                for (long i = 0; i < component->get_rigid_body_count(); ++i)
                    rigid_body_indices[i] = i;
                rigid_body_count = component->get_rigid_body_count();
            }
            for (long i = 0; i < rigid_body_count; ++i)
            {
                long index = rigid_body_indices[i];
                if (!havok_velocity_rigid_body_is_fixed(component, index) && !havok_velocity_rigid_body_is_keyframed(component, index))
                {
                    if (!point || !translational_acceleration || (component->m_rigid_bodies.m_data[index].m_flags & 2))
                    {
                        if (translational_acceleration)
                            component->rigid_body_apply_acceleration(index, &modified_translational_acceleration);
                    }
                    else
                        component->rigid_body_apply_acceleration_at_point(index, point, &modified_translational_acceleration);
                    if (angular_acceleration && !(component->m_rigid_bodies.m_data[index].m_flags & 2))
                        havok_acceleration_angular(component, index, &modified_angular_acceleration);
                    if (component->get_object_postion_controlling_rigid_body_index() == index)
                    {
                        *out_accelerated_object_index = component->m_object_index;
                        component->get_rigid_body_linear_velocity(index, out_translational_velocity);
                        component->get_rigid_body_angular_velocity(index, out_angular_velocity);
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x1073c8 havok_cull_objects
extern "C" void havok_cull_objects()
{
    if (g_havok_world && !g_havok_monitor_globals.garbage_collection_locked)
    {
        long const maximum_memory_usage = 0x10000;
        g_havok_monitor_globals.garbage_collection_locked = true;
        for (long island_index = 0; island_index < g_havok_world->get_active_islands().getSize(); island_index++)
        {
            phys_island *island = g_havok_world->get_active_islands()[island_index];
            bool island_contains_players = havok_all_entities_in_island_contain_players(island);
            if (island->get_integration_mem_usage() > maximum_memory_usage)
            {
                for (long reclaim_level = 0; reclaim_level < 7; reclaim_level++)
                {
                    int best_constraint_count = (int)0x80000000;
                    long best_object_index = -1;
                    for (long entity_index = 0; entity_index < island->get_entities().getSize(); entity_index++)
                    {
                        phys_entity *entity = island->get_entities()[entity_index];
                        int constraint_count = entity->get_chain_constraint_count();
                        if (constraint_count > best_constraint_count)
                        {
                            long object_index = havok_cull_objects_filter_rigid_body(reclaim_level, entity, 0, 1, island_contains_players, -1);
                            if (object_index != -1)
                            {
                                best_constraint_count = constraint_count;
                                best_object_index = object_index;
                            }
                        }
                    }
                    if (best_object_index != -1)
                    {
                        s_damage_globals_definition *damage_globals =
                            (s_damage_globals_definition *)global_game_globals->damage_globals.address;
                        s_object_header *header = object_header_get(best_object_index);
                        object_datum *object = header->datum;
                        if (damage_globals->large_island_cull_effect.index != -1)
                        {
                            effect_new_unattached_from_point(damage_globals->large_island_cull_effect.index, &object->bounding_center,
                                &object->forward, global_up3d, 0, 1, NULL, NULL);
                        }
                        object_disconnect_from_physics(best_object_index);
                        havok_object_delete(best_object_index);
                        if ((header->flags & 0x10) == 0)
                            object_delete(best_object_index);
                        break;
                    }
                }
            }
        }
        g_havok_monitor_globals.garbage_collection_locked = false;
    }
}
extern "C" {
// @address 0x7832f4 g_hk_error
}
// @address 0x260618 ??_Gc_entity_listener@@UAAPAXI@Z
// @address 0x260b60 ??_Ec_entity_listener@@W3AAPAXI@Z
// @address 0x260b70 ??_Gc_world_entity_listener@@UAAPAXI@Z
// @address 0x5bec98 ?on_entity_activated@c_entity_listener@@UAAXPAVphys_entity@@@Z
// @address 0x5bec98 ?contact_point_confirmed_callback@c_entity_listener@@UAAXAAUphys_contact_confirmed_event@@@Z
// @address 0x5bec98 ?contact_point_added_callback@c_world_entity_listener@@UAAXAAUphys_contact_added_event@@@Z
// @address 0x5bec98 ?contact_point_confirmed_callback@c_world_entity_listener@@UAAXAAUphys_contact_confirmed_event@@@Z
// @address 0x5bec98 ?contact_point_removed_callback@c_world_entity_listener@@UAAXAAUphys_contact_removed_event@@@Z
// @address 0x5bec98 ?contact_process_callback@phys_collision_listener@@UAAXAAUphys_contact_process_event@@@Z

static __forceinline long havok_first_active_structure_bsp()
{
    return 31 - _CountLeadingZeros(-g_active_structure_bsp_mask & g_active_structure_bsp_mask);
}
static __forceinline long havok_next_active_structure_bsp(long index)
{
    unsigned long remaining = g_active_structure_bsp_mask & ~(unsigned long)((1i64 << (index + 1)) - 1);
    return 31 - _CountLeadingZeros(-remaining & remaining);
}
// @address 0x5bec98 ?havok_error_stub@@YAXPBDPAX@Z
void __cdecl havok_error_stub(char const *, void *)
{
}
// @address 0x1babe0 ?havok_initialize_for_new_structure_bsp@@YAXK@Z
void havok_initialize_for_new_structure_bsp(unsigned long /*structure_bsp_mask*/)
{
    if (!g_havok_constants.batch_add_entities_disabled)
        g_havok_monitor_globals.inside_simulation_step = true;
    g_havok_monitor_globals.lock_count_4c++;
    phys_bool use_aabb_caching;
    memset(&g_havok_runtime_pool_cache, -1, sizeof(g_havok_runtime_pool_cache.buckets));
    memset(g_havok_runtime_pool_cache.entries, -1, sizeof(g_havok_runtime_pool_cache.entries));
    phys_bool *use_aabb_caching_flag = &use_aabb_caching;
    g_havok_runtime_pool_cache.count = 0;
    g_havok_runtime_pool_cache.next_index = 0;
    g_havok_runtime_pool_cache.initialized = true;
    new (use_aabb_caching_flag) phys_bool(false);
    *g_havok_memory_task_data = game_time_globals->time;
    phys_bvh_agent::set_use_aabb_caching(*use_aabb_caching_flag);
    havok_memory_initialize();
    phys_system_init_verbose = false;
    memset(&g_havok_monitor_globals.base_system_object, 0, sizeof(g_havok_monitor_globals.base_system_object));
    new (&g_havok_monitor_globals.base_system_object) phys_thread_memory((phys_memory *)&g_havok_memory, 0);
    phys_system::init((phys_memory *)&g_havok_memory, (phys_thread_memory *)&g_havok_monitor_globals.base_system_object, havok_error_stub, 0);
    phys_thread_memory *thread_memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    thread_memory->set_stack_area((char *)&g_havok_memory + 2608, 0x32000);
    phys_singleton<phys_error>::replace_singleton_instance((phys_error *)&g_hk_error);
    g_havok_monitor_globals.structure_bsp_state = 0;
    c_entity_listener *entity_listener = new (g_entity_listener_storage) c_entity_listener;
    c_world_entity_listener *world_entity_listener = new (g_world_entity_listener_storage) c_world_entity_listener;
    havok_components_initialize_for_new_map_ltcg_inline(g_havok_component_data, entity_listener, world_entity_listener);
    g_havok_monitor_globals.initialized_for_new_map = true;
    havok_initialize_havok_world();
    if (!g_havok_memory_always_system)
        g_havok_memory_globals.active_allocator = _memory_allocator_runtime;
    for (long structure_bsp_index = havok_first_active_structure_bsp(); structure_bsp_index != -1;
        structure_bsp_index = havok_next_active_structure_bsp(structure_bsp_index))
        havok_build_environment_surface_mopp(structure_bsp_index);
    ((phys_thread_memory *)&g_havok_monitor_globals.base_system_object)->verify_cached_memory();
    if (g_havok_memory_globals.overflow_pool == 0)
        havok_memory_update_initialize();
    phys_rigid_body **body = g_havok_monitor_globals.rigid_bodies;
#if defined(__clang__)
    while (body < &g_havok_monitor_globals.rigid_bodies[NUMBEROF(g_havok_monitor_globals.rigid_bodies)])
#else
    while ((long)body < (long)&g_havok_monitor_globals.rigid_bodies[NUMBEROF(g_havok_monitor_globals.rigid_bodies)])
#endif
    {
        if (*body != 0)
        {
            havok_world_add_entity((phys_entity *)*body, true);
            ((phys_entity *)*body)->add_collision_listener((phys_collision_listener *)g_world_entity_listener);
        }
        body++;
    }
    havok_initialize_object_rigid_bodies();
    if (!g_havok_constants.batch_add_entities_disabled)
    {
        g_havok_monitor_globals.inside_simulation_step = g_havok_constants.batch_add_entities_disabled;
        havok_world_rebuild_all_entities();
    }
    if (g_impacts_globals->disconnected_from_physics)
        impacts_reconnect_to_physics();
    g_havok_monitor_globals.lock_count_4c--;
}

// @address 0x1bc888 ?havok_post_process_simulating_entities@@YAXXZ
void havok_post_process_simulating_entities(void)
{
    c_havok_entity_iterator iterator(g_havok_world);
    iterator.begin();
    while (iterator.next())
        impacts_post_process_havok_entity(iterator.get_entity());
    iterator.end();
    while (true)
    {
        c_entity_listener *listener = c_entity_listener::get_global_entity_listener();
        long next_index = listener->deactivated_entity_index + 1;
        bool empty = next_index >= listener->deactivated_entities.getSize();
        if (empty)
            break;
        listener->deactivated_entity_index = next_index;
        phys_entity *entity = listener->deactivated_entities.m_data[next_index];
        ((phys_world_object *)entity)->object_remove_ref();
        if ((bool)entity->is_body_active())
            continue;
        impacts_post_process_havok_entity(entity);
    }
    c_entity_listener::get_global_entity_listener()->clear_deactivated_entity_queue();
}

// @address 0x1032d8 ?havok_update@@YAXXZ
void havok_update(void)
{
    if (g_havok_world != 0)
    {
        float tick_length = game_tick_length();
        phys_bool use_aabb_caching;
        phys_bool *use_aabb_caching_flag = &use_aabb_caching;
        new (use_aabb_caching_flag) phys_bool(false);
        phys_bvh_agent::set_use_aabb_caching(*use_aabb_caching_flag);
        c_entity_listener::get_global_entity_listener()->queue_deactivated_entities = true;
        havok_memory_garbage_collect_locked();
        havok_shapes_lock_environment_null_shapes();
        g_havok_monitor_globals.update_in_progress = true;
        g_havok_world->step_delta_time(tick_length);
        g_havok_monitor_globals.update_in_progress = false;
        phys_world_op_queue *operation_queue = g_havok_world->m_pending_ops;
        phys_array<phys_world_op::biggest_op> &pending_operations = operation_queue->m_pending_ops;
        bool pending_in_pool =
            ((c_havok_memory_pool *)g_havok_memory_globals.overflow_pool)->contains(pending_operations.m_data);
        if (pending_in_pool)
        {
            if ((pending_operations.m_capacity_and_flags & (int)0x80000000) == 0)
            {
                int capacity = pending_operations.m_capacity_and_flags & 0x3fffffff;
                phys_thread_memory *thread_memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
                thread_memory->free_chunk(pending_operations.m_data,
                    capacity * sizeof(phys_world_op::biggest_op), PHYS_MEM_ARRAY);
            }
            pending_operations.m_data = 0;
            pending_operations.m_size = 0;
            pending_operations.m_capacity_and_flags = (pending_operations.m_capacity_and_flags & (int)0x40000000) | (int)0x80000000;
        }
        havok_shapes_unlock_environment_null_shapes();
        havok_post_process_simulating_entities();
        havok_simulation_island_cleanup_resources();
        havok_cull_objects();
        havok_memory_garbage_collect_locked();
        c_entity_listener::get_global_entity_listener()->queue_deactivated_entities = false;
    }
}

__forceinline void *phys_world::operator new(size_t size) throw()
{
    phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    phys_ref_object *object = (phys_ref_object *)memory->allocate_chunk((int)size, (phys_memory_class)45);
    object->m_mem_size_and_flags = (unsigned short)size;
    return object;
}
struct phys_collider_added_event;
struct phys_collider_removed_event;

class phys_group_filter;

// @rdata 0x82021694
static const float havok_maximum_contact_resting_velocity = 3.402823466e+38f;

static __forceinline const s_havok_constants *havok_initialize_constants_get()
{
    return &g_havok_constants;
}

// @address 0x1bb300
void havok_initialize_havok_world()
{
    phys_group_filter *filter = havok_initialize_collision_filter();
    phys_world_desc info;
    info.m_gravity.set_zero4();
    phys_vec4 margin;
    margin.set(64.0f, 64.0f, 64.0f, 0.0f);
    info.m_broadphase_bounds.m_min_limit.set(-2000.0f, -2000.0f, -2000.0f, 0.0f);
    info.m_broadphase_bounds.m_max_limit.set(2000.0f, 2000.0f, 2000.0f, 0.0f);
    info.m_broadphase_bounds.m_min_limit.sub4(margin);
    info.m_broadphase_bounds.m_max_limit.add4(margin);
    info.setup_solver_info(phys_world_desc::SOLVER_PRESET_8ITER_SOFT);
    info.m_collision_tolerance = havok_initialize_constants_get()->minimum_radius;
    info.m_expected_max_lin_vel = havok_initialize_constants_get()->expected_max_linear_velocity;
    info.m_iter_cast_early_out_dist = havok_initialize_constants_get()->linear_cast_early_out_distance;
    info.m_wake_on_body_move = false;
    info.m_simulation_type = phys_world_desc::SIMULATION_TYPE_CONTINUOUS;
    info.m_contact_point_gen = phys_world_desc::CONTACT_POINT_REJECT_MANY;
    info.m_border_behaviour = phys_world_desc::BROADPHASE_BORDER_DO_NOTHING;
    info.m_expected_min_psi_dt = game_tick_length();
    info.m_contact_resting_vel = havok_maximum_contact_resting_velocity;
    info.m_legacy_edge_weld = havok_initialize_constants_get()->weld_environment;
    info.m_sleep_ref_distance = havok_initialize_constants_get()->deactivation_reference_distance;
    g_havok_world = new phys_world(info, 0x9e35);
    {
        phys_bool enabled;
        phys_bool *enabled_flag = &enabled;
        new (enabled_flag) phys_bool(false);
        phys_singleton<phys_error>::s_instance->error_set_enabled((int)0xad000061, *enabled_flag);
    }
    g_havok_world->lock();
    c_broad_phase_border *broad_phase_border = new c_broad_phase_border(g_havok_world);
    g_havok_world->set_broadphase_border(broad_phase_border);
    broad_phase_border->object_remove_ref();
    phys_axis_sweep *broad_phase = (phys_axis_sweep *)g_havok_world->m_broadphase;
    broad_phase->m_nodes.reserve(2048);
    for (long axis_index = 0; axis_index < 3; axis_index++)
        broad_phase->m_axis[axis_index].m_sweep_endpoints.reserve(4096);
    havok_entity_set_havok_component_index(g_havok_world->m_fixed_body, -1);
    {
        phys_bool enabled;
        phys_bool *enabled_flag = &enabled;
        new (enabled_flag) phys_bool(true);
        g_havok_world->set_collision_filter((phys_collision_filter *)filter, *enabled_flag, PHYS_UPDATE_FILTER_ON_WORLD_FULL_CHECK,
            PHYS_UPDATE_FILTER_PROCESS_COMPOUND_SHAPES);
    }
    ((phys_ref_object *)filter)->object_remove_ref();
    havok_initialize_collision_dispatcher(g_havok_world->m_collision_dispatcher);
}


static __forceinline c_havok_contact_point *havok_impact_get_contact_point(const c_havok_contact_point_iterator &iterator)
{
    // Look up the contact point addressed by the iterator.
    return (c_havok_contact_point *)&((c_havok_component *)(((s_data_array *)g_havok_component_data)->data +
        DATUM_INDEX_TO_ABSOLUTE_INDEX(
            ((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(iterator.object_index)].datum->havok_component) * sizeof(c_havok_component)))
        ->m_rigid_bodies.m_data[iterator.rigid_body_index].m_contact_points[iterator.contact_point_index];
}

/* The unreferenced-delete and impulse-effect thresholds share the same value
   but are kept as separate constants. */
// @rdata 0x8203385c distinct
static const float k_impact_unreferenced_delete_seconds = 0.2f;
// @rdata 0x8203385c distinct
static const float k_impact_impulse_effect_recent_seconds = 0.2f;
static float k_impact_managing_effect_score_scale = 1.5f;

static __forceinline long havok_impact_game_seconds_to_ticks_round(float seconds)
{
    return (long)((float)game_time_globals->ticks_per_second * seconds + 0.5f);
}
static __forceinline float havok_impact_game_ticks_to_seconds(float ticks)
{
    return ticks * game_time_globals->tick_length;
}
static __forceinline float havok_impact_unreferenced_seconds(const c_impact *impact)
{
    float seconds = game_time_globals->tick_length;
    seconds *= (float)impact->unreferenced_ticks;
    // The saturating age starts at zero; initialized game time has a positive tick period.
    // match: 0x112d58: non-negative range keeps f0/f13 colors on the seconds-vs-threshold fcmpu at +0x390; without it the two colors swap (3 words)
    __assume(seconds >= 0.0f);
    return seconds;
}
static __forceinline bool havok_impact_recently_played_impulse_effect(const c_impact *impact)
{
    return impact->queued_tick != -1 &&
        game_time_globals->time - impact->queued_tick <= havok_impact_game_seconds_to_ticks_round(k_impact_impulse_effect_recent_seconds);
}
static __forceinline void havok_impact_update_score(c_impact *impact)
{
    impact->score = c_impact::calculate_score(impact->get_world_space_position(), (c_impact::e_states)impact->activation_state);
    if (impact->is_managing_effect())
        impact->score *= k_impact_managing_effect_score_scale;
}

// @address 0x112d58 ?havok_entity_post_update@@YAXXZ
void havok_entity_post_update(void)
{
    impacts_create_orphaned_impacts();
    {
        c_data_iterator<c_impact> iterator;
        iterator.begin((s_data_array *)TLS_IMPACT_DATA);
        while (iterator.next())
        {
            c_impact *impact = iterator.get_datum();
            object_datum *object = object_get(impact->get_object_index_a());
            if (object->havok_component != -1)
            {
                c_havok_component *component = havok_velocity_component_get(object->havok_component);
                for (c_havok_contact_point_iterator contact_iterator(component->m_object_index, false); contact_iterator.valid(); contact_iterator.next())
                {
                    c_havok_contact_point *contact_point = havok_impact_get_contact_point(contact_iterator);
                    if (contact_point->get_impact_datum_index() == iterator.get_index())
                    {
                        c_global_material_type material_a = contact_point->calculate_global_material_type_a();
                        c_global_material_type material_b = contact_point->calculate_global_material_type_b();
                        if (!(material_a == impact->get_global_material_index_a()) || !(material_b == impact->get_global_material_index_b()))
                        {
                            impact->remove_reference(iterator.get_index(), false);
                            contact_point->set_impact_datum_index(-1);
                            impacts_create_orphaned_impact(impact->get_object_index_a(), contact_point, true);
                        }
                    }
                }
            }
        }
    }
    {
        c_data_iterator<c_impact> iterator;
        iterator.begin((s_data_array *)TLS_IMPACT_DATA);
        while (iterator.next())
        {
            c_impact *impact = iterator.get_datum();
            object_datum *object = ((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(impact->object_index_a)].datum;
            // match: 0x112d58: value range assumption guides branch lowering
            __assume(object != 0);                          /* March ASSERT(object_a) use: keeps the header load above the branch */
            bool delete_impact = false;
            long impact_index = iterator.get_index();
            if (impact->physics_reference_count != 0 &&
                (havok_component_has_active_entity(havok_velocity_component_get(object->havok_component)) || impact->type == 3))
            {
                impact->unreferenced_ticks = 0;
                impact->update(impact_index);
            }
            else
            {
                impact->unreferenced_ticks = (signed char)(impact->unreferenced_ticks + 1 < 126 ? impact->unreferenced_ticks + 1 : 126);
                delete_impact = !((impact->is_managing_effect() || havok_impact_recently_played_impulse_effect(impact)) &&
                    havok_impact_unreferenced_seconds(impact) < k_impact_unreferenced_delete_seconds);
            }
            if (delete_impact)
            {
                impact_delete(impact_index);
            }
            else
            {
                havok_impact_update_score(impact);
            }
        }
    }
    impacts_merge();
    impacts_quicksort();
}

// @address 0x11df850 g_effect_lightprobe_memory
extern "C" {
__declspec(thread) s_effect_lightprobe *g_effect_lightprobe_memory;
}
// @address 0x74b80c g_runtime_state_allocation
