// hkbase/hkSmallArray.obj: from the records 0x6ffff8 0x700088
// @flags /TP /O2 /Oc
// @address 0x6ffff8 ?reserve_array_storage@phys_small_array_util@@YAXPAXHH@Z
// @address 0x700088 ?grow_array_storage@phys_small_array_util@@YAXPAXH@Z
#include "cseries/crt_xdk.h"
#include "phys/phys_base.h"

struct phys_small_array_base {
    void *data;
    unsigned short size;
    unsigned short capacity_and_flags;
};
enum {
    PHYS_SMALL_ARRAY_CAPACITY_MASK = 0x3fff,
    PHYS_SMALL_ARRAY_LOCKED_FLAG = 0x4000,
    PHYS_SMALL_ARRAY_DONT_DEALLOCATE_FLAG = 0x8000
};
#include "phys/phys_thread_memory.h"

void phys_small_array_util::reserve_array_storage(void *storage, int capacity, int element_size)
{
    phys_small_array_base *array = (phys_small_array_base *)storage;
    phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    void *data = memory->allocate_chunk(capacity * element_size, PHYS_MEM_ARRAY);
    memcpy(data, array->data, array->size * element_size);
    if ((PHYS_SMALL_ARRAY_DONT_DEALLOCATE_FLAG & array->capacity_and_flags) == 0U) {
        int prev_capacity = array->capacity_and_flags & PHYS_SMALL_ARRAY_CAPACITY_MASK;
        memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
        memory->free_chunk(array->data, prev_capacity * element_size, PHYS_MEM_ARRAY);
    }
    array->data = data;
    array->capacity_and_flags = (PHYS_SMALL_ARRAY_LOCKED_FLAG & array->capacity_and_flags) | capacity;
}

void phys_small_array_util::grow_array_storage(void *storage, int element_size)
{
  phys_small_array_base *array = (phys_small_array_base *)storage;
  int capacity = array->size != 0 ? array->size * 2 : 1;
  phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
  void *data = memory->allocate_chunk(capacity * element_size, PHYS_MEM_ARRAY);
  memcpy(data, array->data, array->size * element_size);
  if ((PHYS_SMALL_ARRAY_DONT_DEALLOCATE_FLAG & array->capacity_and_flags) == 0U) {
    int prev_capacity = array->capacity_and_flags & PHYS_SMALL_ARRAY_CAPACITY_MASK;
    memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    memory->free_chunk(array->data, prev_capacity * element_size, PHYS_MEM_ARRAY);
  }
  array->data = data;
  array->capacity_and_flags = (PHYS_SMALL_ARRAY_LOCKED_FLAG & array->capacity_and_flags) | capacity;
}

