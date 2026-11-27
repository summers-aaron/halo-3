#ifndef PORT_HD_FONT_H
#define PORT_HD_FONT_H

#if __has_include(<stdint.h>)
#include <stdint.h>
#else
// The game compiler uses the XDK headers, which have no stdint.h.
typedef __UINT8_TYPE__ uint8_t;
typedef __UINT32_TYPE__ uint32_t;
typedef __INT32_TYPE__ int32_t;
#endif
#include "port_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct s_port_hd_glyph {
    int32_t width, height, bearing_x, bearing_y, advance_x;
} s_port_hd_glyph;

// Called on the presenting thread. Returns zero when HD text is disabled or unavailable.
PORT_MS_ABI int port_hd_font_available(void);
// Metrics are in pixels, with a negative descender. Glyph coverage is tightly packed, top row first.
PORT_MS_ABI int port_hd_font_metrics(uint32_t pixel_height, int32_t *out_ascender, int32_t *out_descender,
                                   int32_t *out_line_height);
PORT_MS_ABI int port_hd_font_glyph(uint32_t pixel_height, uint32_t codepoint, s_port_hd_glyph *out,
                                 uint8_t *alpha_buf, uint32_t alpha_buf_size);

#ifdef __cplusplus
}
#endif

#endif
