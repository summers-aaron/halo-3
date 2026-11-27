// port/sdk/kernel: the console's hard drive as a content device (content.c), for the XContent* calls of
// port/sdk/online/xlive_offline.cpp. A content package (a saved game, a film, a map or game variant, a screenshot,
// downloadable content) is a host directory; opening one mounts it as the drive "<root name>:" (storage.h).
// Downloadable content can also be an STFS package file (stfs.h), installed when first opened, or a directory dropped
// into HALO3_DLC / <data>/dlc; it is read-only (content.c).
#ifndef PORT_KERNEL_CONTENT_H
#define PORT_KERNEL_CONTENT_H

#ifdef __cplusplus
extern "C" {
#endif

#define PORT_CONTENT_DEVICE_HARD_DRIVE 1u // XCONTENTDEVICEID of the one device, an XCONTENTDEVICETYPE_HDD
#define PORT_CONTENT_FILE_NAME 42         // XCONTENT_MAX_FILENAME_LENGTH
#define PORT_CONTENT_DISPLAY_NAME 128     // XCONTENT_MAX_DISPLAYNAME_LENGTH

// A package: who owns it, whose and what it is, its names (XCONTENT_DATA's, the file name not always terminated).
typedef struct port_content_package {
    unsigned long long owner; // the XUID of the profile it belongs to; 0 for content every profile shares (DLC)
    unsigned int title_id;
    unsigned int content_type; // XCONTENTTYPE_SAVEDGAME 1, XCONTENTTYPE_MARKETPLACE 2, ...
    char file_name[PORT_CONTENT_FILE_NAME];
    unsigned short display_name[PORT_CONTENT_DISPLAY_NAME]; // UTF-16
    char reserved[6]; // to a multiple of 8 bytes: the same size under the game's ABI (8-aligned) and System V's (4)
} port_content_package;
typedef char port_content_package_size[sizeof(port_content_package) == 320 ? 1 : -1];

// The size of the drive and its free bytes (those of the host volume the data directory is on). Returns 0 or a Win32
// error.
unsigned int PORT_MS_ABI port_content_device_space(unsigned long long *total, unsigned long long *available);

// XContentCreateEx: makes or opens the package as `creation` (XCONTENTFLAG_CREATENEW 1 .. TRUNCATEEXISTING 5) says and
// mounts it as "<root>:". Returns 0 with *disposition XCONTENT_CREATED (1) or XCONTENT_OPENED (2), or a Win32 error with
// *disposition 0: ERROR_ALREADY_EXISTS (CREATENEW and it exists; or an open on a root in use), ERROR_PATH_NOT_FOUND
// (OPENEXISTING / TRUNCATEEXISTING and it does not), ERROR_DUP_NAME (a create on a root in use: the package is made, not
// mounted), ERROR_INVALID_NAME (a file name the drive cannot hold), ERROR_SHARING_VIOLATION (CREATEALWAYS of a package
// open under another root), ERROR_INVALID_PARAMETER (another `creation`). Downloadable content: ERROR_ACCESS_DENIED
// for CREATEALWAYS and TRUNCATEEXISTING, ERROR_FILE_CORRUPT when its package does not match its hashes, ERROR_DISK_FULL
// when its install does not fit. An install can take seconds.
unsigned int PORT_MS_ABI port_content_open(const char *root, const port_content_package *package, unsigned int creation,
    unsigned int *disposition);
// XContentClose / XContentFlush: 0, or ERROR_INVALID_PARAMETER when no package is open as `root`.
unsigned int PORT_MS_ABI port_content_close(const char *root);
unsigned int PORT_MS_ABI port_content_flush(const char *root);
// XContentDelete: 0, ERROR_FILE_NOT_FOUND, ERROR_SHARING_VIOLATION while the package is open, or ERROR_ACCESS_DENIED
// for downloadable content.
unsigned int PORT_MS_ABI port_content_delete(const port_content_package *package);
// The packages of one owner, title and type, in file-name order: *total of them, those from index `first` on copied to
// `packages` (at most `capacity`, *count of them). Returns 0 or a Win32 error.
unsigned int PORT_MS_ABI port_content_list(unsigned long long owner, unsigned int title_id, unsigned int content_type,
    unsigned int first, port_content_package *packages, unsigned int capacity, unsigned int *count,
    unsigned int *total);
// Reads a file from a subfolder of the DLC drop-in directory (HALO3_DLC, else <data>/dlc): <drop>/<folder>/<file>.
// Returns 0 (ERROR_SUCCESS) or a Win32 error.
unsigned int PORT_MS_ABI port_content_dlc_read_file(const char *folder, const char *file, void *buffer,
    unsigned int capacity, unsigned int *size);

#ifdef __cplusplus
}
#endif

#endif
