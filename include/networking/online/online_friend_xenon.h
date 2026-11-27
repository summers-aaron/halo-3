#ifndef NETWORKING_ONLINE_ONLINE_FRIEND_XENON_H
#define NETWORKING_ONLINE_ONLINE_FRIEND_XENON_H

#include <xtl.h>
#include "shell/shell_types.h"
#include "cseries/async_xoverlapped_types.h"

// Overlapped task that looks up the Xbox Live friends list for a controller.
class c_online_find_friend_task : public c_overlapped_task
{
public:
    __forceinline c_online_find_friend_task()
        : c_overlapped_task(0, 0)
    {
        m_controller_index = _controller_index_none;
        m_enumeration_handle = (void *)-1;
        memset(m_friends, 0, sizeof(m_friends));
        m_friend_count = 0;
    }
    __forceinline c_online_find_friend_task(const char *file, long line, e_controller_index controller_index, unsigned __int64 xuid)
        : c_overlapped_task(file, line)
    {
        m_controller_index = controller_index;
        m_enumeration_handle = (void *)-1;
        memset(m_friends, 0, sizeof(m_friends));
        m_friend_count = 0;
    }
    __forceinline c_online_find_friend_task &operator=(const c_online_find_friend_task &other)
    {
        c_overlapped_task::operator=(other);
        m_controller_index = other.m_controller_index;
        m_enumeration_handle = other.m_enumeration_handle;
        for (unsigned long friend_index = 0; friend_index < 100; ++friend_index)
            m_friends[friend_index] = other.m_friends[friend_index];
        m_friend_count = other.m_friend_count;
        return *this;
    }
    unsigned long get_friend_count() const { return m_friend_count; }
    const XONLINE_FRIEND *get_friend(long index) const { return &m_friends[index]; }
    e_controller_index get_controller_index() const { return m_controller_index; }
    virtual ~c_online_find_friend_task();
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    virtual void success(unsigned long friend_count);
    virtual void complete();
    virtual void reset();
private:
    // Close the enumeration handle if it is open.
    __forceinline void cleanup()
    {
        if (m_enumeration_handle != 0 && m_enumeration_handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_enumeration_handle);
            m_enumeration_handle = INVALID_HANDLE_VALUE;
        }
    }
    e_controller_index m_controller_index;
    HANDLE m_enumeration_handle;
    XONLINE_FRIEND m_friends[100];
    unsigned long m_friend_count;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char online_find_friend_task_size[(sizeof(void *) != 4 || (sizeof(c_online_find_friend_task) == 0x4ca4)) ? 1 : -1];
#endif

#endif // NETWORKING_ONLINE_ONLINE_FRIEND_XENON_H
