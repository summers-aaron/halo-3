#ifndef CSERIES_SYSTEM_TIME_H
#define CSERIES_SYSTEM_TIME_H

// XDK winbase.h system-time declarations, usable without the full XTL surface.
#ifndef _XTL_
typedef struct _SYSTEMTIME {
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
extern "C" void __stdcall GetLocalTime(LPSYSTEMTIME system_time);
#endif

#endif
