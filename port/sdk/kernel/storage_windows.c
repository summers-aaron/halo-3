// port/sdk/kernel (Windows host): host directory hooks for storage.c. Windows file systems are already
// case-insensitive, so port_host_match_case has nothing to rewrite.
#include "storage.h"
#include "storage_host.h"
#include "win32_host.h"

extern char *getenv(const char *name);

typedef unsigned int port_dword;

#define PORT_INVALID_HANDLE ((void *)-1)
#define PORT_FILE_ATTRIBUTE_DIRECTORY 0x10ul
#define PORT_FILE_ATTRIBUTE_READONLY 0x01ul
#define PORT_FILE_ATTRIBUTE_REPARSE_POINT 0x400ul
#define PORT_ERROR_SUCCESS 0ul
#define PORT_ERROR_FILE_NOT_FOUND 2ul
#define PORT_ERROR_GEN_FAILURE 31ul
#define PORT_ERROR_ALREADY_EXISTS 183ul

typedef struct port_win32_find_data {
    port_dword attributes;
    port_dword times_and_size[8];
    port_dword reserved[2];
    char name[260];
    char alternate_name[14];
} port_win32_find_data;

PORT_HOST_IMPORT(port_dword, GetModuleFileNameA, 12, (void *module, char *filename, port_dword size));
PORT_HOST_IMPORT(port_dword, GetFileAttributesA, 4, (const char *name));
PORT_HOST_IMPORT(int, SetFileAttributesA, 8, (const char *name, port_dword attributes));
PORT_HOST_IMPORT(int, CreateDirectoryA, 8, (const char *name, void *security));
PORT_HOST_IMPORT(int, RemoveDirectoryA, 4, (const char *name));
PORT_HOST_IMPORT(int, DeleteFileA, 4, (const char *name));
PORT_HOST_IMPORT(void *, FindFirstFileA, 8, (const char *name, port_win32_find_data *data));
PORT_HOST_IMPORT(int, FindNextFileA, 8, (void *handle, port_win32_find_data *data));
PORT_HOST_IMPORT(int, FindClose, 4, (void *handle));
PORT_HOST_IMPORT(port_dword, GetLastError, 0, (void));

static unsigned long port_w_strlen(const char *s)
{
    unsigned long n = 0;
    while (s[n] != 0)
        n++;
    return n;
}

int port_host_executable_directory(char *path, unsigned long size)
{
    port_dword n = host_GetModuleFileNameA(0, path, size);
    if (n == 0 || n >= size)
        return 0;
    while (n > 0 && path[n - 1] != '\\' && path[n - 1] != '/')
        n--;
    if (n == 0)
        return 0;
    if (n == 3 && path[1] == ':')
        path[n] = 0;
    else
        path[n - 1] = 0;
    return 1;
}

int port_host_user_data_directory(char *path, unsigned long size)
{
    const char *base = getenv("LOCALAPPDATA");
    if (base == 0 || base[0] == 0)
        base = getenv("APPDATA");
    if (base == 0 || base[0] == 0)
        return 0;
    unsigned long len = port_w_strlen(base);
    const char *suffix = "\\halo3";
    unsigned long slen = 6;
    if (len + slen + 1 > size)
        return 0;
    __builtin_memcpy(path, base, len);
    __builtin_memcpy(path + len, suffix, slen + 1);
    return 1;
}

int port_host_directory_exists(const char *path)
{
    port_dword attr = host_GetFileAttributesA(path);
    return attr != 0xfffffffful && (attr & PORT_FILE_ATTRIBUTE_DIRECTORY) != 0;
}

unsigned long port_host_make_directories(const char *path)
{
    char work[PORT_STORAGE_PATH];
    unsigned long len = port_w_strlen(path);
    if (len == 0 || len >= sizeof(work))
        return PORT_ERROR_GEN_FAILURE;
    __builtin_memcpy(work, path, len + 1);

    unsigned long start = (len >= 2 && work[1] == ':') ? 3 : 1;
    for (unsigned long i = start; i <= len; i++) {
        if (work[i] != '\\' && work[i] != '/' && work[i] != 0)
            continue;
        char saved = work[i];
        work[i] = 0;
        if (work[0] != 0 && !port_host_directory_exists(work)) {
            if (!host_CreateDirectoryA(work, 0)) {
                port_dword err = host_GetLastError();
                if (err != PORT_ERROR_ALREADY_EXISTS || !port_host_directory_exists(work))
                    return err != 0 ? err : PORT_ERROR_GEN_FAILURE;
            }
        }
        work[i] = saved;
    }
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_empty_directory(const char *path)
{
    port_dword root_attr = host_GetFileAttributesA(path);
    if (root_attr == 0xfffffffful) {
        port_dword err = host_GetLastError();
        return err == PORT_ERROR_FILE_NOT_FOUND ? PORT_ERROR_SUCCESS : err;
    }
    if ((root_attr & PORT_FILE_ATTRIBUTE_DIRECTORY) == 0 || (root_attr & PORT_FILE_ATTRIBUTE_REPARSE_POINT) != 0)
        return PORT_ERROR_GEN_FAILURE;

    char pattern[PORT_STORAGE_PATH];
    unsigned long len = port_w_strlen(path);
    if (len + 3 >= sizeof(pattern))
        return PORT_ERROR_GEN_FAILURE;
    __builtin_memcpy(pattern, path, len);
    pattern[len] = '\\';
    pattern[len + 1] = '*';
    pattern[len + 2] = 0;

    port_win32_find_data entry;
    void *find = host_FindFirstFileA(pattern, &entry);
    if (find == PORT_INVALID_HANDLE) {
        port_dword err = host_GetLastError();
        return err == PORT_ERROR_FILE_NOT_FOUND ? PORT_ERROR_SUCCESS : err;
    }

    unsigned long result = PORT_ERROR_SUCCESS;
    do {
        if ((entry.name[0] == '.' && entry.name[1] == 0)
            || (entry.name[0] == '.' && entry.name[1] == '.' && entry.name[2] == 0))
            continue;
        unsigned long nlen = port_w_strlen(entry.name);
        char child[PORT_STORAGE_PATH];
        if (len + 1 + nlen >= sizeof(child)) {
            result = PORT_ERROR_GEN_FAILURE;
            break;
        }
        __builtin_memcpy(child, path, len);
        child[len] = '\\';
        __builtin_memcpy(child + len + 1, entry.name, nlen + 1);

        if ((entry.attributes & PORT_FILE_ATTRIBUTE_READONLY) != 0)
            host_SetFileAttributesA(child, entry.attributes & ~PORT_FILE_ATTRIBUTE_READONLY);

        if ((entry.attributes & PORT_FILE_ATTRIBUTE_DIRECTORY) != 0) {
            if ((entry.attributes & PORT_FILE_ATTRIBUTE_REPARSE_POINT) == 0) {
                result = port_host_empty_directory(child);
                if (result != PORT_ERROR_SUCCESS)
                    break;
            }
            if (!host_RemoveDirectoryA(child)) {
                result = host_GetLastError();
                break;
            }
        } else {
            if (!host_DeleteFileA(child)) {
                result = host_GetLastError();
                break;
            }
        }
    } while (host_FindNextFileA(find, &entry));

    host_FindClose(find);
    return result;
}

void port_host_match_case(char *path, unsigned long root_length)
{
    (void)path; (void)root_length;
}

// ---------------------------------------------------------------------------------------------------------------
// Whole-file reads and durable replacement, for the files the port keeps in its data directory (storage.h).

#define PORT_GENERIC_READ 0x80000000ul
#define PORT_GENERIC_WRITE 0x40000000ul
#define PORT_FILE_SHARE_READ 0x00000001ul
#define PORT_CREATE_ALWAYS 2ul
#define PORT_OPEN_EXISTING 3ul
#define PORT_OPEN_ALWAYS 4ul
#define PORT_FILE_ATTRIBUTE_NORMAL 0x80ul
#define PORT_MOVEFILE_REPLACE_EXISTING 0x1ul
#define PORT_MOVEFILE_WRITE_THROUGH 0x8ul
#define PORT_ERROR_INSUFFICIENT_BUFFER 122ul

PORT_HOST_IMPORT(void *, CreateFileA, 28, (const char *name, port_dword access, port_dword share, void *security,
    port_dword disposition, port_dword flags, void *template_file));
PORT_HOST_IMPORT(port_dword, GetFileSize, 8, (void *file, port_dword *high));
PORT_HOST_IMPORT(int, ReadFile, 20, (void *file, void *buffer, port_dword bytes, port_dword *read, void *overlapped));
PORT_HOST_IMPORT(int, WriteFile, 20, (void *file, const void *buffer, port_dword bytes, port_dword *written,
    void *overlapped));
PORT_HOST_IMPORT(int, FlushFileBuffers, 4, (void *file));
PORT_HOST_IMPORT(int, CloseHandle, 4, (void *handle));
PORT_HOST_IMPORT(int, MoveFileExA, 12, (const char *from, const char *to, port_dword flags));

static unsigned long port_last_error_or_failure(void)
{
    port_dword err = host_GetLastError();
    return err != 0 ? err : PORT_ERROR_GEN_FAILURE;
}

unsigned long port_host_read_file(const char *path, void *buffer, unsigned long capacity, unsigned int *size)
{
    *size = 0;
    void *file = host_CreateFileA(path, PORT_GENERIC_READ, PORT_FILE_SHARE_READ, 0, PORT_OPEN_EXISTING,
        PORT_FILE_ATTRIBUTE_NORMAL, 0);
    if (file == PORT_INVALID_HANDLE)
        return port_last_error_or_failure();
    port_dword high = 0;
    port_dword length = host_GetFileSize(file, &high);
    if (length == 0xfffffffful && host_GetLastError() != PORT_ERROR_SUCCESS) {
        unsigned long err = port_last_error_or_failure();
        host_CloseHandle(file);
        return err;
    }
    if (high != 0 || length > capacity) {
        *size = high != 0 ? 0xfffffffful : length;
        host_CloseHandle(file);
        return PORT_ERROR_INSUFFICIENT_BUFFER;
    }
    port_dword done = 0;
    while (done < length) {
        port_dword n = 0;
        if (!host_ReadFile(file, (char *)buffer + done, length - done, &n, 0) || n == 0)
            break; // the file shrank under us: report what was read
        done += n;
    }
    host_CloseHandle(file);
    *size = done;
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_write_file_atomic(const char *path, const void *data, unsigned long size)
{
    char temporary[PORT_STORAGE_PATH];
    unsigned long len = port_w_strlen(path);
    if (len + 5 > sizeof(temporary))
        return PORT_ERROR_GEN_FAILURE;
    __builtin_memcpy(temporary, path, len);
    __builtin_memcpy(temporary + len, ".tmp", 5);
    void *file = host_CreateFileA(temporary, PORT_GENERIC_WRITE, 0, 0, PORT_CREATE_ALWAYS, PORT_FILE_ATTRIBUTE_NORMAL, 0);
    if (file == PORT_INVALID_HANDLE)
        return port_last_error_or_failure();
    unsigned long err = PORT_ERROR_SUCCESS;
    port_dword done = 0;
    while (done < size) {
        port_dword n = 0;
        if (!host_WriteFile(file, (const char *)data + done, size - done, &n, 0) || n == 0) {
            err = port_last_error_or_failure();
            break;
        }
        done += n;
    }
    if (err == PORT_ERROR_SUCCESS && !host_FlushFileBuffers(file))
        err = port_last_error_or_failure();
    if (!host_CloseHandle(file) && err == PORT_ERROR_SUCCESS)
        err = port_last_error_or_failure();
    if (err == PORT_ERROR_SUCCESS &&
        !host_MoveFileExA(temporary, path, PORT_MOVEFILE_REPLACE_EXISTING | PORT_MOVEFILE_WRITE_THROUGH))
        err = port_last_error_or_failure();
    if (err != PORT_ERROR_SUCCESS)
        host_DeleteFileA(temporary);
    return err;
}

unsigned long port_host_rename_file(const char *from, const char *to)
{
    if (!host_MoveFileExA(from, to, PORT_MOVEFILE_REPLACE_EXISTING | PORT_MOVEFILE_WRITE_THROUGH))
        return port_last_error_or_failure();
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_lock_file(const char *path)
{
    // shared with no one: while this handle is open every other open of the file fails with ERROR_SHARING_VIOLATION
    void *file = host_CreateFileA(path, PORT_GENERIC_READ | PORT_GENERIC_WRITE, 0, 0, PORT_OPEN_ALWAYS,
        PORT_FILE_ATTRIBUTE_NORMAL, 0);
    if (file == PORT_INVALID_HANDLE)
        return port_last_error_or_failure();
    return PORT_ERROR_SUCCESS; // the handle stays open: the lock lasts as long as the process
}

PORT_HOST_IMPORT(int, GetDiskFreeSpaceExA, 16, (const char *directory, unsigned long long *available,
    unsigned long long *total, unsigned long long *free_bytes));

unsigned long port_host_remove_file(const char *path)
{
    return host_DeleteFileA(path) ? PORT_ERROR_SUCCESS : port_last_error_or_failure();
}

unsigned long port_host_remove_directory(const char *path)
{
    return host_RemoveDirectoryA(path) ? PORT_ERROR_SUCCESS : port_last_error_or_failure();
}

unsigned long port_host_list_directory(const char *path, int (*visit)(void *context, const char *name, int is_directory),
    void *context)
{
    char pattern[PORT_STORAGE_PATH];
    unsigned long len = port_w_strlen(path);
    if (len + 3 > sizeof(pattern))
        return PORT_ERROR_GEN_FAILURE;
    __builtin_memcpy(pattern, path, len);
    pattern[len] = '\\';
    pattern[len + 1] = '*';
    pattern[len + 2] = 0;
    port_win32_find_data entry;
    void *find = host_FindFirstFileA(pattern, &entry);
    if (find == PORT_INVALID_HANDLE) {
        port_dword err = host_GetLastError();
        if (err == PORT_ERROR_FILE_NOT_FOUND) // the directory exists and is empty (no . entry: a drive root)
            return PORT_ERROR_SUCCESS;
        return err != 0 ? err : PORT_ERROR_GEN_FAILURE;
    }
    do {
        if ((entry.name[0] == '.' && entry.name[1] == 0) ||
            (entry.name[0] == '.' && entry.name[1] == '.' && entry.name[2] == 0))
            continue;
        if (!visit(context, entry.name, (entry.attributes & PORT_FILE_ATTRIBUTE_DIRECTORY) != 0))
            break;
    } while (host_FindNextFileA(find, &entry));
    host_FindClose(find);
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_disk_space(const char *path, unsigned long long *total, unsigned long long *available)
{
    unsigned long long free_bytes = 0;
    if (!host_GetDiskFreeSpaceExA(path, available, total, &free_bytes))
        return port_last_error_or_failure();
    return PORT_ERROR_SUCCESS;
}

// ---- streamed files: a file is its host handle ----

#define PORT_ERROR_HANDLE_EOF 38ul

unsigned long port_host_open_read(const char *path, void **file, unsigned long long *size)
{
    *file = 0;
    *size = 0;
    void *handle = host_CreateFileA(path, PORT_GENERIC_READ, PORT_FILE_SHARE_READ, 0, PORT_OPEN_EXISTING,
        PORT_FILE_ATTRIBUTE_NORMAL, 0);
    if (handle == PORT_INVALID_HANDLE)
        return port_last_error_or_failure();
    port_dword high = 0;
    const port_dword low = host_GetFileSize(handle, &high);
    if (low == 0xfffffffful && host_GetLastError() != PORT_ERROR_SUCCESS) {
        const unsigned long err = port_last_error_or_failure();
        host_CloseHandle(handle);
        return err;
    }
    *file = handle;
    *size = (unsigned long long)high << 32 | low;
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_read_at(void *file, unsigned long long offset, void *buffer, unsigned long size)
{
    unsigned long done = 0;
    while (done < size) {
        // a synchronous handle reads at the OVERLAPPED's offset and returns when the read is done
        struct {
            __SIZE_TYPE__ internal, internal_high;
            port_dword offset, offset_high;
            void *event;
        } at = { 0, 0, (port_dword)(offset + done), (port_dword)((offset + done) >> 32), 0 };
        port_dword n = 0;
        if (!host_ReadFile(file, (char *)buffer + done, size - done, &n, &at)) {
            const port_dword err = host_GetLastError();
            return err == PORT_ERROR_HANDLE_EOF ? PORT_ERROR_HANDLE_EOF : port_last_error_or_failure();
        }
        if (n == 0)
            return PORT_ERROR_HANDLE_EOF;
        done += n;
    }
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_create(const char *path, void **file)
{
    *file = 0;
    void *handle = host_CreateFileA(path, PORT_GENERIC_WRITE, 0, 0, PORT_CREATE_ALWAYS, PORT_FILE_ATTRIBUTE_NORMAL, 0);
    if (handle == PORT_INVALID_HANDLE)
        return port_last_error_or_failure();
    *file = handle;
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_write(void *file, const void *data, unsigned long size)
{
    unsigned long done = 0;
    while (done < size) {
        port_dword n = 0;
        if (!host_WriteFile(file, (const char *)data + done, size - done, &n, 0) || n == 0)
            return port_last_error_or_failure();
        done += n;
    }
    return PORT_ERROR_SUCCESS;
}

unsigned long port_host_close(void *file, int flush)
{
    unsigned long err = PORT_ERROR_SUCCESS;
    if (flush && !host_FlushFileBuffers(file))
        err = port_last_error_or_failure();
    if (!host_CloseHandle(file) && flush && err == PORT_ERROR_SUCCESS)
        err = port_last_error_or_failure();
    return err;
}
