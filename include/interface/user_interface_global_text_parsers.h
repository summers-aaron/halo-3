#ifndef INTERFACE_USER_INTERFACE_GLOBAL_TEXT_PARSERS_H
#define INTERFACE_USER_INTERFACE_GLOBAL_TEXT_PARSERS_H

// Functions defined in tu/interfacelibxbox/user_interface_global_text_parsers.cpp that other units call.

extern "C" void user_interface_register_global_text_parsers(void);
extern "C" void user_interface_unregister_global_text_parsers(void);

// The parsers user_interface_register_global_text_parsers registers. March keeps them as file statics of
// user_interface_global_text_parsers.cpp (the map has no names for them; their constructors come from
// the HTML and text parsers). They are globals,
// constant-initialized. Each is named after the tag text it parses. The classes are in
// interface/user_interface_html_text_parsers.h and interface/game_tag_parser.h.
class c_html_comment_game_tag_parser;
class c_html_line_break_game_tag_parser;
class c_html_paragraph_game_tag_parser;
class c_html_paragraph_close_game_tag_parser;
class c_html_div_game_tag_parser;
class c_html_div_close_game_tag_parser;
class c_html_font_game_tag_parser;
class c_html_font_close_game_tag_parser;
class c_color_game_tag_parser;
class c_color_close_game_tag_parser;
class c_magic_string_game_tag_parser;

extern "C" {
extern c_html_comment_game_tag_parser g_html_comment_tag_parser;                 // <!--
extern c_html_line_break_game_tag_parser g_html_line_break_tag_parser;           // <br
extern c_html_paragraph_game_tag_parser g_html_paragraph_tag_parser;             // <p
extern c_html_paragraph_close_game_tag_parser g_html_paragraph_close_tag_parser; // </p
extern c_html_div_game_tag_parser g_html_div_tag_parser;                         // <div
extern c_html_div_close_game_tag_parser g_html_div_close_tag_parser;             // </div
extern c_html_font_game_tag_parser g_html_font_tag_parser;                       // <font
extern c_html_font_close_game_tag_parser g_html_font_close_tag_parser;           // </font
extern c_color_game_tag_parser g_color_tag_parser;                               // <color
extern c_color_close_game_tag_parser g_color_close_tag_parser;                   // </color
}

extern c_magic_string_game_tag_parser g_build_number_tag_parser;                 // <build_number
extern c_magic_string_game_tag_parser g_version_number_tag_parser;               // <version_number
extern c_magic_string_game_tag_parser g_controller0_gamertag_tag_parser;         // <controller0_gamertag
extern c_magic_string_game_tag_parser g_controller1_gamertag_tag_parser;         // <controller1_gamertag
extern c_magic_string_game_tag_parser g_controller2_gamertag_tag_parser;         // <controller2_gamertag
extern c_magic_string_game_tag_parser g_controller3_gamertag_tag_parser;         // <controller3_gamertag
extern c_magic_string_game_tag_parser g_day_of_week_tag_parser;                  // <day_of_week
extern c_magic_string_game_tag_parser g_day_tag_parser;                          // <day
extern c_magic_string_game_tag_parser g_month_name_tag_parser;                   // <month_name
extern c_magic_string_game_tag_parser g_month_tag_parser;                        // <month
extern c_magic_string_game_tag_parser g_year_tag_parser;                         // <year
extern c_magic_string_game_tag_parser g_time_hms_tag_parser;                     // <time_hms
extern c_magic_string_game_tag_parser g_time_hm_tag_parser;                      // <time_hm


// String ids parse_month_name and parse_month look up in the global string list,
// named after their strings in the cache string table.
enum e_user_interface_global_text_parser_string_id
{
    _user_interface_global_text_parser_string_january = 300,
    _user_interface_global_text_parser_string_february = 301,
    _user_interface_global_text_parser_string_march = 302,
    _user_interface_global_text_parser_string_april = 303,
    _user_interface_global_text_parser_string_may = 304,
    _user_interface_global_text_parser_string_june = 305,
    _user_interface_global_text_parser_string_july = 306,
    _user_interface_global_text_parser_string_august = 307,
    _user_interface_global_text_parser_string_september = 308,
    _user_interface_global_text_parser_string_october = 309,
    _user_interface_global_text_parser_string_november = 310,
    _user_interface_global_text_parser_string_december = 311,
    _user_interface_global_text_parser_string_1 = 313,
    _user_interface_global_text_parser_string_2 = 314,
    _user_interface_global_text_parser_string_3 = 315,
    _user_interface_global_text_parser_string_4 = 316,
    _user_interface_global_text_parser_string_5 = 317,
    _user_interface_global_text_parser_string_6 = 318,
    _user_interface_global_text_parser_string_7 = 319,
    _user_interface_global_text_parser_string_8 = 320,
    _user_interface_global_text_parser_string_9 = 321,
    _user_interface_global_text_parser_string_10 = 322,
    _user_interface_global_text_parser_string_11 = 323,
    _user_interface_global_text_parser_string_12 = 324,
};

#endif // INTERFACE_USER_INTERFACE_GLOBAL_TEXT_PARSERS_H
