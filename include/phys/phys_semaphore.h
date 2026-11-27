#ifndef INCLUDE_PHYS_SEMAPHORE_H
#define INCLUDE_PHYS_SEMAPHORE_H

// hkSemaphore, defined in tu/hkbase/hkWin32Semaphore.cpp (the handle is a kernel semaphore).

class phys_semaphore
{
public:
    phys_semaphore(int semaphore_initial_count = 0, int maxCount = 1000);
    ~phys_semaphore();
    void acquire_semaphore();
    void release(int count = 1);

protected:
    void *m_semaphore;
};

#endif // HAVOK_HK_SEMAPHORE_H
