// port/sdk/kernel (both hosts): Xbox-only xapilib calls with no Win32 counterpart that mean the same on every host.
//
// Physical memory protection. XPhysicalAlloc and XPhysicalProtect take Xbox protection values: an access part
// (PAGE_NOACCESS, PAGE_READONLY, PAGE_READWRITE, the PAGE_EXECUTE forms) and cache attributes (PAGE_NOCACHE,
// PAGE_WRITECOMBINE) that only the Xbox's memory controller understands. The access part is applied to the host
// pages. The whole value is recorded, so XQueryMemoryProtect answers what the game set, as it does on the Xbox (Bink,
// for one, warns when its frame buffers are write-combined). Memory the game did not get from XPhysicalAlloc reads
// as PAGE_READWRITE, which is what the Xbox reports for heap and stack pages.
//
// The file system cache. The Xbox reserves XSetFileCacheSize bytes for its own file cache; the host operating system
// caches files itself, so there is nothing to size.
#include "../runtime/port_runtime.h"

typedef unsigned int port_dword;
typedef unsigned long port_size;

#define PORT_PAGE_NOACCESS 0x01ul
#define PORT_PAGE_READONLY 0x02ul
#define PORT_PAGE_READWRITE 0x04ul
#define PORT_PAGE_EXECUTE 0x10ul
#define PORT_PAGE_EXECUTE_READ 0x20ul
// The page protection bits (PAGE_*, PAGE_NOCACHE 0x200, PAGE_WRITECOMBINE 0x400). XPhysicalAlloc's value may also
// carry MEM_* allocation flags (MEM_LARGE_PAGES, MEM_16MB_PAGES), which are not protection.
#define PORT_PAGE_PROTECTION_MASK 0x7fful

// Allocations (protect has no MEM_ flags) and the ranges XPhysicalProtect changed since, newest last.
typedef struct port_protected_range {
    unsigned char *address;
    port_size size;
    port_dword protect;
    int allocation;
} port_protected_range;

#define PORT_PROTECTED_CAPACITY 1024
static port_protected_range port_protected[PORT_PROTECTED_CAPACITY];
static int port_protected_count;
static volatile char port_protected_busy;

// Protection changes are rare (a handful at startup), so a spin lock is enough.
static void port_protected_lock(void)
{
    while (__atomic_test_and_set(&port_protected_busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void port_protected_unlock(void) { __atomic_clear(&port_protected_busy, __ATOMIC_RELEASE); }

static int port_access_of(port_dword protect)
{
    switch (protect & 0xff) {
    case PORT_PAGE_NOACCESS: return PORT_ACCESS_NONE;
    case PORT_PAGE_READONLY: case PORT_PAGE_EXECUTE: case PORT_PAGE_EXECUTE_READ: return PORT_ACCESS_READ;
    default: return PORT_ACCESS_READ_WRITE;
    }
}

// Under the lock.
static void port_protected_add(void *address, port_size size, port_dword protect, int allocation)
{
    unsigned char *start = (unsigned char *)address;
    for (int i = 0; i < port_protected_count; i++) {
        port_protected_range *range = &port_protected[i];
        if (!range->allocation && !allocation && range->address == start && range->size == size) {
            range->protect = protect; // the same range again: no new record
            return;
        }
    }
    if (port_protected_count == PORT_PROTECTED_CAPACITY) {
        port_write_error("port: too many physical memory protection ranges\n");
        port_exit(7);
    }
    port_protected[port_protected_count++] = (port_protected_range){start, size, protect, allocation};
}

void port_physical_allocated(void *address, unsigned long size, unsigned long protect)
{
    protect &= PORT_PAGE_PROTECTION_MASK;
    port_protected_lock();
    port_protected_add(address, size, protect, 1);
    port_protected_unlock();
    if (port_access_of(protect) != PORT_ACCESS_READ_WRITE)
        port_set_page_protection(address, size, port_access_of(protect));
}

// Forgets the allocation at address and every protection change inside it.
void port_physical_released(void *address)
{
    port_protected_lock();
    unsigned char *start = 0, *end = 0;
    for (int i = 0; i < port_protected_count; i++) {
        if (port_protected[i].allocation && port_protected[i].address == (unsigned char *)address) {
            start = port_protected[i].address;
            end = start + port_protected[i].size;
            break;
        }
    }
    int kept = 0;
    for (int i = 0; i < port_protected_count; i++) {
        unsigned char *where = port_protected[i].address;
        if (start != 0 && where >= start && where < end)
            continue;
        port_protected[kept++] = port_protected[i];
    }
    port_protected_count = kept;
    port_protected_unlock();
}

void PORT_MS_ABI XPhysicalProtect(void *address, port_size size, port_dword protect)
{
    protect &= PORT_PAGE_PROTECTION_MASK;
    port_protected_lock();
    port_protected_add(address, size, protect, 0);
    port_protected_unlock();
    port_set_page_protection(address, size, port_access_of(protect));
}

port_dword PORT_MS_ABI XQueryMemoryProtect(void *address)
{
    unsigned char *where = (unsigned char *)address;
    port_dword protect = PORT_PAGE_READWRITE;
    port_protected_lock();
    for (int i = port_protected_count - 1; i >= 0; i--) {
        const port_protected_range *range = &port_protected[i];
        if (where >= range->address && where < range->address + range->size) {
            protect = range->protect;
            break;
        }
    }
    port_protected_unlock();
    return protect;
}

int PORT_MS_ABI XSetFileCacheSize(port_size size)
{
    (void)size;
    return 1;
}
