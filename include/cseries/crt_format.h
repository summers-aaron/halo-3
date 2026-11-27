#ifndef CSERIES_CRT_FORMAT_H
#define CSERIES_CRT_FORMAT_H

#include <stdarg.h>

extern "C" int vsprintf_s(char *buffer, unsigned int count, const char *format, va_list arguments);
extern "C" int _vsnprintf(char *buffer, unsigned int count, const char *format, va_list args);

#endif // CSERIES_CRT_FORMAT_H
