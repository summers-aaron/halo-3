// networkinglibxbox/simulation_game_engine_vip.obj: from the records 0x4297b0 0x4297c0 0x4297d0 0x429878 0x429900 0x429a88
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_definition.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "simulation/simulation_priority.h"
#include "memory/bitstream.h"
#include "cseries/cseries.h"
#include "simulation/simulation_entity_types.h"
// @address 0x4297d0 ?update_minimum_required_bits@c_simulation_vip_engine_globals_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0x43be48 ?global_update_decode@c_simulation_game_engine_globals_definition@@SA_NPAKPAUs_game_engine_state_data@@PAVc_bitstream@@@Z
// @address 0x446e88 ?make_room_for_update@c_entity_update_encode_helper@@QAA_NPAVc_bitstream@@JJJK@Z
// @address 0x446ef8 ?write_component_header@c_entity_update_encode_helper@@QAA_NJPBD@Z
// @address 0x446f80 ?finish_component@c_entity_update_encode_helper@@QAAXXZ


#include "game/game_types.h"

// @address 0x4297b0 ?entity_type_name@c_simulation_vip_engine_globals_definition@@UAAPBDXZ
const char *c_simulation_vip_engine_globals_definition::entity_type_name() {
    return "vip-engine-globals";
}
// @address 0x4297c0 ?initial_update_mask@c_simulation_vip_engine_globals_definition@@UAAKXZ
unsigned long c_simulation_vip_engine_globals_definition::initial_update_mask() {
    return 0x1fffffff;
}
long c_simulation_vip_engine_globals_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update) {
    unsigned int flags = update->update_mask;
    long minimum_bits = global_update_minimum_required_bits(update) + 23;
    for (e_multiplayer_team_designator team = _multiplayer_team_designator_first;
        team < k_multiplayer_team_designator_count; team = (e_multiplayer_team_designator)(team + 1)) {
        if ((flags & (1 << (team + 6))) != 0) {
            long team_bits = 34;
            minimum_bits = team_bits > minimum_bits ? minimum_bits : team_bits;
        }
    }
    for (long player = 0; player < 12; ++player) {
        if ((flags & (1 << (player + 15))) != 0) {
            long player_bits = 37;
            minimum_bits = player_bits > minimum_bits ? minimum_bits : player_bits;
        }
    }
    if ((flags & 0x10000000) != 0) {
        long vip_bits = 33;
        minimum_bits = vip_bits > minimum_bits ? minimum_bits : vip_bits;
    }
    return minimum_bits;
}
// @address 0x42e860 ?update_flag_count@c_simulation_vip_engine_globals_definition@@UAAJXZ
long c_simulation_vip_engine_globals_definition::update_flag_count() {
    return 29;
}

// @address 0x42e810 ?entity_type@c_simulation_vip_engine_globals_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_vip_engine_globals_definition::entity_type()
{
    return _simulation_entity_type_vip_engine_globals;
}

// @address 0x3faeb8 ?state_data_size@c_simulation_vip_engine_globals_definition@@UAAJXZ
long c_simulation_vip_engine_globals_definition::state_data_size()
{
    return 76;
}

// @address 0x42d380 ?entity_state_lossy_compare@c_simulation_vip_engine_globals_definition@@UAA_NPAX0J@Z
bool c_simulation_vip_engine_globals_definition::entity_state_lossy_compare(void *, void *, long)
{
    return true;
}


// @address 0x429878 ?write_update_description_to_string@c_simulation_vip_engine_globals_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_vip_engine_globals_definition::write_update_description_to_string(
    const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer)
{
    int period;
    float relevance;
    char name[1024];

    relevance = 0.0f;
    period = 0;
    global_update_write_description_to_string(update_data, sizeof(name), name);
    ::simulation_calculate_entity_update_priority(entity, update_data, &relevance, (long *)&period);
    csnzprintf(buffer, buffer_size, "vip update: %s relevance=%5.3f: period=%d", name, relevance, period);
}


// @address 0x47d98 k_vip_team_vip_component_name
// @address 0x47dac k_vip_zone_data_component_name
// @address 0x47dc4 k_vip_defending_team_component_name
// @rdata 0x82047d98..0x82047de0
extern "C" {
const char k_vip_team_vip_component_name[20] = "vip-team-vip-exists";
const char k_vip_zone_data_component_name[24] = "vip-zone-data-exists";
const char k_vip_defending_team_component_name[28] = "vip-defending-team-exists";
}
// @rdata-end

// @address 0x47de0 ??_7c_simulation_vip_engine_globals_definition@@6B@
// @address 0x7bb440 g_simulation_vip_engine_globals_definition
c_simulation_vip_engine_globals_definition g_simulation_vip_engine_globals_definition;


// @address 0x429900 ?entity_update_encode@c_simulation_vip_engine_globals_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_vip_engine_globals_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *updated_mask, long state_data_size, const void *state_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, long maximum_bits, bool final_update)
{
    const s_vip_engine_state_data *state = (const s_vip_engine_state_data *)state_data;
    bool result = false;
    unsigned long global_update_mask = update_mask & 63;
    long global_maximum_bits = maximum_bits + 23;
    if (global_update_encode(global_update_mask, updated_mask,
        (const s_game_engine_state_data *)state, packet, global_maximum_bits))
    {
        c_entity_update_encode_helper helper;
        long first_component_index = 6;
        long component_count = 23;
        unsigned long vip_update_mask = update_mask & (((1i64 << component_count) - 1) << first_component_index);
        if (helper.make_room_for_update(packet, maximum_bits, first_component_index,
            component_count, vip_update_mask))
        {
            for (long team = 0; team < 8; ++team)
            {
                if (helper.write_component_header(team + 6, k_vip_team_vip_component_name))
                    packet->write_integer("vip-team-vip", state->team_vip[team] + 1, 5);
                helper.finish_component();
            }
            for (long zone = 0; zone < 12; ++zone)
            {
                if (helper.write_component_header(zone + 15, k_vip_zone_data_component_name))
                    packet->write_integer("vip-zone-data", state->zone_data[zone], 8);
                helper.finish_component();
            }
            if (helper.write_component_header(28, k_vip_defending_team_component_name))
                packet->write_integer("vip-defending-team", state->defending_team + 1, 4);
            helper.finish_component();
            helper.finish_update(updated_mask);
            result = true;
        }
    }
    return result;
}

// @address 0x429a88 ?entity_update_decode@c_simulation_vip_engine_globals_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_vip_engine_globals_definition::entity_update_decode(bool initial, unsigned long *update_mask, long state_size,
                 void *state_data, c_bitstream *stream, bool unknown)
{
    s_vip_engine_state_data *state = (s_vip_engine_state_data *)state_data;
    unsigned long decoded_mask = 0;
    bool success = true;
    success = success && c_simulation_game_engine_globals_definition::global_update_decode(
        &decoded_mask, (s_game_engine_state_data *)state, stream);
    unsigned long mask = decoded_mask;
    for (long team = 0; team < 8; team++)
    {
        if (((c_bitstream *)(stream))->read_bool("vip-team-vip-exists"))
        {
            state->team_vip[team] = (short)(((c_bitstream *)(stream))->read_integer("vip-team-vip", 5) - 1);
            success = (success && state->team_vip[team] >= -1) ? (state->team_vip[team] < 16) : 0;
            mask |= 1 << (team + 6);
        }
    }
    for (long zone = 0; zone < 12; zone++)
    {
        if (((c_bitstream *)(stream))->read_bool("vip-zone-data-exists"))
        {
            state->zone_data[zone] = (unsigned char)((c_bitstream *)(stream))->read_integer("vip-zone-data", 8);
            mask |= 1 << (zone + 15);
        }
    }
    if (((c_bitstream *)(stream))->read_bool("vip-defending-team-exists"))
    {
        state->defending_team = ((c_bitstream *)(stream))->read_integer("vip-defending-team", 4) - 1;
        success = success && state->defending_team >= -1 && state->defending_team < 8;
        mask |= 1 << 28;
    }
    success = success && mask != 0;
    *update_mask = mask;
    return success;
}
