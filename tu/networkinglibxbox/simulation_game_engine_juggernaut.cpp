// networkinglibxbox/simulation_game_engine_juggernaut.obj: from the records 0x428fc0 0x428fd0 0x428fd8 0x429050 0x4290d8 0x429220 0x709ed8 0x70c778
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_definition.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "cseries/cseries.h"
#include "simulation/simulation_priority.h"
#include "memory/bitstream.h"
#include "simulation/simulation_entity_types.h"


// @address 0x428fc0 ?entity_type_name@c_simulation_juggernaut_engine_globals_definition@@UAAPBDXZ
const char *c_simulation_juggernaut_engine_globals_definition::entity_type_name()
{
    return "juggernaut-engine-globals";
}

// @address 0x428fd0 ?initial_update_mask@c_simulation_juggernaut_engine_globals_definition@@UAAKXZ
unsigned long c_simulation_juggernaut_engine_globals_definition::initial_update_mask()
{
    return 0x1FF;
}

// @address 0x4311b0 ?entity_type@c_simulation_juggernaut_engine_globals_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_juggernaut_engine_globals_definition::entity_type()
{
    return _simulation_entity_type_juggernaut_engine_globals;
}

// @address 0x42e810 ?update_flag_count@c_simulation_juggernaut_engine_globals_definition@@UAAJXZ
long c_simulation_juggernaut_engine_globals_definition::update_flag_count()
{
    return 9;
}

// @address 0x1c1248 ?state_data_size@c_simulation_juggernaut_engine_globals_definition@@UAAJXZ
long c_simulation_juggernaut_engine_globals_definition::state_data_size()
{
    return 48;
}

// @address 0x42d380 ?entity_state_lossy_compare@c_simulation_juggernaut_engine_globals_definition@@UAA_NPAX0J@Z
bool c_simulation_juggernaut_engine_globals_definition::entity_state_lossy_compare(void *, void *, long)
{
    return true;
}

// @address 0x428fd8 ?update_minimum_required_bits@c_simulation_juggernaut_engine_globals_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_juggernaut_engine_globals_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data)
{
  unsigned int bits = update_data->update_mask;
  int value = global_update_minimum_required_bits(update_data) + 3;
  if ((bits & 0x40) != 0 && value >= 14) {
    value = 14;
  }
  if ((bits & 0x80) != 0 && value >= 13) {
    value = 13;
  }
  if ((bits & 0x100) != 0 && value >= 10) {
    value = 10;
  }
  return value;
}


// @address 0x429050 ?write_update_description_to_string@c_simulation_juggernaut_engine_globals_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_juggernaut_engine_globals_definition::write_update_description_to_string(
    const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer)
{
  long period;
  float relevance;
  char name[1024];

  relevance = 0.0f;
  period = 0;
  global_update_write_description_to_string(update_data, sizeof(name), name);
  simulation_calculate_entity_update_priority(entity,
      update_data, &relevance, &period);
  csnzprintf(buffer, buffer_size, "juggernaut update: %s relevance=%5.3f: period=%d", name, relevance, period);
}



// @address 0x47b28 k_juggernaut_player_component_name
// @address 0x47b44 k_juggernaut_timer_component_name
// @address 0x47b5c k_juggernaut_visited_zone_component_name
// @rdata 0x82047b28..0x82047b7c
const char k_juggernaut_player_component_name[28] = "juggernaut-player-exists";
const char k_juggernaut_timer_component_name[24] = "juggernaut-timer-exists";
const char k_juggernaut_visited_zone_component_name[32] = "juggernaut-visited-zone-exists";
// @rdata-end
// @address 0x47b80 ??_7c_simulation_juggernaut_engine_globals_definition@@6B@
// @address 0x709ed8 ??__Eg_simulation_juggernaut_engine_globals_definition@@YAXXZ
// @address 0x70c778 ??__Fg_simulation_juggernaut_engine_globals_definition@@YAXXZ
// @address 0x7bb438 g_simulation_juggernaut_engine_globals_definition
c_simulation_juggernaut_engine_globals_definition g_simulation_juggernaut_engine_globals_definition;

// @address 0x4290d8 ?entity_update_encode@c_simulation_juggernaut_engine_globals_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_juggernaut_engine_globals_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *written_update_mask, long state_data_size, const void *state_data,
    const struct s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, long maximum_bits, bool flag)
{
    s_juggernaut_engine_state_data *state = (s_juggernaut_engine_state_data *)state_data;
    bool result = false;
    if (global_update_encode(update_mask & 63, written_update_mask,
            (const s_game_engine_state_data *)state, packet, maximum_bits + 3)) {
        c_entity_update_encode_helper helper;
        if (helper.make_room_for_update(packet, maximum_bits, 6, 3, update_mask & 0x1c0)) {
            if (helper.write_component_header(6, k_juggernaut_player_component_name))
                packet->write_integer("juggernaut-player", state->juggernaut_player + 1, 5);
            helper.finish_component();
            if (helper.write_component_header(7, k_juggernaut_timer_component_name))
                packet->write_integer("juggernaut-timer", state->juggernaut_timer, 4);
            helper.finish_component();
            if (helper.write_component_header(8, k_juggernaut_visited_zone_component_name))
                packet->write_bool("juggernaut-visited-zone", state->visited_zone);
            helper.finish_component();
            helper.finish_update(written_update_mask);
            result = true;
        }
    }
    return result;
}

// @address 0x429220 ?entity_update_decode@c_simulation_juggernaut_engine_globals_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_juggernaut_engine_globals_definition::entity_update_decode(
    bool initial, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool flag)
{
 s_juggernaut_engine_state_data *data=(s_juggernaut_engine_state_data*)state_data;
 unsigned long decoded_mask=0;
 bool success=true;
 bool base_success=c_simulation_game_engine_globals_definition::global_update_decode(&decoded_mask,(s_game_engine_state_data *)data,packet);
 success=success && base_success;
 if(packet->read_bool("juggernaut-player-exists")) {
  data->juggernaut_player=(short)(packet->read_integer("juggernaut-player", 5)-1);
  success=success && data->juggernaut_player>=-1 && data->juggernaut_player<16;
  decoded_mask |= 64;
 }
 if(packet->read_bool("juggernaut-timer-exists")) {
  data->juggernaut_timer=(unsigned char)packet->read_integer("juggernaut-timer", 4);
  success=success && data->juggernaut_timer<=10;
  decoded_mask |=128;
 }
 if(packet->read_bool("juggernaut-visited-zone-exists")) {
  data->visited_zone=packet->read_bool("juggernaut-visited-zone");
  decoded_mask |=256;
 }
 success=success && decoded_mask!=0;
 *update_mask=decoded_mask;
 return success;
}
