// port/sdk/kernel (both hosts): the console's hard drive as the one content device (content.h). A console keeps each
// content package in a file of its own on the drive; here a package is a directory under the data directory
// (port_storage_data_file):
//   content/<owner XUID, 16 hex digits>/<title id, 8 hex digits>/<content type, 8 hex digits>/<host name>/
//   content/<owner>/<title>/<type>/<host name>.header
// The directory holds the package's files (the game reaches them through the mounted drive, "<root>:\mmiof.bmf"); the
// header beside it holds the package's own file name and display name (XCONTENT_DATA). The host name is the file name
// in lower case (the Xbox's file names ignore case, Linux's do not) with every byte but a-z 0-9 _ - and an inner .
// written as %XX, so any file name the title makes is one host directory on both hosts.
//
// Downloadable content (marketplace packages every profile shares: owner 0, XCONTENTTYPE_MARKETPLACE) comes from more
// places, so that a map pack can be dropped in as it was downloaded:
//   - STFS package files ("CON ", "LIVE", "PIRS", stfs.h) of the title and of that type, in the type directory itself
//     (content/0000000000000000/<title>/00000002/<package file>, a console drive's layout) or in the drop-in
//     directory (HALO3_DLC, else <data>/dlc). Such a package is named by its content id (40 upper-case hex digits, a
//     console's name for it) and displayed by its header's name. Opening it installs it once, under
//     <data>/dlc_cache/<content id>/: every file extracted and every block checked against the package's hash
//     tables, then <content id>.installed written beside it (the content id, the top hash table's SHA-1 and the
//     package's size: a changed package is installed again). The drive is that directory;
//   - directories in the drop-in directory: a package already extracted, named and displayed as the directory is
//     (at most 40 printable ASCII characters a drive name can hold), its drive the directory itself.
// Both are read-only, as downloaded content is on a console: they open with XCONTENTFLAG_OPENEXISTING or
// OPENALWAYS; a CREATENEW finds them (ERROR_ALREADY_EXISTS); CREATEALWAYS, TRUNCATEEXISTING, XContentDelete and any
// write through the drive are ERROR_ACCESS_DENIED. A name found in several places is the first: the type directory's
// package directories, its STFS files, then the drop-in directory's.
#include "content.h"
#include "stfs.h"
#include "storage.h"
#include "storage_host.h"
#include "../runtime/port_runtime.h"

typedef unsigned int port_dword;
typedef unsigned long port_size;

extern char *getenv(const char *name);
void PORT_MS_ABI Sleep(port_dword milliseconds); // kernel_linux.c, win32_windows.c

#define PORT_ERROR_SUCCESS 0ul
#define PORT_ERROR_FILE_NOT_FOUND 2ul
#define PORT_ERROR_PATH_NOT_FOUND 3ul
#define PORT_ERROR_TOO_MANY_OPEN_FILES 4ul
#define PORT_ERROR_ACCESS_DENIED 5ul
#define PORT_ERROR_SHARING_VIOLATION 32ul
#define PORT_ERROR_DUP_NAME 52ul
#define PORT_ERROR_INVALID_PARAMETER 87ul
#define PORT_ERROR_DISK_FULL 112ul
#define PORT_ERROR_INVALID_NAME 123ul
#define PORT_ERROR_ALREADY_EXISTS 183ul
#define PORT_ERROR_FILENAME_EXCED_RANGE 206ul
#define PORT_ERROR_FILE_CORRUPT 1392ul

#define PORT_CONTENT_HOST_NAME (PORT_CONTENT_FILE_NAME * 3 + 1) // every byte escaped, and the terminator
#define PORT_CONTENT_OPEN_CAPACITY 16                            // packages open at once (as many as drives mount)
#define PORT_CONTENT_LIST_CAPACITY 256                           // packages of one owner, title and type listed
#define PORT_CONTENT_HOST_ENTRY 256                              // a host directory entry's name and its terminator
#define PORT_CONTENT_DLC_NAME 40                                 // a drop-in directory's longest name
#define PORT_XCONTENTTYPE_MARKETPLACE 2ul

enum { k_create_new = 1, k_create_always, k_open_existing, k_open_always, k_truncate_existing };
enum { k_disposition_none, k_disposition_created, k_disposition_opened };

static volatile char port_content_busy;

static void port_content_lock(void)
{
    while (__atomic_test_and_set(&port_content_busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}

static void port_content_unlock(void)
{
    __atomic_clear(&port_content_busy, __ATOMIC_RELEASE);
}

static port_size port_length(const char *s)
{
    port_size n = 0;
    while (s[n] != 0)
        n++;
    return n;
}

static int port_same_text(const char *a, const char *b)
{
    port_size i = 0;
    while (a[i] != 0 && a[i] == b[i])
        i++;
    return a[i] == b[i];
}

static char port_lower(char c)
{
    return c >= 'A' && c <= 'Z' ? (char)(c + ('a' - 'A')) : c;
}

// ---- names and paths ----

static const char k_hex[] = "0123456789ABCDEF";

// The host name of a file name (at most PORT_CONTENT_FILE_NAME bytes, maybe unterminated). Returns 0 for an empty one.
static int port_host_name(const char *file_name, char out[PORT_CONTENT_HOST_NAME])
{
    port_size n = 0;
    for (port_size i = 0; i < PORT_CONTENT_FILE_NAME && file_name[i] != 0; i++) {
        const unsigned char c = (unsigned char)port_lower(file_name[i]);
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '-' || (c == '.' && i != 0)) {
            out[n++] = (char)c;
        } else {
            out[n++] = '%';
            out[n++] = k_hex[c >> 4];
            out[n++] = k_hex[c & 15];
        }
    }
    out[n] = 0;
    return n != 0;
}

static int port_hex_digit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return -1;
}

// The file name a host name came from (in lower case: the header keeps the real one). Returns 0 when `host_name` is
// not one port_host_name makes.
static int port_file_name_from_host(const char *host_name, char out[PORT_CONTENT_FILE_NAME])
{
    port_size n = 0;
    for (port_size i = 0; host_name[i] != 0; n++) {
        if (n == PORT_CONTENT_FILE_NAME)
            return 0;
        if (host_name[i] == '%') {
            const int high = port_hex_digit(host_name[i + 1]), low = high < 0 ? -1 : port_hex_digit(host_name[i + 2]);
            if (low < 0)
                return 0;
            out[n] = (char)(high << 4 | low);
            i += 3;
        } else {
            out[n] = host_name[i++];
        }
    }
    if (n < PORT_CONTENT_FILE_NAME)
        out[n] = 0;
    return n != 0;
}

static int port_append(char path[PORT_STORAGE_PATH], port_size *length, const char *text)
{
    const port_size n = port_length(text);
    if (*length + n >= PORT_STORAGE_PATH)
        return 0;
    __builtin_memcpy(path + *length, text, n + 1);
    *length += n;
    return 1;
}

static void port_hex_text(unsigned long long value, int digits, char *out)
{
    for (int i = digits - 1; i >= 0; i--, value >>= 4)
        out[i] = k_hex[value & 15];
    out[digits] = 0;
}

// <data>/content/<owner>/<title>/<type>, its length in *length. Returns 0 when there is no data directory.
static int port_type_directory(unsigned long long owner, port_dword title_id, port_dword content_type,
    char path[PORT_STORAGE_PATH], port_size *length)
{
    char owner_text[17], title_text[9], type_text[9];
    const char separator[2] = { PORT_HOST_SEPARATOR, 0 };
    port_hex_text(owner, 16, owner_text);
    port_hex_text(title_id, 8, title_text);
    port_hex_text(content_type, 8, type_text);
    if (!port_storage_data_file("content", path))
        return 0;
    *length = port_length(path);
    return port_append(path, length, separator) && port_append(path, length, owner_text) &&
           port_append(path, length, separator) && port_append(path, length, title_text) &&
           port_append(path, length, separator) && port_append(path, length, type_text);
}

// The package's directory and header file. Returns 0 (ERROR_SUCCESS) or ERROR_INVALID_NAME / ERROR_PATH_NOT_FOUND /
// ERROR_FILENAME_EXCED_RANGE.
static port_dword port_package_paths(const port_content_package *package, char directory[PORT_STORAGE_PATH],
    char header[PORT_STORAGE_PATH])
{
    char host_name[PORT_CONTENT_HOST_NAME];
    const char separator[2] = { PORT_HOST_SEPARATOR, 0 };
    if (!port_host_name(package->file_name, host_name))
        return PORT_ERROR_INVALID_NAME;
    port_size length = 0;
    if (!port_type_directory(package->owner, package->title_id, package->content_type, directory, &length))
        return PORT_ERROR_PATH_NOT_FOUND;
    if (!port_append(directory, &length, separator) || !port_append(directory, &length, host_name) ||
        length + sizeof(".header") > PORT_STORAGE_PATH)
        return PORT_ERROR_FILENAME_EXCED_RANGE;
    __builtin_memcpy(header, directory, length);
    __builtin_memcpy(header + length, ".header", sizeof(".header"));
    return PORT_ERROR_SUCCESS;
}

// ---- the header ----
//
// 336 bytes, little-endian: "H3CH", version 1, the FNV-1a hash of bytes 12.., the owner XUID, the title id, the
// content type, the file name (42 bytes as XCONTENT_DATA has it) and two bytes of padding, the display name (128
// UTF-16 units), 8 bytes reserved (0).

#define PORT_CONTENT_HEADER_MAGIC 0x48433348ul // "H3CH"
#define PORT_CONTENT_HEADER_VERSION 1ul
#define PORT_CONTENT_HEADER_SIZE 336ul

static void port_put32(unsigned char *p, port_dword v)
{
    p[0] = (unsigned char)v;
    p[1] = (unsigned char)(v >> 8);
    p[2] = (unsigned char)(v >> 16);
    p[3] = (unsigned char)(v >> 24);
}

static port_dword port_get32(const unsigned char *p)
{
    return (port_dword)p[0] | (port_dword)p[1] << 8 | (port_dword)p[2] << 16 | (port_dword)p[3] << 24;
}

static port_dword port_fnv1a(const unsigned char *data, port_size size)
{
    port_dword hash = 0x811c9dc5ul;
    for (port_size i = 0; i < size; i++)
        hash = (hash ^ data[i]) * 0x01000193ul;
    return hash;
}

static port_dword port_write_header(const char *path, const port_content_package *package)
{
    unsigned char bytes[PORT_CONTENT_HEADER_SIZE];
    __builtin_memset(bytes, 0, sizeof(bytes));
    port_put32(bytes + 0, PORT_CONTENT_HEADER_MAGIC);
    port_put32(bytes + 4, PORT_CONTENT_HEADER_VERSION);
    port_put32(bytes + 12, (port_dword)package->owner);
    port_put32(bytes + 16, (port_dword)(package->owner >> 32));
    port_put32(bytes + 20, package->title_id);
    port_put32(bytes + 24, package->content_type);
    __builtin_memcpy(bytes + 28, package->file_name, PORT_CONTENT_FILE_NAME);
    for (int i = 0; i < PORT_CONTENT_DISPLAY_NAME; i++) {
        bytes[72 + 2 * i] = (unsigned char)package->display_name[i];
        bytes[73 + 2 * i] = (unsigned char)(package->display_name[i] >> 8);
    }
    port_put32(bytes + 8, port_fnv1a(bytes + 12, sizeof(bytes) - 12));
    return port_host_write_file_atomic(path, bytes, sizeof(bytes));
}

// The names the header at `path` gives the package. Returns 0 when there is no usable header.
static int port_read_header(const char *path, port_content_package *package)
{
    unsigned char bytes[PORT_CONTENT_HEADER_SIZE];
    port_dword size = 0;
    if (port_host_read_file(path, bytes, sizeof(bytes), &size) != PORT_ERROR_SUCCESS || size != sizeof(bytes) ||
        port_get32(bytes) != PORT_CONTENT_HEADER_MAGIC || port_get32(bytes + 4) != PORT_CONTENT_HEADER_VERSION ||
        port_get32(bytes + 8) != port_fnv1a(bytes + 12, sizeof(bytes) - 12))
        return 0;
    __builtin_memcpy(package->file_name, bytes + 28, PORT_CONTENT_FILE_NAME);
    for (int i = 0; i < PORT_CONTENT_DISPLAY_NAME; i++)
        package->display_name[i] = (unsigned short)(bytes[72 + 2 * i] | bytes[73 + 2 * i] << 8);
    package->display_name[PORT_CONTENT_DISPLAY_NAME - 1] = 0;
    return 1;
}

// ---- open packages ----

typedef struct port_open_package {
    char root[64]; // "" when free
    char directory[PORT_STORAGE_PATH];
} port_open_package;

static port_open_package port_open_packages[PORT_CONTENT_OPEN_CAPACITY];

// Under the lock.
static port_open_package *port_find_open_root(const char *root)
{
    for (int i = 0; i < PORT_CONTENT_OPEN_CAPACITY; i++) {
        const char *open = port_open_packages[i].root;
        port_size k = 0;
        while (open[k] != 0 && port_lower(open[k]) == port_lower(root[k]))
            k++;
        if (open[0] != 0 && open[k] == 0 && root[k] == 0)
            return &port_open_packages[i];
    }
    return 0;
}

// Under the lock.
static int port_directory_is_open(const char *directory)
{
    for (int i = 0; i < PORT_CONTENT_OPEN_CAPACITY; i++)
        if (port_open_packages[i].root[0] != 0 && port_same_text(port_open_packages[i].directory, directory))
            return 1;
    return 0;
}

// Under the lock: the package's files and header removed.
static port_dword port_remove_package(const char *directory, const char *header)
{
    port_dword result = port_host_empty_directory(directory);
    if (result == PORT_ERROR_SUCCESS)
        result = port_host_remove_directory(directory);
    if (result == PORT_ERROR_SUCCESS) {
        const port_dword header_result = port_host_remove_file(header);
        if (header_result != PORT_ERROR_FILE_NOT_FOUND)
            result = header_result;
    }
    return result;
}

// Under the lock: a new, empty package.
static port_dword port_make_package(const char *directory, const char *header, const port_content_package *package)
{
    port_dword result = port_host_make_directories(directory);
    if (result == PORT_ERROR_SUCCESS)
        result = port_write_header(header, package);
    return result;
}

// ---- downloadable content: where it comes from ----

enum { k_source_package, k_source_stfs, k_source_folder };

static int port_is_dlc(unsigned long long owner, port_dword content_type)
{
    return owner == 0 && content_type == PORT_XCONTENTTYPE_MARKETPLACE;
}

static int port_same_name_ignoring_case(const char *a, const char *b)
{
    port_size i = 0;
    while (a[i] != 0 && port_lower(a[i]) == port_lower(b[i]))
        i++;
    return a[i] == 0 && b[i] == 0;
}

// The drop-in directory: HALO3_DLC, else <data>/dlc (made, so that a user finds where packages go).
static int port_drop_directory(char path[PORT_STORAGE_PATH])
{
    const char *setting = getenv("HALO3_DLC");
    if (setting != 0 && setting[0] != 0) {
        port_size length = port_length(setting);
        while (length > 1 && (setting[length - 1] == '/' || setting[length - 1] == '\\'))
            length--;
        if (length >= PORT_STORAGE_PATH)
            return 0;
        __builtin_memcpy(path, setting, length);
        path[length] = 0;
        return 1;
    }
    return port_storage_data_file("dlc", path) && port_host_make_directories(path) == PORT_ERROR_SUCCESS;
}

// A drop-in directory's name a package (and so a drive) can be named: 1 to 40 printable ASCII characters but
// \ / : * ? " < > |, no leading dot (a host's hidden directories), no trailing dot or space, not a fixed drive's.
static int port_dlc_folder_name(const char *name)
{
    port_size length = port_length(name);
    if (length == 0 || length > PORT_CONTENT_DLC_NAME || name[0] == '.' || name[length - 1] == '.' ||
        name[length - 1] == ' ')
        return 0;
    for (port_size i = 0; i < length; i++) {
        const unsigned char c = (unsigned char)name[i];
        if (c < 0x20 || c > 0x7e || c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' || c == '"' ||
            c == '<' || c == '>' || c == '|')
            return 0;
    }
    static const char *const k_fixed[] = { "d", "game", "cache", "cache0", "cache1" };
    for (port_size i = 0; i < sizeof(k_fixed) / sizeof(k_fixed[0]); i++)
        if (port_same_name_ignoring_case(name, k_fixed[i]))
            return 0;
    return 1;
}

// <directory><separator><name>. Returns 0 when it is too long.
static int port_join(char path[PORT_STORAGE_PATH], const char *directory, const char *name)
{
    const char separator[2] = { PORT_HOST_SEPARATOR, 0 };
    port_size length = 0;
    path[0] = 0;
    return port_append(path, &length, directory) && port_append(path, &length, separator) &&
           port_append(path, &length, name);
}

static int port_same_bytes(const unsigned char *a, const unsigned char *b, port_size count)
{
    for (port_size i = 0; i < count; i++)
        if (a[i] != b[i])
            return 0;
    return 1;
}

static void port_hex_bytes(const unsigned char *bytes, port_size count, char *out)
{
    for (port_size i = 0; i < count; i++) {
        out[2 * i] = k_hex[bytes[i] >> 4];
        out[2 * i + 1] = k_hex[bytes[i] & 15];
    }
    out[2 * count] = 0;
}

static unsigned char port_peek_bytes[PORT_STFS_PEEK_SIZE]; // under the lock

// Under the lock: the summary of the STFS package at `path` when it is one of title `title_id` and type
// `content_type`, and its name (the content id in hex) in `name`.
static int port_peek_package(const char *path, port_dword title_id, port_dword content_type,
    port_stfs_summary *summary, char name[PORT_CONTENT_DLC_NAME + 1])
{
    void *file = 0;
    unsigned long long size = 0;
    if (port_host_open_read(path, &file, &size) != PORT_ERROR_SUCCESS)
        return 0;
    const int read = size >= PORT_STFS_PEEK_SIZE &&
                     port_host_read_at(file, 0, port_peek_bytes, PORT_STFS_PEEK_SIZE) == PORT_ERROR_SUCCESS;
    (void)port_host_close(file, 0);
    if (!read || port_stfs_peek(port_peek_bytes, PORT_STFS_PEEK_SIZE, summary) != PORT_ERROR_SUCCESS ||
        summary->title_id != title_id || summary->content_type != content_type)
        return 0;
    port_hex_bytes(summary->content_id, sizeof(summary->content_id), name);
    return 1;
}

// ---- downloadable content: installing an STFS package ----
//
// <data>/dlc_cache/<content id>/ holds the files; <content id>.installed, 64 bytes little-endian, says they are whole:
// "H3DI", version 1, the FNV-1a hash of bytes 12.., the content id, the top hash table's SHA-1, the package's size, its
// header size. It is removed before an install starts and written (atomically) when every file is flushed.

#define PORT_INSTALL_STAMP_MAGIC 0x49443348ul // "H3DI"
#define PORT_INSTALL_STAMP_VERSION 1ul
#define PORT_INSTALL_STAMP_SIZE 64ul
#define PORT_INSTALL_CHUNK 0x40000ul

static volatile char port_install_busy;
static port_stfs port_install_package;                       // under the install lock
static unsigned char port_install_buffer[PORT_INSTALL_CHUNK]; // under the install lock

// An install can take seconds (a map pack is hundreds of megabytes), so the lock waits by sleeping.
static void port_install_lock(void)
{
    while (__atomic_test_and_set(&port_install_busy, __ATOMIC_ACQUIRE))
        Sleep(1);
}

static void port_install_unlock(void)
{
    __atomic_clear(&port_install_busy, __ATOMIC_RELEASE);
}

static unsigned long port_install_read(void *context, unsigned long long offset, void *buffer, unsigned long size)
{
    return port_host_read_at(context, offset, buffer, size);
}

static void port_install_stamp(const port_stfs *package, unsigned char stamp[PORT_INSTALL_STAMP_SIZE])
{
    __builtin_memset(stamp, 0, PORT_INSTALL_STAMP_SIZE);
    port_put32(stamp + 0, PORT_INSTALL_STAMP_MAGIC);
    port_put32(stamp + 4, PORT_INSTALL_STAMP_VERSION);
    __builtin_memcpy(stamp + 12, package->summary.content_id, 20);
    __builtin_memcpy(stamp + 32, package->summary.top_hash, 20);
    port_put32(stamp + 52, (port_dword)package->package_size);
    port_put32(stamp + 56, (port_dword)(package->package_size >> 32));
    port_put32(stamp + 60, package->summary.header_size);
    port_put32(stamp + 8, port_fnv1a(stamp + 12, PORT_INSTALL_STAMP_SIZE - 12));
}

// Text for the install's messages.
static void port_append_number(char *text, port_size *length, port_size capacity, unsigned long long value)
{
    char digits[24];
    int n = 0;
    do {
        digits[n++] = (char)('0' + value % 10);
        value /= 10;
    } while (value != 0);
    while (n > 0 && *length + 1 < capacity)
        text[(*length)++] = digits[--n];
    text[*length] = 0;
}

static void port_append_text(char *text, port_size *length, port_size capacity, const char *more)
{
    for (port_size i = 0; more[i] != 0 && *length + 1 < capacity; i++)
        text[(*length)++] = more[i];
    text[*length] = 0;
}

static void port_install_report(const char *what, const char *name, const char *source, unsigned long long bytes,
    port_dword error)
{
    char text[PORT_STORAGE_PATH + 256];
    port_size length = 0;
    text[0] = 0;
    port_append_text(text, &length, sizeof(text), "content: ");
    port_append_text(text, &length, sizeof(text), what);
    port_append_text(text, &length, sizeof(text), " downloadable content ");
    port_append_text(text, &length, sizeof(text), name);
    port_append_text(text, &length, sizeof(text), " from ");
    port_append_text(text, &length, sizeof(text), source);
    if (error != PORT_ERROR_SUCCESS) {
        port_append_text(text, &length, sizeof(text), ": error ");
        port_append_number(text, &length, sizeof(text), error);
    } else {
        port_append_text(text, &length, sizeof(text), ": ");
        port_append_number(text, &length, sizeof(text), bytes);
        port_append_text(text, &length, sizeof(text), " bytes");
    }
    port_append_text(text, &length, sizeof(text), "\n");
    port_write_error(text);
}

// The open package's files into `directory` (made, and empty).
static port_dword port_extract_package(port_stfs *package, const char *directory, unsigned long long *bytes)
{
    *bytes = 0;
    for (port_dword i = 0; i < package->entry_count; i++)
        *bytes += package->entries[i].size;
    unsigned long long total = 0, available = 0;
    port_dword result = port_host_disk_space(directory, &total, &available);
    if (result == PORT_ERROR_SUCCESS && available < *bytes)
        result = PORT_ERROR_DISK_FULL;
    for (port_dword i = 0; i < package->entry_count && result == PORT_ERROR_SUCCESS; i++) {
        char relative[PORT_STORAGE_PATH], path[PORT_STORAGE_PATH];
        result = port_stfs_path(package, i, PORT_HOST_SEPARATOR, relative, sizeof(relative));
        if (result == PORT_ERROR_SUCCESS && !port_join(path, directory, relative))
            result = PORT_ERROR_FILENAME_EXCED_RANGE;
        if (result != PORT_ERROR_SUCCESS)
            break;
        if (package->entries[i].directory) {
            result = port_host_make_directories(path);
            continue;
        }
        void *file = 0;
        result = port_host_create(path, &file);
        for (port_dword offset = 0; result == PORT_ERROR_SUCCESS && offset < package->entries[i].size;) {
            port_dword done = 0;
            result = port_stfs_read(package, i, offset, port_install_buffer, PORT_INSTALL_CHUNK, &done);
            if (result == PORT_ERROR_SUCCESS)
                result = done != 0 ? port_host_write(file, port_install_buffer, done) : PORT_ERROR_FILE_CORRUPT;
            offset += done;
        }
        if (file != 0) {
            const port_dword closed = port_host_close(file, 1);
            if (result == PORT_ERROR_SUCCESS)
                result = closed;
        }
    }
    return result;
}

// The STFS package at `source` installed (unless it already is) under <data>/dlc_cache, its directory in
// `directory`. Returns 0 or a Win32 error (ERROR_FILE_CORRUPT: a block or table that does not match its hash).
static port_dword port_install(const char *source, char directory[PORT_STORAGE_PATH])
{
    char name[PORT_CONTENT_DLC_NAME + 1], relative[sizeof("dlc_cache/") + PORT_CONTENT_DLC_NAME + 1];
    char stamp_path[PORT_STORAGE_PATH];
    unsigned char stamp[PORT_INSTALL_STAMP_SIZE], installed[PORT_INSTALL_STAMP_SIZE];
    port_install_lock();
    void *file = 0;
    unsigned long long size = 0;
    port_dword result = port_host_open_read(source, &file, &size);
    if (result == PORT_ERROR_SUCCESS) {
        port_stfs *package = &port_install_package;
        result = port_stfs_open(package, port_install_read, file, size, 1);
        if (result == PORT_ERROR_SUCCESS) {
            port_hex_bytes(package->summary.content_id, sizeof(package->summary.content_id), name);
            port_size length = 0;
            relative[0] = 0;
            (void)port_append(relative, &length, "dlc_cache/");
            (void)port_append(relative, &length, name);
            port_install_stamp(package, stamp);
            port_dword stamp_size = 0;
            if (!port_storage_data_file(relative, directory) ||
                port_length(directory) + sizeof(".installed") > PORT_STORAGE_PATH) {
                result = PORT_ERROR_FILENAME_EXCED_RANGE;
            } else {
                __builtin_memcpy(stamp_path, directory, port_length(directory));
                __builtin_memcpy(stamp_path + port_length(directory), ".installed", sizeof(".installed"));
                if (port_host_read_file(stamp_path, installed, sizeof(installed), &stamp_size) == PORT_ERROR_SUCCESS &&
                    stamp_size == sizeof(installed) && port_same_bytes(installed, stamp, sizeof(stamp)) &&
                    port_host_directory_exists(directory)) {
                    result = PORT_ERROR_SUCCESS; // installed already
                } else {
                    unsigned long long bytes = 0;
                    result = port_host_remove_file(stamp_path);
                    if (result == PORT_ERROR_FILE_NOT_FOUND || result == PORT_ERROR_PATH_NOT_FOUND)
                        result = PORT_ERROR_SUCCESS;
                    if (result == PORT_ERROR_SUCCESS && port_host_directory_exists(directory))
                        result = port_host_empty_directory(directory);
                    if (result == PORT_ERROR_SUCCESS)
                        result = port_host_make_directories(directory);
                    if (result == PORT_ERROR_SUCCESS)
                        result = port_extract_package(package, directory, &bytes);
                    if (result == PORT_ERROR_SUCCESS)
                        result = port_host_write_file_atomic(stamp_path, stamp, sizeof(stamp));
                    port_install_report(result == PORT_ERROR_SUCCESS ? "installed" : "could not install", name,
                        source, bytes, result);
                }
            }
        } else {
            port_install_report("could not open", "package", source, 0, result);
        }
        (void)port_host_close(file, 0);
    }
    port_install_unlock();
    return result;
}

port_dword PORT_MS_ABI port_content_device_space(unsigned long long *total, unsigned long long *available)
{
    char path[PORT_STORAGE_PATH];
    *total = *available = 0;
    if (!port_storage_data_file("content", path))
        return PORT_ERROR_PATH_NOT_FOUND;
    path[port_length(path) - sizeof("content")] = 0; // the data directory itself, which exists
    return port_host_disk_space(path, total, available);
}

static int port_dlc_resolve(const port_content_package *package, int *kind, char source[PORT_STORAGE_PATH]);

// Downloadable content found outside the type directory (an STFS package file, a drop-in directory): opened read-only,
// an STFS package installed first.
static port_dword port_content_open_dlc(const char *root, int kind, const char *source, port_dword creation,
    port_dword *disposition)
{
    if (creation == k_create_new)
        return PORT_ERROR_ALREADY_EXISTS;
    if (creation == k_create_always || creation == k_truncate_existing)
        return PORT_ERROR_ACCESS_DENIED;
    if (creation != k_open_existing && creation != k_open_always)
        return PORT_ERROR_INVALID_PARAMETER;
    char directory[PORT_STORAGE_PATH];
    port_dword result = PORT_ERROR_SUCCESS;
    if (kind == k_source_stfs)
        result = port_install(source, directory);
    else
        __builtin_memcpy(directory, source, port_length(source) + 1);
    if (result != PORT_ERROR_SUCCESS)
        return result;
    port_content_lock();
    port_open_package *slot = 0;
    if (port_find_open_root(root) != 0 || port_storage_mounted_root(root, 0)) {
        result = PORT_ERROR_ALREADY_EXISTS;
    } else {
        for (int i = 0; i < PORT_CONTENT_OPEN_CAPACITY && slot == 0; i++)
            if (port_open_packages[i].root[0] == 0)
                slot = &port_open_packages[i];
        result = slot == 0 ? PORT_ERROR_TOO_MANY_OPEN_FILES : port_storage_mount(root, directory, 1);
    }
    if (result == PORT_ERROR_SUCCESS) {
        __builtin_memcpy(slot->directory, directory, port_length(directory) + 1);
        __builtin_memcpy(slot->root, root, port_length(root) + 1);
        *disposition = k_disposition_opened;
    }
    port_content_unlock();
    return result;
}

port_dword PORT_MS_ABI port_content_open(const char *root, const port_content_package *package, port_dword creation,
    port_dword *disposition)
{
    char directory[PORT_STORAGE_PATH], header[PORT_STORAGE_PATH];
    *disposition = k_disposition_none;
    if (root == 0 || root[0] == 0 || port_length(root) >= sizeof(port_open_packages[0].root))
        return PORT_ERROR_INVALID_NAME;
    port_dword result = port_package_paths(package, directory, header);
    if (result != PORT_ERROR_SUCCESS)
        return result;
    if (port_is_dlc(package->owner, package->content_type)) {
        int kind = k_source_package;
        char source[PORT_STORAGE_PATH];
        port_content_lock();
        const int found = !port_host_directory_exists(directory) && port_dlc_resolve(package, &kind, source);
        port_content_unlock();
        if (found)
            return port_content_open_dlc(root, kind, source, creation, disposition);
    }
    port_content_lock();
    const int exists = port_host_directory_exists(directory);
    const int root_in_use = port_find_open_root(root) != 0 || port_storage_mounted_root(root, 0);
    int outcome = k_disposition_none;
    switch (creation) {
    case k_create_new:
        if (exists)
            result = PORT_ERROR_ALREADY_EXISTS;
        else
            outcome = k_disposition_created;
        break;
    case k_create_always:
        if (exists && !root_in_use)
            result = port_directory_is_open(directory) ? PORT_ERROR_SHARING_VIOLATION
                                                       : port_remove_package(directory, header);
        if (result == PORT_ERROR_SUCCESS)
            outcome = k_disposition_created;
        break;
    case k_open_existing:
        if (!exists)
            result = PORT_ERROR_PATH_NOT_FOUND;
        else
            outcome = k_disposition_opened;
        break;
    case k_open_always:
        outcome = exists ? k_disposition_opened : k_disposition_created;
        break;
    case k_truncate_existing:
        if (!exists)
            result = PORT_ERROR_PATH_NOT_FOUND;
        else if (root_in_use)
            result = PORT_ERROR_ALREADY_EXISTS; // a console opens the package before it empties it
        else if (port_directory_is_open(directory))
            result = PORT_ERROR_SHARING_VIOLATION;
        else if ((result = port_host_empty_directory(directory)) == PORT_ERROR_SUCCESS)
            outcome = k_disposition_opened;
        break;
    default:
        result = PORT_ERROR_INVALID_PARAMETER;
        break;
    }
    if (outcome != k_disposition_none && root_in_use) {
        // a create on a root in use still makes the package, as a console's does; nothing is mounted
        if (outcome == k_disposition_created && !port_host_directory_exists(directory))
            (void)port_make_package(directory, header, package);
        result = outcome == k_disposition_created ? PORT_ERROR_DUP_NAME : PORT_ERROR_ALREADY_EXISTS;
        outcome = k_disposition_none;
    }
    if (outcome == k_disposition_created)
        result = port_make_package(directory, header, package);
    port_open_package *slot = 0;
    if (outcome != k_disposition_none && result == PORT_ERROR_SUCCESS) {
        for (int i = 0; i < PORT_CONTENT_OPEN_CAPACITY && slot == 0; i++)
            if (port_open_packages[i].root[0] == 0)
                slot = &port_open_packages[i];
        result = slot == 0 ? PORT_ERROR_TOO_MANY_OPEN_FILES : port_storage_mount(root, directory, 0);
    }
    if (outcome != k_disposition_none && result == PORT_ERROR_SUCCESS) {
        __builtin_memcpy(slot->directory, directory, port_length(directory) + 1);
        __builtin_memcpy(slot->root, root, port_length(root) + 1);
        *disposition = (port_dword)outcome;
    }
    port_content_unlock();
    return result;
}

port_dword PORT_MS_ABI port_content_close(const char *root)
{
    if (root == 0)
        return PORT_ERROR_INVALID_PARAMETER;
    port_content_lock();
    port_open_package *open = port_find_open_root(root);
    if (open != 0) {
        (void)port_storage_unmount(open->root);
        open->root[0] = 0;
    }
    port_content_unlock();
    return open != 0 ? PORT_ERROR_SUCCESS : PORT_ERROR_INVALID_PARAMETER;
}

port_dword PORT_MS_ABI port_content_flush(const char *root)
{
    if (root == 0)
        return PORT_ERROR_INVALID_PARAMETER;
    port_content_lock();
    const int open = port_find_open_root(root) != 0;
    port_content_unlock();
    // the game's writes went to host files as it made them; what the host still caches is the host's to keep
    return open ? PORT_ERROR_SUCCESS : PORT_ERROR_INVALID_PARAMETER;
}

port_dword PORT_MS_ABI port_content_delete(const port_content_package *package)
{
    char directory[PORT_STORAGE_PATH], header[PORT_STORAGE_PATH];
    port_dword result = port_package_paths(package, directory, header);
    if (result != PORT_ERROR_SUCCESS)
        return result;
    int kind = k_source_package;
    char source[PORT_STORAGE_PATH];
    port_content_lock();
    if (!port_host_directory_exists(directory))
        result = port_is_dlc(package->owner, package->content_type) && port_dlc_resolve(package, &kind, source)
                     ? PORT_ERROR_ACCESS_DENIED // downloadable content: the game does not delete it
                     : PORT_ERROR_FILE_NOT_FOUND;
    else if (port_directory_is_open(directory))
        result = PORT_ERROR_SHARING_VIOLATION;
    else
        result = port_remove_package(directory, header);
    port_content_unlock();
    return result;
}

// ---- listing ----
//
// The packages of an owner, title and type: the type directory's package directories, then (for downloadable content)
// its STFS package files and the drop-in directory's packages, each name once, kept in file-name order (ASCII case
// ignored).

typedef struct port_list_item {
    char name[PORT_CONTENT_FILE_NAME + 1];   // the file name the title sees, terminated
    unsigned char kind;                      // k_source_package, k_source_stfs, k_source_folder
    unsigned char dropped;                   // found in the drop-in directory (else the type directory)
    char entry[PORT_CONTENT_HOST_ENTRY];     // its host directory entry
} port_list_item;

typedef struct port_listing {
    port_list_item items[PORT_CONTENT_LIST_CAPACITY];
    port_dword count; // items kept, the first PORT_CONTENT_LIST_CAPACITY in file-name order
    port_dword seen;  // packages found
    char type_directory[PORT_STORAGE_PATH];
    char drop_directory[PORT_STORAGE_PATH]; // "" when the listing has none
    // the directory being visited
    int dropped;
    int dlc;
    port_dword title_id, content_type;
} port_listing;

static port_listing port_list_scratch; // under the lock

static int port_compare_names_ignoring_case(const char *a, const char *b)
{
    port_size i = 0;
    while (a[i] != 0 && port_lower(a[i]) == port_lower(b[i]))
        i++;
    return (unsigned char)port_lower(a[i]) - (unsigned char)port_lower(b[i]);
}

static void port_list_add(port_listing *listing, const char *name, int kind, const char *entry)
{
    if (port_length(entry) >= PORT_CONTENT_HOST_ENTRY)
        return;
    for (port_dword i = 0; i < listing->count; i++)
        if (port_compare_names_ignoring_case(listing->items[i].name, name) == 0)
            return; // found before, in a place that comes first
    // insertion into file-name order
    port_dword at = listing->count;
    while (at > 0 && port_compare_names_ignoring_case(listing->items[at - 1].name, name) > 0)
        at--;
    listing->seen++;
    if (listing->count == PORT_CONTENT_LIST_CAPACITY) {
        if (at == PORT_CONTENT_LIST_CAPACITY)
            return; // after every name kept
        listing->count--; // the last name kept makes room
    }
    for (port_dword i = listing->count; i > at; i--)
        listing->items[i] = listing->items[i - 1];
    port_list_item *item = &listing->items[at];
    __builtin_memset(item->name, 0, sizeof(item->name));
    __builtin_memcpy(item->name, name, port_length(name) + 1);
    item->kind = (unsigned char)kind;
    item->dropped = (unsigned char)listing->dropped;
    __builtin_memcpy(item->entry, entry, port_length(entry) + 1);
    listing->count++;
}

static int port_list_visit(void *context, const char *name, int is_directory)
{
    port_listing *listing = (port_listing *)context;
    if (is_directory && listing->dropped) {
        if (port_dlc_folder_name(name))
            port_list_add(listing, name, k_source_folder, name);
        return 1;
    }
    if (is_directory) {
        char file_name[PORT_CONTENT_FILE_NAME + 1];
        if (port_length(name) >= PORT_CONTENT_HOST_NAME || !port_file_name_from_host(name, file_name))
            return 1;
        file_name[PORT_CONTENT_FILE_NAME] = 0;
        char again[PORT_CONTENT_HOST_NAME];
        if (!port_host_name(file_name, again) || !port_same_text(again, name))
            return 1; // not a name this file makes (another tool's directory)
        port_list_add(listing, file_name, k_source_package, name);
        return 1;
    }
    if (listing->dlc) {
        // a file: an STFS package of the title's downloadable content
        char path[PORT_STORAGE_PATH], package_name[PORT_CONTENT_DLC_NAME + 1];
        port_stfs_summary summary;
        const char *directory = listing->dropped ? listing->drop_directory : listing->type_directory;
        if (port_join(path, directory, name) &&
            port_peek_package(path, listing->title_id, listing->content_type, &summary, package_name))
            port_list_add(listing, package_name, k_source_stfs, name);
    }
    return 1;
}

// Under the lock: every package of the owner, title and type into `listing`. Returns 0 or a Win32 error.
static port_dword port_list_sources(unsigned long long owner, port_dword title_id, port_dword content_type,
    port_listing *listing)
{
    port_size length = 0;
    listing->count = listing->seen = 0;
    listing->dlc = port_is_dlc(owner, content_type);
    listing->title_id = title_id;
    listing->content_type = content_type;
    listing->drop_directory[0] = 0;
    if (!port_type_directory(owner, title_id, content_type, listing->type_directory, &length))
        return PORT_ERROR_PATH_NOT_FOUND;
    listing->dropped = 0;
    port_dword result = port_host_list_directory(listing->type_directory, port_list_visit, listing);
    if (result == PORT_ERROR_PATH_NOT_FOUND)
        result = PORT_ERROR_SUCCESS; // nothing of this type yet
    if (result == PORT_ERROR_SUCCESS && listing->dlc && port_drop_directory(listing->drop_directory)) {
        listing->dropped = 1;
        const port_dword dropped = port_host_list_directory(listing->drop_directory, port_list_visit, listing);
        if (dropped != PORT_ERROR_PATH_NOT_FOUND)
            result = dropped;
    }
    return result;
}

// Under the lock: where downloadable content the type directory does not hold comes from (an STFS package file or a
// drop-in directory), its kind in *kind. Returns 0 when there is no such package.
static int port_dlc_resolve(const port_content_package *package, int *kind, char source[PORT_STORAGE_PATH])
{
    char file_name[PORT_CONTENT_FILE_NAME + 1];
    __builtin_memcpy(file_name, package->file_name, PORT_CONTENT_FILE_NAME);
    file_name[PORT_CONTENT_FILE_NAME] = 0;
    port_listing *listing = &port_list_scratch;
    if (port_list_sources(package->owner, package->title_id, package->content_type, listing) != PORT_ERROR_SUCCESS)
        return 0;
    for (port_dword i = 0; i < listing->count; i++) {
        const port_list_item *item = &listing->items[i];
        if (item->kind == k_source_package || port_compare_names_ignoring_case(item->name, file_name) != 0)
            continue;
        *kind = item->kind;
        return port_join(source, item->dropped ? listing->drop_directory : listing->type_directory, item->entry);
    }
    return 0;
}

port_dword PORT_MS_ABI port_content_list(unsigned long long owner, port_dword title_id, port_dword content_type,
    port_dword first, port_content_package *packages, port_dword capacity, port_dword *count,
    port_dword *total)
{
    *count = *total = 0;
    port_content_lock();
    port_listing *listing = &port_list_scratch;
    port_dword result = port_list_sources(owner, title_id, content_type, listing);
    if (result == PORT_ERROR_SUCCESS) {
        *total = listing->seen;
        for (port_dword i = first; i < listing->count && *count < capacity; i++) {
            const port_list_item *item = &listing->items[i];
            port_content_package *package = &packages[(*count)++];
            __builtin_memset(package, 0, sizeof(*package));
            package->owner = owner;
            package->title_id = title_id;
            package->content_type = content_type;
            __builtin_memcpy(package->file_name, item->name, PORT_CONTENT_FILE_NAME);
            const char *directory = item->dropped ? listing->drop_directory : listing->type_directory;
            char path[PORT_STORAGE_PATH], real_name[PORT_CONTENT_HOST_NAME], package_name[PORT_CONTENT_DLC_NAME + 1];
            port_content_package named = *package;
            port_stfs_summary summary;
            if (item->kind == k_source_package && port_join(path, directory, item->entry) &&
                port_length(path) + sizeof(".header") <= PORT_STORAGE_PATH) {
                __builtin_memcpy(path + port_length(path), ".header", sizeof(".header"));
                if (port_read_header(path, &named) && port_host_name(named.file_name, real_name) &&
                    port_same_text(real_name, item->entry))
                    *package = named; // the header's names, the file name's case included
            } else if (item->kind == k_source_stfs && port_join(path, directory, item->entry) &&
                       port_peek_package(path, title_id, content_type, &summary, package_name)) {
                for (int k = 0; k < PORT_CONTENT_DISPLAY_NAME; k++)
                    package->display_name[k] = summary.display_name[k];
            }
            if (package->display_name[0] == 0) {
                // no header (a package copied in by hand, a drop-in directory): the display name is the file name
                for (int k = 0; k < PORT_CONTENT_FILE_NAME && package->file_name[k] != 0; k++)
                    package->display_name[k] = (unsigned char)package->file_name[k];
            }
        }
    }
    port_content_unlock();
    return result;
}

port_dword PORT_MS_ABI port_content_dlc_read_file(const char *folder, const char *file, void *buffer,
    port_dword capacity, port_dword *size)
{
    if (size != 0)
        *size = 0;
    if (folder == 0 || file == 0 || buffer == 0 || size == 0)
        return PORT_ERROR_INVALID_PARAMETER;
    char drop[PORT_STORAGE_PATH], dir[PORT_STORAGE_PATH], path[PORT_STORAGE_PATH];
    if (!port_drop_directory(drop) || !port_join(dir, drop, folder) || !port_join(path, dir, file))
        return PORT_ERROR_PATH_NOT_FOUND;
    return port_host_read_file(path, buffer, capacity, size);
}
