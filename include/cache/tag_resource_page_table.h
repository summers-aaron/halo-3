#ifndef CACHE_TAG_RESOURCE_PAGE_TABLE_H
#define CACHE_TAG_RESOURCE_PAGE_TABLE_H

#include "memory/data.h"
#include "memory/datum_index.h"
#include "memory/basic_buffer.h"
#include "memory/wrapped_flags.h"
#include "cache/tag_resource_page_datum.h"
#include "cache/tag_resource_page_reservation_types.h"

extern "C" void tag_resource_page_handle_lruv_delete(void *context, long handle, bool release);
extern "C" void tag_resource_page_handle_lruv_move(void *context, long handle, unsigned long old_page_index, unsigned long new_page_index);
class c_tag_resource_page_table;
class c_tag_resource_error_receiver;
class c_tag_resource_controller_page_table_io_listener;
extern "C" bool tag_resource_page_table_read_required_pages(c_tag_resource_page_table *table,
    c_tag_resource_controller_page_table_io_listener *listener, bool include_reading, bool synchronous,
    c_tag_resource_error_receiver *flag_receiver);

class c_allocation_base;

class c_tag_resource_page_allocator;
class c_tag_resource_cache_published_interface;
class c_tag_resource_page_datum {
public:
 long m_handle;
 s_tag_resource_page_datum *m_page;
 c_tag_resource_page_allocator *m_datum_handler;
 c_tag_resource_cache_published_interface *m_published_interface;
 __forceinline c_tag_resource_page_datum(long handle, s_tag_resource_page_datum *page, c_tag_resource_page_allocator *datum_handler,
     c_tag_resource_cache_published_interface *published_interface) :
  m_handle(handle), m_page(page), m_datum_handler(datum_handler), m_published_interface(published_interface) {}
 __forceinline bool test_flag(long bit) const { return (m_page->flags & (1 << bit)) != 0; }
 __forceinline void set_flag(long bit, bool value) { if (value) m_page->flags |= (1 << bit); else m_page->flags &= ~(1 << bit); }
 __forceinline long get_reservation() const { return m_page->io_references; }
 __forceinline void set_reservation(long reservation) { short &value = m_page->io_references; value = (short)reservation; }
 __forceinline bool is_unreferenced() const { return get_reservation() == 0 && test_flag(3); }
 __forceinline bool address_is_pinned() const { return m_page->references > 0; }
 __forceinline bool address_is_locked() const;
 __forceinline unsigned long get_page_size() const;
 __forceinline bool is_ready() const { return get_state() >= 3; }
 __forceinline long get_state() const { return m_page->state; }
 __forceinline void set_state(long state) { m_page->state = (unsigned char)state; }
 __forceinline long get_absolute_internal_index() const { return m_handle & 0xffff; }
 __forceinline void pin() { ++m_page->references; }
 __forceinline short &get_reference_count() const { return m_page->references; }
 __forceinline void unpin() { --get_reference_count(); }
 __forceinline bool is_pinned() const { return get_reference_count() > 0; }
};
class c_tag_resource_controller_page_table_io_listener;
#include "cache/indirect_cache_file_reader_types.h"
struct s_lruv_cache;
class c_tag_resource_page_range_allocator;
class c_tag_resource_page_allocator;
class c_tag_resource_page_table;
class c_tag_resource_error_receiver;

class c_tag_resource_page_table_control_interface {
public:
 c_tag_resource_page_table *m_table;
 void pin_page(long handle);
 void unpin_page(long handle);
 void set_page_reservation(long handle, e_tag_resource_page_reservation reservation);
};
class c_asynchronous_io_marker;
class c_synchronized_long;
// Source of page reads: update_io_state polls completed reads, wait_for_io blocks until one completes.
class c_tag_resource_cache_file_reader
{
public:
 virtual void update_io_state() = 0;
 virtual void wait_for_io() = 0;
 virtual bool request_data(c_indirect_cache_file_bulk_read_iterator *iterator) = 0;
 virtual bool request_data_async(const s_indirect_cache_file_read_request *request, c_synchronized_long *bytes_read, c_asynchronous_io_marker *marker) = 0;
};
struct s_cache_file_tag_resource_window
{
    char *m_start;
    long m_size;
    char *get_end() const { return m_start + m_size; }
    unsigned long available(const c_basic_buffer<void> &window) const
    {
        return static_cast<char *>(window.m_address) - m_size - m_start;
    }
    void advance(long size) { m_start += size; m_size -= size; }
};

class c_tag_resource_page_range_allocator
{
public:
    virtual bool set_window(c_basic_buffer<void> window, c_basic_buffer<void> *out_buffer) { return false; }
    virtual bool resize(c_basic_buffer<void> *buffer, unsigned long minimum_size, unsigned long requested_size, long flags) = 0;
    virtual void allocate(c_basic_buffer<void> *buffer, unsigned long size);
    virtual void get_window(s_cache_file_tag_resource_window *out) {}
    virtual void reset_window(s_cache_file_tag_resource_window *out) = 0;
    virtual void set_locked_window(c_basic_buffer<void> window) {}
    virtual void lock_window() {}
};

class c_tag_resource_paging_page_filter
{
public:
    virtual bool page_is_necessary(long handle) = 0;
};

class c_tag_resource_cache_file_datum_handler;
class c_tag_resource_page_table {
public:
 class c_page_io_interface
 {
 public:
     virtual bool request_data(long page_handle, const s_indirect_cache_file_read_request *request) = 0;
 };

 class c_tag_resource_lruv_cache
 {
 public:
     __forceinline c_tag_resource_lruv_cache() : m_buffer() {}
     long m_alignment_bits;
     c_tag_resource_page_range_allocator *m_allocator;
     s_lruv_cache *m_cache;
     // The allocator writes its current window into the LRU buffer's storage.
     union
     {
         struct { c_basic_buffer<void> m_buffer; };
         s_cache_file_tag_resource_window m_window;
     };
 public:
     bool resize(unsigned long minimum_size, unsigned long extra_size, long flags, bool allow_partial, bool *out_full_size);
     __forceinline void initialize_for_new_map(long maximum_block_count, unsigned long page_size_bits,
         c_tag_resource_page_range_allocator *range_allocator, void *client_data, void (*delete_proc)(void *, long, bool),
         bool (*locked_proc)(void *, long), void (*move_proc)(void *, long, unsigned long, unsigned long), c_allocation_base *allocator);
 };
 s_data_array *m_pages;
 c_tag_resource_lruv_cache m_lruv_cache;
 s_data_array *m_async_requests;
 c_wrapped_flags m_ready_pages;
 c_wrapped_flags m_pinned_pages;
 c_tag_resource_cache_file_reader *m_reader;
 c_tag_resource_page_allocator *m_page_allocator;
 c_tag_resource_cache_published_interface *m_published_interface;
 c_allocation_base *m_allocator;
 void *m_storage;
 c_tag_resource_page_table_control_interface m_control_interface;
 c_data_iterator<s_tag_resource_page_datum> m_pages_iterator;
 bool m_async_requests_idle;
 unsigned long m_required_bytes;
 unsigned long m_loaded_bytes;
 bool m_include_unreferenced;
public:
 void initialize_for_new_map(unsigned long maximum_page_count, c_tag_resource_cache_file_reader *reader,
     c_tag_resource_cache_file_datum_handler *datum_handler, c_tag_resource_page_allocator *page_allocator,
     c_tag_resource_cache_published_interface *published_interface, c_allocation_base *allocator,
     c_tag_resource_page_range_allocator *range_allocator);
 bool load_required_pages(c_tag_resource_controller_page_table_io_listener *listener, bool include_reading, bool urgent,
     c_tag_resource_error_receiver *flag_receiver);
 void update_async_requests(c_tag_resource_controller_page_table_io_listener *listener);
 void cancel_async_and_block_until_async_done(c_tag_resource_controller_page_table_io_listener *listener);
 __forceinline void set_page_reservation(long handle, e_tag_resource_page_reservation reservation);
 void demand_page(long page_handle, c_tag_resource_controller_page_table_io_listener *listener);
private:
 bool lruv_is_tag_resource_page_locked(long handle);
 static bool lruv_is_tag_resource_page_locked_thunk(void *context, long handle);
 static void lruv_delete_tag_resource_page_thunk(void *context, long handle, bool release);
 friend void tag_resource_page_handle_lruv_delete(void *context, long handle, bool release);
 void lruv_move_tag_resource_page(long handle, unsigned long old_page_index, unsigned long new_page_index);
 friend void tag_resource_page_handle_lruv_move(void *context, long handle, unsigned long old_page_index, unsigned long new_page_index);
 bool request_page_async(long page_handle, const s_indirect_cache_file_read_request *request);
 friend bool tag_resource_page_table_read_required_pages(c_tag_resource_page_table *table,
     c_tag_resource_controller_page_table_io_listener *listener, bool include_reading, bool synchronous,
    c_tag_resource_error_receiver *flag_receiver);
public:
 __forceinline c_tag_resource_page_datum get_resource_page_from_handle(long handle) {
  s_tag_resource_page_datum *pages = (s_tag_resource_page_datum *)m_pages->data;
  return c_tag_resource_page_datum(handle, &pages[datum_index_to_absolute_index(handle)], m_page_allocator, m_published_interface);
 }
 void *allocate_page_storage(long handle, unsigned long *out_size);
 __forceinline void set_page_state(long handle, long state) {
  c_tag_resource_page_datum page = get_resource_page_from_handle(handle);
  long old_state = page.get_state();
  page.set_state(state);
   if (old_state > state && old_state >= 4 && state < 4) m_pinned_pages.set(page.get_absolute_internal_index(), false);
   if (old_state > state && old_state >= 3 && state < 3) m_ready_pages.set(page.get_absolute_internal_index(), false);
   if (old_state < state && old_state < 4 && state >= 4) m_pinned_pages.set(page.get_absolute_internal_index(), true);
   if (old_state < state && old_state < 3 && state >= 3) m_ready_pages.set(page.get_absolute_internal_index(), true);
 }
 __forceinline void pin_page(long handle) {
  c_tag_resource_page_datum page = get_resource_page_from_handle(handle);
  page.pin();
  set_page_state(handle, 4);
 }
 __forceinline void unpin_page(long handle) {
  c_tag_resource_page_datum page = get_resource_page_from_handle(handle);
  page.unpin();
  if (!page.is_pinned()) set_page_state(handle, 3);
 }
};

typedef char tag_resource_page_include_unreferenced_offset[
    (sizeof(void *) != 4 || ((offsetof(c_tag_resource_page_table, m_include_unreferenced) == 0x60))) ? 1 : -1];
typedef char tag_resource_page_table_size[(sizeof(void *) != 4 || ((sizeof(c_tag_resource_page_table) == 0x64))) ? 1 : -1];

template<bool (c_tag_resource_page_table::*t_request_proc)(long, const s_indirect_cache_file_read_request *)>
class c_tag_resource_page_table_io_wrapper : public c_tag_resource_page_table::c_page_io_interface
{
public:
 __forceinline c_tag_resource_page_table_io_wrapper(c_tag_resource_page_table *page_table) : m_page_table(page_table) {}
 virtual bool request_data(long page_handle, const s_indirect_cache_file_read_request *request)
 {
  return (m_page_table->*t_request_proc)(page_handle, request);
 }

private:
 c_tag_resource_page_table *m_page_table;
};

#endif
