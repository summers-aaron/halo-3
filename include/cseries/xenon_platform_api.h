#ifndef CSERIES_XENON_PLATFORM_API_H
#define CSERIES_XENON_PLATFORM_API_H

struct _XOVERLAPPED;

extern "C" { __declspec(noreturn) void XShowDirtyDiscErrorUI(unsigned long controller_index); }
extern "C" { unsigned long XNetGetEthernetLinkStatus(void); }
extern "C" { unsigned long XGetOverlappedResult(_XOVERLAPPED *overlapped, unsigned long *result, int wait); }
extern "C" int XNetRandom(unsigned char *pb, unsigned int cb);
#if defined(_WIN64)
extern "C" { void *__stdcall XMemAllocDefault(unsigned __int64 size, unsigned long attributes); }
#else
extern "C" { void *__stdcall XMemAllocDefault(unsigned long size, unsigned long attributes); }
#endif
extern "C" { void __stdcall XMemFreeDefault(void *address, unsigned long attributes); }
extern "C" { void __stdcall XPhysicalFree(void *address); }
extern "C" unsigned long __stdcall XNotifyDelayUI(unsigned long milliseconds);
extern "C" { unsigned long __stdcall XGetLaunchDataSize(unsigned long *size); }

#endif // CSERIES_XENON_PLATFORM_API_H
