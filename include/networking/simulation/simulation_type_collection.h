#ifndef NETWORKING_SIMULATION_SIMULATION_TYPE_COLLECTION_H
#define NETWORKING_SIMULATION_SIMULATION_TYPE_COLLECTION_H

#include "networking/simulation/simulation_event_definition.h"
#include <stddef.h>
#include <string.h>

class c_simulation_entity_definition;

// Holds up to 32 entity definitions and 64 event definitions.
class c_simulation_type_collection
{
public:
    __forceinline void initialize()
    {
        m_entity_type_count = -1;
        memset(m_entity_definitions, 0, sizeof(m_entity_definitions));
        m_event_type_count = -1;
        memset(m_event_definitions, 0, sizeof(m_event_definitions));
    }

    // Returns the number of registered event types.
    __forceinline long get_event_definition_count() const { return m_event_type_count; }

    __forceinline c_simulation_event_definition *get_event_definition(e_simulation_event_type type) const
    {
        c_simulation_event_definition *definition = m_event_definitions[type];
        return definition;
    }

    __forceinline c_simulation_entity_definition *get_entity_definition(long type) const
    {
        return m_entity_definitions[type];
    }

    __forceinline void register_entity_definition(long type, c_simulation_entity_definition *definition)
    {
        m_entity_definitions[type] = definition;
    }

    __forceinline void register_event_definition(long type, c_simulation_event_definition *definition)
    {
        m_event_definitions[type] = definition;
    }

    __forceinline void finish_types(long entity_count, long event_count)
    {
        m_event_type_count = event_count;
        m_entity_type_count = entity_count;
    }

private:
    long m_entity_type_count;
    c_simulation_entity_definition *m_entity_definitions[32];
    long m_event_type_count;
    c_simulation_event_definition *m_event_definitions[64];
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char simulation_type_collection_size[(sizeof(void *) != 4 || sizeof(c_simulation_type_collection) == 0x188) ? 1 : -1];
#endif

#endif
