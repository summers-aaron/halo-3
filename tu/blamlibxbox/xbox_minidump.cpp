// blamlibxbox/xbox_minidump.obj: from the records 0x21a4b8
// @compiler xdk6130
// @flags /TP /O1 /Oc

#include "tag_files/files.h"
#include "main/main_crash_report.h"
#include "tag_files/file_reference.h"

// @address 0x21a4b8 xbox_minidump_get_file_reference
extern "C" bool xbox_minidump_get_file_reference(long unused, s_file_reference *reference)
{
    c_main_crash_report_path crash_report_path;

    crash_report_path.m_path.print("%s%s%s", crash_report_path.m_root.get_string(), crash_report_path.m_folder.get_string(),
        "minidump.sdmp");
    file_reference_create_from_path(reference, crash_report_path.m_path.get_string(), false);
    return true;
}
