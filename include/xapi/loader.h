#ifndef XAPI_LOADER_H
#define XAPI_LOADER_H

typedef long NTSTATUS;
typedef int BOOL;
typedef unsigned long ULONG;
typedef void *HANDLE;
typedef void *PVOID;
typedef const char *LPCSTR;
#ifdef __cplusplus
extern "C" {
#endif
BOOL XGetModuleSection(HANDLE module, LPCSTR section_name, PVOID *section_data, ULONG *section_size);
#ifdef __cplusplus
}
#endif

#endif // XAPI_LOADER_H
