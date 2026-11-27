// hkbase/hkJobQueue.obj: from the records 0x700d08 0x700d20 0x700d80 0x700da8 0x700e18 0x700e50 0x700ed8 0x701088 0x701150 0x701680 0x7016e0 0x7016f8 0x701750 0x701410
// @flags /TP /O2
// @address 0x700d08 ?unlock_job_queue@phys_job_queue@@QAAXPAD@Z
// @address 0x700d20 ?release_waiting_threads@phys_job_queue@@IAAXPAUjob_queue_dynamic_data@1@@Z
// @address 0x700d80 ?set_job_query_rules@phys_job_queue@@QAAXW4query_rules@1@@Z
// @address 0x700da8 ?lock_job_queue@phys_job_queue@@QAAPAUjob_queue_dynamic_data@1@PAD@Z
// @address 0x700e18 ?release_waiting_worker@phys_job_queue@@QAAXW4job_kind@1@PAUjob_queue_dynamic_data@1@@Z
// @address 0x700e50 ?set_queue_capacity@phys_job_queue@@QAAXH@Z
// @address 0x701088 ?add_job@phys_job_queue@@QAAXAAUjob_queue_entry@1@W4job_priority@1@W4job_kind@1@@Z
// @address 0x701150 ?finish_job_get_next@phys_job_queue@@QAA?AW4job_status@1@W4worker_thread_kind@1@PBUjob_queue_entry@1@AAU41@PAXW4job_wait_mode@1@@Z
// @address 0x7016e0 ?fetch_next_job@phys_job_queue@@QAA?AW4job_status@1@W4worker_thread_kind@1@AAUjob_queue_entry@1@W4job_wait_mode@1@@Z
// @address 0x7016f8 ??1job_queue_dynamic_data@phys_job_queue@@QAA@XZ
#include <xtl.h>
#include "physics/havok_memory.h"
#include "phys/phys_thread_memory.h"
#include "phys/phys_critical_section.h"
#include "phys/phys_semaphore.h"
extern "C" {
}
#include "phys/phys_job_queue.h"
void phys_job_queue::unlock_job_queue(char *queue_lock_buf)
{
  m_lock.leave();
}
void phys_job_queue::release_waiting_threads(job_queue_dynamic_data *data)
{
  int waiting_cpu_threads = data->m_cpu_threads_waiting;
  data->m_cpu_threads_waiting = 0;
  for (; waiting_cpu_threads > 0; --waiting_cpu_threads) {
    m_cpu_semaphore.release(1);
  }
}
void phys_job_queue::set_job_query_rules(query_rules rules)
{
  rules = JOB_QUERY_RULES_CPU;
  if (rules == JOB_QUERY_RULES_CPU) {
    m_query_rules[0].m_job_types[0] = JOB_KIND_HOST;
    m_query_rules[0].m_job_types[1] = JOB_KIND_OFFLOAD;
    m_query_rules[0].m_job_type_count = 2;
    m_query_rules[1].m_job_types[0] = JOB_KIND_OFFLOAD;
    m_query_rules[1].m_job_types[1] = JOB_KIND_HOST;
    m_query_rules[1].m_job_type_count = 2;
  }
}
phys_job_queue::job_queue_dynamic_data *phys_job_queue::lock_job_queue(char *queue_lock_buf)
{
  m_lock.enter();
  return m_dynamic_data;
}
void phys_job_queue::release_waiting_worker(job_kind waiting_job_kind, job_queue_dynamic_data *data)
{
  if (data->m_cpu_threads_waiting != 0) {
    int job_count = data->m_job_queue[0].getSize() + data->m_job_queue[1].getSize();
    if (job_count != 0) {
      --data->m_cpu_threads_waiting;
      m_cpu_semaphore.release(1);
    }
  }
}
void phys_job_queue::set_queue_capacity(int capacity)
{
  __declspec(align(16)) char queue_lock_buf[128];
  lock_job_queue(queue_lock_buf);
  m_dynamic_data->m_job_queue[1].set_capacity(capacity);
  m_dynamic_data->m_job_queue[0].set_capacity(capacity);
  unlock_job_queue(queue_lock_buf);
}
void phys_job_queue::add_job(job_queue_entry &src_job, job_priority priority, job_kind waiting_job_kind)
{
  __declspec(align(16)) char queue_lock_buf[128];
  job_queue_dynamic_data *data = lock_job_queue(queue_lock_buf);
  if (priority == JOB_PRIORITY_HIGH) {
    data->m_job_queue[waiting_job_kind].enqueue_front(src_job);
  } else {
    data->m_job_queue[waiting_job_kind].enqueue(src_job);
  }
  release_waiting_worker(waiting_job_kind, data);
  unlock_job_queue(queue_lock_buf);
}
phys_job_queue::job_status phys_job_queue::fetch_next_job(worker_thread_kind thread_type, job_queue_entry &src_job, job_wait_mode job_wait_status)
{
  return finish_job_get_next(thread_type, 0, src_job, 0, job_wait_status);
}
phys_job_queue::job_status phys_job_queue::finish_job_get_next(worker_thread_kind thread_type, const job_queue_entry *done_job,
  job_queue_entry &next_job_entry, void *params, job_wait_mode job_wait_status)
{
  for (;;) {
    __declspec(align(16)) char queue_lock_buf[128];
    job_queue_dynamic_data *data = lock_job_queue(queue_lock_buf);
    job_queue_entry_input new_job_entry;
    if (done_job) {
      if (m_finish_job_fn(*this, data, *done_job, new_job_entry) == 0) {
        if (new_job_entry.m_job_priority == 0)
          data->m_job_queue[new_job_entry.m_job_thread_type].enqueue_front(new_job_entry.m_queue_job);
        else
          data->m_job_queue[new_job_entry.m_job_thread_type].enqueue(new_job_entry.m_queue_job);
      }
      if (thread_type == (worker_thread_kind)-1) {
        release_one_waiting_thread((worker_thread_kind)new_job_entry.m_job_thread_type, data);
        unlock_job_queue(queue_lock_buf);
        return (job_status)1;
      }
      data->m_active_job_count--;
      done_job = 0;
    }
    if (find_next_job(thread_type, next_job_entry, data) == (job_status)0) {
      release_one_waiting_thread(thread_type, data);
      unlock_job_queue(queue_lock_buf);
      return (job_status)0;
    }
    if (data->m_active_job_count == 0 && all_queues_empty(data)) {
      release_waiting_threads(data);
      unlock_job_queue(queue_lock_buf);
      return (job_status)3;
    }
    if (job_wait_status == (job_wait_mode)1) {
      unlock_job_queue(queue_lock_buf);
      return (job_status)1;
    }
    data->m_cpu_threads_waiting++;
    unlock_job_queue(queue_lock_buf);
    m_cpu_semaphore.acquire_semaphore();
  }
}
phys_job_queue::job_queue_dynamic_data::~job_queue_dynamic_data()
{
}

// @address 0x701680 ??1phys_job_queue@@QAA@XZ
phys_job_queue::~phys_job_queue()
{
  if (m_dynamic_data) delete m_dynamic_data;
}
// @address 0x701750 ?enqueue@?$phys_queue@Ujob_queue_entry@phys_job_queue@@@@QAAXABUjob_queue_entry@phys_job_queue@@@Z
template <>
void phys_queue<phys_job_queue::job_queue_entry>::enqueue(const phys_job_queue::job_queue_entry &entry)
{
  if (m_used_count >= m_small_capacity) grow_capacity();
  if (m_tail == m_small_capacity) m_tail = 0;
  m_data[m_tail++] = entry;
  m_used_count++;
}
// @address 0x700ed8 ??0phys_job_queue@@QAA@XZ
phys_job_queue::phys_job_queue()
  : m_lock(0), m_cpu_semaphore(0, 1000), m_semaphore_100(0, 1000), m_semaphore_180(0, 1000)
{
  m_dynamic_data = new job_queue_dynamic_data();
  m_dynamic_data->m_cpu_threads_waiting = 0;
  m_dynamic_data->m_coproc_threads_waiting = 0;
  m_dynamic_data->m_active_job_count = 0;
  m_dynamic_data->m_job_queue[0].set_capacity(128);
  m_dynamic_data->m_job_queue[1].set_capacity(128);
  m_dynamic_data->m_job_queue[2].set_capacity(128);
  m_dynamic_data->m_job_queue[3].set_capacity(128);
  m_query_rules[0].m_job_types[0] = JOB_KIND_HOST;
  m_query_rules[0].m_job_types[1] = JOB_KIND_OFFLOAD;
  m_query_rules[0].m_job_type_count = 2;
  m_query_rules[1].m_job_types[0] = JOB_KIND_OFFLOAD;
  m_query_rules[1].m_job_types[1] = JOB_KIND_HOST;
  m_query_rules[1].m_job_type_count = 2;
}
// @address 0x701410 ?add_job_get_next@phys_job_queue@@QAA?AW4job_status@1@W4job_kind@1@W4job_priority@1@W4worker_thread_kind@1@AAUjob_queue_entry@1@W4job_wait_mode@1@@Z
phys_job_queue::job_status phys_job_queue::add_job_get_next(job_kind job_thread_kind, job_priority priority, worker_thread_kind thread_type,
  job_queue_entry &job_slot_io, job_wait_mode job_wait_status)
{
  bool first_pass = true;
  for (;;) {
    __declspec(align(16)) char queue_lock_buf[128];
    job_queue_dynamic_data *data = lock_job_queue(queue_lock_buf);
    if (first_pass) {
      if (priority == JOB_PRIORITY_HIGH)
        data->m_job_queue[job_thread_kind].enqueue_front(job_slot_io);
      else
        data->m_job_queue[job_thread_kind].enqueue(job_slot_io);
      data->m_active_job_count--;
    }
    job_status result = find_next_job(thread_type, job_slot_io, data);
    first_pass = false;
    if (result == (job_status)0) {
      release_one_waiting_thread(thread_type, data);
      unlock_job_queue(queue_lock_buf);
      return (job_status)0;
    }
    if (data->m_active_job_count == 0 && all_queues_empty(data)) {
      release_waiting_threads(data);
      unlock_job_queue(queue_lock_buf);
      return (job_status)3;
    }
    if (job_wait_status == (job_wait_mode)1) {
      unlock_job_queue(queue_lock_buf);
      return (job_status)1;
    }
    data->m_cpu_threads_waiting++;
    unlock_job_queue(queue_lock_buf);
    m_cpu_semaphore.acquire_semaphore();
  }
}
