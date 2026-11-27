#include "hd_font.h"

#ifdef PORT_HAVE_FREETYPE_HEADERS
#include "port_config.h"
#include <dlfcn.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ft2build.h>
#include FT_FREETYPE_H

static struct {
    FT_Error (*init)(FT_Library *);
    FT_Error (*done)(FT_Library);
    FT_Error (*new_face)(FT_Library, const char *, FT_Long, FT_Face *);
    FT_Error (*set_pixel_sizes)(FT_Face, FT_UInt, FT_UInt);
    FT_Error (*load_char)(FT_Face, FT_ULong, FT_Int32);
    FT_UInt (*get_char_index)(FT_Face, FT_ULong);
    FT_Face face;
    uint32_t width_px;
    uint32_t height_px;
    float scale_x;
    float digit_scale_x;
    float scale_y;
    float tracking;
    int is_ddin;
    int initialized;
} font;

static float parse_env_float(const char *name, float fallback)
{
    const char *v = getenv(name);
    if (!v || !v[0])
        return fallback;
    char *end = NULL;
    float parsed = strtof(v, &end);
    return (end != v && parsed > 0.0f) ? parsed : fallback;
}

static FT_Face open_face(FT_Library library, const char *path)
{
    FT_Face face;
    return path && *path && !font.new_face(library, path, 0, &face) ? face : NULL;
}

static FT_Face find_face(FT_Library library)
{
    FT_Face face = open_face(library, getenv("HALO3_HD_FONT"));
    if (face)
        return face;
    face = open_face(library, getenv("HALO3_GUIDE_FONT"));
    if (face)
        return face;

    char executable[PATH_MAX];
    ssize_t length = readlink("/proc/self/exe", executable, sizeof(executable) - 1);
    if (length <= 0 || length >= (ssize_t)sizeof(executable) - 1)
        length = 0;
    executable[length] = '\0';
    char *filename = strrchr(executable, '/');
    if (filename)
        *filename = '\0';

    static const char *const names[] = { "halo3.otf", "D-DIN.ttf" };
    static const char *const dirs[] = { "/fonts", "/../port/data/fonts", "/../../port/data/fonts" };
    for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); i++) {
        char path[PATH_MAX];
        for (size_t j = 0; filename && j < sizeof(dirs) / sizeof(dirs[0]); j++) {
            int n = snprintf(path, sizeof(path), "%s%s/%s", executable, dirs[j], names[i]);
            if (n >= 0 && (size_t)n < sizeof(path) && (face = open_face(library, path)))
                return face;
        }
        snprintf(path, sizeof(path), "port/data/fonts/%s", names[i]);
        if ((face = open_face(library, path)))
            return face;
    }

    static const char *const system_fonts[] = {
        "/usr/share/fonts/google-noto/NotoSans-CondensedMedium.ttf",
        "/usr/share/fonts/google-noto/NotoSans-Medium.ttf",
        "/usr/share/fonts/google-noto/NotoSans-Regular.ttf",
        "/usr/share/fonts/liberation-sans/LiberationSans-Regular.ttf",
        "/usr/share/fonts/liberation-sans-fonts/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/dejavu-sans-fonts/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    };
    for (size_t i = 0; i < sizeof(system_fonts) / sizeof(system_fonts[0]); i++) {
        if ((face = open_face(library, system_fonts[i])))
            return face;
    }
    return NULL;
}

PORT_MS_ABI int port_hd_font_available(void)
{
    const char *guide_enabled = getenv("HALO3_GUIDE_HD_TEXT");
    if (!port_config_hd_text() && (!guide_enabled || strcmp(guide_enabled, "1") != 0))
        return 0;
    if (font.initialized)
        return font.face != NULL;
    font.initialized = 1;

    void *module = dlopen("libfreetype.so.6", RTLD_LAZY | RTLD_LOCAL);
    if (!module)
        return 0;
    font.init = dlsym(module, "FT_Init_FreeType");
    font.done = dlsym(module, "FT_Done_FreeType");
    font.new_face = dlsym(module, "FT_New_Face");
    font.set_pixel_sizes = dlsym(module, "FT_Set_Pixel_Sizes");
    font.load_char = dlsym(module, "FT_Load_Char");
    font.get_char_index = dlsym(module, "FT_Get_Char_Index");
    FT_Library library;
    if (font.init && font.done && font.new_face && font.set_pixel_sizes && font.load_char &&
        font.get_char_index && !font.init(&library)) {
        font.face = find_face(library);
        if (font.face) {
            const char *family = font.face->family_name;
            font.is_ddin = family && strncmp(family, "D-DIN", 5) == 0;
            const int condensed = font.is_ddin && (strstr(family, "Condensed") ||
                (font.face->style_name && strstr(font.face->style_name, "Condensed")));
            font.scale_x = parse_env_float("HALO3_GUIDE_FONT_SCALE_X",
                condensed ? 1.32f : (font.is_ddin ? 1.05f : 1.0f));
            font.digit_scale_x = parse_env_float("HALO3_GUIDE_FONT_DIGIT_SCALE_X", font.scale_x);
            font.scale_y = parse_env_float("HALO3_GUIDE_FONT_SCALE_Y", font.is_ddin ? 1.12f : 1.0f);
            const char *trk = getenv("HALO3_GUIDE_FONT_TRACKING");
            font.tracking = (trk && trk[0]) ? strtof(trk, NULL) : 0.0f;
            return 1;
        }
        font.done(library);
    }
    dlclose(module);
    return 0;
}

static int set_size(uint32_t pixel_height, int is_digit)
{
    if (!pixel_height || !port_hd_font_available())
        return 0;
    const float sx = is_digit ? font.digit_scale_x : font.scale_x;
    const uint32_t w = (uint32_t)(pixel_height * sx + 0.5f);
    const uint32_t h = (uint32_t)(pixel_height * font.scale_y + 0.5f);
    const uint32_t rw = w ? w : 1;
    const uint32_t rh = h ? h : 1;
    if (font.width_px != rw || font.height_px != rh) {
        if (font.set_pixel_sizes(font.face, rw, rh))
            return 0;
        font.width_px = rw;
        font.height_px = rh;
    }
    return 1;
}

PORT_MS_ABI int port_hd_font_metrics(uint32_t pixel_height, int32_t *out_ascender, int32_t *out_descender,
                                   int32_t *out_line_height)
{
    if (!set_size(pixel_height, 0))
        return 0;
    const FT_Size_Metrics *metrics = &font.face->size->metrics;
    *out_ascender = (int32_t)(metrics->ascender / 64);
    *out_descender = (int32_t)(metrics->descender / 64);
    *out_line_height = (int32_t)((metrics->height + 32) / 64);
    return 1;
}

PORT_MS_ABI int port_hd_font_glyph(uint32_t pixel_height, uint32_t codepoint, s_port_hd_glyph *out,
                                 uint8_t *alpha_buf, uint32_t alpha_buf_size)
{
    const int is_digit = (codepoint >= '0' && codepoint <= '9');
    if (!set_size(pixel_height, is_digit) || !font.get_char_index(font.face, codepoint) ||
        font.load_char(font.face, codepoint, FT_LOAD_RENDER | FT_LOAD_NO_BITMAP))
        return 0;
    const FT_GlyphSlot slot = font.face->glyph;
    const FT_Bitmap *bitmap = &slot->bitmap;
    if (bitmap->width && bitmap->rows) {
        if (bitmap->pixel_mode != FT_PIXEL_MODE_GRAY || bitmap->num_grays != 256 ||
            bitmap->rows > alpha_buf_size / bitmap->width)
            return 0;
        for (uint32_t y = 0; y < bitmap->rows; y++) {
            const ptrdiff_t row_index = bitmap->pitch >= 0 ? y : (ptrdiff_t)y - (bitmap->rows - 1);
            const uint8_t *row = bitmap->buffer + row_index * bitmap->pitch;
            memcpy(alpha_buf + y * bitmap->width, row, bitmap->width);
        }
    }
    const float trk = font.tracking + (is_digit && font.is_ddin ? 0.03f : 0.0f);
    const int32_t extra = (int32_t)(pixel_height * trk + (trk >= 0.0f ? 0.5f : -0.5f));
    int32_t adv = (int32_t)((slot->advance.x + 32) / 64) + extra;
    if (adv < 0)
        adv = 0;
    *out = (s_port_hd_glyph){ (int32_t)bitmap->width, (int32_t)bitmap->rows, slot->bitmap_left, slot->bitmap_top, adv };
    return 1;
}
#endif
