#ifndef INCLUDE_PHYS_AGENT_MACHINE_UTIL_H
#define INCLUDE_PHYS_AGENT_MACHINE_UTIL_H

struct phys_batch_agent_track;
struct phys_pair_agent_track;
class phys_statistics_collector;

class phys_agent_machine_util {
public:
    static void gather_batch_agent_stats(const phys_batch_agent_track &track, phys_statistics_collector *collector);
    static void gather_pair_agent_stats(const phys_pair_agent_track &track, phys_statistics_collector *collector);
    static void gather_contact_manager_stats(const phys_pair_agent_track &track, phys_statistics_collector *collector);
};

#endif
