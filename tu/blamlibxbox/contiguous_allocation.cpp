// hand-merged from the records 0x2a6108 0x2a6200 0x2a62f0 0x2a6320 0x2a6360 0x2a6378 0x2a63d0 0x2a6418
// @flags /TP /O2 /Oc
#include "memory/raw_storage_buffer.h"
#include "memory/allocation_description.h"
#include "memory/wrapped_arrays.h"
#include "memory/wrapped_flags.h"
#include "memory/data.h"
#include "memory/basic_buffer.h"
#include "memory/contiguous_allocation.h"
#include "memory/no_allocation.h"
#include "bink/allocation_description_types.h"
// Allocate lists of descriptions together in a single contiguous buffer
// @address 0x2a6108 ?allocate_list_contiguous@@YA_NPAVc_allocation_base@@V?$c_wrapped_array@Us_allocation_description@@@@PBDPAPAXPAK@Z
// @address 0x2a6200 ?allocate_list_contiguous@@YA_NPAVc_allocation_base@@V?$c_wrapped_array@Us_allocation_description_holder@@@@PBDPAPAXPAK@Z
// @address 0x2a62f0 ?get_unaligned_size@c_buffer_allocation_description@@UBAKXZ
// @address 0x2a6320 ?assign_storage@c_buffer_allocation_description@@UBAXAAVc_raw_storage_buffer_iterator@@@Z
// @address 0x2a6360 ?get_unaligned_size@c_wrapped_flags_allocation_description@@UBAKXZ
// @address 0x2a6378 ?assign_storage@c_wrapped_flags_allocation_description@@UBAXAAVc_raw_storage_buffer_iterator@@@Z
// @address 0x2a63d0 ?get_unaligned_size@c_data_array_allocation_description@@UBAKXZ
// @address 0x2a6418 ?assign_storage@c_data_array_allocation_description@@UBAXAAVc_raw_storage_buffer_iterator@@@Z

struct s_data_array;
class c_allocation_base;



bool allocate_list_contiguous(c_allocation_base *allocator, c_wrapped_array<s_allocation_description> list, const char *name,
    void **out_buffer, unsigned long *out_size)
{
    bool success = false;
    unsigned long total_size = 0;
    s_allocation_description *end = list.end();
    for (s_allocation_description *it = list.begin(); it != end; ++it) {
        total_size += it->size;
    }
    void *buffer = allocator->allocate(total_size, 0);
    if (buffer) {
        void *address = buffer;
        for (long index = 0; index < list.count(); index++) {
            void *aligned = align_pointer(address, list[index].alignment_bits);
            address = offset_pointer(aligned, list[index].element_size * list[index].element_count);
            *list[index].destination = aligned;
        }
        *out_buffer = buffer;
        if (out_size) {
            *out_size = total_size;
        }
        success = true;
    }
    return success;
}

bool allocate_list_contiguous(c_allocation_base *allocator, c_wrapped_array<s_allocation_description_holder> holders, const char *name,
    void **out_base, unsigned long *out_size)
{
    char *base = 0;
    bool result = false;
    unsigned long total = 0;
    s_allocation_description_holder *h;
    for (h = holders.begin(); h != holders.end(); h++)
        total += h->description.get_unaligned_size();
    base = (char *)allocator->allocate(total, 0);
    if (base) {
        c_raw_storage_buffer_iterator range(base, total);
        for (int i = 0; i < holders.count(); i++)
            holders[i].description.assign_storage(range);
        *out_base = base;
        result = true;
    }
    return result;
}

unsigned long c_buffer_allocation_description::get_unaligned_size() const
{
    return m_size ? (1 << m_alignment_bits) + m_size - 1 : 0;
}

void c_buffer_allocation_description::assign_storage(c_raw_storage_buffer_iterator &iterator) const
{
    m_buffer->set_buffer(iterator.get_array_elements_explicit(m_size, m_alignment_bits, 1), m_size);
}

unsigned long c_wrapped_flags_allocation_description::get_unaligned_size() const
{
    return ((m_bit_count + 31) >> 5) * sizeof(unsigned long) + 3;
}

void c_wrapped_flags_allocation_description::assign_storage(c_raw_storage_buffer_iterator &iterator) const
{
    unsigned long *bits = iterator.get_array_elements<unsigned long>((m_bit_count + 31) >> 5);
    m_flags->set_bits(bits, m_bit_count);
}

unsigned long c_data_array_allocation_description::get_unaligned_size() const
{
    return data_allocation_size(m_maximum_count, m_datum_size, m_alignment_bits) + 3;
}

void c_data_array_allocation_description::assign_storage(c_raw_storage_buffer_iterator &iterator) const
{
    s_data_array *data = (s_data_array *)iterator.get_array_elements_explicit(
        data_allocation_size(m_maximum_count, m_datum_size, m_alignment_bits), 2, 1);
    data_initialize(data, m_name, m_maximum_count, m_datum_size, m_alignment_bits, &g_no_allocation_storage);
    *m_data = data;
}
