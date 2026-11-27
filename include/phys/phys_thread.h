#ifndef INCLUDE_PHYS_THREAD_H
#define INCLUDE_PHYS_THREAD_H

enum phys_result;

// Thread wrapper holding the platform handle and the child thread's ID.
class phys_thread {
public:
    enum Status { THREAD_STATUS_TERMINATED, THREAD_STATUS_RUNNING, THREAD_STATUS_NOT_STARTED };
    phys_thread();
    ~phys_thread();
    void stop_thread();
    phys_result worker_start_thread(void *(*function)(void *), void *argument);
    Status get_status();
    static unsigned __int64 get_current_thread_id();
    unsigned __int64 get_child_thread_id();
    void *get_thread_handle();

protected:
    void *m_worker_thread;
    unsigned __int64 m_child_thread_id;
};

typedef char havok_size_hkThread[(sizeof(phys_thread) == 0x10) ? 1 : -1];

#endif // INCLUDE_HAVOK_HK_THREAD_H
