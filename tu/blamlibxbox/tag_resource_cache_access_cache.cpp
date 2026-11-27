// blamlibxbox/tag_resource_cache_access_cache.obj: from the records 0x2aeee0 0x2af320 0x2af330 0x2af378 0x2ffad0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "cseries/cseries_wrapped.h"
#include "memory/wrapped_arrays.h"
#include "memory/raw_storage_buffer.h"
#include "memory/allocation_description.h"
#include "cache/tag_resource_cache_address_cache.h"
// @address 0x2aeee0 ?get_unaligned_size@?$c_wrapped_array_allocation_description@V?$c_flags@W4e_tag_resource_access_state_bit@@K$01@@@@UBAKXZ
// @address 0x2af320 ?get_unaligned_size@?$c_wrapped_array_allocation_description@_J@@UBAKXZ
// @address 0x2af330 ?assign_storage@?$c_wrapped_array_allocation_description@_J@@UBAXAAVc_raw_storage_buffer_iterator@@@Z
// @address 0x2af378 ?get_unaligned_size@?$c_wrapped_array_allocation_description@Us_published_resources_state@c_tag_resource_address_cache@@@@UBAKXZ
// @address 0x2ffad0 ?assign_storage@?$c_wrapped_array_allocation_description@J@@UBAXAAVc_raw_storage_buffer_iterator@@@Z
enum e_tag_resource_access_state_bit {
    _tag_resource_access_state_available,
    _tag_resource_access_state_loaded
};

#include "memory/wrapped_array_allocation_description.h"
template <typename T>
static __forceinline long get_total_unaligned_raw_size(long element_count) {
    return element_count * sizeof(T) + __alignof(T) - 1;
}
template <>
unsigned long c_wrapped_array_allocation_description<c_flags<e_tag_resource_access_state_bit, unsigned long, 2> >::get_unaligned_size() const {
    long size = get_total_unaligned_raw_size<c_flags<e_tag_resource_access_state_bit, unsigned long, 2> >(m_element_count);
    return size;
}
template <>
unsigned long c_wrapped_array_allocation_description<__int64>::get_unaligned_size() const {
    long size = get_total_unaligned_raw_size<__int64>(m_element_count);
    return size;
}
template <>
void c_wrapped_array_allocation_description<__int64>::assign_storage(c_raw_storage_buffer_iterator &iterator) const {
    iterator.set_wrapped_array<__int64>(*m_wrapped_array, m_element_count);
}
template <>
unsigned long c_wrapped_array_allocation_description<c_tag_resource_address_cache::s_published_resources_state>::get_unaligned_size() const
{
    long size = get_total_unaligned_raw_size<c_tag_resource_address_cache::s_published_resources_state>(m_element_count);
    return size;
}
template <>
unsigned long c_wrapped_array_allocation_description<long>::get_unaligned_size() const {
    long size = get_total_unaligned_raw_size<long>(m_element_count);
    return size;
}
template <>
void c_wrapped_array_allocation_description<long>::assign_storage(c_raw_storage_buffer_iterator &iterator) const {
    iterator.set_wrapped_array<long>(*m_wrapped_array, m_element_count);
}
