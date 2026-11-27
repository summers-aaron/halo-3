// port/sdk/kernel (Windows host): the Xbox's Win32-style kernel API, forwarded to the host's kernel32 where the
// Xbox call and the Win32 call mean the same thing. Declarations follow the game's (include/cseries/crt_xdk.h);
// handles and structures are the host's (an Xbox RTL_CRITICAL_SECTION, 28 bytes, holds Win32's 24).
// Path-taking file calls translate Xbox drive prefixes through storage.c before calling kernel32.
#include "map_swap.h"
#include "storage.h"
#include "xapi_io.h"
#include "win32_host.h"
#include "../runtime/port_runtime.h"

typedef unsigned int port_dword;
typedef __SIZE_TYPE__ port_size;

#define PORT_INVALID_HANDLE ((void *)(__INTPTR_TYPE__)-1)

__declspec(dllimport) char *__cdecl getenv(const char *name);

PORT_HOST_IMPORT(void, OutputDebugStringA, 4, (const char *text));
PORT_HOST_IMPORT(void, InitializeCriticalSection, 4, (void *section));
PORT_HOST_IMPORT(int, InitializeCriticalSectionAndSpinCount, 8, (void *section, port_dword spin_count));
PORT_HOST_IMPORT(void, EnterCriticalSection, 4, (void *section));
PORT_HOST_IMPORT(void, LeaveCriticalSection, 4, (void *section));
PORT_HOST_IMPORT(int, TryEnterCriticalSection, 4, (void *section));
#ifdef _WIN64
PORT_HOST_IMPORT(void, AcquireSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(unsigned char, TryAcquireSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(void, ReleaseSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(port_dword, GetCurrentThreadId, 0, (void));
PORT_HOST_IMPORT(port_size, VirtualQuery, 12, (const void *address, void *info, port_size length));
#endif
PORT_HOST_IMPORT(port_dword, TlsAlloc, 0, (void));
PORT_HOST_IMPORT(int, TlsFree, 4, (port_dword index));
PORT_HOST_IMPORT(void *, TlsGetValue, 4, (port_dword index));
PORT_HOST_IMPORT(int, TlsSetValue, 8, (port_dword index, void *value));
PORT_HOST_IMPORT(void *, InterlockedPushEntrySList, 8, (void *head, void *entry));
PORT_HOST_IMPORT(void *, InterlockedPopEntrySList, 4, (void *head));
PORT_HOST_IMPORT(void *, InterlockedFlushSList, 4, (void *head));
PORT_HOST_IMPORT(void *, GetProcessHeap, 0, (void));
PORT_HOST_IMPORT(void *, HeapAlloc, 12, (void *heap, port_dword flags, port_size bytes));
PORT_HOST_IMPORT(int, HeapFree, 12, (void *heap, port_dword flags, void *memory));
PORT_HOST_IMPORT(void *, VirtualAlloc, 16, (void *address, port_size size, port_dword type, port_dword protect));
PORT_HOST_IMPORT(int, VirtualFree, 12, (void *address, port_size size, port_dword type));
PORT_HOST_IMPORT(void *, CreateFileMappingA, 24, (void *file, void *security, port_dword protect, port_dword size_high,
    port_dword size_low, const char *name));
PORT_HOST_IMPORT(void *, MapViewOfFileEx, 24, (void *mapping, port_dword access, port_dword offset_high,
    port_dword offset_low, port_size bytes, void *address));
PORT_HOST_IMPORT(int, UnmapViewOfFile, 4, (const void *address));
PORT_HOST_IMPORT(port_dword, GetLastError, 0, (void));
PORT_HOST_IMPORT(void, SetLastError, 4, (port_dword error));
PORT_HOST_IMPORT(int, CloseHandle, 4, (void *handle));
PORT_HOST_IMPORT(long, CompareFileTime, 8, (const void *first, const void *second));
PORT_HOST_IMPORT(void, GetSystemTimeAsFileTime, 4, (void *file_time));
PORT_HOST_IMPORT(void *, CreateFileA, 28, (const char *name, port_dword access, port_dword share, void *security,
    port_dword disposition, port_dword flags, void *template_file));
PORT_HOST_IMPORT(int, ReadFile, 20, (void *file, void *buffer, port_dword bytes, port_dword *read, void *overlapped));
PORT_HOST_IMPORT(int, WriteFile, 20, (void *file, const void *buffer, port_dword bytes, port_dword *written,
    void *overlapped));
PORT_HOST_IMPORT(port_dword, SetFilePointer, 16, (void *file, long distance, long *distance_high, port_dword method));
PORT_HOST_IMPORT(int, GetFileSizeEx, 8, (void *file, long long *size));
PORT_HOST_IMPORT(int, SetFileInformationByHandle, 16, (void *file, int info_class, void *info, port_dword bytes));
PORT_HOST_IMPORT(port_dword, GetFileAttributesA, 4, (const char *name));
PORT_HOST_IMPORT(int, GetFileAttributesExA, 12, (const char *name, int level, void *data));
PORT_HOST_IMPORT(int, SetFileAttributesA, 8, (const char *name, port_dword attributes));
PORT_HOST_IMPORT(int, CreateDirectoryA, 8, (const char *name, void *security));
PORT_HOST_IMPORT(int, RemoveDirectoryA, 4, (const char *name));
PORT_HOST_IMPORT(int, DeleteFileA, 4, (const char *name));
PORT_HOST_IMPORT(int, MoveFileA, 8, (const char *existing, const char *target));
PORT_HOST_IMPORT(int, CopyFileA, 12, (const char *existing, const char *target, int fail_if_exists));
PORT_HOST_IMPORT(void *, FindFirstFileA, 8, (const char *name, void *data));
PORT_HOST_IMPORT(int, GetDiskFreeSpaceExA, 16, (const char *directory, void *available, void *total, void *free_bytes));
PORT_HOST_IMPORT(int, GetQueuedCompletionStatus, 20, (void *port, port_dword *bytes, port_size *key,
    void **overlapped, port_dword milliseconds));
PORT_HOST_IMPORT(int, ReleaseMutex, 4, (void *mutex));
PORT_HOST_IMPORT(port_dword, ResumeThread, 4, (void *thread));
PORT_HOST_IMPORT(port_dword, SuspendThread, 4, (void *thread));
PORT_HOST_IMPORT(int, GetOverlappedResult, 16, (void *file, void *overlapped, port_dword *transferred, int wait));
PORT_HOST_IMPORT(void *, CreateEventA, 16, (void *security, int manual_reset, int initial_state, const char *name));
PORT_HOST_IMPORT(port_dword, FlsAlloc, 4, (void (__stdcall *callback)(void *)));
PORT_HOST_IMPORT(void *, FlsGetValue, 4, (port_dword index));
PORT_HOST_IMPORT(int, FlsSetValue, 8, (port_dword index, void *value));
PORT_HOST_IMPORT(int, InitOnceExecuteOnce, 16, (void **once, int (__stdcall *callback)(void **, void *, void **),
    void *parameter, void **context));
PORT_HOST_IMPORT(int, SetThreadPriority, 8, (void *thread, int priority));
PORT_HOST_IMPORT(port_dword, GetThreadId, 4, (void *thread));
PORT_HOST_IMPORT(int, GetThreadTimes, 20, (void *thread, unsigned long long *creation, unsigned long long *exit_time,
    unsigned long long *kernel_time, unsigned long long *user_time));
PORT_HOST_IMPORT(void, Sleep, 4, (port_dword milliseconds));
PORT_HOST_IMPORT(int, SwitchToThread, 0, (void));

// Critical sections (the Xbox's Rtl names; no delete on the Xbox, see winbase.h).
#ifdef _WIN64
static inline port_dword port_win64_tid(void)
{
#if defined(__x86_64__)
    // Only the low DWORD of Win64 TEB.ClientId.UniqueThread is returned by GetCurrentThreadId.
    port_dword tid;
    __asm__("movl %%gs:0x48, %0" : "=r"(tid));
    return tid;
#else
    return host_GetCurrentThreadId();
#endif
}

// Win64 RTL_CRITICAL_SECTION is 40 bytes, whereas Xbox RTL_CRITICAL_SECTION is 28 bytes.
// Align the recursive SRWLOCK inside the guest block and store the spin count in its last DWORD.
typedef struct port_cs64 {
    void *srw;
    port_dword owner;
    port_dword recursion;
} port_cs64;
_Static_assert(sizeof(port_cs64) + 7 <= 24, "aligned port_cs64 leaves room for the spin count");
static port_cs64 *port_cs64_of(void *section)
{
    return (port_cs64 *)(((__UINTPTR_TYPE__)section + 7u) & ~(__UINTPTR_TYPE__)7u);
}
void RtlInitializeCriticalSection(void *section) { __builtin_memset(section, 0, 28); }
void RtlInitializeCriticalSectionAndSpinCount(void *section, port_dword spin_count)
{
    RtlInitializeCriticalSection(section);
    // The high bit was an event-allocation flag on older versions of Windows.
    spin_count &= 0x7fffffffu;
    if (spin_count > 65535u)
        spin_count = 65535u;
    __builtin_memcpy((unsigned char *)section + 24, &spin_count, sizeof(spin_count));
}
void RtlEnterCriticalSection(void *section)
{
    port_cs64 *cs = port_cs64_of(section);
    port_dword self = port_win64_tid();
    port_dword owner = __atomic_load_n(&cs->owner, __ATOMIC_RELAXED);
    if (owner == self) {
        cs->recursion++;
        return;
    }
    if (owner != 0u) {
        port_dword spins;
        __builtin_memcpy(&spins, (unsigned char *)section + 24, sizeof(spins));
        while (spins-- != 0u) {
            __builtin_ia32_pause();
            if (__atomic_load_n(&cs->owner, __ATOMIC_RELAXED) == 0u)
                break;
        }
    }
    host_AcquireSRWLockExclusive(&cs->srw);
    __atomic_store_n(&cs->owner, self, __ATOMIC_RELAXED);
    cs->recursion = 1;
}
void RtlLeaveCriticalSection(void *section)
{
    port_cs64 *cs = port_cs64_of(section);
    if (--cs->recursion != 0)
        return;
    __atomic_store_n(&cs->owner, 0u, __ATOMIC_RELAXED);
    host_ReleaseSRWLockExclusive(&cs->srw);
}
port_dword RtlTryEnterCriticalSection(void *section)
{
    port_cs64 *cs = port_cs64_of(section);
    port_dword self = port_win64_tid();
    if (__atomic_load_n(&cs->owner, __ATOMIC_RELAXED) == self) {
        cs->recursion++;
        return 1;
    }
    if (!host_TryAcquireSRWLockExclusive(&cs->srw))
        return 0;
    __atomic_store_n(&cs->owner, self, __ATOMIC_RELAXED);
    cs->recursion = 1;
    return 1;
}
#else
void RtlInitializeCriticalSection(void *section) { host_InitializeCriticalSection(section); }
void RtlInitializeCriticalSectionAndSpinCount(void *section, port_dword spin_count)
{
    host_InitializeCriticalSectionAndSpinCount(section, spin_count);
}
void RtlEnterCriticalSection(void *section) { host_EnterCriticalSection(section); }
void RtlLeaveCriticalSection(void *section) { host_LeaveCriticalSection(section); }
port_dword RtlTryEnterCriticalSection(void *section) { return host_TryEnterCriticalSection(section) != 0; }
#endif

// Thread-local slots and lock-free lists.
port_dword TlsAlloc(void) { return host_TlsAlloc(); }
int TlsFree(port_dword index) { return host_TlsFree(index); }
#if defined(_WIN64) && defined(__x86_64__)
void *TlsGetValue(port_dword index)
{
    // Win64 TEB: LastErrorValue at gs:[0x68], TlsSlots[64] at gs:[0x1480].
    if (index < 64u) {
        void *val;
        __asm__ volatile(
            "movl $0, %%gs:0x68\n\t"
            "movq %%gs:0x1480(,%1,8), %0"
            : "=r"(val)
            : "r"((unsigned long long)index)
            : "memory");
        return val;
    }
    // The host handles expansion slots (including a missing expansion array) and invalid indices.
    return host_TlsGetValue(index);
}
#else
void *TlsGetValue(port_dword index) { return host_TlsGetValue(index); }
#endif
int TlsSetValue(port_dword index, void *value) { return host_TlsSetValue(index, value); }
#ifdef _WIN64
// Game queues can have adjacent eight-byte heads. Serialize by head address so a pop cannot read a node
// that another pop or flush has already handed back to its caller. The heads and links carry full pointers.
static struct __declspec(align(64)) { void *srw; } port_slist_locks[64];

static void **port_slist_lock_of(const void *head)
{
    __UINTPTR_TYPE__ addr = (__UINTPTR_TYPE__)head;
    return &port_slist_locks[((addr >> 3) ^ (addr >> 12)) & 63u].srw;
}

static void *port_slist_next(const void *link)
{
    void *next;
    __builtin_memcpy(&next, link, sizeof(next));
    return next;
}

void *InterlockedPushEntrySList(void *head, void *entry)
{
    void **lock = port_slist_lock_of(head);
    host_AcquireSRWLockExclusive(lock);
    void *first = port_slist_next(head);
    __builtin_memcpy(entry, &first, sizeof(first));
    __builtin_memcpy(head, &entry, sizeof(entry));
    host_ReleaseSRWLockExclusive(lock);
    return first;
}
void *InterlockedPopEntrySList(void *head)
{
    void **lock = port_slist_lock_of(head);
    host_AcquireSRWLockExclusive(lock);
    void *first = port_slist_next(head);
    if (first != 0) {
        void *next = port_slist_next(first);
        __builtin_memcpy(head, &next, sizeof(next));
    }
    host_ReleaseSRWLockExclusive(lock);
    return first;
}
void *InterlockedFlushSList(void *head)
{
    void **lock = port_slist_lock_of(head);
    host_AcquireSRWLockExclusive(lock);
    void *first = port_slist_next(head);
    void *empty = 0;
    __builtin_memcpy(head, &empty, sizeof(empty));
    host_ReleaseSRWLockExclusive(lock);
    return first;
}

unsigned short QueryDepthSList(void *head)
{
    // Count under the same lock so the eight-byte head can store a full pointer.
    void **lock = port_slist_lock_of(head);
    host_AcquireSRWLockExclusive(lock);
    unsigned short depth = 0;
    for (void *entry = port_slist_next(head); entry != 0; entry = port_slist_next(entry))
        depth++;
    host_ReleaseSRWLockExclusive(lock);
    return depth;
}
#else
void *InterlockedPushEntrySList(void *head, void *entry) { return host_InterlockedPushEntrySList(head, entry); }
void *InterlockedPopEntrySList(void *head) { return host_InterlockedPopEntrySList(head); }
void *InterlockedFlushSList(void *head) { return host_InterlockedFlushSList(head); }
#endif

// Heaps and memory.
//
// Physical memory with an explicit address lives at the Xbox's 64 KiB-page window (0xA0000000 + physical address)
// and is also visible read-only through the cached view (d3d9gpu.h GPU_CONVERT_CPU_TO_CPU_CACHED_READONLY_ADDRESS:
// 0xC0000000 + physical address), which CPU texture reads use: one pagefile-backed section mapped at both (see
// kernel_linux.c). Requests for anywhere are plain committed memory (below 4 GB on Win64).
#define PORT_XBOX_PHYSICAL_WINDOW 0xa0000000ull
#define PORT_XBOX_CACHED_READONLY_WINDOW 0xc0000000ull
#define PORT_PHYSICAL_VIEW_CAPACITY 64

typedef struct port_physical_view { void *address; void *alias; void *section; } port_physical_view;
static port_physical_view port_physical_views[PORT_PHYSICAL_VIEW_CAPACITY];
static volatile long port_physical_views_lock;

static void port_physical_views_enter(void) { while (__sync_lock_test_and_set(&port_physical_views_lock, 1)) {} }
static void port_physical_views_leave(void) { __sync_lock_release(&port_physical_views_lock); }

// Unmaps the section views starting at address; 0 when address is not one.
static int port_physical_view_release(void *address)
{
    port_physical_view view = {0, 0, 0};
    port_physical_views_enter();
    for (int i = 0; i < PORT_PHYSICAL_VIEW_CAPACITY; i++) {
        if (port_physical_views[i].address == address) {
            view = port_physical_views[i];
            port_physical_views[i].address = 0;
            break;
        }
    }
    port_physical_views_leave();
    if (view.address == 0)
        return 0;
    host_UnmapViewOfFile(view.alias);
    host_UnmapViewOfFile(view.address);
    host_CloseHandle(view.section);
    return 1;
}

#ifdef _WIN64
// Keep game-visible virtual/physical/heap allocations below 4 GB so 32-bit s_tag_pointer<T> slots and any legacy
// 32-bit pointer truncations stay valid while Vulkan/D3D12/host allocations freely use full 64-bit space.
// Skips [0xA0000000, 0xE0000000) which is reserved for the Xbox physical window and its cached read-only alias.
#define PORT_LOW4G_LO 0x20000000ull
#define PORT_LOW4G_HOLE_END 0xe0000000ull
#define PORT_LOW4G_HI 0xf8000000ull
static port_size port_low4g_cursor = PORT_LOW4G_LO;
static void *port_low4g_lock;

typedef struct port_memory_info {
    void *base, *allocation_base;
    port_dword allocation_protect, partition;
    port_size region_size;
    port_dword state, protect, type;
} port_memory_info;
_Static_assert(sizeof(port_memory_info) == 48, "Win64 MEMORY_BASIC_INFORMATION layout");

static port_size port_align_up(port_size value, port_size align)
{
    return (value + align - 1) & ~(align - 1);
}

// The window is the game's alone. Windows places host allocations bottom-up, so without this the graphics driver's
// own reservations land in it: NVIDIA's Vulkan driver reserves two 1 GB ranges at device creation, taking ~1.4 GB of
// the 2.4 GB window and leaving no room for render targets at higher resolutions. The first allocation here reserves
// every free 64 KiB-aligned stretch of the window as a guard (MEM_RESERVE, PAGE_NOACCESS) so host allocations go
// elsewhere; allocations are carved out of guards, and released game memory is guarded again.
#define PORT_LOW4G_GRANULE 0x10000ull
#define PORT_LOW4G_GRANULES ((PORT_LOW4G_HI - PORT_LOW4G_LO) / PORT_LOW4G_GRANULE)
#define PORT_MEM_FREE 0x10000u
#define PORT_MEM_RESERVE 0x2000u
static unsigned int port_low4g_guard_len[PORT_LOW4G_GRANULES]; // at a guard's first granule: its length in granules
static int port_low4g_guards_installed;

static int port_low4g_is_guard(port_size base)
{
    return base >= PORT_LOW4G_LO && base < PORT_LOW4G_HI && (base & (PORT_LOW4G_GRANULE - 1)) == 0 &&
           port_low4g_guard_len[(base - PORT_LOW4G_LO) / PORT_LOW4G_GRANULE] != 0;
}

static port_size port_low4g_guard_end(port_size base)
{
    return base + (port_size)port_low4g_guard_len[(base - PORT_LOW4G_LO) / PORT_LOW4G_GRANULE] * PORT_LOW4G_GRANULE;
}

static void port_low4g_unguard(port_size base)
{
    port_low4g_guard_len[(base - PORT_LOW4G_LO) / PORT_LOW4G_GRANULE] = 0;
    host_VirtualFree((void *)base, 0, 0x8000 /* MEM_RELEASE */);
}

// Guards every free 64 KiB-aligned stretch of [lo, hi), which must not cross the hole (lock held).
static void port_low4g_guard_free(port_size lo, port_size hi)
{
    port_size a = lo;
    while (a < hi) {
        port_memory_info info;
        if (host_VirtualQuery((void *)a, &info, sizeof(info)) != sizeof(info))
            return;
        port_size end = (port_size)info.base + info.region_size;
        if (end <= a)
            return;
        if (end > hi)
            end = hi;
        if (info.state == PORT_MEM_FREE) {
            const port_size gb = port_align_up(a, PORT_LOW4G_GRANULE), ge = end & ~(PORT_LOW4G_GRANULE - 1);
            if (ge > gb && host_VirtualAlloc((void *)gb, ge - gb, PORT_MEM_RESERVE, 0x01 /* PAGE_NOACCESS */) == (void *)gb)
                port_low4g_guard_len[(gb - PORT_LOW4G_LO) / PORT_LOW4G_GRANULE] = (unsigned int)((ge - gb) / PORT_LOW4G_GRANULE);
        }
        a = end;
    }
}

// Guards the free parts of [lo, hi) on both sides of the hole (lock held).
static void port_low4g_guard_window(port_size lo, port_size hi)
{
    if (lo < PORT_XBOX_PHYSICAL_WINDOW)
        port_low4g_guard_free(lo, hi < PORT_XBOX_PHYSICAL_WINDOW ? hi : PORT_XBOX_PHYSICAL_WINDOW);
    if (hi > PORT_LOW4G_HOLE_END)
        port_low4g_guard_free(lo > PORT_LOW4G_HOLE_END ? lo : PORT_LOW4G_HOLE_END, hi);
}

// Allocates [cand, cand + size) if it holds only free space and guards, releasing those guards and guarding what is
// left of them (lock held). On failure *resume is where the scan continues.
static void *port_low4g_claim(port_size cand, port_size size, port_dword type, port_dword protect, port_size *resume)
{
    const port_size need_end = cand + size;
    for (port_size scan = cand; scan < need_end;) {
        port_memory_info info;
        if (host_VirtualQuery((void *)scan, &info, sizeof(info)) != sizeof(info)) {
            *resume = need_end;
            return 0;
        }
        const port_size base = (port_size)info.allocation_base;
        if (info.state == PORT_MEM_FREE)
            scan = (port_size)info.base + info.region_size;
        else if (info.state == PORT_MEM_RESERVE && port_low4g_is_guard(base))
            scan = port_low4g_guard_end(base);
        else {
            *resume = (port_size)info.base + info.region_size; // occupied: continue past it
            return 0;
        }
    }
    port_size lo = cand, hi = need_end;
    for (port_size scan = cand; scan < need_end;) {
        port_memory_info info;
        if (host_VirtualQuery((void *)scan, &info, sizeof(info)) != sizeof(info))
            break;
        const port_size base = (port_size)info.allocation_base;
        if (info.state == PORT_MEM_RESERVE && port_low4g_is_guard(base)) {
            const port_size end = port_low4g_guard_end(base);
            if (base < lo)
                lo = base;
            if (end > hi)
                hi = end;
            port_low4g_unguard(base);
            scan = end;
        } else {
            scan = (port_size)info.base + info.region_size;
        }
    }
    void *p = host_VirtualAlloc((void *)cand, size, type, protect);
    port_low4g_guard_window(lo, hi); // the rest of the released guards (all of them if another thread took the space)
    *resume = need_end;
    return p == (void *)cand ? p : 0;
}

static void *port_valloc_low4g(port_size size, port_size align, port_dword type, port_dword protect)
{
    if (size == 0 || size > PORT_XBOX_PHYSICAL_WINDOW - PORT_LOW4G_LO || align > PORT_LOW4G_HI ||
        (align & (align - 1)) != 0)
        return 0;
    if (align < 0x10000ull)
        align = 0x10000ull;
    host_AcquireSRWLockExclusive(&port_low4g_lock);
    if (!port_low4g_guards_installed) {
        port_low4g_guards_installed = 1;
        port_low4g_guard_window(PORT_LOW4G_LO, PORT_LOW4G_HI);
    }
    port_size start = port_align_up(port_low4g_cursor, align);
    if (start > PORT_LOW4G_HI - size)
        start = port_align_up(PORT_LOW4G_LO, align);
    port_size cand = start;
    for (int pass = 0; pass < 2; pass++) {
        while (cand <= PORT_LOW4G_HI - size && (pass == 0 || cand < start)) {
            if (cand < PORT_LOW4G_HOLE_END && cand + size > PORT_XBOX_PHYSICAL_WINDOW) {
                cand = port_align_up(PORT_LOW4G_HOLE_END, align);
                continue;
            }
            void *p = host_VirtualAlloc((void *)cand, size, type, protect);
            port_memory_info info;
            if (p == 0 && host_VirtualQuery((void *)cand, &info, sizeof(info)) == sizeof(info)) {
                const port_size end = (port_size)info.base + info.region_size;
                if (info.state == PORT_MEM_FREE ||
                    (info.state == PORT_MEM_RESERVE && port_low4g_is_guard((port_size)info.allocation_base))) {
                    // free space or a guard: take it with whatever free space and guards follow
                    port_size resume = end;
                    p = port_low4g_claim(cand, size, type, protect, &resume);
                    if (p == 0) {
                        cand = port_align_up(resume > cand ? resume : cand + align, align);
                        continue;
                    }
                } else if (end > cand) {
                    cand = port_align_up(end, align); // occupied
                    continue;
                }
            }
            if (p != 0) {
                port_low4g_cursor = port_align_up(cand + size, 0x10000ull);
                host_ReleaseSRWLockExclusive(&port_low4g_lock);
                return p;
            }
            cand += align;
        }
        cand = port_align_up(PORT_LOW4G_LO, align);
    }
    host_ReleaseSRWLockExclusive(&port_low4g_lock);
    return 0;
}

// Releases an allocation that starts at address. Inside the window, its range is guarded again so host allocations
// cannot take it.
static int port_low4g_release_guarded(void *address)
{
    const port_size a = (port_size)address;
    if (!port_low4g_guards_installed || a < PORT_LOW4G_LO || a >= PORT_LOW4G_HI ||
        (a >= PORT_XBOX_PHYSICAL_WINDOW && a < PORT_LOW4G_HOLE_END))
        return host_VirtualFree(address, 0, 0x8000 /* MEM_RELEASE */) != 0;
    port_size end = a;
    for (;;) {
        port_memory_info info;
        if (host_VirtualQuery((void *)end, &info, sizeof(info)) != sizeof(info) || info.allocation_base != address ||
            info.state == PORT_MEM_FREE)
            break;
        end = (port_size)info.base + info.region_size;
        if (end >= PORT_LOW4G_HI)
            break;
    }
    if (host_VirtualFree(address, 0, 0x8000 /* MEM_RELEASE */) == 0)
        return 0;
    if (end > a) {
        host_AcquireSRWLockExclusive(&port_low4g_lock);
        port_low4g_guard_window(a, end);
        host_ReleaseSRWLockExclusive(&port_low4g_lock);
    }
    return 1;
}

// Logs a failed allocation of 1 MB or more below 4 GB with its caller's return address (symbolize against the PDB):
// "port: low4g <tag> <size> @ 0 ra <address>", tag V VirtualAlloc, P XPhysicalAlloc, H RtlAllocateHeap,
// R port_low4g_reserve_commit.
static void port_low4g_note(char tag, port_size size, void *p, void *ra)
{
    if (p != 0 || size < (1ull << 20))
        return;
    char line[128];
    int n = 0;
    const char *head = "port: low4g ";
    while (head[n]) { line[n] = head[n]; n++; }
    line[n++] = tag; line[n++] = ' ';
    for (int s = 36; s >= 0; s -= 4) line[n++] = "0123456789abcdef"[(size >> s) & 15];
    line[n++] = ' '; line[n++] = '@'; line[n++] = ' ';
    for (int s = 36; s >= 0; s -= 4) line[n++] = "0123456789abcdef"[((unsigned long long)p >> s) & 15];
    line[n++] = ' '; line[n++] = 'r'; line[n++] = 'a'; line[n++] = ' ';
    for (int s = 36; s >= 0; s -= 4) line[n++] = "0123456789abcdef"[((unsigned long long)ra >> s) & 15];
    line[n++] = '\n'; line[n] = 0;
    port_write_error(line);
}

void *port_low4g_reserve_commit(port_size size)
{
    void *p = port_valloc_low4g(size, 0x10000ull, 0x3000 /* MEM_COMMIT | MEM_RESERVE */, 0x04 /* PAGE_READWRITE */);
    port_low4g_note('R', size, p, __builtin_return_address(0));
    return p;
}

void port_low4g_release(void *base)
{
    if (base != 0)
        port_low4g_release_guarded(base);
}

void *VirtualAlloc(void *address, port_size size, port_dword type, port_dword protect)
{
    if (address == 0 && (type & 0x2000u /* MEM_RESERVE */) != 0) {
        void *p = port_valloc_low4g(size, 0x10000ull, type, protect);
        port_low4g_note('V', size, p, __builtin_return_address(0));
        return p;
    }
    void *p = host_VirtualAlloc(address, size, type, protect);
    const port_size a = (port_size)address & ~(PORT_LOW4G_GRANULE - 1);
    if (p == 0 && address != 0 && (type & PORT_MEM_RESERVE) != 0 && port_low4g_guards_installed &&
        a >= PORT_LOW4G_LO && a + size <= PORT_LOW4G_HI &&
        !(a < PORT_LOW4G_HOLE_END && a + size > PORT_XBOX_PHYSICAL_WINDOW)) {
        // a reservation at a fixed address in the window: take it from the guards there
        port_size resume;
        host_AcquireSRWLockExclusive(&port_low4g_lock);
        p = port_low4g_claim(a, port_align_up((port_size)address + size - a, 0x1000ull), type, protect, &resume);
        host_ReleaseSRWLockExclusive(&port_low4g_lock);
    }
    return p;
}

#define PORT_HEAP_MAGIC 0x48334850u
#define PORT_HEAP_CLASSES 13 // 32B .. 128KB (2^(5+cls))
typedef struct port_heap_block {
    port_dword magic;
    port_dword cls;       // 0..PORT_HEAP_CLASSES-1 for pooled, 0xffffffffu for direct VirtualAlloc
    port_size map_size;
    struct port_heap_block *next;
    port_size align_pad;  // keeps sizeof(port_heap_block) == 32 for 16-byte SIMD alignment
} port_heap_block;

static port_heap_block *port_heap_free[PORT_HEAP_CLASSES];
static unsigned char *port_heap_slab = 0;
static port_size port_heap_slab_rem = 0;
static void *port_heap_lock;
// VirtualAlloc regions start on a 64 KiB boundary. Track the slab pages and direct block headers so a host heap
// allocation can go straight to HeapFree, even when the host heap happens to be below 4 GB.
static unsigned int port_heap_pages[65536 / 32];

static void port_heap_mark(void *base, port_size size, int owned)
{
    port_size page = (port_size)base >> 16;
    for (port_size end = page + (size >> 16); page < end; page++) {
        unsigned int mask = 1u << (page & 31u);
        if (owned)
            __atomic_fetch_or(&port_heap_pages[page >> 5], mask, __ATOMIC_RELAXED);
        else
            __atomic_fetch_and(&port_heap_pages[page >> 5], ~mask, __ATOMIC_RELAXED);
    }
}

void *GetProcessHeap(void) { return host_GetProcessHeap(); }
void *RtlAllocateHeap(void *heap, port_dword flags, port_size bytes)
{
    (void)heap;
    if (bytes == 0)
        bytes = 1;
    if (bytes > PORT_XBOX_PHYSICAL_WINDOW - PORT_LOW4G_LO - sizeof(port_heap_block))
        return 0;
    port_size total = sizeof(port_heap_block) + bytes;
    if (total > (32u << (PORT_HEAP_CLASSES - 1))) {
        port_size map_sz = port_align_up(total, 0x10000ull);
        void *p = port_valloc_low4g(map_sz, 0x10000ull, 0x3000 /* MEM_COMMIT | MEM_RESERVE */, 0x04 /* PAGE_READWRITE */);
        port_low4g_note('H', map_sz, p, __builtin_return_address(0));
        if (p == 0)
            return 0;
        port_heap_block *b = (port_heap_block *)p;
        b->magic = PORT_HEAP_MAGIC;
        b->cls = 0xffffffffu;
        b->map_size = map_sz;
        b->next = 0;
        port_heap_mark(b, 0x10000ull, 1);
        return (void *)(b + 1);
    }
    int cls = 27 - __builtin_clz((unsigned int)(total - 1)); // ceil(log2(total)) - log2(32)
    port_size block_sz = (port_size)32u << cls;
    host_AcquireSRWLockExclusive(&port_heap_lock);
    port_heap_block *b = port_heap_free[cls];
    if (b) {
        port_heap_free[cls] = b->next;
    } else {
        if (port_heap_slab_rem < block_sz) {
            const port_size slab_sz = 2u * 1024u * 1024u;
            void *p = port_valloc_low4g(slab_sz, 0x10000ull, 0x3000, 0x04);
            if (p == 0) {
                host_ReleaseSRWLockExclusive(&port_heap_lock);
                return 0;
            }
            port_heap_mark(p, slab_sz, 1);
            port_heap_slab = (unsigned char *)p;
            port_heap_slab_rem = slab_sz;
        }
        b = (port_heap_block *)port_heap_slab;
        port_heap_slab += block_sz;
        port_heap_slab_rem -= block_sz;
    }
    host_ReleaseSRWLockExclusive(&port_heap_lock);
    b->magic = PORT_HEAP_MAGIC;
    b->cls = (port_dword)cls;
    b->map_size = block_sz - sizeof(port_heap_block);
    b->next = 0;
    void *user = (void *)(b + 1);
    if (flags & 0x8 /* HEAP_ZERO_MEMORY */)
        __builtin_memset(user, 0, bytes);
    return user;
}
int RtlFreeHeap(void *heap, port_dword flags, void *memory)
{
    if (!memory)
        return 1;
    port_size addr = (port_size)memory;
    if (addr < PORT_LOW4G_LO + sizeof(port_heap_block) || addr >= PORT_LOW4G_HI)
        return host_HeapFree(heap, flags, memory) != 0;
    port_size page = (addr - sizeof(port_heap_block)) >> 16;
    if ((__atomic_load_n(&port_heap_pages[page >> 5], __ATOMIC_RELAXED) & (1u << (page & 31u))) == 0)
        return host_HeapFree(heap, flags, memory) != 0;
    port_heap_block *b = ((port_heap_block *)memory) - 1;
    if (b->magic != PORT_HEAP_MAGIC)
        return host_HeapFree(heap, flags, memory) != 0;
    if (b->cls == 0xffffffffu) {
        port_heap_mark(b, 0x10000ull, 0);
        if (port_low4g_release_guarded(b))
            return 1;
        port_heap_mark(b, 0x10000ull, 1);
        return 0;
    }
    port_dword cls = b->cls;
    host_AcquireSRWLockExclusive(&port_heap_lock);
    b->next = port_heap_free[cls];
    port_heap_free[cls] = b;
    host_ReleaseSRWLockExclusive(&port_heap_lock);
    return 1;
}
#else
void *GetProcessHeap(void) { return host_GetProcessHeap(); }
void *RtlAllocateHeap(void *heap, port_dword flags, port_size bytes) { return host_HeapAlloc(heap, flags, bytes); }
int RtlFreeHeap(void *heap, port_dword flags, void *memory) { return host_HeapFree(heap, flags, memory) != 0; }
#endif

void *XPhysicalAlloc(port_size size, port_size address, port_size alignment, port_dword protect)
{
    if (address != ~(port_size)0 && (port_dword)address != ~0u) {
        void *where = (void *)(__UINTPTR_TYPE__)(PORT_XBOX_PHYSICAL_WINDOW + (port_dword)address);
        void *readonly_where = (void *)(__UINTPTR_TYPE__)(PORT_XBOX_CACHED_READONLY_WINDOW + (port_dword)address);
        void *section = host_CreateFileMappingA(PORT_INVALID_HANDLE, 0, 0x04 /* PAGE_READWRITE */,
            (port_dword)((unsigned long long)size >> 32), (port_dword)size, 0);
        void *memory = section ? host_MapViewOfFileEx(section, 0x2 /* FILE_MAP_WRITE */, 0, 0, size, where) : 0;
        void *readonly = memory ? host_MapViewOfFileEx(section, 0x4 /* FILE_MAP_READ */, 0, 0, size, readonly_where) : 0;
        if (memory != where || readonly != readonly_where) {
            port_write_error("port: cannot map Xbox physical memory at its window addresses\n");
            port_exit(7);
        }
        int recorded = 0;
        port_physical_views_enter();
        for (int i = 0; i < PORT_PHYSICAL_VIEW_CAPACITY && !recorded; i++) {
            if (port_physical_views[i].address == 0) {
                port_physical_views[i] = (port_physical_view){memory, readonly, section};
                recorded = 1;
            }
        }
        port_physical_views_leave();
        if (!recorded) {
            port_write_error("port: too many Xbox physical memory blocks at fixed addresses\n");
            port_exit(7);
        }
        port_physical_allocated(memory, (unsigned long)size, protect);
        return memory;
    }
#ifdef _WIN64
    port_size align = alignment > 0x10000ull ? alignment : 0x10000ull;
    void *memory = port_valloc_low4g(size, align, 0x3000 /* MEM_COMMIT | MEM_RESERVE */, 0x04 /* PAGE_READWRITE */);
    port_low4g_note('P', size, memory, __builtin_return_address(0));
#else
    (void)alignment;
    void *memory = host_VirtualAlloc(0, size, 0x3000 /* MEM_COMMIT | MEM_RESERVE */, 0x04 /* PAGE_READWRITE */);
#endif
    if (memory != 0)
        port_physical_allocated(memory, (unsigned long)size, protect);
    return memory;
}
void XPhysicalFree(void *address)
{
    port_physical_released(address);
    if (!port_physical_view_release(address)) {
#ifdef _WIN64
        port_low4g_release_guarded(address);
#else
        host_VirtualFree(address, 0, 0x8000 /* MEM_RELEASE */);
#endif
    }
}
// The game also releases XPhysicalAlloc memory with VirtualFree (restricted_memory.cpp): a section view is released
// as XPhysicalFree releases it; anything else is the host's.
int VirtualFree(void *address, port_size size, port_dword type)
{
    if ((type & 0x8000 /* MEM_RELEASE */) && port_physical_view_release(address)) {
        port_physical_released(address);
        return 1;
    }
#ifdef _WIN64
    if ((type & 0x8000 /* MEM_RELEASE */) && size == 0)
        return port_low4g_release_guarded(address);
#endif
    return host_VirtualFree(address, size, type);
}
port_dword XMemGetPageSize(const void *address)
{
    port_size where = (port_size)address;
    return where >= PORT_XBOX_PHYSICAL_WINDOW && where < 0xc0000000ull ? 0x10000 : 4096;
}
void *XMemSet(void *destination, int value, port_size size) { return __builtin_memset(destination, value, size); }
void *XMemCpy(void *destination, const void *source, port_size size) { return __builtin_memcpy(destination, source, size); }

// Debugging. To an attached debugger as on Windows, and with PORT_DEBUG_OUTPUT set to stderr too, as on Linux
// (kernel_linux.c), so a run without a debugger shows the game's debug output.
void OutputDebugStringA(const char *text)
{
    if (text == 0)
        return;
    if (getenv("PORT_DEBUG_OUTPUT") != 0)
        port_write_error(text);
    host_OutputDebugStringA(text);
}

// Errors.
port_dword GetLastError(void) { return host_GetLastError(); }
port_dword port_nt_status_to_win32(long status); // xapi_system.c
void RtlSetLastNTError(long status) { host_SetLastError(port_nt_status_to_win32(status)); }

// Files, handles and time.
static int port_resolve_win_path(const char *name, int access, char path[PORT_STORAGE_PATH])
{
    unsigned long error = 0;
    if (!port_storage_path(name, access, path, &error)) {
        host_SetLastError(error);
        return 0;
    }
    return 1;
}

int GetVolumeInformationA(const char *root, char *volume_name, port_dword volume_name_size, port_dword *serial_number,
    port_dword *max_component_length, port_dword *file_system_flags, char *file_system_name,
    port_dword file_system_name_size)
{
    port_dword error = 0;
    int ok = port_volume_information(root, volume_name, volume_name_size, serial_number, max_component_length,
        file_system_flags, file_system_name, file_system_name_size, &error);
    host_SetLastError(error);
    return ok;
}

int CloseHandle(void *handle) { return host_CloseHandle(handle); }
long CompareFileTime(const void *first, const void *second) { return host_CompareFileTime(first, second); }
void GetSystemTimeAsFileTime(void *file_time) { host_GetSystemTimeAsFileTime(file_time); }
void *CreateFileA(const char *name, port_dword access, port_dword share, void *security, port_dword disposition,
    port_dword flags, void *template_file)
{
    int write_access = (access & 0x40000000ul) != 0 || disposition == 1 || disposition == 2 || disposition == 4
        || disposition == 5;
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_win_path(name, write_access ? PORT_STORAGE_WRITE : PORT_STORAGE_READ, path))
        return PORT_INVALID_HANDLE;
    return host_CreateFileA(path, access, share, security, disposition, flags, template_file);
}
// The file offset of a header-sized transfer (map_swap.h looks at no other): OVERLAPPED::Offset / OffsetHigh, or
// the file pointer. -1 when unknown.
static unsigned long long transfer_offset(void *file, port_dword bytes, const void *overlapped)
{
    if (bytes != PORT_MAP_SWAP_HEADER_SIZE)
        return ~0ull;
    if (overlapped != 0) {
        typedef struct {
            port_size internal, internal_high;
            port_dword offset, offset_high;
            void *event;
        } port_overlapped;
        const port_overlapped *ov = (const port_overlapped *)overlapped;
        return (unsigned long long)ov->offset_high << 32 | ov->offset;
    }
    long high = 0;
    const port_dword low = host_SetFilePointer(file, 0, &high, 1 /* FILE_CURRENT */);
    if (low == 0xfffffffful && host_GetLastError() != 0)
        return ~0ull;
    return (unsigned long long)(port_dword)high << 32 | low;
}

int ReadFile(void *file, void *buffer, port_dword bytes, port_dword *read, void *overlapped)
{
    const unsigned long long offset = transfer_offset(file, bytes, overlapped);
    port_dword done = 0;
    const int ok = host_ReadFile(file, buffer, bytes, &done, overlapped);
    if (read != 0)
        *read = done;
    // completed now (a pending overlapped read is not seen, map_swap.h): cache-file headers arrive in host order
    if (ok)
        port_map_swap_after_read(buffer, bytes, done, offset);
    return ok;
}

int WriteFile(void *file, const void *buffer, port_dword bytes, port_dword *written, void *overlapped)
{
    if (bytes != PORT_MAP_SWAP_HEADER_SIZE || overlapped != 0) // an overlapped write may complete after we return
        return host_WriteFile(file, buffer, bytes, written, overlapped);
    // cache-file headers leave in disk order (map_swap.h)
    unsigned char scratch[PORT_MAP_SWAP_HEADER_SIZE];
    const void *source = port_map_swap_before_write(buffer, bytes, transfer_offset(file, bytes, 0), scratch);
    return host_WriteFile(file, source, bytes, written, 0);
}

// SetFileInformationByHandle: the two classes the game sets (async_helpers.cpp), with the meaning both hosts give
// them (kernel_linux.c): FileEndOfFileInfo (6) sets the file's size; FileAllocationInfo (5) reserves space by growing
// the file to the size, never shrinking it. Win32's FileAllocationInfo means something else (it reserves clusters and
// leaves the end of file, and a smaller size truncates), and Wine does not implement it, so the cache-file copy
// (async_allocate_file_space) failed there; both classes therefore end in the host's FileEndOfFileInfo.
int SetFileInformationByHandle(void *handle, int info_class, void *info, port_dword bytes)
{
    if ((info_class != 5 && info_class != 6) || info == 0 || bytes < 8) {
        host_SetLastError(87); // ERROR_INVALID_PARAMETER
        return 0;
    }
    long long size;
    __builtin_memcpy(&size, info, sizeof(size));
    if (size < 0) {
        host_SetLastError(87);
        return 0;
    }
    if (info_class == 5) {
        long long current = 0;
        if (!host_GetFileSizeEx(handle, &current))
            return 0;
        if (size <= current)
            return 1;
    }
    return host_SetFileInformationByHandle(handle, 6 /* FileEndOfFileInfo */, &size, sizeof(size));
}

port_dword GetFileAttributesA(const char *name)
{
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_win_path(name, PORT_STORAGE_READ, path))
        return 0xfffffffful;
    return host_GetFileAttributesA(path);
}
int GetFileAttributesExA(const char *name, int level, void *data)
{
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_win_path(name, PORT_STORAGE_READ, path))
        return 0;
    return host_GetFileAttributesExA(path, level, data);
}
int SetFileAttributesA(const char *name, port_dword attributes)
{
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_win_path(name, PORT_STORAGE_WRITE, path))
        return 0;
    return host_SetFileAttributesA(path, attributes);
}
int CreateDirectoryA(const char *name, void *security)
{
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_win_path(name, PORT_STORAGE_WRITE, path))
        return 0;
    return host_CreateDirectoryA(path, security);
}
int RemoveDirectoryA(const char *name)
{
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_win_path(name, PORT_STORAGE_WRITE, path))
        return 0;
    return host_RemoveDirectoryA(path);
}
int DeleteFileA(const char *name)
{
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_win_path(name, PORT_STORAGE_WRITE, path))
        return 0;
    return host_DeleteFileA(path);
}
int MoveFileA(const char *existing, const char *target)
{
    char from[PORT_STORAGE_PATH], to[PORT_STORAGE_PATH];
    if (!port_resolve_win_path(existing, PORT_STORAGE_WRITE, from)
        || !port_resolve_win_path(target, PORT_STORAGE_WRITE, to))
        return 0;
    return host_MoveFileA(from, to);
}
int CopyFileA(const char *existing, const char *target, int fail_if_exists)
{
    char from[PORT_STORAGE_PATH], to[PORT_STORAGE_PATH];
    if (!port_resolve_win_path(existing, PORT_STORAGE_READ, from)
        || !port_resolve_win_path(target, PORT_STORAGE_WRITE, to))
        return 0;
    return host_CopyFileA(from, to, fail_if_exists);
}
void *FindFirstFileA(const char *name, void *data)
{
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_win_path(name, PORT_STORAGE_READ, path))
        return PORT_INVALID_HANDLE;
    return host_FindFirstFileA(path, data);
}
int GetDiskFreeSpaceExA(const char *directory, void *available, void *total, void *free_bytes)
{
    char path[PORT_STORAGE_PATH];
    if (!port_resolve_win_path(directory != 0 ? directory : "d:\\", PORT_STORAGE_READ, path))
        return 0;
    return host_GetDiskFreeSpaceExA(path, available, total, free_bytes);
}
int GetQueuedCompletionStatus(void *port, port_dword *bytes, port_size *key, void **overlapped, port_dword milliseconds)
{
    return host_GetQueuedCompletionStatus(port, bytes, key, overlapped, milliseconds);
}

// Threads.
int ReleaseMutex(void *mutex) { return host_ReleaseMutex(mutex); }
port_dword ResumeThread(void *thread) { return host_ResumeThread(thread); }
int SetThreadPriority(void *thread, int priority) { return host_SetThreadPriority(thread, priority); }

typedef struct port_thread_processor {
    port_dword thread_id;
    unsigned long long created;
    port_dword processor;
} port_thread_processor;
#define PORT_THREAD_PROCESSOR_CAPACITY 256
static port_thread_processor port_thread_processors[PORT_THREAD_PROCESSOR_CAPACITY];
static unsigned int port_thread_processor_next;
static volatile char port_thread_processors_busy;

port_dword XSetThreadProcessor(void *thread, port_dword processor)
{
    port_dword thread_id = host_GetThreadId(thread);
    unsigned long long created, exit_time, kernel_time, user_time;
    if (thread_id == 0 || !host_GetThreadTimes(thread, &created, &exit_time, &kernel_time, &user_time))
        return 0xfffffffful;
    while (__atomic_test_and_set(&port_thread_processors_busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
    port_thread_processor *entry = 0;
    for (unsigned int i = 0; i < PORT_THREAD_PROCESSOR_CAPACITY && entry == 0; i++) {
        port_thread_processor *candidate = &port_thread_processors[i];
        if (candidate->thread_id == thread_id && candidate->created == created)
            entry = candidate;
    }
    port_dword previous = 0;
    if (entry != 0) {
        previous = entry->processor;
    } else {
        entry = &port_thread_processors[port_thread_processor_next++ % PORT_THREAD_PROCESSOR_CAPACITY];
        entry->thread_id = thread_id;
        entry->created = created;
    }
    entry->processor = processor;
    __atomic_clear(&port_thread_processors_busy, __ATOMIC_RELEASE);
    return previous;
}
void port_time_set_resolution(void); // time_windows.c: 1 ms timer resolution, as the Xbox kernel's waits have
void Sleep(port_dword milliseconds)
{
    port_time_set_resolution();
    host_Sleep(milliseconds);
}
int SwitchToThread(void) { return host_SwitchToThread(); }

// SuspendThread (formerly xdk_suspend_thread): handles are the host's.
port_dword SuspendThread(void *thread) { return host_SuspendThread(thread); }
port_dword xdk_suspend_thread(void *thread) { return SuspendThread(thread); }

static port_dword port_io_event_slot = 0xffffffffu;

static void __stdcall port_close_io_event(void *event)
{
    host_CloseHandle(event);
}

static int __stdcall port_init_io_event_slot(void **once, void *parameter, void **context)
{
    (void)once;
    (void)parameter;
    (void)context;
    __atomic_store_n(&port_io_event_slot, host_FlsAlloc(port_close_io_event), __ATOMIC_RELEASE);
    return 1;
}

// xapi_io.h: the transfer of ReadFileEx / WriteFileEx (xapi_io.c). A private OVERLAPPED carries the offset, so a
// handle opened with or without FILE_FLAG_OVERLAPPED works; its own event makes the wait independent of other I/O on
// the handle.
port_dword port_file_transfer_at(void *file, void *buffer, port_dword bytes, unsigned long long offset, int write,
    port_dword *done)
{
    static void *once;
    *done = 0;
    port_dword slot = __atomic_load_n(&port_io_event_slot, __ATOMIC_ACQUIRE);
    if (slot == 0xffffffffu) {
        host_InitOnceExecuteOnce(&once, port_init_io_event_slot, 0, 0);
        slot = __atomic_load_n(&port_io_event_slot, __ATOMIC_ACQUIRE);
    }
    void *ev = slot != 0xffffffffu ? host_FlsGetValue(slot) : 0;
    int close_event = 0;
    if (ev == 0) {
        ev = host_CreateEventA(0, 1, 0, 0);
        if (ev == 0)
            return host_GetLastError();
        close_event = slot == 0xffffffffu || !host_FlsSetValue(slot, ev);
    }
    struct {
        port_size internal, internal_high;
        port_dword offset, offset_high;
        void *event;
    } overlapped = {0, 0, (port_dword)offset, (port_dword)(offset >> 32), ev};
    unsigned char scratch[PORT_MAP_SWAP_HEADER_SIZE];
    const void *source = write ? port_map_swap_before_write(buffer, bytes, offset, scratch) : buffer;
    port_dword moved = 0, error = 0;
    int ok = write ? host_WriteFile(file, source, bytes, &moved, &overlapped)
                   : host_ReadFile(file, buffer, bytes, &moved, &overlapped);
    if (!ok) {
        error = host_GetLastError();
        // ReadFile/WriteFile resets the event for a pending request. Wait before reusing it.
        if (error == 997 /* ERROR_IO_PENDING */)
            error = host_GetOverlappedResult(file, &overlapped, &moved, 1) ? 0 : host_GetLastError();
    }
    if (close_event)
        host_CloseHandle(ev);
    if (error == 0 && !write && moved == 0 && bytes > 0)
        error = PORT_ERROR_HANDLE_EOF;
    if (error != 0)
        return error;
    if (!write)
        port_map_swap_after_read(buffer, bytes, moved, offset);
    *done = moved;
    return 0;
}

// port_runtime.h: advapi32's RtlGenRandom (exported as SystemFunction036), loaded on first use.
PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
int port_host_random(void *buffer, unsigned long size)
{
    typedef unsigned char(__stdcall * port_gen_random)(void *buffer, unsigned long size);
    static port_gen_random gen_random;
    if (gen_random == 0) {
        void *advapi = host_LoadLibraryA("advapi32.dll");
        gen_random = advapi != 0 ? (port_gen_random)host_GetProcAddress(advapi, "SystemFunction036") : 0;
    }
    if (gen_random == 0 || !gen_random(buffer, size)) {
        port_write_error("port: RtlGenRandom is unavailable: no secure random source\n");
        port_exit(7);
    }
    return 1;
}

#ifdef _WIN64
// Clamp host MEMORYSTATUS fields to 0x7fffffff (as kernel_linux.c does): the game assigns dwAvailPhys / dwTotalPhys
// into 32-bit signed longs (cseries_windows.cpp, bink_playback.cpp), which would wrap to 0 or negative on >= 4 GiB PCs.
typedef struct {
    port_dword dwLength;
    port_dword dwMemoryLoad;
    port_size dwTotalPhys;
    port_size dwAvailPhys;
    port_size dwTotalPageFile;
    port_size dwAvailPageFile;
    port_size dwTotalVirtual;
    port_size dwAvailVirtual;
} port_memory_status;
PORT_HOST_IMPORT(void, GlobalMemoryStatus, 4, (port_memory_status *status));

static port_size clamp_mem_31bit(port_size v)
{
    return v > 0x7fffffffu ? (port_size)0x7fffffffu : v;
}

void GlobalMemoryStatus(void *status)
{
    port_memory_status *s = (port_memory_status *)status;
    if (s == 0)
        return;
    host_GlobalMemoryStatus(s);
    s->dwTotalPhys = clamp_mem_31bit(s->dwTotalPhys);
    s->dwAvailPhys = clamp_mem_31bit(s->dwAvailPhys);
    s->dwTotalPageFile = clamp_mem_31bit(s->dwTotalPageFile);
    s->dwAvailPageFile = clamp_mem_31bit(s->dwAvailPageFile);
    s->dwTotalVirtual = clamp_mem_31bit(s->dwTotalVirtual);
    s->dwAvailVirtual = clamp_mem_31bit(s->dwAvailVirtual);
}
#endif
