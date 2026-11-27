// blamlibxbox/game_engine_teleporters.obj: from the records 0x26f1f8 0x26f308 0x272f80 0x29c4c8 0x29c658 0x29c820 0x29c920 0x29ca28 0x29cb40 0x29ccc0 0x29cf40 0x29d0c0 0x29d280 0x29d2d8 0x29d478 0x29d558 0x29d638 0x29d830 0x29d8d8 0x29da00 0x29dba0 0x29ddf8 0x29df00 0x29df60 0x29dff8 0x29e060 0x29e140
#include "game/game_engine_sandbox.h"
#include "camera/director.h"
#include "game/player_with_unit_iterator.h"
#include "memory/data_types.h"
#include "memory/tls_globals.h"
#include "game/game_engine_default.h"
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "objects/object_reference_types.h"
#include "game/player_types.h"
#include "game/game_globals_storage.h"
#include "objects/object_datum.h"
#include "objects/object_iterator.h"
#include "objects/multiplayer_object_types.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "game/game.h"
#include "game/players.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "game/game_engine_area_set.h"
#include "game/game_engine_spawning.h"
#include "game/game_engine_util.h"
#include "objects/multiplayer_game_objects.h"
#include "physics/collisions.h"
#include "units/bipeds.h"
#include "scenario/scenario_map_variant.h"
#include "game/game_engine_teleporters.h"
#include "game/game_engine_vip.h"
#include "math/matrix_math_types.h"
#include "physics/collision_types.h"
#include "cache/cache_files.h"
#include "game/game_engine_sandbox_engine.h"
#include "units/unit_types.h"
#include "game/game_globals.h"
#include "game/game_engine_scoring_globals.h"
#include "game/multiplayer_runtime_globals.h"
#include "game/game_engine_settings_definitions.h"
#include "game/multiplayer_constants.h"
#include "math/real_math.h"
#include "physics/havok_shapes_math_queries.h"
#include "game/game_engine_area_types.h"
#include "physics/collision_feature_types.h"
#include "game/game_engine_teleporter_types.h"
#include "math/real_constant_pool.h"
#include "units/unit_facing.h"
#include "game/game_engine_event_data.h"
#include "animations/animation_definition_storage.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "math/random_math_range.h"

// @flags /TP /O2 /Oc

enum e_director_perspective;
class c_teleporter_area;



struct s_game_engine_globals;




// @rdata 0x8202251c 4
static const float k_teleporter_zero = 0.0f;
// @rdata 0x820221fc 4
static const float k_teleporter_one = 1.0f;





namespace {
class c_teleporter_object_iterator : public c_object_iterator_base {
public:
    __forceinline void begin(unsigned long type_mask, unsigned char header_mask, unsigned long match_flags)
    {
        object_iterator_begin_internal(type_mask, header_mask, match_flags);
    }
    __forceinline bool next() { return object_iterator_next_with_match_flags_internal(); }
};
}

struct c_teleporter_receiver_stack
{
    long count;
    c_teleporter_area *elements[32];
    __forceinline c_teleporter_receiver_stack() : count(0) {}
    __forceinline void push_back(c_teleporter_area *const &element) { long index = count++; elements[index] = element; }
};

// match: 0x29dff8: the area count must be read again between the emptiness test and traversal.
struct s_teleporter_list { c_teleporter_area entries[32]; volatile int count; };

static __forceinline unit_datum *teleporter_get_object_datum(long index)
{
    return (unit_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(index)].datum;
}
static __forceinline s_object_header *teleporter_get_object_header(long index)
{
    return datum_get<s_object_header>(object_header_data, index);
}
static __forceinline s_multiplayer_object_properties *teleporter_try_and_get_multiplayer(long index)
{
    unit_datum *object = (unit_datum *)object_try_and_get_and_verify_type(index, ~0UL);
    return object &&
            object->object.multiplayer_data_offset != -1 ? (s_multiplayer_object_properties *)((char *)object + object->object.multiplayer_data_offset)
        : 0;
}
static __forceinline player_datum *teleporter_get_player_datum(long index)
{
    return datum_get<player_datum>(player_data, index);
}

class c_teleporter_area;
static __forceinline unit_datum *teleporter_get_area_object_datum(c_teleporter_area *area);

__forceinline const s_multiplayer_object_properties *c_teleporter_area::get_multiplayer_properties() const
{
    unit_datum *object = teleporter_get_object_datum(m_object_index);
    return (const s_multiplayer_object_properties *)((char *)object + object->object.multiplayer_data_offset);
}
static __forceinline unit_datum *teleporter_get_area_object_datum(c_teleporter_area *area)
{
    return teleporter_get_object_datum(area->m_object_index);
}

static __forceinline s_multiplayer_object_properties *teleporter_entry_get_multiplayer(const c_teleporter_area *entry)
{
    unit_datum *object = teleporter_get_object_datum(entry->m_object_index);
    return (s_multiplayer_object_properties *)((char *)object + object->object.multiplayer_data_offset);
}
static __forceinline long teleporter_seconds_to_ticks_round(float seconds)
{
    return (long)(seconds * (float)TLS_GAME_TIME_GLOBALS->ticks_per_second + 0.5f);
}

// @address 0x26f1f8 teleporter_update_players
extern "C" unsigned short teleporter_update_players(unsigned short player_mask)
{
    unsigned short result = 0;
    c_player_with_unit_iterator it;
    it.iterator.data = player_data;
    it.iterator.absolute_index = -1;
    it.iterator.index = -1;
    while (it.next()) {
        long index = it.iterator.index;
        long bit = 1 << (short)index;
        bool desired = (player_mask & bit) != 0;
        if (desired && !game_engine_sandbox_is_player_in_edit_mode(index)) {
            bool success = game_engine_sandbox_spawn_monitor_for_player(index);
            if (success) result |= bit;
            else result &= ~bit;
        } else if (!desired && game_engine_sandbox_is_player_in_edit_mode(index)) {
            bool success = game_engine_sandbox_exit_edit_mode(index);
            if (success) result |= bit;
            else result &= ~bit;
        }
    }
    return result;
}

// @address 0x22b14 literal_0_75f
static __forceinline unsigned __int64 teleporter_pack_collision_test_flags(s_collision_test_flags flags)
{
    unsigned __int64 packed;
    memcpy(&packed, &flags, sizeof(packed));
    return packed;
}
// @address 0x29d830 teleporter_area_is_blocked
extern "C" unsigned char teleporter_area_is_blocked(c_teleporter_area *area, long unused)
{
    bool result = false;
    s_collision_test_flags flags;
    flags.collision_flags = 0x200;
    flags.object_flags = 7;
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x29d830: 0.5f (+0x2ac) and 0.75f load off one 0x82022b14 base; spelled literals move the flag stores after the float loads
    float height = literal_0_75f[171];
    real_point3d center = area->m_boundary_geometry.matrix.position;
    collision_feature_list list;
    if (collision_get_features_in_sphere(teleporter_pack_collision_test_flags(flags), &center, area->m_boundary_geometry.radius * height, literal_0_75f[0],
            height, area->m_object_index, -1, &list))
        result = 1;
#else
    float height = 0.5f;
    real_point3d center = area->m_boundary_geometry.matrix.position;
    collision_feature_list list;
    if (collision_get_features_in_sphere(teleporter_pack_collision_test_flags(flags), &center, area->m_boundary_geometry.radius * height, 0.75f,
            height, area->m_object_index, -1, &list))
        result = 1;
#endif
    return result;
}


class c_game_engine;
class c_map_variant;

static __forceinline s_game_engine_globals *teleporter_sandbox_globals()
{
    return (s_game_engine_globals *)tls_game_engine_globals();
}

// @address 0x26f308 ?teleporter_update@@YAXPAVc_teleporter_area@@JJ@Z
void teleporter_update(class c_teleporter_area * _sym_p0, long player_index, long mode)
{
    long absolute_index = datum_index_to_absolute_index(player_index);
    unsigned short flags = teleporter_sandbox_globals()->sandbox.active_player_flags;
    if (!game_engine_sandbox_is_player_in_edit_mode(player_index) != mode)
    {
        if (mode == 0)
            flags |= (1 << absolute_index);
        else
            flags &= ~(1 << absolute_index);
        if (teleporter_update_players(flags) & (1 << absolute_index))
        {
            teleporter_sandbox_globals()->sandbox.active_player_flags = flags;
            simulation_action_game_engine_globals_update(0x40);
            if (mode == 1)
            {
                if (player_index != -1)
                {
                    long perspective = -1;
                    for (;;)
                    {
                        if (player_index != -1)
                            perspective = ((long)::director_get_perspective(player_index, perspective));
                        if (perspective == -1)
                            break;
                        ((c_game_engine *)_sym_p0)->player_hump(perspective);
                    }
                }
                c_sandbox_engine::player_unlock_object(player_index, k_teleporter_zero, 0);
                map_variant_set_placement_marker_visibility(&teleporter_sandbox_globals()->map_info, false);
            }
            else
            {
                teleporter_sandbox_globals()->sandbox.edit_times[absolute_index] = g_sandbox_default_edit_distance;
                map_variant_set_placement_marker_visibility((c_map_variant *)&teleporter_sandbox_globals()->map_info, true);
            }
        }
    }
}
namespace {
static __forceinline s_multiplayer_object_properties *teleporter_try_multiplayer_inline(long object_index)
{
    s_data_array *data = object_header_data;
    s_multiplayer_object_properties *result = 0;
    if (object_index != -1) {
        int absolute_index = (unsigned short)object_index;
        if (absolute_index < data->first_unallocated) {
            s_object_header *header = (s_object_header *)((char *)data->data + data->size * absolute_index);
            int identifier = header->identifier;
            if (identifier != 0 && identifier == (object_index >> 16)
                && (1 << header->object_type) != 0) {
                unit_datum *object = (unit_datum *)header->datum;
                if (object && object->object.multiplayer_data_offset != -1)
                    result = (s_multiplayer_object_properties *)((char *)object + object->object.multiplayer_data_offset);
            }
        }
    }
    return result;
}
}
// @address 0x272f80 game_engine_sandbox_exit_edit_mode
extern "C" bool game_engine_sandbox_exit_edit_mode(long player_index)
{
    player_datum *player = teleporter_get_player_datum(player_index);
    long sender_object = game_engine_sandbox_find_player_original_unit(player_index);
    if (sender_object != -1)
    {
        unit_datum *sender_datum = teleporter_get_object_datum(sender_object);
        sender_datum->unit.map_editor_flags.set(_unit_map_editor_helper_flag_0, false);
        simulation_action_object_update(sender_object, 0x40000000);
        object_activate(sender_object);
        object_set_hidden(sender_object, false);
        object_reconnect_to_physics(sender_object);
    }
    long unit_index = player->unit_index;
    real_vector3d forward;
    real_vector3d up;
    real_point3d camera_position;
    object_get_orientation(unit_index, &forward, &up);
    unit_get_camera_position(unit_index, &camera_position);
    unit_datum *teleported_unit = teleporter_get_object_datum(unit_index);
    float saved_fc = teleported_unit->object.shield_vitality;
    float saved_f8 = teleported_unit->object.body_vitality;
    bool connected = (teleported_unit->object.physics_flags >> 7) & 1;
    if (connected)
        object_disconnect_from_physics(unit_index);
    object_delete(unit_index);
    player_set_unit_index(player_index, sender_object);
    if (sender_object != -1)
    {
        real_point3d origin;
        real_point3d camera2;
        object_get_origin(sender_object, &origin);
        unit_get_camera_position(sender_object, &camera2);
        real_vector3d offset;
        offset.i = origin.x - camera2.x;
        offset.j = origin.y - camera2.y;
        offset.k = origin.z - camera2.z;
        origin.x = camera_position.x + offset.i;
        origin.y = camera_position.y + offset.j;
        origin.z = camera_position.z + offset.k;
        biped_fix_position(sender_object, sender_object, &camera_position, &origin, 0, 0.0f, 1.0f, true, false);
        object_get_origin(sender_object, &origin);
        object_set_position_direct(sender_object, &origin, &forward, &up, 0, false);
        unit_set_facing(sender_object, &forward, &forward);
        simulation_action_object_update(sender_object, 6);
        player_increment_teleport_counter((unsigned long)player_index);
        player_set_facing(player_index, &forward);
        simulation_action_game_engine_player_update((short)player_index, 0x10000);
        bool skip_restore = false;
        if (game_globals->options.game_simulation == 4)
            skip_restore = true;
        if (!skip_restore)
        {
            unit_datum *restore_datum = teleporter_get_object_datum(sender_object);
            restore_datum->object.shield_vitality = saved_fc;
            restore_datum->object.body_vitality = saved_f8;
            simulation_action_object_update(sender_object, 0xc0);
        }
    }
    return true;
}
// @address 0x29c920 teleporter_clear_object_area_flags
extern "C" void teleporter_clear_object_area_flags()
{
    c_teleporter_object_iterator objects;
    objects.begin(2, 0, 0);
    while (objects.next()) {
        long object_index = objects.get_index();
        s_multiplayer_object_properties *properties = teleporter_try_multiplayer_inline(object_index);
        if (properties)
            properties->flags.set(_multiplayer_object_in_teleporter, false);
    }
}
// @address 0x29ca28 teleporter_clear_expired_teleport_flags
extern "C" void teleporter_clear_expired_teleport_flags()
{
    c_teleporter_object_iterator objects;
    objects.begin(2, 0, 0);
    while (objects.next()) {
        long object_index = objects.get_index();
        s_multiplayer_object_properties *properties = teleporter_try_multiplayer_inline(object_index);
        if (properties && !(_rotl(properties->flags, 23) & 1))
            properties->flags.set(_multiplayer_object_teleported, false);
    }
}

// @address 0x29cb40 teleporter_update_area
extern "C" void teleporter_update_area(c_teleporter_area *area)
{
    if (area->update_countdown == 0) {
        area->active = 0;
        if (area->can_teleport_players())
            teleporter_update_players_in_area(area);
        bool skip_teleporter_updates = false;
        if (game_globals->options.game_simulation == 4)
            skip_teleporter_updates = true;
        if (!skip_teleporter_updates) {
            const c_flags_no_init<e_multiplayer_object_boundary_passability, unsigned char, 5> passability = area->get_multiplayer_properties()->passability;
            if (passability.test(_multiplayer_object_boundary_passability_light_land_vehicles) ||
            passability.test(_multiplayer_object_boundary_passability_heavy_land_vehicles) ||
            passability.test(_multiplayer_object_boundary_passability_flying_vehicles))
                teleporter_update_vehicles(area);
            if (area->can_teleport_projectiles())
                teleporter_update_projectiles(area);
        }
    } else {
        area->update_countdown -= 1;
    }
    if (area->active) {
        if (area->refresh_countdown == 0) {
            area->refresh_countdown = TLS_GAME_TIME_GLOBALS->ticks_per_second << 2;
            return;
        }
        area->refresh_countdown -= 1;
        return;
    }
    area->refresh_countdown = 0;
}

// @rdata 0x82022dc0
static const float game_engine_teleporters_player_boundary_expansion = 0.5f;

// @address 0x29ccc0 teleporter_update_players_in_area
extern "C" void teleporter_update_players_in_area(c_teleporter_area *area)
{
    unit_datum *object = teleporter_get_area_object_datum(area);
    bool update = ((s_multiplayer_object_properties *)((char *)object + object->object.multiplayer_data_offset))->object_type.get() == 9 ||
        ((s_multiplayer_object_properties *)((char *)object + object->object.multiplayer_data_offset))->object_type.get() == 10;
    if (update)
    {
        float half = game_engine_teleporters_player_boundary_expansion;
        float one = 1.0f;
        c_player_with_unit_iterator iterator;
        iterator.iterator.absolute_index = -1;
        iterator.iterator.index = -1;
        iterator.iterator.data = player_data;
        while (iterator.next())
        {
            player_datum *player = (player_datum *)iterator.datum;
            if (player->unit_index == -1)
                continue;
            unit_datum *unit = teleporter_get_object_datum(player->unit_index);
            if (!unit || unit->object.parent != -1)
                continue;
            long player_index = iterator.iterator.index;
            if (player_in_multiplayer_object_boundary(player_index, (const s_multiplayer_object_boundary_geometry_data *)area))
            {
                player->flags |= 1 << 8;
                if (!game_is_authoritative())
                    continue;
                area->active = 1;
                if (((unsigned long)player->flags & (1 << 9)) != 0)
                    continue;
                c_teleporter_area *receiver = teleporter_try_and_get_random_available_receiver(area, player->unit_index, 0);
                if (teleporter_teleport_object(area, receiver, player->unit_index))
                {
                    s_game_engine_event_data event;
                    event.type = 0;
                    event.event = 22;
                    event.identifier = -1;
                    event.cause_team_index = -1;
                    event.effect_player_index = -1;
                    event.effect_team_index = -1;
                    event.data = 0;
                    event.data_word = -1;
                    event.cause_player_index = player_index;
                    event.flags = player_index;
                    game_engine_send_event(&event);
                    area->active = 0;
                    half = game_engine_teleporters_player_boundary_expansion;
                }
                else
                {
                    if (area->refresh_countdown == 0 && teleporter_has_receiver_on_channel(area))
                        teleporter_send_blocked_event(player_index);
                    half = game_engine_teleporters_player_boundary_expansion;
                }
            }
            else
            {
                s_multiplayer_object_boundary_geometry_data expanded;
                memcpy(&expanded, area, sizeof(expanded));
                expanded.radius += half;
                expanded.depth += one;
                long boundary_type = expanded.type;
                expanded.neg_height += half;
                expanded.height += half;
                expanded.width += boundary_type == 3 ? one : half;
                if (!player_in_multiplayer_object_boundary(player_index, &expanded))
                    continue;
                player->flags |= 1 << 8;
            }
        }
    }
}


void teleporter_update_projectiles(c_teleporter_area *area)
{
    s_multiplayer_object_properties *area_properties = (s_multiplayer_object_properties *)area->get_multiplayer_properties();
    bool update = area_properties->object_type.get() == 9 || area_properties->object_type.get() == 10;
    if (update)
    {
        c_teleporter_object_iterator objects;
        objects.begin(40, 0, 0x20000);
        while (objects.next())
        {
            long object_index = objects.get_index();
            s_multiplayer_object_properties *multiplayer = teleporter_try_and_get_multiplayer(object_index);
            if (!multiplayer)
                continue;
            real_point3d origin;
            object_get_origin(object_index, &origin);
            if (!point_in_multiplayer_object_boundary(&origin, (const s_multiplayer_object_boundary_geometry_data *)area))
                continue;
            multiplayer->flags.set(_multiplayer_object_in_teleporter, true);
            area->active = 1;
            if (!(_rotl(multiplayer->flags, 22) & 1))
            {
                c_teleporter_area *receiver = ::teleporter_try_and_get_random_available_receiver(area, object_index, (long *)(0));
                area->active = teleporter_teleport_object(area, receiver, object_index) == 0;
            }
        }
    }
}

void teleporter_update_vehicles(c_teleporter_area *area)
{
    s_multiplayer_object_properties *area_properties = (s_multiplayer_object_properties *)area->get_multiplayer_properties();
    bool update = area_properties->object_type.get() == 9 || area_properties->object_type.get() == 10;
    if (update)
    {
        c_teleporter_object_iterator objects;
        objects.begin(2, 0, 0);
        while (objects.next())
        {
            long object_index = objects.get_index();
            s_multiplayer_object_properties *multiplayer = teleporter_try_and_get_multiplayer(object_index);
            if (!multiplayer)
                continue;
            unit_datum *object = teleporter_get_object_datum(object_index);
            if (object->object.parent != -1)
                continue;
            long driver_index = object->unit.driver_object_index;
            if (driver_index == -1)
                continue;
            unit_datum *driver = teleporter_get_object_datum(driver_index);
            if (driver->unit.player_index == -1)
                continue;
            real_point3d origin;
            object_get_origin(object_index, &origin);
            if (!point_in_multiplayer_object_boundary(&origin, (const s_multiplayer_object_boundary_geometry_data *)area))
                continue;
            multiplayer->flags.set(_multiplayer_object_in_teleporter, true);
            area->active = 1;
            if (!(_rotl(multiplayer->flags, 22) & 1))
            {
                c_teleporter_area *receiver = ::teleporter_try_and_get_random_available_receiver(area, object_index, (long *)(0));
                area->active = teleporter_teleport_object(area, receiver, object_index) == 0;
            }
        }
    }
}

// @address 0x29d280 teleporter_is_sender
extern "C" bool teleporter_is_sender(const c_teleporter_area *entry)
{
    s_multiplayer_object_properties *multiplayer = teleporter_entry_get_multiplayer(entry);
    return multiplayer->object_type.get() == 9 || multiplayer->object_type.get() == 10;
}



static __forceinline c_area_set<c_teleporter_area, 32> *teleporter_get_area_set()
{
    return (c_area_set<c_teleporter_area, 32> *)tls_game_engine_globals()->teleporter_slot_table;
}


void game_engine_teleporters_update()
{
    teleporter_get_area_set()->update();
    c_area_set<c_teleporter_area, 32> *areas = teleporter_get_area_set();
    bool skip_teleporters = false;
    if (game_globals->options.game_simulation == 4)
        skip_teleporters = true;
    if (!skip_teleporters)
    {
        for (c_teleporter_area *area = areas->begin(); area != areas->end(); ++area)
            ((c_area *)area)->set_selected(true);
    }
    c_player_with_unit_iterator it;
    it.iterator.absolute_index = -1;
    it.iterator.index = -1;
    it.iterator.data = player_data;
    while (it.next())
    {
        player_datum *player = (player_datum *)it.datum;
        player->flags &= ~(1 << 8);
    }
    teleporter_clear_object_area_flags();
    for (c_teleporter_area *area = teleporter_get_area_set()->begin(); area != teleporter_get_area_set()->end(); ++area)
        teleporter_update_area(area);
    it.iterator.absolute_index = -1;
    it.iterator.index = -1;
    it.iterator.data = player_data;
    while (it.next())
    {
        player_datum *player = (player_datum *)it.datum;
        // match: volatile pointer cast preserves memory access order
        const volatile player_datum *observed_player = player;
        if (!(observed_player->flags & (1 << 8)))
            player->flags &= ~(1 << 9);
    }
    teleporter_clear_expired_teleport_flags();
}

// @address 0x29d2d8 ?teleporter_try_and_get_random_available_receiver@@YAPAVc_teleporter_area@@PAV1@JPAJ@Z
class c_teleporter_area * teleporter_try_and_get_random_available_receiver(class c_teleporter_area * sender, long object_index, long * blocked_ticks)
{
    c_teleporter_area *result = 0;
    c_teleporter_receiver_stack receivers;
    for (c_teleporter_area *area = teleporter_get_area_set()->begin(); area != teleporter_get_area_set()->end(); ++area)
    {
        if (area != sender)
        {
            const s_multiplayer_object_properties *properties = (const s_multiplayer_object_properties *)area->get_multiplayer_properties();
            bool is_receiver = properties->object_type.get() == 9 || properties->object_type.get() == 11;
            if (is_receiver &&
                properties->teleporter_channel.get() ==
                    sender->get_multiplayer_properties()->teleporter_channel.get() &&
                teleporter_can_pass_object(area, object_index) && !teleporter_is_teleport_blocked(sender, area, object_index))
                receivers.push_back(area);
        }
    }
    if (receivers.count > 0)
    {
        short index = _random_range(tls_random_seed(), __FILE__, __FUNCTION__, __LINE__, 0, (short)receivers.count);
        result = receivers.elements[index];
    }
    return result;
}

// @address 0x29d478 teleporter_has_sender_on_channel
extern "C" bool teleporter_has_sender_on_channel(c_teleporter_area *area)
{
    bool result = false;
    for (c_teleporter_area *other = teleporter_get_area_set()->begin(); other != teleporter_get_area_set()->end(); ++other)
    {
        if (other != area)
        {
            const s_multiplayer_object_properties *properties = (const s_multiplayer_object_properties *)other->get_multiplayer_properties();
            bool is_sender = properties->object_type.get() == 9 || properties->object_type.get() == 10;
            if (is_sender &&
            properties->teleporter_channel.get() == ((const s_multiplayer_object_properties *)area->get_multiplayer_properties())->teleporter_channel.get())
            {
                result = true;
                break;
            }
        }
    }
    return result;
}

// @address 0x29d558 teleporter_has_receiver_on_channel
extern "C" bool teleporter_has_receiver_on_channel(c_teleporter_area *area)
{
    bool result = false;
    for (c_teleporter_area *other = teleporter_get_area_set()->begin(); other != teleporter_get_area_set()->end(); ++other)
    {
        if (other != area)
        {
            const s_multiplayer_object_properties *properties = (const s_multiplayer_object_properties *)other->get_multiplayer_properties();
            bool is_receiver = properties->object_type.get() == 9 || properties->object_type.get() == 11;
            if (is_receiver &&
            properties->teleporter_channel.get() == ((const s_multiplayer_object_properties *)area->get_multiplayer_properties())->teleporter_channel.get())
            {
                result = true;
                break;
            }
        }
    }
    return result;
}

bool teleporter_can_pass_object(c_teleporter_area *area, long object_index)
{
    unit_datum *object = teleporter_get_object_datum(object_index);
    s_multiplayer_object_properties *multiplayer = teleporter_try_and_get_multiplayer(object_index);
    bool result = false;
    if ((1 << object->object.identifier.m_type) & 4099)
    {
        if ((1 << object->object.identifier.m_type) & 2)
        {
            if (multiplayer)
            {
                switch (multiplayer->object_type.get())
                {
                case 6: result = area->can_teleport_light_land_vehicles(); break;
                case 7: result = area->can_teleport_heavy_land_vehicles(); break;
                case 8: result = area->can_teleport_flying_vehicles(); break;
                }
            }
        }
        else
        {
            unit_datum *unit = teleporter_get_object_datum(object_index);
            if (unit->unit.player_index != -1)
                result = area->can_teleport_players();
        }
    }
    else if (multiplayer)
    {
        if (multiplayer->object_type.get() == 2 || multiplayer->object_type.get() == 3)
            result = area->can_teleport_projectiles();
    }
    return result;
}

static __forceinline float teleporters_real_abs(float value)
{
    return value < 0.0f ? -value : value;
}

// @address 0x29d8d8 teleporter_compute_destination
extern "C" void teleporter_compute_destination(c_teleporter_area *sender, c_teleporter_area *receiver, long object_index, real_point3d *destination)
{
    real_point3d sender_origin;
    real_point3d object_origin;
    real_point3d receiver_origin;
    real_point3d position;
    real_vector3d forward;
    real_vector3d up;
    object_get_origin(sender->m_object_index, &sender_origin);
    object_get_origin(object_index, &object_origin);
    float object_height = object_origin.z - sender_origin.z;
    long receiver_object_index = receiver->m_object_index;
    object_get_origin(receiver_object_index, &receiver_origin);
    multiplayer_get_spawn_object_position_and_facing_vectors(receiver_object_index, &position, &forward, &up);
    real_vector3d offset;
    offset.i = position.x - receiver_origin.x;
    offset.j = position.y - receiver_origin.y;
    offset.k = position.z - receiver_origin.z;
    normalize3d(&forward);
    normalize3d(&up);
    destination->x = offset.i + position.x;
    destination->y = offset.j + position.y;
    destination->z = teleporters_real_abs(offset.k) + teleporters_real_abs(object_height) + position.z;
}

// @address 0x29da00 teleporter_is_teleport_blocked
extern "C" bool teleporter_is_teleport_blocked(c_teleporter_area *sender, c_teleporter_area *area, long object_index)
{
    bool ok;
    if (object_index == -1)
    {
        ok = teleporter_area_is_blocked(area, 0) == 0;
    }
    else
    {
        s_location location;
        real_point3d center;
        real_point3d destination;
        real_point3d origin;
        long doffset = (object_index << 4) & 0xffff0;
        long object_type = ((s_object_header *)((char *)object_header_data->data + doffset))->object_type;
        teleporter_compute_destination(sender, area, object_index, &destination);
        long type_bit = 1 << object_type;
        if ((type_bit & 1) != 0)
        {
            unit_datum *object = (unit_datum *)((s_object_header *)((char *)object_header_data->data + doffset))->datum;
            ok = game_engine_unit_can_spawn_at_location_unobstructed(object->object.definition, &destination, false);
        }
        else if ((type_bit & 2) != 0)
        {
            bool vehicle_ok = false;
            unit_datum *object = (unit_datum *)((s_object_header *)((char *)object_header_data->data + doffset))->datum;
            if (object->object.definition != -1)
            {
                scenario_location_from_point(&location, &destination);
                vehicle_ok = true;
            }
            ok = vehicle_ok;
        }
        else
        {
            ok = true;
        }
        if (ok)
        {
            object_get_center_of_mass(object_index, &center);
            object_get_origin(object_index, &origin);
            center.x += destination.x - origin.x;
            center.y += destination.y - origin.y;
            center.z += destination.z - origin.z;
            scenario_location_from_point(&location, &center);
            ok = (location.cluster_reference.bsp_index != -1) & ok;
        }
    }
    return ok == 0;
}

void teleporter_flag_object_as_having_teleported_recursive(long object_index)
{
    s_multiplayer_object_properties *multiplayer = teleporter_try_and_get_multiplayer(object_index);
    unit_datum *object = teleporter_get_object_datum(object_index);
    if (multiplayer)
    {
        multiplayer->flags.set(_multiplayer_object_in_teleporter, true);
        multiplayer->flags.set(_multiplayer_object_teleported, true);
    }
    if ((1 << object->object.identifier.m_type) & 4099)
    {
        unit_datum *unit = teleporter_get_object_datum(object_index);
        if (unit->unit.player_index != -1)
        {
            player_datum *player = teleporter_get_player_datum(unit->unit.player_index);
            player->flags |= 1 << 8;
            player->flags |= 1 << 9;
        }
    }
    if (object->object.parent == -1 && object->object.next_object_index != -1)
        teleporter_flag_object_as_having_teleported_recursive(object->object.next_object_index != -1);
    if (object->object.first_child_object_index != -1)
        teleporter_flag_object_as_having_teleported_recursive(object->object.first_child_object_index);
}

struct s_game_engine_event_data;
// @address 0x29df00 teleporter_send_blocked_event
extern "C" void teleporter_send_blocked_event(unsigned int blocked_player_index)
{
    struct {
        unsigned int f_0;
        unsigned int f_4;
        unsigned int f_8;
        unsigned int f_c;
        unsigned int f_10;
        unsigned int f_14;
        unsigned int f_18;
        unsigned int f_1c;
        unsigned int f_20;
        short f_24;
    } blocked_event_data;

    blocked_event_data.f_0 = 0;
    blocked_event_data.f_4 = 0x17;
    blocked_event_data.f_10 = blocked_player_index;
    blocked_event_data.f_8 = 0xffffffff;
    blocked_event_data.f_20 = 0;
    blocked_event_data.f_14 = 0xffffffff;
    blocked_event_data.f_18 = 0xffffffff;
    blocked_event_data.f_1c = 0xffffffff;
    blocked_event_data.f_c = blocked_player_index;
    blocked_event_data.f_24 = -1;
    game_engine_send_event((s_game_engine_event_data *)&blocked_event_data);
}

static __forceinline s_multiplayer_runtime_globals_definition *teleporter_try_and_get_runtime_globals()
{
    if (!((s_game_globals *)global_game_globals))
        return 0;
    s_multiplayer_globals_definition *definition = (s_multiplayer_globals_definition *)((
        cache_file_tag_instance *)global_tag_instances)[datum_index_to_absolute_index(((s_game_globals *)global_game_globals)->multiplayer_globals_index)]
                                                           .definition;
    return definition ? (s_multiplayer_runtime_globals_definition *)definition->runtime.address : 0;
}
// @address 0x29df60 ?get_teleporter_recharge_time_ticks@@YAJXZ
long get_teleporter_recharge_time_ticks(void)
{
    s_multiplayer_runtime_globals_definition *runtime = teleporter_try_and_get_runtime_globals();
    s_multiplayer_constants *recharge = runtime ? (s_multiplayer_constants *)runtime->multiplayer_constants.address : 0;
    return recharge ? teleporter_seconds_to_ticks_round(recharge->teleporter_recharge_time) : 0;
}


c_teleporter_area *teleporter_try_and_get_from_object_index(long object_index)
{
    c_teleporter_area *result = 0;
    if (object_index != -1)
    {
        s_teleporter_list *list = (s_teleporter_list *)tls_game_engine_globals()->teleporter_slot_table;
        if (list->entries != list->entries + list->count)
        {
            c_teleporter_area *end = list->entries + list->count;
            c_teleporter_area *e = list->entries;
            do
            {
                if (e->m_object_index == object_index)
                {
                    result = e;
                    break;
                }
                e++;
            } while (e != end);
        }
    }
    return (c_teleporter_area *)result;
}


static __forceinline bool teleporter_game_mode_is_4()
{
    bool result = false;
    if (game_globals->options.game_simulation == 4)
        result = true;
    return result;
}
static __forceinline c_teleporter_area *teleporter_slot_table_find_selected(c_area_set<c_teleporter_area, 32> *table)
{
    c_teleporter_area *result = 0;
    for (c_teleporter_area *slot = table->m_areas.begin(); slot != &table->m_areas[table->m_area_count]; slot++) {
        if (((c_area *)&*slot)->is_selected()) {
            result = slot;
            break;
        }
    }
    return result;
}
// @address 0x29e060 ?initialize@?$c_area_set@Vc_teleporter_area@@$0CA@@@QAAXJJW4e_multiplayer_team_designator@@@Z
template<class t_area, long k_area_count>
void c_area_set<t_area, k_area_count>::initialize(long type_mask, long flags_mask, e_multiplayer_team_designator owner)
{
    c_area_set<c_teleporter_area, 32> *table = (c_area_set<c_teleporter_area, 32> *)this;
    // match: 0x29e060: all callers use the same team, type mask and flags.
    table->m_team_designator = 8;
    table->m_area_type_flags = 0x440;
    table->m_area_flags = 112;
    table->update();
    if (!teleporter_game_mode_is_4()) {
        c_teleporter_area *slot = teleporter_slot_table_find_selected(table);
        if (slot)
            ((c_area *)&*slot)->set_selected(false);
    }
}

template void c_area_set<c_teleporter_area, 32>::initialize(long, long, e_multiplayer_team_designator);

static __forceinline void *teleporter_slot_datum_try_and_get(s_data_array *data, int handle)
{
    if (handle == -1)
        return 0;
    int absolute_index = datum_index_to_absolute_index(handle);
    if (absolute_index >= data->first_unallocated)
        return 0;
    short *datum = (short *)((char *)data->data + data->size * absolute_index);
    int identifier = *datum;
    if (identifier == 0 || identifier != (handle >> 16))
        return 0;
    return datum;
}
static __forceinline char *teleporter_slot_object_try_and_get(int handle, unsigned int type_mask)
{
    s_object_header *header = (s_object_header *)teleporter_slot_datum_try_and_get(object_header_data, handle);
    if (!header || !((1 << header->object_type) & type_mask))
        return 0;
    return (char *)header->datum;
}
static __forceinline char *teleporter_slot_get_owner_data(int handle)
{
    char *object = teleporter_slot_object_try_and_get(handle, -1);
    if (object) {
        short offset = ((object_datum *)object)->multiplayer_data_offset;
        if (offset != -1)
            return object + offset;
    }
    return 0;
}
static __forceinline s_object_header *teleporter_slot_get_header(int handle)
{
    return datum_get<s_object_header>(object_header_data, handle);
}
static __forceinline c_teleporter_area &teleporter_slot_at(c_area_set<c_teleporter_area, 32> *table, long index) { return table->m_areas[index]; }
static __forceinline long teleporter_slot_get_object_index(const c_teleporter_area &slot) { return slot.m_object_index; }
static __forceinline void teleporter_slot_set_object_index(c_teleporter_area &slot, long index) { slot.m_object_index = index; }
static __forceinline void teleporter_slot_calculate_boundary_geometry(c_teleporter_area &slot)
{
    unsigned __int64 wide = (unsigned int)slot.m_object_index;
    if ((int)wide != -1)
        calculate_influence_boundary_geometry((int)wide, &slot, 0);
}
// @address 0x29e140 ?update_areas@?$c_area_set@Vc_teleporter_area@@$0CA@@@IAAXXZ
template<class t_area, long k_area_count>
void c_area_set<t_area, k_area_count>::update_areas()
{
    c_area_set<c_teleporter_area, 32> *table = (c_area_set<c_teleporter_area, 32> *)this;
    int handles[32];
    c_teleporter_area temporary;
    c_teleporter_object_iterator iterator;
    int count = 0;

    iterator.begin(table->m_area_type_flags, 0, table->m_area_flags);
    while (iterator.next() && count < 32) {
        int handle = iterator.get_index();
        char *owner_data = teleporter_slot_get_owner_data(handle);
        if (!(teleporter_slot_get_header(handle)->flags & 0x10)) {
            if (!owner_data || ((s_multiplayer_object_properties *)owner_data)->team_designator.get() == table->m_team_designator)
                handles[count++] = handle;
        }
    }
    qsort(handles, count, 4, c_area_set_compare_by_order_index);
    if (count > 32)
        count = 32;
    for (long i = 0; i < count; i++) {
        long handle = handles[i];
        if (teleporter_slot_get_object_index(teleporter_slot_at(table, i)) != handle) {
            memcpy(&temporary, &teleporter_slot_at(table, i), sizeof(c_teleporter_area));
            for (long j = i; j < table->m_area_count; j++) {
                if (teleporter_slot_get_object_index(teleporter_slot_at(table, j)) == handle) {
                    memcpy(&teleporter_slot_at(table, i), &teleporter_slot_at(table, j), sizeof(c_teleporter_area));
                    memcpy(&teleporter_slot_at(table, j), &temporary, sizeof(c_teleporter_area));
                    break;
                }
            }
        }
        teleporter_slot_set_object_index(teleporter_slot_at(table, i), handle);
        teleporter_slot_calculate_boundary_geometry(teleporter_slot_at(table, i));
    }
    table->m_area_count = count;
}

template void c_area_set<c_teleporter_area, 32>::update_areas();

// @address 0x29c658 teleporter_are_points_linked
extern "C" bool teleporter_are_points_linked(const real_point3d *sender_point, const real_point3d *receiver_point)
{
    float rz = 0.0f;
    bool result = false;
    for (c_teleporter_area *sender = teleporter_get_area_set()->begin(); sender != teleporter_get_area_set()->end() && !result; ++sender)
    {
        const s_multiplayer_object_properties *sender_properties = (const s_multiplayer_object_properties *)sender->get_multiplayer_properties();
        bool is_sender = sender_properties->object_type.get() == 9 || sender_properties->object_type.get() == 10;
        if (is_sender)
        {
            float dx = sender_point->x - sender->m_boundary_geometry.matrix.position.x;
            float dy = sender_point->y - sender->m_boundary_geometry.matrix.position.y;
            float dz = sender_point->z - sender->m_boundary_geometry.matrix.position.z;
            if (dy * dy + (dz * dz + dx * dx) < 1.0f)
            {
                for (c_teleporter_area *receiver = teleporter_get_area_set()->begin(); receiver != teleporter_get_area_set()->end(); ++receiver)
                {
                    if (receiver != sender)
                    {
                        const s_multiplayer_object_properties *receiver_properties =
                            (const s_multiplayer_object_properties *)receiver->get_multiplayer_properties();
                        bool is_receiver = receiver_properties->object_type.get() == 9 || receiver_properties->object_type.get() == 11;
                        if (is_receiver && receiver_properties->teleporter_channel.get() == sender_properties->teleporter_channel.get())
                        {
                            float rx = receiver_point->x - receiver->m_boundary_geometry.matrix.position.x;
                            float ry = receiver_point->y - receiver->m_boundary_geometry.matrix.position.y;
                            rz = receiver_point->z - receiver->m_boundary_geometry.matrix.position.z;
                            if (ry * ry + (rx * rx + rz * rz) < 1.0f)
                            {
                                result = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x29c820 teleporter_is_channel_linked
extern "C" float teleporter_is_channel_linked(long object_index)
{
    c_teleporter_area *area = teleporter_try_and_get_from_object_index(object_index);
    bool channel_linked = false;
    if (area != 0)
    {
        long doffset = (area->m_object_index << 4) & 0xffff0;
        // match: 0x29c820: value range assumption guides branch lowering
        __assume(doffset != -1);
        s_object_header *dheader = (s_object_header *)((char *)object_header_data->data + doffset);
        unit_datum *object = (unit_datum *)dheader->datum;
        short multiplayer_offset = object->object.multiplayer_data_offset;
        s_multiplayer_object_properties *multiplayer = (s_multiplayer_object_properties *)((char *)object + multiplayer_offset);
        signed char type = multiplayer->object_type.get();
        unsigned char is_sender = (type == 10 || type == 9);
        unsigned char is_receiver = (type == 11 || type == 9);
        if (is_sender)
            channel_linked = teleporter_has_receiver_on_channel(area);
        if (is_receiver != 0 && channel_linked == 0)
            channel_linked = teleporter_has_sender_on_channel(area);
    }
    if (channel_linked != 0)
        return k_teleporter_one;
    return k_teleporter_zero;
}


// @address 0x29dba0 teleporter_teleport_object
extern "C" unsigned char teleporter_teleport_object(c_teleporter_area *area, c_teleporter_area *receiver, long object_index)
{
    bool result = false;
    if (receiver && object_index != -1 && !teleporter_is_teleport_blocked(area, receiver, object_index))
    {
        real_point3d position;
        real_vector3d forward;
        real_vector3d up;
        real_vector3d linear_velocity;
        real_vector3d angular_velocity;

        multiplayer_get_spawn_object_position_and_facing_vectors(receiver->m_object_index, &position, &forward, &up);
        teleporter_compute_destination(area, receiver, object_index, &position);

        unit_datum *object = teleporter_get_object_datum(object_index);
        object_get_velocities(object_index, &linear_velocity, &angular_velocity);

        float speed = sqrtf((linear_velocity.i * linear_velocity.i + linear_velocity.j * linear_velocity.j) + linear_velocity.k * linear_velocity.k);
        linear_velocity.i = forward.i * speed;
        linear_velocity.j = forward.j * speed;
        linear_velocity.k = forward.k * speed;

        object_set_position_direct(object_index, &position, &forward, &up, 0, false);
        object_set_velocities(object_index, &linear_velocity, &angular_velocity);

        if ((1 << object->object.identifier.m_type) & 0x1003)
        {
            unit_datum *player_object = teleporter_get_object_datum(object_index);
            long player_index = player_object->unit.player_index;
            if (player_index != -1)
            {
                float zero = k_teleporter_zero;
                forward.k = zero;
                if (normalize3d(&forward) == zero)
                    forward = k_real_math_identity_matrix_storage.forward;
                player_set_facing(player_index, &forward);
                player_increment_teleport_counter(player_index);
                simulation_action_game_engine_player_update((short)player_index, 0x10000);
            }
            else if (player_object->object.identifier.m_type == 1 && player_object->object.parent == -1 && player_object->unit.driver_object_index != -1)
            {
                long driver_player_index = teleporter_get_object_datum(player_object->unit.driver_object_index)->unit.player_index;
                if (driver_player_index != -1)
                {
                    float zero = k_teleporter_zero;
                    forward.k = zero;
                    if (normalize3d(&forward) == zero)
                        forward = k_real_math_identity_matrix_storage.forward;
                    player_set_facing(driver_player_index, &forward);
                }
            }
        }

        teleporter_flag_object_as_having_teleported_recursive(object_index);
        area->update_countdown = get_teleporter_recharge_time_ticks();
        result = true;
    }
    return result;
}
