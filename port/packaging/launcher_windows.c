// Halo 3 Native PC Port - Windows GUI Launcher & First-Run Setup (Halo3.exe)
//
// Zero-dependency Win32 GUI (/subsystem:windows, x86_64):
//   - Zero black console window when double-clicked or launched from Start Menu / Desktop
//   - Dark Halo 3 branded setup window with drag-and-drop, native ISO/folder pickers,
//     and live progress bar reading `PROGRESS <pct> <msg>` from xiso.exe and shaderpack.exe
//   - Stores all user game data in %LOCALAPPDATA%\halo3 (OS-standard; matches storage_windows.c)
//     and config in %APPDATA%\Halo3\halo3.ini (matches config_windows.c)
//   - Hold Shift while launching (or pass --setup) to open Setup / Maintenance anytime
//   - Full CLI compatibility (--iso, --disc, --res, --scale, --fps, --raw-mouse, --lan, --live, etc.)

#define UNICODE
#define _UNICODE
#define WIN32_LEAN_AND_MEAN
#define COBJMACROS

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "../sdk/kernel/storage.h"

#define IDI_HALO3 101

__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001u;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#define IDC_BTN_ISO     1001
#define IDC_BTN_FOLDER  1002
#define IDC_BTN_PLAY    1003
#define IDC_BTN_REBUILD 1004
#define IDC_BTN_DATA      1005
#define IDC_BTN_CANCEL    1006
#define IDC_BTN_ODST      1007
#define IDC_EDIT_PROFILE  1008
#define IDC_BTN_PROF_MENU 1009
#define IDM_PROF_BASE     2000

#define WM_APP_PROGRESS (WM_APP + 1)
#define WM_APP_DONE     (WM_APP + 2)

#include "../shaders/shader_pack.h"
#include "../shaders/xenos_spirv_abi.h"

#define PATH_CAP 2048
#define CMD_CAP 12288
#define GAMERTAG_MAX 15
#define PROFILE_LIST_MAX 32

// Halo 3 dark color palette
#define COL_BG          RGB(13, 21, 32)
#define COL_CARD        RGB(20, 33, 48)
#define COL_CARD_BORDER RGB(38, 61, 87)
#define COL_ACCENT      RGB(56, 152, 216)
#define COL_ACCENT_HI   RGB(82, 174, 236)
#define COL_ACCENT_DN   RGB(36, 118, 176)
#define COL_BTN         RGB(28, 45, 66)
#define COL_BTN_HI      RGB(38, 60, 88)
#define COL_BTN_DN      RGB(20, 34, 52)
#define COL_AMBER_BTN   RGB(54, 36, 18)
#define COL_AMBER_HI    RGB(77, 51, 24)
#define COL_AMBER_DN    RGB(38, 25, 12)
#define COL_AMBER_BD    RGB(216, 133, 28)
#define COL_AMBER_BD_HI RGB(255, 174, 61)
#define COL_AMBER_TEXT  RGB(255, 226, 176)
#define COL_BTN_DIS     RGB(22, 31, 43)
#define COL_TEXT        RGB(234, 244, 255)
#define COL_TEXT_DIM    RGB(142, 170, 196)
#define COL_TEXT_DIS    RGB(86, 106, 128)
#define COL_BAR_BG      RGB(10, 17, 26)
#define COL_ERR         RGB(255, 112, 112)
#define COL_OK          RGB(94, 227, 158)

typedef struct {
    wchar_t here_dir[PATH_CAP];
    wchar_t bin_dir[PATH_CAP];
    wchar_t data_dir[PATH_CAP];
    wchar_t iso_path[PATH_CAP];
    wchar_t disc_path[PATH_CAP];
    wchar_t saved_disc_path[PATH_CAP];
    wchar_t odst_dlc_src[PATH_CAP];
    wchar_t shader_pack[PATH_CAP];
    wchar_t local_version[128];
    wchar_t shader_abi[64];
    char    profile_name[GAMERTAG_MAX + 1];
    char    profiles[PROFILE_LIST_MAX][GAMERTAG_MAX + 1];
    int     profile_count;
    int     profile_from_cli;
    int     skip_launcher;
    int     custom_shader_pack;
    int     reextract;
    int     rebuild_shaders;
    int     force_setup;
    int     headless;
    int     do_update; // 0 = auto, 1 = force, -1 = no
    int     jobs;
} LauncherConfig;

typedef struct {
    HWND     hwnd;
    HFONT    font_title;
    HFONT    font_sub;
    HFONT    font_body;
    HFONT    font_bold;
    HFONT    font_small;
    HICON    icon_big;
    HICON    icon_small;
    HBRUSH   brush_input;
    UINT     dpi;

    HWND     btn_iso;
    HWND     btn_folder;
    HWND     btn_play;
    HWND     btn_rebuild;
    HWND     btn_data;
    HWND     btn_odst;
    HWND     btn_cancel;
    HWND     edit_profile;
    HWND     btn_prof_menu;
    HWND     hover_btn;

    int      busy;
    int      publishing;
    int      odst_only_job;
    int      ready_to_play;
    int      dlc_loaded;
    int      launch_after_ui;
    int      has_error;
    int      progress_pct;
    int      chk_hover;
    wchar_t  stage_text[256];
    wchar_t  detail_text[512];
    wchar_t  profile_err[128];

    CRITICAL_SECTION lock;
    HANDLE   worker_thread;
    HANDLE   active_child;
    volatile LONG cancel_requested;
} UiState;

static LauncherConfig g_cfg;
static UiState        g_ui;

static int s_px(int v) {
    return MulDiv(v, (int)(g_ui.dpi ? g_ui.dpi : 96), 96);
}

static void trim_ws(wchar_t *s) {
    size_t n = wcslen(s);
    while (n > 0 && (s[n - 1] == L'\r' || s[n - 1] == L'\n' || s[n - 1] == L' ' || s[n - 1] == L'\t')) {
        s[--n] = 0;
    }
}

static int file_exists(const wchar_t *path) {
    DWORD a = GetFileAttributesW(path);
    return (a != INVALID_FILE_ATTRIBUTES) && !(a & FILE_ATTRIBUTE_DIRECTORY);
}

static int dir_exists(const wchar_t *path) {
    DWORD a = GetFileAttributesW(path);
    return (a != INVALID_FILE_ATTRIBUTES) && (a & FILE_ATTRIBUTE_DIRECTORY);
}

static int join_path(wchar_t *out, size_t cap, const wchar_t *a, const wchar_t *b) {
    size_t la = wcslen(a), lb = wcslen(b);
    size_t sep = la > 0 && a[la - 1] != L'\\' && a[la - 1] != L'/';
    if (la + sep + lb >= cap) {
        out[0] = 0;
        return 0;
    }
    wmemcpy(out, a, la);
    if (sep) out[la++] = L'\\';
    wmemcpy(out + la, b, lb + 1);
    return 1;
}

// Keep the output terminated with CRTs that return -1 on truncation.
static int format_wide(wchar_t *out, size_t cap, const wchar_t *fmt, ...) {
    if (!out || !cap) return 0;
    out[0] = 0;
    va_list ap;
    va_start(ap, fmt);
    int n = _vsnwprintf(out, cap, fmt, ap);
    va_end(ap);
    out[cap - 1] = 0;
    return n >= 0 && (size_t)n < cap;
}

static int absolute_path(wchar_t out[PATH_CAP], const wchar_t *path) {
    wchar_t full[PATH_CAP];
    DWORD n = GetFullPathNameW(path, PATH_CAP, full, NULL);
    if (!n || n >= PATH_CAP) return 0;
    wcscpy(out, full);
    return 1;
}

// The game uses narrow Win32 storage APIs. Check the handoff before staging files.
static int game_path_supported(const wchar_t *path, size_t suffix_length) {
    char narrow[PORT_STORAGE_PATH];
    wchar_t wide[PORT_STORAGE_PATH];
    int n = WideCharToMultiByte(CP_ACP, 0, path, -1, narrow, sizeof(narrow), NULL, NULL);
    return n > 0 && (size_t)n + suffix_length <= sizeof(narrow) &&
           MultiByteToWideChar(CP_ACP, 0, narrow, n, wide, PORT_STORAGE_PATH) &&
           wcscmp(path, wide) == 0;
}

static void ensure_dir_recursive(const wchar_t *path) {
    wchar_t tmp[PATH_CAP];
    wcsncpy(tmp, path, PATH_CAP - 1);
    tmp[PATH_CAP - 1] = 0;
    for (wchar_t *p = tmp; *p; ++p) {
        if (*p == L'/') *p = L'\\';
    }
    size_t len = wcslen(tmp);
    while (len > 3 && tmp[len - 1] == L'\\') tmp[--len] = 0;
    for (wchar_t *p = tmp + 1; *p; ++p) {
        if (*p == L'\\') {
            if (p > tmp && *(p - 1) == L':') continue;
            *p = 0;
            CreateDirectoryW(tmp, NULL);
            *p = L'\\';
        }
    }
    CreateDirectoryW(tmp, NULL);
}

static void read_small_text_file(const wchar_t *path, wchar_t *out, size_t cap) {
    out[0] = 0;
    HANDLE h = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) return;
    char buf[256];
    DWORD n = 0;
    if (ReadFile(h, buf, sizeof(buf) - 1, &n, NULL) && n > 0) {
        buf[n] = 0;
        if (n < sizeof(buf) - 1 && MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, buf, -1, out, (int)cap))
            trim_ws(out);
        else
            out[0] = 0;
    }
    CloseHandle(h);
}

static int write_small_text_file(const wchar_t *path, const wchar_t *text) {
    char utf8[256];
    int n = WideCharToMultiByte(CP_UTF8, 0, text, -1, utf8, sizeof(utf8), NULL, NULL);
    if (!n) return 0;
    utf8[n - 1] = '\n';
    FILE *f = _wfopen(path, L"wb");
    if (!f) return 0;
    int ok = fwrite(utf8, 1, (size_t)n, f) == (size_t)n;
    return fclose(f) == 0 && ok;
}

static int is_valid_disc_dir(const wchar_t *dir) {
    if (!dir || !dir[0] || !dir_exists(dir)) return 0;
    wchar_t xex[PATH_CAP], mm[PATH_CAP];
    join_path(xex, PATH_CAP, dir, L"default.xex");
    join_path(mm, PATH_CAP, dir, L"maps\\mainmenu.map");
    return file_exists(xex) && file_exists(mm);
}

static int is_odst_dlc_dir(const wchar_t *dir) {
    if (!dir || !dir[0] || !dir_exists(dir)) return 0;
    wchar_t m1[PATH_CAP], m2[PATH_CAP], m3[PATH_CAP], intro[PATH_CAP];
    join_path(m1, PATH_CAP, dir, L"maps\\midship.map");
    join_path(m2, PATH_CAP, dir, L"maps\\armory.map");
    join_path(m3, PATH_CAP, dir, L"maps\\sandbox.map");
    join_path(intro, PATH_CAP, dir, L"maps\\005_intro.map");
    return file_exists(m1) && file_exists(m2) && file_exists(m3) && !file_exists(intro);
}

static int xdvdfs_find_entry(FILE *fp, uint64_t part_base, uint32_t dir_sector, uint32_t dir_size,
                             const char *target, uint32_t *out_sec, uint32_t *out_sz, uint8_t *out_attr) {
    if (dir_size == 0 || dir_size > (16u << 20)) return 0;
    uint8_t *buf = (uint8_t *)malloc(dir_size);
    if (!buf) return 0;
    if (_fseeki64(fp, (int64_t)(part_base + (uint64_t)dir_sector * 2048ull), SEEK_SET) != 0 ||
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
        if (_stricmp(ename, target) == 0) {
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
static int is_odst_dlc_iso(const wchar_t *iso_path) {
    if (!iso_path || !iso_path[0] || !file_exists(iso_path)) return 0;
    FILE *fp = _wfopen(iso_path, L"rb");
    if (!fp) return 0;
    static const uint64_t bases[] = { 0x00000000ull, 0x0FD90000ull, 0x02080000ull, 0x18300000ull };
    uint64_t part_base = 0;
    uint32_t root_sec = 0, root_sz = 0;
    int valid_xiso = 0;
    uint8_t hdr[32];
    for (size_t i = 0; i < sizeof(bases) / sizeof(bases[0]); ++i) {
        if (_fseeki64(fp, (int64_t)(bases[i] + 32ull * 2048ull), SEEK_SET) != 0) continue;
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
    wchar_t base[PATH_CAP], env_dlc[PATH_CAP] = {0};
    if (GetEnvironmentVariableW(L"HALO3_DLC", env_dlc, PATH_CAP) > 0 && env_dlc[0]) {
        join_path(base, PATH_CAP, env_dlc, L"odst_mythic");
    } else {
        join_path(base, PATH_CAP, g_cfg.data_dir, L"dlc\\odst_mythic");
    }
    wchar_t m1[PATH_CAP], m2[PATH_CAP], m3[PATH_CAP];
    join_path(m1, PATH_CAP, base, L"midship.map");
    join_path(m2, PATH_CAP, base, L"armory.map");
    join_path(m3, PATH_CAP, base, L"sandbox.map");
    return file_exists(m1) && file_exists(m2) && file_exists(m3);
}

static uint64_t gamertag_xuid(const char *tag) {
    uint64_t h = 0xcbf29ce484222325ull;
    for (const char *p = tag; *p; ++p) h = (h ^ (unsigned char)*p) * 0x100000001b3ull;
    return 0xE000000000000000ull | (h & 0x0000ffffffffffffull) | 2ull;
}

static int gamertag_ieq(const char *a, const char *b) {
    while (*a && *b) {
        char ca = (*a >= 'A' && *a <= 'Z') ? (char)(*a + 32) : *a;
        char cb = (*b >= 'A' && *b <= 'Z') ? (char)(*b + 32) : *b;
        if (ca != cb) return 0;
        a++; b++;
    }
    return *a == *b;
}

static int gamertag_valid(const char *tag) {
    if (!tag || !((tag[0] >= 'a' && tag[0] <= 'z') || (tag[0] >= 'A' && tag[0] <= 'Z'))) return 0;
    size_t n = 0;
    for (; tag[n]; ++n) {
        char c = tag[n];
        int alnum = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
        if (n >= GAMERTAG_MAX || !(alnum || (c == ' ' && tag[n - 1] != ' '))) return 0;
    }
    return n > 0 && tag[n - 1] != ' ';
}

static void trim_ascii(char *s) {
    size_t n = strlen(s);
    while (n > 0 && (s[n - 1] == '\r' || s[n - 1] == '\n' || s[n - 1] == ' ' || s[n - 1] == '\t')) s[--n] = 0;
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

    wchar_t ini_path[PATH_CAP];
    join_path(ini_path, PATH_CAP, g_cfg.data_dir, L"launcher.ini");
    FILE *f = _wfopen(ini_path, L"rb");
    if (f) {
        char line[PATH_CAP * 4 + 16];
        while (fgets(line, sizeof(line), f)) {
            // Strip only the record terminator. The disc path may end in spaces.
            line[strcspn(line, "\r\n")] = 0;
            if (strncmp(line, "profile=", 8) == 0) {
                snprintf(saved_profile, sizeof(saved_profile), "%s", line + 8);
                trim_ascii(saved_profile);
            } else if (strncmp(line, "skip_launcher=", 14) == 0) {
                g_cfg.skip_launcher = (atoi(line + 14) != 0);
            } else if (strncmp(line, "disc_path=", 10) == 0) {
                wchar_t path[PATH_CAP];
                if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, line + 10, -1, path, PATH_CAP))
                    wcscpy(g_cfg.saved_disc_path, path);
            }
        }
        fclose(f);
    }

    wchar_t cat_path[PATH_CAP];
    join_path(cat_path, PATH_CAP, g_cfg.data_dir, L"profiles\\catalog.txt");
    char catalog_tmp[PROFILE_LIST_MAX][GAMERTAG_MAX + 1];
    int catalog_n = 0;
    FILE *cf = _wfopen(cat_path, L"rb");
    if (cf) {
        char line[128];
        if (fgets(line, sizeof(line), cf) && strncmp(line, "H3PROFILES 1", 12) == 0) {
            while (fgets(line, sizeof(line), cf) && catalog_n < PROFILE_LIST_MAX) {
                trim_ascii(line);
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

    if (!g_cfg.profile_from_cli) {
        wchar_t env_w[64] = {0};
        char env_a[64] = {0};
        if (GetEnvironmentVariableW(L"HALO3_PROFILE_NAME", env_w, 64) == 0) {
            GetEnvironmentVariableW(L"HALO3_GAMERTAG", env_w, 64);
        }
        if (env_w[0]) WideCharToMultiByte(CP_UTF8, 0, env_w, -1, env_a, sizeof(env_a), NULL, NULL);
        if (env_a[0] && gamertag_valid(env_a)) {
            snprintf(g_cfg.profile_name, sizeof(g_cfg.profile_name), "%s", env_a);
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

static int write_launcher_prefs(const wchar_t *path, const wchar_t *disc) {
    FILE *f = _wfopen(path, L"wb");
    if (!f) return 0;
    int ok = fprintf(f, "profile=%s\nskip_launcher=%d\n",
            g_cfg.profile_name[0] ? g_cfg.profile_name : "Player 1",
            g_cfg.skip_launcher ? 1 : 0) >= 0;
    if (disc[0]) {
        char utf8[PATH_CAP * 4];
        ok = ok && WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, disc, -1,
                                       utf8, sizeof(utf8), NULL, NULL) &&
             fprintf(f, "disc_path=%s\n", utf8) >= 0;
    }
    return fclose(f) == 0 && ok;
}

static void save_launcher_prefs(void) {
    ensure_dir_recursive(g_cfg.data_dir);
    wchar_t ini_path[PATH_CAP], tmp[PATH_CAP];
    if (!join_path(ini_path, PATH_CAP, g_cfg.data_dir, L"launcher.ini") ||
        !format_wide(tmp, PATH_CAP, L"%ls.%lu.tmp", ini_path, GetCurrentProcessId()))
        return;
    if (!write_launcher_prefs(tmp, g_cfg.saved_disc_path) ||
        !MoveFileExW(tmp, ini_path, MOVEFILE_REPLACE_EXISTING))
        DeleteFileW(tmp);
}

static int commit_selected_profile(wchar_t *err_out, size_t err_cap) {
    if (g_ui.edit_profile) {
        wchar_t wbuf[64] = {0};
        GetWindowTextW(g_ui.edit_profile, wbuf, 63);
        char abuf[64] = {0};
        WideCharToMultiByte(CP_UTF8, 0, wbuf, -1, abuf, sizeof(abuf), NULL, NULL);
        snprintf(g_cfg.profile_name, sizeof(g_cfg.profile_name), "%s", abuf);
    }

    char cleaned[GAMERTAG_MAX + 1];
    const char *p = g_cfg.profile_name;
    while (*p == ' ') p++;
    snprintf(cleaned, sizeof(cleaned), "%s", p);
    trim_ascii(cleaned);
    if (!cleaned[0]) snprintf(cleaned, sizeof(cleaned), "Player 1");

    if (!gamertag_valid(cleaned)) {
        if (err_out) format_wide(err_out, err_cap, L"Start with a letter (1-15 chars)");
        return 0;
    }
    snprintf(g_cfg.profile_name, sizeof(g_cfg.profile_name), "%s", cleaned);
    add_profile_unique(cleaned);
    save_launcher_prefs();

    if (!gamertag_ieq(cleaned, "Player 1") && !gamertag_ieq(cleaned, "Player 2") &&
        !gamertag_ieq(cleaned, "Player 3") && !gamertag_ieq(cleaned, "Player 4")) {
        wchar_t prof_dir[PATH_CAP], cat_path[PATH_CAP];
        join_path(prof_dir, PATH_CAP, g_cfg.data_dir, L"profiles");
        ensure_dir_recursive(prof_dir);
        join_path(cat_path, PATH_CAP, prof_dir, L"catalog.txt");

        int exists = 0, has_header = 0, entry_count = 0;
        char xuid_hex[17];
        snprintf(xuid_hex, sizeof(xuid_hex), "%016llX", (unsigned long long)gamertag_xuid(cleaned));
        FILE *rf = _wfopen(cat_path, L"rb");
        if (rf) {
            char line[128];
            if (fgets(line, sizeof(line), rf) && strncmp(line, "H3PROFILES 1", 12) == 0) {
                has_header = 1;
                while (fgets(line, sizeof(line), rf)) {
                    trim_ascii(line);
                    if (strlen(line) >= 18 && line[16] == ' ') {
                        entry_count++;
                        if (gamertag_ieq(line + 17, cleaned) || _strnicmp(line, xuid_hex, 16) == 0) {
                            exists = 1;
                        }
                    }
                }
            }
            fclose(rf);
        }
        // Engine catalog_parse() allows at most PROFILE_CREATED_MAX (12) created entries
        if (!exists && entry_count < 12) {
            FILE *wf = _wfopen(cat_path, has_header ? L"ab" : L"wb");
            if (wf) {
                if (!has_header) fprintf(wf, "H3PROFILES 1\n");
                fprintf(wf, "%s %s\n", xuid_hex, cleaned);
                fclose(wf);
            }
        }
    }
    return 1;
}

static int file_is_newer(const wchar_t *a, const wchar_t *b) {
    WIN32_FILE_ATTRIBUTE_DATA fa, fb;
    if (!GetFileAttributesExW(a, GetFileExInfoStandard, &fa)) return 0;
    if (!GetFileAttributesExW(b, GetFileExInfoStandard, &fb)) return 1;
    return CompareFileTime(&fa.ftLastWriteTime, &fb.ftLastWriteTime) > 0;
}

static void parent_dir(const wchar_t *path, wchar_t *out, size_t cap) {
    wcsncpy(out, path, cap - 1);
    out[cap - 1] = 0;
    wchar_t *slash = wcsrchr(out, L'\\');
    wchar_t *fslash = wcsrchr(out, L'/');
    if (fslash && (!slash || fslash > slash)) slash = fslash;
    if (slash == out || (slash == out + 2 && out[1] == L':')) slash[1] = 0;
    else if (slash) *slash = 0;
    else out[0] = 0;
}

static int normalize_disc_dir(wchar_t out[PATH_CAP], const wchar_t *path) {
    wchar_t candidate[PATH_CAP];
    if (!absolute_path(candidate, path)) return 0;
    size_t len = wcslen(candidate);
    while (len > 3 && (candidate[len - 1] == L'\\' || candidate[len - 1] == L'/')) candidate[--len] = 0;
    if (!is_valid_disc_dir(candidate)) {
        const wchar_t *name = wcsrchr(candidate, L'\\');
        if (!name || _wcsicmp(name + 1, L"maps") != 0) return 0;
        wchar_t parent[PATH_CAP];
        parent_dir(candidate, parent, PATH_CAP);
        if (!is_valid_disc_dir(parent)) return 0;
        wcscpy(candidate, parent);
    }
    wcscpy(out, candidate);
    return 1;
}

static void write_console_or_stdout(const char *utf8) {
    DWORD len = (DWORD)strlen(utf8);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h && h != INVALID_HANDLE_VALUE) {
        DWORD mode = 0, written = 0;
        if (GetConsoleMode(h, &mode)) {
            WriteConsoleA(h, utf8, len, &written, NULL);
            return;
        }
        if (WriteFile(h, utf8, len, &written, NULL) && written > 0) {
            return;
        }
    }
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        h = GetStdHandle(STD_OUTPUT_HANDLE);
        if (h && h != INVALID_HANDLE_VALUE) {
            DWORD written = 0;
            WriteFile(h, utf8, len, &written, NULL);
        }
    }
}

static void print_version(void) {
    char ver[128], abi[64], buf[512];
    WideCharToMultiByte(CP_UTF8, 0, g_cfg.local_version, -1, ver, sizeof(ver), NULL, NULL);
    WideCharToMultiByte(CP_UTF8, 0, g_cfg.shader_abi, -1, abi, sizeof(abi), NULL, NULL);
    snprintf(buf, sizeof(buf), "Halo 3 PC Port %s (Windows, shader ABI %s)\r\n",
             ver[0] ? ver : "unknown", abi[0] ? abi : "unknown");
    write_console_or_stdout(buf);
}

static void print_help(void) {
    static const char *help =
        "Halo 3 - Native 64-bit PC Port (Windows)\r\n"
        "\r\n"
        "Usage:\r\n"
        "  Halo3.exe [C:\\path\\to\\Halo3.iso | C:\\path\\to\\halo3-disc] [options]\r\n"
        "\r\n"
        "Double-click Halo3.exe for guided first-time setup (select your retail Halo 3\r\n"
        "Xbox 360 .iso and play). Hold Shift while launching or pass --setup to reopen Setup.\r\n"
        "\r\n"
        "Disc & Setup:\r\n"
        "  --setup                 Open the Setup & Maintenance window\r\n"
        "  --iso PATH              Retail Halo 3 Xbox 360 .iso (XGD2/Redump/raw XISO)\r\n"
        "  --disc PATH             Extracted Halo 3 disc directory\r\n"
        "  --dlc PATH              DLC map packs directory (STFS LIVE/PIRS/CON)\r\n"
        "  --data PATH             Save/cache directory (default: %LOCALAPPDATA%\\halo3)\r\n"
        "  --config PATH           Path to halo3.ini config file\r\n"
#ifndef HALO3_RELEASE_BUILD
        "  --shader-pack PATH      Override Pack v3 shader pack path\r\n"
#endif
        "  --reextract             Re-extract ISO even if cached disc exists\r\n"
        "  --rebuild-shaders       Re-translate Pack v3 shaders\r\n"
        "  -j, --jobs N            Shader translation threads\r\n"
        "\r\n"
        "Graphics, Input & Multiplayer:\r\n"
        "  --res WxH               Window resolution (e.g. 1920x1080, 3840x2160)\r\n"
        "  --window-mode MODE      windowed, borderless, fullscreen\r\n"
        "  --renderer NAME         auto, vulkan, d3d12\r\n"
#ifndef HALO3_RELEASE_BUILD
        "  --scale 1|2|3           Internal render scale (1=720p, 2=1440p, 3=4K)\r\n"
#endif
        "  --fps N                 Target frame rate (30, 60, 120, 240, or 0 uncapped)\r\n"
#ifndef HALO3_RELEASE_BUILD
        "  --interpolate 0|1       Toggle high-Hz frame interpolation\r\n"
        "  --no-interpolate        Shorthand for --interpolate 0\r\n"
#endif
        "  --vsync 0|1             Toggle VSync\r\n"
#ifndef HALO3_RELEASE_BUILD
        "  --gamma N               Display gamma curve (0..3)\r\n"
        "  --gpu N                 Select Vulkan GPU index\r\n"
        "  --hd-text MODE          HD UI text mode (0=off, 1=native-resolution font, sdf=Guide SDF)\r\n"
        "  --raw-mouse             Enable raw mouse-look\r\n"
        "  --mouse-capture         Capture cursor to window\r\n"
#else
        "  --hd-text 0|1           Enable or disable HD UI text (default: 1)\r\n"
#endif
        "  --mouse-sens FLOAT      Mouse sensitivity multiplier\r\n"
#ifndef HALO3_RELEASE_BUILD
        "  --players 1..4          Local split-screen players\r\n"
        "  --guests 0..3           Local guest players\r\n"
        "  --coop-4p               Allow 4-player local Campaign co-op\r\n"
#endif
        "  --gamertag NAME         Local profile / SlopBoxLive gamertag\r\n"
#ifndef HALO3_RELEASE_BUILD
        "  --audio-backend NAME    Audio backend\r\n"
        "  --audio-channels 2|6    Audio channels\r\n"
        "  --no-audio              Disable audio output\r\n"
#endif
        "  --voice / --no-voice    Enable or disable microphone voice chat\r\n"
#ifndef HALO3_RELEASE_BUILD
        "  --lan                   Enable System Link LAN over UDP\r\n"
        "  --lan-port PORT         System Link UDP port\r\n"
        "  --lan-addr IP           System Link bind address\r\n"
        "  --lan-slots N           Multi-instance LAN slot count\r\n"
        "  --lan-peer IP[:PORT]    Additional LAN peer\r\n"
#endif
        "  --live URL              SlopBoxLive server URL\r\n"
#ifndef HALO3_RELEASE_BUILD
        "  --live-account PATH     Xbox Live account token JSON path\r\n"
        "  --map NAME              Quick-boot directly into a map\r\n"
        "  --frame-stats N         Print frame timing stats every N frames\r\n"
#endif
        "  --update / --no-update  Force or skip automatic update check\r\n"
        "  --version               Print bundled version and exit\r\n";
    write_console_or_stdout(help);
}

static int valid_pe(const wchar_t *path) {
    if (!file_exists(path)) return 0;
    FILE *f = _wfopen(path, L"rb");
    if (!f) return 0;
    uint8_t header[64], signature[4];
    int valid = 0;
    if (fread(header, 1, sizeof(header), f) == sizeof(header) && header[0] == 'M' && header[1] == 'Z') {
        uint32_t offset = (uint32_t)header[60] | ((uint32_t)header[61] << 8) |
                          ((uint32_t)header[62] << 16) | ((uint32_t)header[63] << 24);
        valid = _fseeki64(f, offset, SEEK_SET) == 0 &&
                fread(signature, 1, sizeof(signature), f) == sizeof(signature) &&
                memcmp(signature, "PE\0\0", sizeof(signature)) == 0;
    }
    fclose(f);
    return valid;
}

// Check the pack header, file length and table checksum before offering Play.
static int shader_pack_abi(const wchar_t *path) {
    if (!file_exists(path)) return 0;
    FILE *f = _wfopen(path, L"rb");
    if (!f) return 0;

    union {
        struct s_shader_pack_file_header v2;
        struct s_shader_pack_file_header_v3 v3;
    } header;
    int abi = 0;
    if (fread(&header, 1, sizeof(header), f) != sizeof(header) ||
        header.v3.magic != SHADER_PACK_MAGIC ||
        _fseeki64(f, 0, SEEK_END) != 0 ||
        _ftelli64(f) != (int64_t)header.v3.file_size)
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
        _fseeki64(f, header_size, SEEK_SET) != 0)
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
    wchar_t cached[PATH_CAP];
    if (join_path(cached, PATH_CAP, g_cfg.data_dir, L"disc") && normalize_disc_dir(g_cfg.disc_path, cached))
        return 1;
    if (join_path(cached, PATH_CAP, g_cfg.here_dir, L"disc") && normalize_disc_dir(g_cfg.disc_path, cached))
        return 1;
    return 0;
}

static const wchar_t *external_disc_path(const wchar_t *disc) {
    wchar_t cached[PATH_CAP];
    join_path(cached, PATH_CAP, g_cfg.data_dir, L"disc");
    return _wcsicmp(disc, cached) == 0 ? L"" : disc;
}

static int managed_pack_current(void) {
    if (shader_pack_abi(g_cfg.shader_pack) != XE_ABI_VERSION) return 0;
    wchar_t pack_dir[PATH_CAP], sidecar[PATH_CAP], text[64];
    parent_dir(g_cfg.shader_pack, pack_dir, PATH_CAP);
    if (g_cfg.shader_abi[0]) {
        join_path(sidecar, PATH_CAP, pack_dir, L"abi.txt");
        read_small_text_file(sidecar, text, 64);
        if (wcscmp(text, g_cfg.shader_abi) != 0) return 0;
    }
    if (check_odst_dlc_installed()) {
        join_path(sidecar, PATH_CAP, pack_dir, L"dlc.txt");
        read_small_text_file(sidecar, text, 64);
        if (wcscmp(text, L"1") != 0) return 0;
    }
    return 1;
}

static int check_setup_ready(wchar_t *reason_out, size_t reason_cap) {
    if (reason_out && reason_cap) reason_out[0] = 0;
    if (g_cfg.iso_path[0]) {
        if (reason_out) format_wide(reason_out, reason_cap, g_cfg.reextract ?
                                   L"Re-extracting the selected Halo 3 ISO..." : L"Preparing the selected Halo 3 ISO...");
        return 0;
    }
    if (!select_setup_disc()) {
        if (reason_out) format_wide(reason_out, reason_cap, L"Select your retail Halo 3 ISO or extracted disc folder.");
        return 0;
    }
    if (!game_path_supported(g_cfg.disc_path, sizeof("\\maps\\mainmenu.map") - 1)) {
        if (reason_out) format_wide(reason_out, reason_cap, L"The disc path is too long or uses characters the game cannot open.");
        return 0;
    }
    if (g_cfg.custom_shader_pack && !shader_pack_abi(g_cfg.shader_pack)) {
        if (reason_out) format_wide(reason_out, reason_cap, L"The specified Vulkan shader pack is missing, corrupt, or incompatible.");
        return 0;
    }

    wchar_t pe[PATH_CAP], xex[PATH_CAP];
    join_path(pe, PATH_CAP, g_cfg.data_dir, L"default.pe");
    join_path(xex, PATH_CAP, g_cfg.disc_path, L"default.xex");
    if (!valid_pe(pe) || file_is_newer(xex, pe) ||
        _wcsicmp(external_disc_path(g_cfg.disc_path), g_cfg.saved_disc_path) != 0) {
        if (reason_out) format_wide(reason_out, reason_cap, L"Updating retail executable data...");
        return 0;
    }
    if (!g_cfg.custom_shader_pack && (g_cfg.rebuild_shaders || !managed_pack_current())) {
        if (reason_out) format_wide(reason_out, reason_cap, L"The managed Vulkan shader pack needs rebuilding.");
        return 0;
    }
    return 1;
}

static void ui_set_status(int pct, int err, const wchar_t *stage, const wchar_t *detail) {
    EnterCriticalSection(&g_ui.lock);
    if (pct >= 0) g_ui.progress_pct = pct;
    g_ui.has_error = err;
    if (stage) {
        wcsncpy(g_ui.stage_text, stage, 255);
        g_ui.stage_text[255] = 0;
    }
    if (detail) {
        wcsncpy(g_ui.detail_text, detail, 511);
        g_ui.detail_text[511] = 0;
    }
    LeaveCriticalSection(&g_ui.lock);
    if (g_ui.hwnd) {
        PostMessageW(g_ui.hwnd, WM_APP_PROGRESS, 0, 0);
    }
}

// Encode arguments using the Windows CRT backslash and quote rules.
static int build_command_line(wchar_t *out, size_t cap, const wchar_t *const argv[]) {
    if (!cap) return 0;
    out[0] = 0;
    size_t used = 0;
    for (size_t arg = 0; argv[arg]; ++arg) {
        if (cap - used < (arg ? 4u : 3u)) return 0;
        if (arg) out[used++] = L' ';
        out[used++] = L'"';
        const wchar_t *p = argv[arg];
        for (;;) {
            size_t slashes = 0;
            while (*p == L'\\') {
                ++slashes;
                ++p;
            }
            if (*p == L'"' || !*p) slashes *= 2;
            if (*p == L'"') ++slashes;
            if (slashes + (*p ? 3u : 2u) > cap - used) return 0;
            while (slashes > 0) {
                out[used++] = L'\\';
                --slashes;
            }
            if (!*p) break;
            out[used++] = *p++;
        }
        out[used++] = L'"';
    }
    out[used] = 0;
    return 1;
}

// Run a child tool with CREATE_NO_WINDOW, piping stdout/stderr to setup.log and parsing
// `PROGRESS <0..100> <message>` lines into [pct_start .. pct_end].
static int run_tool_with_progress(const wchar_t *const argv[], HANDLE log_file,
                                  int pct_start, int pct_end, const wchar_t *stage_label,
                                  wchar_t *last_err, size_t last_err_cap) {
    if (InterlockedCompareExchange(&g_ui.cancel_requested, 0, 0)) return -2;

    wchar_t cmd[CMD_CAP];
    if (!build_command_line(cmd, CMD_CAP, argv)) {
        format_wide(last_err, last_err_cap, L"The helper command line is too long.");
        return -1;
    }

    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
    HANDLE read_pipe = NULL, write_pipe = NULL;
    if (!CreatePipe(&read_pipe, &write_pipe, &sa, 0)) return -1;
    SetHandleInformation(read_pipe, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = write_pipe;
    si.hStdError = write_pipe;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    DWORD flags = CREATE_NO_WINDOW;
    if (!CreateProcessW(argv[0], cmd, NULL, NULL, TRUE, flags, NULL, g_cfg.data_dir, &si, &pi)) {
        DWORD error = GetLastError();
        CloseHandle(read_pipe);
        CloseHandle(write_pipe);
        format_wide(last_err, last_err_cap, L"Failed to launch helper tool (error %lu).", error);
        return -1;
    }
    CloseHandle(write_pipe);
    CloseHandle(pi.hThread);

    EnterCriticalSection(&g_ui.lock);
    g_ui.active_child = pi.hProcess;
    if (InterlockedCompareExchange(&g_ui.cancel_requested, 0, 0)) TerminateProcess(pi.hProcess, 1);
    LeaveCriticalSection(&g_ui.lock);

    char chunk[1024];
    char line[1024];
    size_t line_len = 0;
    DWORD nread = 0;
    while (ReadFile(read_pipe, chunk, sizeof(chunk), &nread, NULL) && nread > 0) {
        if (log_file && log_file != INVALID_HANDLE_VALUE) {
            DWORD written = 0;
            WriteFile(log_file, chunk, nread, &written, NULL);
        }
        if (g_cfg.headless) {
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut && hOut != INVALID_HANDLE_VALUE) {
                DWORD w = 0;
                WriteFile(hOut, chunk, nread, &w, NULL);
            }
        }
        for (DWORD i = 0; i < nread; ++i) {
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
                    wchar_t wmsg[512] = {0};
                    MultiByteToWideChar(CP_UTF8, 0, msg, -1, wmsg, 511);
                    ui_set_status(mapped, 0, stage_label, wmsg);
                } else if (line[0] == '[') {
                    int cur = 0, tot = 0;
                    if (sscanf(line, "[%d/%d]", &cur, &tot) == 2 && tot > 0) {
                        int p = (cur * 92) / tot;
                        int mapped = pct_start + (p * (pct_end - pct_start)) / 100;
                        wchar_t wline[512] = {0};
                        MultiByteToWideChar(CP_UTF8, 0, line, -1, wline, 511);
                        ui_set_status(mapped, 0, stage_label, wline);
                    } else if (strncmp(line, "[ SPA ]", 7) == 0) {
                        int mapped = pct_start + (96 * (pct_end - pct_start)) / 100;
                        ui_set_status(mapped, 0, stage_label, L"Extracting 79 Halo 3 achievements & icons (1,750 G)...");
                    }
                } else if (line_len > 0) {
                    wchar_t wline[512] = {0};
                    MultiByteToWideChar(CP_UTF8, 0, line, -1, wline, 511);
                    wcsncpy(last_err, wline, last_err_cap - 1);
                    last_err[last_err_cap - 1] = 0;
                }
                line_len = 0;
            } else if (line_len < sizeof(line) - 1) {
                line[line_len++] = c;
            }
        }
    }
    CloseHandle(read_pipe);

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exit_code = 1;
    GetExitCodeProcess(pi.hProcess, &exit_code);

    EnterCriticalSection(&g_ui.lock);
    g_ui.active_child = NULL;
    LeaveCriticalSection(&g_ui.lock);
    CloseHandle(pi.hProcess);

    if (InterlockedCompareExchange(&g_ui.cancel_requested, 0, 0)) return -2;
    return (int)exit_code;
}

typedef struct {
    wchar_t path[PATH_CAP];
    wchar_t tmp[PATH_CAP];
    wchar_t backup[PATH_CAP];
    int had_previous;
} SetupFile;

static int remove_setup_file(const wchar_t *path) {
    if (DeleteFileW(path)) return 1;
    DWORD error = GetLastError();
    return error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
}

static int prepare_setup_file(SetupFile *file, const wchar_t *dir, const wchar_t *name) {
    if (!join_path(file->path, PATH_CAP, dir, name)) return 0;
    if (!format_wide(file->tmp, PATH_CAP, L"%ls.tmp", file->path) ||
        !format_wide(file->backup, PATH_CAP, L"%ls.bak", file->path)) {
        file->tmp[0] = 0;
        return 0;
    }
    return remove_setup_file(file->tmp);
}

static int move_setup_path(const wchar_t *from, const wchar_t *to) {
    return MoveFileExW(from, to, MOVEFILE_REPLACE_EXISTING);
}

static int sync_setup_file(const wchar_t *path) {
    HANDLE file = CreateFileW(path, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) return 0;
    BOOL ok = FlushFileBuffers(file);
    return CloseHandle(file) && ok;
}

// Only used on extracted staging trees and retired caches. Never traverse links.
static void remove_disc_tree(const wchar_t *path, unsigned depth) {
    if (depth == 0) return;
    DWORD attr = GetFileAttributesW(path);
    if (attr == INVALID_FILE_ATTRIBUTES) return;
    if (!(attr & FILE_ATTRIBUTE_DIRECTORY)) {
        DeleteFileW(path);
        return;
    }
    if (!(attr & FILE_ATTRIBUTE_REPARSE_POINT)) {
        wchar_t child[PATH_CAP];
        WIN32_FIND_DATAW entry;
        if (!join_path(child, PATH_CAP, path, L"*")) return;
        HANDLE find = FindFirstFileW(child, &entry);
        if (find != INVALID_HANDLE_VALUE) {
            do {
                if (wcscmp(entry.cFileName, L".") == 0 || wcscmp(entry.cFileName, L"..") == 0) continue;
                if (join_path(child, PATH_CAP, path, entry.cFileName)) remove_disc_tree(child, depth - 1);
            } while (FindNextFileW(find, &entry));
            FindClose(find);
        }
    }
    RemoveDirectoryW(path);
}

static int setup_cancelled(void) {
    return InterlockedCompareExchange(&g_ui.cancel_requested, 0, 0) != 0;
}

static int start_publishing(void) {
    EnterCriticalSection(&g_ui.lock);
    int publish = !setup_cancelled();
    g_ui.publishing = publish;
    LeaveCriticalSection(&g_ui.lock);
    return publish;
}

// Keep backups until every replacement has been published.
static int publish_setup(SetupFile files[], size_t count, const wchar_t *disc, const wchar_t *staged_disc,
                         const wchar_t *disc_backup) {
    size_t published = 0;
    int saved_disc = 0, moved_disc = 0, ok = 0;
    for (size_t i = 0; i < count; ++i) {
        SetupFile *file = &files[i];
        if (!sync_setup_file(file->tmp)) goto finish;
        DWORD attr = GetFileAttributesW(file->path);
        if (attr == INVALID_FILE_ATTRIBUTES) {
            DWORD error = GetLastError();
            if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND) goto finish;
            continue;
        }
        if (!CreateHardLinkW(file->backup, file->path, NULL) && !CopyFileW(file->path, file->backup, TRUE)) {
            DWORD error = GetLastError();
            if (error != ERROR_FILE_EXISTS && error != ERROR_ALREADY_EXISTS) remove_setup_file(file->backup);
            goto finish;
        }
        file->had_previous = 1;
    }
    if (!start_publishing()) goto finish;
    if (staged_disc[0]) {
        if (GetFileAttributesW(disc_backup) != INVALID_FILE_ATTRIBUTES) goto finish;
        if (GetFileAttributesW(disc) != INVALID_FILE_ATTRIBUTES) {
            if (!MoveFileW(disc, disc_backup)) goto finish;
            saved_disc = 1;
        }
        if (!MoveFileW(staged_disc, disc)) goto finish;
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

static DWORD WINAPI setup_worker_proc(LPVOID param) {
    (void)param;
    SetupFile files[5] = {0};
    size_t count = 0;
    wchar_t cached_disc[PATH_CAP], staged_disc[PATH_CAP] = {0}, disc_backup[PATH_CAP] = {0};
    wchar_t pack_manifest[PATH_CAP] = {0}, last_err[512] = {0};
    int ok = 0, rc = 0;
    HANDLE lock = INVALID_HANDLE_VALUE, log_file = INVALID_HANDLE_VALUE;

    ensure_dir_recursive(g_cfg.data_dir);
    wchar_t lock_path[PATH_CAP], log_path[PATH_CAP];
    if (!join_path(lock_path, PATH_CAP, g_cfg.data_dir, L".setup.lock") ||
        !join_path(log_path, PATH_CAP, g_cfg.data_dir, L"setup.log") ||
        !join_path(cached_disc, PATH_CAP, g_cfg.data_dir, L"disc"))
        goto staging_failed;
    lock = CreateFileW(lock_path, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (lock == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        if (error != ERROR_SHARING_VIOLATION) goto staging_failed;
        ui_set_status(0, 1, L"Halo 3 Setup Is Busy", L"Another launcher is preparing this data folder.");
        goto finish;
    }
    log_file = CreateFileW(log_path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                          CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (g_cfg.custom_shader_pack && !shader_pack_abi(g_cfg.shader_pack)) {
        ui_set_status(0, 1, L"Invalid Vulkan Shader Pack",
                      L"The specified pack is missing, corrupt, or incompatible. Provide a valid pack or remove the override.");
        goto finish;
    }

    int has_base = select_setup_disc();
    int need_extract = g_cfg.iso_path[0] != 0;
    if (has_base && !need_extract && !game_path_supported(g_cfg.disc_path, sizeof("\\maps\\mainmenu.map") - 1)) {
        ui_set_status(0, 1, L"Halo 3 Disc Path Error",
                      L"Choose a shorter disc path with characters supported by the Windows system code page.");
        goto finish;
    }
    int did_odst = 0;
    wchar_t dlc_root[PATH_CAP] = {0};
    DWORD dlc_len = GetEnvironmentVariableW(L"HALO3_DLC", dlc_root, PATH_CAP);
    if (dlc_len >= PATH_CAP) goto staging_failed;
    if (!dlc_len && !join_path(dlc_root, PATH_CAP, g_cfg.data_dir, L"dlc")) goto staging_failed;

    if (g_cfg.odst_dlc_src[0]) {
        wchar_t dlc_out[PATH_CAP], odst_tool[PATH_CAP];
        if (!join_path(dlc_out, PATH_CAP, dlc_root, L"odst_mythic") ||
            !join_path(odst_tool, PATH_CAP, g_cfg.bin_dir, L"extract-odst-dlc.exe"))
            goto staging_failed;
        ensure_dir_recursive(dlc_out);
        int odst_hi = (has_base || need_extract) ? 38 : 99;
        const wchar_t *label = L"Extracting Halo 3: ODST Disc 2 DLC";
        ui_set_status(2, 0, label, L"Reading Mythic disc maps and achievements...");
        const wchar_t *argv[] = { odst_tool, g_cfg.odst_dlc_src, dlc_out, NULL };
        rc = run_tool_with_progress(argv, log_file, 2, odst_hi, label, last_err, 512);
        if (setup_cancelled()) goto cancelled;
        if (rc != 0) {
            ui_set_status(0, 1, L"ODST DLC Extraction Failed",
                          last_err[0] ? last_err : L"Select a retail Halo 3: ODST Disc 2 (Halo 3: Mythic) ISO.");
            goto finish;
        }
        g_cfg.odst_dlc_src[0] = 0;
        did_odst = 1;
        if (!need_extract && !has_base && g_ui.odst_only_job) {
            ui_set_status(0, 0, L"First-Time Setup", L"ODST Disc 2 DLC loaded. Select your Halo 3 ISO next.");
            ok = 1;
            goto finish;
        }
    }

    const wchar_t *disc = g_cfg.disc_path;
    if (need_extract) {
        if (!file_exists(g_cfg.iso_path)) {
            ui_set_status(0, 1, L"Halo 3 ISO Unavailable", L"Choose an ISO file the launcher can read.");
            goto finish;
        }
        wchar_t xiso[PATH_CAP], disc_tmp[PATH_CAP];
        if (!join_path(xiso, PATH_CAP, g_cfg.bin_dir, L"xiso.exe") ||
            !join_path(disc_tmp, PATH_CAP, g_cfg.data_dir, L"disc.tmp") ||
            !join_path(disc_backup, PATH_CAP, g_cfg.data_dir, L"disc.bak"))
            goto staging_failed;
        remove_disc_tree(disc_tmp, 32);
        if (!CreateDirectoryW(disc_tmp, NULL)) goto staging_failed;
        wcscpy(staged_disc, disc_tmp);
        disc = staged_disc;
        const wchar_t *label = L"Step 1 of 2 - Extracting Halo 3 ISO";
        ui_set_status(did_odst ? 38 : 1, 0, label, L"Scanning Xbox 360 disc image...");
        const wchar_t *argv[] = { xiso, L"extract", L"--progress", g_cfg.iso_path, disc, NULL };
        last_err[0] = 0;
        rc = run_tool_with_progress(argv, log_file, did_odst ? 38 : 0, 55, label, last_err, 512);
        if (setup_cancelled()) goto cancelled;
        if (rc != 0 || !is_valid_disc_dir(disc)) {
            ui_set_status(0, 1, L"ISO Extraction Failed",
                          rc != 0 && last_err[0] ? last_err : L"Select a retail Halo 3 Xbox 360 ISO with default.xex and maps/mainmenu.map.");
            goto finish;
        }
    } else if (!has_base) {
        ui_set_status(0, 1, L"Invalid Halo 3 Disc",
                      L"Select an extracted disc folder with default.xex and maps/mainmenu.map.");
        goto finish;
    }

    const wchar_t *external = need_extract ? L"" : external_disc_path(disc);
    int disc_changed = _wcsicmp(external, g_cfg.saved_disc_path) != 0;
    wchar_t xex[PATH_CAP], active_pe[PATH_CAP];
    if (!join_path(xex, PATH_CAP, disc, L"default.xex") ||
        !join_path(active_pe, PATH_CAP, g_cfg.data_dir, L"default.pe"))
        goto staging_failed;
    const wchar_t *pe = active_pe;
    int need_pe = need_extract || disc_changed || !valid_pe(pe) || file_is_newer(xex, pe);
    if (need_pe) {
        wchar_t xex2pe[PATH_CAP];
        SetupFile *output = &files[count++];
        if (!join_path(xex2pe, PATH_CAP, g_cfg.bin_dir, L"xex2pe.exe") ||
            !prepare_setup_file(output, g_cfg.data_dir, L"default.pe"))
            goto staging_failed;
        pe = output->tmp;
        const wchar_t *label = L"Unpacking default.xex";
        int unpack_start = need_extract ? 56 : (did_odst ? 38 : 2);
        ui_set_status(unpack_start, 0, label, L"Preparing the retail executable...");
        const wchar_t *argv[] = { xex2pe, xex, pe, NULL };
        last_err[0] = 0;
        rc = run_tool_with_progress(argv, log_file, unpack_start, unpack_start + 2, label, last_err, 512);
        if (setup_cancelled()) goto cancelled;
        if (rc != 0 || !valid_pe(pe)) {
            ui_set_status(0, 1, L"XEX Unpack Failed",
                          rc != 0 && last_err[0] ? last_err : L"Failed to unpack a valid retail executable. See setup.log for details.");
            goto finish;
        }
    }

    if (!g_cfg.custom_shader_pack && (need_pe || did_odst || g_cfg.rebuild_shaders || !managed_pack_current())) {
        wchar_t pack_dir[PATH_CAP], maps[PATH_CAP], shaderpack[PATH_CAP], jobs[32], abi[8];
        parent_dir(g_cfg.shader_pack, pack_dir, PATH_CAP);
        ensure_dir_recursive(pack_dir);
        SetupFile *output = &files[count++];
        if (!join_path(maps, PATH_CAP, disc, L"maps") ||
            !join_path(shaderpack, PATH_CAP, g_cfg.bin_dir, L"shaderpack.exe") ||
            !prepare_setup_file(output, pack_dir, L"halo3_shaders.pack") ||
            !format_wide(jobs, 32, L"%d", g_cfg.jobs) ||
            !format_wide(abi, 8, L"%d", XE_ABI_VERSION))
            goto staging_failed;
        if (!format_wide(pack_manifest, PATH_CAP, L"%ls.tsv", output->tmp)) {
            pack_manifest[0] = 0;
            goto staging_failed;
        }
        if (!remove_setup_file(pack_manifest)) goto staging_failed;
        const wchar_t *argv[17] = { shaderpack, L"--xenos", L"--xenos-abi", abi, L"--all", L"--progress",
                                 L"--maps-dir", maps, L"--executable", pe, L"--out", output->tmp, L"-j", jobs };
        if (dir_exists(dlc_root)) {
            argv[14] = L"--dlc-dir";
            argv[15] = dlc_root;
        }
        const wchar_t *label = L"Compiling Vulkan Shaders";
        int shader_start = need_extract ? 58 : (did_odst ? 40 : 5);
        ui_set_status(shader_start, 0, label, L"Reading Halo 3 and DLC map files...");
        last_err[0] = 0;
        rc = run_tool_with_progress(argv, log_file, shader_start, 99, label, last_err, 512);
        if (setup_cancelled()) goto cancelled;
        if (rc != 0 || shader_pack_abi(output->tmp) != XE_ABI_VERSION) {
            ui_set_status(0, 1, L"Shader Compilation Failed",
                          rc != 0 && last_err[0] ? last_err : L"The helper did not produce a valid ABI 5 shader pack. See setup.log for details.");
            goto finish;
        }
        if (g_cfg.shader_abi[0]) {
            SetupFile *stamp = &files[count++];
            if (!prepare_setup_file(stamp, pack_dir, L"abi.txt") ||
                !write_small_text_file(stamp->tmp, g_cfg.shader_abi))
                goto staging_failed;
        }
        SetupFile *dlc = &files[count++];
        if (!prepare_setup_file(dlc, pack_dir, L"dlc.txt") ||
            !write_small_text_file(dlc->tmp, check_odst_dlc_installed() ? L"1" : L"0"))
            goto staging_failed;
    }

    if (disc_changed) {
        SetupFile *prefs = &files[count++];
        if (!prepare_setup_file(prefs, g_cfg.data_dir, L"launcher.ini") ||
            !write_launcher_prefs(prefs->tmp, external))
            goto staging_failed;
    }
    if (!publish_setup(files, count, cached_disc, staged_disc, disc_backup)) goto staging_failed;
    if (pack_manifest[0]) {
        wchar_t manifest[PATH_CAP];
        if (format_wide(manifest, PATH_CAP, L"%ls.tsv", g_cfg.shader_pack))
            move_setup_path(pack_manifest, manifest);
    }
    if (need_extract) {
        wcscpy(g_cfg.disc_path, cached_disc);
        g_cfg.iso_path[0] = 0;
        g_cfg.reextract = 0;
    }
    if (disc_changed) wcscpy(g_cfg.saved_disc_path, external);
    g_cfg.rebuild_shaders = 0;
    ui_set_status(0, 0, L"Ready to Play",
                  did_odst ? L"ODST Disc 2 DLC and Vulkan shaders ready." : L"Retail disc and Vulkan shaders ready.");
    ok = 1;
    goto finish;

staging_failed:
    if (!setup_cancelled()) {
        ui_set_status(0, 1, L"Setup Could Not Save Files",
                      L"Check available space, data-folder permissions, and path length, then try again.");
        goto finish;
    }
cancelled:
    ui_set_status(0, 0, L"Setup cancelled.", L"Select your Halo 3 ISO or disc folder to try again.");

finish:
    g_ui.odst_only_job = 0;
    for (size_t i = 0; i < count; ++i) {
        if (files[i].tmp[0]) remove_setup_file(files[i].tmp);
    }
    if (pack_manifest[0]) remove_setup_file(pack_manifest);
    if (staged_disc[0]) remove_disc_tree(staged_disc, 32);
    if (g_cfg.headless && !ok) fwprintf(stderr, L"%ls: %ls\n", g_ui.stage_text, g_ui.detail_text);
    if (log_file != INVALID_HANDLE_VALUE) CloseHandle(log_file);
    if (lock != INVALID_HANDLE_VALUE) CloseHandle(lock);
    if (g_ui.hwnd) PostMessageW(g_ui.hwnd, WM_APP_DONE, (WPARAM)ok, 0);
    return ok ? 0 : 1;
}

static void update_button_states(void) {
    if (!g_ui.hwnd) return;
    BOOL idle = !g_ui.busy;
    BOOL ready_idle = g_ui.ready_to_play && idle;

    int pad = s_px(28), bw = s_px(142), bh = s_px(36), gap = s_px(12), y1 = s_px(324);
    int c0 = pad;
    int c1 = pad + (bw + gap);
    int c2 = pad + (bw + gap) * 2;
    int c3 = pad + (bw + gap) * 3;

    if (g_ui.btn_play) {
        MoveWindow(g_ui.btn_play, c0, y1, bw, bh, TRUE);
        ShowWindow(g_ui.btn_play, ready_idle ? SW_SHOW : SW_HIDE);
        EnableWindow(g_ui.btn_play, ready_idle);
    }
    if (g_ui.btn_iso) {
        SetWindowTextW(g_ui.btn_iso, ready_idle ? L"Change .ISO..." : L"Select .ISO...");
        MoveWindow(g_ui.btn_iso, ready_idle ? c1 : c0, y1, bw, bh, TRUE);
        EnableWindow(g_ui.btn_iso, idle);
        InvalidateRect(g_ui.btn_iso, NULL, FALSE);
    }
    if (g_ui.btn_folder) {
        MoveWindow(g_ui.btn_folder, ready_idle ? c2 : c1, y1, bw, bh, TRUE);
        EnableWindow(g_ui.btn_folder, idle);
        InvalidateRect(g_ui.btn_folder, NULL, FALSE);
    }
    if (g_ui.btn_rebuild) {
        MoveWindow(g_ui.btn_rebuild, c3, y1, bw, bh, TRUE);
        BOOL rebuild = idle && !g_cfg.custom_shader_pack && !g_cfg.iso_path[0] && is_valid_disc_dir(g_cfg.disc_path);
        ShowWindow(g_ui.btn_rebuild, rebuild ? SW_SHOW : SW_HIDE);
        EnableWindow(g_ui.btn_rebuild, rebuild);
    }
    if (g_ui.btn_odst) EnableWindow(g_ui.btn_odst, idle);
    if (g_ui.edit_profile) ShowWindow(g_ui.edit_profile, idle ? SW_SHOW : SW_HIDE);
    if (g_ui.btn_prof_menu) {
        ShowWindow(g_ui.btn_prof_menu, idle ? SW_SHOW : SW_HIDE);
        EnableWindow(g_ui.btn_prof_menu, idle && g_cfg.profile_count > 1);
    }
    SetWindowTextW(g_ui.btn_cancel, g_ui.busy ? L"Cancel" : L"Close");
    InvalidateRect(g_ui.hwnd, NULL, FALSE);
}

static void start_setup_worker(void) {
    if (g_ui.busy) return;
    InterlockedExchange(&g_ui.cancel_requested, 0);
    g_ui.publishing = 0;
    g_ui.ready_to_play = 0;
    g_ui.busy = 1;
    g_ui.has_error = 0;
    g_ui.progress_pct = 0;
    update_button_states();
    g_ui.worker_thread = CreateThread(NULL, 0, setup_worker_proc, NULL, 0, NULL);
    if (!g_ui.worker_thread) {
        g_ui.busy = 0;
        ui_set_status(0, 1, L"Setup Could Not Start", L"Try selecting your Halo 3 media again.");
        update_button_states();
    }
}

static void pick_iso_file(HWND hwnd) {
    wchar_t path[PATH_CAP] = {0};
    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Xbox 360 ISO Images (*.iso)\0*.iso\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = PATH_CAP;
    ofn.lpstrTitle = L"Select your retail Halo 3 Xbox 360 .ISO";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
    if (GetOpenFileNameW(&ofn)) {
        if (is_odst_dlc_iso(path)) {
            wcsncpy(g_cfg.odst_dlc_src, path, PATH_CAP - 1);
            g_ui.odst_only_job = 1;
        } else {
            wcsncpy(g_cfg.iso_path, path, PATH_CAP - 1);
            g_cfg.disc_path[0] = 0;
            g_cfg.reextract = 1;
            g_ui.odst_only_job = 0;
        }
        start_setup_worker();
    }
}

static void pick_odst_dlc_file(HWND hwnd) {
    wchar_t path[PATH_CAP] = {0};
    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = L"Xbox 360 ISO Images (*.iso)\0*.iso\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = path;
    ofn.nMaxFile = PATH_CAP;
    ofn.lpstrTitle = L"Select Halo 3: ODST Disc 2 (Halo 3: Mythic) .ISO";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
    if (GetOpenFileNameW(&ofn)) {
        wcsncpy(g_cfg.odst_dlc_src, path, PATH_CAP - 1);
        g_ui.odst_only_job = 1;
        start_setup_worker();
    }
}

static void pick_disc_folder(HWND hwnd) {
    BROWSEINFOW bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.hwndOwner = hwnd;
    bi.lpszTitle = L"Select your extracted retail Halo 3 disc folder (containing default.xex and maps\\)";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;
    PIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi);
    if (pidl) {
        wchar_t path[PATH_CAP] = {0};
        if (SHGetPathFromIDListW(pidl, path)) {
            if (is_odst_dlc_dir(path)) {
                wcsncpy(g_cfg.odst_dlc_src, path, PATH_CAP - 1);
                g_ui.odst_only_job = 1;
                start_setup_worker();
            } else if (!normalize_disc_dir(g_cfg.disc_path, path)) {
                ui_set_status(0, 1, L"Not a Halo 3 Disc Folder",
                              L"The selected folder does not contain default.xex and maps\\mainmenu.map.");
            } else {
                g_cfg.iso_path[0] = 0;
                g_cfg.reextract = 0;
                g_ui.odst_only_job = 0;
                start_setup_worker();
            }
        }
        CoTaskMemFree(pidl);
    }
}

static void draw_round_rect(HDC hdc, RECT rc, int radius, COLORREF fill, COLORREF border) {
    HBRUSH br = CreateSolidBrush(fill);
    HPEN pen = CreatePen(PS_SOLID, 1, border);
    HGDIOBJ old_br = SelectObject(hdc, br);
    HGDIOBJ old_pen = SelectObject(hdc, pen);
    RoundRect(hdc, rc.left, rc.top, rc.right, rc.bottom, radius, radius);
    SelectObject(hdc, old_br);
    SelectObject(hdc, old_pen);
    DeleteObject(br);
    DeleteObject(pen);
}

static void draw_owner_button(const DRAWITEMSTRUCT *dis) {
    HDC hdc = dis->hDC;
    RECT rc = dis->rcItem;
    int id = (int)dis->CtlID;
    int primary = (id == IDC_BTN_ISO && !g_ui.ready_to_play) || (id == IDC_BTN_PLAY);
    int disabled = (dis->itemState & ODS_DISABLED) != 0;
    int pressed = (dis->itemState & ODS_SELECTED) != 0;
    int hovered = (dis->hwndItem == g_ui.hover_btn);

    COLORREF bg, border, fg;
    if (disabled) {
        bg = COL_BTN_DIS;
        border = RGB(30, 44, 62);
        fg = COL_TEXT_DIS;
    } else if (primary) {
        bg = pressed ? COL_ACCENT_DN : (hovered ? COL_ACCENT_HI : COL_ACCENT);
        border = COL_ACCENT_HI;
        fg = RGB(255, 255, 255);
    } else if (id == IDC_BTN_ODST) {
        bg = pressed ? COL_AMBER_DN : (hovered ? COL_AMBER_HI : COL_AMBER_BTN);
        border = hovered ? COL_AMBER_BD_HI : COL_AMBER_BD;
        fg = COL_AMBER_TEXT;
    } else {
        bg = pressed ? COL_BTN_DN : (hovered ? COL_BTN_HI : COL_BTN);
        border = hovered ? COL_ACCENT : COL_CARD_BORDER;
        fg = COL_TEXT;
    }

    // Fill behind rounded corners with card/window background
    HBRUSH bg_br = CreateSolidBrush(id == IDC_BTN_PROF_MENU ? COL_CARD : COL_BG);
    FillRect(hdc, &rc, bg_br);
    DeleteObject(bg_br);

    draw_round_rect(hdc, rc, s_px(6), bg, border);

    if (id == IDC_BTN_PROF_MENU) {
        int cx = (rc.left + rc.right) / 2;
        int cy = (rc.top + rc.bottom) / 2;
        int hw = s_px(5);
        int hh = s_px(3);
        POINT tri[3] = {
            { cx - hw, cy - hh + 1 },
            { cx + hw, cy - hh + 1 },
            { cx,      cy + hh + 1 }
        };
        HBRUSH tbr = CreateSolidBrush(fg);
        HPEN tpen = CreatePen(PS_SOLID, 1, fg);
        HGDIOBJ ob = SelectObject(hdc, tbr);
        HGDIOBJ op = SelectObject(hdc, tpen);
        Polygon(hdc, tri, 3);
        SelectObject(hdc, ob);
        SelectObject(hdc, op);
        DeleteObject(tbr);
        DeleteObject(tpen);
        return;
    }

    wchar_t text[128] = {0};
    GetWindowTextW(dis->hwndItem, text, 127);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, fg);
    HGDIOBJ old_font = SelectObject(hdc, g_ui.font_body);
    DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, old_font);
}

static void paint_window(HWND hwnd) {
    PAINTSTRUCT ps;
    HDC real_dc = BeginPaint(hwnd, &ps);
    RECT cr;
    GetClientRect(hwnd, &cr);

    HDC hdc = CreateCompatibleDC(real_dc);
    HBITMAP bmp = CreateCompatibleBitmap(real_dc, cr.right, cr.bottom);
    HGDIOBJ old_bmp = SelectObject(hdc, bmp);

    // Background
    HBRUSH bg_br = CreateSolidBrush(COL_BG);
    FillRect(hdc, &cr, bg_br);
    DeleteObject(bg_br);

    // Top accent bar
    RECT top_bar = { 0, 0, cr.right, s_px(4) };
    HBRUSH acc_br = CreateSolidBrush(COL_ACCENT);
    FillRect(hdc, &top_bar, acc_br);
    DeleteObject(acc_br);

    // Header: 48x48 Halo 3 icon at y=23..71 (center y=47), title+subtitle centered on y=47
    int pad = s_px(28);
    int icon_sz = s_px(48);
    if (g_ui.icon_big) {
        DrawIconEx(hdc, pad, s_px(23), g_ui.icon_big, icon_sz, icon_sz, 0, NULL, DI_NORMAL);
    }

    SetBkMode(hdc, TRANSPARENT);
    int tx = pad + icon_sz + s_px(16);
    RECT title_rc = { tx, s_px(24), cr.right - pad, s_px(52) };
    SetTextColor(hdc, COL_TEXT);
    HGDIOBJ old_font = SelectObject(hdc, g_ui.font_title);
    DrawTextW(hdc, L"HALO 3", -1, &title_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    wchar_t sub[256];
    format_wide(sub, 256, L"Native 64-bit PC Port - %ls",
               g_cfg.local_version[0] ? g_cfg.local_version : L"Windows");
    RECT sub_rc = { tx, s_px(52), cr.right - pad, s_px(70) };
    SetTextColor(hdc, COL_TEXT_DIM);
    SelectObject(hdc, g_ui.font_sub);
    DrawTextW(hdc, sub, -1, &sub_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    // Info / Progress card (expanded to house Profile input + Skip launcher checkbox)
    RECT card = { pad, s_px(90), cr.right - pad, s_px(308) };
    draw_round_rect(hdc, card, s_px(10), COL_CARD, COL_CARD_BORDER);

    int cx = card.left + s_px(20);
    int cw = card.right - s_px(20);

    wchar_t stage[256], detail[512];
    int pct, err, busy;
    EnterCriticalSection(&g_ui.lock);
    wcsncpy(stage, g_ui.stage_text, 255); stage[255] = 0;
    wcsncpy(detail, g_ui.detail_text, 511); detail[511] = 0;
    pct = g_ui.progress_pct;
    err = g_ui.has_error;
    busy = g_ui.busy;
    LeaveCriticalSection(&g_ui.lock);

    RECT stage_rc = { cx, card.top + s_px(18), g_ui.dlc_loaded ? (cw - s_px(96)) : cw, card.top + s_px(42) };
    SetTextColor(hdc, err ? COL_ERR : (pct == 100 && !busy ? COL_OK : COL_TEXT));
    SelectObject(hdc, g_ui.font_bold);
    DrawTextW(hdc, stage, -1, &stage_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

    if (g_ui.dlc_loaded) {
        int bw = s_px(84), bh = s_px(22);
        RECT brc = { cw - bw, card.top + s_px(17), cw, card.top + s_px(17) + bh };
        draw_round_rect(hdc, brc, s_px(22), RGB(19, 46, 34), RGB(60, 201, 120));
        SetTextColor(hdc, RGB(124, 232, 168));
        SelectObject(hdc, g_ui.font_small);
        DrawTextW(hdc, L"DLC Loaded", -1, &brc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    RECT detail_rc = { cx, card.top + s_px(46), cw, card.top + s_px(76) };
    SetTextColor(hdc, err ? COL_ERR : COL_TEXT_DIM);
    SelectObject(hdc, g_ui.font_body);
    DrawTextW(hdc, detail, -1, &detail_rc, DT_LEFT | DT_WORDBREAK);

    // Progress bar (shown while busy or finished at 100%)
    if (busy || pct > 0) {
        RECT bar_bg = { cx, card.top + s_px(116), cw, card.top + s_px(138) };
        draw_round_rect(hdc, bar_bg, s_px(6), COL_BAR_BG, COL_CARD_BORDER);
        if (pct > 0) {
            int fill_w = ((bar_bg.right - bar_bg.left - 2) * pct) / 100;
            if (fill_w < s_px(8)) fill_w = s_px(8);
            if (bar_bg.left + 1 + fill_w > bar_bg.right - 1) fill_w = (bar_bg.right - bar_bg.left) - 2;
            RECT bar_fill = { bar_bg.left + 1, bar_bg.top + 1, bar_bg.left + 1 + fill_w, bar_bg.bottom - 1 };
            COLORREF fill_col = (pct >= 100) ? COL_OK : COL_ACCENT;
            draw_round_rect(hdc, bar_fill, s_px(5), fill_col, fill_col);
        }
        wchar_t pct_buf[32];
        format_wide(pct_buf, 32, L"%d%%", pct);
        RECT pct_rc = { cx, card.top + s_px(144), cw, card.top + s_px(164) };
        SetTextColor(hdc, COL_TEXT_DIM);
        SelectObject(hdc, g_ui.font_small);
        DrawTextW(hdc, pct_buf, -1, &pct_rc, DT_RIGHT | DT_SINGLELINE);
    } else {
        // Gamertag label + input border + validation error
        RECT plbl = { cx, card.top + s_px(82), cx + s_px(76), card.top + s_px(100) };
        SetTextColor(hdc, COL_TEXT_DIM);
        SelectObject(hdc, g_ui.font_body);
        DrawTextW(hdc, L"Gamertag", -1, &plbl, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        RECT psub = { cx, card.top + s_px(100), cx + s_px(76), card.top + s_px(116) };
        SetTextColor(hdc, RGB(104, 134, 162));
        SelectObject(hdc, g_ui.font_small);
        DrawTextW(hdc, L"1-15 chars", -1, &psub, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        RECT ibox = { cx + s_px(80), card.top + s_px(82), cx + s_px(324), card.top + s_px(116) };
        draw_round_rect(hdc, ibox, s_px(6), COL_BAR_BG, g_ui.profile_err[0] ? COL_ERR : COL_CARD_BORDER);

        if (g_ui.profile_err[0]) {
            RECT perr = { cx + s_px(372), card.top + s_px(82), cw, card.top + s_px(116) };
            SetTextColor(hdc, COL_ERR);
            SelectObject(hdc, g_ui.font_small);
            DrawTextW(hdc, g_ui.profile_err, -1, &perr, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
        }

        // Concise Data path
        wchar_t path_line[PATH_CAP + 16];
        format_wide(path_line, PATH_CAP + 16, L"Data: %ls", g_cfg.data_dir);
        RECT path_rc = { cx, card.top + s_px(130), cw, card.top + s_px(154) };
        SetTextColor(hdc, RGB(104, 134, 162));
        SelectObject(hdc, g_ui.font_small);
        DrawTextW(hdc, path_line, -1, &path_rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_PATH_ELLIPSIS);

        // Skip launcher checkbox
        int cbx = cx, cby = card.top + s_px(166), cbsz = s_px(18);
        RECT cb_rc = { cbx, cby, cbx + cbsz, cby + cbsz };
        draw_round_rect(hdc, cb_rc, s_px(4),
                        g_cfg.skip_launcher ? COL_ACCENT : COL_BAR_BG,
                        (g_cfg.skip_launcher || g_ui.chk_hover) ? COL_ACCENT_HI : COL_CARD_BORDER);
        if (g_cfg.skip_launcher) {
            HPEN cpen = CreatePen(PS_SOLID, s_px(2) > 1 ? s_px(2) : 2, RGB(255, 255, 255));
            HGDIOBJ op = SelectObject(hdc, cpen);
            MoveToEx(hdc, cbx + s_px(4), cby + s_px(9), NULL);
            LineTo(hdc, cbx + s_px(7), cby + s_px(13));
            LineTo(hdc, cbx + s_px(14), cby + s_px(5));
            SelectObject(hdc, op);
            DeleteObject(cpen);
        }
        RECT clbl = { cbx + cbsz + s_px(8), cby - s_px(2), cw, cby + cbsz + s_px(2) };
        SetTextColor(hdc, g_ui.chk_hover ? COL_TEXT : COL_TEXT_DIM);
        SelectObject(hdc, g_ui.font_body);
        DrawTextW(hdc, L"Skip launcher (hold Shift on launch to re-open)", -1, &clbl, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    }

    SelectObject(hdc, old_font);
    BitBlt(real_dc, 0, 0, cr.right, cr.bottom, hdc, 0, 0, SRCCOPY);
    SelectObject(hdc, old_bmp);
    DeleteObject(bmp);
    DeleteDC(hdc);
    EndPaint(hwnd, &ps);
}

static RECT skip_checkbox_rect(void) {
    RECT r = { s_px(48), s_px(252), s_px(390), s_px(280) };
    return r;
}

static void show_profile_menu(HWND hwnd) {
    if (g_cfg.profile_count <= 1) return;
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    for (int i = 0; i < g_cfg.profile_count; ++i) {
        wchar_t wname[64] = {0};
        MultiByteToWideChar(CP_UTF8, 0, g_cfg.profiles[i], -1, wname, 63);
        UINT flags = MF_STRING | (gamertag_ieq(g_cfg.profiles[i], g_cfg.profile_name) ? MF_CHECKED : 0);
        AppendMenuW(menu, flags, (UINT_PTR)(IDM_PROF_BASE + i), wname);
    }
    RECT br;
    GetWindowRect(g_ui.btn_prof_menu, &br);
    TrackPopupMenu(menu, TPM_LEFTALIGN | TPM_TOPALIGN, br.left - s_px(248), br.bottom + s_px(2), 0, hwnd, NULL);
    DestroyMenu(menu);
}

static LRESULT CALLBACK setup_wnd_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_PAINT:
        paint_window(hwnd);
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_CTLCOLOREDIT: {
        HDC hdc = (HDC)wp;
        SetTextColor(hdc, COL_TEXT);
        SetBkColor(hdc, COL_BAR_BG);
        return (LRESULT)g_ui.brush_input;
    }

    case WM_DRAWITEM:
        draw_owner_button((const DRAWITEMSTRUCT *)lp);
        return TRUE;

    case WM_MOUSEMOVE: {
        POINT pt = { (short)LOWORD(lp), (short)HIWORD(lp) };
        RECT cbr = skip_checkbox_rect();
        int ch = (!g_ui.busy && PtInRect(&cbr, pt)) ? 1 : 0;
        if (ch != g_ui.chk_hover) {
            g_ui.chk_hover = ch;
            InvalidateRect(hwnd, &cbr, FALSE);
        }
        HWND child = ChildWindowFromPoint(hwnd, pt);
        if (child == hwnd) child = NULL;
        if (child != g_ui.hover_btn) {
            HWND prev = g_ui.hover_btn;
            g_ui.hover_btn = child;
            if (prev) InvalidateRect(prev, NULL, FALSE);
            if (child) InvalidateRect(child, NULL, FALSE);
        }
        return 0;
    }

    case WM_LBUTTONDOWN: {
        if (!g_ui.busy) {
            POINT pt = { (short)LOWORD(lp), (short)HIWORD(lp) };
            RECT cbr = skip_checkbox_rect();
            if (PtInRect(&cbr, pt)) {
                g_cfg.skip_launcher = !g_cfg.skip_launcher;
                save_launcher_prefs();
                InvalidateRect(hwnd, &cbr, FALSE);
                return 0;
            }
        }
        break;
    }

    case WM_DROPFILES: {
        if (g_ui.busy) return 0;
        HDROP hdrop = (HDROP)wp;
        wchar_t dropped[PATH_CAP] = {0};
        UINT dropped_len = DragQueryFileW(hdrop, 0, NULL, 0);
        if (dropped_len >= PATH_CAP) {
            ui_set_status(0, 1, L"Halo 3 Path Is Too Long", L"Choose a media path shorter than 2048 characters.");
        } else if (DragQueryFileW(hdrop, 0, dropped, PATH_CAP) > 0) {
            if (dir_exists(dropped)) {
                if (is_odst_dlc_dir(dropped)) {
                    wcsncpy(g_cfg.odst_dlc_src, dropped, PATH_CAP - 1);
                    g_ui.odst_only_job = 1;
                    start_setup_worker();
                } else if (normalize_disc_dir(g_cfg.disc_path, dropped)) {
                    g_cfg.iso_path[0] = 0;
                    g_cfg.reextract = 0;
                    g_ui.odst_only_job = 0;
                    commit_selected_profile(NULL, 0);
                    start_setup_worker();
                } else {
                    ui_set_status(0, 1, L"Not a Halo 3 Disc Folder",
                                  L"Folder must contain default.xex and maps\\mainmenu.map.");
                }
            } else if (file_exists(dropped)) {
                if (is_odst_dlc_iso(dropped)) {
                    wcsncpy(g_cfg.odst_dlc_src, dropped, PATH_CAP - 1);
                    g_ui.odst_only_job = 1;
                    start_setup_worker();
                } else {
                    wcsncpy(g_cfg.iso_path, dropped, PATH_CAP - 1);
                    g_cfg.disc_path[0] = 0;
                    g_cfg.reextract = 1;
                    g_ui.odst_only_job = 0;
                    commit_selected_profile(NULL, 0);
                    start_setup_worker();
                }
            }
        }
        DragFinish(hdrop);
        return 0;
    }

    case WM_COMMAND: {
        int id = LOWORD(wp);
        if (id == IDC_EDIT_PROFILE && HIWORD(wp) == EN_CHANGE) {
            if (g_ui.profile_err[0]) {
                g_ui.profile_err[0] = 0;
                InvalidateRect(hwnd, NULL, FALSE);
            }
            return 0;
        }
        if (id >= IDM_PROF_BASE && id < IDM_PROF_BASE + g_cfg.profile_count) {
            int idx = id - IDM_PROF_BASE;
            snprintf(g_cfg.profile_name, sizeof(g_cfg.profile_name), "%s", g_cfg.profiles[idx]);
            wchar_t wname[64] = {0};
            MultiByteToWideChar(CP_UTF8, 0, g_cfg.profile_name, -1, wname, 63);
            if (g_ui.edit_profile) SetWindowTextW(g_ui.edit_profile, wname);
            g_ui.profile_err[0] = 0;
            save_launcher_prefs();
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
        switch (id) {
        case IDC_BTN_PROF_MENU:
            show_profile_menu(hwnd);
            return 0;
        case IDC_BTN_ISO:
            if (!commit_selected_profile(g_ui.profile_err, 128)) { InvalidateRect(hwnd, NULL, FALSE); return 0; }
            g_ui.profile_err[0] = 0;
            pick_iso_file(hwnd);
            return 0;
        case IDC_BTN_FOLDER:
            if (!commit_selected_profile(g_ui.profile_err, 128)) { InvalidateRect(hwnd, NULL, FALSE); return 0; }
            g_ui.profile_err[0] = 0;
            pick_disc_folder(hwnd);
            return 0;
        case IDC_BTN_PLAY:
            if (!commit_selected_profile(g_ui.profile_err, 128)) { InvalidateRect(hwnd, NULL, FALSE); return 0; }
            g_ui.profile_err[0] = 0;
            g_ui.launch_after_ui = 1;
            DestroyWindow(hwnd);
            return 0;
        case IDC_BTN_REBUILD:
            g_cfg.rebuild_shaders = 1;
            g_ui.odst_only_job = 0;
            start_setup_worker();
            return 0;
        case IDC_BTN_DATA:
            ensure_dir_recursive(g_cfg.data_dir);
            ShellExecuteW(hwnd, L"open", g_cfg.data_dir, NULL, NULL, SW_SHOWNORMAL);
            return 0;
        case IDC_BTN_ODST:
            pick_odst_dlc_file(hwnd);
            return 0;
        case IDC_BTN_CANCEL:
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
            return 0;
        }
        break;
    }

    case WM_APP_PROGRESS:
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_APP_DONE:
        if (g_ui.worker_thread) {
            WaitForSingleObject(g_ui.worker_thread, 2000);
            CloseHandle(g_ui.worker_thread);
            g_ui.worker_thread = NULL;
        }
        g_ui.busy = 0;
        g_ui.dlc_loaded = check_odst_dlc_installed();
        g_ui.ready_to_play = wp != 0 && check_setup_ready(NULL, 0);
        update_button_states();
        return 0;

    case WM_CLOSE:
        if (g_ui.busy) {
            EnterCriticalSection(&g_ui.lock);
            if (!g_ui.publishing) {
                InterlockedExchange(&g_ui.cancel_requested, 1);
                if (g_ui.active_child) TerminateProcess(g_ui.active_child, 1);
            }
            LeaveCriticalSection(&g_ui.lock);
            return 0;
        }
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static WNDPROC g_orig_btn_proc = NULL;

static LRESULT CALLBACK btn_subclass_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_MOUSEMOVE) {
        if (g_ui.hover_btn != hwnd) {
            HWND prev = g_ui.hover_btn;
            g_ui.hover_btn = hwnd;
            if (prev) InvalidateRect(prev, NULL, FALSE);
            InvalidateRect(hwnd, NULL, FALSE);
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
        }
    } else if (msg == WM_MOUSELEAVE) {
        if (g_ui.hover_btn == hwnd) {
            g_ui.hover_btn = NULL;
            InvalidateRect(hwnd, NULL, FALSE);
        }
    }
    return CallWindowProcW(g_orig_btn_proc, hwnd, msg, wp, lp);
}

static HWND make_button(HWND parent, HINSTANCE hInst, int id, const wchar_t *label, int x, int y, int w, int h) {
    HWND b = CreateWindowExW(0, L"BUTTON", label, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                             x, y, w, h, parent, (HMENU)(INT_PTR)id, hInst, NULL);
    if (b) {
        WNDPROC prev = (WNDPROC)SetWindowLongPtrW(b, GWLP_WNDPROC, (LONG_PTR)btn_subclass_proc);
        if (!g_orig_btn_proc) g_orig_btn_proc = prev;
    }
    return b;
}

static int run_setup_gui(HINSTANCE hInst, int auto_start) {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    InitializeCriticalSection(&g_ui.lock);

    // Query system DPI
    HDC sc = GetDC(NULL);
    g_ui.dpi = sc ? (UINT)GetDeviceCaps(sc, LOGPIXELSX) : 96;
    if (sc) ReleaseDC(NULL, sc);

    g_ui.brush_input = CreateSolidBrush(COL_BAR_BG);
    g_ui.icon_big = (HICON)LoadImageW(hInst, MAKEINTRESOURCEW(IDI_HALO3), IMAGE_ICON,
                                      s_px(48), s_px(48), LR_DEFAULTCOLOR);
    g_ui.icon_small = (HICON)LoadImageW(hInst, MAKEINTRESOURCEW(IDI_HALO3), IMAGE_ICON,
                                        s_px(16), s_px(16), LR_DEFAULTCOLOR);

    static const struct { const wchar_t *path, *face; } fonts[] = {
        { L"fonts\\halo3.otf", L"Halo 3 Conduit" },
        { L"fonts\\D-DIN.ttf", L"D-DIN" },
    };
    wchar_t font_path[PATH_CAP];
    const wchar_t *face = NULL;
    for (size_t i = 0; i < sizeof(fonts) / sizeof(fonts[0]); i++) {
        join_path(font_path, PATH_CAP, g_cfg.bin_dir, fonts[i].path);
        if (AddFontResourceExW(font_path, FR_PRIVATE, NULL) > 0) {
            face = fonts[i].face;
            break;
        }
    }
    int has_private_font = face != NULL;
    if (!face) face = L"Segoe UI";

    g_ui.font_title = CreateFontW(-s_px(26), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                                  0, 0, CLEARTYPE_QUALITY, 0, face);
    g_ui.font_sub   = CreateFontW(-s_px(13), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                                  0, 0, CLEARTYPE_QUALITY, 0, face);
    g_ui.font_bold  = CreateFontW(-s_px(16), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                                  0, 0, CLEARTYPE_QUALITY, 0, face);
    g_ui.font_body  = CreateFontW(-s_px(13), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                                  0, 0, CLEARTYPE_QUALITY, 0, face);
    g_ui.font_small = CreateFontW(-s_px(11), 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET,
                                  0, 0, CLEARTYPE_QUALITY, 0, face);

    WNDCLASSEXW wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = setup_wnd_proc;
    wc.hInstance = hInst;
    wc.hIcon = g_ui.icon_big;
    wc.hIconSm = g_ui.icon_small;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.lpszClassName = L"Halo3SetupWindow";
    RegisterClassExW(&wc);

    int cw = s_px(660), ch = s_px(434);
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT wr = { 0, 0, cw, ch };
    AdjustWindowRectEx(&wr, style, FALSE, 0);
    int ww = wr.right - wr.left;
    int wh = wr.bottom - wr.top;
    int sx = (GetSystemMetrics(SM_CXSCREEN) - ww) / 2;
    int sy = (GetSystemMetrics(SM_CYSCREEN) - wh) / 2;

    HWND hwnd = CreateWindowExW(WS_EX_ACCEPTFILES, L"Halo3SetupWindow", L"Halo 3",
                                style, sx, sy, ww, wh, NULL, NULL, hInst, NULL);
    g_ui.hwnd = hwnd;

    // Enable Windows 10/11 dark titlebar if dwmapi.dll is available
    HMODULE dwm = LoadLibraryW(L"dwmapi.dll");
    if (dwm) {
        typedef HRESULT (WINAPI *DwmSetAttrFn)(HWND, DWORD, LPCVOID, DWORD);
        DwmSetAttrFn fn = (DwmSetAttrFn)(void *)GetProcAddress(dwm, "DwmSetWindowAttribute");
        if (fn) {
            BOOL dark = TRUE;
            if (FAILED(fn(hwnd, 20, &dark, sizeof(dark)))) {
                fn(hwnd, 19, &dark, sizeof(dark));
            }
        }
        FreeLibrary(dwm);
    }

    int pad = s_px(28);
    int cx = pad + s_px(20);
    // Gamertag text input + dropdown menu button inside card
    wchar_t wprof[64] = {0};
    MultiByteToWideChar(CP_UTF8, 0, g_cfg.profile_name[0] ? g_cfg.profile_name : "Player 1", -1, wprof, 63);
    g_ui.edit_profile = CreateWindowExW(0, L"EDIT", wprof, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                        cx + s_px(88), s_px(180), s_px(228), s_px(22),
                                        hwnd, (HMENU)IDC_EDIT_PROFILE, hInst, NULL);
    if (g_ui.edit_profile) {
        SendMessageW(g_ui.edit_profile, WM_SETFONT, (WPARAM)g_ui.font_body, TRUE);
        SendMessageW(g_ui.edit_profile, EM_SETLIMITTEXT, GAMERTAG_MAX, 0);
    }
    g_ui.btn_prof_menu = make_button(hwnd, hInst, IDC_BTN_PROF_MENU, L"v",
                                     cx + s_px(328), s_px(172), s_px(34), s_px(34));
    if (g_ui.btn_prof_menu) {
        EnableWindow(g_ui.btn_prof_menu, g_cfg.profile_count > 1);
    }

    int bw = s_px(142), bh = s_px(36), gap = s_px(12);
    int y1 = s_px(324), y2 = s_px(370);
    int c0 = pad;
    int c1 = pad + (bw + gap);
    int c2 = pad + (bw + gap) * 2;
    int c3 = pad + (bw + gap) * 3;

    wchar_t reason[256];
    g_ui.ready_to_play = check_setup_ready(reason, 256);
    g_ui.dlc_loaded = check_odst_dlc_installed();
    if (g_ui.ready_to_play && !auto_start) {
        ui_set_status(0, 0, L"Ready to Play", L"Retail disc and Vulkan shaders cached.");
    } else {
        ui_set_status(0, 0, L"Halo 3 Setup", reason[0] ? reason : L"Preparing Halo 3 data...");
    }

    g_ui.btn_play    = make_button(hwnd, hInst, IDC_BTN_PLAY,    L"Play Halo 3",     c0, y1, bw, bh);
    g_ui.btn_iso     = make_button(hwnd, hInst, IDC_BTN_ISO,     L"Change .ISO...",  c1, y1, bw, bh);
    g_ui.btn_folder  = make_button(hwnd, hInst, IDC_BTN_FOLDER,  L"Disc Folder...",  c2, y1, bw, bh);
    g_ui.btn_rebuild = make_button(hwnd, hInst, IDC_BTN_REBUILD, L"Rebuild Shaders", c3, y1, bw, bh);

    g_ui.btn_data   = make_button(hwnd, hInst, IDC_BTN_DATA,   L"Open Data Folder",    c0, y2, bw, bh);
    g_ui.btn_odst   = make_button(hwnd, hInst, IDC_BTN_ODST,   L"Add ODST Disc 2 DLC", c1, y2, bw, bh);
    g_ui.btn_cancel = make_button(hwnd, hInst, IDC_BTN_CANCEL, L"Close",               c3, y2, bw, bh);

    update_button_states();

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    if (auto_start) {
        start_setup_worker();
    }

    MSG m;
    while (GetMessageW(&m, NULL, 0, 0) > 0) {
        if (m.message == WM_KEYDOWN && m.wParam == VK_RETURN && !g_ui.busy && g_ui.ready_to_play) {
            SendMessageW(hwnd, WM_COMMAND, MAKEWPARAM(IDC_BTN_PLAY, 0), 0);
            continue;
        }
        TranslateMessage(&m);
        DispatchMessageW(&m);
    }

    if (g_ui.worker_thread) {
        WaitForSingleObject(g_ui.worker_thread, INFINITE);
        CloseHandle(g_ui.worker_thread);
    }
    DeleteObject(g_ui.brush_input);
    DeleteObject(g_ui.font_title);
    DeleteObject(g_ui.font_sub);
    DeleteObject(g_ui.font_bold);
    DeleteObject(g_ui.font_body);
    DeleteObject(g_ui.font_small);
    if (has_private_font) RemoveFontResourceExW(font_path, FR_PRIVATE, NULL);
    if (g_ui.icon_big) DestroyIcon(g_ui.icon_big);
    if (g_ui.icon_small) DestroyIcon(g_ui.icon_small);
    DeleteCriticalSection(&g_ui.lock);
    CoUninitialize();
    return g_ui.launch_after_ui;
}

static void maybe_auto_update(void) {
    wchar_t relaunched[2];
    if (GetEnvironmentVariableW(L"HALO3_UPDATE_RELAUNCHED", relaunched, 2) == 1 && relaunched[0] == L'1') {
        SetEnvironmentVariableW(L"HALO3_UPDATE_RELAUNCHED", NULL);
        return;
    }
    if (g_cfg.do_update < 0) return;
    wchar_t env_buf[32] = {0};
    if (g_cfg.do_update == 0 && GetEnvironmentVariableW(L"HALO3_NO_UPDATE", env_buf, 32) > 0 && wcscmp(env_buf, L"1") == 0) {
        return;
    }
    if (g_cfg.headless && g_cfg.do_update == 0) return;

    wchar_t ps1[PATH_CAP];
    join_path(ps1, PATH_CAP, g_cfg.bin_dir, L"update.ps1");
    if (!file_exists(ps1)) return;

    wchar_t system_dir[PATH_CAP], powershell[PATH_CAP];
    UINT system_len = GetSystemDirectoryW(system_dir, PATH_CAP);
    if (!system_len || system_len >= PATH_CAP ||
        !join_path(powershell, PATH_CAP, system_dir, L"WindowsPowerShell\\v1.0\\powershell.exe")) return;

    wchar_t cmd[CMD_CAP];
    const wchar_t *argv[] = {
        powershell, L"-NoProfile", L"-NonInteractive", L"-WindowStyle", L"Hidden", L"-ExecutionPolicy",
        L"Bypass", L"-File", ps1, L"-InstallDir", g_cfg.here_dir, L"-CurrentVersion",
        g_cfg.local_version, g_cfg.do_update > 0 ? L"-Force" : NULL, NULL
    };
    if (!build_command_line(cmd, CMD_CAP, argv)) return;

    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcessW(powershell, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, g_cfg.here_dir, &si, &pi)) {
        return;
    }
    // The script bounds the release check. Wait for any file replacement to finish.
    DWORD code = 0;
    if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_OBJECT_0)
        GetExitCodeProcess(pi.hProcess, &code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (code == 10) {
        wchar_t self[PATH_CAP];
        if (!join_path(self, PATH_CAP, g_cfg.here_dir, L"Halo3.exe")) return;

        // Preserve quoting, relative paths, and every original CLI option.
        const wchar_t *original = GetCommandLineW();
        size_t bytes = (wcslen(original) + 1) * sizeof(*original);
        wchar_t *relaunch = malloc(bytes);
        if (!relaunch) return;
        memcpy(relaunch, original, bytes);
        STARTUPINFOW rsi;
        ZeroMemory(&rsi, sizeof(rsi));
        rsi.cb = sizeof(rsi);
        PROCESS_INFORMATION rpi;
        ZeroMemory(&rpi, sizeof(rpi));
        BOOL started = FALSE;
        if (SetEnvironmentVariableW(L"HALO3_UPDATE_RELAUNCHED", L"1")) {
            started = CreateProcessW(self, relaunch, NULL, NULL, FALSE, 0, NULL, NULL, &rsi, &rpi);
            SetEnvironmentVariableW(L"HALO3_UPDATE_RELAUNCHED", NULL);
        }
        free(relaunch);
        if (started) {
            CloseHandle(rpi.hProcess);
            CloseHandle(rpi.hThread);
            ExitProcess(0);
        }
    }
}

static int launch_game(void) {
    ensure_dir_recursive(g_cfg.data_dir);

    SetEnvironmentVariableW(L"HALO3_DATA", g_cfg.data_dir);
    SetEnvironmentVariableW(L"HALO3_DISC", g_cfg.disc_path);
    SetEnvironmentVariableW(L"HALO3_SHADER_PACK", g_cfg.custom_shader_pack ? g_cfg.shader_pack : NULL);
    if (g_cfg.profile_name[0] && !gamertag_ieq(g_cfg.profile_name, "Player 1")) {
        wchar_t wprof[64] = {0};
        MultiByteToWideChar(CP_UTF8, 0, g_cfg.profile_name, -1, wprof, 63);
        SetEnvironmentVariableW(L"HALO3_PROFILE_NAME", wprof);
        SetEnvironmentVariableW(L"HALO3_GAMERTAG", wprof);
    } else if (!g_cfg.profile_from_cli) {
        SetEnvironmentVariableW(L"HALO3_PROFILE_NAME", NULL);
        SetEnvironmentVariableW(L"HALO3_GAMERTAG", NULL);
    }

    wchar_t tmp[64];
    if (GetEnvironmentVariableW(L"HALO3_VULKAN", tmp, 64) == 0)
        SetEnvironmentVariableW(L"HALO3_VULKAN", L"1");
    if (GetEnvironmentVariableW(L"PORT_STUB_CONTINUE", tmp, 64) == 0)
        SetEnvironmentVariableW(L"PORT_STUB_CONTINUE", L"1");
    SetLastError(ERROR_SUCCESS);
    if (!GetEnvironmentVariableW(L"HALO3_LIVE_ACCOUNT", NULL, 0) && GetLastError() == ERROR_ENVVAR_NOT_FOUND) {
        wchar_t acct[PATH_CAP];
        if (!join_path(acct, PATH_CAP, g_cfg.data_dir, L"live_account.json") ||
            !SetEnvironmentVariableW(L"HALO3_LIVE_ACCOUNT", acct)) {
            MessageBoxW(NULL, L"Could not set the Live account path in the data folder.",
                        L"Halo 3 - Data Path Error", MB_ICONERROR);
            return 1;
        }
    }

    wchar_t game_exe[PATH_CAP];
    join_path(game_exe, PATH_CAP, g_cfg.bin_dir, L"halo3.exe");
    if (!file_exists(game_exe)) {
        MessageBoxW(NULL, L"bin\\halo3.exe was not found next to Halo3.exe.", L"Halo 3 - Missing Binary", MB_ICONERROR);
        return 1;
    }

    wchar_t cmd[CMD_CAP];
    const wchar_t *argv[] = { game_exe, NULL };
    if (!build_command_line(cmd, CMD_CAP, argv)) return 1;

    STARTUPINFOW si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    HANDLE log_file = INVALID_HANDLE_VALUE;
    DWORD flags = 0;

    if (!g_cfg.headless) {
        wchar_t log_path[PATH_CAP];
        join_path(log_path, PATH_CAP, g_cfg.data_dir, L"halo3.log");
        SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
        log_file = CreateFileW(log_path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (log_file != INVALID_HANDLE_VALUE) {
            // No STARTF_USESHOWWINDOW: Windows would apply its wShowWindow to the game's first ShowWindow call
            // and hide the game window. CREATE_NO_WINDOW alone keeps a console from appearing.
            si.dwFlags = STARTF_USESTDHANDLES;
            si.hStdOutput = log_file;
            si.hStdError = log_file;
            si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
        }
        flags = CREATE_NO_WINDOW;
    }

    ULONGLONG t0 = GetTickCount64();
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcessW(game_exe, cmd, NULL, NULL, (log_file != INVALID_HANDLE_VALUE),
                        flags, NULL, g_cfg.here_dir, &si, &pi)) {
        DWORD err = GetLastError();
        if (log_file != INVALID_HANDLE_VALUE) CloseHandle(log_file);
        wchar_t msg[512];
        format_wide(msg, 512, L"Failed to start Halo 3 (Windows error %lu).", err);
        MessageBoxW(NULL, msg, L"Halo 3 - Launch Failed", MB_ICONERROR);
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exit_code = 0;
    GetExitCodeProcess(pi.hProcess, &exit_code);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    if (log_file != INVALID_HANDLE_VALUE) CloseHandle(log_file);

    ULONGLONG elapsed = GetTickCount64() - t0;
    if (!g_cfg.headless && exit_code != 0 && elapsed < 5000) {
        wchar_t msg[PATH_CAP + 256];
        format_wide(msg, PATH_CAP + 256,
                   L"Halo 3 exited immediately (code %lu).\n\n"
                   L"If this is your first launch, ensure your GPU drivers support Vulkan 1.3.\n\n"
                   L"Log file:\n%ls\\halo3.log",
                   exit_code, g_cfg.data_dir);
        MessageBoxW(NULL, msg, L"Halo 3 - Launch Error", MB_ICONERROR);
    }
    return (int)exit_code;
}

static int init_defaults(void) {
    ZeroMemory(&g_cfg, sizeof(g_cfg));
    wchar_t self[PATH_CAP];
    DWORD len = GetModuleFileNameW(NULL, self, PATH_CAP);
    if (!len || len >= PATH_CAP) return 0;
    parent_dir(self, g_cfg.here_dir, PATH_CAP);
    if (!join_path(g_cfg.bin_dir, PATH_CAP, g_cfg.here_dir, L"bin")) return 0;

    wchar_t f[PATH_CAP];
    join_path(f, PATH_CAP, g_cfg.here_dir, L"VERSION");
    read_small_text_file(f, g_cfg.local_version, 128);
    if (!g_cfg.local_version[0]) wcscpy(g_cfg.local_version, L"unknown");

    join_path(f, PATH_CAP, g_cfg.here_dir, L"SHADER_ABI");
    read_small_text_file(f, g_cfg.shader_abi, 64);

    len = GetEnvironmentVariableW(L"HALO3_DATA", g_cfg.data_dir, PATH_CAP);
    if (len >= PATH_CAP) return 0;
    if (!len) {
        wchar_t local_app[PATH_CAP];
        len = GetEnvironmentVariableW(L"LOCALAPPDATA", local_app, PATH_CAP);
        if (len >= PATH_CAP ||
            !join_path(g_cfg.data_dir, PATH_CAP, len ? local_app : g_cfg.here_dir, len ? L"halo3" : L"data"))
            return 0;
    }

    if (GetEnvironmentVariableW(L"HALO3_DISC", g_cfg.disc_path, PATH_CAP) >= PATH_CAP) return 0;
    len = GetEnvironmentVariableW(L"HALO3_SHADER_PACK", g_cfg.shader_pack, PATH_CAP);
    if (len >= PATH_CAP) return 0;
    g_cfg.custom_shader_pack = len != 0;

    wchar_t win_env[16] = {0};
    if (GetEnvironmentVariableW(L"HALO3_WINDOW", win_env, 16) > 0 && wcscmp(win_env, L"0") == 0) {
        g_cfg.headless = 1;
    }

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    g_cfg.jobs = (int)(si.dwNumberOfProcessors ? si.dwNumberOfProcessors : 8);
    return 1;
}

static int parse_args(int argc, wchar_t **argv) {
    for (int i = 1; i < argc; ++i) {
        const wchar_t *a = argv[i];
        const wchar_t *v = (i + 1 < argc) ? argv[i + 1] : NULL;
        if (_wcsicmp(a, L"--help") == 0 || _wcsicmp(a, L"-h") == 0) {
            print_help();
            return 100;
        } else if (_wcsicmp(a, L"--version") == 0) {
            print_version();
            return 100;
        } else if (_wcsicmp(a, L"--setup") == 0) {
            g_cfg.force_setup = 1;
        } else if (_wcsicmp(a, L"--iso") == 0 && v) {
            if (!absolute_path(g_cfg.iso_path, v)) goto path_error;
            i++;
        } else if (_wcsicmp(a, L"--disc") == 0 && v) {
            if (!absolute_path(g_cfg.disc_path, v)) goto path_error;
            i++;
        } else if (_wcsicmp(a, L"--odst-dlc") == 0 && v) {
            if (!absolute_path(g_cfg.odst_dlc_src, v)) goto path_error;
            i++;
        } else if (_wcsicmp(a, L"--data") == 0 && v) {
            if (!absolute_path(g_cfg.data_dir, v)) goto path_error;
            i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (_wcsicmp(a, L"--shader-pack") == 0 && v) {
            if (!absolute_path(g_cfg.shader_pack, v)) goto path_error;
            g_cfg.custom_shader_pack = 1;
            i++;
#endif
        } else if (_wcsicmp(a, L"--dlc") == 0 && v) {
            if (!SetEnvironmentVariableW(L"HALO3_DLC", v)) goto path_error;
            i++;
        } else if (_wcsicmp(a, L"--config") == 0 && v) {
            if (!SetEnvironmentVariableW(L"HALO3_CONFIG", v)) goto path_error;
            i++;
        } else if (_wcsicmp(a, L"--res") == 0 && v) { SetEnvironmentVariableW(L"HALO3_RES", v); i++;
        } else if (_wcsicmp(a, L"--window-mode") == 0 && v) { SetEnvironmentVariableW(L"HALO3_WINDOW_MODE", v); i++;
        } else if (_wcsicmp(a, L"--renderer") == 0 && v) { SetEnvironmentVariableW(L"HALO3_RENDERER", v); i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (_wcsicmp(a, L"--scale") == 0 && v) { SetEnvironmentVariableW(L"HALO3_RES_SCALE", v); i++;
#endif
        } else if (_wcsicmp(a, L"--fps") == 0 && v) { SetEnvironmentVariableW(L"HALO3_FPS", v); i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (_wcsicmp(a, L"--interpolate") == 0 && v) { SetEnvironmentVariableW(L"HALO3_INTERPOLATE", v); i++;
        } else if (_wcsicmp(a, L"--no-interpolate") == 0) { SetEnvironmentVariableW(L"HALO3_INTERPOLATE", L"0");
#endif
        } else if (_wcsicmp(a, L"--vsync") == 0 && v) { SetEnvironmentVariableW(L"HALO3_VSYNC", v); i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (_wcsicmp(a, L"--gamma") == 0 && v) { SetEnvironmentVariableW(L"HALO3_DISPLAY_GAMMA", v); i++;
        } else if (_wcsicmp(a, L"--gpu") == 0 && v) { SetEnvironmentVariableW(L"HALO3_VULKAN_DEVICE", v); i++;
#endif
        } else if (_wcsicmp(a, L"--hd-text") == 0 && v) {
#ifndef HALO3_RELEASE_BUILD
            if (_wcsicmp(v, L"sdf") == 0) {
                SetEnvironmentVariableW(L"HALO3_GUIDE_HD_TEXT", L"sdf");
            } else
#endif
            {
                SetEnvironmentVariableW(L"HALO3_HD_TEXT", v);
                SetEnvironmentVariableW(L"HALO3_GUIDE_HD_TEXT", v);
            }
            i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (_wcsicmp(a, L"--raw-mouse") == 0) { SetEnvironmentVariableW(L"HALO3_RAW_MOUSE", L"1");
        } else if (_wcsicmp(a, L"--mouse-capture") == 0) { SetEnvironmentVariableW(L"HALO3_MOUSE_CAPTURE", L"1");
#endif
        } else if (_wcsicmp(a, L"--mouse-sens") == 0 && v) { SetEnvironmentVariableW(L"HALO3_MOUSE_SENS", v); i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (_wcsicmp(a, L"--players") == 0 && v) { SetEnvironmentVariableW(L"HALO3_LOCAL_PLAYERS", v); i++;
        } else if (_wcsicmp(a, L"--guests") == 0 && v) { SetEnvironmentVariableW(L"HALO3_LOCAL_GUESTS", v); i++;
        } else if (_wcsicmp(a, L"--coop-4p") == 0) { SetEnvironmentVariableW(L"HALO3_COOP_4P", L"1");
#endif
        } else if ((_wcsicmp(a, L"--gamertag") == 0 || _wcsicmp(a, L"--profile") == 0) && v) {
            SetEnvironmentVariableW(L"HALO3_PROFILE_NAME", v);
            SetEnvironmentVariableW(L"HALO3_GAMERTAG", v);
            WideCharToMultiByte(CP_UTF8, 0, v, -1, g_cfg.profile_name, sizeof(g_cfg.profile_name), NULL, NULL);
            g_cfg.profile_from_cli = 1;
            i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (_wcsicmp(a, L"--guide") == 0 && v) { SetEnvironmentVariableW(L"HALO3_GUIDE", v); i++;
        } else if (_wcsicmp(a, L"--guide-render") == 0 && v) { SetEnvironmentVariableW(L"HALO3_GUIDE_RENDER", v); i++;
        } else if (_wcsicmp(a, L"--audio-backend") == 0 && v) { SetEnvironmentVariableW(L"HALO3_AUDIO_BACKEND", v); i++;
        } else if (_wcsicmp(a, L"--audio-channels") == 0 && v) { SetEnvironmentVariableW(L"HALO3_AUDIO_CHANNELS", v); i++;
        } else if (_wcsicmp(a, L"--no-audio") == 0) { SetEnvironmentVariableW(L"HALO3_NO_AUDIO", L"1");
#endif
        } else if (_wcsicmp(a, L"--voice") == 0) { SetEnvironmentVariableW(L"HALO3_VOICE", L"1");
        } else if (_wcsicmp(a, L"--no-voice") == 0) { SetEnvironmentVariableW(L"HALO3_VOICE", L"0");
#ifndef HALO3_RELEASE_BUILD
        } else if (_wcsicmp(a, L"--lan") == 0) { SetEnvironmentVariableW(L"HALO3_LAN", L"1");
        } else if (_wcsicmp(a, L"--lan-port") == 0 && v) { SetEnvironmentVariableW(L"HALO3_LAN_PORT", v); i++;
        } else if (_wcsicmp(a, L"--lan-addr") == 0 && v) { SetEnvironmentVariableW(L"HALO3_LAN_ADDRESS", v); i++;
        } else if (_wcsicmp(a, L"--lan-slots") == 0 && v) { SetEnvironmentVariableW(L"HALO3_LAN_SLOTS", v); i++;
        } else if (_wcsicmp(a, L"--lan-peer") == 0 && v) { SetEnvironmentVariableW(L"HALO3_LAN_PEER", v); i++;
#endif
        } else if (_wcsicmp(a, L"--live") == 0 && v) { SetEnvironmentVariableW(L"HALO3_LIVE_SERVER", v); i++;
#ifndef HALO3_RELEASE_BUILD
        } else if (_wcsicmp(a, L"--live-account") == 0 && v) { SetEnvironmentVariableW(L"HALO3_LIVE_ACCOUNT", v); i++;
        } else if (_wcsicmp(a, L"--map") == 0 && v) { SetEnvironmentVariableW(L"HALO3_PROBE_MAP", v); i++;
        } else if (_wcsicmp(a, L"--frame-stats") == 0 && v) { SetEnvironmentVariableW(L"HALO3_FRAME_STATS", v); i++;
#endif
        } else if (_wcsicmp(a, L"--reextract") == 0) { g_cfg.reextract = 1;
        } else if (_wcsicmp(a, L"--rebuild-shaders") == 0) { g_cfg.rebuild_shaders = 1;
        } else if (_wcsicmp(a, L"--update") == 0) { g_cfg.do_update = 1;
        } else if (_wcsicmp(a, L"--no-update") == 0) { g_cfg.do_update = -1;
        } else if ((_wcsicmp(a, L"-j") == 0 || _wcsicmp(a, L"--jobs") == 0) && v) {
            int j = _wtoi(v);
            if (j > 0) g_cfg.jobs = j;
            i++;
        } else if (a[0] != L'-') {
            if (dir_exists(a)) {
                if (!absolute_path(is_odst_dlc_dir(a) ? g_cfg.odst_dlc_src : g_cfg.disc_path, a))
                    goto path_error;
            } else if (file_exists(a)) {
                if (!absolute_path(is_odst_dlc_iso(a) ? g_cfg.odst_dlc_src : g_cfg.iso_path, a))
                    goto path_error;
            } else {
                wchar_t msg[PATH_CAP + 64];
                format_wide(msg, PATH_CAP + 64, L"Path not found:\n%ls", a);
                MessageBoxW(NULL, msg, L"Halo 3 - Path Not Found", MB_ICONERROR);
                return 1;
            }
        }
    }

    if (!absolute_path(g_cfg.data_dir, g_cfg.data_dir) ||
        (g_cfg.disc_path[0] && !absolute_path(g_cfg.disc_path, g_cfg.disc_path)))
        goto path_error;
    if (!game_path_supported(g_cfg.data_dir, sizeof("\\shaders\\halo3_shaders.abi6.pack") - 1))
        goto game_path_error;
    if (g_cfg.custom_shader_pack) {
        if (!absolute_path(g_cfg.shader_pack, g_cfg.shader_pack)) goto path_error;
        if (!game_path_supported(g_cfg.shader_pack, 0)) goto game_path_error;
    } else if (!join_path(g_cfg.shader_pack, PATH_CAP, g_cfg.data_dir, L"shaders\\halo3_shaders.pack")) {
        goto path_error;
    }
    const wchar_t *relative_vars[] = { L"HALO3_DLC", L"HALO3_CONFIG" };
    for (size_t i = 0; i < sizeof(relative_vars) / sizeof(relative_vars[0]); ++i) {
        wchar_t path[PATH_CAP];
        DWORD len = GetEnvironmentVariableW(relative_vars[i], path, PATH_CAP);
        if (len >= PATH_CAP) goto path_error;
        if (len && !(i == 1 && wcscmp(path, L"0") == 0) &&
            (!absolute_path(path, path) || !SetEnvironmentVariableW(relative_vars[i], path)))
            goto path_error;
    }
    return 0;

path_error:
    MessageBoxW(NULL, L"A Halo 3 path could not be resolved within the launcher’s path limit.",
                L"Halo 3 - Path Error", MB_ICONERROR);
    return 1;

game_path_error:
    MessageBoxW(NULL, L"The game data or shader pack path is too long or uses characters outside the Windows system code page.",
                L"Halo 3 - Path Error", MB_ICONERROR);
    return 1;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {
    (void)hPrev; (void)lpCmdLine; (void)nCmdShow;

    if (!init_defaults()) {
        MessageBoxW(NULL, L"The installation path or a Halo 3 environment path is too long.",
                    L"Halo 3 - Path Error", MB_ICONERROR);
        return 1;
    }

    int argc = 0;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    int pr = parse_args(argc, argv);
    if (argv) LocalFree(argv);
    if (pr == 100) return 0;
    if (pr != 0) return pr;

    load_launcher_prefs();

    // Hold Shift on launch to reopen launcher even when "Skip launcher" is checked
    if (!g_cfg.headless && (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0) {
        g_cfg.force_setup = 1;
    }

    maybe_auto_update();

    int ready = check_setup_ready(NULL, 0);

    if (g_cfg.headless) {
        if (!ready || g_cfg.odst_dlc_src[0]) {
            InitializeCriticalSection(&g_ui.lock);
            DWORD rc = setup_worker_proc(NULL);
            DeleteCriticalSection(&g_ui.lock);
            if (rc != 0) return (int)rc;
        }
        return launch_game();
    }

    if (!ready || g_cfg.force_setup || g_cfg.odst_dlc_src[0] || !g_cfg.skip_launcher) {
        // Auto-start setup if we already have an ISO, ODST DLC ISO, or valid disc
        int auto_start = 0;
        if (!ready || g_cfg.odst_dlc_src[0]) {
            if (g_cfg.odst_dlc_src[0] && !g_cfg.iso_path[0] && !ready) g_ui.odst_only_job = 1;
            auto_start = g_cfg.iso_path[0] || g_cfg.odst_dlc_src[0] || select_setup_disc();
        }
        if (!run_setup_gui(hInst, auto_start)) {
            return 0;
        }
        if (!check_setup_ready(NULL, 0)) {
            return 1;
        }
    }

    return launch_game();
}
