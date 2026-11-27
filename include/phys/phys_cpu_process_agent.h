#ifndef INCLUDE_PHYS_CPU_PROCESS_AGENT_H
#define INCLUDE_PHYS_CPU_PROCESS_AGENT_H

#include "phys/phys_pair_agent_entry.h"
#include "phys/phys_pair_agent_machine.h"
#include "phys/phys_dispatcher.h"
#include "phys/phys_memory.h"
#include "phys/phys_multithreaded_simulation.h"

static __forceinline void phys_cpu_process_agent_helper(phys_pair_agent_entry *entry, const phys_process_collision_input &input,
    phys_process_collision_output &process_output, phys_multithreaded_simulation *simulation)
{
    phys_collider *collider_a = entry->m_collider[0];
    phys_collider *collider_b = entry->m_collider[1];
    input.m_quality_info = input.m_agent_dispatcher->get_collision_quality_info(entry->m_quality_idx);
    input.m_create_pred_agents = input.m_quality_info->m_use_ccd;
    process_output.reset();
    phys_pair_agent_process(entry, input, process_output, entry->m_contact_manager);
    if (phys_memory::get_singleton_instance().get_available_memory() == 0) {
        phys_memory::get_singleton_instance().m_memory_state = phys_memory::MEMORY_STATE_EXHAUSTED;
    }
    if (phys_memory::get_singleton_instance().m_memory_state != phys_memory::MEMORY_STATE_EXHAUSTED) {
        if (!process_output.is_array_empty()) {
            entry->m_contact_manager->process_contact(*collider_a, *collider_b, input, process_output);
        }
        if (process_output.has_toi()) {
            phys_critical_section_lock lock(&simulation->m_toi_queue_lock);
            simulation->add_toi_event(process_output, *entry);
        }
    }
}

#endif
