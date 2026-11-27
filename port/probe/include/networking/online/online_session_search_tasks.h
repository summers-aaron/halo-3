#ifndef NETWORKING_ONLINE_ONLINE_SESSION_SEARCH_TASKS_H
#define NETWORKING_ONLINE_ONLINE_SESSION_SEARCH_TASKS_H

#include "cseries/async_xoverlapped_types.h"
#include <xtl.h>
#include "networking/online/online_search_parameters.h"
#include "networking/transport/transport_security_types.h"
#include "shell/shell_types.h"
#include <string.h>

struct s_online_search_query_internal;

#pragma pack(push, 4)
class c_session_search_overlapped_task : public c_overlapped_task
{
public:
    __forceinline c_session_search_overlapped_task(const char *file, long line) : c_overlapped_task(file, line) {}
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    void initialize(const s_online_search_query_internal *parameters);
    const void *get_results_buffer() const { return m_results_buffer; }
private:
    unsigned char m_results_buffer[0x11000];
    unsigned long m_results_size;
    unsigned long m_reserved;
    unsigned long m_procedure;
    unsigned long m_user_index;
    unsigned long m_context_count;
    s_online_context m_contexts[6];
    unsigned long m_property_count;
    XUSER_PROPERTY m_properties[20];
};
class c_session_search_by_id_overlapped_task : public c_overlapped_task
{
public:
    __forceinline c_session_search_by_id_overlapped_task(const char *file, long line) : c_overlapped_task(file, line) {}
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    const void *get_results_buffer() const { return m_results_buffer; }
    // March initialize: release members retain the same order after the 8-byte base.
    __forceinline void initialize(const s_transport_secure_identifier *session_id, e_controller_index controller_index)
    {
        memcpy(&m_session_id, session_id, sizeof(m_session_id));
        m_user_index = controller_index;
        m_results_size = 0;
    }
private:
    unsigned char m_results_buffer[0x11000];
    unsigned long m_results_size;
    XNKID m_session_id;
    unsigned long m_user_index;
};
#pragma pack(pop)

#endif
