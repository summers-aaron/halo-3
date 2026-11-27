// port/sdk/runtime: the pieces of the MSVC C runtime that compiled code expects and that msvcrt.dll (through
// MinGW's import library) does not provide: atexit, the stack probe, _fltused and the
// thread-local storage directory that __declspec(thread) needs from the image (the 64-bit division helpers are in
// crt_divide.c).
#include "port_runtime.h"

// atexit keeps its own list (MinGW's _onexit is built on atexit), run newest first when main() returns.
#define PORT_ATEXIT_CAPACITY 1024
static void(__cdecl *port_atexit_functions[PORT_ATEXIT_CAPACITY])(void);
static int port_atexit_count = 0;

int __cdecl atexit(void(__cdecl *function)(void))
{
    if (port_atexit_count == PORT_ATEXIT_CAPACITY)
        return -1;
    port_atexit_functions[port_atexit_count++] = function;
    return 0;
}

void port_run_atexit(void)
{
    while (port_atexit_count > 0)
        port_atexit_functions[--port_atexit_count]();
}

int _fltused = 0x9875;

// Thread-local storage: the loader reads the directory named by _tls_used, copies .tls (from _tls_start to
// _tls_end, with the game's __declspec(thread) data between them) for each thread and stores the slot in
// _tls_index. Compiled code reaches the thread's block through fs:[__tls_array] (offset 0x2c in the TEB).
unsigned long _tls_index = 0;
#pragma section(".tls", read, write)
#pragma section(".tls$ZZZ", read, write)
__declspec(allocate(".tls")) char _tls_start = 0;
__declspec(allocate(".tls$ZZZ")) char _tls_end = 0;
typedef void(__stdcall *port_tls_callback)(void *module, unsigned long reason, void *reserved);
#pragma section(".CRT$XLA", read)
#pragma section(".CRT$XLZ", read)
__declspec(allocate(".CRT$XLA")) port_tls_callback __xl_a = 0;
__declspec(allocate(".CRT$XLZ")) port_tls_callback __xl_z = 0;
typedef struct port_tls_directory {
    __UINTPTR_TYPE__ start_of_raw_data;
    __UINTPTR_TYPE__ end_of_raw_data;
    __UINTPTR_TYPE__ address_of_index;
    __UINTPTR_TYPE__ address_of_callbacks;
    unsigned long size_of_zero_fill;
    unsigned long characteristics;
} port_tls_directory;
const port_tls_directory _tls_used = {
    (__UINTPTR_TYPE__)&_tls_start, (__UINTPTR_TYPE__)&_tls_end, (__UINTPTR_TYPE__)&_tls_index,
    (__UINTPTR_TYPE__)(&__xl_a + 1), 0, 0};

#ifdef _WIN64
// Win64 __chkstk: %rax holds byte count; probes guard pages from TEB.StackLimit (%gs:16) down to target without
// modifying %rsp or %rax.
__asm__(
    "\t.text\n"
    "\t.globl\t__chkstk\n"
    "__chkstk:\n"
    "\tsubq\t$16, %rsp\n"
    "\tmovq\t%r10, (%rsp)\n"
    "\tmovq\t%r11, 8(%rsp)\n"
    "\tleaq\t24(%rsp), %r10\n"
    "\tsubq\t%rax, %r10\n"
    "\tjae\t0f\n"
    "\txorq\t%r10, %r10\n"
    "0:\n"
    "\tmovq\t%gs:16, %r11\n"
    "\tcmpq\t%r11, %r10\n"
    "\tjae\t2f\n"
    "\tandq\t$-4096, %r10\n"
    "1:\n"
    "\tsubq\t$4096, %r11\n"
    "\ttestb\t$0, (%r11)\n"
    "\tcmpq\t%r10, %r11\n"
    "\tjne\t1b\n"
    "2:\n"
    "\tmovq\t(%rsp), %r10\n"
    "\tmovq\t8(%rsp), %r11\n"
    "\taddq\t$16, %rsp\n"
    "\tretq\n");

// Clang emits libatomic calls when -fmax-type-align=4 makes 8-byte atomic accesses appear potentially under-aligned.
static void port_atomic_copy(void *dst, const void *src, __SIZE_TYPE__ n)
{
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    for (__SIZE_TYPE__ i = 0; i < n; i++)
        d[i] = s[i];
}

void port_win64_atomic_exchange(__SIZE_TYPE__ size, void *ptr, const void *val, void *ret, int memorder)
    __asm__("__atomic_exchange");
void port_win64_atomic_exchange(__SIZE_TYPE__ size, void *ptr, const void *val, void *ret, int memorder)
{
    (void)memorder;
    switch (size) {
    case 1: {
        unsigned char v = *(const unsigned char *)val;
        *(unsigned char *)ret = __atomic_exchange_n((unsigned char *)ptr, v, __ATOMIC_SEQ_CST);
        break;
    }
    case 2: {
        unsigned short v;
        port_atomic_copy(&v, val, 2);
        v = __atomic_exchange_n((unsigned short *)ptr, v, __ATOMIC_SEQ_CST);
        port_atomic_copy(ret, &v, 2);
        break;
    }
    case 4: {
        unsigned int v;
        port_atomic_copy(&v, val, 4);
        v = __atomic_exchange_n((unsigned int *)ptr, v, __ATOMIC_SEQ_CST);
        port_atomic_copy(ret, &v, 4);
        break;
    }
    case 8: {
        unsigned long long v;
        port_atomic_copy(&v, val, 8);
        v = __atomic_exchange_n((unsigned long long *)ptr, v, __ATOMIC_SEQ_CST);
        port_atomic_copy(ret, &v, 8);
        break;
    }
    default:
        port_atomic_copy(ret, ptr, size);
        port_atomic_copy(ptr, val, size);
        __atomic_thread_fence(__ATOMIC_SEQ_CST);
        break;
    }
}

void port_win64_atomic_store(__SIZE_TYPE__ size, void *ptr, const void *val, int memorder)
    __asm__("__atomic_store");
void port_win64_atomic_store(__SIZE_TYPE__ size, void *ptr, const void *val, int memorder)
{
    (void)memorder;
    switch (size) {
    case 1:
        __atomic_store_n((unsigned char *)ptr, *(const unsigned char *)val, __ATOMIC_SEQ_CST);
        break;
    case 2: {
        unsigned short v;
        port_atomic_copy(&v, val, 2);
        __atomic_store_n((unsigned short *)ptr, v, __ATOMIC_SEQ_CST);
        break;
    }
    case 4: {
        unsigned int v;
        port_atomic_copy(&v, val, 4);
        __atomic_store_n((unsigned int *)ptr, v, __ATOMIC_SEQ_CST);
        break;
    }
    case 8: {
        unsigned long long v;
        port_atomic_copy(&v, val, 8);
        __atomic_store_n((unsigned long long *)ptr, v, __ATOMIC_SEQ_CST);
        break;
    }
    default:
        port_atomic_copy(ptr, val, size);
        __atomic_thread_fence(__ATOMIC_SEQ_CST);
        break;
    }
}

void port_win64_atomic_load(__SIZE_TYPE__ size, const void *ptr, void *ret, int memorder)
    __asm__("__atomic_load");
void port_win64_atomic_load(__SIZE_TYPE__ size, const void *ptr, void *ret, int memorder)
{
    (void)memorder;
    switch (size) {
    case 1:
        *(unsigned char *)ret = __atomic_load_n((const unsigned char *)ptr, __ATOMIC_SEQ_CST);
        break;
    case 2: {
        unsigned short v = __atomic_load_n((const unsigned short *)ptr, __ATOMIC_SEQ_CST);
        port_atomic_copy(ret, &v, 2);
        break;
    }
    case 4: {
        unsigned int v = __atomic_load_n((const unsigned int *)ptr, __ATOMIC_SEQ_CST);
        port_atomic_copy(ret, &v, 4);
        break;
    }
    case 8: {
        unsigned long long v = __atomic_load_n((const unsigned long long *)ptr, __ATOMIC_SEQ_CST);
        port_atomic_copy(ret, &v, 8);
        break;
    }
    default:
        __atomic_thread_fence(__ATOMIC_SEQ_CST);
        port_atomic_copy(ret, ptr, size);
        break;
    }
}
#else
// __tls_array is an absolute symbol. _chkstk probes each page of a large frame in order (the guard page grows the
// stack one page at a time), then moves esp down by eax as the caller expects.
__asm__(
    "\t.globl\t__tls_array\n"
    "\t.set\t__tls_array, 0x2c\n"
    "\t.text\n"
    "\t.globl\t__chkstk\n"
    "__chkstk:\n"
    "\tpushl\t%ecx\n"
    "\tleal\t8(%esp), %ecx\n"     // the caller's esp before the call
    "1:\n"
    "\tcmpl\t$0x1000, %eax\n"
    "\tjb\t2f\n"
    "\tsubl\t$0x1000, %ecx\n"
    "\ttestl\t%ecx, (%ecx)\n"
    "\tsubl\t$0x1000, %eax\n"
    "\tjmp\t1b\n"
    "2:\n"
    "\tsubl\t%eax, %ecx\n"
    "\ttestl\t%ecx, (%ecx)\n"
    "\tleal\t4(%esp), %eax\n"     // address of the return address
    "\tmovl\t%ecx, %esp\n"        // the new stack top
    "\tmovl\t-4(%eax), %ecx\n"    // restore ecx
    "\tpushl\t(%eax)\n"           // return address on the new stack
    "\tsubl\t%esp, %eax\n"        // eax back to the original size
    "\tret\n");
#endif

// MinGW's math functions (libmingwex) report domain and range errors here; MinGW's own runtime passes them to a
// _matherr hook the program may install. The game installs none and reads no matherr state: the functions'
// return values (NaN, HUGE_VAL) and errno already say what happened.
void __cdecl __mingw_raise_matherr(int type, const char *name, double argument1, double argument2, double result)
{
    (void)type; (void)name; (void)argument1; (void)argument2; (void)result;
}

// RtlVsnprintf: the Xbox kernel's _vsnprintf, which msvcrt.dll has with the same meaning.
typedef char *port_va_list;
int __cdecl _vsnprintf(char *destination, __SIZE_TYPE__ count, const char *format, port_va_list arguments);
int __cdecl RtlVsnprintf(char *destination, __SIZE_TYPE__ count, const char *format, port_va_list arguments)
{
    return _vsnprintf(destination, count, format, arguments);
}

// vsnprintf: the Xbox C runtime exports it (the SDK's stdio.h declares it _CRTIMP), as the VC8-era runtime's
// _vsnprintf under its ANSI name; msvcrt.dll exports only _vsnprintf. Code built against the SDK headers imports it
// (__imp__vsnprintf), and the link binds that import to this definition.
int __cdecl vsnprintf(char *destination, __SIZE_TYPE__ count, const char *format, port_va_list arguments)
{
    return _vsnprintf(destination, count, format, arguments);
}
