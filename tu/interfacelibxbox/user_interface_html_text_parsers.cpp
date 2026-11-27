// interfacelibxbox/user_interface_html_text_parsers.obj: from the records 0x4f68d8 0x4f6950 0x4f69a8 0x4f6a20 0x4f6b28 0x4f6bd0 0x4f6ca8 0x4f6d48 0x4f6ef0 0x4f6f68 0x4f7080 0x4f70f8
// @flags /TP /O2
// @address 0x4f69a8 ?parse_tag@c_html_line_break_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x4f6a20 ?parse_tag@c_html_paragraph_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x4f6b28 ?parse_tag@c_html_paragraph_close_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x4f6ca8 ?parse_tag@c_html_div_close_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x4f6ef0 ?parse_tag@c_html_font_close_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x4f70f8 get_html_alignment_change
// @address 0x4f7080 ?parse_tag@c_color_close_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x4f68d8 ?parse_tag@c_html_comment_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
// @address 0x4f6950 ??_Gc_html_line_break_game_tag_parser@@UAAPAXI@Z
#include <wchar.h>
#include <string.h>
#include "interface/user_interface_html_text_parsers.h"

bool c_html_comment_game_tag_parser::parse_tag(const wchar_t *text, long *consumed, wchar_t *characters, long room, long *handled)
{
    bool result = true;
    wchar_t terminator[] = L"-->";
    const wchar_t *end = wcsstr(text, terminator);
    if (end) {
        end += 3;
        *consumed = (long)(end - text);
    } else {
        *consumed = wcslen(text);
    }
    return result;
}

c_html_line_break_game_tag_parser::~c_html_line_break_game_tag_parser()
{
}

bool c_html_line_break_game_tag_parser::parse_tag(const wchar_t *text, long *consumed, wchar_t *characters, long room, long *handled)
{
    bool result = false;
    const wchar_t *end = wcschr(text, L'>');
    if (end != 0 && room >= 1) {
        *characters = 10;
        *consumed = (long)(end - text) + 1;
        *handled = 1;
        result = true;
    }
    return result;
}

bool c_html_paragraph_game_tag_parser::parse_tag(const wchar_t *text, long *consumed, wchar_t *characters, long room, long *handled)
{
    bool result = false;
    const wchar_t *end = wcschr(text, L'>');
    if (end != 0) {
        wchar_t paragraph_break[] = L"\n\n";
        long i;
        if (room >= 2) {
            for (i = 0; i < 2; i++)
                characters[i] = paragraph_break[i];
            *consumed = (long)(end - text) + 1;
            *handled = 2;
            e_text_justification justification;
            if (get_html_alignment_change(text, &justification)) {
                if (i < room) {
                    switch (justification) {
                    case _text_justification_left:
                        characters[i++] = (wchar_t)-7163;
                        break;
                    case _text_justification_right:
                        characters[i++] = (wchar_t)-7162;
                        break;
                    case _text_justification_center:
                        characters[i++] = (wchar_t)-7161;
                        break;
                    }
                    *handled = *handled + 1;
                }
            }
            result = true;
        }
    }
    return result;
}
bool c_html_paragraph_close_game_tag_parser::parse_tag(const wchar_t *text, long *consumed, wchar_t *characters, long room, long *handled)
{
    bool result = false;
    const wchar_t *end = wcschr(text, L'>');
    if (end != 0) {
        wchar_t paragraph_break[] = { (wchar_t)-7158, 10, 10, 0 };
        paragraph_break[3] = 0;
        if (room >= 3) {
            for (long i = 0; i < 3; i++)
                characters[i] = paragraph_break[i];
            *consumed = (long)(end - text) + 1;
            *handled = 3;
            result = true;
        }
    }
    return result;
}

bool c_html_div_game_tag_parser::parse_tag(const wchar_t *text, long *consumed,
    wchar_t *characters, long room, long *handled)
{
    bool result = false;
    const wchar_t *end = wcschr(text, L'>');
    if (end) {
        wchar_t newline[] = L"\n";
        const long newline_length = 1;
        if (room >= newline_length) {
            long character_index;
            for (character_index = 0; character_index < newline_length; character_index++)
                characters[character_index] = newline[character_index];
            *consumed = (long)(end - text) + 1;
            *handled = newline_length;
            e_text_justification justification;
            if (get_html_alignment_change(text, &justification)) {
                if (character_index < room) {
                    switch (justification) {
                    case _text_justification_left:
                        characters[character_index++] = (wchar_t)-7163;
                        break;
                    case _text_justification_right:
                        characters[character_index++] = (wchar_t)-7162;
                        break;
                    case _text_justification_center:
                        characters[character_index++] = (wchar_t)-7161;
                        break;
                    }
                    (*handled)++;
                }
            }
            result = true;
        }
    }
    return result;
}

bool c_html_div_close_game_tag_parser::parse_tag(const wchar_t *text, long *consumed, wchar_t *characters, long room, long *handled)
{
    bool result = false;
    const wchar_t *end = wcschr(text, L'>');
    if (end != 0) {
        wchar_t div_break[] = { (wchar_t)-7158, 10, 0 };
        div_break[2] = 0;
        if (room >= 2) {
            for (long i = 0; i < 2; i++)
                characters[i] = div_break[i];
            *consumed = (long)(end - text) + 1;
            *handled = 2;
            result = true;
        }
    }
    return result;
}

bool c_html_font_close_game_tag_parser::parse_tag(const wchar_t *text, long *consumed, wchar_t *characters, long room, long *handled)
{
    bool result = false;
    const wchar_t *end = wcschr(text, L'>');
    if (end != 0 && room >= 1) {
        *characters = -7160;
        *consumed = (long)(end - text) + 1;
        *handled = 1;
        result = true;
    }
    return result;
}

bool c_color_close_game_tag_parser::parse_tag(const wchar_t *text, long *consumed, wchar_t *characters, long room, long *handled)
{
    bool result = false;
    const wchar_t *end = wcschr(text, L'>');
    if (end != 0 && room >= 1) {
        *characters = -7159;
        *consumed = (long)(end - text) + 1;
        *handled = 1;
        result = true;
    }
    return result;

}


// @address 0x4f6d48 ?parse_tag@c_html_font_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
bool c_html_font_game_tag_parser::parse_tag(const wchar_t *text, long *consumed, wchar_t *characters, long room, long *handled)
{
    bool result = false;
    const wchar_t *end = wcschr(text, L'>');
    if (end != 0) {
        int idx[2];
        struct { const wchar_t *names[12]; wchar_t buf[16]; wchar_t face[64]; } L;
        bool found = false;
        unsigned int i;
        L.names[0] = L"terminal";
        L.names[1] = L"body_text";
        L.names[2] = L"title";
        L.names[3] = L"super_large";
        L.names[4] = L"large_body";
        L.names[5] = L"split_screen_hud_message";
        L.names[6] = L"full_screen_hud_message";
        L.names[7] = L"english_body_text";
        L.names[8] = L"hud_number";
        L.names[9] = L"subtitle";
        L.names[10] = L"main_menu";
        L.buf[0] = 0;
        memset(&L.buf[1], 0, sizeof(L.buf) - sizeof(L.buf[0]));
        if (c_xml_tag_parser<wchar_t>::xml_get_string_attribute(text, L"face", L.face, 64)) {
            for (i = 0; i < 11; i++) {
                if (_wcsnicmp(L.face, L.names[i], 64) == 0) {
                    idx[0] = i;
                    found = true;
                    break;
                }
            }
        }
        if (found) {
            L.buf[0] = (wchar_t)-0x1d00;
            L.buf[1] = (wchar_t)(idx[0] | 0xa100);
            if (room >= 2) {
                for (i = 0; i < 2; i++)
                    characters[i] = L.buf[i];
                *consumed = (long)(end - text) + 1;
                *handled = 2;
                result = true;
            }
        }
    }
    return result;
}

// @address 0x4f6f68 ?parse_tag@c_color_game_tag_parser@@UAA_NPB_WPAJPA_WJ1@Z
bool c_color_game_tag_parser::parse_tag(const wchar_t *text, long *consumed, wchar_t *characters, long room, long *handled)
{
    bool result = false;
    const wchar_t *end = wcschr(text, L'>');
    if (end != 0) {
        struct { unsigned long argb; unsigned int pad[3]; wchar_t buf[16]; } L;
        bool found = false;
        unsigned int i;
        L.buf[0] = 0;
        memset(&L.buf[1], 0, sizeof(L.buf) - sizeof(L.buf[0]));
        if (c_xml_tag_parser<wchar_t>::xml_get_hexidecimal_attribute(text, L"argb", &L.argb))
            found = true;
        if (found) {
            L.buf[0] = (wchar_t)-0x1cff;
            L.buf[1] = (wchar_t)((L.argb >> 24) | 0xb100);
            L.buf[2] = (wchar_t)(((L.argb >> 16) & 255) | 0xb100);
            L.buf[3] = (wchar_t)(((L.argb >> 8) & 255) | 0xb100);
            L.buf[4] = (wchar_t)((L.argb & 255) | 0xb100);
            if (room >= 5) {
                for (i = 0; i < 5; i++)
                    characters[i] = L.buf[i];
                *consumed = (long)(end - text) + 1;
                *handled = 5;
                result = true;
            }
        }
    }
    return result;
}

bool get_html_alignment_change(const wchar_t *html_tag_text, e_text_justification *justification)
{
    bool result = false;
    wchar_t align[32];
    if (c_xml_tag_parser<wchar_t>::xml_get_string_attribute(html_tag_text, L"align", align, 32)) {
        result = true;
        if (_wcsnicmp(align, L"left", 32) == 0)
            *justification = _text_justification_left;
        else if (_wcsnicmp(align, L"center", 32) == 0)
            *justification = _text_justification_center;
        else if (_wcsnicmp(align, L"right", 32) == 0)
            *justification = _text_justification_right;
        else
            result = false;
    }
    return result;
}
