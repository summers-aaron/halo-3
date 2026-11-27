#ifndef SIMULATION_SIMULATION_QUEUE_ENTITIES_H
#define SIMULATION_SIMULATION_QUEUE_ENTITIES_H

#include <stddef.h>

// Functions defined in tu/networkinglibxbox/simulation_queue_entities.cpp.

struct s_simulation_entity;
struct s_simulation_queue_element;
// Queue producers and decoders share these records with the world consumer.
struct s_simulation_queue_entity_data
{
    long entity_index;
    long entity_type;
    long creation_data_size;
    void *creation_data;
    long state_data_size;
    void *state_data;
};

struct s_simulation_queue_decoded_creation_data
{
    long entity_type;
    long gamestate_index;
    long initial_update_mask;
    unsigned char creation_data[1024];
    long creation_data_size;
    unsigned char state_data[1024];
    long state_data_size;
};

struct s_simulation_queue_decoded_update_data
{
    long entity_type;
    long gamestate_index;
    unsigned long update_mask;
    unsigned char state_data[1024];
    long state_data_size;
};

typedef char simulation_queue_entity_data_size[(sizeof(void *) != 4 || ((sizeof(s_simulation_queue_entity_data) == 0x18) )) ? 1 : -1];
typedef char simulation_queue_decoded_creation_data_size[(sizeof(s_simulation_queue_decoded_creation_data) == 0x814) ? 1 : -1];
typedef char simulation_queue_decoded_update_data_size[(sizeof(s_simulation_queue_decoded_update_data) == 0x410) ? 1 : -1];
typedef char simulation_queue_entity_creation_data_offset[(sizeof(void *) != 4
    || ((offsetof(s_simulation_queue_entity_data, creation_data) == 0xc) )) ? 1 : -1];
typedef char simulation_queue_entity_state_data_offset[(sizeof(void *) != 4 || ((offsetof(s_simulation_queue_entity_data, state_data) == 0x14) )) ? 1 : -1];
typedef char simulation_queue_decoded_creation_payload_offset[(offsetof(s_simulation_queue_decoded_creation_data, creation_data) == 0xc) ? 1 : -1];
typedef char simulation_queue_decoded_creation_size_offset[(offsetof(s_simulation_queue_decoded_creation_data, creation_data_size) == 0x40c) ? 1 : -1];
typedef char simulation_queue_decoded_creation_state_offset[(offsetof(s_simulation_queue_decoded_creation_data, state_data) == 0x410) ? 1 : -1];
typedef char simulation_queue_decoded_creation_state_size_offset[(offsetof(s_simulation_queue_decoded_creation_data, state_data_size) == 0x810) ? 1 : -1];
typedef char simulation_queue_decoded_update_state_offset[(offsetof(s_simulation_queue_decoded_update_data, state_data) == 0xc) ? 1 : -1];
typedef char simulation_queue_decoded_update_state_size_offset[(offsetof(s_simulation_queue_decoded_update_data, state_data_size) == 0x40c) ? 1 : -1];

bool simulation_queue_entity_creation_allocate(s_simulation_queue_entity_data *data, unsigned long initial_update_mask,
    s_simulation_queue_element **out_element, long *out_gamestate_index);
void simulation_queue_entity_creation_apply(const s_simulation_queue_element *element);
bool simulation_queue_entity_update_allocate(s_simulation_queue_entity_data *data, long gamestate_index,
    unsigned long update_mask, s_simulation_queue_element **out_element);
void simulation_queue_entity_deletion_insert(s_simulation_entity *entity, bool force_cleanup);
void simulation_queue_entity_deletion_apply(struct s_simulation_queue_element const * element);
void simulation_queue_entity_promotion_insert(s_simulation_entity *entity);
void simulation_queue_entity_promotion_apply(struct s_simulation_queue_element const * _sym_p0);

extern "C" unsigned char decode_simulation_queue_update_from_buffer(unsigned char *buffer, long buffer_size, s_simulation_queue_decoded_update_data *update);

class c_bitstream;
extern "C" bool simulation_queue_decode_entity_reference(c_bitstream *packet, long *entity_type, long *gamestate_index);
bool encode_simulation_queue_creation_to_buffer(unsigned char *buffer, long buffer_size, long gamestate_index,
    s_simulation_queue_entity_data *data, unsigned long initial_update_mask, long *out_buffer_size);
bool encode_simulation_queue_update_to_buffer(unsigned char *buffer, long buffer_size, s_simulation_queue_entity_data *data,
    long gamestate_index, unsigned long update_mask, long *out_buffer_size);
extern "C" bool decode_simulation_queue_creation_from_buffer(unsigned char *buffer, long buffer_size, s_simulation_queue_decoded_creation_data *creation);

#endif // SIMULATION_SIMULATION_QUEUE_ENTITIES_H
