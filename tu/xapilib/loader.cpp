// xapilib/loader.obj: from the records 0x90108
// @flags /TC /Ox /Os
// @address 0x90108 XGetModuleSection
#include "xapi/loader.h"
#include "xapi/xerror.h"
#include "xbox/xboxkrnl.h"

// @address 0x70d434 XexGetModuleSection

BOOL XGetModuleSection(HANDLE hModule, LPCSTR szSectionName, PVOID *pSectionData, ULONG *pSectionSize)
{
    NTSTATUS Status;

    Status = XexGetModuleSection(hModule, szSectionName, pSectionData, pSectionSize);

    if (Status < 0) {
        XapiSetLastNTError(Status);
        return 0;
    }

    return 1;
}
