// port/sdk/kernel (Linux host): host directory and case-folding hooks for storage.c.
#define _GNU_SOURCE
#include "storage.h"
#include "storage_host.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>

#define PORT_ERROR_SUCCESS 0ul
#define PORT_ERROR_ACCESS_DENIED 5ul
#define PORT_ERROR_GEN_FAILURE 31ul
#define PORT_ERROR_DISK_FULL 112ul

static unsigned long port_win32_from_errno(int err)
{
    switch (err) {
    case 0: return PORT_ERROR_SUCCESS;
    case EACCES: case EPERM: case EROFS: return PORT_ERROR_ACCESS_DENIED;
    case ENOSPC: return PORT_ERROR_DISK_FULL;
    default: return PORT_ERROR_GEN_FAILURE;
    }
}

int port_host_executable_directory(char *path, unsigned long size)
{
    if (size < 2)
        return 0;
    ssize_t n = readlink("/proc/self/exe", path, size - 1);
    if (n <= 0)
        return 0;
    path[n] = 0;
    char *slash = strrchr(path, '/');
    if (slash == 0)
        return 0;
    if (slash == path)
        slash[1] = 0;
    else
        *slash = 0;
    return 1;
}

int port_host_user_data_directory(char *path, unsigned long size)
{
    const char *xdg = getenv("XDG_DATA_HOME");
    int written = -1;
    if (xdg != 0 && xdg[0] != 0) {
        written = snprintf(path, size, "%s/halo3", xdg);
    } else {
        const char *home = getenv("HOME");
        if (home != 0 && home[0] != 0)
            written = snprintf(path, size, "%s/.local/share/halo3", home);
    }
    return written > 0 && (unsigned long)written < size;
}

int port_host_directory_exists(const char *path)
{
    struct stat info;
    return stat(path, &info) == 0 && S_ISDIR(info.st_mode);
}

unsigned long port_host_make_directories(const char *path)
{
    char work[PORT_STORAGE_PATH];
    size_t len = strlen(path);
    if (len == 0 || len >= sizeof(work))
        return PORT_ERROR_GEN_FAILURE;
    memcpy(work, path, len + 1);

    for (size_t i = 1; i <= len; i++) {
        if (work[i] != '/' && work[i] != 0)
            continue;
        char saved = work[i];
        work[i] = 0;
        if (work[0] != 0 && mkdir(work, 0777) != 0 && errno != EEXIST) {
            int err = errno;
            struct stat info;
            if (!(err == EISDIR || (stat(work, &info) == 0 && S_ISDIR(info.st_mode))))
                return port_win32_from_errno(err);
        }
        work[i] = saved;
    }
    return PORT_ERROR_SUCCESS;
}

static unsigned long port_empty_fd(int parent_fd, const char *dir_name)
{
    int fd = parent_fd < 0
        ? open(dir_name, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC)
        : openat(parent_fd, dir_name, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC);
    if (fd < 0)
        return port_win32_from_errno(errno);
    DIR *dir = fdopendir(fd);
    if (dir == 0) {
        int err = errno;
        close(fd);
        return port_win32_from_errno(err);
    }

    unsigned long result = PORT_ERROR_SUCCESS;
    struct dirent *entry;
    while ((entry = readdir(dir)) != 0) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        struct stat info;
        if (fstatat(fd, entry->d_name, &info, AT_SYMLINK_NOFOLLOW) != 0) {
            result = port_win32_from_errno(errno);
            break;
        }
        if (S_ISDIR(info.st_mode)) {
            result = port_empty_fd(fd, entry->d_name);
            if (result != PORT_ERROR_SUCCESS)
                break;
            if (unlinkat(fd, entry->d_name, AT_REMOVEDIR) != 0) {
                result = port_win32_from_errno(errno);
                break;
            }
        } else {
            if (unlinkat(fd, entry->d_name, 0) != 0) {
                result = port_win32_from_errno(errno);
                break;
            }
        }
    }
    closedir(dir);
    return result;
}

unsigned long port_host_empty_directory(const char *path)
{
    struct stat info;
    if (lstat(path, &info) != 0)
        return errno == ENOENT ? PORT_ERROR_SUCCESS : port_win32_from_errno(errno);
    if (!S_ISDIR(info.st_mode))
        return PORT_ERROR_GEN_FAILURE;
    return port_empty_fd(-1, path);
}

void port_host_match_case(char *path, unsigned long root_length)
{
    size_t len = strlen(path);
    if (len <= root_length)
        return;

    size_t pos = root_length;
    while (pos < len) {
        while (pos < len && path[pos] == '/')
            pos++;
        if (pos >= len)
            break;
        size_t end = pos;
        int has_wildcard = 0;
        while (end < len && path[end] != '/') {
            if (path[end] == '*' || path[end] == '?')
                has_wildcard = 1;
            end++;
        }
        if (has_wildcard)
            break;

        char saved_end = path[end];
        path[end] = 0;
        struct stat info;
        if (stat(path, &info) != 0) {
            // Open parent directory and look for a case-insensitive match of the same length.
            char saved_sep = path[pos - 1];
            path[pos - 1] = 0;
            const char *parent = path[0] != 0 ? path : "/";
            DIR *dir = opendir(parent);
            path[pos - 1] = saved_sep;
            if (dir != 0) {
                size_t comp_len = end - pos;
                struct dirent *entry;
                while ((entry = readdir(dir)) != 0) {
                    if (strlen(entry->d_name) == comp_len && strcasecmp(entry->d_name, path + pos) == 0) {
                        memcpy(path + pos, entry->d_name, comp_len);
                        break;
                    }
                }
                closedir(dir);
            }
        }
        path[end] = saved_end;
        pos = end;
    }
}

// ---------------------------------------------------------------------------------------------------------------
// Whole-file reads and durable replacement, for the files the port keeps in its data directory (storage.h).

#define PORT_ERROR_FILE_NOT_FOUND 2ul
#define PORT_ERROR_PATH_NOT_FOUND 3ul
#define PORT_ERROR_SHARING_VIOLATION 32ul
#define PORT_ERROR_INSUFFICIENT_BUFFER 122ul

static unsigned long port_file_error(int err)
{
    if (err == ENOENT)
        return PORT_ERROR_FILE_NOT_FOUND;
    if (err == ENOTDIR)
        return PORT_ERROR_PATH_NOT_FOUND;
    return port_win32_from_errno(err);
}

unsigned long port_host_read_file(const char *path, void *buffer, unsigned long capacity, unsigned int *size)
{
    *size = 0;
    int fd = open(path, O_RDONLY | O_CLOEXEC);
    if (fd < 0)
        return port_file_error(errno);
    struct stat info;
    if (fstat(fd, &info) != 0) {
        int err = errno;
        close(fd);
        return port_file_error(err);
    }
    if (!S_ISREG(info.st_mode)) {
        close(fd);
        return PORT_ERROR_GEN_FAILURE;
    }
    if (info.st_size < 0 || (unsigned long long)info.st_size > capacity) {
        *size = info.st_size > 0 && (unsigned long long)info.st_size <= 0xfffffffful ? (unsigned long)info.st_size
                                                                                      : 0xfffffffful;
        close(fd);
        return PORT_ERROR_INSUFFICIENT_BUFFER;
    }
    unsigned long done = 0;
    while (done < (unsigned long)info.st_size) {
        ssize_t n = read(fd, (char *)buffer + done, (unsigned long)info.st_size - done);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0)
            break; // the file shrank under us: report what was read
        done += (unsigned long)n;
    }
    close(fd);
    *size = done;
    return PORT_ERROR_SUCCESS;
}

// The directory of `path` flushed, so a rename into it survives a power loss.
static void port_sync_parent(const char *path)
{
    char parent[PORT_STORAGE_PATH];
    size_t len = strlen(path);
    if (len >= sizeof(parent))
        return;
    memcpy(parent, path, len + 1);
    char *slash = strrchr(parent, '/');
    if (slash == 0)
        return;
    if (slash == parent)
        slash[1] = 0;
    else
        *slash = 0;
    int fd = open(parent, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (fd >= 0) {
        fsync(fd);
        close(fd);
    }
}

unsigned long port_host_write_file_atomic(const char *path, const void *data, unsigned long size)
{
    char temporary[PORT_STORAGE_PATH];
    size_t len = strlen(path);
    if (len + 5 > sizeof(temporary))
        return PORT_ERROR_GEN_FAILURE;
    memcpy(temporary, path, len);
    memcpy(temporary + len, ".tmp", 5);
    int fd = open(temporary, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0666);
    if (fd < 0)
        return port_file_error(errno);
    unsigned long done = 0;
    int err = 0;
    while (done < size) {
        ssize_t n = write(fd, (const char *)data + done, size - done);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0) {
            err = n < 0 ? errno : ENOSPC;
            break;
        }
        done += (unsigned long)n;
    }
    if (err == 0 && fsync(fd) != 0)
        err = errno;
    if (close(fd) != 0 && err == 0)
        err = errno;
    if (err == 0 && rename(temporary, path) != 0)
        err = errno;
    if (err != 0) {
        unlink(temporary);
        return port_file_error(err);
    }
    port_sync_parent(path);
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_rename_file(const char *from, const char *to)
{
    if (rename(from, to) != 0)
        return port_file_error(errno);
    port_sync_parent(to);
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_lock_file(const char *path)
{
    int fd = open(path, O_RDWR | O_CREAT | O_CLOEXEC, 0666);
    if (fd < 0)
        return port_file_error(errno);
    while (flock(fd, LOCK_EX | LOCK_NB) != 0) {
        int err = errno;
        if (err == EINTR)
            continue;
        close(fd);
        return err == EWOULDBLOCK ? PORT_ERROR_SHARING_VIOLATION : port_file_error(err);
    }
    return PORT_ERROR_SUCCESS; // the descriptor stays open: the lock lasts as long as the process
}

unsigned long port_host_remove_file(const char *path)
{
    return unlink(path) == 0 ? PORT_ERROR_SUCCESS : port_file_error(errno);
}

unsigned long port_host_remove_directory(const char *path)
{
    return rmdir(path) == 0 ? PORT_ERROR_SUCCESS : port_file_error(errno);
}

unsigned long port_host_list_directory(const char *path, int (*visit)(void *context, const char *name, int is_directory),
    void *context)
{
    int fd = open(path, O_RDONLY | O_DIRECTORY | O_CLOEXEC);
    if (fd < 0)
        return errno == ENOENT ? PORT_ERROR_PATH_NOT_FOUND : port_file_error(errno);
    DIR *dir = fdopendir(fd);
    if (dir == 0) {
        int err = errno;
        close(fd);
        return port_file_error(err);
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != 0) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        struct stat info;
        int is_directory = fstatat(fd, entry->d_name, &info, 0) == 0 && S_ISDIR(info.st_mode);
        if (!visit(context, entry->d_name, is_directory))
            break;
    }
    closedir(dir);
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_disk_space(const char *path, unsigned long long *total, unsigned long long *available)
{
    struct statvfs info;
    if (statvfs(path, &info) != 0)
        return port_file_error(errno);
    *total = (unsigned long long)info.f_blocks * info.f_frsize;
    *available = (unsigned long long)info.f_bavail * info.f_frsize;
    return PORT_ERROR_SUCCESS;
}

// ---- streamed files: a file is its descriptor plus one (so no file is the null pointer) ----

#define PORT_ERROR_HANDLE_EOF 38ul

static int port_stream_fd(void *file)
{
    return (int)(intptr_t)file - 1;
}

static void *port_stream_file(int fd)
{
    return (void *)(intptr_t)(fd + 1);
}

unsigned long port_host_open_read(const char *path, void **file, unsigned long long *size)
{
    *file = 0;
    *size = 0;
    int fd = open(path, O_RDONLY | O_CLOEXEC | O_LARGEFILE);
    if (fd < 0)
        return port_file_error(errno);
    struct stat64 info;
    const int status = fstat64(fd, &info);
    if (status != 0 || !S_ISREG(info.st_mode)) {
        int err = status != 0 ? errno : EISDIR;
        close(fd);
        return port_file_error(err);
    }
    *file = port_stream_file(fd);
    *size = (unsigned long long)info.st_size;
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_read_at(void *file, unsigned long long offset, void *buffer, unsigned long size)
{
    unsigned long done = 0;
    while (done < size) {
        ssize_t n = pread64(port_stream_fd(file), (char *)buffer + done, size - done, (off64_t)(offset + done));
        if (n < 0 && errno == EINTR)
            continue;
        if (n < 0)
            return port_file_error(errno);
        if (n == 0)
            return PORT_ERROR_HANDLE_EOF;
        done += (unsigned long)n;
    }
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_create(const char *path, void **file)
{
    *file = 0;
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC | O_LARGEFILE, 0666);
    if (fd < 0)
        return port_file_error(errno);
    *file = port_stream_file(fd);
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_write(void *file, const void *data, unsigned long size)
{
    unsigned long done = 0;
    while (done < size) {
        ssize_t n = write(port_stream_fd(file), (const char *)data + done, size - done);
        if (n < 0 && errno == EINTR)
            continue;
        if (n <= 0)
            return port_file_error(n < 0 ? errno : ENOSPC);
        done += (unsigned long)n;
    }
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_close(void *file, int flush)
{
    int fd = port_stream_fd(file), err = 0;
    if (flush && fsync(fd) != 0)
        err = errno;
    if (close(fd) != 0 && err == 0 && flush)
        err = errno;
    return port_file_error(err);
}
