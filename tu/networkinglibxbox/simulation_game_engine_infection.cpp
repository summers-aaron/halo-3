// networkinglibxbox/simulation_game_engine_infection.obj: from the records 0x426fd0 0x429370 0x429380 0x429428 0x4294b0 0x429668
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_definition.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "simulation/simulation_priority.h"
#include "cseries/cseries.h"
#include "memory/bitstream.h"
#include "simulation/simulation_entity_types.h"

struct s_game_engine_globals_state_data
{
    char globals[52];
    short total_budget;
    unsigned short player_roles;
    long edited_object_ids[16];
    // March sandbox encoder 826ca3e8 and 826ca71c identify these retained slots.
    long remove_object_map_variant_index;
    long object_definition_index;
    long variant_object_count;
    long game_type;
};

// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0x446800 ?simulation_calculate_entity_update_priority@@YAMPBUs_simulation_entity@@PBUs_entity_update_data@@PAMPAJ@Z
// @address 0x446e88 ?make_room_for_update@c_entity_update_encode_helper@@QAA_NPAVc_bitstream@@JJJK@Z
// @address 0x446ef8 ?write_component_header@c_entity_update_encode_helper@@QAA_NJPBD@Z
// @address 0x446f80 ?finish_component@c_entity_update_encode_helper@@QAAXXZ
// @address 0x43be48 ?global_update_decode@c_simulation_game_engine_globals_definition@@SA_NPAKPAUs_game_engine_state_data@@PAVc_bitstream@@@Z
// @address 0x47ce0 ??_7c_simulation_infection_engine_globals_definition@@6B@
// @address 0x7bb43c g_simulation_infection_engine_globals_definition
c_simulation_infection_engine_globals_definition g_simulation_infection_engine_globals_definition;

// @address 0x429370 ?entity_type_name@c_simulation_infection_engine_globals_definition@@UAAPBDXZ
const char *c_simulation_infection_engine_globals_definition::entity_type_name() {
    return "infection-engine-globals";
}

// @address 0x42e650 ?entity_type@c_simulation_infection_engine_globals_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_infection_engine_globals_definition::entity_type()
{
    return _simulation_entity_type_infection_engine_globals;
}

// @address 0x426e90 ?update_flag_count@c_simulation_infection_engine_globals_definition@@UAAJXZ
long c_simulation_infection_engine_globals_definition::update_flag_count()
{
    return 11;
}

// @address 0x42e8c0 ?state_data_size@c_simulation_infection_engine_globals_definition@@UAAJXZ
long c_simulation_infection_engine_globals_definition::state_data_size()
{
    return 56;
}

// @address 0x426e98 ?initial_update_mask@c_simulation_infection_engine_globals_definition@@UAAKXZ
unsigned long c_simulation_infection_engine_globals_definition::initial_update_mask()
{
    return 2047;
}

// @address 0x42d380 ?entity_state_lossy_compare@c_simulation_infection_engine_globals_definition@@UAA_NPAX0J@Z
bool c_simulation_infection_engine_globals_definition::entity_state_lossy_compare(void *, void *, long)
{
    return true;
}


// @address 0x429380 ?update_minimum_required_bits@c_simulation_infection_engine_globals_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_infection_engine_globals_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data)

{
  int minimum_bits;
  unsigned int update_flags;

  update_flags = (unsigned int)update_data->update_mask;
  minimum_bits = global_update_minimum_required_bits(update_data);
  minimum_bits = minimum_bits + 5;
  if (((update_flags & 0x40) != 0) && (0x1b <= (int)minimum_bits)) {
    minimum_bits = 0x1b;
  }
  if (((update_flags & 0x80) != 0) && (0x1b <= (int)minimum_bits)) {
    minimum_bits = 0x1b;
  }
  if (((update_flags & 0x100) != 0) && (0x1b <= (int)minimum_bits)) {
    minimum_bits = 0x1b;
  }
  if (((update_flags & 0x200) != 0) && (0x2b <= (int)minimum_bits)) {
    minimum_bits = 0x2b;
  }
  if (((update_flags & 0x400) != 0) && (0x1b <= (int)minimum_bits)) {
    minimum_bits = 0x1b;
  }
  return minimum_bits;
}


// @address 0x429428 ?write_update_description_to_string@c_simulation_infection_engine_globals_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_infection_engine_globals_definition::write_update_description_to_string(
    const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer)
{
    char name[1024];
    float relevance = 0.0f;
    long period = 0;
    global_update_write_description_to_string(update_data, sizeof(name), name);
    simulation_calculate_entity_update_priority(entity, update_data, &relevance, &period);
    csnzprintf(buffer, buffer_size, "infection update: %s relevace=%5.3: period=%d", name, relevance, period);
}

// @address 0x426fd0 ?entity_update_encode@c_simulation_sandbox_engine_globals_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_sandbox_engine_globals_definition::entity_update_encode(bool initial_update,
    unsigned long update_mask, unsigned long *updated_mask, long state_data_size,
    const void *state_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long maximum_bits, bool final_update)
{
    const s_game_engine_globals_state_data *state = (const s_game_engine_globals_state_data *)state_data;
    bool result = false;
    unsigned long global_update_mask = update_mask & 63;
    long global_maximum_bits = maximum_bits + 5;
    if (global_update_encode(global_update_mask, updated_mask,
        (const s_game_engine_state_data *)state, packet, global_maximum_bits))
    {
        c_entity_update_encode_helper helper;
        long first_component_index = 6;
        long component_count = 5;
        unsigned long engine_update_mask = update_mask & (((1i64 << component_count) - 1) << first_component_index);
        if (helper.make_room_for_update(packet, maximum_bits, first_component_index,
            component_count, engine_update_mask))
        {
            if (helper.write_component_header(6, "player-roles-exist"))
                packet->write_integer("player-roles", state->player_roles, 16);
            helper.finish_component();
            if (helper.write_component_header(7, "edited-object-ids-exist"))
            {
                short player_index;
                for (player_index = 0; player_index < 16; ++player_index)
                {
                    if (state->edited_object_ids[player_index] == -1)
                        packet->write_bool("edited-object-id-exists", false);
                    else
                    {
                        packet->write_bool("edited-object-id-exists", true);
                        packet->write_integer("edited-object-id", state->edited_object_ids[player_index], 10);
                    }
                }
            }
            helper.finish_component();
            if (helper.write_component_header(8, "total-budget-exists"))
                packet->write_integer("total-budget", state->total_budget, 16);
            helper.finish_component();
            if (helper.write_component_header(9, "variant-object-count-exists"))
                packet->write_integer("variant-object-count", state->variant_object_count, 32);
            helper.finish_component();
            if (helper.write_component_header(10, "game-type-exists"))
                packet->write_integer("game-type", state->game_type, 32);
            helper.finish_component();
            helper.finish_update(updated_mask);
            result = true;
        }
    }
    return result;
}

// @address 0x4294b0 ?entity_update_encode@c_simulation_infection_engine_globals_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_infection_engine_globals_definition::entity_update_encode(bool initial_update,
    unsigned long update_mask, unsigned long *updated_mask, long state_data_size,
    const void *state_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long maximum_bits, bool final_update)
{
    const s_infection_engine_state_data *state = (const s_infection_engine_state_data *)state_data;
    bool result = false;
    unsigned long global_update_mask = update_mask & 63;
    long global_maximum_bits = maximum_bits + 5;
    if (global_update_encode(global_update_mask, updated_mask,
        (const s_game_engine_state_data *)state, packet, global_maximum_bits))
    {
        c_entity_update_encode_helper helper;
        long first_component_index = 6;
        long component_count = 5;
        unsigned long infection_update_mask = update_mask & (((1i64 << component_count) - 1) << first_component_index);
        if (helper.make_room_for_update(packet, maximum_bits, first_component_index,
            component_count, infection_update_mask))
        {
            if (helper.write_component_header(6, "zombie-bitvector-exists"))
                packet->write_integer("zombie-bitvector", state->zombie_bitvector, 16);
            helper.finish_component();
            if (helper.write_component_header(7, "first-zombie-bitvector-exists"))
                packet->write_integer("first-zombie-bitvector", state->first_zombie_bitvector, 16);
            helper.finish_component();
            if (helper.write_component_header(8, "players-in-safe-haven-bitvector-exists"))
                packet->write_integer("players-in-safe-haven-bitvector", state->players_in_safe_haven_bitvector, 16);
            helper.finish_component();
            if (helper.write_component_header(9, "safe-haven-timer-exists"))
                packet->write_integer("safe-haven-timer", state->safe_haven_timer, 32);
            helper.finish_component();
            if (helper.write_component_header(10, "zombie-team-bitvector-exists"))
                packet->write_integer("zombie-team-bitvector", state->zombie_team_bitvector, 16);
            helper.finish_component();
            helper.finish_update(updated_mask);
            result = true;
        }
    }
    return result;
}

// @address 0x429668 ?entity_update_decode@c_simulation_infection_engine_globals_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_infection_engine_globals_definition::entity_update_decode(bool initial_update,
    unsigned long *update_mask, long state_data_size, void *state_data,
    c_bitstream *packet, bool use_encoding)
{
    unsigned long decoded_mask = 0;
    bool success = true;
    success = success && c_simulation_game_engine_globals_definition::global_update_decode(&decoded_mask,
        (s_game_engine_state_data *)state_data, packet);
    unsigned long mask;

    if (packet->read_bool("zombie-bitvector-exists"))
    {
        ((s_infection_engine_state_data *)state_data)->zombie_bitvector = (unsigned short)packet->read_integer("zombie-bitvector", 16);
        mask = decoded_mask | 64;
    }
    else
    {
        mask = decoded_mask;
    }

    if (packet->read_bool("first-zombie-bitvector-exists"))
    {
        ((s_infection_engine_state_data *)state_data)->first_zombie_bitvector = (unsigned short)packet->read_integer("first-zombie-bitvector", 16);
        mask |= 128;
    }

    unsigned long final_mask = mask;
    if (packet->read_bool("players-in-safe-haven-bitvector-exists"))
    {
        ((s_infection_engine_state_data *)state_data)->players_in_safe_haven_bitvector = (unsigned short)packet->read_integer(
            "players-in-safe-haven-bitvector", 16);
        final_mask |= 256;
    }

    if (packet->read_bool("safe-haven-timer-exists"))
    {
        ((s_infection_engine_state_data *)state_data)->safe_haven_timer = packet->read_integer("safe-haven-timer", 32);
        final_mask |= 512;
    }

    if (packet->read_bool("zombie-team-bitvector-exists"))
    {
        ((s_infection_engine_state_data *)state_data)->zombie_team_bitvector = (unsigned short)packet->read_integer("zombie-team-bitvector", 16);
        final_mask |= 1024;
    }

    success = success && final_mask != 0;
    *update_mask = final_mask;
    return success;
}
