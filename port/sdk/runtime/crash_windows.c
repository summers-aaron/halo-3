// port/sdk/runtime (Windows host): reports for headless runs. Resolve the addresses with BUILD_DIR/stage1/halo3.map;
// the stage-1 build keeps frame pointers.
//   poison    an access fault on a poisoned datum (stub_report.c) is reported by name, not as a crash;
//   crash     on a fault, prints the exception code and address and the return addresses along the frame-pointer
//             chain, the registers, and the return addresses found on the stack (words pointing just past a call
//             instruction in the executable, as exe+RVA so ASLR does not matter: add the map's image base), then
//             exits with 6. The frame chain is garbage after a call through a bad pointer; the stack scan is not;
//   watchdog  PORT_WATCHDOG=SECONDS: after that long, samples the main thread five times, 200 ms apart (where it
//             is and how it got there), then exits with 8. Tells a busy loop from a wait.
//   watch     PORT_WATCH=RVA[,RVA...] (hex, up to 4, executable-relative as the crash report prints them): hardware
//             write watchpoints (debug registers) on 4 bytes at each, set on the main thread at startup. Each write
//             reports the new value and the writer's return addresses, then the run goes on. Finds who overwrites a
//             datum (the reports name the instruction after the write).
#include "port_runtime.h"
#include "../kernel/win32_host.h"

typedef long(__stdcall *port_exception_handler)(void *exception_pointers);
typedef unsigned long(__stdcall *port_thread_start)(void *parameter);
typedef __UINTPTR_TYPE__ port_addr;
PORT_HOST_IMPORT(void *, AddVectoredExceptionHandler, 8, (unsigned long first, port_exception_handler handler));
PORT_HOST_IMPORT(int, IsBadReadPtr, 8, (const void *address, __SIZE_TYPE__ size));
PORT_HOST_IMPORT(void *, CreateThread, 24, (void *security, __SIZE_TYPE__ stack, port_thread_start start,
    void *parameter, unsigned long flags, unsigned long *thread_id));
PORT_HOST_IMPORT(void *, GetCurrentThread, 0, (void));
PORT_HOST_IMPORT(void *, GetCurrentProcess, 0, (void));
PORT_HOST_IMPORT(int, DuplicateHandle, 28, (void *source_process, void *source, void *target_process, void **target,
    unsigned long access, int inherit, unsigned long options));
PORT_HOST_IMPORT(unsigned long, SuspendThread, 4, (void *thread));
PORT_HOST_IMPORT(unsigned long, ResumeThread, 4, (void *thread));
PORT_HOST_IMPORT(int, GetThreadContext, 8, (void *thread, void *context));
PORT_HOST_IMPORT(void, Sleep, 4, (unsigned long milliseconds));
PORT_HOST_IMPORT(int, VirtualProtect, 16, (void *address, __SIZE_TYPE__ size, unsigned long protect,
    unsigned long *old_protect));
PORT_HOST_IMPORT(void *, GetModuleHandleA, 4, (const char *name));
PORT_HOST_IMPORT(int, SetThreadContext, 8, (void *thread, const void *context));
PORT_HOST_IMPORT(unsigned long, WaitForSingleObject, 8, (void *handle, unsigned long milliseconds));

__declspec(dllimport) char *__cdecl getenv(const char *name);
__declspec(dllimport) int __cdecl atoi(const char *text);

#ifdef _WIN64
// AMD64 CONTEXT: 0x4d0 bytes; ContextFlags at 0x30, Dr0..Dr3 at 0x48..0x60, Dr6 at 0x68, Dr7 at 0x70,
// Rax..Rdi at 0x78..0xb0 (Rsp=0x98, Rbp=0xa0), Rip at 0xf8.
#define PORT_CONTEXT_SIZE 0x4d0
#define PORT_CONTEXT_FLAGS_OFF 0x30
#define PORT_CONTEXT_CONTROL 0x100001ul
#define PORT_CONTEXT_DEBUG_REGISTERS 0x100010ul
#define PORT_CONTEXT_DR0 0x48
#define PORT_CONTEXT_DR6 0x68
#define PORT_CONTEXT_DR7 0x70
#define PORT_CONTEXT_RAX 0x78
#define PORT_CONTEXT_RSP 0x98
#define PORT_CONTEXT_RBP 0xa0
#define PORT_CONTEXT_RIP 0xf8
#define PORT_EXCREC_INFO_OFF 0x20
#else
// x86 CONTEXT: 0x2cc bytes; ContextFlags at 0, Edi..Eax at 0x9c..0xb0, Ebp at 0xb4, Eip at 0xb8, Esp at 0xc4.
#define PORT_CONTEXT_SIZE 0x2cc
#define PORT_CONTEXT_FLAGS_OFF 0x00
#define PORT_CONTEXT_CONTROL 0x10001ul
#define PORT_CONTEXT_DEBUG_REGISTERS 0x10010ul
#define PORT_CONTEXT_DR0 0x04 // Dr0..Dr3 at 0x04..0x10, Dr6 at 0x14, Dr7 at 0x18
#define PORT_CONTEXT_DR6 0x14
#define PORT_CONTEXT_DR7 0x18
#define PORT_CONTEXT_EDI 0x9c
#define PORT_CONTEXT_RBP 0xb4
#define PORT_CONTEXT_RIP 0xb8
#define PORT_CONTEXT_RSP 0xc4
#define PORT_EXCREC_INFO_OFF 0x14
#endif

static void port_write_hex(port_addr value)
{
#ifdef _WIN64
    char text[19] = "0x0000000000000000";
    for (int i = 17; i >= 2; i--, value >>= 4)
        text[i] = "0123456789abcdef"[value & 15];
#else
    char text[11] = "0x00000000";
    for (int i = 9; i >= 2; i--, value >>= 4)
        text[i] = "0123456789abcdef"[value & 15];
#endif
    port_write_error(text);
}

static void port_write_frames(const unsigned char *context)
{
    port_write_error("port: at ");
    port_write_hex(*(const port_addr *)(context + PORT_CONTEXT_RIP));
    port_write_error(", frames:");
    port_addr *frame = *(port_addr *const *)(context + PORT_CONTEXT_RBP);
    for (int depth = 0; depth < 24 && frame != 0 && !host_IsBadReadPtr(frame, sizeof(port_addr) * 2); depth++) {
        port_write_error(" ");
        port_write_hex(frame[1]);
        frame = (port_addr *)frame[0];
    }
    port_write_error("\n");
}

// The executable's mapped image: [base, base + SizeOfImage) from its PE header.
static const unsigned char *port_image_base;
static unsigned long port_image_size;

static void port_find_image(void)
{
    const unsigned char *base = (const unsigned char *)host_GetModuleHandleA(0);
    if (base == 0 || base[0] != 'M' || base[1] != 'Z')
        return;
    const unsigned char *pe = base + *(const unsigned long *)(base + 0x3c);
    if (pe[0] != 'P' || pe[1] != 'E' || pe[2] != 0 || pe[3] != 0)
        return;
    port_image_base = base;
    port_image_size = *(const unsigned long *)(pe + 24 + 56); // OptionalHeader.SizeOfImage
}

static int port_in_image(port_addr address)
{
    return port_image_base != 0 && address - (port_addr)port_image_base < port_image_size;
}

static void port_write_image_address(port_addr address)
{
    if (port_in_image(address)) {
        port_write_error("exe+");
        port_write_hex(address - (port_addr)port_image_base);
    } else {
        port_write_hex(address);
    }
}

// Whether the bytes before `address` end in a call instruction (so `address` is a return address, not a stale word):
// call rel32 (E8), or call r/m (FF /2) through a register, [reg], [reg+disp8], [reg+disp32] or [disp32], with or
// without a SIB byte.
static int port_follows_call(port_addr address)
{
    if (!port_in_image(address) || address - (port_addr)port_image_base < 8)
        return 0;
    const unsigned char *at = (const unsigned char *)address;
    if (at[-5] == 0xe8)
        return 1;
    for (int length = 2; length <= 7; length++) {
        const unsigned char *op = at - length;
        if (op[0] != 0xff || ((op[1] >> 3) & 7) != 2)
            continue;
        const int mod = op[1] >> 6, rm = op[1] & 7, sib = mod != 3 && rm == 4;
        int expected = 2 + sib;
        if (mod == 1)
            expected += 1;
        else if (mod == 2 || (mod == 0 && rm == 5) || (mod == 0 && sib && (op[2] & 7) == 5))
            expected += 4;
        if (expected == length)
            return 1;
    }
    return 0;
}

// The registers, and the return addresses on the stack: after a call through a bad pointer the frame chain may be
// garbage, but the calls that led here each left their return address on the stack, just past a call instruction.
static void port_write_registers_and_stack(const unsigned char *context)
{
    port_write_error("port: image ");
    port_write_hex((port_addr)port_image_base);
    port_write_error(" (size ");
    port_write_hex(port_image_size);
    port_write_error(")\nport: registers:");
#ifdef _WIN64
    static const char *const names[8] = { "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi" };
    for (int i = 0; i < 8; i++) {
        port_write_error(" ");
        port_write_error(names[i]);
        port_write_error("=");
        port_write_image_address(*(const port_addr *)(context + PORT_CONTEXT_RAX + sizeof(port_addr) * (unsigned)i));
    }
#else
    static const char *const names[6] = { "edi", "esi", "ebx", "edx", "ecx", "eax" };
    for (int i = 0; i < 6; i++) {
        port_write_error(" ");
        port_write_error(names[i]);
        port_write_error("=");
        port_write_image_address(*(const port_addr *)(context + PORT_CONTEXT_EDI + 4 * i));
    }
    port_write_error(" ebp=");
    port_write_image_address(*(const port_addr *)(context + PORT_CONTEXT_RBP));
    port_write_error(" esp=");
    port_write_hex(*(const port_addr *)(context + PORT_CONTEXT_RSP));
#endif
    const port_addr *stack = *(port_addr *const *)(context + PORT_CONTEXT_RSP);
    port_write_error("\nport: stack top:");
    for (int i = 0; i < 8 && !host_IsBadReadPtr(stack + i, sizeof(port_addr)); i++) {
        port_write_error(" ");
        port_write_image_address(stack[i]);
    }
    port_write_error("\nport: return addresses on the stack (sp+offset: exe+RVA):");
    int found = 0;
    for (int i = 0; i < 1024 && found < 32 && !host_IsBadReadPtr(stack + i, sizeof(port_addr)); i++) {
        if (!port_follows_call(stack[i]))
            continue;
        port_write_error(found % 4 == 0 ? "\nport:  " : "  ");
        port_write_error("+");
        port_write_hex((port_addr)(sizeof(port_addr) * (unsigned)i));
        port_write_error(": ");
        port_write_image_address(stack[i]);
        found++;
    }
    port_write_error(found ? "\n" : " none\n");
}

// PORT_WATCH: the watched addresses (absolute), and how many hits are still reported.
static port_addr port_watch_addresses[4];
static int port_watch_count;
static int port_watch_reports_left = 64;

// A watchpoint hit (EXCEPTION_SINGLE_STEP after the write, DR6 naming the register): what was written, by whom.
static void port_write_watch_hit(unsigned char *context)
{
    const port_addr dr6 = *(const port_addr *)(context + PORT_CONTEXT_DR6);
    for (int i = 0; i < port_watch_count; i++) {
        if (!(dr6 & ((port_addr)1u << i)))
            continue;
        const port_addr address = port_watch_addresses[i];
        port_write_error("port: watch: ");
        port_write_image_address(address);
        port_write_error(" = ");
        port_write_image_address(*(const port_addr *)address);
        port_write_error(", written before ");
        port_write_image_address(*(const port_addr *)(context + PORT_CONTEXT_RIP));
        port_write_error("; return addresses:");
        const port_addr *stack = *(port_addr *const *)(context + PORT_CONTEXT_RSP);
        int found = 0;
        for (int j = 0; j < 1024 && found < 10 && !host_IsBadReadPtr(stack + j, sizeof(port_addr)); j++) {
            if (!port_follows_call(stack[j]))
                continue;
            port_write_error(" ");
            port_write_image_address(stack[j]);
            found++;
        }
        port_write_error("\n");
    }
}

static volatile long port_crash_reporting; // a crash report is being written

static long __stdcall port_crash_handler(void *exception_pointers)
{
    // EXCEPTION_POINTERS { EXCEPTION_RECORD *, CONTEXT * }.
    unsigned char *record = ((unsigned char **)exception_pointers)[0];
    unsigned char *context = ((unsigned char **)exception_pointers)[1];
    unsigned long code = *(const unsigned long *)record;
    const port_addr *info = (const port_addr *)(record + PORT_EXCREC_INFO_OFF);
    if (code == 0x80000004 /* EXCEPTION_SINGLE_STEP */ && port_watch_count > 0 && !port_crash_reporting &&
        (*(const port_addr *)(context + PORT_CONTEXT_DR6) & 0xf) != 0) {
        if (port_watch_reports_left > 0) {
            port_watch_reports_left--;
            port_crash_reporting = 1; // (IsBadReadPtr's probes while the report is written are left to it)
            port_write_watch_hit(context);
            port_crash_reporting = 0;
        }
        *(port_addr *)(context + PORT_CONTEXT_DR6) = 0;
        return -1; // EXCEPTION_CONTINUE_EXECUTION: the write is done, the run goes on
    }
    if (code == 0x406d1388 /* thread name */ || code == 0x40010006 /* debug string */ || (code & 0xf0000000) != 0xc0000000)
        return 0; // EXCEPTION_CONTINUE_SEARCH
    // A fault while the report is written is IsBadReadPtr probing (a vectored handler sees it before IsBadReadPtr's
    // own handler does): left to it, or the report faults into itself over and over.
    if (port_crash_reporting)
        return 0;
    if (code == 0xc0000005 && port_poisoned_access((const void *)info[1]))
        return -1; // EXCEPTION_CONTINUE_EXECUTION: the datum is accessible now
    port_crash_reporting = 1;
    port_write_error("port: exception ");
    port_write_hex(code);
    if (code == 0xc0000005) {
        port_write_error(info[0] ? " writing " : " reading ");
        port_write_hex(info[1]);
    }
    port_write_error("\n");
    port_write_frames(context);
    port_write_registers_and_stack(context);
    port_exit(6);
    return 0;
}

static void *port_main_thread;
static unsigned long port_watchdog_seconds;

static unsigned long __stdcall port_watchdog(void *parameter)
{
    (void)parameter;
    host_Sleep(port_watchdog_seconds * 1000);
    port_write_error("port: watchdog: main thread samples\n");
    for (int sample = 0; sample < 5; sample++) {
        __attribute__((aligned(16))) unsigned char context[PORT_CONTEXT_SIZE] = {0};
        *(unsigned long *)(context + PORT_CONTEXT_FLAGS_OFF) = PORT_CONTEXT_CONTROL;
        if (host_SuspendThread(port_main_thread) != ~0ul) {
            if (host_GetThreadContext(port_main_thread, context))
                port_write_frames(context);
            host_ResumeThread(port_main_thread);
        }
        host_Sleep(200);
    }
    port_exit(8);
    return 0;
}

// PORT_WATCH: a thread's own debug registers are set from another thread, with the target suspended.
static unsigned long __stdcall port_watch_arm(void *thread)
{
    __attribute__((aligned(16))) unsigned char context[PORT_CONTEXT_SIZE] = {0};
    *(unsigned long *)(context + PORT_CONTEXT_FLAGS_OFF) = PORT_CONTEXT_DEBUG_REGISTERS;
    port_addr dr7 = 0;
    for (int i = 0; i < port_watch_count; i++) {
        *(port_addr *)(context + PORT_CONTEXT_DR0 + sizeof(port_addr) * (unsigned)i) = port_watch_addresses[i];
        dr7 |= ((port_addr)1u << (2 * i)) | ((port_addr)1u << (16 + 4 * i)) | ((port_addr)3u << (18 + 4 * i)); // local enable, write, 4 bytes
    }
    *(port_addr *)(context + PORT_CONTEXT_DR7) = dr7;
    if (host_SuspendThread(thread) == ~0ul) {
        port_write_error("port: watch: cannot suspend the main thread\n");
        return 1;
    }
    if (!host_SetThreadContext(thread, context))
        port_write_error("port: watch: SetThreadContext failed\n");
    host_ResumeThread(thread);
    return 0;
}

static port_addr port_parse_hex(const char **text)
{
    port_addr value = 0;
    const char *p = *text;
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X'))
        p += 2;
    for (;; p++) {
        const char c = *p;
        const int digit = c >= '0' && c <= '9' ? c - '0' : c >= 'a' && c <= 'f' ? c - 'a' + 10
            : c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1;
        if (digit < 0)
            break;
        value = value * 16 + (port_addr)digit;
    }
    *text = p;
    return value;
}

static void port_install_watch(void)
{
    const char *list = getenv("PORT_WATCH");
    if (list == 0 || port_image_base == 0)
        return;
    while (*list != 0 && port_watch_count < 4) {
        const port_addr rva = port_parse_hex(&list);
        port_watch_addresses[port_watch_count++] = (port_addr)port_image_base + rva;
        while (*list == ',' || *list == ' ')
            list++;
        if (*list != 0 && !((*list >= '0' && *list <= '9') || (*list >= 'a' && *list <= 'f') || (*list >= 'A' && *list <= 'F')))
            break;
    }
    void *main_thread = 0;
    host_DuplicateHandle(host_GetCurrentProcess(), host_GetCurrentThread(), host_GetCurrentProcess(), &main_thread, 0, 0,
        2 /* DUPLICATE_SAME_ACCESS */);
    void *arm = host_CreateThread(0, 0, port_watch_arm, main_thread, 0, 0);
    if (arm != 0)
        host_WaitForSingleObject(arm, 0xffffffffu);
    port_write_error("port: watch: ");
    for (int i = 0; i < port_watch_count; i++) {
        port_write_error(i ? ", " : "");
        port_write_image_address(port_watch_addresses[i]);
    }
    port_write_error(" (main thread, writes)\n");
}

void port_install_crash_handler(void)
{
    port_find_image();
    host_AddVectoredExceptionHandler(1, port_crash_handler);
    port_install_watch();
    const char *seconds = getenv("PORT_WATCHDOG");
    if (seconds == 0 || atoi(seconds) <= 0)
        return;
    port_watchdog_seconds = (unsigned long)atoi(seconds);
    host_DuplicateHandle(host_GetCurrentProcess(), host_GetCurrentThread(), host_GetCurrentProcess(),
        &port_main_thread, 0, 0, 2 /* DUPLICATE_SAME_ACCESS */);
    host_CreateThread(0, 0, port_watchdog, 0, 0, 0);
}

void port_set_page_access(void *address, unsigned long size, int accessible)
{
    port_set_page_protection(address, size, accessible ? PORT_ACCESS_READ_WRITE : PORT_ACCESS_NONE);
}

void port_set_page_protection(void *address, unsigned long size, int access)
{
    // VirtualProtect rounds the range out to whole pages itself.
    unsigned long protect = access == PORT_ACCESS_NONE ? 0x01 /* PAGE_NOACCESS */
        : access == PORT_ACCESS_READ ? 0x02 /* PAGE_READONLY */ : 0x04 /* PAGE_READWRITE */;
    unsigned long old_protect;
    host_VirtualProtect(address, size, protect, &old_protect);
}
