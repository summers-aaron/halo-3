// port/sdk/runtime (Linux host): reports for headless runs, as on Windows. Resolve the addresses with the link map
// (BUILD_DIR/stage1/halo3.map); the stage-1 build keeps frame pointers.
//   poison    a fault on a poisoned datum (stub_report.c) is reported by name, not as a crash;
//   crash     on SIGSEGV, SIGBUS, SIGILL or SIGFPE, prints the signal, the faulting address and the return addresses
//             along the frame-pointer chain, then exits with 6;
//   watchdog  PORT_WATCHDOG=SECONDS: after that long, samples the main thread five times, 200 ms apart, then exits
//             with 8.
#define _GNU_SOURCE
#include "port_runtime.h"

#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <ucontext.h>
#include <unistd.h>

static void port_write_hex(unsigned long value)
{
    char text[11] = "0x00000000";
    for (int i = 9; i >= 2; i--, value >>= 4)
        text[i] = "0123456789abcdef"[value & 15];
    port_write_error(text);
}

// A frame is followed only while its two words are mapped: mincore fails with ENOMEM on an unmapped page.
static int port_readable(const void *address)
{
    unsigned char resident;
    void *page = (void *)((unsigned long)address & ~0xffful);
    return mincore(page, 4096, &resident) == 0;
}

static void port_write_frames(unsigned long eip, unsigned long ebp)
{
    port_write_error("port: at ");
    port_write_hex(eip);
    port_write_error(", frames:");
    unsigned long *frame = (unsigned long *)ebp;
    for (int depth = 0; depth < 24 && frame != 0 && ((unsigned long)frame & 3) == 0 && port_readable(frame)
         && port_readable(frame + 1); depth++) {
        port_write_error(" ");
        port_write_hex(frame[1]);
        frame = (unsigned long *)frame[0];
    }
    port_write_error("\n");
}

#ifdef __x86_64__
#define PORT_REG_IP REG_RIP
#define PORT_REG_BP REG_RBP
#define PORT_REG_SP REG_RSP
#else
#define PORT_REG_IP REG_EIP
#define PORT_REG_BP REG_EBP
#define PORT_REG_SP REG_ESP
#endif

static void port_crash_handler(int signal_number, siginfo_t *info, void *context)
{
    if (signal_number == SIGSEGV && port_poisoned_access(info->si_addr))
        return; // the datum is accessible now: the access runs again
    const ucontext_t *uc = (const ucontext_t *)context;
    port_write_error("port: signal ");
    port_write_error(signal_number == SIGSEGV ? "SIGSEGV" : signal_number == SIGBUS ? "SIGBUS"
                     : signal_number == SIGILL ? "SIGILL" : "SIGFPE");
    port_write_error(" at address ");
    port_write_hex((unsigned long)info->si_addr);
    port_write_error("\n");
    port_write_frames((unsigned long)uc->uc_mcontext.gregs[PORT_REG_IP], (unsigned long)uc->uc_mcontext.gregs[PORT_REG_BP]);
    // the words at the stack pointer: a call through a bad pointer leaves its caller's return address on top, where
    // the frame chain (EBP/RBP) has nothing to show
    unsigned long *stack = (unsigned long *)uc->uc_mcontext.gregs[PORT_REG_SP];
    port_write_error("port: stack:");
    for (int i = 0; i < 16 && port_readable(stack + i); i++) {
        port_write_error(" ");
        port_write_hex(stack[i]);
    }
    port_write_error("\n");
    port_exit(6);
}

// The watchdog interrupts the main thread with SIGPROF; the handler prints where it is.
static pthread_t port_main_thread;
static unsigned int port_watchdog_seconds;

static void port_sample_handler(int signal_number, siginfo_t *info, void *context)
{
    (void)signal_number; (void)info;
    const ucontext_t *uc = (const ucontext_t *)context;
    port_write_frames((unsigned long)uc->uc_mcontext.gregs[PORT_REG_IP], (unsigned long)uc->uc_mcontext.gregs[PORT_REG_BP]);
}

static void *port_watchdog(void *parameter)
{
    (void)parameter;
    sleep(port_watchdog_seconds);
    port_write_error("port: watchdog: main thread samples\n");
    for (int sample = 0; sample < 5; sample++) {
        pthread_kill(port_main_thread, SIGPROF);
        usleep(200000);
    }
    port_exit(8);
    return 0;
}

static void port_on_signal(int signal_number, void (*handler)(int, siginfo_t *, void *))
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_sigaction = handler;
    action.sa_flags = SA_SIGINFO | SA_ONSTACK | SA_RESTART;
    sigemptyset(&action.sa_mask);
    sigaction(signal_number, &action, 0);
}

void port_install_crash_handler(void)
{
    // An alternate signal stack, so a stack overflow is reported too.
    static unsigned char alternate_stack[64 * 1024];
    stack_t stack = {alternate_stack, 0, sizeof(alternate_stack)};
    sigaltstack(&stack, 0);
    port_on_signal(SIGSEGV, port_crash_handler);
    port_on_signal(SIGBUS, port_crash_handler);
    port_on_signal(SIGILL, port_crash_handler);
    port_on_signal(SIGFPE, port_crash_handler);
    const char *seconds = getenv("PORT_WATCHDOG");
    if (seconds == 0 || atoi(seconds) <= 0)
        return;
    port_watchdog_seconds = (unsigned int)atoi(seconds);
    port_main_thread = pthread_self();
    port_on_signal(SIGPROF, port_sample_handler);
    pthread_t watchdog;
    pthread_create(&watchdog, 0, port_watchdog, 0);
}

void port_set_page_access(void *address, unsigned long size, int accessible)
{
    port_set_page_protection(address, size, accessible ? PORT_ACCESS_READ_WRITE : PORT_ACCESS_NONE);
}

void port_set_page_protection(void *address, unsigned long size, int access)
{
    // mprotect takes whole pages: round the range out.
    unsigned long first = (unsigned long)address & ~0xffful;
    unsigned long end = ((unsigned long)address + size + 0xfff) & ~0xffful;
    int protect = access == PORT_ACCESS_NONE ? PROT_NONE : access == PORT_ACCESS_READ ? PROT_READ : PROT_READ | PROT_WRITE;
    mprotect((void *)first, end - first, protect);
}
