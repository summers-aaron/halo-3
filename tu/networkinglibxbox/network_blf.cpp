// networkinglibxbox/network_blf.obj: from the records 0x3ad388 0x3ad410 0x3ad490 0x3ad5a0 0x3ad778 0x3ad858 0x3ada60
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x3ad388 ??0s_blf_chunk_author@@QAA@XZ
// @address 0x3ad410 ??0s_blf_chunk_content_header@@QAA@XZ
// @address 0x3ad490 ?network_blf_verify_start_of_file@@YA_NPBDJPA_NPAJ@Z
// @address 0x3ad5a0 ?network_blf_read_for_known_chunk@@YA_NPBDJ_NJFPAJPAPBD2PAFPA_N@Z
// @address 0x3ad778 ?network_blf_read_next_known_chunk@@YA_NPBDJ_NJFPAJPAPBD2PAFPA_N@Z
// @address 0x3ad858 ?network_blf_verify_end_of_file@@YA_NPBDJ_N0W4e_blf_file_authentication_type@@@Z
// @address 0x3ada60 ?network_blf_find_chunk_in_file@@YAPAXPBDKJFW4e_blf_file_authentication_type@@PAK@Z
#include <string.h>
// match: 0x3ad388: pragma keeps the 3 memset calls out of line (bl) in the author chunk ctor; without it c2 inline-expands stores plus a bdnz zero loop
#pragma function(memset)
#include "cseries/byte_swapping.h"
#include "saved_games/saved_film_recording_types.h"
#include "scenario/scenario.h"
#include "cache/security_functions.h"
#include "cache/security_signature.h"
#include "networking/tools/network_blf.h"
#include "memory/crc.h"
#include "networking/session/network_session_parameters.h"
#include "cache/cache_file_globals.h"
#include "networking/tools/blf_header_types.h"
#include "networking/tools/blf_end_of_file_types.h"
#include "networking/tools/blf_chunk_author_types.h"
// 0x3ad410: the content header chunk's constructor (dev name unknown: the name follows the chunk tag)

#include "networking/tools/blf_authentication_types.h"

#pragma pack(push, 1)
struct n_network_blf_end_of_file_chunk
{
    s_blf_header header;
    long total_file_size;
    unsigned char authentication_type;
    unsigned char authentication_data[256];
};
#pragma pack(pop)

s_blf_chunk_author::s_blf_chunk_author()
{
    initialize();
}
#pragma pack(push, 4)
#pragma pack(pop)
s_blf_chunk_content_header::s_blf_chunk_content_header()
{
    header.setup('chdr', sizeof(*this), 9, 2);
    build_number = 11855;
    int slot_index = -1;
    if (global_scenario)
    {
        slot_index = cache_files_get_header()->minor_version;
    }
    scenario_slot = (unsigned short)slot_index;
    metadata.name[0] = 0;
    metadata.description[0] = 0;
    metadata.author[0] = 0;
    metadata.file_type = -1;
    metadata.date = 0;
    metadata.length_seconds = 0;
    metadata.map_id = -1;
    metadata.game_engine_type = 0;
}

#define network_blf_byte_swap_word(value) ((unsigned short)(value) >> 8 | (unsigned short)(value) << 8)


#pragma pack(push, 4)
#pragma pack(pop)


bool network_blf_verify_start_of_file(const char *buffer, long buffer_count, bool *out_byte_swap, long *out_file_size)
{
    bool result = false;
    const s_blf_chunk_start_of_file *start_of_file = (const s_blf_chunk_start_of_file *)buffer;
    if (out_file_size)
        *out_file_size = 0;
    if (buffer_count >= sizeof(s_blf_chunk_start_of_file))
    {
        do
        {
            long chunk_type;
            unsigned long chunk_size;
            short major_version;
            if ((unsigned short)start_of_file->byte_order_marker == 0xfffe)
            {
                chunk_type = start_of_file->header.chunk_type;
                chunk_size = start_of_file->header.chunk_size;
                major_version = start_of_file->header.major_version;
                if (out_byte_swap)
                    *out_byte_swap = false;
            }
            else if ((unsigned short)network_blf_byte_swap_word((unsigned short)start_of_file->byte_order_marker) == 0xfffe)
            {
                chunk_type = byte_swap_long(start_of_file->header.chunk_type);
                chunk_size = byte_swap_long(start_of_file->header.chunk_size);
                major_version = network_blf_byte_swap_word(start_of_file->header.major_version);
                if (out_byte_swap)
                    *out_byte_swap = true;
            }
            else
            {
                break;
            }
            if (chunk_type == '_blf' && major_version == 1 && chunk_size >= sizeof(s_blf_chunk_start_of_file))
            {
                if (out_file_size)
                    *out_file_size = chunk_size;
                result = true;
            }
        } while (false);
    }
    return result;
}

bool network_blf_read_for_known_chunk(const char *buffer, long buffer_count, bool byte_swap, long chunk_type, short major_version,
    long *out_chunk_size, const char **out_chunk_buffer, long *out_chunk_buffer_size, short *out_minor_version, bool *out_eof_found)
{
    bool result = false;
    *out_chunk_size = 0;
    if (out_chunk_buffer)
        *out_chunk_buffer = 0;
    if (out_chunk_buffer_size)
        *out_chunk_buffer_size = 0;
    if (out_eof_found)
        *out_eof_found = false;
    if (buffer_count >= sizeof(s_blf_header))
    {
        const s_blf_header *source = (const s_blf_header *)buffer;
        s_blf_header header;
        header.chunk_type = byte_swap ? byte_swap_long(source->chunk_type) : source->chunk_type;
        header.chunk_size = byte_swap ? byte_swap_long(source->chunk_size) : source->chunk_size;
        header.major_version = byte_swap ? network_blf_byte_swap_word(source->major_version) : source->major_version;
        header.minor_version = byte_swap ? network_blf_byte_swap_word(source->minor_version) : source->minor_version;
        if (header.chunk_size >= sizeof(s_blf_header) && header.chunk_size <= buffer_count && header.major_version >= 0 && header.minor_version >= 0)
        {
            *out_chunk_size = header.chunk_size;
            if (header.chunk_type == chunk_type && header.major_version == major_version)
            {
                if (out_chunk_buffer)
                    *out_chunk_buffer = buffer + sizeof(s_blf_header);
                if (out_chunk_buffer_size)
                    *out_chunk_buffer_size = header.chunk_size - sizeof(s_blf_header);
                result = true;
                if (header.chunk_type == '_eof' && header.chunk_size >= sizeof(s_blf_chunk_end_of_file) &&
                    buffer_count >= sizeof(s_blf_chunk_end_of_file) && out_eof_found)
                    *out_eof_found = result;
            }
            else if (header.chunk_type == '_eof')
            {
                if (header.chunk_size >= sizeof(s_blf_chunk_end_of_file) && buffer_count >= sizeof(s_blf_chunk_end_of_file))
                {
                    result = true;
                    if (out_eof_found)
                        *out_eof_found = result;
                }
            }
            else
            {
                result = true;
            }
        }
    }
    return result;
}




bool network_blf_read_next_known_chunk(const char *buffer, long buffer_count, bool byte_swap, long chunk_type, short major_version,
    long *out_chunk_size, const char **out_chunk_buffer, long *out_chunk_buffer_size, short *out_minor_version, bool *out_eof_found)
{
    bool found;
    do
    {
        const char *chunk_buffer = 0;
        long chunk_size = 0;
        bool eof_found = false;
        bool read_ok = network_blf_read_for_known_chunk(
            buffer, buffer_count, byte_swap, chunk_type, major_version, &chunk_size, &chunk_buffer, 0, 0, &eof_found);
        if (out_chunk_size)
            *out_chunk_size = chunk_size;
        if (out_chunk_buffer)
            *out_chunk_buffer = chunk_buffer;
        if (out_eof_found)
            *out_eof_found = eof_found;
        found = chunk_buffer != 0 && chunk_size > 0;
        if (!read_ok || eof_found || chunk_size <= 0)
            break;
        buffer += chunk_size;
        buffer_count -= chunk_size;
    } while (!found);
    return found;
}

bool network_blf_verify_end_of_file(
    const char *buffer, long buffer_count, bool byte_swap, const char *chunk, e_blf_file_authentication_type authentication_type)
{
    bool result = false;
    const n_network_blf_end_of_file_chunk *end_of_file = (const n_network_blf_end_of_file_chunk *)chunk;
    long total_file_size;
    if (byte_swap)
        total_file_size = byte_swap_long(end_of_file->total_file_size);
    else
        total_file_size = end_of_file->total_file_size;
    if (authentication_type == end_of_file->authentication_type)
    {
        switch (end_of_file->authentication_type)
        {
        case _blf_file_authentication_none:
            result = true;
            break;
        case _blf_file_authentication_crc:
            if (buffer_count >= sizeof(s_blf_chunk_end_of_file) + sizeof(unsigned long) && total_file_size == buffer_count - 21)
            {
                long chunk_offset = (const char *)end_of_file - buffer;
                if (chunk_offset >= 1 && (unsigned long)chunk_offset <= (unsigned long)(buffer_count - 21))
                {
                    unsigned long crc = crc_checksum_buffer(0xffffffff, buffer, chunk_offset);
                    unsigned long stored_crc = *(const unsigned long *)end_of_file->authentication_data;
                    if (byte_swap)
                        stored_crc = byte_swap_long(stored_crc);
                    if (stored_crc == crc)
                        result = true;
                }
            }
            break;
        case _blf_file_authentication_hash:
            if (buffer_count >= sizeof(s_blf_chunk_end_of_file) + sizeof(end_of_file->authentication_data) && total_file_size == buffer_count - 273)
            {
                long chunk_offset = (const char *)end_of_file - buffer;
                if (chunk_offset >= 1 && (unsigned long)chunk_offset <= (unsigned long)(buffer_count - 273))
                {
                    s_network_http_request_hash hash;
                    if (security_calculate_hash(buffer, chunk_offset, false, &hash))
                    {
                        if (memcmp(&hash, end_of_file->authentication_data, sizeof(hash)) == 0)
                            result = true;
                    }
                }
            }
            break;
        case _blf_file_authentication_signature:
            if (buffer_count >= sizeof(s_blf_chunk_end_of_file) + sizeof(end_of_file->authentication_data) && total_file_size == buffer_count - 273)
            {
                long chunk_offset = (const char *)end_of_file - buffer;
                if (chunk_offset >= 1 && (unsigned long)chunk_offset <= (unsigned long)(buffer_count - 273))
                {
                    s_network_http_request_hash hash;
                    unsigned char signature[256];
                    if (security_calculate_hash(buffer, chunk_offset, false, &hash))
                    {
                        memcpy(signature, end_of_file->authentication_data, sizeof(signature));
                        if (security_verify_signature(&hash, signature))
                            result = true;
                    }
                }
            }
            break;
        }
    }
    return result;
}

void *network_blf_find_chunk_in_file(const char *buffer, unsigned long size, long chunk_type, short major_version,
    e_blf_file_authentication_type authentication_type, unsigned long *out_size)
{
    const char *chunk = 0;
    bool byte_swap;
    long offset;
    long found_size = 0;
    const char *found;
    bool eof_found;
    bool valid = false;
    void *result = 0;
    if (out_size)
        *out_size = 0;
    if (size >= 77)
    {
        offset = 0;
        if (network_blf_verify_start_of_file(buffer, size, &byte_swap, &offset))
        {
            chunk = buffer + offset;
            found = 0;
            eof_found = false;
            if (network_blf_read_for_known_chunk(
                    chunk, buffer - chunk + size, byte_swap, chunk_type, major_version, &offset, &found, &found_size, 0, &eof_found))
            {
                do
                {
                    if (found)
                    {
                        result = (void *)found;
                        if (out_size)
                            *out_size = found_size;
                    }
                    if (eof_found)
                    {
                        if (network_blf_verify_end_of_file(buffer, size, byte_swap, chunk, authentication_type))
                            valid = true;
                        break;
                    }
                    chunk += offset;
                }
                while (network_blf_read_for_known_chunk(
                    chunk, buffer - chunk + size, byte_swap, chunk_type, major_version, &offset, &found, &found_size, 0, &eof_found))
                    ;
            }
        }
    }
    return valid ? result : 0;
}
