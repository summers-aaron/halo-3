// blamlibxbox/unicode.obj: from the records 0x8a3c0 0x189a68 0x189b28 0x191970 0x191a28 0x1ab008 0x1ab068 0x1ab128 0x1ab178 0x1ab1f0 0x1ab328 0x1ab390 0x1ab440 0x1ab5c0 0x1ab630 0x1ac1a8 0x1ac628
// @flags /TP /O2 /Oc
#include "cseries/cseries_unicode.h"
// @address 0x1ab008 ?ustrnzcat@@YAPA_WPA_WPB_WJ@Z
// @address 0x1ab068 ?ustrnupr@@YAPA_WPA_WJ@Z
// @address 0x8a3c0 ?ustrnzcpy@@YAPA_WPA_WPB_WJ@Z
// @address 0x187780 _vsnwprintf_s
// @address 0x80608 GetTickCount

#include <stdarg.h>
#include <string.h>
#include "cseries/crt_xdk.h"
#pragma pack(push, 1)
#include "interface/chud/utf32_types.h"
#pragma pack(pop)
#include "text/unicode.h"
#include "text/utf32_range_types.h"
#include "text/escape_table_types.h"
#include "text/unicode_tables.h"
#include "text/draw_string.h"
// @address 0x330b8 k_unicode_escape_table
#if !defined(_XBOX) || defined(__clang__)
const s_escape_table k_unicode_escape_table[10] = {
    { '|', 0x007C, 0, 0 },
    { 'l', 0xE405, 0, 0 },
    { 'r', 0xE406, 0, 0 },
    { 'c', 0xE407, 0, 0 },
    { 'n', 0x000D, 0, 0 },
    { 't', 0x0009, 0, 0 },
    { 'k', 0xE301, ascii_parse_set_color_sequence, wchar_parse_set_color_sequence },
    { 'j', 0xE409, 0, 0 },
    { 0, 0, 0, 0 },
    { 0xFFFF, 0xFFFFFFFF, 0, 0 },
};
#endif
// @rdata 0x8203b548..0x8203b578
// @address 0x3b548 k_unicode_line_break_ranges
const s_utf32_range k_unicode_line_break_ranges[12] = {
    { 0x0009, 0x000D },
    { 0x0020, 0x0020 },
    { 0x0085, 0x0085 },
    { 0x00A0, 0x00A0 },
    { 0x1680, 0x1680 },
    { 0x180E, 0x180E },
    { 0x2000, 0x200A },
    { 0x2028, 0x2028 },
    { 0x2029, 0x2029 },
    { 0x202F, 0x202F },
    { 0x205F, 0x205F },
    { 0x3000, 0x3000 },
};
// @rdata-end



wchar_t *ustrnzcpy(wchar_t *destination, const wchar_t *source, long size)
{
    const wchar_t *s = source;
    for (long index = 0; index <= size - 1; index++) {
        destination[index] = *s;
        if (destination[index] == 0)
            break;
        s++;
    }
    destination[size - 1] = 0;
    return destination;
}


wchar_t *ustrnzcat(wchar_t *destination, const wchar_t *source, long size)
{
    wchar_t *end = destination;
    while (*end) {
        end++;
        size--;
    }
    while (*source && size > 1) {
        *end++ = *source++;
        size--;
    }
    *end = 0;
    return destination;
}

wchar_t *ustrnupr(wchar_t *text, long count)
{
    wchar_t *cursor = text;
    if (*text == 0)
        return text;
    do {
        if (count < 0)
            return text;
        unsigned short character = *cursor;
        unsigned short uppercase;
        if (character >= 97 && character <= 122)
            uppercase = character - 32;
        else if (character >= 224 && character <= 254)
            uppercase = character - 32;
        else if (character >= 257 && character <= 382)
            uppercase = character - 1;
        else if (character >= 945 && character <= 969)
            uppercase = character - 32;
        else
            uppercase = character;
        *cursor = uppercase;
        cursor++;
        count--;
    } while (*cursor != 0);
    return text;
}

// @address 0x1ab128 usnprintf
int usnprintf(wchar_t *buffer, int count, const wchar_t *format, ...)
{
    va_list args;
    va_start(args, format);
    return _vsnwprintf_s(buffer, count, _TRUNCATE, format, args);
}

// @address 0x1ab178 ?usnzprintf@@YAHPA_WJPB_WZZ
int usnzprintf(wchar_t *buffer, long size, const wchar_t *format, ...)
{
    va_list args;
    va_start(args, format);
    int result = _vsnwprintf_s(buffer, size - 1, _TRUNCATE, format, args);
    buffer[size - 1] = 0;
    return result;
}

/* Encodes a zero-terminated UTF-16 string as UTF-8 into a bounded buffer, truncating and terminating it. */
static __forceinline int utf8_lead_byte(unsigned int character, unsigned int *lead)
{
    if (character <= 0x7f) {
        *lead = character;
        return 1;
    }
    if (character <= 0x7ff) {
        *lead = (character >> 6) | 0xc0;
        return 2;
    }
    if (character <= 0xffff) {
        *lead = (character >> 12) | 0xe0;
        return 3;
    }
    if (character <= 0x1fffff) {
        *lead = (character >> 18) | 0xf0;
        return 4;
    }
    return 0;
}

// @address 0x1ab1f0 wchar_string_to_utf8_string
extern "C" void wchar_string_to_utf8_string(const unsigned short *source, unsigned char *destination, int size, int unused)
{
    int length = 0;
    for (int index = 0; source[index] != 0; index++) {
        unsigned int character = source[index];
        int remaining = size - length;
        unsigned char *out = destination + length;
        unsigned int lead;
        int count = utf8_lead_byte(character, &lead);
        if (count != 0 && remaining > 0) {
            out[0] = lead;
        }
        for (int j = 1; j < remaining && j < count; j++) {
            out[j] = (unsigned char)(((character >> ((count - j) * 6 - 6)) & 0x3f) | 0x80);
        }
        length += (count > remaining) ? remaining : count;
    }
    if (length < size) {
        destination[length] = 0;
    } else if (length > 0) {
        destination[length - 1] = 0;
    }
}

// @address 0x1ab328 ?utf32_in_list@@YA_NUutf32@@PBUs_utf32_range@@J@Z
bool utf32_in_list(utf32 character, const s_utf32_range *ranges, long count)
{
    // match: 0x1ab328: value range assumption guides branch lowering
    __assume(ranges == k_unicode_line_break_ranges);
    // match: 0x1ab328: value range assumption guides branch lowering
    __assume(count == 12);
    unsigned short value = (unsigned short)character.character;
    bool result = false;
    long low = 0;
    long high = count - 1;
    while (low <= high) {
        long middle = (low + high) / 2;
        const s_utf32_range *middle_range = &ranges[middle];
        if (value < middle_range->start)
            high = middle - 1;
        else if (value > middle_range->end)
            low = middle + 1;
        else {
            result = true;
            break;
        }
    }
    return result;
}

/* Converts a UTF-8 string to UTF-16 in a sized buffer, always terminating it; the last argument is unused. */
struct utf8;

// @address 0x1ab440 ?utf8_string_to_wchar_string@@YAXPBUutf8@@PA_WJPAJ@Z
void utf8_string_to_wchar_string(const utf8 *source_utf8, wchar_t *destination_wchar, long count, long *unused)
{
  const unsigned char *source = (const unsigned char *)source_utf8;
  unsigned short *destination = (unsigned short *)destination_wchar;
  unsigned long code;
  int index = 0;
  int written = 0;
  int total = 0;
  while (source[index] != 0) {
    bool valid = true;
    unsigned long lead = source[index];
    int length = 1;
    if ((lead & ~0x7f) == 0) {
      code = lead;
    } else if ((lead & 0xe0) == 0xc0) {
      code = lead & ~0xe0;
      length = 2;
    } else if ((lead & 0xf0) == 0xe0) {
      code = lead & ~0xf0;
      length = 3;
    } else if ((lead & 0xf8) == 0xf0) {
      code = lead & ~0xf8;
      length = 4;
    } else {
      valid = false;
    }
    if (valid) {
      for (int i = 1; valid && i < length; i++) {
        unsigned char next = source[index + i];
        code = (next & ~0xc0) | (code << 6);
        if ((next & ~0x3fU) != 0x80)
          valid = false;
      }
    }
    if (valid)
      index += length;
    else
      index++;
    total += valid ? length : 1;
    // match: 0x1ab440: value range assumption guides branch lowering
    __assume(total >= 0);
    if (valid && written < count) {
      if (code <= (unsigned long)-1)
        destination[written] = (unsigned short)code;
      else
        destination[written] = '?';
      written++;
    }
  }
  if (written < count)
    destination[written] = 0;
  else if (written > 0)
    destination[written - 1] = 0;
}


// @address 0x1ab5c0 utf32_is_east_asian_character
unsigned char utf32_is_east_asian_character(int c)
{
    int v;
    if (!((c < 0x1100 || c > 0x11ff) && (c < 12288 || c > 0xd7af) && (c < 0xf900 || c > 0xfaff) && (unsigned)(c - 0xff00) > 0xdc))
        v = 1;
    else
        v = 0;
    return (unsigned char)v;
}


// @address 0x1ab630 utf32_is_non_beginning_character
bool utf32_is_non_beginning_character(unsigned int character_code)
{
    bool result = false;
    switch (character_code) {
    case L'!':
    case L'"':
    case L'%':
    case L')':
    case L',':
    case L'.':
    case L':':
    case L';':
    case L'?':
    case L']':
    case L'}':
    case L'\u00b0':
    case L'\u00b7':
    case L'\u2013':
    case L'\u2014':
    case L'\u2019':
    case L'\u201d':
    case L'\u2022':
    case L'\u2026':
    case L'\u2027':
    case L'\u2032':
    case L'\u2033':
    case L'\u2103':
    case L'\u3001':
    case L'\u3002':
    case L'\u3009':
    case L'\u300b':
    case L'\u300d':
    case L'\u300f':
    case L'\u3011':
    case L'\u3015':
    case L'\u301e':
    case L'\u3041':
    case L'\u3043':
    case L'\u3045':
    case L'\u3047':
    case L'\u3049':
    case L'\u3063':
    case L'\u3083':
    case L'\u3085':
    case L'\u3087':
    case L'\u308e':
    case L'\u30a1':
    case L'\u30a3':
    case L'\u30a5':
    case L'\u30a7':
    case L'\u30a9':
    case L'\u30c3':
    case L'\u30e3':
    case L'\u30e5':
    case L'\u30e7':
    case L'\u30ee':
    case L'\u30fc':
    case L'\ufe30':
    case L'\ufe50':
    case L'\ufe51':
    case L'\ufe52':
    case L'\ufe54':
    case L'\ufe55':
    case L'\ufe56':
    case L'\ufe57':
    case L'\ufe5a':
    case L'\ufe5c':
    case L'\ufe5e':
    case L'\uff01':
    case L'\uff05':
    case L'\uff09':
    case L'\uff0c':
    case L'\uff0e':
    case L'\uff1a':
    case L'\uff1b':
    case L'\uff1f':
    case L'\uff3d':
    case L'\uff5d':
    case L'\uff61':
    case L'\uff64':
    case L'\uff70':
    case L'\uff9e':
    case L'\uff9f':
    case L'\uffe0':
        result = true;
    }
    return result;
}


// @address 0x1ac1a8 utf32_is_non_ending_character
bool utf32_is_non_ending_character(unsigned int codepoint)
{
    bool result = false;
    switch (codepoint) {
    case L'"':
    case L'$':
    case L'(':
    case L'[':
    case L'\\':
    case L'{':
    case L'\u2018':
    case L'\u201c':
    case L'\u2035':
    case L'\u3008':
    case L'\u300a':
    case L'\u300c':
    case L'\u300e':
    case L'\u3010':
    case L'\u3014':
    case L'\u301d':
    case L'\ufe59':
    case L'\ufe5b':
    case L'\ufe5d':
    case L'\uff04':
    case L'\uff08':
    case L'\uff3b':
    case L'\uff5b':
    case L'\uffe1':
    case L'\uffe5':
    case L'\uffe6':
        result = true;
    }
    return result;
}

// @address 0x119a3e8 g_unicode_next_escape_warning_time
extern "C" {
unsigned int g_unicode_next_escape_warning_time;
}
// @address 0x1ac628 wchar_escape_character_to_utf32
unsigned int wchar_escape_character_to_utf32(unsigned short c, const s_escape_table *table, bool *found,
    t_unicode_char_sequence_parser *out_ascii_parser, t_unicode_sequence_parser *out_wchar_parser)
{
    bool result = false;
    t_unicode_char_sequence_parser ascii_parser = 0;
    t_unicode_sequence_parser wchar_parser = 0;
    unsigned int unicode = table[0].unicode_character;
    for (int i = 0; table[i].wchar_character != 0xffff; i++) {
        if (c == table[i].wchar_character) {
            result = true;
            unicode = table[i].unicode_character;
            ascii_parser = table[i].ascii_sequence_parser;
            wchar_parser = table[i].wchar_sequence_parser;
            break;
        }
    }
    if (!result && c != 0) {
        unsigned int now = GetTickCount();
        if (now > g_unicode_next_escape_warning_time)
            g_unicode_next_escape_warning_time = now + 60000;
    }
    *found = result;
    if (out_ascii_parser)
        *out_ascii_parser = ascii_parser;
    if (out_wchar_parser)
        *out_wchar_parser = wchar_parser;
    return unicode;
}

// @address 0x189b28 ?wchar_string_to_utf32_characters@@YA?AUutf32@@PB_WPBUs_escape_table@@PAPB_WPAJPAU1@J3@Z
utf32 wchar_string_to_utf32_characters(
    const wchar_t *string, const s_escape_table *table, const wchar_t **end, long *consumed, utf32 *out, long max, long *length)
{
    const wchar_t *next = string + 1;
    long result_length = 1;
    wchar_t c = *string;
    if (table && table->wchar_character == c) {
        bool parsed = false;
        t_unicode_sequence_parser parser = 0;
        out->character = wchar_escape_character_to_utf32(string[1], table, &parsed, 0, &parser);
        if (parser)
            parser(string, &next, out, max, &result_length);
        else if (parsed)
            next++;
    } else {
        out->character = c;
    }
    *end = next;
    *consumed = next - string;
    *length = result_length;
    return *out;
}

// @address 0x191a28 ?ascii_string_to_utf32_characters@@YA?AUutf32@@PBDPBUs_escape_table@@PAPBDPAJPAU1@J3@Z
utf32 ascii_string_to_utf32_characters(const char *string, const s_escape_table *table, const char **end, long *consumed, utf32 *out, long max, long *length)
{
    const char *next = string + 1;
    long result_length = 1;
    unsigned char c = *string;
    if (table && table->wchar_character == c) {
        bool parsed = false;
        t_unicode_char_sequence_parser parser = 0;
        out->character = wchar_escape_character_to_utf32(string[1], table, &parsed, &parser, 0);
        if (parser)
            parser(string, &next, out, max, &result_length);
        else if (parsed)
            next++;
    } else {
        unsigned int v = c;
        if (v > 127) v = 9633;
        out->character = v;
    }
    *end = next;
    *consumed = next - string;
    *length = result_length;
    return *out;
}

// @address 0x1ab390 ?utf32_can_line_break@@YA_NUutf32@@0@Z
bool utf32_can_line_break(utf32 c1, utf32 c2)
{
    int first_character_code = c1.character;
    int second_character_code = c2.character;
    bool result = false;
    if (first_character_code != 0) {
        bool a = utf32_is_non_beginning_character(second_character_code);
        bool b = utf32_is_non_ending_character(first_character_code);
        result = (utf32_in_list(c1, k_unicode_line_break_ranges, 12) || utf32_is_east_asian_character(first_character_code) ||
                     utf32_is_east_asian_character(second_character_code)) &&
            !a && !b;
    }
    return result;
}


template<typename t_character, long k_maximum_sequence_length>
void string_to_utf32_string(const t_character *string, const s_escape_table *escape_table, utf32 *destination,
    utf32 (*character_parser)(const t_character *, const s_escape_table *, const t_character **, long *, utf32 *, long, long *),
    long destination_count, long *characters_consumed)
{
    const t_character *cursor = string;
    utf32 *output = destination;
    long remaining = destination_count;
    long consumed = 0;
    while (remaining > 0) {
        long character_count;
        utf32 sequence[k_maximum_sequence_length];
        long sequence_length;
        *output = character_parser(cursor, escape_table, &cursor, &character_count, sequence, k_maximum_sequence_length, &sequence_length);
        if (sequence_length >= remaining) {
            output->character = 0;
        } else {
            consumed += character_count;
            if (sequence_length > 1)
                memcpy(output, sequence, sizeof(sequence));
        }
        if (output->character == 0)
            break;
        output += sequence_length;
        remaining -= sequence_length;
    }
    *characters_consumed = consumed;
}

// @address 0x189a68 ?wchar_string_to_utf32_string@@YAXPB_WPBUs_escape_table@@PAUutf32@@JPAJ@Z
void wchar_string_to_utf32_string(
    const wchar_t *string, const s_escape_table *escape_table, utf32 *destination, long destination_count, long *characters_consumed)
{
    // match: 0x189a68: value range assumption guides branch lowering
    __assume(escape_table == k_unicode_escape_table);
    long consumed;
    string_to_utf32_string<wchar_t, 8>(string, escape_table, destination, wchar_string_to_utf32_characters, destination_count, &consumed);
    if (characters_consumed)
        *characters_consumed = consumed;
}

// @address 0x191970 ?ascii_string_to_utf32_string@@YAXPBDPBUs_escape_table@@PAUutf32@@JPAJ@Z
void ascii_string_to_utf32_string(const char *string, const s_escape_table *escape_table, utf32 *destination, long destination_count, long *characters_consumed)
{
    // match: 0x191970: value range assumption guides branch lowering
    __assume(escape_table == k_unicode_escape_table);
    // match: 0x191970: value range assumption guides branch lowering
    __assume(destination_count == 256);
    long consumed;
    string_to_utf32_string<char, 8>(string, escape_table, destination, ascii_string_to_utf32_characters, destination_count, &consumed);
    if (characters_consumed)
        *characters_consumed = consumed;
}
