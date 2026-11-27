// port/sdk/kernel: the Xbox's storage devices, mapped onto host directories (storage.c). Each host's file functions
// translate the game's Xbox paths with port_storage_path; the host hooks it needs are in storage_host.h.
#ifndef PORT_KERNEL_STORAGE_H
#define PORT_KERNEL_STORAGE_H

#include "../runtime/port_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PORT_STORAGE_PATH 1024 // bytes, the terminator included

enum { PORT_STORAGE_READ, PORT_STORAGE_WRITE };

// Translates an Xbox path ("d:\maps\shared.map") to a host path. access is PORT_STORAGE_WRITE for anything that
// creates, changes or removes. Returns 1, or 0 with the Win32 error in *error: no such drive or not mounted
// (ERROR_PATH_NOT_FOUND), writing to the disc (ERROR_WRITE_PROTECT) or to a read-only drive (ERROR_ACCESS_DENIED), a
// ".." component (ERROR_INVALID_NAME), too long (ERROR_FILENAME_EXCED_RANGE).
int port_storage_path(const char *xbox_path, int access, char host_path[PORT_STORAGE_PATH], unsigned long *error);
// The host path of `name` in the port's own data directory (HALO3_DATA, else the host's user data directory: the parent
// of the cache0 / cache1 drives, which the game may format), the directory created. For files the port keeps for
// itself, such as the renderer's pipeline cache. A name may hold directories ("profiles/<xuid>/achievements.bin"), which
// are created. Returns 1, or 0 when there is no data directory or the path is too long.
int port_storage_data_file(const char *name, char host_path[PORT_STORAGE_PATH]);
// Whole-file access to the files of the data directory (port_storage_data_file). Each returns 0 (ERROR_SUCCESS) or a
// Win32 error. Read: *size the file's length; ERROR_FILE_NOT_FOUND when there is none, ERROR_INSUFFICIENT_BUFFER when it
// holds more than `capacity` bytes. Write replaces the file atomically and durably (<name>.tmp, flushed, renamed over
// it): a crash or power loss leaves the old contents or the new ones. Set aside renames the file to <name>.corrupt, for a
// file the port could not use, so that the next write does not destroy what a user may still want to recover. Lock
// makes the file when missing and holds it for this process until the process ends (port_host_lock_file), for what
// only one process running on a data directory may have, such as the console's identity (port/sdk/online/lan.c):
// ERROR_SHARING_VIOLATION while another process holds it.
unsigned int PORT_MS_ABI port_storage_data_read(const char *name, void *buffer, unsigned int capacity, unsigned int *size);
unsigned int PORT_MS_ABI port_storage_data_write(const char *name, const void *data, unsigned int size);
unsigned int PORT_MS_ABI port_storage_data_set_aside(const char *name);
unsigned int port_storage_data_lock(const char *name);
// Drives made at run time: a content package opened by XContentCreateEx is the drive "<root name>:" over its host
// directory (port/sdk/kernel/content.c). Names are matched ignoring case; d, game, cache, cache0 and cache1 cannot be
// mounted over. A read-only drive (downloadable content) refuses every path translated for writing with
// ERROR_ACCESS_DENIED. Mount returns 0, ERROR_INVALID_NAME, ERROR_ALREADY_EXISTS (the name is in use) or
// ERROR_TOO_MANY_OPEN_FILES (16 are mounted); unmount 0 or ERROR_NOT_FOUND. port_storage_mounted_root says whether
// the name is mounted, and copies its host directory when `host_directory` is not 0.
unsigned int port_storage_mount(const char *name, const char *host_directory, int read_only);
unsigned int port_storage_unmount(const char *name);
int port_storage_mounted_root(const char *name, char host_directory[PORT_STORAGE_PATH]);
int port_volume_information(const char *root, char *volume_name, unsigned int volume_name_size,
    unsigned int *serial_number, unsigned int *max_component_length, unsigned int *file_system_flags,
    char *file_system_name, unsigned int file_system_name_size, unsigned int *error);

#ifdef __cplusplus
}
#endif

#endif
