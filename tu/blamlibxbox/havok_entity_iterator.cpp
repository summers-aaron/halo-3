// blamlibxbox/havok_entity_iterator.obj: from the records 0x106d08
// @flags /TP /O2 /Oc
// @address 0x106d08 ?next@c_havok_entity_iterator@@QAA_NXZ
#include <ppcintrinsics.h>
#include "phys/phys_island.h"
#include "physics/havok_entity_iterator.h"


bool c_havok_entity_iterator::next()
{
    bool found = false;
    if (m_collection_index == -1) {
        m_collection_index = 0;
        m_entity_index = 0;
        if (m_iteration_type == 1)
            m_first_collections_done = 1;
    } else {
        m_entity_index = m_entity_index + 1;
    }
    if (m_first_collections_done == 0) {
        while (m_collection_index < m_world->m_active_islands.m_size) {
            if (m_entity_index < m_world->m_active_islands.m_data[m_collection_index]->m_island_entities.m_size) {
                found = true;
                break;
            }
            m_entity_index = 0;
            m_collection_index = m_collection_index + 1;
        }
        if (found == 0) {
            m_first_collections_done = 1;
            m_collection_index = 0;
            m_entity_index = 0;
        }
    }
    if (m_first_collections_done == 0)
        return found;
    if (m_iteration_type != 2 && m_iteration_type != 1)
        return found;
    while (m_collection_index < m_world->m_inactive_islands.m_size) {
        if (m_entity_index < m_world->m_inactive_islands.m_data[m_collection_index]->m_island_entities.m_size) {
            found = true;
            break;
        }
        m_entity_index = 0;
        m_collection_index = m_collection_index + 1;
    }
    return found;
}
