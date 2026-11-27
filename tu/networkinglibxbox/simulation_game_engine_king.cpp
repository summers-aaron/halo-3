// networkinglibxbox/simulation_game_engine_king.obj: from the records 0x4287f0 0x428808 0x428868 0x4288f0 0x428a00 0x709eb8 0x70c748
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_definition.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "memory/bitstream.h"
#include "cseries/cseries.h"
#include "simulation/simulation_priority.h"
#include "simulation/simulation_entity_types.h"
// @address 0x4287f0 ?entity_type_name@c_simulation_king_engine_globals_definition@@UAAPBDXZ
// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0x43be48 ?global_update_decode@c_simulation_game_engine_globals_definition@@SA_NPAKPAUs_game_engine_state_data@@PAVc_bitstream@@@Z
// @address 0x446800 ?simulation_calculate_entity_update_priority@@YAMPBUs_simulation_entity@@PBUs_entity_update_data@@PAMPAJ@Z
// @address 0x446e88 ?make_room_for_update@c_entity_update_encode_helper@@QAA_NPAVc_bitstream@@JJJK@Z
// @address 0x446ef8 ?write_component_header@c_entity_update_encode_helper@@QAA_NJPBD@Z
// @address 0x446f80 ?finish_component@c_entity_update_encode_helper@@QAAXXZ
// @address 0x4288f0 ?entity_update_encode@c_simulation_king_engine_globals_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z

const char *c_simulation_king_engine_globals_definition::entity_type_name() {
    return "king-engine-globals";
}

// @address 0x3fc550 ?entity_type@c_simulation_king_engine_globals_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_king_engine_globals_definition::entity_type()
{
    return _simulation_entity_type_king_engine_globals;
}

// @address 0x42e650 ?update_flag_count@c_simulation_king_engine_globals_definition@@UAAJXZ
long c_simulation_king_engine_globals_definition::update_flag_count()
{
    return 8;
}

// @address 0x1c1248 ?state_data_size@c_simulation_king_engine_globals_definition@@UAAJXZ
long c_simulation_king_engine_globals_definition::state_data_size()
{
    return 48;
}

// @address 0x428800 ?initial_update_mask@c_simulation_king_engine_globals_definition@@UAAKXZ
unsigned long c_simulation_king_engine_globals_definition::initial_update_mask()
{
    return 255;
}

// @address 0x42d380 ?entity_state_lossy_compare@c_simulation_king_engine_globals_definition@@UAA_NPAX0J@Z
bool c_simulation_king_engine_globals_definition::entity_state_lossy_compare(void *, void *, long)
{
    return true;
}

// @address 0x428808 ?update_minimum_required_bits@c_simulation_king_engine_globals_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_king_engine_globals_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data)
{
    unsigned int bits = update_data->update_mask;
    int count = global_update_minimum_required_bits(update_data) + 2;

    if ((bits & 0x40) != 0 && count >= 24) {
        count = 24;
    }
    if ((bits & 0x80) != 0 && count >= 24) {
        count = 24;
    }
    return count;
}


// @address 0x428868 ?write_update_description_to_string@c_simulation_king_engine_globals_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_king_engine_globals_definition::write_update_description_to_string(
    const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer)
{
  int period;
  float relevance;
  char name[1024];

  relevance = 0.0f;
  period = 0;
  global_update_write_description_to_string(update_data, sizeof(name), name);
  simulation_calculate_entity_update_priority(entity,
      update_data, &relevance, (long *)&period);
  csnzprintf(buffer, buffer_size, "king update: %s relevance=%5.3f: period=%d", name, relevance, period);
}


struct s_game_engine_state_data;


// @address 0x478e0 k_king_players_in_hill_component_name
// @address 0x478f8 k_king_hill_timer_component_name
// @rdata 0x820478e0..0x8204790c
extern "C" {
const char k_king_players_in_hill_component_name[24] = "players-in-hill-exists";
const char k_king_hill_timer_component_name[20] = "hill-timer-exists";
}
// @rdata-end

// @address 0x47910 ??_7c_simulation_king_engine_globals_definition@@6B@
// @address 0x709eb8 ??__Eg_simulation_king_engine_globals_definition@@YAXXZ
// @address 0x70c748 ??__Fg_simulation_king_engine_globals_definition@@YAXXZ
// @address 0x7bb430 g_simulation_king_engine_globals_definition
c_simulation_king_engine_globals_definition g_simulation_king_engine_globals_definition;

bool c_simulation_king_engine_globals_definition::entity_update_encode(bool initial_update,
    unsigned long update_mask, unsigned long *updated_mask, long state_data_size,
    const void *state_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long maximum_bits, bool final_update)
{
    const s_king_engine_state_data *state = (const s_king_engine_state_data *)state_data;
    bool result = false;
    unsigned long global_update_mask = update_mask & 63;
    long global_maximum_bits = maximum_bits + 2;
    if (global_update_encode(global_update_mask, updated_mask,
        (const s_game_engine_state_data *)state, packet, global_maximum_bits))
    {
        c_entity_update_encode_helper helper;
        long first_component_index = 6;
        long component_count = 8 - first_component_index;
        unsigned long king_update_mask = update_mask & (((1i64 << component_count) - 1) << first_component_index);
        if (helper.make_room_for_update(packet, maximum_bits, first_component_index,
            component_count, king_update_mask))
        {
            if (helper.write_component_header(6, k_king_players_in_hill_component_name))
                packet->write_integer("players-in-hill", state->second, 16);
            helper.finish_component();
            if (helper.write_component_header(7, k_king_hill_timer_component_name))
                packet->write_integer("hill-timer", state->first, 16);
            helper.finish_component();
            helper.finish_update(updated_mask);
            result = true;
        }
    }
    return result;
}

// @address 0x428a00 ?entity_update_decode@c_simulation_king_engine_globals_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_king_engine_globals_definition::entity_update_decode(bool initial, unsigned long *update_mask, long state_data_size,
    void *state_data, c_bitstream *packet, bool flag)
{
    s_king_engine_state_data *state = (s_king_engine_state_data *)state_data;
    unsigned long decoded_mask = 0;
    bool success = true;
    success = success && c_simulation_game_engine_globals_definition::global_update_decode(&decoded_mask,
        (s_game_engine_state_data *)state, packet);
    unsigned long mask;

    if (packet->read_bool("second-exists"))
    {
        state->second = (unsigned short)packet->read_integer("second", 16);
        mask = decoded_mask | 64;
    }
    else
    {
        mask = decoded_mask;
    }

    if (packet->read_bool("first-exists"))
    {
        state->first = (unsigned short)packet->read_integer("first", 16);
        mask |= 128;
    }

    success = success && mask != 0;
    *update_mask = mask;
    return success;
}
