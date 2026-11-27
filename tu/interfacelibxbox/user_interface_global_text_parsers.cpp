// interfacelibxbox/user_interface_global_text_parsers.obj: from the records 0x4e36e0 0x4e38d8 0x4e3a10 0x4e3a48 0x4e3aa8 0x4e3b08 0x4e3b68 0x4e3ed0 0x4e4000 0x4e4130 0x4e41a8 0x4e4228
// @flags /TP /O2 /Oc
#include "cseries/cseries_unicode.h"
#include "memory/static_arrays.h"
#include "shell/shell.h"
// @address 0x4e3a10 parse_empty_string
// @address 0x4e4130 parse_year
// @address 0x4e41a8 parse_time
// @address 0x4e4228 parse_short_time
// @address 0x4e36e0 user_interface_register_global_text_parsers
// @address 0x4e38d8 user_interface_unregister_global_text_parsers
// @address 0x4e3a48 ?parse_controller0_gamertag@@YA_NPAXPA_WJ@Z
// @address 0x4e3aa8 ?parse_controller1_gamertag@@YA_NPAXPA_WJ@Z
// @address 0x4e3b08 ?parse_controller2_gamertag@@YA_NPAXPA_WJ@Z
// @address 0x4e3b68 ?parse_controller3_gamertag@@YA_NPAXPA_WJ@Z
// @address 0x4e3ed0 ?parse_month_name@@YA_NPAXPA_WJ@Z
// @address 0x4e4000 ?parse_month@@YA_NPAXPA_WJ@Z
// @address 0x7c3508 g_html_comment_tag_parser
// @address 0x7c3514 g_html_line_break_tag_parser
#include "cseries/cseries.h"
#include "text/unicode.h"
#include "interface/game_tag_parser.h"
#include "interface/user_interface_html_text_parsers.h"
#include "interface/user_interface_text_parser.h"
#include "interface/user_interface_global_text_parsers.h"
#include "interface/user_interface_utilities.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include <time.h>

enum e_calendar_month
{
    _calendar_month_january,
    _calendar_month_february,
    _calendar_month_march,
    _calendar_month_april,
    _calendar_month_may,
    _calendar_month_june,
    _calendar_month_july,
    _calendar_month_august,
    _calendar_month_september,
    _calendar_month_october,
    _calendar_month_november,
    _calendar_month_december
};

extern "C" void user_interface_register_global_text_parsers(void)
{
    g_parse_manager.register_parser(&g_html_comment_tag_parser, -1);
    g_parse_manager.register_parser(&g_html_line_break_tag_parser, -1);
    g_parse_manager.register_parser(&g_html_paragraph_tag_parser, -1);
    g_parse_manager.register_parser(&g_html_paragraph_close_tag_parser, -1);
    g_parse_manager.register_parser(&g_html_div_tag_parser, -1);
    g_parse_manager.register_parser(&g_html_div_close_tag_parser, -1);
    g_parse_manager.register_parser(&g_html_font_tag_parser, -1);
    g_parse_manager.register_parser(&g_html_font_close_tag_parser, -1);
    g_parse_manager.register_parser(&g_color_tag_parser, -1);
    g_parse_manager.register_parser(&g_color_close_tag_parser, -1);
    g_parse_manager.register_parser(&g_build_number_tag_parser, -1);
    g_parse_manager.register_parser(&g_version_number_tag_parser, -1);
    g_parse_manager.register_parser(&g_controller0_gamertag_tag_parser, -1);
    g_parse_manager.register_parser(&g_controller1_gamertag_tag_parser, -1);
    g_parse_manager.register_parser(&g_controller2_gamertag_tag_parser, -1);
    g_parse_manager.register_parser(&g_controller3_gamertag_tag_parser, -1);
    g_parse_manager.register_parser(&g_day_of_week_tag_parser, -1);
    g_parse_manager.register_parser(&g_day_tag_parser, -1);
    g_parse_manager.register_parser(&g_month_name_tag_parser, -1);
    g_parse_manager.register_parser(&g_month_tag_parser, -1);
    g_parse_manager.register_parser(&g_year_tag_parser, -1);
    g_parse_manager.register_parser(&g_time_hms_tag_parser, -1);
    g_parse_manager.register_parser(&g_time_hm_tag_parser, -1);
}

void user_interface_unregister_global_text_parsers(void)
{
    user_interface_text_parser_unregister(&g_html_comment_tag_parser, -1);
    user_interface_text_parser_unregister(&g_html_line_break_tag_parser, -1);
    user_interface_text_parser_unregister(&g_html_paragraph_tag_parser, -1);
    user_interface_text_parser_unregister(&g_html_paragraph_close_tag_parser, -1);
    user_interface_text_parser_unregister(&g_html_div_tag_parser, -1);
    user_interface_text_parser_unregister(&g_html_div_close_tag_parser, -1);
    user_interface_text_parser_unregister(&g_html_font_tag_parser, -1);
    user_interface_text_parser_unregister(&g_html_font_close_tag_parser, -1);
    user_interface_text_parser_unregister(&g_color_tag_parser, -1);
    user_interface_text_parser_unregister(&g_color_close_tag_parser, -1);
    user_interface_text_parser_unregister(&g_build_number_tag_parser, -1);
    user_interface_text_parser_unregister(&g_version_number_tag_parser, -1);
    user_interface_text_parser_unregister(&g_controller0_gamertag_tag_parser, -1);
    user_interface_text_parser_unregister(&g_controller1_gamertag_tag_parser, -1);
    user_interface_text_parser_unregister(&g_controller2_gamertag_tag_parser, -1);
    user_interface_text_parser_unregister(&g_controller3_gamertag_tag_parser, -1);
    user_interface_text_parser_unregister(&g_day_of_week_tag_parser, -1);
    user_interface_text_parser_unregister(&g_day_tag_parser, -1);
    user_interface_text_parser_unregister(&g_month_name_tag_parser, -1);
    user_interface_text_parser_unregister(&g_month_tag_parser, -1);
    user_interface_text_parser_unregister(&g_year_tag_parser, -1);
    user_interface_text_parser_unregister(&g_time_hms_tag_parser, -1);
    user_interface_text_parser_unregister(&g_time_hm_tag_parser, -1);
}


static __forceinline bool user_interface_global_text_parsers_string_get(long string_id, c_static_wchar_string<1024> *string)
{
    return user_interface_global_string_get(string_id, (unsigned short *)string->m_string);
}


// 0x4e3a10
extern "C" bool parse_empty_string(void *context, wchar_t *buffer, long buffer_length)
{
    usnzprintf(buffer, buffer_length, L"");
    return true;
}

// 0x4e3a48
bool parse_controller0_gamertag(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    c_controller_interface *controller = controller_get(_controller0);
    if (controller->in_use())
    {
        ustrnzcpy(buffer, controller->display_name, buffer_length);
        success = true;
    }
    return success;
}

// 0x4e3aa8
bool parse_controller1_gamertag(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    c_controller_interface *controller = controller_get(_controller1);
    if (controller->in_use())
    {
        ustrnzcpy(buffer, controller->display_name, buffer_length);
        success = true;
    }
    return success;
}

// 0x4e3b08
bool parse_controller2_gamertag(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    c_controller_interface *controller = controller_get(_controller2);
    if (controller->in_use())
    {
        ustrnzcpy(buffer, controller->display_name, buffer_length);
        success = true;
    }
    return success;
}

// 0x4e3b68
bool parse_controller3_gamertag(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    c_controller_interface *controller = controller_get(_controller3);
    if (controller->in_use())
    {
        ustrnzcpy(buffer, controller->display_name, buffer_length);
        success = true;
    }
    return success;
}

// 0x4e3ed0
bool parse_month_name(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    __time64_t current_time = _time64(NULL);
    struct tm local_time;
    if (_localtime64_s(&local_time, &current_time) == 0)
    {
        long string_id = NONE;
        switch (local_time.tm_mon)
        {
        case _calendar_month_january: string_id = _user_interface_global_text_parser_string_january; break;
        case _calendar_month_february: string_id = _user_interface_global_text_parser_string_february; break;
        case _calendar_month_march: string_id = _user_interface_global_text_parser_string_march; break;
        case _calendar_month_april: string_id = _user_interface_global_text_parser_string_april; break;
        case _calendar_month_may: string_id = _user_interface_global_text_parser_string_may; break;
        case _calendar_month_june: string_id = _user_interface_global_text_parser_string_june; break;
        case _calendar_month_july: string_id = _user_interface_global_text_parser_string_july; break;
        case _calendar_month_august: string_id = _user_interface_global_text_parser_string_august; break;
        case _calendar_month_september: string_id = _user_interface_global_text_parser_string_september; break;
        case _calendar_month_october: string_id = _user_interface_global_text_parser_string_october; break;
        case _calendar_month_november: string_id = _user_interface_global_text_parser_string_november; break;
        case _calendar_month_december: string_id = _user_interface_global_text_parser_string_december; break;
        }
        if (string_id != NONE)
        {
            c_static_wchar_string<1024> month_string;
            if (user_interface_global_text_parsers_string_get(string_id, &month_string))
            {
                long length = buffer_length;
                if (length > NUMBEROF(month_string.m_string))
                    length = NUMBEROF(month_string.m_string);
                ustrnzcpy(buffer, month_string.get_string(), length);
                success = true;
            }
        }
    }
    return success;
}
// 0x4e4000
bool parse_month(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    __time64_t current_time = _time64(NULL);
    struct tm local_time;
    if (_localtime64_s(&local_time, &current_time) == 0)
    {
        long string_id = NONE;
        switch (local_time.tm_mon)
        {
        case _calendar_month_january: string_id = _user_interface_global_text_parser_string_1; break;
        case _calendar_month_february: string_id = _user_interface_global_text_parser_string_2; break;
        case _calendar_month_march: string_id = _user_interface_global_text_parser_string_3; break;
        case _calendar_month_april: string_id = _user_interface_global_text_parser_string_4; break;
        case _calendar_month_may: string_id = _user_interface_global_text_parser_string_5; break;
        case _calendar_month_june: string_id = _user_interface_global_text_parser_string_6; break;
        case _calendar_month_july: string_id = _user_interface_global_text_parser_string_7; break;
        case _calendar_month_august: string_id = _user_interface_global_text_parser_string_8; break;
        case _calendar_month_september: string_id = _user_interface_global_text_parser_string_9; break;
        case _calendar_month_october: string_id = _user_interface_global_text_parser_string_10; break;
        case _calendar_month_november: string_id = _user_interface_global_text_parser_string_11; break;
        case _calendar_month_december: string_id = _user_interface_global_text_parser_string_12; break;
        }
        if (string_id != NONE)
        {
            c_static_wchar_string<1024> month_string;
            if (user_interface_global_text_parsers_string_get(string_id, &month_string))
            {
                long length = buffer_length;
                if (length > NUMBEROF(month_string.m_string))
                    length = NUMBEROF(month_string.m_string);
                ustrnzcpy(buffer, month_string.get_string(), length);
                success = true;
            }
        }
    }
    return success;
}

// 0x4e4130
extern "C" bool parse_year(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    __time64_t current_time = _time64(NULL);
    struct tm local_time;
    if (_localtime64_s(&local_time, &current_time) == 0)
    {
        usnzprintf(buffer, buffer_length, L"%d", local_time.tm_year + 1900);
        success = buffer[0] != 0;
    }
    return success;
}

// 0x4e41a8
extern "C" bool parse_time(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    __time64_t current_time = _time64(NULL);
    struct tm local_time;
    if (_localtime64_s(&local_time, &current_time) == 0)
    {
        usnzprintf(buffer, buffer_length, L"%02d:%02d:%02d", local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
        success = buffer[0] != 0;
    }
    return success;
}

// 0x4e4228
extern "C" bool parse_short_time(void *context, wchar_t *buffer, long buffer_length)
{
    bool success = false;
    __time64_t current_time = _time64(NULL);
    struct tm local_time;
    if (_localtime64_s(&local_time, &current_time) == 0)
    {
        usnzprintf(buffer, buffer_length, L"%02d:%02d", local_time.tm_hour, local_time.tm_min);
        success = buffer[0] != 0;
    }
    return success;
}

__forceinline c_html_comment_game_tag_parser::c_html_comment_game_tag_parser() : c_simple_game_tag_parser(L"<!--") {}
__forceinline c_html_line_break_game_tag_parser::c_html_line_break_game_tag_parser() : c_simple_game_tag_parser(L"<br") {}
__forceinline c_html_paragraph_game_tag_parser::c_html_paragraph_game_tag_parser() : c_simple_game_tag_parser(L"<p") {}
__forceinline c_html_paragraph_close_game_tag_parser::c_html_paragraph_close_game_tag_parser() : c_simple_game_tag_parser(L"</p") {}
__forceinline c_html_div_game_tag_parser::c_html_div_game_tag_parser() : c_simple_game_tag_parser(L"<div") {}
__forceinline c_html_div_close_game_tag_parser::c_html_div_close_game_tag_parser() : c_simple_game_tag_parser(L"</div") {}
__forceinline c_html_font_game_tag_parser::c_html_font_game_tag_parser() : c_simple_game_tag_parser(L"<font") {}
__forceinline c_html_font_close_game_tag_parser::c_html_font_close_game_tag_parser() : c_simple_game_tag_parser(L"</font") {}
__forceinline c_color_game_tag_parser::c_color_game_tag_parser() : c_simple_game_tag_parser(L"<color") {}
__forceinline c_color_close_game_tag_parser::c_color_close_game_tag_parser() : c_simple_game_tag_parser(L"</color") {}

extern "C" {
c_html_comment_game_tag_parser g_html_comment_tag_parser;
c_html_line_break_game_tag_parser g_html_line_break_tag_parser;
c_html_paragraph_game_tag_parser g_html_paragraph_tag_parser;
c_html_paragraph_close_game_tag_parser g_html_paragraph_close_tag_parser;
c_html_div_game_tag_parser g_html_div_tag_parser;
c_html_div_close_game_tag_parser g_html_div_close_tag_parser;
c_html_font_game_tag_parser g_html_font_tag_parser;
c_html_font_close_game_tag_parser g_html_font_close_tag_parser;
c_color_game_tag_parser g_color_tag_parser;
c_color_close_game_tag_parser g_color_close_tag_parser;
}

c_magic_string_game_tag_parser g_build_number_tag_parser = c_magic_string_game_tag_parser(L"<build_number", NULL, parse_empty_string);
c_magic_string_game_tag_parser g_version_number_tag_parser = c_magic_string_game_tag_parser(L"<version_number", NULL, parse_empty_string);
c_magic_string_game_tag_parser g_controller0_gamertag_tag_parser = c_magic_string_game_tag_parser(L"<controller0_gamertag", NULL, parse_controller0_gamertag);
c_magic_string_game_tag_parser g_controller1_gamertag_tag_parser = c_magic_string_game_tag_parser(L"<controller1_gamertag", NULL, parse_controller1_gamertag);
c_magic_string_game_tag_parser g_controller2_gamertag_tag_parser = c_magic_string_game_tag_parser(L"<controller2_gamertag", NULL, parse_controller2_gamertag);
c_magic_string_game_tag_parser g_controller3_gamertag_tag_parser = c_magic_string_game_tag_parser(L"<controller3_gamertag", NULL, parse_controller3_gamertag);
c_magic_string_game_tag_parser g_day_of_week_tag_parser = c_magic_string_game_tag_parser(L"<day_of_week", NULL, parse_current_day_of_week);
c_magic_string_game_tag_parser g_day_tag_parser = c_magic_string_game_tag_parser(L"<day", NULL, user_interface_get_month_day_name);
c_magic_string_game_tag_parser g_month_name_tag_parser = c_magic_string_game_tag_parser(L"<month_name", NULL, parse_month_name);
c_magic_string_game_tag_parser g_month_tag_parser = c_magic_string_game_tag_parser(L"<month", NULL, parse_month);
c_magic_string_game_tag_parser g_year_tag_parser = c_magic_string_game_tag_parser(L"<year", NULL, parse_year);
c_magic_string_game_tag_parser g_time_hms_tag_parser = c_magic_string_game_tag_parser(L"<time_hms", NULL, parse_time);
c_magic_string_game_tag_parser g_time_hm_tag_parser = c_magic_string_game_tag_parser(L"<time_hm", NULL, parse_short_time);

