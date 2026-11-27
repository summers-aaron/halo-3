#ifndef NETWORKING_ONLINE_ONLINE_LSP_SEARCH_TASK_H
#define NETWORKING_ONLINE_ONLINE_LSP_SEARCH_TASK_H

#include "cseries/async_xoverlapped_types.h"
#include "networking/online/xtitle_server_info_types.h"

class c_lsp_search_task : public c_overlapped_task {
public:
    __forceinline c_lsp_search_task(const char *file, long line) :
        c_overlapped_task(file, line), m_enumeration_handle((void *)-1), m_overlapped_results_buffer(0), m_server_address_count(0)
    {
    }
    virtual const char *get_context_string() const;
    virtual unsigned long start(void *overlapped);
    virtual void success(unsigned long count);
    virtual void complete();
    virtual void reset();
    __forceinline long get_found_server_count() const { return m_server_address_count; }
    __forceinline XTITLE_SERVER_INFO *get_found_server(long server_index) { return &m_server_addresses[server_index]; }
private:
    void *m_enumeration_handle;
    XTITLE_SERVER_INFO m_server_addresses[16];
    XTITLE_SERVER_INFO *m_overlapped_results_buffer;
    unsigned long m_server_address_count;
};


#endif
