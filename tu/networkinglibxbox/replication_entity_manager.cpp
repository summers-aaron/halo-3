// networkinglibxbox/replication_entity_manager.obj: from the records 0x4165d8 0x416740 0x4167f0 0x416858
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "memory/datum_index.h"
#include "networking/replication/replication_entity_manager.h"
#include "networking/replication/replication_entity_manager_view.h"
#include "networking/replication/replication_entity_view_state_types.h"
#include "simulation/simulation_queue_entities.h"
#include "networking/simulation/simulation_entity_database.h"
#include "networking/simulation/simulation_entity_definition_types.h"
#include "networking/simulation/simulation_entity_database_operations.h"
#include "simulation/simulation_gamestate_entities.h"

// @address 0x40a2c8 ?notify_mark_entity_for_deletion@c_simulation_entity_database@@QAAXJ_N@Z
// @address 0x40a448 ?entity_delete_internal@c_simulation_entity_database@@AAAXJ@Z
// @address 0x416858 ?make_all_local@c_replication_entity_manager@@QAAXXZ
// @address 0x41b9c0 ?mark_entity_for_deletion@c_replication_entity_manager_view@@QAAXJ@Z

__forceinline long c_replication_entity_manager::preallocate_entity() {
    long entity_index = -1;
    long index;
    for (index = m_next_entity; index < 1024; index++) {
        if (!m_entity_data[index].allocated()) {
            entity_index = index;
            break;
        } else {
            continue;
        }
    }
    if (entity_index == -1) {
        long wrapped_index;
        for (wrapped_index = 0; wrapped_index < m_next_entity; wrapped_index++) {
            if (!m_entity_data[wrapped_index].allocated()) {
                entity_index = wrapped_index;
                break;
            } else {
                continue;
            }
        }
    }
    if (entity_index != -1) {
        m_entity_data[entity_index].flags = 1;
        m_next_entity = (entity_index + 1) % 1024;
    }
    return entity_index;
}

__forceinline long c_replication_entity_manager::create_local_entity_internal(long entity_index) {
    s_replication_entity_data *entity = &m_entity_data[entity_index];
    entity->flags |= 4;
    entity->seed = (entity->seed + 1) % 16;
    long entity_id = entity_index | (entity->seed << 28);
    long view_index;
    for (view_index = 0; view_index < 15; view_index++) {
        if (m_views[view_index] != 0) {
            m_views[view_index]->create_entity(entity_id);
        }
    }
    return entity_id;
}

__forceinline unsigned char c_replication_entity_manager::rotate_seed(unsigned char seed) {
    return (unsigned char)((seed + 1) % 16);
}
__forceinline void c_replication_entity_manager::delete_entity_internal(long entity_index) {
    s_replication_entity_data *entity = get_entity(entity_index);
    m_client->entity_delete_internal(entity_index);
    entity->flags &= ~1;
}

long c_replication_entity_manager::create_local_entity() {
    long entity_id = -1;
    long entity_index = preallocate_entity();
    if (entity_index != -1) {
        entity_id = create_local_entity_internal(entity_index);
    }
    return entity_id;
}

// @address 0x416740 ?delete_local_entity@c_replication_entity_manager@@QAAXJ@Z
void c_replication_entity_manager::delete_local_entity(long entity_index) {
    s_replication_entity_data *entity = get_entity(entity_index);
    entity->flags |= 2;
    m_client->c_simulation_entity_database::notify_mark_entity_for_deletion(entity_index, false);
    if (m_view_mask != 0) {
        long view_index;
        for (view_index = 0; view_index < 15; view_index++) {
            if ((m_view_mask & (1 << view_index)) != 0) {
                m_views[view_index]->mark_entity_for_deletion(entity_index);
            }
        }
    }
    if (entity->views == 0) {
        delete_entity_internal(entity_index);
    }
}

// @address 0x4167f0 ?acknowledge_entity_deletion@c_replication_entity_manager@@AAAXJJ@Z
void c_replication_entity_manager::acknowledge_entity_deletion(long view_index, long entity_index) {
    s_replication_entity_data *entity = get_entity(entity_index);
    if (entity->views == 0) {
        delete_entity_internal(entity_index);
    }
}

// @address 0x416858 ?make_all_local@c_replication_entity_manager@@QAAXXZ
void c_replication_entity_manager::make_all_local() {
    for (long entity_index = 0; entity_index < 1024; entity_index++) {
        s_replication_entity_data *entity = &m_entity_data[entity_index];
        if (entity->flags & 1) {
            long entity_id = entity_index | (entity->seed << 28);
            entity->seed = rotate_seed(entity->seed);
            s_simulation_entity *simulation_entity = m_client->entity_get(entity_id);
            long entity_word = simulation_entity->entity_index;
            c_simulation_entity_definition *definition = m_client->get_entity_definition(simulation_entity->entity_type);
            int generation = ((unsigned int)entity_word >> 28) + 1;
            int rotated_generation = generation % 16;
            simulation_entity->entity_index = (rotated_generation << 28) | (entity_word & 0x3ff);
            definition->rotate_entity_indices(simulation_entity);
            if (simulation_entity->gamestate_index != -1) {
                s_simulation_gamestate_entity_datum *gamestate_entities =
                    (s_simulation_gamestate_entity_datum *)tls_simulation_gamestate_entity_data()->data;
                gamestate_entities[datum_index_to_absolute_index(simulation_entity->gamestate_index)].entity_index =
                    simulation_entity->entity_index;
            }
            if (!(entity->flags & 4)) {
                entity->flags |= 4;
                simulation_queue_entity_promotion_insert((s_simulation_entity *)m_client->entity_get(entity_index));
            }
        }
    }
}
