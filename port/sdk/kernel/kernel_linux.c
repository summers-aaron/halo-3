// port/sdk/kernel (Linux host): the Xbox's Win32-style kernel API over POSIX threads and files. The same names as
// the Windows host defines (win32_windows.c, time_windows.c and the forwarded list win32_forward.txt), with Win32
// meaning; declarations follow the game's (include/cseries/crt_xdk.h, the XDK's winbase.h).
//
// Handles index one table of kernel objects: files, find searches, events, semaphores, mutexes, threads and I/O
// completion ports. Waitable state lives under one lock, and every change wakes every waiter, which then rechecks
// what it waits for: simple and correct, and the game's waits are few.
//
// Files: Xbox drive paths ("d:\...", "cache0:\...", "cache1:\...") are translated via storage.c.
#define _GNU_SOURCE
#include "map_swap.h"
#include "storage.h"
#include "xapi_io.h"
#include "../runtime/port_runtime.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <linux/futex.h>
#include <signal.h>
#include <sys/syscall.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <unistd.h>

typedef unsigned int port_dword;
typedef unsigned long port_size;
typedef unsigned short port_word;

#define PORT_INVALID_HANDLE ((void *)-1)
#define PORT_CURRENT_THREAD ((void *)-2)
#define PORT_INFINITE 0xfffffffful
#define PORT_WAIT_OBJECT_0 0ul
#define PORT_WAIT_TIMEOUT 0x102ul
#define PORT_WAIT_FAILED 0xfffffffful
#define PORT_STILL_ACTIVE 259ul

#define PORT_ERROR_FILE_NOT_FOUND 2ul
#define PORT_ERROR_PATH_NOT_FOUND 3ul
#define PORT_ERROR_ACCESS_DENIED 5ul
#define PORT_ERROR_INVALID_HANDLE 6ul
#define PORT_ERROR_NOT_ENOUGH_MEMORY 8ul
#define PORT_ERROR_NO_MORE_FILES 18ul
#define PORT_ERROR_GEN_FAILURE 31ul
#define PORT_ERROR_SHARING_VIOLATION 32ul
#define PORT_ERROR_HANDLE_EOF 38ul
#define PORT_ERROR_FILE_EXISTS 80ul
#define PORT_ERROR_INVALID_PARAMETER 87ul
#define PORT_ERROR_DISK_FULL 112ul
#define PORT_ERROR_DIR_NOT_EMPTY 145ul
#define PORT_ERROR_ALREADY_EXISTS 183ul
#define PORT_ERROR_NOT_OWNER 288ul
#define PORT_ERROR_TOO_MANY_POSTS 298ul

// ---------------------------------------------------------------------------------------------------------------
// Last error.

static __thread port_dword port_last_error;

port_dword PORT_MS_ABI GetLastError(void) { return port_last_error; }
void PORT_MS_ABI SetLastError(port_dword error) { port_last_error = error; }
port_dword port_nt_status_to_win32(long status); // xapi_system.c
void PORT_MS_ABI RtlSetLastNTError(long status) { port_last_error = port_nt_status_to_win32(status); }

static port_dword port_error_from_errno(int error)
{
    switch (error) {
    case ENOENT: return PORT_ERROR_FILE_NOT_FOUND;
    case ENOTDIR: return PORT_ERROR_PATH_NOT_FOUND;
    case EACCES: case EPERM: case EROFS: return PORT_ERROR_ACCESS_DENIED;
    case EEXIST: return PORT_ERROR_ALREADY_EXISTS;
    case ENOSPC: return PORT_ERROR_DISK_FULL;
    case ENOTEMPTY: return PORT_ERROR_DIR_NOT_EMPTY;
    case EBADF: return PORT_ERROR_INVALID_HANDLE;
    case ENOMEM: return PORT_ERROR_NOT_ENOUGH_MEMORY;
    case EINVAL: return PORT_ERROR_INVALID_PARAMETER;
    }
    return PORT_ERROR_GEN_FAILURE;
}

static int port_fail_errno(void)
{
    port_last_error = port_error_from_errno(errno);
    return 0;
}

// ---------------------------------------------------------------------------------------------------------------
// Critical sections (the Xbox's Rtl names): a recursive pthread mutex inside the game's 28-byte structure.

#ifdef __x86_64__
#include <linux/futex.h>
typedef struct port_cs64 {
    unsigned int state;      // 0 = unlocked, 1 = locked (no waiters), 2 = locked (waiters)
    unsigned int owner;      // linux tid of owner
    unsigned int recursion;  // nesting depth
    unsigned int spin_count; // user-mode spin iterations before futex sleep
} port_cs64;
_Static_assert(sizeof(port_cs64) <= 28, "port_cs64 fits inside Xbox 28-byte RTL_CRITICAL_SECTION");

static unsigned int port_cs64_tid(void)
{
    static __thread unsigned int cached_tid;
    if (!cached_tid)
        cached_tid = (unsigned int)syscall(SYS_gettid);
    return cached_tid;
}

void PORT_MS_ABI RtlInitializeCriticalSection(void *section)
{
    port_cs64 *cs = (port_cs64 *)section;
    cs->state = 0;
    cs->owner = 0;
    cs->recursion = 0;
    cs->spin_count = 0;
}
void PORT_MS_ABI RtlInitializeCriticalSectionAndSpinCount(void *section, port_dword spin_count)
{
    port_cs64 *cs = (port_cs64 *)section;
    cs->state = 0;
    cs->owner = 0;
    cs->recursion = 0;
    cs->spin_count = spin_count;
}
void PORT_MS_ABI RtlEnterCriticalSection(void *section)
{
    port_cs64 *cs = (port_cs64 *)section;
    unsigned int self = port_cs64_tid();
    if (__atomic_load_n(&cs->owner, __ATOMIC_RELAXED) == self) {
        cs->recursion++;
        return;
    }
    unsigned int zero = 0;
    if (!__atomic_compare_exchange_n(&cs->state, &zero, 1u, 0, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
        if (cs->spin_count != 0u) {
            unsigned int spins = cs->spin_count;
            do {
                __builtin_ia32_pause();
                if (__atomic_load_n(&cs->state, __ATOMIC_RELAXED) == 0u) {
                    zero = 0;
                    if (__atomic_compare_exchange_n(&cs->state, &zero, 1u, 0, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
                        goto acquired;
                }
            } while (--spins != 0u);
        }
        if (zero != 2u)
            zero = __atomic_exchange_n(&cs->state, 2u, __ATOMIC_ACQUIRE);
        while (zero != 0) {
            syscall(SYS_futex, &cs->state, FUTEX_WAIT_PRIVATE, 2u, 0, 0, 0);
            zero = __atomic_exchange_n(&cs->state, 2u, __ATOMIC_ACQUIRE);
        }
    }
acquired:
    __atomic_store_n(&cs->owner, self, __ATOMIC_RELAXED);
    cs->recursion = 1;
}
void PORT_MS_ABI RtlLeaveCriticalSection(void *section)
{
    port_cs64 *cs = (port_cs64 *)section;
    if (--cs->recursion != 0)
        return;
    __atomic_store_n(&cs->owner, 0u, __ATOMIC_RELAXED);
    if (__atomic_sub_fetch(&cs->state, 1u, __ATOMIC_RELEASE) != 0) {
        __atomic_store_n(&cs->state, 0u, __ATOMIC_RELEASE);
        syscall(SYS_futex, &cs->state, FUTEX_WAKE_PRIVATE, 1, 0, 0, 0);
    }
}
port_dword PORT_MS_ABI RtlTryEnterCriticalSection(void *section)
{
    port_cs64 *cs = (port_cs64 *)section;
    unsigned int self = port_cs64_tid();
    if (__atomic_load_n(&cs->owner, __ATOMIC_RELAXED) == self) {
        cs->recursion++;
        return 1;
    }
    unsigned int zero = 0;
    if (!__atomic_compare_exchange_n(&cs->state, &zero, 1u, 0, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
        return 0;
    __atomic_store_n(&cs->owner, self, __ATOMIC_RELAXED);
    cs->recursion = 1;
    return 1;
}
#else
_Static_assert(sizeof(pthread_mutex_t) <= 28, "an Xbox RTL_CRITICAL_SECTION is 28 bytes");

void PORT_MS_ABI RtlInitializeCriticalSection(void *section)
{
    pthread_mutexattr_t attributes;
    pthread_mutexattr_init(&attributes);
    pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init((pthread_mutex_t *)section, &attributes);
    pthread_mutexattr_destroy(&attributes);
}
void PORT_MS_ABI RtlInitializeCriticalSectionAndSpinCount(void *section, port_dword spin_count)
{
    (void)spin_count;
    RtlInitializeCriticalSection(section);
}
void PORT_MS_ABI RtlEnterCriticalSection(void *section) { pthread_mutex_lock((pthread_mutex_t *)section); }
void PORT_MS_ABI RtlLeaveCriticalSection(void *section) { pthread_mutex_unlock((pthread_mutex_t *)section); }
port_dword PORT_MS_ABI RtlTryEnterCriticalSection(void *section) { return pthread_mutex_trylock((pthread_mutex_t *)section) == 0; }
#endif

// ---------------------------------------------------------------------------------------------------------------
// Thread-local slots and lock-free lists.

port_dword PORT_MS_ABI TlsAlloc(void)
{
    pthread_key_t key;
    return pthread_key_create(&key, 0) == 0 ? (port_dword)key : 0xfffffffful;
}
int PORT_MS_ABI TlsFree(port_dword index) { return pthread_key_delete((pthread_key_t)index) == 0; }
void *PORT_MS_ABI TlsGetValue(port_dword index)
{
    port_last_error = 0;
    return pthread_getspecific((pthread_key_t)index);
}
int PORT_MS_ABI TlsSetValue(port_dword index, void *value) { return pthread_setspecific((pthread_key_t)index, value) == 0; }

// SLIST_HEADER: { SLIST_ENTRY Next; WORD Depth; WORD Sequence; }; SLIST_ENTRY: { SLIST_ENTRY *Next; }.
#ifdef __x86_64__
// Xbox SLIST_HEADER is 8 bytes (some callers declare it as __int64). User-mode x86_64 virtual addresses fit in 47 bits,
// leaving the top 16 bits for a lock-free ABA sequence counter with zero global mutex contention.
typedef struct port_slist_entry { struct port_slist_entry *next; } port_slist_entry;
#define PORT_SLIST_PTR_MASK 0x0000ffffffffffffull

void *PORT_MS_ABI InterlockedPushEntrySList(void *head, void *entry)
{
    unsigned long long *slot = (unsigned long long *)head;
    unsigned long long old = __atomic_load_n(slot, __ATOMIC_RELAXED);
    for (;;) {
        port_slist_entry *first = (port_slist_entry *)(__UINTPTR_TYPE__)(old & PORT_SLIST_PTR_MASK);
        ((port_slist_entry *)entry)->next = first;
        unsigned long long seq = (old >> 48) + 1ull;
        unsigned long long next = (seq << 48) | ((unsigned long long)(__UINTPTR_TYPE__)entry & PORT_SLIST_PTR_MASK);
        if (__atomic_compare_exchange_n(slot, &old, next, 1, __ATOMIC_RELEASE, __ATOMIC_RELAXED))
            return first;
    }
}
void *PORT_MS_ABI InterlockedPopEntrySList(void *head)
{
    unsigned long long *slot = (unsigned long long *)head;
    unsigned long long old = __atomic_load_n(slot, __ATOMIC_ACQUIRE);
    for (;;) {
        port_slist_entry *first = (port_slist_entry *)(__UINTPTR_TYPE__)(old & PORT_SLIST_PTR_MASK);
        if (first == 0)
            return 0;
        port_slist_entry *after = first->next;
        unsigned long long seq = (old >> 48) + 1ull;
        unsigned long long next = (seq << 48) | ((unsigned long long)(__UINTPTR_TYPE__)after & PORT_SLIST_PTR_MASK);
        if (__atomic_compare_exchange_n(slot, &old, next, 1, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))
            return first;
    }
}
void *PORT_MS_ABI InterlockedFlushSList(void *head)
{
    unsigned long long *slot = (unsigned long long *)head;
    unsigned long long old = __atomic_load_n(slot, __ATOMIC_ACQUIRE);
    for (;;) {
        port_slist_entry *first = (port_slist_entry *)(__UINTPTR_TYPE__)(old & PORT_SLIST_PTR_MASK);
        if (first == 0)
            return 0;
        unsigned long long seq = (old >> 48) + 1ull;
        unsigned long long next = seq << 48;
        if (__atomic_compare_exchange_n(slot, &old, next, 1, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))
            return first;
    }
}
#else
typedef struct port_slist_entry { struct port_slist_entry *next; } port_slist_entry;
typedef struct port_slist_header { port_slist_entry *next; port_word depth, sequence; } port_slist_header;
static pthread_mutex_t port_slist_lock = PTHREAD_MUTEX_INITIALIZER;

void *PORT_MS_ABI InterlockedPushEntrySList(void *head, void *entry)
{
    port_slist_header *list = (port_slist_header *)head;
    pthread_mutex_lock(&port_slist_lock);
    port_slist_entry *first = list->next;
    ((port_slist_entry *)entry)->next = first;
    list->next = (port_slist_entry *)entry;
    list->depth++;
    list->sequence++;
    pthread_mutex_unlock(&port_slist_lock);
    return first;
}
void *PORT_MS_ABI InterlockedPopEntrySList(void *head)
{
    port_slist_header *list = (port_slist_header *)head;
    pthread_mutex_lock(&port_slist_lock);
    port_slist_entry *first = list->next;
    if (first != 0) {
        list->next = first->next;
        list->depth--;
        list->sequence++;
    }
    pthread_mutex_unlock(&port_slist_lock);
    return first;
}
void *PORT_MS_ABI InterlockedFlushSList(void *head)
{
    port_slist_header *list = (port_slist_header *)head;
    pthread_mutex_lock(&port_slist_lock);
    port_slist_entry *first = list->next;
    list->next = 0;
    list->depth = 0;
    pthread_mutex_unlock(&port_slist_lock);
    return first;
}
#endif

// ---------------------------------------------------------------------------------------------------------------
// Heaps and memory.
//
// The Xbox maps physical memory at fixed virtual windows; the 64 KiB-page window is 0xA0000000 + physical address.
// The game reserves physical memory once with an explicit address (0x05470000, all of it to 0x1FC00000) and then
// uses the window's constant addresses (0xA5470000...), ignoring the pointer returned. So a request with an
// explicit address is mapped at that window address (a 32-bit process on a 64-bit kernel has 4 GiB); one with ~0
// (anywhere) goes wherever the host puts it, aligned as asked.
//
// The same physical pages are also visible read-only through the cached view (d3d9gpu.h:
// GPU_CONVERT_CPU_TO_CPU_CACHED_READONLY_ADDRESS, GPU_CPU_CACHED_READONLY_VIRTUAL_ADDRESS + GPU address, which for
// the 64 KiB window is 0xC0000000 + physical address); CPU texture reads (bitmap sampling) go through it. An
// explicit-address block is therefore one shared memory object mapped twice: read-write at the window and
// read-only at the cached view.

#define PORT_XBOX_PHYSICAL_WINDOW 0xa0000000ul
#define PORT_XBOX_CACHED_READONLY_WINDOW 0xc0000000ul
#define PORT_PHYSICAL_CAPACITY 1024

typedef struct port_physical { void *address; port_size size; void *alias; } port_physical;
static port_physical port_physical_blocks[PORT_PHYSICAL_CAPACITY];
static pthread_mutex_t port_physical_lock = PTHREAD_MUTEX_INITIALIZER;

void *PORT_MS_ABI GetProcessHeap(void) { return (void *)0x1000; }
#ifdef __x86_64__
static void *port_mmap_low4g(port_size size, port_size align, int prot, int flags);

// Low-4GB segregated free-list heap for RtlAllocateHeap/HeapAlloc on x86_64 Linux so D3D headers
// (D3DTexture, D3DVertexBuffer, D3DIndexBuffer) and other game heap objects fit in 32-bit TAG_POINTER slots.
#define PORT_HEAP_MAGIC 0x48334850u
#define PORT_HEAP_CLASSES 13 // 32B .. 128KB (2^(5+cls))
typedef struct port_heap_block {
    port_dword magic;
    port_dword cls;       // 0..PORT_HEAP_CLASSES-1 for pooled, 0xffffffffu for direct mmap
    port_size map_size; // for direct mmap; also payload size for pooled
    struct port_heap_block *next;
    port_size align_pad;  // keeps sizeof(port_heap_block) == 32 for 16-byte SIMD alignment
} port_heap_block;

static port_heap_block *port_heap_free[PORT_HEAP_CLASSES];
static unsigned char *port_heap_slab = 0;
static port_size port_heap_slab_rem = 0;
static pthread_mutex_t port_heap_lock = PTHREAD_MUTEX_INITIALIZER;

static int port_heap_class_for(port_size total)
{
    port_size sz = 32;
    for (int i = 0; i < PORT_HEAP_CLASSES; i++, sz <<= 1) {
        if (total <= sz)
            return i;
    }
    return -1;
}

void *PORT_MS_ABI RtlAllocateHeap(void *heap, port_dword flags, port_size bytes)
{
    (void)heap;
    if (bytes == 0)
        bytes = 1;
    port_size total = sizeof(port_heap_block) + bytes;
    int cls = port_heap_class_for(total);
    if (cls < 0) {
        port_size map_sz = (total + 0xfffful) & ~0xfffful;
        void *p = port_mmap_low4g(map_sz, 0x10000ul, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS);
        if (p == MAP_FAILED)
            return 0;
        port_heap_block *b = (port_heap_block *)p;
        b->magic = PORT_HEAP_MAGIC;
        b->cls = 0xffffffffu;
        b->map_size = map_sz;
        b->next = 0;
        return (void *)(b + 1);
    }
    port_size block_sz = (port_size)32u << cls;
    pthread_mutex_lock(&port_heap_lock);
    port_heap_block *b = port_heap_free[cls];
    if (b) {
        port_heap_free[cls] = b->next;
    } else {
        if (port_heap_slab_rem < block_sz) {
            const port_size slab_sz = 2u * 1024u * 1024u;
            void *p = port_mmap_low4g(slab_sz, 0x10000ul, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS);
            if (p == MAP_FAILED) {
                pthread_mutex_unlock(&port_heap_lock);
                return 0;
            }
            port_heap_slab = (unsigned char *)p;
            port_heap_slab_rem = slab_sz;
        }
        b = (port_heap_block *)port_heap_slab;
        port_heap_slab += block_sz;
        port_heap_slab_rem -= block_sz;
    }
    pthread_mutex_unlock(&port_heap_lock);
    b->magic = PORT_HEAP_MAGIC;
    b->cls = (port_dword)cls;
    b->map_size = block_sz - sizeof(port_heap_block);
    b->next = 0;
    void *user = (void *)(b + 1);
    if (flags & 0x8 /* HEAP_ZERO_MEMORY */)
        memset(user, 0, bytes);
    return user;
}
int PORT_MS_ABI RtlFreeHeap(void *heap, port_dword flags, void *memory)
{
    (void)heap; (void)flags;
    if (!memory)
        return 1;
    port_heap_block *b = ((port_heap_block *)memory) - 1;
    if (b->magic != PORT_HEAP_MAGIC) {
        free(memory);
        return 1;
    }
    if (b->cls == 0xffffffffu) {
        munmap(b, b->map_size);
        return 1;
    }
    port_dword cls = b->cls;
    pthread_mutex_lock(&port_heap_lock);
    b->next = port_heap_free[cls];
    port_heap_free[cls] = b;
    pthread_mutex_unlock(&port_heap_lock);
    return 1;
}
#else
void *PORT_MS_ABI RtlAllocateHeap(void *heap, port_dword flags, port_size bytes)
{
    (void)heap;
    return flags & 0x8 /* HEAP_ZERO_MEMORY */ ? calloc(1, bytes) : malloc(bytes);
}
int PORT_MS_ABI RtlFreeHeap(void *heap, port_dword flags, void *memory)
{
    (void)heap; (void)flags;
    free(memory);
    return 1;
}
#endif

static void port_physical_record(void *address, port_size size, void *alias)
{
    pthread_mutex_lock(&port_physical_lock);
    for (int i = 0; i < PORT_PHYSICAL_CAPACITY; i++) {
        if (port_physical_blocks[i].address == 0) {
            port_physical_blocks[i].address = address;
            port_physical_blocks[i].size = size;
            port_physical_blocks[i].alias = alias;
            break;
        }
    }
    pthread_mutex_unlock(&port_physical_lock);
}

static int port_physical_release(void *address)
{
    int found = 0;
    pthread_mutex_lock(&port_physical_lock);
    for (int i = 0; i < PORT_PHYSICAL_CAPACITY; i++) {
        if (port_physical_blocks[i].address == address) {
            munmap(address, port_physical_blocks[i].size);
            if (port_physical_blocks[i].alias)
                munmap(port_physical_blocks[i].alias, port_physical_blocks[i].size);
            port_physical_blocks[i].address = 0;
            port_physical_blocks[i].alias = 0;
            found = 1;
            break;
        }
    }
    pthread_mutex_unlock(&port_physical_lock);
    return found;
}

#ifdef __x86_64__
// Keep game-visible virtual/physical allocations below 4 GB so 32-bit s_tag_pointer<T> slots and any legacy
// 32-bit pointer truncations stay valid while Vulkan/X11/audio/host allocations freely use full 64-bit space.
// Skips [0xA0000000, 0xE0000000) which is reserved for the Xbox physical window and its cached read-only alias.
static port_size port_low4g_cursor = 0x20000000ul;
static pthread_mutex_t port_low4g_lock = PTHREAD_MUTEX_INITIALIZER;

static void *port_mmap_low4g(port_size size, port_size align, int prot, int flags)
{
    if (align < 0x10000ul)
        align = 0x10000ul;
    const port_size lo = 0x20000000ul;
    const port_size hole_lo = PORT_XBOX_PHYSICAL_WINDOW;
    const port_size hole_hi = 0xe0000000ul;
    const port_size hi = 0xf8000000ul;
    pthread_mutex_lock(&port_low4g_lock);
    port_size start = (port_low4g_cursor + align - 1) & ~(align - 1);
    if (start < lo || start + size > hi)
        start = lo;
    port_size cand = start;
    for (int pass = 0; pass < 2; pass++) {
        while (cand + size <= hi) {
            if (cand < hole_hi && cand + size > hole_lo) {
                cand = (hole_hi + align - 1) & ~(align - 1);
                continue;
            }
            void *p = mmap((void *)cand, size, prot, flags | MAP_FIXED_NOREPLACE, -1, 0);
            if (p != MAP_FAILED) {
                port_low4g_cursor = (cand + size + 0xfffful) & ~0xfffful;
                pthread_mutex_unlock(&port_low4g_lock);
                return p;
            }
            if (errno != EEXIST) {
                pthread_mutex_unlock(&port_low4g_lock);
                return MAP_FAILED;
            }
            cand += align;
        }
        cand = lo;
    }
    pthread_mutex_unlock(&port_low4g_lock);
    return MAP_FAILED;
}
#endif

void *PORT_MS_ABI XPhysicalAlloc(port_size size, port_size address, port_size alignment, port_dword protect)
{
    size = (size + 4095) & ~(port_size)4095;
    if (address != ~(port_size)0 && (port_dword)address != ~0u) {
        void *where = (void *)(PORT_XBOX_PHYSICAL_WINDOW + (port_dword)address);
        void *readonly_where = (void *)(PORT_XBOX_CACHED_READONLY_WINDOW + (port_dword)address);
        int pages = memfd_create("xbox physical memory", MFD_CLOEXEC);
        if (pages < 0 || ftruncate(pages, (off_t)size) != 0) {
            port_write_error("port: cannot create Xbox physical memory\n");
            port_exit(7);
        }
        void *memory = mmap(where, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED_NOREPLACE, pages, 0);
        void *readonly = memory == where
            ? mmap(readonly_where, size, PROT_READ, MAP_SHARED | MAP_FIXED_NOREPLACE, pages, 0) : MAP_FAILED;
        close(pages);
        if (memory != where || readonly != readonly_where) {
            port_write_error("port: cannot map Xbox physical memory at its window addresses\n");
            port_exit(7);
        }
        port_physical_record(memory, size, readonly);
        port_physical_allocated(memory, size, protect);
        return memory;
    }
    port_size align = alignment > 4096 ? alignment : 4096;
#ifdef __x86_64__
    unsigned char *aligned = (unsigned char *)port_mmap_low4g(size, align, PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS);
    if (aligned == MAP_FAILED)
        return 0;
#else
    unsigned char *block = mmap(0, size + align, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (block == MAP_FAILED)
        return 0;
    unsigned char *aligned = (unsigned char *)(((port_size)block + align - 1) & ~(align - 1));
    if (aligned > block)
        munmap(block, (port_size)(aligned - block));
    port_size tail = (port_size)(block + size + align - (aligned + size));
    if (tail > 0)
        munmap(aligned + size, tail);
#endif
    port_physical_record(aligned, size, 0);
    port_physical_allocated(aligned, size, protect);
    return aligned;
}
void PORT_MS_ABI XPhysicalFree(void *address)
{
    port_physical_released(address);
    port_physical_release(address);
}
port_dword PORT_MS_ABI XMemGetPageSize(const void *address)
{
    port_size where = (port_size)address;
    return where >= PORT_XBOX_PHYSICAL_WINDOW && where < 0xc0000000ul ? 0x10000 : 4096;
}
void *PORT_MS_ABI XMemSet(void *destination, int value, port_size size) { return memset(destination, value, size); }
void *PORT_MS_ABI XMemCpy(void *destination, const void *source, port_size size) { return memcpy(destination, source, size); }
// Virtual memory: VirtualAlloc reserves (MEM_RESERVE: address space, no access) and commits (MEM_COMMIT: pages with
// the requested access) as Win32 does; VirtualFree decommits (MEM_DECOMMIT: the pages lose their contents and
// access, the range stays reserved) or releases a whole reservation (MEM_RELEASE, size 0). Reservations are
// recorded so a release unmaps exactly what was reserved. The game also hands VirtualFree the memory XPhysicalAlloc
// gave it (restricted_memory.cpp), which is released as XPhysicalFree releases it.
#define PORT_MEM_COMMIT 0x1000ul
#define PORT_MEM_RESERVE 0x2000ul
#define PORT_MEM_DECOMMIT 0x4000ul
#define PORT_MEM_RELEASE 0x8000ul
#define PORT_VIRTUAL_CAPACITY 4096

static port_physical port_virtual_blocks[PORT_VIRTUAL_CAPACITY];

static int port_virtual_protection(port_dword protect)
{
    switch (protect & 0xff) {
    case 0x01: return PROT_NONE;                                        // PAGE_NOACCESS
    case 0x02: case 0x10: case 0x20: return PROT_READ;                  // READONLY, EXECUTE, EXECUTE_READ
    default: return PROT_READ | PROT_WRITE;                             // READWRITE, EXECUTE_READWRITE, ...
    }
}

// Under port_physical_lock: the reservation holding [start, start + size), or 0.
static port_physical *port_virtual_find(const unsigned char *start, port_size size)
{
    for (int i = 0; i < PORT_VIRTUAL_CAPACITY; i++) {
        unsigned char *base = (unsigned char *)port_virtual_blocks[i].address;
        if (base != 0 && start >= base && start + size <= base + port_virtual_blocks[i].size)
            return &port_virtual_blocks[i];
    }
    return 0;
}

void *PORT_MS_ABI VirtualAlloc(void *address, port_size size, port_dword type, port_dword protect)
{
    if (size == 0 || (type & (PORT_MEM_COMMIT | PORT_MEM_RESERVE)) == 0) {
        port_last_error = PORT_ERROR_INVALID_PARAMETER;
        return 0;
    }
    const int prot = port_virtual_protection(protect);
    if ((type & PORT_MEM_RESERVE) == 0) {
        // commit inside an existing reservation: whole pages covering [address, address + size)
        unsigned char *start = (unsigned char *)((port_size)address & ~4095ul);
        port_size length = (((port_size)address + size + 4095) & ~4095ul) - (port_size)start;
        pthread_mutex_lock(&port_physical_lock);
        port_physical *block = address != 0 ? port_virtual_find(start, length) : 0;
        pthread_mutex_unlock(&port_physical_lock);
        if (block == 0) {
            port_last_error = PORT_ERROR_INVALID_PARAMETER;
            return 0;
        }
        return mprotect(start, length, prot) == 0 ? start : (port_fail_errno(), (void *)0);
    }
    // a new reservation, at a 64 KB boundary when the caller names an address (Win32's allocation granularity)
    port_size length = (size + 4095) & ~4095ul;
    void *where = address != 0 ? (void *)((port_size)address & ~0xfffful) : 0;
    if (where != 0)
        length = (((port_size)address + size + 4095) & ~4095ul) - (port_size)where;
    int map_prot = (type & PORT_MEM_COMMIT) != 0 ? prot : PROT_NONE;
#ifdef __x86_64__
    void *memory = where != 0
        ? mmap(where, length, map_prot, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE | MAP_FIXED_NOREPLACE, -1, 0)
        : port_mmap_low4g(length, 0x10000ul, map_prot, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE);
#else
    void *memory = mmap(where, length, map_prot,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE | (where != 0 ? MAP_FIXED_NOREPLACE : 0), -1, 0);
#endif
    if (memory == MAP_FAILED || (where != 0 && memory != where)) {
        if (memory != MAP_FAILED)
            munmap(memory, length);
        port_last_error = PORT_ERROR_NOT_ENOUGH_MEMORY;
        return 0;
    }
    pthread_mutex_lock(&port_physical_lock);
    port_physical *slot = 0;
    for (int i = 0; i < PORT_VIRTUAL_CAPACITY && slot == 0; i++)
        if (port_virtual_blocks[i].address == 0)
            slot = &port_virtual_blocks[i];
    if (slot != 0)
        *slot = (port_physical){memory, length, 0};
    pthread_mutex_unlock(&port_physical_lock);
    if (slot == 0) {
        munmap(memory, length);
        port_last_error = PORT_ERROR_NOT_ENOUGH_MEMORY;
        return 0;
    }
    return memory;
}

int PORT_MS_ABI VirtualFree(void *address, port_size size, port_dword type)
{
    unsigned char *start = (unsigned char *)address;
    if (type & PORT_MEM_RELEASE) {
        pthread_mutex_lock(&port_physical_lock);
        port_physical *block = port_virtual_find(start, 1);
        port_physical released = {0};
        if (block != 0 && block->address == address) {
            released = *block;
            block->address = 0;
        }
        pthread_mutex_unlock(&port_physical_lock);
        if (released.address != 0)
            return munmap(released.address, released.size) == 0 ? 1 : port_fail_errno();
        if (port_physical_release(address)) {
            port_physical_released(address);
            return 1;
        }
        port_last_error = PORT_ERROR_INVALID_PARAMETER;
        return 0;
    }
    if (type & PORT_MEM_DECOMMIT) {
        pthread_mutex_lock(&port_physical_lock);
        port_physical *block = port_virtual_find(start, size != 0 ? size : 1);
        unsigned char *first = start, *end = start + size;
        if (block != 0 && size == 0) { // size 0: the whole reservation from address on
            end = (unsigned char *)block->address + block->size;
        }
        pthread_mutex_unlock(&port_physical_lock);
        if (block == 0) {
            port_last_error = PORT_ERROR_INVALID_PARAMETER;
            return 0;
        }
        first = (unsigned char *)((port_size)first & ~4095ul);
        end = (unsigned char *)(((port_size)end + 4095) & ~4095ul);
        // a fresh no-access mapping over the pages drops their contents and keeps the range reserved
        void *again = mmap(first, (port_size)(end - first), PROT_NONE,
            MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE | MAP_FIXED, -1, 0);
        return again == first ? 1 : port_fail_errno();
    }
    port_last_error = PORT_ERROR_INVALID_PARAMETER;
    return 0;
}

void PORT_MS_ABI GlobalMemoryStatus(void *status)
{
    // MEMORYSTATUS: dwLength, dwMemoryLoad, then six SIZE_T byte counts.
    typedef struct port_memory_status {
        port_dword dwLength;
        port_dword dwMemoryLoad;
        port_size dwTotalPhys;
        port_size dwAvailPhys;
        port_size dwTotalPageFile;
        port_size dwAvailPageFile;
        port_size dwTotalVirtual;
        port_size dwAvailVirtual;
    } port_memory_status;
    struct sysinfo info;
    sysinfo(&info);
    unsigned long long total = (unsigned long long)info.totalram * info.mem_unit;
    unsigned long long available = (unsigned long long)info.freeram * info.mem_unit;
    port_memory_status *fields = (port_memory_status *)status;
    fields->dwLength = (port_dword)sizeof(port_memory_status);
    fields->dwMemoryLoad = total == 0 ? 0 : (port_dword)(100 - available * 100 / total);
    fields->dwTotalPhys = total > 0x7fffffffull ? 0x7fffffff : (port_size)total;
    fields->dwAvailPhys = available > 0x7fffffffull ? 0x7fffffff : (port_size)available;
    fields->dwTotalPageFile = fields->dwTotalPhys;
    fields->dwAvailPageFile = fields->dwAvailPhys;
    fields->dwTotalVirtual = 0x7fffffff;
    fields->dwAvailVirtual = 0x7fffffff;
}

// ---------------------------------------------------------------------------------------------------------------
// Kernel objects.

typedef enum port_kind {
    PORT_FREE, PORT_FILE, PORT_FIND, PORT_EVENT, PORT_SEMAPHORE, PORT_MUTEX, PORT_THREAD, PORT_COMPLETION,
    PORT_THREAD_ALIAS           // another handle to a thread (an opened thread handle): every call but CloseHandle
                                // acts on the thread
} port_kind;

typedef struct port_packet {
    port_dword bytes;
    port_size key;
    void *overlapped;
    struct port_packet *next;
} port_packet;

typedef struct port_object {
    port_kind kind;
    int closed;                 // a thread's handle closed while it runs: freed when it exits
    int waiters;                // single-object waiters parked on port_object_conds[this - port_objects]
    // events, threads (exited), semaphores, mutexes
    int signaled, manual_reset;
    long count, maximum;
    long owner, recursion;
    // files and find searches
    int fd;
    dev_t file_device;
    ino_t file_inode;
    port_dword file_access, file_share;
    struct port_object *completion;
    port_size key;
    DIR *directory;
    char pattern[256];
    // threads
    pthread_t thread;
    port_dword (PORT_MS_ABI *start)(void *);
    void *parameter;
    port_dword exit_code, thread_id;
    long suspend_count;
    int started;                // past CREATE_SUSPENDED: running its start routine
    volatile int park, parked;  // SuspendThread of a running thread: futex words (xdk_suspend_thread)
    port_dword processor;       // XSetThreadProcessor's choice
    struct port_object *target; // a thread alias: the thread
    long aliases;               // a thread: its open aliases (it is freed when exited, closed and unaliased)
    // completion ports
    port_packet *head, *tail;
} port_object;

#define PORT_HANDLE_CAPACITY 4096
#define PORT_HANDLE_BASE 0x10000ul
static port_object port_objects[PORT_HANDLE_CAPACITY];
static pthread_cond_t port_object_conds[PORT_HANDLE_CAPACITY];
static unsigned char port_object_cond_ready[PORT_HANDLE_CAPACITY];
static pthread_mutex_t port_kernel_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t port_kernel_changed;
static int port_global_waiters;
static pthread_once_t port_kernel_once = PTHREAD_ONCE_INIT;
static __thread port_object *port_current_thread;

static void port_kernel_initialize(void)
{
    pthread_condattr_t attributes;
    pthread_condattr_init(&attributes);
    pthread_condattr_setclock(&attributes, CLOCK_MONOTONIC);
    pthread_cond_init(&port_kernel_changed, &attributes);
    pthread_condattr_destroy(&attributes);
}

static pthread_cond_t *port_object_cond(port_object *object)
{
    port_size idx = (port_size)(object - port_objects);
    if (!port_object_cond_ready[idx]) {
        pthread_condattr_t attributes;
        pthread_condattr_init(&attributes);
        pthread_condattr_setclock(&attributes, CLOCK_MONOTONIC);
        pthread_cond_init(&port_object_conds[idx], &attributes);
        pthread_condattr_destroy(&attributes);
        port_object_cond_ready[idx] = 1;
    }
    return &port_object_conds[idx];
}

static void port_lock(void)
{
    pthread_once(&port_kernel_once, port_kernel_initialize);
    pthread_mutex_lock(&port_kernel_lock);
}
static void port_unlock(void) { pthread_mutex_unlock(&port_kernel_lock); }
static void port_changed(void)
{
    if (port_global_waiters > 0)
        pthread_cond_broadcast(&port_kernel_changed);
}
static void port_wake_object(port_object *object, int wake_all)
{
    if (object != 0 && object->waiters > 0) {
        pthread_cond_t *cond = &port_object_conds[object - port_objects];
        if (wake_all)
            pthread_cond_broadcast(cond);
        else
            pthread_cond_signal(cond);
    }
    if (port_global_waiters > 0)
        pthread_cond_broadcast(&port_kernel_changed);
}

// Under the lock.
static port_object *port_new_object(port_kind kind, void **handle)
{
    for (int i = 0; i < PORT_HANDLE_CAPACITY; i++) {
        if (port_objects[i].kind == PORT_FREE) {
            memset(&port_objects[i], 0, sizeof(port_objects[i]));
            port_objects[i].kind = kind;
            port_objects[i].fd = -1;
            *handle = (void *)(PORT_HANDLE_BASE + (port_size)i * 4);
            return &port_objects[i];
        }
    }
    port_write_error("port: kernel handle table full\n");
    port_exit(7);
    return 0;
}

static port_object *port_object_of(void *handle)
{
    if (handle == PORT_CURRENT_THREAD)
        return port_current_thread;
    port_size value = (port_size)handle;
    if (value < PORT_HANDLE_BASE || (value - PORT_HANDLE_BASE) % 4 != 0)
        return 0;
    port_size index = (value - PORT_HANDLE_BASE) / 4;
    if (index >= PORT_HANDLE_CAPACITY || port_objects[index].kind == PORT_FREE)
        return 0;
    if (port_objects[index].kind == PORT_THREAD_ALIAS)
        return port_objects[index].target;
    return &port_objects[index];
}

// The object a handle names itself (a thread alias, not its thread). Under the lock.
static port_object *port_handle_object(void *handle)
{
    port_size value = (port_size)handle;
    if (value < PORT_HANDLE_BASE || (value - PORT_HANDLE_BASE) % 4 != 0)
        return 0;
    port_size index = (value - PORT_HANDLE_BASE) / 4;
    if (index >= PORT_HANDLE_CAPACITY || port_objects[index].kind == PORT_FREE)
        return 0;
    return &port_objects[index];
}

static long port_thread_token(void)
{
    static __thread long cached_tid;
    if (!cached_tid)
        cached_tid = (long)syscall(SYS_gettid);
    return cached_tid;
}

// Wait conditions, under the lock.
static int port_ready(port_object *object)
{
    switch (object->kind) {
    case PORT_EVENT: case PORT_THREAD: return object->signaled;
    case PORT_SEMAPHORE: return object->count > 0;
    case PORT_MUTEX: return object->owner == 0 || object->owner == port_thread_token();
    case PORT_FILE: return 1;
    default: return 0;
    }
}

static void port_acquire(port_object *object)
{
    switch (object->kind) {
    case PORT_EVENT:
        if (!object->manual_reset)
            object->signaled = 0;
        break;
    case PORT_SEMAPHORE:
        object->count--;
        break;
    case PORT_MUTEX:
        object->owner = port_thread_token();
        object->recursion++;
        break;
    default:
        break;
    }
}

static struct timespec port_deadline(port_dword milliseconds)
{
    struct timespec when;
    clock_gettime(CLOCK_MONOTONIC, &when);
    when.tv_sec += milliseconds / 1000;
    when.tv_nsec += (long)(milliseconds % 1000) * 1000000;
    if (when.tv_nsec >= 1000000000) {
        when.tv_sec++;
        when.tv_nsec -= 1000000000;
    }
    return when;
}

// Under the lock: 0 when the deadline passed.
static int port_wait_changed(port_dword milliseconds, const struct timespec *deadline)
{
    if (milliseconds == 0)
        return 0;
    port_global_waiters++;
    int res;
    if (milliseconds == PORT_INFINITE) {
        pthread_cond_wait(&port_kernel_changed, &port_kernel_lock);
        res = 1;
    } else {
        res = pthread_cond_timedwait(&port_kernel_changed, &port_kernel_lock, deadline) != ETIMEDOUT;
    }
    port_global_waiters--;
    return res;
}

// Under the lock: single-object wait on the object's own condvar.
static int port_wait_object(port_object *object, port_dword milliseconds, const struct timespec *deadline)
{
    if (milliseconds == 0)
        return 0;
    pthread_cond_t *cond = port_object_cond(object);
    object->waiters++;
    int res;
    if (milliseconds == PORT_INFINITE) {
        pthread_cond_wait(cond, &port_kernel_lock);
        res = 1;
    } else {
        res = pthread_cond_timedwait(cond, &port_kernel_lock, deadline) != ETIMEDOUT;
    }
    object->waiters--;
    return res;
}

port_dword PORT_MS_ABI WaitForMultipleObjects(port_dword count, void *const *handles, int wait_all, port_dword milliseconds)
{
    port_object *objects[64];
    if (count == 0 || count > 64) {
        port_last_error = PORT_ERROR_INVALID_PARAMETER;
        return PORT_WAIT_FAILED;
    }
    port_lock();
    for (port_dword i = 0; i < count; i++) {
        objects[i] = port_object_of(handles[i]);
        if (objects[i] == 0) {
            port_unlock();
            port_last_error = PORT_ERROR_INVALID_HANDLE;
            return PORT_WAIT_FAILED;
        }
    }
    struct timespec deadline;
    int have_deadline = 0;
    for (;;) {
        if (wait_all) {
            port_dword ready = 0;
            while (ready < count && port_ready(objects[ready]))
                ready++;
            if (ready == count) {
                for (port_dword i = 0; i < count; i++)
                    port_acquire(objects[i]);
                port_unlock();
                return PORT_WAIT_OBJECT_0;
            }
        } else {
            for (port_dword i = 0; i < count; i++) {
                if (port_ready(objects[i])) {
                    port_acquire(objects[i]);
                    port_unlock();
                    return PORT_WAIT_OBJECT_0 + i;
                }
            }
        }
        if (milliseconds == 0) {
            port_unlock();
            return PORT_WAIT_TIMEOUT;
        }
        if (milliseconds != PORT_INFINITE && !have_deadline) {
            deadline = port_deadline(milliseconds);
            have_deadline = 1;
        }
        int ok = count == 1 ? port_wait_object(objects[0], milliseconds, &deadline)
                            : port_wait_changed(milliseconds, &deadline);
        if (!ok) {
            port_unlock();
            return PORT_WAIT_TIMEOUT;
        }
    }
}

port_dword PORT_MS_ABI WaitForSingleObject(void *handle, port_dword milliseconds)
{
    port_lock();
    port_object *object = port_object_of(handle);
    if (object == 0) {
        port_unlock();
        port_last_error = PORT_ERROR_INVALID_HANDLE;
        return PORT_WAIT_FAILED;
    }
    if (port_ready(object)) {
        port_acquire(object);
        port_unlock();
        return PORT_WAIT_OBJECT_0;
    }
    if (milliseconds == 0) {
        port_unlock();
        return PORT_WAIT_TIMEOUT;
    }
    struct timespec deadline;
    if (milliseconds != PORT_INFINITE)
        deadline = port_deadline(milliseconds);
    for (;;) {
        if (!port_wait_object(object, milliseconds, &deadline)) {
            port_unlock();
            return PORT_WAIT_TIMEOUT;
        }
        if (port_ready(object)) {
            port_acquire(object);
            port_unlock();
            return PORT_WAIT_OBJECT_0;
        }
    }
}

port_dword PORT_MS_ABI WaitForSingleObjectEx(void *handle, port_dword milliseconds, int alertable)
{
    (void)alertable;
    return WaitForSingleObject(handle, milliseconds);
}

void *PORT_MS_ABI CreateEventA(void *security, int manual_reset, int initial_state, const char *name)
{
    (void)security; (void)name;
    void *handle;
    port_lock();
    port_object *event = port_new_object(PORT_EVENT, &handle);
    event->manual_reset = manual_reset != 0;
    event->signaled = initial_state != 0;
    port_unlock();
    return handle;
}

static int port_set_event(void *handle, int signaled)
{
    port_lock();
    port_object *event = port_object_of(handle);
    if (event == 0 || event->kind != PORT_EVENT) {
        port_unlock();
        port_last_error = PORT_ERROR_INVALID_HANDLE;
        return 0;
    }
    event->signaled = signaled;
    if (signaled)
        port_wake_object(event, event->manual_reset);
    port_unlock();
    return 1;
}
int PORT_MS_ABI SetEvent(void *handle) { return port_set_event(handle, 1); }
int PORT_MS_ABI ResetEvent(void *handle) { return port_set_event(handle, 0); }

void *PORT_MS_ABI CreateSemaphoreA(void *security, long initial_count, long maximum_count, const char *name)
{
    (void)security; (void)name;
    void *handle;
    port_lock();
    port_object *semaphore = port_new_object(PORT_SEMAPHORE, &handle);
    semaphore->count = initial_count;
    semaphore->maximum = maximum_count;
    port_unlock();
    return handle;
}

int PORT_MS_ABI ReleaseSemaphore(void *handle, int release_count, int *previous_count)
{
    port_lock();
    port_object *semaphore = port_object_of(handle);
    if (semaphore == 0 || semaphore->kind != PORT_SEMAPHORE) {
        port_unlock();
        port_last_error = PORT_ERROR_INVALID_HANDLE;
        return 0;
    }
    if (release_count <= 0 || semaphore->count + release_count > semaphore->maximum) {
        port_unlock();
        port_last_error = PORT_ERROR_TOO_MANY_POSTS;
        return 0;
    }
    if (previous_count != 0)
        *previous_count = semaphore->count;
    semaphore->count += release_count;
    port_wake_object(semaphore, release_count > 1);
    port_unlock();
    return 1;
}

void *PORT_MS_ABI CreateMutexA(void *security, int initial_owner, const char *name)
{
    (void)security; (void)name;
    void *handle;
    port_lock();
    port_object *mutex = port_new_object(PORT_MUTEX, &handle);
    if (initial_owner) {
        mutex->owner = port_thread_token();
        mutex->recursion = 1;
    }
    port_unlock();
    return handle;
}

int PORT_MS_ABI ReleaseMutex(void *handle)
{
    port_lock();
    port_object *mutex = port_object_of(handle);
    if (mutex == 0 || mutex->kind != PORT_MUTEX || mutex->owner != port_thread_token()) {
        port_unlock();
        port_last_error = mutex == 0 ? PORT_ERROR_INVALID_HANDLE : PORT_ERROR_NOT_OWNER;
        return 0;
    }
    if (--mutex->recursion == 0) {
        mutex->owner = 0;
        port_wake_object(mutex, 0);
    }
    port_unlock();
    return 1;
}

// ---------------------------------------------------------------------------------------------------------------
// Threads. CREATE_SUSPENDED (4) holds the new thread before its start routine until ResumeThread.

static port_dword port_next_thread_id = 1;

static void *port_thread_main(void *parameter)
{
    port_object *thread = (port_object *)parameter;
    port_current_thread = thread;
    port_lock();
    while (thread->suspend_count > 0) {
        port_global_waiters++;
        pthread_cond_wait(&port_kernel_changed, &port_kernel_lock);
        port_global_waiters--;
    }
    thread->started = 1;
    port_unlock();
    port_dword code = thread->start(thread->parameter);
    port_lock();
    thread->exit_code = code;
    thread->signaled = 1;
    port_wake_object(thread, 1);
    if (thread->closed && thread->aliases == 0)
        thread->kind = PORT_FREE;
    port_unlock();
    return 0;
}

void *PORT_MS_ABI CreateThread(void *security, port_size stack_size, port_dword (PORT_MS_ABI *start)(void *), void *parameter,
    port_dword flags, port_dword *thread_id)
{
    (void)security;
    void *handle;
    port_lock();
    port_object *thread = port_new_object(PORT_THREAD, &handle);
    thread->start = start;
    thread->parameter = parameter;
    thread->suspend_count = flags & 4 ? 1 : 0;
    thread->thread_id = port_next_thread_id++;
    port_unlock();
    // x86 frames differ from PowerPC ones; give every thread at least 1 MiB.
    pthread_attr_t attributes;
    pthread_attr_init(&attributes);
    pthread_attr_setstacksize(&attributes, stack_size > 0x100000 ? stack_size : 0x100000);
    pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
    int failed = pthread_create(&thread->thread, &attributes, port_thread_main, thread);
    pthread_attr_destroy(&attributes);
    if (failed) {
        port_lock();
        thread->kind = PORT_FREE;
        port_unlock();
        port_last_error = PORT_ERROR_NOT_ENOUGH_MEMORY;
        return 0;
    }
    if (thread_id != 0)
        *thread_id = thread->thread_id;
    return handle;
}

// SuspendThread (the game's xdk_suspend_thread). A thread still held by CREATE_SUSPENDED only counts up. A running
// thread is stopped where it is: PORT_SIGNAL_SUSPEND interrupts it, and its handler parks it on a futex until
// ResumeThread brings the count back to 0; SuspendThread returns once the thread is parked, as Win32's returns with
// the thread stopped. Like Win32's, it stops the thread whatever it holds (a lock the suspender then takes waits
// for the resume): the game suspends its sound worker threads only around minidumps and system pauses.
#define PORT_SIGNAL_SUSPEND (SIGRTMIN + 2)

static void port_futex_wait(volatile int *word, int value)
{
    syscall(SYS_futex, word, FUTEX_WAIT_PRIVATE, value, 0, 0, 0);
}
static void port_futex_wake(volatile int *word)
{
    syscall(SYS_futex, word, FUTEX_WAKE_PRIVATE, 0x7fffffff, 0, 0, 0);
}

// Async-signal-safe: futex words and errno only.
static void port_suspend_handler(int signal_number, siginfo_t *info, void *context)
{
    (void)signal_number; (void)context;
    port_object *thread = (port_object *)info->si_value.sival_ptr;
    int saved_errno = errno;
    __atomic_add_fetch(&thread->parked, 1, __ATOMIC_RELEASE); // counts handler entries: the suspender waits for a new one
    port_futex_wake(&thread->parked);
    while (__atomic_load_n(&thread->park, __ATOMIC_ACQUIRE))
        port_futex_wait(&thread->park, 1);
    errno = saved_errno;
}

static pthread_once_t port_suspend_once = PTHREAD_ONCE_INIT;
static void port_suspend_install(void)
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_sigaction = port_suspend_handler;
    action.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&action.sa_mask);
    sigaction(PORT_SIGNAL_SUSPEND, &action, 0);
}

port_dword PORT_MS_ABI SuspendThread(void *handle)
{
    pthread_once(&port_suspend_once, port_suspend_install);
    port_lock();
    port_object *thread = port_object_of(handle);
    if (thread == 0 || thread->kind != PORT_THREAD) {
        port_unlock();
        port_last_error = PORT_ERROR_INVALID_HANDLE;
        return 0xfffffffful;
    }
    port_dword previous = (port_dword)thread->suspend_count;
    thread->suspend_count++;
    if (previous != 0 || !thread->started || thread->signaled) {
        port_unlock();
        return previous;
    }
    __atomic_store_n(&thread->park, 1, __ATOMIC_RELEASE);
    if (thread == port_current_thread) {
        port_unlock();
        while (__atomic_load_n(&thread->park, __ATOMIC_ACQUIRE))
            port_futex_wait(&thread->park, 1);
        return previous;
    }
    // Signalled under the lock: a thread that has not exited by now cannot exit before the signal is queued
    // (exiting takes the lock), so its pthread_t is still valid.
    const int entries = __atomic_load_n(&thread->parked, __ATOMIC_ACQUIRE);
    union sigval value;
    value.sival_ptr = thread;
    int failed = pthread_sigqueue(thread->thread, PORT_SIGNAL_SUSPEND, value);
    port_unlock();
    if (failed) {
        port_lock();
        thread->suspend_count--;
        __atomic_store_n(&thread->park, 0, __ATOMIC_RELEASE);
        port_unlock();
        port_last_error = PORT_ERROR_GEN_FAILURE;
        return 0xfffffffful;
    }
    int now;
    while ((now = __atomic_load_n(&thread->parked, __ATOMIC_ACQUIRE)) == entries)
        port_futex_wait(&thread->parked, now);
    return previous;
}

port_dword PORT_MS_ABI xdk_suspend_thread(void *handle) { return SuspendThread(handle); }

port_dword PORT_MS_ABI ResumeThread(void *handle)
{
    port_lock();
    port_object *thread = port_object_of(handle);
    if (thread == 0 || thread->kind != PORT_THREAD) {
        port_unlock();
        port_last_error = PORT_ERROR_INVALID_HANDLE;
        return 0xfffffffful;
    }
    port_dword previous = (port_dword)thread->suspend_count;
    if (thread->suspend_count > 0)
        thread->suspend_count--;
    if (thread->suspend_count == 0 && thread->park) {
        __atomic_store_n(&thread->park, 0, __ATOMIC_RELEASE);
        port_futex_wake(&thread->park);
    }
    port_changed();
    port_unlock();
    return previous;
}

int PORT_MS_ABI GetExitCodeThread(void *handle, port_dword *exit_code)
{
    port_lock();
    port_object *thread = port_object_of(handle);
    if (thread == 0 || thread->kind != PORT_THREAD) {
        port_unlock();
        port_last_error = PORT_ERROR_INVALID_HANDLE;
        return 0;
    }
    *exit_code = thread->signaled ? thread->exit_code : PORT_STILL_ACTIVE;
    port_unlock();
    return 1;
}

port_dword PORT_MS_ABI GetCurrentThreadId(void)
{
    return port_current_thread != 0 ? port_current_thread->thread_id : 0x10000ul + (port_dword)port_thread_token();
}

int PORT_MS_ABI SetThreadPriority(void *thread, int priority) { (void)thread; (void)priority; return 1; }

// The Xbox runs each thread on the hardware thread the game names (0 to 5). The host scheduler places threads
// itself, so the choice is recorded and the previous one returned; no thread is pinned.
static __thread port_dword port_unlisted_processor; // the calling thread, when this kernel did not create it (main)
port_dword PORT_MS_ABI XSetThreadProcessor(void *handle, port_dword processor)
{
    port_dword previous;
    port_lock();
    port_object *thread = port_object_of(handle);
    if (thread != 0 && thread->kind == PORT_THREAD) {
        previous = thread->processor;
        thread->processor = processor;
    } else if (handle == PORT_CURRENT_THREAD) {
        previous = port_unlisted_processor;
        port_unlisted_processor = processor;
    } else {
        port_unlock();
        port_last_error = PORT_ERROR_INVALID_HANDLE;
        return 0xfffffffful;
    }
    port_unlock();
    return previous;
}

void PORT_MS_ABI Sleep(port_dword milliseconds)
{
    if (milliseconds == 0) {
        sched_yield();
        return;
    }
    struct timespec duration = {(time_t)(milliseconds / 1000), (long)(milliseconds % 1000) * 1000000};
    while (nanosleep(&duration, &duration) != 0 && errno == EINTR) {
    }
}

int PORT_MS_ABI SwitchToThread(void) { return sched_yield() == 0; }

// ---------------------------------------------------------------------------------------------------------------
// I/O completion ports.

void *PORT_MS_ABI CreateIoCompletionPort(void *file, void *existing_port, port_size key, port_dword threads)
{
    (void)threads;
    void *handle = existing_port;
    port_lock();
    port_object *completion = existing_port != 0 ? port_object_of(existing_port) : 0;
    if (completion == 0)
        completion = port_new_object(PORT_COMPLETION, &handle);
    if (file != PORT_INVALID_HANDLE) {
        port_object *target = port_object_of(file);
        if (target == 0 || target->kind != PORT_FILE) {
            port_unlock();
            port_last_error = PORT_ERROR_INVALID_HANDLE;
            return 0;
        }
        target->completion = completion;
        target->key = key;
    }
    port_unlock();
    return handle;
}

// Under the lock.
static void port_post(port_object *completion, port_dword bytes, port_size key, void *overlapped)
{
    port_packet *packet = (port_packet *)malloc(sizeof(port_packet));
    packet->bytes = bytes;
    packet->key = key;
    packet->overlapped = overlapped;
    packet->next = 0;
    if (completion->tail != 0)
        completion->tail->next = packet;
    else
        completion->head = packet;
    completion->tail = packet;
    port_wake_object(completion, 0);
}

int PORT_MS_ABI GetQueuedCompletionStatus(void *port, port_dword *bytes, port_size *key, void **overlapped, port_dword milliseconds)
{
    port_lock();
    port_object *completion = port_object_of(port);
    if (completion == 0 || completion->kind != PORT_COMPLETION) {
        port_unlock();
        port_last_error = PORT_ERROR_INVALID_HANDLE;
        *overlapped = 0;
        return 0;
    }
    struct timespec deadline;
    int have_deadline = 0;
    while (completion->head == 0) {
        if (milliseconds == 0) {
            port_unlock();
            *overlapped = 0;
            port_last_error = PORT_WAIT_TIMEOUT;
            return 0;
        }
        if (milliseconds != PORT_INFINITE && !have_deadline) {
            deadline = port_deadline(milliseconds);
            have_deadline = 1;
        }
        if (!port_wait_object(completion, milliseconds, &deadline)) {
            port_unlock();
            *overlapped = 0;
            port_last_error = PORT_WAIT_TIMEOUT;
            return 0;
        }
    }
    port_packet *packet = completion->head;
    completion->head = packet->next;
    if (completion->head == 0)
        completion->tail = 0;
    port_unlock();
    *bytes = packet->bytes;
    *key = packet->key;
    *overlapped = packet->overlapped;
    free(packet);
    return 1;
}

// ---------------------------------------------------------------------------------------------------------------
// Files.

enum {
    PORT_FILE_SHARE_READ = 1,
    PORT_FILE_SHARE_WRITE = 2,
    PORT_FILE_SHARE_DELETE = 4,
    PORT_FILE_SHARE_ALL = PORT_FILE_SHARE_READ | PORT_FILE_SHARE_WRITE | PORT_FILE_SHARE_DELETE,
};

// The caller holds the kernel lock through the operation that needs this check.
static int port_file_sharing_conflict(const struct stat *info, port_dword access, port_dword share)
{
    for (int i = 0; i < PORT_HANDLE_CAPACITY; i++) {
        const port_object *file = &port_objects[i];
        if (file->kind == PORT_FILE && file->file_device == info->st_dev && file->file_inode == info->st_ino
            && ((access & ~file->file_share) || (file->file_access & ~share))) {
            port_last_error = PORT_ERROR_SHARING_VIOLATION;
            return 1;
        }
    }
    return 0;
}

static int port_resolve_xbox_path(const char *name, int access, char path[PORT_STORAGE_PATH])
{
    unsigned long error = 0;
    if (!port_storage_path(name, access, path, &error)) {
        port_last_error = error;
        return 0;
    }
    return 1;
}

int PORT_MS_ABI GetVolumeInformationA(const char *root, char *volume_name, port_dword volume_name_size, port_dword *serial_number,
    port_dword *max_component_length, port_dword *file_system_flags, char *file_system_name,
    port_dword file_system_name_size)
{
    unsigned int error = 0;
    if (!port_volume_information(root, volume_name, volume_name_size, serial_number, max_component_length,
            file_system_flags, file_system_name, file_system_name_size, &error)) {
        port_last_error = error;
        return 0;
    }
    port_last_error = 0;
    return 1;
}

void *PORT_MS_ABI CreateFileA(const char *name, port_dword access, port_dword share, void *security, port_dword disposition,
    port_dword flags, void *template_file)
{
    (void)security; (void)flags; (void)template_file;
    int write_access = (access & 0x40000000ul) != 0 || disposition == 1 || disposition == 2 || disposition == 4
        || disposition == 5;
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_xbox_path(name, write_access ? PORT_STORAGE_WRITE : PORT_STORAGE_READ, path))
        return PORT_INVALID_HANDLE;
    int mode = (access & 0x40000000ul) /* GENERIC_WRITE */
        ? ((access & 0x80000000ul) /* GENERIC_READ */ ? O_RDWR : O_WRONLY) : O_RDONLY;
    switch (disposition) {
    case 1: mode |= O_CREAT | O_EXCL; break;    // CREATE_NEW
    case 2: mode |= O_CREAT | O_TRUNC; break;   // CREATE_ALWAYS
    case 4: mode |= O_CREAT; break;             // OPEN_ALWAYS
    case 5: mode |= O_TRUNC; break;             // TRUNCATE_EXISTING
    default: break;                             // OPEN_EXISTING
    }
    port_dword file_access = 0;
    if (access & 0x80000000ul) // GENERIC_READ
        file_access |= PORT_FILE_SHARE_READ;
    if (access & 0x40000000ul) // GENERIC_WRITE
        file_access |= PORT_FILE_SHARE_WRITE;
    if (access & 0x00010000ul) // DELETE
        file_access |= PORT_FILE_SHARE_DELETE;

    port_lock();
    struct stat info;
    int existed = stat(path, &info) == 0;
    // In particular, check before open can truncate an existing file.
    if (existed && disposition != 1 && port_file_sharing_conflict(&info, file_access, share)) {
        port_unlock();
        return PORT_INVALID_HANDLE;
    }
    int fd = open(path, mode | O_CLOEXEC, 0666);
    if (fd < 0) {
        port_last_error = errno == EEXIST ? PORT_ERROR_FILE_EXISTS : port_error_from_errno(errno);
        port_unlock();
        return PORT_INVALID_HANDLE;
    }
    if (fstat(fd, &info) != 0) {
        port_fail_errno();
        close(fd);
        port_unlock();
        return PORT_INVALID_HANDLE;
    }
    void *handle;
    port_object *file = port_new_object(PORT_FILE, &handle);
    file->fd = fd;
    file->file_device = info.st_dev;
    file->file_inode = info.st_ino;
    file->file_access = file_access;
    file->file_share = share;
    port_unlock();
    port_last_error = existed && (disposition == 2 || disposition == 4) ? PORT_ERROR_ALREADY_EXISTS : 0;
    return handle;
}

static port_object *port_file_of(void *handle)
{
    port_lock();
    port_object *file = port_object_of(handle);
    port_unlock();
    if (file == 0 || file->kind != PORT_FILE) {
        port_last_error = PORT_ERROR_INVALID_HANDLE;
        return 0;
    }
    return file;
}

// OVERLAPPED: Internal, InternalHigh, Offset, OffsetHigh, hEvent. The transfer happens now; completion is reported
// the Win32 way: the status and count in the structure, the event set, a packet on the file's completion port.
typedef struct port_overlapped {
    port_size internal, internal_high;
    port_dword offset, offset_high;
    void *event;
} port_overlapped;

// A write of PORT_MAP_SWAP_HEADER_SIZE bytes: a cache-file header in host order is written in disk order (map_swap.h).
static ssize_t port_write_header_size(int fd, const void *buffer, off_t position, int positioned)
{
    unsigned char scratch[PORT_MAP_SWAP_HEADER_SIZE];
    const void *source = port_map_swap_before_write(buffer, PORT_MAP_SWAP_HEADER_SIZE, (unsigned long long)position,
        scratch);
    return positioned ? pwrite(fd, source, PORT_MAP_SWAP_HEADER_SIZE, position)
                      : write(fd, source, PORT_MAP_SWAP_HEADER_SIZE);
}

static int port_transfer(void *handle, void *buffer, port_dword bytes, port_dword *done, void *overlapped, int write_to)
{
    port_object *file = port_file_of(handle);
    if (file == 0)
        return 0;
    ssize_t count;
    port_overlapped *request = (port_overlapped *)overlapped;
    // the file offset (only asked of the host for header-sized transfers, the only ones map_swap.h looks at)
    off_t position = request != 0 ? (off_t)(((unsigned long long)request->offset_high << 32) | request->offset)
        : bytes == PORT_MAP_SWAP_HEADER_SIZE ? lseek(file->fd, 0, SEEK_CUR) : -1;
    if (write_to && bytes == PORT_MAP_SWAP_HEADER_SIZE)
        count = port_write_header_size(file->fd, buffer, position, request != 0);
    else if (request != 0)
        count = write_to ? pwrite(file->fd, buffer, bytes, position) : pread(file->fd, buffer, bytes, position);
    else
        count = write_to ? write(file->fd, buffer, bytes) : read(file->fd, buffer, bytes);
    // cache-file headers cross the SDK in disk order (map_swap.h)
    if (!write_to && count > 0 && position >= 0)
        port_map_swap_after_read(buffer, bytes, (port_dword)count, (unsigned long long)position);
    if (count < 0) {
        if (done != 0)
            *done = 0;
        return port_fail_errno();
    }
    if (done != 0)
        *done = (port_dword)count;
    if (request == 0)
        return 1;
    int at_end = !write_to && count == 0 && bytes > 0;
    request->internal = at_end ? 0xc0000011ul /* STATUS_END_OF_FILE */ : 0;
    request->internal_high = (port_size)count;
    if (request->event != 0)
        SetEvent(request->event);
    port_lock();
    if (file->completion != 0 && !at_end)
        port_post(file->completion, (port_dword)count, file->key, overlapped);
    port_unlock();
    if (at_end) {
        port_last_error = PORT_ERROR_HANDLE_EOF;
        return 0;
    }
    return 1;
}

int PORT_MS_ABI ReadFile(void *file, void *buffer, port_dword bytes, port_dword *read_count, void *overlapped)
{
    return port_transfer(file, buffer, bytes, read_count, overlapped, 0);
}

int PORT_MS_ABI WriteFile(void *file, const void *buffer, port_dword bytes, port_dword *written, void *overlapped)
{
    return port_transfer(file, (void *)buffer, bytes, written, overlapped, 1);
}

// xapi_io.h: the transfer of ReadFileEx / WriteFileEx (xapi_io.c), which reports its completion itself (no event,
// no completion port packet: those belong to ReadFile / WriteFile with an OVERLAPPED).
port_dword port_file_transfer_at(void *handle, void *buffer, port_dword bytes, unsigned long long offset, int write,
    port_dword *done)
{
    *done = 0;
    port_object *file = port_file_of(handle);
    if (file == 0)
        return PORT_ERROR_INVALID_HANDLE;
    ssize_t count;
    if (write && bytes == PORT_MAP_SWAP_HEADER_SIZE)
        count = port_write_header_size(file->fd, buffer, (off_t)offset, 1);
    else
        count = write ? pwrite(file->fd, buffer, bytes, (off_t)offset) : pread(file->fd, buffer, bytes, (off_t)offset);
    if (count < 0) {
        port_fail_errno();
        return port_last_error;
    }
    if (!write && count == 0 && bytes > 0)
        return PORT_ERROR_HANDLE_EOF;
    if (!write)
        port_map_swap_after_read(buffer, bytes, (port_dword)count, offset);
    *done = (port_dword)count;
    return 0;
}

port_dword PORT_MS_ABI GetFileSize(void *handle, port_dword *size_high)
{
    port_object *file = port_file_of(handle);
    struct stat info;
    if (file == 0 || fstat(file->fd, &info) != 0) {
        if (file != 0)
            port_fail_errno();
        return 0xfffffffful;
    }
    if (size_high != 0)
        *size_high = (port_dword)((unsigned long long)info.st_size >> 32);
    port_last_error = 0;
    return (port_dword)info.st_size;
}

port_dword PORT_MS_ABI SetFilePointer(void *handle, int distance, int *distance_high, port_dword method)
{
    port_object *file = port_file_of(handle);
    if (file == 0)
        return 0xffffffffu;
    long long move = distance_high != 0 ? (long long)(((unsigned long long)(port_dword)*distance_high << 32)
        | (port_dword)distance) : (long long)distance;
    off_t position = lseek(file->fd, (off_t)move, method == 2 ? SEEK_END : method == 1 ? SEEK_CUR : SEEK_SET);
    if (position < 0) {
        port_fail_errno();
        return 0xffffffffu;
    }
    if (distance_high != 0)
        *distance_high = (int)((unsigned long long)position >> 32);
    port_last_error = 0;
    return (port_dword)position;
}

int PORT_MS_ABI SetEndOfFile(void *handle)
{
    port_object *file = port_file_of(handle);
    if (file == 0)
        return 0;
    off_t position = lseek(file->fd, 0, SEEK_CUR);
    return position >= 0 && ftruncate(file->fd, position) == 0 ? 1 : port_fail_errno();
}

int PORT_MS_ABI SetFileInformationByHandle(void *handle, int info_class, void *info, port_dword bytes)
{
    port_object *file = port_file_of(handle);
    if (file == 0)
        return 0;
    // 5 = FileAllocationInfo, 6 = FileEndOfFileInfo (8-byte signed offset).
    if ((info_class == 5 || info_class == 6) && info != 0 && bytes >= 8) {
        long long size = 0;
        memcpy(&size, info, sizeof(size));
        if (size < 0) {
            port_last_error = PORT_ERROR_INVALID_PARAMETER;
            return 0;
        }
        if (info_class == 6)
            return ftruncate(file->fd, (off_t)size) == 0 ? 1 : port_fail_errno();
        // FileAllocationInfo: preallocate or extend without truncating existing contents if larger.
        struct stat st;
        if (fstat(file->fd, &st) == 0 && (off_t)size > st.st_size)
            return ftruncate(file->fd, (off_t)size) == 0 ? 1 : port_fail_errno();
        return 1;
    }
    port_last_error = PORT_ERROR_INVALID_PARAMETER;
    return 0;
}

int PORT_MS_ABI FlushFileBuffers(void *handle)
{
    port_object *file = port_file_of(handle);
    return file != 0 && fsync(file->fd) == 0 ? 1 : (file != 0 ? port_fail_errno() : 0);
}

// Attributes: FILE_ATTRIBUTE_READONLY 0x1, DIRECTORY 0x10, NORMAL 0x80. FILETIME: 100 ns units since 1601.
static port_dword port_attributes(const struct stat *info)
{
    port_dword attributes = S_ISDIR(info->st_mode) ? 0x10 : 0;
    if (!(info->st_mode & S_IWUSR))
        attributes |= 0x1;
    return attributes != 0 ? attributes : 0x80;
}

static void port_file_time(const struct timespec *time, port_dword *file_time)
{
    unsigned long long ticks = (unsigned long long)time->tv_sec * 10000000ull + (unsigned long long)time->tv_nsec / 100
        + 116444736000000000ull;
    file_time[0] = (port_dword)ticks;
    file_time[1] = (port_dword)(ticks >> 32);
}

// The leading WIN32_FILE_ATTRIBUTE_DATA part of WIN32_FIND_DATAA too: attributes, three FILETIMEs, size high, low.
static void port_attribute_data(const struct stat *info, port_dword *data)
{
    data[0] = port_attributes(info);
    port_file_time(&info->st_ctim, data + 1);
    port_file_time(&info->st_atim, data + 3);
    port_file_time(&info->st_mtim, data + 5);
    data[7] = (port_dword)((unsigned long long)info->st_size >> 32);
    data[8] = (port_dword)info->st_size;
}

port_dword PORT_MS_ABI GetFileAttributesA(const char *name)
{
    char path[PORT_STORAGE_PATH];
    struct stat info;
    if (!port_resolve_xbox_path(name, PORT_STORAGE_READ, path))
        return 0xfffffffful;
    if (stat(path, &info) != 0) {
        port_fail_errno();
        return 0xfffffffful;
    }
    return port_attributes(&info);
}

int PORT_MS_ABI GetFileAttributesExA(const char *name, int level, void *data)
{
    (void)level;
    char path[PORT_STORAGE_PATH];
    struct stat info;
    if (!port_resolve_xbox_path(name, PORT_STORAGE_READ, path))
        return 0;
    if (stat(path, &info) != 0)
        return port_fail_errno();
    port_attribute_data(&info, (port_dword *)data);
    return 1;
}

int PORT_MS_ABI SetFileAttributesA(const char *name, port_dword attributes)
{
    char path[PORT_STORAGE_PATH];
    struct stat info;
    if (!port_resolve_xbox_path(name, PORT_STORAGE_WRITE, path))
        return 0;
    if (stat(path, &info) != 0)
        return port_fail_errno();
    mode_t mode = attributes & 0x1 ? info.st_mode & ~(mode_t)0222 : info.st_mode | S_IWUSR;
    return chmod(path, mode) == 0 ? 1 : port_fail_errno();
}

int PORT_MS_ABI CreateDirectoryA(const char *name, void *security)
{
    (void)security;
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_xbox_path(name, PORT_STORAGE_WRITE, path))
        return 0;
    return mkdir(path, 0777) == 0 ? 1 : port_fail_errno();
}

int PORT_MS_ABI RemoveDirectoryA(const char *name)
{
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_xbox_path(name, PORT_STORAGE_WRITE, path))
        return 0;
    return rmdir(path) == 0 ? 1 : port_fail_errno();
}

int PORT_MS_ABI DeleteFileA(const char *name)
{
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_xbox_path(name, PORT_STORAGE_WRITE, path))
        return 0;
    port_lock();
    struct stat info;
    int deleted = 0;
    if (lstat(path, &info) != 0)
        port_fail_errno();
    else if (!port_file_sharing_conflict(&info, PORT_FILE_SHARE_DELETE, PORT_FILE_SHARE_ALL))
        deleted = unlink(path) == 0 ? 1 : port_fail_errno();
    port_unlock();
    return deleted;
}

int PORT_MS_ABI MoveFileA(const char *existing, const char *target)
{
    char from[PORT_STORAGE_PATH], to[PORT_STORAGE_PATH];
    struct stat info;
    if (!port_resolve_xbox_path(existing, PORT_STORAGE_WRITE, from)
        || !port_resolve_xbox_path(target, PORT_STORAGE_WRITE, to))
        return 0;
    if (stat(to, &info) == 0) {
        port_last_error = PORT_ERROR_ALREADY_EXISTS;
        return 0;
    }
    return rename(from, to) == 0 ? 1 : port_fail_errno();
}

int PORT_MS_ABI CopyFileA(const char *existing, const char *target, int fail_if_exists)
{
    char from[PORT_STORAGE_PATH], to[PORT_STORAGE_PATH], buffer[65536];
    if (!port_resolve_xbox_path(existing, PORT_STORAGE_READ, from)
        || !port_resolve_xbox_path(target, PORT_STORAGE_WRITE, to))
        return 0;
    int source = open(from, O_RDONLY | O_CLOEXEC);
    if (source < 0)
        return port_fail_errno();
    int destination = open(to, O_WRONLY | O_CREAT | O_CLOEXEC | (fail_if_exists ? O_EXCL : O_TRUNC), 0666);
    if (destination < 0) {
        int error = errno;
        close(source);
        port_last_error = error == EEXIST ? PORT_ERROR_FILE_EXISTS : port_error_from_errno(error);
        return 0;
    }
    ssize_t count;
    int ok = 1;
    while ((count = read(source, buffer, sizeof(buffer))) > 0) {
        if (write(destination, buffer, (size_t)count) != count) {
            ok = port_fail_errno();
            break;
        }
    }
    if (count < 0)
        ok = port_fail_errno();
    close(source);
    close(destination);
    return ok;
}

int PORT_MS_ABI GetDiskFreeSpaceExA(const char *directory, unsigned long long *available, unsigned long long *total,
    unsigned long long *free_bytes)
{
    char path[PORT_STORAGE_PATH];
    struct statvfs info;
    if (!port_resolve_xbox_path(directory != 0 ? directory : "d:\\", PORT_STORAGE_READ, path))
        return 0;
    if (statvfs(path, &info) != 0)
        return port_fail_errno();
    if (available != 0)
        *available = (unsigned long long)info.f_bavail * info.f_frsize;
    if (total != 0)
        *total = (unsigned long long)info.f_blocks * info.f_frsize;
    if (free_bytes != 0)
        *free_bytes = (unsigned long long)info.f_bfree * info.f_frsize;
    return 1;
}

// Find searches: the pattern is the last path component (wildcards * and ?); WIN32_FIND_DATAA is the attribute data,
// two reserved words, the 260-byte name and the 14-byte short name.
static int port_find_next(port_object *find, void *data)
{
    struct dirent *entry;
    while ((entry = readdir(find->directory)) != 0) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (fnmatch(find->pattern, entry->d_name, FNM_CASEFOLD) != 0)
            continue;
        struct stat info;
        memset(data, 0, 320);
        if (fstatat(dirfd(find->directory), entry->d_name, &info, 0) == 0)
            port_attribute_data(&info, (port_dword *)data);
        strncpy((char *)data + 44, entry->d_name, 259);
        return 1;
    }
    port_last_error = PORT_ERROR_NO_MORE_FILES;
    return 0;
}

void *PORT_MS_ABI FindFirstFileA(const char *name, void *data)
{
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_xbox_path(name, PORT_STORAGE_READ, path))
        return PORT_INVALID_HANDLE;
    char *slash = strrchr(path, '/');
    const char *pattern = slash != 0 ? slash + 1 : path;
    char directory[PORT_STORAGE_PATH];
    if (slash != 0) {
        size_t length = (size_t)(slash - path);
        memcpy(directory, path, length);
        directory[length == 0 ? 1 : length] = 0;
        if (length == 0)
            directory[0] = '/';
    } else {
        strcpy(directory, ".");
    }
    DIR *listing = opendir(directory);
    if (listing == 0) {
        port_fail_errno();
        return PORT_INVALID_HANDLE;
    }
    void *handle;
    port_lock();
    port_object *find = port_new_object(PORT_FIND, &handle);
    find->directory = listing;
    strncpy(find->pattern, strcmp(pattern, "*.*") == 0 ? "*" : pattern, sizeof(find->pattern) - 1);
    port_unlock();
    if (!port_find_next(find, data)) {
        port_lock();
        closedir(listing);
        find->kind = PORT_FREE;
        port_unlock();
        port_last_error = PORT_ERROR_FILE_NOT_FOUND;
        return PORT_INVALID_HANDLE;
    }
    return handle;
}

int PORT_MS_ABI FindNextFileA(void *handle, void *data)
{
    port_lock();
    port_object *find = port_object_of(handle);
    port_unlock();
    if (find == 0 || find->kind != PORT_FIND) {
        port_last_error = PORT_ERROR_INVALID_HANDLE;
        return 0;
    }
    return port_find_next(find, data);
}

int PORT_MS_ABI CloseHandle(void *handle)
{
    port_lock();
    port_object *object = port_handle_object(handle);
    if (object == 0 || handle == PORT_CURRENT_THREAD) {
        port_unlock();
        port_last_error = PORT_ERROR_INVALID_HANDLE;
        return 0;
    }
    switch (object->kind) {
    case PORT_THREAD_ALIAS: {
        port_object *thread = object->target;
        thread->aliases--;
        if (thread->closed && thread->signaled && thread->aliases == 0)
            thread->kind = PORT_FREE;
        break;
    }
    case PORT_FILE:
        close(object->fd);
        break;
    case PORT_FIND:
        closedir(object->directory);
        break;
    case PORT_THREAD:
        if (!object->signaled || object->aliases > 0) {
            object->closed = 1; // freed when the thread exits and its aliases are closed
            port_unlock();
            return 1;
        }
        break;
    case PORT_COMPLETION:
        while (object->head != 0) {
            port_packet *next = object->head->next;
            free(object->head);
            object->head = next;
        }
        break;
    default:
        break;
    }
    object->kind = PORT_FREE;
    port_changed();
    port_unlock();
    return 1;
}

// ---------------------------------------------------------------------------------------------------------------
// Debugging.

void PORT_MS_ABI OutputDebugStringA(const char *text)
{
    if (getenv("PORT_DEBUG_OUTPUT") != 0)
        port_write_error(text);
}

// The Xbox raises 0x406D1388 to name a thread; any other exception is reported.
void PORT_MS_ABI RaiseException(port_dword code, port_dword flags, port_dword argument_count, const port_size *arguments)
{
    (void)flags; (void)argument_count; (void)arguments;
    if (code == 0x406d1388ul)
        return;
    port_write_error("port: RaiseException\n");
    port_exit(6);
}

// port_runtime.h: the kernel's random source (getrandom blocks only until the pool is first seeded at boot).
int port_host_random(void *buffer, unsigned long size)
{
    unsigned char *out = (unsigned char *)buffer;
    while (size > 0) {
        long got = syscall(SYS_getrandom, out, size, 0);
        if (got < 0) {
            if (errno == EINTR)
                continue;
            port_write_error("port: getrandom failed: no secure random source\n");
            port_exit(7);
        }
        out += got;
        size -= (unsigned long)got;
    }
    return 1;
}

// ---------------------------------------------------------------------------------------------------------------
// The kernel's thread list, as the game's minidump writer reads it (cseries/xenon_minidump.cpp calls these; retail
// 0x826f2d98..0x826f3090 walk the KPROCESS thread list and the KTHREAD fields). The threads are this kernel's.

typedef struct port_minidump_thread_list {
    port_dword count;
    void *handles[63];          // thread handles (opened: the caller closes them) or thread ids
} port_minidump_thread_list;

typedef struct port_minidump_thread_information { // s_xenon_minidump_thread_information, 0x30 bytes
    long suspend_count, priority, tls_base, start_address, stack_base, stack_limit;
    unsigned long long creation_time;
    port_dword stack_slack_space;
    unsigned char current_processor;
    port_dword last_error;
} port_minidump_thread_information;

// Under the lock: a new handle for a thread (released by CloseHandle).
static void *port_thread_alias(port_object *thread)
{
    void *handle;
    port_object *alias = port_new_object(PORT_THREAD_ALIAS, &handle);
    alias->target = thread;
    thread->aliases++;
    return handle;
}

static int port_thread_listed(const port_object *object)
{
    return object->kind == PORT_THREAD && object->started && !object->signaled;
}

// Bit 0 of flags: include the calling thread. Bit 1: opened handles (else thread ids).
void PORT_MS_ABI xenon_minidump_enumerate_thread_handles(port_dword flags, port_minidump_thread_list *list)
{
    list->count = 0;
    port_lock();
    for (int i = 0; i < PORT_HANDLE_CAPACITY && list->count < 63; i++) {
        port_object *thread = &port_objects[i];
        if (!port_thread_listed(thread) || ((flags & 1) == 0 && thread == port_current_thread))
            continue;
        list->handles[list->count++] = (flags & 2) != 0 ? port_thread_alias(thread) : (void *)(port_size)thread->thread_id;
    }
    port_unlock();
}

// An opened handle to the thread with this id, 0 if there is none.
void *PORT_MS_ABI xenon_minidump_get_thread_handle(port_dword thread_id)
{
    void *handle = 0;
    port_lock();
    for (int i = 0; i < PORT_HANDLE_CAPACITY && handle == 0; i++)
        if (port_thread_listed(&port_objects[i]) && port_objects[i].thread_id == thread_id)
            handle = port_thread_alias(&port_objects[i]);
    port_unlock();
    return handle;
}

void PORT_MS_ABI xenon_minidump_get_thread_information(void *handle, port_minidump_thread_information *information)
{
    port_lock();
    port_object *thread = port_object_of(handle);
    if (thread == 0 || thread->kind != PORT_THREAD) {
        port_unlock();
        return;
    }
    memset(information, 0, sizeof(*information));
    information->suspend_count = thread->suspend_count;
    information->start_address = (long)(port_size)thread->start;
    information->current_processor = (unsigned char)thread->processor;
    pthread_attr_t attributes;
    void *stack = 0;
    size_t stack_size = 0;
    if (thread->started && pthread_getattr_np(thread->thread, &attributes) == 0) {
        if (pthread_attr_getstack(&attributes, &stack, &stack_size) == 0) {
            information->stack_limit = (long)(port_size)stack; // the stack grows down from its base
            information->stack_base = (long)((port_size)stack + stack_size);
        }
        pthread_attr_destroy(&attributes);
    }
    port_unlock();
}

// The Xbox CONTEXT (the PowerPC register file). A host thread has none: every field after ContextFlags reads as the
// retail call leaves the ones it cannot fill (all bits set), and the caller falls back to the thread's whole stack.
void PORT_MS_ABI xenon_minidump_get_thread_context(void *handle, void *context)
{
    port_lock();
    port_object *thread = port_object_of(handle);
    if (thread != 0 && thread->kind == PORT_THREAD)
        memset((char *)context + 4, 0xff, 2620);
    port_unlock();
}
