#ifndef INTERFACE_USER_INTERFACE_HTML_TEXT_PARSERS_H
#define INTERFACE_USER_INTERFACE_HTML_TEXT_PARSERS_H

#include "interface/game_tag_parser.h"
#include "text/draw_string.h"

// Functions and classes defined in tu/interfacelibxbox/user_interface_html_text_parsers.cpp.
// March user_interface_html_text_parsers.obj owns the parsers' constructors (each passes its
// tag text to c_simple_game_tag_parser); the parsers are constructed by the static initializers of
// user_interface_global_text_parsers.cpp.

// Reads an HTML alignment tag and reports the requested text justification.
extern "C" bool get_html_alignment_change(const wchar_t *html_tag_text, e_text_justification *justification);

class c_html_comment_game_tag_parser : public c_simple_game_tag_parser
{
public:
    c_html_comment_game_tag_parser();
    virtual bool parse_tag(const wchar_t *text, long *parsed, wchar_t *output, long capacity, long *written);
};

class c_html_line_break_game_tag_parser : public c_simple_game_tag_parser
{
public:
    c_html_line_break_game_tag_parser();
    virtual ~c_html_line_break_game_tag_parser();
    virtual bool parse_tag(const wchar_t *text, long *parsed, wchar_t *output, long capacity, long *written);
};

class c_html_paragraph_game_tag_parser : public c_simple_game_tag_parser
{
public:
    c_html_paragraph_game_tag_parser();
    virtual bool parse_tag(const wchar_t *text, long *parsed, wchar_t *output, long capacity, long *written);
};

class c_html_paragraph_close_game_tag_parser : public c_simple_game_tag_parser
{
public:
    c_html_paragraph_close_game_tag_parser();
    virtual bool parse_tag(const wchar_t *text, long *parsed, wchar_t *output, long capacity, long *written);
};

class c_html_div_game_tag_parser : public c_simple_game_tag_parser
{
public:
    c_html_div_game_tag_parser();
    virtual bool parse_tag(const wchar_t *text, long *parsed, wchar_t *output, long capacity, long *written);
};

class c_html_div_close_game_tag_parser : public c_simple_game_tag_parser
{
public:
    c_html_div_close_game_tag_parser();
    virtual bool parse_tag(const wchar_t *text, long *parsed, wchar_t *output, long capacity, long *written);
};

class c_html_font_game_tag_parser : public c_simple_game_tag_parser
{
public:
    c_html_font_game_tag_parser();
    virtual bool parse_tag(const wchar_t *text, long *parsed, wchar_t *output, long capacity, long *written);
};

class c_html_font_close_game_tag_parser : public c_simple_game_tag_parser
{
public:
    c_html_font_close_game_tag_parser();
    virtual bool parse_tag(const wchar_t *text, long *parsed, wchar_t *output, long capacity, long *written);
};

class c_color_game_tag_parser : public c_simple_game_tag_parser
{
public:
    c_color_game_tag_parser();
    virtual bool parse_tag(const wchar_t *text, long *parsed, wchar_t *output, long capacity, long *written);
};

class c_color_close_game_tag_parser : public c_simple_game_tag_parser
{
public:
    c_color_close_game_tag_parser();
    virtual bool parse_tag(const wchar_t *text, long *parsed, wchar_t *output, long capacity, long *written);
};

#endif // INTERFACE_USER_INTERFACE_HTML_TEXT_PARSERS_H
