#ifndef PHYSICS_HAVOK_MONITOR_STORAGE_H
#define PHYSICS_HAVOK_MONITOR_STORAGE_H
#include "phys/phys_monitor_stream.h"
#ifdef HALO3_64BIT
#include "phys/phys_dispatcher.h"
#endif
class phys_rigid_body;
struct s_havok_globals
{
    phys_rigid_body *rigid_bodies[16];
    long structure_bsp_state;
    long entity_count;
    long initialize_time;
    long lock_count_4c;
    bool initialized_for_new_map;
    bool environment_active;
    bool allocated_state_deferred;
    bool garbage_collection_locked;
    bool update_in_progress;
    bool inside_simulation_step;
    __declspec(align(8)) char base_system_object[sizeof(phys_thread_memory)];
#ifdef HALO3_64BIT
    __declspec(align(8)) char thread_memory[sizeof(phys_thread_memory)];
    __declspec(align(8)) char monitor_stream[sizeof(phys_monitor_stream)];
#else
    char thread_memory[816];
    char monitor_stream[20];
#endif
    __declspec(align(16)) char scratch[65536];
    char monitor_streams[1][sizeof(phys_monitor_stream)];
    long monitor_stream_count;
#ifdef HALO3_64BIT
    __declspec(align(8)) char cached_dispatcher[sizeof(phys_dispatcher)];
#else
    char cached_dispatcher[7440];
#endif
    bool dispatcher_initialized;
};
typedef char havok_base_system_object_offset[(sizeof(void *) != 4 || ((offsetof(s_havok_globals, base_system_object) == 0x58))) ? 1 : -1];
typedef char havok_thread_memory_offset[(sizeof(void *) != 4 || ((offsetof(s_havok_globals, thread_memory) == 0x388))) ? 1 : -1];
typedef char havok_scratch_offset[(sizeof(void *) != 4 || ((offsetof(s_havok_globals, scratch) == 0x6d0))) ? 1 : -1];
extern "C" s_havok_globals g_havok_monitor_globals;
#endif
