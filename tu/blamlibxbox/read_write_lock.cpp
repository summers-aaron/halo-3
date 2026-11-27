// blamlibxbox/read_write_lock.obj: from the records 0x21c560 0x21c5d8 0x21c690
// @flags /TP /O2 /Oc
// @address 0x21c560 ?setup@c_read_write_lock@@QAAXW4e_critical_sections@@W4e_synchronization_semaphore@@@Z
// @address 0x21c5d8 ?write_lock@c_read_write_lock@@QAAXXZ
// @address 0x21c690 ?write_unlock@c_read_write_lock@@QAAXXZ
#include "multithreading/read_write_lock.h"
#include "multithreading/synchronization.h"
#include "multithreading/synchronization_types.h"

static __forceinline void read_write_lock_leave_section(long section)
{
    if (section != -1)
    {
        _InterlockedDecrement(&g_synch_globals.critical_sections[section].lock_count.value);
        RtlLeaveCriticalSection(&g_synch_globals.critical_sections[section].critical_section);
    }
}

void c_read_write_lock::setup(e_critical_sections section, e_synchronization_semaphore semaphore)
{
    m_max_count = sempahore_get_max_signal_count(semaphore);
    m_section = section;
    m_semaphore = semaphore;
    for (long index = 0; index < m_max_count; ++index)
        internal_semaphore_release(m_semaphore);
}

void c_read_write_lock::write_lock()
{
    if (m_section != -1)
        internal_critical_section_enter(m_section);
    for (long index = 0; index < m_max_count; ++index)
        internal_semaphore_take(m_semaphore);
    read_write_lock_leave_section(m_section);
}

void c_read_write_lock::write_unlock()
{
    for (long index = 0; index < m_max_count; ++index)
        internal_semaphore_release(m_semaphore);
}
