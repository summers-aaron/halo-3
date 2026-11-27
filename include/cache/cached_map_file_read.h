#ifndef CACHE_CACHED_MAP_FILE_READ_H
#define CACHE_CACHED_MAP_FILE_READ_H



class c_synchronized_long;

extern "C" long cached_map_file_read_header(long index, long offset, long size, void *buffer,
    c_synchronized_long *bytes_read, c_synchronized_long *done, long category, long priority);

#endif // CACHE_CACHED_MAP_FILE_READ_H
