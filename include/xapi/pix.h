#ifndef XAPI_PIX_H
#define XAPI_PIX_H

#include <stddef.h>
#include <stdarg.h>

typedef int (*pix_debug_monitor_callback)(int event, int value);

extern "C" {
int RtlVsnprintf(char *destination, size_t count, const char *format, va_list arguments);
__declspec(dllimport) extern pix_debug_monitor_callback *KeDebugMonitorData;
void pix_set_marker(const char *format, va_list *arguments);
}

#endif // XAPI_PIX_H
