// blamlibxbox/tag_resource_cache_control.obj: from the records 0x857a8 0x85c20 0x85eb8 0x85fd8 0x86140 0x864a8 0x865a0 0x86780 0x86a60 0x878a0 0x87b90 0x87c20 0x87c80 0x1b2a58 0x1b2c28 0x2ad860 0x2ada78 0x2adb48 0x2adf60 0x2ae030 0x2ae110 0x2ae1f8 0x2ae338 0x2ae5b0 0x2ae650 0x2ae7b0 0x2ae850 0x2ae970 0x2aeb40 0x2aec30 0x2aecb8 0x2aed90
// @flags /TP /O2 /Oc
// @address 0x865a0 tag_resource_cache_get_resource_page_buffer
// @address 0x87b90 ?set_state@c_tag_resource_control_datum@@QAAXW4e_tag_resource_control_datum_state@@@Z
// @address 0x87c20 ?deferred@c_tag_resource_control_datum@@QBA_NXZ
// @address 0x2aecb8 ?get_page_reservation@c_tag_resource_control_datum@@QBAJXZ
// @address 0x1018a0 ?datum_new_at_index@@YAJPAUs_data_array@@J@Z
#include "tag_files/tag_resource_internals.h"
#include "cache/tag_resource_page_reservation_types.h"
#include "memory/data.h"
#include "tag_files/tag_resource_cache_control.h"
#include "memory/data_types.h"
#include "memory/contiguous_allocation.h"
#include "memory/data_types.h"
#include "tag_files/tag_resource_interop_location_types.h"
#include "tag_files/tag_resource_fixup_location_types.h"
struct s_tag_resource_update_iterator;
struct s_tag_resource_controller_update_view;
struct s_tag_resource_control_datum_update_view;
#include <ppcintrinsics.h>
#include "cseries/crt_xdk.h"
#include "cseries/pointer_utilities.h"
#include <stdlib.h>
#include <string.h>
#include <new>
#include "memory/wrapped_arrays.h"

#include "memory/basic_buffer.h"
#include "cache/tag_resource_page_datum.h"
#include "memory/wrapped_flags.h"
class c_tag_resource_page_table_control_interface;
class c_tag_resource_runtime_listener;
class c_tag_resource_cache_file_datum_handler;
class c_tag_resource_runtime_active_set;
#include "memory/lruv_cache_types.h"
#include "memory/lruv_block_types.h"
#include "tag_files/tag_resource_control_datum.h"
#include "cache/tag_resource_cache_file_datum_handler.h"
#include "cache/tag_resource_runtime_interfaces.h"
typedef char resource_control_datum_size[(sizeof(s_tag_resource_control_datum_data) == 8) ? 1 : -1];
typedef char resource_control_page_offset[(offsetof(s_tag_resource_control_datum_data, page_handle) == 4) ? 1 : -1];

class c_tag_resource_cache_controller;

#include "cache/cache_file_tag_resource_runtime_manager_storage.h"
#include "cache/tag_resource_page_table.h"
#include "cache/tag_resource_cache_new.h"
#include "cache/tag_resource_cache_address_cache.h"
#include "tag_files/tag_resource_internals_types.h"
#include "memory/datum_index_inlines.h"

struct tag_resource_cache_lookup_result {
    bool optional;
    s_tag_resource_control_datum_data *data;
    __forceinline bool is_optional() const { return optional; }
};

struct s_tag_resource_page_reservations
{
    union {
        struct {
            unsigned __int64 required_reservation : 17;
            unsigned __int64 optional_reservation : 17;
            unsigned __int64 index : 15;
            unsigned __int64 salt : 15;
        };
        __int64 packed;
    };
};
typedef char tag_resource_page_reservations_size[(sizeof(s_tag_resource_page_reservations) == 8) ? 1 : -1];

class c_tag_resource_address_cache_control_interface
{
public:
    c_tag_resource_address_cache *m_data;
    __forceinline bool resource_address_published(long resource_handle)
    {
        return m_data->m_available_addresses.test(resource_handle & 0xffff);
    }
    __forceinline void publish_resource_address(long resource_handle, s_tag_resource_page_reservations address)
    {
        c_tag_resource_address_cache *cache = m_data;
        long index = resource_handle & 0xffff;
        cache->m_available_addresses.set(index, true);
        cache->m_pages[index] = address.packed;
    }
    __forceinline void mark_unrequired_resource(long index)
    {
        unsigned long *bits = m_data->m_required_resources.m_flags;
        long word_index = index >> 5;
        bits[word_index] &= ~(1 << (index & 31));
    }
    __forceinline void staleify_resource_address(long resource_handle)
    {
        c_tag_resource_address_cache *cache = m_data;
        long index = resource_handle & 0xffff;
        cache->m_available_addresses.set(index, false);
        cache->m_pages[index] = -1;
    }
};

// Word-wise bit-vector expressions: operands are built from flags by value and
// combined lazily, one 32-bit word at a time.
class c_bit_vector_operand
{
public:
    __forceinline c_bit_vector_operand(c_wrapped_flags flags) :
        m_bits(flags.m_flags)
    {
    }
    __forceinline unsigned long get_word(long word_index) const { return m_bits[word_index]; }
private:
    const unsigned long *m_bits;
};
class c_bit_vector_constant
{
public:
    __forceinline c_bit_vector_constant(bool value) : m_word(value ? 0xffffffff : 0) { }
    __forceinline unsigned long get_word(long word_index) const { return m_word; }
private:
    unsigned long m_word;
};
template<typename t_operand>
class c_bit_vector_not
{
public:
    __forceinline c_bit_vector_not(const t_operand &operand) : m_operand(operand) { }
    __forceinline unsigned long get_word(long word_index) const { return ~m_operand.get_word(word_index); }
private:
    t_operand m_operand;
};
template<typename t_left, typename t_right>
class c_bit_vector_and
{
public:
    __forceinline c_bit_vector_and(const t_left &left, const t_right &right) : m_left(left), m_right(right) { }
    __forceinline unsigned long get_word(long word_index) const { return m_left.get_word(word_index) & m_right.get_word(word_index); }
private:
    t_left m_left;
    t_right m_right;
};
template<typename t_left, typename t_right>
class c_bit_vector_or
{
public:
    __forceinline c_bit_vector_or(const t_left &left, const t_right &right) : m_left(left), m_right(right) { }
    __forceinline unsigned long get_word(long word_index) const { return m_left.get_word(word_index) | m_right.get_word(word_index); }
private:
    t_left m_left;
    t_right m_right;
};
template<typename t_left, typename t_right>
class c_bit_vector_xor
{
public:
    __forceinline c_bit_vector_xor(const t_left &left, const t_right &right) : m_left(left), m_right(right) { }
    __forceinline unsigned long get_word(long word_index) const { return m_left.get_word(word_index) ^ m_right.get_word(word_index); }
private:
    t_left m_left;
    t_right m_right;
};

// Pops the set bits of an expression, lowest bit first.
class c_bit_vector_expression_iterator
{
public:
    __forceinline c_bit_vector_expression_iterator(unsigned long bit_count) :
        m_word_index(-1), m_word_count((bit_count + 31) >> 5), m_word(0)
    {
    }
    template<typename t_expression>
    __forceinline bool next(const t_expression &expression, long *bit_index)
    {
        while (m_word == 0)
        {
            if (++m_word_index >= m_word_count)
                break;
            m_word = expression.get_word(m_word_index);
        }
        bool result = false;
        if (m_word_index < m_word_count)
        {
            unsigned long lowest_bit = m_word & -m_word;
            m_word = (m_word - 1) & m_word;
            result = true;
            *bit_index = (m_word_index << 5) + 31 - _CountLeadingZeros(lowest_bit & -lowest_bit);
        }
        return result;
    }
private:
    long m_word_index;
    long m_word_count;
    unsigned long m_word;
};

typedef c_bit_vector_and<c_bit_vector_or<c_bit_vector_operand, c_bit_vector_constant>, c_bit_vector_operand> c_tag_resource_needed_expression;
typedef c_bit_vector_and<c_bit_vector_and<c_bit_vector_operand, c_bit_vector_operand>, c_bit_vector_not<c_tag_resource_needed_expression> >
    c_tag_resource_published_unneeded_expression;
typedef c_bit_vector_and<c_bit_vector_and<c_bit_vector_and<c_bit_vector_or<c_bit_vector_operand, c_bit_vector_constant>,
                                              c_bit_vector_not<c_bit_vector_operand> >,
                             c_bit_vector_operand>,
    c_bit_vector_operand>
    c_tag_resource_unpublished_needed_expression;
typedef c_bit_vector_and<c_bit_vector_and<c_bit_vector_operand, c_bit_vector_not<c_bit_vector_operand> >, c_bit_vector_operand>
    c_tag_resource_available_unrequested_expression;
typedef c_bit_vector_and<c_bit_vector_operand, c_bit_vector_xor<c_bit_vector_operand, c_tag_resource_needed_expression> >
    c_tag_resource_mispublished_expression;

c_basic_buffer<void> c_tag_resource_control_datum::try_to_get_optional_data()
{
    c_basic_buffer<void> optional_data = enabled ? get_cache_data() : c_basic_buffer<void>();
    return optional_data;
}


static __forceinline void *tag_resource_cache_control_inl_address(
    unsigned int fixup, const c_basic_buffer<void> *control_pointer, const c_basic_buffer<void> *cache, const c_basic_buffer<void> *optional)
{
    const c_basic_buffer<void> &control = *control_pointer;
    unsigned int offset = fixup & 0x1fffffff;
    void *result;
    switch (fixup >> 29) {
    case _tag_resource_fixup_type_value: result = pointer_from_address(offset); break;
    case _tag_resource_fixup_type_data: result = (char *)control.get_buffer() + offset; break;
    case _tag_resource_fixup_type_cache_data: result = cache ? (char *)cache->get_buffer() + offset : 0; break;
    case _tag_resource_fixup_type_optional_data: result = optional ? (char *)optional->get_buffer() + offset : 0; break;
    // match: 0x878a0: without it the fixup-type jump table gains a cmplwi 3 range check and branch
    default: __assume(0);
    }
    return result;
}
// @address 0x878a0 ?get_root_address@c_tag_resource_control_datum@@QAAPAXXZ
void *c_tag_resource_control_datum::get_root_address()
{
    c_tag_resource_fixup root = handler->get_root_address_fixup(external_handle);
    bool optional = enabled;
    int handle = external_handle;
    void *result;
    if (optional) {
        c_basic_buffer<void> optional_data = handler->get_control_data(handle, optional);
        result = tag_resource_cache_control_inl_address(root.value, &optional_data, 0, 0);
    } else {
        c_basic_buffer<void> control_data = handler->get_control_data(handle, optional);
        result = tag_resource_cache_control_inl_address(root.value, &control_data, 0, 0);
    }
    return result;
}

bool c_tag_resource_control_datum::deferred() const
{
    return enabled ? active() : (((data->flags >> 1) & 1) == 0 && (data->flags & 4) != 0);
}

// @address 0x1b2a58 ?set_window@c_cache_file_tag_resource_runtime_manager_storage@@UAA_NV?$c_basic_buffer@X@@PAV2@@Z
bool c_cache_file_tag_resource_runtime_manager_storage::set_window(c_basic_buffer<void> window, c_basic_buffer<void> *out_buffer)
{
    char *base = static_cast<char *>(m_buffer.m_address);
    c_basic_buffer<void> result(base, static_cast<char *>(window.m_address) + window.m_size - base);
    m_requested_buffer = result;
    m_result_buffer = result;
    *out_buffer = result;
    return true;
}

// @address 0x1b2c28 ?allocate@c_cache_file_tag_resource_runtime_manager_storage@@UAAXPAV?$c_basic_buffer@X@@K@Z
void c_cache_file_tag_resource_runtime_manager_storage::allocate(c_basic_buffer<void> *buffer, unsigned long size)
{
    c_basic_buffer<void> base = m_buffer;
    c_basic_buffer<void> result(base.m_address, size);
    m_result_buffer = result;
    *buffer = result;
}

 __forceinline c_tag_resource_control_datum c_tag_resource_cache_controller::get_resource_control_from_handle(long h)
 {
     return c_tag_resource_control_datum(m_can_predict_optional, h, &m_datum_info,
         datum_get<s_tag_resource_control_datum_data>(m_control_data, h),
         (c_resource_buffer_handler *)m_file_datum_handler);
 }
 __forceinline void c_tag_resource_cache_controller::set_bit(long index, bool value)
 {
     unsigned long *bits = m_requested.m_flags;
     if (value)
         bits[index >> 5] |= 1 << (index & 31);
     else
         bits[index >> 5] &= ~(1 << (index & 31));
 }
 __forceinline void c_tag_resource_cache_controller::record_new_resource_state(long handle, long old_state, long new_state)
 {
     if (old_state < new_state)
     {
         if (new_state == 4)
             set_bit(handle & 65535, true);
     }
     else if (new_state < old_state)
     {
         if (old_state == 4)
             set_bit(handle & 65535, false);
     }
 }
static __forceinline void tag_cache_ctl_set_state(c_tag_resource_control_datum *resource, long state)
{
    long old_state = resource->data->state;
    resource->data->state = (unsigned char)state;
    resource->owner_info->m_controller->record_new_resource_state(resource->handle, old_state, state);
}
void c_tag_resource_control_datum::set_state(e_tag_resource_control_datum_state state)
{
    long old_state = data->state;
    data->state = (unsigned char)state;
    owner_info->m_controller->record_new_resource_state(handle, old_state, state);
}
// @address 0x2ae650 ?begin_tracking_resource@c_tag_resource_cache_controller@@QAAXJ_N@Z
void c_tag_resource_cache_controller::begin_tracking_resource(long resource_handle, bool unused) {
 long cache_handle=datum_new_at_index(this->m_control_data,resource_handle);
 c_tag_resource_control_datum resource=this->get_resource_control_from_handle(cache_handle);
 tag_cache_ctl_set_state(&resource, 0);
 resource.set_flag(4,false);
 bool available=resource.enabled ? ((c_tag_resource_cache_file_datum_handler *)resource.handler)->any_optional_data(resource.external_handle)
     : ((c_tag_resource_cache_file_datum_handler *)resource.handler)->any_pageable_data(resource.external_handle);
 resource.set_flag(5,available);
 if (resource.test_flag(5))
     resource.data->page_handle = resource.enabled
         ? ((c_tag_resource_cache_file_datum_handler *)this->m_file_datum_handler)->get_optional_page_index(resource.external_handle)
         : ((c_tag_resource_cache_file_datum_handler *)this->m_file_datum_handler)->get_required_page_index(resource.external_handle);
 if(resource.enabled) { unsigned long *bits=this->m_required.m_flags; long index=resource.handle&65535; bits[index>>5]|=1<<(index&31); }
}

// @address 0x2aec30 tag_resource_cache_get_page_reservation
extern "C" int tag_resource_cache_get_page_reservation(c_tag_resource_cache_controller *cache_controller, int target_resource_handle)
{
    s_tag_resource_control_datum_data *control_datum = (s_tag_resource_control_datum_data *)(cache_controller->m_control_data->data
        + ((target_resource_handle << 3) & 0x7fff8));
    if (control_datum->test_flag(5)) {
        unsigned short definition_index = (unsigned short)control_datum->page_handle;
        c_tag_resource_page_table *reservation_table = cache_controller->m_page_table->m_table;
        int result = -1;
        if (((s_tag_resource_page_datum *)reservation_table->m_pages->data)[definition_index].state >= 3)
            result = ((s_lruv_block *)((s_data_array *)reservation_table->m_lruv_cache.m_cache->header.block_data)->data)[definition_index].page_index;
        return result;
    }
    return -1;
}

long c_tag_resource_control_datum::get_page_reservation() const
{
    long reservation = 0;
    if (test_flag_5()) {
        if (unneeded())
            reservation = 2;
        else if (deferred())
            reservation = 1;
        else if (active())
            reservation = 3;
    }
    return reservation;
}

bool c_tag_resource_control_datum::always_required() const
{
    return active() && !enabled && !deferred() && !not_required();
}




c_basic_buffer<void> c_tag_resource_control_datum::get_cache_data()
{
    c_tag_resource_cache_controller *controller = owner_info->m_controller;
    long page_handle = datum_get<s_tag_resource_control_datum_data>(controller->m_control_data, handle)->page_handle;
    const s_tag_resource_page_datum *page = datum_get<s_tag_resource_page_datum>(controller->m_page_table->m_table->m_pages, page_handle);
    c_basic_buffer<void> cache_data = page->buffer;
    unsigned long optional_offset = enabled ? handler->get_optional_offset(external_handle) : 0;
    return enabled && optional_offset ? cache_data.get_sub_buffer(optional_offset, cache_data.m_size - optional_offset) : cache_data;
}

class c_tag_resource_runtime_listener;
class c_tag_resource_cache_file_datum_handler;
class c_tag_resource_address_cache_control_interface;
class c_tag_resource_cache_controller;
class c_tag_resource_runtime_active_set;


// @address 0x2ae5b0 ?resource_decommit@c_tag_resource_cache_controller@@AAAXJ@Z
void c_tag_resource_cache_controller::resource_decommit(long resource_handle)
{
    resource_prepare_for_unloading(resource_handle);
    resource_tear_down_references(resource_handle);
    c_tag_resource_control_datum resource = this->get_resource_control_from_handle(resource_handle);
    tag_cache_ctl_set_state(&resource, 0);
}


// @address 0x2ae7b0 ?end_tracking_resource@c_tag_resource_cache_controller@@QAAXJ@Z
void c_tag_resource_cache_controller::end_tracking_resource(long resource_handle)
{
    c_tag_resource_control_datum resource = this->get_resource_control_from_handle(resource_handle);
    resource.set_flag(1, false);
    unsigned char *flags = &resource.data->flags;
    *flags &= ~4;
    resource_decommit(resource_handle);
    release_page_reservation(resource_handle);
    if (resource.enabled) {
        unsigned long *bits = m_required.m_flags;
        long index = resource.handle & 0xffff;
        bits[index >> 5] &= ~(1 << (index & 31));
    }
    ::datum_delete(m_control_data, resource_handle);
}

// @address 0x412f4 ??_7c_data_array_allocation_description@@6B@
// @address 0x415b8 ??_7c_wrapped_flags_allocation_description@@6B@
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z

// @address 0x2ad860 ?initialize_for_new_map@c_tag_resource_cache_controller@@QAAXKPAVc_tag_resource_page_table_control_interface@@PAVc_tag_resource_runtime_listener@@PAVc_tag_resource_cache_file_datum_handler@@PAVc_allocation_base@@@Z
void c_tag_resource_cache_controller::initialize_for_new_map(unsigned long maximum_control_count,
    c_tag_resource_page_table_control_interface *page_table, c_tag_resource_runtime_listener *runtime_listener,
    c_tag_resource_cache_file_datum_handler *file_datum_handler, c_allocation_base *allocator)
{
    m_allocator = allocator;
    m_page_table = page_table;
    m_resource_listener = runtime_listener;
    m_file_datum_handler = file_datum_handler;
    s_allocation_description_holder descriptions[] = {
        { c_data_array_allocation_description(m_control_data.get_restricted_data_array_address(),
            "tag resource cache control state", maximum_control_count * 2, 8, 2) },
        { c_wrapped_flags_allocation_description(m_pending, maximum_control_count * 2) },
        { c_wrapped_flags_allocation_description(m_predicted, maximum_control_count * 2) },
        { c_wrapped_flags_allocation_description(m_required, maximum_control_count * 2) },
        { c_wrapped_flags_allocation_description(m_requested, maximum_control_count * 2) },
        { c_wrapped_flags_allocation_description(m_available, maximum_control_count * 2) },
        { c_wrapped_flags_allocation_description(m_pending_release, maximum_control_count * 2) },
        { c_wrapped_flags_allocation_description(m_optional, maximum_control_count * 2) }
    };
    c_wrapped_array_no_init<s_allocation_description_holder> list_storage;
    c_wrapped_array<s_allocation_description_holder> *list =
        new (&list_storage) c_wrapped_array<s_allocation_description_holder>(descriptions);
    allocate_list_contiguous(allocator, *list, "tag resource cache controller", &m_buffer, 0);
    s_data_array *control_data = m_control_data;
    control_data->valid = 1;
    data_delete_all(control_data);
    m_pending.clear();
    m_predicted.clear();
    m_required.clear();
    m_requested.clear();
    m_available.clear();
    m_pending_release.clear();
    m_optional.clear();
    m_datum_info.initialize_for_new_map(this);
    m_prediction_generation = 0;
    m_busy = 0;
    m_can_load_all_desired_optional = 0;
}

static __forceinline c_basic_buffer<void> tag_resource_cache_control_get_optional_page_data(
    c_tag_resource_cache_controller *controller, unsigned short definition)
{
    c_tag_resource_page_table_control_interface *table = controller->m_page_table;
    s_data_array *arr = table->m_table->m_pages;
    s_tag_resource_page_datum *entry = &((s_tag_resource_page_datum *)arr->data)[definition];
    c_basic_buffer<void> cache_data(c_basic_buffer<void>::_uninitialized);
    memcpy(&cache_data, &entry->buffer, sizeof(cache_data));
    return cache_data;
}

extern "C" c_basic_buffer<void> tag_resource_cache_get_resource_page_buffer(c_tag_resource_cache_controller *controller, long resource_handle)
{
    s_tag_resource_control_datum_data *datum = (s_tag_resource_control_datum_data *)controller->m_control_data->data + (resource_handle & 0xffff);
    unsigned short definition = (unsigned short)datum->page_handle;
    c_basic_buffer<void> optional_data =
        ((datum->flags & 0x20) != 0) ? tag_resource_cache_control_get_optional_page_data(controller, definition) : c_basic_buffer<void>();
    return optional_data;
}

static __forceinline bool tag_resource_cache_control_is_optional(long handle)
{
    unsigned short salt = (unsigned short)(handle >> 16);
    return (salt & 0x8000) != 0;
}
void c_tag_resource_cache_controller::orphan_unnecessary_unlocked_resources(c_tag_resource_address_cache_control_interface *address_cache_control)
{
    c_data_iterator<void> iterator;
    iterator.begin(m_control_data);
    while (iterator.next()) {
        long resource_handle = iterator.get_index();
        c_tag_resource_control_datum resource = this->get_resource_control_from_handle(resource_handle);
        if (!resource.test_flag(1)) {
            bool required = !resource.enabled && resource.test_flag(3);
            if (!required) {
                release_resource_page_references(resource_handle, address_cache_control);
                resource_decommit(resource_handle);
            }
        }
    }
}
// @address 0x2ae030 ?adopt_necessary_resources@c_tag_resource_cache_controller@@QAAXXZ
void c_tag_resource_cache_controller::adopt_necessary_resources()
{
    c_data_iterator<void> iterator;
    iterator.begin(m_control_data);
    while (iterator.next()) {
        long index = iterator.get_index();
        long optional = tag_resource_cache_control_is_optional(index);
        s_tag_resource_control_datum_data *datum = (s_tag_resource_control_datum_data *)(m_control_data->data
            + (((unsigned long)index << 3) & 0x7fff8));
        if ((datum->flags & 2) != 0) {
            if (datum->state == 0)
                datum->state = 1;
        } else {
            bool should_adopt = ((unsigned char)optional == 0) && ((datum->flags & 8) != 0);
            if (should_adopt && datum->state == 0)
                datum->state = 1;
        }
    }
}
// @address 0x2aeb40 ?release_page_reservation@c_tag_resource_cache_controller@@AAAXJ@Z
void c_tag_resource_cache_controller::release_page_reservation(long resource_handle)
{
    long index = resource_handle & 0xffff;
    s_tag_resource_control_datum_data *datum = (s_tag_resource_control_datum_data *)(m_control_data->data + (index << 3));
    int flags = datum->flags;
    if ((flags & 0x40) != 0) {
        s_tag_resource_control_datum_data *res = (s_tag_resource_control_datum_data *)(m_control_data->data + (index << 3));
        res->flags &= 0xbf;
        unsigned short definition = (unsigned short)datum->page_handle;
        c_tag_resource_page_table *table = ((c_tag_resource_page_table_control_interface *)m_page_table)->m_table;
        s_tag_resource_page_datum *pages = ((s_tag_resource_page_datum *)table->m_pages->data);
        s_tag_resource_page_datum *entry = (s_tag_resource_page_datum *)&pages[definition];
        short *reservation_slot = &entry->io_references;
        long old_reservation = (short)*reservation_slot;
        *reservation_slot = 0;
        if ((entry->flags & 1) != 0 && old_reservation >= 2) {
            s_lruv_cache *cache = table->m_lruv_cache.m_cache;
            s_lruv_block *other_entry =
                &((s_lruv_block *)((s_data_array *)cache->header.block_data)->data)[definition];
            other_entry->flags &= (unsigned short)~2;
        }
    }
    long tailA = resource_handle & 0xffff;
    long tailB = resource_handle & 0xffff;
    unsigned long *bits2 = m_available.m_flags;
    bits2[tailA >> 5] = bits2[tailA >> 5] & ~(1 << (tailB & 31));
    unsigned long *bits4 = m_optional.m_flags;
    bits4[tailB >> 5] = bits4[tailB >> 5] & ~(1 << (tailA & 31));
}



// @address 0x864a8 tag_resource_cache_get_resource_cache_data
extern "C" c_basic_buffer<void> *tag_resource_cache_get_resource_cache_data(c_basic_buffer<void> *out, c_tag_resource_control_datum *datum)
{
    c_tag_resource_cache_controller *controller = datum->owner_info->m_controller;
    long handle = datum->handle;
    c_basic_buffer<void> cache_storage(c_basic_buffer<void>::_uninitialized);
    c_basic_buffer<void> if_storage(c_basic_buffer<void>::_uninitialized);
    c_basic_buffer<void> *source;
    if (datum->enabled) {
long salt = handle >> 16;
unsigned long index = (unsigned long)(handle ^ (salt << 16));
long external = ((((unsigned short)~salt) | 0x8000) << 16) | ((index >> 1) & 0x7fff);
        long control = ((~(external >> 16) & 0x7fff) << 16) | (((external << 1) & 0x1fffe) + 1);
        ((long (*)(c_basic_buffer<void> *, c_tag_resource_cache_controller *, long))tag_resource_cache_get_resource_page_buffer)(
            &if_storage, controller, control);
        source = &if_storage;
    } else {
        source = ((c_basic_buffer<void> * (*)(c_basic_buffer<void> *, c_tag_resource_cache_controller *, long))
                tag_resource_cache_get_resource_page_buffer)(&cache_storage, controller, handle);
    }
    cache_storage = *source;
    c_resource_buffer_handler *handler = datum->handler;
    long external_handle = datum->external_handle;
    unsigned long optional_offset = ((c_tag_resource_cache_file_datum_handler *)handler)->get_pageable_offset(external_handle);
    c_basic_buffer<void> sub(c_basic_buffer<void>::_uninitialized);
    c_basic_buffer<void> *selected;
    if (optional_offset != 0) {
        sub.m_address = (char *)cache_storage.m_address + optional_offset;
        sub.m_size = cache_storage.m_size - optional_offset;
        selected = &sub;
    } else {
        selected = &cache_storage;
    }
    *out = *selected;
    return out;
}
static __forceinline long tag_resource_cache_control_get_required_handle(const long &handle)
{
    int hi = handle >> 16;
    unsigned short salt = (unsigned short)~hi;
    long external = (0x80000000 | ((salt << 16) & 0x7fff0000)) | ((((unsigned long)handle >> 1)) & 0x7fff);
    return ((~(external >> 16) & 0x7fff) << 16) | (((external << 1) & 0x1fffe) + 1);
}

static __forceinline bool tag_resource_cache_control_page_loaded(c_tag_resource_cache_controller *controller, s_tag_resource_control_datum_data *datum)
{
    unsigned short definition = (unsigned short)datum->page_handle;
    c_tag_resource_page_table_control_interface *table = controller->m_page_table;
    s_data_array *pages = table->m_table->m_pages;
    s_tag_resource_page_datum *entry = &((s_tag_resource_page_datum *)pages->data)[definition];
    return entry->state >= 3;
}

static __forceinline bool tag_resource_cache_control_optional_bit(long handle)
{
    bool optional = (((unsigned short)(handle >> 16) & 0x8000) != 0);
    return optional;
}
static __forceinline tag_resource_cache_lookup_result tag_resource_cache_control_lookup_resource(c_tag_resource_cache_controller *controller, long handle)
{
    tag_resource_cache_lookup_result result;
    long salt = handle >> 16;
    result.optional = tag_resource_cache_control_optional_bit(handle);
    result.data = &((s_tag_resource_control_datum_data *)controller->m_control_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(handle)];
    return result;
}
// @address 0x85eb8 ?all_pages_loaded@c_tag_resource_cache_controller@@AAA_NJ@Z
bool c_tag_resource_cache_controller::all_pages_loaded(long resource_handle)
{
    tag_resource_cache_lookup_result resource = tag_resource_cache_control_lookup_resource(this, resource_handle);
    s_tag_resource_control_datum_data *datum = resource.data;
    bool optional = resource.is_optional();
    bool result = true;
    if ((datum->flags & 0x20) != 0) {
    if (optional != 0) {
        long required_handle = tag_resource_cache_control_get_required_handle(resource_handle);
        s_tag_resource_control_datum_data *required_datum = (s_tag_resource_control_datum_data *)(m_control_data->data + ((required_handle << 3) & 0x7fff8));
        result = tag_resource_cache_control_page_loaded(this, required_datum) && tag_resource_cache_control_page_loaded(this, datum);
    } else {
        result = tag_resource_cache_control_page_loaded(this, datum);
    }
    }
    return result;
}

static __forceinline bool tag_resource_cache_control_reservation_sufficient(c_tag_resource_page_table_control_interface *pages, long handle, long reservation)
{
    return ((s_tag_resource_page_datum *)pages->m_table->m_pages->data)[(unsigned short)handle].io_references >= reservation;
}
static __forceinline void tag_resource_set_wrapped_flag(c_wrapped_flags *flags, const long &index, bool enabled)
{
    unsigned long *bits = flags->m_flags;
    if (enabled)
        bits[index >> 5] |= 1 << (index & 31);
    else
        bits[index >> 5] &= ~(1 << (index & 31));
}
// @address 0x2ae970 tag_resource_cache_update_resource_reservation
extern "C" void tag_resource_cache_update_resource_reservation(c_tag_resource_cache_controller *controller, long handle)
{
    c_tag_resource_control_datum resource(controller->m_can_predict_optional, handle, &controller->m_datum_info,
        (s_tag_resource_control_datum_data *)controller->m_control_data->data + (handle & 0xffff),
        (c_resource_buffer_handler *)controller->m_file_datum_handler);
    if (resource.test_flag_5())
    {
        long reservation = resource.get_page_reservation();
        c_tag_resource_page_table_control_interface *pages = (c_tag_resource_page_table_control_interface *)controller->m_page_table;
        long page_handle = resource.data->page_handle;
        if (!tag_resource_cache_control_reservation_sufficient(pages, page_handle, reservation))
            pages->set_page_reservation(page_handle, (e_tag_resource_page_reservation)reservation);
        resource.data->flags |= 0x40;
    }
    tag_resource_set_wrapped_flag(&controller->m_available, resource.handle & 0xffff, !resource.unneeded());
    tag_resource_set_wrapped_flag(&controller->m_optional, resource.handle & 0xffff, resource.unneeded());
}

static __forceinline long tag_resource_cache_control_try_get_page_index(c_tag_resource_cache_controller *controller, long resource_handle)
{
    s_tag_resource_control_datum_data *datum = &((s_tag_resource_control_datum_data *)controller->m_control_data->data)[(unsigned short)resource_handle];
    if ((datum->flags & 0x20) != 0)
    {
        unsigned short definition = (unsigned short)datum->page_handle;
        c_tag_resource_page_table *table = controller->m_page_table->m_table;
        long reservation = -1;
        if (((s_tag_resource_page_datum *)table->m_pages->data)[definition].state >= 3)
            reservation = ((s_lruv_block *)((s_data_array *)table->m_lruv_cache.m_cache->header.block_data)->data)[definition].page_index;
        return reservation;
    }
    return -1;
}

static __forceinline long tag_resource_cache_control_required_handle(long external_handle)
{
    unsigned long salt = ~(external_handle >> 16) & 0x7fff;
    unsigned long absolute_index = ((external_handle & 0xffff) << 1) + 1;
    return (salt << 16) | absolute_index;
}

static __forceinline s_tag_resource_page_reservations tag_resource_cache_control_reservation_set_required(
    s_tag_resource_page_reservations original, long required)
{
    s_tag_resource_page_reservations result = original;
    result.required_reservation = required;
    return result;
}
static __forceinline long tag_resource_cache_control_reservation_external_handle(long handle) { return c_tag_resource_cache_internal_handle(handle).external; }
// @address 0x87c80 tag_resource_cache_build_resource_address
extern "C" s_tag_resource_page_reservations tag_resource_cache_build_resource_address(const c_tag_resource_control_datum *resource)
{
    s_tag_resource_page_reservations reservations;
    reservations.salt = (__int64)resource->external_handle >> 16;
    reservations.index = resource->external_handle;
    if (resource->enabled)
    {
        c_tag_resource_cache_controller *controller = resource->owner_info->m_controller;
        reservations.optional_reservation = tag_resource_cache_control_try_get_page_index(controller, resource->handle);
        long required_handle = tag_resource_cache_control_required_handle(tag_resource_cache_control_reservation_external_handle((resource->handle)));
        reservations = tag_resource_cache_control_reservation_set_required(
            reservations, tag_resource_cache_get_page_reservation(controller, required_handle));
    }
    else
    {
        reservations.optional_reservation = -1;
        reservations.required_reservation =
            tag_resource_cache_get_page_reservation(resource->owner_info->m_controller, resource->handle);
    }
    return reservations;
}

// @address 0x2ae850 ?release_resource_page_references@c_tag_resource_cache_controller@@AAAXJPAVc_tag_resource_address_cache_control_interface@@@Z
void c_tag_resource_cache_controller::release_resource_page_references(
    long resource_handle, c_tag_resource_address_cache_control_interface *address_cache_control)
{
    c_tag_resource_control_datum resource(m_can_predict_optional, resource_handle, &m_datum_info,
        (s_tag_resource_control_datum_data *)m_control_data->data + (resource_handle & 0xffff),
        (c_resource_buffer_handler *)m_file_datum_handler);
    if (resource.deferred())
    {
        if (m_pending.test(resource.get_absolute_internal_index()))
            m_pending.set(resource.get_absolute_internal_index(), false);
    }
    address_cache_control->staleify_resource_address(resource_handle);
    resource_prepare_for_unloading(resource_handle);
    resource_tear_down_references(resource_handle);
}

// @address 0x2ada78 ?prepare_for_disposal_from_old_map@c_tag_resource_cache_controller@@QAAXPAVc_tag_resource_address_cache_control_interface@@@Z
void c_tag_resource_cache_controller::prepare_for_disposal_from_old_map(c_tag_resource_address_cache_control_interface *old_map)
{
    c_data_iterator<void> iterator;
    iterator.begin(m_control_data);
    while (iterator.next()) {
        long index = iterator.get_index();
        long cache_index = index & 0xffff;
        s_tag_resource_control_datum_data *datum = (s_tag_resource_control_datum_data *)(m_control_data->data + (cache_index << 3));
        unsigned char *flags = &datum->flags;
        datum->flags &= ~2;
        *flags &= ~4;
        datum->flags &= ~8;

        release_page_reservation(index);
        old_map->mark_unrequired_resource(cache_index);
    }
    orphan_unnecessary_unlocked_resources(old_map);
}

// @address 0x2ae110 ?update_required_resources@c_tag_resource_cache_controller@@QAAXPAVc_tag_resource_address_cache_control_interface@@@Z
void c_tag_resource_cache_controller::update_required_resources(class c_tag_resource_address_cache_control_interface *address_cache_control)
{
    c_data_iterator<void> iterator;
    iterator.begin(m_control_data);
    bool fixed_up = false;
    while (iterator.next()) {
        long resource_handle = iterator.get_index();
        c_tag_resource_control_datum resource(m_can_predict_optional, resource_handle, &m_datum_info,
            (s_tag_resource_control_datum_data *)m_control_data->data + (resource_handle & 0xffff),
            (c_resource_buffer_handler *)m_file_datum_handler);
        if (resource.always_required())
            tag_resource_cache_update_resource_address(this, true, resource_handle, &fixed_up, address_cache_control);
    }
}

// @address 0x2ae1f8 ?resource_prepare_for_unloading@c_tag_resource_cache_controller@@AAAXJ@Z
void c_tag_resource_cache_controller::resource_prepare_for_unloading(long resource_handle)
{
    c_tag_resource_control_datum resource = this->get_resource_control_from_handle(resource_handle);
    if (resource.data->state == 4) {
        void *root = ((c_tag_resource_control_datum *)&resource)->get_root_address();
        if (!resource.test_flag(4)) {
            c_tag_resource_cache_file_datum_handler *resource_handler = (c_tag_resource_cache_file_datum_handler *)resource.handler;
            long resource_type = resource_handler->get_resource_definition(resource.external_handle);
            long owner = resource_handler->get_resource_owner(resource.external_handle);
            m_resource_listener->unregister_resource(owner, resource_type, root);
        }
        tag_cache_ctl_set_state(&resource, 3);
    }
}


static __forceinline void tag_resource_tear_down_cache_data_references(s_tag_resource_fixup_setup *setup)
{
    if (setup->data_to_fixup.m_size != 0) {
        c_basic_buffer<void> data_to_fixup = setup->data_to_fixup;
        long fixup_count = setup->fixups.count();
        if (fixup_count > 0) {
            const s_tag_resource_fixup_location *fixup = setup->fixups.begin();
            unsigned long remaining = (unsigned long)fixup_count;
            do {
                *data_to_fixup.get_as_type_from_offset<unsigned long>(fixup->offset) = 0;
                fixup++;
                remaining--;
            } while (remaining != 0);
        }
    }
}

static __forceinline c_basic_buffer<void> tag_resource_cache_fetch_data(c_tag_resource_control_datum *resource)
{
    c_basic_buffer<void> data(c_basic_buffer<void>::_uninitialized);
    tag_resource_cache_get_resource_cache_data(&data, resource);
    return data;
}
static __forceinline c_tag_resource_control_datum tag_resource_cache_control_get_resource(
    c_tag_resource_cache_controller *const &controller, const long &handle)
{
    s_tag_resource_control_datum_data *datum = &((s_tag_resource_control_datum_data *)controller->m_control_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(handle)];
    return c_tag_resource_control_datum(controller->m_can_predict_optional, handle, &controller->m_datum_info,
        datum, (c_resource_buffer_handler *)controller->m_file_datum_handler);
}
static __forceinline void tag_resource_cache_control_unpin_resource_pages(c_tag_resource_cache_controller *controller, long handle)
{
    c_tag_resource_control_datum resource = tag_resource_cache_control_get_resource(controller, handle);
    const unsigned long &flags = resource.data->flags;
    if ((flags & 0x20) != 0) {
        c_tag_resource_page_table_control_interface *pages = (c_tag_resource_page_table_control_interface *)controller->m_page_table;
        if (resource.enabled) {
            long required_index = (resource.external_handle << 1) + 1;
            s_tag_resource_control_datum_data *required_datum =
                (s_tag_resource_control_datum_data *)(controller->m_control_data->data + ((required_index << 3) & 0x7fff8));
            pages->unpin_page(required_datum->page_handle);
            ((c_tag_resource_page_table_control_interface *)controller->m_page_table)->unpin_page(resource.data->page_handle);
        }
        else
            ((c_tag_resource_page_table_control_interface *)controller->m_page_table)->unpin_page(resource.data->page_handle);
    }
}
// @address 0x2ae338 ?resource_tear_down_references@c_tag_resource_cache_controller@@AAAXJ@Z
void c_tag_resource_cache_controller::resource_tear_down_references(long resource_handle)
{
    long resource_handle_loc = resource_handle;
    if (resource_handle_loc != -1) {
        c_tag_resource_control_datum resource = tag_resource_cache_control_get_resource(this, resource_handle_loc);
        c_tag_resource_cache_file_datum_handler *handler = (c_tag_resource_cache_file_datum_handler *)resource.handler;
        long external_handle = resource.external_handle;
        bool optional = resource.enabled;
        if (resource.data->state == 3) {
            if ((resource.data->flags & 0x10) == 0) {
                s_tag_resource_interop_fixup_setup interop_setup = {
                    handler->get_control_data(external_handle, optional),
                    tag_resource_cache_fetch_data(&resource),
                    resource.try_to_get_optional_data(),
                    handler->get_interop_buffer(external_handle, optional),
                    handler->get_interop_locations(external_handle) };
                tag_resource_teardown_interops(&interop_setup);
                s_tag_resource_fixup_setup fixup_setup = {
                    handler->get_control_data(external_handle, optional),
                    tag_resource_cache_fetch_data(&resource),
                    handler->get_control_fixups(external_handle) };
                tag_resource_tear_down_cache_data_references(&fixup_setup);
            }
            {
                long old_state = resource.data->state;
                resource.data->state = 2;
                resource.owner_info->m_controller->record_new_resource_state(resource.handle, old_state, 2);
            }
            tag_resource_cache_control_unpin_resource_pages(this, resource_handle_loc);
            {
                short old_state = resource.data->state;
                resource.data->state = 1;
                resource.owner_info->m_controller->record_new_resource_state(resource.handle, old_state, 1);
            }
        }
    }
}

static __forceinline long tag_resource_cache_control_internal_handle_from_external(long external)
{
    return ((~(external >> 16) & 0x7fff) << 16) | (((external << 1) & 0x1fffe) + 1);
}
static __forceinline void tag_resource_cache_control_pin_resource_pages(
    c_tag_resource_cache_controller *controller, long handle, s_tag_resource_control_datum_data *datum_data)
{
    c_tag_resource_control_datum resource = tag_resource_cache_control_get_resource(controller, handle);
    if (resource.enabled) {
        long required_handle = tag_resource_cache_control_internal_handle_from_external(resource.external_handle);
        s_tag_resource_control_datum_data *required_datum = (s_tag_resource_control_datum_data *)controller->m_control_data->data + (required_handle & 0xffff);
        ((c_tag_resource_page_table_control_interface *)controller->m_page_table)->pin_page(required_datum->page_handle);
        ((c_tag_resource_page_table_control_interface *)controller->m_page_table)->pin_page(datum_data->page_handle);
    } else {
        ((c_tag_resource_page_table_control_interface *)controller->m_page_table)->pin_page(datum_data->page_handle);
    }
}
static __forceinline void tag_resource_cache_control_rebuild_set_state(c_tag_resource_control_datum *resource, s_tag_resource_control_datum_data *datum_data)
{
    long old_state = datum_data->state;
    datum_data->state = 3;
    if (old_state < 3)
        return;
    if (old_state > 3)
    {
        if (old_state == 4)
        {
            long index = resource->handle & 0xffff;
            long word = index >> 5;
            long shift = index & 31;
            long mask = 1 << shift;
            c_tag_resource_cache_controller *controller = resource->owner_info->m_controller;
            unsigned long *bits = controller->m_requested.m_flags;
            bits[word] &= ~mask;
        }
    }
}
// @address 0x86140 ?resource_rebuild_references@c_tag_resource_cache_controller@@AAA_NJ@Z
bool c_tag_resource_cache_controller::resource_rebuild_references(long resource_handle)
{
    if (resource_handle != -1) {
        bool cache_enabled = m_can_predict_optional;
        s_tag_resource_control_datum_data *datum_data = (s_tag_resource_control_datum_data *)m_control_data->data + (resource_handle & 0xffff);
        c_tag_resource_control_datum resource(cache_enabled, resource_handle, &m_datum_info, datum_data,
            (c_resource_buffer_handler *)m_file_datum_handler);
        if (datum_data->state == 2) {
            if (datum_data->flags & 0x20)
                tag_resource_cache_control_pin_resource_pages(this, resource_handle, datum_data);
            if (!(datum_data->flags & 0x10)) {
                c_tag_resource_cache_file_datum_handler *handler = (c_tag_resource_cache_file_datum_handler *)resource.handler;
                s_tag_resource_fixup_setup fixup_setup = {
                    handler->get_control_data(resource.external_handle, resource.enabled),
                    tag_resource_cache_fetch_data(&resource),
                    handler->get_control_fixups(resource.external_handle) };
                if (resource.enabled) {
                    c_basic_buffer<void> optional_data = resource.get_cache_data();
                    tag_resource_fixup_cache_data_references(&fixup_setup, &optional_data);
                } else {
                    tag_resource_fixup_cache_data_references(&fixup_setup, 0);
                }
                s_tag_resource_interop_fixup_setup interop_setup = {
                    handler->get_control_data(resource.external_handle, resource.enabled),
                    tag_resource_cache_fetch_data(&resource),
                    resource.try_to_get_optional_data(),
                    handler->get_interop_buffer(resource.external_handle, resource.enabled),
                    handler->get_interop_locations(resource.external_handle) };
                tag_resource_fixup_interops(&interop_setup);
            }
            tag_resource_cache_control_rebuild_set_state(&resource, datum_data);
        }
    }
    return false;
}



static __forceinline bool tag_resource_cache_register_resource(
    c_tag_resource_cache_controller *controller, const c_tag_resource_control_datum &resource, void *root)
{
    c_tag_resource_cache_file_datum_handler *file_handler = (c_tag_resource_cache_file_datum_handler *)resource.handler;
    long resource_type = file_handler->get_resource_definition(resource.external_handle);
    long owner = file_handler->get_resource_owner(resource.external_handle);
    return controller->m_resource_listener->register_resource(owner, resource_type, root);
}

// @address 0x85fd8 ?resource_update_state_to_valid@c_tag_resource_cache_controller@@AAAXJ@Z
void c_tag_resource_cache_controller::resource_update_state_to_valid(long resource_handle)
{
    c_tag_resource_control_datum resource(m_can_predict_optional, resource_handle, &m_datum_info,
        (s_tag_resource_control_datum_data *)m_control_data->data + (resource_handle & 0xffff),
        (c_resource_buffer_handler *)m_file_datum_handler);
    if ((resource.data->flags & 1) != 0)
        return;
    if (resource.data->state == 2)
        resource_rebuild_references(resource_handle);
    if (resource.data->state != 3)
        return;
    void *root = resource.get_root_address();
    bool valid = (resource.data->flags & 0x10) != 0 || tag_resource_cache_register_resource(this, resource, root);
    if (valid) {
        resource.set_state(_tag_resource_control_datum_state_active);
        return;
    }
    resource_decommit(resource_handle);
    resource.data->flags |= 1;
}

static __forceinline bool tag_resource_cache_control_valid_state(const c_tag_resource_control_datum *resource)
{
    return resource->data->state >= 4;
}

// @address 0x85c20 tag_resource_cache_update_resource_address
extern "C" void tag_resource_cache_update_resource_address(c_tag_resource_cache_controller *controller, bool required, long resource_handle,
    bool *fixed_up, c_tag_resource_address_cache_control_interface *address_cache_control)
{
    c_tag_resource_control_datum resource(controller->m_can_predict_optional, resource_handle, &controller->m_datum_info,
        (s_tag_resource_control_datum_data *)controller->m_control_data->data + (resource_handle & 0xffff),
        (c_resource_buffer_handler *)controller->m_file_datum_handler);
    if (required)
    {
        if (resource.data->state == 1 && c_tag_resource_cache_controller::_symfix_all_pages_loaded(controller, resource_handle))
            resource.data->state = 2;
        if (resource.data->state == 2 || resource.data->state == 3)
            c_tag_resource_cache_controller::_symfix_resource_update_state_to_valid(controller, resource_handle);
        if (tag_resource_cache_control_valid_state(&resource) && !resource.unneeded())
        {
            bool was_published = address_cache_control->resource_address_published(resource_handle);
            if (resource.deferred())
                controller->m_pending.set(resource.get_absolute_internal_index(), true);
            resource.get_root_address();
            s_tag_resource_page_reservations address = tag_resource_cache_build_resource_address((const c_tag_resource_control_datum *)&resource);
            address_cache_control->publish_resource_address(resource_handle, address);
            if (!was_published)
                *fixed_up = true;
        }
    }
    else if (resource.deferred())
    {
        bool was_published = address_cache_control->resource_address_published(resource_handle);
        controller->m_pending.set(resource.get_absolute_internal_index(), false);
        address_cache_control->staleify_resource_address(resource_handle);
        if (was_published)
            *fixed_up = true;
    }
}

// fixup_available_and_necessary_resources: unfix tracked resources that are published but no
// longer needed, publish the needed ones that are not, fix up pending resources that became
// available, then scan for any remaining published/needed mismatch (its ASSERT is stripped in
// release, leaving only the scan).
// @address 0x857a8 tag_resource_cache_reconcile_published_resources
extern "C" void tag_resource_cache_reconcile_published_resources(
    c_tag_resource_cache_controller *controller, bool *any_changed, c_tag_resource_address_cache_control_interface *address_cache_control)
{
    const c_wrapped_flags &published_flags = address_cache_control->m_data->m_available_addresses;
    const c_wrapped_flags &available_flags = controller->m_pending_release;
    const c_wrapped_flags &desired_flags = controller->m_predicted;
    const c_wrapped_flags &optional_flags = controller->m_required;
    {
        c_bit_vector_operand tracked(optional_flags);
        c_bit_vector_operand published(published_flags);
        c_bit_vector_operand desired(desired_flags);
        c_bit_vector_operand available(available_flags);
        c_tag_resource_published_unneeded_expression expression(
            c_bit_vector_and<c_bit_vector_operand, c_bit_vector_operand>(tracked, published),
            c_tag_resource_needed_expression(c_bit_vector_or<c_bit_vector_operand, c_bit_vector_constant>(desired,
                controller->m_can_load_all_desired_optional), available));
        c_bit_vector_expression_iterator iterator(controller->m_control_data->first_unallocated);
        long absolute_index;
        while (iterator.next(expression, &absolute_index))
        {
            bool fixed_up = false;
            tag_resource_cache_update_resource_address(controller, false,
                datum_absolute_index_to_index(controller->m_control_data, absolute_index), &fixed_up,
                address_cache_control);
            *any_changed = *any_changed || fixed_up;
        }
    }
    {
        c_bit_vector_operand tracked(optional_flags);
        c_bit_vector_operand published(published_flags);
        c_bit_vector_operand desired(desired_flags);
        c_bit_vector_operand available(available_flags);
        c_tag_resource_unpublished_needed_expression expression(
            c_bit_vector_and<c_bit_vector_and<c_bit_vector_or<c_bit_vector_operand, c_bit_vector_constant>, c_bit_vector_not<c_bit_vector_operand> >,
                c_bit_vector_operand>(
                c_bit_vector_and<c_bit_vector_or<c_bit_vector_operand, c_bit_vector_constant>, c_bit_vector_not<c_bit_vector_operand> >(
                    c_bit_vector_or<c_bit_vector_operand, c_bit_vector_constant>(desired, controller->m_can_load_all_desired_optional), published),
                available),
            tracked);
        c_bit_vector_expression_iterator iterator(controller->m_control_data->first_unallocated);
        long absolute_index;
        while (iterator.next(expression, &absolute_index))
        {
            bool fixed_up = false;
            tag_resource_cache_update_resource_address(controller, true,
                datum_absolute_index_to_index(controller->m_control_data, absolute_index), &fixed_up,
                address_cache_control);
            *any_changed = *any_changed || fixed_up;
        }
    }
    {
        c_bit_vector_operand requested(controller->m_requested);
        c_bit_vector_operand pending(controller->m_optional);
        c_bit_vector_operand available(available_flags);
        c_tag_resource_available_unrequested_expression expression(
            c_bit_vector_and<c_bit_vector_operand, c_bit_vector_not<c_bit_vector_operand> >(pending, requested),
            available);
        c_bit_vector_expression_iterator iterator(controller->m_control_data->first_unallocated);
        long absolute_index;
        while (iterator.next(expression, &absolute_index))
        {
            bool fixed_up = false;
            tag_resource_cache_update_resource_address(controller, true,
                datum_absolute_index_to_index(controller->m_control_data, absolute_index), &fixed_up,
                address_cache_control);
        }
    }
    {
        c_bit_vector_operand available(available_flags);
        c_bit_vector_operand tracked(optional_flags);
        c_bit_vector_operand published(published_flags);
        c_bit_vector_operand desired(desired_flags);
        c_tag_resource_mispublished_expression expression(tracked,
            c_bit_vector_xor<c_bit_vector_operand, c_tag_resource_needed_expression>(published,
                c_tag_resource_needed_expression(c_bit_vector_or<c_bit_vector_operand, c_bit_vector_constant>(desired,
                    controller->m_can_load_all_desired_optional), available)));
        c_bit_vector_expression_iterator iterator(controller->m_control_data->first_unallocated);
        long absolute_index;
        iterator.next(expression, &absolute_index);
    }
}


// @address 0x2adb48 tag_resource_cache_update_resources_from_active_set
extern "C" void tag_resource_cache_update_resources_from_active_set(c_tag_resource_cache_controller *controller,
    c_tag_resource_runtime_active_set *active_set, c_tag_resource_address_cache_control_interface *address_cache_control, bool *changed)
{
    c_data_iterator<void> iterator;
    iterator.begin(controller->m_control_data);
    bool new_resources = false;
    while (iterator.next())
    {
        long resource_handle = iterator.get_index();
        c_tag_resource_control_datum resource = tag_resource_cache_control_get_resource(controller, resource_handle);
        bool was_necessary = resource.test_flag(1);
        bool was_unneeded = resource.test_flag(3);
        unsigned char *flags = &resource.data->flags;
        *flags &= ~2;
        resource.set_flag(2, false);
        resource.set_flag(3, false);
        long external_handle = resource.external_handle;
        long owner = resource.handler->get_resource_owner(external_handle);
        bool necessary = active_set->is_resource_required(owner, external_handle);
        bool deferred = active_set->is_resource_deferred(owner, external_handle);
        bool unneeded = active_set->is_resource_unneeded(owner, external_handle);
        resource.set_flag(1, necessary);
        resource.set_flag(2, deferred);
        resource.set_flag(3, resource.test_flag_5() && unneeded);
        if (resource.test_flag_5() && !was_necessary && !was_unneeded && (necessary || unneeded))
            new_resources = true;
        if (resource.unneeded())
        {
            address_cache_control->staleify_resource_address(resource_handle);
            tag_resource_cache_update_resource_reservation(controller, resource_handle);
        }
        else if (resource.deferred())
        {
            tag_resource_cache_update_resource_reservation(controller, resource_handle);
        }
        else if (resource.active())
        {
            tag_resource_cache_update_resource_reservation(controller, resource_handle);
        }
        else
        {
            address_cache_control->staleify_resource_address(resource_handle);
            c_wrapped_flags tracking_bits;
            tracking_bits.m_flags = controller->m_pending.m_flags;
            tracking_bits.m_count = controller->m_pending.m_count;
            if (tracking_bits.test(resource.get_absolute_internal_index()))
                tracking_bits.set(resource.get_absolute_internal_index(), false);
            c_tag_resource_cache_controller::_symfix_release_page_reservation(controller, resource_handle);
        }
        if (resource.always_required())
            address_cache_control->m_data->m_required_resources.set(resource.get_absolute_internal_index(), true);
        else
            address_cache_control->mark_unrequired_resource(resource.get_absolute_internal_index());
    }
    *changed = new_resources;
}
