// port/sdk/kernel (Windows host): time. The Xbox API's performance counter and tick count are Win32's.
#include "win32_host.h"

PORT_HOST_IMPORT(int, QueryPerformanceCounter, 4, (long long *count));
PORT_HOST_IMPORT(int, QueryPerformanceFrequency, 4, (long long *frequency));
PORT_HOST_IMPORT(unsigned long, GetTickCount, 0, (void));
PORT_HOST_IMPORT(void *, CreateWaitableTimerExW, 16, (void *attributes, const unsigned short *name, unsigned long flags,
    unsigned long access));
PORT_HOST_IMPORT(int, SetWaitableTimer, 24, (void *timer, const long long *due, long period, void *routine,
    void *argument, int resume));
PORT_HOST_IMPORT(unsigned long, WaitForSingleObject, 8, (void *handle, unsigned long milliseconds));
PORT_HOST_IMPORT(void, Sleep, 4, (unsigned long milliseconds));
PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
PORT_HOST_IMPORT(int, InitOnceExecuteOnce, 16, (void **once, int (__stdcall *callback)(void **, void *, void **),
    void *parameter, void **context));

int QueryPerformanceCounter(long long *count)
{
    return host_QueryPerformanceCounter(count);
}

static long long port_qpf(void)
{
    static long long cached;
    long long f = __atomic_load_n(&cached, __ATOMIC_RELAXED);
    if (f == 0) {
        if (!host_QueryPerformanceFrequency(&f) || f <= 0)
            return 0;
        __atomic_store_n(&cached, f, __ATOMIC_RELAXED);
    }
    return f;
}

int QueryPerformanceFrequency(long long *frequency)
{
    if (frequency == 0)
        return host_QueryPerformanceFrequency(frequency);
    long long f = port_qpf();
    if (f == 0)
        return 0;
    *frequency = f;
    return 1;
}

unsigned long GetTickCount(void)
{
    return host_GetTickCount();
}

// mftb: the Xbox 360 time base runs at 50 MHz; scale the host counter to it.
unsigned long long __mftb(void)
{
    long long count;
    unsigned long long frequency = (unsigned long long)port_qpf();
    if (frequency == 0 || !host_QueryPerformanceCounter(&count))
        return 0;
    // The common 10 MHz QPC clock is already in 100 ns units.
    if (frequency == 10000000ull)
        return (unsigned long long)count * 5ull;
    unsigned long long seconds = (unsigned long long)count / frequency;
    unsigned long long rest = (unsigned long long)count % frequency;
    return seconds * 50000000ull + rest * 50000000ull / frequency;
}

// The Xbox kernel's timed waits (Sleep, timeouts) have millisecond resolution; Windows' default timer runs at 64 Hz,
// so a Sleep(1) or Sleep(3) takes 15.6 ms there. The first Sleep (win32_windows.c) or precise wait sets the process's
// timer resolution to 1 ms (winmm timeBeginPeriod; per process since Windows 10 2004), as the console's waits are.
static int resolution_set;

static int __stdcall port_time_init_resolution(void **once, void *parameter, void **context)
{
    (void)once;
    (void)parameter;
    (void)context;
    void *winmm = host_LoadLibraryA("winmm.dll");
    unsigned int(__stdcall *begin_period)(unsigned int) =
        winmm ? (unsigned int(__stdcall *)(unsigned int))host_GetProcAddress(winmm, "timeBeginPeriod") : 0;
    if (begin_period)
        begin_period(1);
    __atomic_store_n(&resolution_set, 1, __ATOMIC_RELEASE);
    return 1;
}

void port_time_set_resolution(void)
{
    static void *once;
    if (!__atomic_load_n(&resolution_set, __ATOMIC_ACQUIRE))
        host_InitOnceExecuteOnce(&once, port_time_init_resolution, 0, 0);
}

#define PORT_CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002ul // Windows 10 1803
#define PORT_TIMER_ALL_ACCESS 0x001f0003ul
#define PORT_INFINITE 0xfffffffful

// Wait on a high-resolution timer until 250 us before the QPC deadline. When at most 350 us remain, poll QPC directly
// to avoid arming a timer for at most 100 us. Reserve 1 ms for the spin when using the coarse Sleep fallback.
void port_sleep_until_counter(long long counter)
{
    static __declspec(thread) void *timer;
    static __declspec(thread) int timer_tried;
    const long long frequency = port_qpf();
    long long now;
    if (frequency == 0 || !host_QueryPerformanceCounter(&now) || now >= counter)
        return;
    if (!timer_tried) {
        timer_tried = 1;
        timer = host_CreateWaitableTimerExW(0, 0, PORT_CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, PORT_TIMER_ALL_ACCESS);
        if (!host_QueryPerformanceCounter(&now))
            return;
    }
    const long long spin_ticks = frequency / 4000;
    const long long timer_threshold = frequency * 350 / 1000000;
    const long long sleep_threshold = (frequency + 499) / 500;
    while (now < counter) {
        const long long remaining = counter - now;
        if (timer && remaining > timer_threshold) {
            long long wait = remaining - spin_ticks;
            if (wait > frequency)
                wait = frequency;
            const long long due = frequency == 10000000ll ? -wait
                : -((wait * 10000000ll + frequency - 1) / frequency); // relative, 100 ns
            if (host_SetWaitableTimer(timer, &due, 0, 0, 0, 0) &&
                host_WaitForSingleObject(timer, PORT_INFINITE) == 0 /* WAIT_OBJECT_0 */) {
                if (!host_QueryPerformanceCounter(&now))
                    return;
                continue;
            }
        }
        if (remaining < sleep_threshold)
            break;
        const long long wait = remaining > frequency ? frequency : remaining;
        const long long milliseconds = wait * 1000 / frequency - 1;
        port_time_set_resolution();
        host_Sleep((unsigned long)milliseconds);
        if (!host_QueryPerformanceCounter(&now))
            return;
    }
    while (now < counter) {
        __builtin_ia32_pause();
        if (!host_QueryPerformanceCounter(&now))
            return;
    }
}
