// port/sdk/runtime (Linux host): the renderer's host threads (pthreads): the pipeline prewarm's workers and their
// monitor. Interface: port/sdk/d3d/gpu_host.h.
#define _GNU_SOURCE
#include "../d3d/gpu_host.h"

#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

struct s_port_gpu_monitor {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
};

s_port_gpu_monitor *port_gpu_monitor_create(void)
{
    s_port_gpu_monitor *monitor = (s_port_gpu_monitor *)calloc(1, sizeof(s_port_gpu_monitor));
    if (monitor == 0)
        return 0;
    if (pthread_mutex_init(&monitor->mutex, 0) != 0) {
        free(monitor);
        return 0;
    }
    if (pthread_cond_init(&monitor->condition, 0) != 0) {
        pthread_mutex_destroy(&monitor->mutex);
        free(monitor);
        return 0;
    }
    return monitor;
}

void port_gpu_monitor_destroy(s_port_gpu_monitor *monitor)
{
    if (monitor == 0)
        return;
    pthread_cond_destroy(&monitor->condition);
    pthread_mutex_destroy(&monitor->mutex);
    free(monitor);
}

void port_gpu_monitor_lock(s_port_gpu_monitor *monitor) { pthread_mutex_lock(&monitor->mutex); }
void port_gpu_monitor_unlock(s_port_gpu_monitor *monitor) { pthread_mutex_unlock(&monitor->mutex); }
void port_gpu_monitor_wait(s_port_gpu_monitor *monitor) { pthread_cond_wait(&monitor->condition, &monitor->mutex); }
void port_gpu_monitor_broadcast(s_port_gpu_monitor *monitor) { pthread_cond_broadcast(&monitor->condition); }

struct s_thread_start {
    void (*entry)(void *);
    void *parameter;
    pthread_t thread;
};

static void *thread_main(void *start)
{
    struct s_thread_start *s = (struct s_thread_start *)start;
    s->entry(s->parameter);
    return 0;
}

void *port_gpu_thread_start(void (*entry)(void *parameter), void *parameter)
{
    struct s_thread_start *start = (struct s_thread_start *)calloc(1, sizeof(struct s_thread_start));
    if (start == 0)
        return 0;
    start->entry = entry;
    start->parameter = parameter;
    if (pthread_create(&start->thread, 0, thread_main, start) != 0) {
        free(start);
        return 0;
    }
    return start;
}

void port_gpu_thread_join(void *thread)
{
    struct s_thread_start *start = (struct s_thread_start *)thread;
    if (start == 0)
        return;
    pthread_join(start->thread, 0);
    free(start);
}

void port_gpu_thread_set_low_priority(void)
{
#ifdef SCHED_BATCH
    struct sched_param sp = { 0 };
    pthread_setschedparam(pthread_self(), SCHED_BATCH, &sp);
#endif
    setpriority(PRIO_PROCESS, (id_t)syscall(SYS_gettid), 10);
}

unsigned int port_gpu_cpu_count(void)
{
    const long count = sysconf(_SC_NPROCESSORS_ONLN);
    return count > 0 ? (unsigned int)count : 1u;
}

double port_gpu_thread_cpu_ms(void)
{
    struct timespec now;
    if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &now) != 0)
        return -1.0;
    return (double)now.tv_sec * 1000.0 + (double)now.tv_nsec / 1.0e6;
}
