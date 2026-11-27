// networkinglibxbox/simulation_game_items.obj: from the records 0x433b18 0x433b28 0x433b30 0x433b38 0x433ba8 0x433d68 0x433df8 0x433fa0 0x433fc0 0x433fc8 0x434038 0x4340d0 0x434108 0x434118 0x4342c0 0x43a758 0x43ae28 0x709f38 0x70c808
// @flags /TP /O2 /Oc
#include <string.h>
#include <ppcintrinsics.h>
#include "networking/simulation/simulation_entity_definition.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "objects/object_types.h"
#include "objects/object_placement.h"
#include "objects/objects.h"
#include "cache/cache_files.h"
#include "items/equipment.h"
#include "simulation/game_interface/simulation_game_object_data_types.h"
#include "simulation/game_interface/simulation_game_object_entity_definition.h"
#include "simulation/game_interface/simulation_game_items.h"
#include "simulation/simulation_priority.h"
#include "memory/data_types.h"
#include "game/game_time_inline.h"
#include "cseries/cseries.h"
#include "animations/animation_manager.h"
#include "memory/tls_data_arrays.h"
#include "memory/bitstream.h"
#include "game/player_types.h"
#include "memory/tls_globals.h"
#include "items/item_types.h"
#include "objects/object_datum.h"
#include "objects/object_header_access.h"
#include "memory/datum_index_inlines.h"
#include "simulation/game_interface/simulation_game_object_creation_inline.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "simulation/simulation_entity_types.h"

// @address 0x48f68 ??_7c_simulation_item_entity_definition@@6B@
// @address 0x709f38 ??__Eg_simulation_item_entity_definition@@YAXXZ
// @address 0x70c808 ??__Fg_simulation_item_entity_definition@@YAXXZ
// @address 0x7bb4bc g_simulation_item_entity_definition
c_simulation_item_entity_definition g_simulation_item_entity_definition;

// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x446e00 ?entity_is_active@c_simulation_view_telemetry_provider@@QBA_NJ@Z
// @address 0x433b18 ?entity_type_name@c_simulation_item_entity_definition@@UAAPBDXZ
// @address 0x433b38 ?build_baseline_state_data@c_simulation_item_entity_definition@@UAA_NJPBXJPAX@Z
// @address 0x434038 ?update_minimum_required_bits@c_simulation_item_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
// @address 0x4340d0 ?entity_update_encode@c_simulation_item_entity_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
// @address 0x434108 ?entity_update_decode@c_simulation_item_entity_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
// @address 0x433ba8 ?handle_object_update@c_simulation_item_entity_definition@@UAAKJKJPAX@Z
// @address 0x446e88 ?make_room_for_update@c_entity_update_encode_helper@@QAA_NPAVc_bitstream@@JJJK@Z
// @address 0x446ef8 ?write_component_header@c_entity_update_encode_helper@@QAA_NJPBD@Z
// @address 0x446f80 ?finish_component@c_entity_update_encode_helper@@QAAXXZ
// @address 0x40b128 ?object_build_baseline_state_data@c_simulation_object_entity_definition@@KA_NPBUs_simulation_object_creation_data@@PAUs_simulation_object_state_data@@@Z
// @address 0x40c8e0 ?object_creation_decode@c_simulation_object_entity_definition@@KA_NPAUs_simulation_object_creation_data@@PAVc_bitstream@@_N@Z
// @address 0x40cb38 ?object_update_encode@c_simulation_object_entity_definition@@KA_N_NKPAKPBUs_simulation_view_telemetry_data@@PBUs_simulation_object_state_data@@PAVc_bitstream@@J00@Z
// @address 0x40d178 ?object_update_decode@c_simulation_object_entity_definition@@KA_N_NPAKPAUs_simulation_object_state_data@@PAVc_bitstream@@0@Z
// @address 0x40c9d0 ?object_calculate_update_requirements@c_simulation_object_entity_definition@@KAXPBUs_simulation_entity@@PBUs_entity_update_data@@PAJ@Z



const char *c_simulation_item_entity_definition::entity_type_name(void)
{
    return "item";
}

// 0x433b38
bool c_simulation_item_entity_definition::build_baseline_state_data(long creation_size, const void *creation_data, long state_size, void *state_data)
{
    bool result = false;
    s_simulation_item_state_data *state = (s_simulation_item_state_data *)state_data;
    memset(state, 0, sizeof(s_simulation_item_state_data));
    if (c_simulation_object_entity_definition::object_build_baseline_state_data(
            (const s_simulation_object_creation_data *)creation_data, (s_simulation_object_state_data *)state))
    {
        state->at_rest = false;
        state->equipment_deployed = false;
        state->equipment_activated = false;
        state->equipment_creator_player_index = -1;
        result = true;
    }
    return result;
}

bool c_simulation_object_entity_definition::object_entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry)
{
    bool result = true;
    const s_simulation_object_state_data *state_data = (const s_simulation_object_state_data *)entity->state_data;
    long parent_entity_index = state_data->parent_entity_index;
    if (parent_entity_index != NONE)
    {
        c_simulation_view_telemetry_provider *provider = telemetry->provider;
        result = provider->entity_is_active(parent_entity_index);
    }
    return result;
}

long c_simulation_item_entity_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update)
{
    unsigned long update_mask = update->update_mask;
    long value;
    object_calculate_update_requirements(entity, update, &value);
    value += 4;
    if ((update_mask & 0x4000) != 0 && value > 19)
        value = 19;
    if ((update_mask & 0x8000) != 0 && value > 19)
        value = 19;
    if ((update_mask & 0x10000) != 0 && value > 19)
        value = 19;
    if ((update_mask & 0x20000) != 0 && value > 23)
        value = 23;
    return value;
}


// 0x4340d0
bool c_simulation_item_entity_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *written_update_mask, long size, const void *state, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long maximum_bits, bool final_update)
{
    return this->item_update_encode(initial_update, update_mask, written_update_mask, telemetry,
        (const s_simulation_item_state_data *)state, packet, maximum_bits, final_update);
}

// 0x434108
bool c_simulation_item_entity_definition::entity_update_decode(
    bool initial_update, unsigned long *update_mask, long size, void *state, c_bitstream *packet, bool final_update)
{
    return simulation_item_decode_update(this, initial_update, update_mask, state, packet, final_update);
}

// @address 0x434118 ?item_update_encode@c_simulation_item_entity_definition@@IAA_N_NKPAKPBUs_simulation_view_telemetry_data@@PBUs_simulation_item_state_data@@PAVc_bitstream@@J0@Z
bool c_simulation_item_entity_definition::item_update_encode(bool initial_update, unsigned long update_mask, unsigned long *updated_mask,
    const s_simulation_view_telemetry_data *telemetry, const s_simulation_item_state_data *state_data, c_bitstream *packet,
    long maximum_bits, bool final_update)
{
    bool result = false;
    const s_simulation_item_state_data *state = (const s_simulation_item_state_data *)state_data;
    if (c_simulation_object_entity_definition::object_update_encode(initial_update, update_mask & 0x3fff, updated_mask, telemetry,
            (const s_simulation_object_state_data *)state, packet, maximum_bits + 4,
            ensure_object_position_update_quantization_inside_bsp(), final_update))
    {
        c_entity_update_encode_helper helper;
        if (helper.make_room_for_update(packet, maximum_bits, 14, 4, update_mask & 0x3c000))
        {
            if (helper.write_component_header(14, "at-rest-exists"))
                packet->write_bool("at-rest", state->at_rest);
            helper.finish_component();
            if (helper.write_component_header(15, "equipment-deployed-exists"))
                packet->write_bool("equipment-deployed", state->equipment_deployed);
            helper.finish_component();
            if (helper.write_component_header(16, "equipment-activated-exists"))
                packet->write_bool("equipment-activated", state->equipment_activated);
            helper.finish_component();
            if (helper.write_component_header(17, "equipment-creator-exists"))
                packet->write_integer("equipment-creator", state->equipment_creator_player_index + 1, 5);
            helper.finish_component();
            helper.finish_update(updated_mask);
            result = true;
        }
    }
    return result;
}

// @address 0x4342c0 simulation_item_decode_update
bool simulation_item_decode_update(c_simulation_item_entity_definition *entity_definition, bool initial_update,
    unsigned long *update_mask, void *state_data, c_bitstream *packet, bool final_update)
{
    (void)entity_definition;
    s_simulation_item_state_data *state = (s_simulation_item_state_data *)state_data;
    bool valid = true;
    bool object_valid = c_simulation_object_entity_definition::object_update_decode(
        initial_update, update_mask, (s_simulation_object_state_data *)state, packet, final_update);
    if (packet->read_bool("at-rest-exists"))
    {
        state->at_rest = packet->read_bool("at-rest");
        *update_mask |= 0x4000;
    }
    if (packet->read_bool("equipment-deployed-exists"))
    {
        state->equipment_deployed = packet->read_bool("equipment-deployed");
        *update_mask |= 0x8000;
    }
    if (packet->read_bool("equipment-activated-exists"))
    {
        state->equipment_activated = packet->read_bool("equipment-activated");
        *update_mask |= 0x10000;
    }
    if (packet->read_bool("equipment-creator-exists"))
    {
        state->equipment_creator_player_index = packet->read_integer("equipment-creator", 5) - 1;
        *update_mask |= 0x20000;
        if (state->equipment_creator_player_index != -1
            && (state->equipment_creator_player_index < 0 || state->equipment_creator_player_index >= 16))
            valid = false;
    }
    bool decoded = valid && object_valid;
    bool result = decoded && !packet->overflowed();
    return result;
}

// 0x43a758
void c_simulation_object_entity_definition::object_entity_build_creation_data(long object_index, long creation_data_size, void *creation_data)
{
    s_simulation_object_creation_data *creation = (s_simulation_object_creation_data *)creation_data;
    memset(creation, 0, sizeof(s_simulation_object_creation_data));
    object_build_creation_data(object_index, creation);
}


// @address 0x433fc8 ?write_creation_description_to_string@c_simulation_item_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@JPAD@Z
void c_simulation_item_entity_definition::write_creation_description_to_string(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer)
{
    float relevance;

    relevance = 0.0f;
    simulation_calculate_entity_creation_priority(entity, telemetry, &relevance);
    csnzprintf(buffer, buffer_size, "item creation: relevance=%5.3f", relevance);
}

unsigned long c_simulation_item_entity_definition::handle_object_update(long object_index, unsigned long update_mask, long state_data_size, void *state_data)
{
    object_datum *object;
    unsigned char at_rest;
    float active_fraction;
    long equipment_index;
    unsigned long result;
    unsigned char is_special;
    unsigned char is_active;
    unsigned char active;
    short inventory_index;
    object = object_get(object_index);
    result = c_simulation_object_entity_definition::object_handle_update(object_index, update_mask & 0x3fff, (s_simulation_object_state_data *)state_data);
    s_simulation_item_state_data *state = (s_simulation_item_state_data *)state_data;
    if ((update_mask & 0x4000) != 0) {
        at_rest = (unsigned char)((object->physics_flags >> 9) & 1);
        if (at_rest != state->at_rest_byte) {
            result |= 0x4000;
            state->at_rest_byte = at_rest;
        }
    }
    if ((update_mask & 0x8000) != 0) {
        is_special = 0;
        if (((1 << object->object_type) & 8) != 0) {
            if ((unsigned short)object_get(object_index)->animation.offset != 0xffff)
                is_special = ((c_animation_manager *)((char *)object + object->animation.offset))->get_state_name() - 0x2c6 == 0;
        }
        if (is_special != state->equipment_deployed_byte) {
            result |= 0x8000;
            state->equipment_deployed_byte = is_special;
        }
    }
    if ((update_mask & 0x10000) != 0) {
        is_active = 0;
        if (((1 << object->object_type) & 8) != 0) {
            active_fraction = equipment_active_fraction(object_index);
            active = active_fraction > 0.0f ? 1 : 0;
            is_active = active;
        }
        if (is_active != state->equipment_activated_byte) {
            result |= 0x10000;
            state->equipment_activated_byte = is_active;
        }
    }
    if ((update_mask & 0x20000) != 0) {
        inventory_index = -1;
        if (((1 << object->object_type) & 8) != 0) {
            equipment_index = ((equipment_datum *)object)->creator_damage_owner.player_index;
            if (equipment_index != -1)
                inventory_index = (short)equipment_index;
        }
        if (inventory_index != state->equipment_creator_player_index) {
            state->equipment_creator_player_index = inventory_index;
            return result | 0x20000;
        }
    }
    return result;
}

// @address 0x433df8 ?apply_object_update@c_simulation_item_entity_definition@@UAAXJKJPBX@Z
void c_simulation_item_entity_definition::apply_object_update(long object_index, unsigned long update_mask, long state_data_size,
    const void *state_data)
{
    (void)state_data_size;
    object_datum *object;
    const s_simulation_item_state_data *state = static_cast<const s_simulation_item_state_data *>(state_data);
    if ((update_mask & 0x3fff) != 0)
        object_apply_update(object_index, update_mask & 0x3fff, static_cast<const s_simulation_object_state_data *>(state_data));
    if ((update_mask & 0x4000) != 0)
        object_set_at_rest(object_index, state->at_rest);
    s_object_header *header = object_header_get(object_index);
    if (((1 << header->object_type) & 8) != 0) {
        object = (object_datum *)header->datum;
        if ((update_mask & 0x8000) != 0 && state->equipment_deployed) {
            unsigned short manager_offset = (unsigned short)header->datum->animation.offset;
            if (manager_offset != 0xffff) {
                c_animation_manager *manager = (c_animation_manager *)((char *)object + (short)manager_offset);
                manager->set_goal(226, 226, 226, 710, 130, 29);
            }
        }
        if ((update_mask & 0x10000) != 0 && state->equipment_activated)
            ((equipment_datum *)object)->activation_time = game_time_get();
        if ((update_mask & 0x20000) != 0) {
            long player_index = state->equipment_creator_player_index;
            s_damage_owner owner = k_damage_owner_unknown;
            if (player_index != -1) {
                s_data_array *players = tls_player_data();
                if (player_index >= 0 && player_index < players->first_unallocated) {
                    player_datum *entry = (player_datum *)(players->data + players->size * player_index);
                    if (entry->identifier != 0) {
                    long team = entry->configuration.host.team_index;
                    memset(&owner, 0, sizeof(owner));
                    owner.team = (char)team;
                    owner.player_index = datum_absolute_index_to_index(tls_player_data(), player_index);
                    owner.object_index = entry->unit_index;
                    }
                }
            }
            ((equipment_datum *)object)->creator_damage_owner = owner;
        }
    }
}
// @address 0x433d68 ?create_object@c_simulation_item_entity_definition@@UAAJJPBXPAKJ0@Z
long c_simulation_item_entity_definition::create_object(
    long creation_data_size, void const *creation_data, unsigned long *flags, long state_data_size, void const *state_data)
{
    (void)creation_data_size;
    (void)state_data_size;
    const s_simulation_object_creation_data *creation = (const s_simulation_object_creation_data *)creation_data;
    long result = -1;
    object_placement_data placement;
    if (creation->definition_index != -1 && tag_try_and_get_unsafe(0x6974656d, creation->definition_index) != 0)
    {
        c_simulation_object_entity_definition::object_setup_placement_data((s_simulation_object_creation_data const *)creation_data,
            (s_simulation_object_state_data const *)state_data, (unsigned long *)(flags), (object_placement_data *)&placement);
        result = this->object_create_object(
            (s_simulation_object_creation_data const *)creation_data, (s_simulation_object_state_data const *)state_data,
            (unsigned long *)(flags), &placement);
    }
    return result;
}



// @address 0x42e678 ?entity_type@c_simulation_item_entity_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_item_entity_definition::entity_type()
{
    return _simulation_entity_type_item;
}

// @address 0x433b28 ?state_data_size@c_simulation_item_entity_definition@@UAAJXZ
long c_simulation_item_entity_definition::state_data_size()
{
    return sizeof(s_simulation_item_state_data);
}

// @address 0x42e748 ?creation_data_size@c_simulation_item_entity_definition@@UAAJXZ
long c_simulation_item_entity_definition::creation_data_size()
{
    return sizeof(s_simulation_object_creation_data);
}

// @address 0x433b30 ?update_flag_count@c_simulation_item_entity_definition@@UAAJXZ
long c_simulation_item_entity_definition::update_flag_count()
{
    return 18;
}

// @address 0x42d380 ?entity_type_is_gameworld_object@c_simulation_item_entity_definition@@UAA_NXZ
bool c_simulation_item_entity_definition::entity_type_is_gameworld_object()
{
    return true;
}

// @address 0x433fa0 ?entity_can_be_created@c_simulation_item_entity_definition@@UAA_NPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_item_entity_definition::entity_can_be_created(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return object_entity_can_be_created(entity, telemetry);
}

// @address 0x433fc0 ?creation_minimum_required_bits@c_simulation_item_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_item_entity_definition::creation_minimum_required_bits(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 110;
}

// @address 0x43ae18 ?entity_creation_encode@c_simulation_item_entity_definition@@UAAXJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@_N@Z
void c_simulation_item_entity_definition::entity_creation_encode(long creation_data_size, const void *creation_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update)
{
    object_creation_encode((const s_simulation_object_creation_data *)creation_data, packet, final_update);
}

// @address 0x43ae28 ?entity_creation_decode@c_simulation_item_entity_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_item_entity_definition::entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet,
    bool quantize)
{
    return object_entity_creation_decode(creation_data_size, creation_data, packet, quantize);
}

// @address 0x436808 ?entity_state_lossy_compare@c_simulation_item_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_item_entity_definition::entity_state_lossy_compare(void *first, void *second, long size)
{
    return object_state_compare((s_simulation_object_state_data *)first, (s_simulation_object_state_data *)second);
}

// @address 0x42d380 ?entity_creation_lossy_compare@c_simulation_item_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_item_entity_definition::entity_creation_lossy_compare(void *first, void *second, long size)
{
    return true;
}

// @address 0x43add8 ?rotate_entity_indices@c_simulation_item_entity_definition@@UAAXPBUs_simulation_entity@@@Z
void c_simulation_item_entity_definition::rotate_entity_indices(const s_simulation_entity *entity)
{
    object_entity_rotate_indices(entity);
}

// @address 0x5bec98 ?prepare_creation_data_for_gameworld@c_simulation_item_entity_definition@@UAAXJPAX@Z
void c_simulation_item_entity_definition::prepare_creation_data_for_gameworld(long size, void *creation_data)
{
}

// @address 0x434748 ?prepare_state_data_for_gameworld@c_simulation_item_entity_definition@@UAAXKJPAX@Z
void c_simulation_item_entity_definition::prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state)
{
    object_entity_prepare_state_data(update_mask, size, state);
}

// @address 0x43a758 ?build_object_creation_data@c_simulation_item_entity_definition@@UAAXJJPAX@Z
void c_simulation_item_entity_definition::build_object_creation_data(long object_index, long creation_data_size, void *creation_data)
{
    object_entity_build_creation_data(object_index, creation_data_size, creation_data);
}

// @address 0x42d380 ?object_required_to_join_game@c_simulation_item_entity_definition@@UAA_NJ@Z
bool c_simulation_item_entity_definition::object_required_to_join_game(long object_index)
{
    return true;
}

// @address 0x42d298 ?handle_delete_object@c_simulation_item_entity_definition@@UAA_NJ@Z
bool c_simulation_item_entity_definition::handle_delete_object(long object_index)
{
    return false;
}

// @address 0x42d380 ?promote_object_to_authority@c_simulation_item_entity_definition@@UAA_NJ@Z
bool c_simulation_item_entity_definition::promote_object_to_authority(long object_index)
{
    return true;
}

// @address 0x42d380 ?ensure_object_position_update_quantization_inside_bsp@c_simulation_item_entity_definition@@UAA_NXZ
bool c_simulation_item_entity_definition::ensure_object_position_update_quantization_inside_bsp()
{
    return true;
}
