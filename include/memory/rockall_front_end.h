#ifndef MEMORY_ROCKALL_FRONT_END_H
#define MEMORY_ROCKALL_FRONT_END_H

#include <stddef.h>
#include "cseries/crt_xdk.h"

class CACHE;
class FIND;
class HEAP;
class NEW_PAGE;
class ROCKALL_BACK_END;
class THREAD_SAFE;

// Rockall's interface order is shared by rockall_heap.cpp and network_memory.cpp.
class ROCKALL_FRONT_END
{
public:
    struct CACHE_DETAILS
    {
        int size;
        int cache_size;
        int chunk_size;
        int allocation_size;
    };
    ROCKALL_FRONT_END(CACHE_DETAILS *caches1, CACHE_DETAILS *caches2, int find_cache_size, int find_cache_threshold,
        int find_size, int max_free_space, int *new_page_sizes, ROCKALL_BACK_END *back_end, bool recycle,
        bool single_image, int stride, int thread_safe_size, bool delete_all);
    virtual bool Delete(void *allocation, int flags);
    virtual bool Details(void *allocation, int *size);
    virtual bool KnownArea(void *allocation);
    virtual bool MultipleDelete(int count, void **const allocations, int flags);
    virtual bool MultipleNew(int *sizes, void **const allocations, int count, int alignment, int *actual_sizes, bool commit);
    virtual void *New(int size, int *actual_size, bool commit);
    virtual void *Resize(void *allocation, int size, int alignment, int *actual_size, bool move, bool commit);
    virtual bool Verify(void *allocation, int *size);
    virtual void DeleteAll(bool release);
    virtual void LockAll();
    virtual bool Truncate(int size);
    virtual void UnlockAll();
    virtual bool Walk(bool *first, void **allocation, int *size);
    virtual ~ROCKALL_FRONT_END();
    static __forceinline void operator delete(void *allocation)
    {
        if (allocation)
            RtlFreeHeap(GetProcessHeap(), 0, allocation);
    }
protected:
    virtual void Exception(char *message);
    virtual void *SpecialNew(int size);
private:
    // Layout from the stores in the March ROCKALL_FRONT_END constructor (0x83422210).
    CACHE *Caches;
    THREAD_SAFE *ThreadSafe;
    HEAP *Heap;
    NEW_PAGE *NewPage;
    FIND *PrivateFind;
    FIND *PublicFind;
    ROCKALL_BACK_END *RockallBackEnd;
    void *Allocation;
    bool GlobalDelete;
    int GuardWord;
    int NumberOfCaches;
    int TotalSize;
};

class c_fixed_memory_rockall_frontend : public ROCKALL_FRONT_END {
public:
    c_fixed_memory_rockall_frontend(void *, long, long *, bool, bool);
    static __forceinline void *operator new(size_t size) {
        return RtlAllocateHeap(GetProcessHeap(), 0, size);
    }
    virtual ~c_fixed_memory_rockall_frontend() {}
};

#endif // MEMORY_ROCKALL_FRONT_END_H
