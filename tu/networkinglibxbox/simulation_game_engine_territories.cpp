// networkinglibxbox/simulation_game_engine_territories.obj: from the records 0x428ad8 0x428ae8 0x428af0 0x428b00 0x428ba8 0x428c30 0x428e18 0x709ec8 0x70c760
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_definition.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "simulation/simulation_game_entity_definitions.h"
#include "memory/bitstream.h"
#include "simulation/simulation_priority.h"
#include "cseries/cseries.h"
#include "simulation/simulation_entity_types.h"





// @address 0x428ad8 ?entity_type_name@c_simulation_territories_engine_globals_definition@@UAAPBDXZ
const char *c_simulation_territories_engine_globals_definition::entity_type_name()
{
    return "territories-engine-globals";
}

// @address 0x428ae8 ?state_data_size@c_simulation_territories_engine_globals_definition@@UAAJXZ
long c_simulation_territories_engine_globals_definition::state_data_size()
{
    return 160;
}

// @address 0x428af0 ?initial_update_mask@c_simulation_territories_engine_globals_definition@@UAAKXZ
unsigned long c_simulation_territories_engine_globals_definition::initial_update_mask()
{
    return (1 << 17) - 1;
}

// @address 0x428ad0 ?entity_type@c_simulation_territories_engine_globals_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_territories_engine_globals_definition::entity_type()
{
    return _simulation_entity_type_territories_engine_globals;
}

// @address 0x45fca8 ?update_flag_count@c_simulation_territories_engine_globals_definition@@UAAJXZ
long c_simulation_territories_engine_globals_definition::update_flag_count()
{
    return 17;
}

// @address 0x42d380 ?entity_state_lossy_compare@c_simulation_territories_engine_globals_definition@@UAA_NPAX0J@Z
bool c_simulation_territories_engine_globals_definition::entity_state_lossy_compare(void *, void *, long)
{
    return true;
}

// @address 0x428b00 ?update_minimum_required_bits@c_simulation_territories_engine_globals_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_territories_engine_globals_definition::update_minimum_required_bits(
    const s_simulation_entity *entity, const s_entity_update_data *update_data)
{
    unsigned int bits = update_data->update_mask;
    int value = global_update_minimum_required_bits(update_data) + 11;
    int index = 0;
    do {
        if (((1u << (index + 6)) & bits) != 0 && value >= 79) {
            value = 79;
        }
        index++;
    } while (index < 8);
    if ((bits & 0x4000) != 0 && value >= 21) {
        value = 21;
    }
    if ((bits & 0x8000) != 0 && value >= 41) {
        value = 41;
    }
    if ((bits & 0x10000) != 0 && value >= 33) {
        value = 33;
    }
    return value;
}

extern "C" {
 /* "territories update: %s relevance=%5.3f: period=%d" */
}

// @address 0x428ba8 ?write_update_description_to_string@c_simulation_territories_engine_globals_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_territories_engine_globals_definition::write_update_description_to_string(
    const s_simulation_entity *entity, const s_entity_update_data *update_data, long buffer_size, char *buffer)
{
    int period;
    float relevance;
    char name[1024];

    relevance = 0.0f;
    period = 0;
    global_update_write_description_to_string(update_data, sizeof(name), name);
    ::simulation_calculate_entity_update_priority(entity, update_data, &relevance, (long *)&period);
    csnzprintf(buffer, buffer_size, "territories update: %s relevance=%5.3f: period=%d", name, relevance, period);
}




extern "C" {
// @address 0x479d8 k_territory_exists_component_name
// @address 0x479ec k_territories_defending_team_exists_name
// @address 0x47a10 k_territories_sudden_death_time_exists_name
// @address 0x47a38 k_territories_grace_period_time_exists_name
// @rdata 0x820479d8..0x82047a60
const char k_territory_exists_component_name[20] = "territory-exists";
const char k_territories_defending_team_exists_name[36] = "territories-defending-team-exists";
const char k_territories_sudden_death_time_exists_name[40] = "territories-sudden-death-time-exists";
const char k_territories_grace_period_time_exists_name[40] = "territories-grace-period-time-exists";
// @rdata-end
}

// @address 0x47a60 ??_7c_simulation_territories_engine_globals_definition@@6B@
// @address 0x709ec8 ??__Eg_simulation_territories_engine_globals_definition@@YAXXZ
// @address 0x70c760 ??__Fg_simulation_territories_engine_globals_definition@@YAXXZ
// @address 0x7bb434 g_simulation_territories_engine_globals_definition
c_simulation_territories_engine_globals_definition g_simulation_territories_engine_globals_definition;

// @address 0x428c30 ?entity_update_encode@c_simulation_territories_engine_globals_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_territories_engine_globals_definition::entity_update_encode(bool initial_update, unsigned long update_mask,
    unsigned long *written_update_mask, long state_data_size, const void *state_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long maximum_bits, bool final_update)
{
    const s_territories_engine_state_data *territories_state_data = (const s_territories_engine_state_data *)state_data;
    bool success = false;

    if (global_update_encode(update_mask & 63, written_update_mask, territories_state_data, packet, maximum_bits + 11))
    {
        c_entity_update_encode_helper encode_helper;
        long first_component = 6;
        long component_count = 17 - first_component;
        unsigned long component_update_mask = update_mask & (((1 << component_count) - 1) << first_component);
        if (encode_helper.make_room_for_update(packet, maximum_bits, first_component, component_count, component_update_mask))
        {
            for (long territory_index = 0; territory_index < 8; territory_index++)
            {
                if (encode_helper.write_component_header(territory_index + 6, k_territory_exists_component_name))
                {
                    packet->write_integer("territory-controller-index", territories_state_data->territory_controller_team[territory_index], 4);
                    for (long team_index = 0; team_index < 8; team_index++)
                    {
                        packet->write_integer("territory-capture-time", territories_state_data->territory_capture_time[territory_index][team_index], 7);
                    }
                    packet->write_integer("territory-state", territories_state_data->territory_state[territory_index], 2);
                }
                encode_helper.finish_component();
            }

            if (encode_helper.write_component_header(14, k_territories_defending_team_exists_name))
            {
                packet->write_signed_integer("territories-defending-team", territories_state_data->defending_team, 4);
            }
            encode_helper.finish_component();

            if (encode_helper.write_component_header(15, k_territories_sudden_death_time_exists_name))
            {
                packet->write_integer("territories-sudden-death-time", territories_state_data->sudden_death_time + 1, 24);
            }
            encode_helper.finish_component();

            if (encode_helper.write_component_header(16, k_territories_grace_period_time_exists_name))
            {
                packet->write_integer("territories-grace-period-time", territories_state_data->grace_period_time + 1, 16);
            }
            encode_helper.finish_component();

            encode_helper.finish_update(written_update_mask);
            success = true;
        }
    }

    return success;
}

static __forceinline long territories_read_signed_integer(c_bitstream *packet, const char *name, long size_in_bits)
{
    unsigned long value = packet->read_integer(name, size_in_bits);
    if (size_in_bits < 32 && (value & (1 << (size_in_bits - 1))) != 0)
    {
        long sign_extension = ~((1i64 << size_in_bits) - 1);
        value |= sign_extension;
    }
    return value;
}

// @address 0x428e18 ?entity_update_decode@c_simulation_territories_engine_globals_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_territories_engine_globals_definition::entity_update_decode(bool initial_update,
    unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *packet, bool final_update)
{
    s_territories_engine_state_data *state = (s_territories_engine_state_data *)state_data;
    unsigned long mask = 0;
    bool success = true;
    success = success && c_simulation_game_engine_globals_definition::global_update_decode(&mask,
        (s_game_engine_state_data *)state, packet);

    for (long territory_index = 0; territory_index < 8; ++territory_index)
    {
        if (packet->read_bool("territory-exists"))
        {
            long controller_team = packet->read_integer("territory-controller-index", 4);
            state->territory_controller_team[territory_index] = controller_team;
            if (controller_team < 0 || controller_team >= 9)
            {
                success = false;
            }
            for (long team_index = 0; team_index < 8; ++team_index)
            {
                state->territory_capture_time[territory_index][team_index] =
                    (unsigned char)packet->read_integer("territory-capture-time", 7);
            }
            state->territory_state[territory_index] = (char)packet->read_integer("territory-state", 2);
            mask |= 1 << (territory_index + 6);
        }
    }

    if (packet->read_bool("territories-defending-team-exists"))
    {
        state->defending_team = territories_read_signed_integer(packet, "territories-defending-team", 4);
        if (state->defending_team < -1 || state->defending_team >= 8)
        {
            success = false;
        }
        mask |= 0x4000;
    }

    if (packet->read_bool("territories-sudden-death-time-exists"))
    {
        state->sudden_death_time = packet->read_integer("territories-sudden-death-time", 24) - 1;
        mask |= 0x8000;
    }

    if (packet->read_bool("territories-grace-period-time-exists"))
    {
        state->grace_period_time = packet->read_integer("territories-grace-period-time", 16) - 1;
        mask |= 0x10000;
    }

    if (mask == 0)
    {
        success = false;
    }
    *update_mask = mask;
    return success;
}
