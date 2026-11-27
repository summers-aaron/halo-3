#ifndef TAG_FILES_TAG_RESOURCE_CACHE_PAGING_H
#define TAG_FILES_TAG_RESOURCE_CACHE_PAGING_H

#include "cache/tag_resource_paging_task.h"

// Functions defined in tu/blamlibxbox/tag_resource_cache_paging.cpp.

class c_tag_resource_cache_new;

class c_tag_resource_error_receiver
{
public:
    __forceinline c_tag_resource_error_receiver()
        : m_reserved(false), m_page_failed(false), m_read_failed(false), m_hash_failed(false), finished(false)
    {
    }

    __forceinline ~c_tag_resource_error_receiver() {}

    virtual void report_page_failure();
    virtual void report_read_failure();
    virtual void report_hash_failure();

    __forceinline bool succeeded() const { return !m_read_failed && !m_page_failed && !m_hash_failed; }

private:
    bool m_reserved;
    bool m_page_failed;
    bool m_read_failed;
    bool m_hash_failed;

public:
    bool finished;
};
class c_tag_resource_controller_page_table_io_listener
{
public:
    c_tag_resource_cache_new *m_owner;
};
class c_tag_resource_page_table;
struct s_tag_resource_paging_partition
{
    unsigned __int64 *first;
    unsigned __int64 *second;
    s_tag_resource_paging_partition(unsigned __int64 *a, unsigned __int64 *b) : first(a), second(b) {}
};
// match: 0x2ad420: noalias preserves caller live registers.
extern "C" __declspec(noalias) s_tag_resource_paging_partition tag_resource_partition_priorities(unsigned __int64 *first, unsigned __int64 *last);
class c_tag_resource_page_table_control_interface;
class c_tag_resource_paging_page_filter;
class c_tag_resource_cache_controller;

extern "C" void tag_resource_paging_controller_reset(c_tag_resource_cache_controller *controller,
    c_tag_resource_page_table_control_interface *control_interface);
extern "C" unsigned char tag_resource_cache_is_loaded(c_tag_resource_page_table *page_table);
extern "C" void tag_resource_page_table_dispose_minimum_size(c_tag_resource_page_table *table, unsigned int minimum_size);
extern "C" void tag_resource_page_table_compact_pages(c_tag_resource_page_table *table);
extern "C" void tag_resource_page_table_compute_resize_sizes(c_tag_resource_page_table *table,
    unsigned long *out_necessary_size,
    unsigned long *out_reading_size);
extern "C" void tag_resource_page_table_update_required_pages(c_tag_resource_page_table *table, c_tag_resource_paging_page_filter *filter);
extern "C" void tag_resource_page_table_dispose_reserved_pages(c_tag_resource_page_table *table);
extern "C" void tag_resource_page_table_update_next_page(c_tag_resource_page_table *table, c_tag_resource_controller_page_table_io_listener *listener);

class c_tag_resource_page_datum;
struct s_tag_resource_page_hash_pair;
struct s_tag_resource_paging_fiber;
struct tag_resource_cache_paging_decoder_state;

extern "C" void tag_resource_page_handle_lruv_delete(void *context, long handle, bool release);
extern "C" unsigned char tag_resource_page_datum_is_unavailable(const c_tag_resource_page_datum *page);
extern "C" int tag_resource_page_compute_hash(const char *data, unsigned long size, bool salted, s_tag_resource_page_hash_pair *pair);
extern "C" void tag_resource_page_table_dispose_page(c_tag_resource_page_table *table, unsigned int handle);
extern "C" void tag_resource_page_handle_lruv_move(void *context, long handle, unsigned long old_page_index, unsigned long new_page_index);
extern "C" void tag_resource_paging_fiber_dispose(s_tag_resource_paging_fiber *context);
extern "C" int tag_resource_cache_paging_decoder_get_byte(tag_resource_cache_paging_decoder_state *state);
extern "C" bool tag_resource_page_table_allocate_page(c_tag_resource_page_table *table,
    long handle,
    c_tag_resource_controller_page_table_io_listener *listener);
extern "C" void tag_resource_cache_paging_decoder_decode_huffman_window(tag_resource_cache_paging_decoder_state *state);
extern "C" void tag_resource_cache_paging_decoder_decode_huffman_output(tag_resource_cache_paging_decoder_state *state);
extern "C" void __stdcall tag_resource_cache_paging_decoder_handle_fiber(void *decoder_state);
extern "C" void tag_resource_cache_paging_decoder_initialize_literal_table(tag_resource_cache_paging_decoder_state *state, int *a, int *b, int *c);
extern "C" void tag_resource_cache_paging_decoder_decode_stream(tag_resource_cache_paging_decoder_state *state);

template<typename> class c_basic_buffer;
class c_tag_resource_page_datum;

extern "C" void tag_resource_page_handle_pending_flags(c_tag_resource_error_receiver *pending_flag_receiver, c_tag_resource_page_datum page_datum);
extern "C" void tag_resource_page_table_new_page(c_tag_resource_page_table *table, long handle, c_basic_buffer<void> buffer, bool verify,
    c_tag_resource_controller_page_table_io_listener *listener);
extern c_tag_resource_paging_secondary_task g_tag_resource_paging_secondary_context;
extern "C" void *tag_resource_paging_get_task_context(void);
extern "C" void *tag_resource_paging_get_secondary_context(void);

#endif // TAG_FILES_TAG_RESOURCE_CACHE_PAGING_H
