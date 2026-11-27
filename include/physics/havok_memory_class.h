#ifndef PHYSICS_HAVOK_MEMORY_CLASS_H
#define PHYSICS_HAVOK_MEMORY_CLASS_H

#include "phys/phys_memory.h"

struct phys_memory_stats;

class c_havok_memory : public phys_memory
{
public:
    __forceinline c_havok_memory() {}
    virtual ~c_havok_memory();
    virtual void allocate_chunk_batch(void **blocks, int count, int size);
    virtual void free_chunk_batch(void **blocks, int count, int size);
    virtual void *allocate_chunk(int size, phys_memory_class memory_class);
    virtual void free_chunk(void *block, int size, phys_memory_class memory_class);
    virtual void *allocate_runtime_block(int size, phys_memory_class memory_class);
    virtual void free_runtime_block(void *block, int size, phys_memory_class memory_class);
    virtual void preallocate_runtime_block(int size, phys_memory_class memory_class);
    virtual void provide_runtime_block(void *block, int size, phys_memory_class memory_class);
    virtual void release_runtime_blocks();
    virtual void print_memory_stats(phys_out_stream *stream);
    virtual void getStatSynopsis(phys_memory_stats *statistics);
private:
    char m_memory_data[0x32a40 - 0xa30];
};
extern c_havok_memory g_havok_memory;


typedef char havok_size_c_havok_memory[(sizeof(void *) != 4 || ((sizeof(c_havok_memory) == 0x32a40) )) ? 1 : -1];

#endif
