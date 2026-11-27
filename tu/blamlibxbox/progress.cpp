// blamlibxbox/progress.obj: from the records 0x1e4878 0x219d28 0x219dd8 0x219ed8
// @flags /TP /O2 /Oc
// @address 0x1e4878 progress_finish_crash_report
// @address 0x219d28 progress_new
// @address 0x219dd8 progress_update_with_description
// @address 0x219ed8 progress_done
#include "cseries/crt_xdk.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include "cseries/cseries.h"
#include "cseries/progress.h"
#include "main/main_crash_report_progress.h"

// @address 0x7e8788 progress_globals
s_progress_globals progress_globals;

// @address 0x96df30 g_main_crash_upload_progress
s_main_crash_report_progress g_main_crash_upload_progress;

// 0x1e4878
extern "C" void progress_finish_crash_report(bool report_failed, void *)
{
    g_main_crash_upload_progress.active.set(0);
    g_main_crash_upload_progress.complete.set(1);
    g_main_crash_upload_progress.failed.set(report_failed != 0);
    progress_done();
}

extern "C" void progress_new(const char *description)
{
    progress_done();
    progress_globals.currently_valid = true;
    // March: csstrnzcpy(progress_globals.description, description, NUMBEROF(progress_globals.description))
    strncpy(progress_globals.description, description, NUMBEROF(progress_globals.description));
    progress_globals.description[NUMBEROF(progress_globals.description) - 1] = 0;
    progress_globals.last_percentage = -1;
    progress_globals.last_update_time = 0;
    if (!progress_globals.disable_callbacks)
    {
        if (progress_globals.custom_callbacks.progress_new_proc)
        {
            progress_globals.custom_callbacks.progress_new_proc(progress_globals.description,
                progress_globals.custom_callbacks.progress_data);
        }
        else if (progress_globals.default_callbacks.progress_new_proc)
        {
            progress_globals.default_callbacks.progress_new_proc(progress_globals.description,
                progress_globals.default_callbacks.progress_data);
        }
    }
}

extern "C" long progress_update_with_description(long current, long total)
{
    unsigned long now = GetTickCount();
    __int64 numerator = MIN(MAX(current, 0), total);
    __int64 denominator = MAX(total, 1);
    long percentage = (long)(numerator * 100 / denominator);
    if (progress_globals.currently_valid)
    {
        progress_globals.last_percentage = percentage;
        progress_globals.last_update_time = now;
        if (!progress_globals.disable_callbacks)
        {
            if (progress_globals.custom_callbacks.progress_update_proc)
            {
                progress_globals.custom_callbacks.progress_update_proc(progress_globals.description, NULL, percentage,
                    progress_globals.custom_callbacks.progress_data);
            }
            else if (progress_globals.default_callbacks.progress_update_proc)
            {
                progress_globals.default_callbacks.progress_update_proc(progress_globals.description, NULL, percentage,
                    progress_globals.default_callbacks.progress_data);
            }
        }
    }
    return percentage;
}

extern "C" void progress_done(void)
{
    if (progress_globals.currently_valid)
    {
        if (!progress_globals.disable_callbacks)
        {
            if (progress_globals.custom_callbacks.progress_done_proc)
            {
                progress_globals.custom_callbacks.progress_done_proc(progress_globals.description,
                    progress_globals.custom_callbacks.progress_data);
            }
            else if (progress_globals.default_callbacks.progress_done_proc)
            {
                progress_globals.default_callbacks.progress_done_proc(progress_globals.description,
                    progress_globals.default_callbacks.progress_data);
            }
        }
        progress_globals.currently_valid = false;
    }
}
