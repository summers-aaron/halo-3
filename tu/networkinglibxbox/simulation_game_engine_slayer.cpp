// networkinglibxbox/simulation_game_engine_slayer.obj: from the records 0x427350 0x427360 0x427368 0x427370 0x427378 0x427400 0x427418 0x709e78 0x70c6e8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "cseries/cseries.h"
#include "simulation/simulation_priority.h"
#include "memory/bitstream.h"
#include "simulation/simulation_entity_types.h"

struct s_game_engine_state_data;




// @address 0x474c8 ??_7c_simulation_slayer_engine_globals_definition@@6B@
// @address 0x709e78 ??__Eg_simulation_slayer_engine_globals_definition@@YAXXZ
// @address 0x70c6e8 ??__Fg_simulation_slayer_engine_globals_definition@@YAXXZ
// @address 0x7bb420 g_simulation_slayer_engine_globals_definition
c_simulation_slayer_engine_globals_definition g_simulation_slayer_engine_globals_definition;

// @address 0x427350 ?entity_type_name@c_simulation_slayer_engine_globals_definition@@UAAPBDXZ
const char *c_simulation_slayer_engine_globals_definition::entity_type_name()
{
    return "slayer-engine-globals";
}

// @address 0x427360 ?state_data_size@c_simulation_slayer_engine_globals_definition@@UAAJXZ
long c_simulation_slayer_engine_globals_definition::state_data_size()
{
    return 44;
}

// @address 0x42d298 ?entity_type@c_simulation_slayer_engine_globals_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_slayer_engine_globals_definition::entity_type()
{
    return _simulation_entity_type_slayer_engine_globals;
}

// @address 0x4311b0 ?update_flag_count@c_simulation_slayer_engine_globals_definition@@UAAJXZ
long c_simulation_slayer_engine_globals_definition::update_flag_count()
{
    return 6;
}

// @address 0x42d380 ?entity_state_lossy_compare@c_simulation_slayer_engine_globals_definition@@UAA_NPAX0J@Z
bool c_simulation_slayer_engine_globals_definition::entity_state_lossy_compare(void *, void *, long)
{
    return true;
}

// @address 0x427368 ?initial_update_mask@c_simulation_slayer_engine_globals_definition@@UAAKXZ
unsigned long c_simulation_slayer_engine_globals_definition::initial_update_mask()
{
    return 63;
}

// @address 0x427370 ?update_minimum_required_bits@c_simulation_slayer_engine_globals_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_slayer_engine_globals_definition::update_minimum_required_bits(
    const s_simulation_entity *entity, const s_entity_update_data *update_data)
{
    return global_update_minimum_required_bits(update_data);
}


// @address 0x427378 ?write_update_description_to_string@c_simulation_slayer_engine_globals_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_slayer_engine_globals_definition::write_update_description_to_string(
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
  csnzprintf(buffer, buffer_size, "slayer update: %s relevance=%5.3f: period=%d", name, relevance, period);
}

// @address 0x427400 ?entity_update_encode@c_simulation_slayer_engine_globals_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_slayer_engine_globals_definition::entity_update_encode(bool initial, unsigned long update_mask,
    unsigned long *updated_mask, long state_data_size, const void *state_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long maximum_bits, bool use_encoding)
{
    return global_update_encode(update_mask, updated_mask, (const s_game_engine_state_data *)state_data, packet, maximum_bits);
}

// @address 0x427418 ?entity_update_decode@c_simulation_slayer_engine_globals_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_slayer_engine_globals_definition::entity_update_decode(
    bool initial, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool flag)
{
    unsigned long decoded_mask = 0;
    bool success = c_simulation_game_engine_globals_definition::global_update_decode(&decoded_mask, (s_game_engine_state_data *)state_data, packet);
    success = success && decoded_mask != 0;
    *update_mask = decoded_mask;
    return success;
}
