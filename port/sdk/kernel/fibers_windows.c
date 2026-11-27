// port/sdk/kernel (Windows host): Win32 fiber wrappers bridging game cdecl LPFIBER_START_ROUTINE callbacks (built
// with -D__stdcall=) to kernel32's __stdcall fiber API, plus GetCurrentFiber().
//
// Both ConvertThreadToFiber and CreateFiber return a uniform port_win_fiber_wrap* whose first field is user_param
// (matching Win32 GetFiberData() = *(void **)GetCurrentFiber()), so SwitchToFiber(context->stack) and
// SwitchToFiber(context->fiber) handle thread fibers and child fibers identically.
#include "win32_host.h"

typedef void (__stdcall *host_fiber_start_fn)(void *);

PORT_HOST_IMPORT(void *, ConvertThreadToFiber, 4, (void *parameter));
PORT_HOST_IMPORT(int, ConvertFiberToThread, 0, (void));
PORT_HOST_IMPORT(void *, CreateFiber, 12, (__SIZE_TYPE__ stack_size, host_fiber_start_fn start, void *parameter));
PORT_HOST_IMPORT(void, DeleteFiber, 4, (void *fiber));
PORT_HOST_IMPORT(void, SwitchToFiber, 4, (void *fiber));
PORT_HOST_IMPORT(void *, GetProcessHeap, 0, (void));
PORT_HOST_IMPORT(void *, HeapAlloc, 12, (void *heap, unsigned long flags, __SIZE_TYPE__ bytes));
PORT_HOST_IMPORT(int, HeapFree, 12, (void *heap, unsigned long flags, void *mem));

typedef struct port_win_fiber_wrap {
    void *user_param; // offset 0: matches GetFiberData()
    void (*cdecl_start)(void *);
    void *os_fiber;
    int is_thread;
} port_win_fiber_wrap;

static __declspec(thread) port_win_fiber_wrap *port_win_current_fiber;

static void __stdcall port_win_fiber_trampoline(void *ctx_ptr)
{
    port_win_fiber_wrap *wrap = (port_win_fiber_wrap *)ctx_ptr;
    port_win_current_fiber = wrap;
    if (wrap && wrap->cdecl_start)
        wrap->cdecl_start(wrap->user_param);
}

void *GetCurrentFiber(void)
{
    return (void *)port_win_current_fiber;
}

void *ConvertThreadToFiber(void *parameter)
{
    if (port_win_current_fiber)
        return (void *)port_win_current_fiber;
    port_win_fiber_wrap *wrap = (port_win_fiber_wrap *)host_HeapAlloc(host_GetProcessHeap(), 0x08, sizeof(port_win_fiber_wrap));
    if (!wrap)
        return 0;
    void *os = host_ConvertThreadToFiber(wrap);
    if (!os) {
        host_HeapFree(host_GetProcessHeap(), 0, wrap);
        return 0;
    }
    wrap->user_param = parameter;
    wrap->cdecl_start = 0;
    wrap->os_fiber = os;
    wrap->is_thread = 1;
    port_win_current_fiber = wrap;
    return (void *)wrap;
}

int ConvertFiberToThread(void)
{
    port_win_fiber_wrap *wrap = port_win_current_fiber;
    if (!wrap || !wrap->is_thread)
        return 0;
    int ok = host_ConvertFiberToThread();
    if (ok) {
        port_win_current_fiber = 0;
        host_HeapFree(host_GetProcessHeap(), 0, wrap);
    }
    return ok;
}

void *CreateFiber(unsigned long stack_size, void (*start_address)(void *), void *parameter)
{
    if (!start_address)
        return 0;
    port_win_fiber_wrap *wrap = (port_win_fiber_wrap *)host_HeapAlloc(host_GetProcessHeap(), 0x08, sizeof(port_win_fiber_wrap));
    if (!wrap)
        return 0;
    wrap->user_param = parameter;
    wrap->cdecl_start = start_address;
    wrap->is_thread = 0;
    wrap->os_fiber = host_CreateFiber(stack_size, port_win_fiber_trampoline, wrap);
    if (!wrap->os_fiber) {
        host_HeapFree(host_GetProcessHeap(), 0, wrap);
        return 0;
    }
    return (void *)wrap;
}

void DeleteFiber(void *fiber)
{
    port_win_fiber_wrap *wrap = (port_win_fiber_wrap *)fiber;
    if (!wrap)
        return;
    void *os_fiber = wrap->os_fiber;
    int is_thread = wrap->is_thread;
    if (wrap == port_win_current_fiber)
        port_win_current_fiber = 0;
    host_HeapFree(host_GetProcessHeap(), 0, wrap);
    if (!is_thread && os_fiber)
        host_DeleteFiber(os_fiber);
}

void SwitchToFiber(void *fiber)
{
    port_win_fiber_wrap *to = (port_win_fiber_wrap *)fiber;
    if (!to || !to->os_fiber || to == port_win_current_fiber)
        return;
    port_win_current_fiber = to;
    host_SwitchToFiber(to->os_fiber);
}
