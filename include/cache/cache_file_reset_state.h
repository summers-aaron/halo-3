#ifndef CACHE_CACHE_FILE_RESET_STATE_H
#define CACHE_CACHE_FILE_RESET_STATE_H

#include "cache/cache_file_runtime_globals.h"
#include "shell/utility_drive.h"

// The cache-enable and utility-drive bytes read by sound-cache initialization.
__forceinline bool cache_files_reset_in_progress()
{
    return CACHE_FILES_ENABLED && !g_utility_drive_mounted;
}

#endif
