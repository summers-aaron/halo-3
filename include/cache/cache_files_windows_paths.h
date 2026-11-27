#ifndef CACHE_CACHE_FILES_WINDOWS_PATHS_H
#define CACHE_CACHE_FILES_WINDOWS_PATHS_H

template<long k_maximum_count> class c_static_string;

extern "C" void cache_files_windows_get_canonical_map_path(const char *path, c_static_string<256> *buffer);

extern "C" bool cache_files_windows_is_external_map_path(const char *path);

#endif // CACHE_CACHE_FILES_WINDOWS_PATHS_H
