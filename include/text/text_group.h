#ifndef TEXT_TEXT_GROUP_H
#define TEXT_TEXT_GROUP_H

#include <stddef.h>

#include "tag_files/tag_types.h"
#include "networking/logic/storage/network_http_request_hash.h"

// Functions defined in tu/blamlibxbox/text_group.cpp.

extern "C" bool text_group_verify_digest(const void *data, long size, bool salted,
    const s_network_http_request_hash *expected, s_network_http_request_hash *actual);
extern "C" bool multilingual_unicode_string_list_test_string(long tag_index, long string_id);
extern "C" char *text_group_get_digest_string(const s_network_http_request_hash *digest, char *buffer, long size);

#include "text/language_pack.h"

// A run of the language pack's string references: the strings one unic tag holds in one language.
struct s_string_reference
{
    short offset;
    short length;
};

// The unic (multilingual unicode string list) tag. Only the per-language runs are used; the strings
// themselves live in the language packs of the globals tag. The block and data ahead of them are unread here.
struct s_string_definition
{
    s_tag_block string_references;
    s_tag_data string_data_utf8;
    s_string_reference languages[12];
};

typedef char s_string_definition_languages_offset[(sizeof(void *) != 4 || offsetof(s_string_definition, languages) == 0x20) ? 1 : -1];

extern "C" bool string_list_get_normal_string(long string_list_index, long string_id, c_static_wchar_string<256> *string);
extern "C" bool string_list_get_normal_string_by_language(long tag_index, long string_id, long language, c_static_wchar_string<256> *string);
extern "C" bool text_group_get_string(long tag_index, long string_id, c_static_wchar_string<1024> *string);
extern "C" bool string_list_get_interface_string_by_language(long tag_index, long string_id, long language, c_static_wchar_string<1024> *string);

// The language whose pack is loaded (main_game.cpp).
extern long g_loaded_language_index;

#endif // TEXT_TEXT_GROUP_H
