// blamlibxbox/files_windows.obj: from the records 0x1985d8 0x1ad5b0 0x1ad6d0 0x1ad7d0 0x1ad930 0x1ad9e0 0x1adb00 0x1adbf8 0x1ae008 0x1ae0c8 0x1ae198 0x1ae220 0x1ae2d8 0x1ae380 0x1ae418 0x1ae4a0 0x1ae500 0x1ae838 0x1ae8c8 0x1ae928 0x1aea50 0x1aeac0
// @flags /TP /O2 /J /Oc
// @address 0x1ae8c8 ?file_path_remove_name@@YAXPAD@Z
// @address 0x1ae928 ?file_path_split@@YAXPADPAPAD111_N@Z
#include <xtl.h>
#include <new>
#include <string.h>
#include "tag_files/files.h"
#include "tag_files/files_windows.h"
#include "tag_files/files_windows_globals.h"
#include "tag_files/files_windows_types.h"
#include "cseries/cseries.h"
#include "shell/utility_drive.h"
#include "tag_files/file_reference.h"

// @address 0x8a1ead g_utility_drive_mounted

struct named_object {
    unsigned long signature;
    unsigned short flags;
    short location;
    char name[1];
};
namespace {
static __forceinline s_file_reference *file_reference_get_info(s_file_reference *reference)
{
    return reference;
}
}

static __forceinline void find_files_start_with_search_spec(
    s_find_file_data *find_file_data, unsigned long flags, const s_file_reference *file_reference, const char *search_spec)
{
    s_find_file_data *data = find_file_data;
    const s_file_reference *info = file_reference;
    for (unsigned long index = 0; index < 16; ++index)
        data->handles[index] = (void *)-1;
    data->flags = flags;
    data->depth = 0;
    data->location = info->location;
    csstrnzcpy(data->path, info->path, 256);
    csstrnzcpy(data->search_spec, search_spec, 256);
}

// @address 0x1ad930 ?file_exists@@YA_NPBUs_file_reference@@@Z
bool file_exists(const s_file_reference *reference)
{
    char path[256];
    bool exists;

    path[0] = 0;
    memset(path + 1, 0, sizeof(path) - 1);
    exists = false;
    file_location_get_full_path(reference->location, reference->path, path, 256);
    if (GetFileAttributesA(path) != 0xffffffff)
        exists = true;
    else {
        unsigned long error = GetLastError();
        if (error == 5)
            exists = true;
        else if (error != 2 && error != 3)
            file_error("file_exists", (named_object *)reference, 0, false);
    }
    return exists;
}

// @address 0x1ad9e0 ?file_rename@@YA_NPAUs_file_reference@@PBD@Z
bool file_rename(s_file_reference *reference, const char *new_name)
{
    char old_path[256] = "";
    char new_path[256] = "";
    char name[256];
    bool success = false;

    file_location_get_full_path(reference->location, reference->path, old_path, 256);
    csstrnzcpy(new_path, old_path, 256);
    file_path_remove_name(new_path);
    file_path_add_name(new_path, 256, new_name);
    if (MoveFileA(old_path, new_path) != 0) {
        file_path_remove_name(reference->path);
        file_path_add_name(reference->path, 256, new_name);
        success = true;
    }
    file_reference_get_name(reference, 13, name, 256);
    if (file_is_cache_path(name) && g_utility_drive_mounted != 0)
        XFlushUtilityDrive();
    if (!success)
        file_error("file_rename", (named_object *)reference, 0, false);
    return success;
}

// @address 0x1adb00 ?file_copy_to@@YA_NPBUs_file_reference@@0_N@Z
bool file_copy_to(const s_file_reference *source, const s_file_reference *destination, bool overwrite)
{
    char source_path[256] = "";
    char destination_path[256] = "";
    char name[256];
    bool success;

    success = false;
    file_create_parent_directories_if_not_present(destination);
    file_location_get_full_path(source->location, source->path, source_path, 256);
    file_location_get_full_path(destination->location, destination->path, destination_path, 256);
    if (CopyFileA(source_path, destination_path, 0) != 0)
        success = true;
    file_reference_get_name(destination, 13, name, 256);
    if (file_is_cache_path(name) && g_utility_drive_mounted != 0)
        XFlushUtilityDrive();
    if (!success)
        file_error("file_copy_to", (named_object *)source, (named_object *)destination, false);
    return success;
}

// @address 0x1adbf8 ?file_open@@YA_NPAUs_file_reference@@KPAK@Z
// match: 0x1adbf8: pointer __restrict qualifier prevents alias reload
bool file_open(s_file_reference *reference, unsigned long flags, unsigned long *__restrict error)
{
    char path[256] = "";
    char name[256];
    unsigned long access = 0;
    unsigned long share = 0;
    unsigned long attributes = 128;
    void *handle;
    bool success = false;
    unsigned long *error_code = error;

    *error_code = 0;
    file_location_get_full_path(reference->location, reference->path, path, 256);
    if ((flags & 1) != 0)
        access |= 0x80000000;
    if ((flags & 2) != 0)
        access |= 0x40000000;
    if ((flags & 2) == 0 || (flags & 8) != 0)
        share = 1;
    if ((flags & 32) != 0) {
        attributes &= ~128;
        attributes |= 256;
    }
    if ((flags & 64) != 0)
        attributes = 0x4000000;
    if ((flags & 128) != 0)
        attributes = 0x10000000;
    if ((flags & 256) != 0)
        attributes = 0x8000000;
    handle = CreateFileA(path, access, share, 0, 3, attributes, 0);
    if (handle != 0 && handle != (void *)-1) {
        reference->handle = handle;
        reference->position = 0;
        success = true;
    } else {
        switch (GetLastError()) {
        case ERROR_FILE_NOT_FOUND: *error_code = 1; break;
        case ERROR_PATH_NOT_FOUND: *error_code = 3; break;
        case ERROR_INVALID_DRIVE: *error_code = 4; break;
        case ERROR_ACCESS_DENIED: *error_code = 2; break;
        case ERROR_SHARING_VIOLATION: *error_code = 5; break;
        case ERROR_INVALID_NAME: *error_code = 6; break;
        default: *error_code = 6; break;
        }
    }
    if (success && (flags & 4) != 0) {
        reference->position = SetFilePointer(reference->handle, 0, 0, 2);
        if (reference->position == -1) {
            CloseHandle(reference->handle);
            reference->position = 0;
            success = false;
            reference->handle = (void *)-1;
        }
    }
    if ((flags & 2) != 0) {
        file_reference_get_name(reference, 13, name, 256);
        if (file_is_cache_path(name))
            reference->flags |= 2;
        else
            reference->flags &= ~2;
    }
    if (!success)
        file_error("file_open", (named_object *)reference, 0, (flags >> 4) & 1);
    return success;
}

// @address 0x1ae008 ?file_close@@YA_NPAUs_file_reference@@@Z
bool file_close(s_file_reference *reference)
{
    bool closed = false;

    if (reference->handle != 0 && reference->handle != (void *)-1) {
        if (CloseHandle(reference->handle)) {
            reference->handle = (void *)-1;
            reference->position = 0;
            closed = true;
        }
    }
    else
        SetLastError(6);
    if ((reference->flags & 2) != 0) {
        if (g_utility_drive_mounted != 0)
            XFlushUtilityDrive();
    }
    if (!closed)
        file_error("file_close", (named_object *)reference, 0, false);
    return closed;
}

// @address 0x1ae0c8 ?file_set_position@@YA_NPAUs_file_reference@@K_N@Z
bool file_set_position(s_file_reference *reference, unsigned long position, bool suppress_errors)
{
    s_file_reference *info = file_reference_get_info(reference);
    bool success = false;
    if (info->position == position)
    {
        success = true;
    }
    else
    {
        if (info->handle == 0 || info->handle == (void *)-1)
            SetLastError(6);
        else
            info->position = SetFilePointer(info->handle, position, 0, 0);
        success = info->position != 0xffffffff;
        if (success)
        {
            if ((info->flags & 2) != 0)
            {
                if (g_utility_drive_mounted != 0)
                    XFlushUtilityDrive();
            }
        }
        if (!success)
            file_error("file_set_position", (named_object *)reference, 0, suppress_errors);
    }
    return success;
}

// @address 0x1ae198 ?file_get_eof@@YAKPBUs_file_reference@@@Z
unsigned long file_get_eof(const s_file_reference *reference)
{
    unsigned long size = 0xffffffff;

    if (reference->handle != 0 && reference->handle != (void *)-1)
        size = GetFileSize(reference->handle, 0);
    else
        SetLastError(6);
    if (size == 0xffffffff)
        file_error("file_get_eof", (named_object *)reference, 0, false);
    return size;
}

// @address 0x1ae220 ?file_read@@YA_NPAUs_file_reference@@K_NPAX@Z
bool file_read(s_file_reference *reference, unsigned long size, bool suppress_errors, void *buffer)
{
    unsigned long bytes_read = 0;
    bool success = false;

    if (reference->handle == 0 || reference->handle == (void *)-1) {
        SetLastError(6);
        success = false;
    }
    else if (ReadFile(reference->handle, buffer, size, &bytes_read, 0) != 0) {
        if (bytes_read == size)
            success = true;
        else
            SetLastError(38);
    }
    reference->position += bytes_read;
    if (!success)
        file_error("file_read", (named_object *)reference, 0, suppress_errors);
    return success;
}

// @address 0x1ae2d8 ?file_write@@YA_NPAUs_file_reference@@KPBX@Z
bool file_write(s_file_reference *reference, unsigned long size, const void *buffer)
{
    unsigned long bytes_written = 0;
    bool success = false;

    if (reference->handle != 0 && reference->handle != (void *)-1) {
        if (WriteFile(reference->handle, buffer, size, &bytes_written, 0) != 0 && bytes_written == size)
            success = true;
    }
    else
        SetLastError(6);
    reference->position += bytes_written;
    if (!success)
        file_error("file_write", (named_object *)reference, 0, false);
    return success;
}

// @address 0x1ae380 ?file_get_size@@YA_NPBUs_file_reference@@PAK@Z
bool file_get_size(const s_file_reference *reference, unsigned long *size)
{
    char path[256];
    WIN32_FILE_ATTRIBUTE_DATA info;
    bool success;

    path[0] = 0;
    memset(path + 1, 0, sizeof(path) - 1);
    success = false;
    file_location_get_full_path(reference->location, reference->path, path, 256);
    if (GetFileAttributesExA(path, GetFileExInfoStandard, &info) != 0) {
        *size = info.nFileSizeLow;
        success = true;
    }
    if (!success)
        file_error("file_get_size", (named_object *)reference, 0, false);
    return success;
}

// @address 0x1ae418 ?find_files_start@@YAXPAUs_find_file_data@@KPBUs_file_reference@@PBD@Z
void find_files_start(s_find_file_data *find_file_data, unsigned long flags, const s_file_reference *file_reference, const char *search_spec)
{
    find_files_start_with_search_spec(find_file_data, flags, file_reference, search_spec);
}

// @address 0x1ae4a0 ?find_files_end@@YAXPAUs_find_file_data@@@Z
void find_files_end(s_find_file_data *find_file_data)
{
    s_find_file_data *data = find_file_data;
    short index = data->depth;
    void **handles = data->handles;
    while (index >= 0) {
        if (handles[index] != 0 && handles[index] != (void *)-1) {
            CloseHandle(handles[index]);
            handles[index] = (void *)-1;
        }
        --index;
    }
}


namespace {
static __forceinline bool string_is_absolute_path(const char *path)
{
    bool result = strstr(path, ":\\") > path;
    return result;
}

static __forceinline void files_windows_location_get_full_path(short location, const char *path, char (&full_path)[256])
{
    full_path[0] = 0;
    if (!string_is_absolute_path(path))
        csstrnzcpy(full_path, "d:\\", 256);
    csstrnzcat(full_path, path, 256);
}
}

static __forceinline s_file_reference *files_windows_file_reference_create(s_file_reference *reference, short location)
{
    memset(reference, 0, sizeof(*reference));
    reference->signature = 'filo';
    reference->location = location;
    reference->handle = (void *)-1;
    return reference;
}


// @address 0x1ae500 ?find_files_next@@YA_NPAUs_find_file_data@@PAUs_file_reference@@PAUs_file_last_modification_date@@@Z
bool find_files_next(s_find_file_data *find_file_data, s_file_reference *file_reference, s_file_last_modification_date *file_last_modification_date)
{
    short depth = find_file_data->depth;
    char *path = find_file_data->path;
    void **handles = find_file_data->handles;
    STATIC_ASSERT(sizeof(WIN32_FIND_DATAA) == sizeof(find_file_data->native_find_data), native_find_data_size);
    STATIC_ASSERT(__alignof(WIN32_FIND_DATAA) <= __alignof(s_find_file_data) &&
        offsetof(s_find_file_data, native_find_data) % __alignof(WIN32_FIND_DATAA) == 0, native_find_data_alignment);
    WIN32_FIND_DATAA *data = new (find_file_data->native_find_data) WIN32_FIND_DATAA;
    char full_path[256];
    bool found;

    full_path[0] = 0;
    memset(full_path + 1, 0, sizeof(full_path) - 1);
    found = false;
    while (depth >= 0) {
        if (handles[depth] == 0 || handles[depth] == (void *)-1) {
            files_windows_location_get_full_path(find_file_data->location, path, full_path);
            file_path_add_name(full_path, 256, find_file_data->search_spec);
            handles[depth] = FindFirstFileA(full_path, data);
            if (handles[depth] == 0 || handles[depth] == (void *)-1) {
                file_path_remove_name(path);
                depth--;
                continue;
            }
        } else if (FindNextFileA(handles[depth], data) == 0) {
            CloseHandle(handles[depth]);
            handles[depth] = (void *)-1;
            file_path_remove_name(path);
            depth--;
            continue;
        }
        if ((data->dwFileAttributes & 16) != 0) {
            if (strcmp(data->cFileName, ".") == 0 || strcmp(data->cFileName, "..") == 0)
                continue;
            bool descended = false;
            if ((find_file_data->flags & 1) != 0) {
                file_path_add_name(path, 256, data->cFileName);
                descended = true;
                depth++;
            }
            if ((find_file_data->flags & 2) != 0 || (find_file_data->flags & 4) != 0) {
                files_windows_file_reference_create(file_reference, find_file_data->location);
                file_path_add_name(file_reference->path, 256, path);
                if (!descended)
                    file_path_add_name(file_reference->path, 256, data->cFileName);
                if (file_last_modification_date != 0)
                    memcpy(file_last_modification_date, &data->ftLastWriteTime, sizeof(*file_last_modification_date));
                found = true;
                break;
            }
        } else if ((find_file_data->flags & 4) != 0 || (find_file_data->flags & 2) == 0) {
            files_windows_file_reference_create(file_reference, find_file_data->location);
            file_path_add_name(file_reference->path, 256, path);
            file_reference_set_name(file_reference, data->cFileName);
            if (file_last_modification_date != 0)
                memcpy(file_last_modification_date, &data->ftLastWriteTime, sizeof(*file_last_modification_date));
            found = true;
            break;
        }
    }
    find_file_data->depth = depth;
    return found;
}

void file_path_remove_name(char *path)
{
    short index = (short)strlen(path);
    while (index > 0 && path[index] != '\\') {
        index--;
    }
    path[index] = 0;
}

void file_path_split(char *path, char **drive, char **directory, char **root, char **extension, bool is_file)
{
    char terminator;
    char *end;
    int offset;
    signed char character;

    offset = (int)strlen(path);
    terminator = 0;
    end = path + offset;
    *drive = end;
    *directory = end;
    *root = end;
    *extension = end;
    for (; 0 < offset; offset = offset - 1) {
        character = *(signed char *)(path + offset);
        if (character == '.') {
            if (is_file && **root == '\0' && **extension == '\0') {
                path[offset] = terminator;
                *extension = path + (offset + 1);
            }
        }
        else if (character == '\\') {
            if (!is_file || **root != '\0') {
                if (**directory == '\0') {
                    *directory = path + (offset + 1);
                }
            }
            else {
                path[offset] = terminator;
                *root = path + (offset + 1);
            }
        }
    }
    if (is_file && **root == '\0') {
        *root = path;
        return;
    }
    if (*root != path) {
        *drive = path;
        return;
    }
}

// @address 0x1ae838 file_path_add_name
extern "C" void file_path_add_name(char *path, long maximum_path_length, const char *name)
{
    if (*name != 0) {
        long length = (long)strlen(path);
        char *end = path + length;
        if (end != path && end[-1] != '\\') {
            *end = '\\';
            end++;
            *end = 0;
            length++;
        }
        unsigned int size = 256 - length;
        strncpy(end, name, size);
        end[size - 1] = 0;
    }
}


// @address 0x1aea50 ?file_location_get_full_path@@YAXFPBDPADJ@Z
void file_location_get_full_path(short location, const char *path, char *full_path, long full_path_length)
{
    full_path[0] = 0;
    if (!string_is_absolute_path(path))
        csstrnzcpy(full_path, "d:\\", full_path_length);
    csstrnzcat(full_path, path, full_path_length);
}

// @address 0x11df825 g_suppress_file_errors
extern "C" {
__declspec(thread) char g_suppress_file_errors;
}

// @address 0x1ad5b0 ?file_create@@YA_NPAUs_file_reference@@@Z
bool file_create(s_file_reference *reference)
{
    char path[256] = {0};
    char name[256];
    bool success = false;
    file_location_get_full_path(reference->location, reference->path, path, 256);
    if (reference->flags & 1) {
        void *handle = CreateFileA(path, 0x40000000, 0, 0, 2, 128, 0);
        if (handle && handle != (void *)-1) {
            CloseHandle(handle);
            success = true;
        }
    } else if (CreateDirectoryA(path, 0)) {
        success = true;
    }
    if (success) {
        if (reference->flags & 2) {
            file_reference_get_name(reference, 13, name, 256);
            if (file_is_cache_path(name) && g_utility_drive_mounted)
                XFlushUtilityDrive();
        }
    }
    if (!success)
        file_error("file_create", (named_object *)reference, 0, false);
    return success;
}

// @address 0x1ad6d0 ?file_delete@@YA_NPAUs_file_reference@@@Z
bool file_delete(s_file_reference *reference)
{
    char path[256] = "";
    char name[256];
    bool success = false;

    file_location_get_full_path(reference->location, reference->path, path, 256);
    if ((reference->flags & 1) != 0) {
        if (SetFileAttributesA(path, 128) != 0 && DeleteFileA(path) != 0)
            success = true;
    } else if (RemoveDirectoryA(path) != 0) {
        success = true;
    }
    file_reference_get_name(reference, 13, name, 256);
    if (file_is_cache_path(name) && g_utility_drive_mounted != 0)
        XFlushUtilityDrive();
    if (!success)
        file_error("file_delete", (named_object *)reference, 0, false);
    return success;
}

// @address 0x1ad7d0 ?file_delete_recursive@@YA_NPAUs_file_reference@@@Z
bool file_delete_recursive(s_file_reference *reference)
{
    char path[256];
    s_find_file_data find_file_data;
    s_file_reference child;
    bool result;

    path[0] = 0;
    memset(path + 1, 0, sizeof(path) - 1);
    result = false;
    file_location_get_full_path(reference->location, reference->path, path, 256);
    if ((reference->flags & 1) != 0) {
        if (SetFileAttributesA(path, 128) != 0 && DeleteFileA(path) != 0)
            result = true;
    } else {
        result = true;
        find_files_start(&find_file_data, 4, reference, "*.*");
        while (find_files_next(&find_file_data, &child, 0)) {
            if (!file_delete_recursive(&child)) {
                result = false;
                break;
            }
        }
        find_files_end(&find_file_data);
        if (result && RemoveDirectoryA(path) == 0)
            result = false;
    }
    if (!result)
        file_error("file_delete_recursive", (named_object *)reference, 0, false);
    return result;
}

// @address 0x1aeac0 file_error
extern "C" void file_error(const char *function_name, named_object *first, named_object *second, bool quiet)
{
  char call[1024];
  char result[1024];
  int value;

  value = GetLastError();
  if (g_suppress_file_errors == 0 && !quiet) {
    if (second != 0) {
      csnzprintf(call, 0x400, "%s('%s', '%s')", function_name, first->name, second->name);
    }
    else {
      csnzprintf(call, 0x400, "%s('%s')", function_name, first->name);
    }
    csnzprintf(result, 0x400, "0x%08x", value);
  }
  SetLastError(0);
}
// @address 0x1985d8 file_is_cache_path
extern "C" bool file_is_cache_path(const char *path)
{
    bool result = false;
    if (g_utility_drive_mounted != 0 && string_is_absolute_path(path)) {
        if (strstr(path, "cache0:\\") || strstr(path, "cache1:\\"))
            result = true;
    }
    return result;
}
