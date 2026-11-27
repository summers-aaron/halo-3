// networkinglibxbox/simulation_game_weapons.obj: from the records 0x435e00 0x437a60 0x437a70 0x437ac8 0x437ad8 0x437ae8 0x437af8 0x437b08 0x437b18 0x437b20 0x437b30 0x437b40 0x437b50 0x437b60 0x437b70 0x437b80 0x437b90 0x437b98 0x437ba8 0x437c40 0x437cb8 0x437eb8 0x437f08 0x437fa8 0x4381b0 0x438220 0x438278 0x4382f0 0x438360 0x438508 0x4386a0 0x438738 0x438740 0x4387e0 0x438840 0x438a80 0x438da8 0x438e60 0x439388 0x4393e8 0x439430 0x439490 0x4397a0 0x439800 0x4398c8 0x4399b0 0x439a10 0x439a78 0x439b58 0x439bb8 0x439c30 0x439d38 0x43a0b0 0x43a0b8 0x43a118 0x43a168 0x43a1d8 0x43a428 0x43a480 0x43a4f8 0x43a748 0x5bec98 0x709f88 0x70c880
// @flags /TP /O2 /Oc
#include "simulation/game_interface/simulation_game_weapons.h"
#include "simulation/simulation_queue_event_types.h"
#include "networking/simulation/simulation_entity_definition.h"
#include "memory/tls_globals.h"
#include "tag_files/tag_index.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "networking/simulation/simulation_entity_database.h"
#include "math/real_math_types.h"
#include "units/unit_control.h"
#include "units/motor_enums.h"
#include "game/game_globals.h"
#include "motor/action_system.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "cache/cache_files.h"
#include "effects/effects.h"
#include "game/aim_assist.h"
#include "game/game_engine_notifications.h"
#include "items/weapons.h"
#include "motor/motor_system.h"
#include "networking/replication/replication_encoding.h"
#include "simulation/game_interface/simulation_game_entities.h"
#include "simulation/game_interface/simulation_game_interface.h"
#include "simulation/game_interface/simulation_game_internal.h"
#include "simulation/simulation_encoding.h"
#include "simulation/simulation_gamestate_entities.h"
#include "units/unit_animation.h"
#include "cseries/cseries.h"
#include "simulation/simulation_priority.h"
#include "simulation/simulation_entity_types.h"
#include "simulation/game_interface/simulation_game_projectiles.h"
#include "motor/motor_animation.h"
#include "scenario/scenario.h"
#include "items/weapon_definitions.h"
#include "units/unit_types.h"
#include "ai/weapon_datum_types.h"
#include "memory/bitstream.h"
#include "memory/datum_index.h"
#include <stddef.h>
#include <string.h>
#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "simulation/game_interface/simulation_game_items.h"
#include "simulation/game_interface/simulation_game_object_entity_definition.h"
#include "networking/replication/replication_motion.h"
#include "networking/replication/replication_event.h"
#include "simulation/simulation_node_space_encoding_types.h"
#include "networking/replication/replication_entity.h"
#include "items/item_types.h"
#include "units/action_request.h"
#include "math/real_math.h"
#include "units/unit_weapon_pickup_result.h"
#include "effects/effect_creation_data.h"
#include "game/game_engine_settings_definitions.h"
#include "game/multiplayer_runtime_globals.h"
#include "memory/static_arrays.h"
#include "math/matrix_math.h"
#include "game/game_globals_storage.h"
#include "game/game_time_types.h"
#include "items/weapon_fire_types.h"
#include "units/unit_facing.h"
#include "game/multiplayer_constants.h"
#include "objects/object_header_access.h"
#include "cseries/bit_operations.h"
#include "tag_files/string_ids.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "interface/user_interface_messages.h"
#include "networking/session/network_session_parameters_base.h"
#include "game/game_engine_base_variant.h"
union real_point3d;
union real_rectangle3d;
struct s_player_predicted_aim_assist;
extern "C" {
// @address 0x49758 ??_7c_simulation_weapon_entity_definition@@6B@
// @address 0x709f88 ??__Eg_simulation_weapon_entity_definition@@YAXXZ
// @address 0x70c880 ??__Fg_simulation_weapon_entity_definition@@YAXXZ
// @address 0x7bb500 g_simulation_weapon_entity_definition
c_simulation_weapon_entity_definition g_simulation_weapon_entity_definition;
// @address 0x497ec ??_7c_simulation_weapon_fire_event_definition@@6B@
// @address 0x7bb504 g_simulation_weapon_fire_event_definition
c_simulation_weapon_fire_event_definition g_simulation_weapon_fire_event_definition;
// @address 0x49824 ??_7c_simulation_weapon_reload_event_definition@@6B@
// @address 0x7bb508 g_simulation_weapon_reload_event_definition
c_simulation_weapon_reload_event_definition g_simulation_weapon_reload_event_definition;
// @address 0x4985c ??_7c_simulation_weapon_pickup_event_definition@@6B@
// @address 0x7bb50c g_simulation_weapon_pickup_event_definition
c_simulation_weapon_pickup_event_definition g_simulation_weapon_pickup_event_definition;
// @address 0x49894 ??_7c_simulation_weapon_put_away_event_definition@@6B@
// @address 0x7bb510 g_simulation_weapon_put_away_event_definition
c_simulation_weapon_put_away_event_definition g_simulation_weapon_put_away_event_definition;
// @address 0x498cc ??_7c_simulation_weapon_drop_event_definition@@6B@
// @address 0x7bb514 g_simulation_weapon_drop_event_definition
c_simulation_weapon_drop_event_definition g_simulation_weapon_drop_event_definition;
// @address 0x49904 ??_7c_simulation_weapon_effect_event_definition@@6B@
// @address 0x7bb518 g_simulation_weapon_effect_event_definition
c_simulation_weapon_effect_event_definition g_simulation_weapon_effect_event_definition;
// @address 0x4993c ??_7c_simulation_weapon_empty_click_event_definition@@6B@
// @address 0x7bb51c g_simulation_weapon_empty_click_event_definition
c_simulation_weapon_empty_click_event_definition g_simulation_weapon_empty_click_event_definition;
}
// @address 0x439490 ?apply_game_event@c_simulation_weapon_reload_event_definition@@UAA_NJPBJJPBX@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0xa3258 ?aim_assist_targeting_decode@@YA_NPAVc_bitstream@@PAUs_aim_assist_targeting_result@@@Z
// @address 0x433b38 ?build_baseline_state_data@c_simulation_item_entity_definition@@UAA_NJPBXJPAX@Z
// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0x43ae28 ?entity_creation_decode@c_simulation_item_entity_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x40fb00 ?predicted_aim_assist_decode@@YA_NPAVc_bitstream@@PAUs_player_predicted_aim_assist@@@Z
// @address 0x3d2398 ?simulation_entity_get_gamestate_index@@YAJJ@Z
// @address 0x40ebd8 ?simulation_read_definition_index@@YAJPBDPAVc_bitstream@@@Z
// @address 0x40eb58 ?simulation_write_definition_index@@YAXPBDPAVc_bitstream@@J@Z
// @address 0x3adbd8 ?simulation_write_quantized_position@@YAXPAVc_bitstream@@PBTreal_point3d@@J_NPBTreal_rectangle3d@@@Z
// @address 0x434038 ?update_minimum_required_bits@c_simulation_item_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
// @address 0x40b128 ?object_build_baseline_state_data@c_simulation_object_entity_definition@@KA_NPBUs_simulation_object_creation_data@@PAUs_simulation_object_state_data@@@Z
// @address 0x40c818 ?object_creation_encode@c_simulation_object_entity_definition@@KAXPBUs_simulation_object_creation_data@@PAVc_bitstream@@_N@Z
// @address 0xdd8d8 ?motor_system_interrupted@@YAXJPAVc_motor_task@@_N@Z
// @address 0x355ec8 ?motor_task_find_by_action@@YAPAVc_motor_task@@JW4e_action@@@Z
// @address 0x3abe70 ?simulation_action_unit_reload@@YAXJ_N0@Z
// @address 0x3af138 ?simulation_gamestate_entity_get_object_index@@YAJJ@Z
// @address 0xa05d8 ?weapon_reloading@@YA_NJJPAF@Z
// @address 0x34ea98 ?weapon_reset@@YAXJ@Z
// @address 0x3503e8 ?weapon_set_state@@YA_NJJ_N@Z
// @address 0x437ba8 ?build_object_creation_data@c_simulation_weapon_entity_definition@@UAAXJJPAX@Z
// @address 0x4382f0 ?update_minimum_required_bits@c_simulation_weapon_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
// @address 0x438278 ?entity_creation_decode@c_simulation_weapon_entity_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x437b90 ?state_data_size@c_simulation_weapon_entity_definition@@UAAJXZ
// @address 0x437b98 ?initial_update_mask@c_simulation_weapon_entity_definition@@UAAKXZ
// @address 0x43a0b0 ?minimum_required_bits@c_simulation_weapon_effect_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x438360 ?entity_update_encode@c_simulation_weapon_entity_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
// @address 0x0a2b80 ?quantize_real@@YAJMMMJ_N0@Z
// @address 0x446e88 ?make_room_for_update@c_entity_update_encode_helper@@QAA_NPAVc_bitstream@@JJJK@Z
// @address 0x446ef8 ?write_component_header@c_entity_update_encode_helper@@QAA_NJPBD@Z
// @address 0x446f80 ?finish_component@c_entity_update_encode_helper@@QAAXXZ
#define BITSTREAM_COMPARE_QUANTIZED_REALS
class c_replication_outgoing_event;

namespace {
class c_simulation_object_state_helpers {
public:
    static __forceinline void prepare(unsigned long update_mask, s_simulation_object_state_data *state) {
        if (update_mask & (1 << 11))
            state->parent_gamestate_index = simulation_entity_get_gamestate_index(state->parent_entity_index);
    }
};
class c_simulation_item_state_helpers {
public:
    static __forceinline unsigned long initial_update_mask() { return 23359; }
    static __forceinline void prepare(unsigned long update_mask, s_simulation_object_state_data *state) {
        unsigned long object_update_mask = update_mask & 0x3fff;
        if (object_update_mask)
            c_simulation_object_state_helpers::prepare(object_update_mask, state);
    }
};
}
struct s_simulation_item_state_data;

// 0x437a60
// @address 0x437a60 ?event_type_name@c_simulation_weapon_fire_event_definition@@UAAPBDXZ
char const *c_simulation_weapon_fire_event_definition::event_type_name()
{
    return "weapon_fire";
}

// 0x437ac8
// @address 0x437ac8 ?event_type_name@c_simulation_weapon_reload_event_definition@@UAAPBDXZ
char const *c_simulation_weapon_reload_event_definition::event_type_name()
{
    return "weapon_reload";
}

// 0x437ae8
// @address 0x437ae8 ?event_type_name@c_simulation_weapon_pickup_event_definition@@UAAPBDXZ
char const *c_simulation_weapon_pickup_event_definition::event_type_name()
{
    return "weapon_pickup";
}

// 0x437b08
// @address 0x437b08 ?event_type_name@c_simulation_weapon_put_away_event_definition@@UAAPBDXZ
char const *c_simulation_weapon_put_away_event_definition::event_type_name()
{
    return "weapon_put_away";
}

// 0x437b18
// @address 0x437b18 ?event_type@c_simulation_weapon_drop_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_weapon_drop_event_definition::event_type()
{
    return _simulation_event_type_weapon_drop;
}

// 0x437b20
// @address 0x437b20 ?event_type_name@c_simulation_weapon_drop_event_definition@@UAAPBDXZ
char const *c_simulation_weapon_drop_event_definition::event_type_name()
{
    return "weapon_drop";
}

// 0x437b40
// @address 0x437b40 ?event_type_name@c_simulation_weapon_effect_event_definition@@UAAPBDXZ
char const *c_simulation_weapon_effect_event_definition::event_type_name()
{
    return "weapon_effect";
}

// 0x437b60
// @address 0x437b60 ?event_type_name@c_simulation_weapon_empty_click_event_definition@@UAAPBDXZ
char const *c_simulation_weapon_empty_click_event_definition::event_type_name()
{
    return "weapon_empty_click";
}

// 0x437b80
// @address 0x437b80 ?entity_type_name@c_simulation_weapon_entity_definition@@UAAPBDXZ
char const *c_simulation_weapon_entity_definition::entity_type_name()
{
    return "weapon";
}

// 0x437b90
long c_simulation_weapon_entity_definition::state_data_size()
{
    return sizeof(s_simulation_weapon_update_state);
}

// 0x437b98
unsigned long c_simulation_weapon_entity_definition::initial_update_mask()
{
    return 0x1fdb3f;
}

namespace {
static __forceinline void weapon_object_build_creation_data(long object_index, s_simulation_object_creation_data *creation)
{
    weapon_datum *object = (weapon_datum *)object_header_get(object_index)->datum;
    creation->definition_index = object->item.object.definition_index;
    creation->scenario_datum_index = object->item.object.scenario_datum_index;
    creation->map_variant_index = object->item.object.map_variant_index;
    creation->allow_duplicate_scenario_objects = (object->item.object.flags_d6 >> 2) & 1;
    creation->child_variant_index = object->item.object.marker;
}
static __forceinline void weapon_item_build_creation_data(long object_index, long creation_data_size, void *creation_data)
{
    s_simulation_object_creation_data *creation = (s_simulation_object_creation_data *)creation_data;
    memset(creation, 0, sizeof(*creation));
    weapon_object_build_creation_data(object_index, creation);
}
}

// 0x437ba8
void c_simulation_weapon_entity_definition::build_object_creation_data(long object_index, long creation_data_size, void *creation_data)
{
    s_simulation_weapon_creation_data *creation = (s_simulation_weapon_creation_data *)creation_data;
    weapon_datum *weapon = (weapon_datum *)object_header_get(object_index)->datum;
    memset(creation, 0, sizeof(*creation));
    weapon_item_build_creation_data(object_index, sizeof(creation->object), creation);
    creation->simulation_weapon_identifier = weapon->simulation_weapon_identifier;
}

// @address 0x437c40 ?build_baseline_state_data@c_simulation_weapon_entity_definition@@UAA_NJPBXJPAX@Z
bool c_simulation_weapon_entity_definition::build_baseline_state_data(
    long creation_size, const void *creation_data, long state_size, void *state_data)
{
    bool built = false;
    memset(state_data, 0, sizeof(s_simulation_weapon_update_state));
    if (c_simulation_item_entity_definition::build_baseline_state_data(
            sizeof(s_simulation_object_creation_data), creation_data, sizeof(s_simulation_item_state_data), state_data))
    {
        ((s_simulation_weapon_update_state *)state_data)->multiplayer_flags = 0;
        ((s_simulation_weapon_update_state *)state_data)->multiplayer_weapon_identifier = -1;
        built = true;
    }
    return built;
}

struct s_simulation_object_creation_data;

namespace {

static __forceinline short simulation_game_weapons_multiplayer_type(long object_index)
{
    weapon_datum *weapon = (weapon_datum *)object_get_and_verify_type(object_index, _object_mask_weapon);
    weapon_definition *definition = (weapon_definition *)tag_get('weap', weapon->definition_index);
    return definition->multiplayer_weapon_type;
}
}

// @address 0x437cb8 ?handle_object_update@c_simulation_weapon_entity_definition@@UAAKJKJPAX@Z
unsigned long c_simulation_weapon_entity_definition::handle_object_update(long object_index, unsigned long update_mask, long state_data_size, void *state_data)
{
    s_simulation_weapon_update_state *state = (s_simulation_weapon_update_state *)state_data;
    weapon_datum *weapon = (weapon_datum *)object_get_and_verify_type(object_index, _object_mask_weapon);
    unsigned long result = c_simulation_item_entity_definition::handle_object_update(
        object_index, update_mask & 0x3ffff, sizeof(s_simulation_item_state_data), state_data);
    if (simulation_game_weapons_multiplayer_type(object_index) != 0)
    {
        if (update_mask & 0x40000)
        {
            byte flags = 0;
            for (long bit = 0; bit < 4; bit++)
                SET_BIT(flags, bit, (weapon->flags & (1 << (bit + 9))) != 0);
            if (state->multiplayer_flags != flags)
            {
                state->multiplayer_flags = flags;
                result |= 0x40000;
            }
        }
        if (update_mask & 0x80000)
        {
            if (state->multiplayer_weapon_identifier != (short)(unsigned short)weapon->multiplayer_weapon_identifier)
            {
                state->multiplayer_weapon_identifier = (short)(unsigned short)weapon->multiplayer_weapon_identifier;
                result |= 0x80000;
            }
        }
    }
    if (update_mask & 0x100000)
    {
        weapon_definition *definition = (weapon_definition *)tag_get('weap', weapon->definition_index);
        bool age_changed = false;
        bool force = !weapon_can_be_fired(object_index);
        if (state->age != weapon->age || force)
        {
            state->age = weapon->age;
            result |= 0x100000;
            age_changed = true;
        }
        if (definition->magazines.count() > 0)
        {
            const s_weapon_magazine *magazine = &weapon->magazines[0];
            if (state->rounds_loaded != (unsigned short)magazine->rounds_loaded
                || state->rounds_inventory != (unsigned short)magazine->rounds_inventory
                || age_changed || force)
            {
                state->age = weapon->age;
                state->rounds_loaded = (unsigned short)magazine->rounds_loaded;
                state->rounds_inventory = (unsigned short)magazine->rounds_inventory;
                result |= 0x100000;
            }
        }
    }
    return result;
}

// 0x437eb8
// @address 0x437eb8 ?prepare_state_data_for_gameworld@c_simulation_weapon_entity_definition@@UAAXKJPAX@Z
void c_simulation_weapon_entity_definition::prepare_state_data_for_gameworld(unsigned long update_mask, long state_data_size, void *state_data)
{
    s_simulation_object_state_data *state = (s_simulation_object_state_data *)state_data;
    unsigned long item_update_mask = update_mask & 0x3ffff;
    if (item_update_mask)
        c_simulation_item_state_helpers::prepare(item_update_mask, state);
}


// @address 0x437f08 ?create_object@c_simulation_weapon_entity_definition@@UAAJJPBXPAKJ0@Z
long c_simulation_weapon_entity_definition::create_object(
    long creation_data_size, const void *creation_data, unsigned long *flags, long state_data_size, const void *state_data)
{
    const s_simulation_weapon_creation_data *creation = (const s_simulation_weapon_creation_data *)creation_data;
    long object_index = -1;
    if (creation->object.definition_index != -1 && tag_try_and_get_unsafe('weap', creation->object.definition_index))
    {
        object_index = c_simulation_item_entity_definition::create_object(
            sizeof(creation->object), creation_data, flags, sizeof(s_simulation_item_state_data), state_data);
        if (object_index != -1)
            ((weapon_datum *)object_header_get(object_index)->datum)->simulation_weapon_identifier
                = creation->simulation_weapon_identifier;
    }
    return object_index;
}

// @address 0x437fa8 ?apply_object_update@c_simulation_weapon_entity_definition@@UAAXJKJPBX@Z
void c_simulation_weapon_entity_definition::apply_object_update(long object_index, unsigned long update_mask, long state_data_size, const void *state_data)
{
    const s_simulation_weapon_update_state *state = (const s_simulation_weapon_update_state *)state_data;
    weapon_datum *weapon = (weapon_datum *)object_get_and_verify_type(object_index, _object_mask_weapon);
    long item_update_mask = update_mask & 0x3ffff;
    if (item_update_mask)
        c_simulation_item_entity_definition::apply_object_update(object_index, item_update_mask, sizeof(s_simulation_item_state_data),
            state_data);
    if ((update_mask & 0x40000) || (update_mask & 0x80000))
    {
        if (simulation_game_weapons_multiplayer_type(object_index) != 0)
        {
            if (update_mask & 0x40000)
            {
                for (long bit = 0; bit < 4; bit++)
                    SET_BIT(weapon->flags, bit + 9, (state->multiplayer_flags & (1 << bit)) != 0);
            }
            if ((update_mask & 0x80000) && (unsigned short)weapon->multiplayer_weapon_identifier != (unsigned short)state->multiplayer_weapon_identifier)
            {
                if (weapon->weapon_flags.test(6))
                    game_engine_multiplayer_weapon_deregister(object_index);
                if (state->multiplayer_weapon_identifier != -1)
                    game_engine_multiplayer_weapon_register(object_index, state->multiplayer_weapon_identifier);
            }
        }
    }
    if (update_mask & 0x100000)
    {
        weapon_definition *definition = (weapon_definition *)tag_get('weap', weapon->definition_index);
        weapon->age = state->age;
        if (definition->magazines.count() > 0)
        {
            const s_weapon_magazine_definition *magazine = &definition->magazines[0];
            if ((short)state->rounds_loaded <= magazine->rounds_loaded_maximum && (short)state->rounds_inventory <= magazine->rounds_inventory_maximum)
            {
                weapon->magazines[0].rounds_loaded = state->rounds_loaded;
                weapon->magazines[0].rounds_inventory = state->rounds_inventory;
            }
        }
    }
}

// 0x4382f0
long c_simulation_weapon_entity_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update)
{
    unsigned long update_mask = update->update_mask;
    long required_bits = c_simulation_item_entity_definition::update_minimum_required_bits(entity, update);
    required_bits += 3;
    if (update_mask & 0x40000) {
        long minimum_bits = 25;
        required_bits = required_bits > minimum_bits ? minimum_bits : required_bits;
    }
    if (update_mask & 0x80000) {
        long minimum_bits = 26;
        required_bits = required_bits > minimum_bits ? minimum_bits : required_bits;
    }
    if (update_mask & 0x100000) {
        long minimum_bits = 47;
        required_bits = required_bits > minimum_bits ? minimum_bits : required_bits;
    }
    return required_bits;
}

extern "C" {
// @rdata 0x82049628..0x82049680
// @address 0x49628 k_multiplayer_flags_exist
const char k_multiplayer_flags_exist[28] = "multiplayer-state-exists";
// @address 0x49644 k_multiplayer_weapon_identifier_exists
const char k_multiplayer_weapon_identifier_exists[40] = "multiplayer-weapon-identifier-exists";
// @address 0x4966c k_rounds_exist
const char k_rounds_exist[20] = "weapon-ammo-exists";
// @rdata-end
}
const float k_zero = 0.0f;
const float k_one = 1.0f;

// 0x438360
bool c_simulation_weapon_entity_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *written_update_mask, long size, const void *state, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long maximum_bits, bool final_update)
{
    bool result = false;
    if (this->item_update_encode(initial_update, update_mask & 0x3ffff, written_update_mask, telemetry,
            (const s_simulation_item_state_data *)state, packet, maximum_bits + 3, final_update))
    {
        c_entity_update_encode_helper helper;
        const s_simulation_weapon_update_state *weapon_state = (const s_simulation_weapon_update_state *)state;
        if (helper.make_room_for_update(packet, maximum_bits, 18, 3, update_mask & 0x1c0000))
        {
            if (helper.write_component_header(18, k_multiplayer_flags_exist))
                packet->write_integer("multiplayer-state-flags", weapon_state->multiplayer_flags, 4);
            helper.finish_component();
            if (helper.write_component_header(19, k_multiplayer_weapon_identifier_exists))
                packet->write_integer("multiplayer-weapon-identifier", weapon_state->multiplayer_weapon_identifier + 1, 5);
            helper.finish_component();
            if (helper.write_component_header(20, k_rounds_exist))
            {
                packet->write_integer("rounds-loaded", (short)weapon_state->rounds_loaded, 8);
                packet->write_integer("rounds-inventory", (short)weapon_state->rounds_inventory, 11);
                packet->write_quantized_real("age", weapon_state->age, k_zero, k_one, 7, true, true);
            }
            helper.finish_component();
            helper.finish_update(written_update_mask);
            result = true;
        }
    }
    return result;
}


// @address 0x438508 ?entity_update_decode@c_simulation_weapon_entity_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_weapon_entity_definition::entity_update_decode(
    bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool final_update)
{
    s_simulation_weapon_update_state *state = (s_simulation_weapon_update_state *)state_data;
    bool success = true;
    bool item_success = simulation_item_decode_update(this, initial_update, update_mask, state, packet, final_update);
    success = success && item_success;
    if (packet->read_bool("multiplayer-flags-exist"))
    {
        state->multiplayer_flags = (unsigned char)packet->read_integer("multiplayer-flags", 4);
        *update_mask |= 1UL << 18;
    }
    if (packet->read_bool("multiplayer-weapon-identifier-exists"))
    {
        state->multiplayer_weapon_identifier = (short)(packet->read_integer("multiplayer-weapon-identifier", 5) - 1);
        *update_mask |= 1UL << 19;
        if (state->multiplayer_weapon_identifier != -1)
            success = success && state->multiplayer_weapon_identifier >= 0 && state->multiplayer_weapon_identifier < 9;
    }
    if (packet->read_bool("rounds-exist"))
    {
        state->rounds_loaded = (unsigned short)packet->read_integer("rounds-loaded", 8);
        state->rounds_inventory = (unsigned short)packet->read_integer("rounds-inventory", 11);
        state->age = packet->read_quantized_real("age", k_zero, k_one, 7, true);
        *update_mask |= 1UL << 20;
    }
    success = success && !packet->overflowed();
    return success;
}

// 0x438738
// @address 0x438738 ?payload_size@c_simulation_weapon_fire_event_definition@@UAAJXZ
long c_simulation_weapon_fire_event_definition::payload_size()
{
    return sizeof(s_simulation_weapon_fire_event_data);
}

// @address 0x438740 ?minimum_required_bits@c_simulation_weapon_fire_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_weapon_fire_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
  const s_simulation_weapon_fire_event_data *fire_data = (const s_simulation_weapon_fire_event_data *)event->get_payload_data();
  int required_bits = 45;
  if (fire_data->impact_target_count > 0)
    required_bits = fire_data->impact_target_count * 10 + 46;
  long slot_count = fire_data->projectile_count;
  if (slot_count > 0) {
    int first_header_bits = (slot_count == 1) ? 12 : 4;
    for (int slot_index = 0; slot_index < slot_count; slot_index++) {
      required_bits += 6;
      if (fire_data->projectiles[slot_index].impact_valid)
        required_bits += first_header_bits + 16;
      if (fire_data->projectiles[slot_index].conical_valid)
        required_bits += 16;
    }
  }
  if (fire_data->tracked_data_valid)
    required_bits += 19;
  return required_bits;
}

// 0x4387e0
// @address 0x4387e0 ?write_description_to_string@c_simulation_weapon_fire_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_weapon_fire_event_definition::write_description_to_string(c_replication_outgoing_event const *event,
    s_simulation_view_telemetry_data const *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "weapon fire : relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}


static __forceinline void simulation_game_weapons_read_unit_vector(c_bitstream *packet, const char *name, real_vector3d *vector)
{
    long quantization = packet->read_integer(name, 19);
    bitstream_decode_unit_vector(quantization, vector);
}
static __forceinline void simulation_game_weapons_write_unit_vector(c_bitstream *packet, const char *name, const real_vector3d *vector)
{
    packet->write_value_internal((unsigned long)bitstream_encode_unit_vector(vector), 19);
}
// @address 0x438840 ?event_payload_encode@c_simulation_weapon_fire_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_weapon_fire_event_definition::event_payload_encode(long payload_size, void const *payload, c_bitstream *packet, bool quantize)
{
    const s_simulation_weapon_fire_event_data *data = (const s_simulation_weapon_fire_event_data *)payload;
    packet->write_integer("unit-inventory-index", data->unit_inventory_index + 1, 3);
    simulation_write_definition_index("weapon-definition", packet, data->weapon_definition_index);
    packet->write_integer("barrel-index", data->barrel_index, 1);
    packet->write_bool("primary-trigger", data->primary_trigger);
    packet->write_integer("proj-impact-target-count", data->impact_target_count, 4);
    for (long target_index = 0; target_index < data->impact_target_count; target_index++)
    {
        const s_simulation_weapon_fire_impact_target *target = &data->impact_targets[target_index];
        packet->write_integer("encoding-type", target->encoding_type, 1);
        if (quantize)
            replication_entity_index_encode(packet, target->entity_index);
        else
            packet->write_integer("predicted-gamestate-index", target->gamestate_index, 32);
    }
    packet->write_integer("proj-data-count", data->projectile_count, 4);
    for (long projectile_index = 0; projectile_index < data->projectile_count; projectile_index++)
    {
        long position_bits = data->projectile_count == 1 ? 12 : 4;
        const s_simulation_weapon_fire_projectile *projectile = &data->projectiles[projectile_index];
        packet->write_integer("projectile-id", projectile->projectile_id, 4);
        packet->write_bool("impact-valid", projectile->impact_valid);
        if (projectile->impact_valid)
        {
            packet->write_integer("target-index", projectile->target_index, 4);
            packet->write_integer("obj-node-index", projectile->object_node_index, 8);
            simulation_write_quantized_node_space_position(packet, &projectile->impact_position,
                (e_simulation_node_space_encoding_type)data->impact_targets[projectile->target_index].encoding_type, position_bits);
        }
        packet->write_bool("conical-valid", projectile->conical_valid);
        if (projectile->conical_valid)
            packet->write_integer("conical-random-seed", projectile->conical_random_seed, 16);
    }
    if (quantize)
        predicted_aim_assist_encode(packet, &data->predicted_aim_assist);
    else
        aim_assist_targeting_encode(packet, &data->aim_assist);
    simulation_game_weapons_write_unit_vector(packet, "aiming-vector", &data->aiming_vector);
    packet->write_bool("tracked-data-valid", data->tracked_data_valid);
    if (data->tracked_data_valid)
    {
        packet->write_integer("tracked-model-target-index", data->tracked_model_target_index, 5);
        if (quantize)
            replication_entity_index_encode(packet, data->tracked_entity_index);
        else
            packet->write_integer("tracked-gamestate-index", data->tracked_gamestate_index, 32);
    }
    packet->write_bool("generated-from-host-live-fire", data->generated_from_host_live_fire);
}
// @address 0x438a80 ?event_payload_decode@c_simulation_weapon_fire_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_weapon_fire_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_weapon_fire_event_data *data = (s_simulation_weapon_fire_event_data *)payload;
    bool valid = true;
    data->unit_inventory_index = packet->read_integer("unit-inventory-index", 3) - 1;
    if (data->unit_inventory_index != -1)
        valid = valid && data->unit_inventory_index >= 0 && data->unit_inventory_index < 4;
    data->weapon_definition_index = simulation_read_definition_index("weapon-definition", packet);
    data->barrel_index = packet->read_integer("barrel-index", 1);
    valid = valid && data->barrel_index >= 0 && data->barrel_index < 2;
    data->primary_trigger = packet->read_bool("primary-trigger");
    data->impact_target_count = packet->read_integer("proj-impact-target-count", 4);
    for (long target_index = 0; target_index < data->impact_target_count; target_index++)
    {
        s_simulation_weapon_fire_impact_target *target = &data->impact_targets[target_index];
        target->encoding_type = packet->read_integer("encoding-type", 1);
        if (quantize)
            replication_entity_index_decode(packet, &target->entity_index);
        else
            target->gamestate_index = packet->read_integer("predicted-gamestate-index", 32);
    }
    data->projectile_count = packet->read_integer("proj-data-count", 4);
    for (long projectile_index = 0; projectile_index < data->projectile_count; projectile_index++)
    {
        long position_bits = data->projectile_count == 1 ? 12 : 4;
        s_simulation_weapon_fire_projectile *projectile = &data->projectiles[projectile_index];
        projectile->projectile_id = packet->read_integer("projectile-id", 4);
        projectile->impact_valid = packet->read_bool("impact-valid");
        if (projectile->impact_valid)
        {
            projectile->target_index = packet->read_integer("target-index", 4);
            projectile->object_node_index = packet->read_integer("obj-node-index", 8);
            simulation_read_quantized_node_space_position(
                packet, &projectile->impact_position, data->impact_targets[projectile->target_index].encoding_type, position_bits);
        }
        projectile->conical_valid = packet->read_bool("conical-valid");
        if (projectile->conical_valid)
            projectile->conical_random_seed = (unsigned short)packet->read_integer("conical-random-seed", 16);
    }
    valid = valid && data->projectile_count >= 0;
    bool aim_valid = quantize ? predicted_aim_assist_decode(packet, &data->predicted_aim_assist) : aim_assist_targeting_decode(packet, &data->aim_assist);
    valid = valid && aim_valid;
    simulation_game_weapons_read_unit_vector(packet, "aiming-vector", &data->aiming_vector);
    data->tracked_data_valid = packet->read_bool("tracked-data-valid");
    if (data->tracked_data_valid)
    {
        data->tracked_model_target_index = packet->read_integer("tracked-model-target-index", 5);
        if (quantize)
            replication_entity_index_decode(packet, &data->tracked_entity_index);
        else
            data->tracked_gamestate_index = packet->read_integer("tracked-gamestate-index", 32);
    }
    data->generated_from_host_live_fire = packet->read_bool("generated-from-host-live-fire");
    return valid;
}

// 0x439388
struct s_source;
struct s_dest;

// 0x438da8
// @address 0x438da8 ?prepare_event_data_for_gameworld@c_simulation_weapon_fire_event_definition@@UAAXJPAX@Z
void c_simulation_weapon_fire_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
    s_simulation_weapon_fire_event_data *fire = (s_simulation_weapon_fire_event_data *)payload;
    fire->tracked_gamestate_index = -1;
    if (fire->tracked_data_valid) {
        fire->tracked_gamestate_index = simulation_entity_get_gamestate_index(fire->tracked_entity_index);
    }
    s_aim_assist_targeting_result *aim_target = &fire->aim_assist;
    memset(aim_target, 0, sizeof(*aim_target));
    aim_target->object_index = -1;
    aim_target->model_index = -1;
    aim_target->target_index = -1;
    ::simulation_game_reconstruct_predicted_aim_assist(&fire->predicted_aim_assist,
        aim_target);
    for (long i = 0; i < fire->impact_target_count; ++i) {
        s_simulation_weapon_fire_impact_target *entry = &fire->impact_targets[i];
        if (entry->entity_index != -1) {
            entry->gamestate_index = simulation_entity_get_gamestate_index(entry->entity_index);
        }
    }
}

// @address 0x439388 ?write_description_to_string@c_simulation_weapon_reload_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_weapon_reload_event_definition::write_description_to_string(c_replication_outgoing_event const *event,
    s_simulation_view_telemetry_data const *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "weapon-reload: relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x4393e8
// @address 0x4393e8 ?event_payload_encode@c_simulation_weapon_reload_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_weapon_reload_event_definition::event_payload_encode(long payload_size, void const *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_weapon_reload_event_data const *data = (s_simulation_weapon_reload_event_data const *)payload;
    packet->write_bool("reload-primary", data->primary);
    packet->write_bool("reload-secondary", data->secondary);
    packet->write_bool("reload-tertiary", data->tertiary);
}

// 0x439430
// @address 0x439430 ?event_payload_decode@c_simulation_weapon_reload_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_weapon_reload_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_weapon_reload_event_data *data = (s_simulation_weapon_reload_event_data *)payload;
    data->primary = packet->read_bool("reload-primary");
    data->secondary = packet->read_bool("reload-secondary");
    data->tertiary = packet->read_bool("reload-tertiary");
    return true;
}

// 0x439490
class c_motor_task;


static long simulation_game_weapons_unit_weapon(long unit_index, long slot)
{
    long weapon_index = -1;
    unit_datum *object = (unit_datum *)object_header_get(unit_index)->datum;
    signed char inventory_index = object->unit.current_weapon_indices[slot];
    if (inventory_index != -1)
        weapon_index = object->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}

static void simulation_game_weapons_interrupt_action(long unit_index, long action)
{
    c_motor_task *task = motor_task_find_by_action(unit_index, (e_action)action);
    if (task)
        motor_system_interrupted(unit_index, task, true);
}

bool c_simulation_weapon_reload_event_definition::apply_game_event(long event_index, long const *entity_reference, long payload_size, void const *payload)
{
    bool applied = false;
    if (*entity_reference != -1)
    {
        long unit_index = simulation_gamestate_entity_get_object_index(*entity_reference);
        if (unit_index != -1)
        {
            s_object_header *header = object_header_get(unit_index);
            if ((1 << header->object_type) & 0x1003)
            {
                unit_datum *object = (unit_datum *)header->datum;
                s_simulation_weapon_reload_event_data const *data = (s_simulation_weapon_reload_event_data const *)payload;
                bool disabled = (object->object.damage_flags >> 2) & 1;
                if (!disabled)
                {
                    if (!data->primary)
                    {
                        simulation_game_weapons_interrupt_action(unit_index, 60);
                        simulation_game_weapons_interrupt_action(unit_index, 56);
                        simulation_game_weapons_interrupt_action(unit_index, 57);
                    }
                    if (data->secondary)
                    {
                        long weapon_index = simulation_game_weapons_unit_weapon(unit_index, 0);
                        if (data->primary)
                        {
                            simulation_game_weapons_interrupt_action(unit_index, 61);
                            simulation_game_weapons_interrupt_action(unit_index, 62);
                            if (weapon_index != -1)
                                weapon_interrupt_reload(weapon_index, false);
                        }
                        else
                        {
                            if (weapon_index != -1 && !weapon_reloading(weapon_index, 0, NULL))
                            {
                                weapon_reset(weapon_index);
                                weapon_set_state(weapon_index, 0, true);
                            }
                            action_submit_event(unit_index, 61);
                        }
                    }
                    if (data->tertiary)
                    {
                        long weapon_index = simulation_game_weapons_unit_weapon(unit_index, 1);
                        if (data->primary)
                        {
                            simulation_game_weapons_interrupt_action(unit_index, 61);
                            simulation_game_weapons_interrupt_action(unit_index, 62);
                            if (weapon_index != -1)
                                weapon_interrupt_reload(weapon_index, false);
                        }
                        else
                        {
                            if (weapon_index != -1 && !weapon_reloading(weapon_index, 1, NULL))
                            {
                                weapon_reset(weapon_index);
                                weapon_set_state(weapon_index, 0, true);
                            }
                            action_submit_event(unit_index, 63);
                        }
                    }
                    if (data->primary)
                    {
                        simulation_action_unit_reload_interrupt(unit_index, data->secondary, data->tertiary);
                        applied = true;
                    }
                    else
                    {
                        simulation_action_unit_reload(unit_index, data->secondary, data->tertiary);
                        applied = true;
                    }
                }
            }
        }
    }
    return applied;
}

// 0x4397a0
// @address 0x4397a0 ?write_description_to_string@c_simulation_weapon_drop_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_weapon_drop_event_definition::write_description_to_string(c_replication_outgoing_event const *event,
    s_simulation_view_telemetry_data const *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "weapon-drop : relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// @address 0x4398c8 ?apply_game_event@c_simulation_weapon_drop_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_weapon_drop_event_definition::apply_game_event(long event_index, long const *entity_reference, long payload_size, void const *payload)
{
    if (*entity_reference != -1)
    {
        long object_index = simulation_gamestate_entity_get_object_index(*entity_reference);
        if (object_index != -1)
        {
            s_object_header *header = object_header_get(object_index);
            if ((1 << header->object_type) & 0x1003)
            {
                unit_datum *object = (unit_datum *)header->datum;
                s_simulation_weapon_drop_event_data const *data = (s_simulation_weapon_drop_event_data const *)payload;
                bool disabled = (object->object.damage_flags >> 2) & 1;
                if (!disabled)
                {
                    if (data->unit_inventory_index == object->unit.current_weapon_indices[data->unit_weapon_set_index])
                    {
                        long child_index = object->unit.weapon_object_indices[data->unit_inventory_index];
                        if (child_index != -1 && data->weapon_definition_index == object_header_get(child_index)->datum->definition_index)
                            action_submit_event(object_index, (data->unit_weapon_set_index != 0) + 46);
                    }
                }
            }
        }
    }
    return false;
}

// 0x4399b0
// @address 0x4399b0 ?write_description_to_string@c_simulation_weapon_put_away_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_weapon_put_away_event_definition::write_description_to_string(c_replication_outgoing_event const *event,
    s_simulation_view_telemetry_data const *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "weapon-put-away : relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// @address 0x439a78 ?apply_game_event@c_simulation_weapon_put_away_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_weapon_put_away_event_definition::apply_game_event(long event_index, long const *entity_reference, long payload_size, void const *payload)
{
    if (*entity_reference != -1)
    {
        long object_index = simulation_gamestate_entity_get_object_index(*entity_reference);
        if (object_index != -1)
        {
            s_object_header *header = object_header_get(object_index);
            if ((1 << header->object_type) & 0x1003)
            {
                unit_datum *object = (unit_datum *)header->datum;
                s_simulation_weapon_put_away_event_data const *data = (s_simulation_weapon_put_away_event_data const *)payload;
                bool disabled = (object->object.damage_flags >> 2) & 1;
                if (!disabled)
                {
                    if (data->unit_inventory_index == object->unit.current_weapon_indices[data->unit_weapon_set_index])
                    {
                        long child_index = object->unit.weapon_object_indices[data->unit_inventory_index];
                        if (child_index != -1 && data->weapon_definition_index == object_header_get(child_index)->datum->definition_index)
                            ::unit_put_away_weapon(object_index, data->unit_weapon_set_index, 0);
                    }
                }
            }
        }
    }
    return false;
}

// 0x439b58
// @address 0x439b58 ?write_description_to_string@c_simulation_weapon_pickup_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_weapon_pickup_event_definition::write_description_to_string(c_replication_outgoing_event const *event,
    s_simulation_view_telemetry_data const *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "weapon-pickup : relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// 0x439bb8
// @address 0x439bb8 ?event_payload_encode@c_simulation_weapon_pickup_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_weapon_pickup_event_definition::event_payload_encode(long payload_size, void const *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_weapon_pickup_event_data const *data = (s_simulation_weapon_pickup_event_data const *)payload;
    s_unit_weapon_set const *weapon_set = &data->weapon_set;
    packet->write_integer("weapon-set-identifier", weapon_set->set_identifier + 1, 5);
    packet->write_integer("primary-weapon-index", weapon_set->weapon_indices[0] + 1, 3);
    packet->write_integer("secondary-weapon-index", weapon_set->weapon_indices[1] + 1, 3);
    packet->write_integer("add-weapon-type", data->unit_add_weapon_mode, 3);
}

// 0x439c30
// @address 0x439c30 ?event_payload_decode@c_simulation_weapon_pickup_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_weapon_pickup_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_weapon_pickup_event_data *data = (s_simulation_weapon_pickup_event_data *)payload;
    s_unit_weapon_set *weapon_set = &data->weapon_set;
    bool valid = true;
    weapon_set->set_identifier = packet->read_integer("weapon-set-identifier", 5) - 1;
    weapon_set->weapon_indices[0] = packet->read_integer("primary-weapon-index", 3) - 1;
    weapon_set->weapon_indices[1] = packet->read_integer("secondary-weapon-index", 3) - 1;
    data->unit_add_weapon_mode = packet->read_integer("add-weapon-type", 3);
    valid = valid && (weapon_set->weapon_indices[0] == -1 || (weapon_set->weapon_indices[0] >= 0 && weapon_set->weapon_indices[0] < 4));
    valid = valid && (weapon_set->weapon_indices[1] == -1 || (weapon_set->weapon_indices[1] >= 0 && weapon_set->weapon_indices[1] < 4));
    valid = valid && data->unit_add_weapon_mode >= 0 && data->unit_add_weapon_mode < 8;
    return valid;
}

// 0x439d38
struct unit_weapon_pickup_result;
enum e_weapon_addition_method;
namespace {
static __forceinline weapon_definition *simulation_game_weapons_weapon_get_definition(long weapon_index)
{
    item_datum *weapon = (item_datum *)object_get_and_verify_type(weapon_index, FLAG(2));
    return (weapon_definition *)tag_get('weap', weapon->object.definition_index);
}
static __forceinline bool simulation_game_weapons_weapon_must_be_readied(long weapon_index)
{
    return TEST_BIT(simulation_game_weapons_weapon_get_definition(weapon_index)->flags, 3);
}
static __forceinline long simulation_game_weapons_unit_get_current_weapon_in_set(long unit_index, long weapon_set_index)
{
    unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 0x1003);
    long weapon_index = NONE;
    signed char inventory_index = unit->unit.current_weapon_indices[weapon_set_index];
    if (inventory_index != NONE)
        weapon_index = unit->unit.weapon_object_indices[inventory_index];
    return weapon_index;
}
}

// @address 0x439d38 ?apply_game_event@c_simulation_weapon_pickup_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_weapon_pickup_event_definition::apply_game_event(long event_index, long const *gamestate_indices, long payload_size, void const *payload)
{
    s_simulation_weapon_pickup_event_data const *data = (s_simulation_weapon_pickup_event_data const *)payload;
    long unit_index = NONE;
    long weapon_index = NONE;
    bool success = false;
    if (gamestate_indices[0] != NONE)
        unit_index = simulation_gamestate_entity_get_object_index(gamestate_indices[0]);
    if (gamestate_indices[1] != NONE)
        weapon_index = simulation_gamestate_entity_get_object_index(gamestate_indices[1]);
    if (unit_index != NONE && weapon_index != NONE)
    {
        if ((FLAG(object_header_get(unit_index)->object_type) & 0x1003) && (FLAG(object_header_get(weapon_index)->object_type) & FLAG(2)))
        {
            unit_datum *unit = (unit_datum *)object_get_and_verify_type(unit_index, 0x1003);
            item_datum *weapon = (item_datum *)object_get_and_verify_type(weapon_index, FLAG(2));
            weapon_definition *definition = (weapon_definition *)tag_get('weap', weapon->object.definition_index);
            bool dead = (unit->object.damage_flags >> 2) & 1;
            if (!dead && weapon->toss_unit_index != unit_index && weapon->owner_relation == 0)
            {
                if (memcmp(&unit->unit.current_weapon_set, &data->weapon_set, sizeof(s_unit_weapon_set)) == 0)
                {
                    bool primary;
                    long weapon_set_index;
                    switch (data->unit_add_weapon_mode)
                    {
                    case 5:
                    case 6:
                    case 7:
                        primary = false;
                        weapon_set_index = 1;
                        break;
                    default:
                        primary = true;
                        weapon_set_index = 0;
                        break;
                    }
                    long current_weapon_index = simulation_game_weapons_unit_get_current_weapon_in_set(unit_index, weapon_set_index);
                    if (current_weapon_index != NONE && simulation_game_weapons_weapon_must_be_readied(current_weapon_index))
                        action_submit_event(unit_index, !primary + _action_primary_weapon_drop);
                    unit_weapon_pickup_result pickup_result;
                    short resource_mask;
                    if (unit_can_pickup_weapon(unit_index, weapon_index, (e_weapon_addition_method)data->unit_add_weapon_mode, &pickup_result)
                        && unit_test_pickup_with_mode(unit_index, &pickup_result, (e_weapon_addition_method)data->unit_add_weapon_mode, &resource_mask)
                        && motor_task_allow(unit_index, _action_weapon_pickup, resource_mask))
                    {
                        long weapon_class = definition->animation_weapon_class;
                        bool definition_must_be_readied = simulation_game_weapons_weapon_must_be_readied(weapon_index);
                        bool definition_is_support_weapon = TEST_BIT(simulation_game_weapons_weapon_get_definition(weapon_index)->flags, 13);
                        bool readied_or_support = definition_must_be_readied || definition_is_support_weapon;
                        bool must_be_readied = readied_or_support
                            && unit->object.parent_object_index == NONE;
                        if (data->weapon_set.weapon_indices[primary ? 1 : 0] != NONE)
                            weapon_class = 486;
                        if (unit_animation_test_weapon(unit_index, 226, weapon_class, definition->animation_weapon_type, must_be_readied))
                        {
                            if (distance_squared3d(&unit->object.bounding_center, &weapon->object.bounding_center) < 2.0f * 2.0f)
                            {
                                action_request request;
                                memset(&request, 0, sizeof(request));
                                memset(&request, 0, sizeof(request));
                                request.type = 44;
                                request.weapon = weapon_index;
                                request.mode = data->unit_add_weapon_mode;
                                if (action_submit(unit_index, &request))
                                    success = true;
                            }
                        }
                    }
                }
            }
        }
    }
    return success;
}

// 0x43a0b0
long c_simulation_weapon_effect_event_definition::minimum_required_bits(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *)
{
    return 37;
}

// 0x43a0b8
// @address 0x43a0b8 ?write_description_to_string@c_simulation_weapon_empty_click_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_weapon_empty_click_event_definition::write_description_to_string(c_replication_outgoing_event const *event,
    s_simulation_view_telemetry_data const *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "weapon-effect : relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

union real_point3d;
union real_rectangle3d;

// @address 0x43a118 ?event_payload_encode@c_simulation_weapon_effect_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_weapon_effect_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_weapon_effect_event_data const *data = (s_simulation_weapon_effect_event_data const *)payload;
    packet->write_integer("weapon-effect-type", data->effect_type, 1);
    simulation_write_quantized_position(packet, &data->position, 12, true, 0);
}

// @address 0x43a168 ?event_payload_decode@c_simulation_weapon_effect_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_weapon_effect_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *stream, bool quantize)
{
    s_simulation_weapon_effect_event_data *data = (s_simulation_weapon_effect_event_data *)payload;
    unsigned long decoded_effect_type;
    decoded_effect_type = stream->read_integer("weapon-effect-type", 1);
    data->effect_type = decoded_effect_type;
    ::simulation_read_quantized_position(stream, &data->position, 12, 0);
    return (unsigned long)data->effect_type < 2;
}

// 0x43a428
// @address 0x43a428 ?event_payload_encode@c_simulation_weapon_empty_click_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_weapon_empty_click_event_definition::event_payload_encode(long payload_size, void const *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_weapon_empty_click_event_data const *data = (s_simulation_weapon_empty_click_event_data const *)payload;
    packet->write_integer("unit-inventory-index", data->unit_inventory_index + 1, 3);
    simulation_write_definition_index("weapon-definition", packet, data->weapon_definition_index);
}

// 0x43a480
// @address 0x43a480 ?event_payload_decode@c_simulation_weapon_empty_click_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_weapon_empty_click_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool quantize)
{
    s_simulation_weapon_empty_click_event_data *data = (s_simulation_weapon_empty_click_event_data *)payload;
    bool valid = true;
    data->unit_inventory_index = packet->read_integer("unit-inventory-index", 3) - 1;
    if (data->unit_inventory_index != -1)
        valid = valid && data->unit_inventory_index >= 0 && data->unit_inventory_index < 4;
    data->weapon_definition_index = simulation_read_definition_index("weapon-definition", packet);
    return valid;
}

// 0x43a4f8
static __forceinline void simulation_game_weapons_weapon_effect_fire_empty_click(long weapon_index, long definition_index)
{
    weapon_definition *definition =
        (weapon_definition *)tag_get('weap', definition_index);
    if (definition->barrel_count > 0)
    {
        s_weapon_barrel_definition *barrel = definition->barrels;
        if (barrel->firing_effect_count > 0)
        {
            long effect_index = barrel->firing_effects[0].effects[2].index;
            if (effect_index != -1)
                weapon_effect_new(weapon_index, -1, effect_index, 1.0f, 0.0f);
        }
    }
}

// @address 0x43a4f8 ?apply_game_event@c_simulation_weapon_empty_click_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_weapon_empty_click_event_definition::apply_game_event(long event_index, long const *entity_reference, long payload_size, void const *payload)
{
    if (*entity_reference != -1)
    {
        long unit_index = simulation_gamestate_entity_get_object_index(*entity_reference);
        if (unit_index != -1)
        {
            s_object_header *header = object_header_get(unit_index);
            if ((1 << header->object_type) & 0x1003)
            {
                s_simulation_weapon_empty_click_event_data const *data = (s_simulation_weapon_empty_click_event_data const *)payload;
                if (data->unit_inventory_index >= 0 && data->unit_inventory_index < 4)
                {
                    unit_datum *unit = (unit_datum *)header->datum;
                    bool dead = (unit->object.damage_flags >> 2) & 1;
                    if (!dead && unit->unit.player_index != -1)
                    {
                        long weapon_index = unit->unit.weapon_object_indices[data->unit_inventory_index];
                        if (weapon_index != -1)
                        {
                            long definition_index = object_header_get(weapon_index)->datum->definition_index;
                            if (data->weapon_definition_index == definition_index)
                            {
                                simulation_game_weapons_weapon_effect_fire_empty_click(weapon_index, definition_index);
                                simulation_action_weapon_empty_click(weapon_index);
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

// @address 0x43a748 ?initial_update_mask@c_simulation_item_entity_definition@@UAAKXZ
unsigned long c_simulation_item_entity_definition::initial_update_mask()
{
    return c_simulation_item_state_helpers::initial_update_mask() | 0x20000 | 0x8000 | 0x10000;
}

// 0x5bec98
// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_weapon_empty_click_event_definition@@UAAXJPAX@Z
void c_simulation_weapon_empty_click_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

void c_message::initialize()
{
}

void c_message::update()
{
}

void c_network_session_parameter_base::notify_set_update_required()
{
}

void c_network_session_parameter_base::notify_set_change_request()
{
}

void c_network_session_parameter_base::notify_update_payload_sent_to_peer(long peer_index)
{
}

void c_network_session_parameter_base::notify_change_request_payload_sent()
{
}

void c_game_engine_base_variant::custom_team_score_stats(long team_index, long old_score, long new_score) const
{
}


// 0x4381b0: the "weapon creation" debug line with the object's relevance

// @address 0x4381b0 ?write_creation_description_to_string@c_simulation_weapon_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@JPAD@Z
void c_simulation_weapon_entity_definition::write_creation_description_to_string(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long size, char *buffer)
{
    float relevance = 0.0f;
    simulation_calculate_entity_creation_priority(entity, telemetry, &relevance);
    csnzprintf(buffer, size, "weapon creation: relevance=%5.3f", relevance);
}

// @address 0x435e00 ?build_baseline_state_data@c_simulation_projectile_entity_definition@@UAA_NJPBXJPAX@Z
bool c_simulation_projectile_entity_definition::build_baseline_state_data(long, void const *creation_data, long, void *state_data)
{
    bool built = false;
    memset(state_data, 0, offsetof(s_simulation_item_state_data, equipment_creator_player_index));
    if (c_simulation_object_entity_definition::object_build_baseline_state_data(
            (const s_simulation_object_creation_data *)creation_data, (s_simulation_object_state_data *)state_data))
    {
        ((s_simulation_item_state_data *)state_data)->at_rest = false;
        built = true;
    }
    return built;
}

// @address 0x438220 ?entity_creation_encode@c_simulation_weapon_entity_definition@@UAAXJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@_N@Z
void c_simulation_weapon_entity_definition::entity_creation_encode(
    long, const void *creation_data, const s_simulation_view_telemetry_data *, c_bitstream *packet, bool quantize)
{
    c_simulation_object_entity_definition::object_creation_encode((const s_simulation_object_creation_data *)creation_data, packet, quantize);
    packet->write_integer("simulation-weapon-identifier", (short)((const s_simulation_weapon_creation_data *)creation_data)->simulation_weapon_identifier, 12);
}

bool c_simulation_weapon_entity_definition::entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet, bool quantize)
{
    bool decoded = c_simulation_item_entity_definition::entity_creation_decode(16, creation_data, packet, quantize);
    ((s_simulation_weapon_creation_data *)creation_data)->simulation_weapon_identifier = (unsigned short)packet->read_integer(
        "simulation-weapon-identifier", 12);
    bool overrun = packet->m_current.current_stream_bit_position > (packet->m_data_size_bytes << 3);
    return !overrun && decoded;
}


// @address 0x4386a0 ?entity_state_lossy_compare@c_simulation_weapon_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_weapon_entity_definition::entity_state_lossy_compare(void *first, void *second, long state_data_size)
{
    s_simulation_weapon_update_state *first_state = (s_simulation_weapon_update_state *)first;
    s_simulation_weapon_update_state *second_state = (s_simulation_weapon_update_state *)second;
    bool equal = c_simulation_object_entity_definition::object_state_compare((s_simulation_object_state_data *)first, (s_simulation_object_state_data *)second);
    equal = equal && c_bitstream::compare_quantized_reals(first_state->age, second_state->age, k_zero, k_one, 7, true, false);
    first_state->age = second_state->age = k_zero;
    return equal;
}

// @address 0x43a1d8 ?apply_game_event@c_simulation_weapon_effect_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_weapon_effect_event_definition::apply_game_event(long event_index, long const *gamestate_indices, long payload_size, void const *payload)
{
    if (*gamestate_indices != -1)
    {
        long weapon_index = simulation_gamestate_entity_get_object_index(*gamestate_indices);
        if (weapon_index != -1 && simulation_game_weapons_multiplayer_type(weapon_index) == 1)
        {
            s_simulation_weapon_effect_event_data const *data = (s_simulation_weapon_effect_event_data const *)payload;
            s_multiplayer_globals_definition *multiplayer_globals =
                (s_multiplayer_globals_definition *)tag_get('mulg', global_game_globals->multiplayer_globals_index);
            s_multiplayer_runtime_globals_definition *runtime = (s_multiplayer_runtime_globals_definition *)multiplayer_globals->runtime.address;
            s_multiplayer_constants *runtime_data = (s_multiplayer_constants *)runtime->multiplayer_constants.address;
            long effect_index = -1;
            switch (data->effect_type)
            {
            case 0:
                effect_index = runtime_data->bomb_explosion_effect.index;
                break;
            case 1:
                effect_index = runtime_data->bomb_reset_effect.index;
                break;
            default:
                return false;
            }
            if (effect_index != -1)
            {
                s_effect_vector vectors[5];
                vectors[0].point = vectors[1].point = vectors[2].point = vectors[3].point = vectors[4].point = data->position;
                vectors[0].vector = *global_up3d;
                vectors[1].vector = *global_down3d;
                vectors[2].vector = *global_up3d;
                vectors[3].vector.i = -0.0f;
                vectors[3].vector.j = -0.0f;
                vectors[3].vector.k = -1.0f;
                vectors[4].vector = *global_up3d;
                vectors[0].marker_name = _string_id_forward;
                vectors[1].marker_name = _string_id_gravity;
                vectors[2].marker_name = _string_id_up;
                vectors[3].marker_name = _string_id_backward;
                vectors[4].marker_name = _string_id_root;
                s_effect_creation_data creation_data;
                creation_data.flags = 0;
                effect_creation_data_initialize(&creation_data);
                creation_data.definition_index = effect_index;
                creation_data.plane = 0;
                creation_data.damage_reporting_type = 0;
                creation_data.effect_vector_count = 5;
                creation_data.effect_vectors = vectors;
                creation_data.flags = 4;
                effect_new_from_creation_data(&creation_data);
            }
        }
    }
    return false;
}




static __forceinline s_weapon_barrel_definition *simulation_game_weapons_fire_barrel_definition_get(
    const weapon_definition *definition, long barrel_index)
{
    return (s_weapon_barrel_definition *)tag_block_get_element_with_size(
        (const s_tag_block *)&definition->barrel_count, barrel_index, sizeof(s_weapon_barrel_definition));
}

static __forceinline bool simulation_game_weapons_barrel_has_maximum_shots(long weapon_index, short barrel_index)
{
    bool result = false;
    weapon_datum *weapon = (weapon_datum *)object_get_and_verify_type(weapon_index, _object_mask_weapon);
    weapon_definition *definition = (weapon_definition *)tag_get('weap', weapon->item.object.definition_index);
    short maximum_shots = definition->barrels[barrel_index].shots_maximum;
    if (maximum_shots != 0 && weapon->barrels[barrel_index].value >= maximum_shots)
        result = true;
    return result;
}

static __forceinline void simulation_game_weapons_barrel_set_recovery_delay(long weapon_index, short barrel_index)
{
    weapon_datum *weapon = (weapon_datum *)object_get_and_verify_type(weapon_index, _object_mask_weapon);
    s_weapon_barrel *barrel = &weapon->barrels[barrel_index];
    long ticks = (long)(((float)tls_game_time_globals()->ticks_per_second * 0.5f) + 0.5f);
    if (ticks > 255)
        ticks = 255;
    barrel->recovery_delay_ticks = (unsigned char)ticks;
}

// @address 0x438e60
// @address 0x438e60 ?apply_game_event@c_simulation_weapon_fire_event_definition@@UAA_NJPBJJPBX@Z
bool c_simulation_weapon_fire_event_definition::apply_game_event(
    long reference_gamestate_count, long const *gamestate_indices, long payload_size, void const *payload)
{
    const s_simulation_weapon_fire_event_data *weapon_fire_data = (const s_simulation_weapon_fire_event_data *)payload;
    long object_index = -1;
    bool result = false;
    if (*gamestate_indices != -1)
        object_index = simulation_gamestate_entity_get_object_index(*gamestate_indices);
    if (object_index != -1)
    {
        s_object_header *headers = (s_object_header *)tls_object_header_data()->data;
        long object_type = object_header_get(object_index)->object_type;
        long gunner_unit_index = -1;
        long weapon_index = -1;
        if (object_type == 2)
            weapon_index = object_index;
        else if ((1 << object_type) & 0x1003)
        {
            if (weapon_fire_data->unit_inventory_index >= 0 && weapon_fire_data->unit_inventory_index < 4)
            {
                unit_datum *unit = (unit_datum *)headers[datum_index_to_absolute_index(object_index)].datum;
                if (!(bool)((unit->object.damage_flags >> 2) & 1))
                {
                    bool ready = false;
                    for (long weapon_slot = 0; weapon_slot < 2; ++weapon_slot)
                    {
                        if (unit->unit.current_weapon_indices[weapon_slot] == weapon_fire_data->unit_inventory_index)
                        {
                            ready = true;
                            break;
                        }
                    }
                    if (ready)
                    {
                        weapon_index = unit->unit.weapon_object_indices[weapon_fire_data->unit_inventory_index];
                        gunner_unit_index = unit->unit.gunner_object_index != -1 ? unit->unit.gunner_object_index : object_index;
                    }
                }
            }
        }
        if (weapon_index != -1)
        {
            weapon_datum *weapon = (weapon_datum *)headers[datum_index_to_absolute_index(weapon_index)].datum;
            weapon_definition *definition = (weapon_definition *)tag_get('weap', weapon->item.object.definition_index);
            if (weapon_fire_data->weapon_definition_index == weapon->item.object.definition_index)
            {
                long barrel_index = weapon_fire_data->barrel_index;
                if (barrel_index >= 0 && barrel_index < definition->barrel_count)
                {
                    bool fired = false;
                    long predicted_fire_count = 0;
                    c_static_array<s_predicted_weapon_fire_data, 4> predicted_fire_data;
                    memset(&predicted_fire_data, 0, sizeof(predicted_fire_data));
                    if (gunner_unit_index != -1)
                    {
                        unit_datum *gunner_unit = (unit_datum *)headers[datum_index_to_absolute_index(gunner_unit_index)].datum;
                        if (weapon_fire_data->aim_assist.target_object_index != -1)
                            memcpy(&gunner_unit->unit.targeting, &weapon_fire_data->aim_assist, sizeof(gunner_unit->unit.targeting));
                        const s_weapon_barrel_definition *barrel_definition =
                            simulation_game_weapons_fire_barrel_definition_get(definition, weapon_fire_data->barrel_index);
                        if (barrel_definition->barrel_flags.test(_weapon_barrel_tracks_target) && weapon_fire_data->tracked_data_valid)
                        {
                            object_index = simulation_gamestate_entity_get_object_index(weapon_fire_data->tracked_gamestate_index);
                            long tracked_object_index = object_index;
                            if (tracked_object_index != -1 && weapon_fire_data->tracked_model_target_index != -1)
                            {
                                weapon->tracked_model_target_index = (signed char)weapon_fire_data->tracked_model_target_index;
                                weapon->tracked_object_index = tracked_object_index;
                            }
                        }
                        for (long projectile_index = 0; projectile_index < weapon_fire_data->projectile_count; ++projectile_index)
                        {
                            if ((unsigned long)predicted_fire_count < 4)
                            {
                                const s_simulation_weapon_fire_projectile *projectile = &weapon_fire_data->projectiles[projectile_index];
                                if (projectile->projectile_id >= 0 && projectile->projectile_id < 4 && (projectile->conical_valid || projectile->impact_valid))
                                {
                                    s_predicted_weapon_fire_data *predicted = &predicted_fire_data[predicted_fire_count];
                                    if (projectile->impact_valid && projectile->target_index >= 0 &&
                                        projectile->target_index < weapon_fire_data->impact_target_count)
                                    {
                                        const s_simulation_weapon_fire_impact_target *target = &weapon_fire_data->impact_targets[projectile->target_index];
                                        if (target->gamestate_index != -1)
                                        {
                                            long target_object_index = simulation_gamestate_entity_get_object_index(target->gamestate_index);
                                            if (target_object_index != -1)
                                            {
                                                object_datum *target_object =
                                                    (object_datum *)object_get_and_verify_type(
                                                        target_object_index, ~0UL);
                                                long node_count = target_object->node_matrices.size / sizeof(real_matrix4x3);
                                                real_matrix4x3 *node_matrices = (real_matrix4x3 *)((char *)target_object + target_object->node_matrices.offset);
                                                if (projectile->object_node_index >= 0 && projectile->object_node_index < node_count)
                                                {
                                                    matrix4x3_transform_point(&node_matrices[projectile->object_node_index],
                                                        &projectile->impact_position, &predicted->impact_position);
                                                    predicted->impact_valid = true;
                                                }
                                            }
                                        }
                                    }
                                    if (projectile->conical_valid)
                                    {
                                        predicted->conical_random_seed = projectile->conical_random_seed;
                                        predicted->conical_valid = true;
                                    }
                                    if (predicted->impact_valid || predicted->conical_valid)
                                    {
                                        predicted->projectile_id = projectile->projectile_id;
                                        ++predicted_fire_count;
                                    }
                                }
                            }
                        }
                        unit_set_facing(gunner_unit_index, &weapon_fire_data->aiming_vector, &weapon_fire_data->aiming_vector);
                    }
                    weapon_interrupt_reload_and_clear_timer(weapon_index, 1, true);
                    if (!weapon_fire_data->generated_from_host_live_fire)
                    {
                        bool predicted = false;
                        if (tls_game_globals()->options.game_simulation == 4)
                            predicted = true;
                        long shots = 0;
                        long recovery_ticks = -1;
                        long recovery_duration = -1;
                        long firing_ticks = -1;
                        long firing_duration = -1;
                        if (weapon_barrel_is_ready_to_fire(weapon_index, (short)weapon_fire_data->barrel_index, &shots, &recovery_ticks,
                                &recovery_duration, &firing_ticks, &firing_duration))
                        {
                            weapon_barrel_fire(weapon_index, (short)weapon_fire_data->barrel_index, predicted);
                            if (weapon_fire_data->primary_trigger ||
                                simulation_game_weapons_barrel_has_maximum_shots(weapon_index, (short)weapon_fire_data->barrel_index))
                                weapon_barrel_recover(weapon_index, (short)weapon_fire_data->barrel_index, false);
                            else
                                simulation_game_weapons_barrel_set_recovery_delay(weapon_index, (short)weapon_fire_data->barrel_index);
                            fired = true;
                        }
                    }
                    else
                        fired = true;
                    if (fired)
                    {
                        weapon_barrel_add_fire_error(weapon_index, weapon_fire_data->barrel_index);
                        long forced_barrel_index = -1;
                        if (weapon_fire_data->generated_from_host_live_fire)
                            forced_barrel_index = weapon_fire_data->barrel_index;
                        weapon_fire_barrels(weapon_index, predicted_fire_data.get_elements(), predicted_fire_count, true, forced_barrel_index);
                        result = true;
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x42e7c0 ?entity_type@c_simulation_weapon_entity_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_weapon_entity_definition::entity_type()
{
    return _simulation_entity_type_weapon;
}

typedef char simulation_weapon_creation_size[(sizeof(s_simulation_weapon_creation_data) == 20) ? 1 : -1];
// @address 0x42e7c0 ?creation_data_size@c_simulation_weapon_entity_definition@@UAAJXZ
long c_simulation_weapon_entity_definition::creation_data_size()
{
    return sizeof(s_simulation_weapon_creation_data);
}

// @address 0x42e7e8 ?update_flag_count@c_simulation_weapon_entity_definition@@UAAJXZ
long c_simulation_weapon_entity_definition::update_flag_count()
{
    return 21;
}

// @address 0x42d260 ?event_type@c_simulation_weapon_fire_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_weapon_fire_event_definition::event_type()
{
    return _simulation_event_type_weapon_fire;
}

// @address 0x42d380 ?number_of_entity_references@c_simulation_weapon_fire_event_definition@@UAAJXZ
long c_simulation_weapon_fire_event_definition::number_of_entity_references()
{
    return 1;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_weapon_fire_event_definition@@UAA_NXZ
bool c_simulation_weapon_fire_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_weapon_fire_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_weapon_fire_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_weapon_fire_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_weapon_fire_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x437a70 ?event_requirements@c_simulation_weapon_fire_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_weapon_fire_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = c_simulation_weapon_fire_event_definition::minimum_required_bits(event, telemetry);
    return requirements;
}

// @address 0x42e8a0 ?event_type@c_simulation_weapon_reload_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_weapon_reload_event_definition::event_type()
{
    return _simulation_event_type_weapon_reload;
}

// @address 0x45fc28 ?payload_size@c_simulation_weapon_reload_event_definition@@UAAJXZ
long c_simulation_weapon_reload_event_definition::payload_size()
{
    return sizeof(s_simulation_weapon_reload_event_data);
}

// @address 0x42d380 ?number_of_entity_references@c_simulation_weapon_reload_event_definition@@UAAJXZ
long c_simulation_weapon_reload_event_definition::number_of_entity_references()
{
    return 1;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_weapon_reload_event_definition@@UAA_NXZ
bool c_simulation_weapon_reload_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_weapon_reload_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_weapon_reload_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x45fc28 ?minimum_required_bits@c_simulation_weapon_reload_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_weapon_reload_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 3;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_weapon_reload_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_weapon_reload_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x437ad8 ?event_requirements@c_simulation_weapon_reload_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_weapon_reload_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 3;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_weapon_reload_event_definition@@UAAXJPAX@Z
void c_simulation_weapon_reload_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x45fca8 ?event_type@c_simulation_weapon_pickup_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_weapon_pickup_event_definition::event_type()
{
    return _simulation_event_type_weapon_pickup;
}

// @address 0x42e650 ?payload_size@c_simulation_weapon_pickup_event_definition@@UAAJXZ
long c_simulation_weapon_pickup_event_definition::payload_size()
{
    return sizeof(s_simulation_weapon_pickup_event_data);
}

// @address 0x3fc598 ?number_of_entity_references@c_simulation_weapon_pickup_event_definition@@UAAJXZ
long c_simulation_weapon_pickup_event_definition::number_of_entity_references()
{
    return 2;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_weapon_pickup_event_definition@@UAA_NXZ
bool c_simulation_weapon_pickup_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_weapon_pickup_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_weapon_pickup_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42e8a0 ?minimum_required_bits@c_simulation_weapon_pickup_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_weapon_pickup_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 14;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_weapon_pickup_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_weapon_pickup_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x437af8 ?event_requirements@c_simulation_weapon_pickup_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_weapon_pickup_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 2;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 14;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_weapon_pickup_event_definition@@UAAXJPAX@Z
void c_simulation_weapon_pickup_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x433b30 ?event_type@c_simulation_weapon_put_away_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_weapon_put_away_event_definition::event_type()
{
    return _simulation_event_type_weapon_put_away;
}

// @address 0x42e650 ?payload_size@c_simulation_weapon_put_away_event_definition@@UAAJXZ
long c_simulation_weapon_put_away_event_definition::payload_size()
{
    return sizeof(s_simulation_weapon_put_away_event_data);
}

// @address 0x42d380 ?number_of_entity_references@c_simulation_weapon_put_away_event_definition@@UAAJXZ
long c_simulation_weapon_put_away_event_definition::number_of_entity_references()
{
    return 1;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_weapon_put_away_event_definition@@UAA_NXZ
bool c_simulation_weapon_put_away_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_weapon_put_away_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_weapon_put_away_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42e748 ?minimum_required_bits@c_simulation_weapon_put_away_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_weapon_put_away_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 16;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_weapon_put_away_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_weapon_put_away_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x437b30 ?event_requirements@c_simulation_weapon_put_away_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_weapon_put_away_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 16;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_weapon_put_away_event_definition@@UAAXJPAX@Z
void c_simulation_weapon_put_away_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x439a10 ?event_payload_encode@c_simulation_weapon_put_away_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_weapon_put_away_event_definition::event_payload_encode(long payload_size, void const *payload, c_bitstream *packet,
    bool quantize)
{
    s_simulation_weapon_put_away_event_data const *data = (s_simulation_weapon_put_away_event_data const *)payload;
    packet->write_integer("unit-weapon-set-index", data->unit_weapon_set_index, 1);
    packet->write_integer("unit-inventory-index", data->unit_inventory_index, 3);
    simulation_write_definition_index("weapon-definition", packet, data->weapon_definition_index);
}

// @address 0x439800 ?event_payload_decode@c_simulation_weapon_put_away_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_weapon_put_away_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet,
    bool quantize)
{
    s_simulation_weapon_put_away_event_data *data = (s_simulation_weapon_put_away_event_data *)payload;
    data->unit_weapon_set_index = packet->read_integer("unit-weapon-set-index", 1);
    data->unit_inventory_index = packet->read_integer("unit-inventory-index", 3);
    data->weapon_definition_index = simulation_read_definition_index("weapon-definition", packet);
    bool valid = true;
    valid = valid && data->unit_weapon_set_index >= 0 && data->unit_weapon_set_index < 2;
    valid = valid && data->unit_inventory_index >= 0 && data->unit_inventory_index < 4;
    return valid;
}

// @address 0x42e650 ?payload_size@c_simulation_weapon_drop_event_definition@@UAAJXZ
long c_simulation_weapon_drop_event_definition::payload_size()
{
    return sizeof(s_simulation_weapon_drop_event_data);
}

// @address 0x42d380 ?number_of_entity_references@c_simulation_weapon_drop_event_definition@@UAAJXZ
long c_simulation_weapon_drop_event_definition::number_of_entity_references()
{
    return 1;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_weapon_drop_event_definition@@UAA_NXZ
bool c_simulation_weapon_drop_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_weapon_drop_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_weapon_drop_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42e748 ?minimum_required_bits@c_simulation_weapon_drop_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_weapon_drop_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 16;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_weapon_drop_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_weapon_drop_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x437b30 ?event_requirements@c_simulation_weapon_drop_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_weapon_drop_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 16;
    return requirements;
}

// @address 0x439a10 ?event_payload_encode@c_simulation_weapon_drop_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
void c_simulation_weapon_drop_event_definition::event_payload_encode(long payload_size, const void *payload, c_bitstream *packet,
    bool quantize)
{
    s_simulation_weapon_drop_event_data const *data = (s_simulation_weapon_drop_event_data const *)payload;
    packet->write_integer("unit-weapon-set-index", data->unit_weapon_set_index, 1);
    packet->write_integer("unit-inventory-index", data->unit_inventory_index, 3);
    simulation_write_definition_index("weapon-definition", packet, data->weapon_definition_index);
}

// @address 0x439800 ?event_payload_decode@c_simulation_weapon_drop_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_weapon_drop_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet,
    bool quantize)
{
    s_simulation_weapon_drop_event_data *data = (s_simulation_weapon_drop_event_data *)payload;
    data->unit_weapon_set_index = packet->read_integer("unit-weapon-set-index", 1);
    data->unit_inventory_index = packet->read_integer("unit-inventory-index", 3);
    data->weapon_definition_index = simulation_read_definition_index("weapon-definition", packet);
    bool valid = true;
    valid = valid && data->unit_weapon_set_index >= 0 && data->unit_weapon_set_index < 2;
    valid = valid && data->unit_inventory_index >= 0 && data->unit_inventory_index < 4;
    return valid;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_weapon_drop_event_definition@@UAAXJPAX@Z
void c_simulation_weapon_drop_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x45fc50 ?event_type@c_simulation_weapon_effect_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_weapon_effect_event_definition::event_type()
{
    return _simulation_event_type_weapon_effect;
}

typedef char simulation_weapon_effect_size[(sizeof(s_simulation_weapon_effect_event_data) == 16) ? 1 : -1];
// @address 0x42e748 ?payload_size@c_simulation_weapon_effect_event_definition@@UAAJXZ
long c_simulation_weapon_effect_event_definition::payload_size()
{
    return sizeof(s_simulation_weapon_effect_event_data);
}

// @address 0x42d380 ?number_of_entity_references@c_simulation_weapon_effect_event_definition@@UAAJXZ
long c_simulation_weapon_effect_event_definition::number_of_entity_references()
{
    return 1;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_weapon_effect_event_definition@@UAA_NXZ
bool c_simulation_weapon_effect_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_weapon_effect_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_weapon_effect_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_weapon_effect_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_weapon_effect_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x43a0b8 ?write_description_to_string@c_simulation_weapon_effect_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_weapon_effect_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "weapon-effect : relevance=%5.3f", calculate_relevance(event, telemetry, relevance));
}

// @address 0x437b50 ?event_requirements@c_simulation_weapon_effect_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_weapon_effect_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 37;
    return requirements;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_weapon_effect_event_definition@@UAAXJPAX@Z
void c_simulation_weapon_effect_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

// @address 0x426e90 ?event_type@c_simulation_weapon_empty_click_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_weapon_empty_click_event_definition::event_type()
{
    return _simulation_event_type_weapon_empty_click;
}

typedef char simulation_weapon_empty_click_size[(sizeof(s_simulation_weapon_empty_click_event_data) == 8) ? 1 : -1];
// @address 0x42e650 ?payload_size@c_simulation_weapon_empty_click_event_definition@@UAAJXZ
long c_simulation_weapon_empty_click_event_definition::payload_size()
{
    return sizeof(s_simulation_weapon_empty_click_event_data);
}

// @address 0x42d380 ?number_of_entity_references@c_simulation_weapon_empty_click_event_definition@@UAAJXZ
long c_simulation_weapon_empty_click_event_definition::number_of_entity_references()
{
    return 1;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_weapon_empty_click_event_definition@@UAA_NXZ
bool c_simulation_weapon_empty_click_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x42d380 ?event_can_be_transmitted@c_simulation_weapon_empty_click_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_weapon_empty_click_event_definition::event_can_be_transmitted(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42e678 ?minimum_required_bits@c_simulation_weapon_empty_click_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_weapon_empty_click_event_definition::minimum_required_bits(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 15;
}

// @address 0x4311b8 ?calculate_relevance@c_simulation_weapon_empty_click_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_weapon_empty_click_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return simulation_calculate_event_relevance(this, event, relevance, telemetry, 0);
}

// @address 0x437b70 ?event_requirements@c_simulation_weapon_empty_click_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
s_simulation_event_requirements c_simulation_weapon_empty_click_event_definition::event_requirements(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 1;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 15;
    return requirements;
}
