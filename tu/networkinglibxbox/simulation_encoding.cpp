// networkinglibxbox/simulation_encoding.obj: from the records 0xa2810 0xa2eb0 0xa3258 0xa55f0 0xa5810 0xa5a00 0xa5e58 0x3adbd8 0x3addb0 0x3ade48 0x3adf70 0x3ae060 0x3ae320 0x3ae408 0x3ae670 0x3ae760 0x3ae7f8 0x3ae928 0x3aea30 0x3aec78 0x3aee70 0x3aef18 0x57e750
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "math/real_math_bounds_storage.h"
#include "simulation/player_action.h"
#include "simulation/simulation_node_space_encoding_types.h"
#include "multithreading/thread_tls.h"
#include "memory/tls_globals.h"
#include "memory/data_types.h"
#include "game/player_absolute_index.h"
#include "tag_files/tag_types.h"
#include "units/unit_control.h"
#include "game/players.h"
#include "scenario/scenario.h"
#include "game/aim_assist.h"
#include "networking/replication/replication_encoding.h"
#include "physics/bsp3d.h"
#include "simulation/simulation_encoding.h"
#include "memory/bitstream.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "networking/player_configuration_encoding_types.h"
#include "simulation/simulation_machine_update_entry_types.h"
#include "cache/cache_files.h"
#include "networking/network_game_definitions.h"
#include "networking/transport/transport_unique_identifier_types.h"
#include "cache/cache_file_tag_resource_runtime_interface.h"
#include "cseries/cseries.h"
#include "math/real_math.h"
#include <stdlib.h>
#include <string.h>
#include "scenario/scenario_access.h"
#include "structures/structure_bsp_tag.h"
#include "simulation/simulation_machine_update_types.h"
#include "simulation/simulation_queue_types.h"
#include "simulation/simulation_queue.h"
#include "simulation/simulation_players.h"
#include "math/real_math_constants.h"
#include "simulation/simulation_encoding_constants.h"
#include "networking/replication/replication_motion.h"
#include "simulation/simulation_update_types.h"
// @address 0x7c5d34 k_null_rectangle3d_storage
struct bsp3d;
class c_bitstream;
union real_point3d;
union real_rectangle3d;
// @address 0x9bc18 ?bsp3d_test_point@@YAJPBUbsp3d@@JPBTreal_point3d@@@Z
// @address 0xa31c8 ?dequantize_real@@YAMJMMJ_N@Z
// @address 0x19e0f0 ?dequantize_real_point3d@@YAXPBTlong_point3d@@PBTreal_rectangle3d@@JPATreal_point3d@@@Z
// @address 0x8a1ecc ?g_active_structure_bsp_mask@@3KA
// @address 0x7cff08 ?global_scenario@@3PAUscenario@@A
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0xa2b80 ?quantize_real@@YAJMMMJ_N0@Z
// @address 0x19e068 ?quantize_real_point3d@@YAXPBTreal_point3d@@PBTreal_rectangle3d@@JPATlong_point3d@@@Z
// @address 0x19e158 ?real_rectangle3d_enclose_rectangle@@YAPATreal_rectangle3d@@PAT1@PBT1@@Z
// @address 0x221228 ?write_bits_internal@c_bitstream@@AAAXPBEJ@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x2212e0 ?read_bits_internal@c_bitstream@@AAAXPAEJ@Z
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x220b48 ?read_vector@c_bitstream@@QAAXPBDPATreal_vector3d@@MMJ@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x221560 ?write_value_internal@c_bitstream@@QAAX_KJ@Z
// @address 0x3adf70 ?simulation_write_quantized_node_space_position@@YAXPAVc_bitstream@@PBTreal_point3d@@W4e_simulation_node_space_encoding_type@@J@Z
// @address 0xa3258 ?aim_assist_targeting_decode@@YA_NPAVc_bitstream@@PAUs_aim_assist_targeting_result@@@Z
// @address 0x3ade48 ?simulation_compare_quantized_positions@@YA_NPBTreal_point3d@@0JPBTreal_rectangle3d@@@Z
// @address 0x3d6b50 ?allocate@c_simulation_queue@@QAAXJPAPAUs_simulation_queue_element@@@Z
// @address 0xa5e58 ?decode@c_simulation_queue@@QAA_NPAVc_bitstream@@@Z
// @address 0xa2eb0 ?player_action_decode@@YA_NPAVc_bitstream@@PAUplayer_action@@@Z
// @address 0x57e750 simulation_decode_checksums
// @address 0x3addb0 ?simulation_read_quantized_position@@YAXPAVc_bitstream@@PATreal_point3d@@JPBTreal_rectangle3d@@@Z
// @address 0x3aee70 ?simulation_encoding_get_world_bounds@@YAXPATreal_rectangle3d@@@Z
// @address 0x3aef18 ?simulation_encoding_point_within_world@@YA_NPBTreal_point3d@@@Z
// @address 0x3c8850 ?player_configuration_client_decode@@YA_NPAVc_bitstream@@PAUs_player_configuration_from_client@@W4e_player_configuration_encoding_type@@@Z
// @address 0x3c8da8 ?player_configuration_host_decode@@YA_NPAVc_bitstream@@PAUs_player_configuration_from_host@@W4e_player_configuration_encoding_type@@@Z
// @address 0x220d10 ?compare_quantized_reals@c_bitstream@@SA_NMMMMJ_N0@Z
// @address 0x220e68 ?compare_vectors@c_bitstream@@SA_NPBTreal_vector3d@@0MMJ@Z
// @address 0x3c8638 player_configuration_client_encode_packet
// @address 0x3c8c80 ?player_configuration_host_encode@@YAXPAVc_bitstream@@PBUs_player_configuration_from_host@@W4e_player_configuration_encoding_type@@@Z

struct scenario;



static const float k_simulation_encoding_lead_vector_minimum_magnitude = 0.01f;
static const float k_simulation_encoding_lead_vector_maximum_magnitude = 15.0f;

// @address 0x3adbd8 ?simulation_write_quantized_position@@YAXPAVc_bitstream@@PBTreal_point3d@@J_NPBTreal_rectangle3d@@@Z
class c_cache_file_tag_resource_allocator;
extern "C" {
// @address 0x7c6060 k_quantized_point_neighbor_offsets
const long_point3d k_quantized_point_neighbor_offsets[14] =
{
    { 0, 0, 1 },
    { 1, 1, 1 },
    { -1, 1, 1 },
    { 1, -1, 1 },
    { -1, -1, 1 },
    { 1, 1, -1 },
    { -1, 1, -1 },
    { 1, -1, -1 },
    { -1, -1, -1 },
    { 1, 0, 0 },
    { 0, 1, 0 },
    { -1, 0, 0 },
    { 0, -1, 0 },
    { 0, 0, -1 },
};
}

static __forceinline bool simulation_encoding_thread_has_resource_access()
{
    return g_geometry_globals.manager->m_resource_cache.m_render_access.m_per_thread_acquired_access_cache[get_current_thread_index()] != 0;
}


// @address 0x270d0 k_simulation_node_space_bounds


static __forceinline void simulation_encoding_read_bits(c_bitstream *stream, unsigned char *data, long size_in_bits)
{
    for (; size_in_bits >= 64; size_in_bits -= 64, data += 8)
        *(unsigned __int64 *)data = bitstream_read_window_bits(stream, 64);
    if (size_in_bits > 0)
    {
        unsigned __int64 value = bitstream_read_window_bits(stream, size_in_bits) << (64 - size_in_bits);
        for (unsigned long byte_count = (unsigned long)size_in_bits / 8; byte_count != 0; --byte_count)
        {
            *data++ = (unsigned char)(value >> 56);
            value <<= 8;
        }
        if (size_in_bits % 8)
            *data = (unsigned char)(value >> 56);
    }
}

// @address 0x3ae670 ?simulation_machine_update_decode@@YA_NPAVc_bitstream@@PAUsimulation_machine_update@@@Z

struct bsp3d;

static __forceinline bool error_occurred(const c_bitstream *stream)
{
    bool result = stream->m_current.current_stream_bit_position > (stream->m_data_size_bytes << 3);
    if (stream->m_error_occurred)
        result = true;
    return result;
}

// @address 0x3ae760 simulation_machine_update_entry_encode

static __forceinline void simulation_encoding_write_unit_vector(c_bitstream *stream, const real_vector3d *vector)
{
    stream->write_integer("vector", bitstream_encode_unit_vector(vector), 19);
}
static __forceinline void simulation_encoding_read_unit_vector(c_bitstream *stream, real_vector3d *vector)
{
    long quantized_vector = stream->read_integer("vector", 19);
    bitstream_decode_unit_vector(quantized_vector, vector);
}

// @address 0x3aea30 ?unit_control_encode@@YA_NPAVc_bitstream@@PBUunit_control_data@@@Z

// @address 0x3aec78 ?unit_control_decode@@YA_NPAVc_bitstream@@PAUunit_control_data@@@Z

// @address 0xa5810 simulation_queue_encode

bool aim_assist_targeting_decode(c_bitstream *packet, s_aim_assist_targeting_result *result)
{
    bool success = true;
    aim_assist_targeting_clear(result);
    if (packet->read_bool("autoaim-exists"))
    {
        result->primary_autoaim_level = packet->read_quantized_real("primary-autoaim-level", 0.0f, 1.0f, 4, false);
        result->secondary_autoaim_level = packet->read_quantized_real("secondary-autoaim-level", 0.0f, 1.0f, 4, false);
        result->flags = packet->read_integer("flags", 3);
        if (result->flags & 4)
            packet->read_vector("lead-vector", &result->lead_vector, k_simulation_encoding_lead_vector_minimum_magnitude,
                k_simulation_encoding_lead_vector_maximum_magnitude, 8);
        result->target_object_index = packet->read_integer("target-object", 32);
        if (result->target_object_index != NONE && packet->read_bool("model-target-exists"))
            result->model_target = packet->read_integer("model-target", 5);
    }
    if (packet->read_bool("target-player-exists"))
    {
        long absolute_player_index = packet->read_integer("target-player", 4);
        result->target_player_index = player_index_from_absolute_player_index(absolute_player_index);
    }
    return success;
}

static __forceinline s_simulation_queue_element *simulation_encoding_queue_get_first_element(const c_simulation_queue *queue)
{
    s_simulation_queue_element *result = 0;
    if (queue->initialized && queue->count > 0)
        result = queue->first;
    return result;
}
static __forceinline s_simulation_queue_element *simulation_encoding_queue_get_next_element(
    const c_simulation_queue *queue, s_simulation_queue_element *element)
{
    s_simulation_queue_element *result = 0;
    if (queue->initialized)
        result = element->next;
    return result;
}

extern "C" void simulation_queue_encode(const c_simulation_queue *queue, c_bitstream *stream)
{
    stream->write_integer("count", queue->count, 12);
    stream->write_integer("size", queue->bytes, 17);
    for (s_simulation_queue_element *element = simulation_encoding_queue_get_first_element(queue); element;
        element = simulation_encoding_queue_get_next_element(queue, element))
    {
        stream->write_integer("type", element->type, 4);
        stream->write_integer("size", element->data_size, 10);
        stream->write_raw_data("data", element->data, element->data_size * 8);
    }
}

void simulation_write_quantized_position(c_bitstream *stream, const real_point3d *point, long bits, bool within_world,
    const real_rectangle3d *source)
{
    long_point3d quantized;
    real_rectangle3d bounds;
    if (source)
        bounds = *source;
    else
        simulation_encoding_get_world_bounds(&bounds);
    quantize_real_point3d(point, &bounds, bits, &quantized);
    if (within_world && simulation_encoding_thread_has_resource_access())
    {
        real_point3d dequantized;
        dequantize_real_point3d(&quantized, &bounds, bits, &dequantized);
        if (!simulation_encoding_point_within_world(&dequantized) && simulation_encoding_point_within_world(point))
        {
            for (unsigned long index = 0; index < 14; index++)
            {
                long_point3d candidate;
                bool valid = true;
                for (long axis = 0; axis < 3; axis++)
                {
                    candidate.n[axis] = quantized.n[axis] + k_quantized_point_neighbor_offsets[index].n[axis];
                    if (candidate.n[axis] >= 0 && candidate.n[axis] < (1 << bits))
                        continue;
                    else
                    {
                        valid = false;
                        break;
                    }
                }
                if (valid)
                {
                    dequantize_real_point3d(&candidate, &bounds, bits, &dequantized);
                    if (simulation_encoding_point_within_world(&dequantized))
                    {
                        quantized = candidate;
                        break;
                    }
                }
            }
        }
    }
    stream->write_point3d("point-quantization", &quantized, bits);
}

void simulation_read_quantized_position(class c_bitstream * stream, union real_point3d * out, long bit_count,
    union real_rectangle3d const * source)
{
    long_point3d values;
    real_rectangle3d block;
    if (source)
        block = *source;
    else
        simulation_encoding_get_world_bounds(&block);
    for (int i = 0; i < 3; i++)
        values.n[i] = stream->read_integer("point-quantization", bit_count);
    dequantize_real_point3d(&values, &block, bit_count, out);
}

bool simulation_compare_quantized_positions(const real_point3d *first, const real_point3d *second, long bits, const real_rectangle3d *source)
{
    // match: 0x3ade48: pins bits to li r5,16 at both quantize calls so bits needs no saved register (0xa0 frame)
    __assume(bits == 16);
    bool result = true;
    long_point3d first_quantized;
    real_rectangle3d bounds;
    long_point3d second_quantized;
    if (!source)
        simulation_encoding_get_world_bounds(&bounds);
    else
        bounds = *source;
    quantize_real_point3d(first, &bounds, bits, &first_quantized);
    quantize_real_point3d(second, &bounds, bits, &second_quantized);
    for (long axis = 0; axis < 3; ++axis)
        result = result && abs(first_quantized.n[axis] - second_quantized.n[axis]) <= 1;
    return result;
}

void simulation_write_quantized_node_space_position(
    c_bitstream *packet, const real_point3d *position, e_simulation_node_space_encoding_type encoding_type, long bits)
{
    long_point3d quantized_position;
    quantize_real_point3d(position, (const real_rectangle3d *)k_simulation_node_space_bounds[encoding_type], bits, &quantized_position);
    packet->write_point3d("node-point-quantization", &quantized_position, bits);
}

struct s_player_configuration_from_client;
struct s_player_configuration_from_host;

extern "C" {
// @address 0x221fc k_real_constant_pool
// @address 0x32938 k_simulation_encoding_signed_unit_pool
// @address 0xc20 k_simulation_encoding_negative_half_pi
}

// @address 0xa2810 ?player_action_encode@@YAXPAVc_bitstream@@PBUplayer_action@@@Z
void player_action_encode(c_bitstream *bitstream, const player_action *action)
{
    bitstream->write_integer("control-flags", action->unit_valid, 1);
    if (action->unit_valid == 1)
        bitstream->write_integer("action-flags", action->unit_flags, 4);
    bitstream->write_integer("turn", action->flags, 32);
    bitstream->write_integer("triggers", action->action_flags, 14);
    bitstream->write_quantized_real("yaw", action->facing.yaw, 0.0f, 6.2831855f, 13, false, false);
    bitstream->write_quantized_real("pitch", action->facing.pitch, -3.1415927f, 3.1415927f, 12, false, false);
    bitstream->write_quantized_real("throttle-x", action->move.x, -1.0f, 1.0f, 5, true, false);
    bitstream->write_quantized_real("throttle-y", action->move.y, -1.0f, 1.0f, 5, true, false);
    bitstream->write_bool("map-editor-rotation-valid", action->map_editor_rotation_valid);
    if (action->map_editor_rotation_valid)
    {
        bitstream->write_quantized_real("map-editor-rotation-yaw", action->extra.x, -1.0f, 1.0f, 5, true, false);
        bitstream->write_quantized_real("map-editor-rotation-pitch", action->extra.y, -1.0f, 1.0f, 5, true, false);
    }
    bitstream->write_integer("other", action->extra_short, 2);
    bitstream->write_quantized_real("trigger", action->primary, 0.0f, 1.0f, 5, false, false);
    bitstream->write_quantized_real("secondary-trigger", action->secondary, 0.0f, 1.0f, 5, false, false);
    bitstream->write_integer("weapon-set-identifier", action->weapons.set_identifier + 1, 5);
    bitstream->write_integer("primary-weapon-index", action->weapons.weapon_indices[0] + 1, 3);
    bitstream->write_integer("secondary-weapon-index", action->weapons.weapon_indices[1] + 1, 3);
    bitstream->write_integer("grenade-index", action->grenade + 1, 3);
    bitstream->write_integer("zoom-level", action->zoom + 1, 2);
    bitstream->write_integer("interaction-type", action->action_context.interaction.type, 4);
    if (action->action_context.interaction.type != 0)
    {
        bitstream->write_integer("interaction-object", action->action_context.interaction.object_index, 32);
        switch (action->action_context.interaction.type)
        {
        case 4:
        case 5:
        case 8:
            bitstream->write_integer("interaction-seat-index", action->action_context.interaction.seat_index, 6);
            break;
        case 1:
        case 9:
            bitstream->write_integer("interaction-pickup-flags", action->action_context.interaction.weapon_flags, 4);
            break;
        }
    }
    bitstream->write_bool("melee-target-exists", action->action_context.melee_target_unit + 1 != 0);
    if (action->action_context.melee_target_unit != -1)
        bitstream->write_integer("melee-target-unit", action->action_context.melee_target_unit, 32);
    aim_assist_targeting_encode(bitstream, &action->aim_assist);
    bitstream->write_bool("velocity-exceeds-motion-tracker-threshold", action->velocity_exceeds_motion_tracker_threshold);
}

// @address 0xa2eb0 ?player_action_decode@@YA_NPAVc_bitstream@@PAUplayer_action@@@Z
bool player_action_decode(c_bitstream *bitstream, player_action *action)
{
    bool success = true;
    player_action_clear(action);
    action->unit_valid = bitstream->read_integer("control-flags", 1);
    if (action->unit_valid == 1)
        action->unit_flags = (unsigned char)bitstream->read_integer("action-flags", 4);
    action->flags = bitstream->read_integer("turn", 32);
    action->action_flags = (unsigned short)bitstream->read_integer("triggers", 14);
    action->facing.yaw = bitstream->read_quantized_real("yaw", 0.0f, 6.2831855f, 13, false);
    action->facing.pitch = bitstream->read_quantized_real("pitch", -3.1415927f, 3.1415927f, 12, false);
    action->move.x = bitstream->read_quantized_real("throttle-x", -1.0f, 1.0f, 5, true);
    action->move.y = bitstream->read_quantized_real("throttle-y", -1.0f, 1.0f, 5, true);
    action->map_editor_rotation_valid = bitstream->read_bool("map-editor-rotation-valid");
    if (action->map_editor_rotation_valid)
    {
        action->extra.x = bitstream->read_quantized_real("map-editor-rotation-yaw", -1.0f, 1.0f, 5, true);
        action->extra.y = bitstream->read_quantized_real("map-editor-rotation-pitch", -1.0f, 1.0f, 5, true);
    }
    action->extra_short = (unsigned short)bitstream->read_integer("other", 2);
    action->primary = bitstream->read_quantized_real("trigger", 0.0f, 1.0f, 5, false);
    action->secondary = bitstream->read_quantized_real("secondary-trigger", 0.0f, 1.0f, 5, false);
    action->weapons.set_identifier = (short)(bitstream->read_integer("weapon-set-identifier", 5) - 1);
    action->weapons.weapon_indices[0] = (signed char)(bitstream->read_integer("primary-weapon-index", 3) - 1);
    action->weapons.weapon_indices[1] = (signed char)(bitstream->read_integer("secondary-weapon-index", 3) - 1);
    action->grenade = (short)(bitstream->read_integer("grenade-index", 3) - 1);
    action->zoom = (short)(bitstream->read_integer("zoom-level", 2) - 1);
    unsigned short *interaction_type = &action->action_context.interaction.type;
    *interaction_type = (unsigned short)bitstream->read_integer("interaction-type", 4);
    if (action->action_context.interaction.type != 0)
    {
        action->action_context.interaction.object_index = bitstream->read_integer("interaction-object", 32);
        switch (action->action_context.interaction.type)
        {
        case 4:
        case 5:
        case 8:
            action->action_context.interaction.seat_index = (short)bitstream->read_integer("interaction-seat-index", 6);
            break;
        case 1:
        case 9:
            action->action_context.interaction.weapon_flags = (unsigned char)bitstream->read_integer("interaction-pickup-flags", 4);
            break;
        }
    }
    else
    {
        action->action_context.interaction.object_index = NONE;
    }
    if (bitstream->read_bool("melee-target-exists"))
        action->action_context.melee_target_unit = bitstream->read_integer("melee-target-unit", 32);
    else
        action->action_context.melee_target_unit = NONE;
    aim_assist_targeting_decode(bitstream, &action->aim_assist);
    action->velocity_exceeds_motion_tracker_threshold = bitstream->read_bool("velocity-exceeds-motion-tracker-threshold");
    success = success && player_is_action_valid(action);
    return success;
}

// @address 0x3ae060 ?player_action_compare@@YA_NPBUplayer_action@@PAU1@@Z
bool player_action_compare(const player_action *action1, player_action *action2)
{
    const s_aim_assist_targeting_result *state1 = &action1->aim_assist;
    s_aim_assist_targeting_result *state2 = &action2->aim_assist;
    bool result = true;
    result = result && c_bitstream::compare_quantized_reals(action1->facing.yaw, action2->facing.yaw,
        0.0f, 6.2831855f, 13, false, true);
    result = result && c_bitstream::compare_quantized_reals(action1->facing.pitch, action2->facing.pitch,
        -3.1415927f, 3.1415927f, 12, false, false);
    result = result && c_bitstream::compare_quantized_reals(action1->move.x, action2->move.x,
        -1.0f, 1.0f, 5, true, false);
    result = result && c_bitstream::compare_quantized_reals(action1->move.y, action2->move.y,
        -1.0f, 1.0f, 5, true, false);
    result = result &&
        c_bitstream::compare_quantized_reals(
            action1->primary, action2->primary,
        0.0f, 1.0f, 5, false, false);
    result = result &&
        c_bitstream::compare_quantized_reals(
            action1->secondary, action2->secondary,
        0.0f, 1.0f, 5, false, false);
    result = result &&
        c_bitstream::compare_quantized_reals(
            state1->primary_autoaim_level, state2->primary_autoaim_level,
        0.0f, 1.0f, 4, false, false);
    result = result &&
        c_bitstream::compare_quantized_reals(
            state1->secondary_autoaim_level, state2->secondary_autoaim_level,
        0.0f, 1.0f, 4, false, false);
    result = result && c_bitstream::compare_vectors(&state1->lead_vector, &state2->lead_vector,
        k_simulation_encoding_lead_vector_minimum_magnitude, k_simulation_encoding_lead_vector_maximum_magnitude, 8);
    if (result)
    {
        action2->facing = action1->facing;
        action2->move = action1->move;
        action2->primary = action1->primary;
        action2->secondary = action1->secondary;
        state2->primary_autoaim_level = state1->primary_autoaim_level;
        state2->secondary_autoaim_level = state1->secondary_autoaim_level;
        state2->lead_vector = state1->lead_vector;
    }
    return result;
}

struct s_network_game_definitions_client_configuration;

// @address 0x3ae320 ?simulation_player_update_encode@@YAXPAVc_bitstream@@PBUsimulation_player_update@@@Z
void simulation_player_update_encode(c_bitstream *packet, const simulation_player_update *player_update)
{
    packet->write_integer("player-index", player_update->player_index, 4);
    packet->write_raw_data("data", player_update->player_identifier.data, 64);
    packet->write_integer("update-type", player_update->update_type, 3);
    if (player_update->update_type == 3)
    {
        packet->write_bool("player-left-game", player_update->player_left_game);
        packet->write_bool("player-joined-in-progress", player_update->player_joined_in_progress);
        if (!player_update->player_left_game)
        {
            packet->write_raw_data("data", player_update->machine_identifier.data, 48);
            packet->write_integer("user-index", player_update->machine_user_index, 2);
            packet->write_integer("controller-index", player_update->machine_controller_index, 2);
        }
    }
    if (player_update->update_type == 3 || player_update->update_type == 4)
    {
        player_configuration_client_encode_packet(packet,
            (const s_network_game_definitions_client_configuration *)&player_update->player_data.client,
            _player_configuration_encoding_full);
        player_configuration_host_encode(
            packet, (const s_player_configuration_from_host *)&player_update->player_data.host, _player_configuration_encoding_full);
    }
    if (player_update->update_type == 1)
    {
        packet->write_integer("swap-player-index", player_update->swap_player_index, 4);
        packet->write_raw_data("data", player_update->swap_player_identifier.data, 64);
    }
}

// @address 0x3ae408 ?simulation_player_update_decode@@YA_NPAVc_bitstream@@PAUsimulation_player_update@@@Z
bool simulation_player_update_decode(c_bitstream *packet, simulation_player_update *player_update)
{
    bool result = true;
    player_update->player_index = packet->read_integer("player-index", 4);
    packet->read_raw_data("data", (void *)(player_update->player_identifier.data), 64);
    player_update->update_type = packet->read_integer("update-type", 3);
    if (player_update->player_index >= 0 && player_update->player_index < 16 &&
        player_update->update_type >= 0 && player_update->update_type < 5)
    {
        if (player_update->update_type == 3)
        {
            player_update->player_left_game = packet->read_bool("player-left-game");
            player_update->player_joined_in_progress = packet->read_bool("player-joined-in-progress");
            if (!player_update->player_left_game)
            {
                unsigned char *buffer = player_update->machine_identifier.data;
                unsigned __int64 value = bitstream_read_window_bits(packet, 48) << 16;
                unsigned long bytes = 6;
                do {
                    *buffer++ = (unsigned char)(value >> 56);
                    value <<= 8;
                    --bytes;
                } while (bytes != 0);
                player_update->machine_user_index = packet->read_integer("user-index", 2);
                player_update->machine_controller_index = packet->read_integer("controller-index", 2);
                result = result && player_update->machine_user_index >= 0 && player_update->machine_user_index < 4;
                result = result && player_update->machine_controller_index >= 0 && player_update->machine_controller_index < 4;
            }
            else
            {
                memset(&player_update->machine_identifier, 0, sizeof(player_update->machine_identifier));
                player_update->machine_user_index = -1;
                player_update->machine_controller_index = -1;
            }
        }
        if (player_update->update_type == 3 || player_update->update_type == 4)
        {
            result = result &&
                player_configuration_client_decode(
                    packet, (s_player_configuration_from_client *)&player_update->player_data.client, _player_configuration_encoding_full);
            result = result &&
                player_configuration_host_decode(
                    packet, (s_player_configuration_from_host *)&player_update->player_data.host, _player_configuration_encoding_full);
        }
        if (player_update->update_type == 1)
        {
            player_update->swap_player_index = packet->read_integer("swap-player-index", 4);
            packet->read_raw_data("data", (void *)(player_update->swap_player_identifier.data), 64);
            result = result && player_update->swap_player_index >= 0 && player_update->swap_player_index < 16;
            result = result && player_update->swap_player_index != player_update->player_index;
        }
    }
    else
        result = false;
    return result;
}

bool simulation_machine_update_decode(c_bitstream *packet, simulation_machine_update *machine_update)
{
    bool success = true;
    machine_update->machine_valid_mask = packet->read_integer("machine-valid-mask", 16);
    for (long machine_index = 0; machine_index < 16; ++machine_index)
    {
        if (machine_update->machine_valid_mask & (1 << machine_index))
            simulation_encoding_read_bits(packet, (unsigned char *)&machine_update->machine_identifiers[machine_index], 48);
        else
            memset(&machine_update->machine_identifiers[machine_index], 0, sizeof(machine_update->machine_identifiers[machine_index]));
    }
    success = success && !packet->error_occurred();
    return success;
}

extern "C" void simulation_machine_update_entry_encode(c_bitstream *stream, const s_simulation_camera_update *update)
{
    stream->write_integer("type", update->type, 2);
    if (update->type != 0)
    {
        stream->write_integer("machine-index", update->machine_index + 1, 5);
        if (update->type == 2)
        {
            simulation_write_quantized_position(stream, &update->position, 16, true, 0);
            stream->write_raw_data("identifier", update->identifier, 96);
            stream->write_bool("valid", update->valid);
        }
    }
}

// @address 0x3ae7f8 ?simulation_camera_update_decode@@YA_NPAVc_bitstream@@PAUs_simulation_camera_update@@@Z
bool simulation_camera_update_decode(c_bitstream *stream, s_simulation_camera_update *update)
{
    bool success = true;
    update->type = stream->read_integer("type", 2);
    if (update->type != 0) {
        update->machine_index = stream->read_integer("machine-index", 5) - 1;
        if (update->type == 2) {
            ::simulation_read_quantized_position(
                stream, &update->position, 16, 0);
            stream->read_raw_data("data", (void *)(update->identifier), 96);
            update->valid = stream->read_bool("valid");
        }
        success = success && (update->machine_index == NONE || VALID_INDEX(update->machine_index, 16));
    }
    success = success && !error_occurred(stream);
    success = success && VALID_INDEX(update->type, 3);
    return success;
}

bool unit_control_encode(c_bitstream *bitstream, const unit_control_data *control)
{
    bitstream->write_integer("animation-seat", control->animation_seat, 32);
    bitstream->write_integer("animation-speed", control->animation_speed, 32);
    bitstream->write_integer("weapon-set-id", control->weapon_set_id, 32);
    bitstream->write_raw_data("weapon-set-indices", control->weapon_set_indices, 16);
    bitstream->write_integer("grenade-index", control->grenade_index, 32);
    bitstream->write_integer("zoom-level", control->zoom_level, 32);
    bitstream->write_integer("control-flags", control->control_flags, 32);
    bitstream->write_quantized_real("primary-trigger", control->primary_trigger, 0.0f, 1.0f, 20, true, false);
    bitstream->write_quantized_real("secondary-trigger", control->secondary_trigger, 0.0f, 1.0f, 20, true, false);
    bitstream->write_quantized_real("throttle-i", control->throttle.i, -1.0f, 1.0f, 20, true, false);
    bitstream->write_quantized_real("throttle-j", control->throttle.j, -1.0f, 1.0f, 20, true, false);
    bitstream->write_quantized_real("throttle-k", control->throttle.k, -1.0f, 1.0f, 20, true, false);
    simulation_encoding_write_unit_vector(bitstream, &control->facing);
    simulation_encoding_write_unit_vector(bitstream, &control->aiming);
    simulation_encoding_write_unit_vector(bitstream, &control->looking);
    simulation_write_quantized_position(bitstream, &control->position, 16, false, 0);
    bitstream->write_raw_data("aim-assist-data", control->aim_assist_data, 288);
    return !bitstream->error_occurred();
}

bool unit_control_decode(c_bitstream *bitstream, unit_control_data *control)
{
    control->animation_seat = bitstream->read_integer("animation-seat", 32);
    control->animation_speed = (short)bitstream->read_integer("animation-speed", 32);
    control->weapon_set_id = (short)bitstream->read_integer("weapon-set-id", 32);
    simulation_encoding_read_bits(bitstream, control->weapon_set_indices, 16);
    control->grenade_index = (short)bitstream->read_integer("grenade-index", 32);
    control->zoom_level = (short)bitstream->read_integer("zoom-level", 32);
    control->control_flags = bitstream->read_integer("control-flags", 32);
    control->primary_trigger = bitstream->read_quantized_real("primary-trigger", 0.0f, 1.0f, 20, true);
    control->secondary_trigger = bitstream->read_quantized_real("secondary-trigger", 0.0f, 1.0f, 20, true);
    control->throttle.i = bitstream->read_quantized_real("throttle-i", -1.0f, 1.0f, 20, true);
    control->throttle.j = bitstream->read_quantized_real("throttle-j", -1.0f, 1.0f, 20, true);
    control->throttle.k = bitstream->read_quantized_real("throttle-k", -1.0f, 1.0f, 20, true);
    simulation_encoding_read_unit_vector(bitstream, &control->facing);
    simulation_encoding_read_unit_vector(bitstream, &control->aiming);
    simulation_encoding_read_unit_vector(bitstream, &control->looking);
    ::simulation_read_quantized_position(bitstream, &control->position, 16, 0);
    bitstream->read_raw_data("aim-assist-data", control->aim_assist_data, 288);
    return !bitstream->error_occurred();
}

void simulation_encoding_get_world_bounds(real_rectangle3d *bounds)
{
    unsigned long active_mask = g_active_structure_bsp_mask;
    *bounds = k_null_rectangle3d_storage;
    for (long bsp_index = 0; bsp_index < 16; bsp_index++) {
        if (active_mask & (1 << bsp_index)) {
            structure_bsp *bsp = global_structure_bsp_get(bsp_index);
            real_rectangle3d_enclose_rectangle(bounds, &bsp->world_bounds);
        }
    }
}

bool simulation_encoding_point_within_world(const real_point3d *point)
{
    unsigned long active_mask = global_structure_bsp_active_mask_get();
    bool result = false;
    for (long bsp_index = 0; bsp_index < 16; bsp_index++) {
        if ((active_mask & (1 << bsp_index)) && bsp3d_test_point(::global_bsp3d_get(bsp_index), 0, point) != NONE) {
            result = true;
            break;
        }
    }
    return result;
}

extern "C" bool simulation_decode_checksums(c_bitstream *stream, long *values)
{
    unsigned long flags = stream->read_integer("checksum-zero-mask", 32);
    values[0] = flags;
    if (!(flags & 1))
        values[1] = stream->read_integer("checksum", 32);
    else
        values[1] = 0;
    flags >>= 1;
    if (!(flags & 1))
        values[2] = stream->read_integer("checksum", 32);
    else
        values[2] = 0;
    flags >>= 1;
    for (long index = 3; index <= 32; ++index)
    {
        if (!(flags & 1))
            values[index] = stream->read_integer("checksum", 32);
        else
            values[index] = 0;
        flags >>= 1;
    }
    return !stream->error_occurred();
}

// @address 0x3ae928 ?simulation_update_compare@@YA_NPBUsimulation_update@@PAU1@@Z
bool simulation_update_compare(const simulation_update *update1, simulation_update *update2)
{
    bool result = true;
    if (update1->player_valid_mask == update2->player_valid_mask)
    {
        for (long index = 0; index < 16; index++)
        {
            if ((update1->player_valid_mask & (1 << index)) && result)
            {
                result = player_action_compare(&update1->actions[index], &update2->actions[index]);

            }
        }
        if (update1->prediction_mask == update2->prediction_mask)
        {
            for (long index = 0; index < 16; index++)
            {
                if ((update1->prediction_mask & (1 << index)) && result)
                {
                    result = player_prediction_compare_identical(&update1->predictions[index], &update2->predictions[index]);

                }
            }
        }
        else
            result = false;
    }
    else
        result = false;
    if (result)
    {
        result = update1->bookkeeping.compare(&update2->bookkeeping);

    }
    if (result)
    {
        result = update1->simulation.compare(&update2->simulation);

    }
    return result;
}

static __forceinline void simulation_encoding_machine_update_encode(c_bitstream *packet, const simulation_machine_update *machine_update)
{
    packet->write_integer("machine-valid-mask", machine_update->machine_valid_mask, 16);
    for (long machine_index = 0; machine_index < 16; ++machine_index)
    {
        if (machine_update->machine_valid_mask & (1 << machine_index))
        {
            const unsigned char *identifier = machine_update->machine_identifiers[machine_index].data;
            unsigned __int64 value = 0;
            for (unsigned long byte_index = 6; byte_index != 0; --byte_index)
                value = (value << 8) | *identifier++;
            packet->write_value_internal(value, 48);
        }
    }
}

// @address 0xa55f0 ?simulation_update_encode@@YAXPAVc_bitstream@@PBUsimulation_update@@@Z
void simulation_update_encode(c_bitstream *packet, const simulation_update *update)
{
    packet->write_integer("update-number", update->update_number, 32);
    packet->write_integer("flags", update->flags, 4);
    packet->write_integer("player-flags", update->player_valid_mask, 16);
    packet->write_integer("action-test-flags", update->action_test_flags, 27);
    for (long index = 0; index < 16; ++index)
    {
        if (update->player_valid_mask & (1 << index))
            player_action_encode(packet, &update->actions[index]);
    }
    packet->write_integer("valid-actor-mask", update->actor_valid_mask, 16);
    for (long index = 0; index < 16; ++index)
    {
        if (update->actor_valid_mask & (1 << index))
        {
            packet->write_integer("actor-unit-index", update->actor_unit_indices[index], 32);
            unit_control_encode(packet, &update->actor_controls[index]);
        }
    }
    packet->write_bool("machine-update-exists", update->machine_update_exists);
    if (update->machine_update_exists)
        simulation_encoding_machine_update_encode(packet, &update->machine_update);
    packet->write_integer("valid-player-prediction-mask", update->prediction_mask, 16);
    for (long index = 0; index < 16; ++index)
    {
        if (update->prediction_mask & (1 << index))
            player_prediction_encode(packet, &update->predictions[index], false);
    }
    packet->write_integer("valid-camera-mask", update->camera_valid_mask, 1);
    if ((long)update->camera_valid_mask & 1)
        simulation_machine_update_entry_encode(packet, &update->camera);
    packet->write_integer("verify-game-time", update->verify_game_time, 32);
    packet->write_integer("verify-random", update->verify_random, 32);
    packet->write_bool("checksums-exist", false);
    simulation_queue_encode(&update->bookkeeping, packet);
    simulation_queue_encode(&update->simulation, packet);
}

static __forceinline bool simulation_encoding_version_valid(short version)
{
    return ((unsigned long)version & 0xfffffff0) == 0 && 1;
}
static __forceinline void simulation_encoding_queue_dispose(c_simulation_queue *queue)
{
    if (queue->initialized)
    {
        queue->clear();
        queue->initialized = false;
    }
}

// @address 0xa5a00 ?simulation_update_decode@@YA_NPAVc_bitstream@@PAUsimulation_update@@@Z
bool simulation_update_decode(c_bitstream *packet, simulation_update *update)
{
    bool success = true;
    long checksums[34];
    update->update_number = packet->read_integer("update-number", 32);
    update->flags = (short)packet->read_integer("flags", 4);
    update->player_valid_mask = packet->read_integer("player-flags", 16);
    update->action_test_flags = packet->read_integer("action-test-flags", 27);
    for (long index = 0; index < 16; ++index)
    {
        if (update->player_valid_mask & (1 << index))
            success = (success && player_action_decode(packet, &update->actions[index])) ? 1 : 0;
    }
    update->actor_valid_mask = packet->read_integer("valid-actor-mask", 16);
    for (long index = 0; index < 16; ++index)
    {
        if (update->actor_valid_mask & (1 << index))
        {
            update->actor_unit_indices[index] = packet->read_integer("actor-unit-index", 32);
            success = (success && unit_control_decode(packet, &update->actor_controls[index])) ? 1 : 0;
        }
    }
    update->machine_update_exists = packet->read_bool("machine-update-exists");
    if (update->machine_update_exists)
        success = (success && simulation_machine_update_decode(packet, &update->machine_update)) ? 1 : 0;
    update->prediction_mask = packet->read_integer("valid-player-prediction-mask", 16);
    for (long index = 0; index < 16; ++index)
    {
        if (update->prediction_mask & (1 << index))
            success = (success && player_prediction_decode(packet, &update->predictions[index], false)) ? 1 : 0;
    }
    update->camera_valid_mask = packet->read_integer("valid-camera-mask", 1);
    if ((long)update->camera_valid_mask & 1)
        success = (success && simulation_camera_update_decode(packet, &update->camera)) ? 1 : 0;
    update->verify_game_time = packet->read_integer("verify-game-time", 32);
    update->verify_random = packet->read_integer("verify-random", 32);
    if (packet->read_bool("checksums-exist"))
        success = (success && simulation_decode_checksums(packet, checksums)) ? 1 : 0;
    bool bookkeeping_success = success && update->bookkeeping.decode(packet);
    bool simulation_success = bookkeeping_success && update->simulation.decode(packet);
    bool stream_success = simulation_success && !error_occurred(packet);
    bool bounds_success = stream_success && !packet->overflowed();
    bool time_success = bounds_success && update->verify_game_time >= 0;
    bool number_success = time_success && update->update_number >= 0;
    bool result = (number_success && simulation_encoding_version_valid(update->flags)) ? 1 : 0;
    if (!result)
    {
        simulation_encoding_queue_dispose(&update->bookkeeping);
        simulation_encoding_queue_dispose(&update->simulation);
    }
    return result;
}

// @address 0xa5e58 ?decode@c_simulation_queue@@QAA_NPAVc_bitstream@@@Z
bool c_simulation_queue::decode(c_bitstream *stream)
{
    bool success = true;
    allocated_elements = 0;
    allocated_bytes = 0;
    count = 0;
    bytes = 0;
    first = 0;
    last = 0;
    initialized = true;
    long queued_count = stream->read_integer("queued-count", 12);
    long queued_size = stream->read_integer("queued-size", 17);
    if (queued_count >= 0 && queued_count <= 4096 && queued_size >= 0 && queued_size <= 0x20000)
    {
        for (long index = 0; index < queued_count; index++)
        {
            long type = stream->read_integer("element-type", 4);
            long size = stream->read_integer("element-size", 10);
            s_simulation_queue_element *element = 0;
            if (type >= 0 && type <= 11 && type != 0)
            {
                if (size > 0)
                {
                    allocate(size, &element);
                    if (element)
                    {
                        element->type = type;
                        stream->read_raw_data("element-data", element->data, element->data_size * 8);
                        enqueue(element);
                    }
                    else
                    {
                        success = false;
                    }
                }
                else
                {
                    success = false;
                }
            }
            else
            {
                success = false;
            }
            if (!success)
                break;
        }
        if (queued_count != count)
            success = false;
        if (queued_size != bytes)
            success = false;
    }
    else
    {
        success = false;
    }
    return success;
}
