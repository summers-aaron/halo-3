#ifndef TEXT_ESCAPE_TABLE_TYPES_H
#define TEXT_ESCAPE_TABLE_TYPES_H

struct utf32;

typedef void (*t_unicode_char_sequence_parser)(const char *, const char **, utf32 *, long, long *);
typedef void (*t_unicode_sequence_parser)(const wchar_t *, const wchar_t **, utf32 *, long, long *);

struct s_escape_table
{
    unsigned short wchar_character;
    unsigned int unicode_character;
    t_unicode_char_sequence_parser ascii_sequence_parser;
    t_unicode_sequence_parser wchar_sequence_parser;
};

#endif // TEXT_ESCAPE_TABLE_TYPES_H
