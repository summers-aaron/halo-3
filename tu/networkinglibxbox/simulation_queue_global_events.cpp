// networkinglibxbox/simulation_queue_global_events.obj: from the records 0x3af2c0 0x3af420 0x3af6a8 0x3af800 0x3af9a8 0x3afb60 0x3afcc0 0x3afe30 0x3b01d0 0x3b0658 0x3b07d0
#include "simulation/simulation_queue_global_events_services.h"
#include "simulation/simulation_queue_player_event_data_types.h"
#include "memory/tls_globals.h"
#include "game/game_engine_scoring_globals.h"
#include "main/main_runtime_globals.h"
#include "memory/data_types.h"
#include "game/game_simulation_types.h"
#include "simulation/simulation_globals.h"
#include "simulation/simulation.h"
#include "math/real_math.h"
#include <string.h>
#include "game/game.h"
#include "game/players.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_sandbox.h"
#include "game/game_engine_sandbox_engine.h"
#include "simulation/game_interface/simulation_game_internal.h"
#include "simulation/simulation_encoding.h"
#include "simulation/simulation_players.h"
#include "simulation/simulation_queue_global_events.h"
#include "game/game_engine_notifications.h"
#include "memory/bitstream.h"
#include "game/map_variant.h"
#include "game/player_types.h"
#include "simulation/simulation_queue_player_event_types.h"
#include "memory/data.h"
#include "simulation/simulation_queue_types.h"
#include "simulation/simulation_queue_global_event_types.h"
#include "simulation/simulation_world_types.h"
#include "game/game_globals_storage.h"
#include "simulation/simulation_gamestate_entities.h"
#include "math/real_math_types.h"
#include "networking/simulation/simulation_accessors.h"
#include "main/main_flag_state.h"
#include "game/game_engine_sandbox_dependencies.h"
#include "memory/data_absolute_index.h"
// @compiler xdk6130
// @flags /TP /O2 /Oc
static __forceinline bool simulation_queue_global_events_is_distributed(const game_globals_storage *options)
{
    bool result = false;
    if ((unsigned int)(options->options.simulation_type - 2) <= 1)
        result = true;
    return result;
}
static __forceinline bool simulation_queue_global_events_is_authority(const game_globals_storage *options)
{
    bool result = true;
    if (options->options.simulation_type == 2 || options->options.simulation_type == 4)
        result = false;
    return result;
}
void simulation_queue_game_global_event_insert(e_simulation_queue_game_global_event_type event_type)
{
    if (!game_is_cooperative())
    {
        const game_globals_storage *options = tls_game_globals();
        if (simulation_queue_global_events_is_distributed(options) && !simulation_queue_global_events_is_authority(options))
            return;

        unsigned char buffer[128];
        c_bitstream packet;
        bitstream_initialize(&packet, buffer, sizeof(buffer));
        packet.begin_writing(1, 0, false);
        packet.write_integer("game-global-event-type", event_type, 4);
        packet.finish_writing(0);
        long bit_position = packet.m_current.current_stream_bit_position;
        long window_bits_used = packet.m_current.window_bits_used;
        long data_size = (bit_position + window_bits_used + 7) / 8;
        if (!packet.error_occurred())
        {
            c_simulation_world *world = simulation_get_world();
            s_simulation_queue_element *element = 0;
            world->simulation_queue_allocate(_simulation_queue_element_type_game_global_event, data_size, &element);
            if (element)
            {
                memcpy(element->data, buffer, data_size);
                world->simulation_queue_enqueue(element);
            }
        }
    }
}
static __forceinline bool simulation_queue_global_events_world_is_local(const c_simulation_world *world)
{
    return world->m_world_type != 4 && world->m_world_type != 6 && world->m_world_type != 8;
}
extern "C" void simulation_queue_game_global_event_apply(const s_simulation_queue_element *element)
{
    c_bitstream packet;
    bitstream_construct(&packet, (unsigned char *)element->data, element->data_size);
    packet.begin_reading();
    unsigned long event_type = packet.read_integer("game-global-event-type", 4);
    if (!packet.error_occurred())
    {
        switch (event_type)
        {
        case 0:
            c_simulation_world::claim_authority_gameworld();
            break;
        case 1:
            game_simulation_set(_game_simulation_distributed_server);
            break;
        case 2:
            game_won();
            break;
        case 3:
            main_globals.revert_requested = true;
            break;
        case 4:
            main_globals.save_requested = true;
            main_globals.reset_map_parameter = false;
            break;
        case 5:
            if (!simulation_globals_get()->shutdown_requested)
            {
                simulation_globals_get()->shutdown_requested = true;
                main_globals.shutdown_requested = true;
            }
            break;
        case 6:
            if (!game_is_cooperative())
            {
                c_simulation_world *world = simulation_get_world();
                if (!simulation_queue_global_events_world_is_local(world))
                    world->send_player_acknowledgements(true);
            }
            simulation_globals_get()->reset_in_progress = false;
            break;
        case 7:
            simulation_globals_get()->reset_in_progress = true;
            break;
        case 8:
            if (tls_game_globals()->options.game_mode == 1 && game_campaign_is_multiplayer())
                players_update_for_checkpoint();
            break;
        }
    }
}
// @address 0x3af6a8 simulation_queue_game_simulation_insert
void simulation_queue_game_simulation_insert(void)
{
    if (!game_is_cooperative())
    {
        const game_globals_storage *options = tls_game_globals();
        if (simulation_queue_global_events_is_distributed(options) && !simulation_queue_global_events_is_authority(options))
            return;

        unsigned char buffer[128];
        c_bitstream packet;
        bitstream_initialize(&packet, buffer, sizeof(buffer));
        packet.begin_writing(1, 0, false);
        packet.write_integer("game-simulation", 0, 1);
        packet.finish_writing(0);
        long bit_position = packet.m_current.current_stream_bit_position;
        long window_bits_used = packet.m_current.window_bits_used;
        long data_size = (bit_position + window_bits_used + 7) / 8;
        if (!packet.error_occurred())
        {
            c_simulation_world *world = simulation_get_world();
            s_simulation_queue_element *element = 0;
            world->simulation_queue_allocate(_simulation_queue_element_type_game_simulation, data_size, &element);
            if (element)
            {
                memcpy(element->data, buffer, data_size);
                world->simulation_queue_enqueue(element);
            }
        }
    }
}
void simulation_queue_player_event_insert(e_simulation_queue_player_event_type type, long player_index, const s_simulation_queue_player_event_data *data)
{
    if (!game_is_cooperative())
    {
        if (simulation_queue_global_events_is_distributed(tls_game_globals()) && !simulation_queue_global_events_is_authority(tls_game_globals()))
            return;

        unsigned char buffer[128];
        c_bitstream packet;
        bitstream_initialize(&packet, buffer, sizeof(buffer));
        packet.begin_writing(1, 0, false);
        packet.write_integer("player-event-type", type, 2);
        packet.write_integer("player-index", (unsigned short)player_index, 4);
        switch (type)
        {
        case _simulation_queue_player_event_active:
            packet.write_bool("active", data->active);
            break;
        case _simulation_queue_player_event_respawn_target:
            packet.write_integer("respawn_target_absolute_player_index", data->respawn_target_absolute_player_index + 1, 5);
            break;
        default:
            // match: 0x3af800: type is 0..1; without the hint the switch keeps a bounds branch and repeats the packet setup
            __assume(0);
        }
        packet.finish_writing(0);
        long window_bits_used = packet.m_current.window_bits_used;
        long bit_position = packet.m_current.current_stream_bit_position;
        long data_size = (bit_position + window_bits_used + 7) / 8;
        if (!packet.error_occurred())
        {
            c_simulation_world *world = simulation_get_world();
            s_simulation_queue_element *element = 0;
            world->simulation_queue_allocate(_simulation_queue_element_type_player_event, data_size, &element);
            if (element)
            {
                memcpy(element->data, buffer, data_size);
                world->simulation_queue_enqueue(element);
            }
        }
    }
}
static __forceinline player_datum *simulation_queue_player_get(const s_data_array *array, long player_index)
{
    return const_cast<player_datum *>(datum_get<player_datum>(array, player_index));
}
void simulation_queue_player_event_apply(const s_simulation_queue_element *element)
{
    c_bitstream packet;
    bitstream_construct(&packet, (unsigned char *)element->data, element->data_size);
    packet.begin_reading();
    unsigned long event_type = packet.read_integer("event-type", 2);
    long absolute_player_index = packet.read_integer("player-index", 4);
    const s_data_array *players = tls_player_data();
    long player_index = datum_absolute_index_to_index(players, absolute_player_index);
    if (!packet.error_occurred())
    {
        if (event_type >= 1)
        {
            long absolute_target_index = packet.read_integer("respawn-target", 5) - 1;
            player_datum *player = simulation_queue_player_get(players, player_index);
            if (absolute_target_index != -1)
                player->respawn_target_player_index =
                    datum_absolute_index_to_index(tls_player_data(), absolute_target_index);
            else
                player->respawn_target_player_index = -1;
            simulation_action_game_engine_player_update((short)player_index, 0x8000);
        }
        else
        {
            bool active = packet.read_bool("active");
            player_datum *player = simulation_queue_player_get(players, player_index);
            if ((long)(player->flags & 1) != active)
            {
                if (active)
                    player->flags |= 1;
                else
                    player->flags &= ~1;
                if (active)
                    game_engine_notify_player_activated(player_index);
                simulation_action_game_engine_player_update((short)player_index, 0x2000);
            }
        }
    }
}
void simulation_queue_player_update_insert(const simulation_player_update *player_update)
{
    if (!game_is_cooperative())
    {
        const game_globals_storage *options = tls_game_globals();
        if (simulation_queue_global_events_is_distributed(options) && !simulation_queue_global_events_is_authority(options))
            return;

        unsigned char buffer[1024];
        c_bitstream packet;
        bitstream_initialize(&packet, buffer, sizeof(buffer));
        packet.begin_writing(1, 0, false);
        simulation_player_update_encode(&packet, player_update);
        packet.finish_writing(0);
        long *stream_bit_position = &packet.m_current.current_stream_bit_position;
        long bit_position = *stream_bit_position;
        long data_size = (bit_position + packet.m_current.window_bits_used + 7) / 8;
        if (!packet.error_occurred())
        {
            c_simulation_world *world = simulation_get_world();
            s_simulation_queue_element *element = 0;
            world->simulation_queue_allocate(_simulation_queue_element_type_player_update, data_size, &element);
            if (element)
            {
                memcpy(element->data, buffer, data_size);
                world->simulation_queue_enqueue(element);
            }
        }
    }
}

void simulation_queue_player_update_apply(const s_simulation_queue_element *element)
{
    simulation_player_update player_update = { 0 };
    c_bitstream packet;
    bitstream_construct(&packet, (unsigned char *)element->data, element->data_size);
    packet.begin_reading();
    simulation_player_update_decode(&packet, &player_update);
    if (!packet.error_occurred())
    {
        switch (player_update.update_type)
        {
        case 0:
            simulation_player_update_apply_left_game(&player_update);
            break;
        case 1:
            simulation_player_update_apply_swap(&player_update);
            break;
        case 2:
            simulation_player_update_apply_remove(&player_update);
            break;
        case 3:
            simulation_player_update_apply_added(&player_update);
            break;
        case 4:
            simulation_player_update_apply_configuration(&player_update);
            break;
        }
    }
    packet.finish_reading();
}
void simulation_queue_gamestates_delete_insert(const s_simulation_queue_gamestate_clear_data *clear_data)
{
    if (!game_is_cooperative())
    {
        const game_globals_storage *options = tls_game_globals();
        if (simulation_queue_global_events_is_distributed(options) && !simulation_queue_global_events_is_authority(options))
            return;

        unsigned char buffer[1024];
        c_bitstream packet;
        bitstream_initialize(&packet, buffer, sizeof(buffer));
        packet.begin_writing(1, 0, false);
        for (long word_index = 0; word_index < 64; word_index++)
            packet.write_integer("clear-data", clear_data->bits[word_index], 32);
        packet.finish_writing(0);
        long *stream_bit_position = &packet.m_current.current_stream_bit_position;
        long bit_position = *stream_bit_position;
        long data_size = (bit_position + packet.m_current.window_bits_used + 7) / 8;
        if (!packet.error_occurred())
        {
            c_simulation_world *world = simulation_get_world();
            s_simulation_queue_element *element = 0;
            world->simulation_queue_allocate(_simulation_queue_element_type_gamestates_delete, data_size, &element);
            if (element)
            {
                memcpy(element->data, buffer, data_size);
                world->simulation_queue_enqueue(element);
            }
        }
    }
}
static __forceinline void simulation_gamestate_entities_clear_by_flags(const s_simulation_queue_gamestate_clear_data *clear_data)
{
    for (long absolute_index = 0; absolute_index < 2048; absolute_index++)
    {
        if ((clear_data->bits[absolute_index >> 5] & (1UL << (absolute_index & 31))) != 0)
        {
            s_simulation_gamestate_entity_datum *datum = &((s_simulation_gamestate_entity_datum *)tls_simulation_gamestate_entity_data()->data)[absolute_index];
            long gamestate_index = absolute_index | (datum->identifier << 16);
            ::datum_delete(tls_simulation_gamestate_entity_data(), gamestate_index);
        }
    }
}
void simulation_queue_gamestates_delete_apply(const s_simulation_queue_element *element)
{
    c_bitstream packet;
    bitstream_construct(&packet, (unsigned char *)element->data, element->data_size);
    s_simulation_queue_gamestate_clear_data clear_data;
    memset(&clear_data, 0, sizeof(clear_data));
    packet.begin_reading();
    unsigned long *bits = clear_data.bits;
    for (long word_index = 0; word_index < 64; word_index++)
        bits[word_index] = packet.read_integer("clear-data", 32);
    if (!packet.error_occurred())
        simulation_gamestate_entities_clear_by_flags(&clear_data);
    packet.finish_reading();
}


extern "C" void simulation_queue_editor_action_insert(game_engine_sandbox_editor_action *action)
{
    if (!game_is_cooperative())
    {
        const game_globals_storage *options = tls_game_globals();
        if (simulation_queue_global_events_is_distributed(options) && !simulation_queue_global_events_is_authority(options))
            return;
        unsigned char buffer[128];
        c_bitstream packet;
        bitstream_initialize(&packet, buffer, sizeof(buffer));
        packet.begin_writing(1, 0, false);
        packet.write_integer("editor-action-type", action->action, 4);
        switch (action->action)
        {
        case 0:
            packet.write_integer("player-index", action->player_index + 1, 5);
            simulation_write_definition_index("object-definition-index", &packet, action->definition_index);
            simulation_write_quantized_position(&packet, &action->position, 16, false, 0);
            break;
        case 6:
            packet.write_integer("player-index", action->player_index + 1, 5);
            simulation_write_definition_index("object-definition-index", &packet, action->definition_index);
            packet.write_integer("maximum-count", action->maximum_count, 8);
            packet.write_integer("minimum-count", action->minimum_count, 8);
            break;
        case 1:
        case 3:
            packet.write_integer("player-index", action->player_index + 1, 5);
            packet.write_integer("owner-index", action->owner_index, 32);
            break;
        case 4:
        {
            packet.write_integer("owner-index", action->owner_index, 32);
            packet.write_quantized_real("editing-progress", action->value, 0.0f, 1.0f, 8, false, false);
            break;
        }
        case 2:
            packet.write_integer("player-index", action->player_index + 1, 5);
            break;
        case 7:
            packet.write_integer("player-index", action->player_index + 1, 5);
            packet.write_integer("subtype", action->subtype, 32);
            break;
        case 5:
            packet.write_integer("player-index", action->player_index + 1, 5);
            packet.write_integer("owner-index", action->owner_index, 16);
            packet.write_integer("game-engine-flags", action->properties.game_engine_flags, 16);
            packet.write_integer("spawn-rate", action->properties.spawn_rate, 8);
            packet.write_integer("spare-clips", action->properties.spare_clips, 8);
            packet.write_integer("team-designator", action->properties.team_designator, 8);
            packet.write_integer("placement-flags", action->properties.placement_flags, 8);
            packet.write_integer("teleporter-channel", action->properties.teleporter_channel, 8);
            packet.write_integer("boundary-shape", action->properties.boundary_shape, 8);
            packet.write_raw_data("boundary", &action->properties.boundary_width, 128);
            break;
        }
        packet.finish_writing(0);
        long window_bits_used = packet.m_current.window_bits_used;
        long bit_position = packet.m_current.current_stream_bit_position;
        long data_size = (bit_position + window_bits_used + 7) / 8;
        if (!packet.error_occurred())
        {
            c_simulation_world *world = simulation_get_world();
            s_simulation_queue_element *element = 0;
            world->simulation_queue_allocate(_simulation_queue_element_type_editor_action, data_size, &element);
            if (element)
            {
                memcpy(element->data, buffer, data_size);
                world->simulation_queue_enqueue(element);
            }
        }
    }
}
static __forceinline float simulation_queue_global_events_read_quantized_real(
    c_bitstream *packet, const char *name, float minimum, float maximum, long size_in_bits, bool exact_midpoint)
{
    long quantized = packet->read_integer(name, size_in_bits);
    return dequantize_real(quantized, minimum, maximum, size_in_bits, exact_midpoint);
}
static __forceinline void simulation_queue_sandbox_event_decode(c_bitstream *packet, game_engine_sandbox_editor_action *action)
{
    memset(action, 0, sizeof(*action));
    action->player_index = -1;
    long encoded_player_index;
    action->action = packet->read_integer("editor-action-type", 4);
    switch (action->action)
    {
    case 0:
        encoded_player_index = packet->read_integer("player-index", 5);
        action->player_index = encoded_player_index - 1;
        action->definition_index = simulation_read_definition_index("object-definition-index", packet);
        simulation_read_quantized_position(packet, &action->position, 16, 0);
        break;
    case 6:
        encoded_player_index = packet->read_integer("player-index", 5);
        action->player_index = encoded_player_index - 1;
        action->definition_index = simulation_read_definition_index("object-definition-index", packet);
        action->maximum_count = (unsigned char)packet->read_integer("maximum-count", 8);
        action->minimum_count = (unsigned char)packet->read_integer("minimum-count", 8);
        break;
    case 1:
    case 3:
        encoded_player_index = packet->read_integer("player-index", 5);
        action->player_index = encoded_player_index - 1;
        action->owner_index = packet->read_integer("owner-index", 32);
        break;
    case 4:
        action->owner_index = packet->read_integer("owner-index", 32);
        action->value = simulation_queue_global_events_read_quantized_real(packet, "editing-progress", 0.0f, 1.0f, 8, false);
        break;
    case 2:
        encoded_player_index = packet->read_integer("player-index", 5);
        action->player_index = encoded_player_index - 1;
        break;
    case 7:
        encoded_player_index = packet->read_integer("player-index", 5);
        action->player_index = encoded_player_index - 1;
        action->subtype = packet->read_integer("subtype", 32);
        break;
    case 5:
        encoded_player_index = packet->read_integer("player-index", 5);
        action->player_index = encoded_player_index - 1;
        action->owner_index = packet->read_integer("owner-index", 16);
        action->properties.game_engine_flags = (unsigned short)packet->read_integer("game-engine-flags", 16);
        action->properties.spawn_rate = (unsigned char)packet->read_integer("spawn-rate", 8);
        action->properties.spare_clips = (unsigned char)packet->read_integer("spare-clips", 8);
        action->properties.team_designator = (e_variant_team_designator)packet->read_integer("team-designator", 8);
        action->properties.placement_flags = (unsigned char)packet->read_integer("placement-flags", 8);
        action->properties.teleporter_channel = (signed char)packet->read_integer("teleporter-channel", 8);
        action->properties.boundary_shape = (e_multiplayer_object_boundary_shape)packet->read_integer("boundary-shape", 8);
        packet->read_raw_data("boundary", &action->properties.boundary_width, 128);
        break;
    }
}
static __forceinline void simulation_queue_sandbox_event_execute(const game_engine_sandbox_editor_action *action)
{
    if (action->player_index != -1)
    {
        long player_index = player_try_and_get_player_index_from_absolute_player_index(action->player_index);
        if (player_index != -1)
        {
            if (((player_datum *)player_data->data)[datum_index_to_absolute_index(player_index)].unit_index != -1)
            {
                switch (action->action)
                {
                case 0:
                    game_engine_sandbox_spawn_object_for_player(action->definition_index, player_index, &action->position);
                    break;
                case 1:
                    if (game_engine_sandbox_is_active())
                    {
                        tls_game_engine_globals()->sandbox.handle_slots[datum_index_to_absolute_index(player_index)] = -1;
                        c_sandbox_engine::player_delete_object(player_index, action->owner_index);
                    }
                    break;
                case 3:
                    game_engine_sandbox_start_editing_object(player_index, action->owner_index);
                    break;
                case 2:
                    game_engine_sandbox_toggle_player_mode(player_index);
                    break;
                case 5:
                    game_engine_sandbox_set_variant_object_properties(player_index, action->owner_index, &action->properties);
                    break;
                case 6:
                    game_engine_sandbox_update_budget_quotas(player_index, action->definition_index, action->maximum_count, action->minimum_count);
                    break;
                case 7:
                    break;
                case 4:
                    break;
                default:
                    // match: 0x3b01d0: action is 0..7; without the hint the switch gains a range check and the 7/4 tail drops below a branch
                    __assume(0);
                }
            }
            if (action->action == 7)
                game_engine_sandbox_end_round_with_value(player_index, action->subtype);
        }
    }
    if (action->action == 4)
        game_engine_sandbox_stop_editing_object(action->owner_index, action->value, 0);
}
void simulation_queue_sandbox_event_apply(const s_simulation_queue_element *element)
{
    game_engine_sandbox_editor_action action;
    c_bitstream packet;
    bitstream_construct(&packet, (unsigned char *)element->data, element->data_size);
    packet.begin_reading();
    simulation_queue_sandbox_event_decode(&packet, &action);
    if (!packet.error_occurred())
        simulation_queue_sandbox_event_execute(&action);
    packet.finish_reading();
}
// @address 0x3afe30 simulation_queue_editor_action_insert
// @address 0x3af2c0 ?simulation_queue_game_global_event_insert@@YAXW4e_simulation_queue_game_global_event_type@@@Z
// @address 0x3af9a8 ?simulation_queue_player_event_apply@@YAXPBUs_simulation_queue_element@@@Z
// @address 0x3afb60 ?simulation_queue_player_update_insert@@YAXPBUsimulation_player_update@@@Z
// @address 0x3afcc0 ?simulation_queue_player_update_apply@@YAXPBUs_simulation_queue_element@@@Z
// @address 0x3b01d0 ?simulation_queue_sandbox_event_apply@@YAXPBUs_simulation_queue_element@@@Z
// @address 0x3b0658 ?simulation_queue_gamestates_delete_insert@@YAXPBUs_simulation_queue_gamestate_clear_data@@@Z
// @address 0x3b07d0 ?simulation_queue_gamestates_delete_apply@@YAXPBUs_simulation_queue_element@@@Z
// @address 0xa2788 ?begin_writing@c_bitstream@@QAAXJJ_N@Z
// @address 0xa2dc0 ?finish_writing@c_bitstream@@QAAXPAJ@Z
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ
// @address 0xa2b80 ?quantize_real@@YAJMMMJ_N0@Z
// @address 0x2210e8 ?reset@c_bitstream@@QAAXJ@Z
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x3ae320 ?simulation_player_update_encode@@YAXPAVc_bitstream@@PBUsimulation_player_update@@@Z
// @address 0x3bfe40 ?simulation_queue_allocate@c_simulation_world@@QAAXW4e_simulation_queue_element_type@@JPAPAUs_simulation_queue_element@@@Z
// @address 0x3c0018 ?simulation_queue_enqueue@c_simulation_world@@QAAXPAUs_simulation_queue_element@@@Z
// @address 0x40eb58 ?simulation_write_definition_index@@YAXPBDPAVc_bitstream@@J@Z
// @address 0x3adbd8 ?simulation_write_quantized_position@@YAXPAVc_bitstream@@PBTreal_point3d@@J_NPBTreal_rectangle3d@@@Z
// @address 0x221228 ?write_bits_internal@c_bitstream@@AAAXPBEJ@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
// @address 0x3af420 simulation_queue_game_global_event_apply
// @address 0x3bee08 ?send_player_acknowledgements@c_simulation_world@@QAAX_N@Z
// @address 0x1b8ca0 ?game_simulation_set@@YAXW4e_game_simulation_type@@@Z
// @address 0x1b9010 ?game_won@@YAXXZ
// @address 0x98818 ?game_campaign_is_multiplayer@@YA_NXZ
// @address 0xa86b0 ?players_update_for_checkpoint@@YAXXZ
// @address 0x3af800 ?simulation_queue_player_event_insert@@YAXW4e_simulation_queue_player_event_type@@JPBUs_simulation_queue_player_event_data@@@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
