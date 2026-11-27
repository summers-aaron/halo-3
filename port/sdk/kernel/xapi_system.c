// port/sdk/kernel (both hosts): xapilib calls with no Win32 counterpart, or whose Xbox meaning differs from the
// host's: the default XMemAlloc allocator, NT status errors, title modules, the dashboard, the screen saver, PIX
// counters, and the cache-line copy routines. Some reach the port under the game's own names for them (noted).
#include "../runtime/port_runtime.h"
#include "../audio/port_audio.h"

typedef unsigned int port_dword;
typedef __SIZE_TYPE__ port_size_t;

void *malloc(port_size_t size);
void free(void *memory);
void PORT_MS_ABI SetLastError(port_dword error);
void *PORT_MS_ABI XPhysicalAlloc(port_dword size, port_dword address, port_dword alignment, port_dword protect);
void PORT_MS_ABI XPhysicalFree(void *address);

// ---- XMemAllocDefault / XMemFreeDefault / XMemSizeDefault ----
//
// The allocation attributes are the Xbox's XALLOC_ATTRIBUTES word (MAKE_XALLOC_ATTRIBUTES, xbox.h): object type
// bits 0-12, heap-tracks-attributes 13, must-succeed 14, fixed-size 15, allocator id 16-23, alignment 24-27, memory
// protection 28-29, zero-initialize 30, memory type 31 (heap 0, physical 1). The game passes these values as they
// are (0x20824000 for XAudio's heap), so they are decoded by value, never through a bit-field.
#define PORT_XALLOC_PHYSICAL(attributes) (((attributes) >> 31) & 1ul)
#define PORT_XALLOC_ZERO(attributes) (((attributes) >> 30) & 1ul)
#define PORT_XALLOC_PROTECT(attributes) (((attributes) >> 28) & 3ul)
#define PORT_XALLOC_ALIGNMENT(attributes) (((attributes) >> 24) & 15ul)

#define PORT_PAGE_READWRITE 0x04ul
#define PORT_PAGE_NOCACHE 0x200ul
#define PORT_PAGE_WRITECOMBINE 0x400ul

// Heap blocks carry their size and the start of the host block just below the aligned address.
typedef struct port_xmem_header {
    void *block;
    port_size_t size;
} port_xmem_header;

// Physical blocks: their sizes, for XMemSizeDefault (XPhysicalAlloc keeps none the game can ask for).
#define PORT_XMEM_PHYSICAL_CAPACITY 4096
static struct { void *address; port_size_t size; } port_xmem_physical[PORT_XMEM_PHYSICAL_CAPACITY];
static volatile char port_xmem_busy;
static void port_xmem_lock(void)
{
    while (__atomic_test_and_set(&port_xmem_busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void port_xmem_unlock(void) { __atomic_clear(&port_xmem_busy, __ATOMIC_RELEASE); }

void *PORT_MS_ABI XMemAllocDefault(port_size_t size, port_dword attributes)
{
    const port_dword alignment_code = PORT_XALLOC_ALIGNMENT(attributes);
    void *memory;
    if (PORT_XALLOC_PHYSICAL(attributes)) {
        // XALLOC_PHYSICAL_ALIGNMENT_n: 1 << code (4 KB by default); XALLOC_MEMPROTECT_*: write-combined (large pages
        // or not), uncached, or plain read-write memory, recorded for XQueryMemoryProtect as the Xbox would.
        const port_dword alignment = alignment_code == 0 ? 4096ul : 1ul << alignment_code;
        static const port_dword k_protect[4] = {
            PORT_PAGE_READWRITE | PORT_PAGE_WRITECOMBINE, PORT_PAGE_READWRITE | PORT_PAGE_NOCACHE, PORT_PAGE_READWRITE,
            PORT_PAGE_READWRITE | PORT_PAGE_WRITECOMBINE,
        };
        memory = XPhysicalAlloc(size != 0 ? size : 1, ~0u, alignment, k_protect[PORT_XALLOC_PROTECT(attributes)]);
        if (memory == 0)
            return 0;
        port_xmem_lock();
        for (int i = 0; i < PORT_XMEM_PHYSICAL_CAPACITY; i++) {
            if (port_xmem_physical[i].address == 0) {
                port_xmem_physical[i].address = memory;
                port_xmem_physical[i].size = size;
                break;
            }
        }
        port_xmem_unlock();
    } else {
        // XALLOC_ALIGNMENT_4 / 8 / 16 are codes 1 / 2 / 4 (4 bytes each); the default is 16, the strictest the game's
        // heap data needs (vectors).
        port_size_t alignment = alignment_code == 0 ? 16u : alignment_code * 4u;
        if (alignment < sizeof(void *))
            alignment = sizeof(void *);
        unsigned char *block = (unsigned char *)malloc(size + alignment + sizeof(port_xmem_header));
        if (block == 0)
            return 0;
        port_size_t start = ((port_size_t)block + sizeof(port_xmem_header) + alignment - 1) & ~(alignment - 1);
        port_xmem_header *header = (port_xmem_header *)start - 1;
        header->block = block;
        header->size = size;
        memory = (void *)start;
    }
    if (PORT_XALLOC_ZERO(attributes))
        __builtin_memset(memory, 0, size);
    return memory;
}

void PORT_MS_ABI XMemFreeDefault(void *address, port_dword attributes)
{
    if (address == 0)
        return;
    if (PORT_XALLOC_PHYSICAL(attributes)) {
        port_xmem_lock();
        for (int i = 0; i < PORT_XMEM_PHYSICAL_CAPACITY; i++) {
            if (port_xmem_physical[i].address == address) {
                port_xmem_physical[i].address = 0;
                break;
            }
        }
        port_xmem_unlock();
        XPhysicalFree(address);
        return;
    }
    free(((port_xmem_header *)address - 1)->block);
}

// XMemSizeDefault (0x582760, after XMemFreeDefault in xmem.obj; the game calls it
// sound_xaudio_waves_allocation_size).
static port_size_t port_xmem_size(const void *address, port_dword attributes)
{
    if (address == 0)
        return 0;
    if (!PORT_XALLOC_PHYSICAL(attributes))
        return ((const port_xmem_header *)address - 1)->size;
    port_size_t size = 0;
    port_xmem_lock();
    for (int i = 0; i < PORT_XMEM_PHYSICAL_CAPACITY; i++) {
        if (port_xmem_physical[i].address == address) {
            size = port_xmem_physical[i].size;
            break;
        }
    }
    port_xmem_unlock();
    return size;
}

port_size_t PORT_MS_ABI XMemSizeDefault(void *address, port_dword attributes)
{
    return port_xmem_size(address, attributes);
}

int PORT_MS_ABI sound_xaudio_waves_allocation_size(const void *address, port_dword attributes)
{
    return (int)port_xmem_size(address, attributes);
}

// ---- NT status codes ----

// RtlNtStatusToDosError for the statuses the Xbox kernel reports to titles; others read as
// ERROR_MR_MID_NOT_FOUND, as Windows answers for a status it has no Win32 error for.
port_dword port_nt_status_to_win32(long status)
{
    static const struct { port_dword status, error; } k_map[] = {
        {0x00000000ul, 0},     // STATUS_SUCCESS
        {0x00000102ul, 258},   // STATUS_TIMEOUT: WAIT_TIMEOUT
        {0x00000103ul, 997},   // STATUS_PENDING: ERROR_IO_PENDING
        {0x80000005ul, 234},   // STATUS_BUFFER_OVERFLOW: ERROR_MORE_DATA
        {0x80000006ul, 18},    // STATUS_NO_MORE_FILES
        {0xc0000002ul, 1},     // STATUS_NOT_IMPLEMENTED: ERROR_INVALID_FUNCTION
        {0xc0000008ul, 6},     // STATUS_INVALID_HANDLE
        {0xc000000dul, 87},    // STATUS_INVALID_PARAMETER
        {0xc000000ful, 2},     // STATUS_NO_SUCH_FILE: ERROR_FILE_NOT_FOUND
        {0xc0000011ul, 38},    // STATUS_END_OF_FILE: ERROR_HANDLE_EOF
        {0xc0000017ul, 8},     // STATUS_NO_MEMORY: ERROR_NOT_ENOUGH_MEMORY
        {0xc0000022ul, 5},     // STATUS_ACCESS_DENIED
        {0xc0000023ul, 122},   // STATUS_BUFFER_TOO_SMALL: ERROR_INSUFFICIENT_BUFFER
        {0xc0000033ul, 123},   // STATUS_OBJECT_NAME_INVALID: ERROR_INVALID_NAME
        {0xc0000034ul, 2},     // STATUS_OBJECT_NAME_NOT_FOUND: ERROR_FILE_NOT_FOUND
        {0xc0000035ul, 183},   // STATUS_OBJECT_NAME_COLLISION: ERROR_ALREADY_EXISTS
        {0xc000003aul, 3},     // STATUS_OBJECT_PATH_NOT_FOUND: ERROR_PATH_NOT_FOUND
        {0xc0000043ul, 32},    // STATUS_SHARING_VIOLATION
        {0xc000007ful, 112},   // STATUS_DISK_FULL
        {0xc000009aul, 1450},  // STATUS_INSUFFICIENT_RESOURCES: ERROR_NO_SYSTEM_RESOURCES
        {0xc000009dul, 1167},  // STATUS_DEVICE_NOT_CONNECTED
        {0xc00000baul, 5},     // STATUS_FILE_IS_A_DIRECTORY: ERROR_ACCESS_DENIED
        {0xc0000101ul, 145},   // STATUS_DIRECTORY_NOT_EMPTY: ERROR_DIR_NOT_EMPTY
        {0xc0000103ul, 267},   // STATUS_NOT_A_DIRECTORY: ERROR_DIRECTORY
        {0xc0000120ul, 995},   // STATUS_CANCELLED: ERROR_OPERATION_ABORTED
    };
    for (unsigned int i = 0; i < sizeof(k_map) / sizeof(k_map[0]); i++) {
        if (k_map[i].status == (port_dword)status)
            return k_map[i].error;
    }
    return 317; // ERROR_MR_MID_NOT_FOUND
}

void PORT_MS_ABI XapiSetLastNTError(long status)
{
    SetLastError(port_nt_status_to_win32(status));
}

// ---- title modules ----
//
// LoadLibraryA / GetProcAddress load the Xbox title's own DLL modules (XEX images: the Waves audio DSP libraries
// sound_xaudio_xenon.cpp imports by ordinal). Those are PowerPC code; the port has a host stand-in for each
// (port/sdk/audio/waves_bypass.c: the Q10 / L360 DSP run from the retail presets, the retail versions). Any other
// module does not exist on a PC: the load fails as for a missing file.
void *PORT_MS_ABI LoadLibraryA(const char *name)
{
    void *module = port_waves_load_library(name);
    if (module == 0)
        SetLastError(126); // ERROR_MOD_NOT_FOUND
    return module;
}

void *PORT_MS_ABI GetProcAddress(void *module, const char *name)
{
    void *procedure = port_waves_get_procedure(module, name);
    if (procedure == 0)
        SetLastError(127); // ERROR_PROC_NOT_FOUND
    return procedure;
}

// ---- the dashboard, the screen saver, PIX ----

// XLaunchNewImage: another title or, with no path, the dashboard. The PC has neither; the game asks for it to leave
// (the dashboard from the main menu, or to accept a cross-title invite), so the process ends.
void PORT_MS_ABI XLaunchNewImage(const char *path, port_dword flags)
{
    (void)flags;
    port_write_error(path != 0 && path[0] != 0 ? "port: XLaunchNewImage of another title; exiting\n"
                                               : "port: XLaunchNewImage of the dashboard; exiting\n");
    port_exit_flush();
    port_run_atexit();
    port_exit(0);
}

// XEnableScreenSaver (formerly xbox_enable_screen_saver): the dashboard dims the screen after a while without input
// unless a title turns that off (cutscenes, films). Hosts manage their own screen savers; the setting is kept and
// the previous one returned.
static int port_screen_saver_enabled = 1;
port_dword PORT_MS_ABI XEnableScreenSaver(int enable)
{
    int previous = __atomic_exchange_n(&port_screen_saver_enabled, enable != 0, __ATOMIC_ACQ_REL);
    return (port_dword)previous;
}

port_dword PORT_MS_ABI xbox_enable_screen_saver(int enable)
{
    return XEnableScreenSaver(enable);
}

// pix_add_named_counter (xapilib pix.obj): a value plotted by an attached PIX capture. There is no PIX on a PC.
void PORT_MS_ABI pix_add_named_counter(float value, const char *name, ...)
{
    (void)value; (void)name;
}

// ---- cache-line copies ----
//
// The two 128-byte block copy loops of xapilib's copycachelines.obj that restricted_memory_copy picks between by
// source alignment (the game's names: restricted_memory_copy_aligned_blocks / _unaligned_blocks). They copy
// `size` bytes, a multiple of 128, between buffers that do not overlap.
#if defined(__linux__)
#include <linux/futex.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

#define PORT_MIRROR_COPY_MAX_HELPERS 7
#define PORT_MIRROR_COPY_MIN_BYTES (256u * 1024u)

static struct {
    volatile int init_state; // 0 = uninit, 1 = initing, 2 = ready
    int num_helpers;
    volatile int active;
    uint32_t gen;
    uint32_t remaining;
    uint32_t sleepers;
    unsigned char *dst;
    const unsigned char *src;
    unsigned int size;
} g_mirror_copy;

static void *mirror_copy_worker(void *arg)
{
    const int id = (int)(intptr_t)arg; // 1 .. num_helpers
    uint32_t seen = 0;
    for (;;) {
        uint32_t gen = __atomic_load_n(&g_mirror_copy.gen, __ATOMIC_ACQUIRE);
        if (gen == seen) {
            for (int spin = 0; spin < 256 && gen == seen; ++spin) {
                __builtin_ia32_pause();
                gen = __atomic_load_n(&g_mirror_copy.gen, __ATOMIC_ACQUIRE);
            }
            if (gen == seen) {
                __atomic_fetch_add(&g_mirror_copy.sleepers, 1u, __ATOMIC_SEQ_CST);
                while ((gen = __atomic_load_n(&g_mirror_copy.gen, __ATOMIC_SEQ_CST)) == seen)
                    syscall(SYS_futex, &g_mirror_copy.gen, FUTEX_WAIT_PRIVATE, seen, NULL, NULL, 0);
                __atomic_fetch_sub(&g_mirror_copy.sleepers, 1u, __ATOMIC_RELAXED);
            }
        }
        seen = gen;
        const unsigned int total = g_mirror_copy.size;
        const unsigned int total_threads = (unsigned int)(g_mirror_copy.num_helpers + 1);
        const unsigned int chunk = ((total / total_threads) + 127u) & ~127u;
        const unsigned int start = chunk * (unsigned int)id;
        if (start < total) {
            unsigned int len = total - start;
            if (id < g_mirror_copy.num_helpers && len > chunk)
                len = chunk;
            __builtin_memcpy(g_mirror_copy.dst + start, g_mirror_copy.src + start, len);
        }
        __atomic_fetch_sub(&g_mirror_copy.remaining, 1u, __ATOMIC_RELEASE);
    }
    return NULL;
}

static void mirror_copy_init(void)
{
    int expected = 0;
    if (!__atomic_compare_exchange_n(&g_mirror_copy.init_state, &expected, 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        while (__atomic_load_n(&g_mirror_copy.init_state, __ATOMIC_ACQUIRE) != 2)
            __builtin_ia32_pause();
        return;
    }
    int threads = 4;
    const char *env = getenv("HALO3_MIRROR_COPY_THREADS");
    if (env && env[0]) {
        int v = atoi(env);
        if (v < 1)
            v = 1;
        if (v > PORT_MIRROR_COPY_MAX_HELPERS + 1)
            v = PORT_MIRROR_COPY_MAX_HELPERS + 1;
        threads = v;
    }
    int helpers = threads - 1;
    int started = 0;
    for (int i = 0; i < helpers; ++i) {
        pthread_t th;
        if (pthread_create(&th, NULL, mirror_copy_worker, (void *)(intptr_t)(i + 1)) == 0) {
            pthread_detach(th);
            ++started;
        }
    }
    g_mirror_copy.num_helpers = started;
    __atomic_store_n(&g_mirror_copy.init_state, 2, __ATOMIC_RELEASE);
}

static void restricted_memory_copy_parallel(void *destination, const void *source, unsigned int size)
{
    if (size < PORT_MIRROR_COPY_MIN_BYTES) {
        __builtin_memcpy(destination, source, size);
        return;
    }
    if (__builtin_expect(__atomic_load_n(&g_mirror_copy.init_state, __ATOMIC_ACQUIRE) != 2, 0))
        mirror_copy_init();
    const int helpers = g_mirror_copy.num_helpers;
    if (helpers <= 0 || __atomic_exchange_n(&g_mirror_copy.active, 1, __ATOMIC_ACQUIRE) != 0) {
        __builtin_memcpy(destination, source, size);
        return;
    }
    const unsigned int total_threads = (unsigned int)(helpers + 1);
    const unsigned int chunk = ((size / total_threads) + 127u) & ~127u;
    g_mirror_copy.dst = (unsigned char *)destination;
    g_mirror_copy.src = (const unsigned char *)source;
    g_mirror_copy.size = size;
    __atomic_store_n(&g_mirror_copy.remaining, (uint32_t)helpers, __ATOMIC_RELEASE);
    __atomic_fetch_add(&g_mirror_copy.gen, 1u, __ATOMIC_SEQ_CST);
    if (__atomic_load_n(&g_mirror_copy.sleepers, __ATOMIC_SEQ_CST) != 0)
        syscall(SYS_futex, &g_mirror_copy.gen, FUTEX_WAKE_PRIVATE, helpers, NULL, NULL, 0);

    const unsigned int first_len = chunk < size ? chunk : size;
    __builtin_memcpy(destination, source, first_len);

    while (__atomic_load_n(&g_mirror_copy.remaining, __ATOMIC_ACQUIRE) != 0)
        __builtin_ia32_pause();
    __atomic_store_n(&g_mirror_copy.active, 0, __ATOMIC_RELEASE);
}
#endif

int PORT_MS_ABI restricted_memory_copy_aligned_blocks(void *destination, const void *source, unsigned int size)
{
#if defined(__linux__)
    restricted_memory_copy_parallel(destination, source, size);
#else
    __builtin_memcpy(destination, source, size);
#endif
    return 0;
}

int PORT_MS_ABI restricted_memory_copy_unaligned_blocks(void *destination, const void *source, unsigned int size)
{
#if defined(__linux__)
    restricted_memory_copy_parallel(destination, source, size);
#else
    __builtin_memcpy(destination, source, size);
#endif
    return 0;
}
