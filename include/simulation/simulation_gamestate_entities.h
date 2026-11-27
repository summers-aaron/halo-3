#ifndef SIMULATION_SIMULATION_GAMESTATE_ENTITIES_H
#define SIMULATION_SIMULATION_GAMESTATE_ENTITIES_H

// Functions defined in tu/networkinglibxbox/simulation_gamestate_entities.cpp.

#include <stddef.h>

// Each entity slot holds an entity index and an object index. The entity manager rotates entity_index;
// object creation stores object_index; build_clear_flags queues each slot once.
struct s_simulation_gamestate_entity_datum
{
    short identifier;
    short pad2;
    long entity_index;
    long object_index;
    unsigned char deletion_queued;
};

struct s_simulation_queue_gamestate_clear_data
{
    unsigned long bits[64];
};

typedef char simulation_gamestate_entity_size[(sizeof(s_simulation_gamestate_entity_datum) == 0x10) ? 1 : -1];
typedef char simulation_gamestate_entity_index_offset[(offsetof(s_simulation_gamestate_entity_datum, entity_index) == 4) ? 1 : -1];
typedef char simulation_gamestate_entity_object_offset[(offsetof(s_simulation_gamestate_entity_datum, object_index) == 8) ? 1 : -1];
typedef char simulation_gamestate_entity_deletion_offset[(offsetof(s_simulation_gamestate_entity_datum, deletion_queued) == 0xc) ? 1 : -1];


void simulation_gamestate_entities_build_clear_flags(s_simulation_queue_gamestate_clear_data *clear_data);
void simulation_gamestate_entities_notify_simulation_world_reset(void);
long simulation_gamestate_entity_create(void);
long simulation_gamestate_entity_get_object_index(long entity_index);
long simulation_gamestate_entity_try_and_get_object_index(long handle, unsigned long type_mask);
void tls_set_g_gamestate_entity_data_allocator(void *object);

template<long, void (*)(void *), void (*)(void *), void (*)(void *)> class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &tls_set_g_gamestate_entity_data_allocator> g_simulation_gamestate_entity_data_allocator;

#endif // SIMULATION_SIMULATION_GAMESTATE_ENTITIES_H
