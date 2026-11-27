// port/sdk/kernel (both hosts): read-only Xbox 360 STFS content packages (stfs.h).
#include "stfs.h"

typedef unsigned int port_dword;

#define PORT_ERROR_SUCCESS 0ul
#define PORT_ERROR_BAD_FORMAT 11ul
#define PORT_ERROR_HANDLE_EOF 38ul
#define PORT_ERROR_NOT_SUPPORTED 50ul
#define PORT_ERROR_INVALID_PARAMETER 87ul
#define PORT_ERROR_INSUFFICIENT_BUFFER 122ul
#define PORT_ERROR_FILE_CORRUPT 1392ul

#define PORT_STFS_END_OF_CHAIN 0xfffffful
#define PORT_STFS_HASH_ENTRY 0x18u      // SHA-1, then the info word
#define PORT_STFS_DIRECTORY_ENTRY 0x40u
#define PORT_STFS_MAX_BLOCKS 0xfffffful // block numbers are 24 bits

// Data blocks one table covers at each level: 170, 170^2, 170^3.
static const port_dword k_blocks_per_level[3] = { 170ul, 28900ul, 4913000ul };

// port/sdk/kernel/xex_crypt.c.
void XeCryptSha(const void *in1, unsigned int size1, const void *in2, unsigned int size2, const void *in3,
    unsigned int size3, unsigned char *out, unsigned int out_size);

static port_dword port_be16(const unsigned char *p) { return (port_dword)p[0] << 8 | p[1]; }
static port_dword port_le16(const unsigned char *p) { return (port_dword)p[1] << 8 | p[0]; }
static port_dword port_le24(const unsigned char *p) { return (port_dword)p[2] << 16 | (port_dword)p[1] << 8 | p[0]; }

static port_dword port_be32(const unsigned char *p)
{
    return (port_dword)p[0] << 24 | (port_dword)p[1] << 16 | (port_dword)p[2] << 8 | p[3];
}

static int port_same_bytes(const unsigned char *a, const unsigned char *b, unsigned long size)
{
    unsigned char difference = 0;
    for (unsigned long i = 0; i < size; i++)
        difference |= (unsigned char)(a[i] ^ b[i]);
    return difference == 0;
}

static int port_hash_matches(const unsigned char *data, unsigned long size, const unsigned char expected[20])
{
    unsigned char digest[20];
    XeCryptSha(data, (unsigned int)size, 0, 0, 0, 0, digest, sizeof(digest));
    return port_same_bytes(digest, expected, sizeof(digest));
}

static char port_stfs_lower(char c)
{
    return c >= 'A' && c <= 'Z' ? (char)(c + ('a' - 'A')) : c;
}

// ---- the header ----

unsigned long port_stfs_peek(const void *bytes, unsigned long size, port_stfs_summary *summary)
{
    const unsigned char *p = (const unsigned char *)bytes;
    __builtin_memset(summary, 0, sizeof(*summary));
    if (p == 0 || size < PORT_STFS_PEEK_SIZE)
        return PORT_ERROR_BAD_FORMAT;
    const port_dword magic = port_be32(p);
    if (magic != PORT_STFS_MAGIC_CON && magic != PORT_STFS_MAGIC_LIVE && magic != PORT_STFS_MAGIC_PIRS)
        return PORT_ERROR_BAD_FORMAT;
    const port_dword header_size = port_be32(p + 0x340);
    // the descriptor's length, a single data file, an STFS (not SVOD) volume
    if (header_size < PORT_STFS_PEEK_SIZE || header_size > 0x100000ul || p[0x379] != 0x24 ||
        port_be32(p + 0x39d) != 0 || port_be32(p + 0x3a9) != 0)
        return PORT_ERROR_BAD_FORMAT;
    summary->magic = magic;
    summary->header_size = header_size;
    summary->content_type = port_be32(p + 0x344);
    summary->title_id = port_be32(p + 0x360);
    __builtin_memcpy(summary->content_id, p + 0x32c, sizeof(summary->content_id));
    __builtin_memcpy(summary->top_hash, p + 0x381, sizeof(summary->top_hash));
    for (unsigned int i = 0; i + 1 < 128; i++) {
        summary->display_name[i] = (unsigned short)port_be16(p + 0x411 + 2 * i);
        if (summary->display_name[i] == 0)
            break;
    }
    return PORT_ERROR_SUCCESS;
}

// ---- blocks and hash tables ----

// The package offset of data block `block`: the hash tables before it are skipped (each takes tables_per_level
// backing blocks).
static unsigned long long port_stfs_block_offset(const port_stfs *package, port_dword block)
{
    unsigned long long position = block;
    for (int level = 0; level < 3; level++) {
        position += (unsigned long long)((block + k_blocks_per_level[level]) / k_blocks_per_level[level]) *
                    package->tables_per_level;
        if (block < k_blocks_per_level[level])
            break;
    }
    return package->data_offset + (position << 12);
}

// The backing-block position of the level `level` hash table covering data block `block`. A table is written just
// before the first block it covers, except that the tree's higher tables are written when it first grows to need
// them: level 1's first table after the first 170 blocks, level 2's (the only one) after the first 28900.
static port_dword port_stfs_table_position(const port_stfs *package, port_dword block, int level)
{
    const port_dword tables = package->tables_per_level;
    const port_dword step0 = k_blocks_per_level[0] + tables;                                 // a level-0 group
    const port_dword step1 = k_blocks_per_level[1] + (k_blocks_per_level[0] + 1) * tables; // a level-1 group
    if (level == 2)
        return step1;
    if (block < k_blocks_per_level[level])
        return level == 0 ? 0 : step0;
    port_dword position = (block / k_blocks_per_level[level]) * (level == 0 ? step0 : step1);
    if (level == 0) {
        position += (block / k_blocks_per_level[1] + 1) * tables;
        if (block < k_blocks_per_level[1])
            return position;
    }
    return position + tables;
}

static port_dword port_stfs_read_bytes(port_stfs *package, unsigned long long offset, void *buffer, port_dword size)
{
    if (offset > package->package_size || package->package_size - offset < size)
        return PORT_ERROR_FILE_CORRUPT; // the package is shorter than its own tables say
    return package->read(package->context, offset, buffer, size);
}

// The level-0 hash entry of data block `block`: the tables from the top down, each checked against the entry above it
// (the top one against the header) when verifying. The current copy of a read-write package's table is the one its
// parent entry names.
static port_dword port_stfs_hash_entry(port_stfs *package, port_dword block, const unsigned char **entry)
{
    if (block >= package->total_blocks)
        return PORT_ERROR_FILE_CORRUPT;
    const unsigned char *expected = package->summary.top_hash;
    int second = !package->read_only && package->root_active;
    for (int level = package->top_level; level >= 0; level--) {
        const unsigned long long offset = package->data_offset +
            ((unsigned long long)port_stfs_table_position(package, block, level) << 12) + (second ? 0x1000u : 0u);
        port_stfs_table *table = &package->tables[level];
        if (!table->loaded || table->offset != offset) {
            table->loaded = 0;
            const port_dword result = port_stfs_read_bytes(package, offset, table->bytes, PORT_STFS_BLOCK_SIZE);
            if (result != PORT_ERROR_SUCCESS)
                return result;
            if (package->verify && !port_hash_matches(table->bytes, PORT_STFS_BLOCK_SIZE, expected))
                return PORT_ERROR_FILE_CORRUPT;
            table->offset = offset;
            table->loaded = 1;
        }
        const port_dword record = level == 0 ? block % k_blocks_per_level[0]
                                             : (block / k_blocks_per_level[level - 1]) % k_blocks_per_level[0];
        expected = table->bytes + record * PORT_STFS_HASH_ENTRY;
        second = !package->read_only && (port_be32(expected + 20) & 0x40000000ul) != 0;
    }
    *entry = expected;
    return PORT_ERROR_SUCCESS;
}

// The block after `block` in its file's chain.
static port_dword port_stfs_next_block(port_stfs *package, port_dword block, port_dword *next)
{
    const unsigned char *entry = 0;
    const port_dword result = port_stfs_hash_entry(package, block, &entry);
    if (result == PORT_ERROR_SUCCESS)
        *next = port_be32(entry + 20) & PORT_STFS_END_OF_CHAIN;
    return result;
}

// Reads `count` data blocks whose package offsets are consecutive into the staging area, each checked against its
// level-0 entry when verifying.
static port_dword port_stfs_read_run(port_stfs *package, const port_dword *blocks, port_dword count)
{
    port_dword result = port_stfs_read_bytes(package, port_stfs_block_offset(package, blocks[0]), package->staging,
        count * PORT_STFS_BLOCK_SIZE);
    for (port_dword i = 0; i < count && result == PORT_ERROR_SUCCESS && package->verify; i++) {
        const unsigned char *entry = 0;
        result = port_stfs_hash_entry(package, blocks[i], &entry);
        if (result == PORT_ERROR_SUCCESS &&
            !port_hash_matches(package->staging + i * PORT_STFS_BLOCK_SIZE, PORT_STFS_BLOCK_SIZE, entry))
            result = PORT_ERROR_FILE_CORRUPT;
    }
    return result;
}

// The data block holding block `ordinal` of file `index`. A read-only package numbers a contiguous file's blocks
// consecutively; any other file is a chain, walked from the last read's position when it is not past `ordinal`.
static port_dword port_stfs_file_block(port_stfs *package, port_dword index, port_dword ordinal, port_dword *block)
{
    const port_stfs_entry *entry = &package->entries[index];
    if (ordinal >= entry->block_count)
        return PORT_ERROR_FILE_CORRUPT;
    if (package->read_only && entry->contiguous) {
        *block = entry->first_block + ordinal;
        return *block < package->total_blocks ? PORT_ERROR_SUCCESS : PORT_ERROR_FILE_CORRUPT;
    }
    port_dword at = 0, current = entry->first_block;
    if (package->cursor_entry == index && package->cursor_ordinal <= ordinal) {
        at = package->cursor_ordinal;
        current = package->cursor_block;
    }
    while (at < ordinal) {
        const port_dword result = port_stfs_next_block(package, current, &current);
        if (result != PORT_ERROR_SUCCESS)
            return result;
        if (current >= package->total_blocks)
            return PORT_ERROR_FILE_CORRUPT; // the chain ends (or leaves the package) before the file does
        at++;
    }
    package->cursor_entry = index;
    package->cursor_ordinal = at;
    package->cursor_block = current;
    *block = current;
    return current < package->total_blocks ? PORT_ERROR_SUCCESS : PORT_ERROR_FILE_CORRUPT;
}

// ---- the directory ----

// A name a host drive can hold as it is: printable ASCII but \ / : * ? " < > |, not . or .., no trailing dot or space.
static int port_stfs_valid_name(const char *name, port_dword length)
{
    if (length == 0 || (length == 1 && name[0] == '.') || (length == 2 && name[0] == '.' && name[1] == '.') ||
        name[length - 1] == '.' || name[length - 1] == ' ')
        return 0;
    for (port_dword i = 0; i < length; i++) {
        const unsigned char c = (unsigned char)name[i];
        if (c < 0x20 || c > 0x7e || c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' || c == '"' ||
            c == '<' || c == '>' || c == '|')
            return 0;
    }
    return 1;
}

static int port_stfs_same_name(const char *a, const char *b)
{
    port_dword i = 0;
    while (a[i] != 0 && port_stfs_lower(a[i]) == port_stfs_lower(b[i]))
        i++;
    return a[i] == 0 && b[i] == 0;
}

// One 64-byte directory entry, appended. Returns 0, ERROR_FILE_CORRUPT or ERROR_NOT_SUPPORTED (too many entries).
static port_dword port_stfs_add_entry(port_stfs *package, const unsigned char *bytes)
{
    const port_dword length = bytes[0x28] & 0x3fu;
    if (length > 40 || !port_stfs_valid_name((const char *)bytes, length))
        return PORT_ERROR_FILE_CORRUPT;
    if (package->entry_count == PORT_STFS_ENTRY_CAPACITY)
        return PORT_ERROR_NOT_SUPPORTED;
    port_stfs_entry *entry = &package->entries[package->entry_count];
    __builtin_memset(entry, 0, sizeof(*entry));
    __builtin_memcpy(entry->name, bytes, length);
    entry->directory = (bytes[0x28] & 0x80u) != 0;
    entry->contiguous = (bytes[0x28] & 0x40u) != 0;
    entry->block_count = port_le24(bytes + 0x2c);
    entry->first_block = port_le24(bytes + 0x2f);
    entry->parent = (unsigned short)port_be16(bytes + 0x32);
    entry->size = entry->directory ? 0 : port_be32(bytes + 0x34);
    // a parent read before it, and a directory; a file's blocks inside the package and enough for its bytes
    if (entry->parent != PORT_STFS_ROOT &&
        (entry->parent >= package->entry_count || !package->entries[entry->parent].directory))
        return PORT_ERROR_FILE_CORRUPT;
    if (!entry->directory) {
        const port_dword needed = (port_dword)(((unsigned long long)entry->size + PORT_STFS_BLOCK_SIZE - 1) >> 12);
        if (entry->block_count < needed || entry->block_count > package->total_blocks ||
            (entry->block_count != 0 && entry->first_block >= package->total_blocks))
            return PORT_ERROR_FILE_CORRUPT;
    }
    // two entries of one directory whose names differ only in case would be one host file
    for (port_dword i = 0; i < package->entry_count; i++)
        if (package->entries[i].parent == entry->parent && port_stfs_same_name(package->entries[i].name, entry->name))
            return PORT_ERROR_FILE_CORRUPT;
    package->entry_count++;
    return PORT_ERROR_SUCCESS;
}

// The file table: `count` blocks chained from `first`, each up to 64 entries (an empty name ends a block's).
static port_dword port_stfs_read_directory(port_stfs *package, port_dword first, port_dword count)
{
    port_dword block = first;
    for (port_dword n = 0; n < count; n++) {
        port_dword result = port_stfs_read_run(package, &block, 1);
        if (result != PORT_ERROR_SUCCESS)
            return result;
        // the entries are parsed in the staging area: nothing reads a block until the next one
        for (port_dword m = 0; m < PORT_STFS_BLOCK_SIZE / PORT_STFS_DIRECTORY_ENTRY; m++) {
            const unsigned char *bytes = package->staging + m * PORT_STFS_DIRECTORY_ENTRY;
            if (bytes[0] == 0)
                break;
            result = port_stfs_add_entry(package, bytes);
            if (result != PORT_ERROR_SUCCESS)
                return result;
        }
        if (n + 1 < count) {
            result = port_stfs_next_block(package, block, &block);
            if (result != PORT_ERROR_SUCCESS)
                return result;
            if (block >= package->total_blocks)
                return PORT_ERROR_FILE_CORRUPT;
        }
    }
    return PORT_ERROR_SUCCESS;
}

unsigned long port_stfs_open(port_stfs *package, port_stfs_read_function read, void *context,
    unsigned long long package_size, int verify)
{
    __builtin_memset(package, 0, sizeof(*package));
    package->read = read;
    package->context = context;
    package->package_size = package_size;
    package->verify = verify != 0;
    package->cursor_entry = 0xfffffffful;
    unsigned char *header = package->staging; // the header's first bytes, before any block is read
    if (package_size < PORT_STFS_PEEK_SIZE)
        return PORT_ERROR_BAD_FORMAT;
    port_dword result = read(context, 0, header, PORT_STFS_PEEK_SIZE);
    if (result != PORT_ERROR_SUCCESS)
        return result == PORT_ERROR_HANDLE_EOF ? PORT_ERROR_BAD_FORMAT : result;
    if (port_be32(header) == PORT_STFS_MAGIC_CON || port_be32(header) == PORT_STFS_MAGIC_LIVE ||
        port_be32(header) == PORT_STFS_MAGIC_PIRS) {
        if (port_be32(header + 0x3a9) != 0 || port_be32(header + 0x39d) != 0)
            return PORT_ERROR_NOT_SUPPORTED; // SVOD (games on demand), or a package split over several files
    }
    result = port_stfs_peek(header, PORT_STFS_PEEK_SIZE, &package->summary);
    if (result != PORT_ERROR_SUCCESS)
        return result;
    const unsigned char *descriptor = header + 0x379;
    package->read_only = (descriptor[2] & 1u) != 0;
    package->root_active = (descriptor[2] & 2u) != 0;
    package->tables_per_level = package->read_only ? 1 : 2;
    const port_dword table_count = port_le16(descriptor + 3), table_first = port_le24(descriptor + 5);
    package->total_blocks = port_be32(descriptor + 0x1c);
    package->data_offset = ((unsigned long long)package->summary.header_size + 0xfffu) & ~0xfffull;
    if (package->total_blocks == 0 || package->total_blocks > PORT_STFS_MAX_BLOCKS || table_count == 0 ||
        table_first >= package->total_blocks || package->data_offset >= package_size)
        return PORT_ERROR_FILE_CORRUPT;
    // the top level is the lowest one whose table covers every block. A package of exactly 170 or 28900 blocks is the
    // one doubt: a console makes a level's first table when the block that needs it is allocated, so it has none
    // above the full level, but other writers (and readers) count one level more. The lower level is tried first,
    // then the higher; the top hash decides when verifying, else the first whose tables can be read.
    int candidates[2], candidate_count = 0;
    candidates[candidate_count++] = package->total_blocks <= k_blocks_per_level[0] ? 0
                                  : package->total_blocks <= k_blocks_per_level[1] ? 1 : 2;
    if (package->total_blocks == k_blocks_per_level[0] || package->total_blocks == k_blocks_per_level[1])
        candidates[candidate_count++] = candidates[0] + 1;
    for (int i = 0; i < candidate_count; i++) {
        const unsigned char *entry = 0;
        package->top_level = candidates[i];
        __builtin_memset(package->tables, 0, sizeof(package->tables));
        result = port_stfs_hash_entry(package, 0, &entry);
        if (result != PORT_ERROR_FILE_CORRUPT)
            break;
    }
    if (result != PORT_ERROR_SUCCESS)
        return result;
    return port_stfs_read_directory(package, table_first, table_count);
}

// ---- lookups and reads ----

long port_stfs_find(const port_stfs *package, const char *path)
{
    port_dword parent = PORT_STFS_ROOT;
    long found = -1;
    while (*path != 0) {
        while (*path == '\\' || *path == '/')
            path++;
        if (*path == 0)
            break;
        port_dword length = 0;
        while (path[length] != 0 && path[length] != '\\' && path[length] != '/')
            length++;
        found = -1;
        for (port_dword i = 0; i < package->entry_count && found < 0; i++) {
            const port_stfs_entry *entry = &package->entries[i];
            if (entry->parent != parent)
                continue;
            port_dword k = 0;
            while (k < length && entry->name[k] != 0 && port_stfs_lower(entry->name[k]) == port_stfs_lower(path[k]))
                k++;
            if (k == length && entry->name[k] == 0)
                found = (long)i;
        }
        if (found < 0)
            return -1;
        path += length;
        parent = (port_dword)found;
        if (*path != 0 && !package->entries[found].directory)
            return -1; // a file used as a directory
    }
    return found;
}

unsigned long port_stfs_path(const port_stfs *package, unsigned long entry, char separator, char *path,
    unsigned long capacity)
{
    if (entry >= package->entry_count || capacity == 0)
        return PORT_ERROR_INVALID_PARAMETER;
    // the length first (parents come before their children, so the walk ends)
    port_dword length = 0;
    for (port_dword i = entry; i != PORT_STFS_ROOT; i = package->entries[i].parent) {
        port_dword n = 0;
        while (package->entries[i].name[n] != 0)
            n++;
        length += n + (i == entry ? 0 : 1);
    }
    if (length + 1 > capacity)
        return PORT_ERROR_INSUFFICIENT_BUFFER;
    path[length] = 0;
    port_dword end = length;
    for (port_dword i = entry; i != PORT_STFS_ROOT; i = package->entries[i].parent) {
        port_dword n = 0;
        while (package->entries[i].name[n] != 0)
            n++;
        end -= n;
        __builtin_memcpy(path + end, package->entries[i].name, n);
        if (end != 0)
            path[--end] = separator;
    }
    return PORT_ERROR_SUCCESS;
}

unsigned long port_stfs_read(port_stfs *package, unsigned long entry, unsigned long offset, void *buffer,
    unsigned long size, unsigned int *done)
{
    *done = 0;
    if (entry >= package->entry_count || package->entries[entry].directory)
        return PORT_ERROR_INVALID_PARAMETER;
    const port_stfs_entry *file = &package->entries[entry];
    if (offset >= file->size)
        return PORT_ERROR_SUCCESS;
    if (size > file->size - offset)
        size = file->size - offset;
    unsigned char *out = (unsigned char *)buffer;
    while (*done < size) {
        const port_dword position = offset + *done;
        const port_dword ordinal = position >> 12, within = position & 0xfffu;
        port_dword wanted = (within + (size - *done) + PORT_STFS_BLOCK_SIZE - 1) >> 12;
        if (wanted > PORT_STFS_STAGING_BLOCKS)
            wanted = PORT_STFS_STAGING_BLOCKS;
        // the longest run of the file's blocks that lie one after another in the package
        port_dword count = 0;
        unsigned long long first_offset = 0;
        while (count < wanted) {
            port_dword block = 0;
            const port_dword result = port_stfs_file_block(package, entry, ordinal + count, &block);
            if (result != PORT_ERROR_SUCCESS)
                return result;
            const unsigned long long block_offset = port_stfs_block_offset(package, block);
            if (count == 0)
                first_offset = block_offset;
            else if (block_offset != first_offset + ((unsigned long long)count << 12))
                break;
            package->run_blocks[count++] = block;
        }
        const port_dword result = port_stfs_read_run(package, package->run_blocks, count);
        if (result != PORT_ERROR_SUCCESS)
            return result;
        port_dword bytes = (count << 12) - within;
        if (bytes > size - *done)
            bytes = size - *done;
        __builtin_memcpy(out + *done, package->staging + within, bytes);
        *done += bytes;
    }
    return PORT_ERROR_SUCCESS;
}
