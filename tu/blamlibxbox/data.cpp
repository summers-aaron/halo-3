// blamlibxbox/data.obj: from the records 0x84740 0x85528 0xaae70 0xac7c0 0xd04c8 0xd61b0 0x1018a0 0x1ec160 0x1ec210 0x1ec288 0x1ec2e8 0x1ec360 0x1ec3f8 0x1ec580 0x1ec680 0x1ec740 0x1ec7b0 0x1ec830
// @flags /TP /O2
// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z
// @address 0x85528 ?datum_delete@@YAXPAUs_data_array@@J@Z
// @address 0xaae70 ?datum_new@@YAJPAUs_data_array@@@Z
// @address 0xac7c0 ?datum_try_and_get@@YAPAXPBUs_data_array@@J@Z
// @address 0xd04c8 ?data_next_index@@YAJPAUs_data_array@@J@Z
// @address 0x1018a0 ?datum_new_at_index@@YAJPAUs_data_array@@J@Z
// @address 0x1ec288 ?data_dispose@@YAXPAUs_data_array@@@Z
// @address 0x1ec2e8 ?data_initialize@@YAXPAUs_data_array@@PBDJJJPAVc_allocation_base@@@Z
// @address 0x1ec3f8 ?data_connect@@YAXPAUs_data_array@@JPAX@Z
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z
// @address 0x1ec680 ?datum_new_at_absolute_index@@YAJPAUs_data_array@@J@Z
// @address 0x1ec740 ?datum_initialize@@YAXPAUs_data_array@@PAUs_datum_header@@@Z
// @address 0x1ec7b0 ?data_next_absolute_index@@YAJPBUs_data_array@@J@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0xd61b0 data_iterator_next_with_flags
#include "memory/data.h"
#include <ppcintrinsics.h>


#include "memory/data_inline.h"
#include "cseries/pointer_utilities.h"

#define BIT_VECTOR_CLEAR(vector, index) ((vector)[(index) >> 5] &= ~(1 << ((index) & 31)))


void *data_iterator_next(s_data_iterator *iterator)
{
    s_data_array *data = iterator->data;
    long absolute_index = data_next_absolute_index(data, iterator->absolute_index + 1);
    unsigned short *datum;
    if (absolute_index != -1)
    {
        datum = (unsigned short *)(data->data + absolute_index * data->size);
        iterator->absolute_index = absolute_index;
        iterator->index = (*datum << 16) | absolute_index;
    }
    else
    {
        datum = 0;
        iterator->absolute_index = iterator->data->maximum_count;
        iterator->index = -1;
    }
    return datum;
}

void datum_delete(s_data_array *data, datum_index index)
{
    // The absolute index is the low word of the datum index; as an int (not
    // an unsigned short) its word index takes srawi.
    int absolute_index = index & 0xffff;
    short *datum = (short *)(data->data + data->size * absolute_index);
    if (flag_test(data->flags, 3))
        memset(datum, 0xba, data->size);
    BIT_VECTOR_CLEAR((unsigned int *)data->in_use_bit_vector, absolute_index);
    *datum = 0;
    if (absolute_index < data->next_index)
        data->next_index = absolute_index;
    if (absolute_index + 1 == data->first_unallocated) {
        do {
            data->first_unallocated--;
            datum = (short *)((char *)datum - data->size);
        } while (data->first_unallocated > 0 && *datum == 0);
    }
    data->actual_count--;
}

long datum_new(s_data_array *data)
{
    int result = -1;
    int absolute_index = -1;
    int index;
    int first_unallocated = data->first_unallocated;

    for (index = data->next_index; index < data->first_unallocated; index++) {
        if (!(((unsigned int *)data->in_use_bit_vector)[index >> 5] & (1 << (index & 31)))) {
            absolute_index = index;
            break;
        }
    }
    if (absolute_index == -1 && first_unallocated < data->maximum_count)
        absolute_index = first_unallocated;

    if (absolute_index != -1) {
        unsigned short *datum = (unsigned short *)(data->data + absolute_index * data->size);
        ((unsigned int *)data->in_use_bit_vector)[absolute_index >> 5] |= 1 << (absolute_index & 31);
        data->actual_count++;
        data->next_index = absolute_index + 1;
        if (data->first_unallocated <= absolute_index)
            data->first_unallocated = absolute_index + 1;
        memset(datum, 0, data->size);
        if ((unsigned short)data->next_identifier == 0xffff)
            data->next_identifier = 0x8000;
        *datum = data->next_identifier;
        data->next_identifier++;
        result = (*datum << 16) | absolute_index;
    }
    return result;
}

void *datum_try_and_get(const s_data_array *data, long datum_index)
{
    void *result = 0;
    if (datum_index != -1) {
        int absolute_index = datum_index & 0xffff;
        if (absolute_index < data->first_unallocated) {
            short *datum = (short *)(data->data + data->size * absolute_index);
            int identifier = *datum;
            if (identifier != 0 && identifier == (datum_index >> 16))
                result = datum;
        }
    }
    return result;
}

long data_next_index(s_data_array *array, long index)
{
    long absolute_index = data_next_absolute_index(array, (index + 1) & 0xffff);
    long result = -1;
    if (absolute_index != -1)
        result = (*(unsigned short *)(array->data + absolute_index * array->size) << 16) | absolute_index;
    return result;
}

static __forceinline long data_next_index_with_flags(s_data_array *data, long index, long offset, unsigned char mask, unsigned char match)
{
    long result = -1;
    if (index < 0)
        return result;
    long size = 0;
    char *field = 0;
    long first_unallocated = data->first_unallocated;
    if (index < first_unallocated) {
        size = data->size;
        long element_offset = size * index;
        char *elements = data->data;
        unsigned int *in_use = (unsigned int *)data->in_use_bit_vector;
        field = elements + element_offset + offset;
        do {
            __dcbt(element_offset, elements);
            if (((1 << (index & 31)) & in_use[index >> 5]) != 0 &&
                (unsigned int)(*(unsigned char *)field & (unsigned char)mask) == (unsigned char)match) {
                result = index;
                break;
            }
            index++;
            element_offset += size;
            field += size;
        } while (index < first_unallocated);
    }
    return result;
}

// @address 0xd61b0 data_iterator_next_with_flags
extern "C" void *data_iterator_next_with_flags(s_data_iterator *iterator, long offset, unsigned char mask, unsigned char match)
{
    s_data_array *data = iterator->data;
    long absolute_index = data_next_index_with_flags(data, iterator->absolute_index + 1, offset, mask, match);
    void *result;
    if (absolute_index != -1) {
        unsigned short *datum = (unsigned short *)(data->data + absolute_index * data->size);
        iterator->absolute_index = absolute_index;
        iterator->index = (*datum << 16) | absolute_index;
        result = datum;
    } else {
        iterator->absolute_index = data->maximum_count;
        iterator->index = -1;
        result = 0;
    }
    return result;
}

long datum_new_at_index(s_data_array *data, long requested_index)
{
    long absolute_index = (unsigned short)requested_index;
    long identifier = requested_index >> 16;
    if (absolute_index >= 0 && absolute_index < data->maximum_count) {
        s_datum_header *datum = (s_datum_header *)(data->data + absolute_index * data->size);
        if ((unsigned short)datum->identifier == 0) {
            ((unsigned int *)data->in_use_bit_vector)[absolute_index >> 5] |= 1 << (absolute_index & 31);
            data->actual_count++;
            if (absolute_index >= data->first_unallocated)
                data->first_unallocated = absolute_index + 1;
            datum_initialize(data, datum);
            datum->identifier = (short)identifier;
            return ((unsigned long)datum->identifier << 16) | absolute_index;
        }
    }
    return -1;
}

// @address 0x1ec160 ?data_new@@YAPAUs_data_array@@PBDJJJPAVc_allocation_base@@@Z
s_data_array *data_new(const char *name, long maximum_count, long size, long alignment_bits, c_allocation_base *allocator)
{
    s_data_array *data = (s_data_array *)allocator->allocate(data_allocation_size(maximum_count, size, alignment_bits), name);
    if (data) {
        data_initialize(data, name, maximum_count, size, alignment_bits, allocator);
        data->flags |= 4;
    }
    return data;
}

// @address 0x1ec210 ?data_initialize_disconnected@@YAPAXPBDJJJPAVc_allocation_base@@@Z
void *data_initialize_disconnected(const char *name, long maximum_count, long size, long alignment_bits, c_allocation_base *allocator)
{
#ifdef HALO3_64BIT
    void *data = allocator->allocate((long)(sizeof(s_data_array) + (((maximum_count + 31) >> 5) << 2)), name);
#else
    void *data = allocator->allocate((((maximum_count + 31) >> 5) + 21) << 2, name);
#endif
    if (data != 0)
        data_initialize_header((s_data_array *)data, name, maximum_count, size, 0, allocator, (char *)data + sizeof(s_data_array));
    return data;
}

void data_dispose(s_data_array *data)
{
    c_allocation_base *allocator = data->allocator;
    memset(data, 0, sizeof(*data));
    if (allocator)
        allocator->deallocate(data);
}

static __forceinline char *data_align_storage(s_data_array *data, long alignment_bits)
{
    return (char *)align_pointer(data + 1, alignment_bits);
}

void data_initialize(s_data_array *data, const char *name, long maximum_count, long size,
                     long alignment_bits, c_allocation_base *allocator)
{
    char *storage = data_align_storage(data, alignment_bits);
    char *end = storage + maximum_count * size;
    data_initialize_header(data, name, maximum_count, size, alignment_bits, allocator, end);
    data->data = storage;
    data->data_offset = storage - (char *)data;
    data->flags &= ~2;
    data->flags &= ~1;
    memset(data->in_use_bit_vector, 0, ((maximum_count + 31) >> 5) * 4);
}

void data_connect(s_data_array *data, long maximum_count, void *datums)
{
    data->flags &= ~2;
    data->maximum_count = maximum_count;
    data->data_offset = (char *)datums - (char *)data;
    data->data = (char *)datums;
    data->valid = 1;
    data->first_unallocated = 0;
    data->actual_count = 0;
    data->next_index = data->maximum_count;
    strncpy((char *)&data->next_identifier, data->name, 2);
    byte_swap_memory(&data->next_identifier, 1, -2);
    data->next_identifier |= 0x8000;
    short inverted = ~data->next_identifier;
    data->isolated_next_identifier = inverted | 0x8000U;
    for (long i = 0; i < data->maximum_count; i++)
    {
        short *datum = (short *)(data->data + i * data->size);
        bool in_use = *datum != 0;
        if (flag_test(data->flags, 3) && !in_use)
        {
            memset(datum, 0xba, data->size);
            *datum = 0;
        }
        if (in_use)
            ((unsigned int *)data->in_use_bit_vector)[i >> 5] |= 1UL << (i & 31);
        else
            ((unsigned int *)data->in_use_bit_vector)[i >> 5] &= ~(1UL << (i & 31));
        if (in_use)
        {
            data->first_unallocated = i + 1;
            data->actual_count++;
        }
        else if (i < data->next_index)
            data->next_index = i;
    }
}

void data_delete_all(s_data_array *data)
{
    data->first_unallocated = 0;
    data->actual_count = 0;
    data->next_index = 0;
    strncpy((char *)&data->next_identifier, data->name, 2);
    byte_swap_memory(&data->next_identifier, 1, -2);
    data->next_identifier |= 0x8000;
    data->isolated_next_identifier = (short)~data->next_identifier;
    data->isolated_next_identifier |= 0x8000U;
    if (flag_test(data->flags, 3))
        memset(data->data, 0xba, data->size * data->maximum_count);
    for (long index = 0; index < data->maximum_count; ++index)
        *(short *)(data->data + index * data->size) = 0;
    memset(data->in_use_bit_vector, 0, ((data->maximum_count + 31) >> 5) * 4);
}

long datum_new_at_absolute_index(s_data_array *data, long absolute_index)
{
    if (absolute_index >= 0 && absolute_index < data->maximum_count) {
        s_datum_header *datum = (s_datum_header *)(data->data + absolute_index * data->size);
        if ((unsigned short)datum->identifier == 0) {
            ((unsigned int *)data->in_use_bit_vector)[absolute_index >> 5] |= 1 << (absolute_index & 31);
            data->actual_count++;
            if (absolute_index >= data->first_unallocated)
                data->first_unallocated = absolute_index + 1;
            datum_initialize(data, datum);
            return ((unsigned long)(unsigned short)datum->identifier << 16) | absolute_index;
        }
    }
    return -1;
}

void datum_initialize(s_data_array *data, s_datum_header *datum)
{
    memset(datum, 0, data->size);
    if (data->next_identifier == -1)
        data->next_identifier = (short)0x8000;
    datum->identifier = data->next_identifier++;
}

void data_set_new_base_address(s_data_array **out, s_data_array *block)
{
    if (block != 0) {
        if (block->data_offset != 0) {
            char *rebased_address = (char *)block + block->data_offset;
            if (block->data != rebased_address)
                block->data = rebased_address;
        } else {
            block->data = 0;
        }
        if (block->in_use_bit_vector_offset != 0) {
            char *rebased_address = (char *)block + block->in_use_bit_vector_offset;
            if (block->in_use_bit_vector != rebased_address)
                block->in_use_bit_vector = rebased_address;
        } else {
            block->in_use_bit_vector = 0;
        }
    }
    *out = block;
}

// 0x1ec7b0
static __forceinline bool data_in_use_bit_test(const unsigned long *vector, long index)
{
    return (vector[(index) >> 5] & (1 << ((index) & 31))) != 0;
}

// data_next_index: the first allocated datum at or after the absolute index, or -1.
long data_next_absolute_index(const s_data_array *data, long index)
{
    long result = -1;
    if (index >= 0 && index < data->first_unallocated) {
        unsigned long *in_use = (unsigned long *)data->in_use_bit_vector;
        char *elements = data->data;
        do {
            __dcbt(index * data->size, elements);
            if (data_in_use_bit_test(in_use, index)) {
                result = index;
                break;
            }
            index++;
        } while (index < data->first_unallocated);
    }
    return result;
}

// @address 0x1ec360 data_initialize_header
void data_initialize_header(s_data_array *data, const char *name, long maximum_count, long size, long alignment_bits, c_allocation_base *allocator, char *end)
{
    memset(data, 0, sizeof(*data));
    csstrnzcpy(data->name, name, sizeof(data->name));
    data->maximum_count = maximum_count;
    data->size = size;
    data->alignment_bits = (unsigned char)alignment_bits;
    data->signature = 0x64407440;
    data->data = 0;
    data->data_offset = 0;
    data->valid = 0;
    data->in_use_bit_vector = end;
    data->in_use_bit_vector_offset = end - (char *)data;
    data->allocator = allocator;
    data->flags |= 3;
}

