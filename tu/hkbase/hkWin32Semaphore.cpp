// OBJ: hkbase/hkWin32Semaphore.obj from the records 0x60c4e8 0x60c528 0x60c530 0x60c540
// @compiler xdk6130
// @flags /TP /O1
// @address 0x60c4e8 ??0phys_semaphore@@QAA@HH@Z
// @address 0x60c528 ??1phys_semaphore@@QAA@XZ
// @address 0x60c530 ?acquire_semaphore@phys_semaphore@@QAAXXZ
// @address 0x60c540 ?release@phys_semaphore@@QAAXH@Z
#include <xtl.h>

#include "phys/phys_semaphore.h"

phys_semaphore::phys_semaphore(int semaphore_initial_count, int sem_max_cnt)
{
    m_semaphore = CreateSemaphoreA(0, semaphore_initial_count, sem_max_cnt, 0);
}

phys_semaphore::~phys_semaphore()
{
    CloseHandle(m_semaphore);
}

void phys_semaphore::acquire_semaphore()
{
    WaitForSingleObject(m_semaphore, INFINITE);
}

void phys_semaphore::release(int count)
{
    ReleaseSemaphore(m_semaphore, count, 0);
}

