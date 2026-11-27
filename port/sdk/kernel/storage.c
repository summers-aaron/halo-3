// port/sdk/kernel (both hosts): the Xbox's storage drives mapped onto host directories.
//
// Drives the game addresses:
//   d: / game:            the read-only title disc (HALO3_DISC, else <exe>/disc, <exe>, <exe>/../../local/h3disc,
//                         "./local/h3disc", "." or the user's data folder, the first with "maps" inside it; halo3.ini's
//                         maps_dir sets HALO3_DISC: port/sdk/runtime/port_config.h);
//   cache: / cache0:      utility partition 0 (cached maps, cached_fonts, preferences.dat), mounted by
//                         XMountUtilityDrive under <HALO3_DATA or user_data>/cache0;
//   cache1:               utility partition 1 (autosaves, saved film snippets), mounted under .../cache1 when
//                         utility_drive_get_mount_flags selects the dual-partition table (flag 0x10).
#include "storage.h"
#include "storage_host.h"
#include "../runtime/port_runtime.h"

extern char *getenv(const char *name);

typedef unsigned int port_dword;
typedef unsigned long port_size;

#define PORT_ERROR_SUCCESS 0ul
#define PORT_ERROR_FILE_NOT_FOUND 2ul
#define PORT_ERROR_PATH_NOT_FOUND 3ul
#define PORT_ERROR_ACCESS_DENIED 5ul
#define PORT_ERROR_WRITE_PROTECT 19ul
#define PORT_ERROR_INVALID_PARAMETER 87ul
#define PORT_ERROR_INVALID_NAME 123ul
#define PORT_ERROR_FILENAME_EXCED_RANGE 206ul
#define PORT_ERROR_MEDIA_OFFLINE 4304ul
#define PORT_ERROR_TOO_MANY_OPEN_FILES 4ul
#define PORT_ERROR_ALREADY_EXISTS 183ul
#define PORT_ERROR_NOT_FOUND 1168ul

#define PORT_STORAGE_MOUNTS 16      // drives mounted at once (port_storage_mount)
#define PORT_STORAGE_ROOT_NAME 64   // bytes of a mounted drive's name, the terminator included

static volatile char port_storage_busy;
static int port_disc_resolved;
static int port_disc_present;
static char port_disc_root[PORT_STORAGE_PATH];
static int port_utility_mounted;
static int port_utility_dual;
static char port_cache0_root[PORT_STORAGE_PATH];
static char port_cache1_root[PORT_STORAGE_PATH];

static void port_storage_lock(void)
{
    while (__atomic_test_and_set(&port_storage_busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}

static void port_storage_unlock(void)
{
    __atomic_clear(&port_storage_busy, __ATOMIC_RELEASE);
}

static port_size port_str_len(const char *s)
{
    port_size n = 0;
    while (s[n] != 0)
        n++;
    return n;
}

static char port_lower(char c)
{
    return c >= 'A' && c <= 'Z' ? (char)(c + ('a' - 'A')) : c;
}

static int port_drive_equals(const char *name, port_size len, const char *target)
{
    port_size i = 0;
    for (; i < len && target[i] != 0; i++) {
        if (port_lower(name[i]) != target[i])
            return 0;
    }
    return i == len && target[i] == 0;
}

static int port_copy_trimmed_root(char out[PORT_STORAGE_PATH], const char *src)
{
    port_size len = port_str_len(src);
    while (len > 1 && (src[len - 1] == '/' || src[len - 1] == '\\')) {
        if (len == 3 && src[1] == ':')
            break;
        len--;
    }
    if (len == 0 || len >= PORT_STORAGE_PATH)
        return 0;
    for (port_size i = 0; i < len; i++)
        out[i] = src[i] == '/' || src[i] == '\\' ? PORT_HOST_SEPARATOR : src[i];
    out[len] = 0;
    return 1;
}

static int port_join_subpath(char out[PORT_STORAGE_PATH], const char *base, const char *sub)
{
    if (!port_copy_trimmed_root(out, base))
        return 0;
    port_size len = port_str_len(out);
    if (len > 0 && out[len - 1] != PORT_HOST_SEPARATOR) {
        if (len + 1 >= PORT_STORAGE_PATH)
            return 0;
        out[len++] = PORT_HOST_SEPARATOR;
    }
    for (port_size i = 0; sub[i] != 0; i++) {
        if (len + 1 >= PORT_STORAGE_PATH)
            return 0;
        out[len++] = sub[i] == '/' || sub[i] == '\\' ? PORT_HOST_SEPARATOR : sub[i];
    }
    out[len] = 0;
    return 1;
}

static int port_try_disc_candidate(const char *candidate)
{
    char maps[PORT_STORAGE_PATH];
    if (candidate == 0 || candidate[0] == 0)
        return 0;
    if (!port_join_subpath(maps, candidate, "maps") || !port_host_directory_exists(maps))
        return 0;
    if (!port_copy_trimmed_root(port_disc_root, candidate))
        return 0;
    port_disc_present = 1;
    return 1;
}

// Under port_storage_lock.
static void port_resolve_disc_locked(void)
{
    if (port_disc_resolved)
        return;
    port_disc_resolved = 1;

    const char *override = getenv("HALO3_DISC");
    if (override != 0 && override[0] != 0) {
        if (port_copy_trimmed_root(port_disc_root, override) && port_host_directory_exists(port_disc_root))
            port_disc_present = 1;
        return;
    }

    char exe_dir[PORT_STORAGE_PATH];
    char candidate[PORT_STORAGE_PATH];
    if (port_host_executable_directory(exe_dir, sizeof(exe_dir))) {
        if (port_join_subpath(candidate, exe_dir, "disc") && port_try_disc_candidate(candidate))
            return;
        if (port_try_disc_candidate(exe_dir))
            return;
        // Build layout: <repo>/build-pc/<target>/halo3 -> <repo>/local/h3disc.
        if (port_join_subpath(candidate, exe_dir, "../../local/h3disc") && port_try_disc_candidate(candidate))
            return;
    }
    if (port_try_disc_candidate("local/h3disc"))
        return;
    if (port_try_disc_candidate("."))
        return;
    // An installed game: the maps in the user's data folder (port/sdk/runtime/port_config.h)
    if (port_host_user_data_directory(candidate, sizeof(candidate)))
        (void)port_try_disc_candidate(candidate);
}

static int port_resolve_data_root(char base[PORT_STORAGE_PATH])
{
    const char *override = getenv("HALO3_DATA");
    if (override != 0 && override[0] != 0)
        return port_copy_trimmed_root(base, override);
    if (port_host_user_data_directory(base, PORT_STORAGE_PATH))
        return 1;
    char exe_dir[PORT_STORAGE_PATH];
    if (port_host_executable_directory(exe_dir, sizeof(exe_dir)))
        return port_join_subpath(base, exe_dir, "userdata");
    return 0;
}

int port_storage_data_file(const char *name, char host_path[PORT_STORAGE_PATH])
{
    char base[PORT_STORAGE_PATH];
    if (name == 0 || name[0] == 0 || !port_resolve_data_root(base) ||
        port_host_make_directories(base) != PORT_ERROR_SUCCESS || !port_join_subpath(host_path, base, name))
        return 0;
    // a name with directories in it ("profiles/<xuid>/achievements.bin"): those directories too
    port_size parent = port_str_len(host_path);
    while (parent > 0 && host_path[parent - 1] != PORT_HOST_SEPARATOR)
        parent--;
    if (parent > port_str_len(base) + 1) {
        host_path[parent - 1] = 0;
        port_dword status = port_host_make_directories(host_path);
        host_path[parent - 1] = PORT_HOST_SEPARATOR;
        if (status != PORT_ERROR_SUCCESS)
            return 0;
    }
    return 1;
}

port_dword PORT_MS_ABI port_storage_data_read(const char *name, void *buffer, port_dword capacity, port_dword *size)
{
    char path[PORT_STORAGE_PATH];
    *size = 0;
    if (!port_storage_data_file(name, path))
        return PORT_ERROR_PATH_NOT_FOUND;
    return port_host_read_file(path, buffer, capacity, size);
}

port_dword PORT_MS_ABI port_storage_data_write(const char *name, const void *data, port_dword size)
{
    char path[PORT_STORAGE_PATH];
    if (!port_storage_data_file(name, path))
        return PORT_ERROR_PATH_NOT_FOUND;
    return port_host_write_file_atomic(path, data, size);
}

port_dword PORT_MS_ABI port_storage_data_set_aside(const char *name)
{
    static const char suffix[] = ".corrupt";
    char path[PORT_STORAGE_PATH], aside[PORT_STORAGE_PATH];
    if (!port_storage_data_file(name, path))
        return PORT_ERROR_PATH_NOT_FOUND;
    port_size len = port_str_len(path);
    if (len + sizeof(suffix) > PORT_STORAGE_PATH)
        return PORT_ERROR_FILENAME_EXCED_RANGE;
    __builtin_memcpy(aside, path, len);
    __builtin_memcpy(aside + len, suffix, sizeof(suffix));
    return port_host_rename_file(path, aside);
}

port_dword port_storage_data_lock(const char *name)
{
    char path[PORT_STORAGE_PATH];
    if (!port_storage_data_file(name, path))
        return PORT_ERROR_PATH_NOT_FOUND;
    return port_host_lock_file(path);
}

typedef enum { PORT_DRIVE_NONE, PORT_DRIVE_DISC, PORT_DRIVE_CACHE0, PORT_DRIVE_CACHE1, PORT_DRIVE_MOUNTED } port_drive_kind;

// Drives mounted at run time (port_storage_mount): the content packages XContentCreateEx opens. A free entry has an empty
// name. Under port_storage_lock.
typedef struct port_mount {
    char name[PORT_STORAGE_ROOT_NAME];
    char root[PORT_STORAGE_PATH];
    int read_only;
} port_mount;

static port_mount port_mounts[PORT_STORAGE_MOUNTS];

// Under port_storage_lock: the mount named `name` (`len` bytes, ignoring case, as the Xbox's object names do), or 0.
static port_mount *port_find_mount_locked(const char *name, port_size len)
{
    for (int i = 0; i < PORT_STORAGE_MOUNTS; i++) {
        const char *mounted = port_mounts[i].name;
        port_size k = 0;
        while (k < len && mounted[k] != 0 && port_lower(mounted[k]) == port_lower(name[k]))
            k++;
        if (mounted[0] != 0 && k == len && mounted[k] == 0)
            return &port_mounts[i];
    }
    return 0;
}

// The drive a path names, its name in *name_out / *name_length_out, and the rest of the path after the colon.
static port_drive_kind port_parse_drive(const char *xbox_path, const char **rest_out, const char **name_out,
    port_size *name_length_out)
{
    if (xbox_path == 0)
        return PORT_DRIVE_NONE;
    port_size colon = 0;
    while (xbox_path[colon] != 0 && xbox_path[colon] != ':' && xbox_path[colon] != '\\' && xbox_path[colon] != '/')
        colon++;
    if (xbox_path[colon] != ':' || colon == 0)
        return PORT_DRIVE_NONE;
    const char *rest = xbox_path + colon + 1;
    while (*rest == '\\' || *rest == '/')
        rest++;
    *rest_out = rest;
    *name_out = xbox_path;
    *name_length_out = colon;
    if (port_drive_equals(xbox_path, colon, "d") || port_drive_equals(xbox_path, colon, "game"))
        return PORT_DRIVE_DISC;
    if (port_drive_equals(xbox_path, colon, "cache") || port_drive_equals(xbox_path, colon, "cache0"))
        return PORT_DRIVE_CACHE0;
    if (port_drive_equals(xbox_path, colon, "cache1"))
        return PORT_DRIVE_CACHE1;
    return PORT_DRIVE_MOUNTED;
}

int port_storage_path(const char *xbox_path, int access, char host_path[PORT_STORAGE_PATH], unsigned long *error)
{
    unsigned long dummy = 0;
    if (error == 0)
        error = &dummy;
    *error = PORT_ERROR_SUCCESS;

    const char *rest = 0, *drive_name = 0;
    port_size drive_name_length = 0;
    port_drive_kind drive = port_parse_drive(xbox_path, &rest, &drive_name, &drive_name_length);
    if (drive == PORT_DRIVE_NONE) {
        *error = PORT_ERROR_PATH_NOT_FOUND;
        return 0;
    }
    if (drive == PORT_DRIVE_DISC && access == PORT_STORAGE_WRITE) {
        *error = PORT_ERROR_WRITE_PROTECT;
        return 0;
    }

    char root[PORT_STORAGE_PATH];
    port_storage_lock();
    int ok = 0, denied = 0;
    if (drive == PORT_DRIVE_DISC) {
        port_resolve_disc_locked();
        if (port_disc_present) {
            ok = port_copy_trimmed_root(root, port_disc_root);
        }
    } else if (drive == PORT_DRIVE_CACHE0) {
        if (port_utility_mounted)
            ok = port_copy_trimmed_root(root, port_cache0_root);
    } else if (drive == PORT_DRIVE_CACHE1) {
        if (port_utility_mounted && port_utility_dual)
            ok = port_copy_trimmed_root(root, port_cache1_root);
    } else if (drive == PORT_DRIVE_MOUNTED) {
        const port_mount *mount = port_find_mount_locked(drive_name, drive_name_length);
        if (mount != 0 && mount->read_only && access == PORT_STORAGE_WRITE)
            denied = 1;
        else if (mount != 0)
            ok = port_copy_trimmed_root(root, mount->root);
    }
    port_storage_unlock();

    if (!ok) {
        *error = denied ? PORT_ERROR_ACCESS_DENIED : PORT_ERROR_PATH_NOT_FOUND;
        return 0;
    }

    port_size root_length = port_str_len(root);
    __builtin_memcpy(host_path, root, root_length + 1);
    port_size out_len = root_length;

    while (*rest != 0) {
        const char *comp = rest;
        while (*rest != 0 && *rest != '\\' && *rest != '/')
            rest++;
        port_size comp_len = (port_size)(rest - comp);
        while (*rest == '\\' || *rest == '/')
            rest++;
        if (comp_len == 0 || (comp_len == 1 && comp[0] == '.'))
            continue;
        if (comp_len == 2 && comp[0] == '.' && comp[1] == '.') {
            *error = PORT_ERROR_INVALID_NAME;
            return 0;
        }
        for (port_size i = 0; i < comp_len; i++) {
            if (comp[i] == ':') {
                *error = PORT_ERROR_INVALID_NAME;
                return 0;
            }
        }
        if (out_len > 0 && host_path[out_len - 1] != PORT_HOST_SEPARATOR) {
            if (out_len + 1 >= PORT_STORAGE_PATH) {
                *error = PORT_ERROR_FILENAME_EXCED_RANGE;
                return 0;
            }
            host_path[out_len++] = PORT_HOST_SEPARATOR;
        }
        if (out_len + comp_len >= PORT_STORAGE_PATH) {
            *error = PORT_ERROR_FILENAME_EXCED_RANGE;
            return 0;
        }
        __builtin_memcpy(host_path + out_len, comp, comp_len);
        out_len += comp_len;
        host_path[out_len] = 0;
    }

    port_host_match_case(host_path, root_length);
    return 1;
}

// ---------------------------------------------------------------------------------------------------------------
// Xbox utility-drive and disc volume APIs.

port_dword utility_drive_get_mount_flags(port_dword flags)
{
    return flags | 0x10ul;
}

port_dword PORT_MS_ABI XMountUtilityDriveUseStfc(port_dword flags)
{
    return utility_drive_get_mount_flags(flags);
}

port_dword utility_drive_release_mount(void)
{
    port_storage_lock();
    int was_mounted = port_utility_mounted;
    port_utility_mounted = 0;
    port_utility_dual = 0;
    port_storage_unlock();
    return was_mounted ? PORT_ERROR_SUCCESS : PORT_ERROR_MEDIA_OFFLINE;
}

port_dword PORT_MS_ABI XUnmountUtilityDrive(void)
{
    return utility_drive_release_mount();
}

port_dword PORT_MS_ABI XMountUtilityDrive(port_dword flags, port_dword bytes_per_cluster, port_dword file_cache_size)
{
    (void)bytes_per_cluster; (void)file_cache_size;
    char base[PORT_STORAGE_PATH];
    char c0[PORT_STORAGE_PATH];
    char c1[PORT_STORAGE_PATH];
    if (!port_resolve_data_root(base) || !port_join_subpath(c0, base, "cache0") || !port_join_subpath(c1, base, "cache1"))
        return PORT_ERROR_PATH_NOT_FOUND;

    int dual = (flags & 0x10ul) != 0;
    int format0 = dual ? (flags & 0x01ul) != 0 : flags != 0;
    int format1 = dual && (flags & 0x02ul) != 0;

    port_dword status = port_host_make_directories(c0);
    if (status != PORT_ERROR_SUCCESS)
        return status;
    if (format0) {
        status = port_host_empty_directory(c0);
        if (status != PORT_ERROR_SUCCESS)
            return status;
    }
    if (dual) {
        status = port_host_make_directories(c1);
        if (status != PORT_ERROR_SUCCESS)
            return status;
        if (format1) {
            status = port_host_empty_directory(c1);
            if (status != PORT_ERROR_SUCCESS)
                return status;
        }
    }

    port_storage_lock();
    __builtin_memcpy(port_cache0_root, c0, port_str_len(c0) + 1);
    __builtin_memcpy(port_cache1_root, c1, port_str_len(c1) + 1);
    port_utility_dual = dual;
    port_utility_mounted = 1;
    port_storage_unlock();
    return PORT_ERROR_SUCCESS;
}

port_dword PORT_MS_ABI XMountUtilityDriveEx(port_dword flags, port_dword bytes_per_cluster, port_dword file_cache_size)
{
    return XMountUtilityDrive(flags, bytes_per_cluster, file_cache_size);
}

port_dword PORT_MS_ABI XFlushUtilityDrive(void)
{
    port_storage_lock();
    int mounted = port_utility_mounted;
    port_storage_unlock();
    return mounted ? PORT_ERROR_SUCCESS : PORT_ERROR_MEDIA_OFFLINE;
}

int port_volume_information(const char *root, char *volume_name, unsigned int volume_name_size,
    unsigned int *serial_number, unsigned int *max_component_length, unsigned int *file_system_flags,
    char *file_system_name, unsigned int file_system_name_size, unsigned int *error)
{
    char host_path[PORT_STORAGE_PATH];
    unsigned long path_error = 0;
    if (!port_storage_path(root != 0 ? root : "d:\\", PORT_STORAGE_READ, host_path, &path_error)) {
        if (error != 0)
            *error = (unsigned int)path_error;
        return 0;
    }
    if (!port_host_directory_exists(host_path)) {
        if (error != 0)
            *error = PORT_ERROR_PATH_NOT_FOUND;
        return 0;
    }
    const char *rest = 0, *drive_name = 0;
    port_size drive_name_length = 0;
    port_drive_kind drive = port_parse_drive(root != 0 ? root : "d:\\", &rest, &drive_name, &drive_name_length);
    const char *fs = drive == PORT_DRIVE_DISC ? "GDFX" : "FATX";
    port_size fs_len = port_str_len(fs);

    if (volume_name != 0 && volume_name_size > 0)
        volume_name[0] = 0;
    if (serial_number != 0)
        *serial_number = 0x4d5307e6ul;
    if (max_component_length != 0)
        *max_component_length = 42ul;
    if (file_system_flags != 0)
        *file_system_flags = 0ul;
    if (file_system_name != 0) {
        if (file_system_name_size <= fs_len) {
            if (error != 0)
                *error = PORT_ERROR_INVALID_PARAMETER;
            return 0;
        }
        __builtin_memcpy(file_system_name, fs, fs_len + 1);
    }
    if (error != 0)
        *error = PORT_ERROR_SUCCESS;
    return 1;
}

// ---------------------------------------------------------------------------------------------------------------
// Drives mounted at run time.

static int port_valid_root_name(const char *name, port_size *length)
{
    port_size n = 0;
    for (; name != 0 && name[n] != 0; n++) {
        if (name[n] == ':' || name[n] == '\\' || name[n] == '/' || n + 1 >= PORT_STORAGE_ROOT_NAME)
            return 0;
    }
    *length = n;
    // the fixed drives keep their names
    return n != 0 && !port_drive_equals(name, n, "d") && !port_drive_equals(name, n, "game") &&
           !port_drive_equals(name, n, "cache") && !port_drive_equals(name, n, "cache0") &&
           !port_drive_equals(name, n, "cache1");
}

port_dword port_storage_mount(const char *name, const char *host_directory, int read_only)
{
    port_size length = 0;
    if (!port_valid_root_name(name, &length))
        return PORT_ERROR_INVALID_NAME;
    char root[PORT_STORAGE_PATH];
    if (host_directory == 0 || !port_copy_trimmed_root(root, host_directory))
        return PORT_ERROR_FILENAME_EXCED_RANGE;
    port_dword result = PORT_ERROR_TOO_MANY_OPEN_FILES;
    port_storage_lock();
    if (port_find_mount_locked(name, length) != 0) {
        result = PORT_ERROR_ALREADY_EXISTS;
    } else {
        for (int i = 0; i < PORT_STORAGE_MOUNTS; i++) {
            if (port_mounts[i].name[0] == 0) {
                __builtin_memcpy(port_mounts[i].root, root, port_str_len(root) + 1);
                __builtin_memcpy(port_mounts[i].name, name, length + 1);
                port_mounts[i].read_only = read_only != 0;
                result = PORT_ERROR_SUCCESS;
                break;
            }
        }
    }
    port_storage_unlock();
    return result;
}

port_dword port_storage_unmount(const char *name)
{
    port_size length = 0;
    if (!port_valid_root_name(name, &length))
        return PORT_ERROR_INVALID_NAME;
    port_storage_lock();
    port_mount *mount = port_find_mount_locked(name, length);
    if (mount != 0)
        mount->name[0] = 0;
    port_storage_unlock();
    return mount != 0 ? PORT_ERROR_SUCCESS : PORT_ERROR_NOT_FOUND;
}

int port_storage_mounted_root(const char *name, char host_directory[PORT_STORAGE_PATH])
{
    port_size length = 0;
    if (!port_valid_root_name(name, &length))
        return 0;
    port_storage_lock();
    const port_mount *mount = port_find_mount_locked(name, length);
    if (mount != 0 && host_directory != 0)
        __builtin_memcpy(host_directory, mount->root, port_str_len(mount->root) + 1);
    port_storage_unlock();
    return mount != 0;
}
