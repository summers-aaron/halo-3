#ifndef CACHE_CACHE_FILES_GLOBALS_TYPES_H
#define CACHE_CACHE_FILES_GLOBALS_TYPES_H

#if defined(_XBOX) && !defined(__clang__)
#include <stddef.h>

// Shared cache enable/override state at 0x828a1eaf; override is at +0xc7.
// enabled is a byte: shell_test_dvd_presence stores it with a plain stb (no bool normalize).
// Tag-only view: the bytes between are owned by other subsystems' globals
// (scenario zone masks, shell/thread/main state, upload_debug, saved_game_files).
struct cache_files_progress_globals
{
    unsigned char enabled;
    char other_subsystem_globals[0xc6];
    bool override;
};
typedef char cache_files_override_offset[(offsetof(cache_files_progress_globals, override) == 0xc7) ? 1 : -1];
typedef char cache_files_globals_size[(sizeof(cache_files_progress_globals) == 0xc8) ? 1 : -1];

#define CACHE_FILES_ENABLED (g_cache_files_globals.enabled)
#define CACHE_FILES_OVERRIDE (g_cache_files_globals.override)
#else
extern "C" {
extern unsigned char g_cache_files_enabled;
extern bool g_cache_files_override;
}
#define CACHE_FILES_ENABLED g_cache_files_enabled
#define CACHE_FILES_OVERRIDE g_cache_files_override
#endif

#endif
