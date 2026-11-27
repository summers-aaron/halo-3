#ifndef MAIN_MAIN_CRASH_REPORT_PROGRESS_TYPES_H
#define MAIN_MAIN_CRASH_REPORT_PROGRESS_TYPES_H

#include "multithreading/synchronized_value.h"

struct s_main_crash_report_progress
{
    c_synchronized_long active;
    c_synchronized_long complete;
    c_synchronized_long failed;
    c_synchronized_long current;
    c_synchronized_long total;
};

typedef char main_crash_report_progress_size_check[(sizeof(s_main_crash_report_progress) == 0x14) ? 1 : -1];

#endif
