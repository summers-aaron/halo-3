// interfacelibxbox/user_interface_text_parser.obj: from the records 0x4872e0 0x487390 0x487450 0x4874c0 0x4874d8
// @flags /TP /O2 /Oc
// @address 0x4872e0 ?user_interface_text_parser_unregister@@YAXPAVc_game_tag_parser@@J@Z
#include <string.h>
#include "interface/user_interface_text_parser.h"

// @address 0x57fbe8 ?parse_text@?$c_xml_parser@_W@@QAA_NPB_WPA_WJ@Z
// @address 0x57fe90 ?parse_non_tag@?$c_xml_parser@_W@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x57ff78 ??_G?$c_xml_parser@_W@@UAAPAXI@Z
#include "interface/game_tag_parser.h"

class c_user_interface_text_parser_flags
{
public:
    unsigned long m_bits[8];
    __forceinline void clear() { memset(m_bits, 0, sizeof(m_bits)); }
    __forceinline void set(long bit) { m_bits[bit >> 5] |= 1 << (bit & 31); }
    __forceinline bool test(long bit) const { return (m_bits[bit >> 5] & (1 << (bit & 31))) != 0; }
};

__forceinline void c_user_interface_text_parse_manager::unregister_parser(c_game_tag_parser *parser, long id)
{
    for (long parser_index = 0; parser_index < m_count; parser_index++)
    {
        if (m_parsers[parser_index].parser == parser && m_parsers[parser_index].id == id)
        {
            m_parsers[parser_index].parser = 0;
            for (long shift_index = parser_index; shift_index < m_count; shift_index++)
                m_parsers[shift_index] = m_parsers[shift_index + 1];
            m_count--;
            return;
        }
    }
}

__forceinline bool c_user_interface_text_parse_manager::parse_xml_text(const wchar_t *text, wchar_t *output, long capacity, bool *overflowed, long id)
{
    c_xml_parser<wchar_t> parser;
    long count = 0;
    c_xml_tag_parser<wchar_t> *parsers[256];
    for (long parser_index = 0; parser_index < m_count; parser_index++)
    {
        if (m_parsers[parser_index].id == -1 || m_parsers[parser_index].id == id)
            parsers[count++] = m_parsers[parser_index].parser;
    }
    parser.set_parsers(parsers, count);
    bool result = parser.parse_text(text, output, capacity);
    if (overflowed)
        *overflowed = parser.has_overflowed();
    return result;
}

__forceinline c_user_interface_text_parse_manager::c_user_interface_text_parse_manager()
    : m_count(0)
{
    memset(m_parsers, 0, sizeof(m_parsers));
}

void user_interface_text_parser_unregister(c_game_tag_parser *parser, long id)
{
    if (parser->get_magic_character() >= 0xf7ff && parser->get_magic_character() <= 0xf8fe)
        g_parse_manager.unregister_parser(parser, id);
}

// @address 0x487390 ?user_interface_text_parse_xml@@YA_NPB_WPA_WJPA_NJ@Z
bool user_interface_text_parse_xml(const wchar_t *text, wchar_t *output, long capacity, bool *overflowed, long id)
{
    return g_parse_manager.parse_xml_text(text, output, capacity, overflowed, id);
}

// @address 0x487450 ?parse_tag@c_magic_string_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
bool c_magic_string_game_tag_parser::parse_tag(const wchar_t *text, long *parsed, wchar_t *output, long capacity, long *written)
{
    bool result = false;
    const wchar_t *close = wcschr(text, L'>');
    if (close != 0)
    {
        *parsed = (close - text) + 1;
        *output = get_magic_character();
        *written = 1;
        result = true;
    }
    return result;
}

// @address 0x4874c0 ?get_magic_string@c_magic_string_game_tag_parser@@UAA_NPA_WJ@Z
bool c_magic_string_game_tag_parser::get_magic_string(wchar_t *output, long capacity)
{
    return m_game_data_function(m_context, output, capacity);
}

// @address 0x4874d8 ?register_parser@c_user_interface_text_parse_manager@@QAAXPAVc_game_tag_parser@@J@Z
void c_user_interface_text_parse_manager::register_parser(c_game_tag_parser *parser, long id)
{
    if ((unsigned long)g_parse_manager.m_count < 256)
    {
        bool register_parser = true;
        for (long parser_index = 0; parser_index < g_parse_manager.m_count; parser_index++)
        {
            if (g_parse_manager.m_parsers[parser_index].id == id &&
                wcscmp(g_parse_manager.m_parsers[parser_index].parser->get_tag_text(), parser->get_tag_text()) == 0)
            {
                register_parser = false;
                break;
            }
        }
        if (register_parser)
        {
            c_user_interface_text_parser_flags flags;
            long parser_index = g_parse_manager.m_count;
            flags.clear();
            for (parser_index = 0; parser_index < g_parse_manager.m_count; parser_index++)
            {
                wchar_t magic_character = g_parse_manager.m_parsers[parser_index].parser->get_magic_character();
                long bit = magic_character - 0xf7ff;
                flags.set(bit);
            }
            parser_index = g_parse_manager.m_count;
            g_parse_manager.m_parsers[parser_index].parser = parser;
            g_parse_manager.m_parsers[parser_index].id = id;
            parser->set_magic_character(0);
            parser_index = 0; do { if (!flags.test(parser_index)) break; parser_index++; } while ((unsigned long)parser_index < 256);
            wchar_t magic_character = (wchar_t)(parser_index + 0xf7ff);
            parser->set_magic_character(magic_character);
            g_parse_manager.m_count++;
        }
    }
}

c_user_interface_text_parse_manager g_parse_manager;

