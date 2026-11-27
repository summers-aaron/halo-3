// port/sdk/kernel: what storage.c needs from the host (storage_linux.c, storage_windows.c).
#ifndef PORT_KERNEL_STORAGE_HOST_H
#define PORT_KERNEL_STORAGE_HOST_H

#ifdef _WIN32
#define PORT_HOST_SEPARATOR '\\'
#else
#define PORT_HOST_SEPARATOR '/'
#endif

// The directory holding the executable, and the host's per-user application data directory with "halo3" appended.
// Each returns 1, or 0 when the host does not say.
int port_host_executable_directory(char *path, unsigned long size);
int port_host_user_data_directory(char *path, unsigned long size);

int port_host_directory_exists(const char *path);
// Creates the directory and any missing parents. Returns 0 (ERROR_SUCCESS) or a Win32 error.
unsigned long port_host_make_directories(const char *path);
// Removes everything inside the directory, never following a link out of it. Returns 0 or a Win32 error.
unsigned long port_host_empty_directory(const char *path);

// Called on every translated path, whose first root_length bytes are the drive's host directory. On a host whose
// file names are case-sensitive, each later component that does not exist as spelled takes the spelling of an
// entry that matches it ignoring case (the Xbox's file systems ignore case). Components with wildcards are kept.
void port_host_match_case(char *path, unsigned long root_length);

// The whole file into `buffer`. Returns 0 (ERROR_SUCCESS) with *size its length; ERROR_FILE_NOT_FOUND or
// ERROR_PATH_NOT_FOUND when it does not exist; ERROR_INSUFFICIENT_BUFFER, *size its length, when it holds more than
// `capacity` bytes; or another Win32 error.
unsigned long port_host_read_file(const char *path, void *buffer, unsigned long capacity, unsigned int *size);
// Replaces the file with `size` bytes durably: they are written to <path>.tmp, flushed to the device, and the
// temporary file renamed over `path` (on Linux the directory is flushed too), so after a crash or a power loss the
// file holds either its old contents or its new ones, never a mix. Returns 0 or a Win32 error (the temporary removed).
unsigned long port_host_write_file_atomic(const char *path, const void *data, unsigned long size);
// Renames `from` to `to`, replacing `to`. Returns 0 or a Win32 error.
unsigned long port_host_rename_file(const char *from, const char *to);
// Opens the file (made when missing) and keeps it locked for this process alone until the process ends, however it
// ends: the host drops the lock with the process, so a crash leaves nothing to clear. Returns 0 (ERROR_SUCCESS),
// ERROR_SHARING_VIOLATION while another process holds the file, or another Win32 error.
unsigned long port_host_lock_file(const char *path);
// Removes a file, or an empty directory. Each returns 0 or a Win32 error (ERROR_FILE_NOT_FOUND / ERROR_PATH_NOT_FOUND
// when it does not exist).
unsigned long port_host_remove_file(const char *path);
unsigned long port_host_remove_directory(const char *path);
// Calls visit(context, name, is_directory) for each entry of the directory but . and .., in the host's order, until
// visit returns 0. Returns 0 or a Win32 error (ERROR_PATH_NOT_FOUND when it does not exist).
unsigned long port_host_list_directory(const char *path, int (*visit)(void *context, const char *name, int is_directory),
    void *context);
// The size of the volume holding `path` and the bytes free on it for this user. Returns 0 or a Win32 error.
unsigned long port_host_disk_space(const char *path, unsigned long long *total, unsigned long long *available);

// Streamed files, for what is too large to hold whole (a content package installed from its STFS file, content.c).
// Open-read gives a file read at any offset and its size; read-at moves exactly `size` bytes (ERROR_HANDLE_EOF when
// the file ends first). Create makes (or empties) a file written from its start. Close of a created file with `flush`
// flushes it to the device first and returns that flush's error. Each returns 0 or a Win32 error.
unsigned long port_host_open_read(const char *path, void **file, unsigned long long *size);
unsigned long port_host_read_at(void *file, unsigned long long offset, void *buffer, unsigned long size);
unsigned long port_host_create(const char *path, void **file);
unsigned long port_host_write(void *file, const void *data, unsigned long size);
unsigned long port_host_close(void *file, int flush);

#endif
