// networkinglibxbox/simulation_game_engine_sandbox.obj: from the records 0x426e78 0x426e88 0x426e90 0x426e98 0x426ea0 0x426f48 0x4271c8 0x42d260 0x42d278 0x42d3f8 0x42d610 0x42d740 0x42d900 0x42da98 0x42e180 0x709f18 0x70c7d8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_entity_definition.h"
#include "memory/tls_globals.h"
#include <string.h>
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "cseries/cseries_types.h"
#include "math/real_math.h"
#include "game/map_variant.h"
#include "simulation/game_interface/simulation_game_internal.h"
#include "simulation/simulation_encoding.h"
#include "simulation/game_interface/simulation_game_entities.h"
#include "simulation/game_interface/simulation_game_engine_sandbox.h"
#include "memory/bitstream.h"
#include "networking/replication/replication_entity.h"
#include "simulation/simulation_priority.h"
#include "game/game_engine_scoring_globals.h"
#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "simulation/simulation_gamestate_entities.h"
#include "simulation/simulation_entity_types.h"
// @address 0x1858b8 ?csnzprintf@@YAPADPADIPBDZZ
// @address 0x446800 ?simulation_calculate_entity_update_priority@@YAMPBUs_simulation_entity@@PBUs_entity_update_data@@PAMPAJ@Z
// @address 0x43be48 ?global_update_decode@c_simulation_game_engine_globals_definition@@SA_NPAKPAUs_game_engine_state_data@@PAVc_bitstream@@@Z
// @address 0x426e88 ?state_data_size@c_simulation_sandbox_engine_globals_definition@@UAAJXZ
// @address 0x426e90 ?update_flag_count@c_simulation_sandbox_engine_globals_definition@@UAAJXZ
// @address 0x426e98 ?initial_update_mask@c_simulation_sandbox_engine_globals_definition@@UAAKXZ

#include "game/game_engine_sandbox_state_types.h"
typedef game_engine_sandbox_update_data s_sandbox_engine_state_data;

struct s_sandbox_map_variant_object_state
{
    s_variant_multiplayer_object_properties_definition properties;
    long quota_object_definition_index;
    unsigned char quota_placed_on_map;
    unsigned char quota_maximum_count;
    unsigned char quota_minimum_count;
    real_point3d position;
    real_vector3d forward;
    real_vector3d up;
    short flags;
    long entity_index;
    long gamestate_index;
    long parent_entity_index;
    long parent_gamestate_index;
};

// @address 0x4313c8 ?entity_type@c_simulation_sandbox_engine_globals_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_sandbox_engine_globals_definition::entity_type()
{
    return _simulation_entity_type_sandbox_engine_globals;
}

// @address 0x42d380 ?entity_state_lossy_compare@c_simulation_sandbox_engine_globals_definition@@UAA_NPAX0J@Z
bool c_simulation_sandbox_engine_globals_definition::entity_state_lossy_compare(void *, void *, long)
{
    return true;
}

// @address 0x426e78 ?entity_type_name@c_simulation_sandbox_engine_globals_definition@@UAAPBDXZ
const char *c_simulation_sandbox_engine_globals_definition::entity_type_name()
{
    return "sandbox-engine-globals";
}

long c_simulation_sandbox_engine_globals_definition::state_data_size()
{
    return 136;
}

long c_simulation_sandbox_engine_globals_definition::update_flag_count()
{
    return 11;
}

unsigned long c_simulation_sandbox_engine_globals_definition::initial_update_mask()
{
    return 0x7ff;
}

// @address 0x426ea0 ?update_minimum_required_bits@c_simulation_sandbox_engine_globals_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_sandbox_engine_globals_definition::update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update_data)

{
  int required_bits;
  unsigned int pending_update_mask;
  
  pending_update_mask = (unsigned int)update_data->update_mask;
  required_bits = global_update_minimum_required_bits(update_data);
  required_bits = required_bits + 5;
  if (((pending_update_mask & 0x40) != 0) && (0x1b <= (int)required_bits)) {
    required_bits = 0x1b;
  }
  if (((pending_update_mask & 0x80) != 0) && (0xab <= (int)required_bits)) {
    required_bits = 0xab;
  }
  if (((pending_update_mask & 0x100) != 0) && (0x1b <= (int)required_bits)) {
    required_bits = 0x1b;
  }
  if (((pending_update_mask & 0x200) != 0) && (0x2b <= (int)required_bits)) {
    required_bits = 0x2b;
  }
  if (((pending_update_mask & 0x400) != 0) && (0x2b <= (int)required_bits)) {
    required_bits = 0x2b;
  }
  return required_bits;
}


// @address 0x426f48 ?write_update_description_to_string@c_simulation_sandbox_engine_globals_definition@@UAAXPBUs_simulation_entity@@PBUs_entity_update_data@@JPAD@Z
void c_simulation_sandbox_engine_globals_definition::write_update_description_to_string(
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
  csnzprintf(buffer, buffer_size, "sandbox update: %s relevance=%5.3f: period=%d", name, relevance, period);
}


// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z



struct s_game_engine_state_data;

// @address 0x4271c8 ?entity_update_decode@c_simulation_sandbox_engine_globals_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_sandbox_engine_globals_definition::entity_update_decode(
    bool initial, unsigned long *update_mask, long state_data_size, void *state_data, c_bitstream *stream, bool flag)
{
    s_sandbox_engine_state_data *state = (s_sandbox_engine_state_data *)state_data;
    unsigned long decoded_mask = 0;
    bool success = c_simulation_game_engine_globals_definition::global_update_decode(&decoded_mask, (s_game_engine_state_data *)state, stream);
    unsigned long mask;
    if (success) {
        if (stream->read_bool("player-roles-exist")) {
            state->player_roles = (unsigned short)stream->read_integer("player-roles", 16);
            mask = decoded_mask | 64;
        } else {
            mask = decoded_mask;
        }
        if (stream->read_bool("edited-object-ids-exist")) {
            short i = 0;
            do {
                if (stream->read_bool("edited-object-exists")) {
                    state->edited_object_ids[i] = stream->read_integer("edited-object-id", 10);
                } else {
                    state->edited_object_ids[i] = -1;
                }
                ++i;
            } while (i < 16);
            mask |= 128;
        }
        if (stream->read_bool("total-budget-exists")) {
            state->total_budget = (unsigned short)stream->read_integer("total-budget", 16);
            mask |= 256;
        }
        if (stream->read_bool("variant-object-count-exists")) {
            state->variant_object_count = stream->read_integer("variant-object-count", 32);
            mask |= 512;
        }
        if (stream->read_bool("game-type-exists")) {
            state->game_type = stream->read_integer("game-type", 32);
            mask |= 1024;
        }
    } else {
        mask = decoded_mask;
    }
    bool result = success && mask != 0;
    *update_mask = mask;
    return result;
}

// @address 0x42d3f8 ?update_minimum_required_bits@c_simulation_map_variant_group_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_entity_update_data@@@Z
long c_simulation_map_variant_group_entity_definition::update_minimum_required_bits(
    const s_simulation_entity *entity, const s_entity_update_data *update_data)
{
    int mask = *(const int *)update_data;
    int shift = 0;
    int minimum_bits = 2147483647;
    do {
        int bit = shift;
        if ((unsigned int)(1 << bit & mask) != 0 && minimum_bits >= 108) {
            minimum_bits = 108;
        }
        bit += 2;
        if ((unsigned int)(1 << bit & mask) != 0 && minimum_bits >= 168) {
            minimum_bits = 168;
        }
        --bit;
        if ((unsigned int)(1 << bit & mask) != 0 && minimum_bits >= 76) {
            minimum_bits = 76;
        }
        bit += 2;
        if ((unsigned int)(1 << bit & mask) != 0 && minimum_bits >= 68) {
            minimum_bits = 68;
        }
        shift += 4;
    } while (shift + 1 < 33);
    if (minimum_bits == 2147483647) {
        return 0;
    } else {
        return minimum_bits;
    }
}

// 0x42d610

// @address 0x42d610 ?create_game_entity@c_simulation_map_variant_group_entity_definition@@UAA_NJJPBXKJ0@Z
bool c_simulation_map_variant_group_entity_definition::create_game_entity(
    long handle, long creation_data_size, const void *creation_data,
    unsigned long update_mask, long state_data_size, const void *state_data)
{
    long entity_data = *(const long *)creation_data;
    s_game_engine_globals *globals = tls_game_engine_globals();
    s_simulation_gamestate_entity_datum *data_block = (s_simulation_gamestate_entity_datum *)tls_simulation_gamestate_entity_data()->data;
    data_block[datum_index_to_absolute_index(handle)].object_index = entity_data;
    globals->map_info.m_simulation_indices[entity_data] = handle;
    if (update_mask != 0) {
        update_game_entity(handle, update_mask, state_data_size, state_data);
    }
    return 1;
}


// 0x42d740
static __forceinline void simulation_sandbox_salt_bump(int *handle)
{
    int v = *handle;
    if (v != -1) {
        *handle = ((((int)((unsigned int)v >> 28) + 1) % 16) << 28) | (v & 0x3ff);
    }
}

// @address 0x42d740 ?rotate_entity_indices@c_simulation_map_variant_group_entity_definition@@UAAXPBUs_simulation_entity@@@Z
void c_simulation_map_variant_group_entity_definition::rotate_entity_indices(const s_simulation_entity *entity)
{
    int count = 2;
    s_sandbox_map_variant_object_state *target = (s_sandbox_map_variant_object_state *)((s_simulation_entity *)entity)->state_data;
    do {
        simulation_sandbox_salt_bump((int *)&target[0].entity_index);
        simulation_sandbox_salt_bump((int *)&target[0].parent_entity_index);
        simulation_sandbox_salt_bump((int *)&target[1].entity_index);
        simulation_sandbox_salt_bump((int *)&target[1].parent_entity_index);
        simulation_sandbox_salt_bump((int *)&target[2].entity_index);
        simulation_sandbox_salt_bump((int *)&target[2].parent_entity_index);
        simulation_sandbox_salt_bump((int *)&target[3].entity_index);
        simulation_sandbox_salt_bump((int *)&target[3].parent_entity_index);
        count = count - 1;
        target = target + 4;
    } while ((unsigned int)count != 0);
}

// @address 0x48680 ??_7c_simulation_map_variant_group_entity_definition@@6B@
// @address 0x709f18 ??__Eg_simulation_sandbox_engine_globals_definition@@YAXXZ
// @address 0x70c7d8 ??__Fg_simulation_sandbox_engine_globals_definition@@YAXXZ
// @address 0x7bb470 g_simulation_sandbox_engine_globals_definition
c_simulation_map_variant_group_entity_definition g_simulation_sandbox_engine_globals_definition;

// 0x42da98









static __forceinline void simulation_game_engine_sandbox_gamestate_index_encode(c_bitstream *packet, long gamestate_index)
{
    packet->write_integer("gamestgate-index-id", (unsigned short)(gamestate_index >> 16), 16);
    packet->write_integer("gamestate-index-absolute", (unsigned short)gamestate_index, 11);
}

// @address 0x42da98 ?entity_update_encode@c_simulation_map_variant_group_entity_definition@@UAA_N_NKPAKJPBXPBUs_simulation_view_telemetry_data@@PAVc_bitstream@@J0@Z
bool c_simulation_map_variant_group_entity_definition::entity_update_encode(bool initial_update,
    unsigned long update_mask, unsigned long *updated_mask, long state_data_size,
    const void *state_data, const s_simulation_view_telemetry_data *telemetry,
    c_bitstream *packet, long maximum_bits, bool is_replicated)
{
    const s_sandbox_map_variant_object_state *states = (const s_sandbox_map_variant_object_state *)state_data;
    c_entity_update_encode_helper helper;
    bool result = false;
    if (helper.make_room_for_update(packet, maximum_bits, 0, 32, update_mask))
    {
        for (long object_index = 0; object_index < 8; object_index++)
        {
            const s_sandbox_map_variant_object_state *object_state = &states[object_index];
            long component_index = object_index * 4;
            if (helper.write_component_header(component_index, "map-variant-object-position-exists"))
            {
                bool active = true;
                if (is_replicated)
                {
                    if (object_state->entity_index != -1 && telemetry)
                        active = telemetry->provider->entity_is_active(object_state->entity_index);
                }
                if (active)
                {
                    simulation_write_quantized_position(packet, &object_state->position, 16, false, 0);
                    if (magnitude_squared3d(&object_state->forward) >= k_real_epsilon &&
                        magnitude_squared3d(&object_state->up) >= k_real_epsilon)
                    {
                        packet->write_bool("map-variant-object-axes-exist", true);
                        packet->write_axes("map-variant-object-axes", &object_state->forward, &object_state->up);
                    }
                    else
                    {
                        packet->write_bool("map-variant-object-axes-exist", false);
                    }
                }
                else
                {
                    helper.skip_component();
                }
            }
            helper.finish_component();
            if (helper.write_component_header(component_index + 2, "map-variant-object-properties-exist"))
            {
                bool active = true;
                if (is_replicated)
                {
                    if (object_state->entity_index != -1 && telemetry)
                        active = telemetry->provider->entity_is_active(object_state->entity_index);
                }
                if (active)
                {
                    packet->write_integer("game-engine-flags", object_state->properties.game_engine_flags, 16);
                    packet->write_integer("placement-flags", object_state->properties.placement_flags, 8);
                    packet->write_integer("spawn-rate", object_state->properties.spawn_rate, 8);
                    packet->write_integer("spare-clips", object_state->properties.spare_clips, 8);
                    packet->write_integer("boundary-shape", object_state->properties.boundary_shape, 8);
                    packet->write_integer("team-designator", object_state->properties.team_designator, 8);
                    packet->write_integer("flags", object_state->flags, 16);
                    switch (object_state->properties.boundary_shape)
                    {
                    case 1:
                        packet->write_quantized_real("shape-boundary-width-radius", object_state->properties.boundary_width, 0.0f, 60.0f, 16, false, false);
                        break;
                    case 2:
                        packet->write_quantized_real("shape-boundary-width-radius", object_state->properties.boundary_width, 0.0f, 60.0f, 16, false, false);
                        packet->write_quantized_real("shape-boundary-positive-height", object_state->properties.boundary_positive_height,
                            0.0f, 60.0f, 16, false, false);
                        packet->write_quantized_real("shape-boundary-negative-height", object_state->properties.boundary_negative_height,
                            0.0f, 60.0f, 16, false, false);
                        break;
                    case 3:
                        packet->write_quantized_real("shape-boundary-width-radius", object_state->properties.boundary_width, 0.0f, 60.0f, 16, false, false);
                        packet->write_quantized_real("shape-boundary-box-length", object_state->properties.boundary_box_length, 0.0f, 60.0f, 16, false, false);
                        packet->write_quantized_real("shape-boundary-positive-height", object_state->properties.boundary_positive_height,
                            0.0f, 60.0f, 16, false, false);
                        packet->write_quantized_real("shape-boundary-negative-height", object_state->properties.boundary_negative_height,
                            0.0f, 60.0f, 16, false, false);
                        break;
                    }
                }
                else
                {
                    helper.skip_component();
                }
            }
            helper.finish_component();
            if (helper.write_component_header(component_index + 1, "map-variant-object-indices-exist"))
            {
                if (is_replicated)
                {
                    long entity_index = object_state->entity_index;
                    long parent_entity_index = object_state->parent_entity_index;
                    bool active = entity_index == -1;
                    if (entity_index != -1 && telemetry)
                        active = telemetry->provider->entity_is_active(entity_index);
                    if (active && parent_entity_index != -1)
                        active = telemetry->provider->entity_is_active(parent_entity_index);
                    if (active)
                    {
                        packet->write_bool("entity-exists", entity_index != -1);
                        if (entity_index != -1)
                            replication_entity_index_encode(packet, entity_index);
                        packet->write_bool("parent-entity-exists", parent_entity_index != -1);
                        if (parent_entity_index != -1)
                            replication_entity_index_encode(packet, parent_entity_index);
                        packet->write_integer("flags", object_state->flags, 16);
                    }
                    else
                    {
                        helper.skip_component();
                    }
                }
                else
                {
                    packet->write_bool("entity-exists", object_state->gamestate_index != -1);
                    if (object_state->gamestate_index != -1)
                        simulation_game_engine_sandbox_gamestate_index_encode(packet, object_state->gamestate_index);
                    packet->write_bool("parent-entity-exists", object_state->parent_gamestate_index != -1);
                    if (object_state->parent_gamestate_index != -1)
                        simulation_game_engine_sandbox_gamestate_index_encode(packet, object_state->parent_gamestate_index);
                    packet->write_integer("flags", object_state->flags, 16);
                }
            }
            helper.finish_component();
            if (helper.write_component_header(component_index + 3, "map-variant-object-budget-exists"))
            {
                bool active = true;
                if (is_replicated)
                {
                    if (object_state->entity_index != -1 && telemetry)
                        active = telemetry->provider->entity_is_active(object_state->entity_index);
                }
                if (active)
                {
                    simulation_write_definition_index("object-definition-index", packet, object_state->quota_object_definition_index);
                    packet->write_integer("placed-count", object_state->quota_placed_on_map, 8);
                    packet->write_integer("maximum-count", object_state->quota_maximum_count, 8);
                    packet->write_integer("minimum-count", object_state->quota_minimum_count, 8);
                }
                else
                {
                    helper.skip_component();
                }
            }
            helper.finish_component();
        }
        helper.finish_update(updated_mask);
        result = true;
    }
    return result;
}



// @address 0x42d900 ?build_baseline_state_data@c_simulation_map_variant_group_entity_definition@@UAA_NJPBXJPAX@Z
bool c_simulation_map_variant_group_entity_definition::build_baseline_state_data(
    long creation_size, const void *creation_data, long state_size, void *state_data)
{
    s_sandbox_map_variant_object_state *states = (s_sandbox_map_variant_object_state *)state_data;
    memset(states, 0, sizeof(*states) * 8);
    for (long i = 0; i < 8; ++i)
    {
        s_sandbox_map_variant_object_state &object = states[i];
        object.entity_index = -1;
        object.parent_entity_index = -1;
        s_sandbox_map_variant_object_state *state = states + i;
        state->properties.initialize_to_default();
        state->quota_object_definition_index = -1;
    }
    return true;
}


static __forceinline long simulation_game_engine_sandbox_entity_index_decode(c_bitstream *packet)
{
    long index = packet->read_integer("entity-absolute-index", 10);
    long seed = packet->read_integer("entity-seed", 4);
    return (seed << 28) | index;
}
static __forceinline long simulation_game_engine_sandbox_gamestate_index_decode(c_bitstream *packet)
{
    long identifier = packet->read_integer("gamestate-index-id", 16);
    long index = packet->read_integer("gamestate-index-absolute", 11);
    return (identifier << 16) | index;
}

// @address 0x42e180 ?entity_update_decode@c_simulation_map_variant_group_entity_definition@@UAA_N_NPAKJPAXPAVc_bitstream@@0@Z
bool c_simulation_map_variant_group_entity_definition::entity_update_decode(bool initial_update, unsigned long *updated_mask,
    long state_data_size, void *state_data, c_bitstream *packet, bool is_replicated)
{
    s_sandbox_map_variant_object_state *states = (s_sandbox_map_variant_object_state *)state_data;
    unsigned long mask = 0;
    bool success = true;
    s_sandbox_map_variant_object_state *state = states;
    for (long component_index = 0; component_index + 1 < 33; component_index += 4, ++state)
    {
        if (packet->read_bool("position"))
        {
            ::simulation_read_quantized_position(packet, &state->position, 16, 0);
            if (packet->read_bool("map-variant-object-axes-exist"))
                packet->read_axes("map-variant-object-axes", &state->forward, &state->up);
            mask |= 1 << component_index;
        }
        if (packet->read_bool("properties"))
        {
            state->properties.game_engine_flags.set_unsafe((unsigned short)packet->read_integer("game-engine-flags", 16));
            state->properties.placement_flags.set_unsafe((unsigned char)packet->read_integer("placement-flags", 8));
            state->properties.spawn_rate = (unsigned char)packet->read_integer("spawn-rate", 8);
            state->properties.spare_clips = (unsigned char)packet->read_integer("spare-clips", 8);
            state->properties.boundary_shape = (e_multiplayer_object_boundary_shape)packet->read_integer("boundary-shape", 8);
            state->properties.team_designator = (e_variant_team_designator)packet->read_integer("team-designator", 8);
            state->flags = (short)packet->read_integer("flags", 16);
            switch (state->properties.boundary_shape)
            {
            case 1:
                state->properties.boundary_width = packet->read_quantized_real("shape-boundary-width-radius", 0.0f, 60.0f, 16, false);
                break;
            case 2:
                state->properties.boundary_width = packet->read_quantized_real("shape-boundary-width-radius", 0.0f, 60.0f, 16, false);
                state->properties.boundary_positive_height = packet->read_quantized_real("shape-boundary-positive-height", 0.0f, 60.0f, 16, false);
                state->properties.boundary_negative_height = packet->read_quantized_real("shape-boundary-negative-height", 0.0f, 60.0f, 16, false);
                break;
            case 3:
                state->properties.boundary_width = packet->read_quantized_real("shape-boundary-width-radius", 0.0f, 60.0f, 16, false);
                state->properties.boundary_box_length = packet->read_quantized_real("shape-boundary-box-length", 0.0f, 60.0f, 16, false);
                state->properties.boundary_positive_height = packet->read_quantized_real("shape-boundary-positive-height", 0.0f, 60.0f, 16, false);
                state->properties.boundary_negative_height = packet->read_quantized_real("shape-boundary-negative-height", 0.0f, 60.0f, 16, false);
                break;
            }
            mask |= 1 << (component_index + 2);
        }
        if (packet->read_bool("identity"))
        {
            if (is_replicated)
            {
                if (packet->read_bool("entity-exists"))
                {
                    state->entity_index = simulation_game_engine_sandbox_entity_index_decode(packet);
                    if (!simulation_entity_is_object_entity(state->entity_index))
                        success = false;
                }
                else
                    state->entity_index = -1;
                if (packet->read_bool("parent-entity-exists"))
                {
                    state->parent_entity_index = simulation_game_engine_sandbox_entity_index_decode(packet);
                    if (!simulation_entity_is_object_entity(state->parent_entity_index))
                        success = false;
                }
                else
                    state->parent_entity_index = -1;
            }
            else
            {
                if (packet->read_bool("entity-exists"))
                    state->gamestate_index = simulation_game_engine_sandbox_gamestate_index_decode(packet);
                else
                    state->gamestate_index = -1;
                if (packet->read_bool("parent-entity-exists"))
                    state->parent_gamestate_index = simulation_game_engine_sandbox_gamestate_index_decode(packet);
                else
                    state->parent_gamestate_index = -1;
            }
            state->flags = (short)packet->read_integer("flags", 16);
            mask |= 1 << (component_index + 1);
        }
        if (packet->read_bool("quota"))
        {
            state->quota_object_definition_index = simulation_read_definition_index("object-definition-index", packet);
            state->quota_placed_on_map = (unsigned char)packet->read_integer("placed-count", 8);
            state->quota_maximum_count = (unsigned char)packet->read_integer("maximum-count", 8);
            state->quota_minimum_count = (unsigned char)packet->read_integer("minimum-count", 8);
            mask |= 1 << (component_index + 3);
        }
    }
    bool result = success && mask != 0;
    *updated_mask = mask;
    return result;
}

#if defined(__clang__)
static_assert(sizeof(s_sandbox_map_variant_object_state) == 88, "the retail state is 8 records of 88 bytes");
static_assert(__builtin_offsetof(s_sandbox_map_variant_object_state, entity_index) == 72 &&
    __builtin_offsetof(s_sandbox_map_variant_object_state, gamestate_index) == 76 &&
    __builtin_offsetof(s_sandbox_map_variant_object_state, parent_entity_index) == 80 &&
    __builtin_offsetof(s_sandbox_map_variant_object_state, parent_gamestate_index) == 84, "retail 0x8242d5a8 record offsets");
#endif

// @address 0x42d260 ?entity_type@c_simulation_map_variant_group_entity_definition@@UAA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_simulation_map_variant_group_entity_definition::entity_type()
{
    return _simulation_entity_type_map_variant;
}

// @address 0x42d268 ?entity_type_name@c_simulation_map_variant_group_entity_definition@@UAAPBDXZ
const char *c_simulation_map_variant_group_entity_definition::entity_type_name()
{
    return "map-variant";
}

// @address 0x42d278 ?state_data_size@c_simulation_map_variant_group_entity_definition@@UAAJXZ
long c_simulation_map_variant_group_entity_definition::state_data_size()
{
    return 8 * sizeof(s_sandbox_map_variant_object_state);
}

// @address 0x3fc550 ?creation_data_size@c_simulation_map_variant_group_entity_definition@@UAAJXZ
long c_simulation_map_variant_group_entity_definition::creation_data_size()
{
    return sizeof(long);
}

// @address 0x42d280 ?update_flag_count@c_simulation_map_variant_group_entity_definition@@UAAJXZ
long c_simulation_map_variant_group_entity_definition::update_flag_count()
{
    return 32;
}

// @address 0x42d298 ?entity_replication_required_for_view_activation@c_simulation_map_variant_group_entity_definition@@UAA_NPBUs_simulation_entity@@@Z
bool c_simulation_map_variant_group_entity_definition::entity_replication_required_for_view_activation(const s_simulation_entity *entity)
{
    return false;
}

// @address 0x42d298 ?entity_type_is_gameworld_object@c_simulation_map_variant_group_entity_definition@@UAA_NXZ
bool c_simulation_map_variant_group_entity_definition::entity_type_is_gameworld_object()
{
    return false;
}

// @address 0x42d380 ?entity_can_be_created@c_simulation_map_variant_group_entity_definition@@UAA_NPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
bool c_simulation_map_variant_group_entity_definition::entity_can_be_created(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return true;
}

// @address 0x42d280 ?creation_minimum_required_bits@c_simulation_map_variant_group_entity_definition@@UAAJPBUs_simulation_entity@@PBUs_simulation_view_telemetry_data@@@Z
long c_simulation_map_variant_group_entity_definition::creation_minimum_required_bits(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry)
{
    return 32;
}

// @address 0x42d380 ?entity_state_lossy_compare@c_simulation_map_variant_group_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_map_variant_group_entity_definition::entity_state_lossy_compare(void *first, void *second, long size)
{
    return true;
}

// @address 0x42d380 ?entity_creation_lossy_compare@c_simulation_map_variant_group_entity_definition@@UAA_NPAX0J@Z
bool c_simulation_map_variant_group_entity_definition::entity_creation_lossy_compare(void *first, void *second, long size)
{
    return true;
}

// @address 0x5bec98 ?prepare_creation_data_for_gameworld@c_simulation_map_variant_group_entity_definition@@UAAXJPAX@Z
void c_simulation_map_variant_group_entity_definition::prepare_creation_data_for_gameworld(long size, void *creation_data)
{
}
