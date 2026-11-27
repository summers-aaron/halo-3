// port/sdk/kernel (Linux host): 32-bit i686 Win32 fiber API (GetCurrentFiber, ConvertThreadToFiber,
// ConvertFiberToThread, CreateFiber, DeleteFiber, SwitchToFiber) used by tag_resource_cache_paging and saved_film.
#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#ifdef __x86_64__
typedef struct port_fiber {
    void *parameter;        // offset 0: matches Win32 GetFiberData()
    unsigned long long rsp; // 8
    unsigned long long rbp; // 16
    unsigned long long rbx; // 24
    unsigned long long r12; // 32
    unsigned long long r13; // 40
    unsigned long long r14; // 48
    unsigned long long r15; // 56
    unsigned int mxcsr;     // 64
    unsigned short x87_cw;  // 68
    unsigned short is_thread;
    void *stack_map;
    unsigned long stack_map_size;
    void (PORT_MS_ABI *start_routine)(void *);
} port_fiber;

static __thread port_fiber *port_current_fiber;

__attribute__((naked)) static void port_fiber_swap(port_fiber *from, port_fiber *to)
{
    __asm__ volatile(
        // SysV AMD64: %rdi = from, %rsi = to
        "movq %rbp, 16(%rdi)\n\t"
        "movq %rbx, 24(%rdi)\n\t"
        "movq %r12, 32(%rdi)\n\t"
        "movq %r13, 40(%rdi)\n\t"
        "movq %r14, 48(%rdi)\n\t"
        "movq %r15, 56(%rdi)\n\t"
        "stmxcsr 64(%rdi)\n\t"
        "fnstcw 68(%rdi)\n\t"
        "movq %rsp, 8(%rdi)\n\t"

        "movq 8(%rsi), %rsp\n\t"
        "fldcw 68(%rsi)\n\t"
        "ldmxcsr 64(%rsi)\n\t"
        "movq 56(%rsi), %r15\n\t"
        "movq 48(%rsi), %r14\n\t"
        "movq 40(%rsi), %r13\n\t"
        "movq 32(%rsi), %r12\n\t"
        "movq 24(%rsi), %rbx\n\t"
        "movq 16(%rsi), %rbp\n\t"
        "retq\n\t"
    );
}
#else
typedef struct port_fiber {
    void *parameter; // offset 0: matches Win32 GetFiberData() (*(void **)GetCurrentFiber())
    unsigned int esp;
    unsigned int ebp;
    unsigned int ebx;
    unsigned int esi;
    unsigned int edi;
    unsigned int mxcsr;
    unsigned short x87_cw;
    unsigned short is_thread;
    void *stack_map;
    unsigned long stack_map_size;
    void (*start_routine)(void *);
} port_fiber;

static __thread port_fiber *port_current_fiber;

__attribute__((naked)) static void port_fiber_swap(port_fiber *from, port_fiber *to)
{
    __asm__ volatile(
        "movl 4(%esp), %eax\n\t"      // eax = from
        "movl 8(%esp), %edx\n\t"      // edx = to
        "movl %ebp, 8(%eax)\n\t"
        "movl %ebx, 12(%eax)\n\t"
        "movl %esi, 16(%eax)\n\t"
        "movl %edi, 20(%eax)\n\t"
        "stmxcsr 24(%eax)\n\t"
        "fnstcw 28(%eax)\n\t"
        "movl %esp, 4(%eax)\n\t"

        "movl 4(%edx), %esp\n\t"
        "fldcw 28(%edx)\n\t"
        "ldmxcsr 24(%edx)\n\t"
        "movl 20(%edx), %edi\n\t"
        "movl 16(%edx), %esi\n\t"
        "movl 12(%edx), %ebx\n\t"
        "movl 8(%edx), %ebp\n\t"
        "retl\n\t"
    );
}
#endif

__attribute__((used)) static void port_fiber_entry_trampoline(void)
{
    port_fiber *self = port_current_fiber;
    if (self && self->start_routine)
        self->start_routine(self->parameter);
    pthread_exit(0);
}

void *PORT_MS_ABI GetCurrentFiber(void)
{
    return (void *)port_current_fiber;
}

void *PORT_MS_ABI ConvertThreadToFiber(void *parameter)
{
    if (port_current_fiber != 0)
        return (void *)port_current_fiber;
    port_fiber *f = (port_fiber *)calloc(1, sizeof(port_fiber));
    if (!f)
        return 0;
    f->parameter = parameter;
    f->is_thread = 1;
    __asm__ volatile("stmxcsr %0" : "=m"(f->mxcsr));
    __asm__ volatile("fnstcw %0" : "=m"(f->x87_cw));
    port_current_fiber = f;
    return (void *)f;
}

int PORT_MS_ABI ConvertFiberToThread(void)
{
    port_fiber *f = port_current_fiber;
    if (!f || !f->is_thread)
        return 0;
    port_current_fiber = 0;
    free(f);
    return 1;
}

void *PORT_MS_ABI CreateFiber(unsigned long stack_size, void (PORT_MS_ABI *start_address)(void *), void *parameter)
{
    if (!start_address)
        return 0;
    unsigned long page = 4096ul;
    unsigned long usable = stack_size < 65536ul ? 65536ul : ((stack_size + page - 1ul) & ~(page - 1ul));
    unsigned long total = usable + page; // 1 guard page at bottom

    void *mem = mmap(0, total, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED)
        return 0;
    mprotect(mem, page, PROT_NONE);

    port_fiber *f = (port_fiber *)calloc(1, sizeof(port_fiber));
    if (!f) {
        munmap(mem, total);
        return 0;
    }

    f->parameter = parameter;
    f->is_thread = 0;
    f->stack_map = mem;
    f->stack_map_size = total;
    f->start_routine = start_address;
    f->mxcsr = 0x1f80u;
    f->x87_cw = 0x027fu;

#ifdef __x86_64__
    // On entry into port_fiber_entry_trampoline (after retq pops return RIP), %rsp % 16 == 8.
    unsigned long long top = ((unsigned long long)(__UINTPTR_TYPE__)((char *)mem + total)) & ~15ull;
    unsigned long long *sp = (unsigned long long *)(__UINTPTR_TYPE__)top;
    *(--sp) = 0; // fake return RIP (keeps %rsp % 16 == 8 after retq)
    *(--sp) = (unsigned long long)(__UINTPTR_TYPE__)port_fiber_entry_trampoline;
    f->rsp = (unsigned long long)(__UINTPTR_TYPE__)sp;
    f->rbp = 0;
#else
    // Align stack so on entry into port_fiber_entry_trampoline (after retl pops return EIP), %esp % 16 == 12.
    unsigned int top = ((unsigned int)(__UINTPTR_TYPE__)((char *)mem + total)) & ~15u;
    unsigned int *sp = (unsigned int *)(__UINTPTR_TYPE__)top;
    *(--sp) = 0; // padding
    *(--sp) = 0; // padding
    *(--sp) = 0; // padding
    *(--sp) = 0; // fake return EIP for trampoline
    *(--sp) = (unsigned int)(__UINTPTR_TYPE__)port_fiber_entry_trampoline; // popped by port_fiber_swap's retl
    f->esp = (unsigned int)(__UINTPTR_TYPE__)sp;
    f->ebp = 0;
#endif

    return (void *)f;
}

void PORT_MS_ABI DeleteFiber(void *fiber)
{
    port_fiber *f = (port_fiber *)fiber;
    if (!f)
        return;
    if (f == port_current_fiber) {
        port_current_fiber = 0;
        pthread_exit(0);
    }
    if (!f->is_thread && f->stack_map)
        munmap(f->stack_map, f->stack_map_size);
    free(f);
}

void PORT_MS_ABI SwitchToFiber(void *fiber)
{
    port_fiber *to = (port_fiber *)fiber;
    port_fiber *from = port_current_fiber;
    if (!to || !from || to == from)
        return;
    port_current_fiber = to;
    port_fiber_swap(from, to);
}
