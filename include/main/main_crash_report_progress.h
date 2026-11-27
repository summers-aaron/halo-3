#ifndef MAIN_MAIN_CRASH_REPORT_PROGRESS_H
#define MAIN_MAIN_CRASH_REPORT_PROGRESS_H

#include "main/main_crash_report_progress_types.h"

// State of the crash report upload, shared between main.cpp (which starts the upload and polls it) and
// progress_finish_crash_report (the upload's completion callback). Every field is written with an lwsync
// followed by an interlocked exchange.

typedef char s_main_crash_report_progress_size[(sizeof(s_main_crash_report_progress) == 0x14) ? 1 : -1];

extern s_main_crash_report_progress g_main_crash_upload_progress;

extern "C" void progress_finish_crash_report(bool report_failed, void *context);

#endif // MAIN_MAIN_CRASH_REPORT_PROGRESS_H
