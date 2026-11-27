// blamlibxbox/rockall_heap.obj: from the records 0x57e848 0x57e8c0 0x57e8c8 0x57ead8 0x57eb80 0x57ebe0
// @flags /TP /O2 /Oc
// @address 0x57e848 ?DeleteArea@c_fixed_memory_rockall_backend@@UAAXPAXH_N@Z
// @address 0x57e8c0 ?NaturalSize@c_fixed_memory_rockall_backend@@UAAHXZ
// @address 0x57e8c8 ?NewArea@c_fixed_memory_rockall_backend@@UAAPAXHH_N@Z
// @address 0x57ead8 ?allocate@c_rockall_heap@@QAAPAXJ@Z
// @address 0x57eb80 ?deallocate@c_rockall_heap@@QAAXPAX@Z
// @address 0x57ebe0 ??0c_fixed_memory_rockall_frontend@@QAA@PAXJPAJ_N2@Z

#include "cseries/crt_xdk.h"
#include "cseries/placement_new.h"
#include "cseries/pointer_utilities.h"

class ROCKALL_BACK_END
{
public:
    ROCKALL_BACK_END();
    virtual void DeleteArea(void *area, int size, bool release);
    virtual int NaturalSize();
    virtual void *NewArea(int size, int alignment, bool commit);
    virtual ~ROCKALL_BACK_END();
    static __forceinline void operator delete(void *allocation)
    {
        if (allocation)
            RtlFreeHeap(GetProcessHeap(), 0, allocation);
    }
};

#include "networking/network_memory_types.h"
enum
{
    k_rockall_page_size = 4096,
};


class c_fixed_memory_rockall_backend : public ROCKALL_BACK_END
{
public:
    c_fixed_memory_rockall_backend(void *memory, long size, long *free_page_count, bool clear_free_page_count);
    virtual ~c_fixed_memory_rockall_backend() {}
    virtual void DeleteArea(void *area, int size, bool release);
    virtual int NaturalSize();
    virtual void *NewArea(int alignment_mask, int size, bool commit);
    __forceinline void flag_and_check_pages(long first, long count, bool allocated);
    char *m_base;
    char *m_end;
    int m_page_count;
#ifdef HALO3_64BIT
    unsigned int m_page_bits[1015];
#else
    unsigned int m_page_bits[1019];
#endif
    long *m_free_page_count;
};
typedef char fixed_memory_rockall_backend_size[sizeof(c_fixed_memory_rockall_backend) == 4096 ? 1 : -1];


// match: 0x57ebe0: Keep the mutable cache and page-size tables file-static for the constructor's shared data anchor.
// @address 0x72c4f8 rockall_small_caches
static ROCKALL_FRONT_END::CACHE_DETAILS rockall_small_caches[] =
{
    { 4, 0, 32, 512 },
    { 8, 0, 32, 512 },
    { 12, 0, 64, 512 },
    { 16, 0, 64, 512 },
    { 20, 0, 64, 512 },
    { 24, 0, 512, 512 },
    { 28, 0, 512, 512 },
    { 32, 0, 512, 512 },
    { 40, 0, 512, 512 },
    { 48, 0, 512, 512 },
    { 56, 0, 512, 512 },
    { 64, 0, 1024, 1024 },
    { 80, 0, 1024, 1024 },
    { 96, 0, 1024, 1024 },
    { 112, 0, 1024, 1024 },
    { 128, 0, 2048, 2048 },
    { 160, 0, 2048, 2048 },
    { 192, 0, 2048, 2048 },
    { 224, 0, 2048, 2048 },
    { 256, 0, 4096, 4096 },
    { 320, 0, 4096, 4096 },
    { 384, 0, 4096, 4096 },
    { 448, 0, 4096, 4096 },
    { 512, 0, 4096, 4096 },
    { 576, 0, 8192, 8192 },
    { 640, 0, 4096, 4096 },
    { 704, 0, 4096, 4096 },
    { 768, 0, 4096, 4096 },
    { 832, 0, 8192, 8192 },
    { 896, 0, 8192, 8192 },
    { 960, 0, 4096, 4096 },
    { 0, 0, 0, 0 },
};

// @address 0x72c6f8 rockall_large_caches
static ROCKALL_FRONT_END::CACHE_DETAILS rockall_large_caches[] =
{
    { 1024, 0, 8192, 8192 },
    { 2048, 0, 8192, 8192 },
    { 3072, 0, 8192, 8192 },
    { 4096, 0, 65536, 65536 },
    { 5120, 0, 65536, 65536 },
    { 6144, 0, 65536, 65536 },
    { 7168, 0, 65536, 65536 },
    { 8192, 0, 65536, 65536 },
    { 9216, 0, 65536, 65536 },
    { 10240, 0, 65536, 65536 },
    { 12288, 0, 65536, 65536 },
    { 16384, 0, 65536, 65536 },
    { 21504, 0, 65536, 65536 },
    { 32768, 0, 65536, 65536 },
    { 65536, 0, 65536, 65536 },
    { 65536, 0, 65536, 65536 },
    { 0, 0, 0, 0 },
};

// @address 0x72c4e4 rockall_new_page_sizes
static int rockall_new_page_sizes[] = { 1, 4, 16, 64, 0 };

void c_fixed_memory_rockall_backend::DeleteArea(void *area, int size, bool release)
{
    int first_page = ((char *)area - m_base) / 4096;
    int last_page = ((char *)area - m_base + size + 4095) / 4096;
    for (int page = first_page; page < last_page; page++)
        m_page_bits[page >> 5] &= ~(1 << (page & 31));
    *m_free_page_count = *m_free_page_count - last_page + first_page;
}


int c_fixed_memory_rockall_backend::NaturalSize()
{
    return k_rockall_page_size;
}

__forceinline void c_fixed_memory_rockall_backend::flag_and_check_pages(long first, long count, bool allocated)
{
    for (long page = first; page < first + count; page++)
    {
        if (allocated)
            m_page_bits[page >> 5] |= 1 << (page & 31);
        else
            m_page_bits[page >> 5] &= ~(1 << (page & 31));
    }
}

void *c_fixed_memory_rockall_backend::NewArea(int alignment_mask, int size, bool commit)
{
    void *area = 0;
    long page_alignment = 0;
    long alignment_bits = 0;
    switch (alignment_mask)
    {
    case k_rockall_page_size * 1 - 1: page_alignment = 1; alignment_bits = 0; break;
    case k_rockall_page_size * 2 - 1: page_alignment = 2; alignment_bits = 1; break;
    case k_rockall_page_size * 4 - 1: page_alignment = 4; alignment_bits = 2; break;
    case k_rockall_page_size * 8 - 1: page_alignment = 8; alignment_bits = 3; break;
    case k_rockall_page_size * 16 - 1: page_alignment = 16; alignment_bits = 4; break;
    }
    if (page_alignment > 0)
    {
        long page_count = (size + 4095) / 4096;
        long first_page = pointer_difference(m_base,
            align_pointer(m_base, alignment_bits + 12)) / 4096;
        long initial_page = first_page;
        while (first_page + page_count <= m_page_count)
        {
            bool available = true;
            long scanned_page = first_page;
            long end_page = first_page + page_count;
            for (long page = first_page; page < end_page && available; page++, scanned_page++)
                available = (m_page_bits[page >> 5] & (1 << (page & 31))) == 0;
            if (available)
                break;
            else
                first_page = initial_page + align_address(
                    (first_page + 1 > scanned_page ? first_page + 1 : scanned_page) - initial_page,
                    alignment_bits);
        }
        if (first_page + page_count <= m_page_count)
        {
            flag_and_check_pages(first_page, page_count, true);
            area = m_base + first_page * 4096;
            *m_free_page_count += page_count;
        }
    }
    return area;
}

void *c_rockall_heap::allocate(long size)
{
    void *allocation = m_rockall_front_end->New(size, 0, false);
    if (!allocation)
    {
        m_rockall_front_end->Truncate(0);
        allocation = m_rockall_front_end->New(size, 0, false);
        if (!allocation)
            return 0;
    }
    m_allocation_count++;
    return allocation;
}

void c_rockall_heap::deallocate(void *allocation)
{
    m_rockall_front_end->Delete(allocation, -1);
    if (allocation)
        m_allocation_count--;
}

c_fixed_memory_rockall_backend::c_fixed_memory_rockall_backend(void *memory, long size, long *free_page_count, bool clear_free_page_count)
{
    m_base = (char *)memory + 4096;
    m_end = (char *)memory + size;
    m_free_page_count = free_page_count;
    memset(m_page_bits, 0, sizeof(m_page_bits));
    m_page_count = size / 4096 - 1;
    if (clear_free_page_count)
        *m_free_page_count = 0;
}

c_fixed_memory_rockall_frontend::c_fixed_memory_rockall_frontend(void *memory, long size, long *free_page_count, bool, bool)
    : ROCKALL_FRONT_END(rockall_small_caches, rockall_large_caches, 2048, 0, 1024, 0x100000, rockall_new_page_sizes,
        new (memory) c_fixed_memory_rockall_backend(memory, size, free_page_count, true), true, false, 4, 1024, false)
{
}
