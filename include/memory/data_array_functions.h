#ifndef MEMORY_DATA_ARRAY_FUNCTIONS_H
#define MEMORY_DATA_ARRAY_FUNCTIONS_H

struct s_data_array;
class c_allocation_base;

// Out-of-line data-array operations; no inline allocation helpers.
void data_initialize(s_data_array *data, const char *name, long maximum_count, long size, long alignment_bits, c_allocation_base *allocator);
void data_delete_all(s_data_array *data);
long datum_new_at_index(s_data_array *data, long requested_index);
long datum_new(s_data_array *data);

struct s_data_iterator;
void *data_iterator_next(s_data_iterator *iterator);
long data_next_index(s_data_array *data, long index);
long data_next_absolute_index(const s_data_array *data, long index);
void *datum_try_and_get(const s_data_array *data, long datum_index);

void datum_delete(s_data_array *data, long index);

void data_set_new_base_address(s_data_array **out, s_data_array *block);
#endif // MEMORY_DATA_ARRAY_FUNCTIONS_H
