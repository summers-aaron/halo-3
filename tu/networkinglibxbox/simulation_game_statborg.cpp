// networkinglibxbox/simulation_game_statborg.obj: from the records 0x42b820 0x42b838 0x42b898 0x42b908 0x42b990 0x42ba10 0x42ba68 0x42bae0 0x42bb10 0x42bb90 0x42bc40 0x42bdd0 0x709ef8 0x70c7a8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_definition.h"
#include "simulation/simulation_entity_types.h"
#include "memory/tls_globals.h"
#include "networking/simulation/simulation_entity_database.h"
#include "simulation/simulation_game_entity_definitions.h"
#include <string.h>
#include "simulation/simulation_priority.h"
#include "cseries/cseries.h"
#include "game/game_engine_default.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_list.h"
#include "simulation/game_interface/simulation_game_statborg.h"

class c_bitstream;

#include "simulation/game_interface/simulation_game_statborg_entity_definition.h"

#include "memory/bitstream.h"
struct s_statborg_entry
{
    short stats[11];
    bool flag;
};
struct s_simulation_game_statborg_state_data
{
    s_statborg_entry players[16];
    s_statborg_entry teams[8];
    bool flag;
};


// @address 0x48278 ??_7c_simulation_game_statborg_entity_definition@@6B@
// @address 0x709ef8 ??__Eg_simulation_game_statborg_entity_definition@@YAXXZ
// @address 0x70c7a8 ??__Fg_simulation_game_statborg_entity_definition@@YAXXZ
// @address 0x7bb450 g_simulation_game_statborg_entity_definition
c_simulation_game_statborg_entity_definition g_simulation_game_statborg_entity_definition;

enum { k_statborg_baseline_size = 0x242 };

namespace
{
    static __forceinline c_game_engine *statborg_current_game_engine()
    {
        long index = ((s_game_engine_globals *)tls_game_engine_globals())->game_engine_index;
        if (index < 1)
            return 0;
        if (index > 10)
            return 0;
        return game_engines[index];
    }

    static __forceinline long game_engine_globals_get_statborg_gamestate_index()
    {
        long index = -1;
        if (statborg_current_game_engine())
            index = ((s_game_engine_globals *)tls_game_engine_globals())->statborg_gamestate_index;
        return index;
    }

    static __forceinline void game_engine_globals_set_statborg_gamestate_index(long gamestate_index)
    {
        ((s_game_engine_globals *)tls_game_engine_globals())->statborg_gamestate_index = gamestate_index;
    }

        static __forceinline void game_engine_globals_build_statborg_baseline(long state_size, void *state_data)
    {
        void *baseline = state_data;
        memset(baseline, 0, k_statborg_baseline_size);
    }
}

// @address 0x42b820 ?entity_type_name@c_simulation_game_statborg_entity_definition@@UAAPBDXZ
const char *c_simulation_game_statborg_entity_definition::entity_type_name()
{
    return "game-engine-statborg";
}

// @address 0x42b838 ?gameworld_attachment_valid@c_simulation_game_statborg_entity_definition@@UAA_NJ@Z
bool c_simulation_game_statborg_entity_definition::gameworld_attachment_valid(long gamestate_index)
{
    bool valid = false;
    long attached_gamestate_index = -1;
    attached_gamestate_index = game_engine_globals_get_statborg_gamestate_index();
    if (attached_gamestate_index != -1)
    {
        if (attached_gamestate_index == gamestate_index)
        {
            valid = true;
        }
    }
    return valid;
}

// @address 0x42b908 ?update_minimum_required_bits@c_simulation_game_statborg_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_game_statborg_entity_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update)
{
    unsigned long update_mask = update->update_mask;
    long minimum_bits = 25;
    for (long player_index = 0; player_index < 16; player_index++)
    {
        if ((update_mask & (1UL << player_index)) != 0)
        {
            long player_bits = 23;
            minimum_bits = (player_bits > minimum_bits ? minimum_bits : player_bits);
        }
    }
    for (long team_index = 0; team_index < 8; team_index++)
    {
        if ((update_mask & (1UL << (team_index + 16))) != 0)
        {
            long team_bits = 23;
            minimum_bits = (team_bits > minimum_bits ? minimum_bits : team_bits);
        }
    }
    if ((update_mask & (1UL << 24)) != 0)
    {
        long state_bits = 1;
        minimum_bits = (state_bits > minimum_bits ? minimum_bits : state_bits);
    }
    return minimum_bits;
}

// @address 0x42ba10 ?create_game_entity@c_simulation_game_statborg_entity_definition@@UAA_NJJPBXKJ0@Z
bool c_simulation_game_statborg_entity_definition::create_game_entity(long gamestate_index, long creation_size, const void *creation_data,
    unsigned long update_mask, long state_size, const void *state_data)
{
    bool success = true;
    game_engine_globals_set_statborg_gamestate_index(gamestate_index);
    if (update_mask != 0)
        update_game_entity(gamestate_index, update_mask, state_size, state_data);
    return success;
}

// @address 0x42ba68 ?delete_game_entity@c_simulation_game_statborg_entity_definition@@UAA_NJ@Z
bool c_simulation_game_statborg_entity_definition::delete_game_entity(long gamestate_index)
{
    bool success = false;
    if (gameworld_attachment_valid(gamestate_index))
    {
        game_engine_globals_set_statborg_gamestate_index(-1);
        success = true;
    }
    return success;
}

// @address 0x42bae0 ?build_baseline_state_data@c_simulation_game_statborg_entity_definition@@UAA_NJPBXJPAX@Z
bool c_simulation_game_statborg_entity_definition::build_baseline_state_data(long creation_size, const void *creation_data, long state_size, void *state_data)
{
    bool success = true;
    game_engine_globals_build_statborg_baseline(state_size, state_data);
    return success;
}

// @address 0x42bb10 ?build_updated_state_data@c_simulation_game_statborg_entity_definition@@UAA_NPBUs_simulation_entity@@PAKJPAX@Z
bool c_simulation_game_statborg_entity_definition::build_updated_state_data(
    const s_simulation_entity *entity, unsigned long *update_mask, long size, void *state)
{
    c_game_statborg *statborg = 0;
    if (statborg_current_game_engine() != 0)
        statborg = &((s_game_engine_globals *)tls_game_engine_globals())->statborg;
    statborg->build_statborg_update(update_mask, size, state);
    return true;
}

// @address 0x42bb90 ?update_game_entity@c_simulation_game_statborg_entity_definition@@UAA_NJKJPBX@Z
bool c_simulation_game_statborg_entity_definition::update_game_entity(long gamestate_index, unsigned long update_mask, long state_size, const void *state_data)
{
    bool updated = false;
    if (gameworld_attachment_valid(gamestate_index))
    {
        c_game_statborg *statborg = 0;
        if (statborg_current_game_engine() != 0)
            statborg = &((s_game_engine_globals *)tls_game_engine_globals())->statborg;
        if (statborg->apply_statborg_update(update_mask, state_size, state_data))
            updated = true;
    }
    return updated;
}

// @address 0x42bc40 ?entity_update_encode@c_simulation_game_statborg_entity_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_game_statborg_entity_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *written_update_mask, long size, const void *state, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long maximum_bits, bool final_update)
{
    s_simulation_game_statborg_state_data *state_data = (s_simulation_game_statborg_state_data *)state;
    bool result = false;
    c_entity_update_encode_helper helper;
    if (helper.make_room_for_update(packet, maximum_bits, 0, 25, update_mask))
    {
        long index;
        for (index = 0; index < 16; ++index)
        {
            if (helper.write_component_header(index, "player-update-exists"))
            {
                for (long stat = 0; stat < 11; ++stat)
                    packet->write_integer("stat", (unsigned short)state_data->players[index].stats[stat], 16);
                packet->write_bool("exists", state_data->players[index].flag);
            }
            helper.finish_component();
        }
        for (index = 0; index < 8; ++index)
        {
            if (helper.write_component_header(index + 16, "team-update-exists"))
            {
                for (long stat = 0; stat < 11; ++stat)
                    packet->write_integer("stat", (unsigned short)state_data->teams[index].stats[stat], 16);
                packet->write_bool("exists", state_data->teams[index].flag);
            }
            helper.finish_component();
        }
        if (helper.write_component_header(24, "finalized-exists"))
            packet->write_bool("exists", state_data->flag);
        helper.finish_component();
        helper.finish_update(written_update_mask);
        result = true;
    }
    return result;
}
// @address 0x42bdd0 ?entity_update_decode@c_simulation_game_statborg_entity_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_game_statborg_entity_definition::entity_update_decode(
    bool initial_update, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *stream, bool unknown)
{
    s_simulation_game_statborg_state_data *state = (s_simulation_game_statborg_state_data *)state_data;
    unsigned long mask = 0;
    bool success = true;
    for (long player = 0; player < 16; ++player)
    {
        if (stream->read_bool("player-update-exists"))
        {
            for (long stat = 0; stat < 11; ++stat)
            {
                unsigned long value = stream->read_integer("stat", 16);
                if (value & 0x8000)
                    value |= 0xffff0000UL;
                state->players[player].stats[stat] = (short)value;
            }
            state->players[player].flag = stream->read_bool("exists");
            mask |= 1UL << player;
        }
    }
    for (long team = 0; team < 8; ++team)
    {
        if (stream->read_bool("team-update-exists"))
        {
            for (long stat = 0; stat < 11; ++stat)
            {
                unsigned long value = stream->read_integer("stat", 16);
                if (value & 0x8000)
                    value |= 0xffff0000UL;
                state->teams[team].stats[stat] = (short)value;
            }
            state->teams[team].flag = stream->read_bool("exists");
            mask |= 1UL << (team + 16);
        }
    }
    if (stream->read_bool("exists"))
    {
        state->flag = stream->read_bool("exists");
        mask |= 1UL << 24;
    }
    success = success && mask != 0;
    *update_mask = mask;
    return success;
}

extern "C" {
 /* "statborg creation: relevance=%5.3f" */
}

// @address 0x42b898 ?write_creation_description_to_string@c_simulation_game_statborg_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@JPAD@Z
void c_simulation_game_statborg_entity_definition::write_creation_description_to_string(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer)
{
    float relevance;

    relevance = 0.0f;
    simulation_calculate_entity_creation_priority(entity, telemetry, &relevance);
    csnzprintf(buffer, buffer_size, "statborg creation: relevance=%5.3f", relevance);
}

extern "C" {
 /* "statborg update: relevance=%5.3f: period=%d" */
}

// @address 0x42b990 ?write_update_description_to_string@c_simulation_game_statborg_entity_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_game_statborg_entity_definition::write_update_description_to_string(
    const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer)
{
    float relevance;
    int period;

    relevance = 0.0f;
    period = 0;
    ::simulation_calculate_entity_update_priority(entity, update_data, &relevance, (long *)&period);
    csnzprintf(buffer, buffer_size, "statborg update: relevance=%5.3f:period=%d", relevance, period);
}

// @address 0x426e90 ?entity_type@c_simulation_game_statborg_entity_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_game_statborg_entity_definition::entity_type()
{
    return _simulation_entity_type_game_engine_statborg;
}

// @address 0x42b830 ?state_data_size@c_simulation_game_statborg_entity_definition@@UAAJXZ
long c_simulation_game_statborg_entity_definition::state_data_size()
{
    return 578;
}

// @address 0x42d298 ?creation_data_size@c_simulation_game_statborg_entity_definition@@UAAJXZ
long c_simulation_game_statborg_entity_definition::creation_data_size()
{
    return 0;
}

// @address 0x42e798 ?update_flag_count@c_simulation_game_statborg_entity_definition@@UAAJXZ
long c_simulation_game_statborg_entity_definition::update_flag_count()
{
    return 25;
}

// @address 0x42d298 ?initial_update_mask@c_simulation_game_statborg_entity_definition@@UAAKXZ
unsigned long c_simulation_game_statborg_entity_definition::initial_update_mask()
{
    return 0;
}

// @address 0x42d380 ?entity_replication_required_for_view_activation@c_simulation_game_statborg_entity_definition@@UAA_NPBUs_simulation_entity@@@Z
bool c_simulation_game_statborg_entity_definition::entity_replication_required_for_view_activation(const s_simulation_entity *entity)
{
    return true;
}

// @address 0x42d298 ?entity_type_is_gameworld_object@c_simulation_game_statborg_entity_definition@@UAA_NXZ
bool c_simulation_game_statborg_entity_definition::entity_type_is_gameworld_object()
{
    return false;
}

// @address 0x42d380 ?entity_can_be_created@c_simulation_game_statborg_entity_definition@@UAA_NPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_game_statborg_entity_definition::entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42d298 ?creation_minimum_required_bits@c_simulation_game_statborg_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_game_statborg_entity_definition::creation_minimum_required_bits(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 0;
}

// @address 0x5bec98 ?entity_creation_encode@c_simulation_game_statborg_entity_definition@@UAAXJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@_N@Z
void c_simulation_game_statborg_entity_definition::entity_creation_encode(long creation_data_size, const void *creation_data,
    const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update)
{
}

// @address 0x42d380 ?entity_creation_decode@c_simulation_game_statborg_entity_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_game_statborg_entity_definition::entity_creation_decode(long creation_data_size, void *creation_data, c_bitstream *packet, bool final_update)
{
    return true;
}

// @address 0x42d380 ?entity_state_lossy_compare@c_simulation_game_statborg_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_game_statborg_entity_definition::entity_state_lossy_compare(void *first, void *second, long size)
{
    return true;
}

// @address 0x42d380 ?entity_creation_lossy_compare@c_simulation_game_statborg_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_game_statborg_entity_definition::entity_creation_lossy_compare(void *first, void *second, long size)
{
    return true;
}

// @address 0x5bec98 ?build_creation_data@c_simulation_game_statborg_entity_definition@@UAAXJJPAX@Z
void c_simulation_game_statborg_entity_definition::build_creation_data(long gamestate_index, long size, void *creation_data)
{
}

// @address 0x5bec98 ?rotate_entity_indices@c_simulation_game_statborg_entity_definition@@UAAXPBUs_simulation_entity@@@Z
void c_simulation_game_statborg_entity_definition::rotate_entity_indices(const s_simulation_entity *entity)
{
}

// @address 0x5bec98 ?prepare_creation_data_for_gameworld@c_simulation_game_statborg_entity_definition@@UAAXJPAX@Z
void c_simulation_game_statborg_entity_definition::prepare_creation_data_for_gameworld(long size, void *creation_data)
{
}

// @address 0x5bec98 ?prepare_state_data_for_gameworld@c_simulation_game_statborg_entity_definition@@UAAXKJPAX@Z
void c_simulation_game_statborg_entity_definition::prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state)
{
}
