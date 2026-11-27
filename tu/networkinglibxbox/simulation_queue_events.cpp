// networkinglibxbox/simulation_queue_events.obj: from the records 0x41c970 0x41ca80 0x41cc28
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include <string.h>
#include "simulation/simulation_globals.h"
#include "simulation/game_interface/simulation_game_entities.h"
#include "simulation/simulation_queue_events.h"
#include "memory/bitstream.h"
#include "networking/simulation/simulation_gamestate_encoding.h"
#include "simulation/entity_definition_types.h"
#include "networking/simulation/simulation_type_collection.h"
#include "networking/simulation/simulation_accessors.h"
#include "simulation/simulation_queue_types.h"
#include "simulation/simulation_world_types.h"


static __forceinline void simulation_entity_references_get_gamestate_indices(const long *references, long count, long *gamestate)
{
    if (count > 0)
        for (long i = 0; i < count; i++)
            gamestate[i] = simulation_entity_get_gamestate_index(references[i]);
}

// 0x41c970: encode a queued event and copy it into the world's simulation queue.
// @address 0x41c970 simulation_queue_event_insert
extern "C" void simulation_queue_event_insert(long type, long count, const long *references, long size, void *payload)
{
    c_simulation_event_definition *definition = simulation_get_type_collection()->get_event_definition((e_simulation_event_type)type);
    long encoded_size = 0;
    bool success = false;
    definition->prepare_event_data_for_gameworld(size, payload);
    long gamestate[2];
    simulation_entity_references_get_gamestate_indices(references, count, gamestate);
    unsigned char buffer[1024];
    success = encode_event_to_buffer(buffer, 1024, &encoded_size, (e_simulation_event_type)type, count, gamestate, payload, size);
    if (success)
    {
        long bytes = encoded_size;
        c_simulation_world *world = simulation_get_world();
        s_simulation_queue_element *element = 0;
        world->simulation_queue_allocate(_simulation_queue_element_type_event, bytes, &element);
        if (element)
        {
            memcpy(element->data, buffer, bytes);
            world->simulation_queue_enqueue(element);
        }
    }
}

// 0x41ca80: write one queued event (type, gamestate references, payload) into buffer; out_buffer_size gets its length.
// @address 0x41ca80 encode_event_to_buffer
extern "C" bool encode_event_to_buffer(unsigned char *buffer, long buffer_size, long *out_buffer_size, e_simulation_event_type event_type,
    long reference_count, long *references, void *payload, long payload_size)
{
    bool success = false;
    c_bitstream packet;
    packet.m_data_size_alignment = 1;
    packet.set_data(buffer, 1024);
    packet.begin_writing();
    packet.write_integer("event-type", event_type, 6);
    packet.write_integer("reference-count", reference_count, 2);
    for (long reference_index = 0; reference_index < reference_count; reference_index++)
    {
        long gamestate_index = references[reference_index];
        packet.write_bool("gamestate-index-exists", gamestate_index != -1);
        if (gamestate_index != -1)
        {
            packet.write_integer("gamestate-index-id", (unsigned short)(gamestate_index >> 16), 16);
            packet.write_integer("gamestate-index-absolute", (unsigned short)gamestate_index, 11);
        }
    }
    c_simulation_event_definition *event_definition = simulation_get_type_collection()->get_event_definition(event_type);
    event_definition->event_payload_encode(payload_size, payload, &packet, false);
    packet.finish_writing(0);
    *out_buffer_size = packet.get_space_used_in_bytes();
    success = !packet.error_occurred();
    return success;
}

bool decode_event_from_buffer(unsigned char *buffer, long buffer_size, s_simulation_queue_decoded_event_data *event)
{
    bool success = false;
    c_bitstream packet;
    packet.m_data_size_alignment = 1;
    packet.set_data(buffer, buffer_size);
    packet.begin_reading();
    event->event_type = (e_simulation_event_type)packet.read_integer("event-type", 6);
    event->reference_count = packet.read_integer("reference-count", 2);
    if (event->event_type >= 0 && event->event_type < k_simulation_event_type_count
        && (unsigned long)event->reference_count <= 2)
    {
        for (long reference_index = 0; reference_index < event->reference_count; reference_index++)
        {
            if (packet.read_bool("gamestate-index-exists"))
                simulation_gamestate_index_decode(&packet, &event->references[reference_index]);
            else
                event->references[reference_index] = -1;
        }
        c_simulation_type_collection *type_collection = simulation_get_type_collection();
        c_simulation_event_definition *event_definition = type_collection->get_event_definition(event->event_type);
        event->payload_size = event_definition->payload_size();
        if ((unsigned long)event->payload_size <= sizeof(event->payload))
        {
            if (event->payload_size > 0)
                event_definition->event_payload_decode(event->payload_size, event->payload, &packet, false);
            success = !packet.error_occurred();
            packet.finish_reading();
        }
    }
    return success;
}
