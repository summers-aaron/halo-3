#ifndef INCLUDE_PHYSICS_HAVOK_PENETRATION_COLLECTOR_H
#define INCLUDE_PHYSICS_HAVOK_PENETRATION_COLLECTOR_H
#include "phys/phys_collision_proxy.h"
#include "phys/phys_collectors.h"
#include "physics/havok_utilities.h"
#include "physics/havok_memory.h"
#include "physics/havok_memory_class.h"
#include "physics/havok_memory_globals.h"
class c_penetration_collector : public phys_proxy_pair_collector {
public:
    virtual void add_proxy_pair(const phys_collision_proxy &, const phys_collision_proxy &);
    static __forceinline void operator delete(void *p, size_t nbytes)
    {
        phys_thread_memory *memory = &phys_thread_memory::get_singleton_instance();
        if (g_havok_memory_globals.active_allocator == _memory_allocator_havok)
        {
            phys_memory *allocator = &g_havok_memory;
            allocator->free_chunk(p, static_cast<int>(nbytes), PHYS_MEM_AGENT);
        }
        else
            memory->free_chunk_to_list(p, static_cast<int>(nbytes));
    }
};
#endif
