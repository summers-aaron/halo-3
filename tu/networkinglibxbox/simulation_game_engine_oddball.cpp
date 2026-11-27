// networkinglibxbox/simulation_game_engine_oddball.obj: from the records 0x428168 0x428178 0x428180 0x428210 0x428298 0x428530 0x428740 0x709ea8 0x70c730
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_definition.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "math/real_math_globals.h"
#include "simulation/simulation_encoding.h"
#include "cseries/cseries.h"
#include "simulation/simulation_priority.h"
#include "math/real_math_types.h"
#include <math.h>
#include "memory/bitstream.h"
#include "memory/static_arrays.h"
#include "simulation/simulation_entity_types.h"
struct s_game_engine_state_data;

namespace {
struct s_oddball_compare_state
{
    char global_state[44];
    c_static_array<long, 5> ball_object_indices;
    c_static_array<real_point3d, 5> ball_positions;
};
}
// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x3adbd8 ?simulation_write_quantized_position@@YAXPAVc_bitstream@@PBTreal_point3d@@J_NPBTreal_rectangle3d@@@Z
// @address 0x3ade48 ?simulation_compare_quantized_positions@@YA_NPBTreal_point3d@@0JPBTreal_rectangle3d@@@Z
// @address 0x43bbe8 ?global_update_encode@c_simulation_game_engine_globals_definition@@SA_NKPAKPBUs_game_engine_state_data@@PAVc_bitstream@@J@Z
// @address 0x446800 ?simulation_calculate_entity_update_priority@@YAMPBUs_simulation_entity@@PBUs_entity_update_data@@PAMPAJ@Z
// @address 0x446e88 ?make_room_for_update@c_entity_update_encode_helper@@QAA_NPAVc_bitstream@@JJJK@Z
// @address 0x446ef8 ?write_component_header@c_entity_update_encode_helper@@QAA_NJPBD@Z
// @address 0x446f80 ?finish_component@c_entity_update_encode_helper@@QAAXXZ
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x43be48 ?global_update_decode@c_simulation_game_engine_globals_definition@@SA_NPAKPAUs_game_engine_state_data@@PAVc_bitstream@@@Z

// @address 0x47828 ??_7c_simulation_oddball_engine_globals_definition@@6B@
// @address 0x709ea8 ??__Eg_simulation_oddball_engine_globals_definition@@YAXXZ
// @address 0x70c730 ??__Fg_simulation_oddball_engine_globals_definition@@YAXXZ
// @address 0x7bb42c g_simulation_oddball_engine_globals_definition
c_simulation_oddball_engine_globals_definition g_simulation_oddball_engine_globals_definition;

// @address 0x428168 ?entity_type_name@c_simulation_oddball_engine_globals_definition@@UAAPBDXZ
const char *c_simulation_oddball_engine_globals_definition::entity_type_name()
{
    return "oddball-engine-globals";
}

// @address 0x428180 ?update_minimum_required_bits@c_simulation_oddball_engine_globals_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_oddball_engine_globals_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data)
{
  int minimum_bits;
  unsigned int update_flags;
  update_flags = update_data->update_mask;
  minimum_bits = global_update_minimum_required_bits(update_data);
  minimum_bits = minimum_bits + 4;
  if (((update_flags & 0x40) != 0) && (0x32 <= minimum_bits)) {
    minimum_bits = 0x32;
  }
  if (((update_flags & 0x80) != 0) && (0xff <= minimum_bits)) {
    minimum_bits = 0xff;
  }
  if (((update_flags & 0x100) != 0) && (0x32 <= minimum_bits)) {
    minimum_bits = 0x32;
  }
  if (((update_flags & 0x200) != 0) && (0x46 <= minimum_bits)) {
    minimum_bits = 0x46;
  }
  return minimum_bits;
}


// @address 0x428210 ?write_update_description_to_string@c_simulation_oddball_engine_globals_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_oddball_engine_globals_definition::write_update_description_to_string(
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
  csnzprintf(buffer, buffer_size, "oddball update: %s relevance=%5.3f: period=%d", name, relevance, period);
}







// @address 0x428298 ?entity_update_encode@c_simulation_oddball_engine_globals_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_oddball_engine_globals_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *written_update_mask, long state_data_size, const void *state_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, long maximum_bits, bool flag)
{
    const s_oddball_engine_state_data *state = (const s_oddball_engine_state_data *)state_data;
    bool result = false;
    if (global_update_encode(update_mask & 63, written_update_mask,
            (const s_game_engine_state_data *)state, packet, maximum_bits + 4)) {
        c_entity_update_encode_helper helper;
        if (helper.make_room_for_update(packet, maximum_bits, 6, 4, update_mask & 0x3c0)) {
            if (helper.write_component_header(6, "spawn-timers-exist")) {
                for (long i = 0; i < 5; ++i) {
                    packet->write_bool("spawn-timer-exists", state->spawn_timers[i] != 0);
                    if (state->spawn_timers[i] != 0)
                        packet->write_integer("spawn-timer", state->spawn_timers[i], 12);
                }
            }
            helper.finish_component();
            if (helper.write_component_header(7, "spawn-positions-exist")) {
                for (long i = 0; i < 5; ++i) {
                    bool exists = fabs(state->spawn_positions[i].x) < 0.0001f &&
                        fabs(state->spawn_positions[i].y) < 0.0001f &&
                        fabs(state->spawn_positions[i].z) < 0.0001f;
                    packet->write_bool("spawn-position-exists", exists);
                    if (exists)
                        simulation_write_quantized_position(packet, &state->spawn_positions[i], 16, false, 0);
                }
            }
            helper.finish_component();
            if (helper.write_component_header(8, "reset-timers-exist")) {
                for (long i = 0; i < 5; ++i) {
                    packet->write_bool("reset-timer-exists", state->reset_timers[i] != 0);
                    if (state->reset_timers[i] != 0)
                        packet->write_integer("reset-timer", state->reset_timers[i], 12);
                }
            }
            helper.finish_component();
            if (helper.write_component_header(9, "ownership-exists")) {
                for (long i = 0; i < 5; ++i) {
                    packet->write_integer("owner-player-absolute-index", state->owner_player_indices[i] + 1, 5);
                    packet->write_integer("ball-seconds", state->ball_seconds[i], 7);
                }
            }
            helper.finish_component();
            helper.finish_update(written_update_mask);
            result = true;
        }
    }
    return result;
}


// @address 0x428530 ?entity_update_decode@c_simulation_oddball_engine_globals_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_oddball_engine_globals_definition::entity_update_decode(bool, unsigned long *out_flags, long, void *state_data, c_bitstream *stream, bool)
{
    s_oddball_engine_state_data *state = static_cast<s_oddball_engine_state_data *>(state_data);
    unsigned long local = 0;
    unsigned long flags;
    bool valid = !(!((unsigned char)c_simulation_game_engine_globals_definition::global_update_decode(&local, (s_game_engine_state_data *)state, stream))) && 1;
    if (stream->read_bool("spawn-timers-exist")) {
        for (long i = 0; i < 5; ++i) {
            if (stream->read_bool("spawn-timer-exists"))
                state->spawn_timers[i] = stream->read_integer("spawn-timer", 12);
            else
                state->spawn_timers[i] = 0;
        }
        flags = local | 64;
    } else {
        flags = local;
    }
    if (stream->read_bool("spawn-positions-exist")) {
        for (long i = 0; i < 5; ++i) {
            if (stream->read_bool("spawn-position-exists"))
                ::simulation_read_quantized_position(
                    stream, &state->spawn_positions[i], 16, 0);
            else
                state->spawn_positions[i] = *global_origin3d;
        }
        flags |= 128;
    }
    if (stream->read_bool("reset-timers-exist")) {
        for (long i = 0; i < 5; ++i) {
            if (stream->read_bool("reset-timer-exists"))
                state->reset_timers[i] = (short)stream->read_integer("reset-timer", 12);
            else
                state->reset_timers[i] = 0;
        }
        flags |= 256;
    }
    if (stream->read_bool("ownership-exists")) {
        for (long i = 0; i < 5; ++i) {
            state->owner_player_indices[i] = (short)(stream->read_integer("owner-player-absolute-index", 5) - 1);
            state->ball_seconds[i] = (char)stream->read_integer("ball-seconds", 7);
        }
        flags |= 512;
    }
    {
        bool result = valid && flags != 0;
        *out_flags = flags;
        return result;
    }
}

// @address 0x45fc28 ?entity_type@c_simulation_oddball_engine_globals_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_oddball_engine_globals_definition::entity_type()
{
    return _simulation_entity_type_oddball_engine_globals;
}

// @address 0x45fc50 ?update_flag_count@c_simulation_oddball_engine_globals_definition@@UAAJXZ
long c_simulation_oddball_engine_globals_definition::update_flag_count()
{
    return 10;
}

// @address 0x428178 ?state_data_size@c_simulation_oddball_engine_globals_definition@@UAAJXZ
long c_simulation_oddball_engine_globals_definition::state_data_size()
{
    return 152;
}

// @address 0x427368 ?initial_update_mask@c_simulation_oddball_engine_globals_definition@@UAAKXZ
unsigned long c_simulation_oddball_engine_globals_definition::initial_update_mask()
{
    return 63;
}

// @address 0x428740 ?entity_state_lossy_compare@c_simulation_oddball_engine_globals_definition@@UAA_NPAX0J@Z
bool c_simulation_oddball_engine_globals_definition::entity_state_lossy_compare(void *first_data, void *second_data, long size)
{
    s_oddball_compare_state *first = (s_oddball_compare_state *)first_data;
    s_oddball_compare_state *second = (s_oddball_compare_state *)second_data;
    bool equal = true;
    for (long ball = 0; ball < 5; ++ball) {
        equal = equal && simulation_compare_quantized_positions(&first->ball_positions[ball], &second->ball_positions[ball], 16, 0);
        first->ball_positions[ball] = *global_origin3d;
        second->ball_positions[ball] = *global_origin3d;
    }
    return equal;
}
