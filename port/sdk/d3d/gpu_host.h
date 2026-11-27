#ifndef PORT_D3D_GPU_HOST_H
#define PORT_D3D_GPU_HOST_H

/*
 * Host threads for the renderer's background work (the pipeline prewarm of vulkan_device.cpp, which every backend's
 * pipeline creation runs on): plain host threads that run only host code (graphics API object creation), outside the
 * game's thread model. port/sdk/runtime/gpu_host_linux.c (pthreads), gpu_host_windows.c (Win32).
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_port_gpu_monitor s_port_gpu_monitor; /* a mutex and its condition variable */
s_port_gpu_monitor *port_gpu_monitor_create(void);     /* NULL on failure */
void port_gpu_monitor_destroy(s_port_gpu_monitor *monitor);
void port_gpu_monitor_lock(s_port_gpu_monitor *monitor);
void port_gpu_monitor_unlock(s_port_gpu_monitor *monitor);
/* Waits (the lock held) until port_gpu_monitor_broadcast; may also return spuriously. */
void port_gpu_monitor_wait(s_port_gpu_monitor *monitor);
void port_gpu_monitor_broadcast(s_port_gpu_monitor *monitor);
/* A thread running entry(parameter); NULL on failure. port_gpu_thread_join waits for it and releases it. */
void *port_gpu_thread_start(void (*entry)(void *parameter), void *parameter);
void port_gpu_thread_join(void *thread);
/* Lower only the calling background worker thread's OS priority so it does not preempt game/render threads. */
void port_gpu_thread_set_low_priority(void);
/* Logical processors available to the process (at least 1). */
unsigned int port_gpu_cpu_count(void);
/* The calling thread's CPU time so far, in milliseconds (HALO3_FRAME_TRACE: the draw thread's CPU time a frame, which
 * other processes on the host do not inflate); negative where the host has no such clock fine enough (Windows:
 * GetThreadTimes counts in scheduler quanta). */
double port_gpu_thread_cpu_ms(void);

#ifdef __cplusplus
}
#endif

#endif /* PORT_D3D_GPU_HOST_H */
