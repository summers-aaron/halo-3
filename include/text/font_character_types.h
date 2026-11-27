#ifndef INCLUDE_TEXT_FONT_CHARACTER_TYPES_H
#define INCLUDE_TEXT_FONT_CHARACTER_TYPES_H


struct s_font_character
{
    short advance;
    unsigned short packed_size;
    short width;
    short height;
    short bearing;
    short top;
    // The packed pixel data follows the header.
};
#endif
