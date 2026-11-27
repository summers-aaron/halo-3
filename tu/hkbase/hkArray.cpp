// hkbase/hkArray.obj: from the records 0x60c180 0x60c210 0x60c2b0
// @flags /TP /O2
// @address 0x60c180 ?reserve_array_storage@phys_array_util@@YAXPAXHH@Z
// @address 0x60c210 ?grow_array_storage@phys_array_util@@YAXPAXH@Z
// @address 0x60c2b0 ?shrink_array_storage@phys_array_util@@YAXPAXHPADH@Z
#include "phys/phys_base.h"

void phys_array_util::reserve_array_storage(void *storage, int capacity, int element_size)
{
    phys_array_base *array = (phys_array_base *)storage;
    phys_thread_memory *memory = phys_thread_memory::get_instance_ptr();
    void *data = memory->allocate_chunk(capacity * element_size, PHYS_MEM_ARRAY);
    memcpy(data, array->data, array->size * element_size);
    if ((array->capacity_and_flags & PHYS_ARRAY_DONT_DEALLOCATE_FLAG) == 0) {
        int prev_capacity = array->capacity_and_flags & PHYS_ARRAY_CAPACITY_MASK;
        memory = phys_thread_memory::get_instance_ptr();
        memory->free_chunk(array->data, prev_capacity * element_size, PHYS_MEM_ARRAY);
    }
    array->data = data;
    array->capacity_and_flags = (array->capacity_and_flags & PHYS_ARRAY_LOCKED_FLAG) | capacity;
}

void phys_array_util::grow_array_storage(void *storage, int element_size)
{
    phys_array_base *array = (phys_array_base *)storage;
    int capacity = array->size ? array->size * 2 : 1;
    phys_thread_memory *memory = phys_thread_memory::get_instance_ptr();
    void *data = memory->allocate_chunk(capacity * element_size, PHYS_MEM_ARRAY);
    memcpy(data, array->data, element_size * array->size);
    if ((array->capacity_and_flags & PHYS_ARRAY_DONT_DEALLOCATE_FLAG) == 0) {
        int prev_capacity = array->capacity_and_flags & PHYS_ARRAY_CAPACITY_MASK;
        memory = phys_thread_memory::get_instance_ptr();
        memory->free_chunk(array->data, prev_capacity * element_size, PHYS_MEM_ARRAY);
    }
    array->data = data;
    array->capacity_and_flags = (array->capacity_and_flags & PHYS_ARRAY_LOCKED_FLAG) | capacity;
}

void phys_array_util::shrink_array_storage(void *storage, int element_size, char *buffer, int inline_buf_cap)
{
    phys_array_base *array = (phys_array_base *)storage;
    void *data;
    int capacity;
    int flags;
    if (buffer && array->size < inline_buf_cap) {
        data = buffer;
        capacity = inline_buf_cap;
        flags = PHYS_ARRAY_DONT_DEALLOCATE_FLAG;
    } else {
        capacity = (array->capacity_and_flags & PHYS_ARRAY_CAPACITY_MASK) / 2;
        phys_thread_memory *memory = phys_thread_memory::get_instance_ptr();
        data = memory->allocate_chunk(capacity * element_size, PHYS_MEM_ARRAY);
        flags = 0;
    }
    phys_string::mem_copy(data, array->data, array->size * element_size);
    int prev_capacity = array->capacity_and_flags & PHYS_ARRAY_CAPACITY_MASK;
    phys_thread_memory *memory = phys_thread_memory::get_instance_ptr();
    memory->free_chunk(array->data, prev_capacity * element_size, PHYS_MEM_ARRAY);
    array->data = data;
    array->capacity_and_flags = capacity | flags | (array->capacity_and_flags & PHYS_ARRAY_LOCKED_FLAG);
}

