#include "hd_font.h"

#if !defined(PORT_HAVE_FREETYPE_HEADERS) && !defined(_WIN32)
PORT_MS_ABI int port_hd_font_available(void) { return 0; }

PORT_MS_ABI int port_hd_font_metrics(uint32_t pixel_height, int32_t *out_ascender, int32_t *out_descender,
                                   int32_t *out_line_height)
{
    (void)pixel_height;
    (void)out_ascender;
    (void)out_descender;
    (void)out_line_height;
    return 0;
}

PORT_MS_ABI int port_hd_font_glyph(uint32_t pixel_height, uint32_t codepoint, s_port_hd_glyph *out,
                                 uint8_t *alpha_buf, uint32_t alpha_buf_size)
{
    (void)pixel_height;
    (void)codepoint;
    (void)out;
    (void)alpha_buf;
    (void)alpha_buf_size;
    return 0;
}
#endif
