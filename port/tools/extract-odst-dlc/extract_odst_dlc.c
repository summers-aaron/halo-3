// extract_odst_dlc.c — Extract all 13 Halo 3 DLC maps (Heroic, Legendary, Cold Storage,
// Mythic, Mythic II) and the expanded 79-achievement SPA (4D5307E6.spa, 1,750 G) from a
// Halo 3: ODST Disc 2 ("Halo 3: Mythic") Xbox 360 ISO (Redump XGD2 or trimmed XISO) or
// an already-extracted disc directory into the Halo 3 port's DLC folder
// (<data>/dlc/odst_mythic/).
//
// Zero external dependencies; builds with any C99 compiler on Linux or Windows:
//   cc -O2 -Wall -Wextra extract_odst_dlc.c -o extract_odst_dlc
//
// Usage:
//   extract_odst_dlc <odst_disc2.iso | extracted_disc_dir> [output_dlc_dir]

#define _FILE_OFFSET_BITS 64
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#include <sys/stat.h>
#define PATH_SEP '\\'
#define fseek64 _fseeki64
#define stat64_t struct __stat64
#define stat64_fn _stat64
#define mkdir_one(p) _mkdir(p)
static int str_ieq(const char *a, const char *b) { return _stricmp(a, b) == 0; }
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#define PATH_SEP '/'
#define fseek64 fseeko
#define stat64_t struct stat
#define stat64_fn stat
#define mkdir_one(p) mkdir((p), 0755)
static int str_ieq(const char *a, const char *b)
{
    while (*a && *b) {
        unsigned char ca = (unsigned char)*a++;
        unsigned char cb = (unsigned char)*b++;
        if (ca >= 'A' && ca <= 'Z') ca += 'a' - 'A';
        if (cb >= 'A' && cb <= 'Z') cb += 'a' - 'A';
        if (ca != cb) return 0;
    }
    return *a == *b;
}
#endif

// ---- Halo 3 DLC map table (all 13 maps present on ODST Disc 2) ----

typedef struct {
    const char *pack;
    const char *map_name;   // maps/<map_name>.map & maps/info/<map_name>.mapinfo
    const char *image_base; // maps/images/<image_base>{,_sm,_variant,_clip,_film}.blf
    const char *title;
} dlc_map_desc_t;

static const dlc_map_desc_t k_dlc_maps[] = {
    // Heroic Map Pack
    { "Heroic",       "armory",      "dlc_armory",      "Rat's Nest"   },
    { "Heroic",       "bunkerworld", "dlc_bunkerworld", "Standoff"     },
    { "Heroic",       "warehouse",   "dlc_warehouse",   "Foundry"      },
    // Legendary Map Pack
    { "Legendary",    "ghosttown",   "dlc_ghost_town",  "Ghost Town"   },
    { "Legendary",    "lockout",     "dlc_lockout",     "Blackout"     },
    { "Legendary",    "sidewinder",  "dlc_sidewinder",  "Avalanche"    },
    // Cold Storage
    { "Cold Storage", "chillout",    "dlc_chillout",    "Cold Storage" },
    // Mythic Map Pack
    { "Mythic",       "descent",     "dlc_descent",     "Assembly"     },
    { "Mythic",       "sandbox",     "dlc_sandbox",     "Sandbox"      },
    { "Mythic",       "spacecamp",   "dlc_space_camp",  "Orbital"      },
    // Mythic II Map Pack
    { "Mythic II",    "docks",       "dlc_docks",       "Longshore"    },
    { "Mythic II",    "fortress",    "dlc_citadel",     "Citadel"      },
    { "Mythic II",    "midship",     "dlc_midship",     "Heretic"      },
};

static const char *const k_blf_suffixes[] = {
    ".blf",
    "_sm.blf",
    "_variant.blf",
    "_clip.blf",
    "_film.blf",
};

// ---- Helpers ----

static uint16_t rd16le(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t rd32le(const uint8_t *p)
{
    return (uint32_t)p[0]
         | ((uint32_t)p[1] << 8)
         | ((uint32_t)p[2] << 16)
         | ((uint32_t)p[3] << 24);
}

static uint16_t rd16be(const uint8_t *p)
{
    return (uint16_t)(((uint16_t)p[0] << 8) | p[1]);
}

static uint32_t rd32be(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24)
         | ((uint32_t)p[1] << 16)
         | ((uint32_t)p[2] << 8)
         | (uint32_t)p[3];
}

static int mkdir_p(const char *path)
{
    char buf[4096];
    size_t len = strlen(path);
    if (len == 0 || len >= sizeof(buf))
        return -1;
    memcpy(buf, path, len + 1);
    for (size_t i = 1; i < len; i++) {
        if (buf[i] == '/' || buf[i] == '\\') {
            char saved = buf[i];
            buf[i] = '\0';
#ifdef _WIN32
            if (!(i == 2 && buf[1] == ':'))
#endif
            {
                if (mkdir_one(buf) != 0 && errno != EEXIST)
                    return -1;
            }
            buf[i] = saved;
        }
    }
    if (mkdir_one(buf) != 0 && errno != EEXIST)
        return -1;
    return 0;
}

static int file_exists_with_size(const char *path, uint64_t expected_size)
{
    stat64_t st;
    if (stat64_fn(path, &st) != 0)
        return 0;
    return (uint64_t)st.st_size == expected_size;
}

static int is_directory(const char *path)
{
    stat64_t st;
    if (stat64_fn(path, &st) != 0)
        return 0;
    return (st.st_mode & S_IFMT) == S_IFDIR;
}

static int default_dlc_dir(char *out, size_t cap)
{
    const char *env_dlc = getenv("HALO3_DLC");
    if (env_dlc && env_dlc[0]) {
        int n = snprintf(out, cap, "%s%codst_mythic", env_dlc, PATH_SEP);
        return (n > 0 && (size_t)n < cap) ? 0 : -1;
    }
#ifdef _WIN32
    const char *base = getenv("LOCALAPPDATA");
    if (!base || !base[0]) base = getenv("USERPROFILE");
    if (!base || !base[0]) return -1;
    int n = snprintf(out, cap, "%s\\halo3\\dlc\\odst_mythic", base);
    return (n > 0 && (size_t)n < cap) ? 0 : -1;
#else
    const char *xdg = getenv("XDG_DATA_HOME");
    if (xdg && xdg[0]) {
        int n = snprintf(out, cap, "%s/halo3/dlc/odst_mythic", xdg);
        return (n > 0 && (size_t)n < cap) ? 0 : -1;
    }
    const char *home = getenv("HOME");
    if (!home || !home[0]) return -1;
    int n = snprintf(out, cap, "%s/.local/share/halo3/dlc/odst_mythic", home);
    return (n > 0 && (size_t)n < cap) ? 0 : -1;
#endif
}

// ---- XDVDFS (Xbox 360 ISO / XISO) reader ----

#define XDVDFS_SECTOR_SIZE 2048u
#define XDVDFS_MAGIC "MICROSOFT*XBOX*MEDIA"
#define XDVDFS_ATTR_DIR 0x10u

typedef struct {
    FILE *fp;
    uint64_t part_base;
    uint32_t root_sector;
    uint32_t root_size;
} xdvdfs_t;

typedef struct {
    uint32_t sector;
    uint32_t size;
    uint8_t attr;
} xdvdfs_entry_t;

static int xdvdfs_open(xdvdfs_t *fs, const char *iso_path)
{
    static const uint64_t k_partition_bases[] = {
        0x00000000ULL, // Trimmed XISO
        0x0FD90000ULL, // Redump XGD2 (Xbox 360 standard — Halo 3 / ODST)
        0x02080000ULL, // Redump XGD3
        0x18300000ULL, // Redump XGD1 (Original Xbox)
    };

    memset(fs, 0, sizeof(*fs));
    fs->fp = fopen(iso_path, "rb");
    if (!fs->fp) {
        fprintf(stderr, "error: cannot open '%s': %s\n", iso_path, strerror(errno));
        return -1;
    }

    uint8_t hdr[32];
    for (size_t i = 0; i < sizeof(k_partition_bases) / sizeof(k_partition_bases[0]); i++) {
        uint64_t off = k_partition_bases[i] + 32ULL * XDVDFS_SECTOR_SIZE;
        if (fseek64(fs->fp, (int64_t)off, SEEK_SET) != 0)
            continue;
        if (fread(hdr, 1, sizeof(hdr), fs->fp) != sizeof(hdr))
            continue;
        if (memcmp(hdr, XDVDFS_MAGIC, 20) == 0) {
            fs->part_base = k_partition_bases[i];
            fs->root_sector = rd32le(hdr + 20);
            fs->root_size = rd32le(hdr + 24);
            return 0;
        }
    }

    fprintf(stderr, "error: '%s' is not a recognized Xbox 360 ISO / XISO image\n", iso_path);
    fclose(fs->fp);
    fs->fp = NULL;
    return -1;
}

static void xdvdfs_close(xdvdfs_t *fs)
{
    if (fs->fp) {
        fclose(fs->fp);
        fs->fp = NULL;
    }
}

static int xdvdfs_find_in_dir(xdvdfs_t *fs, uint32_t dir_sector, uint32_t dir_size,
                              const char *name, xdvdfs_entry_t *out)
{
    if (dir_size == 0 || dir_size > (64u << 20))
        return -1;
    uint8_t *buf = (uint8_t *)malloc(dir_size);
    if (!buf)
        return -1;
    uint64_t off = fs->part_base + (uint64_t)dir_sector * XDVDFS_SECTOR_SIZE;
    if (fseek64(fs->fp, (int64_t)off, SEEK_SET) != 0 || fread(buf, 1, dir_size, fs->fp) != dir_size) {
        free(buf);
        return -1;
    }

    // Iterative traversal of XDVDFS binary directory tree (handles any case ordering).
    uint32_t stack[512];
    size_t sp = 0;
    stack[sp++] = 0;
    int found = -1;

    while (sp > 0) {
        uint32_t node = stack[--sp];
        uint32_t byte_off = node * 4u;
        if (byte_off + 14u > dir_size)
            continue;
        uint16_t left = rd16le(buf + byte_off + 0);
        uint16_t right = rd16le(buf + byte_off + 2);
        uint32_t esec = rd32le(buf + byte_off + 4);
        uint32_t esz = rd32le(buf + byte_off + 8);
        uint8_t attr = buf[byte_off + 12];
        uint8_t nlen = buf[byte_off + 13];
        if (byte_off + 14u + nlen > dir_size)
            continue;

        char ename[256];
        memcpy(ename, buf + byte_off + 14, nlen);
        ename[nlen] = '\0';
        if (str_ieq(ename, name)) {
            out->sector = esec;
            out->size = esz;
            out->attr = attr;
            found = 0;
            break;
        }
        if (left != 0 && left != 0xFFFF && sp < 512)
            stack[sp++] = left;
        if (right != 0 && right != 0xFFFF && sp < 512)
            stack[sp++] = right;
    }

    free(buf);
    return found;
}

static int xdvdfs_find_path(xdvdfs_t *fs, const char *rel_path, xdvdfs_entry_t *out)
{
    uint32_t cur_sec = fs->root_sector;
    uint32_t cur_sz = fs->root_size;
    const char *p = rel_path;
    while (*p == '/' || *p == '\\')
        p++;

    while (*p) {
        char comp[256];
        size_t n = 0;
        while (*p && *p != '/' && *p != '\\' && n + 1 < sizeof(comp))
            comp[n++] = *p++;
        comp[n] = '\0';
        while (*p == '/' || *p == '\\')
            p++;

        xdvdfs_entry_t ent;
        if (xdvdfs_find_in_dir(fs, cur_sec, cur_sz, comp, &ent) != 0)
            return -1;
        if (*p == '\0') {
            *out = ent;
            return 0;
        }
        if (!(ent.attr & XDVDFS_ATTR_DIR))
            return -1;
        cur_sec = ent.sector;
        cur_sz = ent.size;
    }
    return -1;
}

static int verify_halo3_map_header(FILE *fp, uint64_t offset, const char *map_name)
{
    uint8_t hdr[0x140];
    if (fseek64(fp, (int64_t)offset, SEEK_SET) != 0 || fread(hdr, 1, sizeof(hdr), fp) != sizeof(hdr)) {
        fprintf(stderr, "error: failed to read header of %s.map\n", map_name);
        return -1;
    }
    if (memcmp(hdr, "head", 4) != 0) {
        fprintf(stderr, "error: %s.map does not have Halo 3 'head' magic\n", map_name);
        return -1;
    }
    uint32_t ver = ((uint32_t)hdr[4] << 24) | ((uint32_t)hdr[5] << 16) | ((uint32_t)hdr[6] << 8) | hdr[7];
    if (ver != 11u) {
        fprintf(stderr, "error: %s.map has unexpected version %u (expected 11)\n", map_name, ver);
        return -1;
    }
    char build[33];
    memcpy(build, hdr + 0x11c, 32);
    build[32] = '\0';
    if (strstr(build, "halo3_ship") == NULL) {
        fprintf(stderr, "error: %s.map build '%s' is not Halo 3 retail\n", map_name, build);
        return -1;
    }
    return 0;
}

static int copy_stream_range(FILE *src, uint64_t src_off, uint64_t size, const char *dst_path,
                             uint8_t *io_buf, size_t io_buf_sz)
{
    if (fseek64(src, (int64_t)src_off, SEEK_SET) != 0)
        return -1;
    FILE *dst = fopen(dst_path, "wb");
    if (!dst) {
        fprintf(stderr, "error: cannot create '%s': %s\n", dst_path, strerror(errno));
        return -1;
    }
    uint64_t rem = size;
    while (rem > 0) {
        size_t chunk = rem < io_buf_sz ? (size_t)rem : io_buf_sz;
        if (fread(io_buf, 1, chunk, src) != chunk) {
            fclose(dst);
            return -1;
        }
        if (fwrite(io_buf, 1, chunk, dst) != chunk) {
            fclose(dst);
            return -1;
        }
        rem -= chunk;
    }
    if (fclose(dst) != 0)
        return -1;
    return 0;
}

// ---- Unified extraction from either ISO or extracted disc directory ----

typedef struct {
    int from_iso;
    xdvdfs_t iso;
    const char *dir_root;
    uint8_t *io_buf;
    size_t io_buf_sz;
    uint64_t bytes_written;
    uint64_t bytes_skipped;
    int files_written;
    int files_skipped;
} extractor_t;

static int extract_one(extractor_t *ex, const char *rel_path, const char *dst_path, int check_map_hdr,
                       const char *map_name)
{
    if (ex->from_iso) {
        xdvdfs_entry_t ent;
        if (xdvdfs_find_path(&ex->iso, rel_path, &ent) != 0)
            return -1;
        uint64_t off = ex->iso.part_base + (uint64_t)ent.sector * XDVDFS_SECTOR_SIZE;
        if (check_map_hdr && verify_halo3_map_header(ex->iso.fp, off, map_name) != 0)
            return -1;
        if (file_exists_with_size(dst_path, ent.size)) {
            ex->bytes_skipped += ent.size;
            ex->files_skipped++;
            return 0;
        }
        if (copy_stream_range(ex->iso.fp, off, ent.size, dst_path, ex->io_buf, ex->io_buf_sz) != 0)
            return -1;
        ex->bytes_written += ent.size;
        ex->files_written++;
        return 0;
    } else {
        char src_path[4096];
        snprintf(src_path, sizeof(src_path), "%s%c%s", ex->dir_root, PATH_SEP, rel_path);
        stat64_t st;
        if (stat64_fn(src_path, &st) != 0)
            return -1;
        uint64_t size = (uint64_t)st.st_size;
        FILE *fp = fopen(src_path, "rb");
        if (!fp)
            return -1;
        if (check_map_hdr && verify_halo3_map_header(fp, 0, map_name) != 0) {
            fclose(fp);
            return -1;
        }
        if (file_exists_with_size(dst_path, size)) {
            fclose(fp);
            ex->bytes_skipped += size;
            ex->files_skipped++;
            return 0;
        }
        int rc = copy_stream_range(fp, 0, size, dst_path, ex->io_buf, ex->io_buf_sz);
        fclose(fp);
        if (rc != 0)
            return -1;
        ex->bytes_written += size;
        ex->files_written++;
        return 0;
    }
}

// ---- XEX2 4D5307E6 SPA (XDBF) extraction from default.xex ----

static const uint8_t k_aes_sbox_inv[256] = {
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

static const uint8_t k_aes_sbox[256] = {
    0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
    0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
    0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
    0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
    0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
    0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
    0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
    0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
    0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
    0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
    0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
    0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
    0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
    0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
    0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
    0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static uint8_t g_mul9[256], g_mul11[256], g_mul13[256], g_mul14[256];

static uint8_t gf_mul(uint8_t a, uint8_t b)
{
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        uint8_t hi = a & 0x80;
        a <<= 1;
        if (hi) a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

static void aes128_init_tables(void)
{
    for (int i = 0; i < 256; i++) {
        g_mul9[i]  = gf_mul((uint8_t)i, 9);
        g_mul11[i] = gf_mul((uint8_t)i, 11);
        g_mul13[i] = gf_mul((uint8_t)i, 13);
        g_mul14[i] = gf_mul((uint8_t)i, 14);
    }
}

static void aes128_expand_key(const uint8_t key[16], uint8_t rk[176])
{
    memcpy(rk, key, 16);
    uint8_t rcon = 1;
    for (int i = 16; i < 176; i += 4) {
        uint8_t t[4] = { rk[i - 4], rk[i - 3], rk[i - 2], rk[i - 1] };
        if (i % 16 == 0) {
            uint8_t u = t[0];
            t[0] = k_aes_sbox[t[1]] ^ rcon;
            t[1] = k_aes_sbox[t[2]];
            t[2] = k_aes_sbox[t[3]];
            t[3] = k_aes_sbox[u];
            rcon = gf_mul(rcon, 2);
        }
        for (int j = 0; j < 4; j++)
            rk[i + j] = rk[i - 16 + j] ^ t[j];
    }
}

static void aes128_decrypt_block(const uint8_t rk[176], const uint8_t in[16], uint8_t out[16])
{
    uint8_t s[16], t[16];
    for (int i = 0; i < 16; i++)
        s[i] = in[i] ^ rk[160 + i];

    for (int round = 9; round >= 1; round--) {
        t[0]  = k_aes_sbox_inv[s[0]];  t[1]  = k_aes_sbox_inv[s[13]]; t[2]  = k_aes_sbox_inv[s[10]]; t[3]  = k_aes_sbox_inv[s[7]];
        t[4]  = k_aes_sbox_inv[s[4]];  t[5]  = k_aes_sbox_inv[s[1]];  t[6]  = k_aes_sbox_inv[s[14]]; t[7]  = k_aes_sbox_inv[s[11]];
        t[8]  = k_aes_sbox_inv[s[8]];  t[9]  = k_aes_sbox_inv[s[5]];  t[10] = k_aes_sbox_inv[s[2]];  t[11] = k_aes_sbox_inv[s[15]];
        t[12] = k_aes_sbox_inv[s[12]]; t[13] = k_aes_sbox_inv[s[9]];  t[14] = k_aes_sbox_inv[s[6]];  t[15] = k_aes_sbox_inv[s[3]];

        for (int i = 0; i < 16; i++)
            t[i] ^= rk[round * 16 + i];

        for (int c = 0; c < 4; c++) {
            uint8_t a0 = t[c * 4 + 0], a1 = t[c * 4 + 1], a2 = t[c * 4 + 2], a3 = t[c * 4 + 3];
            s[c * 4 + 0] = g_mul14[a0] ^ g_mul11[a1] ^ g_mul13[a2] ^ g_mul9[a3];
            s[c * 4 + 1] = g_mul9[a0]  ^ g_mul14[a1] ^ g_mul11[a2] ^ g_mul13[a3];
            s[c * 4 + 2] = g_mul13[a0] ^ g_mul9[a1]  ^ g_mul14[a2] ^ g_mul11[a3];
            s[c * 4 + 3] = g_mul11[a0] ^ g_mul13[a1] ^ g_mul9[a2]  ^ g_mul14[a3];
        }
    }

    out[0]  = k_aes_sbox_inv[s[0]]  ^ rk[0];  out[1]  = k_aes_sbox_inv[s[13]] ^ rk[1];
    out[2]  = k_aes_sbox_inv[s[10]] ^ rk[2];  out[3]  = k_aes_sbox_inv[s[7]]  ^ rk[3];
    out[4]  = k_aes_sbox_inv[s[4]]  ^ rk[4];  out[5]  = k_aes_sbox_inv[s[1]]  ^ rk[5];
    out[6]  = k_aes_sbox_inv[s[14]] ^ rk[6];  out[7]  = k_aes_sbox_inv[s[11]] ^ rk[7];
    out[8]  = k_aes_sbox_inv[s[8]]  ^ rk[8];  out[9]  = k_aes_sbox_inv[s[5]]  ^ rk[9];
    out[10] = k_aes_sbox_inv[s[2]]  ^ rk[10]; out[11] = k_aes_sbox_inv[s[15]] ^ rk[11];
    out[12] = k_aes_sbox_inv[s[12]] ^ rk[12]; out[13] = k_aes_sbox_inv[s[9]]  ^ rk[13];
    out[14] = k_aes_sbox_inv[s[6]]  ^ rk[14]; out[15] = k_aes_sbox_inv[s[3]]  ^ rk[15];
}

static int verify_xdbf_spa(const uint8_t *spa, uint32_t size,
                           uint32_t *out_ach_count, uint32_t *out_gamerscore, uint32_t *out_png_count)
{
    if (size < 24 || rd32be(spa) != 0x58444246u) // "XDBF"
        return -1;
    uint32_t entry_count = rd32be(spa + 8);
    uint32_t entry_used = rd32be(spa + 12);
    uint32_t free_count = rd32be(spa + 16);
    if (entry_count > 0x10000u || free_count > 0x10000u || entry_used > entry_count)
        return -1;
    uint32_t base = 24u + 18u * entry_count + 8u * free_count;
    if (base > size)
        return -1;

    static const uint8_t k_png_sig[8] = { 0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a };
    uint32_t ach_count = 0, gamerscore = 0, png_count = 0;
    int found_xach = 0;

    for (uint32_t i = 0; i < entry_used; i++) {
        const uint8_t *e = spa + 24u + 18u * i;
        uint16_t sec = rd16be(e);
        uint32_t id_hi = rd32be(e + 2);
        uint32_t id_lo = rd32be(e + 6);
        uint32_t off = rd32be(e + 10);
        uint32_t len = rd32be(e + 14);
        if (off > size - base || len > size - base - off)
            continue;
        const uint8_t *payload = spa + base + off;
        if (sec == 1 && id_hi == 0 && id_lo == 0x58414348u && len >= 14 && rd32be(payload) == 0x58414348u) {
            uint32_t count = rd16be(payload + 12);
            if (14u + 0x24u * count <= len) {
                found_xach = 1;
                ach_count = count;
                for (uint32_t k = 0; k < count; k++)
                    gamerscore += rd16be(payload + 14u + 0x24u * k + 12u);
            }
        } else if (sec == 2 && len > 8 && memcmp(payload, k_png_sig, 8) == 0) {
            png_count++;
        }
    }
    if (!found_xach || ach_count == 0)
        return -1;
    *out_ach_count = ach_count;
    *out_gamerscore = gamerscore;
    *out_png_count = png_count;
    return 0;
}

static int extract_spa_from_xex(extractor_t *ex, const char *dst_path,
                                uint32_t *out_ach_count, uint32_t *out_gamerscore, uint32_t *out_png_count)
{
    uint8_t *xex = NULL;
    uint32_t xex_size = 0;

    if (ex->from_iso) {
        xdvdfs_entry_t ent;
        if (xdvdfs_find_path(&ex->iso, "default.xex", &ent) != 0 || ent.size < 0x200u || ent.size > (64u << 20))
            return -1;
        xex_size = ent.size;
        xex = (uint8_t *)malloc(xex_size);
        if (!xex)
            return -1;
        uint64_t off = ex->iso.part_base + (uint64_t)ent.sector * XDVDFS_SECTOR_SIZE;
        if (fseek64(ex->iso.fp, (int64_t)off, SEEK_SET) != 0 || fread(xex, 1, xex_size, ex->iso.fp) != xex_size) {
            free(xex);
            return -1;
        }
    } else {
        char src_path[4096];
        snprintf(src_path, sizeof(src_path), "%s%cdefault.xex", ex->dir_root, PATH_SEP);
        stat64_t st;
        if (stat64_fn(src_path, &st) != 0 || st.st_size < 0x200 || (uint64_t)st.st_size > (64u << 20))
            return -1;
        xex_size = (uint32_t)st.st_size;
        xex = (uint8_t *)malloc(xex_size);
        if (!xex)
            return -1;
        FILE *fp = fopen(src_path, "rb");
        if (!fp || fread(xex, 1, xex_size, fp) != xex_size) {
            if (fp) fclose(fp);
            free(xex);
            return -1;
        }
        fclose(fp);
    }

    int rc = -1;
    uint8_t *spa = NULL;
    uint32_t spa_size = 0;

    if (memcmp(xex, "XEX2", 4) == 0) {
        uint32_t pe_offset = rd32be(xex + 8);
        uint32_t security = rd32be(xex + 0x10);
        uint32_t header_count = rd32be(xex + 0x14);
        uint32_t res_offset = 0, fmt_offset = 0;
        for (uint32_t i = 0; i < header_count && 0x18u + 8u * (i + 1u) <= xex_size; i++) {
            uint32_t key = rd32be(xex + 0x18u + 8u * i);
            uint32_t val = rd32be(xex + 0x1cu + 8u * i);
            if (key == 0x000002ffu) res_offset = val;
            if (key == 0x000003ffu) fmt_offset = val;
        }

        uint32_t spa_va = 0;
        if (res_offset != 0 && res_offset + 4u <= xex_size) {
            uint32_t info_size = rd32be(xex + res_offset);
            uint32_t n = info_size >= 4u ? (info_size - 4u) / 16u : 0;
            for (uint32_t i = 0; i < n && res_offset + 4u + 16u * (i + 1u) <= xex_size; i++) {
                const uint8_t *entry = xex + res_offset + 4u + 16u * i;
                if (memcmp(entry, "4D5307E6", 8) == 0) {
                    spa_va = rd32be(entry + 8);
                    spa_size = rd32be(entry + 12);
                    break;
                }
            }
        }

        if (spa_va >= 0x82000000u && spa_size > 24u && fmt_offset != 0 && fmt_offset + 8u <= xex_size &&
            pe_offset + 16u <= xex_size && security + 0x160u <= xex_size) {
            uint32_t fmt_size = rd32be(xex + fmt_offset);
            uint16_t encryption = rd16be(xex + fmt_offset + 4);
            uint16_t compression = rd16be(xex + fmt_offset + 6);
            if (compression == 1 && fmt_size >= 8u && encryption <= 1) {
                uint32_t blocks = (fmt_size - 8u) / 8u;
                uint32_t total_img = 0;
                for (uint32_t i = 0; i < blocks; i++)
                    total_img += rd32be(xex + fmt_offset + 8u + 8u * i) + rd32be(xex + fmt_offset + 12u + 8u * i);

                uint32_t res_off = spa_va - 0x82000000u;
                uint32_t res_end = res_off + spa_size;
                uint32_t raw_len = ((xex_size - pe_offset) / 16u) * 16u;
                const uint8_t *raw = xex + pe_offset;
                uint8_t session_rk[176];
                int header_ok = 1;

                if (encryption == 1) {
                    static const uint8_t k_retail_key[16] = {
                        0x20,0xB1,0x85,0xA5,0x9D,0x28,0xFD,0xC3,0x40,0x58,0x3F,0xBB,0x08,0x96,0xBF,0x91
                    };
                    aes128_init_tables();
                    uint8_t master_rk[176], session_key[16], blk0[16];
                    aes128_expand_key(k_retail_key, master_rk);
                    aes128_decrypt_block(master_rk, xex + security + 0x150, session_key);
                    aes128_expand_key(session_key, session_rk);
                    aes128_decrypt_block(session_rk, raw, blk0);
                    header_ok = (blk0[0] == 'M' && blk0[1] == 'Z');
                } else {
                    header_ok = (raw[0] == 'M' && raw[1] == 'Z');
                }

                if (header_ok && res_end <= total_img) {
                    spa = (uint8_t *)calloc(1, spa_size);
                    if (spa) {
                        uint32_t src_pos = 0, dst_pos = 0;
                        for (uint32_t i = 0; i < blocks && dst_pos < res_end; i++) {
                            uint32_t d = rd32be(xex + fmt_offset + 8u + 8u * i);
                            uint32_t z = rd32be(xex + fmt_offset + 12u + 8u * i);
                            uint32_t ov_start = dst_pos > res_off ? dst_pos : res_off;
                            uint32_t ov_end = (dst_pos + d) < res_end ? (dst_pos + d) : res_end;
                            if (ov_start < ov_end && src_pos + d <= raw_len) {
                                uint32_t raw_start = src_pos + (ov_start - dst_pos);
                                uint32_t raw_end = src_pos + (ov_end - dst_pos);
                                if (encryption == 0) {
                                    memcpy(spa + (ov_start - res_off), raw + raw_start, raw_end - raw_start);
                                } else {
                                    uint32_t blk_first = raw_start & ~15u;
                                    uint32_t blk_last = (raw_end + 15u) & ~15u;
                                    for (uint32_t blk = blk_first; blk < blk_last && blk + 16u <= raw_len; blk += 16u) {
                                        uint8_t pt[16];
                                        aes128_decrypt_block(session_rk, raw + blk, pt);
                                        if (blk > 0) {
                                            for (int b = 0; b < 16; b++)
                                                pt[b] ^= raw[blk - 16u + b];
                                        }
                                        uint32_t copy_s = blk > raw_start ? blk : raw_start;
                                        uint32_t copy_e = (blk + 16u) < raw_end ? (blk + 16u) : raw_end;
                                        memcpy(spa + (ov_start - res_off) + (copy_s - raw_start),
                                               pt + (copy_s - blk), copy_e - copy_s);
                                    }
                                }
                            }
                            src_pos += d;
                            dst_pos += d + z;
                        }

                        if (verify_xdbf_spa(spa, spa_size, out_ach_count, out_gamerscore, out_png_count) == 0) {
                            if (file_exists_with_size(dst_path, spa_size)) {
                                ex->bytes_skipped += spa_size;
                                ex->files_skipped++;
                                rc = 0;
                            } else {
                                FILE *dst = fopen(dst_path, "wb");
                                if (dst) {
                                    int wrote_ok = fwrite(spa, 1, spa_size, dst) == spa_size;
                                    if (fclose(dst) == 0 && wrote_ok) {
                                        ex->bytes_written += spa_size;
                                        ex->files_written++;
                                        rc = 0;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    free(spa);
    free(xex);
    return rc;
}

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 3 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr,
                "Halo 3: ODST Disc 2 (Halo 3: Mythic) DLC Map & Achievement Extractor\n"
                "Extracts all 13 Halo 3 DLC maps (Heroic, Legendary, Cold Storage, Mythic, Mythic II)\n"
                "and the 79-achievement SPA (4D5307E6.spa) into the Halo 3 port's DLC folder.\n\n"
                "Usage:\n"
                "  %s <odst_disc2.iso | extracted_disc_dir> [output_dlc_dir]\n\n"
                "If [output_dlc_dir] is omitted, extracts to:\n"
                "  $HALO3_DLC/odst_mythic  (if HALO3_DLC is set)\n"
                "  ~/.local/share/halo3/dlc/odst_mythic  (Linux)\n"
                "  %%LOCALAPPDATA%%\\halo3\\dlc\\odst_mythic  (Windows)\n",
                argv[0]);
        return (argc >= 2) ? 0 : 1;
    }

    const char *src_path = argv[1];
    char dst_dir[4096];
    if (argc == 3) {
        snprintf(dst_dir, sizeof(dst_dir), "%s", argv[2]);
    } else if (default_dlc_dir(dst_dir, sizeof(dst_dir)) != 0) {
        fprintf(stderr, "error: could not determine default DLC directory; pass [output_dlc_dir]\n");
        return 1;
    }

    extractor_t ex;
    memset(&ex, 0, sizeof(ex));
    ex.io_buf_sz = 4u << 20; // 4 MiB streaming copy buffer
    ex.io_buf = (uint8_t *)malloc(ex.io_buf_sz);
    if (!ex.io_buf) {
        fprintf(stderr, "error: out of memory\n");
        return 1;
    }

    if (is_directory(src_path)) {
        ex.from_iso = 0;
        ex.dir_root = src_path;
    } else {
        ex.from_iso = 1;
        if (xdvdfs_open(&ex.iso, src_path) != 0) {
            free(ex.io_buf);
            return 1;
        }
    }

    // Quick sanity check that this disc actually has the ODST Disc 2 DLC maps.
    {
        xdvdfs_entry_t probe;
        if (ex.from_iso) {
            if (xdvdfs_find_path(&ex.iso, "maps/docks.map", &probe) != 0 ||
                xdvdfs_find_path(&ex.iso, "maps/armory.map", &probe) != 0) {
                fprintf(stderr,
                        "error: '%s' does not appear to be Halo 3: ODST Disc 2 (Halo 3: Mythic).\n"
                        "       Missing maps/armory.map or maps/docks.map.\n",
                        src_path);
                xdvdfs_close(&ex.iso);
                free(ex.io_buf);
                return 1;
            }
        }
    }

    if (mkdir_p(dst_dir) != 0) {
        fprintf(stderr, "error: cannot create output directory '%s': %s\n", dst_dir, strerror(errno));
        if (ex.from_iso) xdvdfs_close(&ex.iso);
        free(ex.io_buf);
        return 1;
    }

    printf("Source: %s (%s)\n", src_path, ex.from_iso ? "Xbox 360 ISO" : "directory");
    printf("Target: %s\n\n", dst_dir);

    const size_t map_count = sizeof(k_dlc_maps) / sizeof(k_dlc_maps[0]);
    const size_t blf_count = sizeof(k_blf_suffixes) / sizeof(k_blf_suffixes[0]);
    int failed = 0;

    for (size_t i = 0; i < map_count; i++) {
        const dlc_map_desc_t *m = &k_dlc_maps[i];
        char rel[256], out[4096];
        printf("[%2zu/%2zu] %-12s | %-12s (%s) ... ", i + 1, map_count, m->pack, m->title, m->map_name);
        fflush(stdout);

        // 1. .mapinfo (placed in DLC folder root so Halo 3's DLC enumerator discovers it)
        snprintf(rel, sizeof(rel), "maps/info/%s.mapinfo", m->map_name);
        snprintf(out, sizeof(out), "%s%c%s.mapinfo", dst_dir, PATH_SEP, m->map_name);
        if (extract_one(&ex, rel, out, 0, m->map_name) != 0) {
            printf("FAILED (%s)\n", rel);
            failed++;
            continue;
        }

        // 2. .map (placed in DLC folder root matching mapinfo scenario_file)
        snprintf(rel, sizeof(rel), "maps/%s.map", m->map_name);
        snprintf(out, sizeof(out), "%s%c%s.map", dst_dir, PATH_SEP, m->map_name);
        if (extract_one(&ex, rel, out, 1, m->map_name) != 0) {
            printf("FAILED (%s)\n", rel);
            failed++;
            continue;
        }

        // 3. .blf preview/variant/clip/film images (matching mapinfo image_file_base)
        int blf_ok = 1;
        for (size_t j = 0; j < blf_count; j++) {
            snprintf(rel, sizeof(rel), "maps/images/%s%s", m->image_base, k_blf_suffixes[j]);
            snprintf(out, sizeof(out), "%s%c%s%s", dst_dir, PATH_SEP, m->image_base, k_blf_suffixes[j]);
            if (extract_one(&ex, rel, out, 0, m->map_name) != 0) {
                printf("FAILED (%s)\n", rel);
                blf_ok = 0;
                failed++;
                break;
            }
        }
        if (blf_ok)
            printf("ok\n");
    }

    // 4. Expanded Halo 3 SPA (79 achievements / 1,750 G + all PNG icons) from default.xex
    {
        char out[4096];
        uint32_t ach_count = 0, gamerscore = 0, png_count = 0;
        snprintf(out, sizeof(out), "%s%c4D5307E6.spa", dst_dir, PATH_SEP);
        printf("[ SPA ] Achievements | 4D5307E6.spa ... ");
        fflush(stdout);
        if (extract_spa_from_xex(&ex, out, &ach_count, &gamerscore, &png_count) != 0) {
            printf("FAILED (default.xex / 4D5307E6)\n");
            failed++;
        } else {
            printf("ok (%u achievements, %u G, %u PNG icons)\n", ach_count, gamerscore, png_count);
        }
    }

    if (ex.from_iso)
        xdvdfs_close(&ex.iso);
    free(ex.io_buf);

    printf("\nDone: %d files extracted (%.1f MiB), %d already up-to-date (%.1f MiB), %d failed.\n",
           ex.files_written, (double)ex.bytes_written / (1024.0 * 1024.0),
           ex.files_skipped, (double)ex.bytes_skipped / (1024.0 * 1024.0),
           failed);
    return failed ? 1 : 0;
}
