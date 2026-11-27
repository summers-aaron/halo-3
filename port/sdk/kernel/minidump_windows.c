// port/sdk/kernel (Windows host): the thread, loaded-module and address-space queries of the game's minidump writer
// and tag resource thread lock (cseries/xenon_minidump.cpp, cache/tag_resource_cache_thread_lock.cpp). Retail answers
// them from the Xbox kernel (0x826f2c18..0x826f33f0: the process's thread list, the loader's module list, each
// module's PE section table, the page protections of the title's address ranges); here kernel32's thread and module
// snapshots, each module's PE headers, and VirtualQuery.
#include "win32_host.h"

typedef unsigned int port_dword;

#define PORT_TH32CS_SNAPTHREAD 0x4ul
#define PORT_TH32CS_SNAPMODULE 0x8ul
#define PORT_THREAD_ALL_ACCESS 0x1fffffu
#define PORT_INVALID_HANDLE ((void *)-1)

typedef struct port_thread_entry32 { // THREADENTRY32
    port_dword dwSize, cntUsage, th32ThreadID, th32OwnerProcessID;
    long tpBasePri, tpDeltaPri;
    port_dword dwFlags;
} port_thread_entry32;

typedef struct port_module_entry32 { // MODULEENTRY32 (ANSI)
    port_dword dwSize, th32ModuleID, th32ProcessID, GlblcntUsage, ProccntUsage;
    unsigned char *modBaseAddr;
    port_dword modBaseSize;
    void *hModule;
    char szModule[256];
    char szExePath[260];
} port_module_entry32;

typedef struct port_memory_basic_information { // MEMORY_BASIC_INFORMATION (32-bit and 64-bit)
    void *BaseAddress, *AllocationBase;
    port_dword AllocationProtect;
#ifdef _WIN64
    unsigned short PartitionId;
    unsigned short Alignment1;
#endif
    __SIZE_TYPE__ RegionSize;
    port_dword State, Protect, Type;
#ifdef _WIN64
    port_dword Alignment2;
#endif
} port_memory_basic_information;

PORT_HOST_IMPORT(void *, CreateToolhelp32Snapshot, 8, (port_dword flags, port_dword process_id));
PORT_HOST_IMPORT(int, Thread32First, 8, (void *snapshot, port_thread_entry32 *entry));
PORT_HOST_IMPORT(int, Thread32Next, 8, (void *snapshot, port_thread_entry32 *entry));
PORT_HOST_IMPORT(int, Module32First, 8, (void *snapshot, port_module_entry32 *entry));
PORT_HOST_IMPORT(int, Module32Next, 8, (void *snapshot, port_module_entry32 *entry));
PORT_HOST_IMPORT(int, CloseHandle, 4, (void *handle));
PORT_HOST_IMPORT(void *, OpenThread, 12, (port_dword access, int inherit, port_dword thread_id));
PORT_HOST_IMPORT(port_dword, GetCurrentThreadId, 0, (void));
PORT_HOST_IMPORT(port_dword, GetCurrentProcessId, 0, (void));
PORT_HOST_IMPORT(__SIZE_TYPE__, VirtualQuery, 12, (const void *address, port_memory_basic_information *information,
    __SIZE_TYPE__ size));

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

typedef struct port_minidump_module_info { // s_xenon_minidump_module_info, 280 bytes
    port_dword image_base, image_size, timestamp, checksum, flags; // flags bit 0: a DLL
    char filename[260];
} port_minidump_module_info;

typedef struct port_module_section { // the PE section record retail reads: address, size, name
    port_dword address, size;
    char name[8];
} port_module_section;

typedef struct port_memory_range {
    port_dword address, size, protection; // protection: 4 (PAGE_READWRITE) as retail reports
} port_memory_range;

static void port_zero(void *memory, port_dword size)
{
    for (port_dword i = 0; i < size; i++)
        ((unsigned char *)memory)[i] = 0;
}

// Bit 0 of flags: include the calling thread. Bit 1: opened handles (else thread ids).
void xenon_minidump_enumerate_thread_handles(port_dword flags, port_minidump_thread_list *list)
{
    list->count = 0;
    void *snapshot = host_CreateToolhelp32Snapshot(PORT_TH32CS_SNAPTHREAD, 0);
    if (snapshot == PORT_INVALID_HANDLE)
        return;
    const port_dword process = host_GetCurrentProcessId(), current = host_GetCurrentThreadId();
    port_thread_entry32 entry;
    entry.dwSize = sizeof(entry);
    for (int more = host_Thread32First(snapshot, &entry); more && list->count < 63; more = host_Thread32Next(snapshot, &entry)) {
        if (entry.th32OwnerProcessID != process || ((flags & 1) == 0 && entry.th32ThreadID == current))
            continue;
        void *value = (void *)(__UINTPTR_TYPE__)entry.th32ThreadID;
        if ((flags & 2) != 0 && (value = host_OpenThread(PORT_THREAD_ALL_ACCESS, 0, entry.th32ThreadID)) == 0)
            continue;
        list->handles[list->count++] = value;
    }
    host_CloseHandle(snapshot);
}

void *xenon_minidump_get_thread_handle(port_dword thread_id)
{
    return host_OpenThread(PORT_THREAD_ALL_ACCESS, 0, thread_id);
}

// What kernel32 tells without a debugger: the rest reads as zero.
void xenon_minidump_get_thread_information(void *thread, port_minidump_thread_information *information)
{
    (void)thread;
    port_zero(information, sizeof(*information));
}

// The Xbox CONTEXT (the PowerPC register file). A host thread has none: every field after ContextFlags reads as the
// retail call leaves the ones it cannot fill (all bits set), and the caller falls back to the thread's whole stack.
void xenon_minidump_get_thread_context(void *thread, void *context)
{
    (void)thread;
    for (port_dword i = 0; i < 2620; i++)
        ((unsigned char *)context)[4 + i] = 0xff;
}

static int port_module_find(int module, port_module_entry32 *out)
{
    if (module < 0)
        return 0;
    void *snapshot = host_CreateToolhelp32Snapshot(PORT_TH32CS_SNAPMODULE, 0);
    if (snapshot == PORT_INVALID_HANDLE)
        return 0;
    int found = 0, index = 0;
    out->dwSize = sizeof(*out);
    for (int more = host_Module32First(snapshot, out); more; more = host_Module32Next(snapshot, out))
        if (index++ == module) {
            found = 1;
            break;
        }
    host_CloseHandle(snapshot);
    return found;
}

long long xenon_minidump_get_loaded_module_count(void)
{
    void *snapshot = host_CreateToolhelp32Snapshot(PORT_TH32CS_SNAPMODULE, 0);
    if (snapshot == PORT_INVALID_HANDLE)
        return 0;
    long count = 0;
    port_module_entry32 entry;
    entry.dwSize = sizeof(entry);
    for (int more = host_Module32First(snapshot, &entry); more; more = host_Module32Next(snapshot, &entry))
        count++;
    host_CloseHandle(snapshot);
    return count;
}

// The module's PE headers: file header (NumberOfSections at +6, TimeDateStamp at +8, SizeOfOptionalHeader at +20,
// Characteristics at +22), optional header (CheckSum at +64), then the section table (40 bytes each).
static const unsigned char *port_module_nt_headers(const unsigned char *base)
{
    return base + *(const long *)(base + 0x3c) + 4;
}

int xenon_minidump_get_module_info(int module, port_minidump_module_info *info)
{
    port_zero(info, sizeof(*info));
    port_module_entry32 entry;
    if (!port_module_find(module, &entry))
        return 0;
    const unsigned char *file = port_module_nt_headers(entry.modBaseAddr);
    info->image_base = (port_dword)(__UINTPTR_TYPE__)entry.modBaseAddr;
    info->image_size = entry.modBaseSize;
    info->timestamp = *(const port_dword *)(file + 4);
    info->checksum = *(const port_dword *)(file + 20 + 64);
    info->flags = (*(const unsigned short *)(file + 18) & 0x2000) != 0 ? 1u : 0u;
    for (int i = 0; i < 259 && entry.szExePath[i] != '\0'; i++)
        info->filename[i] = entry.szExePath[i];
    return 1;
}

int tag_resource_cache_module_section_count(int module)
{
    port_module_entry32 entry;
    if (!port_module_find(module, &entry))
        return 0;
    return *(const unsigned short *)(port_module_nt_headers(entry.modBaseAddr) + 2);
}

int tag_resource_cache_module_get_section(int module, unsigned int section_index, port_module_section *section)
{
    port_module_entry32 entry;
    if (!port_module_find(module, &entry))
        return 0;
    const unsigned char *file = port_module_nt_headers(entry.modBaseAddr);
    if (section_index >= *(const unsigned short *)(file + 2))
        return 0;
    const unsigned char *record = file + 20 + *(const unsigned short *)(file + 16) + section_index * 40;
    section->address = (port_dword)(__UINTPTR_TYPE__)entry.modBaseAddr + *(const port_dword *)(record + 12);
    section->size = *(const port_dword *)(record + 8);
    for (int i = 0; i < 8; i++)
        section->name[i] = (char)record[i];
    return 1;
}

// The next readable committed range at or above `address` (a handle: 0 starts the walk): fills *range and returns
// where the walk continues (the range's end), or 0 when no readable memory is left in the 32-bit address space.
int tag_resource_cache_next_readable_memory_range(int address, port_memory_range *range)
{
    range->address = range->size = range->protection = 0;
    port_dword at = (port_dword)address;
    port_memory_basic_information information;
    while (host_VirtualQuery((const void *)(__UINTPTR_TYPE__)at, &information, sizeof(information)) == sizeof(information)) {
        const __UINTPTR_TYPE__ start64 = (__UINTPTR_TYPE__)information.BaseAddress;
        const __UINTPTR_TYPE__ end64 = start64 + information.RegionSize;
        if (start64 >= 0x100000000ull)
            break;
        const port_dword end = end64 >= 0x100000000ull ? 0u : (port_dword)end64;
        const port_dword protect = information.Protect & 0xffu;
        const int readable = information.State == 0x1000u && (information.Protect & 0x100u) == 0 && protect != 0x01u &&
            protect != 0x10u; // MEM_COMMIT, not PAGE_GUARD / PAGE_NOACCESS / PAGE_EXECUTE
        if (readable && end > at) {
            range->address = at;
            range->size = end - at;
            range->protection = 4;
            return (int)end;
        }
        if (end <= at)
            break;
        at = end;
    }
    return 0;
}
