// hkutilities/hkMultithreadingUtil.obj: from the records 0x619f78 0x619fa0 0x61a000 0x61a020 0x61a0c0 0x61a0c8 0x61a140 0x61a198 0x61a1e0 0x61a258 0x61a2e0 0x61a2e8 0x61a340 0x61a3d0 0x61a4c8 0x61a570 0x61a960 0x61aa38 0x61ab08 0x61ab70 0x61ac38
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x619f78 ??0phys_multithreading_util_desc@@QAA@XZ
// @address 0x619fa0 ??0worker_thread_data@phys_multithreading_util@@QAA@XZ
// @address 0x61a000 ?attach_world@phys_multithreading_util@@QAAXPAVphys_world@@@Z
// @address 0x61a020 ?detach_world@phys_multithreading_util@@QAAXXZ
// @address 0x61a0c0 ?set_frame_time_marker@phys_multithreading_util@@QAAXM@Z
// @address 0x61a0c8 ?begin_world_step@phys_multithreading_util@@QAAXM@Z
// @address 0x61a140 ?wait_for_step_world@phys_multithreading_util@@QAAXXZ
// @address 0x61a198 ?wait_for_init_done@phys_multithreading_util@@QAAXXZ
// @address 0x61a1e0 ?step_world@phys_multithreading_util@@QAAXM@Z
// @address 0x61a258 ?quit_multithreading@phys_multithreading_util@@IAAXXZ
// @address 0x61a2e0 ?get_thread_count@phys_multithreading_util@@QAAHXZ
// @address 0x61a2e8 ?reassign_thread_tokens@phys_multithreading_util@@IAAXXZ
// @address 0x61a340 ?remove_worker_thread@phys_multithreading_util@@IAA?AW4phys_result@@XZ
// @address 0x61a3d0 ??0State@phys_multithreading_util@@QAA@XZ
// @address 0x61a4c8 ??1phys_multithreading_util@@QAA@XZ
// @address 0x61aa38 ?add_worker_thread@phys_multithreading_util@@IAA?AW4phys_result@@XZ
// @address 0x61ab08 ??0phys_multithreading_util@@QAA@ABUphys_multithreading_util_desc@@@Z
// @address 0x61ac38 ??1State@phys_multithreading_util@@QAA@XZ
#include <xtl.h>
#include "phys/phys_base_types.h"
#include <vectorintrinsics.h>
#include "physics/havok.h"
#include "physics/havok_halo_external.h"
#include "phys/phys_math.h"
#include "phys/phys_multithreading_util.h"
#include "phys/phys_memory.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_semaphore.h"
#include "phys/phys_critical_section.h"
#include "phys/phys_thread_memory_allocator.h"
#include "phys/phys_system.h"
#include "phys/phys_monitor_stream.h"


struct c_synchronized_lock;
class phys_physics_system;
#include "phys/phys_object.h"
#include "phys/phys_action.h"
#include "phys/phys_action_listener.h"
#include "phys/phys_collision_listener.h"
#include "phys/phys_constraint_listener.h"
#include "phys/phys_entity_listener.h"
#include "phys/phys_island_activation_listener.h"
#include "phys/phys_island_post_collide_listener.h"
#include "phys/phys_island_post_integrate_listener.h"
#include "phys/phys_post_simulation_listener.h"
#include "phys/phys_post_integrate_listener.h"
#include "phys/phys_post_collide_listener.h"
#include "phys/phys_world_op_queue_types.h"
#include "phys/phys_world_deletion_listener.h"
#include "phys/phys_volume_listener.h"
namespace {
static __forceinline void add_world_ref(phys_world *world)
{
    phys_ref_object *object = (phys_ref_object *)world;
    if (object->m_mem_size_and_flags != 0)
        ++object->m_object_ref_count;
}
static __forceinline void remove_world_ref(phys_world *world)
{
    ((phys_ref_object *)world)->object_remove_ref();
}
}
long g_next_phys_thread_index = -1;

phys_multithreading_util_desc::phys_multithreading_util_desc()
{
    m_world = 0;
    m_thread_count = 1;
    m_stack_size = 4000000;
    m_timers_enabled = 1;
    m_primary_thread_count = 0;
}

phys_multithreading_util::worker_thread_data::worker_thread_data() : m_semaphore(0, 1)
{
    m_monitor_stream = 0;
    m_monitor_stream_end = 0;
    m_kill_thread = 0;
    m_lock_thread_id = -1;
    m_state = 0;
}

void phys_multithreading_util::attach_world(phys_world *world)
{
    m_state.m_world = world;
    add_world_ref(world);
}

void phys_multithreading_util::detach_world()
{
    remove_world_ref(m_state.m_world);
    m_state.m_world = 0;
}

void phys_multithreading_util::set_frame_time_marker(float time)
{
    m_state.m_frame_time_marker = time;
}

void phys_multithreading_util::begin_world_step(float body_delta_time)
{
    m_state.m_physics_running = 1;
    m_state.m_delta_time = body_delta_time;
    for (int i = m_state.m_thread_count - 1; i >= 0; --i) {
        worker_thread_data &worker_data = m_state.m_worker_threads[i];
        worker_data.m_semaphore.release(1);
    }
}

void phys_multithreading_util::wait_for_step_world()
{
    if (m_state.m_physics_running) {
        for (int i = 0; i < m_state.m_thread_count; ++i)
            m_state.m_step_semaphore.acquire_semaphore();
        m_state.m_physics_running = 0;
    }
}

void phys_multithreading_util::wait_for_init_done()
{
    for (int i = 0; i < m_state.m_thread_count; ++i)
        m_state.m_init_semaphore.acquire_semaphore();
}

void phys_multithreading_util::step_world(float body_delta_time)
{
    m_state.m_world->lock();
    m_state.m_world->check_determinism();
    m_state.m_world->unlock();
    begin_world_step(body_delta_time);
    wait_for_step_world();
    m_state.m_world->lock();
    m_state.m_world->check_determinism();
    m_state.m_world->unlock();
}

void phys_multithreading_util::quit_multithreading()
{
    wait_for_step_world();
    for (int i = 0; i < m_state.m_thread_count; ++i) {
        worker_thread_data &worker_data = m_state.m_worker_threads[i];
        worker_data.m_kill_thread = 1;
        worker_data.m_semaphore.release(1);
    }
    for (int i = 0; i < m_state.m_thread_count; ++i)
        m_state.m_step_semaphore.acquire_semaphore();
}

int phys_multithreading_util::get_thread_count()
{
    return m_state.m_thread_count;
}

void phys_multithreading_util::reassign_thread_tokens()
{
    m_state.m_world->reset_thread_tokens();
    for (int i = 0; i < m_state.m_thread_count; ++i) {
        worker_thread_data &worker_data = m_state.m_worker_threads[i];
        worker_data.m_thread_token = m_state.m_world->get_thread_token();
    }
}

phys_result phys_multithreading_util::remove_worker_thread()
{
    if (m_state.m_thread_count > 1) {
        --m_state.m_thread_count;
        worker_thread_data &worker_data = m_state.m_worker_threads[m_state.m_thread_count];
        worker_data.m_kill_thread = 1;
        worker_data.m_semaphore.release(1);
        m_state.m_step_semaphore.acquire_semaphore();
        reassign_thread_tokens();
        return PHYS_SUCCESS;
    } else {
        return PHYS_FAILURE;
    }
}

phys_multithreading_util::State::State()
    : m_init_semaphore(0, 7), m_step_semaphore(0, 7), m_step_init_semaphore(0, 7), m_step_end_semaphore(0, 7),
        m_advance_time_semaphore(0, 7), m_barrier_semaphore(0, 7)
{
    m_thread_count = 0;
    m_physics_running = false;
    m_world = 0;
    m_delta_time = 0.016666668f;
    m_frame_time_marker = -1.0f;
    m_cpu_thread_count = 1;
}

phys_multithreading_util::~phys_multithreading_util()
{
    if (m_state.m_world)
        remove_world_ref(m_state.m_world);
    quit_multithreading();
}

phys_result phys_multithreading_util::add_worker_thread()
{
    int max_threads = m_state.m_world->m_simulation_type >= 3 ? 7 - m_state.m_cpu_thread_count : 1;
    if (m_state.m_thread_count < max_threads) {
        worker_thread_data &worker_data = m_state.m_worker_threads[m_state.m_thread_count];
        worker_data.m_state = &m_state;
        worker_data.m_lock_thread_id = m_state.m_thread_count;
        worker_data.m_monitor_stream = 0;
        worker_data.m_monitor_stream_end = 0;
        worker_data.m_kill_thread = 0;
        g_next_phys_thread_index = m_state.m_thread_count;
        worker_data.m_worker_thread.worker_start_thread(worker_thread_main, &worker_data);
        ++m_state.m_thread_count;
        reassign_thread_tokens();
        return PHYS_SUCCESS;
    } else {
        return PHYS_FAILURE;
    }
}

phys_multithreading_util::phys_multithreading_util(const phys_multithreading_util_desc &info)
{
    m_state.m_stack_size = info.m_stack_size;
    m_state.m_primary_thread_count = info.m_primary_thread_count;
    m_state.m_timers_enabled = info.m_timers_enabled;
    init_multithreading(info.m_world, info.m_thread_count);
}

phys_multithreading_util::State::~State()
{
}

// @address 0x1b65a8 ?havok_thread_user_data_get@@YAPAXJ@Z



// @address 0x61a570 ?worker_thread_main@@YAPAXPAX@Z
void *worker_thread_main(void *v)
{
    INT_PTR thread_index = reinterpret_cast<INT_PTR>(v);
    phys_multithreading_util::worker_thread_data &data = *static_cast<phys_multithreading_util::worker_thread_data *>(havok_thread_user_data_get(thread_index));
    phys_multithreading_util::State &state = *data.m_state;
    havok_thread_user_data_get(thread_index);

    int thread_slot = (data.m_lock_thread_id % 3) * 2;
    if (data.m_lock_thread_id > 2)
        thread_slot++;
    XSetThreadProcessor((void *)-2, thread_slot);

    phys_thread_memory system_thread_memory(&phys_memory::get_singleton_instance(), 16);
    phys_system::init_thread_memory(&system_thread_memory);

    char *stack_buffer;
    {
        int thread_stack_size = state.m_stack_size;
        stack_buffer = phys_alloc<char>(thread_stack_size, PHYS_MEM_BASE);
        system_thread_memory.set_stack_area(stack_buffer, thread_stack_size);
    }

    if (state.m_timers_enabled) {
        int monitor_stream_size = 2000000;
        phys_monitor_stream::get_singleton_instance().resize(monitor_stream_size);
    }
    data.m_monitor_stream = phys_monitor_stream::get_singleton_instance().get_start();
    data.m_monitor_stream_end = phys_monitor_stream::get_singleton_instance().get_track_end();

    state.m_init_semaphore.release(1);
    data.m_semaphore.acquire_semaphore();

    phys_thread_token token = data.m_thread_token;

    while (data.m_kill_thread == false) {
        havok_notify_thread_frame_started(thread_index);
        phys_monitor_stream::get_singleton_instance().reset();
        data.m_monitor_stream_end = phys_monitor_stream::get_singleton_instance().get_track_end();

        phys_critical_section::enable_lock_timers();

        if (state.m_frame_time_marker == -1.f) {
            if (token == PHYS_THREAD_TOKEN_FIRST) {
                state.m_world->step_begin_st(state.m_delta_time);
                state.m_step_init_semaphore.release(state.m_thread_count + state.m_cpu_thread_count);
            }
            state.m_step_init_semaphore.acquire_semaphore();
            state.m_world->step_process_mt(token);
            if (token == PHYS_THREAD_TOKEN_FIRST) {
                state.m_world->step_end_st();
            }
        } else {
            if (token == PHYS_THREAD_TOKEN_FIRST) {
                state.m_world->set_frame_time_marker(state.m_frame_time_marker);
                state.m_world->advance_time();
                state.m_advance_time_semaphore.release(state.m_thread_count + state.m_cpu_thread_count);
            }
            state.m_advance_time_semaphore.acquire_semaphore();

            while (!state.m_world->sim_at_marker()) {
                if (token == PHYS_THREAD_TOKEN_FIRST) {
                    for (int t = 0; t < state.m_thread_count + state.m_cpu_thread_count - 1; t++) {
                        state.m_barrier_semaphore.acquire_semaphore();
                    }
                } else {
                    state.m_barrier_semaphore.release(1);
                }

                if (token == PHYS_THREAD_TOKEN_FIRST) {
                    state.m_world->step_begin_st(state.m_delta_time);
                    state.m_step_init_semaphore.release(state.m_thread_count + state.m_cpu_thread_count);
                }
                state.m_step_init_semaphore.acquire_semaphore();
                state.m_world->step_process_mt(token);
                if (token == PHYS_THREAD_TOKEN_FIRST) {
                    state.m_world->step_end_st();
                    state.m_world->sim_at_psi();
                    state.m_step_end_semaphore.release(state.m_thread_count + state.m_cpu_thread_count);
                }
                state.m_step_end_semaphore.acquire_semaphore();

                if (token == PHYS_THREAD_TOKEN_FIRST) {
                    for (int t = 0; t < state.m_thread_count + state.m_cpu_thread_count - 1; t++) {
                        state.m_barrier_semaphore.acquire_semaphore();
                    }
                } else {
                    state.m_barrier_semaphore.release(1);
                }
            }
        }

        phys_critical_section::disable_lock_timers();
        phys_monitor_stream &stream = phys_monitor_stream::get_singleton_instance();
        data.m_monitor_stream_end = stream.get_track_end();
        havok_notify_thread_frame_finished(thread_index);

        state.m_step_semaphore.release(1);
        data.m_semaphore.acquire_semaphore();
    }

    system_thread_memory.set_stack_area(0, 0);
    phys_free_memory<char>(stack_buffer);
    phys_system::release_thread_resources();

    state.m_step_semaphore.release(1);

    return 0;
}

// @address 0x61a960 ?init_multithreading@phys_multithreading_util@@IAAXPAVphys_world@@H@Z
void phys_multithreading_util::init_multithreading(phys_world *world, int worker_thread_count)
{
    attach_world(world);
    if (worker_thread_count > 7)
        worker_thread_count = 7;
    for (int i = 0; i < worker_thread_count; ++i) {
        worker_thread_data &worker_data = m_state.m_worker_threads[m_state.m_thread_count++];
        worker_data.m_state = &m_state;
        worker_data.m_lock_thread_id = i;
        g_next_phys_thread_index = i;
        worker_data.m_worker_thread.worker_start_thread(worker_thread_main, &worker_data);
    }
    m_state.m_world->reset_thread_tokens();
    for (int i = 0; i < m_state.m_thread_count; ++i) {
        worker_thread_data &worker_data = m_state.m_worker_threads[i];
        worker_data.m_thread_token = m_state.m_world->get_thread_token();
    }
}

static __forceinline phys_result remove_thread_inline(phys_multithreading_util *penetration_util)
{
    if (penetration_util->m_state.m_thread_count > 1) {
        --penetration_util->m_state.m_thread_count;
        phys_multithreading_util::worker_thread_data &worker_data = penetration_util->m_state.m_worker_threads[penetration_util->m_state.m_thread_count];
        worker_data.m_kill_thread = 1;
        worker_data.m_semaphore.release(1);
        penetration_util->m_state.m_step_semaphore.acquire_semaphore();
        penetration_util->m_state.m_world->reset_thread_tokens();
        for (int i = 0; i < penetration_util->m_state.m_thread_count; ++i) {
            phys_multithreading_util::worker_thread_data &data = penetration_util->m_state.m_worker_threads[i];
            data.m_thread_token = penetration_util->m_state.m_world->get_thread_token();
        }
        return PHYS_SUCCESS;
    }
    return PHYS_FAILURE;
}

// @address 0x61ab70 ?set_thread_count@phys_multithreading_util@@QAAHH@Z
int phys_multithreading_util::set_thread_count(int worker_thread_count)
{
    while (m_state.m_thread_count < worker_thread_count && PHYS_SUCCESS == add_worker_thread()) { }
    while (m_state.m_thread_count > worker_thread_count && PHYS_SUCCESS == remove_thread_inline(this)) { }
    return m_state.m_thread_count;
}

