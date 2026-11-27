// blamlibxbox/cseries_windows.obj: from the records 0x19a308 0x19a310 0x1e4b98
// @compiler xdk6130
// @flags /TP /O1 /Oc
#include <xtl.h>
#include <string.h>
// match: 0x19a310: inlines both memsets as std/stw zero stores (without it they compile to bl memset calls)
#pragma intrinsic(memset)
#include "cseries/cseries_windows.h"
#include "tag_files/files.h"
#include "cseries/system_memory_information_types.h"
#include "main/main_crash_report.h"
#include "tag_files/file_reference.h"


void system_exit(int code)
{
    for (;;)
    {
    }
}

void system_memory_information_get(s_system_memory_information *information)
{
    MEMORYSTATUS status;
    memset(&status, 0, sizeof(status));
    status.dwLength = sizeof(status);
    GlobalMemoryStatus(&status);
    memset(information, 0, sizeof(*information));
    information->available_physical_memory = status.dwAvailPhys;
    information->total_physical_memory = status.dwTotalPhys;
}


// @address 0x1e4b98 main_get_crash_report_reference
extern "C" bool main_get_crash_report_reference(long unused, s_file_reference *reference)
{
    c_main_crash_report_path crash_report_path;

    crash_report_path.m_path.print("%s%s%s", crash_report_path.m_root.get_string(), crash_report_path.m_folder.get_string(),
        "crash_screenshot.dds");
    file_reference_create_from_path(reference, crash_report_path.m_path.get_string(), false);
    return true;
}
