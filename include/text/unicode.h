#ifndef TEXT_UNICODE_H
#define TEXT_UNICODE_H

// Functions defined in tu/blamlibxbox/unicode.cpp.

#include "text/escape_table_types.h"

struct utf32;
struct utf8;

// Tick deadline limiting warnings about unrecognized escape characters to once a minute.
extern "C" unsigned int g_unicode_next_escape_warning_time;

wchar_t *ustrnzcpy(wchar_t *destination, const wchar_t *source, long size);
wchar_t *ustrnzcat(wchar_t *destination, const wchar_t *source, long size);
wchar_t *ustrnupr(wchar_t *text, long count);
void utf8_string_to_wchar_string(const utf8 *source_utf8, wchar_t *destination_wchar, long count, long *unused);
void wchar_string_to_utf32_string(const wchar_t *string, const s_escape_table *escape_table, utf32 *destination,
    long destination_count, long *characters_consumed);
void ascii_string_to_utf32_string(const char *string, const s_escape_table *escape_table, utf32 *destination,
    long destination_count, long *characters_consumed);
extern "C" int usnprintf(wchar_t *buffer, int count, const wchar_t *format, ...);
extern "C" unsigned char utf32_is_east_asian_character(int c);
extern "C" bool utf32_is_non_beginning_character(unsigned int character);
extern "C" bool utf32_is_non_ending_character(unsigned int character);
extern "C" unsigned int wchar_escape_character_to_utf32(unsigned short c, const s_escape_table *table, bool *found,
    t_unicode_char_sequence_parser *out_ascii_parser, t_unicode_sequence_parser *out_wchar_parser);
utf32 wchar_string_to_utf32_characters(const wchar_t *string, const s_escape_table *table, const wchar_t **end, long *consumed,
    utf32 *out, long max, long *length);
utf32 ascii_string_to_utf32_characters(const char *string, const s_escape_table *table, const char **end, long *consumed,
    utf32 *out, long max, long *length);

struct s_utf32_range;
bool utf32_can_line_break(utf32 previous_character, utf32 character);
bool utf32_in_list(utf32 character, const s_utf32_range *ranges, long count);

// Inline bounded copy used by fixed-size UI strings.
static __forceinline void ustrnzcpy_inline(wchar_t *destination, const wchar_t *source, int count)
{
    for (int i = 0; i <= count - 1; i++) {
        if ((destination[i] = *source) == 0)
            break;
        source++;
    }
    destination[count - 1] = 0;
}

#endif // TEXT_UNICODE_H
