#ifndef MAIN_THREAD_GLOBALS_LOCK_H
#define MAIN_THREAD_GLOBALS_LOCK_H

// match: 0x81500, 0x19f488, 0x197c70: preserve the accepted inline constructor path.
enum e_inline_thread_globals_lock { _inline_thread_globals_lock };

class c_thread_globals_lock
{
public:
    c_thread_globals_lock();
    __forceinline c_thread_globals_lock(e_inline_thread_globals_lock);
    ~c_thread_globals_lock();

private:
    bool m_acquired;
};

#endif // MAIN_THREAD_GLOBALS_LOCK_H
