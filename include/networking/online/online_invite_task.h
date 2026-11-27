#ifndef NETWORKING_ONLINE_ONLINE_INVITE_TASK_H
#define NETWORKING_ONLINE_ONLINE_INVITE_TASK_H

#include "cseries/async_xoverlapped_types.h"
#include "shell/shell.h"
#include <stddef.h>

class c_invite_task : public c_overlapped_task
{
public:
    c_invite_task();
    __forceinline virtual ~c_invite_task() {}
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    e_controller_index m_controller_index;
    unsigned __int64 m_target_xuid;
    wchar_t m_message_text[256];
};

typedef char invite_task_controller_offset[(sizeof(void *) != 4 || ((offsetof(c_invite_task, m_controller_index) == 8))) ? 1 : -1];
typedef char invite_task_target_offset[(sizeof(void *) != 4 || ((offsetof(c_invite_task, m_target_xuid) == 16))) ? 1 : -1];
typedef char invite_task_message_offset[(sizeof(void *) != 4 || ((offsetof(c_invite_task, m_message_text) == 24))) ? 1 : -1];
#endif
