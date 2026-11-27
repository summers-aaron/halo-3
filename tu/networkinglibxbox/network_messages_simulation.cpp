// networkinglibxbox/network_messages_simulation.obj: from the records 0x3f3058 0x3f30f0 0x3f3278 0x3f32e8
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/bitstream.h"
// @address 0x3f3058 ?encode@c_network_message_view_establishment@@SAXPAVc_bitstream@@JPBX@Z

#include "networking/messages/network_messages_simulation.h"
void c_network_message_view_establishment::encode(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_view_establishment *message = (const s_network_message_view_establishment *)message_storage;
    stream->write_integer("establishment-mode", message->establishment_mode, 3);
    stream->write_bool("establishment-identifier-exists", message->establishment_identifier != -1);
    if (message->establishment_identifier != -1)
    {
        stream->write_integer("establishment-identifier", message->establishment_identifier, 32);
    }
    stream->write_bool("has-data", message->has_data);
    if (message->has_data)
    {
        stream->write_integer("data-size", message->data_size, 7);
        if (message->data_size > 0)
        {
            stream->write_raw_data("data", message->data, message->data_size * 8);
        }
    }
}

// @address 0x3f30f0 ?decode@c_network_message_view_establishment@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_view_establishment::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_view_establishment *message = (s_network_message_view_establishment *)message_storage;
    bool success = true;
    message->establishment_mode = stream->read_integer("establishment-mode", 3);
    if (stream->read_bool("establishment-identifier-exists"))
        message->establishment_identifier = stream->read_integer("establishment-identifier", 32);
    else
        message->establishment_identifier = -1;
    message->has_data = stream->read_bool("has-data");
    if (message->has_data)
    {
        message->data_size = stream->read_integer("data-size", 7);
        if (message->data_size > 0 && message->data_size <= 60U)
            stream->read_raw_data("data", message->data, message->data_size * 8);
    }
    success = success && !stream->error_occurred();
    success = success && message->establishment_mode >= 0 && message->establishment_mode < 6;
    success = success && (message->establishment_identifier == -1 || message->establishment_identifier >= 0);
    success = success && (!message->has_data || (message->data_size >= 0 && message->data_size <= 60U));
    return success;
}

// @address 0x3f32e8 ?decode@c_network_message_player_acknowledge@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_player_acknowledge::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_player_acknowledge *message = (s_network_message_player_acknowledge *)message_storage;
    bool success = true;
    message->valid_mask = stream->read_integer("player-valid-mask", 16);
    message->in_game_mask = stream->read_integer("player-in-game-mask", 16);
    for (long player_index = 0; player_index < 16; player_index++)
    {
        if (message->valid_mask & (1 << player_index))
            stream->read_raw_data("player-identifier", &message->identifiers[player_index], 64);
    }
    success = success && !stream->error_occurred();
    success = success && (message->in_game_mask & ~message->valid_mask) == 0;
    return success;
}

// @address 0x3f3278 ?encode@c_network_message_player_acknowledge@@SAXPAVc_bitstream@@JPBX@Z
void c_network_message_player_acknowledge::encode(c_bitstream *stream, long size, const void *storage)
{
    const s_network_message_player_acknowledge *message = (const s_network_message_player_acknowledge *)storage;
    stream->write_value_internal(message->valid_mask, 16);
    stream->write_value_internal(message->in_game_mask, 16);
    long player_index = 0;
    const s_player_identifier *identifier = message->identifiers;
    do {
        if (message->valid_mask & (1 << player_index)) {
            stream->write_raw_data("player-identifier", identifier, 64);
        }
        ++player_index;
        ++identifier;
    } while (player_index < 16);
}
