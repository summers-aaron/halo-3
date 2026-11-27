// blamlibxbox/game_engine_candy_monitor.obj: from the records 0x2729d0 0x273400 0x273510 0x2735e8 0x2736d0 0x273748 0x273898 0x273b60 0x273f40 0x274110 0x274180 0x274278 0x274348 0x2744e0 0x2745b0 0x274638 0x2747b0 0x274938 0x274a18 0x274bc8 0x274ce8 0x274e20 0x274e80 0x275018 0x2751c0 0x275298 0x2753a8 0x2754e0 0x275580
// @flags /TP /O2 /Oc
#include "game/player_with_unit_iterator.h"
#include "tag_files/tag_groups.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "game/game_time_types.h"
#include "objects/object_types.h"
#include "game/game_types.h"
#include "objects/object_iterator.h"
#include "objects/object_datum.h"
#include "items/item_types.h"
#include "units/unit_types.h"
#include "game/game_simulation_predicates.h"
#include "objects/multiplayer_object_types.h"
#include "objects/object_placement.h"
// @address 0x274110 ?initialize_for_new_map@c_multiplayer_candy_monitor_manager@@QAAXXZ
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "game/game_engine_candy_monitor_types.h"
#include "game/game_engine_sandbox.h"
#include "game/multiplayer_definitions.h"
#include "items/weapons.h"
#include "objects/object_early_movers.h"
#include "game/game_globals.h"
#include "math/matrix_math.h"
#include "math/real_math.h"
#include "objects/object_header_access.h"
#include "memory/data.h"
#include "scenario/scenario_map_variant.h"
#include "game/map_variant.h"
#include "game/player_types.h"
#include "cache/cache_files.h"
#include "objects/object_type_definition.h"
#include "game/game_engine_default.h"
#include "game/game_engine_scoring_globals.h"
#include "scenario/scenario_objects.h"
#include "scenario/scenario_tag.h"
#include "tag_files/tag_types.h"
#include "game/game_globals_storage.h"
#include "game/game_engine_list.h"
#include "memory/tls_globals.h"
#include "math/real_math_globals.h"
#include "game/universal_types.h"
#include "game/game_variant_inline.h"
#include "objects/object_definitions.h"
#include "units/unit_definitions.h"
#include "scenario/scenario_light_types.h"
#include <string.h>
#include "game/game_engine_runtime_constants.h"



// an object's identity in a scenario (unique id, origin bsp, type and source)

class c_multiplayer_candy_monitor_manager;
namespace {
static __forceinline long remap_weapon_definition(long definition_index, const s_weapon_set *weapon_set)
{
    long result = -1;
    if (definition_index != -1)
    {
        for (long index = 0; index < weapon_set->remaps.count; ++index)
        {
            const s_weapon_set_remap *remap = &((const s_weapon_set_remap *)weapon_set->remaps.address)[index];
            long original_definition = ::multiplayer_universal_data_get_weapon_selection_definition_index(remap->original_selection);
            if (original_definition == definition_index)
            {
                result = ::multiplayer_universal_data_get_weapon_selection_definition_index(remap->replacement_selection);
                break;
            }
        }
    }
    return result;
}

static __forceinline long remap_vehicle_definition(long definition_index, const s_multiplayer_vehicle_set *vehicle_set)
{
    long result = definition_index;
    if (vehicle_set && definition_index != -1)
    {
        for (long index = 0; index < vehicle_set->remaps.count; ++index)
        {
            const s_multiplayer_vehicle_set_remap *remap = &((const s_multiplayer_vehicle_set_remap *)vehicle_set->remaps.address)[index];
            long original_definition = multiplayer_universal_data_get_vehicle_selection_definition_index(remap->original_selection);
            if (original_definition == definition_index)
            {
                result = multiplayer_universal_data_get_vehicle_selection_definition_index(remap->replacement_selection);
                break;
            }
        }
    }
    return result;
}

}

long game_engine_remap_weapon_definition_index(long definition_index)
{
    long result = definition_index;
    if (current_game_variant())
    {
        short weapon_set_index = current_game_variant()->weapon_set_absolute_index;
        if (weapon_set_index != -1)
        {
            if (weapon_set_index == -2)
            {
                if (game_simulation::game_is_authoritative())
                    result = ::multiplayer_universal_data_get_random_weapon_definition_index();
            }
            else
            {
                const s_weapon_set *weapon_set =
                    (const s_weapon_set *)::multiplayer_universal_data_weapon_set_try_and_get(weapon_set_index);
                if (weapon_set)
                    result = remap_weapon_definition(definition_index, weapon_set);
            }
        }
    }
    return result;
}
// @address 0x273510 game_engine_remap_vehicle_definition_index
extern "C" long game_engine_remap_vehicle_definition_index(long definition_index)
{
    long result = definition_index;
    if (current_game_variant())
    {
        unsigned short vehicle_set_index = current_game_variant()->vehicle_set_absolute_index;
        if (vehicle_set_index != 0xffff)
        {
            const s_multiplayer_vehicle_set *vehicle_set =
                (const s_multiplayer_vehicle_set *)multiplayer_universal_data_vehicle_set_try_and_get(vehicle_set_index);
            result = remap_vehicle_definition(definition_index, vehicle_set);
        }
    }
    return result;
}

namespace {

static __forceinline bool candy_unit_seat_filled(long index, short seat)
{
    return ::unit_seat_get_occupant(index, seat) != -1;
}

static __forceinline c_game_engine *candy_monitor_current_engine()
{
    if (((s_game_engine_globals *)tls_game_engine_globals())->game_engine_index >= 1 &&
        ((s_game_engine_globals *)tls_game_engine_globals())->game_engine_index <= 10)
        return game_engines[((s_game_engine_globals *)tls_game_engine_globals())->game_engine_index];
    return 0;
}
static __forceinline c_multiplayer_candy_monitor_manager *candy_monitor_manager_get()
{
    return tls_game_engine_globals()->get_candy_monitor_manager();
}
static __forceinline long candy_monitor_tick_rate()
{
    return tls_game_time_globals()->ticks_per_second;
}
static __forceinline long candy_monitor_time_get()
{
    return tls_game_time_globals()->time;
}
}

// @address 0x2735e8 ?game_engine_register_object@@YAXJ@Z
void game_engine_register_object(long object_index)
{
    if (game_simulation::game_is_predicted())
        return;
    if (candy_monitor_current_engine())
    {
        s_object_header *header = object_header_get(object_index);
        if ((1 << header->object_type) & 0x40e)
        {
            object_datum *datum = header->datum;
            if ((unsigned short)datum->multiplayer_data_offset != 0xffff &&
                !(_rotl(*(unsigned short *)((char *)datum + (short)datum->multiplayer_data_offset), 29) & 1))
            {
                if ((unsigned short)datum->map_variant_index != 0xffff || (unsigned short)datum->scenario_datum_index != 0xffff)
                    candy_monitor_manager_get()->monitor_object(datum->map_variant_index, object_index);
            }
        }
    }
}

namespace {
static __forceinline c_map_variant *candy_runtime_map_variant()
{
    return &((s_game_engine_globals *)tls_game_engine_globals())->map_info;
}
static __forceinline const s_variant_quota *candy_runtime_object_quota(long quota_index)
{
    return &((s_game_engine_globals *)tls_game_engine_globals())->map_info.quotas[quota_index];
}
static __forceinline s_multiplayer_object_properties *candy_object_try_and_get_multiplayer(long object_index)
{
    object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, -1);
    if (object) {
        short offset = object->multiplayer_data_offset;
        if (offset != -1)
            return (s_multiplayer_object_properties *)((char *)object + offset);
    }
    return 0;
}
}
// @address 0x273748 ?initialize@c_candy_spawner@@QAAXJ@Z
void c_candy_spawner::initialize(long object_index)
{
    s_multiplayer_object_properties *properties = candy_object_try_and_get_multiplayer(object_index);
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    flags = 0;
    set_flag(0, true);
    set_flag(1, (properties->flags & 64) != 0);
    set_flag(3, properties->spawn_timer_mode == 0);
    set_flag(2, properties->spawn_timer_mode == 1);
    map_variant_index = object->map_variant_index;
    last_spawned_object_index = object_index;
    respawn_timer_seconds = 32767;
    s_variant_object_datum variant_object;
    long variant_object_index = map_variant_index;
    c_map_variant *map_variant = candy_runtime_map_variant();
    map_variant->get_variant_object_datum(variant_object_index, &variant_object);
    if (!variant_object.flags.is_empty()) {
        variant_object.flags.set(_variant_object_placement_preserve_helper_bit, true);
        map_variant->set_variant_object_datum(variant_object_index, &variant_object, false);
    }
}

__forceinline void c_candy_spawner::respawn_timer_update()
{
    short timer = get_respawn_timer_seconds();
    if (timer == 32767)
    {
        short respawn_time = (short)object_get_respawn_time_seconds(map_variant_index);
        respawn_timer_seconds = respawn_time;
        if (respawn_time == 0)
            initialize();
        if (monitoring_something())
            candy_runtime_map_variant()->remove_object(map_variant_index, _object_removal_origin_1);
    }
    else
    {
        respawn_timer_seconds = timer == -32768 ? -32768 : timer - 1;
    }
}

// @address 0x273898 ?update@c_candy_spawner@@QAAXXZ
void c_candy_spawner::update()
{
    if (monitoring_something())
    {
        c_candy_monitor *monitor = get_last_object_spawned_index() != -1
            ? candy_monitor_manager_get()->monitor_try_and_get(get_last_object_spawned_index()) : 0;
        bool disturbed = (monitor && monitor->disturbed_from_initial_placement()) ? true : false;
        bool dead =
            (monitor && ((_rotl(((object_datum *)object_get_and_verify_type(
                monitor->get_spawned_object_index(), -1))->damage_flags, 30) & 1) != 0)) ? true : false;
        bool at_rest =
            monitor && ((_rotl(((object_datum *)object_get_and_verify_type(monitor->get_spawned_object_index(), -1))->physics_flags, 23) & 1) != 0);
        bool update_timer = !monitor || get_respawn_timer_seconds() != 32767
            || (disturbed && at_rest && respawn_timer_starts_on_disturbance())
            || (dead && respawn_timer_starts_on_death());
        bool should_spawn = false;
        bool bypass_timer = false;
        bool quotas_permit = false;
        if (update_timer)
            respawn_timer_update();
        if (monitoring_something())
        {
            if (!monitor || disturbed || dead)
            {
                c_multiplayer_candy_monitor_manager *manager = candy_monitor_manager_get();
                quotas_permit = manager->quotas_permit_spawn((short)map_variant_index, &bypass_timer);
            }
            if (quotas_permit)
                should_spawn = get_respawn_timer_seconds() <= 0 || bypass_timer;
            if (should_spawn)
                candy_monitor_manager_get()->try_to_spawn_new_object(get_map_variant_index(), this);
        }
    }
}

struct s_scenario_weapon;
// @address 0x273b60 ?spawn_object@c_candy_spawner@@QAA_N_N@Z
bool c_candy_spawner::spawn_object(bool)
{
    bool result = false;
    long definition_index = -1;
    object_placement_data placement;
    e_object_type object_type = _object_type_none;
    long scenario_index = -1;
    s_variant_object_datum variant_object;
    scenario *scenario_data = global_scenario;
    if (candy_runtime_map_variant()->get_variant_object_datum(map_variant_index, &variant_object))
    {
        if (candy_runtime_map_variant()->get_object_scenario_properties(map_variant_index, &object_type, &scenario_index))
        {
            const s_scenario_object *scenario_object =
                scenario_object_try_and_get(scenario_data, object_type, scenario_index);
            if (object_placement_data_new_from_scenario_object(
                    object_type, scenario_index, scenario_object,
                    (s_tag_block *)((char *)scenario_data +
                                    object_type_definitions[object_type]->scenario_palette_offset),
                    true, false, &placement))
            {
                definition_index = placement.definition_index;
                if (object_type == _object_type_equipment &&
                    game_get_grenade_type_index_from_item_defintion(definition_index) != -1)
                {
                    if ((current_game_variant()->variant_flags & 1) == 0)
                        definition_index = -1;
                }
            }
        }
        else
        {
            const s_variant_quota *quota = candy_runtime_object_quota(variant_object.quota_index);
            definition_index = quota->object_definition_index;
            object_placement_data_new(&placement, definition_index, -1, 0);
        }
        map_variant_get_object_transform(candy_runtime_map_variant(), map_variant_index, &placement.position, &placement.forward, &placement.up);
        if (definition_index != -1)
        {
            bool clear = true;
            s_object_definition *blocker =
                (s_object_definition *)tag_get('obje', definition_index);
            if (blocker->object_type == 1)
            {
                real_point3d center;
                center.x = blocker->bounding_offset.x + placement.position.x;
                center.y = blocker->bounding_offset.y + placement.position.y;
                center.z = blocker->bounding_offset.z + placement.position.z;
                s_location location;
                scenario_location_from_point(&location, &center);
                long object_indices[16];
                short count = objects_in_sphere(1, 1, &location, &center, blocker->bounding_radius, object_indices, 16);
                for (long i = 0; i < count; i++)
                {
                    unit_datum *object =
                        (unit_datum *)((object_datum *)object_get_and_verify_type(object_indices[i], 8195));
                    if (object->unit.type_change_time == -1)
                    {
                        clear = false;
                        break;
                    }
                }
            }
            if (clear)
            {
                long object_index = object_new(&placement);
                if (object_index != -1)
                {
                    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
                    object_set_at_rest(object_index, false);
                    s_object_header *header = object_header_get(object_index);
                    if (((1 << header->object_type) & 4) != 0)
                        object_placement_update_multiplayer_item_flags(object_index);
                    s_multiplayer_object_properties *multiplayer =
                        (s_multiplayer_object_properties *)((char *)object + (short)object->multiplayer_data_offset);
                    if (object_type == _object_type_weapon && scenario_index != -1)
                    {
                        const s_scenario_object *scenario_weapon =
                            scenario_object_try_and_get(scenario_data, _object_type_weapon, scenario_index);
                        weapon_set_initial_rounds_from_scenario_weapon(object_index,
                                                                       (const s_scenario_weapon *)scenario_weapon);
                    }
                    flags = 1;
                    switch ((signed char)multiplayer->spawn_timer_mode)
                    {
                    case 0:
                        flags |= 8;
                        break;
                    case 1:
                        flags |= 4;
                        break;
                    }
                    last_spawned_object_index = object_index;
                    c_map_variant::find_definition_index(object_index, map_variant_index, 1);
                    candy_runtime_map_variant()->apply_multiplayer_properties(map_variant_index, object_index);
                    c_map_variant *map_variant = candy_runtime_map_variant();
                    map_variant->get_variant_object_datum(map_variant_index, &variant_object);
                    variant_object.flags.set(_variant_object_placement_preserve_helper_bit, true);
                    map_variant->set_variant_object_datum(map_variant_index, &variant_object, false);
                    game_engine_register_object(object_index);
                    object->flags_d6 |= 4;
                    simulation_action_object_create(object_index);
                    respawn_timer_seconds = 32767;
                    result = true;
                }
                else
                {
                    respawn_timer_seconds = 10;
                }
            }
            else
            {
                respawn_timer_seconds = 10;
            }
        }
    }
    return result;
}

// @address 0x2736d0 game_engine_candy_monitor_delete_entry
extern "C" void game_engine_candy_monitor_delete_entry(int object_name)
{
    if (object_name == -1)
        return;
    long index = 0;
    c_candy_spawner *entries = candy_monitor_manager_get()->spawners;
    for (; index < 512; ++index)
    {
        if ((short)entries[index].map_variant_index == (short)object_name)
        {
            c_candy_spawner *entry = &entries[index];
            if (entry)
            {
                entry->flags = 0;
                entry->respawn_timer_seconds = 0;
                entry->map_variant_index = -1;
                entry->last_spawned_object_index = -1;
            }
            return;
        }
    }
}

void c_multiplayer_candy_monitor_manager::initialize_for_new_map()
{
    for (long index = 0; index < 512; index++)
        spawners[index].initialize();
    for (long index = 0; index < 512; index++)
        monitors[index].initialize(-1);
}

// @address 0x274180 ?dispose_from_old_round@c_multiplayer_candy_monitor_manager@@QAAXXZ
void c_multiplayer_candy_monitor_manager::dispose_from_old_round()
{
    if (game_simulation::game_is_authoritative())
    {
        bool reset = (current_game_variant()->flags & 4) != 0;
        if (reset)
        {
            for (long i = 0; i < 512; i++)
            {
                long object_index = monitors[i].object_index;
                if (object_index != -1)
                    object_delete(object_index);
                volatile c_candy_monitor &monitor = monitors[i];
                monitor.initialize(-1);
            }
            for (long i = 0; i < 512; i++)
                spawners[i].initialize();
        }
    }
    else
        initialize_for_new_map();
}

// @address 0x274278 ?update@c_multiplayer_candy_monitor_manager@@QAAXXZ
void c_multiplayer_candy_monitor_manager::update()
{
    if (game_simulation::game_is_authoritative())
    {
        long tick_rate = candy_monitor_tick_rate();
        long time = candy_monitor_time_get();
        for (long index = 0; index < 512; ++index)
        {
            if (index % tick_rate == time % tick_rate)
                spawners[index].update();
        }
        for (long index = 0; index < 512; ++index)
        {
            if (index % tick_rate == time % tick_rate)
                monitors[index].update();
        }
    }
}

void c_multiplayer_candy_monitor_manager::detach_object(long object_index)
{
    if (game_simulation::game_is_authoritative() && object_index != -1) {
        c_candy_monitor *monitor = monitor_try_and_get(object_index);
        if (monitor)
            monitor->initialize(-1);
        for (long index = 0; index < 512; ++index) {
            if (spawners[index].get_last_object_spawned_index() == object_index)
                spawners[index].set_last_spawned_object_index(-1);
        }
    }
}

// @address 0x2745b0 ?get_candy_monitor_object_definition_index@c_multiplayer_candy_monitor_manager@@SAJF@Z
long c_multiplayer_candy_monitor_manager::get_candy_monitor_object_definition_index(short object_index)
{
    long definition_index = -1;
    s_variant_object_datum variant_object;
    candy_runtime_map_variant()->get_variant_object_datum(object_index, &variant_object);
    if (variant_object.quota_index != -1) {
        const s_variant_quota *quota = candy_runtime_object_quota(variant_object.quota_index);
        definition_index = quota->object_definition_index;
    }
    return definition_index;
}

// @address 0x274638 game_engine_candy_monitor_count_quota_objects
extern "C" long game_engine_candy_monitor_count_quota_objects(c_multiplayer_candy_monitor_manager *manager, long quota_index, long definition_index)
{
    long count = 0;
    if (definition_index != -1)
    {
        s_object_definition *definition = (s_object_definition *)tag_get('obje', definition_index);
        long object_type = definition->object_type;
        if ((unsigned long)object_type <= 13)
        {
            c_object_iterator<object_datum> iterator;
            iterator.begin(1 << object_type, 0);
            while (iterator.next())
            {
                object_datum *object = (object_datum *)iterator.get_datum();
                if (definition_index == object->definition_index)
                {
                    long object_index = iterator.get_index();
                    c_candy_monitor *monitor = 0;
                    if ((unsigned short)((object_datum *)object_get_and_verify_type(object_index, 0))->multiplayer_data_offset != 0xffff && object_index != -1)
                        monitor = manager->monitor_try_and_get(object_index);
                    long counted;
                    if (monitor == 0 || !(bool)(_rotl(*(unsigned short *)((char *)object + (short)object->multiplayer_data_offset), 26) & 1))
                        counted = 1;
                    else
                        counted = (object->map_variant_index == quota_index);
                    count += counted;
                }
            }
        }
    }
    return count;
}

// @address 0x2747b0 ?quotas_permit_spawn@c_multiplayer_candy_monitor_manager@@QAA_NFPA_N@Z
bool c_multiplayer_candy_monitor_manager::quotas_permit_spawn(short variant_index, bool *bypass_timer)
{
    c_map_variant *map_variant = candy_runtime_map_variant();
    bool result = false;
    long count = 0;
    s_variant_object_datum variant_object;
    *bypass_timer = false;
    if (map_variant->get_variant_object_datum(variant_index, &variant_object))
    {
        if (variant_object.quota_index != -1)
        {
        const s_variant_quota *quota = &map_variant->quotas[variant_object.quota_index];
        count = game_engine_candy_monitor_count_quota_objects(this, variant_index, quota->object_definition_index);
        long maximum = quota->maximum_count;
        long minimum = quota->minimum_count;
        e_object_type object_type;
        long scenario_index;
        if (map_variant->get_object_scenario_properties(variant_index, &object_type, &scenario_index))
        {
            if (object_type == 1 || object_type == 2)
            {
                long placement_index = scenario_index;
                const s_scenario_multiplayer_object_properties *properties = scenario_multiplayer_object_properties_try_and_get(object_type, placement_index);
                bool use_scenario_counts;
                long scenario_minimum;
                long scenario_maximum;
                if (properties)
                {
                    use_scenario_counts = true;
                    scenario_minimum = (signed char)properties->minimum_count;
                    scenario_maximum = properties->maximum_count;
                }
                else
                {
                    { long stale_scenario_index_a; scenario_maximum = stale_scenario_index_a; }
                    use_scenario_counts = false;
                    { long stale_scenario_index_b; scenario_minimum = stale_scenario_index_b; }
                }
                if ((unsigned char)use_scenario_counts != 0)
                {
                    count = game_engine_candy_monitor_count_scenario_objects(map_variant, object_type, placement_index);
                    maximum = scenario_maximum;
                    minimum = scenario_minimum;
                }
            }
        }
        if (minimum > 0)
            *bypass_timer = count < minimum;
        if (maximum > 0)
            result = count < maximum;
        else
            result = true;
        }
    }
    return result;
}

// @address 0x274938 ?try_to_spawn_new_object@c_multiplayer_candy_monitor_manager@@QAAXFPAVc_candy_spawner@@@Z
void c_multiplayer_candy_monitor_manager::try_to_spawn_new_object(short variant_index, c_candy_spawner *spawner)
{
    c_candy_spawner *best_spawner = 0;
    short best_respawn_time = 0;
    long definition_index = get_candy_monitor_object_definition_index(variant_index);
    for (long index = 0; index < 512; index++)
    {
        c_candy_spawner *candidate = &spawners[index];
        if (candidate->monitoring_something() &&
            candidate->get_respawn_timer_seconds() < best_respawn_time &&
            get_candy_monitor_object_definition_index(candidate->get_map_variant_index()) == definition_index)
        {
            best_spawner = candidate;
            best_respawn_time = candidate->get_respawn_timer_seconds();
        }
    }
    if (!best_spawner)
    {
        if (spawner)
        {
            best_spawner = spawner;
        }
        else
        {
            for (long index = 0; index < 512; index++)
            {
                if (!spawners[index].monitoring_something())
                {
                    best_spawner = &spawners[index];
                    break;
                }
            }
        }
    }
    if (best_spawner)
        best_spawner->spawn_object(true);
}
namespace {
static __forceinline long game_engine_candy_monitor_definition_respawn_time(long definition_index)
{
    if (definition_index != -1) {
        s_object_definition *definition = (s_object_definition *)tag_get('obje', definition_index);
        if (definition && definition->multiplayer_object.count() > 0)
            return definition->multiplayer_object[0].spawn_time;
    }
    return -1;
}
static __forceinline long game_engine_candy_monitor_definition_abandonment_time(long definition_index)
{
    if (definition_index != -1) {
        s_object_definition *definition = (s_object_definition *)tag_get('obje', definition_index);
        if (definition && definition->multiplayer_object.count() > 0)
            return (short)definition->multiplayer_object[0].abandon_time;
    }
    return -1;
}
static __forceinline long game_engine_candy_monitor_scenario_abandonment_time(e_object_type type, long scenario_index)
{
    long seconds = -1;
    if (scenario_index != -1) {
        const s_scenario_multiplayer_object_properties *properties =
            (const s_scenario_multiplayer_object_properties *)scenario_multiplayer_object_properties_try_and_get(type, scenario_index);
        seconds = properties ? (short)properties->abandon_time : -1;
    }
    return seconds;
}
static __forceinline bool game_engine_candy_monitor_scenario_has_infinite_lifetime(e_object_type type, long scenario_index)
{
    const s_scenario_multiplayer_object_properties *properties =
        (const s_scenario_multiplayer_object_properties *)scenario_multiplayer_object_properties_try_and_get(type, scenario_index);
    bool infinite = properties ? (short)properties->abandon_time > 0 || ((1 << type) & 0x40e) != 0 : false;
    return infinite;
}
}
// @address 0x274a18 game_engine_candy_monitor_count_scenario_objects
extern "C" int game_engine_candy_monitor_count_scenario_objects(
    const c_map_variant *map_variant, e_object_type object_type, long placement_index)
{
    int count = 0;
    scenario *scenario_data = global_scenario;
    if (scenario_data != 0) {
        long wanted_index = scenario_object_get_palette_entry(
            scenario_data, scenario_object_try_and_get(scenario_data, object_type, placement_index))->object.index;
        c_object_iterator<object_datum> iterator;
        iterator.begin(1 << object_type, 0);
        if (iterator.next())
        {
            do {
                object_datum *object = iterator.get_datum();
                short scenario_datum_index = object->scenario_datum_index;
                if (scenario_datum_index != -1) {
                    const s_scenario_object *scenario_object = 0;
                    if ((unsigned int)object_type <= _object_type_effect_scenery) {
                        object_type_definition *type = object_type_definitions[object_type];
                        s_tag_block *datums = (s_tag_block *)((char *)scenario_data + type->scenario_datums_offset);
                        if (scenario_datum_index >= 0 && scenario_datum_index < datums->count) {
                            scenario_object = (const s_scenario_object *)((char *)datums->address
                                + type->scenario_datum_size * scenario_datum_index);
                        }
                    }
                    long palette_index = scenario_object_get_palette_entry(scenario_data, scenario_object)->object.index;
                    if (palette_index == wanted_index) {
                        count = count + 1;
                    }
                } else {
                    long variant_index = object->map_variant_index;
                    if (variant_index != -1) {
                        if ((unsigned int)variant_index <= 639) {
                            const s_variant_object_datum *variant_object = &map_variant->objects[variant_index];
                            if (variant_object != 0) {
                                long quota_index = variant_object->quota_index;
                                if (quota_index != -1) {
                                    unsigned short storage = variant_object->flags.m_storage;
                                    unsigned short object_flags = storage >> 3;
                                    bool blocked = (object_flags & 1) != 0;
                                    if ((storage & 1023) != 0 && blocked == false) {
                                        long definition_index = map_variant->quotas[quota_index].object_definition_index;
                                        if (definition_index == wanted_index) ++count;
                                    }
                                }
                            }
                        }
                    } else {
                        long definition_index = object->definition_index;
                        if (definition_index == wanted_index) ++count;
                    }
                }
            }
            while (iterator.next())
                ;
        }
    }
    return count;
}
// @address 0x274bc8 ?object_get_respawn_time_seconds@@YAJJ@Z
long object_get_respawn_time_seconds(long variant_index)
{
    c_map_variant *map_variant = candy_runtime_map_variant();
    s_variant_object_datum variant_object;
    e_object_type object_type;
    long scenario_index;
    map_variant->get_variant_object_datum(variant_index, &variant_object);
    long scenario_time = -1;
    if (map_variant->get_object_scenario_properties(variant_index, &object_type, &scenario_index)) {
        const s_scenario_multiplayer_object_properties *properties =
            (const s_scenario_multiplayer_object_properties *)scenario_multiplayer_object_properties_try_and_get(object_type, scenario_index);
        scenario_time = properties ? properties->spawn_time : -1;
    }
    long definition_index = candy_monitor_manager_get()->get_candy_monitor_object_definition_index((short)variant_index);
    long seconds = game_engine_candy_monitor_definition_respawn_time(definition_index);
    if (!variant_object.flags.is_empty())
        seconds = variant_object.properties.spawn_rate;
    else if (scenario_time > 0)
        seconds = scenario_time;
    else if (seconds <= 0)
        seconds = 0;
    return seconds;
}
// @address 0x274ce8 ?scenario_object_get_abandonment_time_seconds@@YAJJ@Z
long scenario_object_get_abandonment_time_seconds(long object_index)
{
    object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, -1));
    long scenario_index = (short)object->scenario_datum_index;
    e_object_type object_type = (e_object_type)object->object_type;
    long scenario_time = game_engine_candy_monitor_scenario_abandonment_time(object_type, scenario_index);
    long definition_time = game_engine_candy_monitor_definition_abandonment_time(object->definition_index);
    if (scenario_time > 0)
        return scenario_time;
    if (definition_time > 0)
        return definition_time;
    if (scenario_index != -1 && game_engine_candy_monitor_scenario_has_infinite_lifetime(object_type, scenario_index))
        return 32767;
    return 10;
}

// @address 0x274e20 game_engine_object_is_marked_item
extern "C" bool game_engine_object_is_marked_item(long object_index)
{
    bool result = false;
    if (object_index != -1)
    {
        object_datum *object = ((object_datum *)object_get_and_verify_type(object_index, 0));
        if ((1 << object->object_type) & 12)
        {
            if (((item_datum *)object)->owner_relation != 0)
                result = true;
        }
    }
    return result;
}

// @address 0x274e80 ?game_engine_object_has_been_disturbed@@YA_NJ@Z
bool game_engine_object_has_been_disturbed(long object_index)
{
    bool disturbed = false;
    if (object_index != -1 && ::game_engine_is_object_being_edited(object_index, (long *)(0)) == 0)
    {
        if (game_engine_sandbox_is_object_tracked(object_index) == 0)
        {
            s_object_header *header = object_header_get(object_index);
            object_datum *datum = (object_datum *)header->datum;
            if ((bool)(_rotl(datum->damage_flags, 30) & 1))
                disturbed = true;
            else if ((unsigned char)game_engine_object_is_marked_item(object_index) != 0)
                disturbed = true;
            else if ((unsigned char)game_engine_object_is_spent_weapon(object_index) != 0)
                disturbed = true;
            else
            {
                real_point3d initial;
                real_point3d origin;
                object_get_origin(object_index, &origin);
                e_object_type object_type = (e_object_type)header->object_type;
                // match: 0x274e80: restrict preserves the scenario-index and scenario-pointer load schedule.
                scenario *__restrict scenario_data;
                scenario_data = global_scenario;
                long scenario_index = datum->scenario_datum_index;
                const s_scenario_object *scenario_object = scenario_object_try_and_get(scenario_data, object_type, scenario_index);
                short variant_index = datum->map_variant_index;
                if (variant_index == -1)
                {
                    if (scenario_object)
                        initial = scenario_object->position;
                    else
                        initial = origin;
                }
                else
                {
                    s_variant_object_datum variant_object;
                    map_variant_get_object_transform(candy_runtime_map_variant(), variant_index, &initial, 0, 0);
                }
                float dx = 0.0f, dz = 0.0f, dy = 0.0f;
                dz = initial.z - origin.z;
                dx = initial.x - origin.x;
                dy = initial.y - origin.y;
                if (dy * dy + (dz * dz + dx * dx) >= game_engine_disturbance_distance_squared())
                    disturbed = true;
            }
        }
    }
    return disturbed;
}
struct s_player_datum;

namespace {
static __forceinline void game_engine_candy_monitor_player_iterator_begin(c_player_with_unit_iterator *iterator)
{
    iterator->iterator.absolute_index = -1;
    iterator->iterator.index = -1;
    iterator->iterator.data = tls_player_data();
}
}

// @address 0x275018 ?game_engine_object_has_been_abandoned@@YA_NJ@Z
bool game_engine_object_has_been_abandoned(long object_index)
{
    bool abandoned = false;
    if (object_index != -1)
    {
        object_datum *datum = object_get(object_index);
        if (datum->parent_object_index == -1)
        {
            bool respawn_elapsed = (datum->damage_flags >> 2) & 1;
            bool hidden = ::object_is_hidden(object_index);
            bool at_rest = (datum->physics_flags >> 9) & 1;
            int nearby_result;
            if ((unsigned char)game_engine_player_is_nearby(&datum->bounding_center, 3.0f) != 0)
                nearby_result = 1;
            else
                nearby_result = game_engine_players_nearby_and_looking_at_sphere(&datum->bounding_center, datum->bounding_radius, 8.0f);
            unsigned char object_type = object_header_get(object_index)->object_type;
            unsigned int vehicle_check;
            switch (object_type)
            {
            case _object_type_biped:
                object_type = (unsigned int)respawn_elapsed != 0 || ((unit_datum *)datum)->unit.player_index == -1;
                if (object_type != 0)
                    abandoned = true;
                break;
            case _object_type_vehicle:
                if ((unsigned int)at_rest != 0)
                {
                    vehicle_check = (unsigned char)::game_engine_vehicle_has_passengers(object_index);
                    if (vehicle_check == 0)
                    {
                        if ((unsigned int)(nearby_result & 255) == 0)
                            abandoned = true;
                    }
                }
                break;
            case _object_type_weapon:
            case _object_type_equipment:
                vehicle_check = (unsigned char)hidden;
                // match: 0x275018: The reference keeps hidden in r25 and respawn_elapsed in r24.
                if (const unsigned int &is_hidden = vehicle_check)
                    break;
                if ((unsigned int)(nearby_result & 255) == 0)
                    abandoned = true;
                break;
            case _object_type_scenery:
            case _object_type_crate:
                if ((unsigned int)(nearby_result & 255) == 0)
                    abandoned = true;
                break;
            case _object_type_terminal:
            case _object_type_projectile:
            case _object_type_machine:
            case _object_type_control:
            case _object_type_sound_scenery:
            case _object_type_creature:
            case _object_type_giant:
            case _object_type_effect_scenery:
                break;
            default:
                goto L_after_switch;
            }
        L_after_switch:;
        }
        if (abandoned)
            return ::game_engine_is_object_being_edited(object_index, (long *)(0)) == 0;
    }
    return abandoned;
}
// @address 0x2751c0 ?game_engine_player_is_nearby@@YA_NPBTreal_point3d@@M@Z
bool game_engine_player_is_nearby(const real_point3d *point, float radius)
{
    bool result = false;
    c_player_with_unit_iterator iterator;
    game_engine_candy_monitor_player_iterator_begin(&iterator);
    while (iterator.next()) {
        player_datum *player = (player_datum *)iterator.datum;
        if (player->unit_index != -1) {
            object_datum *unit = ((object_datum *)object_get_and_verify_type(player->unit_index, 8195));
            float dz = 0.0f, dx = 0.0f, dy = 0.0f;
            real_point3d *position = &unit->bounding_center;
            dx = point->x - position->x;
            dz = point->z - position->z;
            dy = point->y - position->y;
            if (dy * dy + (dz * dz + dx * dx) < 9.0f) {
                result = true;
                break;
            } else { continue; }
        } else { continue; }
    }
    return result;
}
// @address 0x275298 ?game_engine_any_player_nearby_and_looking_at_sphere@@YA_NPBTreal_point3d@@M@Z
bool game_engine_any_player_nearby_and_looking_at_sphere(const real_point3d *point, float radius)
{
    bool result = false;
    c_player_with_unit_iterator iterator;
    game_engine_candy_monitor_player_iterator_begin(&iterator);
    while (iterator.next()) {
        player_datum *player = (player_datum *)iterator.datum;
        if (player->unit_index != -1) {
            object_datum *unit = ((object_datum *)object_get_and_verify_type(player->unit_index, 8195));
            if (distance_squared3d(&unit->bounding_center, point) < 64.0f &&
                game_engine_player_is_looking_at_sphere(iterator.iterator.index, point, radius)) {
                result = true;
                break;
            } else { continue; }
        } else { continue; }
    }
    return result;
}
// @address 0x2753a8 ?game_engine_player_is_looking_at_sphere@@YA_NJPBTreal_point3d@@M@Z
bool game_engine_player_is_looking_at_sphere(long player_index, const real_point3d *center, float radius)
{
    player_datum *player = datum_get<player_datum>(tls_player_data(), player_index);
    long unit_index = player->unit_index;
    unit_datum *unit = unit_index != -1 ? (unit_datum *)object_get(unit_index) : 0;
    bool result = false;
    if (unit) {
        real_vector3d vector = unit->unit.looking_vector;
        real_point3d position;
        unit_get_body_position(unit_index, &position);
        float distance = distance3d(&position, center);
        if (fast_vector_intersects_sphere(&position, scale_vector3d(&vector, distance, &vector), center, radius)) result = true;
    }
    return result;
}
// @address 0x2754e0 ?game_engine_vehicle_has_passengers@@YA_NJ@Z
bool game_engine_vehicle_has_passengers(long vehicle_index)
{
    object_datum *vehicle = ((object_datum *)object_get_and_verify_type(vehicle_index, 2));
    unit_definition *definition = (unit_definition *)tag_get('vehi', vehicle->definition_index);
    long seat;
    for (seat = 0; seat < definition->seats.count(); ++seat) {
        if (candy_unit_seat_filled(vehicle_index, (short)seat)) break;
    }
    return seat != definition->seats.count();
}
// @address 0x275580 game_engine_object_is_spent_weapon
extern "C" bool game_engine_object_is_spent_weapon(long object_index)
{
    bool result = false;
    s_object_header *header = object_header_get(object_index);
    if (((1 << header->object_type) & 4) != 0 && (unsigned char)weapon_can_be_fired(object_index) == 0 &&
        (unsigned char)game_engine_object_is_marked_item(object_index) == 0)
        result = true;
    return result;
}

static __forceinline bool game_engine_candy_monitor_object_in_use(long object_index)
{
    bool in_use = false;
    if (object_index != -1)
    {
        s_object_header *header = object_header_get(object_index);
        object_datum *object = (object_datum *)header->datum;
        if (object->parent_object_index != -1)
        {
            in_use = true;
        }
        else
        {
            switch ((e_object_type)header->object_type)
            {
            case 1:
                in_use = ::game_engine_vehicle_has_passengers(object_index);
                break;
            case 2:
                in_use = object_is_hidden(object_index);
                break;
            case 3:
                in_use = object_is_hidden(object_index);
                break;
            }
        }
    }
    return in_use;
}

// @address 0x273f40 ?update@c_candy_monitor@@QAAXXZ
void c_candy_monitor::update()
{
    if (object_index != -1)
    {
        long spawned_object_index = get_spawned_object_index();
        bool delete_object = false;
        bool disturbed = state && !::game_engine_is_object_being_edited(spawned_object_index, (long *)(0)) &&
            !game_engine_sandbox_is_object_tracked(spawned_object_index);
        if (!disturbed && game_engine_object_has_been_disturbed(spawned_object_index))
        {
            disturbed = true;
            state = true;
        }
        bool abandoned = ((disturbed && ::game_engine_object_has_been_abandoned(spawned_object_index)) ||
                             game_engine_object_is_spent_weapon(spawned_object_index))
            ? true
            : false;
        bool update_timer = (short)flags > 0 || abandoned;
        if (update_timer)
            flags = (short)flags == 0x7fff ? 0x7fff : (short)flags + 1;
        if (abandoned)
        {
            if ((short)flags >= abandonment_time)
                delete_object = true;
        }
        else if (game_engine_candy_monitor_object_in_use(spawned_object_index))
        {
            flags = 0;
        }
        if (delete_object)
            object_delete(object_index);
    }
}

// @address 0x2729d0 game_engine_candy_monitor_attach_variant_object

#pragma pack(push, 4)



#pragma pack(pop)

static __forceinline long object_get_attach(object_datum *object)
{
    long result = -1;
    if ((unsigned char)((object->physics_flags >> 1) & 1) || (unsigned char)((object->physics_flags >> 2) & 1))
        result = object->local_physics_object_index;
    return result;
}
extern "C" void game_engine_candy_monitor_attach_variant_object(long object_index, long unused)
{
    object_datum *object = object_get(object_index);
    if ((unsigned short)object->map_variant_index == 0xffff)
        return;
    long attach = object_get_attach(object);
    if (attach == -1) {
        long root = -1;
        long index = object_index;
        while (index != -1) {
            root = index;
            index = object_get(index)->parent_object_index;
        }
        if (root != object_index && object_early_mover_is_owner(root, false))
            attach = root;
    }
    if (attach != -1 && attach != object_index) {
        real_vector3d offset;
        real_point3d object_position;
        real_point3d attach_position;
        real_vector3d forward;
        real_vector3d up;
        s_variant_object_datum entry;
        object_get_origin(object_index, &object_position);
        object_get_origin(attach, &attach_position);
        ::object_get_orientation(object_index, &forward, &up);
        ((s_game_engine_globals *)tls_game_engine_globals())->map_info.get_variant_object_datum(object->map_variant_index, &entry);
        object_datum *parent = object_get(attach);
        offset.i = attach_position.x - object_position.x;
        offset.j = attach_position.y - object_position.y;
        offset.k = attach_position.z - object_position.z;
        real_matrix4x3 matrix;
        memcpy(&matrix, (char *)parent + parent->node_matrices_offset, sizeof(matrix));
        matrix.position = *global_origin3d;
        matrix4x3_inverse_transform_vector(&matrix, &offset, &offset);
        matrix4x3_inverse_transform_vector(&matrix, &forward, &entry.forward);
        matrix4x3_inverse_transform_vector(&matrix, &up, &entry.up);
        entry.parent_object_identifier = parent->object_identifier;
        unsigned short flags = entry.flags.m_storage | 0x100;
        entry.position.x = offset.i;
        entry.position.y = offset.j;
        entry.position.z = offset.k;
        entry.flags.m_storage = flags;
        if (game_engine_sandbox_is_object_fixed(object_index) && !(unsigned char)((object->physics_flags >> 4) & 1)) {
            if ((unsigned char)object->parent_node_index != 0xff)
                object_detach(object_index);
            object_attach_to_node_with_type(attach, object_index, 0);
            entry.flags.m_storage = flags | 0x200;
            unsigned long *f4 = &object->object_flags;
            *f4 |= 0x4000000;
            ::simulation_action_object_update(object_index, 0x800);
        }
        ((s_game_engine_globals *)tls_game_engine_globals())->map_info.set_variant_object_datum(object->map_variant_index, &entry, true);
        simulation_action_sandbox_group_update(object->map_variant_index, 2, false);
    }
}

// @address 0x274348 ?monitor_object@c_multiplayer_candy_monitor_manager@@QAAXFJ@Z
void c_multiplayer_candy_monitor_manager::monitor_object(short quota_index, long object_index)
{
    s_object_header *header = object_header_get(object_index);
    if ((1 << header->object_type) & 0x40e) {
        object_datum *object = header->datum;
        const s_multiplayer_object_properties *properties = (const s_multiplayer_object_properties *)((char *)object + (short)object->multiplayer_data_offset);
        unsigned char delete_object = (unsigned char)(_rotl(properties->flags.m_storage, 25) & 1);
        c_candy_spawner *spawner = spawner_try_and_get(quota_index);
        if (spawner && delete_object)
            delete_object = false;
        if (!spawner) {
            for (long index = 0; index < 512; index++) {
                if (!spawners[index].monitoring_something()) {
                    spawner = &spawners[index];
                    spawner->initialize(object_index);
                    break;
                }
            }
            if (!spawner && delete_object)
                delete_object = false;
        }
        c_candy_monitor *monitor = 0;
        for (long index = 0; index < 512; index++) {
            if (monitors[index].get_spawned_object_index() == -1) {
                monitor = &monitors[index];
                break;
            }
        }
        if (monitor) {
            monitor->initialize(object_index);
            if (delete_object)
                object_delete(monitor->get_spawned_object_index());
        }
    }
}
