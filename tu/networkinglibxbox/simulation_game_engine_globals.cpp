// networkinglibxbox/simulation_game_engine_globals.obj: from the records 0x43b520 0x43b530 0x43b5b0 0x43b5b8 0x43b5c8 0x43b5d8 0x43b668 0x43b710 0x43b7c8 0x43b850 0x43b8d0 0x43b940 0x43b9b0 0x43ba28 0x43bad8 0x43bbe8 0x43be48 0x43c060 0x43c068 0x43c110 0x43c1d0 0x43c230 0x43c260 0x43c3d8 0x43c578 0x43c710 0x43c720 0x43c750 0x43c798 0x43c7f8
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_definition.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "cseries/cseries.h"
#include "math/real_constant_pool_access.h"
#include "memory/data_types.h"
#include "memory/bitstream.h"
#include "game/player_types.h"
#include "game/game_engine_default.h"
#include "game/game_engine_list.h"
#include "game/game_engine.h"
#include "simulation/simulation_priority.h"
#include "game/game_engine_current_inline.h"
#include "game/game_engine_simulation.h"
#include "memory/data_absolute_index.h"
#include "networking/replication/replication_event.h"
#include "networking/simulation/simulation_game_engine_event_definitions.h"
#include "simulation/simulation_game_entity_definitions.h"
// hand merge: one game_engine_globals (game_engine_globals) and one s_data_array (player_data) for every record
// @address 0x43b5c8 ?event_requirements@c_simulation_game_engine_request_boot_player_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x43bbe8 ?global_update_encode@c_simulation_game_engine_globals_definition@@SA_NKPAKPBUs_game_engine_state_data@@PAVc_bitstream@@J@Z
// @address 0x43be48 ?global_update_decode@c_simulation_game_engine_globals_definition@@SA_NPAKPAUs_game_engine_state_data@@PAVc_bitstream@@@Z
// @address 0x43c3d8 ?event_payload_decode@c_simulation_game_engine_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0x43b520 ?event_type_name@c_simulation_game_engine_event_definition@@UAAPBDXZ
// @address 0x43b530 ?event_requirements@c_simulation_game_engine_event_definition@@UAA?AUs_simulation_event_requirements@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x49c94 ??_7c_simulation_game_engine_request_boot_player_event_definition@@6B@
// @address 0x43b5b0 ?event_type@c_simulation_game_engine_request_boot_player_event_definition@@UAA?AW4e_simulation_event_type@@XZ
// @address 0x43b5b8 ?event_type_name@c_simulation_game_engine_request_boot_player_event_definition@@UAAPBDXZ
// @address 0x43b5d8 ?gameworld_attachment_valid@c_simulation_game_engine_globals_definition@@UAA_NJ@Z
// @address 0x43b668 ?build_baseline_state_data@c_simulation_game_engine_globals_definition@@UAA_NJPBXJPAX@Z
// @address 0x43b710 ?build_updated_state_data@c_simulation_game_engine_globals_definition@@UAA_NPBUs_simulation_entity@@PAKJPAX@Z
// @address 0x43b7c8 ?update_game_entity@c_simulation_game_engine_globals_definition@@UAA_NJKJPBX@Z
// @address 0x43b850 ?promote_game_entity_to_authority@c_simulation_game_engine_globals_definition@@UAA_NJ@Z
// @address 0x43b940 ?create_game_entity@c_simulation_game_engine_globals_definition@@UAA_NJJPBXKJ0@Z
// @address 0x43b9b0 ?delete_game_entity@c_simulation_game_engine_globals_definition@@UAA_NJ@Z
// @address 0x43ba28 ?global_update_minimum_required_bits@c_simulation_game_engine_globals_definition@@SAJPBUs_entity_update_data@@@Z
// @address 0x43bad8 ?global_update_write_description_to_string@c_simulation_game_engine_globals_definition@@SAXPBUs_entity_update_data@@JPAD@Z
// @address 0x43c060 ?payload_size@c_simulation_game_engine_event_definition@@UAAJXZ
// @address 0x43c068 ?event_can_be_transmitted@c_simulation_game_engine_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x43c110 ?minimum_required_bits@c_simulation_game_engine_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
// @address 0x43c1d0 ?calculate_relevance@c_simulation_game_engine_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
// @address 0x43c260 ?event_payload_encode@c_simulation_game_engine_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x43c578 ?apply_game_event@c_simulation_game_engine_event_definition@@UAA_NJPBJJPBX@Z
// @address 0x43c750 ?event_payload_encode@c_simulation_game_engine_request_boot_player_event_definition@@UAAXJPBXPAVc_bitstream@@_N@Z
// @address 0x43c798 ?event_payload_decode@c_simulation_game_engine_request_boot_player_event_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
// @address 0x43c7f8 ?apply_game_event@c_simulation_game_engine_request_boot_player_event_definition@@UAA_NJPBJJPBX@Z
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>

struct s_game_engine_request_boot_player_event_data { long booted,booting; };


struct s_game_engine_state_data;
static __forceinline void game_engine_globals_set_gamestate_index(long index) { tls_game_engine_globals()->gamestate_index=index; }




const char *c_simulation_game_engine_event_definition::event_type_name()
{
    return "game_engine";
}

// @address 0x4313c8 ?event_type@c_simulation_game_engine_event_definition@@UAA?AW4e_simulation_event_type@@XZ
e_simulation_event_type c_simulation_game_engine_event_definition::event_type()
{
    return _simulation_event_type_game_engine;
}

// @address 0x42d298 ?number_of_entity_references@c_simulation_game_engine_event_definition@@UAAJXZ
long c_simulation_game_engine_event_definition::number_of_entity_references()
{
    return 0;
}

// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_game_engine_event_definition@@UAA_NXZ
bool c_simulation_game_engine_event_definition::reference_delays_entity_deletion()
{
    return false;
}

// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_game_engine_event_definition@@UAAXJPAX@Z
void c_simulation_game_engine_event_definition::prepare_event_data_for_gameworld(long payload_size, void *payload)
{
}

s_simulation_event_requirements c_simulation_game_engine_event_definition::event_requirements(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 0;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = c_simulation_game_engine_event_definition::event_can_be_transmitted(event, telemetry);
    requirements.minimum_required_bits = c_simulation_game_engine_event_definition::minimum_required_bits(event, telemetry);
    return requirements;
}

e_simulation_event_type c_simulation_game_engine_request_boot_player_event_definition::event_type()
{
    return _simulation_event_type_game_engine_request_boot_player;
}

const char *c_simulation_game_engine_request_boot_player_event_definition::event_type_name()
{
    return "game_engine_request_boot_player";
}

s_simulation_event_requirements c_simulation_game_engine_request_boot_player_event_definition::event_requirements(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry)
{
    s_simulation_event_requirements requirements;
    requirements.reference_count = 0;
    requirements.reference_delays_entity_deletion = false;
    requirements.can_be_transmitted = true;
    requirements.minimum_required_bits = 8;
    return requirements;
}

// @address 0x42e650 ?payload_size@c_simulation_game_engine_request_boot_player_event_definition@@UAAJXZ
long c_simulation_game_engine_request_boot_player_event_definition::payload_size() { return 8; }
// @address 0x42d298 ?number_of_entity_references@c_simulation_game_engine_request_boot_player_event_definition@@UAAJXZ
long c_simulation_game_engine_request_boot_player_event_definition::number_of_entity_references() { return 0; }
// @address 0x42d298 ?reference_delays_entity_deletion@c_simulation_game_engine_request_boot_player_event_definition@@UAA_NXZ
bool c_simulation_game_engine_request_boot_player_event_definition::reference_delays_entity_deletion() { return false; }
// @address 0x42d380 ?event_can_be_transmitted@c_simulation_game_engine_request_boot_player_event_definition@@UAA_NPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_game_engine_request_boot_player_event_definition::event_can_be_transmitted(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) { return true; }
// @address 0x42e650 ?minimum_required_bits@c_simulation_game_engine_request_boot_player_event_definition@@UAAJPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_game_engine_request_boot_player_event_definition::minimum_required_bits(
    const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) { return 8; }
// @address 0x5bec98 ?prepare_event_data_for_gameworld@c_simulation_game_engine_request_boot_player_event_definition@@UAAXJPAX@Z
void c_simulation_game_engine_request_boot_player_event_definition::prepare_event_data_for_gameworld(long, void *) {}

extern "C" {
bool c_simulation_game_engine_globals_definition::gameworld_attachment_valid(long gamestate_index)
{
    bool result = false;
    long type = game_engine_globals_get_simulation_entity_type();
    long current_index = -1;
    if (current_game_engine())
        current_index = tls_game_engine_globals()->gamestate_index;
    if (current_index == gamestate_index)
        result = true;
    return result;

}
}

bool c_simulation_game_engine_globals_definition::build_baseline_state_data(
    long creation_data_size,
    const void *creation_data,
    long baseline_state_data_size,
    void *baseline_state_data)
{
    bool result = false;
    if (entity_type() == game_engine_globals_get_simulation_entity_type()) {
        const c_game_engine *engine;
        if (tls_game_engine_globals()->game_engine_index >= 1 && tls_game_engine_globals()->game_engine_index <= 10)
            engine = game_engines[tls_game_engine_globals()->game_engine_index];
        else
            engine = 0;
        engine->build_simulation_baseline(baseline_state_data_size, baseline_state_data);
        result = true;
    }
    return result;
}

bool c_simulation_game_engine_globals_definition::build_updated_state_data(
    const s_simulation_entity *simulation_entity,
    unsigned long *state_update_mask,
    long update_state_data_size,
    void *update_state_data)
{
    bool result = false;
    long type = game_engine_globals_get_simulation_entity_type();
    if (entity_type() == type) {
        const c_game_engine *engine;
        if (tls_game_engine_globals()->game_engine_index >= 1 && tls_game_engine_globals()->game_engine_index <= 10)
            engine = game_engines[tls_game_engine_globals()->game_engine_index];
        else
            engine = 0;
        engine->build_simulation_update(state_update_mask, update_state_data_size, update_state_data);
        result = true;
    }
    return result;
}

bool c_simulation_game_engine_globals_definition::update_game_entity(
    long entity_gamestate_index,
    unsigned long entity_update_mask,
    long update_state_data_size,
    const void *update_state_data)
{
    bool result = false;
    if (gameworld_attachment_valid(entity_gamestate_index))
    {
        long type = game_engine_globals_get_simulation_entity_type();
        if (entity_type() == type)
            result = game_engine_globals_apply_simulation_update(entity_update_mask, update_state_data_size, update_state_data);
    }
    return result;

}

bool c_simulation_game_engine_globals_definition::promote_game_entity_to_authority(long index) {
 bool success=false;
 if(gameworld_attachment_valid(index)) {
  long type=game_engine_globals_get_simulation_entity_type();
  if(entity_type()==type) {
   game_engine_globals_set_gamestate_index(index);
   success=true;
  }
 }
 return success;
}

/* Formats the "player creation" debug line with the object's relevance. */
// @address 0x43b8d0 ?write_creation_description_to_string@c_simulation_game_engine_globals_definition@@UAAXPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@JPAD@Z
void c_simulation_game_engine_globals_definition::write_creation_description_to_string(
    const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry, long buffer_size, char *buffer)
{
  float relevance;

  relevance = 0.0f;
  simulation_calculate_entity_creation_priority(entity, telemetry, &relevance);
  csnzprintf(buffer, buffer_size, "game engine global creation: relevance=%5.3f", relevance);
}

/* WARNING: Function: __savegprlr_27 replaced with injection: __savegprlr_27 */
/* WARNING: Function: __restgprlr_27 replaced with injection: __restgprlr_27 */

bool c_simulation_game_engine_globals_definition::create_game_entity(
    long assigned_gamestate_index,
    long creation_data_size,
    const void *creation_data,
    unsigned long initial_update_mask,
    long state_data_size,
    const void *initial_state_data)
{
    game_engine_globals_get_simulation_entity_type();
    entity_type();
    game_engine_globals_set_gamestate_index(assigned_gamestate_index);
    if (initial_update_mask != 0)
        game_engine_globals_apply_simulation_update(initial_update_mask, state_data_size, initial_state_data);
    return true;

}

extern "C" {
bool c_simulation_game_engine_globals_definition::delete_game_entity(long gamestate_index)
{
    bool result = false;
    if (gameworld_attachment_valid(gamestate_index))
    {
        game_engine_globals_set_gamestate_index(-1);
        result = true;
    }
    return result;

}
}

// @address 0x42d298 ?creation_data_size@c_simulation_game_engine_globals_definition@@UAAJXZ
long c_simulation_game_engine_globals_definition::creation_data_size() { return 0; }
// @address 0x42d380 ?entity_replication_required_for_view_activation@c_simulation_game_engine_globals_definition@@UAA_NPBUs_simulation_entity@@@Z
bool c_simulation_game_engine_globals_definition::entity_replication_required_for_view_activation(
    const s_simulation_entity *) { return true; }
// @address 0x42d298 ?entity_type_is_gameworld_object@c_simulation_game_engine_globals_definition@@UAA_NXZ
bool c_simulation_game_engine_globals_definition::entity_type_is_gameworld_object() { return false; }
// @address 0x42d380 ?entity_can_be_created@c_simulation_game_engine_globals_definition@@UAA_NPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_game_engine_globals_definition::entity_can_be_created(
    const s_simulation_entity *, const s_simulation_view_telemetry_data *) { return true; }
// @address 0x42d298 ?creation_minimum_required_bits@c_simulation_game_engine_globals_definition@@UAAJPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_game_engine_globals_definition::creation_minimum_required_bits(
    const s_simulation_entity *, const s_simulation_view_telemetry_data *) { return 0; }
// @address 0x5bec98 ?entity_creation_encode@c_simulation_game_engine_globals_definition@@UAAXJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@_N@Z
void c_simulation_game_engine_globals_definition::entity_creation_encode(long, const void *,
    const s_simulation_view_telemetry_data *, c_bitstream *, bool) {}
// @address 0x42d380 ?entity_creation_decode@c_simulation_game_engine_globals_definition@@UAA_NJPAXPAVc_bitstream@@_N@Z
bool c_simulation_game_engine_globals_definition::entity_creation_decode(long, void *, c_bitstream *, bool) { return true; }
// @address 0x42d380 ?entity_creation_lossy_compare@c_simulation_game_engine_globals_definition@@UAA_NPAX0J@Z
bool c_simulation_game_engine_globals_definition::entity_creation_lossy_compare(void *, void *, long) { return true; }
// @address 0x5bec98 ?build_creation_data@c_simulation_game_engine_globals_definition@@UAAXJJPAX@Z
void c_simulation_game_engine_globals_definition::build_creation_data(long, long, void *) {}
// @address 0x5bec98 ?rotate_entity_indices@c_simulation_game_engine_globals_definition@@UAAXPBUs_simulation_entity@@@Z
void c_simulation_game_engine_globals_definition::rotate_entity_indices(const s_simulation_entity *) {}
// @address 0x5bec98 ?prepare_creation_data_for_gameworld@c_simulation_game_engine_globals_definition@@UAAXJPAX@Z
void c_simulation_game_engine_globals_definition::prepare_creation_data_for_gameworld(long, void *) {}
// @address 0x5bec98 ?prepare_state_data_for_gameworld@c_simulation_game_engine_globals_definition@@UAAXKJPAX@Z
void c_simulation_game_engine_globals_definition::prepare_state_data_for_gameworld(unsigned long, long, void *) {}

long c_simulation_game_engine_globals_definition::global_update_minimum_required_bits(const s_entity_update_data *entity_update_data)

{
  unsigned int value;
  int minimum_bits;
  
  value = entity_update_data->update_mask;
  minimum_bits = 0x7fffffff;
  if ((value & 1) != 0) {
    minimum_bits = 0x4b;
  }
  if (((value & 2) != 0) && (0x40 < minimum_bits)) {
    minimum_bits = 0x40;
  }
  if (((value & 4) != 0) && (9 < minimum_bits)) {
    minimum_bits = 9;
  }
  if (((value & 8) != 0) && (7 < minimum_bits)) {
    minimum_bits = 7;
  }
  if (((value & 0x10) != 0) && (0xb < minimum_bits)) {
    minimum_bits = 0xb;
  }
  if (((value & 0x20) != 0) && (0x16 < minimum_bits)) {
    return 0x16;
  }
  if (minimum_bits != 0x7fffffff) {
    return minimum_bits;
  }
  return 0;
}

void c_simulation_game_engine_globals_definition::global_update_write_description_to_string(
    const s_entity_update_data *update_data,
    long buffer_size,
    char *description_buffer)

{
  unsigned int value;
  
  value = update_data->update_mask;
  csstrnzcpy(description_buffer, "", 1024);
  if ((value & 1) != 0) {
    csstrnzcat(description_buffer, "team-mapping:", 1024);
  }
  if ((value & 2) != 0) {
    csstrnzcat(description_buffer, "shared-team-lives:", 1024);
  }
  if ((value & 4) != 0) {
    csstrnzcat(description_buffer, "current-state:", 1024);
  }
  if ((value & 8) != 0) {
    csstrnzcat(description_buffer, "game-finished:", 1024);
  }
  if ((value & 0x10) != 0) {
    csstrnzcat(description_buffer, "current-round:", 1024);
  }
  if ((value & 0x20) != 0) {
    csstrnzcat(description_buffer, "round-timer:", 1024);
  }
  return;
}


bool c_simulation_game_engine_globals_definition::global_update_encode(unsigned long update_mask, unsigned long *updated_mask,
    const s_game_engine_state_data *state_data, c_bitstream *packet, long maximum_bits)
{
    c_entity_update_encode_helper helper;
    bool result = false;
    if (helper.make_room_for_update(packet, maximum_bits, 0, 6, update_mask)) {
        if (helper.write_component_header(0, "team-mapping-exists")) {
            packet->write_integer("initial-teams", state_data->initial_teams, 8);
            packet->write_integer("valid-designators", state_data->valid_designators, 9);
            packet->write_integer("valid-teams", state_data->valid_teams, 8);
            packet->write_integer("active-teams", state_data->active_teams, 8);
            packet->write_integer("ever-active-teams", state_data->ever_active_teams, 8);
            for (long team_designator_index = 0; team_designator_index < 9; ++team_designator_index) {
                if (state_data->valid_designators & (1 << team_designator_index))
                    packet->write_integer("team-designator-to-team-index", state_data->team_designator_to_team_index[team_designator_index] + 1, 4);
            }
        }
        helper.finish_component();
        if (helper.write_component_header(1, "shared-team-lives-exists")) {
            for (long team_index = 0; team_index < 8; ++team_index)
                packet->write_integer("shared-team-lives", (unsigned char)state_data->shared_team_lives[team_index], 8);
        }
        helper.finish_component();
        if (helper.write_component_header(2, "current-state-exists"))
            packet->write_integer("current-state", state_data->current_state, 3);
        helper.finish_component();
        if (helper.write_component_header(3, "game-finished-exists"))
            packet->write_bool("game-finished", state_data->game_finished != 0);
        helper.finish_component();
        if (helper.write_component_header(4, "current-round-exists"))
            packet->write_integer("current-round", state_data->current_round, 5);
        helper.finish_component();
        if (helper.write_component_header(5, "round-timer-exists"))
            packet->write_integer("round-timer", state_data->round_timer + 1, 16);
        helper.finish_component();
        helper.finish_update(updated_mask);
        result = true;
    }
    return result;
}

static __forceinline long simulation_game_engine_globals_read_signed_integer(c_bitstream *packet, const char *name, long bits)
{
    unsigned long value = packet->read_integer(name, bits);
    if (bits < 32 && (value & (1 << (bits - 1))) != 0)
    {
        long sign_extension = ~((1i64 << bits) - 1);
        value |= sign_extension;
    }
    return value;
}

bool c_simulation_game_engine_globals_definition::global_update_decode(unsigned long *update_mask,
    s_game_engine_state_data *state, c_bitstream *packet)
{
    unsigned long decoded_mask = 0;
    bool result = true;
    if (packet->read_bool("team-mapping-exists"))
    {
        state->initial_teams = (unsigned short)packet->read_integer("initial-teams", 8);
        state->valid_designators = (unsigned short)packet->read_integer("valid-designators", 9);
        state->valid_teams = (unsigned short)packet->read_integer("valid-teams", 8);
        state->active_teams = (unsigned short)packet->read_integer("active-teams", 8);
        state->ever_active_teams = (unsigned short)packet->read_integer("ever-active-teams", 8);
        for (long designator_index = 0; designator_index < 9; ++designator_index)
        {
            if ((state->valid_designators & (1 << designator_index)) != 0)
            {
                state->team_designator_to_team_index[designator_index] = (short)(packet->read_integer("team-designator-to-team-index", 4) - 1);
                result = result && state->team_designator_to_team_index[designator_index] >= 0 && state->team_designator_to_team_index[designator_index] < 8;
            }
            else
                state->team_designator_to_team_index[designator_index] = -1;
        }
        decoded_mask |= 1;
    }
    if (packet->read_bool("shared-team-lives-exists"))
    {
        for (long team_index = 0; team_index < 8; ++team_index)
            state->shared_team_lives[team_index] = (signed char)simulation_game_engine_globals_read_signed_integer(packet, "shared-team-lives", 8);
        decoded_mask |= 2;
    }
    if (packet->read_bool("current-state-exists"))
    {
        state->current_state = (unsigned char)packet->read_integer("current-state", 3);
        decoded_mask |= 4;
    }
    if (packet->read_bool("game-finished-exists"))
    {
        state->game_finished = packet->read_bool("game-finished");
        decoded_mask |= 8;
    }
    if (packet->read_bool("current-round-exists"))
    {
        state->current_round = (short)packet->read_integer("current-round", 5);
        decoded_mask |= 16;
    }
    if (packet->read_bool("round-timer-exists"))
    {
        state->round_timer = (short)(packet->read_integer("round-timer", 16) - 1);
        decoded_mask |= 32;
    }
    *update_mask |= decoded_mask;
    return result;
}

long c_simulation_game_engine_event_definition::payload_size()
{
    return 40;
}

bool c_simulation_game_engine_event_definition::event_can_be_transmitted(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry)
{
  const s_game_engine_event_data *definition = (const s_game_engine_event_data *)event->get_payload_data();
  bool result = true;
  if (definition->audience_player_index != -1)
    result = result && (telemetry->acknowledged_player_mask & (1 << definition->audience_player_index)) != 0;
  if (definition->cause_player_index != -1)
    result = result && (telemetry->acknowledged_player_mask & (1 << definition->cause_player_index)) != 0;
  if (definition->effect_player_index != -1)
    result = result && (telemetry->acknowledged_player_mask & (1 << definition->effect_player_index)) != 0;
  return result;
}

long c_simulation_game_engine_event_definition::minimum_required_bits(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry)
{
    const s_game_engine_event_data *event_data = (const s_game_engine_event_data *)event->get_payload_data();

    long count_bits;
    if (event_data->data != 0)
        count_bits = (event_data->data != 1 ? 32 : 0) + 1;
    else
        count_bits = 0;

    long bits = 18 + (event_data->audience_player_index != NONE ? 4 : 0);
    bits += (event_data->cause_player_index != NONE ? 4 : 0) + 1;
    bits += (event_data->cause_team_index != NONE ? 4 : 0) + 1;
    bits += (event_data->effect_player_index != NONE ? 4 : 0) + 1;
    bits += (event_data->effect_team_index != NONE ? 4 : 0) + 1;
    bits += count_bits;

    if (event_data->event == 9)
        bits += 4;

    return bits;
}

float c_simulation_game_engine_event_definition::calculate_relevance(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    const s_game_engine_event_data *payload = (const s_game_engine_event_data *)event->get_payload_data();
    float result = real_constant_pool_zero();
    int index = 0;
    int count = telemetry->number_of_players;
    if (count > 0) {
        const s_simulation_view_player_telemetry_data *record = telemetry->players;
        do {
            int entity_reference = record->player_absolute_index;
            if (entity_reference == payload->cause_player_index ||
                entity_reference == payload->effect_player_index) {
                result = real_constant_pool_one();
                break;
            }
            ++record;
            ++index;
        } while (index < count);
    }
    return result;
}

// @address 0x43c230 ?write_description_to_string@c_simulation_game_engine_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_game_engine_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "game engine");
}

void c_simulation_game_engine_event_definition::event_payload_encode(long event_payload_size, const void *payload, c_bitstream *output_packet, bool unknown)
{
  const s_game_engine_event_data *event_payload = (const s_game_engine_event_data *)payload;
  unsigned int value;
  
  ((c_bitstream *)output_packet)->write_integer(0,event_payload->type,4);
  ((c_bitstream *)output_packet)->write_integer(0,event_payload->event,6);
  ((c_bitstream *)output_packet)->write_integer(0,event_payload->identifier,6);
  ((c_bitstream *)output_packet)->write_bool(0,event_payload->audience_player_index == -1);
  if (event_payload->audience_player_index != -1) {
    ((c_bitstream *)output_packet)->write_integer(0,event_payload->audience_player_index,4);
  }
  ((c_bitstream *)output_packet)->write_bool(0,event_payload->cause_player_index == -1);
  if (event_payload->cause_player_index != -1) {
    ((c_bitstream *)output_packet)->write_integer(0,event_payload->cause_player_index,4);
  }
  ((c_bitstream *)output_packet)->write_bool(0,event_payload->cause_team_index == -1);
  if (event_payload->cause_team_index != -1) {
    ((c_bitstream *)output_packet)->write_integer(0,event_payload->cause_team_index,5);
  }
  ((c_bitstream *)output_packet)->write_bool(0,event_payload->effect_player_index == -1);
  if (event_payload->effect_player_index != -1) {
    ((c_bitstream *)output_packet)->write_integer(0,event_payload->effect_player_index,4);
  }
  ((c_bitstream *)output_packet)->write_bool(0,event_payload->effect_team_index == -1);
  if (event_payload->effect_team_index != -1) {
    ((c_bitstream *)output_packet)->write_integer(0,event_payload->effect_team_index,5);
  }
  ((c_bitstream *)output_packet)->write_bool(0,event_payload->data == 0);
  if (event_payload->data != 0) {
    ((c_bitstream *)output_packet)->write_bool(0,event_payload->data == 1);
    if (event_payload->data != 1) {
      ((c_bitstream *)output_packet)->write_integer(0,event_payload->data,0x20);
    }
  }
  if (event_payload->type == 9) {
    ((c_bitstream *)output_packet)->write_integer(0,(int)event_payload->data_word + 1,4);
  }
  return;
}

bool c_simulation_game_engine_event_definition::event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool force)
{
    s_game_engine_event_data *event = static_cast<s_game_engine_event_data *>(payload);
    event->type = packet->read_integer("event-type", 4);
    event->event = packet->read_integer("audience", 6);
    event->identifier = packet->read_integer("team", 6);
    event->audience_player_index = -1;
    if (!packet->read_bool("audience-player-none"))
        event->audience_player_index = packet->read_integer("audience-player", 4);
    event->cause_player_index = -1;
    if (!packet->read_bool("cause-player-none"))
        event->cause_player_index = packet->read_integer("cause-player", 4);
    event->cause_team_index = -1;
    if (!packet->read_bool("cause-team-none"))
        event->cause_team_index = packet->read_integer("cause-team", 5);
    event->effect_player_index = -1;
    if (!packet->read_bool("effect-player-none"))
        event->effect_player_index = packet->read_integer("effect-player", 4);
    event->effect_team_index = -1;
    if (!packet->read_bool("effect-team-none"))
        event->effect_team_index = packet->read_integer("effect-team", 5);
    event->data = 0;
    if (!packet->read_bool("value-zero"))
    {
        event->data = 1;
        if (!packet->read_bool("value-one"))
            event->data = packet->read_integer("event-data", 32);
    }
    if (event->type == 9)
        event->data_word = (short)(packet->read_integer("extra", 4) - 1);
    else
        event->data_word = 0;
    return true;
}

bool c_simulation_game_engine_event_definition::apply_game_event(
    long reference_gamestate_count,
    const long *gamestate_indices,
    long payload_size,
    const void *payload)
{
    const s_game_engine_event_data *event_data = (const s_game_engine_event_data *)payload;
    bool result = true;
    s_game_engine_event_data game_event = *event_data;
    if (result && event_data->audience_player_index != -1) {
        void *player = datum_try_and_get_absolute(player_data, event_data->audience_player_index);
        if (player)
            game_event.audience_player_index = event_data->audience_player_index | (*(short *)player << 16);
        else
            result = false;
    }
    if (result && event_data->cause_player_index != -1) {
        void *player = datum_try_and_get_absolute(player_data, event_data->cause_player_index);
        if (player)
            game_event.cause_player_index = event_data->cause_player_index | (*(short *)player << 16);
        else
            result = false;
    }
    if (result && event_data->effect_player_index != -1) {
        void *player = datum_try_and_get_absolute(player_data, event_data->effect_player_index);
        if (player)
            game_event.effect_player_index = event_data->effect_player_index | (*(short *)player << 16);
        else
            result = false;
    }
    if (result)
        game_engine_handle_event((const s_game_engine_event_data *)&game_event, true);
    return result;
}

// @address 0x43c710 ?calculate_relevance@c_simulation_game_engine_request_boot_player_event_definition@@UAAMPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@M@Z
float c_simulation_game_engine_request_boot_player_event_definition::calculate_relevance(
    const c_replication_outgoing_event *event, const s_simulation_view_telemetry_data *telemetry, float relevance)
{
    return 0.8f;
}

// @address 0x43c720 ?write_description_to_string@c_simulation_game_engine_request_boot_player_event_definition@@UAAXPBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@MJPAD@Z
void c_simulation_game_engine_request_boot_player_event_definition::write_description_to_string(const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer)
{
    csnzprintf(buffer, buffer_size, "game engine request boot player");
}

void c_simulation_game_engine_request_boot_player_event_definition::event_payload_encode(
    long payload_size,
    const void *payload,
    c_bitstream *encode_packet,
    bool unknown)
{
  const unsigned int *boot_player_payload = (const unsigned int *)payload;
  unsigned int value;
  
  ((c_bitstream *)encode_packet)->write_integer(0,*boot_player_payload,4);
  ((c_bitstream *)encode_packet)->write_integer(0,boot_player_payload[1],4);
  return;
}

bool c_simulation_game_engine_request_boot_player_event_definition::event_payload_decode(
    long boot_payload_size,
    void *payload,
    c_bitstream *incoming_packet,
    bool unknown)
{
  unsigned int *boot_payload = (unsigned int *)payload;
  unsigned int value;
  
  value = incoming_packet->read_integer(0, 4);
  *boot_payload = value;
  value = incoming_packet->read_integer(0, 4);
  boot_payload[1] = value;
  return 1;
}

bool c_simulation_game_engine_request_boot_player_event_definition::apply_game_event(
    long reference_count, const long *references, long payload_size, const void *payload)
{
    const s_game_engine_request_boot_player_event_data *event_data=(const s_game_engine_request_boot_player_event_data *)payload;
    long booted_index=-1;
    long booting_index=-1;
    bool result=false;
    if(event_data->booted!=-1) {
        player_datum *booted=(player_datum *)datum_try_and_get_absolute(player_data,event_data->booted);
        if(booted) {
            booted_index=(booted->identifier<<16)|event_data->booted;
            if(event_data->booting!=-1) {
                player_datum *booting=(player_datum *)datum_try_and_get_absolute(player_data,event_data->booting);
                if(booting) {
                    booting_index=(booting->identifier<<16)|event_data->booting;
                    game_engine_boot_player(booted_index);
                    result=true;
                }
            }
        }
    }
    return result;
}

extern "C" {
// @address 0x49c5c ??_7c_simulation_game_engine_event_definition@@6B@
// @address 0x7bb528 g_simulation_game_engine_event_definition
c_simulation_game_engine_event_definition g_simulation_game_engine_event_definition;
// @address 0x7bb52c g_simulation_engine_request_boot_player_event_definition
c_simulation_game_engine_request_boot_player_event_definition g_simulation_engine_request_boot_player_event_definition;
}
