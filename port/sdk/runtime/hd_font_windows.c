// port/sdk/runtime (Windows host): HD font rasterizer using built-in GDI (gdi32.dll)
// CreateDIBSection + ExtTextOutW (supports both OpenType CFF halo3.otf and TrueType fonts)
// with per-size HFONT caching so glyph rasterization is fast and zero-dependency.
#include "hd_font.h"

#ifdef _WIN32
#include "port_config.h"
#include "port_runtime.h"
#include "../kernel/win32_host.h"

__declspec(dllimport) char *__cdecl getenv(const char *name);

PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
PORT_HOST_IMPORT(unsigned long, GetModuleFileNameW, 12, (void *module, unsigned short *buf, unsigned long size));
PORT_HOST_IMPORT(unsigned long, GetEnvironmentVariableW, 12, (const unsigned short *name, unsigned short *buf,
                                                              unsigned long size));
PORT_HOST_IMPORT(unsigned long, GetFileAttributesW, 4, (const unsigned short *name));

#define HD_INVALID_FILE_ATTRIBUTES 0xfffffffful
#define HD_FILE_ATTRIBUTE_DIRECTORY 0x10ul
#define HD_FR_PRIVATE 0x10ul
#define HD_GGI_MARK_NONEXISTING_GLYPHS 0x0001ul
#define HD_GDI_ERROR 0xfffffffful
#define HD_FW_NORMAL 400
#define HD_DEFAULT_CHARSET 1ul
#define HD_OUT_DEFAULT_PRECIS 0ul
#define HD_CLIP_DEFAULT_PRECIS 0ul
#define HD_ANTIALIASED_QUALITY 4ul
#define HD_DEFAULT_PITCH_DONTCARE 0ul
#define HD_BI_RGB 0ul
#define HD_DIB_RGB_COLORS 0u
#define HD_TRANSPARENT 1
#define HD_TA_LEFT 0u
#define HD_TA_TOP 0u
#define HD_TA_NOUPDATECP 0u

#define HD_DIB_SIZE 256
#define HD_FONT_CACHE_SLOTS 32

typedef struct s_hd_textmetricw {
    int32_t tmHeight;
    int32_t tmAscent;
    int32_t tmDescent;
    int32_t tmInternalLeading;
    int32_t tmExternalLeading;
    int32_t tmAveCharWidth;
    int32_t tmMaxCharWidth;
    int32_t tmWeight;
    int32_t tmOverhang;
    int32_t tmDigitizedAspectX;
    int32_t tmDigitizedAspectY;
    uint16_t tmFirstChar;
    uint16_t tmLastChar;
    uint16_t tmDefaultChar;
    uint16_t tmBreakChar;
    uint8_t tmItalic;
    uint8_t tmUnderlined;
    uint8_t tmStruckOut;
    uint8_t tmPitchAndFamily;
    uint8_t tmCharSet;
    uint8_t _pad[3];
} s_hd_textmetricw;

typedef struct s_hd_size {
    int32_t cx;
    int32_t cy;
} s_hd_size;

typedef struct s_hd_bitmapinfoheader {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} s_hd_bitmapinfoheader;

typedef struct s_hd_bitmapinfo {
    s_hd_bitmapinfoheader bmiHeader;
    uint32_t bmiColors[1];
} s_hd_bitmapinfo;

typedef struct s_hd_cached_size {
    uint32_t pixel_height;
    void *hfont;
    int32_t ascender;
    int32_t descender;
    int32_t line_height;
} s_hd_cached_size;

typedef int (__stdcall *pfn_AddFontResourceExW)(const unsigned short *, unsigned long, void *);
typedef void *(__stdcall *pfn_CreateCompatibleDC)(void *);
typedef void *(__stdcall *pfn_CreateDIBSection)(void *, const s_hd_bitmapinfo *, unsigned int, void **, void *,
                                                unsigned long);
typedef void *(__stdcall *pfn_CreateFontW)(int, int, int, int, int, unsigned long, unsigned long, unsigned long,
                                           unsigned long, unsigned long, unsigned long, unsigned long, unsigned long,
                                           const unsigned short *);
typedef void *(__stdcall *pfn_SelectObject)(void *, void *);
typedef int (__stdcall *pfn_DeleteObject)(void *);
typedef int (__stdcall *pfn_SetBkMode)(void *, int);
typedef unsigned long (__stdcall *pfn_SetTextColor)(void *, unsigned long);
typedef unsigned int (__stdcall *pfn_SetTextAlign)(void *, unsigned int);
typedef int (__stdcall *pfn_GetTextMetricsW)(void *, s_hd_textmetricw *);
typedef int (__stdcall *pfn_GetTextExtentPoint32W)(void *, const unsigned short *, int, s_hd_size *);
typedef unsigned long (__stdcall *pfn_GetGlyphIndicesW)(void *, const unsigned short *, int, unsigned short *,
                                                        unsigned long);
typedef int (__stdcall *pfn_ExtTextOutW)(void *, int, int, unsigned int, const void *, const unsigned short *,
                                         unsigned int, const int *);
typedef int (__stdcall *pfn_GdiFlush)(void);

static struct {
    pfn_AddFontResourceExW add_font_resource_ex_w;
    pfn_CreateCompatibleDC create_compatible_dc;
    pfn_CreateDIBSection create_dib_section;
    pfn_CreateFontW create_font_w;
    pfn_SelectObject select_object;
    pfn_DeleteObject delete_object;
    pfn_SetBkMode set_bk_mode;
    pfn_SetTextColor set_text_color;
    pfn_SetTextAlign set_text_align;
    pfn_GetTextMetricsW get_text_metrics_w;
    pfn_GetTextExtentPoint32W get_text_extent_point32_w;
    pfn_GetGlyphIndicesW get_glyph_indices_w;
    pfn_ExtTextOutW ext_text_out_w;
    pfn_GdiFlush gdi_flush;
    void *hdc;
    void *hdib;
    uint32_t *dib_bits;
    unsigned short family[64];
    s_hd_cached_size sizes[HD_FONT_CACHE_SLOTS];
    uint32_t size_count;
    uint32_t next_evict;
    uint32_t current_height;
    int32_t current_ascender;
    int env_checked;
    int env_force_guide;
    int initialized;
    int available;
} g_font;

static size_t wlen(const unsigned short *s)
{
    size_t n = 0;
    while (s[n] != 0)
        n++;
    return n;
}

static void wcopy(unsigned short *dst, size_t cap, const unsigned short *src)
{
    size_t i = 0;
    while (i + 1 < cap && src[i] != 0) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = 0;
}

static int try_load_font_file(const unsigned short *path)
{
    if (!path || !path[0])
        return 0;
    unsigned long attrs = host_GetFileAttributesW(path);
    if (attrs == HD_INVALID_FILE_ATTRIBUTES || (attrs & HD_FILE_ATTRIBUTE_DIRECTORY))
        return 0;
    return g_font.add_font_resource_ex_w(path, HD_FR_PRIVATE, 0) > 0;
}

static int try_load_env_font(const unsigned short *var)
{
    unsigned short path[520];
    unsigned long n = host_GetEnvironmentVariableW(var, path, (unsigned long)(sizeof(path) / sizeof(path[0])));
    if (n == 0 || n >= (unsigned long)(sizeof(path) / sizeof(path[0])))
        return 0;
    return try_load_font_file(path);
}

static int load_bundled_font(void)
{
    static const unsigned short k_family_halo3[] = L"Halo 3 Conduit";
    static const unsigned short k_family_fallback[] = L"Segoe UI";

    if (try_load_env_font(L"HALO3_HD_FONT") || try_load_env_font(L"HALO3_GUIDE_FONT")) {
        wcopy(g_font.family, sizeof(g_font.family) / sizeof(g_font.family[0]), k_family_halo3);
        return 1;
    }

    unsigned short exe[520];
    unsigned long len = host_GetModuleFileNameW(0, exe, (unsigned long)(sizeof(exe) / sizeof(exe[0])));
    if (len > 0 && len < (unsigned long)(sizeof(exe) / sizeof(exe[0]))) {
        unsigned long slash = len;
        for (unsigned long i = 0; i < len; i++) {
            if (exe[i] == L'\\' || exe[i] == L'/')
                slash = i;
        }
        if (slash < len)
            exe[slash] = 0;
    } else {
        exe[0] = 0;
    }

    static const unsigned short *const rels[] = {
        L"\\fonts\\halo3.otf",
        L"\\..\\share\\halo3\\fonts\\halo3.otf",
        L"\\..\\port\\data\\fonts\\halo3.otf",
        L"\\..\\..\\port\\data\\fonts\\halo3.otf",
    };
    if (exe[0]) {
        size_t exe_len = wlen(exe);
        for (size_t i = 0; i < sizeof(rels) / sizeof(rels[0]); i++) {
            unsigned short path[520];
            size_t rel_len = wlen(rels[i]);
            if (exe_len + rel_len + 1 < sizeof(path) / sizeof(path[0])) {
                __builtin_memcpy(path, exe, exe_len * sizeof(unsigned short));
                __builtin_memcpy(path + exe_len, rels[i], (rel_len + 1) * sizeof(unsigned short));
                if (try_load_font_file(path)) {
                    wcopy(g_font.family, sizeof(g_font.family) / sizeof(g_font.family[0]), k_family_halo3);
                    return 1;
                }
            }
        }
    }

    if (try_load_font_file(L"port\\data\\fonts\\halo3.otf")) {
        wcopy(g_font.family, sizeof(g_font.family) / sizeof(g_font.family[0]), k_family_halo3);
        return 1;
    }

    // No halo3.otf loaded: missing, or on a path GDI will not load fonts from (a network share such as \\wsl.localhost).
    port_write_error("port: HD text: could not load fonts\\halo3.otf beside the game (or HALO3_HD_FONT); using Segoe UI\n");
    wcopy(g_font.family, sizeof(g_font.family) / sizeof(g_font.family[0]), k_family_fallback);
    return 1;
}

PORT_MS_ABI int port_hd_font_available(void)
{
    if (!g_font.env_checked) {
        const char *guide_enabled = getenv("HALO3_GUIDE_HD_TEXT");
        g_font.env_force_guide = (guide_enabled != 0 && guide_enabled[0] == '1' && guide_enabled[1] == '\0');
        g_font.env_checked = 1;
    }
    if (!port_config_hd_text() && !g_font.env_force_guide)
        return 0;
    if (g_font.initialized)
        return g_font.available;
    g_font.initialized = 1;

    void *gdi = host_LoadLibraryA("gdi32.dll");
    if (!gdi)
        return 0;
    g_font.add_font_resource_ex_w = (pfn_AddFontResourceExW)host_GetProcAddress(gdi, "AddFontResourceExW");
    g_font.create_compatible_dc = (pfn_CreateCompatibleDC)host_GetProcAddress(gdi, "CreateCompatibleDC");
    g_font.create_dib_section = (pfn_CreateDIBSection)host_GetProcAddress(gdi, "CreateDIBSection");
    g_font.create_font_w = (pfn_CreateFontW)host_GetProcAddress(gdi, "CreateFontW");
    g_font.select_object = (pfn_SelectObject)host_GetProcAddress(gdi, "SelectObject");
    g_font.delete_object = (pfn_DeleteObject)host_GetProcAddress(gdi, "DeleteObject");
    g_font.set_bk_mode = (pfn_SetBkMode)host_GetProcAddress(gdi, "SetBkMode");
    g_font.set_text_color = (pfn_SetTextColor)host_GetProcAddress(gdi, "SetTextColor");
    g_font.set_text_align = (pfn_SetTextAlign)host_GetProcAddress(gdi, "SetTextAlign");
    g_font.get_text_metrics_w = (pfn_GetTextMetricsW)host_GetProcAddress(gdi, "GetTextMetricsW");
    g_font.get_text_extent_point32_w = (pfn_GetTextExtentPoint32W)host_GetProcAddress(gdi, "GetTextExtentPoint32W");
    g_font.get_glyph_indices_w = (pfn_GetGlyphIndicesW)host_GetProcAddress(gdi, "GetGlyphIndicesW");
    g_font.ext_text_out_w = (pfn_ExtTextOutW)host_GetProcAddress(gdi, "ExtTextOutW");
    g_font.gdi_flush = (pfn_GdiFlush)host_GetProcAddress(gdi, "GdiFlush");
    if (!g_font.add_font_resource_ex_w || !g_font.create_compatible_dc || !g_font.create_dib_section ||
        !g_font.create_font_w || !g_font.select_object || !g_font.delete_object || !g_font.set_bk_mode ||
        !g_font.set_text_color || !g_font.set_text_align || !g_font.get_text_metrics_w ||
        !g_font.get_text_extent_point32_w || !g_font.get_glyph_indices_w || !g_font.ext_text_out_w ||
        !g_font.gdi_flush)
        return 0;

    g_font.hdc = g_font.create_compatible_dc(0);
    if (!g_font.hdc)
        return 0;

    s_hd_bitmapinfo bmi;
    __builtin_memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(s_hd_bitmapinfoheader);
    bmi.bmiHeader.biWidth = HD_DIB_SIZE;
    bmi.bmiHeader.biHeight = -HD_DIB_SIZE; // top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = HD_BI_RGB;

    void *bits = 0;
    g_font.hdib = g_font.create_dib_section(g_font.hdc, &bmi, HD_DIB_RGB_COLORS, &bits, 0, 0);
    if (!g_font.hdib || !bits)
        return 0;
    g_font.dib_bits = (uint32_t *)bits;
    g_font.select_object(g_font.hdc, g_font.hdib);
    g_font.set_bk_mode(g_font.hdc, HD_TRANSPARENT);
    g_font.set_text_color(g_font.hdc, 0x00fffffful);
    g_font.set_text_align(g_font.hdc, HD_TA_LEFT | HD_TA_TOP | HD_TA_NOUPDATECP);

    g_font.available = load_bundled_font();
    return g_font.available;
}

static s_hd_cached_size *select_cached_size(uint32_t pixel_height)
{
    if (!pixel_height || !port_hd_font_available())
        return 0;

    for (uint32_t i = 0; i < g_font.size_count; i++) {
        if (g_font.sizes[i].pixel_height == pixel_height) {
            if (g_font.current_height != pixel_height) {
                g_font.select_object(g_font.hdc, g_font.sizes[i].hfont);
                g_font.current_height = pixel_height;
                g_font.current_ascender = g_font.sizes[i].ascender;
            }
            return &g_font.sizes[i];
        }
    }

    void *hfont = g_font.create_font_w(-(int)pixel_height, 0, 0, 0, HD_FW_NORMAL, 0, 0, 0,
                                       HD_DEFAULT_CHARSET, HD_OUT_DEFAULT_PRECIS, HD_CLIP_DEFAULT_PRECIS,
                                       HD_ANTIALIASED_QUALITY, HD_DEFAULT_PITCH_DONTCARE, g_font.family);
    if (!hfont)
        return 0;

    g_font.select_object(g_font.hdc, hfont);
    s_hd_textmetricw tm;
    if (!g_font.get_text_metrics_w(g_font.hdc, &tm)) {
        g_font.delete_object(hfont);
        g_font.current_height = 0;
        return 0;
    }

    uint32_t slot;
    if (g_font.size_count < HD_FONT_CACHE_SLOTS) {
        slot = g_font.size_count++;
    } else {
        slot = g_font.next_evict;
        g_font.next_evict = (g_font.next_evict + 1) % HD_FONT_CACHE_SLOTS;
        if (g_font.sizes[slot].hfont)
            g_font.delete_object(g_font.sizes[slot].hfont);
    }

    s_hd_cached_size *entry = &g_font.sizes[slot];
    entry->pixel_height = pixel_height;
    entry->hfont = hfont;
    entry->ascender = tm.tmAscent;
    entry->descender = -tm.tmDescent;
    entry->line_height = tm.tmHeight + tm.tmExternalLeading;
    g_font.current_height = pixel_height;
    g_font.current_ascender = entry->ascender;
    return entry;
}

PORT_MS_ABI int port_hd_font_metrics(uint32_t pixel_height, int32_t *out_ascender, int32_t *out_descender,
                                     int32_t *out_line_height)
{
    s_hd_cached_size *entry = select_cached_size(pixel_height);
    if (!entry)
        return 0;
    *out_ascender = entry->ascender;
    *out_descender = entry->descender;
    *out_line_height = entry->line_height;
    return 1;
}

PORT_MS_ABI int port_hd_font_glyph(uint32_t pixel_height, uint32_t codepoint, s_port_hd_glyph *out,
                                   uint8_t *alpha_buf, uint32_t alpha_buf_size)
{
    if (codepoint > 0xFFFFu || pixel_height >= HD_DIB_SIZE - 32)
        return 0;

    s_hd_cached_size *entry = select_cached_size(pixel_height);
    if (!entry)
        return 0;

    unsigned short ch = (unsigned short)codepoint;
    unsigned short glyph_index = 0xFFFFu;
    if (g_font.get_glyph_indices_w(g_font.hdc, &ch, 1, &glyph_index, HD_GGI_MARK_NONEXISTING_GLYPHS) == HD_GDI_ERROR ||
        glyph_index == 0xFFFFu)
        return 0;

    s_hd_size ext = { 0, 0 };
    if (!g_font.get_text_extent_point32_w(g_font.hdc, &ch, 1, &ext))
        return 0;

    const int origin_x = (int)(pixel_height / 2) + 8;
    const int origin_y = 8;
    int region_w = origin_x + ext.cx + (int)(pixel_height / 2) + 8;
    int region_h = origin_y + ext.cy + 16;
    if (region_w > HD_DIB_SIZE)
        region_w = HD_DIB_SIZE;
    if (region_h > HD_DIB_SIZE)
        region_h = HD_DIB_SIZE;

    // Clear only the rows we will draw into.
    __builtin_memset(g_font.dib_bits, 0, (size_t)region_h * HD_DIB_SIZE * sizeof(uint32_t));
    if (!g_font.ext_text_out_w(g_font.hdc, origin_x, origin_y, 0, 0, &ch, 1, 0))
        return 0;
    g_font.gdi_flush();

    int min_x = region_w, min_y = region_h, max_x = -1, max_y = -1;
    for (int y = 0; y < region_h; y++) {
        const uint32_t *row = g_font.dib_bits + (size_t)y * HD_DIB_SIZE;
        for (int x = 0; x < region_w; x++) {
            uint32_t px = row[x];
            if ((px & 0x00ffffffu) != 0) {
                if (x < min_x) min_x = x;
                if (x > max_x) max_x = x;
                if (y < min_y) min_y = y;
                if (y > max_y) max_y = y;
            }
        }
    }

    if (max_x < min_x || max_y < min_y) {
        *out = (s_port_hd_glyph){ 0, 0, 0, 0, ext.cx };
        return 1;
    }

    const uint32_t bw = (uint32_t)(max_x - min_x + 1);
    const uint32_t bh = (uint32_t)(max_y - min_y + 1);
    if (bh > alpha_buf_size / bw)
        return 0;

    for (uint32_t y = 0; y < bh; y++) {
        const uint32_t *row = g_font.dib_bits + (size_t)(min_y + (int)y) * HD_DIB_SIZE + min_x;
        uint8_t *dst = alpha_buf + y * bw;
        for (uint32_t x = 0; x < bw; x++) {
            // ANTIALIASED_QUALITY renders grayscale white-on-black; use green channel as coverage.
            dst[x] = (uint8_t)((row[x] >> 8) & 0xffu);
        }
    }

    *out = (s_port_hd_glyph){
        (int32_t)bw,
        (int32_t)bh,
        (int32_t)(min_x - origin_x),
        (int32_t)((origin_y + entry->ascender) - min_y),
        ext.cx,
    };
    return 1;
}
#endif
