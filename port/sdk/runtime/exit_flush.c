// port/sdk/runtime/exit_flush.c: the title's writer of what it still holds in memory for its players, run before the
// process ends on request (port_runtime.h: port_set_exit_flush, port_exit_flush). Both hosts.
#include "port_runtime.h"

// port/sdk/kernel (time_*.c, kernel_linux.c / win32_windows.c)
extern unsigned long PORT_MS_ABI GetTickCount(void);
extern int PORT_MS_ABI SwitchToThread(void);

// How long a second thread on its way out waits for the writer before it ends the process all the same.
#define PORT_EXIT_FLUSH_WAIT_MILLISECONDS 5000ul

static void (PORT_MS_ABI *g_exit_flush)(void);
static int g_exit_flush_state; // 0 not run, 1 running, 2 done

void PORT_MS_ABI port_set_exit_flush(void (PORT_MS_ABI *flush)(void))
{
    __atomic_store_n(&g_exit_flush, flush, __ATOMIC_RELEASE);
}

void port_exit_flush(void)
{
    int idle = 0;
    if (!__atomic_compare_exchange_n(&g_exit_flush_state, &idle, 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        // another thread is on its way out too (the window closed while the title asked to leave): it runs the writer,
        // and this one ends the process once it has finished (or after a while: a writer that never returns must not
        // keep the process from ending)
        const unsigned long start = GetTickCount();
        while (__atomic_load_n(&g_exit_flush_state, __ATOMIC_ACQUIRE) != 2 &&
               GetTickCount() - start < PORT_EXIT_FLUSH_WAIT_MILLISECONDS)
            SwitchToThread();
        return;
    }
    void (PORT_MS_ABI *flush)(void) = __atomic_load_n(&g_exit_flush, __ATOMIC_ACQUIRE);
    if (flush != 0)
        flush();
    __atomic_store_n(&g_exit_flush_state, 2, __ATOMIC_RELEASE);
}
