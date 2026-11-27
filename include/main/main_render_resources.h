#ifndef MAIN_MAIN_RENDER_RESOURCES_H
#define MAIN_MAIN_RENDER_RESOURCES_H

#include <string.h>
#include "objects/objects_hierarchy_list.h"

class c_tag_resource_demand_collector;

class c_tag_resource_demand_callback
{
public:
    virtual unsigned long get_maximum_priority_count() = 0;
    virtual unsigned long get_inactive_priority_count() = 0;
    virtual void demand_callback(long priority, c_tag_resource_demand_collector *collector) = 0;
};

class c_main_render_demand_callback : public c_tag_resource_demand_callback
{
public:
    __forceinline c_main_render_demand_callback(long scenario_index, long first_person_hands_tag, long zone_set_index, bool print_messages)
        : m_scenario_index(scenario_index), m_first_person_hands_tag(first_person_hands_tag), m_zone_set_index(zone_set_index), m_print_messages(print_messages)
    {
        m_objects.cluster_object_count = 0;
        m_objects.object_indices.count = 0;
        m_objects.definition_indices.count = 0;
        memset(m_objects.object_set.bits, 0, sizeof(m_objects.object_set.bits));
    }

    virtual unsigned long get_maximum_priority_count();
    virtual unsigned long get_inactive_priority_count();
    virtual void demand_callback(long priority, c_tag_resource_demand_collector *collector);

private:
    long m_scenario_index;
    long m_first_person_hands_tag;
    long m_zone_set_index;
    bool m_print_messages;
    s_objects_hierarchy_list m_objects;
};

unsigned long _internal_halt_render_thread_and_lock_resources(const char *file, long line);
void unlock_resources_and_resume_render_thread(unsigned long flags);

#endif
