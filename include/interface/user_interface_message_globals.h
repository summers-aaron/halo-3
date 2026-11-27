#ifndef INTERFACE_USER_INTERFACE_MESSAGE_GLOBALS_H
#define INTERFACE_USER_INTERFACE_MESSAGE_GLOBALS_H

#include <string.h>
#include <stddef.h>
#include "cseries/cseries.h"
#include "interface/user_interface_messages.h"

class c_message_globals
{
public:
    struct s_message_queue_node
    {
        s_message_queue_node *m_next;
        s_message_queue_node *m_previous;
        const c_message *m_message;
    };
    c_message_globals()
    {
        memset(m_queue, 0, sizeof(m_queue));
        m_next_read = 0;
        m_next_write = 0;
        flag = false;
        notification_handle = 0;
    }
    ~c_message_globals() {}
    __forceinline void initialize_for_new_map()
    {
        for (long index = 0; index < NUMBEROF(m_queue); index++)
        {
            m_queue[index].m_next = index == NUMBEROF(m_queue) - 1 ? &m_queue[0] : &m_queue[index + 1];
            m_queue[index].m_previous = index == 0 ? &m_queue[NUMBEROF(m_queue) - 1] : &m_queue[index - 1];
            m_queue[index].m_message = 0;
        }
        m_next_write = m_queue;
        m_next_read = m_queue;
    }
    const c_message *dequeue_node(s_message_queue_node *node, bool unlink);
    void get_next_message(long screen_name, e_controller_index controller, e_window_index window, const c_message **message_reference);
    __forceinline void dequeue(const c_message *message);
    __forceinline bool can_read() { return m_next_read->m_message != 0; }
    __forceinline bool message_match(const c_message *message, long screen_name, e_controller_index controller, e_window_index window)
    {
        bool result = false;
        if ((screen_name == -1 || message->get_screen_name() == screen_name)
            && (controller == k_any_controller || message->get_controller() == controller)
            && (window == k_any_window || window == _window_none || message->get_window() == window))
            result = true;
        return result;
    }
    s_message_queue_node m_queue[16];
    s_message_queue_node *m_next_read;
    s_message_queue_node *m_next_write;
    void *notification_handle;
    bool flag;
};

typedef char message_globals_next_write_offset[(sizeof(void *) != 4 || ((offsetof(c_message_globals, m_next_write) == 0xc4))) ? 1 : -1];

#endif
