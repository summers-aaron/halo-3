// OBJ: from the records 0x427af0 0x427b00 0x427b88 0x427c48 0x427f08 0x709e98 0x70c718
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_definition.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "simulation/simulation_entity_types.h"
// @address 0x427af0 ?entity_type_name@c_simulation_assault_engine_globals_definition@@UAAPBDXZ
// @address 0x427c48 ?entity_update_encode@c_simulation_assault_engine_globals_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
// @address 0x427f08 ?entity_update_decode@c_simulation_assault_engine_globals_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
#include "simulation/simulation_priority.h"
#include "cseries/cseries.h"
#include "memory/bitstream.h"

struct s_game_engine_state_data;

// @address 0x426e88 ?state_data_size@c_simulation_assault_engine_globals_definition@@UAAJXZ
long c_simulation_assault_engine_globals_definition::state_data_size()
{
    return 136;
}

const char *c_simulation_assault_engine_globals_definition::entity_type_name()
{
    return "assault-engine-globals";
}

// @address 0x3fc598 ?entity_type@c_simulation_assault_engine_globals_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_assault_engine_globals_definition::entity_type()
{
    return _simulation_entity_type_assault_engine_globals;
}

// @address 0x42e748 ?update_flag_count@c_simulation_assault_engine_globals_definition@@UAAJXZ
long c_simulation_assault_engine_globals_definition::update_flag_count()
{
    return 16;
}

// @address 0x427498 ?initial_update_mask@c_simulation_assault_engine_globals_definition@@UAAKXZ
unsigned long c_simulation_assault_engine_globals_definition::initial_update_mask()
{
    return 127;
}

// @address 0x42d380 ?entity_state_lossy_compare@c_simulation_assault_engine_globals_definition@@UAA_NPAX0J@Z
bool c_simulation_assault_engine_globals_definition::entity_state_lossy_compare(void *, void *, long)
{
    return true;
}

// @address 0x427b00 ?write_update_description_to_string@c_simulation_assault_engine_globals_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_assault_engine_globals_definition::write_update_description_to_string(
    const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer)
{
    char name[1024];
    float relevance = 0.0f;
    long period = 0;
    global_update_write_description_to_string(update_data, sizeof(name), name);
    simulation_calculate_entity_update_priority(entity, update_data, &relevance, &period);
    csnzprintf(buffer, buffer_size, "assault update: %s relevance=%5.3f: period=%d", name, relevance, period);
}

// @address 0x427b88 ?update_minimum_required_bits@c_simulation_assault_engine_globals_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_assault_engine_globals_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data)

{
  int minimum_bits;
  unsigned int assault_update_mask;
  
  assault_update_mask = (unsigned int)update_data->update_mask;
  minimum_bits = global_update_minimum_required_bits(update_data);
  minimum_bits = minimum_bits + 10;
  if (((assault_update_mask & 0x40) != 0) && (0x14 <= (int)minimum_bits)) {
    minimum_bits = 0x14;
  }
  if (((assault_update_mask & 0x2000) != 0) && (0x58 <= (int)minimum_bits)) {
    minimum_bits = 0x58;
  }
  if (((assault_update_mask & 0x1000) != 0) && (0x6a <= (int)minimum_bits)) {
    minimum_bits = 0x6a;
  }
  if (((assault_update_mask & 0x4000) != 0) && (0x2b <= (int)minimum_bits)) {
    minimum_bits = 0x2b;
  }
  if (((assault_update_mask & 0x8000) != 0) && (0x3d <= (int)minimum_bits)) {
    minimum_bits = 0x3d;
  }
  if (((assault_update_mask & 0x800) != 0) && (0xf <= (int)minimum_bits)) {
    minimum_bits = 0xf;
  }
  return minimum_bits;
}

// @address 0x47710 ??_7c_simulation_assault_engine_globals_definition@@6B@
// @address 0x709e98 ??__Eg_simulation_assault_engine_globals_definition@@YAXXZ
// @address 0x70c718 ??__Fg_simulation_assault_engine_globals_definition@@YAXXZ
// @address 0x7bb428 g_simulation_assault_engine_globals_definition
c_simulation_assault_engine_globals_definition g_simulation_assault_engine_globals_definition;

bool c_simulation_assault_engine_globals_definition::entity_update_encode(bool initial_update,
    unsigned long update_mask, unsigned long *updated_mask, long state_data_size,
    const void *state_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long maximum_bits, bool final_update)
{
    const s_assault_engine_state_data *state = (const s_assault_engine_state_data *)state_data;
    bool result = false;
    if (global_update_encode(update_mask & 63, updated_mask,
        (const s_game_engine_state_data *)state, packet, maximum_bits + 10))
    {
        c_entity_update_encode_helper helper;
        if (helper.make_room_for_update(packet, maximum_bits, 6, 10, update_mask & 0xffc0))
        {
            if (helper.write_component_header(6, "defensive-team-exists"))
                packet->write_integer("defensive-team", state->defensive_team + 1, 4);
            helper.finish_component();

            if (helper.write_component_header(13, "bomb-reset-timers"))
            {
                for (long timer_index = 0; timer_index < 9; ++timer_index)
                {
                    packet->write_bool("bomb-reset-timer-exists", state->bomb_reset_timers[timer_index] + 1 != 0);
                    if (state->bomb_reset_timers[timer_index] != -1)
                        packet->write_integer("bomb-reset-timer", state->bomb_reset_timers[timer_index], 8);
                }
            }
            helper.finish_component();

            if (helper.write_component_header(12, "bomb-arming-timers"))
            {
                for (long timer_index = 0; timer_index < 9; ++timer_index)
                {
                    packet->write_bool("bomb-arming-timer-exists", state->bomb_arming_timers[timer_index] + 1 != 0);
                    if (state->bomb_arming_timers[timer_index] != -1)
                        packet->write_integer("bomb-arming-timer", state->bomb_arming_timers[timer_index], 10);
                }
            }
            helper.finish_component();

            if (helper.write_component_header(14, "bomb-weapon-flags"))
            {
                for (long flag_index = 0; flag_index < 9; ++flag_index)
                {
                    packet->write_bool("bomb-weapon-flag-exists", state->bomb_weapon_flags[flag_index] != 0);
                    if (state->bomb_weapon_flags[flag_index] != 0)
                        packet->write_integer("bomb-weapon-flag", state->bomb_weapon_flags[flag_index], 3);
                }
            }
            helper.finish_component();

            if (helper.write_component_header(15, "player-that-placed-bombs"))
            {
                for (long player_index = 0; player_index < 9; ++player_index)
                    packet->write_integer("player-that-placed-bomb", state->player_that_placed_bombs[player_index] + 1, 5);
            }
            helper.finish_component();

            if (helper.write_component_header(11, "helper-flags-exist"))
                packet->write_integer("helper-flags", state->helper_flags, 3);
            helper.finish_component();

            helper.finish_update(updated_mask);
            result = true;
        }
    }
    return result;
}

bool c_simulation_assault_engine_globals_definition::entity_update_decode(
    bool initial_update,
    unsigned long *update_mask,
    long state_data_size,
    void *state_data,
    c_bitstream *packet,
    bool use_encoding)
{
    s_assault_engine_state_data *state = (s_assault_engine_state_data *)state_data;
    unsigned long decoded_mask = 0;
    bool success = true;
    success = success && c_simulation_game_engine_globals_definition::global_update_decode(
        &decoded_mask,
        (s_game_engine_state_data *)state,
        packet);
    unsigned long mask;

    if (packet->read_bool("defensive-team-exists"))
    {
        long defensive_team = packet->read_integer("defensive-team", 4) - 1;
        state->defensive_team = defensive_team;
        if (defensive_team != -1)
        {
            success = (success && defensive_team >= 0) ? (defensive_team < 8) : 0;
        }
        mask = decoded_mask | 0x40;
    }
    else
    {
        mask = decoded_mask;
    }

    if (packet->read_bool("bomb-reset-timers"))
    {
        for (long timer_index = 0; timer_index < 9; ++timer_index)
        {
            if (packet->read_bool("bomb-reset-timer-exists"))
            {
                state->bomb_reset_timers[timer_index] = (short)packet->read_integer("bomb-reset-timer", 8);
            }
            else
            {
                state->bomb_reset_timers[timer_index] = -1;
            }
        }
        mask |= 0x2000;
    }

    if (packet->read_bool("bomb-arming-timers"))
    {
        for (long timer_index = 0; timer_index < 9; ++timer_index)
        {
            if (packet->read_bool("bomb-arming-timer-exists"))
            {
                state->bomb_arming_timers[timer_index] = (short)packet->read_integer("bomb-arming-timer", 10);
            }
            else
            {
                state->bomb_arming_timers[timer_index] = -1;
            }
        }
        mask |= 0x1000;
    }

    if (packet->read_bool("bomb-weapon-flags"))
    {
        for (long flag_index = 0; flag_index < 9; ++flag_index)
        {
            if (packet->read_bool("bomb-weapon-flag-exists"))
            {
                state->bomb_weapon_flags[flag_index] = (unsigned char)packet->read_integer("bomb-weapon-flag", 3);
            }
            else
            {
                state->bomb_weapon_flags[flag_index] = 0;
            }
        }
        mask |= 0x4000;
    }

    if (packet->read_bool("player-that-placed-bombs"))
    {
        for (long player_index = 0; player_index < 9; ++player_index)
        {
            state->player_that_placed_bombs[player_index] = packet->read_integer("player-that-placed-bomb", 5) - 1;
        }
        mask |= 0x8000;
    }

    if (packet->read_bool("helper-flags-exist"))
    {
        state->helper_flags = (unsigned short)packet->read_integer("helper-flags", 3);
        mask |= 0x800;
    }

    success = success && mask != 0;
    *update_mask = mask;
    return success;
}
