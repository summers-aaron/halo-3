#ifndef INCLUDE_MEMORY_DATA_H
#define INCLUDE_MEMORY_DATA_H

#include "cseries/cseries.h"

class c_allocation_base {
public:
    virtual void *allocate(unsigned int size, const char *name) = 0;
    virtual void deallocate(void *allocation) = 0;
};

#include "memory/data_types.h"
#include "memory/data_array_functions.h"

// Disconnect the backing storage while retaining the array descriptor.
static __forceinline void data_disconnect(s_data_array *data)
{
    data->data = 0;
    data->data_offset = 0;
    data->valid = false;
    data->flags |= 2;
}

// Out-of-line data.obj functions
static __forceinline long data_allocation_size(long maximum_count, long size, long alignment_bits)
{
    long alignment_mask = alignment_bits ? (1 << alignment_bits) - 1 : 0;
    return sizeof(s_data_array) + ((maximum_count + 31) >> 5) * 4 + maximum_count * size + alignment_mask;
}

s_data_array *data_new(const char *name, long maximum_count, long size, long alignment_bits, c_allocation_base *allocator);
void *data_initialize_disconnected(const char *name, long maximum_count, long size, long alignment_bits, c_allocation_base *allocator);
void data_dispose(s_data_array *data);

// Mark the array valid and clear all entries.
static __forceinline void data_make_valid(s_data_array *data)
{
    data->valid = true;
    data_delete_all(data);
}
void data_set_new_base_address(s_data_array **out, s_data_array *block);
void data_connect(s_data_array *data, long maximum_count, void *datums);

long datum_new_at_absolute_index(s_data_array *data, long absolute_index);
void datum_initialize(s_data_array *data, s_datum_header *datum);
void *datum_try_and_get(const s_data_array *data, long datum_index);
void datum_delete(s_data_array *data, long index);

// Local inline body for inlined call sites of the out-of-line datum_try_and_get
// (data.obj): this build emits bl for cross-unit calls, so inlined sites need
// this copy. Mirrors tu/blamlibxbox/data.cpp datum_try_and_get exactly.
static __forceinline void *inl_datum_try_and_get(const s_data_array *data, long datum_index)
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

void *data_iterator_next(s_data_iterator *iterator);
long data_next_index(s_data_array *data, long index);
long data_next_absolute_index(const s_data_array *data, long index);

static __forceinline void *datum_get(s_data_array *data, long index) {
    return data->data + (unsigned short)index * data->size;
}

static __forceinline const void *datum_get(const s_data_array *data, long index) {
    return data->data + (unsigned short)index * data->size;
}

// Typed arrays have a compile-time element stride.
template<typename t_datum>
static __forceinline t_datum *datum_get(s_data_array *data, long index)
{
    return &((t_datum *)data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(index)];
}

template<typename t_datum>
static __forceinline const t_datum *datum_get(const s_data_array *data, long index)
{
    return &((const t_datum *)data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(index)];
}

#define DATUM_GET(data, type, index) ((type *)datum_get((data), (index)))
#define DATUM_GET_CONST(data, type, index) ((const type *)datum_get((data), (index)))
#define DATUM_TRY_AND_GET(data, type, index) ((type *)datum_try_and_get((data), (index)))
#define DATUM_TRY_AND_GET_CONST(data, type, index) ((const type *)datum_try_and_get((data), (index)))

static __forceinline void data_iterator_begin(s_data_iterator *iterator, s_data_array *data) {
    // March data.cpp:1345 data_iterator_begin: data, absolute_index, index (then a debug-only cookie).
    iterator->data = data;
    iterator->absolute_index = NONE;
    iterator->index = NONE;
}

template<typename t_datum>
class c_smart_data_array {
public:
    s_data_array *m_data;

    __forceinline c_smart_data_array &operator=(s_data_array *data) { m_data = data; return *this; }
    __forceinline s_data_array *get() const { return m_data; }
    __forceinline s_data_array *&get_restricted_data_array_address() { return m_data; }
    __forceinline operator s_data_array *() const { return m_data; }
    __forceinline s_data_array *operator->() const { return m_data; }
    __forceinline t_datum *operator[](long index) const { return (t_datum *)datum_get(m_data, index); }
};

template<typename t_datum>
class c_data_iterator {
public:
    // Reset the iterator to the start of the array.
    __forceinline void begin(s_data_array *data) {
        m_iterator.data = data;
        m_iterator.absolute_index = NONE;
        m_iterator.index = NONE;
    }
    __forceinline bool next() {
        m_datum = (t_datum *)data_iterator_next(&m_iterator);
        return m_datum != NULL;
    }
    __forceinline t_datum *get_datum() const { return m_datum; }
    __forceinline long get_index() const { return m_iterator.index; }
    __forceinline short get_absolute_index() const { return (short)DATUM_INDEX_TO_ABSOLUTE_INDEX(m_iterator.index); }

    t_datum *m_datum;
    s_data_iterator m_iterator;
};

typedef char data_h_size_smart_array[(sizeof(void *) != 4 || ((sizeof(c_smart_data_array<s_datum_header>) == 4) )) ? 1 : -1];
typedef char data_h_size_iterator[(sizeof(void *) != 4 || ((sizeof(c_data_iterator<s_datum_header>) == 16) )) ? 1 : -1];

// Functions defined in tu/blamlibxbox/data.cpp that other units call.

extern "C" void *data_iterator_next_with_flags(s_data_iterator *iterator, long offset, unsigned char mask, unsigned char match);
extern "C" void data_initialize_header(s_data_array *data, const char *name, long maximum_count,
    long size, long alignment_bits, c_allocation_base *allocator, char *end);

#endif // INCLUDE_MEMORY_DATA_H
