// networkinglibxbox/simulation_queue_entities.obj: from the records 0x41cdd8 0x41cf28 0x41d080 0x41d198 0x41d308 0x41d428 0x41d588 0x41d6a0 0x41d760 0x41d910 0x41daf0 0x41dc70
#include "networking/simulation/simulation_entity_database.h"
#include "simulation/simulation_globals.h"
#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"
#include <string.h>
#include "game/game.h"
#include "simulation/simulation_gamestate_entities.h"
#include "simulation/simulation_queue_entities.h"
#include "memory/bitstream.h"
#include "memory/data_types.h"
#include "networking/simulation/simulation_entity_definition_types.h"
#include "networking/simulation/simulation_type_collection.h"
#include "simulation/simulation_queue_types.h"
#include "simulation/simulation_world_types.h"
#include "objects/object_lifecycle.h"
#include "networking/simulation/simulation_accessors.h"

// match: 0x41d588: the image leaves r4 unset at the slot-28 call; a (long) argument adds mr r4 + r30 save.
class c_simulation_queue_entities_promotion_definition
{
public:
    virtual e_simulation_entity_type entity_type();
    virtual const char *entity_type_name();
    virtual long state_data_size();
    virtual long creation_data_size();
    virtual long update_flag_count();
    virtual unsigned long initial_update_mask();
    virtual bool entity_replication_required_for_view_activation(const s_simulation_entity *entity);
    virtual bool entity_type_is_gameworld_object();
    virtual bool gameworld_attachment_valid(long gamestate_index);
    virtual bool entity_can_be_created(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual long creation_minimum_required_bits(const s_simulation_entity *entity, const s_simulation_view_telemetry_data *telemetry);
    virtual void write_creation_description_to_string(const s_simulation_entity *entity,
        const s_simulation_view_telemetry_data *telemetry, long size, char *buffer);
    virtual long update_minimum_required_bits(const s_simulation_entity *entity, const s_entity_update_data *update);
    virtual void entity_creation_encode(long size, const void *creation_data,
        const s_simulation_view_telemetry_data *telemetry, c_bitstream *packet, bool final_update);
    virtual bool entity_creation_decode(long size, void *creation_data, c_bitstream *packet, bool final_update);
    virtual bool entity_update_encode(bool initial_update, unsigned long update_mask, unsigned long *written_update_mask,
        long size, const void *state, const s_simulation_view_telemetry_data *telemetry,
        c_bitstream *packet, long maximum_bits, bool final_update);
    virtual bool entity_update_decode(bool initial_update, unsigned long *update_mask, long size,
        void *state, c_bitstream *packet, bool final_update);
    virtual bool entity_state_lossy_compare(void *first, void *second, long size);
    virtual bool entity_creation_lossy_compare(void *first, void *second, long size);
    virtual void build_creation_data(long gamestate_index, long size, void *creation_data);
    virtual bool build_baseline_state_data(long creation_size, const void *creation_data, long state_size, void *state_data);
    virtual bool build_updated_state_data(const s_simulation_entity *entity, unsigned long *update_mask, long size, void *state);
    virtual void rotate_entity_indices(const s_simulation_entity *entity);
    virtual void prepare_creation_data_for_gameworld(long size, void *creation_data);
    virtual void prepare_state_data_for_gameworld(unsigned long update_mask, long size, void *state);
    virtual bool create_game_entity(long gamestate_index, long creation_size, const void *creation_data,
        unsigned long update_mask, long state_size, const void *state_data);
    virtual bool update_game_entity(long gamestate_index, unsigned long update_mask, long state_size, const void *state_data);
    virtual bool delete_game_entity(long gamestate_index);
    virtual void promote_game_entity_to_authority();
    virtual void write_update_description_to_string(const s_simulation_entity *entity,
        const s_entity_update_data *update, long size, char *buffer);
};

// @flags /TP /O2 /Oc
// @address 0x41cdd8 ?simulation_queue_entity_creation_allocate@@YA_NPAUs_simulation_queue_entity_data@@KPAPAUs_simulation_queue_element@@PAJ@Z
// @address 0x41cf28 ?simulation_queue_entity_creation_apply@@YAXPBUs_simulation_queue_element@@@Z
// @address 0x41d080 ?simulation_queue_entity_update_allocate@@YA_NPAUs_simulation_queue_entity_data@@JKPAPAUs_simulation_queue_element@@@Z
// @address 0x41d198 ?simulation_queue_entity_deletion_insert@@YAXPAUs_simulation_entity@@_N@Z
// @address 0x41d428 ?simulation_queue_entity_promotion_insert@@YAXPAUs_simulation_entity@@@Z
// @address 0x41d760 ?encode_simulation_queue_creation_to_buffer@@YA_NPAEJJPAUs_simulation_queue_entity_data@@KPAJ@Z
// @address 0x41daf0 ?encode_simulation_queue_update_to_buffer@@YA_NPAEJPAUs_simulation_queue_entity_data@@JKPAJ@Z
// @address 0xac7c0 ?datum_try_and_get@@YAPAXPBUs_data_array@@J@Z
// @address 0xa2dc0 ?finish_writing@c_bitstream@@QAAXPAJ@Z
// @address 0x2210e8 ?reset@c_bitstream@@QAAXJ@Z
// @address 0x3bfe40 ?simulation_queue_allocate@c_simulation_world@@QAAXW4e_simulation_queue_element_type@@JPAPAUs_simulation_queue_element@@@Z
// @address 0x3c0018 ?simulation_queue_enqueue@c_simulation_world@@QAAXPAUs_simulation_queue_element@@@Z

static __forceinline void simulation_queue_entities_gamestate_index_encode(c_bitstream *packet, long gamestate_index)
{
    packet->write_integer("gamestate-index-high-bits", (unsigned short)(gamestate_index >> 16), 16);
    packet->write_integer("gamestate-index-low-bits", (unsigned short)gamestate_index, 11);
}

static __forceinline void simulation_queue_entity_encode_header(c_bitstream *packet, long entity_type, long gamestate_index)
{
    packet->write_integer("entity-type", entity_type, 5);
    simulation_queue_entities_gamestate_index_encode(packet, gamestate_index);
}

static __forceinline bool simulation_queue_entities_is_distributed(const game_globals_storage *options)
{
    bool result = false;
    if ((unsigned long)(options->options.game_simulation - 4) <= 1)
        result = true;
    return result;
}

bool simulation_queue_entity_creation_allocate(s_simulation_queue_entity_data *data, unsigned long initial_update_mask,
    s_simulation_queue_element **out_element, long *out_gamestate_index)
{
    bool success = false;
    s_simulation_queue_element *element = 0;
    long gamestate_index = simulation_gamestate_entity_create();
    if (gamestate_index != -1)
    {
        c_simulation_entity_definition *definition = simulation_get_type_collection()->get_entity_definition(data->entity_type);
        long buffer_size = 0;
        unsigned char buffer[1024];

        definition->prepare_creation_data_for_gameworld(data->creation_data_size, data->creation_data);
        definition->prepare_state_data_for_gameworld(initial_update_mask, data->state_data_size, data->state_data);

        if (encode_simulation_queue_creation_to_buffer(buffer, sizeof(buffer), gamestate_index, data, initial_update_mask, &buffer_size))
        {
            simulation_get_world()->simulation_queue_allocate(_simulation_queue_element_type_entity_creation, buffer_size, &element);
            if (element)
            {
                memcpy(element->data, buffer, buffer_size);
                success = true;
            }
        }
    }

    if (!success)
    {
        if (gamestate_index != -1)
            ::datum_delete(tls_simulation_gamestate_entity_data(), gamestate_index);
        if (element)
            simulation_get_world()->simulation_queue_free(element);
    }
    else
    {
        *out_element = element;
        *out_gamestate_index = gamestate_index;
    }

    return success;
}

static __forceinline long simulation_queue_entity_creation_gamestate_create_at_index(long gamestate_index)
{
    long index = datum_new_at_index(tls_simulation_gamestate_entity_data(), gamestate_index);
    if (index == -1)
    {
        ::datum_delete(tls_simulation_gamestate_entity_data(), gamestate_index);
        index = datum_new_at_index(tls_simulation_gamestate_entity_data(), gamestate_index);
    }
    if (index != -1)
    {
        s_simulation_gamestate_entity_datum *datum = datum_get<s_simulation_gamestate_entity_datum>(
            tls_simulation_gamestate_entity_data(), index);
        datum->object_index = -1;
        datum->entity_index = -1;
        datum->deletion_queued = 0;
    }
    return gamestate_index;
}

void simulation_queue_entity_creation_apply(const s_simulation_queue_element *element)
{
    if (simulation_queue_entities_is_distributed(tls_game_globals()))
    {
        s_simulation_queue_decoded_creation_data decoded;
        memset(&decoded, 0, sizeof(decoded));
        if (decode_simulation_queue_creation_from_buffer(element->data, element->data_size, &decoded))
        {
            c_simulation_entity_definition *definition = simulation_get_type_collection()->get_entity_definition(decoded.entity_type);
            bool create_game_entity = true;
            if (game_is_cooperative())
            {
                long created_gamestate_index = simulation_queue_entity_creation_gamestate_create_at_index(decoded.gamestate_index);
                if (created_gamestate_index != decoded.gamestate_index || created_gamestate_index == -1)
                    create_game_entity = false;
            }
            if (create_game_entity)
                definition->create_game_entity(decoded.gamestate_index, decoded.creation_data_size, decoded.creation_data,
                    decoded.initial_update_mask, decoded.state_data_size, decoded.state_data);
        }
    }
}

bool simulation_queue_entity_update_allocate(
    s_simulation_queue_entity_data *data, long gamestate_index, unsigned long update_mask, s_simulation_queue_element **out_element)
{
    bool success = false;
    s_simulation_queue_element *element = 0;

    if (data->entity_index != -1 && gamestate_index != -1)
    {
        c_simulation_entity_definition *definition = simulation_get_type_collection()->get_entity_definition(data->entity_type);
        long buffer_size = 0;
        definition->prepare_state_data_for_gameworld(update_mask, data->state_data_size, data->state_data);

        unsigned char buffer[1024];
        if (encode_simulation_queue_update_to_buffer(buffer, sizeof(buffer), data, gamestate_index, update_mask, &buffer_size))
        {
            simulation_get_world()->simulation_queue_allocate(_simulation_queue_element_type_entity_update, buffer_size, &element);
            if (element)
            {
                memcpy(element->data, buffer, buffer_size);
                success = true;
            }
        }
    }

    if (!success)
    {
        if (element)
            simulation_get_world()->simulation_queue_free(element);
    }
    else
    {
        *out_element = element;
    }

    return success;
}

void simulation_queue_entity_deletion_insert(s_simulation_entity *entity, bool force_cleanup)
{
    unsigned char buffer[512];
    c_bitstream packet;
    packet.m_data_size_alignment = 1;
    packet.set_data(buffer, sizeof(buffer));
    packet.begin_writing();
    simulation_queue_entity_encode_header(&packet, entity->entity_type, entity->gamestate_index);
    packet.write_bool("force-cleanup", force_cleanup);
    packet.finish_writing(0);
    long data_size = packet.get_space_used_in_bytes();
    if (!packet.error_occurred())
    {
        c_simulation_world *world = simulation_get_world();
        s_simulation_queue_element *element = 0;
        world->simulation_queue_allocate(_simulation_queue_element_type_entity_deletion, data_size, &element);
        if (element)
        {
            memcpy(element->data, buffer, data_size);
            world->simulation_queue_enqueue(element);
        }
    }
}

// @address 0x41d308 ?simulation_queue_entity_deletion_apply@@YAXPBUs_simulation_queue_element@@@Z
void simulation_queue_entity_deletion_apply(struct s_simulation_queue_element const * element)
{
    long entity_type = -1;
    long gamestate_index = -1;
    c_bitstream packet;
    packet.m_data_size_bytes = element->data_size;
    packet.m_data_size_alignment = 1;
    packet.m_data = element->data;
    packet.m_data_max = element->data + element->data_size;
    packet.reset(0);
    packet.reset(3);
    if (simulation_queue_decode_entity_reference(&packet, &entity_type, &gamestate_index))
    {
        c_simulation_entity_definition *entity_definition = simulation_get_type_collection()->get_entity_definition(entity_type);
        bool force_cleanup = packet.read_bool("force-cleanup");
        long object_index = -1;
        if (force_cleanup && entity_definition->entity_type_is_gameworld_object() && gamestate_index != -1)
        {
            long gamestate_object_index = simulation_gamestate_entity_get_object_index(gamestate_index);
            if (gamestate_object_index != -1)
                object_index = gamestate_object_index;
        }
        entity_definition->delete_game_entity(gamestate_index);
        if (object_index != -1)
            objects_purge_deleted_object(object_index);
        ::datum_delete(tls_simulation_gamestate_entity_data(), gamestate_index);
    }
}

void simulation_queue_entity_promotion_insert(s_simulation_entity *entity)
{
    unsigned char buffer[512];
    c_bitstream packet;
    packet.m_data_size_alignment = 1;
    packet.set_data(buffer, sizeof(buffer));
    packet.begin_writing();
    simulation_queue_entity_encode_header(&packet, entity->entity_type, entity->gamestate_index);
    packet.finish_writing(0);
    long data_size = packet.get_space_used_in_bytes();
    if (!packet.error_occurred())
    {
        c_simulation_world *world = simulation_get_world();
        s_simulation_queue_element *element = 0;
        world->simulation_queue_allocate(_simulation_queue_element_type_entity_promotion, data_size, &element);
        if (element)
        {
            memcpy(element->data, buffer, data_size);
            world->simulation_queue_enqueue(element);
        }
    }
}


// @address 0x41d588 ?simulation_queue_entity_promotion_apply@@YAXPBUs_simulation_queue_element@@@Z
void simulation_queue_entity_promotion_apply(struct s_simulation_queue_element const * element)
{
    if (simulation_queue_entities_is_distributed(tls_game_globals()))
    {
        c_bitstream packet;
        long entity_type = -1;
        long gamestate_index = -1;
        long data_size = element->data_size;
        unsigned char *data = element->data;
        packet.m_data_size_alignment = 1;
        packet.m_data_size_bytes = data_size;
        packet.m_data = data;
        packet.m_data_max = data + data_size;
        packet.reset(0);
        packet.reset(3);
        if (simulation_queue_decode_entity_reference(&packet, &entity_type, &gamestate_index))
        {
            bool exists = false;
            if (gamestate_index != -1)
                exists = datum_try_and_get(tls_simulation_gamestate_entity_data(), gamestate_index) != 0;
            if (exists)
            {
                c_simulation_entity_definition *definition = simulation_get_type_collection()->get_entity_definition(entity_type);
#if defined(__clang__)
                definition->promote_game_entity_to_authority(gamestate_index);
#else
                ((c_simulation_queue_entities_promotion_definition *)definition)->promote_game_entity_to_authority();
#endif
            }
        }
    }
}

// @address 0x41d6a0 simulation_queue_decode_entity_reference
extern "C" bool simulation_queue_decode_entity_reference(c_bitstream *packet, long *entity_type, long *gamestate_index)
{
    bool valid = false;
    unsigned long high_bits;
    long gamestate;

    *entity_type = packet->read_integer("entity-type", 5);
    high_bits = packet->read_integer("gamestate-index-high-bits", 16);
    gamestate = packet->read_integer("gamestate-index-low-bits", 11) | (high_bits << 16);
    *gamestate_index = gamestate;
    if (*entity_type >= 0 && *entity_type < 22 && gamestate != -1)
    {
        bool error = packet->overflowed();
        if (packet->m_error_occurred)
            error = true;
        if (!error)
            valid = true;
    }
    return valid;
}

bool encode_simulation_queue_creation_to_buffer(unsigned char *buffer, long buffer_size, long gamestate_index,
    s_simulation_queue_entity_data *data, unsigned long initial_update_mask, long *out_buffer_size)
{
    c_bitstream packet;
    packet.m_data_size_alignment = 1;
    packet.set_data(buffer, 1024);
    c_simulation_entity_definition *definition = simulation_get_type_collection()->get_entity_definition(data->entity_type);
    packet.begin_writing();
    simulation_queue_entity_encode_header(&packet, data->entity_type, gamestate_index);
    packet.write_integer("initial-update-mask", initial_update_mask, 32);
    definition->entity_creation_encode(data->creation_data_size, data->creation_data, 0, &packet, false);
    if (initial_update_mask)
    {
        unsigned long written_update_mask = 0;
        definition->entity_update_encode(true, initial_update_mask, &written_update_mask, data->state_data_size, data->state_data, 0, &packet, 0, false);
    }
    packet.finish_writing(0);
    *out_buffer_size = packet.get_space_used_in_bytes();
    return !packet.error_occurred();
}

bool encode_simulation_queue_update_to_buffer(unsigned char *buffer, long buffer_size, s_simulation_queue_entity_data *data,
    long gamestate_index, unsigned long update_mask, long *out_buffer_size)
{
    c_bitstream packet;
    packet.m_data_size_alignment = 1;
    packet.set_data(buffer, 1024);
    packet.begin_writing();
    simulation_queue_entity_encode_header(&packet, data->entity_type, gamestate_index);
    packet.write_integer("update-mask", update_mask, 32);
    c_simulation_entity_definition *definition = simulation_get_type_collection()->get_entity_definition(data->entity_type);
    unsigned long written_update_mask = 0;
    definition->entity_update_encode(false, update_mask, &written_update_mask, data->state_data_size, data->state_data, 0, &packet, 0, false);
    packet.finish_writing(0);
    *out_buffer_size = packet.get_space_used_in_bytes();
    return !packet.error_occurred();
}

// @address 0x41d910 decode_simulation_queue_creation_from_buffer
extern "C" bool decode_simulation_queue_creation_from_buffer(unsigned char *buffer, long buffer_size, s_simulation_queue_decoded_creation_data *creation)
{
    bool success = false;
    c_bitstream stream;
    stream.m_data = buffer;
    c_simulation_type_collection *type_collection = simulation_get_type_collection();
    stream.m_data_size_alignment = 1;
    stream.m_data_max = buffer + buffer_size;
    stream.m_data_size_bytes = buffer_size;
    stream.reset(0);
    stream.reset(3);
    if (simulation_queue_decode_entity_reference(&stream, &creation->entity_type, &creation->gamestate_index))
    {
        creation->initial_update_mask = (long)stream.read_integer("initial-update-mask", 32);
        c_simulation_entity_definition *definition = type_collection->get_entity_definition(creation->entity_type);
        creation->creation_data_size = definition->creation_data_size();
        creation->state_data_size = definition->state_data_size();
        if ((unsigned long)creation->creation_data_size <= sizeof(creation->creation_data) &&
            (unsigned long)creation->state_data_size <= sizeof(creation->state_data))
        {
            success = definition->entity_creation_decode(creation->creation_data_size, creation->creation_data, &stream, false);
            if (success)
            {
                if ((unsigned long)creation->initial_update_mask != 0)
                {
                    unsigned long update_mask = 0;
                    definition->build_baseline_state_data(creation->creation_data_size, creation->creation_data,
                        creation->state_data_size, creation->state_data);
                    definition->entity_update_decode(true, &update_mask, creation->state_data_size, creation->state_data, &stream, false);
                }
                bool error = stream.m_current.current_stream_bit_position > stream.m_data_size_bytes * 8;
                if (stream.m_error_occurred)
                    error = true;
                success = !error;
            }
        }
    }
    if (success && creation->gamestate_index == -1)
        success = false;
    return success;
}

// @address 0x41dc70 decode_simulation_queue_update_from_buffer
extern "C" unsigned char decode_simulation_queue_update_from_buffer(unsigned char *buffer, long buffer_size, s_simulation_queue_decoded_update_data *update)
{
    unsigned char success = false;
    c_bitstream stream;
    stream.m_data = buffer;
    stream.m_data_size_bytes = buffer_size;
    stream.m_data_size_alignment = 1;
    stream.m_data_max = buffer + buffer_size;
    stream.reset(0);
    stream.reset(3);
    if (simulation_queue_decode_entity_reference(&stream, &update->entity_type, &update->gamestate_index))
    {
        c_simulation_entity_definition *definition = simulation_get_type_collection()->get_entity_definition(update->entity_type);
        unsigned char scratch[1024] = {0};
        update->state_data_size = definition->state_data_size();
        update->update_mask = stream.read_integer("update-mask", 32);
        if ((unsigned long)definition->creation_data_size() <= sizeof(scratch))
        {
            unsigned long update_mask = 0;
            definition->build_creation_data(update->gamestate_index, definition->creation_data_size(), scratch);
            definition->build_baseline_state_data(
                definition->creation_data_size(), scratch, update->state_data_size, update->state_data);
            success = definition->entity_update_decode(false, &update_mask, update->state_data_size, update->state_data, &stream, false);
            if (success)
            {
                bool error = stream.m_current.current_stream_bit_position > stream.m_data_size_bytes * 8;
                if (stream.m_error_occurred)
                    error = true;
                success = !error;
            }
        }
    }
    return success;
}
