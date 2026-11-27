// blamlibxbox/tag_resource_cache_address_cache.obj: from the records 0x880c0 0x2aeef0 0x2af1e8 0x2af2b8 0x2af2d0 0x2af388
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x880c0 ?populate_access_cache@c_tag_resource_address_cache@@QAAXPAVc_tag_resource_cache_file_access_cache@@_N@Z
// @address 0x2af1e8 ?initialize_resource_address@c_tag_resource_address_cache@@QAAXJPAX@Z
// @address 0x2af2b8 ?get_unaligned_size@?$c_wrapped_array_allocation_description@Us_cached_resource_state@@@@UBAKXZ
// @address 0x2af2d0 ?assign_storage@?$c_wrapped_array_allocation_description@Us_cached_resource_state@@@@UBAXAAVc_raw_storage_buffer_iterator@@@Z
// @address 0x2af388 ?assign_storage@?$c_wrapped_array_allocation_description@Us_published_resources_state@c_tag_resource_address_cache@@@@UBAXAAVc_raw_storage_buffer_iterator@@@Z
#include "cseries/cseries.h"
#include "cseries/crt_xdk.h"
#include "cache/tag_resource_cache_internal_handle.h"
#include "cache/tag_resource_cache_address_cache.h"
#include "memory/data.h"
#include "memory/raw_storage_buffer.h"
#include "memory/allocation_description.h"
#include "memory/basic_buffer.h"
#include "cache/tag_resource_cache_address_cache_allocation.h"
#include "memory/wrapped_flags.h"
#include <xutility>
#include <new>
#include "game/cached_resource_state_types.h"

#include "memory/wrapped_array_allocation_description.h"
template <>
unsigned long c_wrapped_array_allocation_description<s_cached_resource_state>::get_unaligned_size() const {
    return m_element_count * sizeof(s_cached_resource_state) + __alignof(s_cached_resource_state) - 1;
}
template <>
void c_wrapped_array_allocation_description<s_cached_resource_state>::assign_storage(c_raw_storage_buffer_iterator &iterator) const {
    iterator.set_wrapped_array<s_cached_resource_state>(*m_wrapped_array, m_element_count);
}
template <>
void c_wrapped_array_allocation_description<c_tag_resource_address_cache::s_published_resources_state>::assign_storage(
    c_raw_storage_buffer_iterator &iterator) const
{
    iterator.set_wrapped_array<c_tag_resource_address_cache::s_published_resources_state>(*m_wrapped_array, m_element_count);
}

static __forceinline bool tag_resource_cache_handle_is_optional(long handle)
{
    return c_tag_resource_cache_internal_handle(handle).optional;
}

static __forceinline long tag_resource_cache_handle_get_external_handle(long handle)
{
    return c_tag_resource_cache_internal_handle(handle).external;
}

typedef char tag_resource_cache_file_access_cache_size[(sizeof(void *) != 4 || sizeof(c_tag_resource_cache_file_access_cache) == 0x28) ? 1 : -1];

__forceinline void c_wrapped_flags::set_bits(c_basic_buffer<void> buffer, unsigned long bit_count)
{
    set_bits(buffer.get_buffer(), bit_count);
}

template <typename T>
static __forceinline void tag_resource_cache_address_cache_fill_array(c_wrapped_array<T> *array, const T &value)
{
    std::fill(array->begin(), array->end(), value);
}

extern "C" {
// @address 0x41644 k_tag_resource_address_cache_name
// @rdata 0x82041644..0x82041660
const char k_tag_resource_address_cache_name[28] = "tag resource address cache";
// @rdata-end
}

// @address 0x2a62f0 ?get_unaligned_size@c_buffer_allocation_description@@UBAKXZ
// @address 0x2a6320 ?assign_storage@c_buffer_allocation_description@@UBAXAAVc_raw_storage_buffer_iterator@@@Z
// @address 0x2a6360 ?get_unaligned_size@c_wrapped_flags_allocation_description@@UBAKXZ
// @address 0x2a6378 ?assign_storage@c_wrapped_flags_allocation_description@@UBAXAAVc_raw_storage_buffer_iterator@@@Z
// @address 0x2af320 ?get_unaligned_size@?$c_wrapped_array_allocation_description@_J@@UBAKXZ
// @address 0x2af330 ?assign_storage@?$c_wrapped_array_allocation_description@_J@@UBAXAAVc_raw_storage_buffer_iterator@@@Z
// @address 0x2af378 ?get_unaligned_size@?$c_wrapped_array_allocation_description@Us_published_resources_state@c_tag_resource_address_cache@@@@UBAKXZ

void c_tag_resource_address_cache::initialize_resource_address(long handle, void *address)
{
    long external_handle = tag_resource_cache_handle_get_external_handle(handle);
    long index = (unsigned short)external_handle;
    if (tag_resource_cache_handle_is_optional(handle))
        { s_cached_resource_state *s = m_states.begin() + index; s->optional_address = address; }
    else
        { s_cached_resource_state *s = m_states.begin() + index; s->required_address = address; }
    { s_cached_resource_state *s = m_states.begin() + index; s->handle = external_handle; }
}

// Callers always pass false for required_only.
void c_tag_resource_address_cache::populate_access_cache(c_tag_resource_cache_file_access_cache *access_cache, bool required_only)
{
    tag_resource_cache_address_cache_fill_array<void *>(&access_cache->m_resource_values, 0);
    tag_resource_cache_address_cache_fill_array<long>(&access_cache->m_resource_handles, -1);
    tag_resource_cache_address_cache_fill_array<__int64>(&access_cache->m_resource_pages, -1);
    tag_resource_cache_address_cache_fill_array<unsigned long>(&access_cache->m_resource_flags, 0);

    s_published_resources_state *published_state = find_or_add_cached_published_state(access_cache);
    bool &published = published_state->published;
    published = true;
    published_state->available.clear();
    published_state->required.clear();
    published_state->unknown.clear();

    for (s_cached_resource_state *state = m_states.begin(), *states_end = m_states.end(); state < states_end; state++)
    {
        // match: 0x880c0: the _rotl(x, 0) handle test keeps the or of the published flags as or r27,r10,r27
        bool resource_is_valid = (long)_rotl(state->handle, 0) != NONE;
        if (!resource_is_valid)
            continue;

        long handle = state->handle;
        long optional_handle = tag_resource_cache_make_internal_handle(state->handle, true);
        long required_handle = tag_resource_cache_make_internal_handle(state->handle, false);
        bool optional_available = is_resource_address_available(optional_handle);
        if (optional_available)
        {
            long required_index = (unsigned short)required_handle;
            long optional_index = (unsigned short)optional_handle;
            published_state->mark_required(required_index);
            published_state->available.set(optional_index, true);
            access_cache->set_resource(state->handle, state->optional_address, m_pages.begin()[optional_index]);
        }
        else if (is_resource_address_available(required_handle))
        {
            unsigned long required_index = (unsigned short)required_handle;
            published_state->mark_required(required_index);
            access_cache->set_resource(state->handle, state->required_address, m_pages.begin()[required_index]);
        }

        if (optional_available)
            access_cache->set_resource_flag(handle, 0);
        if (is_external_resource_demanded(handle))
            access_cache->set_resource_flag(handle, 1);
    }
}


// @address 0x2aeef0
void c_tag_resource_address_cache::initialize_for_new_map(
    unsigned long count, unsigned long maximum_count, unsigned long published_count, c_allocation_base *allocator)
{
    unsigned long internal_count = count * 2;
    m_allocator = allocator;
    c_basic_buffer<void> published_bits;
    c_basic_buffer<void> published_pages;
    s_allocation_description_holder descriptions[] = {
        { c_wrapped_array_allocation_description<s_cached_resource_state>(m_states, count) },
        { c_wrapped_array_allocation_description<__int64>(m_pages, internal_count) },
        { c_wrapped_array_allocation_description<c_tag_resource_address_cache::s_published_resources_state>(m_published_states, published_count) },
        { c_buffer_allocation_description(published_bits, internal_count * published_count * 2, 2) },
        { c_buffer_allocation_description(published_pages, maximum_count * published_count, 2) },
        { c_wrapped_flags_allocation_description(m_available_addresses, internal_count) },
        { c_wrapped_flags_allocation_description(m_required_resources, internal_count) },
        { c_wrapped_flags_allocation_description(m_demanded_resources, count) }
    };
    c_wrapped_array_no_init<s_allocation_description_holder> list_storage;
    c_wrapped_array<s_allocation_description_holder> *list = new (&list_storage) c_wrapped_array<s_allocation_description_holder>(descriptions);
    allocate_list_contiguous(allocator, *list, k_tag_resource_address_cache_name, &m_allocation, 0);

    unsigned long state_bytes = ((internal_count + 31) >> 5) * sizeof(unsigned long);
    unsigned long page_bytes = ((maximum_count + 31) >> 5) * sizeof(unsigned long);
    for (unsigned long index = 0; index < published_count; index++)
    {
        m_published_states.begin()[index].access_cache = 0;
        m_published_states.begin()[index].published = false;
        m_published_states.begin()[index].available.set_bits(c_basic_buffer<void>(published_bits, index * 2 * state_bytes, state_bytes), internal_count);
        m_published_states.begin()[index].required.set_bits(
            c_basic_buffer<void>(published_bits, (index * 2 + 1) * state_bytes, state_bytes), internal_count);
        m_published_states.begin()[index].unknown.set_bits(c_basic_buffer<void>(published_pages, index * page_bytes, page_bytes), maximum_count);
    }
    m_available_addresses.clear();
    m_required_resources.clear();
    m_demanded_resources.clear();
    s_cached_resource_state empty_state = { NONE, 0, 0 };
    tag_resource_cache_address_cache_fill_array(&m_states, empty_state);
    tag_resource_cache_address_cache_fill_array<__int64>(&m_pages, -1);
    m_controller = this;
}
