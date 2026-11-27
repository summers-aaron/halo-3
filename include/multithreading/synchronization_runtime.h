#ifndef MULTITHREADING_SYNCHRONIZATION_RUNTIME_H
#define MULTITHREADING_SYNCHRONIZATION_RUNTIME_H
#include <xtl.h>
// Kernel entry points are not declared by every XDK xtl.h configuration.
extern "C" {
void RtlEnterCriticalSection(RTL_CRITICAL_SECTION *critical_section);
void RtlLeaveCriticalSection(RTL_CRITICAL_SECTION *critical_section);
unsigned long RtlTryEnterCriticalSection(RTL_CRITICAL_SECTION *critical_section);
void RtlInitializeCriticalSectionAndSpinCount(RTL_CRITICAL_SECTION *critical_section, unsigned long spin_count);
}
#endif
