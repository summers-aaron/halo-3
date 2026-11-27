#ifndef MAIN_MAIN_CRASH_REPORT_H
#define MAIN_MAIN_CRASH_REPORT_H

// c_main_crash_report_path: where a crash report's files go. The constructor (main.cpp, 0x1ad2d8) fills the
// root and the folder; each user builds the file path in m_path with print("%s%s%s", root, folder, file name).

#include "memory/static_arrays.h"

class c_main_crash_report_path
{
public:
    c_main_crash_report_path();
    __forceinline void clear()
    {
        m_root.clear();
        m_path.clear();
        m_folder.clear();
    }

    c_static_string<32> m_root;
    c_static_string<32> m_folder;
    c_static_string<260> m_path;
};

typedef char main_crash_report_path_size[(sizeof(c_main_crash_report_path) == 324) ? 1 : -1];

#endif // MAIN_MAIN_CRASH_REPORT_H
