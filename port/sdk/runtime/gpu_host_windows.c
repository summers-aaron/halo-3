// port/sdk/runtime (Windows host): the renderer's host threads (Win32: SRW lock, condition variable) through
// kernel32's host imports (the game declares its own cdecl Win32 names; see port/sdk/kernel/win32_host.h): the pipeline
// prewarm's workers and their monitor. Interface: port/sdk/d3d/gpu_host.h.
#include "../d3d/gpu_host.h"
#include "../kernel/win32_host.h"

PORT_HOST_IMPORT(void *, CreateThread, 24,
    (void *security, __SIZE_TYPE__ stack_size, unsigned long(__stdcall *start)(void *), void *parameter,
        unsigned long flags, unsigned long *thread_id));
PORT_HOST_IMPORT(unsigned long, WaitForSingleObject, 8, (void *handle, unsigned long milliseconds));
PORT_HOST_IMPORT(int, CloseHandle, 4, (void *handle));
PORT_HOST_IMPORT(void, AcquireSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(void, ReleaseSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(int, SleepConditionVariableSRW, 16, (void **condition, void **lock, unsigned long milliseconds,
    unsigned long flags));
PORT_HOST_IMPORT(void, WakeAllConditionVariable, 4, (void **condition));
PORT_HOST_IMPORT(void *, HeapAlloc, 12, (void *heap, unsigned long flags, __SIZE_TYPE__ bytes));
PORT_HOST_IMPORT(int, HeapFree, 12, (void *heap, unsigned long flags, void *memory));
PORT_HOST_IMPORT(void *, GetProcessHeap, 0, (void));
PORT_HOST_IMPORT(unsigned long, GetActiveProcessorCount, 4, (unsigned short group));
PORT_HOST_IMPORT(void *, GetCurrentThread, 0, (void));
PORT_HOST_IMPORT(int, SetThreadPriority, 8, (void *thread, int priority));

#define HEAP_ZERO_MEMORY 0x00000008ul
#define ALL_PROCESSOR_GROUPS 0xffffu
#define THREAD_PRIORITY_BELOW_NORMAL (-1)

struct s_port_gpu_monitor {
    void *lock;      // SRWLOCK (zero: unlocked)
    void *condition; // CONDITION_VARIABLE (zero: initialized)
};

s_port_gpu_monitor *port_gpu_monitor_create(void)
{
    return (s_port_gpu_monitor *)host_HeapAlloc(host_GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(s_port_gpu_monitor));
}

void port_gpu_monitor_destroy(s_port_gpu_monitor *monitor)
{
    if (monitor != 0)
        host_HeapFree(host_GetProcessHeap(), 0, monitor);
}

void port_gpu_monitor_lock(s_port_gpu_monitor *monitor) { host_AcquireSRWLockExclusive(&monitor->lock); }
void port_gpu_monitor_unlock(s_port_gpu_monitor *monitor) { host_ReleaseSRWLockExclusive(&monitor->lock); }
void port_gpu_monitor_wait(s_port_gpu_monitor *monitor)
{
    host_SleepConditionVariableSRW(&monitor->condition, &monitor->lock, 0xfffffffful /* INFINITE */, 0);
}
void port_gpu_monitor_broadcast(s_port_gpu_monitor *monitor) { host_WakeAllConditionVariable(&monitor->condition); }

struct s_thread_start {
    void (*entry)(void *);
    void *parameter;
    void *handle;
};

static unsigned long __stdcall thread_main(void *start)
{
    struct s_thread_start *s = (struct s_thread_start *)start;
    s->entry(s->parameter);
    return 0;
}

void *port_gpu_thread_start(void (*entry)(void *parameter), void *parameter)
{
    struct s_thread_start *start =
        (struct s_thread_start *)host_HeapAlloc(host_GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(struct s_thread_start));
    if (start == 0)
        return 0;
    start->entry = entry;
    start->parameter = parameter;
    start->handle = host_CreateThread(0, 0, thread_main, start, 0, 0);
    if (start->handle == 0) {
        host_HeapFree(host_GetProcessHeap(), 0, start);
        return 0;
    }
    return start;
}

void port_gpu_thread_join(void *thread)
{
    struct s_thread_start *start = (struct s_thread_start *)thread;
    if (start == 0)
        return;
    host_WaitForSingleObject(start->handle, 0xfffffffful /* INFINITE */);
    host_CloseHandle(start->handle);
    host_HeapFree(host_GetProcessHeap(), 0, start);
}

void port_gpu_thread_set_low_priority(void)
{
    host_SetThreadPriority(host_GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
}

unsigned int port_gpu_cpu_count(void)
{
    const unsigned long count = host_GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
    return count > 0 ? (unsigned int)count : 1u;
}

double port_gpu_thread_cpu_ms(void)
{
    return -1.0; // GetThreadTimes advances in scheduler quanta (15.6 ms): too coarse for a frame
}
