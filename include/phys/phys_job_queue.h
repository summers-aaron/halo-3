#ifndef INCLUDE_PHYS_JOB_QUEUE_H
#define INCLUDE_PHYS_JOB_QUEUE_H

#include "phys/phys_queue.h"
#include "phys/phys_critical_section.h"
#include "phys/phys_semaphore.h"
#include <stddef.h>

class phys_job_queue {
public:
    // Queue1 is CPU broadphase work; the old SPU label remains until the queue owner migrates its four uses.
    enum job_kind { JOB_KIND_HOST = 0, JOB_KIND_HOST_BROADPHASE = 1, JOB_KIND_OFFLOAD = 1 };
    enum job_priority { JOB_PRIORITY_HIGH = 0, JOB_PRIORITY_LOW = 1 };
    enum worker_thread_kind { WORKER_THREAD_CPU = 0, WORKER_THREAD_BROADPHASE = 1, WORKER_THREAD_PRIMARY = 1 };
    enum job_wait_mode { JOB_WAIT_FOR_NEXT = 0, JOB_WAIT_NONE = 1 };
    enum job_status { JOB_QUEUE_HAS_WORK = 0, JOB_STATUS_NONE_READY = 1, JOB_QUEUE_DRAINED = 3 };
    enum query_rules { JOB_QUERY_RULES_CPU = 0, JOB_QUERY_RULES_COPROC = 2 };
    enum job_pop_result { JOB_QUEUE_POP_ENTRY, JOB_QUEUE_KEEP_ENTRY };
    enum job_creation_status { JOB_ENQUEUED, JOB_NOT_CREATED };
    struct job_queue_entry { __declspec(align(16)) unsigned char m_data[128]; };
    struct job_queue_entry_input {
        unsigned int m_job_thread_type;
        unsigned int m_job_priority;
        job_queue_entry m_queue_job;
    };
    struct job_queue_dynamic_data {
        __declspec(align(16)) int m_active_job_count;
        int m_cpu_threads_waiting;
        int m_coproc_threads_waiting;
        phys_queue<job_queue_entry> m_job_queue[4];
        __forceinline job_queue_dynamic_data() {}
        PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_DYNAMICS, job_queue_dynamic_data);
        ~job_queue_dynamic_data();
    };
    typedef job_pop_result (__cdecl *job_pop_fn)(phys_job_queue &, job_queue_dynamic_data *, job_queue_entry &, job_queue_entry &);
    typedef job_creation_status (__cdecl *job_finish_fn)(phys_job_queue &, job_queue_dynamic_data *, const job_queue_entry &, job_queue_entry_input &);

    phys_job_queue();
    ~phys_job_queue();
    job_queue_dynamic_data *lock_job_queue(char *job_queue_storage);
    void unlock_job_queue(char *job_queue_storage);
    void set_job_query_rules(query_rules rules);
    void release_waiting_worker(job_kind queue_job_kind, job_queue_dynamic_data *data);
    void set_queue_capacity(int capacity);
    void add_job(job_queue_entry &src_job, job_priority priority, job_kind queue_job_kind);
    __forceinline job_status find_next_job(worker_thread_kind queue_thread_type, job_queue_entry &next_job_out, job_queue_dynamic_data *data);
    __forceinline phys_bool all_queues_empty(job_queue_dynamic_data *data);
    __forceinline void release_one_waiting_thread(worker_thread_kind queue_thread_type, job_queue_dynamic_data *data);
    job_status fetch_next_job(worker_thread_kind queue_thread_type, job_queue_entry &src_job, job_wait_mode queue_wait_status);
    job_status add_job_get_next(job_kind queue_job_kind, job_priority priority, worker_thread_kind queue_thread_type,
        job_queue_entry &src_job, job_wait_mode queue_wait_status);
    job_status finish_job_get_next(worker_thread_kind queue_thread_type, const job_queue_entry *finished_job_entry,
        job_queue_entry &queue_next_job, void *thread, job_wait_mode queue_wait_status);
protected:
    void release_waiting_threads(job_queue_dynamic_data *data);
public:
    __declspec(align(64)) phys_critical_section m_lock;
    __declspec(align(64)) job_queue_dynamic_data *m_dynamic_data;
    struct query_rule {
        unsigned char m_job_type_count;
        phys_enum<job_kind, unsigned char> m_job_types[2];
    } m_query_rules[2];
    job_pop_fn m_job_pop_func;
    job_finish_fn m_finish_job_fn;
    void *m_user_data;
    __declspec(align(128)) phys_semaphore m_cpu_semaphore;
    __declspec(align(128)) phys_semaphore m_semaphore_100;
    __declspec(align(128)) phys_semaphore m_semaphore_180;
};

template <> void phys_queue<phys_job_queue::job_queue_entry>::set_capacity(int array_requested_capacity);
template <> void phys_queue<phys_job_queue::job_queue_entry>::enqueue_front(const phys_job_queue::job_queue_entry &entry);
template <> void phys_queue<phys_job_queue::job_queue_entry>::enqueue(const phys_job_queue::job_queue_entry &entry);

__forceinline phys_bool phys_job_queue::all_queues_empty(job_queue_dynamic_data *data)
{
    int queue_job_count = data->m_job_queue[0].getSize() + data->m_job_queue[1].getSize();
    return queue_job_count == 0;
}

__forceinline void phys_job_queue::release_one_waiting_thread(worker_thread_kind thread_type, job_queue_dynamic_data *data)
{
    if (data->m_cpu_threads_waiting != 0) {
        int queue_job_count = data->m_job_queue[0].getSize() + data->m_job_queue[1].getSize();
        if (queue_job_count != 0) {
            data->m_cpu_threads_waiting--;
            m_cpu_semaphore.release(1);
        }
    }
}

__forceinline phys_job_queue::job_status phys_job_queue::find_next_job(worker_thread_kind thread_type, job_queue_entry &found_job, job_queue_dynamic_data *data)
{
    query_rule &thread_rules = m_query_rules[thread_type];
    for (int i = 0; i < thread_rules.m_job_type_count; i++) {
        phys_queue<job_queue_entry> *queue = &data->m_job_queue[thread_rules.m_job_types[i]];
        if (!queue->is_array_empty()) {
            __declspec(align(16)) job_queue_entry src_job;
            queue->dequeue(src_job);
            if (m_job_pop_func(*this, data, src_job, found_job) == 1) queue->enqueue_front(src_job);
            data->m_active_job_count++;
            return (job_status)0;
        }
    }
    return (job_status)1;
}

typedef char phys_job_queue_data_size[(sizeof(void *) != 4 || ((sizeof(phys_job_queue::job_queue_dynamic_data) == 96) )) ? 1 : -1];
typedef char phys_job_queue_data_offset[(sizeof(void *) != 4 || ((offsetof(phys_job_queue, m_dynamic_data) == 64) )) ? 1 : -1];
typedef char phys_job_queue_user_offset[(sizeof(void *) != 4 || ((offsetof(phys_job_queue, m_user_data) == 84) )) ? 1 : -1];
typedef char phys_job_queue_cpu_semaphore_offset[(sizeof(void *) != 4 || ((offsetof(phys_job_queue, m_cpu_semaphore) == 128) )) ? 1 : -1];
typedef char phys_job_queue_semaphore100_offset[(sizeof(void *) != 4 || ((offsetof(phys_job_queue, m_semaphore_100) == 256) )) ? 1 : -1];
typedef char phys_job_queue_semaphore180_offset[(sizeof(void *) != 4 || ((offsetof(phys_job_queue, m_semaphore_180) == 384) )) ? 1 : -1];
typedef char phys_job_queue_size[(sizeof(void *) != 4 || ((sizeof(phys_job_queue) == 512) )) ? 1 : -1];

#endif
