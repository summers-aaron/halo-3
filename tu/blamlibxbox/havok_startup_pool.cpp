// blamlibxbox/havok_startup_pool.obj: from the records 0x2dfc60 0x2dfcd0 0x2dfce0 0x2dfce8 0x2dfda8 0x2dfe08 0x2e0008 0x2e00f0
// @flags /TP /O2 /Oc
// @address 0x2dfce0 ?free_chunk@c_havok_memory_pool@@UAAXPAXHW4phys_memory_class@@@Z
// @address 0x2dfce8 ?allocate_chunk_batch@c_havok_memory_pool@@UAAXPAPAXHH@Z
// @address 0x2dfda8 ?free_chunk_batch@c_havok_memory_pool@@UAAXPAPAXHH@Z
// @address 0x2e00f0 ??_Gc_havok_memory_pool@@UAAPAXI@Z
#include "phys/phys_base_types.h"

#include "phys/phys_thread_memory.h"

#include "phys/phys_memory.h"
#include "phys/phys_critical_section.h"
#include "phys/phys_critical_section_entry.h"
#include "physics/havok_memory_pool.h"

// @address 0x2dfc60 ??0c_havok_memory_pool@@QAA@PADJJ@Z
c_havok_memory_pool::c_havok_memory_pool(char *buffer, long buffer_count, long minimum_allocation_size)
    : m_buffer(buffer), m_buffer_count(buffer_count & 0x0ffffffc), m_current_allocation_count(0),
      m_minimum_allocation_size(0), m_critical_section(4000)
{
    // March asserts "the buffer we have been passed must be 16 byte aligned." (havok_memory_pool.h:35).
}

// @address 0x2dfcd0 ?allocate_chunk@c_havok_memory_pool@@UAAPAXHW4phys_memory_class@@@Z
void *c_havok_memory_pool::allocate_chunk(int size, phys_memory_class memory_class)
{
    return aligned_allocate(16, size, memory_class);
}

void c_havok_memory_pool::free_chunk(void *block, int size, phys_memory_class memory_class)
{
    aligned_deallocate(block);
}

void c_havok_memory_pool::allocate_chunk_batch(void **blocks, int count, int size)
{
  c_havok_memory_pool *self = this;
  for (int allocation_index = 0; allocation_index < count; allocation_index++) {
    blocks[allocation_index] = self->allocate_chunk(size, (phys_memory_class)0);
    if (blocks[allocation_index] == 0) {
      for (int next_index = allocation_index - 1; next_index >= 0; next_index--) {
        self->free_chunk(blocks[next_index], size, (phys_memory_class)0);
        blocks[next_index] = 0;
      }
      return;
    }
  }
}

void c_havok_memory_pool::free_chunk_batch(void **blocks, int count, int size)
{
  c_havok_memory_pool *self = this;
  for (int allocation_index = 0; allocation_index < count; allocation_index++)
    self->free_chunk(blocks[allocation_index], size, (phys_memory_class)0);
}

static __forceinline long havok_startup_pool_allocation_end(const long &start, unsigned long count)
{
  return start + count;
}

// @address 0x2dfe08 ?aligned_allocate@c_havok_memory_pool@@AAAPAXHHW4phys_memory_class@@@Z
void *c_havok_memory_pool::aligned_allocate(int alignment, int nbytes, phys_memory_class memory_class)
{
  // allocateChunk is the sole caller and always requests 16-byte alignment.
  // match: 0x2dfe08: value range assumption guides branch lowering
  __assume(alignment == 16);
  void *result = 0;
  long previous_end = 0;
  havok_critical_section_enter(&m_critical_section);
  if (can_create_additional_allocation_record()) {
    for (long allocation_index = 0; allocation_index < m_current_allocation_count; allocation_index++) {
      long start = get_allocation(allocation_index)->start;
      if (start >= previous_end + nbytes) {
        s_allocation gap_allocation;
        result = alignedAllocateInSpace(&gap_allocation, alignment, nbytes, previous_end, start);
        if (result != 0) {
          m_current_allocation_count = m_current_allocation_count + 1;
          for (long next_index = m_current_allocation_count - 1; next_index > allocation_index; next_index--)
            *get_allocation(next_index) = *get_allocation(next_index - 1);
          *get_allocation(allocation_index) = gap_allocation;
          break;
        }
      }
      previous_end = havok_startup_pool_allocation_end(start, get_allocation(allocation_index)->count);
    }
    if (result == 0) {
      s_allocation tail_allocation;
      result = alignedAllocateInSpace(&tail_allocation, alignment, nbytes, previous_end, calculate_next_buffer_count());
      if (result != 0)
        *add_allocation() = tail_allocation;
    }
  }
  m_critical_section.leave();
  return result;
}

// @address 0x2e0008 ?aligned_deallocate@c_havok_memory_pool@@AAAXPAX@Z
void c_havok_memory_pool::aligned_deallocate(void *pointer)
{
  havok_critical_section_enter(&m_critical_section);
  for (long allocation_index = 0; allocation_index < m_current_allocation_count; allocation_index++) {
    if (m_buffer + get_allocation(allocation_index)->start == pointer) {
      for (long next_index = allocation_index; next_index < m_current_allocation_count - 1; next_index++)
        *get_allocation(next_index) = *get_allocation(next_index + 1);
      m_current_allocation_count = m_current_allocation_count - 1;
      break;
    }
  }
  m_critical_section.leave();
}

// The startup pool serves fixed chunks only; runtime blocks trap if reached.
// @address 0x1b65a8 ?allocate_runtime_block@c_havok_memory_pool@@UAAPAXHW4phys_memory_class@@@Z
void *c_havok_memory_pool::allocate_runtime_block(int, phys_memory_class)
{
  __debugbreak();
  // match: 0x1b65a8: unreachable hint keeps the body a single trap word
  __assume(0);
}

// @address 0x1b65a8 ?free_runtime_block@c_havok_memory_pool@@UAAXPAXHW4phys_memory_class@@@Z
void c_havok_memory_pool::free_runtime_block(void *, int, phys_memory_class)
{
  __debugbreak();
  // match: 0x1b65a8: unreachable hint keeps the body a single trap word
  __assume(0);
}

// @address 0x1b65a8 ?preallocate_runtime_block@c_havok_memory_pool@@UAAXHW4phys_memory_class@@@Z
void c_havok_memory_pool::preallocate_runtime_block(int, phys_memory_class)
{
  __debugbreak();
  // match: 0x1b65a8: unreachable hint keeps the body a single trap word
  __assume(0);
}

// @address 0x1b65a8 ?provide_runtime_block@c_havok_memory_pool@@UAAXPAXHW4phys_memory_class@@@Z
void c_havok_memory_pool::provide_runtime_block(void *, int, phys_memory_class)
{
  __debugbreak();
  // match: 0x1b65a8: unreachable hint keeps the body a single trap word
  __assume(0);
}

// @address 0x1b65a8 ?release_runtime_blocks@c_havok_memory_pool@@UAAXXZ
void c_havok_memory_pool::release_runtime_blocks()
{
  __debugbreak();
  // match: 0x1b65a8: unreachable hint keeps the body a single trap word
  __assume(0);
}

// The startup pool reports no statistics.
// @address 0x5bec98 ?print_memory_stats@c_havok_memory_pool@@UAAXPAVphys_out_stream@@@Z
void c_havok_memory_pool::print_memory_stats(phys_out_stream *) {}

// @address 0x5bec98 ?getStatSynopsis@c_havok_memory_pool@@UAAXPAUhkMemoryStatistics@@@Z
void c_havok_memory_pool::getStatSynopsis(phys_memory_stats *) {}

