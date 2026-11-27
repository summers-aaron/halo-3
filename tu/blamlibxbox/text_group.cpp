// blamlibxbox/text_group.obj: from the records 0x141d58 0x19f168 0x1bf810 0x1bf918 0x1bf968 0x1bfa40 0x1bfa90 0x1bfb68 0x1bfc48 0x1bfdb0 0x1bfe40 0x1bfeb8
// @flags /TP /O2 /Oc
// hand merge (finish-object): one declaration per global and type; lifted pointer arithmetic on them written as casts
// @address 0x19f168 text_group_get_digest_string
// @address 0x1bf810 multilingual_unicode_string_list_test_string
// @address 0x1bf918 string_list_get_normal_string
// @address 0x1bf968 string_list_get_normal_string_by_language
// @address 0x1bfa40 text_group_get_string
// @address 0x1bfa90 string_list_get_interface_string_by_language
// @address 0x1bfb68 string_list_get_string
// @address 0x1bfe40 ?set@?$c_static_wchar_string@$0BAA@@@QAAXPB_W@Z
// @address 0x1bfeb8 ?set@?$c_static_wchar_string@$0EAA@@@QAAXPB_W@Z
#include "tag_files/tag_groups.h"
#include <ppcintrinsics.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "memory/static_arrays.h"
#include "text/unicode.h"
#include "cache/cache_files.h"
#include "cache/security_functions.h"
class c_language_pack;
struct s_string_definition;
static __forceinline bool language_pack_get_string(c_language_pack *, long, s_string_definition *, long, c_static_wchar_string<256> *);
static __forceinline bool language_pack_get_string_1024(c_language_pack *, long, s_string_definition *, long, c_static_wchar_string<1024> *);
#include "text/text_group.h"
#include "cseries/cseries.h"
#include "cseries/language.h"
#include "memory/physical_memory_map.h"
#include "networking/session/network_session_parameters.h"
#include "networking/logic/storage/network_http_request_hash.h"
#include "game/game_globals.h"

// @address 0x74b540 g_current_language
#include "scenario/scenario.h"
#include "tag_files/tag_types.h"


static __forceinline bool language_pack_test_string(c_language_pack *pack, long string_id, long offset, long length)
{
    bool result = false;
    if (pack->string_count) {
        for (long index = offset; index < offset + length; ++index) {
            if (index >= 0 && index < pack->count && pack->strings[index].string_id == string_id) {
                result = true;
                break;
            }
        }
    }
    return result;
}
static __forceinline bool language_pack_get_string(c_language_pack *pack, long string_id,
    s_string_definition *definition, long language, c_static_wchar_string<256> *string)
{
    bool result = false;
    if (pack->string_count) {
        const utf8 *string_utf8 = pack->get_string_utf8(string_id, definition->languages[language].offset, definition->languages[language].length);
        if (string_utf8) {
            wchar_t buffer[256];
            utf8_string_to_wchar_string(string_utf8, buffer, 256, NULL);
            string->set(buffer);
            result = true;
        }
    }
    return result;
}
static __forceinline bool language_pack_get_string_1024(c_language_pack *pack, long string_id,
    s_string_definition *definition, long language, c_static_wchar_string<1024> *string)
{
    bool result = false;
    if (pack->string_count) {
        const utf8 *string_utf8 = pack->get_string_utf8(string_id, definition->languages[language].offset, definition->languages[language].length);
        if (string_utf8) {
            wchar_t buffer[1024];
            utf8_string_to_wchar_string(string_utf8, buffer, 1024, NULL);
            string->set(buffer);
            result = true;
        }
    }
    return result;
}
static __forceinline long text_group_round_up_read_size(long size)
{
    long read_size = size;
    if (size & 4095)
        read_size = (size | 4095) + 1;
    return read_size;
}

// @address 0x19f058 ?security_calculate_hash@@YA_NPBDK_NPAUs_network_http_request_hash@@@Z

// @address 0x141d58 text_group_verify_digest
extern "C" bool text_group_verify_digest(const void *data, long size, bool salted,
    const s_network_http_request_hash *expected, s_network_http_request_hash *actual)
{
    bool result = false;
    s_network_http_request_hash digest;
    memset(&digest, 0, sizeof(digest));
    if (security_calculate_hash((const char *)data, size, salted, &digest)) {
        if (memcmp(&digest, expected, sizeof(digest)) == 0)
            result = true;
    }
    if (actual)
        memcpy(actual, &digest, sizeof(digest));
    return result;
}

// 0x19f168
// every caller passes size 256 (the digest is 20 bytes, 40 hex digits)
static const long text_group_digest_string_size = 256;
char *text_group_get_digest_string(const s_network_http_request_hash *digest, char *buffer, long size)
{
    csstrnzcpy(buffer, "", text_group_digest_string_size);
    for (long index = 0; index < 20; index++)
        csnzappendf(buffer, text_group_digest_string_size, "%02X", digest->hash[index]);
    return buffer;
}

extern "C" bool multilingual_unicode_string_list_test_string(long tag_index, long string_id)
{
    bool result = false;
    if (tag_index != -1) {
        long language = get_current_language();
        if (language == g_loaded_language_index) {
            s_string_definition *definition = TAG_GET('unic', s_string_definition, tag_index);
            long offset = definition->languages[language].offset;
            long length = definition->languages[language].length;
            c_language_pack *pack = &global_game_globals->language_packs[language];
            result = language_pack_test_string(pack, string_id, offset, length);
        }
    }
    return result;
}

bool string_list_get_normal_string(long string_list_index, long string_id, c_static_wchar_string<256> *string)
{
    return string_list_get_normal_string_by_language(string_list_index, string_id, get_current_language(), string);
}

bool string_list_get_normal_string_by_language(long tag_index, long string_id, long language, c_static_wchar_string<256> *string)
{
    bool result = false;
    if (tag_index != -1 && language == g_loaded_language_index) {
        c_language_pack *pack = &global_game_globals->language_packs[language];
        s_string_definition *definition = TAG_GET('unic', s_string_definition, tag_index);
        result = language_pack_get_string(pack, string_id, definition, language, string);
    }
    return result;
}

bool text_group_get_string(long tag_index, long string_id, c_static_wchar_string<1024> *string)
{
    return string_list_get_interface_string_by_language(tag_index, string_id, get_current_language(), string);
}

bool string_list_get_interface_string_by_language(long tag_index, long string_id, long language, c_static_wchar_string<1024> *string)
{
    bool result = false;
    if (tag_index != -1 && language == g_loaded_language_index) {
        c_language_pack *pack = &global_game_globals->language_packs[language];
        s_string_definition *definition = TAG_GET('unic', s_string_definition, tag_index);
        result = language_pack_get_string_1024(pack, string_id, definition, language, string);
    }
    return result;
}

bool string_list_get_string(long tag_index, long string_id, wchar_t *string, long maximum_string_length)
{
    long language = get_current_language();
    bool result = false;
    if (tag_index != -1 && language == g_loaded_language_index) {
        c_language_pack *pack = &global_game_globals->language_packs[language];
        s_string_definition *definition = TAG_GET('unic', s_string_definition, tag_index);
        bool success = false;
        if (pack->string_count) {
            const utf8 *string_utf8 = pack->get_string_utf8(string_id, definition->languages[language].offset,
                definition->languages[language].length);
            if (string_utf8) {
                utf8_string_to_wchar_string(string_utf8, string, 256, NULL);
                success = true;
            }
        }
        result = success;
    }
    return result;
}

// @address 0x1bfc48 ?load_data@c_language_pack@@QAA_NXZ
bool c_language_pack::load_data()
{
    bool result = false;
    struct
    {
        long computed_digests[16];
        char digest_strings[4][256];
    } scratch;

    strings = (s_language_pack_string_reference *)_physical_memory_malloc((memory_stage)5, NULL, count * sizeof(s_language_pack_string_reference), 0, NULL, -1);
    cache_file_blocking_read(3, m_string_references_offset, text_group_round_up_read_size(count * sizeof(s_language_pack_string_reference)), strings);
    data = (char *)_physical_memory_malloc((memory_stage)5, NULL, m_string_data_size, 0, NULL, -1);
    cache_file_blocking_read(3, m_string_data_offset, text_group_round_up_read_size(m_string_data_size), data);

    bool digests_valid = true;
    long string_references_size = count * sizeof(s_language_pack_string_reference);
    digests_valid &= text_group_verify_digest(strings, string_references_size, false,
        &m_string_references_digest, (s_network_http_request_hash *)&scratch.computed_digests[8]);
    digests_valid &= text_group_verify_digest(data, m_string_data_size, false, &m_string_data_digest, (s_network_http_request_hash *)scratch.computed_digests);
    if (digests_valid) {
#if !defined(_XBOX) || defined(__clang__)
        // The string references are big-endian {string_id, offset} pairs on disc and the digest covers those
        // bytes, so a little-endian host converts them after the check.
        for (long index = 0; index < count; index++) {
#if defined(__clang__) || defined(__GNUC__)
            strings[index].string_id = (long)__builtin_bswap32((unsigned long)strings[index].string_id);
            strings[index].offset = (long)__builtin_bswap32((unsigned long)strings[index].offset);
#else
            strings[index].string_id = (long)_byteswap_ulong((unsigned long)strings[index].string_id);
            strings[index].offset = (long)_byteswap_ulong((unsigned long)strings[index].offset);
#endif
        }
#endif
        string_count = 1;
        result = true;
    } else {
        text_group_get_digest_string((s_network_http_request_hash *)&scratch.computed_digests[8], scratch.digest_strings[0], 0x100);
        text_group_get_digest_string(&m_string_references_digest, scratch.digest_strings[1], 0x100);
        text_group_get_digest_string((s_network_http_request_hash *)scratch.computed_digests, scratch.digest_strings[2], 0x100);
        text_group_get_digest_string(&m_string_data_digest, scratch.digest_strings[3], 0x100);
    }
    return result;
}

// @address 0x1bfdb0 ?get_string_utf8@c_language_pack@@AAAPBUutf8@@JJJ@Z
const utf8 *c_language_pack::get_string_utf8(long string_id, long offset, long length)
{
    bool found = false;
    const char *string = NULL;
    for (long index = offset; index < offset + length; index++) {
        if (index >= 0 && index < count && strings[index].string_id == string_id) {
            found = true;
            string = data + strings[index].offset;
            break;
        }
    }
    if (found)
        return (const utf8 *)string;
    return (const utf8 *)"";
}


// 0x1bfe40 / 0x1bfeb8
template <long k_maximum_count>
void c_static_wchar_string<k_maximum_count>::set(const wchar_t *source)
{
    ustrnzcpy_inline(m_string, source, k_maximum_count);
}
#if !defined(_XBOX) || defined(__clang__)
template void c_static_wchar_string<256>::set(const wchar_t *source);
template void c_static_wchar_string<1024>::set(const wchar_t *source);
#endif
