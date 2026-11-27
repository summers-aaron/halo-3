// networkinglibxbox/network_messages_simulation_synchronous.obj: from the records 0x3f33d0 0x3f3448 0x3f35d8 0x3f3688 0x3f36e0 0x3f3750 0x3f3840 0x3f38d0 0x3f3a30 0x3f3ad0 0x3f3ae0 0x3f3b80 0x3f3c08 0x3f3d70 0x3f3f50 0x3f3f98
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/bitstream.h"
#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"
#include <string.h>
#include "game/game.h"
#include "simulation/simulation.h"
#include "networking/messages/network_messages_simulation_synchronous.h"
#include "simulation/simulation_encoding.h"
#include "game/game_results.h"
#include "simulation/simulation_queue.h"
#include "simulation/simulation_update_types.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/messages/network_message_handler_types.h"
#include "networking/network_globals.h"
#include "main/main_game.h"
#include "game/game_results_packet.h"
#include "networking/messages/network_messages_synchronous_results.h"
// @address 0x95e608 main_game_globals


struct simulation_update;
// @address 0x3ae928 ?simulation_update_compare@@YA_NPBUsimulation_update@@PAU1@@Z

class c_network_message_synchronous_update {
public:
    static bool compare(long, void *, void *);
    static void dispose(long, void *);
};

class c_network_message_synchronous_actions {
public:
    static bool compare(long message_storage_size, void *first_storage, void *second_storage);
    static void dispose(long message_storage_size, void *message_storage);
};

struct s_network_message_simulation_synchronous_entry {
    long flag;
    long index;
    long value;
};

struct s_network_message_simulation_synchronous_payload
{
    unsigned char type;
    long chunk_offset_next_update_compressed_checksum;
    long chunk_size;
    long decompressed_checksum;
    unsigned char data[1];
};

class c_network_message_synchronous_gamestate {
public:
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

// The first eight bytes are the two raw 32-bit words emitted by encode_game_results.
struct s_network_message_synchronous_client_ready
{
    unsigned char header[8];
    unsigned char game_results[1];
};
typedef char synchronous_payload_data_offset[(offsetof(s_network_message_simulation_synchronous_payload, data) == 16) ? 1 : -1];
typedef char synchronous_update_metadata_offset[
    (sizeof(void *) != 4 || ((offsetof(s_network_message_synchronous_update, metadata.async_buffer_position) == 0x14ac))) ? 1 : -1];

static __forceinline void network_messages_simulation_synchronous_clear_words(void *queue)
{
#ifdef HALO3_64BIT
    memset(queue, 0, sizeof(c_simulation_queue));
#else
    long *word = (long *)queue;
    long remaining = 7;
    do
    {
        *word = 0;
        word++;
        remaining--;
    } while (remaining != 0);
#endif
}

// @address 0x3f35d8 ?compare@c_network_message_synchronous_update@@SA_NJPAX0@Z
bool c_network_message_synchronous_update::compare(long, void *p1, void *p2)
{
    bool result = simulation_update_compare((const simulation_update *)p1, (simulation_update *)p2);
    network_messages_simulation_synchronous_clear_words(&((simulation_update *)p1)->bookkeeping);
    network_messages_simulation_synchronous_clear_words(&((simulation_update *)p2)->bookkeeping);
    network_messages_simulation_synchronous_clear_words(&((simulation_update *)p1)->simulation);
    network_messages_simulation_synchronous_clear_words(&((simulation_update *)p2)->simulation);
    return result;
}

// @address 0x3f3688 ?dispose@c_network_message_synchronous_update@@SAXJPAX@Z
void c_network_message_synchronous_update::dispose(long, void *p1)
{
    c_simulation_queue *queue = &((simulation_update *)p1)->bookkeeping;
    if (queue->initialized != 0)
    {
        queue->clear();
        queue->initialized = 0;
    }
    c_simulation_queue *second_queue = &((simulation_update *)p1)->simulation;
    if (second_queue->initialized != 0)
    {
        second_queue->clear();
        second_queue->initialized = 0;
    }
}

// @address 0xa2810 ?player_action_encode@@YAXPAVc_bitstream@@PBUplayer_action@@@Z

// @address 0x3f3840 network_message_synchronous_encode_actions
void network_message_synchronous_encode_actions(c_bitstream *stream, long message_storage_size, const void *message_data)
{
    const s_network_message_synchronous_actions *message = (const s_network_message_synchronous_actions *)message_data;
    stream->write_integer("action-number", message->action_number, 32);
    stream->write_integer("current-update-number", message->update_number + 1, 32);
    stream->write_integer("user-flags", message->action_mask, 4);
    for (long action_index = 0; action_index < 4; action_index++)
    {
        if (((1 << action_index) & message->action_mask) != 0)
            player_action_encode(stream, (const player_action *)&message->actions[action_index]);
    }
    stream->write_integer("action-test-flags", message->checksum, 27);
}

// @address 0x3f38d0 network_message_synchronous_decode_actions
bool network_message_synchronous_decode_actions(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_synchronous_actions *message = (s_network_message_synchronous_actions *)message_storage;
    bool success = true;
    message->action_number = stream->read_integer("action-number", 32);
    message->update_number = stream->read_integer("current-update-number", 32) - 1;
    message->action_mask = stream->read_integer("user-flags", 4);
    for (long action_index = 0; action_index < 4; action_index++)
    {
        if (((1 << action_index) & message->action_mask) != 0)
            success = success && player_action_decode(stream, (player_action *)&message->actions[action_index]);
    }
    message->checksum = stream->read_integer("action-test-flags", 27);
    success = success && !stream->error_occurred();
    success = success && message->action_number >= 0;
    success = success && message->update_number >= -1;
    return success;
}

// @address 0x3f36e0 network_message_synchronous_encode_update_ack
void network_message_synchronous_encode_update_ack(
    c_bitstream *stream, long message_storage_size, const s_network_message_simulation_synchronous_entry *message)
{
    stream->write_integer("ack-flag", message->flag, 1);
    stream->write_integer("ack-index", message->index + 1, 5);
    stream->write_bool("ack-value-valid", message->value != -1);
    if (message->value != -1)
    {
        stream->write_integer("ack-value", message->value, 32);
    }
}

// @address 0x3f3750 network_message_synchronous_decode_update_ack
bool network_message_synchronous_decode_update_ack(c_bitstream *stream, long message_storage_size, s_network_message_simulation_synchronous_entry *message)
{
    message->flag = stream->read_integer("ack-flag", 1);
    message->index = stream->read_integer("ack-index", 5) - 1;
    if (stream->read_bool("ack-value-valid"))
    {
        message->value = stream->read_integer("ack-value", 32);
    }
    else
    {
        message->value = -1;
    }
    bool success = true;
    success = success && !stream->error_occurred();
    success = success && message->flag >= 0 && message->flag < 2;
    return success;
}

// @address 0x3ae060 ?player_action_compare@@YA_NPBUplayer_action@@PAU1@@Z
// @address 0x3f3a30 ?compare@c_network_message_synchronous_actions@@SA_NJPAX0@Z
bool c_network_message_synchronous_actions::compare(long message_storage_size, void *first_storage, void *second_storage)
{
    s_network_message_synchronous_actions *first_message = (s_network_message_synchronous_actions *)first_storage;
    s_network_message_synchronous_actions *second_message = (s_network_message_synchronous_actions *)second_storage;
    bool result = true;
    if (first_message->action_mask == second_message->action_mask) {
        for (long i = 0; i < 4; i++) {
            if (((1 << i) & first_message->action_mask) != 0) {
                result = result && player_action_compare((const player_action *)&first_message->actions[i], (player_action *)&second_message->actions[i]);
            }
        }
        return result;
    }
    return false;
}

// @address 0x5bec98 ?dispose@c_network_message_synchronous_actions@@SAXJPAX@Z
void c_network_message_synchronous_actions::dispose(long message_storage_size, void *message_storage)
{
}

// @address 0x3f3ad0 network_message_synchronous_encode_update_received
void network_message_synchronous_encode_update_received(c_bitstream *stream, long message_storage_size, const long *value)
{
    stream->write_integer("update-number", *value + 1, 32);
}

// @address 0x3f3ae0 network_message_synchronous_decode_update_received
int network_message_synchronous_decode_update_received(c_bitstream *stream, long message_storage_size, long *value)
{
    *value = stream->read_integer("update-number", 32) - 1;
    bool success = true;
    success = success && !stream->error_occurred();
    success = success && *value >= -1;
    return success;
}

// @address 0x3f3b80 network_message_synchronous_encode_gamestate
void network_message_synchronous_encode_gamestate(
    c_bitstream *stream, long message_storage_size, const s_network_message_simulation_synchronous_payload *message)
{
    const unsigned char *data = message->data;
    stream->write_integer("message-type", message->type, 8);
    stream->write_integer("chunk-offset-next-update-compressed-checksum", message->chunk_offset_next_update_compressed_checksum, 32);
    stream->write_integer("chunk-size", message->chunk_size, 13);
    if (message->type == 2)
        stream->write_integer("decompressed-checksum", message->decompressed_checksum, 32);
    if (message->chunk_size > 0)
        stream->write_raw_data("additional-data", data, message->chunk_size << 3);
}

// @address 0x3f3c08 ?decode@c_network_message_synchronous_gamestate@@SA_NPAVc_bitstream@@JPAX@Z
bool c_network_message_synchronous_gamestate::decode(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_simulation_synchronous_payload *message = (s_network_message_simulation_synchronous_payload *)message_storage;
    unsigned char *data = message->data;
    message->type = (unsigned char)stream->read_integer("message-type", 8);
    message->chunk_offset_next_update_compressed_checksum = stream->read_integer("chunk-offset-next-update-compressed-checksum", 32);
    message->chunk_size = stream->read_integer("chunk-size", 13);
    if (message->type == 2)
        message->decompressed_checksum = stream->read_integer("decompressed-checksum", 32);
    bool success = true;
    success = success && !stream->error_occurred();
    success = success && message->chunk_size >= 0 && message->chunk_size <= 0x10000;
    success = success && message_storage_size == message->chunk_size + 16;
    if (success && message->chunk_size > 0)
    {
        stream->read_raw_data("additional-data", data, message->chunk_size << 3);
        success = success && !stream->error_occurred();
    }
    return success;
}

// @address 0xa55f0 ?simulation_update_encode@@YAXPAVc_bitstream@@PBUsimulation_update@@@Z
// @address 0xa5a00 ?simulation_update_decode@@YA_NPAVc_bitstream@@PAUsimulation_update@@@Z
// @address 0x11aac8 ?simulation_destroy_update@@YAXPAUsimulation_update@@@Z

// @address 0x3f33d0 network_message_synchronous_encode_update
void network_message_synchronous_encode_update(c_bitstream *stream, long message_storage_size, s_network_message_synchronous_update *message)
{
    simulation_update_encode(stream, &message->update);
    stream->write_integer("key-flags", message->metadata.flags, 3);
    if ((bool)((message->metadata.flags >> 2) & 1))
    {
        stream->write_integer("key-a", message->metadata.async_buffer_position, 32);
        stream->write_integer("key-b", message->metadata.film_storage_used, 32);
    }
}




// @address 0x3f3448 network_message_synchronous_decode_update
bool network_message_synchronous_decode_update(c_bitstream *stream, long message_storage_size, s_network_message_synchronous_update *message)
{
    bool result = false;
    bool game_ready = false;
    game_globals_storage *game = tls_game_globals();
    if (game != 0 && game->in_progress != 0 && game->initializing == 0 && game->map_active != 0)
        game_ready = true;
    if (game_ready || main_game_globals.reset_in_progress != 0)
    {
        if (!(game_is_cooperative() && game->options.game_mode == 2 && game->active_structure_bsp_mask == 0))
        {
            bool success = simulation_update_decode(stream, &message->update);
            unsigned short key_flags = (unsigned short)stream->read_integer("key-flags", 3);
            message->metadata.flags = key_flags;
            if ((bool)(((key_flags >> 2) & 0x3fff) & 1))
            {
                message->metadata.async_buffer_position = stream->read_integer("key-a", 32);
                message->metadata.film_storage_used = stream->read_integer("key-b", 32);
            }
            bool ok = true;
            ok = ok && !stream->error_occurred();
            ok = ok && success;
            if (success && !ok)
                simulation_destroy_update(&message->update);
            result = ok;
        }
    }
    return result;
}

typedef void (*network_messages_simulation_synchronous_encode_proc)(c_bitstream *, long, const void *);
typedef bool (*network_messages_simulation_synchronous_decode_proc)(c_bitstream *, long, void *);
typedef bool (*network_messages_simulation_synchronous_compare_proc)(long, void *, void *);
typedef void (*network_messages_simulation_synchronous_dispose_proc)(long, void *);

// @address 0x8c4450 g_network_message_type_collection

static __forceinline void network_messages_simulation_synchronous_register_type(
    c_network_message_type_collection *message_collection,
    long type_index,
    const char *name,
    long flags,
    long minimum_size,
    long maximum_size,
    network_messages_simulation_synchronous_encode_proc encode,
    network_messages_simulation_synchronous_decode_proc decode,
    network_messages_simulation_synchronous_compare_proc compare,
    network_messages_simulation_synchronous_dispose_proc dispose)
{
    s_network_message_type_definition *type = &message_collection->m_message_types[type_index];
    type->name = name;
    type->flags = flags;
    type->minimum_size = minimum_size;
    type->maximum_size = maximum_size;
    type->encode = encode;
    type->decode = decode;
    type->compare = compare;
    type->dispose = dispose;
    type->initialized = true;
}

// @address 0x3f3d70 ?network_message_types_register_simulation_synchronous@@YAXPAVc_network_message_type_collection@@@Z
void network_message_types_register_simulation_synchronous(class c_network_message_type_collection * message_collection)
{
    c_network_message_type_collection *volatile *home = &message_collection;
    c_network_message_type_collection *collection = g_network_message_type_collection;
#ifdef HALO3_64BIT
    network_messages_simulation_synchronous_register_type(collection, 38, "synchronous-update", 0,
        sizeof(s_network_message_synchronous_update), sizeof(s_network_message_synchronous_update),
        (network_messages_simulation_synchronous_encode_proc)network_message_synchronous_encode_update,
        (network_messages_simulation_synchronous_decode_proc)network_message_synchronous_decode_update,
        c_network_message_synchronous_update::compare,
        c_network_message_synchronous_update::dispose);
#else
    network_messages_simulation_synchronous_register_type(collection, 38, "synchronous-update", 0, 5300, 5300,
        (network_messages_simulation_synchronous_encode_proc)network_message_synchronous_encode_update,
        (network_messages_simulation_synchronous_decode_proc)network_message_synchronous_decode_update,
        c_network_message_synchronous_update::compare,
        c_network_message_synchronous_update::dispose);
#endif
    network_messages_simulation_synchronous_register_type(collection, 39, "synchronous-playback-control", 0, 12, 12,
        (network_messages_simulation_synchronous_encode_proc)network_message_synchronous_encode_update_ack,
        (network_messages_simulation_synchronous_decode_proc)network_message_synchronous_decode_update_ack,
        0, 0);
    network_messages_simulation_synchronous_register_type(collection, 40, "synchronous-actions", 0, 480, 480,
        network_message_synchronous_encode_actions, network_message_synchronous_decode_actions,
        c_network_message_synchronous_actions::compare,
        c_network_message_synchronous_actions::dispose);
    network_messages_simulation_synchronous_register_type(collection, 41, "synchronous-acknowledge", 0, 4, 4,
        (network_messages_simulation_synchronous_encode_proc)network_message_synchronous_encode_update_received,
        (network_messages_simulation_synchronous_decode_proc)network_message_synchronous_decode_update_received,
        0, 0);
    network_messages_simulation_synchronous_register_type(collection, 42, "synchronous-gamestate", 1, 16, 65536,
        (network_messages_simulation_synchronous_encode_proc)network_message_synchronous_encode_gamestate,
        c_network_message_synchronous_gamestate::decode,
        0, 0);
    network_messages_simulation_synchronous_register_type(collection, 44, "synchronous-client-ready", 2, 0, 0,
        0, 0, 0, 0);
}

// @address 0x3f3f50 network_message_synchronous_encode_game_results
void network_message_synchronous_encode_game_results(c_bitstream *stream, long message_storage_size, const void *message_storage)
{
    const s_network_message_synchronous_client_ready *message = (const s_network_message_synchronous_client_ready *)message_storage;
    stream->write_raw_data("results-header", message->header, 32);
    stream->write_raw_data("results-header", message->header + 4, 32);
    game_results_encode_packet(stream, (const s_game_results_incremental_update *)message->game_results);
}

static __forceinline void network_messages_simulation_synchronous_read_bytes(
    c_bitstream *packet, unsigned char *buffer, long size_in_bits)
{
    unsigned __int64 value = bitstream_read_window_bits(packet, size_in_bits) << (64 - size_in_bits);
    unsigned long bytes = size_in_bits / 8;
    do
    {
        *buffer++ = (unsigned char)(value >> 56);
        value <<= 8;
        --bytes;
    } while (bytes != 0);
}

// @address 0x3f3f98 network_message_synchronous_decode_game_results
bool network_message_synchronous_decode_game_results(c_bitstream *stream, long message_storage_size, void *message_storage)
{
    s_network_message_synchronous_client_ready *message = (s_network_message_synchronous_client_ready *)message_storage;
    bool success = true;
    network_messages_simulation_synchronous_read_bytes(stream, message->header, 32);
    network_messages_simulation_synchronous_read_bytes(stream, message->header + 4, 32);
    success = success && game_results_decode_packet(stream, (s_game_results_incremental_update *)message->game_results);
    return success;
}
