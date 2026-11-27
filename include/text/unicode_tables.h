#ifndef TEXT_UNICODE_TABLES_H
#define TEXT_UNICODE_TABLES_H

#include "text/escape_table_types.h"
#include "text/utf32_range_types.h"

// Shared by Unicode conversion and the draw-string parser. Address tags stay
// in unicode.cpp, which supplies the conversion routines using these tables.
extern "C"
{
    extern const s_escape_table k_unicode_escape_table[];
    extern const s_utf32_range k_unicode_line_break_ranges[];
}

#endif // TEXT_UNICODE_TABLES_H
