// Halo 3 Native PC Port - Linux GUI Launcher & First-Run Setup (halo3-launcher)
//
// Shared by BOTH Halo3-x86_64.AppImage and Halo3-x86_64.flatpak.
// Links ONLY against libc, libm, libpthread, libdl (GLIBC <= 2.31).
// Runtime-dlopens:
//   - libX11.so.6       (dark Halo 3 setup & progress window with Xdnd drag-and-drop)
//   - libfreetype.so.6  (renders bundled fonts; crisp bitmap fallback if absent)
//   - libdbus-1.so.3    (XDG Desktop Portal FileChooser.OpenFile - works in Flatpak sandbox
//                        with zero --filesystem=host, and native KDE/GNOME pickers on AppImage;
//                        falls back to kdialog / zenity)
//   - libvulkan.so.1    (preflight check with distro-specific install hints)
//
// User data directory: $HALO3_DATA -> $XDG_DATA_HOME/halo3 -> $HOME/.local/share/halo3
// (Inside Flatpak, Flatpak automatically maps XDG_DATA_HOME to
//  ~/.var/app/io.github.summers_aaron.Halo3/data and XDG_CONFIG_HOME to .../config.)

#define _GNU_SOURCE
#include <ctype.h>
#include <dbus/dbus.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "../shaders/shader_pack.h"
#include "../shaders/xenos_spirv_abi.h"

#define PATH_CAP 2048
#define GAMERTAG_MAX 15
#define PROFILE_LIST_MAX 32

// Halo 3 dark color palette (0x00RRGGBB)
#define COL_BG          0x0D1520u
#define COL_CARD        0x142130u
#define COL_CARD_BORDER 0x263D57u
#define COL_ACCENT      0x3898D8u
#define COL_ACCENT_HI   0x52AEECu
#define COL_ACCENT_DN   0x2476B0u
#define COL_BTN         0x1C2D42u
#define COL_BTN_HI      0x263C58u
#define COL_BTN_DN      0x142234u
#define COL_BTN_DIS     0x161F2Bu
#define COL_AMBER_BTN   0x362412u
#define COL_AMBER_HI    0x4D3318u
#define COL_AMBER_DN    0x26190Cu
#define COL_AMBER_BD    0xD8851Cu
#define COL_AMBER_BD_HI 0xFFAE3Du
#define COL_AMBER_TEXT  0xFFE2B0u
#define COL_TEXT        0xEAF4FFu
#define COL_TEXT_DIM    0x8EAAC4u
#define COL_TEXT_HINT   0x6886A2u
#define COL_TEXT_DIS    0x566A80u
#define COL_BAR_BG      0x0A111Au
#define COL_ERR         0xFF7070u
#define COL_OK          0x5EE39Eu

// ---------------------------------------------------------------------------
// Configuration & shared state
// ---------------------------------------------------------------------------
typedef struct {
    char bin_dir[PATH_CAP];
    char share_dir[PATH_CAP];
    char data_dir[PATH_CAP];
    char iso_path[PATH_CAP];
    char disc_path[PATH_CAP];
    char saved_disc_path[PATH_CAP];
    char odst_dlc_src[PATH_CAP];
    char shader_pack[PATH_CAP];
    char local_version[128];
    char shader_abi[64];
    char profile_name[GAMERTAG_MAX + 1];
    char profiles[PROFILE_LIST_MAX][GAMERTAG_MAX + 1];
    int  profile_count;
    int  profile_from_cli;
    int  skip_launcher;
    int  custom_shader_pack;
    int  reextract;
    int  rebuild_shaders;
    int  force_setup;
    int  headless;
    int  is_flatpak;
    int  jobs;
} LauncherConfig;

static LauncherConfig g_cfg;

enum {
    BTN_NONE = 0,
    BTN_PLAY,
    BTN_ISO,
    BTN_FOLDER,
    BTN_REBUILD,
    BTN_DATA,
    BTN_ODST,
    BTN_CANCEL,
};

typedef struct {
    int id;
    int x, y, w, h;
    int primary;
    int enabled;
    int visible;
    const char *label;
} UiButton;

typedef struct {
    pthread_mutex_t lock;
    pthread_t       worker;
    int             worker_started;
    pid_t           active_child;
    atomic_int      cancel_requested;
    int             busy;
    int             publishing;
    int             odst_only_job;
    int             ready_to_play;
    int             dlc_loaded;
    int             launch_after_ui;
    int             has_error;
    int             error_only_mode;
    int             progress_pct;
    char            stage_text[256];
    char            detail_text[1024];
    int             wake_pipe[2];
    int             done_flag;
    int             done_ok;
} UiState;

static UiState g_ui;

// ---------------------------------------------------------------------------
// Utility helpers
// ---------------------------------------------------------------------------
static void trim_ws(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\r' || s[n - 1] == '\n' || s[n - 1] == ' ' || s[n - 1] == '\t')) {
        s[--n] = 0;
    }
}

static int file_exists(const char *p) {
    struct stat st;
    return p && p[0] && stat(p, &st) == 0 && S_ISREG(st.st_mode);
}

static int dir_exists(const char *p) {
    struct stat st;
    return p && p[0] && stat(p, &st) == 0 && S_ISDIR(st.st_mode);
}

static int file_is_newer(const char *a, const char *b) {
    struct stat sa, sb;
    if (stat(a, &sa) != 0) return 0;
    if (stat(b, &sb) != 0) return 1;
    if (sa.st_mtim.tv_sec != sb.st_mtim.tv_sec) return sa.st_mtim.tv_sec > sb.st_mtim.tv_sec;
    return sa.st_mtim.tv_nsec > sb.st_mtim.tv_nsec;
}

static int join_path(char *out, size_t cap, const char *a, const char *b) {
    size_t la = strlen(a), lb = strlen(b);
    size_t sep = la > 0 && a[la - 1] != '/';
    if (la + sep + lb >= cap) {
        out[0] = 0;
        return 0;
    }
    memcpy(out, a, la);
    if (sep) out[la++] = '/';
    memcpy(out + la, b, lb + 1);
    return 1;
}

static void parent_dir(const char *path, char *out, size_t cap) {
    snprintf(out, cap, "%s", path);
    char *s = strrchr(out, '/');
    if (s == out) {
        out[1] = 0;
    } else if (s) {
        *s = 0;
    } else {
        snprintf(out, cap, ".");
    }
}

static void ensure_dir_recursive(const char *path) {
    char tmp[PATH_CAP];
    snprintf(tmp, sizeof(tmp), "%s", path);
    size_t len = strlen(tmp);
    while (len > 1 && tmp[len - 1] == '/') tmp[--len] = 0;
    for (char *p = tmp + 1; *p; ++p) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    mkdir(tmp, 0755);
}

static void read_small_text_file(const char *path, char *out, size_t cap) {
    out[0] = 0;
    FILE *f = fopen(path, "rb");
    if (!f) return;
    size_t n = fread(out, 1, cap - 1, f);
    if (fgetc(f) != EOF || ferror(f)) n = 0;
    fclose(f);
    out[n] = 0;
    trim_ws(out);
}

static int write_small_text_file(const char *path, const char *text) {
    FILE *f = fopen(path, "wb");
    if (!f) return 0;
    int ok = fprintf(f, "%s\n", text) >= 0;
    return fclose(f) == 0 && ok;
}

static int is_valid_disc_dir(const char *dir) {
    if (!dir || !dir[0] || !dir_exists(dir)) return 0;
    char xex[PATH_CAP], mm[PATH_CAP];
    join_path(xex, sizeof(xex), dir, "default.xex");
    join_path(mm, sizeof(mm), dir, "maps/mainmenu.map");
    return file_exists(xex) && file_exists(mm);
}

static int normalize_disc_dir(char out[PATH_CAP], const char *path) {
    char candidate[PATH_CAP];
    if (!path[0]) return 0;
    if (path[0] == '/') {
        if (strlen(path) >= sizeof(candidate)) return 0;
        strcpy(candidate, path);
    } else {
        char cwd[PATH_CAP];
        if (!getcwd(cwd, sizeof(cwd)) || !join_path(candidate, sizeof(candidate), cwd, path)) return 0;
    }
    size_t len = strlen(candidate);
    while (len > 1 && candidate[len - 1] == '/') candidate[--len] = 0;
    if (!is_valid_disc_dir(candidate)) {
        const char *name = strrchr(candidate, '/');
        if (!name || strcasecmp(name + 1, "maps") != 0) return 0;
        char parent[PATH_CAP];
        parent_dir(candidate, parent, sizeof(parent));
        if (!is_valid_disc_dir(parent)) return 0;
        strcpy(candidate, parent);
    }
    // Keep the selected path, including any Flatpak document portal grant.
    strcpy(out, candidate);
    return 1;
}

static int is_odst_dlc_dir(const char *dir) {
    if (!dir || !dir[0] || !dir_exists(dir)) return 0;
    char m1[PATH_CAP], m2[PATH_CAP], m3[PATH_CAP], intro[PATH_CAP];
    join_path(m1, sizeof(m1), dir, "maps/midship.map");
    join_path(m2, sizeof(m2), dir, "maps/armory.map");
    join_path(m3, sizeof(m3), dir, "maps/sandbox.map");
    join_path(intro, sizeof(intro), dir, "maps/005_intro.map");
    return file_exists(m1) && file_exists(m2) && file_exists(m3) && !file_exists(intro);
}

static int xdvdfs_find_entry(FILE *fp, uint64_t part_base, uint32_t dir_sector, uint32_t dir_size,
                             const char *target, uint32_t *out_sec, uint32_t *out_sz, uint8_t *out_attr) {
    if (dir_size == 0 || dir_size > (16u << 20)) return 0;
    uint8_t *buf = malloc(dir_size);
    if (!buf) return 0;
    if (fseeko(fp, (off_t)(part_base + (uint64_t)dir_sector * 2048ull), SEEK_SET) != 0 ||
        fread(buf, 1, dir_size, fp) != dir_size) {
        free(buf);
        return 0;
    }
    // This is the binary search tree within one directory. The caller only reads the root and maps/.
    uint16_t stack[4097] = {0};
    uint8_t visited[8192] = {0};
    size_t sp = 1;
    int found = 0;
    for (unsigned steps = 0; sp > 0 && steps < 4096; ++steps) {
        uint16_t node = stack[--sp];
        uint32_t off = (uint32_t)node * 4u;
        if (off + 14u > dir_size || (visited[node / 8] & (1u << (node % 8)))) continue;
        visited[node / 8] |= (uint8_t)(1u << (node % 8));
        uint16_t left  = (uint16_t)(buf[off] | ((uint16_t)buf[off + 1] << 8));
        uint16_t right = (uint16_t)(buf[off + 2] | ((uint16_t)buf[off + 3] << 8));
        uint32_t esec  = (uint32_t)buf[off + 4] | ((uint32_t)buf[off + 5] << 8) |
                         ((uint32_t)buf[off + 6] << 16) | ((uint32_t)buf[off + 7] << 24);
        uint32_t esz   = (uint32_t)buf[off + 8] | ((uint32_t)buf[off + 9] << 8) |
                         ((uint32_t)buf[off + 10] << 16) | ((uint32_t)buf[off + 11] << 24);
        uint8_t attr = buf[off + 12];
        uint8_t nlen = buf[off + 13];
        if (off + 14u + nlen > dir_size) continue;
        char ename[256];
        memcpy(ename, buf + off + 14, nlen);
        ename[nlen] = 0;
        if (strcasecmp(ename, target) == 0) {
            if (out_sec) *out_sec = esec;
            if (out_sz) *out_sz = esz;
            if (out_attr) *out_attr = attr;
            found = 1;
            break;
        }
        if (left != 0 && left != 0xFFFF) stack[sp++] = left;
        if (right != 0 && right != 0xFFFF) stack[sp++] = right;
    }
    free(buf);
    return found;
}

// Fast (<1ms) check whether an Xbox 360 ISO is Halo 3: ODST Disc 2 ("Halo 3: Mythic").
static int is_odst_dlc_iso(const char *iso_path) {
    if (!iso_path || !iso_path[0] || !file_exists(iso_path)) return 0;
    FILE *fp = fopen(iso_path, "rb");
    if (!fp) return 0;
    static const uint64_t bases[] = { 0x00000000ull, 0x0FD90000ull, 0x02080000ull, 0x18300000ull };
    uint64_t part_base = 0;
    uint32_t root_sec = 0, root_sz = 0;
    int valid_xiso = 0;
    uint8_t hdr[32];
    for (size_t i = 0; i < sizeof(bases) / sizeof(bases[0]); ++i) {
        if (fseeko(fp, (off_t)(bases[i] + 32ull * 2048ull), SEEK_SET) != 0) continue;
        if (fread(hdr, 1, sizeof(hdr), fp) != sizeof(hdr)) continue;
        if (memcmp(hdr, "MICROSOFT*XBOX*MEDIA", 20) == 0) {
            part_base = bases[i];
            root_sec = (uint32_t)hdr[20] | ((uint32_t)hdr[21] << 8) | ((uint32_t)hdr[22] << 16) | ((uint32_t)hdr[23] << 24);
            root_sz  = (uint32_t)hdr[24] | ((uint32_t)hdr[25] << 8) | ((uint32_t)hdr[26] << 16) | ((uint32_t)hdr[27] << 24);
            valid_xiso = 1;
            break;
        }
    }
    int is_odst = 0;
    if (valid_xiso) {
        uint32_t maps_sec = 0, maps_sz = 0;
        uint8_t maps_attr = 0;
        if (xdvdfs_find_entry(fp, part_base, root_sec, root_sz, "maps", &maps_sec, &maps_sz, &maps_attr) &&
            (maps_attr & 0x10u)) {
            int has_midship = xdvdfs_find_entry(fp, part_base, maps_sec, maps_sz, "midship.map", NULL, NULL, NULL);
            int has_armory  = xdvdfs_find_entry(fp, part_base, maps_sec, maps_sz, "armory.map",  NULL, NULL, NULL);
            int has_intro   = xdvdfs_find_entry(fp, part_base, maps_sec, maps_sz, "005_intro.map", NULL, NULL, NULL);
            is_odst = (has_midship && has_armory && !has_intro);
        }
    }
    fclose(fp);
    return is_odst;
}

static int check_odst_dlc_installed(void) {
    char base[PATH_CAP];
    const char *env_dlc = getenv("HALO3_DLC");
    if (env_dlc && env_dlc[0]) {
        join_path(base, sizeof(base), env_dlc, "odst_mythic");
    } else {
        join_path(base, sizeof(base), g_cfg.data_dir, "dlc/odst_mythic");
    }
    char m1[PATH_CAP], m2[PATH_CAP], m3[PATH_CAP];
    join_path(m1, sizeof(m1), base, "midship.map");
    join_path(m2, sizeof(m2), base, "armory.map");
    join_path(m3, sizeof(m3), base, "sandbox.map");
    return file_exists(m1) && file_exists(m2) && file_exists(m3);
}

// Matches port/sdk/xam/profiles.c gamertag_xuid() and port_profile_gamertag_valid() exactly.
static uint64_t gamertag_xuid(const char *tag) {
    uint64_t h = 0xcbf29ce484222325ull;
    for (const char *p = tag; *p; ++p) h = (h ^ (unsigned char)*p) * 0x100000001b3ull;
    return 0xE000000000000000ull | (h & 0x0000ffffffffffffull) | 2ull;
}

static int gamertag_ieq(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++; b++;
    }
    return tolower((unsigned char)*a) == tolower((unsigned char)*b);
}

static int gamertag_valid(const char *tag) {
    if (!tag || !isalpha((unsigned char)tag[0])) return 0;
    size_t n = 0;
    for (; tag[n]; ++n) {
        unsigned char c = (unsigned char)tag[n];
        int ok = isalnum(c) || (c == ' ' && tag[n - 1] != ' ');
        if (n >= GAMERTAG_MAX || !ok) return 0;
    }
    return n > 0 && tag[n - 1] != ' ';
}

static void add_profile_unique(const char *tag) {
    if (!tag || !tag[0] || g_cfg.profile_count >= PROFILE_LIST_MAX) return;
    for (int i = 0; i < g_cfg.profile_count; ++i) {
        if (gamertag_ieq(g_cfg.profiles[i], tag)) return;
    }
    snprintf(g_cfg.profiles[g_cfg.profile_count], GAMERTAG_MAX + 1, "%s", tag);
    g_cfg.profile_count++;
}

static void load_launcher_prefs(void) {
    g_cfg.profile_count = 0;
    char saved_profile[GAMERTAG_MAX + 1] = {0};

    // 1. Read launcher.ini (profile + skip_launcher)
    char ini_path[PATH_CAP];
    join_path(ini_path, sizeof(ini_path), g_cfg.data_dir, "launcher.ini");
    FILE *f = fopen(ini_path, "rb");
    if (f) {
        char line[PATH_CAP + 16];
        while (fgets(line, sizeof(line), f)) {
            // Strip only the record terminator. The disc path may end in spaces.
            line[strcspn(line, "\r\n")] = 0;
            if (strncmp(line, "profile=", 8) == 0) {
                snprintf(saved_profile, sizeof(saved_profile), "%s", line + 8);
                trim_ws(saved_profile);
            } else if (strncmp(line, "skip_launcher=", 14) == 0) {
                g_cfg.skip_launcher = (atoi(line + 14) != 0);
            } else if (strncmp(line, "disc_path=", 10) == 0) {
                if (strlen(line + 10) < sizeof(g_cfg.saved_disc_path))
                    strcpy(g_cfg.saved_disc_path, line + 10);
            }
        }
        fclose(f);
    }

    // 2. Read <data>/profiles/catalog.txt created by Halo 3 Guide or launcher
    char cat_path[PATH_CAP];
    join_path(cat_path, sizeof(cat_path), g_cfg.data_dir, "profiles/catalog.txt");
    char catalog_tmp[PROFILE_LIST_MAX][GAMERTAG_MAX + 1];
    int catalog_n = 0;
    FILE *cf = fopen(cat_path, "rb");
    if (cf) {
        char line[128];
        if (fgets(line, sizeof(line), cf) && strncmp(line, "H3PROFILES 1", 12) == 0) {
            while (fgets(line, sizeof(line), cf) && catalog_n < PROFILE_LIST_MAX) {
                trim_ws(line);
                if (strlen(line) >= 18 && line[16] == ' ') {
                    const char *gt = line + 17;
                    if (gamertag_valid(gt)) {
                        snprintf(catalog_tmp[catalog_n++], GAMERTAG_MAX + 1, "%s", gt);
                    }
                }
            }
        }
        fclose(cf);
    }

    // 3. Determine active profile (CLI/env > saved launcher.ini > Player 1)
    if (!g_cfg.profile_from_cli) {
        const char *env_p = getenv("HALO3_PROFILE_NAME");
        if (!env_p || !env_p[0]) env_p = getenv("HALO3_GAMERTAG");
        if (env_p && gamertag_valid(env_p)) {
            snprintf(g_cfg.profile_name, sizeof(g_cfg.profile_name), "%s", env_p);
        } else if (saved_profile[0] && gamertag_valid(saved_profile)) {
            snprintf(g_cfg.profile_name, sizeof(g_cfg.profile_name), "%s", saved_profile);
        } else {
            snprintf(g_cfg.profile_name, sizeof(g_cfg.profile_name), "Player 1");
        }
    }

    add_profile_unique(g_cfg.profile_name);
    add_profile_unique("Player 1");
    for (int i = 0; i < catalog_n; ++i) add_profile_unique(catalog_tmp[i]);
}

static int write_launcher_prefs_stream(FILE *f, const char *disc) {
    if (!f) return 0;
    int ok = fprintf(f, "profile=%s\nskip_launcher=%d\n",
            g_cfg.profile_name[0] ? g_cfg.profile_name : "Player 1",
            g_cfg.skip_launcher ? 1 : 0) >= 0;
    if (disc[0])
        ok = ok && !strpbrk(disc, "\r\n") && fprintf(f, "disc_path=%s\n", disc) >= 0;
    return fclose(f) == 0 && ok;
}

static void save_launcher_prefs(void) {
    ensure_dir_recursive(g_cfg.data_dir);
    char ini_path[PATH_CAP], tmp[PATH_CAP];
    if (!join_path(ini_path, PATH_CAP, g_cfg.data_dir, "launcher.ini") ||
        !join_path(tmp, PATH_CAP, g_cfg.data_dir, "launcher.ini.XXXXXX"))
        return;
    int fd = mkstemp(tmp);
    if (fd < 0) return;
    FILE *f = fdopen(fd, "wb");
    if (!f) close(fd);
    if (!write_launcher_prefs_stream(f, g_cfg.saved_disc_path) || rename(tmp, ini_path) != 0)
        unlink(tmp);
}

static int commit_selected_profile(char *err_out, size_t err_cap) {
    // Trim leading/trailing spaces in-place
    char cleaned[GAMERTAG_MAX + 1];
    const char *p = g_cfg.profile_name;
    while (*p == ' ') p++;
    snprintf(cleaned, sizeof(cleaned), "%s", p);
    trim_ws(cleaned);
    if (!cleaned[0]) snprintf(cleaned, sizeof(cleaned), "Player 1");

    if (!gamertag_valid(cleaned)) {
        if (err_out) snprintf(err_out, err_cap, "Start with a letter (1-15 chars)");
        return 0;
    }
    snprintf(g_cfg.profile_name, sizeof(g_cfg.profile_name), "%s", cleaned);
    add_profile_unique(cleaned);
    save_launcher_prefs();

    // Register non-builtin custom gamertag in profiles/catalog.txt so Guide & launcher stay in sync
    if (!gamertag_ieq(cleaned, "Player 1") && !gamertag_ieq(cleaned, "Player 2") &&
        !gamertag_ieq(cleaned, "Player 3") && !gamertag_ieq(cleaned, "Player 4")) {
        char prof_dir[PATH_CAP], cat_path[PATH_CAP];
        join_path(prof_dir, sizeof(prof_dir), g_cfg.data_dir, "profiles");
        ensure_dir_recursive(prof_dir);
        join_path(cat_path, sizeof(cat_path), prof_dir, "catalog.txt");

        int exists = 0, has_header = 0, entry_count = 0;
        char xuid_hex[17];
        snprintf(xuid_hex, sizeof(xuid_hex), "%016llX", (unsigned long long)gamertag_xuid(cleaned));
        FILE *rf = fopen(cat_path, "rb");
        if (rf) {
            char line[128];
            if (fgets(line, sizeof(line), rf) && strncmp(line, "H3PROFILES 1", 12) == 0) {
                has_header = 1;
                while (fgets(line, sizeof(line), rf)) {
                    trim_ws(line);
                    if (strlen(line) >= 18 && line[16] == ' ') {
                        entry_count++;
                        if (gamertag_ieq(line + 17, cleaned) || strncasecmp(line, xuid_hex, 16) == 0) {
                            exists = 1;
                        }
                    }
                }
            }
            fclose(rf);
        }
        // Engine catalog_parse() allows at most PROFILE_CREATED_MAX (12) created entries
        if (!exists && entry_count < 12) {
            FILE *wf = fopen(cat_path, has_header ? "ab" : "wb");
            if (wf) {
                if (!has_header) fprintf(wf, "H3PROFILES 1\n");
                fprintf(wf, "%s %s\n", xuid_hex, cleaned);
                fclose(wf);
            }
        }
    }
    return 1;
}

static int url_decode_file_uri(const char *uri, char *out, size_t cap) {
    const char *p = uri;
    if (strncmp(p, "file://", 7) == 0) {
        p += 7;
        // Skip optional host part (file://localhost/...)
        if (*p != '/') {
            const char *slash = strchr(p, '/');
            if (!slash) return 0;
            p = slash;
        }
    }
    size_t j = 0;
    while (*p && *p != '\r' && *p != '\n' && j + 1 < cap) {
        if (p[0] == '%' && isxdigit((unsigned char)p[1]) && isxdigit((unsigned char)p[2])) {
            char hex[3] = { p[1], p[2], 0 };
            out[j++] = (char)strtol(hex, NULL, 16);
            p += 3;
        } else {
            out[j++] = *p++;
        }
    }
    out[j] = 0;
    return j > 0;
}

static void distro_vulkan_hint(char *out, size_t cap) {
    char id[128] = {0}, id_like[256] = {0};
    FILE *f = fopen("/etc/os-release", "r");
    if (f) {
        char line[512];
        while (fgets(line, sizeof(line), f)) {
            if (strncmp(line, "ID=", 3) == 0) {
                snprintf(id, sizeof(id), "%s", line + 3);
            } else if (strncmp(line, "ID_LIKE=", 8) == 0) {
                snprintf(id_like, sizeof(id_like), "%s", line + 8);
            }
        }
        fclose(f);
    }
    char comb[512];
    snprintf(comb, sizeof(comb), " %s %s ", id, id_like);
    for (char *p = comb; *p; ++p) {
        if (*p == '"' || *p == '\n' || *p == '\r') *p = ' ';
        else *p = (char)tolower((unsigned char)*p);
    }
    if (strstr(comb, " fedora ") || strstr(comb, " rhel ") || strstr(comb, " centos ")) {
        snprintf(out, cap, "Fedora/RHEL: sudo dnf install vulkan-loader mesa-vulkan-drivers");
    } else if (strstr(comb, " ubuntu ") || strstr(comb, " debian ") || strstr(comb, " pop ") || strstr(comb, " mint ")) {
        snprintf(out, cap, "Ubuntu/Debian: sudo apt update && sudo apt install libvulkan1 mesa-vulkan-drivers");
    } else if (strstr(comb, " arch ") || strstr(comb, " steamos ") || strstr(comb, " cachyos ") || strstr(comb, " manjaro ")) {
        snprintf(out, cap, "Arch Linux: sudo pacman -S vulkan-icd-loader vulkan-radeon (or vulkan-intel / nvidia-utils)");
    } else if (strstr(comb, " suse ")) {
        snprintf(out, cap, "openSUSE: sudo zypper install libvulkan1 Mesa-vulkan-device-select");
    } else {
        snprintf(out, cap, "Install your distribution's 64-bit Vulkan loader (libvulkan.so.1) and GPU Vulkan driver.");
    }
}

static int has_vulkan_loader(void) {
    void *h = dlopen("libvulkan.so.1", RTLD_LAZY | RTLD_LOCAL);
    if (!h) return 0;
    dlclose(h);
    return 1;
}

static int valid_pe(const char *path) {
    if (!file_exists(path)) return 0;
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    uint8_t header[64], signature[4];
    int valid = 0;
    if (fread(header, 1, sizeof(header), f) == sizeof(header) && header[0] == 'M' && header[1] == 'Z') {
        uint32_t offset = (uint32_t)header[60] | ((uint32_t)header[61] << 8) |
                          ((uint32_t)header[62] << 16) | ((uint32_t)header[63] << 24);
        valid = fseeko(f, offset, SEEK_SET) == 0 &&
                fread(signature, 1, sizeof(signature), f) == sizeof(signature) &&
                memcmp(signature, "PE\0\0", sizeof(signature)) == 0;
    }
    fclose(f);
    return valid;
}

// Check the pack header, file length and table checksum before offering Play.
static int shader_pack_abi(const char *path) {
    if (!file_exists(path)) return 0;
    FILE *f = fopen(path, "rb");
    if (!f) return 0;

    union {
        struct s_shader_pack_file_header v2;
        struct s_shader_pack_file_header_v3 v3;
    } header;
    int abi = 0;
    if (fread(&header, 1, sizeof(header), f) != sizeof(header) ||
        header.v3.magic != SHADER_PACK_MAGIC ||
        fseeko(f, 0, SEEK_END) != 0 ||
        ftello(f) != (int64_t)header.v3.file_size)
        goto done;

    uint32_t header_size, blob_offset, table_crc;
    int version_abi;
    if (header.v3.version == SHADER_PACK_VERSION_XENOS && header.v3.module_count && header.v3.xenos_count) {
        version_abi = (int)header.v3.abi_version;
        if (version_abi != XE_ABI_VERSION && version_abi != XE_ABI_VERSION_BINDLESS) goto done;
        header_size = version_abi == XE_ABI_VERSION_BINDLESS ? sizeof(struct s_shader_pack_file_header_v3_abi6)
                                                            : sizeof(header.v3);
        blob_offset = header.v3.blob_offset;
        table_crc = header.v3.table_crc32;
        if (header.v3.blob_size != header.v3.file_size - blob_offset) goto done;
    } else if (header.v2.version == SHADER_PACK_VERSION && header.v2.module_count && header.v2.microcode_count) {
        version_abi = SHADER_PACK_VERSION;
        header_size = sizeof(header.v2);
        blob_offset = header.v2.blob_offset;
        table_crc = header.v2.table_crc32;
        if (header.v2.blob_size != header.v2.file_size - blob_offset) goto done;
    } else {
        goto done;
    }
    if (header.v3.header_size != header_size || blob_offset <= header_size || blob_offset >= header.v3.file_size ||
        fseeko(f, header_size, SEEK_SET) != 0)
        goto done;

    uint32_t crc_table[256];
    for (uint32_t i = 0; i < 256; ++i) {
        uint32_t entry = i;
        for (int bit = 0; bit < 8; ++bit)
            entry = (entry >> 1) ^ ((entry & 1u) ? 0xedb88320u : 0);
        crc_table[i] = entry;
    }
    uint32_t crc = 0xffffffffu;
    uint8_t bytes[8192];
    for (uint32_t remaining = blob_offset - header_size; remaining > 0;) {
        size_t n = remaining < sizeof(bytes) ? remaining : sizeof(bytes);
        if (fread(bytes, 1, n, f) != n) goto done;
        for (size_t i = 0; i < n; ++i)
            crc = (crc >> 8) ^ crc_table[(crc ^ bytes[i]) & 0xffu];
        remaining -= (uint32_t)n;
    }
    if ((crc ^ 0xffffffffu) == table_crc) abi = version_abi;

done:
    fclose(f);
    return abi;
}

static int select_setup_disc(void) {
    if (g_cfg.disc_path[0])
        return normalize_disc_dir(g_cfg.disc_path, g_cfg.disc_path);
    if (g_cfg.saved_disc_path[0] && normalize_disc_dir(g_cfg.disc_path, g_cfg.saved_disc_path))
        return 1;
    char cached[PATH_CAP];
    if (join_path(cached, PATH_CAP, g_cfg.data_dir, "disc") && normalize_disc_dir(g_cfg.disc_path, cached))
        return 1;
    return 0;
}

static const char *external_disc_path(const char *disc) {
    char cached[PATH_CAP];
    join_path(cached, PATH_CAP, g_cfg.data_dir, "disc");
    return strcmp(disc, cached) == 0 ? "" : disc;
}

static int managed_pack_current(void) {
    if (shader_pack_abi(g_cfg.shader_pack) != XE_ABI_VERSION) return 0;
    char pack_dir[PATH_CAP], sidecar[PATH_CAP], text[64];
    parent_dir(g_cfg.shader_pack, pack_dir, PATH_CAP);
    if (g_cfg.shader_abi[0]) {
        join_path(sidecar, PATH_CAP, pack_dir, "abi.txt");
        read_small_text_file(sidecar, text, 64);
        if (strcmp(text, g_cfg.shader_abi) != 0) return 0;
    }
    if (check_odst_dlc_installed()) {
        join_path(sidecar, PATH_CAP, pack_dir, "dlc.txt");
        read_small_text_file(sidecar, text, 64);
        if (strcmp(text, "1") != 0) return 0;
    }
    return 1;
}

static int check_setup_ready(char *reason, size_t reason_cap) {
    if (reason && reason_cap) reason[0] = 0;
    if (g_cfg.iso_path[0]) {
        if (reason) snprintf(reason, reason_cap, "%s", g_cfg.reextract ?
                             "Re-extracting the selected Halo 3 ISO..." : "Preparing the selected Halo 3 ISO...");
        return 0;
    }
    if (!select_setup_disc()) {
        if (reason) snprintf(reason, reason_cap, "Select your retail Halo 3 ISO or extracted disc folder.");
        return 0;
    }
    if (g_cfg.custom_shader_pack && !shader_pack_abi(g_cfg.shader_pack)) {
        if (reason) snprintf(reason, reason_cap, "The specified Vulkan shader pack is missing, corrupt, or incompatible.");
        return 0;
    }

    char pe[PATH_CAP], xex[PATH_CAP];
    join_path(pe, PATH_CAP, g_cfg.data_dir, "default.pe");
    join_path(xex, PATH_CAP, g_cfg.disc_path, "default.xex");
    if (!valid_pe(pe) || file_is_newer(xex, pe) ||
        strcmp(external_disc_path(g_cfg.disc_path), g_cfg.saved_disc_path) != 0) {
        if (reason) snprintf(reason, reason_cap, "Updating retail executable data...");
        return 0;
    }
    if (!g_cfg.custom_shader_pack && (g_cfg.rebuild_shaders || !managed_pack_current())) {
        if (reason) snprintf(reason, reason_cap, "The managed Vulkan shader pack needs rebuilding.");
        return 0;
    }
    return 1;
}

static void ui_wake(void) {
    if (g_ui.wake_pipe[1] >= 0) {
        char b = 1;
        ssize_t r = write(g_ui.wake_pipe[1], &b, 1);
        (void)r;
    }
}

static void ui_set_status(int pct, int err, const char *stage, const char *detail) {
    pthread_mutex_lock(&g_ui.lock);
    if (pct >= 0) g_ui.progress_pct = pct;
    g_ui.has_error = err;
    if (stage) snprintf(g_ui.stage_text, sizeof(g_ui.stage_text), "%s", stage);
    if (detail) snprintf(g_ui.detail_text, sizeof(g_ui.detail_text), "%s", detail);
    pthread_mutex_unlock(&g_ui.lock);
    ui_wake();
}

// ---------------------------------------------------------------------------
// Subprocess runner with live PROGRESS parsing & setup.log recording
// ---------------------------------------------------------------------------
static int run_tool_with_progress(const char *const argv[], FILE *log_file,
                                  int pct_start, int pct_end, const char *stage_label,
                                  char *last_err, size_t last_err_cap) {
    if (g_ui.cancel_requested) return -2;

    int pfd[2];
    if (pipe(pfd) != 0) return -1;

    pid_t pid = fork();
    if (pid < 0) {
        close(pfd[0]);
        close(pfd[1]);
        return -1;
    }
    if (pid == 0) {
        close(pfd[0]);
        dup2(pfd[1], STDOUT_FILENO);
        dup2(pfd[1], STDERR_FILENO);
        close(pfd[1]);
        execv(argv[0], (char *const *)argv);
        fprintf(stderr, "failed to exec %s: %s\n", argv[0], strerror(errno));
        _exit(127);
    }
    close(pfd[1]);

    pthread_mutex_lock(&g_ui.lock);
    g_ui.active_child = pid;
    if (g_ui.cancel_requested) kill(pid, SIGTERM);
    pthread_mutex_unlock(&g_ui.lock);

    char chunk[1024];
    char line[1024];
    size_t line_len = 0;
    ssize_t nr;
    while ((nr = read(pfd[0], chunk, sizeof(chunk))) > 0) {
        if (log_file) {
            fwrite(chunk, 1, (size_t)nr, log_file);
            fflush(log_file);
        }
        if (g_cfg.headless) {
            fwrite(chunk, 1, (size_t)nr, stdout);
            fflush(stdout);
        }
        for (ssize_t i = 0; i < nr; ++i) {
            char c = chunk[i];
            if (c == '\r') continue;
            if (c == '\n') {
                line[line_len] = 0;
                if (strncmp(line, "PROGRESS ", 9) == 0) {
                    char *endp = NULL;
                    long p = strtol(line + 9, &endp, 10);
                    if (p < 0) p = 0;
                    if (p > 100) p = 100;
                    const char *msg = (endp && *endp == ' ') ? (endp + 1) : "";
                    int mapped = pct_start + (int)((p * (pct_end - pct_start)) / 100);
                    ui_set_status(mapped, 0, stage_label, msg);
                } else if (line[0] == '[') {
                    int cur = 0, tot = 0;
                    if (sscanf(line, "[%d/%d]", &cur, &tot) == 2 && tot > 0) {
                        int p = (cur * 92) / tot;
                        int mapped = pct_start + (p * (pct_end - pct_start)) / 100;
                        ui_set_status(mapped, 0, stage_label, line);
                    } else if (strncmp(line, "[ SPA ]", 7) == 0) {
                        int mapped = pct_start + (96 * (pct_end - pct_start)) / 100;
                        ui_set_status(mapped, 0, stage_label, "Extracting 79 Halo 3 achievements & icons (1,750 G)...");
                    }
                } else if (line_len > 0) {
                    snprintf(last_err, last_err_cap, "%s", line);
                }
                line_len = 0;
            } else if (line_len < sizeof(line) - 1) {
                line[line_len++] = c;
            }
        }
    }
    close(pfd[0]);

    int status = 0;
    pid_t waited;
    do {
        waited = waitpid(pid, &status, 0);
    } while (waited < 0 && errno == EINTR);

    pthread_mutex_lock(&g_ui.lock);
    g_ui.active_child = 0;
    pthread_mutex_unlock(&g_ui.lock);

    if (g_ui.cancel_requested) return -2;
    if (waited == pid && WIFEXITED(status)) return WEXITSTATUS(status);
    return 1;
}

typedef struct {
    char path[PATH_CAP];
    char tmp[PATH_CAP];
    char backup[PATH_CAP];
    int had_previous;
} SetupFile;

static int remove_setup_file(const char *path) {
    return unlink(path) == 0 || errno == ENOENT;
}

static int prepare_setup_file(SetupFile *file, const char *dir, const char *name) {
    if (!join_path(file->path, PATH_CAP, dir, name)) return 0;
    if (snprintf(file->tmp, PATH_CAP, "%s.tmp", file->path) >= PATH_CAP ||
        snprintf(file->backup, PATH_CAP, "%s.bak", file->path) >= PATH_CAP) {
        file->tmp[0] = 0;
        return 0;
    }
    return remove_setup_file(file->tmp);
}

static int move_setup_path(const char *from, const char *to) {
    return rename(from, to) == 0;
}

static int sync_setup_file(const char *path) {
    int fd = open(path, O_RDWR | O_CLOEXEC | O_NOFOLLOW);
    if (fd < 0) return 0;
    int ok = fsync(fd) == 0;
    return close(fd) == 0 && ok;
}

static int backup_setup_file(const SetupFile *file) {
    if (link(file->path, file->backup) == 0) return 1;
    if (errno != EPERM && errno != EOPNOTSUPP) return 0;

    // Some filesystems, including exFAT, cannot create hard links.
    FILE *source = fopen(file->path, "rb");
    if (!source) return 0;
    int fd = open(file->backup, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
    if (fd < 0) {
        fclose(source);
        return 0;
    }
    FILE *backup = fdopen(fd, "wb");
    int ok = backup != NULL;
    if (backup) {
        char bytes[65536];
        size_t n;
        while (ok && (n = fread(bytes, 1, sizeof(bytes), source)) != 0)
            ok = fwrite(bytes, 1, n, backup) == n;
        ok = fclose(backup) == 0 && ok && !ferror(source);
    } else {
        close(fd);
    }
    fclose(source);
    if (!ok) remove_setup_file(file->backup);
    return ok;
}

// Only used on extracted staging trees and retired caches. Never traverse links.
static void remove_disc_tree(const char *path, unsigned depth) {
    if (depth == 0) return;
    struct stat st;
    if (lstat(path, &st) != 0) return;
    if (!S_ISDIR(st.st_mode)) {
        unlink(path);
        return;
    }
    DIR *dir = opendir(path);
    if (!dir) return;
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        char child[PATH_CAP];
        if (join_path(child, PATH_CAP, path, entry->d_name)) remove_disc_tree(child, depth - 1);
    }
    closedir(dir);
    rmdir(path);
}

static int setup_cancelled(void) {
    return g_ui.cancel_requested;
}

static int start_publishing(void) {
    pthread_mutex_lock(&g_ui.lock);
    int publish = !setup_cancelled();
    g_ui.publishing = publish;
    pthread_mutex_unlock(&g_ui.lock);
    return publish;
}

// Keep backups until every replacement has been published.
static int publish_setup(SetupFile files[], size_t count, const char *disc, const char *staged_disc,
                         const char *disc_backup) {
    size_t published = 0;
    int saved_disc = 0, moved_disc = 0, ok = 0;
    for (size_t i = 0; i < count; ++i) {
        SetupFile *file = &files[i];
        if (!sync_setup_file(file->tmp)) goto finish;
        struct stat st;
        if (lstat(file->path, &st) != 0) {
            if (errno != ENOENT) goto finish;
            continue;
        }
        if (!backup_setup_file(file)) goto finish;
        file->had_previous = 1;
    }
    if (!start_publishing()) goto finish;
    if (staged_disc[0]) {
        struct stat st;
        if (lstat(disc_backup, &st) == 0 || errno != ENOENT) goto finish;
        if (lstat(disc, &st) == 0) {
            if (!S_ISDIR(st.st_mode) || rename(disc, disc_backup) != 0) goto finish;
            saved_disc = 1;
        } else if (errno != ENOENT) {
            goto finish;
        }
        if (rename(staged_disc, disc) != 0) goto finish;
        moved_disc = 1;
    }
    for (; published < count; ++published) {
        if (!move_setup_path(files[published].tmp, files[published].path)) goto finish;
    }
    ok = 1;

finish:
    if (!ok) {
        for (size_t i = 0; i < published; ++i) {
            if (files[i].had_previous) {
                move_setup_path(files[i].backup, files[i].path);
                files[i].had_previous = 0;
            } else {
                remove_setup_file(files[i].path);
            }
        }
        if (moved_disc) move_setup_path(disc, staged_disc);
        if (saved_disc) move_setup_path(disc_backup, disc);
    } else if (saved_disc) {
        remove_disc_tree(disc_backup, 32);
    }
    for (size_t i = 0; i < count; ++i) {
        if (files[i].had_previous) remove_setup_file(files[i].backup);
    }
    return ok;
}

static void *setup_worker_proc(void *arg) {
    (void)arg;
    SetupFile files[5] = {0};
    size_t count = 0;
    char cached_disc[PATH_CAP], staged_disc[PATH_CAP] = {0}, disc_backup[PATH_CAP] = {0};
    char pack_manifest[PATH_CAP] = {0}, last_err[512] = {0};
    int ok = 0, rc = 0;
    int lock = -1;
    FILE *log_file = NULL;

    ensure_dir_recursive(g_cfg.data_dir);
    char lock_path[PATH_CAP], log_path[PATH_CAP];
    if (!join_path(lock_path, PATH_CAP, g_cfg.data_dir, ".setup.lock") ||
        !join_path(log_path, PATH_CAP, g_cfg.data_dir, "setup.log") ||
        !join_path(cached_disc, PATH_CAP, g_cfg.data_dir, "disc"))
        goto staging_failed;
    lock = open(lock_path, O_WRONLY | O_CREAT | O_CLOEXEC | O_NOFOLLOW, 0600);
    if (lock < 0) goto staging_failed;
    if (flock(lock, LOCK_EX | LOCK_NB) != 0) {
        if (errno != EWOULDBLOCK) goto staging_failed;
        ui_set_status(0, 1, "Halo 3 Setup Is Busy", "Another launcher is preparing this data folder.");
        goto finish;
    }
    log_file = fopen(log_path, "wb");

    if (g_cfg.custom_shader_pack && !shader_pack_abi(g_cfg.shader_pack)) {
        ui_set_status(0, 1, "Invalid Vulkan Shader Pack",
                      "The specified pack is missing, corrupt, or incompatible. Provide a valid pack or remove the override.");
        goto finish;
    }

    int has_base = select_setup_disc();
    int need_extract = g_cfg.iso_path[0] != 0;
    int did_odst = 0;
    char dlc_root[PATH_CAP] = {0};
    const char *env_dlc = getenv("HALO3_DLC");
    if (env_dlc && env_dlc[0]) {
        if (strlen(env_dlc) >= sizeof(dlc_root)) goto staging_failed;
        strcpy(dlc_root, env_dlc);
    } else if (!join_path(dlc_root, PATH_CAP, g_cfg.data_dir, "dlc")) {
        goto staging_failed;
    }

    if (g_cfg.odst_dlc_src[0]) {
        char dlc_out[PATH_CAP], odst_tool[PATH_CAP];
        if (!join_path(dlc_out, PATH_CAP, dlc_root, "odst_mythic") ||
            !join_path(odst_tool, PATH_CAP, g_cfg.bin_dir, "extract-odst-dlc"))
            goto staging_failed;
        ensure_dir_recursive(dlc_out);
        int odst_hi = (has_base || need_extract) ? 38 : 99;
        const char *label = "Extracting Halo 3: ODST Disc 2 DLC";
        ui_set_status(2, 0, label, "Reading Mythic disc maps and achievements...");
        const char *argv[] = { odst_tool, g_cfg.odst_dlc_src, dlc_out, NULL };
        rc = run_tool_with_progress(argv, log_file, 2, odst_hi, label, last_err, 512);
        if (setup_cancelled()) goto cancelled;
        if (rc != 0) {
            ui_set_status(0, 1, "ODST DLC Extraction Failed",
                          last_err[0] ? last_err : "Select a retail Halo 3: ODST Disc 2 (Halo 3: Mythic) ISO.");
            goto finish;
        }
        g_cfg.odst_dlc_src[0] = 0;
        did_odst = 1;
        if (!need_extract && !has_base && g_ui.odst_only_job) {
            ui_set_status(0, 0, "First-Time Setup", "ODST Disc 2 DLC loaded. Select your Halo 3 ISO next.");
            ok = 1;
            goto finish;
        }
    }

    const char *disc = g_cfg.disc_path;
    if (need_extract) {
        if (!file_exists(g_cfg.iso_path)) {
            ui_set_status(0, 1, "Halo 3 ISO Unavailable", "Choose an ISO file the launcher can read.");
            goto finish;
        }
        char xiso[PATH_CAP], disc_tmp[PATH_CAP];
        if (!join_path(xiso, PATH_CAP, g_cfg.bin_dir, "xiso") ||
            !join_path(disc_tmp, PATH_CAP, g_cfg.data_dir, "disc.tmp") ||
            !join_path(disc_backup, PATH_CAP, g_cfg.data_dir, "disc.bak"))
            goto staging_failed;
        remove_disc_tree(disc_tmp, 32);
        if (!(mkdir(disc_tmp, 0755) == 0)) goto staging_failed;
        strcpy(staged_disc, disc_tmp);
        disc = staged_disc;
        const char *label = "Step 1 of 2 - Extracting Halo 3 ISO";
        ui_set_status(did_odst ? 38 : 1, 0, label, "Scanning Xbox 360 disc image...");
        const char *argv[] = { xiso, "extract", "--progress", g_cfg.iso_path, disc, NULL };
        last_err[0] = 0;
        rc = run_tool_with_progress(argv, log_file, did_odst ? 38 : 0, 55, label, last_err, 512);
        if (setup_cancelled()) goto cancelled;
        if (rc != 0 || !is_valid_disc_dir(disc)) {
            ui_set_status(0, 1, "ISO Extraction Failed",
                          rc != 0 && last_err[0] ? last_err : "Select a retail Halo 3 Xbox 360 ISO with default.xex and maps/mainmenu.map.");
            goto finish;
        }
    } else if (!has_base) {
        ui_set_status(0, 1, "Invalid Halo 3 Disc",
                      "Select an extracted disc folder with default.xex and maps/mainmenu.map.");
        goto finish;
    }

    const char *external = need_extract ? "" : external_disc_path(disc);
    int disc_changed = strcmp(external, g_cfg.saved_disc_path) != 0;
    char xex[PATH_CAP], active_pe[PATH_CAP];
    if (!join_path(xex, PATH_CAP, disc, "default.xex") ||
        !join_path(active_pe, PATH_CAP, g_cfg.data_dir, "default.pe"))
        goto staging_failed;
    const char *pe = active_pe;
    int need_pe = need_extract || disc_changed || !valid_pe(pe) || file_is_newer(xex, pe);
    if (need_pe) {
        char xex2pe[PATH_CAP];
        SetupFile *output = &files[count++];
        if (!join_path(xex2pe, PATH_CAP, g_cfg.bin_dir, "xex2pe") ||
            !prepare_setup_file(output, g_cfg.data_dir, "default.pe"))
            goto staging_failed;
        pe = output->tmp;
        const char *label = "Unpacking default.xex";
        int unpack_start = need_extract ? 56 : (did_odst ? 38 : 2);
        ui_set_status(unpack_start, 0, label, "Preparing the retail executable...");
        const char *argv[] = { xex2pe, xex, pe, NULL };
        last_err[0] = 0;
        rc = run_tool_with_progress(argv, log_file, unpack_start, unpack_start + 2, label, last_err, 512);
        if (setup_cancelled()) goto cancelled;
        if (rc != 0 || !valid_pe(pe)) {
            ui_set_status(0, 1, "XEX Unpack Failed",
                          rc != 0 && last_err[0] ? last_err : "Failed to unpack a valid retail executable. See setup.log for details.");
            goto finish;
        }
    }

    if (!g_cfg.custom_shader_pack && (need_pe || did_odst || g_cfg.rebuild_shaders || !managed_pack_current())) {
        char pack_dir[PATH_CAP], maps[PATH_CAP], shaderpack[PATH_CAP], jobs[32], abi[8];
        parent_dir(g_cfg.shader_pack, pack_dir, PATH_CAP);
        ensure_dir_recursive(pack_dir);
        SetupFile *output = &files[count++];
        if (!join_path(maps, PATH_CAP, disc, "maps") ||
            !join_path(shaderpack, PATH_CAP, g_cfg.bin_dir, "shaderpack") ||
            !prepare_setup_file(output, pack_dir, "halo3_shaders.pack") ||
            snprintf(jobs, sizeof(jobs), "%d", g_cfg.jobs) >= (int)sizeof(jobs) ||
            snprintf(abi, sizeof(abi), "%d", XE_ABI_VERSION) >= (int)sizeof(abi))
            goto staging_failed;
        if (snprintf(pack_manifest, PATH_CAP, "%s.tsv", output->tmp) >= PATH_CAP) {
            pack_manifest[0] = 0;
            goto staging_failed;
        }
        if (!remove_setup_file(pack_manifest)) goto staging_failed;
        const char *argv[17] = { shaderpack, "--xenos", "--xenos-abi", abi, "--all", "--progress",
                                 "--maps-dir", maps, "--executable", pe, "--out", output->tmp, "-j", jobs };
        if (dir_exists(dlc_root)) {
            argv[14] = "--dlc-dir";
            argv[15] = dlc_root;
        }
        const char *label = "Compiling Vulkan Shaders";
        int shader_start = need_extract ? 58 : (did_odst ? 40 : 5);
        ui_set_status(shader_start, 0, label, "Reading Halo 3 and DLC map files...");
        last_err[0] = 0;
        rc = run_tool_with_progress(argv, log_file, shader_start, 99, label, last_err, 512);
        if (setup_cancelled()) goto cancelled;
        if (rc != 0 || shader_pack_abi(output->tmp) != XE_ABI_VERSION) {
            ui_set_status(0, 1, "Shader Compilation Failed",
                          rc != 0 && last_err[0] ? last_err : "The helper did not produce a valid ABI 5 shader pack. See setup.log for details.");
            goto finish;
        }
        if (g_cfg.shader_abi[0]) {
            SetupFile *stamp = &files[count++];
            if (!prepare_setup_file(stamp, pack_dir, "abi.txt") ||
                !write_small_text_file(stamp->tmp, g_cfg.shader_abi))
                goto staging_failed;
        }
        SetupFile *dlc = &files[count++];
        if (!prepare_setup_file(dlc, pack_dir, "dlc.txt") ||
            !write_small_text_file(dlc->tmp, check_odst_dlc_installed() ? "1" : "0"))
            goto staging_failed;
    }

    if (disc_changed) {
        SetupFile *prefs = &files[count++];
        if (!prepare_setup_file(prefs, g_cfg.data_dir, "launcher.ini") ||
            !write_launcher_prefs_stream(fopen(prefs->tmp, "wb"), external))
            goto staging_failed;
    }
    if (!publish_setup(files, count, cached_disc, staged_disc, disc_backup)) goto staging_failed;
    if (pack_manifest[0]) {
        char manifest[PATH_CAP];
        if (snprintf(manifest, PATH_CAP, "%s.tsv", g_cfg.shader_pack) < PATH_CAP)
            move_setup_path(pack_manifest, manifest);
    }
    if (need_extract) {
        strcpy(g_cfg.disc_path, cached_disc);
        g_cfg.iso_path[0] = 0;
        g_cfg.reextract = 0;
    }
    if (disc_changed) strcpy(g_cfg.saved_disc_path, external);
    g_cfg.rebuild_shaders = 0;
    ui_set_status(0, 0, "Ready to Play",
                  did_odst ? "ODST Disc 2 DLC and Vulkan shaders ready." : "Retail disc and Vulkan shaders ready.");
    ok = 1;
    goto finish;

staging_failed:
    if (!setup_cancelled()) {
        ui_set_status(0, 1, "Setup Could Not Save Files",
                      "Check available space, data-folder permissions, and path length, then try again.");
        goto finish;
    }
cancelled:
    ui_set_status(0, 0, "Setup cancelled.", "Select your Halo 3 ISO or disc folder to try again.");

finish:
    g_ui.odst_only_job = 0;
    for (size_t i = 0; i < count; ++i) {
        if (files[i].tmp[0]) remove_setup_file(files[i].tmp);
    }
    if (pack_manifest[0]) remove_setup_file(pack_manifest);
    if (staged_disc[0]) remove_disc_tree(staged_disc, 32);
    if (!ok && !g_ui.busy) fprintf(stderr, "halo3: %s %s\n", g_ui.stage_text, g_ui.detail_text);
    if (log_file) fclose(log_file);
    if (lock >= 0) close(lock);
    pthread_mutex_lock(&g_ui.lock);
    g_ui.done_flag = 1;
    g_ui.done_ok = ok;
    pthread_mutex_unlock(&g_ui.lock);
    ui_wake();
    return (void *)(intptr_t)(ok ? 0 : 1);
}

// ---------------------------------------------------------------------------
// Compact 5x7 ASCII bitmap font (fallback if FreeType or bundled fonts unavailable)
// ---------------------------------------------------------------------------
static const uint8_t FONT5X7[96][5] = {
    {0x00,0x00,0x00,0x00,0x00}, {0x00,0x00,0x5F,0x00,0x00}, {0x00,0x07,0x00,0x07,0x00}, {0x14,0x7F,0x14,0x7F,0x14},
    {0x24,0x2A,0x7F,0x2A,0x12}, {0x23,0x13,0x08,0x64,0x62}, {0x36,0x49,0x55,0x22,0x50}, {0x00,0x05,0x03,0x00,0x00},
    {0x00,0x1C,0x22,0x41,0x00}, {0x00,0x41,0x22,0x1C,0x00}, {0x14,0x08,0x3E,0x08,0x14}, {0x08,0x08,0x3E,0x08,0x08},
    {0x00,0x50,0x30,0x00,0x00}, {0x08,0x08,0x08,0x08,0x08}, {0x00,0x60,0x60,0x00,0x00}, {0x20,0x10,0x08,0x04,0x02},
    {0x3E,0x51,0x49,0x45,0x3E}, {0x00,0x42,0x7F,0x40,0x00}, {0x42,0x61,0x51,0x49,0x46}, {0x21,0x41,0x45,0x4B,0x31},
    {0x18,0x14,0x12,0x7F,0x10}, {0x27,0x45,0x45,0x45,0x39}, {0x3C,0x4A,0x49,0x49,0x30}, {0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36}, {0x06,0x49,0x49,0x29,0x1E}, {0x00,0x36,0x36,0x00,0x00}, {0x00,0x56,0x36,0x00,0x00},
    {0x08,0x14,0x22,0x41,0x00}, {0x14,0x14,0x14,0x14,0x14}, {0x00,0x41,0x22,0x14,0x08}, {0x02,0x01,0x51,0x09,0x06},
    {0x32,0x49,0x79,0x41,0x3E}, {0x7E,0x11,0x11,0x11,0x7E}, {0x7F,0x49,0x49,0x49,0x36}, {0x3E,0x41,0x41,0x41,0x22},
    {0x7F,0x41,0x41,0x22,0x1C}, {0x7F,0x49,0x49,0x49,0x41}, {0x7F,0x09,0x09,0x09,0x01}, {0x3E,0x41,0x49,0x49,0x7A},
    {0x7F,0x08,0x08,0x08,0x7F}, {0x00,0x41,0x7F,0x41,0x00}, {0x20,0x40,0x41,0x3F,0x01}, {0x7F,0x08,0x14,0x22,0x41},
    {0x7F,0x40,0x40,0x40,0x40}, {0x7F,0x02,0x0C,0x02,0x7F}, {0x7F,0x04,0x08,0x10,0x7F}, {0x3E,0x41,0x41,0x41,0x3E},
    {0x7F,0x09,0x09,0x09,0x06}, {0x3E,0x41,0x51,0x21,0x5E}, {0x7F,0x09,0x19,0x29,0x46}, {0x46,0x49,0x49,0x49,0x31},
    {0x01,0x01,0x7F,0x01,0x01}, {0x3F,0x40,0x40,0x40,0x3F}, {0x1F,0x20,0x40,0x20,0x1F}, {0x3F,0x40,0x38,0x40,0x3F},
    {0x63,0x14,0x08,0x14,0x63}, {0x07,0x08,0x70,0x08,0x07}, {0x61,0x51,0x49,0x45,0x43}, {0x00,0x7F,0x41,0x41,0x00},
    {0x02,0x04,0x08,0x10,0x20}, {0x00,0x41,0x41,0x7F,0x00}, {0x04,0x02,0x01,0x02,0x04}, {0x40,0x40,0x40,0x40,0x40},
    {0x00,0x01,0x02,0x04,0x00}, {0x20,0x54,0x54,0x54,0x78}, {0x7F,0x48,0x44,0x44,0x38}, {0x38,0x44,0x44,0x44,0x20},
    {0x38,0x44,0x44,0x48,0x7F}, {0x38,0x54,0x54,0x54,0x18}, {0x08,0x7E,0x09,0x01,0x02}, {0x0C,0x52,0x52,0x52,0x3E},
    {0x7F,0x08,0x04,0x04,0x78}, {0x00,0x44,0x7D,0x40,0x00}, {0x20,0x40,0x44,0x3D,0x00}, {0x7F,0x10,0x28,0x44,0x00},
    {0x00,0x41,0x7F,0x40,0x00}, {0x7C,0x04,0x18,0x04,0x78}, {0x7C,0x08,0x04,0x04,0x78}, {0x38,0x44,0x44,0x44,0x38},
    {0x7C,0x14,0x14,0x14,0x08}, {0x08,0x14,0x14,0x18,0x7C}, {0x7C,0x08,0x04,0x04,0x08}, {0x48,0x54,0x54,0x54,0x20},
    {0x04,0x3F,0x44,0x40,0x20}, {0x3C,0x40,0x40,0x20,0x7C}, {0x1C,0x20,0x40,0x20,0x1C}, {0x3C,0x40,0x30,0x40,0x3C},
    {0x44,0x28,0x10,0x28,0x44}, {0x0C,0x50,0x50,0x50,0x3C}, {0x44,0x64,0x54,0x4C,0x44}, {0x00,0x08,0x36,0x41,0x00},
    {0x00,0x00,0x7F,0x00,0x00}, {0x00,0x41,0x36,0x08,0x00}, {0x08,0x04,0x08,0x10,0x08}, {0x00,0x00,0x00,0x00,0x00},
};

// ---------------------------------------------------------------------------
// Dynamic FreeType loader (optional - falls back to FONT5X7)
// ---------------------------------------------------------------------------
typedef struct {
    void *so;
    FT_Error (*Init_FreeType)(FT_Library *);
    FT_Error (*Done_FreeType)(FT_Library);
    FT_Error (*New_Face)(FT_Library, const char *, FT_Long, FT_Face *);
    FT_Error (*Done_Face)(FT_Face);
    FT_Error (*Set_Pixel_Sizes)(FT_Face, FT_UInt, FT_UInt);
    FT_Error (*Load_Char)(FT_Face, FT_ULong, FT_Int32);
    FT_Library lib;
    FT_Face    face;
    int        ok;
} FtApi;

static FtApi g_ft;

static void ft_init(void) {
    char font_path[PATH_CAP];
    g_ft.so = dlopen("libfreetype.so.6", RTLD_LAZY | RTLD_LOCAL);
    if (!g_ft.so) return;
    g_ft.Init_FreeType   = dlsym(g_ft.so, "FT_Init_FreeType");
    g_ft.Done_FreeType   = dlsym(g_ft.so, "FT_Done_FreeType");
    g_ft.New_Face        = dlsym(g_ft.so, "FT_New_Face");
    g_ft.Done_Face       = dlsym(g_ft.so, "FT_Done_Face");
    g_ft.Set_Pixel_Sizes = dlsym(g_ft.so, "FT_Set_Pixel_Sizes");
    g_ft.Load_Char       = dlsym(g_ft.so, "FT_Load_Char");
    if (!g_ft.Init_FreeType || !g_ft.New_Face || !g_ft.Set_Pixel_Sizes || !g_ft.Load_Char) return;
    if (g_ft.Init_FreeType(&g_ft.lib) != 0) return;
    const char *fonts[] = { "fonts/halo3.otf", "fonts/D-DIN.ttf" };
    for (size_t i = 0; i < sizeof(fonts) / sizeof(fonts[0]); i++) {
        join_path(font_path, sizeof(font_path), g_cfg.bin_dir, fonts[i]);
        if (g_ft.New_Face(g_ft.lib, font_path, 0, &g_ft.face) == 0) {
            g_ft.ok = 1;
            break;
        }
    }
}

static void ft_shutdown(void) {
    if (g_ft.face && g_ft.Done_Face) g_ft.Done_Face(g_ft.face);
    if (g_ft.lib && g_ft.Done_FreeType) g_ft.Done_FreeType(g_ft.lib);
    if (g_ft.so) dlclose(g_ft.so);
    memset(&g_ft, 0, sizeof(g_ft));
}

// ---------------------------------------------------------------------------
// Dynamic X11 loader
// ---------------------------------------------------------------------------
typedef struct {
    void *so;
    Display *(*XOpenDisplay)(const char *);
    int      (*XCloseDisplay)(Display *);
    Window   (*XCreateSimpleWindow)(Display *, Window, int, int, unsigned int, unsigned int, unsigned int, unsigned long, unsigned long);
    int      (*XDestroyWindow)(Display *, Window);
    int      (*XMapWindow)(Display *, Window);
    int      (*XSelectInput)(Display *, Window, long);
    Atom     (*XInternAtom)(Display *, const char *, Bool);
    Status   (*XSetWMProtocols)(Display *, Window, Atom *, int);
    int      (*XChangeProperty)(Display *, Window, Atom, Atom, int, int, const unsigned char *, int);
    int      (*XStoreName)(Display *, Window, const char *);
    void     (*XSetWMNormalHints)(Display *, Window, XSizeHints *);
    int      (*XSetClassHint)(Display *, Window, XClassHint *);
    XSizeHints *(*XAllocSizeHints)(void);
    XClassHint *(*XAllocClassHint)(void);
    int      (*XFree)(void *);
    GC       (*XCreateGC)(Display *, Drawable, unsigned long, XGCValues *);
    int      (*XFreeGC)(Display *, GC);
    XImage  *(*XCreateImage)(Display *, Visual *, unsigned int, int, int, char *, unsigned int, unsigned int, int, int);
    int      (*XPutImage)(Display *, Drawable, GC, XImage *, int, int, int, int, unsigned int, unsigned int);
    int      (*XPending)(Display *);
    int      (*XNextEvent)(Display *, XEvent *);
    int      (*XFlush)(Display *);
    int      (*XMoveWindow)(Display *, Window, int, int);
    int      (*XConvertSelection)(Display *, Atom, Atom, Atom, Window, Time);
    int      (*XGetWindowProperty)(Display *, Window, Atom, long, long, Bool, Atom, Atom *, int *, unsigned long *, unsigned long *, unsigned char **);
    Status   (*XSendEvent)(Display *, Window, Bool, long, XEvent *);
    int      (*XLookupString)(XKeyEvent *, char *, int, KeySym *, XComposeStatus *);
    Bool     (*XQueryPointer)(Display *, Window, Window *, Window *, int *, int *, int *, int *, unsigned int *);
} X11Api;

static X11Api g_x11;

static int x11_load(void) {
    if (g_x11.so) return 1;
    g_x11.so = dlopen("libX11.so.6", RTLD_LAZY | RTLD_LOCAL);
    if (!g_x11.so) return 0;
#define LOAD_X(sym) do { g_x11.sym = dlsym(g_x11.so, #sym); if (!g_x11.sym) return 0; } while (0)
    LOAD_X(XOpenDisplay);
    LOAD_X(XCloseDisplay);
    LOAD_X(XCreateSimpleWindow);
    LOAD_X(XDestroyWindow);
    LOAD_X(XMapWindow);
    LOAD_X(XSelectInput);
    LOAD_X(XInternAtom);
    LOAD_X(XSetWMProtocols);
    LOAD_X(XChangeProperty);
    LOAD_X(XStoreName);
    LOAD_X(XSetWMNormalHints);
    LOAD_X(XSetClassHint);
    LOAD_X(XAllocSizeHints);
    LOAD_X(XAllocClassHint);
    LOAD_X(XFree);
    LOAD_X(XCreateGC);
    LOAD_X(XFreeGC);
    LOAD_X(XCreateImage);
    LOAD_X(XPutImage);
    LOAD_X(XPending);
    LOAD_X(XNextEvent);
    LOAD_X(XFlush);
    LOAD_X(XMoveWindow);
    LOAD_X(XConvertSelection);
    LOAD_X(XGetWindowProperty);
    LOAD_X(XSendEvent);
    LOAD_X(XLookupString);
    LOAD_X(XQueryPointer);
#undef LOAD_X
    return 1;
}

static int x11_shift_held(void) {
    if (!getenv("DISPLAY") || !getenv("DISPLAY")[0]) return 0;
    if (!x11_load()) return 0;
    Display *dpy = g_x11.XOpenDisplay(NULL);
    if (!dpy) return 0;
    Window rr = 0, cr = 0;
    int rx = 0, ry = 0, wx = 0, wy = 0;
    unsigned int mask = 0;
    Bool ok = g_x11.XQueryPointer(dpy, DefaultRootWindow(dpy), &rr, &cr, &rx, &ry, &wx, &wy, &mask);
    g_x11.XCloseDisplay(dpy);
    return ok && ((mask & ShiftMask) != 0);
}

// ---------------------------------------------------------------------------
// Software 32-bit framebuffer renderer (anti-aliased Halo 3 UI)
// ---------------------------------------------------------------------------
typedef struct {
    int       w, h;
    uint32_t *px;
} Surface;

static inline uint32_t blend_pixel(uint32_t dst, uint32_t src, uint8_t a) {
    if (a == 0) return dst;
    if (a == 255) return src;
    uint32_t ia = 255u - a;
    uint32_t r = (((src >> 16) & 255u) * a + ((dst >> 16) & 255u) * ia + 127u) / 255u;
    uint32_t g = (((src >> 8)  & 255u) * a + ((dst >> 8)  & 255u) * ia + 127u) / 255u;
    uint32_t b = (( src        & 255u) * a + ( dst        & 255u) * ia + 127u) / 255u;
    return (r << 16) | (g << 8) | b;
}

static void surf_fill(Surface *s, int x, int y, int w, int h, uint32_t col) {
    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = x + w > s->w ? s->w : x + w;
    int y1 = y + h > s->h ? s->h : y + h;
    for (int j = y0; j < y1; ++j) {
        uint32_t *row = s->px + j * s->w;
        for (int i = x0; i < x1; ++i) row[i] = col;
    }
}

static void surf_round_rect(Surface *s, int x, int y, int w, int h, int rad, uint32_t fill, uint32_t border) {
    if (w <= 0 || h <= 0) return;
    if (rad * 2 > w) rad = w / 2;
    if (rad * 2 > h) rad = h / 2;
    int x0 = x < 0 ? 0 : x;
    int y0 = y < 0 ? 0 : y;
    int x1 = x + w > s->w ? s->w : x + w;
    int y1 = y + h > s->h ? s->h : y + h;
    float cx0 = (float)(x + rad);
    float cy0 = (float)(y + rad);
    float cx1 = (float)(x + w - 1 - rad);
    float cy1 = (float)(y + h - 1 - rad);
    float r = (float)rad;

    for (int py = y0; py < y1; ++py) {
        uint32_t *row = s->px + py * s->w;
        float fy = (float)py;
        float dy = 0.0f;
        if (fy < cy0) dy = cy0 - fy;
        else if (fy > cy1) dy = fy - cy1;
        for (int px = x0; px < x1; ++px) {
            float fx = (float)px;
            float dx = 0.0f;
            if (fx < cx0) dx = cx0 - fx;
            else if (fx > cx1) dx = fx - cx1;
            float dist;
            if (dx > 0.0f && dy > 0.0f) {
                dist = sqrtf(dx * dx + dy * dy) - r;
            } else {
                float ex = (fx < cx0) ? (cx0 - fx - r) : (fx > cx1 ? (fx - cx1 - r) : -r);
                float ey = (fy < cy0) ? (cy0 - fy - r) : (fy > cy1 ? (fy - cy1 - r) : -r);
                dist = ex > ey ? ex : ey;
            }
            if (dist > 0.7f) continue;
            uint32_t c = (dist > -1.2f) ? border : fill;
            if (dist > -0.5f) {
                float alpha = 1.0f - (dist + 0.5f) / 1.2f;
                if (alpha < 0.0f) alpha = 0.0f;
                if (alpha > 1.0f) alpha = 1.0f;
                row[px] = blend_pixel(row[px], c, (uint8_t)(alpha * 255.0f));
            } else {
                row[px] = c;
            }
        }
    }
}

#include "halo3_icon_48.h"

static void surf_draw_halo_badge(Surface *s, int x, int y) {
    for (int iy = 0; iy < HALO3_ICON_H; ++iy) {
        int py = y + iy;
        if (py < 0 || py >= s->h) continue;
        uint32_t *row = s->px + py * s->w;
        const uint32_t *src = HALO3_ICON_48 + iy * HALO3_ICON_W;
        for (int ix = 0; ix < HALO3_ICON_W; ++ix) {
            int px = x + ix;
            if (px < 0 || px >= s->w) continue;
            uint32_t argb = src[ix];
            uint8_t a = (uint8_t)(argb >> 24);
            if (a == 0) continue;
            row[px] = (a == 255) ? (argb & 0xFFFFFFu) : blend_pixel(row[px], argb & 0xFFFFFFu, a);
        }
    }
}

static uint32_t utf8_next(const char **p) {
    const unsigned char *s = (const unsigned char *)*p;
    if (!*s) return 0;
    if (s[0] < 0x80) { *p += 1; return s[0]; }
    if ((s[0] & 0xE0) == 0xC0 && s[1]) {
        uint32_t cp = ((uint32_t)(s[0] & 0x1F) << 6) | (s[1] & 0x3F);
        *p += 2; return cp;
    }
    if ((s[0] & 0xF0) == 0xE0 && s[1] && s[2]) {
        uint32_t cp = ((uint32_t)(s[0] & 0x0F) << 12) | ((uint32_t)(s[1] & 0x3F) << 6) | (s[2] & 0x3F);
        *p += 3; return cp;
    }
    if ((s[0] & 0xF8) == 0xF0 && s[1] && s[2] && s[3]) {
        uint32_t cp = ((uint32_t)(s[0] & 0x07) << 18) | ((uint32_t)(s[1] & 0x3F) << 12) |
                      ((uint32_t)(s[2] & 0x3F) << 6) | (s[3] & 0x3F);
        *p += 4; return cp;
    }
    *p += 1;
    return '?';
}

static int text_width_px(const char *txt, int px_size) {
    if (g_ft.ok) {
        g_ft.Set_Pixel_Sizes(g_ft.face, 0, (FT_UInt)px_size);
        int w = 0;
        const char *p = txt;
        while (*p) {
            uint32_t cp = utf8_next(&p);
            if (g_ft.Load_Char(g_ft.face, cp, FT_LOAD_DEFAULT) == 0) {
                w += (int)(g_ft.face->glyph->advance.x >> 6);
            }
        }
        return w;
    }
    int sc = px_size >= 22 ? 3 : (px_size >= 14 ? 2 : 1);
    int n = 0;
    const char *p = txt;
    while (*p) { utf8_next(&p); n++; }
    return n * 6 * sc;
}

static void surf_draw_text(Surface *s, int x, int y_top, int px_size, uint32_t col, const char *txt, int max_w) {
    if (g_ft.ok) {
        g_ft.Set_Pixel_Sizes(g_ft.face, 0, (FT_UInt)px_size);
        int pen_x = x;
        int baseline = y_top + (px_size * 4) / 5;
        const char *p = txt;
        while (*p) {
            uint32_t cp = utf8_next(&p);
            if (g_ft.Load_Char(g_ft.face, cp, FT_LOAD_RENDER) != 0) continue;
            FT_GlyphSlot g = g_ft.face->glyph;
            int adv = (int)(g->advance.x >> 6);
            if (max_w > 0 && (pen_x - x) + adv > max_w) break;
            int gx = pen_x + g->bitmap_left;
            int gy = baseline - g->bitmap_top;
            for (unsigned int row = 0; row < g->bitmap.rows; ++row) {
                int dy = gy + (int)row;
                if (dy < 0 || dy >= s->h) continue;
                uint32_t *dst = s->px + dy * s->w;
                const unsigned char *src = g->bitmap.buffer + row * (unsigned int)abs(g->bitmap.pitch);
                for (unsigned int col_i = 0; col_i < g->bitmap.width; ++col_i) {
                    int dx = gx + (int)col_i;
                    if (dx < 0 || dx >= s->w) continue;
                    if (src[col_i]) dst[dx] = blend_pixel(dst[dx], col, src[col_i]);
                }
            }
            pen_x += adv;
        }
        return;
    }
    int sc = px_size >= 22 ? 3 : (px_size >= 14 ? 2 : 1);
    int pen_x = x;
    const char *p = txt;
    while (*p) {
        uint32_t cp = utf8_next(&p);
        if (cp == 0x2014 || cp == 0x2013) cp = '-';
        else if (cp == 0x2022) cp = '*';
        else if (cp < 32 || cp > 126) cp = ' ';
        if (max_w > 0 && (pen_x - x) + 6 * sc > max_w) break;
        const uint8_t *gl = FONT5X7[cp - 32];
        for (int cx = 0; cx < 5; ++cx) {
            for (int cy = 0; cy < 7; ++cy) {
                if ((gl[cx] >> cy) & 1) {
                    surf_fill(s, pen_x + cx * sc, y_top + cy * sc + 2, sc, sc, col);
                }
            }
        }
        pen_x += 6 * sc;
    }
}

static void surf_draw_wrapped(Surface *s, int x, int y, int max_w, int max_lines, int px_size, uint32_t col, const char *txt) {
    char buf[1024];
    snprintf(buf, sizeof(buf), "%s", txt);
    int line_h = px_size + 5;
    int line_no = 0;
    char *p = buf;
    while (*p && line_no < max_lines) {
        // Explicit newline handling
        char *nl = strchr(p, '\n');
        if (nl) *nl = 0;

        char *cur = p;
        while (*cur && line_no < max_lines) {
            char line[512] = {0};
            char *last_space = NULL;
            size_t len = 0;
            while (cur[len]) {
                if (len + 1 >= sizeof(line)) break;
                line[len] = cur[len];
                line[len + 1] = 0;
                if (cur[len] == ' ') last_space = cur + len;
                if (text_width_px(line, px_size) > max_w && len > 0) {
                    if (last_space && last_space > cur) {
                        len = (size_t)(last_space - cur);
                    }
                    break;
                }
                len++;
            }
            line[len] = 0;
            surf_draw_text(s, x, y + line_no * line_h, px_size, col, line, max_w);
            line_no++;
            cur += len;
            while (*cur == ' ') cur++;
        }
        if (!nl) break;
        p = nl + 1;
    }
}

// ---------------------------------------------------------------------------
// Native File/Folder Picker: XDG Desktop Portal (D-Bus) -> kdialog -> zenity
// ---------------------------------------------------------------------------
typedef struct {
    void *so;
    void (*dbus_error_init)(DBusError *);
    void (*dbus_error_free)(DBusError *);
    dbus_bool_t (*dbus_error_is_set)(const DBusError *);
    DBusConnection *(*dbus_bus_get_private)(DBusBusType, DBusError *);
    const char *(*dbus_bus_get_unique_name)(DBusConnection *);
    void (*dbus_bus_add_match)(DBusConnection *, const char *, DBusError *);
    void (*dbus_connection_close)(DBusConnection *);
    void (*dbus_connection_unref)(DBusConnection *);
    void (*dbus_connection_flush)(DBusConnection *);
    dbus_bool_t (*dbus_connection_read_write)(DBusConnection *, int);
    DBusMessage *(*dbus_connection_pop_message)(DBusConnection *);
    DBusMessage *(*dbus_connection_send_with_reply_and_block)(DBusConnection *, DBusMessage *, int, DBusError *);
    DBusMessage *(*dbus_message_new_method_call)(const char *, const char *, const char *, const char *);
    void (*dbus_message_unref)(DBusMessage *);
    dbus_bool_t (*dbus_message_is_signal)(DBusMessage *, const char *, const char *);
    const char *(*dbus_message_get_path)(DBusMessage *);
    void (*dbus_message_iter_init_append)(DBusMessage *, DBusMessageIter *);
    dbus_bool_t (*dbus_message_iter_append_basic)(DBusMessageIter *, int, const void *);
    dbus_bool_t (*dbus_message_iter_open_container)(DBusMessageIter *, int, const char *, DBusMessageIter *);
    dbus_bool_t (*dbus_message_iter_close_container)(DBusMessageIter *, DBusMessageIter *);
    dbus_bool_t (*dbus_message_iter_init)(DBusMessage *, DBusMessageIter *);
    int (*dbus_message_iter_get_arg_type)(DBusMessageIter *);
    void (*dbus_message_iter_get_basic)(DBusMessageIter *, void *);
    void (*dbus_message_iter_recurse)(DBusMessageIter *, DBusMessageIter *);
    dbus_bool_t (*dbus_message_iter_next)(DBusMessageIter *);
} DbusApi;

static DbusApi g_dbus;
static int g_portal_seq = 0;

static int dbus_load(void) {
    if (g_dbus.so) return 1;
    g_dbus.so = dlopen("libdbus-1.so.3", RTLD_LAZY | RTLD_LOCAL);
    if (!g_dbus.so) return 0;
#define LOAD_D(sym) do { g_dbus.sym = dlsym(g_dbus.so, #sym); if (!g_dbus.sym) return 0; } while (0)
    LOAD_D(dbus_error_init);
    LOAD_D(dbus_error_free);
    LOAD_D(dbus_error_is_set);
    LOAD_D(dbus_bus_get_private);
    LOAD_D(dbus_bus_get_unique_name);
    LOAD_D(dbus_bus_add_match);
    LOAD_D(dbus_connection_close);
    LOAD_D(dbus_connection_unref);
    LOAD_D(dbus_connection_flush);
    LOAD_D(dbus_connection_read_write);
    LOAD_D(dbus_connection_pop_message);
    LOAD_D(dbus_connection_send_with_reply_and_block);
    LOAD_D(dbus_message_new_method_call);
    LOAD_D(dbus_message_unref);
    LOAD_D(dbus_message_is_signal);
    LOAD_D(dbus_message_get_path);
    LOAD_D(dbus_message_iter_init_append);
    LOAD_D(dbus_message_iter_append_basic);
    LOAD_D(dbus_message_iter_open_container);
    LOAD_D(dbus_message_iter_close_container);
    LOAD_D(dbus_message_iter_init);
    LOAD_D(dbus_message_iter_get_arg_type);
    LOAD_D(dbus_message_iter_get_basic);
    LOAD_D(dbus_message_iter_recurse);
    LOAD_D(dbus_message_iter_next);
#undef LOAD_D
    return 1;
}

static void dbus_append_dict_str(DBusMessageIter *dict, const char *key, const char *val) {
    DBusMessageIter entry, var;
    g_dbus.dbus_message_iter_open_container(dict, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
    g_dbus.dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
    g_dbus.dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "s", &var);
    g_dbus.dbus_message_iter_append_basic(&var, DBUS_TYPE_STRING, &val);
    g_dbus.dbus_message_iter_close_container(&entry, &var);
    g_dbus.dbus_message_iter_close_container(dict, &entry);
}

static void dbus_append_dict_bool(DBusMessageIter *dict, const char *key, dbus_bool_t val) {
    DBusMessageIter entry, var;
    g_dbus.dbus_message_iter_open_container(dict, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
    g_dbus.dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
    g_dbus.dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "b", &var);
    g_dbus.dbus_message_iter_append_basic(&var, DBUS_TYPE_BOOLEAN, &val);
    g_dbus.dbus_message_iter_close_container(&entry, &var);
    g_dbus.dbus_message_iter_close_container(dict, &entry);
}

static void dbus_append_iso_filters(DBusMessageIter *dict) {
    DBusMessageIter entry, var, filters, f1, pats1, p1, p2, f2, pats2, p3;
    const char *key = "filters";
    g_dbus.dbus_message_iter_open_container(dict, DBUS_TYPE_DICT_ENTRY, NULL, &entry);
    g_dbus.dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);
    g_dbus.dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, "a(sa(us))", &var);
    g_dbus.dbus_message_iter_open_container(&var, DBUS_TYPE_ARRAY, "(sa(us))", &filters);

    // Filter 1: Xbox 360 ISO Images (*.iso, *.ISO)
    const char *label1 = "Xbox 360 ISO Images (*.iso)";
    dbus_uint32_t glob_type = 0;
    const char *pat_lo = "*.iso", *pat_hi = "*.ISO";
    g_dbus.dbus_message_iter_open_container(&filters, DBUS_TYPE_STRUCT, NULL, &f1);
    g_dbus.dbus_message_iter_append_basic(&f1, DBUS_TYPE_STRING, &label1);
    g_dbus.dbus_message_iter_open_container(&f1, DBUS_TYPE_ARRAY, "(us)", &pats1);
    g_dbus.dbus_message_iter_open_container(&pats1, DBUS_TYPE_STRUCT, NULL, &p1);
    g_dbus.dbus_message_iter_append_basic(&p1, DBUS_TYPE_UINT32, &glob_type);
    g_dbus.dbus_message_iter_append_basic(&p1, DBUS_TYPE_STRING, &pat_lo);
    g_dbus.dbus_message_iter_close_container(&pats1, &p1);
    g_dbus.dbus_message_iter_open_container(&pats1, DBUS_TYPE_STRUCT, NULL, &p2);
    g_dbus.dbus_message_iter_append_basic(&p2, DBUS_TYPE_UINT32, &glob_type);
    g_dbus.dbus_message_iter_append_basic(&p2, DBUS_TYPE_STRING, &pat_hi);
    g_dbus.dbus_message_iter_close_container(&pats1, &p2);
    g_dbus.dbus_message_iter_close_container(&f1, &pats1);
    g_dbus.dbus_message_iter_close_container(&filters, &f1);

    // Filter 2: All Files (*)
    const char *label2 = "All Files";
    const char *pat_all = "*";
    g_dbus.dbus_message_iter_open_container(&filters, DBUS_TYPE_STRUCT, NULL, &f2);
    g_dbus.dbus_message_iter_append_basic(&f2, DBUS_TYPE_STRING, &label2);
    g_dbus.dbus_message_iter_open_container(&f2, DBUS_TYPE_ARRAY, "(us)", &pats2);
    g_dbus.dbus_message_iter_open_container(&pats2, DBUS_TYPE_STRUCT, NULL, &p3);
    g_dbus.dbus_message_iter_append_basic(&p3, DBUS_TYPE_UINT32, &glob_type);
    g_dbus.dbus_message_iter_append_basic(&p3, DBUS_TYPE_STRING, &pat_all);
    g_dbus.dbus_message_iter_close_container(&pats2, &p3);
    g_dbus.dbus_message_iter_close_container(&f2, &pats2);
    g_dbus.dbus_message_iter_close_container(&filters, &f2);

    g_dbus.dbus_message_iter_close_container(&var, &filters);
    g_dbus.dbus_message_iter_close_container(&entry, &var);
    g_dbus.dbus_message_iter_close_container(dict, &entry);
}

typedef void (*PumpExposeFn)(void *ctx);

// Returns: 1 = picked a path, 0 = user cancelled, -1 = portal unavailable (try fallback)
static int pick_via_portal(Window xwin, int pick_dir, char *out_path, size_t out_cap,
                           PumpExposeFn pump, void *pump_ctx) {
    if (!dbus_load()) return -1;

    DBusError err;
    g_dbus.dbus_error_init(&err);
    DBusConnection *conn = g_dbus.dbus_bus_get_private(DBUS_BUS_SESSION, &err);
    if (!conn || g_dbus.dbus_error_is_set(&err)) {
        g_dbus.dbus_error_free(&err);
        if (conn) { g_dbus.dbus_connection_close(conn); g_dbus.dbus_connection_unref(conn); }
        return -1;
    }

    const char *uniq = g_dbus.dbus_bus_get_unique_name(conn);
    if (!uniq || !uniq[0]) {
        g_dbus.dbus_connection_close(conn);
        g_dbus.dbus_connection_unref(conn);
        return -1;
    }

    char sender[128];
    snprintf(sender, sizeof(sender), "%s", uniq[0] == ':' ? uniq + 1 : uniq);
    for (char *p = sender; *p; ++p) if (*p == '.') *p = '_';

    char token[64];
    snprintf(token, sizeof(token), "halo3_%d_%d", (int)getpid(), ++g_portal_seq);

    char expected_path[256];
    snprintf(expected_path, sizeof(expected_path),
             "/org/freedesktop/portal/desktop/request/%s/%s", sender, token);

    char rule[512];
    snprintf(rule, sizeof(rule),
             "type='signal',sender='org.freedesktop.portal.Desktop',"
             "interface='org.freedesktop.portal.Request',member='Response',path='%s'",
             expected_path);
    g_dbus.dbus_bus_add_match(conn, rule, &err);
    g_dbus.dbus_connection_flush(conn);
    if (g_dbus.dbus_error_is_set(&err)) {
        g_dbus.dbus_error_free(&err);
    }

    DBusMessage *msg = g_dbus.dbus_message_new_method_call(
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.FileChooser",
        "OpenFile");
    if (!msg) {
        g_dbus.dbus_connection_close(conn);
        g_dbus.dbus_connection_unref(conn);
        return -1;
    }

    char parent_str[64] = "";
    if (xwin) snprintf(parent_str, sizeof(parent_str), "x11:%lx", (unsigned long)xwin);
    const char *parent_ptr = parent_str;
    const char *title = pick_dir
        ? "Select extracted Halo 3 disc folder (containing default.xex)"
        : "Select your retail Halo 3 Xbox 360 .ISO";

    DBusMessageIter iter, dict;
    g_dbus.dbus_message_iter_init_append(msg, &iter);
    g_dbus.dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &parent_ptr);
    g_dbus.dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &title);
    g_dbus.dbus_message_iter_open_container(&iter, DBUS_TYPE_ARRAY, "{sv}", &dict);
    dbus_append_dict_str(&dict, "handle_token", token);
    dbus_append_dict_bool(&dict, "modal", 1);
    if (pick_dir) {
        dbus_append_dict_bool(&dict, "directory", 1);
    } else {
        dbus_append_iso_filters(&dict);
    }
    g_dbus.dbus_message_iter_close_container(&iter, &dict);

    DBusMessage *reply = g_dbus.dbus_connection_send_with_reply_and_block(conn, msg, 5000, &err);
    g_dbus.dbus_message_unref(msg);
    if (!reply || g_dbus.dbus_error_is_set(&err)) {
        g_dbus.dbus_error_free(&err);
        if (reply) g_dbus.dbus_message_unref(reply);
        g_dbus.dbus_connection_close(conn);
        g_dbus.dbus_connection_unref(conn);
        return -1;
    }

    char actual_path[256];
    snprintf(actual_path, sizeof(actual_path), "%s", expected_path);
    DBusMessageIter riter;
    if (g_dbus.dbus_message_iter_init(reply, &riter) &&
        g_dbus.dbus_message_iter_get_arg_type(&riter) == DBUS_TYPE_OBJECT_PATH) {
        const char *rp = NULL;
        g_dbus.dbus_message_iter_get_basic(&riter, &rp);
        if (rp && rp[0]) snprintf(actual_path, sizeof(actual_path), "%s", rp);
    }
    g_dbus.dbus_message_unref(reply);

    int res = 0;
    for (;;) {
        if (pump) pump(pump_ctx);
        if (!g_dbus.dbus_connection_read_write(conn, 50)) {
            res = -1;
            break;
        }
        DBusMessage *sig = NULL;
        int got_response = 0;
        while ((sig = g_dbus.dbus_connection_pop_message(conn)) != NULL) {
            const char *sp = g_dbus.dbus_message_get_path(sig);
            if (g_dbus.dbus_message_is_signal(sig, "org.freedesktop.portal.Request", "Response") &&
                sp && (strcmp(sp, actual_path) == 0 || strcmp(sp, expected_path) == 0)) {
                got_response = 1;
                DBusMessageIter si;
                if (g_dbus.dbus_message_iter_init(sig, &si) &&
                    g_dbus.dbus_message_iter_get_arg_type(&si) == DBUS_TYPE_UINT32) {
                    dbus_uint32_t code = 1;
                    g_dbus.dbus_message_iter_get_basic(&si, &code);
                    if (code == 0 && g_dbus.dbus_message_iter_next(&si) &&
                        g_dbus.dbus_message_iter_get_arg_type(&si) == DBUS_TYPE_ARRAY) {
                        DBusMessageIter arr;
                        g_dbus.dbus_message_iter_recurse(&si, &arr);
                        while (g_dbus.dbus_message_iter_get_arg_type(&arr) == DBUS_TYPE_DICT_ENTRY) {
                            DBusMessageIter de;
                            g_dbus.dbus_message_iter_recurse(&arr, &de);
                            const char *k = NULL;
                            g_dbus.dbus_message_iter_get_basic(&de, &k);
                            if (k && strcmp(k, "uris") == 0 && g_dbus.dbus_message_iter_next(&de) &&
                                g_dbus.dbus_message_iter_get_arg_type(&de) == DBUS_TYPE_VARIANT) {
                                DBusMessageIter v, uris;
                                g_dbus.dbus_message_iter_recurse(&de, &v);
                                if (g_dbus.dbus_message_iter_get_arg_type(&v) == DBUS_TYPE_ARRAY) {
                                    g_dbus.dbus_message_iter_recurse(&v, &uris);
                                    if (g_dbus.dbus_message_iter_get_arg_type(&uris) == DBUS_TYPE_STRING) {
                                        const char *u = NULL;
                                        g_dbus.dbus_message_iter_get_basic(&uris, &u);
                                        if (u && url_decode_file_uri(u, out_path, out_cap)) {
                                            res = 1;
                                        }
                                    }
                                }
                            }
                            g_dbus.dbus_message_iter_next(&arr);
                        }
                    }
                }
                g_dbus.dbus_message_unref(sig);
                break;
            }
            g_dbus.dbus_message_unref(sig);
        }
        if (got_response) break;
    }

    g_dbus.dbus_connection_close(conn);
    g_dbus.dbus_connection_unref(conn);
    return res;
}

static int run_picker_cmd(const char *const argv[], char *out, size_t cap) {
    int pfd[2];
    if (pipe(pfd) != 0) return -1;
    pid_t pid = fork();
    if (pid < 0) { close(pfd[0]); close(pfd[1]); return -1; }
    if (pid == 0) {
        close(pfd[0]);
        dup2(pfd[1], STDOUT_FILENO);
        int devnull = open("/dev/null", O_WRONLY);
        if (devnull >= 0) { dup2(devnull, STDERR_FILENO); close(devnull); }
        close(pfd[1]);
        execvp(argv[0], (char *const *)argv);
        _exit(127);
    }
    close(pfd[1]);
    size_t total = 0;
    ssize_t n;
    while (total + 1 < cap && (n = read(pfd[0], out + total, cap - 1 - total)) > 0) {
        total += (size_t)n;
    }
    out[total] = 0;
    close(pfd[0]);
    int st = 0;
    while (waitpid(pid, &st, 0) < 0 && errno == EINTR) {}
    trim_ws(out);
    if (WIFEXITED(st) && WEXITSTATUS(st) == 127) return -1;
    if (WIFEXITED(st) && WEXITSTATUS(st) == 0 && out[0]) return 1;
    return 0;
}

static int pick_path_native(Window xwin, int pick_dir, char *out, size_t cap,
                            PumpExposeFn pump, void *pump_ctx) {
    out[0] = 0;
    int r = pick_via_portal(xwin, pick_dir, out, cap, pump, pump_ctx);
    if (r >= 0) return r;

    const char *home = getenv("HOME");
    if (!home || !home[0]) home = "/";
    if (pick_dir) {
        const char *kd[] = { "kdialog", "--title", "Select extracted Halo 3 disc folder", "--getexistingdirectory", home, NULL };
        r = run_picker_cmd(kd, out, cap);
        if (r >= 0) return r;
        const char *zn[] = { "zenity", "--file-selection", "--directory", "--title=Select extracted Halo 3 disc folder", NULL };
        return run_picker_cmd(zn, out, cap);
    } else {
        const char *kd[] = { "kdialog", "--title", "Select your retail Halo 3 Xbox 360 ISO", "--getopenfilename", home, "*.iso *.ISO|Xbox 360 ISO", NULL };
        r = run_picker_cmd(kd, out, cap);
        if (r >= 0) return r;
        const char *zn[] = { "zenity", "--file-selection", "--title=Select your retail Halo 3 Xbox 360 ISO",
                             "--file-filter=Xbox 360 ISO (*.iso) | *.iso *.ISO", "--file-filter=All files | *", NULL };
        return run_picker_cmd(zn, out, cap);
    }
}

static void open_folder_in_desktop(const char *path) {
    ensure_dir_recursive(path);
    pid_t pid = fork();
    if (pid == 0) {
        int dn = open("/dev/null", O_RDWR);
        if (dn >= 0) { dup2(dn, 0); dup2(dn, 1); dup2(dn, 2); close(dn); }
        execlp("xdg-open", "xdg-open", path, (char *)NULL);
        _exit(127);
    }
}

// ---------------------------------------------------------------------------
// X11 Setup & Progress GUI
// ---------------------------------------------------------------------------
typedef struct {
    Display  *dpy;
    Window    win;
    GC        gc;
    XImage   *img;
    Surface   surf;
    Atom      wm_delete;
    Atom      xdnd_aware, xdnd_enter, xdnd_position, xdnd_status, xdnd_drop, xdnd_selection, xdnd_finished, atom_uri_list, atom_action_copy;
    Window    xdnd_src;
    UiButton  btns[8];
    int       nbtns;
    int       hover_id;
    int       down_id;
    int       input_focused;
    int       dropdown_open;
    int       dropdown_hover;
    int       chk_hover;
    int       drop_btn_hover;
    char      profile_err[96];
} XGui;

// Interactive widget rectangles inside the main card (card_x=28, card_y=90, card_w=604, card_h=218)
static const int UI_CARD_X = 28, UI_CARD_Y = 90, UI_CARD_W = 604, UI_CARD_H = 218;
static const int UI_INP_X  = 128, UI_INP_Y = 172, UI_INP_W = 244, UI_INP_H = 34;
static const int UI_DRP_X  = 376, UI_DRP_Y = 172, UI_DRP_W = 34,  UI_DRP_H = 34;
static const int UI_CHK_X  = 48,  UI_CHK_Y = 254, UI_CHK_W = 360, UI_CHK_H = 24;

static int pt_in(int mx, int my, int x, int y, int w, int h) {
    return mx >= x && mx < x + w && my >= y && my < y + h;
}

static void surf_draw_chevron_down(Surface *s, int cx, int cy, uint32_t col) {
    for (int row = 0; row < 5; ++row) {
        int span = 4 - row;
        for (int dx = -span; dx <= span; ++dx) {
            int px = cx + dx, py = cy - 2 + row;
            if (px >= 0 && px < s->w && py >= 0 && py < s->h) {
                s->px[py * s->w + px] = col;
            }
        }
    }
}

static void surf_draw_checkmark(Surface *s, int x, int y, uint32_t col) {
    // Crisp 2px-thick checkmark inside an 18x18 box
    static const int pts[][2] = {
        {4,9},{5,10},{6,11},{7,12},{8,11},{9,10},{10,9},{11,8},{12,7},{13,6},{14,5}
    };
    for (size_t i = 0; i < sizeof(pts) / sizeof(pts[0]); ++i) {
        for (int dy = 0; dy <= 1; ++dy) {
            int px = x + pts[i][0], py = y + pts[i][1] + dy;
            if (px >= 0 && px < s->w && py >= 0 && py < s->h) s->px[py * s->w + px] = col;
        }
    }
}

static void xgui_layout_buttons(XGui *g) {
    int pad = 28;
    int bw = 142, bh = 36, gap = 12;
    int y1 = 324, y2 = 370;
    int c0 = pad;
    int c1 = pad + (bw + gap);
    int c2 = pad + (bw + gap) * 2;
    int c3 = pad + (bw + gap) * 3;
    g->nbtns = 0;

    if (g_ui.error_only_mode) {
        g->btns[g->nbtns++] = (UiButton){ BTN_DATA,   c0, y2, bw, bh, 0, 1, 1, "Open Data Folder" };
        g->btns[g->nbtns++] = (UiButton){ BTN_CANCEL, c3, y2, bw, bh, 1, 1, 1, "Close" };
        return;
    }

    int idle = !g_ui.busy;
    if (g_ui.ready_to_play && idle) {
        g->btns[g->nbtns++] = (UiButton){ BTN_PLAY,    c0, y1, bw, bh, 1, 1, 1, "Play Halo 3" };
        g->btns[g->nbtns++] = (UiButton){ BTN_ISO,     c1, y1, bw, bh, 0, 1, 1, "Change .ISO..." };
        g->btns[g->nbtns++] = (UiButton){ BTN_FOLDER,  c2, y1, bw, bh, 0, 1, 1, "Disc Folder..." };
    } else {
        g->btns[g->nbtns++] = (UiButton){ BTN_ISO,     c0, y1, bw, bh, 1, idle, 1, "Select .ISO..." };
        g->btns[g->nbtns++] = (UiButton){ BTN_FOLDER,  c1, y1, bw, bh, 0, idle, 1, "Disc Folder..." };
    }
    if (idle && !g_cfg.custom_shader_pack && !g_cfg.iso_path[0] && is_valid_disc_dir(g_cfg.disc_path))
        g->btns[g->nbtns++] = (UiButton){ BTN_REBUILD, c3, y1, bw, bh, 0, 1, 1, "Rebuild Shaders" };
    g->btns[g->nbtns++] = (UiButton){ BTN_DATA,   c0, y2, bw, bh, 0, 1,    1, "Open Data Folder" };
    g->btns[g->nbtns++] = (UiButton){ BTN_ODST,   c1, y2, bw, bh, 0, idle, 1, "Add ODST Disc 2 DLC" };
    g->btns[g->nbtns++] = (UiButton){ BTN_CANCEL, c3, y2, bw, bh, 0, 1,    1, idle ? "Close" : "Cancel" };
}

static void xgui_render(XGui *g) {
    Surface *s = &g->surf;
    xgui_layout_buttons(g);

    surf_fill(s, 0, 0, s->w, s->h, COL_BG);
    surf_fill(s, 0, 0, s->w, 4, COL_ACCENT);

    int pad = 28;
    // 48x48 icon at y=23..71 (center y=47); title+subtitle block visually centered on y=47
    surf_draw_halo_badge(s, pad, 23);
    surf_draw_text(s, pad + 64, 28, 26, COL_TEXT, "HALO 3", 0);

    char sub[256];
    snprintf(sub, sizeof(sub), "Native 64-bit PC Port - %s%s",
             g_cfg.local_version[0] ? g_cfg.local_version : "Linux",
             g_cfg.is_flatpak ? "  (Flatpak)" : "");
    surf_draw_text(s, pad + 64, 55, 13, COL_TEXT_DIM, sub, s->w - pad * 2 - 64);

    // Main card
    surf_round_rect(s, UI_CARD_X, UI_CARD_Y, UI_CARD_W, UI_CARD_H, 10, COL_CARD, COL_CARD_BORDER);

    char stage[256], detail[1024];
    int pct, err, busy;
    pthread_mutex_lock(&g_ui.lock);
    snprintf(stage, sizeof(stage), "%s", g_ui.stage_text);
    snprintf(detail, sizeof(detail), "%s", g_ui.detail_text);
    pct = g_ui.progress_pct;
    err = g_ui.has_error;
    busy = g_ui.busy;
    pthread_mutex_unlock(&g_ui.lock);

    int cx = UI_CARD_X + 20, cw = UI_CARD_W - 40;
    uint32_t stage_col = err ? COL_ERR : (pct == 100 && !busy ? COL_OK : COL_TEXT);
    int stage_max_w = (g_ui.dlc_loaded && !g_ui.error_only_mode) ? (cw - 96) : cw;
    surf_draw_text(s, cx, UI_CARD_Y + 18, 16, stage_col, stage, stage_max_w);
    if (g_ui.dlc_loaded && !g_ui.error_only_mode) {
        const char *badge = "DLC Loaded";
        int bw = text_width_px(badge, 11) + 20;
        if (bw < 80) bw = 80;
        int bh = 22;
        int bx = cx + cw - bw;
        int by = UI_CARD_Y + 15;
        surf_round_rect(s, bx, by, bw, bh, 11, 0x132E22u, 0x3CC978u);
        int btw = text_width_px(badge, 11);
        surf_draw_text(s, bx + (bw - btw) / 2, by + 7, 11, 0x7CE8A8u, badge, bw - 8);
    }
    surf_draw_wrapped(s, cx, UI_CARD_Y + 46, cw, 2, 13, err ? COL_ERR : COL_TEXT_DIM, detail);

    if (!g_ui.error_only_mode) {
        if (busy || pct > 0) {
            int bx = cx, by = UI_CARD_Y + 116, bw = cw, bh = 22;
            surf_round_rect(s, bx, by, bw, bh, 6, COL_BAR_BG, COL_CARD_BORDER);
            if (pct > 0) {
                int fw = ((bw - 2) * pct) / 100;
                if (fw < 10) fw = 10;
                if (fw > bw - 2) fw = bw - 2;
                uint32_t fc = (pct >= 100) ? COL_OK : COL_ACCENT;
                surf_round_rect(s, bx + 1, by + 1, fw, bh - 2, 5, fc, fc);
            }
            char pbuf[32];
            snprintf(pbuf, sizeof(pbuf), "%d%%", pct);
            int pw = text_width_px(pbuf, 12);
            surf_draw_text(s, bx + bw - pw, by + 28, 12, COL_TEXT_DIM, pbuf, 0);
        } else {
            // Gamertag input + existing-profile selector row
            surf_draw_text(s, cx, UI_INP_Y + 2, 13, COL_TEXT_DIM, "Gamertag", 76);
            surf_draw_text(s, cx, UI_INP_Y + 19, 10, COL_TEXT_HINT, "1-15 chars", 76);

            uint32_t inp_border = g->profile_err[0] ? COL_ERR : (g->input_focused ? COL_ACCENT : COL_CARD_BORDER);
            surf_round_rect(s, UI_INP_X, UI_INP_Y, UI_INP_W, UI_INP_H, 6, COL_BAR_BG, inp_border);

            const char *disp = g_cfg.profile_name[0] ? g_cfg.profile_name : "Player 1";
            uint32_t txt_col = g_cfg.profile_name[0] ? COL_TEXT : COL_TEXT_HINT;
            surf_draw_text(s, UI_INP_X + 12, UI_INP_Y + 9, 14, txt_col, disp, UI_INP_W - 24);
            if (g->input_focused) {
                int caret_x = UI_INP_X + 12 + text_width_px(g_cfg.profile_name, 14) + 1;
                if (caret_x < UI_INP_X + UI_INP_W - 6) {
                    surf_fill(s, caret_x, UI_INP_Y + 8, 2, 18, COL_ACCENT_HI);
                }
            }

            // Dropdown toggle button (only usable when > 1 profile exists)
            int drp_en = (g_cfg.profile_count > 1);
            uint32_t drp_bg = !drp_en ? COL_BTN_DIS
                            : (g->dropdown_open ? COL_BTN_DN : (g->drop_btn_hover ? COL_BTN_HI : COL_BTN));
            uint32_t drp_bd = !drp_en ? 0x1E2C3Eu
                            : ((g->dropdown_open || g->drop_btn_hover) ? COL_ACCENT : COL_CARD_BORDER);
            uint32_t drp_fg = drp_en ? COL_TEXT : COL_TEXT_DIS;
            surf_round_rect(s, UI_DRP_X, UI_DRP_Y, UI_DRP_W, UI_DRP_H, 6, drp_bg, drp_bd);
            surf_draw_chevron_down(s, UI_DRP_X + UI_DRP_W / 2, UI_DRP_Y + UI_DRP_H / 2, drp_fg);

            // Validation message beside dropdown (only shown on error)
            if (g->profile_err[0]) {
                surf_draw_text(s, UI_DRP_X + UI_DRP_W + 12, UI_INP_Y + 10, 11, COL_ERR, g->profile_err,
                               (UI_CARD_X + UI_CARD_W - 20) - (UI_DRP_X + UI_DRP_W + 12));
            }

            // Data folder path
            char path_line[PATH_CAP + 16];
            snprintf(path_line, sizeof(path_line), "Data: %s", g_cfg.data_dir);
            surf_draw_text(s, cx, UI_CARD_Y + 132, 12, COL_TEXT_HINT, path_line, cw);

            // Skip launcher checkbox
            int cbx = UI_CHK_X, cby = UI_CHK_Y + 3;
            uint32_t cb_bg = g_cfg.skip_launcher ? COL_ACCENT : COL_BAR_BG;
            uint32_t cb_bd = (g_cfg.skip_launcher || g->chk_hover) ? COL_ACCENT_HI : COL_CARD_BORDER;
            surf_round_rect(s, cbx, cby, 18, 18, 4, cb_bg, cb_bd);
            if (g_cfg.skip_launcher) surf_draw_checkmark(s, cbx, cby, 0xFFFFFFu);
            surf_draw_text(s, cbx + 26, UI_CHK_Y + 4, 13, g->chk_hover ? COL_TEXT : COL_TEXT_DIM,
                           "Skip launcher (run with --setup to re-open)", cw - 30);
        }
    }

    // Buttons (uniform 142x36 shape, 6px radius, 13px centered label)
    for (int i = 0; i < g->nbtns; ++i) {
        UiButton *b = &g->btns[i];
        if (!b->visible) continue;
        uint32_t bg, border, fg;
        if (!b->enabled) {
            bg = COL_BTN_DIS; border = 0x1E2C3Eu; fg = COL_TEXT_DIS;
        } else if (b->primary) {
            bg = (g->down_id == b->id) ? COL_ACCENT_DN : (g->hover_id == b->id ? COL_ACCENT_HI : COL_ACCENT);
            border = COL_ACCENT_HI;
            fg = 0xFFFFFFu;
        } else if (b->id == BTN_ODST) {
            bg = (g->down_id == b->id) ? COL_AMBER_DN : (g->hover_id == b->id ? COL_AMBER_HI : COL_AMBER_BTN);
            border = (g->hover_id == b->id) ? COL_AMBER_BD_HI : COL_AMBER_BD;
            fg = COL_AMBER_TEXT;
        } else {
            bg = (g->down_id == b->id) ? COL_BTN_DN : (g->hover_id == b->id ? COL_BTN_HI : COL_BTN);
            border = (g->hover_id == b->id) ? COL_ACCENT : COL_CARD_BORDER;
            fg = COL_TEXT;
        }
        surf_round_rect(s, b->x, b->y, b->w, b->h, 6, bg, border);
        int fsz = 13;
        int tw = text_width_px(b->label, fsz);
        int tx = b->x + (b->w - tw) / 2;
        int ty = b->y + (b->h - fsz) / 2 + 1;
        surf_draw_text(s, tx, ty, fsz, fg, b->label, b->w - 8);
    }

    // Floating profile dropdown popup (drawn on top of card & buttons when open)
    if (g->dropdown_open && !busy && !g_ui.error_only_mode && g_cfg.profile_count > 1) {
        int show_n = g_cfg.profile_count > 7 ? 7 : g_cfg.profile_count;
        int item_h = 30;
        int mx = UI_INP_X, my = UI_INP_Y + UI_INP_H + 4;
        int mw = UI_INP_W + 4 + UI_DRP_W, mh = show_n * item_h + 8;
        surf_round_rect(s, mx, my, mw, mh, 8, 0x101C2Au, COL_ACCENT);
        for (int i = 0; i < show_n; ++i) {
            int iy = my + 4 + i * item_h;
            int active = gamertag_ieq(g_cfg.profiles[i], g_cfg.profile_name);
            if (g->dropdown_hover == i) {
                surf_round_rect(s, mx + 4, iy + 1, mw - 8, item_h - 2, 5, COL_BTN_HI, COL_BTN_HI);
            }
            if (active) {
                surf_round_rect(s, mx + 12, iy + 11, 8, 8, 4, COL_ACCENT_HI, COL_ACCENT_HI);
            }
            surf_draw_text(s, mx + 28, iy + 7, 13, active ? 0xFFFFFFu : COL_TEXT, g_cfg.profiles[i], mw - 38);
        }
    }

    g_x11.XPutImage(g->dpy, g->win, g->gc, g->img, 0, 0, 0, 0, (unsigned int)s->w, (unsigned int)s->h);
    g_x11.XFlush(g->dpy);
}

static void xgui_pump_expose(void *ctx) {
    XGui *g = (XGui *)ctx;
    int need_draw = 0;
    while (g_x11.XPending(g->dpy) > 0) {
        XEvent ev;
        g_x11.XNextEvent(g->dpy, &ev);
        if (ev.type == Expose) need_draw = 1;
    }
    if (need_draw) xgui_render(g);
}

static int xgui_hit_button(XGui *g, int mx, int my) {
    for (int i = 0; i < g->nbtns; ++i) {
        UiButton *b = &g->btns[i];
        if (!b->visible || !b->enabled) continue;
        if (mx >= b->x && mx < b->x + b->w && my >= b->y && my < b->y + b->h) return b->id;
    }
    return BTN_NONE;
}

static void start_setup_worker(void) {
    pthread_mutex_lock(&g_ui.lock);
    if (g_ui.busy) { pthread_mutex_unlock(&g_ui.lock); return; }
    g_ui.cancel_requested = 0;
    g_ui.publishing = 0;
    g_ui.ready_to_play = 0;
    g_ui.busy = 1;
    g_ui.has_error = 0;
    g_ui.progress_pct = 0;
    g_ui.done_flag = 0;
    g_ui.done_ok = 0;
    pthread_mutex_unlock(&g_ui.lock);
    if (pthread_create(&g_ui.worker, NULL, setup_worker_proc, NULL) == 0) {
        g_ui.worker_started = 1;
    } else {
        g_ui.busy = 0;
        ui_set_status(0, 1, "Setup Could Not Start", "Try selecting your Halo 3 media again.");
    }
}

static void cancel_setup_worker(void) {
    pthread_mutex_lock(&g_ui.lock);
    if (!g_ui.publishing) {
        g_ui.cancel_requested = 1;
        if (g_ui.active_child > 0) kill(g_ui.active_child, SIGTERM);
    }
    pthread_mutex_unlock(&g_ui.lock);
}

static void handle_dropped_path(const char *path) {
    if (g_ui.busy || !path || !path[0]) return;
    if (dir_exists(path)) {
        if (is_odst_dlc_dir(path)) {
            snprintf(g_cfg.odst_dlc_src, sizeof(g_cfg.odst_dlc_src), "%s", path);
            g_ui.odst_only_job = 1;
            start_setup_worker();
        } else if (normalize_disc_dir(g_cfg.disc_path, path)) {
            g_cfg.iso_path[0] = 0;
            g_cfg.reextract = 0;
            g_ui.odst_only_job = 0;
            commit_selected_profile(NULL, 0);
            start_setup_worker();
        } else {
            ui_set_status(0, 1, "Not a Halo 3 Disc Folder",
                          "Dropped folder does not contain default.xex and maps/mainmenu.map.");
        }
    } else if (file_exists(path)) {
        if (is_odst_dlc_iso(path)) {
            snprintf(g_cfg.odst_dlc_src, sizeof(g_cfg.odst_dlc_src), "%s", path);
            g_ui.odst_only_job = 1;
            start_setup_worker();
        } else {
            snprintf(g_cfg.iso_path, sizeof(g_cfg.iso_path), "%s", path);
            g_cfg.disc_path[0] = 0;
            g_cfg.reextract = 1;
            g_ui.odst_only_job = 0;
            commit_selected_profile(NULL, 0);
            start_setup_worker();
        }
    }
}

static int run_setup_gui(int auto_start, int error_only, const char *err_title, const char *err_msg) {
    if (!x11_load()) return 0;
    Display *dpy = g_x11.XOpenDisplay(NULL);
    if (!dpy) return 0;

    ft_init();
    if (pipe(g_ui.wake_pipe) != 0) { g_ui.wake_pipe[0] = g_ui.wake_pipe[1] = -1; }
    else {
        fcntl(g_ui.wake_pipe[0], F_SETFL, O_NONBLOCK);
        fcntl(g_ui.wake_pipe[1], F_SETFL, O_NONBLOCK);
    }

    XGui g;
    memset(&g, 0, sizeof(g));
    g.dpy = dpy;
    g.surf.w = 660;
    g.surf.h = 434;
    g.dropdown_hover = -1;
    g.surf.px = calloc((size_t)(g.surf.w * g.surf.h), sizeof(uint32_t));

    int scr = DefaultScreen(dpy);
    Window root = RootWindow(dpy, scr);
    int sw = DisplayWidth(dpy, scr);
    int sh = DisplayHeight(dpy, scr);
    int wx = (sw > g.surf.w) ? (sw - g.surf.w) / 2 : 0;
    int wy = (sh > g.surf.h) ? (sh - g.surf.h) / 2 : 0;

    g.win = g_x11.XCreateSimpleWindow(dpy, root, wx, wy, (unsigned int)g.surf.w, (unsigned int)g.surf.h,
                                      0, COL_BG, COL_BG);
    g_x11.XStoreName(dpy, g.win, error_only ? "Halo 3 - Error" : "Halo 3");

    // Fixed window size
    XSizeHints *hints = g_x11.XAllocSizeHints();
    if (hints) {
        hints->flags = PMinSize | PMaxSize | PPosition;
        hints->min_width = hints->max_width = g.surf.w;
        hints->min_height = hints->max_height = g.surf.h;
        hints->x = wx;
        hints->y = wy;
        g_x11.XSetWMNormalHints(dpy, g.win, hints);
        g_x11.XFree(hints);
    }
    XClassHint *ch = g_x11.XAllocClassHint();
    if (ch) {
        ch->res_name = "halo3-launcher";
        ch->res_class = "io.github.summers_aaron.Halo3";
        g_x11.XSetClassHint(dpy, g.win, ch);
        g_x11.XFree(ch);
    }

    g.wm_delete = g_x11.XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    g_x11.XSetWMProtocols(dpy, g.win, &g.wm_delete, 1);

    // Set X11 window / taskbar icon (_NET_WM_ICON) to the exact same 48x48 Halo 3 icon
    {
        Atom net_wm_icon = g_x11.XInternAtom(dpy, "_NET_WM_ICON", False);
        unsigned long *icon_prop = malloc((size_t)(2 + HALO3_ICON_W * HALO3_ICON_H) * sizeof(unsigned long));
        if (icon_prop) {
            icon_prop[0] = (unsigned long)HALO3_ICON_W;
            icon_prop[1] = (unsigned long)HALO3_ICON_H;
            for (int i = 0; i < HALO3_ICON_W * HALO3_ICON_H; ++i) {
                icon_prop[2 + i] = (unsigned long)HALO3_ICON_48[i];
            }
            g_x11.XChangeProperty(dpy, g.win, net_wm_icon, XA_CARDINAL, 32, PropModeReplace,
                                  (unsigned char *)icon_prop, 2 + HALO3_ICON_W * HALO3_ICON_H);
            free(icon_prop);
        }
    }

    // Advertise Xdnd v5 drag-and-drop support
    g.xdnd_aware       = g_x11.XInternAtom(dpy, "XdndAware", False);
    g.xdnd_enter       = g_x11.XInternAtom(dpy, "XdndEnter", False);
    g.xdnd_position    = g_x11.XInternAtom(dpy, "XdndPosition", False);
    g.xdnd_status      = g_x11.XInternAtom(dpy, "XdndStatus", False);
    g.xdnd_drop        = g_x11.XInternAtom(dpy, "XdndDrop", False);
    g.xdnd_selection   = g_x11.XInternAtom(dpy, "XdndSelection", False);
    g.xdnd_finished    = g_x11.XInternAtom(dpy, "XdndFinished", False);
    g.atom_uri_list    = g_x11.XInternAtom(dpy, "text/uri-list", False);
    g.atom_action_copy = g_x11.XInternAtom(dpy, "XdndActionCopy", False);
    Atom xdnd_ver = 5;
    g_x11.XChangeProperty(dpy, g.win, g.xdnd_aware, XA_ATOM, 32, PropModeReplace, (unsigned char *)&xdnd_ver, 1);

    g_x11.XSelectInput(dpy, g.win, ExposureMask | ButtonPressMask | ButtonReleaseMask |
                                   PointerMotionMask | LeaveWindowMask | KeyPressMask);

    g.gc = g_x11.XCreateGC(dpy, g.win, 0, NULL);
    g.img = g_x11.XCreateImage(dpy, DefaultVisual(dpy, scr), (unsigned int)DefaultDepth(dpy, scr),
                               ZPixmap, 0, (char *)g.surf.px,
                               (unsigned int)g.surf.w, (unsigned int)g.surf.h, 32, 0);

    g_ui.error_only_mode = error_only;
    g_ui.dlc_loaded = check_odst_dlc_installed();
    if (error_only) {
        ui_set_status(0, 1, err_title ? err_title : "Halo 3 - Error", err_msg ? err_msg : "");
    } else {
        char reason[256];
        g_ui.ready_to_play = check_setup_ready(reason, sizeof(reason));
        if (g_ui.ready_to_play && !auto_start) {
            ui_set_status(0, 0, "Ready to Play", "Retail disc and Vulkan shaders cached.");
        } else {
            ui_set_status(0, 0, "Halo 3 Setup", reason[0] ? reason : "Preparing Halo 3 data...");
        }
    }

    g_x11.XMapWindow(dpy, g.win);
    g_x11.XMoveWindow(dpy, g.win, wx, wy);
    xgui_render(&g);

    if (auto_start && !error_only) {
        start_setup_worker();
    }

    int xfd = ConnectionNumber(dpy);
    int running = 1;
    while (running) {
        int need_draw = 0;

        // Check worker completion
        pthread_mutex_lock(&g_ui.lock);
        int done = g_ui.done_flag;
        int ok = g_ui.done_ok;
        g_ui.done_flag = 0;
        pthread_mutex_unlock(&g_ui.lock);
        if (done) {
            if (g_ui.worker_started) {
                pthread_join(g_ui.worker, NULL);
                g_ui.worker_started = 0;
            }
            g_ui.busy = 0;
            g_ui.dlc_loaded = check_odst_dlc_installed();
            g_ui.ready_to_play = ok && check_setup_ready(NULL, 0);
            need_draw = 1;
        }

        while (g_x11.XPending(dpy) > 0) {
            XEvent ev;
            g_x11.XNextEvent(dpy, &ev);
            switch (ev.type) {
            case Expose:
                need_draw = 1;
                break;
            case KeyPress: {
                if (g_ui.busy || g_ui.error_only_mode) break;
                char buf[16] = {0};
                KeySym ks = 0;
                int n = g_x11.XLookupString(&ev.xkey, buf, (int)sizeof(buf) - 1, &ks, NULL);
                if (ks == XK_Escape) {
                    if (g.dropdown_open) { g.dropdown_open = 0; need_draw = 1; }
                    else if (g.input_focused) { g.input_focused = 0; need_draw = 1; }
                    else { running = 0; }
                } else if (ks == XK_Return || ks == XK_KP_Enter) {
                    if (g.dropdown_open) {
                        g.dropdown_open = 0;
                        need_draw = 1;
                    } else if (g_ui.ready_to_play) {
                        if (commit_selected_profile(g.profile_err, sizeof(g.profile_err))) {
                            g.profile_err[0] = 0;
                            g_ui.launch_after_ui = 1;
                            running = 0;
                        } else {
                            need_draw = 1;
                        }
                    }
                } else if (g.input_focused) {
                    if (ks == XK_BackSpace) {
                        size_t len = strlen(g_cfg.profile_name);
                        if (len > 0) {
                            g_cfg.profile_name[len - 1] = 0;
                            g.profile_err[0] = 0;
                            need_draw = 1;
                        }
                    } else if (n == 1) {
                        unsigned char c = (unsigned char)buf[0];
                        size_t len = strlen(g_cfg.profile_name);
                        int allowed = isalnum(c) || (c == ' ' && len > 0 && g_cfg.profile_name[len - 1] != ' ');
                        if (allowed && len < GAMERTAG_MAX) {
                            g_cfg.profile_name[len] = (char)c;
                            g_cfg.profile_name[len + 1] = 0;
                            g.profile_err[0] = 0;
                            need_draw = 1;
                        }
                    }
                }
                break;
            }
            case MotionNotify: {
                int mx = ev.xmotion.x, my = ev.xmotion.y;
                if (g.dropdown_open) {
                    int show_n = g_cfg.profile_count > 7 ? 7 : g_cfg.profile_count;
                    int dx = UI_INP_X, dy = UI_INP_Y + UI_INP_H + 4;
                    int dw = UI_INP_W + 4 + UI_DRP_W, dh = show_n * 30 + 8;
                    int dhov = -1;
                    if (pt_in(mx, my, dx + 4, dy + 4, dw - 8, dh - 8)) {
                        dhov = (my - (dy + 4)) / 30;
                        if (dhov < 0 || dhov >= show_n) dhov = -1;
                    }
                    if (dhov != g.dropdown_hover) { g.dropdown_hover = dhov; need_draw = 1; }
                    break;
                }
                int h = xgui_hit_button(&g, mx, my);
                if (h != g.hover_id) { g.hover_id = h; need_draw = 1; }
                int chov = (!g_ui.busy && !g_ui.error_only_mode && pt_in(mx, my, UI_CHK_X, UI_CHK_Y, UI_CHK_W, UI_CHK_H));
                if (chov != g.chk_hover) { g.chk_hover = chov; need_draw = 1; }
                int dh = (!g_ui.busy && !g_ui.error_only_mode && g_cfg.profile_count > 1 && pt_in(mx, my, UI_DRP_X, UI_DRP_Y, UI_DRP_W, UI_DRP_H));
                if (dh != g.drop_btn_hover) { g.drop_btn_hover = dh; need_draw = 1; }
                break;
            }
            case LeaveNotify:
                if (g.hover_id != BTN_NONE || g.chk_hover || g.drop_btn_hover || g.dropdown_hover != -1) {
                    g.hover_id = BTN_NONE;
                    g.chk_hover = 0;
                    g.drop_btn_hover = 0;
                    g.dropdown_hover = -1;
                    need_draw = 1;
                }
                break;
            case ButtonPress:
                if (ev.xbutton.button == Button1) {
                    int mx = ev.xbutton.x, my = ev.xbutton.y;
                    if (g.dropdown_open) {
                        int show_n = g_cfg.profile_count > 7 ? 7 : g_cfg.profile_count;
                        int dx = UI_INP_X, dy = UI_INP_Y + UI_INP_H + 4;
                        int dw = UI_INP_W + 4 + UI_DRP_W, dh = show_n * 30 + 8;
                        if (pt_in(mx, my, dx, dy, dw, dh)) {
                            int idx = (my - (dy + 4)) / 30;
                            if (idx >= 0 && idx < show_n) {
                                snprintf(g_cfg.profile_name, sizeof(g_cfg.profile_name), "%s", g_cfg.profiles[idx]);
                                g.profile_err[0] = 0;
                                save_launcher_prefs();
                            }
                        }
                        g.dropdown_open = 0;
                        g.dropdown_hover = -1;
                        need_draw = 1;
                        break;
                    }
                    if (!g_ui.busy && !g_ui.error_only_mode) {
                        if (pt_in(mx, my, UI_INP_X, UI_INP_Y, UI_INP_W, UI_INP_H)) {
                            g.input_focused = 1;
                            need_draw = 1;
                            break;
                        }
                        if (g_cfg.profile_count > 1 && pt_in(mx, my, UI_DRP_X, UI_DRP_Y, UI_DRP_W, UI_DRP_H)) {
                            g.input_focused = 0;
                            g.dropdown_open = !g.dropdown_open;
                            g.dropdown_hover = -1;
                            need_draw = 1;
                            break;
                        }
                        if (pt_in(mx, my, UI_CHK_X, UI_CHK_Y, UI_CHK_W, UI_CHK_H)) {
                            g.input_focused = 0;
                            g_cfg.skip_launcher = !g_cfg.skip_launcher;
                            save_launcher_prefs();
                            need_draw = 1;
                            break;
                        }
                    }
                    if (g.input_focused) { g.input_focused = 0; need_draw = 1; }
                    int h = xgui_hit_button(&g, mx, my);
                    if (h != g.down_id) { g.down_id = h; need_draw = 1; }
                }
                break;
            case ButtonRelease:
                if (ev.xbutton.button == Button1) {
                    int clicked = (g.down_id != BTN_NONE && g.down_id == xgui_hit_button(&g, ev.xbutton.x, ev.xbutton.y)) ? g.down_id : BTN_NONE;
                    g.down_id = BTN_NONE;
                    need_draw = 1;
                    if (clicked == BTN_PLAY) {
                        if (commit_selected_profile(g.profile_err, sizeof(g.profile_err))) {
                            g.profile_err[0] = 0;
                            g_ui.launch_after_ui = 1;
                            running = 0;
                        }
                    } else if (clicked == BTN_ISO) {
                        if (!commit_selected_profile(g.profile_err, sizeof(g.profile_err))) break;
                        g.profile_err[0] = 0;
                        char picked[PATH_CAP];
                        int pr = pick_path_native(g.win, 0, picked, sizeof(picked), xgui_pump_expose, &g);
                        if (pr == 1) {
                            if (is_odst_dlc_iso(picked)) {
                                snprintf(g_cfg.odst_dlc_src, sizeof(g_cfg.odst_dlc_src), "%s", picked);
                                g_ui.odst_only_job = 1;
                            } else {
                                snprintf(g_cfg.iso_path, sizeof(g_cfg.iso_path), "%s", picked);
                                g_cfg.disc_path[0] = 0;
                                g_cfg.reextract = 1;
                                g_ui.odst_only_job = 0;
                            }
                            start_setup_worker();
                        } else if (pr < 0) {
                            ui_set_status(0, 1, "No File Picker Found",
                                          "Drop your .iso onto this window or pass its path on the command line.");
                        }
                    } else if (clicked == BTN_FOLDER) {
                        if (!commit_selected_profile(g.profile_err, sizeof(g.profile_err))) break;
                        g.profile_err[0] = 0;
                        char picked[PATH_CAP];
                        int pr = pick_path_native(g.win, 1, picked, sizeof(picked), xgui_pump_expose, &g);
                        if (pr == 1) {
                            if (is_odst_dlc_dir(picked)) {
                                snprintf(g_cfg.odst_dlc_src, sizeof(g_cfg.odst_dlc_src), "%s", picked);
                                g_ui.odst_only_job = 1;
                                start_setup_worker();
                            } else if (normalize_disc_dir(g_cfg.disc_path, picked)) {
                                g_cfg.iso_path[0] = 0;
                                g_cfg.reextract = 0;
                                g_ui.odst_only_job = 0;
                                start_setup_worker();
                            } else {
                                ui_set_status(0, 1, "Not a Halo 3 Disc Folder",
                                              "Folder must contain default.xex and maps/mainmenu.map.");
                            }
                        } else if (pr < 0) {
                            ui_set_status(0, 1, "No Folder Picker Found",
                                          "Drop your disc folder onto this window or pass --disc PATH.");
                        }
                    } else if (clicked == BTN_REBUILD) {
                        g_cfg.rebuild_shaders = 1;
                        g_ui.odst_only_job = 0;
                        start_setup_worker();
                    } else if (clicked == BTN_ODST) {
                        char picked[PATH_CAP];
                        int pr = pick_path_native(g.win, 0, picked, sizeof(picked), xgui_pump_expose, &g);
                        if (pr == 1) {
                            snprintf(g_cfg.odst_dlc_src, sizeof(g_cfg.odst_dlc_src), "%s", picked);
                            g_ui.odst_only_job = 1;
                            start_setup_worker();
                        } else if (pr < 0) {
                            ui_set_status(0, 1, "No File Picker Found",
                                          "Pass --odst-dlc /path/to/ODST_Disc2.iso to extract ODST DLC.");
                        }
                    } else if (clicked == BTN_DATA) {
                        open_folder_in_desktop(g_cfg.data_dir);
                    } else if (clicked == BTN_CANCEL) {
                        if (g_ui.busy) cancel_setup_worker();
                        else running = 0;
                    }
                }
                break;
            case ClientMessage:
                if ((Atom)ev.xclient.data.l[0] == g.wm_delete) {
                    if (g_ui.busy) cancel_setup_worker();
                    else running = 0;
                } else if (ev.xclient.message_type == g.xdnd_enter) {
                    g.xdnd_src = (Window)ev.xclient.data.l[0];
                } else if (ev.xclient.message_type == g.xdnd_position) {
                    XEvent reply;
                    memset(&reply, 0, sizeof(reply));
                    reply.xclient.type = ClientMessage;
                    reply.xclient.window = (Window)ev.xclient.data.l[0];
                    reply.xclient.message_type = g.xdnd_status;
                    reply.xclient.format = 32;
                    reply.xclient.data.l[0] = (long)g.win;
                    reply.xclient.data.l[1] = g_ui.busy ? 0 : 1;
                    reply.xclient.data.l[4] = (long)g.atom_action_copy;
                    g_x11.XSendEvent(dpy, reply.xclient.window, False, NoEventMask, &reply);
                    g_x11.XFlush(dpy);
                } else if (ev.xclient.message_type == g.xdnd_drop) {
                    g.xdnd_src = (Window)ev.xclient.data.l[0];
                    g_x11.XConvertSelection(dpy, g.xdnd_selection, g.atom_uri_list, g.xdnd_selection,
                                            g.win, (Time)ev.xclient.data.l[2]);
                }
                break;
            case SelectionNotify:
                if (ev.xselection.property != None) {
                    Atom actual_type;
                    int actual_fmt;
                    unsigned long nitems = 0, bytes_after = 0;
                    unsigned char *data = NULL;
                    if (g_x11.XGetWindowProperty(dpy, g.win, ev.xselection.property, 0, 4096, True,
                                                 AnyPropertyType, &actual_type, &actual_fmt,
                                                 &nitems, &bytes_after, &data) == Success && data) {
                        char decoded[PATH_CAP];
                        if (url_decode_file_uri((const char *)data, decoded, sizeof(decoded))) {
                            handle_dropped_path(decoded);
                        }
                        g_x11.XFree(data);
                    }
                }
                if (g.xdnd_src) {
                    XEvent fin;
                    memset(&fin, 0, sizeof(fin));
                    fin.xclient.type = ClientMessage;
                    fin.xclient.window = g.xdnd_src;
                    fin.xclient.message_type = g.xdnd_finished;
                    fin.xclient.format = 32;
                    fin.xclient.data.l[0] = (long)g.win;
                    fin.xclient.data.l[1] = 1;
                    fin.xclient.data.l[2] = (long)g.atom_action_copy;
                    g_x11.XSendEvent(dpy, g.xdnd_src, False, NoEventMask, &fin);
                    g_x11.XFlush(dpy);
                    g.xdnd_src = 0;
                }
                need_draw = 1;
                break;
            }
        }

        if (!running) break;
        if (need_draw) xgui_render(&g);

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(xfd, &rfds);
        int maxfd = xfd;
        if (g_ui.wake_pipe[0] >= 0) {
            FD_SET(g_ui.wake_pipe[0], &rfds);
            if (g_ui.wake_pipe[0] > maxfd) maxfd = g_ui.wake_pipe[0];
        }
        struct timeval tv = { 0, 100000 };
        if (select(maxfd + 1, &rfds, NULL, NULL, &tv) > 0) {
            if (g_ui.wake_pipe[0] >= 0 && FD_ISSET(g_ui.wake_pipe[0], &rfds)) {
                char drain[64];
                while (read(g_ui.wake_pipe[0], drain, sizeof(drain)) > 0) {}
                xgui_render(&g);
            }
        }
    }

    if (g_ui.worker_started) {
        cancel_setup_worker();
        pthread_join(g_ui.worker, NULL);
        g_ui.worker_started = 0;
    }

    if (g.img) { g.img->data = NULL; XDestroyImage(g.img); }
    free(g.surf.px);
    if (g.gc) g_x11.XFreeGC(dpy, g.gc);
    g_x11.XDestroyWindow(dpy, g.win);
    g_x11.XCloseDisplay(dpy);
    if (g_ui.wake_pipe[0] >= 0) { close(g_ui.wake_pipe[0]); close(g_ui.wake_pipe[1]); }
    ft_shutdown();
    return g_ui.launch_after_ui;
}

static void show_fatal_error(const char *title, const char *msg) {
    fprintf(stderr, "%s\n%s\n", title, msg);
    if (!g_cfg.headless && getenv("DISPLAY") && getenv("DISPLAY")[0]) {
        run_setup_gui(0, 1, title, msg);
    }
}

// ---------------------------------------------------------------------------
// Help / CLI / Defaults / Launch
// ---------------------------------------------------------------------------
static void print_version(void) {
    printf("Halo 3 PC Port %s (x86_64%s, shader ABI %s)\n",
           g_cfg.local_version[0] ? g_cfg.local_version : "unknown",
           g_cfg.is_flatpak ? " Flatpak" : "",
           g_cfg.shader_abi[0] ? g_cfg.shader_abi : "unknown");
}

static void print_help(void) {
    puts(
        "Halo 3 - Native 64-bit PC Port (Linux)\n"
        "\n"
        "Usage:\n"
        "  Halo3-x86_64.AppImage [/path/to/Halo3.iso | /path/to/halo3-disc] [options]\n"
        "  flatpak run io.github.summers_aaron.Halo3 [/path/to/Halo3.iso] [options]\n"
        "\n"
        "On first launch, a guided setup window asks you to select your retail Halo 3\n"
        "Xbox 360 .iso (or drag & drop it). Game files and Vulkan 1.3 SPIR-V shaders are\n"
        "cached in your standard XDG user data folder; subsequent launches start immediately.\n"
        "\n"
        "Disc & Setup:\n"
        "  --setup                 Open the Setup & Maintenance window\n"
        "  --iso PATH              Retail Halo 3 Xbox 360 .iso (XGD2 / Redump / raw XISO)\n"
        "  --disc PATH             Already-extracted retail Halo 3 disc directory\n"
        "  --dlc PATH              DLC map packs directory (STFS LIVE/PIRS/CON)\n"
        "  --data PATH             Override data/cache directory (default: $XDG_DATA_HOME/halo3)\n"
        "  --config PATH           Path to halo3.ini config file\n"
#ifndef HALO3_RELEASE_BUILD
        "  --shader-pack PATH      Override Pack v3 shader pack path\n"
#endif
        "  --reextract             Re-extract the ISO even if cached disc files exist\n"
        "  --rebuild-shaders       Re-translate the shader pack even if it exists\n"
        "  -j, --jobs N            Shader translation threads (default: CPU core count)\n"
        "\n"
        "Graphics, Input & Multiplayer:\n"
        "  --res WxH               Window resolution (e.g. 1920x1080, 3840x2160)\n"
        "  --window-mode MODE      windowed, borderless, fullscreen\n"
        "  --renderer NAME         auto, vulkan, d3d12\n"
#ifndef HALO3_RELEASE_BUILD
        "  --scale 1|2|3           Internal render scale (1=720p, 2=1440p, 3=4K)\n"
#endif
        "  --fps N                 Target frame rate (30, 60, 120, 240, or 0 uncapped)\n"
#ifndef HALO3_RELEASE_BUILD
        "  --interpolate 0|1       Toggle high-Hz frame interpolation (default: 1)\n"
        "  --no-interpolate        Shorthand for --interpolate 0\n"
#endif
        "  --vsync 0|1             Enable or disable VSync\n"
#ifndef HALO3_RELEASE_BUILD
        "  --gamma N               Display gamma curve (0..3)\n"
        "  --gpu N                 Select Vulkan GPU device index\n"
        "  --hd-text MODE          HD UI text mode (0=off, 1=native-resolution font, sdf=Guide SDF)\n"
        "  --raw-mouse             Enable raw mouse-look\n"
        "  --mouse-capture         Capture/lock cursor to window\n"
#else
        "  --hd-text 0|1           Enable or disable HD UI text (default: 1)\n"
#endif
        "  --mouse-sens FLOAT      Mouse sensitivity multiplier\n"
#ifndef HALO3_RELEASE_BUILD
        "  --players 1..4          Sign in 1-4 local split-screen players\n"
        "  --guests 0..3           Sign in 0-3 local guest players\n"
        "  --coop-4p               Allow up to 4 local players in Campaign co-op\n"
#endif
        "  --gamertag NAME         Set local profile / SlopBoxLive gamertag\n"
#ifndef HALO3_RELEASE_BUILD
        "  --guide MODE            Guide UI mode (interactive or auto)\n"
        "  --guide-render MODE     Guide renderer (ingame or window)\n"
        "  --audio-backend NAME    Audio backend (pulse, alsa, null, wav:<path>)\n"
        "  --audio-channels 2|6    Audio channels (2=stereo, 6=5.1 surround)\n"
        "  --no-audio              Disable audio output\n"
#endif
        "  --voice / --no-voice    Enable or disable microphone voice chat\n"
#ifndef HALO3_RELEASE_BUILD
        "  --lan                   Enable System Link LAN multiplayer over UDP\n"
        "  --lan-port PORT         UDP port for System Link (default: 3074)\n"
        "  --lan-addr IP           Bind address for System Link\n"
        "  --lan-slots N           Local console slots for multi-instance LAN\n"
        "  --lan-peer IP[:PORT]    Additional broadcast peer(s) for System Link\n"
#endif
        "  --live URL              Connect to a SlopBoxLive server URL\n"
#ifndef HALO3_RELEASE_BUILD
        "  --live-account PATH     Path to persist Xbox Live account token JSON\n"
        "  --map NAME              Quick-boot directly into a map (e.g. 010_jungle)\n"
        "  --frame-stats N         Print frame timing stats every N frames\n"
#endif
        "  --update / --no-update  Force or skip AppImage auto-update check\n"
        "  --version               Print bundled version and exit\n"
        "  -h, --help              Show this help message"
    );
}

static int init_defaults(void) {
    memset(&g_cfg, 0, sizeof(g_cfg));
    pthread_mutex_init(&g_ui.lock, NULL);
    g_ui.wake_pipe[0] = g_ui.wake_pipe[1] = -1;

    char self[PATH_CAP] = {0};
    ssize_t len = readlink("/proc/self/exe", self, sizeof(self) - 1);
    if (len > 0) {
        self[len] = 0;
        parent_dir(self, g_cfg.bin_dir, sizeof(g_cfg.bin_dir));
    } else {
        snprintf(g_cfg.bin_dir, sizeof(g_cfg.bin_dir), ".");
    }

    char prefix[PATH_CAP];
    parent_dir(g_cfg.bin_dir, prefix, sizeof(prefix));
    join_path(g_cfg.share_dir, sizeof(g_cfg.share_dir), prefix, "share/halo3");

    g_cfg.is_flatpak = (access("/.flatpak-info", F_OK) == 0 || (getenv("FLATPAK_ID") && getenv("FLATPAK_ID")[0]));

    char f[PATH_CAP];
    join_path(f, sizeof(f), g_cfg.share_dir, "VERSION");
    read_small_text_file(f, g_cfg.local_version, sizeof(g_cfg.local_version));
    if (!g_cfg.local_version[0]) snprintf(g_cfg.local_version, sizeof(g_cfg.local_version), "unknown");

    join_path(f, sizeof(f), g_cfg.share_dir, "SHADER_ABI");
    read_small_text_file(f, g_cfg.shader_abi, sizeof(g_cfg.shader_abi));

    const char *env_data = getenv("HALO3_DATA");
    if (env_data && env_data[0]) {
        if (strlen(env_data) >= sizeof(g_cfg.data_dir)) return 0;
        strcpy(g_cfg.data_dir, env_data);
    } else {
        const char *xdg = getenv("XDG_DATA_HOME");
        if (xdg && xdg[0]) {
            join_path(g_cfg.data_dir, sizeof(g_cfg.data_dir), xdg, "halo3");
        } else {
            const char *home = getenv("HOME");
            join_path(g_cfg.data_dir, sizeof(g_cfg.data_dir), (home && home[0]) ? home : "/tmp", ".local/share/halo3");
        }
    }

    const char *env_disc = getenv("HALO3_DISC");
    if (env_disc && env_disc[0]) {
        if (strlen(env_disc) >= sizeof(g_cfg.disc_path)) return 0;
        strcpy(g_cfg.disc_path, env_disc);
    }

    const char *env_sp = getenv("HALO3_SHADER_PACK");
    if (env_sp && env_sp[0]) {
        if (strlen(env_sp) >= sizeof(g_cfg.shader_pack)) return 0;
        strcpy(g_cfg.shader_pack, env_sp);
        g_cfg.custom_shader_pack = 1;
    }

    const char *env_win = getenv("HALO3_WINDOW");
    if (env_win && strcmp(env_win, "0") == 0) g_cfg.headless = 1;

    long ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    g_cfg.jobs = (ncpu > 0) ? (int)ncpu : 8;
    return 1;
}

static int parse_args(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        const char *a = argv[i];
        const char *v = (i + 1 < argc) ? argv[i + 1] : NULL;
        if (strcmp(a, "-h") == 0 || strcmp(a, "--help") == 0) {
            print_help();
            return 100;
        } else if (strcmp(a, "--version") == 0) {
            print_version();
            return 100;
        } else if (strcmp(a, "--setup") == 0) {
            g_cfg.force_setup = 1;
        } else if (strcmp(a, "--iso") == 0 && v) {
            if (strlen(v) >= sizeof(g_cfg.iso_path)) goto path_error;
            strcpy(g_cfg.iso_path, v); i++;
        } else if (strcmp(a, "--disc") == 0 && v) {
            if (strlen(v) >= sizeof(g_cfg.disc_path)) goto path_error;
            strcpy(g_cfg.disc_path, v); i++;
        } else if (strcmp(a, "--odst-dlc") == 0 && v) {
            if (strlen(v) >= sizeof(g_cfg.odst_dlc_src)) goto path_error;
            strcpy(g_cfg.odst_dlc_src, v); i++;
        } else if (strcmp(a, "--data") == 0 && v) {
            if (strlen(v) >= sizeof(g_cfg.data_dir)) goto path_error;
            strcpy(g_cfg.data_dir, v); i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (strcmp(a, "--shader-pack") == 0 && v) {
            if (strlen(v) >= sizeof(g_cfg.shader_pack)) goto path_error;
            strcpy(g_cfg.shader_pack, v); g_cfg.custom_shader_pack = 1; i++;
#endif
        } else if (strcmp(a, "--dlc") == 0 && v) { setenv("HALO3_DLC", v, 1); i++;
        } else if (strcmp(a, "--config") == 0 && v) { setenv("HALO3_CONFIG", v, 1); i++;
        } else if (strcmp(a, "--res") == 0 && v) { setenv("HALO3_RES", v, 1); i++;
        } else if (strcmp(a, "--window-mode") == 0 && v) { setenv("HALO3_WINDOW_MODE", v, 1); i++;
        } else if (strcmp(a, "--renderer") == 0 && v) { setenv("HALO3_RENDERER", v, 1); i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (strcmp(a, "--scale") == 0 && v) { setenv("HALO3_RES_SCALE", v, 1); i++;
#endif
        } else if (strcmp(a, "--fps") == 0 && v) { setenv("HALO3_FPS", v, 1); i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (strcmp(a, "--interpolate") == 0 && v) { setenv("HALO3_INTERPOLATE", v, 1); i++;
        } else if (strcmp(a, "--no-interpolate") == 0) { setenv("HALO3_INTERPOLATE", "0", 1);
#endif
        } else if (strcmp(a, "--vsync") == 0 && v) { setenv("HALO3_VSYNC", v, 1); i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (strcmp(a, "--gamma") == 0 && v) { setenv("HALO3_DISPLAY_GAMMA", v, 1); i++;
        } else if (strcmp(a, "--gpu") == 0 && v) { setenv("HALO3_VULKAN_DEVICE", v, 1); i++;
#endif
        } else if (strcmp(a, "--hd-text") == 0 && v) {
#ifndef HALO3_RELEASE_BUILD
            if (strcmp(v, "sdf") == 0) {
                setenv("HALO3_GUIDE_HD_TEXT", "sdf", 1);
            } else
#endif
            {
                setenv("HALO3_HD_TEXT", v, 1);
                setenv("HALO3_GUIDE_HD_TEXT", v, 1);
            }
            i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (strcmp(a, "--raw-mouse") == 0) { setenv("HALO3_RAW_MOUSE", "1", 1);
        } else if (strcmp(a, "--mouse-capture") == 0) { setenv("HALO3_MOUSE_CAPTURE", "1", 1);
#endif
        } else if (strcmp(a, "--mouse-sens") == 0 && v) { setenv("HALO3_MOUSE_SENS", v, 1); i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (strcmp(a, "--players") == 0 && v) { setenv("HALO3_LOCAL_PLAYERS", v, 1); i++;
        } else if (strcmp(a, "--guests") == 0 && v) { setenv("HALO3_LOCAL_GUESTS", v, 1); i++;
        } else if (strcmp(a, "--coop-4p") == 0) { setenv("HALO3_COOP_4P", "1", 1);
#endif
        } else if ((strcmp(a, "--gamertag") == 0 || strcmp(a, "--profile") == 0) && v) {
            snprintf(g_cfg.profile_name, sizeof(g_cfg.profile_name), "%s", v);
            g_cfg.profile_from_cli = 1;
            setenv("HALO3_PROFILE_NAME", v, 1);
            setenv("HALO3_GAMERTAG", v, 1);
            i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (strcmp(a, "--guide") == 0 && v) { setenv("HALO3_GUIDE", v, 1); i++;
        } else if (strcmp(a, "--guide-render") == 0 && v) { setenv("HALO3_GUIDE_RENDER", v, 1); i++;
        } else if (strcmp(a, "--audio-backend") == 0 && v) { setenv("HALO3_AUDIO_BACKEND", v, 1); i++;
        } else if (strcmp(a, "--audio-channels") == 0 && v) { setenv("HALO3_AUDIO_CHANNELS", v, 1); i++;
        } else if (strcmp(a, "--no-audio") == 0) { setenv("HALO3_NO_AUDIO", "1", 1);
#endif
        } else if (strcmp(a, "--voice") == 0) { setenv("HALO3_VOICE", "1", 1);
        } else if (strcmp(a, "--no-voice") == 0) { setenv("HALO3_VOICE", "0", 1);
#ifndef HALO3_RELEASE_BUILD
        } else if (strcmp(a, "--lan") == 0) { setenv("HALO3_LAN", "1", 1);
        } else if (strcmp(a, "--lan-port") == 0 && v) { setenv("HALO3_LAN_PORT", v, 1); i++;
        } else if (strcmp(a, "--lan-addr") == 0 && v) { setenv("HALO3_LAN_ADDRESS", v, 1); i++;
        } else if (strcmp(a, "--lan-slots") == 0 && v) { setenv("HALO3_LAN_SLOTS", v, 1); i++;
        } else if (strcmp(a, "--lan-peer") == 0 && v) { setenv("HALO3_LAN_PEER", v, 1); i++;
#endif
        } else if (strcmp(a, "--live") == 0 && v) { setenv("HALO3_LIVE_SERVER", v, 1); i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (strcmp(a, "--live-account") == 0 && v) { setenv("HALO3_LIVE_ACCOUNT", v, 1); i++;
        } else if (strcmp(a, "--map") == 0 && v) { setenv("HALO3_PROBE_MAP", v, 1); i++;
        } else if (strcmp(a, "--frame-stats") == 0 && v) { setenv("HALO3_FRAME_STATS", v, 1); i++;
#endif
        } else if (strcmp(a, "--reextract") == 0) { g_cfg.reextract = 1;
        } else if (strcmp(a, "--rebuild-shaders") == 0) { g_cfg.rebuild_shaders = 1;
        } else if (strcmp(a, "--update") == 0 || strcmp(a, "--no-update") == 0) {
            // Handled by AppRun when running as an AppImage; ignored in Flatpak
        } else if ((strcmp(a, "-j") == 0 || strcmp(a, "--jobs") == 0) && v) {
            int j = atoi(v);
            if (j > 0) g_cfg.jobs = j;
            i++;
        } else if (a[0] == '-') {
            fprintf(stderr, "halo3: unknown option '%s' (try --help)\n", a);
            return 2;
        } else {
            if (dir_exists(a)) {
                if (is_odst_dlc_dir(a)) snprintf(g_cfg.odst_dlc_src, sizeof(g_cfg.odst_dlc_src), "%s", a);
                else snprintf(g_cfg.disc_path, sizeof(g_cfg.disc_path), "%s", a);
            } else if (file_exists(a)) {
                if (is_odst_dlc_iso(a)) snprintf(g_cfg.odst_dlc_src, sizeof(g_cfg.odst_dlc_src), "%s", a);
                else snprintf(g_cfg.iso_path, sizeof(g_cfg.iso_path), "%s", a);
            } else {
                char msg[PATH_CAP + 64];
                snprintf(msg, sizeof(msg), "Path not found: %s", a);
                show_fatal_error("Halo 3 - Path Not Found", msg);
                return 1;
            }
        }
    }

    if (g_cfg.data_dir[0] != '/') {
        char cwd[PATH_CAP], data[PATH_CAP];
        if (!g_cfg.data_dir[0] || !getcwd(cwd, sizeof(cwd)) ||
            !join_path(data, sizeof(data), cwd, g_cfg.data_dir)) {
            show_fatal_error("Halo 3 - Data Path Error", "Could not resolve the data folder path.");
            return 1;
        }
        strcpy(g_cfg.data_dir, data);
    }
    if (!g_cfg.custom_shader_pack &&
        !join_path(g_cfg.shader_pack, sizeof(g_cfg.shader_pack), g_cfg.data_dir, "shaders/halo3_shaders.pack"))
        goto path_error;
    return 0;

path_error:
    show_fatal_error("Halo 3 - Path Error", "A Halo 3 path is too long for the launcher.");
    return 1;
}

static int launch_game(void) {
    ensure_dir_recursive(g_cfg.data_dir);

    setenv("HALO3_DATA", g_cfg.data_dir, 1);
    setenv("HALO3_DISC", g_cfg.disc_path, 1);
    if (g_cfg.custom_shader_pack) setenv("HALO3_SHADER_PACK", g_cfg.shader_pack, 1);
    else unsetenv("HALO3_SHADER_PACK");
    setenv("HALO3_VULKAN", "1", 0);
    setenv("PORT_STUB_CONTINUE", "1", 0);

    // Keep Player 1 on its canonical built-in PORT_PROFILE_OFFLINE_XUID; export custom gamertags.
    if (g_cfg.profile_name[0] && !gamertag_ieq(g_cfg.profile_name, "Player 1")) {
        setenv("HALO3_PROFILE_NAME", g_cfg.profile_name, 1);
        setenv("HALO3_GAMERTAG", g_cfg.profile_name, 1);
    } else if (!g_cfg.profile_from_cli) {
        unsetenv("HALO3_PROFILE_NAME");
        unsetenv("HALO3_GAMERTAG");
    }

    if (!getenv("HALO3_LIVE_ACCOUNT")) {
        char acct[PATH_CAP];
        if (!join_path(acct, sizeof(acct), g_cfg.data_dir, "live_account.json") ||
            setenv("HALO3_LIVE_ACCOUNT", acct, 0) != 0) {
            show_fatal_error("Halo 3 - Data Path Error", "Could not set the Live account path in the data folder.");
            return 1;
        }
    }

    char game_bin[PATH_CAP];
    join_path(game_bin, sizeof(game_bin), g_cfg.bin_dir, "halo3");
    if (!file_exists(game_bin)) {
        show_fatal_error("Halo 3 - Missing Binary", "Could not find halo3 game executable next to halo3-launcher.");
        return 1;
    }

    // If invoked from a terminal or headless mode, exec directly so stdout/stderr stream live.
    if (g_cfg.headless || isatty(STDOUT_FILENO) || isatty(STDERR_FILENO)) {
        char *const argv[] = { game_bin, NULL };
        execv(game_bin, argv);
        fprintf(stderr, "halo3: failed to exec %s: %s\n", game_bin, strerror(errno));
        return 1;
    }

    // Graphical launch (double-clicked AppImage / Flatpak menu): log output to $DATA_DIR/halo3.log
    // and report immediate startup errors in a friendly GUI card.
    char log_path[PATH_CAP];
    join_path(log_path, sizeof(log_path), g_cfg.data_dir, "halo3.log");
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    pid_t pid = fork();
    if (pid < 0) return 1;
    if (pid == 0) {
        int fd = open(log_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd >= 0) {
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
        char *const argv[] = { game_bin, NULL };
        execv(game_bin, argv);
        _exit(127);
    }

    int status = 0;
    while (waitpid(pid, &status, 0) < 0 && errno == EINTR) {}
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (double)(t1.tv_sec - t0.tv_sec) + (double)(t1.tv_nsec - t0.tv_nsec) * 1e-9;

    int code = WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    if (code != 0 && elapsed < 5.0) {
        char msg[PATH_CAP + 256];
        snprintf(msg, sizeof(msg),
                 "Halo 3 exited immediately (exit code %d).\n"
                 "Ensure your GPU supports Vulkan 1.3 and check the log:\n%s",
                 code, log_path);
        show_fatal_error("Halo 3 - Launch Error", msg);
    }
    return code;
}

int main(int argc, char **argv) {
    if (!init_defaults()) {
        show_fatal_error("Halo 3 - Path Error", "A Halo 3 environment path is too long.");
        return 1;
    }
    int pr = parse_args(argc, argv);
    if (pr == 100) return 0;
    if (pr != 0) return pr;

    load_launcher_prefs();

    // Preflight 1: 64-bit Vulkan loader check
    const char *vk_env = getenv("HALO3_VULKAN");
    if ((!vk_env || strcmp(vk_env, "0") != 0) && !has_vulkan_loader()) {
        char hint[256], msg[768];
        distro_vulkan_hint(hint, sizeof(hint));
        snprintf(msg, sizeof(msg),
                 "64-bit Vulkan loader (libvulkan.so.1) was not found.\n\n%s", hint);
        show_fatal_error("Halo 3 - Vulkan Not Found", msg);
        return 1;
    }

    // Preflight 2: XWayland check on pure Wayland sessions
    if (!g_cfg.headless && (!getenv("DISPLAY") || !getenv("DISPLAY")[0]) &&
        getenv("WAYLAND_DISPLAY") && getenv("WAYLAND_DISPLAY")[0]) {
        show_fatal_error("Halo 3 - XWayland Required",
                         "WAYLAND_DISPLAY is set, but DISPLAY is not set.\n"
                         "Halo 3 currently opens an X11 window (via XWayland on Wayland desktops).");
        return 1;
    }

    int ready = check_setup_ready(NULL, 0);

    int can_gui = (!g_cfg.headless && getenv("DISPLAY") && getenv("DISPLAY")[0] != 0);

    if (!can_gui) {
        if (!ready || g_cfg.odst_dlc_src[0]) {
            int rc = (int)(intptr_t)setup_worker_proc(NULL);
            if (rc != 0) return rc;
        }
        return launch_game();
    }

    int show_launcher = !ready || g_cfg.force_setup || g_cfg.odst_dlc_src[0] ||
                        !g_cfg.skip_launcher || x11_shift_held();

    if (show_launcher) {
        int auto_start = 0;
        if (!ready || g_cfg.odst_dlc_src[0]) {
            if (g_cfg.odst_dlc_src[0] && !g_cfg.iso_path[0] && !ready) g_ui.odst_only_job = 1;
            auto_start = g_cfg.iso_path[0] || g_cfg.odst_dlc_src[0] || select_setup_disc();
        }
        if (!run_setup_gui(auto_start, 0, NULL, NULL)) {
            return 0;
        }
        if (!check_setup_ready(NULL, 0)) {
            return 1;
        }
    }

    return launch_game();
}
