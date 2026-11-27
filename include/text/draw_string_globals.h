#ifndef TEXT_DRAW_STRING_GLOBALS_H
#define TEXT_DRAW_STRING_GLOBALS_H

#include "text/font_id_types.h"
#include "math/real_math_types.h"

struct s_font_globals;
struct utf32;
class c_font_cache;

extern "C++" s_font_globals g_font_globals;
extern "C" bool rasterizer_text_try_and_get_glyph(c_font_cache *font_cache, e_font_id font, utf32 character, bool precache, long *out_value, long *out_index);

#endif // TEXT_DRAW_STRING_GLOBALS_H
