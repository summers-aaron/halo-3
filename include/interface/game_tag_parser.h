#ifndef INTERFACE_GAME_TAG_PARSER_H
#define INTERFACE_GAME_TAG_PARSER_H

#include "text/xml_parser.h"

// March user_interface_text_parser.obj owns these classes. Its asserts name the
// .cpp (188, 222-223), not a header; this leaf keeps the definitions independent
// of the existing function-only user_interface_text_parser.h.
class c_game_tag_parser : public c_xml_tag_parser<wchar_t>
{
public:
    __forceinline c_game_tag_parser(const wchar_t *tag_text)
        : c_xml_tag_parser<wchar_t>(tag_text), m_tag(0) {}
    virtual __forceinline ~c_game_tag_parser() {}
    __forceinline wchar_t get_magic_character() { return m_tag; }
    __forceinline void set_magic_character(wchar_t tag) { m_tag = tag; }

protected:
    wchar_t m_tag;
};

// March user_interface_text_parser.obj: the base of the html and color tag parsers
// (user_interface_html_text_parsers.cpp). It adds no data, and get_magic_string returns
// false by default.
// It declares parse_tag without implementing it,
// so it is left to the derived parsers here.
class c_simple_game_tag_parser : public c_game_tag_parser
{
public:
    __forceinline c_simple_game_tag_parser(const wchar_t *tag_text)
        : c_game_tag_parser(tag_text) {}
    virtual __forceinline ~c_simple_game_tag_parser() {}
    virtual bool get_magic_string(wchar_t *output, long capacity) { return false; }
};

class c_magic_string_game_tag_parser : public c_game_tag_parser
{
public:
    __forceinline c_magic_string_game_tag_parser(const wchar_t *tag_text, void *context,
        bool (*game_data_function)(void *, wchar_t *, long))
        : c_game_tag_parser(tag_text), m_context(context), m_game_data_function(game_data_function)
    {
        // March: ASSERT(m_game_data_function != NULL).
    }
    virtual __forceinline ~c_magic_string_game_tag_parser() {}
    virtual bool parse_tag(const wchar_t *text, long *parsed, wchar_t *output, long capacity, long *written);
    virtual bool get_magic_string(wchar_t *output, long capacity);

private:
    void *m_context;
    bool (*m_game_data_function)(void *, wchar_t *, long);
};

typedef char c_game_tag_parser_size[(sizeof(void *) != 4 || sizeof(c_game_tag_parser) == 0xc) ? 1 : -1];
typedef char c_magic_string_game_tag_parser_size[(sizeof(void *) != 4 || sizeof(c_magic_string_game_tag_parser) == 0x14) ? 1 : -1];

#endif // INTERFACE_GAME_TAG_PARSER_H
