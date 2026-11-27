#ifndef INCLUDE_PHYS_PRED_SIMPLEX_CYLINDER_NARROW_AGENT_H
#define INCLUDE_PHYS_PRED_SIMPLEX_CYLINDER_NARROW_AGENT_H

class phys_dispatcher;
class phys_vec4;
struct phys_narrow_agent_input;
struct phys_narrow_agent_process_input;
struct phys_agent_entry;
struct phys_process_collision_output;

namespace phys_pred_simplex_cylinder_narrow_agent {
    void *create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *data);
    void *process(const phys_narrow_agent_process_input &input, phys_agent_entry *entry, void *data, phys_vec4 *normal, phys_process_collision_output &output);
    void register_narrow_agent(phys_dispatcher *dispatcher);
}

#endif
