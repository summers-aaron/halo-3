#ifndef TEXT_LANGUAGE_PACK_H
#define TEXT_LANGUAGE_PACK_H

#include "networking/logic/storage/network_http_request_hash.h"
#include "tag_files/tag_types.h"

// A language pack: the strings of one language, held by the globals tag (s_game_globals::language_packs) and
// read from the cache file by c_language_pack::load_data (text_group.cpp).

struct utf8;
struct s_string_definition;
template<long k_maximum_count> class c_static_wchar_string;

extern "C" bool string_list_get_string(long tag_index, long string_id, wchar_t *string, long maximum_string_length);

// One entry of a language pack's string reference table: the string's id and the offset of its utf8 text in the
// pack's string data (c_language_pack::get_string_utf8).
struct s_language_pack_string_reference
{
    long string_id;
    long offset;
};

typedef char s_language_pack_string_reference_size[(sizeof(s_language_pack_string_reference) == 8) ? 1 : -1];

class c_language_pack
{
public:
    bool load_data();
    TAG_POINTER(s_language_pack_string_reference) strings;
    TAG_POINTER(char) data;
    long count;
    long m_string_data_size;
    long m_string_references_offset;
    long m_string_data_offset;
    s_network_http_request_hash m_string_references_digest;
    s_network_http_request_hash m_string_data_digest;
    long string_count;
private:
    const utf8 *get_string_utf8(long string_id, long offset, long length);
    friend bool language_pack_get_string(c_language_pack *, long, s_string_definition *, long, c_static_wchar_string<256> *);
    friend bool language_pack_get_string_1024(c_language_pack *, long, s_string_definition *, long, c_static_wchar_string<1024> *);
    friend bool string_list_get_string(long, long, wchar_t *, long);
};

typedef char c_language_pack_size[(sizeof(c_language_pack) == 0x44) ? 1 : -1];

#endif // TEXT_LANGUAGE_PACK_H
