#ifndef CACHE_TAG_RESOURCE_CACHE_NEW_H
#define CACHE_TAG_RESOURCE_CACHE_NEW_H

#include "cache/tag_resource_cache_address_cache.h"
#include "memory/data_types.h"
#include "memory/data.h"
#include "memory/wrapped_arrays.h"
#include "tag_files/tag_resource_cache_paging.h"
#include "tag_files/tag_resource_control_datum.h"


// Include after the owning page-table declaration (embedded by value below).
class c_allocation_base;
class c_tag_resource_runtime_listener;
class c_tag_resource_address_cache_control_interface;
static __forceinline void tag_resource_cache_external_access_data_make_invalid(s_data_array *data)
{
    data->valid = 0;
}
struct s_tag_resource_cache_external_access_wrapped_array {
    void *m_elements;
    long m_count;
    __forceinline void set_elements(void *elements, long count)
    {
        m_count = count;
        m_elements = elements;
    }
};
struct s_tag_resource_cache_external_access_wrapped_flags : public c_wrapped_flags {
    __forceinline s_tag_resource_cache_external_access_wrapped_flags() {}
};


struct s_tag_resource_cache_external_access_slot {
    short identifier;
    unsigned short first;
    unsigned short count;
    unsigned short capacity;
};
typedef char external_access_slot_capacity_offset[(offsetof(s_tag_resource_cache_external_access_slot, capacity) == 6) ? 1 : -1];
struct s_tag_resource_cache_external_access_slot_owner {
    __forceinline s_tag_resource_cache_external_access_slot_owner() : flags(0) {}
    __forceinline void dispose_from_old_map()
    {
        tag_resource_cache_external_access_data_make_invalid(table);
        table = 0;
        list.set_elements(0, 0);
        m_allocator->deallocate(m_storage);
    }
    c_allocation_base *m_allocator;
    void *m_storage;
    c_wrapped_array<long> list;
    s_data_array *table;
    unsigned short flags;
    unsigned short cursor;
};
class c_tag_resource_cache_file_datum_handler;
struct c_tag_resource_cache_external_access_page_data;
struct s_data_array;
class c_tag_resource_address_cache_control_interface;
class c_tag_resource_cache_new;

#include "cache/tag_resource_cache_external_access_dependencies.h"

class c_tag_resource_control_datum;
struct s_tag_resource_control_datum_data;
class c_tag_resource_page_table_control_interface;
class c_tag_resource_runtime_active_set;
class c_tag_resource_cache_controller;
class c_tag_resource_cache_controller {
private:
    void release_resource_page_references(long resource_handle, c_tag_resource_address_cache_control_interface *address_cache_control);
    void release_page_reservation(long resource_handle);
    bool all_pages_loaded(long resource_handle);
    void resource_prepare_for_unloading(long resource_handle);
    bool resource_rebuild_references(long resource_handle);
    void resource_update_state_to_valid(long resource_handle);
    void resource_tear_down_references(long resource_handle);
    void resource_decommit(long resource_handle);
public:
    static __forceinline void release_resource_page_references_for_cache(
        c_tag_resource_cache_controller *controller, long handle, c_tag_resource_address_cache_control_interface *address_cache)
    {
        controller->release_resource_page_references(handle, address_cache);
    }
    __forceinline c_tag_resource_control_datum get_resource_control_from_handle(long h);
    __forceinline void set_bit(long index, bool value);
    __forceinline void record_new_resource_state(long handle, long old_state, long new_state);
public:
    void prepare_for_disposal_from_old_map(c_tag_resource_address_cache_control_interface *old_map);
    c_allocation_base *m_allocator;
    void *m_buffer;
    union { s_data_array *m_resources; c_smart_data_array<s_tag_resource_control_datum_data> m_control_data; };
    s_tag_resource_cache_external_access_wrapped_flags m_required;
    s_tag_resource_cache_external_access_wrapped_flags m_requested;
    s_tag_resource_cache_external_access_wrapped_flags m_available;
    s_tag_resource_cache_external_access_wrapped_flags m_pending_release;
    s_tag_resource_cache_external_access_wrapped_flags m_optional;
    s_tag_resource_cache_external_access_wrapped_flags m_pending;
    s_tag_resource_cache_external_access_wrapped_flags m_predicted;
    c_tag_resource_page_table_control_interface *m_page_table;
    c_tag_resource_runtime_listener *m_resource_listener;
    c_tag_resource_cache_file_datum_handler *m_file_datum_handler;
    c_tag_resource_cache_control_datum_info m_datum_info;
    unsigned long m_prediction_generation;
    unsigned long m_prediction_state;
    bool m_prediction_pending;
    bool m_can_predict_optional;
    bool m_busy;
    bool m_can_load_all_desired_optional;
    bool m_require_async_page_capacity;
    __forceinline void dispose_from_old_map() {
        m_datum_info.dispose_from_old_map();
        m_requested.set_bits((void *)0, 0);
        m_required.set_bits((void *)0, 0);
        m_predicted.set_bits((void *)0, 0);
        m_pending.set_bits((void *)0, 0);
        m_optional.set_bits((void *)0, 0);
        tag_resource_cache_external_access_data_make_invalid(m_resources);
        m_allocator->deallocate(m_buffer);
        m_resource_listener = 0;
    }
    void begin_tracking_resource(long resource_handle, bool unused);
    void end_tracking_resource(long resource_handle);
    void update_required_resources(c_tag_resource_address_cache_control_interface *x);
    void adopt_necessary_resources();
    void orphan_unnecessary_unlocked_resources(c_tag_resource_address_cache_control_interface *x);
    void initialize_for_new_map(unsigned long maximum_control_count, class c_tag_resource_page_table_control_interface *page_table,
        class c_tag_resource_runtime_listener *runtime_listener, class c_tag_resource_cache_file_datum_handler *file_datum_handler,
        class c_allocation_base *allocator);

    public:
    static __forceinline bool _symfix_all_pages_loaded(c_tag_resource_cache_controller *self, long handle)
    { return self->all_pages_loaded(handle); }
    static __forceinline void _symfix_resource_update_state_to_valid(c_tag_resource_cache_controller *self, long handle)
    { self->resource_update_state_to_valid(handle); }
    static __forceinline void _symfix_release_page_reservation(c_tag_resource_cache_controller *self, long handle)
    { self->release_page_reservation(handle); }
};
typedef char external_controller_prediction_generation_offset[
    (sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_controller, m_prediction_generation) == 0x54))) ? 1 : -1];
typedef char external_controller_prediction_state_offset[
    (sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_controller, m_prediction_state) == 0x58))) ? 1 : -1];
typedef char external_controller_prediction_pending_offset[
    (sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_controller, m_prediction_pending) == 0x5c))) ? 1 : -1];
typedef char external_controller_busy_offset[(sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_controller, m_busy) == 0x5e))) ? 1 : -1];
typedef char external_controller_size[(sizeof(void *) != 4 || ((sizeof(c_tag_resource_cache_controller) == 100))) ? 1 : -1];
typedef char external_controller_required_offset[(sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_controller, m_required) == 12))) ? 1 : -1];
typedef char external_controller_prediction_offset[(sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_controller, m_predicted) == 60))) ? 1 : -1];
class c_tag_resource_cache_new {
    void publish_necessary_resources(bool a, bool b);
    friend void tag_resource_cache_new_commit_dirty(c_tag_resource_cache_new *cache);
    friend void tag_resource_cache_new_update_zones(c_tag_resource_cache_new *cache, long unused, bool synchronous,
        c_tag_resource_error_receiver *flag_receiver);
    friend void tag_resource_cache_new_update(c_tag_resource_cache_new *cache);
    friend void tag_resource_cache_new_update_pages(c_tag_resource_cache_new *cache, bool force);
    friend void tag_resource_cache_new_handle_space_demand(c_tag_resource_cache_new *cache, unsigned long size);
    void attempt_to_resize(bool force);
    void revoke_active_access_cache();
    void release_unlocked_resources(bool block);
    void verify_all_required_resources_available();
public:
    c_tag_resource_cache_new();
    __forceinline void prepare_for_disposal_from_old_map();
    __forceinline void unsetup_bulk_tracked_resources(class c_cache_file_tag_resource_setup_iterator *iterator);
    __forceinline void unsetup_bulk_tracked_pages(struct s_cache_file_tag_resource_set_iterator *iterator);
    __forceinline void setup_bulk_tracked_pages(struct s_cache_file_tag_resource_set_iterator *iterator);
    __forceinline void setup_bulk_tracked_resources(class c_cache_file_tag_resource_setup_iterator *iterator);
    __forceinline void enable_optional_resources(bool enable) { m_allow_optional = enable; }
    bool resource_is_locked_externally(long resource_handle);
    c_tag_resource_cache_new *m_published_interface_owner;
    c_tag_resource_controller_page_table_io_listener m_page_io_listener_owner;
    c_tag_resource_page_table m_page_table;
    c_tag_resource_cache_controller m_controller;
    c_tag_resource_address_cache m_resource_address_cache;
    s_tag_resource_cache_external_access_slot_owner m_inverse_page_table;
    c_tag_resource_cache_file_datum_handler *m_resource_interface;
    class c_tag_resource_cache_new_loaded_query *m_loaded_query;
    class c_tag_resource_paging_page_filter *m_page_filter;
    __declspec(align(32)) c_tag_resource_cache_thread_lock_lock_freeish m_resource_thread_access_lock;
    __declspec(align(32)) bool m_resources_changed;
    bool m_resources_valid;
    bool m_resize_policy_enabled;
    bool m_flag303;
    bool m_thread_access_locked;
    bool m_allow_optional;
    bool m_resize_succeeded;
    bool m_full_size;
    bool m_incremental_publishing;
    bool m_resource_spam;
    bool m_use_demand_policy;
    bool m_per_frame_publishing;
    long m_unknown30c;
    long m_resize_flags;
    c_allocation_base *m_allocator;
    void end_tracking_resource(long resource);
private:
  bool update_active_resources();
    public:
    void dispose_from_old_map(void);

};

typedef char external_cache_page_io_listener_owner_offset[
    (sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_new, m_page_io_listener_owner) == 4))) ? 1 : -1];
typedef char external_cache_loaded_query_offset[
    (sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_new, m_loaded_query) == 0x130))) ? 1 : -1];
typedef char external_cache_page_filter_offset[
    (sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_new, m_page_filter) == 0x134))) ? 1 : -1];
typedef char external_cache_demand_policy_offset[
    (sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_new, m_use_demand_policy) == 0x30a))) ? 1 : -1];
typedef char external_cache_published_interface_owner_offset[
    (offsetof(c_tag_resource_cache_new, m_published_interface_owner) == 0) ? 1 : -1];
typedef char external_cache_thread_access_lock_offset[
    (sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_new, m_resource_thread_access_lock) == 0x140))) ? 1 : -1];
typedef char external_cache_resources_changed_offset[
    (sizeof(void *) != 4 || ((offsetof(c_tag_resource_cache_new, m_resources_changed) == 0x300))) ? 1 : -1];

#endif
