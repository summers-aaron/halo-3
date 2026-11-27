#ifndef MULTITHREADING_CRITICAL_SECTION_SCOPE_H
#define MULTITHREADING_CRITICAL_SECTION_SCOPE_H

#include "multithreading/synchronization.h"

void internal_critical_section_leave(long index);

class c_critical_section_scope
{
public:
    c_critical_section_scope(long critical_section, long timeout_in_milliseconds, bool *out_lock_acquired);
    void release();
    __forceinline c_critical_section_scope(long critical_section)
    {
        m_critical_section = critical_section;
        internal_critical_section_enter(critical_section);
        m_locked = true;
    }
    __forceinline ~c_critical_section_scope()
    {
        if (m_locked)
            internal_critical_section_leave(m_critical_section);
    }

    long m_critical_section;
    bool m_locked;
};

#endif // MULTITHREADING_CRITICAL_SECTION_SCOPE_H
