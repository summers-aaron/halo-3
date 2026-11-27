#ifndef INCLUDE_PHYS_CRITICAL_SECTION_H
#define INCLUDE_PHYS_CRITICAL_SECTION_H

#include <stddef.h>

#include "phys/phys_thread_memory.h"
#include "phys/phys_thread.h"
#include "phys/phys_semaphore.h"

class phys_semaphore;

class phys_critical_section_base {
protected:
    phys_critical_section_base(int spinCount);
    // match: hkWorld 0x642058 inlines two lock constructions; the int overload stays out of line.
    __forceinline phys_critical_section_base(long spinCount)
    {
        s_list_semaphore.acquire_semaphore();
        m_spin_count = spinCount;
        m_prev_link = 0;
        m_next = s_list_head;
        if (s_list_head != 0) s_list_head->m_prev_link = this;
        s_list_head = this;
        s_list_semaphore.release(1);
    }
    ~phys_critical_section_base();
    int m_spin_count;
    phys_critical_section_base *m_prev_link;
    phys_critical_section_base *m_next;
public:
    static phys_critical_section_base *s_list_head;
    static phys_semaphore s_list_semaphore;
};

// A lock built on a system critical section and the owning thread id.
class phys_critical_section : public phys_critical_section_base {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_BASE_CLASS, phys_critical_section);

    // hkCriticalSectionBase(spinCount), RtlInitializeCriticalSectionAndSpinCount, then m_currentThread = -1.
    phys_critical_section(int spinCount = 0);
    // match: retain the accepted hkWorld constructor's inline lock sites at 0x642058.
    __forceinline phys_critical_section(long spinCount) : phys_critical_section_base(spinCount)
    {
        RtlInitializeCriticalSectionAndSpinCount(&m_section, spinCount);
        m_current_thread = (unsigned __int64)-1;
    }
    inline ~phys_critical_section() {}

    inline void enter() {
        if (!RtlTryEnterCriticalSection(&m_section)) {
            if (m_time_locks.get_chain_data()) {
                RtlEnterCriticalSection(&m_section);
            } else {
                RtlEnterCriticalSection(&m_section);
            }
        }
        m_current_thread = phys_thread::get_current_thread_id();
    }
    inline void leave() {
        m_current_thread = static_cast<unsigned __int64>(-1);
        RtlLeaveCriticalSection(&m_section);
    }
    inline bool owned_by_this_thread() { return m_current_thread == phys_thread::get_current_thread_id(); }
    inline bool is_entered() const { return m_current_thread != static_cast<unsigned __int64>(-1); }

    static inline void enable_lock_timers() { m_time_locks.lock_set_data(1); }
    static inline void disable_lock_timers() { m_time_locks.lock_set_data(0); }
    static __forceinline unsigned int atomic_exchange_add(unsigned int *var, int increment) {
        return _InterlockedExchangeAdd(reinterpret_cast<long volatile *>(var), increment);
    }
    static phys_thread_local_data<int> m_time_locks;

    RTL_CRITICAL_SECTION m_section;
    unsigned __int64 m_current_thread;
};

typedef char havok_size_hkCriticalSectionBase[(sizeof(void *) != 4 || ((sizeof(phys_critical_section_base) == 0x0c) )) ? 1 : -1];
typedef char havok_offset_hkCriticalSection_m_section[(sizeof(void *) != 4 || ((offsetof(phys_critical_section, m_section) == 0x0c) )) ? 1 : -1];
typedef char havok_offset_hkCriticalSection_m_currentThread[(sizeof(void *) != 4 || ((offsetof(phys_critical_section, m_current_thread) == 0x28) )) ? 1 : -1];
typedef char havok_size_hkCriticalSection[(sizeof(void *) != 4 || ((sizeof(phys_critical_section) == 0x30) )) ? 1 : -1];

class phys_critical_section_lock {
public:
    __forceinline phys_critical_section_lock(phys_critical_section *section) : m_section(section) { m_section->enter(); }
    __forceinline ~phys_critical_section_lock() { m_section->leave(); }
    phys_critical_section *m_section;
};

#endif // INCLUDE_HAVOK_HK_CRITICAL_SECTION_H
