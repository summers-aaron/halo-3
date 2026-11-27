// blamlibxbox/memory_pool.obj: from the records 0x285c70 0x285cd8 0x285d30 0x285dd0 0x285e70 0x286040 0x2860b0 0x2861e8 0x2862b8 0x286358 0x286430
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x285c70 memory_pool_new
// @address 0x285d30 memory_pool_initialize
// @address 0x285dd0 memory_pool_block_allocate_handle
// @address 0x285e70 memory_pool_block_reallocate_handle
// @address 0x2860b0 ?memory_pool_compact@@YAXPAUs_memory_pool@@@Z
// @address 0x2861e8 memory_pool_block_initialize
#include "cseries/crt_xdk.h"
#include "cseries/pointer_utilities.h"
#include "memory/memory_pool.h"
#include "memory/memory_pool_types.h"
#include "memory/data.h"
#include "objects/objects.h"

// @address 0x744c40 g_memory_pool_relocation_callbacks
pool_relocation_callback g_memory_pool_relocation_callbacks[] =
{
    0,
    objects_update_header_callback,
};

struct s_memory_pool_debug_header
{
    unsigned long signature;
    const char *file;
    long line;
    unsigned long milliseconds;
};
static inline long header_size(s_memory_pool *pool)
{
    long size = sizeof(s_memory_pool_block);
    if (pool->debugging_headers_enabled)
        size += sizeof(s_memory_pool_debug_header);
    return size;
}
static inline s_memory_pool_block *pool_block(s_memory_pool *pool, long offset)
{
    return offset ? (s_memory_pool_block *)((char *)pool + offset) : 0;
}
static inline long pool_offset(s_memory_pool *pool, s_memory_pool_block *block)
{
    return block ? (long)((char *)block - (char *)pool) : 0;
}
static inline char *pool_data(s_memory_pool *pool)
{
    return pool->data_offset ? (char *)pool + pool->data_offset : 0;
}
static inline char *pool_block_base(s_memory_pool *pool, s_memory_pool_block *block)
{
    char *base = (char *)block;
#ifdef HALO3_64BIT
    if (pool->debugging_headers_enabled) base -= sizeof(s_memory_pool_debug_header);
#else
    if (pool->debugging_headers_enabled) base -= 16;
#endif
    return base;
}
static inline s_memory_pool_block *pool_base_block(s_memory_pool *pool, char *base)
{
#ifdef HALO3_64BIT
    if (pool->debugging_headers_enabled) base += sizeof(s_memory_pool_debug_header);
#else
    if (pool->debugging_headers_enabled) base += 16;
#endif
    return (s_memory_pool_block *)base;
}

extern "C" s_memory_pool *memory_pool_new(const char *pool_name,int pool_size,unsigned int callback_kind,unsigned int *allocation_base)
{
  s_memory_pool *new_pool;
  
  new_pool = (s_memory_pool *)((c_allocation_base *)allocation_base)->allocate(
      pool_size + sizeof(s_memory_pool) + sizeof(s_memory_pool_block), (const char *)pool_name);
  if (new_pool) {
    memory_pool_initialize(new_pool, pool_name, pool_size, (e_memory_pool_callback_list)callback_kind,
        (c_allocation_base *)allocation_base);
  }
  return new_pool;
}

void memory_pool_dispose(s_memory_pool *pool)
{
  s_memory_pool *allocation = pool;
  c_allocation_base *allocator = pool->allocator;
  memset(allocation, 0, sizeof(*allocation));
  allocator->deallocate(allocation);
}

extern "C" void memory_pool_initialize(s_memory_pool *pool, const char *pool_name, long pool_size,
    e_memory_pool_callback_list callback_list, c_allocation_base *allocator)
{
#ifdef HALO3_64BIT
    uintptr_t pool_address = reinterpret_cast<uintptr_t>(pool);
    uintptr_t aligned_data_base;
    int pool_data_offset;
    memset(pool, 0, sizeof(s_memory_pool));
    pool->signature = 0x706f6f6c;
    pool->name.set(pool_name);
    aligned_data_base = ((pool_address + sizeof(s_memory_pool) + 15) & ~static_cast<uintptr_t>(15));
    pool->allocator = allocator;
    pool->callback_list = callback_list;
    pool_data_offset = (aligned_data_base != 0) ? static_cast<int>(aligned_data_base - pool_address) : 0;
#else
    unsigned int pool_address = address_from_pointer(pool);
    unsigned int aligned_data_base;
    int pool_data_offset;
    memset(pool, 0, sizeof(s_memory_pool));
    pool->signature = 0x706f6f6c;
    pool->name.set(pool_name);
    aligned_data_base = ((pool_address + 83) & -16);
    pool->allocator = allocator;
    pool->callback_list = callback_list;
    pool_data_offset = ((aligned_data_base != 0)) ? (aligned_data_base - pool_address) : 0;
#endif
    pool->data_offset = pool_data_offset;
    pool->size = pool_size;
    pool->free_size = pool_size;
    pool->first_block_handle = 0;
    pool->last_block_handle = 0;
    pool->reference_tracking_disabled = false;
    pool->allocation_from_anywhere = false;
    pool->debugging_headers_enabled = false;
    return;
}

int memory_pool_block_allocate_handle(s_memory_pool *pool, long block_reference, long request_size, const char *file, long line)
{
    s_memory_pool_block *new_block;
    char *new_block_data;
    int aligned_size;
    int block_header_size;
    int result;
    block_header_size = 16;
    if (pool->debugging_headers_enabled) {
#ifdef HALO3_64BIT
        block_header_size = sizeof(s_memory_pool_block) + sizeof(s_memory_pool_debug_header);
#else
        block_header_size = 32;
#endif
    }
    aligned_size = (block_header_size + request_size);
    if (!(((aligned_size & 15) == 0))) {
        aligned_size = ((aligned_size | 15) + 1);
    }
    result = 0;
    new_block = memory_pool_block_new(pool, aligned_size, 0, 0);
    if (new_block != 0) {
        new_block->reference = block_reference;
        new_block_data = (char *)new_block + 16;
        pool->free_size -= new_block->size;
        result = new_block_data ? (new_block_data - (char *)pool) : 0;
    }
    return result;
}

int memory_pool_block_reallocate_handle(char *storage, int handle, int new_size, int source_file, int source_line)
{
  int result;
  int header = handle - sizeof(s_memory_pool_block);
  s_memory_pool_block *block = pool_block((s_memory_pool *)storage, header);
  s_memory_pool *pool = (s_memory_pool *)storage;
  int actual = header_size(pool) + new_size;
  if (actual & 15) actual = (actual | 15) + 1;
  char *limit;
  int next = block->next;
  result = 0;
  if (next) {
    limit = storage + next;
  } else {
    int off = pool->data_offset;
    char *t = off ? storage + off : 0;
    limit = t + pool->size;
  }
  if ((char *)block + actual <= limit) {
    pool->free_size = block->size - actual + pool->free_size;
    block->size = actual;
    if (pool->debugging_headers_enabled) {
      s_memory_pool_debug_header *dbg = (s_memory_pool_debug_header *)block - 1;
      if (dbg) {
        dbg->file = 0;
        dbg->line = 0;
        dbg->milliseconds = GetTickCount();
      }
    }
    char *p = (char *)block + sizeof(s_memory_pool_block);
    result = p ? p - storage : 0;
    if (result) return result;
  }
  {
    int new_handle = memory_pool_block_allocate_handle(pool, block->reference, new_size, 0, 0);
    if (new_handle) {
    char *dst = storage + new_handle;
    memcpy(dst, handle ? storage + handle : 0, block->size - header_size(pool));
    memory_pool_block_free((s_memory_pool *)pool, handle);
    int nh = new_handle - 16;
    s_memory_pool_block *nb = pool_block(pool, nh);
    if (!pool->reference_tracking_disabled) {
      pool_relocation_callback cb = g_memory_pool_relocation_callbacks[pool->callback_list];
      cb(nb->reference, (nb ? (char *)nb - storage : 0) + 16);
    }
    return new_handle;
    }
  }
  return result;
}

// @address 0x82286040 ?memory_pool_block_free@@YAXPAUs_memory_pool@@J@Z
void memory_pool_block_free(s_memory_pool *heap, long offset)
{
  long header_offset = offset - 16;
  s_memory_pool_block *block = header_offset ? (s_memory_pool_block *)((char *)heap + header_offset) : 0;
  // match: 0x82286040: volatile pointer cast preserves memory access order
  heap->free_size += ((volatile s_memory_pool_block *)block)->size;
  if (block->previous != 0)
    ((s_memory_pool_block *)((char *)heap + block->previous))->next = block->next;
  else
    heap->first_block_handle = block->next;
  if (block->next != 0)
    ((s_memory_pool_block *)((char *)heap + block->next))->previous = block->previous;
  else
    heap->last_block_handle = block->previous;
}

void memory_pool_compact(struct s_memory_pool * pool)
{
    if (!pool->reference_tracking_disabled && pool->first_block_handle)
    {
        s_memory_pool_block *block = pool_block(pool, pool->first_block_handle);
        s_memory_pool_block *previous = 0;
        char *next_address = pool_data(pool);
        while (block)
        {
            char *base = pool_block_base(pool, block);
            if (base > next_address)
            {
                memmove(next_address, base, block->size);
                base = next_address;
                block = pool_base_block(pool, base);
                if (!pool->reference_tracking_disabled)
                {
                    pool_relocation_callback callback = g_memory_pool_relocation_callbacks[pool->callback_list];
                    callback(block->reference, pool_offset(pool, block) + 16);
                }
            }
            block->previous = pool_offset(pool, previous);
            if (previous)
                previous->next = pool_offset(pool, block);
            else
                pool->first_block_handle = pool_offset(pool, block);
            next_address = base + block->size;
            previous = block;
            block = pool_block(pool, block->next);
        }
        previous->next = 0;
        pool->last_block_handle = pool_offset(pool, previous);
    }
}


s_memory_pool_block *memory_pool_block_initialize(s_memory_pool *pool, char *memory, int size, const char *name,
    int line, s_memory_pool_block *next, s_memory_pool_block *previous)
{
  s_memory_pool_block *block = (s_memory_pool_block *)memory;
#ifdef HALO3_64BIT
  if (pool->debugging_headers_enabled) block = (s_memory_pool_block *)(memory + sizeof(s_memory_pool_debug_header));
#else
  if (pool->debugging_headers_enabled) block++;
#endif
  if (pool->debugging_headers_enabled) {
    s_memory_pool_debug_header *header = (s_memory_pool_debug_header *)block - 1;
    if (header) {
      header->file = 0;
      header->line = 0;
      header->signature = 0x68656164;
      header->milliseconds = GetTickCount();
    }
  }
  block->size = size;
  block->next = next ? (char *)next - (char *)pool : 0;
  block->previous = previous ? (char *)previous - (char *)pool : 0;
  if (!previous) pool->first_block_handle = (char *)block - (char *)pool; else previous->next = (char *)block - (char *)pool;
  if (!next) pool->last_block_handle = (char *)block - (char *)pool; else next->previous = (char *)block - (char *)pool;
  return block;
}

// @address 0x2862b8 ?memory_pool_block_new_from_end_of_pool@@YAPAUs_memory_pool_block@@PAUs_memory_pool@@JPBDJ@Z
s_memory_pool_block *memory_pool_block_new_from_end_of_pool(s_memory_pool *pool, long size, const char *name, long line)
{
    s_memory_pool_block *result = 0;
    char *memory;
    if (!pool->last_block_handle)
    {
        memory = pool_data(pool);
    }
    else
    {
        s_memory_pool_block *last = pool_block(pool, pool->last_block_handle);
        memory = pool_block_base(pool, last) + last->size;
    }
    if (memory + size <= pool_data(pool) + pool->size)
        result = (s_memory_pool_block *)memory_pool_block_initialize(pool, memory, size, 0, 0, 0,
            (s_memory_pool_block *)pool_block(pool, pool->last_block_handle));
    return result;
}

// @address 0x286358 ?memory_pool_block_new_from_in_between_blocks@@YAPAUs_memory_pool_block@@PAUs_memory_pool@@JPBDJ@Z
s_memory_pool_block *memory_pool_block_new_from_in_between_blocks(s_memory_pool *pool, long size, const char *name, long line)
{
    s_memory_pool_block *result = 0;
    s_memory_pool_block *block = pool_block(pool, pool->first_block_handle);
    while (block)
    {
        s_memory_pool_block *next = pool_block(pool, block->next);
        if (next)
        {
            char *next_base = pool_block_base(pool, next);
            char *memory = pool_block_base(pool, block) + block->size;
            if (memory + size <= next_base)
            {
                result = (s_memory_pool_block *)memory_pool_block_initialize(pool, memory, size, 0, 0,
                    (s_memory_pool_block *)next, (s_memory_pool_block *)block);
                break;
            }
        }
        block = next;
    }
    return result;
}

// @address 0x286430 memory_pool_block_new
static __forceinline s_memory_pool_block *memory_pool_block_new_at_start(s_memory_pool *pool,long size,const char *file,long line) {
 s_memory_pool_block *block=0;
 void *limit;
 if(pool->first_block_handle==0) limit=(char *)pool_data(pool)+pool->size;
 else limit=pool_block_base(pool,pool_block(pool,pool->first_block_handle));
 if((char *)pool_data(pool)+size<=(char *)limit)
   block=(s_memory_pool_block *)memory_pool_block_initialize(pool, pool_data(pool), size, file, line,
       (s_memory_pool_block *)pool_block(pool, pool->first_block_handle), 0);
 return block;
}
extern "C" s_memory_pool_block *memory_pool_block_new(s_memory_pool *pool, long size, const char *source_file, long source_line)
{
 s_memory_pool_block *block;
 const char *file=0; long line=0;
 if(pool->allocation_from_anywhere) {
   block=memory_pool_block_new_from_in_between_blocks(pool,size,file,line);
   if(!block) {
     block=memory_pool_block_new_at_start(pool,size,file,line);
     if(!block) block=memory_pool_block_new_from_end_of_pool(pool,size,file,line);
   }
 } else block=memory_pool_block_new_from_end_of_pool(pool,size,file,line);
 return block;
}
