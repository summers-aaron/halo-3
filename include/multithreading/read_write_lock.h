#ifndef MULTITHREADING_READ_WRITE_LOCK_H
#define MULTITHREADING_READ_WRITE_LOCK_H

#include "cache/critical_sections_types.h"
#include "cache/synchronization_semaphore_types.h"
#include "multithreading/semaphore_definitions.h"
#include "multithreading/synchronization.h"

// March read_write_lock.cpp: section, semaphore and maximum signal count.
class c_read_write_lock
{
public:
    __forceinline c_read_write_lock() : m_section(section_none), m_semaphore(semaphore_none), m_max_count(-1) {}
    __forceinline ~c_read_write_lock()
    {
        m_section = section_none;
        m_semaphore = semaphore_none;
        m_max_count = -1;
    }

    void setup(e_critical_sections section, e_synchronization_semaphore semaphore);
    __forceinline void read_lock() { internal_semaphore_take(m_semaphore); }
    __forceinline void read_unlock() { internal_semaphore_release(m_semaphore); }
    void write_lock();
    void write_unlock();

private:
    e_critical_sections m_section;
    e_synchronization_semaphore m_semaphore;
    long m_max_count;
};

typedef char read_write_lock_size[(sizeof(long) != 4 || (sizeof(c_read_write_lock) == 12)) ? 1 : -1];

#endif // MULTITHREADING_READ_WRITE_LOCK_H
