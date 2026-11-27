#ifndef TEXT_DRAW_STRING_H
#define TEXT_DRAW_STRING_H

#include "text/font_id_types.h"

// Functions defined in tu/blamlibxbox/draw_string.cpp.

struct s_font_header;
struct utf32;

enum e_text_justification
{
    _text_justification_left = 0,
    _text_justification_right,
    _text_justification_center,
    k_text_justification_count
};

const s_font_header *font_get_header(long font_id);
short font_get_kerning_pair_offset(const s_font_header *header, unsigned long first_character, unsigned long second_character);
void ascii_parse_set_color_sequence(const char *in_src, const char **out_src, utf32 *result, long result_max_length, long *result_length);
void wchar_parse_set_color_sequence(const wchar_t *source, const wchar_t **end, utf32 *output, long maximum_length, long *length);

#endif // TEXT_DRAW_STRING_H
