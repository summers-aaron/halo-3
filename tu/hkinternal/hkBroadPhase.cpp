// hkinternal/hkBroadPhase.obj: from the records 0x6a5360 0x6a53c0 0x6a53d8 0x6a54a0 0x6a5508
// @compiler xdk5632
// @flags /TP /O2
// @address 0x6a5508 ??1phys_broadphase@@UAA@XZ
// @address 0x6a5360 ??0phys_broadphase@@QAA@XZ
// @address 0x6a53c0 ?unlock_impl@phys_broadphase@@IAAXXZ
#include <xtl.h>
#include "phys/phys_broadphase.h"
#include "phys/phys_critical_section.h"

phys_broadphase::phys_broadphase()
{
    m_display_lock = 0;
    m_mt_lock.disable_checks();
}

void phys_broadphase::unlock_impl()
{
    m_display_lock->leave();
}

// The lock counterpart of unlockImplementation: hkCriticalSection::enter on the broad phase's section.
// @address 0x6a54a0 broadphase_try_lock
extern "C" void broadphase_try_lock(phys_broadphase *phase)
{
    phase->m_display_lock->enter();
}

phys_broadphase::~phys_broadphase()
{
    if (m_display_lock != 0)
    {
        delete m_display_lock;
        m_display_lock = 0;
    }
}

// @address 0x6a53d8 ?enable_multithreading@phys_broadphase@@QAAXH@Z
void phys_broadphase::enable_multithreading(int critical_section_spin)
{
    if (!m_display_lock)
    {
        m_display_lock = new phys_critical_section(critical_section_spin);
        m_mt_lock.enable_checks();
    }
}

