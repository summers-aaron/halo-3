// port/sdk/kernel: implementation of the Rockall heap front-end and default back-end (ROCKALL_FRONT_END,
// ROCKALL_BACK_END), routing page requests through RockallBackEnd->NewArea / DeleteArea so c_fixed_memory_rockall_backend
// keeps allocations inside the game's fixed-address arenas and maintains free-page counts.
#include <xtl.h>
#include "memory/rockall_front_end.h"

class ROCKALL_BACK_END
{
public:
    ROCKALL_BACK_END();
    virtual void DeleteArea(void *area, int size, bool release);
    virtual int NaturalSize();
    virtual void *NewArea(int alignment_mask, int size, bool commit);
    virtual ~ROCKALL_BACK_END();
};

namespace {

enum {
    k_arena_magic = 0x524b4152u, // 'RKAR'
    k_block_magic = 0x524b424cu, // 'RKBL'
    k_page_size   = 4096,
};

struct s_rockall_arena;

struct s_rockall_block {
    unsigned int magic;
    int user_size;
    int payload_bytes;
    int is_free;
    s_rockall_arena *arena;
    s_rockall_block *prev_phys;
    s_rockall_block *next_phys;
    unsigned int pad[sizeof(void *) == 8 ? 2 : 1];
};

typedef char rockall_block_align_check[(sizeof(s_rockall_block) == (sizeof(void *) == 8 ? 48 : 32)) ? 1 : -1];

struct s_rockall_arena {
    unsigned int magic;
    int total_bytes;
    s_rockall_arena *prev;
    s_rockall_arena *next;
    s_rockall_block *first_block;
    unsigned int pad[sizeof(void *) == 8 ? 4 : 3];
};

typedef char rockall_arena_align_check[(sizeof(s_rockall_arena) == (sizeof(void *) == 8 ? 48 : 32)) ? 1 : -1];

enum {
    k_free_bins = 24,
};

struct s_rockall_free_links {
    s_rockall_block *prev_free;
    s_rockall_block *next_free;
};

static inline s_rockall_free_links *free_links_of(s_rockall_block *blk)
{
    return (s_rockall_free_links *)(blk + 1);
}

static inline int rockall_size_to_bin(int payload_bytes)
{
    if (payload_bytes <= 256)
        return (payload_bytes >> 4) - 1;
    unsigned int v = (unsigned int)(payload_bytes - 1);
    int log2 = 31 - __builtin_clz(v);
    int bin = 16 + (log2 - 8);
    return bin >= k_free_bins ? (k_free_bins - 1) : bin;
}

} // namespace

class HEAP
{
public:
    bool use_lock;
    CRITICAL_SECTION cs;
    s_rockall_arena *arenas;
    s_rockall_arena *last_arena;
    int stride;
    unsigned int free_bitmap;
    s_rockall_block *free_bins[k_free_bins];

    void lock()
    {
        if (use_lock)
            EnterCriticalSection(&cs);
    }

    void unlock()
    {
        if (use_lock)
            LeaveCriticalSection(&cs);
    }
};

static inline void free_list_insert(HEAP *h, s_rockall_block *blk)
{
    int bin = rockall_size_to_bin(blk->payload_bytes);
    s_rockall_block *head = h->free_bins[bin];
    s_rockall_free_links *links = free_links_of(blk);
    links->prev_free = 0;
    links->next_free = head;
    if (head)
        free_links_of(head)->prev_free = blk;
    h->free_bins[bin] = blk;
    h->free_bitmap |= (1u << bin);
}

static inline void free_list_remove(HEAP *h, s_rockall_block *blk)
{
    int bin = rockall_size_to_bin(blk->payload_bytes);
    s_rockall_free_links *links = free_links_of(blk);
    if (links->prev_free)
        free_links_of(links->prev_free)->next_free = links->next_free;
    else
        h->free_bins[bin] = links->next_free;
    if (links->next_free)
        free_links_of(links->next_free)->prev_free = links->prev_free;
    links->prev_free = 0;
    links->next_free = 0;
    if (!h->free_bins[bin])
        h->free_bitmap &= ~(1u << bin);
}

static void init_arena_free_block(HEAP *h, s_rockall_arena *arena)
{
    s_rockall_block *blk = (s_rockall_block *)(arena + 1);
    blk->magic = k_block_magic;
    blk->user_size = 0;
    blk->payload_bytes = arena->total_bytes - (int)sizeof(s_rockall_arena) - (int)sizeof(s_rockall_block);
    blk->is_free = 1;
    blk->arena = arena;
    blk->prev_phys = 0;
    blk->next_phys = 0;
    blk->pad[0] = 0;
    arena->first_block = blk;
    free_list_insert(h, blk);
}

static inline s_rockall_block *check_arena_block(s_rockall_arena *a, void *allocation)
{
    char *ptr = (char *)allocation;
    char *start = (char *)(a + 1) + sizeof(s_rockall_block);
    char *end = (char *)a + a->total_bytes;
    if (ptr >= start && ptr < end) {
        s_rockall_block *blk = (s_rockall_block *)allocation - 1;
        if (blk->magic == k_block_magic && blk->arena == a && !blk->is_free)
            return blk;
    }
    return 0;
}

static s_rockall_block *find_valid_block(HEAP *h, void *allocation)
{
    if (!h || !allocation || ((size_t)allocation & 15u) != 0)
        return 0;
    s_rockall_arena *cached = h->last_arena;
    if (cached) {
        s_rockall_block *blk = check_arena_block(cached, allocation);
        if (blk)
            return blk;
    }
    char *ptr = (char *)allocation;
    for (s_rockall_arena *a = h->arenas; a != 0; a = a->next) {
        if (a == cached)
            continue;
        char *start = (char *)(a + 1) + sizeof(s_rockall_block);
        char *end = (char *)a + a->total_bytes;
        if (ptr >= start && ptr < end) {
            s_rockall_block *blk = (s_rockall_block *)allocation - 1;
            if (blk->magic == k_block_magic && blk->arena == a && !blk->is_free) {
                h->last_arena = a;
                return blk;
            }
            return 0;
        }
    }
    return 0;
}

static void split_block_if_needed(HEAP *h, s_rockall_block *blk, int needed)
{
    int min_rem = (int)sizeof(s_rockall_block) + 16;
    if (blk->payload_bytes >= needed + min_rem) {
        s_rockall_block *rem = (s_rockall_block *)((char *)(blk + 1) + needed);
        rem->magic = k_block_magic;
        rem->user_size = 0;
        rem->payload_bytes = blk->payload_bytes - needed - (int)sizeof(s_rockall_block);
        rem->is_free = 1;
        rem->arena = blk->arena;
        rem->prev_phys = blk;
        rem->next_phys = blk->next_phys;
        rem->pad[0] = 0;
        if (blk->next_phys)
            blk->next_phys->prev_phys = rem;
        blk->next_phys = rem;
        blk->payload_bytes = needed;
        free_list_insert(h, rem);
    }
}

static void coalesce_free_block(HEAP *h, s_rockall_block *blk)
{
    if (blk->next_phys && blk->next_phys->is_free) {
        s_rockall_block *nx = blk->next_phys;
        free_list_remove(h, nx);
        blk->payload_bytes += (int)sizeof(s_rockall_block) + nx->payload_bytes;
        blk->next_phys = nx->next_phys;
        if (nx->next_phys)
            nx->next_phys->prev_phys = blk;
        nx->magic = 0;
    }
    if (blk->prev_phys && blk->prev_phys->is_free) {
        s_rockall_block *pv = blk->prev_phys;
        free_list_remove(h, pv);
        pv->payload_bytes += (int)sizeof(s_rockall_block) + blk->payload_bytes;
        pv->next_phys = blk->next_phys;
        if (blk->next_phys)
            blk->next_phys->prev_phys = pv;
        blk->magic = 0;
        blk = pv;
    }
    free_list_insert(h, blk);
}

// ---------------------------------------------------------------------------------------------------------------
// ROCKALL_BACK_END

ROCKALL_BACK_END::ROCKALL_BACK_END() {}
ROCKALL_BACK_END::~ROCKALL_BACK_END() {}

int ROCKALL_BACK_END::NaturalSize()
{
    return k_page_size;
}

void *ROCKALL_BACK_END::NewArea(int alignment_mask, int size, bool commit)
{
    (void)alignment_mask;
    if (size <= 0)
        return 0;
    DWORD type = MEM_RESERVE | (commit ? MEM_COMMIT : 0);
    return VirtualAlloc(0, (SIZE_T)size, type, PAGE_READWRITE);
}

void ROCKALL_BACK_END::DeleteArea(void *area, int size, bool release)
{
    if (!area)
        return;
    if (release)
        VirtualFree(area, 0, MEM_RELEASE);
    else if (size > 0)
        VirtualFree(area, (SIZE_T)size, MEM_DECOMMIT);
}

// ---------------------------------------------------------------------------------------------------------------
// ROCKALL_FRONT_END

static ROCKALL_BACK_END g_default_rockall_back_end;

ROCKALL_FRONT_END::ROCKALL_FRONT_END(CACHE_DETAILS *caches1, CACHE_DETAILS *caches2, int find_cache_size,
    int find_cache_threshold, int find_size, int max_free_space, int *new_page_sizes, ROCKALL_BACK_END *back_end,
    bool recycle, bool single_image, int stride, int thread_safe_size, bool delete_all)
{
    (void)caches1; (void)caches2; (void)find_cache_size; (void)find_cache_threshold;
    (void)find_size; (void)max_free_space; (void)new_page_sizes; (void)recycle; (void)single_image;

    Caches = 0;
    ThreadSafe = 0;
    NewPage = 0;
    PrivateFind = 0;
    PublicFind = 0;
    RockallBackEnd = back_end ? back_end : &g_default_rockall_back_end;
    Allocation = 0;
    GlobalDelete = delete_all;
    GuardWord = 0;
    NumberOfCaches = 0;
    TotalSize = 0;

    Heap = (HEAP *)RtlAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(HEAP));
    if (Heap) {
        Heap->arenas = 0;
        Heap->last_arena = 0;
        Heap->stride = stride > 16 ? stride : 16;
        Heap->free_bitmap = 0;
        Heap->use_lock = thread_safe_size != 0;
        if (Heap->use_lock)
            InitializeCriticalSection(&Heap->cs);
    }
}

ROCKALL_FRONT_END::~ROCKALL_FRONT_END()
{
    if (Heap) {
        if (GlobalDelete)
            DeleteAll(true);
        if (Heap->use_lock)
            DeleteCriticalSection(&Heap->cs);
        RtlFreeHeap(GetProcessHeap(), 0, Heap);
        Heap = 0;
    }
}

void *ROCKALL_FRONT_END::New(int size, int *actual_size, bool commit)
{
    if (!Heap || size < 0)
        return 0;

    int needed = size <= 16 ? 16 : ((size + 15) & ~15);
    int min_bin = rockall_size_to_bin(needed);
    Heap->lock();

    s_rockall_block *found = 0;
    if (Heap->free_bitmap & (1u << min_bin)) {
        if (min_bin < 16) {
            found = Heap->free_bins[min_bin];
        } else {
            for (s_rockall_block *b = Heap->free_bins[min_bin]; b != 0; b = free_links_of(b)->next_free) {
                if (b->payload_bytes >= needed) {
                    found = b;
                    break;
                }
            }
        }
    }
    if (!found && min_bin + 1 < k_free_bins) {
        unsigned int higher = Heap->free_bitmap & (~0u << (min_bin + 1));
        if (higher)
            found = Heap->free_bins[__builtin_ctz(higher)];
    }
    if (found) {
        free_list_remove(Heap, found);
        split_block_if_needed(Heap, found, needed);
        found->is_free = 0;
        found->user_size = size;
        Heap->last_arena = found->arena;
        if (actual_size)
            *actual_size = found->payload_bytes;
        Heap->unlock();
        return (void *)(found + 1);
    }

    int min_arena = ((int)sizeof(s_rockall_arena) + (int)sizeof(s_rockall_block) + needed + (k_page_size - 1)) & ~(k_page_size - 1);
    int pref_arena = min_arena < 65536 ? 65536 : min_arena;
    void *raw = RockallBackEnd->NewArea(k_page_size - 1, pref_arena, commit ? true : true);
    int alloc_arena_bytes = pref_arena;
    if (!raw && pref_arena > min_arena) {
        raw = RockallBackEnd->NewArea(k_page_size - 1, min_arena, true);
        alloc_arena_bytes = min_arena;
    }
    if (!raw) {
        Heap->unlock();
        return 0;
    }

    s_rockall_arena *arena = (s_rockall_arena *)raw;
    arena->magic = k_arena_magic;
    arena->total_bytes = alloc_arena_bytes;
    arena->prev = 0;
    arena->next = Heap->arenas;
    if (Heap->arenas)
        Heap->arenas->prev = arena;
    Heap->arenas = arena;
    Heap->last_arena = arena;
    TotalSize += alloc_arena_bytes;
    init_arena_free_block(Heap, arena);

    s_rockall_block *blk = arena->first_block;
    free_list_remove(Heap, blk);
    split_block_if_needed(Heap, blk, needed);
    blk->is_free = 0;
    blk->user_size = size;
    if (actual_size)
        *actual_size = blk->payload_bytes;

    Heap->unlock();
    return (void *)(blk + 1);
}

bool ROCKALL_FRONT_END::Delete(void *allocation, int flags)
{
    (void)flags;
    if (!allocation)
        return true;
    if (!Heap)
        return false;

    Heap->lock();
    s_rockall_block *blk = find_valid_block(Heap, allocation);
    if (!blk) {
        Heap->unlock();
        return false;
    }
    blk->is_free = 1;
    blk->user_size = 0;
    coalesce_free_block(Heap, blk);
    Heap->unlock();
    return true;
}

bool ROCKALL_FRONT_END::Details(void *allocation, int *size)
{
    if (!Heap)
        return false;
    Heap->lock();
    s_rockall_block *blk = find_valid_block(Heap, allocation);
    bool ok = blk != 0;
    if (ok && size)
        *size = blk->payload_bytes;
    Heap->unlock();
    return ok;
}

bool ROCKALL_FRONT_END::Verify(void *allocation, int *size)
{
    if (!allocation)
        return Heap != 0;
    return Details(allocation, size);
}

bool ROCKALL_FRONT_END::KnownArea(void *allocation)
{
    if (!Heap || !allocation)
        return false;
    Heap->lock();
    bool known = false;
    char *ptr = (char *)allocation;
    for (s_rockall_arena *a = Heap->arenas; a != 0; a = a->next) {
        if (ptr >= (char *)a && ptr < (char *)a + a->total_bytes) {
            known = true;
            break;
        }
    }
    Heap->unlock();
    return known;
}

bool ROCKALL_FRONT_END::MultipleDelete(int count, void **const allocations, int flags)
{
    bool ok = true;
    for (int i = 0; i < count; i++) {
        if (!Delete(allocations[i], flags))
            ok = false;
    }
    return ok;
}

bool ROCKALL_FRONT_END::MultipleNew(int *sizes, void **const allocations, int count, int alignment, int *actual_sizes, bool commit)
{
    (void)alignment;
    for (int i = 0; i < count; i++) {
        int actual = 0;
        allocations[i] = New(sizes[i], &actual, commit);
        if (actual_sizes)
            actual_sizes[i] = actual;
        if (!allocations[i]) {
            for (int j = 0; j < i; j++) {
                Delete(allocations[j], -1);
                allocations[j] = 0;
            }
            return false;
        }
    }
    return true;
}

void *ROCKALL_FRONT_END::Resize(void *allocation, int size, int alignment, int *actual_size, bool move, bool commit)
{
    (void)alignment;
    if (!allocation)
        return New(size, actual_size, commit);
    if (size <= 0) {
        Delete(allocation, -1);
        if (actual_size)
            *actual_size = 0;
        return 0;
    }
    if (!Heap)
        return 0;

    int needed = size <= 16 ? 16 : ((size + 15) & ~15);
    Heap->lock();
    s_rockall_block *blk = find_valid_block(Heap, allocation);
    if (!blk) {
        Heap->unlock();
        return 0;
    }
    int old_user = blk->user_size;
    if (needed <= blk->payload_bytes) {
        split_block_if_needed(Heap, blk, needed);
        blk->user_size = size;
        if (actual_size)
            *actual_size = blk->payload_bytes;
        Heap->unlock();
        return allocation;
    }
    if (blk->next_phys && blk->next_phys->is_free &&
        blk->payload_bytes + (int)sizeof(s_rockall_block) + blk->next_phys->payload_bytes >= needed) {
        s_rockall_block *nx = blk->next_phys;
        free_list_remove(Heap, nx);
        blk->payload_bytes += (int)sizeof(s_rockall_block) + nx->payload_bytes;
        blk->next_phys = nx->next_phys;
        if (nx->next_phys)
            nx->next_phys->prev_phys = blk;
        nx->magic = 0;
        split_block_if_needed(Heap, blk, needed);
        blk->user_size = size;
        if (actual_size)
            *actual_size = blk->payload_bytes;
        Heap->unlock();
        return allocation;
    }
    Heap->unlock();

    if (!move)
        return 0;
    void *fresh = New(size, actual_size, commit);
    if (!fresh)
        return 0;
    int copy_bytes = old_user < size ? old_user : size;
    if (copy_bytes > 0)
        __builtin_memcpy(fresh, allocation, (size_t)copy_bytes);
    Delete(allocation, -1);
    return fresh;
}

void ROCKALL_FRONT_END::DeleteAll(bool release)
{
    if (!Heap)
        return;
    Heap->lock();
    Heap->free_bitmap = 0;
    __builtin_memset(Heap->free_bins, 0, sizeof(Heap->free_bins));
    Heap->last_arena = 0;
    s_rockall_arena *a = Heap->arenas;
    while (a) {
        s_rockall_arena *next = a->next;
        if (release) {
            TotalSize -= a->total_bytes;
            RockallBackEnd->DeleteArea(a, a->total_bytes, true);
        } else {
            init_arena_free_block(Heap, a);
        }
        a = next;
    }
    if (release)
        Heap->arenas = 0;
    Heap->unlock();
}

bool ROCKALL_FRONT_END::Truncate(int size)
{
    (void)size;
    if (!Heap)
        return false;
    Heap->lock();
    s_rockall_arena *a = Heap->arenas;
    while (a) {
        s_rockall_arena *next = a->next;
        s_rockall_block *b = a->first_block;
        if (b && b->is_free && b->prev_phys == 0 && b->next_phys == 0) {
            free_list_remove(Heap, b);
            if (Heap->last_arena == a)
                Heap->last_arena = 0;
            if (a->prev)
                a->prev->next = a->next;
            else
                Heap->arenas = a->next;
            if (a->next)
                a->next->prev = a->prev;
            TotalSize -= a->total_bytes;
            RockallBackEnd->DeleteArea(a, a->total_bytes, true);
        }
        a = next;
    }
    Heap->unlock();
    return true;
}

void ROCKALL_FRONT_END::LockAll()
{
    if (Heap)
        Heap->lock();
}

void ROCKALL_FRONT_END::UnlockAll()
{
    if (Heap)
        Heap->unlock();
}

bool ROCKALL_FRONT_END::Walk(bool *first, void **allocation, int *size)
{
    if (!Heap || !first || !allocation)
        return false;
    Heap->lock();
    s_rockall_block *target = 0;
    if (*first) {
        *first = false;
        for (s_rockall_arena *a = Heap->arenas; a != 0 && !target; a = a->next) {
            for (s_rockall_block *b = a->first_block; b != 0; b = b->next_phys) {
                if (!b->is_free) {
                    target = b;
                    break;
                }
            }
        }
    } else if (*allocation) {
        s_rockall_block *cur = find_valid_block(Heap, *allocation);
        if (cur) {
            for (s_rockall_block *b = cur->next_phys; b != 0; b = b->next_phys) {
                if (!b->is_free) {
                    target = b;
                    break;
                }
            }
            for (s_rockall_arena *a = cur->arena->next; a != 0 && !target; a = a->next) {
                for (s_rockall_block *b = a->first_block; b != 0; b = b->next_phys) {
                    if (!b->is_free) {
                        target = b;
                        break;
                    }
                }
            }
        }
    }
    if (target) {
        *allocation = (void *)(target + 1);
        if (size)
            *size = target->payload_bytes;
        Heap->unlock();
        return true;
    }
    *allocation = 0;
    if (size)
        *size = 0;
    Heap->unlock();
    return false;
}

void ROCKALL_FRONT_END::Exception(char *message)
{
    (void)message;
}

void *ROCKALL_FRONT_END::SpecialNew(int size)
{
    return size > 0 ? RtlAllocateHeap(GetProcessHeap(), HEAP_ZERO_MEMORY, (SIZE_T)size) : 0;
}
