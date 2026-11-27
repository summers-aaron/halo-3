// port/sdk/runtime: process entry and the few host services the stage-1 runtime needs. Host code: it never sees
// the XDK headers, and it declares the host C runtime functions it uses itself.
#ifndef PORT_RUNTIME_H
#define PORT_RUNTIME_H

#include "port_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

void PORT_MS_ABI port_write_error(const char *text);
void PORT_MS_ABI port_exit(int code);
void port_stub_called(const char *name, int can_return);
void PORT_MS_ABI port_run_atexit(void);
void port_install_crash_handler(void);

// What the title keeps in memory for its players and writes later (achievements it granted during a cinematic: the
// game writes them once the cinematic is over, port/probe/extra/achievements_exit_flush.cpp) is written out before the
// process ends on request: the game window closed (vulkan_device.cpp), XLaunchNewImage, main returning. One writer,
// set by the title side (port_set_exit_flush, NULL for none); port_exit_flush runs it once, on the calling thread.
// Never called on a fault (crash handlers, stubs, the watchdog end with port_exit alone).
void PORT_MS_ABI port_set_exit_flush(void (PORT_MS_ABI *flush)(void));
void port_exit_flush(void);

// Poisoned data (port/tools/stubgen): retail data with real values that the game does not define in source yet.
// Each datum has pages of its own; port_poison_table lists them, ending with a row of zeros.
typedef struct port_poisoned_datum {
    void *address;
    __SIZE_TYPE__ size;
    const char *name;
} port_poisoned_datum;
extern const port_poisoned_datum port_poison_table[];
extern const __SIZE_TYPE__ port_poison_count;

// Protects every poisoned datum (before the static constructors run).
void port_protect_poisoned_data(void);
// The fault handlers ask this about a faulting address: 0 when it is not a poisoned datum. Otherwise it reports the
// datum and stops, or, in the diagnostic mode (PORT_STUB_CONTINUE), reports it once, makes it accessible (it reads as
// zeros from then on) and returns 1 so the faulting access runs again.
int port_poisoned_access(const void *address);
// Host page protection (crash_windows.c, crash_linux.c). port_set_page_access: read-write (1) or no access (0).
// port_set_page_protection: one of the PORT_ACCESS_ levels. Both round the range out to whole host pages.
void port_set_page_access(void *address, unsigned long size, int accessible);
enum { PORT_ACCESS_NONE, PORT_ACCESS_READ, PORT_ACCESS_READ_WRITE };
void port_set_page_protection(void *address, unsigned long size, int access);

// Xbox physical memory (port/sdk/kernel/xapi.c): each host's XPhysicalAlloc reports a new block with the Xbox
// protection value it was asked for, and XPhysicalFree reports the block's release, so XPhysicalProtect and
// XQueryMemoryProtect work the same on both hosts.
void port_physical_allocated(void *address, unsigned long size, unsigned long protect);
void port_physical_released(void *address);

// Cryptographically secure random bytes from the host (getrandom on Linux, RtlGenRandom on Windows). Returns 1 on
// success; a host without a source reports it and stops (there is no safe fallback for key material).
int port_host_random(void *buffer, unsigned long size);

#ifdef __cplusplus
}
#endif

#endif
