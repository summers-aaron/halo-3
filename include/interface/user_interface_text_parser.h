#ifndef INTERFACE_USER_INTERFACE_TEXT_PARSER_H
#define INTERFACE_USER_INTERFACE_TEXT_PARSER_H

#include "interface/parser_entry_types.h"

// Functions defined in tu/interfacelibxbox/user_interface_text_parser.cpp that other units call.

class c_game_tag_parser;

void user_interface_text_parser_unregister(c_game_tag_parser *parser, long id);
bool user_interface_text_parse_xml(const wchar_t *text, wchar_t *output, long capacity, bool *overflowed, long id);

// Only the manager methods used by other units are declared here.
class c_user_interface_text_parse_manager
{
public:
    c_user_interface_text_parse_manager();
    void register_parser(c_game_tag_parser *parser, long id);
    void unregister_parser(c_game_tag_parser *parser, long id);
    bool parse_xml_text(const wchar_t *text, wchar_t *output, long capacity, bool *overflowed, long id);
    bool parse_magic_character(wchar_t character, wchar_t *text, long capacity);

    long m_count;
    s_parser_entry m_parsers[256];
};

extern c_user_interface_text_parse_manager g_parse_manager;




#endif // INTERFACE_USER_INTERFACE_TEXT_PARSER_H
