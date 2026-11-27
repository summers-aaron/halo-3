// port/sdk/kernel: read-only access to Xbox 360 STFS content packages (stfs.c): the "CON ", "LIVE" and "PIRS"
// containers a console keeps downloadable content, title updates and saved games in. The package is read through a
// callback, so the reader has no host dependency; content.c installs packages from them (content.h).
//
// The format, as a console writes it (every multi-byte field big-endian unless noted):
//   0x000  magic: "CON " (console-signed), "LIVE" / "PIRS" (Microsoft-signed)
//   0x32c  content id: 20 bytes, the package's name on a console's drive (40 hex digits)
//   0x340  header size; the data area starts at the next multiple of 0x1000
//   0x344  content type (XCONTENTTYPE_MARKETPLACE 2 for downloadable content); 0x360 title id
//   0x379  volume descriptor (0x24 bytes): length 0x24, version, flags (bit 0: read-only format, one backing block
//          per hash table; bit 1: the top table's active copy is the second), file-table block count (16 bits,
//          little-endian), file-table first block (24 bits, little-endian), SHA-1 of the top hash table, total
//          allocated blocks, free blocks
//   0x39d  data file count (0: a single-file package); 0x3a9 volume type (0 STFS, 1 SVOD)
//   0x411  display names, 128 UTF-16 units per language, English first
// The data area is 0x1000-byte blocks. Every 170 data blocks are preceded by a level-0 hash table (170 entries of a
// block's SHA-1 and a word whose low 24 bits number the next block of its file); a level-1 table hashes 170 level-0
// tables, a level-2 table 170 level-1 tables. A read-write package keeps two copies of each table and its parent
// entry (bit 30) says which is current. The file table is a chain of blocks of 64 directory entries of 64 bytes:
// name (40 bytes), flags (low 6 bits the name length, bit 6 contiguous, bit 7 directory), block counts and the
// first block (24 bits, little-endian), the parent directory's entry index (0xffff: the root), the length.
#ifndef PORT_KERNEL_STFS_H
#define PORT_KERNEL_STFS_H

#ifdef __cplusplus
extern "C" {
#endif

#define PORT_STFS_BLOCK_SIZE 0x1000u
#define PORT_STFS_PEEK_SIZE 0x600u   // the header bytes port_stfs_peek reads (through the English display name)
#define PORT_STFS_ENTRY_CAPACITY 1024u
#define PORT_STFS_NAME_SIZE 41u      // a 40-byte name and its terminator
#define PORT_STFS_ROOT 0xffffu       // the parent of an entry in the root directory
#define PORT_STFS_STAGING_BLOCKS 16u // blocks a read moves from the package at once

#define PORT_STFS_MAGIC_CON 0x434f4e20ul
#define PORT_STFS_MAGIC_LIVE 0x4c495645ul
#define PORT_STFS_MAGIC_PIRS 0x50495253ul

// Reads exactly `size` bytes of the package at `offset`. Returns 0, ERROR_HANDLE_EOF (38) when the package ends
// first, or another Win32 error.
typedef unsigned long (*port_stfs_read_function)(void *context, unsigned long long offset, void *buffer,
    unsigned long size);

// What a package's header says about it: enough to list it.
typedef struct port_stfs_summary {
    unsigned long magic;
    unsigned long header_size;
    unsigned long content_type;
    unsigned long title_id;
    unsigned char content_id[20];
    unsigned char top_hash[20];         // the volume descriptor's SHA-1 of the top hash table: names the contents
    unsigned short display_name[128];   // English, host order, terminated
} port_stfs_summary;

// A file or directory of an open package.
typedef struct port_stfs_entry {
    char name[PORT_STFS_NAME_SIZE]; // terminated
    unsigned char directory;
    unsigned char contiguous;
    unsigned short parent;          // an earlier entry's index, or PORT_STFS_ROOT
    unsigned long first_block;
    unsigned long block_count;      // blocks allocated to the file
    unsigned long size;             // bytes (0 for a directory)
} port_stfs_entry;

typedef struct port_stfs_table {
    unsigned long long offset; // in the package
    int loaded;
    unsigned char bytes[PORT_STFS_BLOCK_SIZE];
} port_stfs_table;

// An open package: large (the directory and a few blocks), so callers keep one in static storage.
typedef struct port_stfs {
    port_stfs_read_function read;
    void *context;
    unsigned long long package_size;
    unsigned long long data_offset; // block 0's position in the package
    port_stfs_summary summary;
    unsigned long total_blocks;
    unsigned long tables_per_level; // backing blocks per hash table: 1 (read-only format) or 2
    int read_only;
    int root_active;                // the top table's current copy is the second
    int top_level;                  // 0, 1 or 2
    int verify;                     // check every block and table against its parent's SHA-1
    unsigned long entry_count;
    port_stfs_entry entries[PORT_STFS_ENTRY_CAPACITY];
    port_stfs_table tables[3];      // the last hash table read at each level
    unsigned long cursor_entry, cursor_ordinal, cursor_block; // where the last read of a chained file ended
    unsigned int run_blocks[PORT_STFS_STAGING_BLOCKS];
    unsigned char staging[PORT_STFS_STAGING_BLOCKS * PORT_STFS_BLOCK_SIZE];
} port_stfs;

// The summary of the header whose first `size` bytes (at least PORT_STFS_PEEK_SIZE) are `bytes`. Returns 0, or
// ERROR_BAD_FORMAT (11) for anything but a single-file STFS package.
unsigned long port_stfs_peek(const void *bytes, unsigned long size, port_stfs_summary *summary);

// Opens the package (`package_size` bytes, read through `read`): its header, hash tables and directory. With `verify`,
// the top hash table is checked against the header, every table read against its parent entry and every block read
// against its level-0 entry. Returns 0, ERROR_BAD_FORMAT (not an STFS package), ERROR_NOT_SUPPORTED (50: SVOD,
// several data files, more than PORT_STFS_ENTRY_CAPACITY entries), ERROR_FILE_CORRUPT (1392: a hash that does not
// match, a block outside the package, a name a drive cannot hold, a broken chain) or the read's error.
unsigned long port_stfs_open(port_stfs *package, port_stfs_read_function read, void *context,
    unsigned long long package_size, int verify);

// The entry at `path` ("maps\\riverworld.map" or "maps/riverworld.map", ASCII case ignored), or -1.
long port_stfs_find(const port_stfs *package, const char *path);

// The entry's path from the root, components joined by `separator`. Returns 0, or ERROR_INSUFFICIENT_BUFFER (122).
unsigned long port_stfs_path(const port_stfs *package, unsigned long entry, char separator, char *path,
    unsigned long capacity);

// Up to `size` bytes of file `entry` from byte `offset` into `buffer`; *done the bytes read (fewer at the end of the
// file, 0 at or past it). Returns 0, ERROR_INVALID_PARAMETER (a directory, no such entry), ERROR_FILE_CORRUPT, or the
// read's error.
unsigned long port_stfs_read(port_stfs *package, unsigned long entry, unsigned long offset, void *buffer,
    unsigned long size, unsigned int *done);

#ifdef __cplusplus
}
#endif

#endif
