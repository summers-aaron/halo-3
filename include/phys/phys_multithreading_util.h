#ifndef INCLUDE_PHYS_MULTITHREADING_UTIL_H
#define INCLUDE_PHYS_MULTITHREADING_UTIL_H

#include <stddef.h>

#include "phys/phys_base_types.h"
#include "phys/phys_semaphore.h"
#include "phys/phys_thread.h"
#include "phys/phys_thread_memory.h"

class phys_world;
enum phys_result;
#include "phys/phys_thread_token.h"

struct phys_multithreading_util_desc {
    phys_world *m_world;
    int m_thread_count;
    int m_stack_size;
    bool m_timers_enabled;
    int m_primary_thread_count;
    phys_multithreading_util_desc();
};

// Steps the world across seven worker threads synchronized by semaphores.
class phys_multithreading_util {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_UTILITIES, phys_multithreading_util);

    struct State;
    struct worker_thread_data {
        State *m_state;
        phys_thread m_worker_thread;
        int m_lock_thread_id;
        bool m_kill_thread;
        phys_semaphore m_semaphore;
        phys_thread_token m_thread_token;
        char *m_monitor_stream;
        char *m_monitor_stream_end;
        worker_thread_data();
    };
    struct State {
        phys_world *m_world;
        float m_delta_time;
        float m_frame_time_marker;
        phys_semaphore m_init_semaphore;
        phys_semaphore m_step_semaphore;
        phys_semaphore m_step_init_semaphore;
        phys_semaphore m_step_end_semaphore;
        phys_semaphore m_advance_time_semaphore;
        phys_semaphore m_barrier_semaphore;
        phys_bool m_physics_running;
        worker_thread_data m_worker_threads[7];
        int m_thread_count;
        int m_stack_size;
        bool m_timers_enabled;
        int m_primary_thread_count;
        int m_cpu_thread_count;
        State();
        ~State();
    };
    State m_state;
    phys_multithreading_util(const phys_multithreading_util_desc &info);
    ~phys_multithreading_util();
    void attach_world(phys_world *world);
    void detach_world();
    void set_frame_time_marker(float time);
    void begin_world_step(float body_delta_time);
    void wait_for_step_world();
    void wait_for_init_done();
    void step_world(float body_delta_time);
    int get_thread_count();
    int set_thread_count(int worker_thread_count);
protected:
    void quit_multithreading();
    void reassign_thread_tokens();
    phys_result remove_worker_thread();
    phys_result add_worker_thread();
    void init_multithreading(phys_world *world, int worker_thread_count);
};

typedef char havok_size_hkMultithreadingUtilCinfo[(sizeof(void *) != 4 || ((sizeof(phys_multithreading_util_desc) == 0x14) )) ? 1 : -1];
typedef char havok_offset_WorkerThreadData_thread[(offsetof(phys_multithreading_util::worker_thread_data, m_worker_thread) == 8) ? 1 : -1];
typedef char havok_size_WorkerThreadData[(sizeof(void *) != 4 || ((sizeof(phys_multithreading_util::worker_thread_data) == 0x30) )) ? 1 : -1];
typedef char havok_offset_MultithreadingState_workers[(sizeof(void *) != 4 || ((offsetof(phys_multithreading_util::State,
    m_worker_threads) == 0x28) )) ? 1 : -1];
typedef char havok_size_MultithreadingState[(sizeof(void *) != 4 || ((sizeof(phys_multithreading_util::State) == 0x190) )) ? 1 : -1];

void *worker_thread_main(void *v);
extern long g_next_phys_thread_index;

#endif // HAVOK_HK_MULTITHREADING_UTIL_H
