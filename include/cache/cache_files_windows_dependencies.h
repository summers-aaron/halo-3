#ifndef CACHE_CACHE_FILES_WINDOWS_DEPENDENCIES_H
#define CACHE_CACHE_FILES_WINDOWS_DEPENDENCIES_H

#include "cache/cache_files_globals_types.h"
#include "cache/cache_file_runtime_globals.h"
struct cache_files_windows_range {
  long minimum;
  long maximum;
  long value;
};
extern "C" const cache_files_windows_range k_cache_file_ranges[5];
extern "C" void lruv_complete_request(int, int, int);

#endif // CACHE_CACHE_FILES_WINDOWS_DEPENDENCIES_H
