#ifndef CACHE_CACHE_FILE_HEADER_H
#define CACHE_CACHE_FILE_HEADER_H

#include <stddef.h>
#include "networking/logic/storage/network_http_request_hash.h"
#include "tag_files/file_last_modification_date.h"

#include "tag_files/tag_types.h"
#include "cache/cache_file_allocation_range_types.h"

// The cache file (.map) header and the tags header it points at (cache\cache_files.cpp).
// Both live in the cache file, so their pointers are 32-bit tag pointers on every host.

enum
{
    k_cache_file_header_signature = 'head',
    k_cache_file_footer_signature = 'foot',
    k_cache_file_version = 11
};

// One tag interop to tear down when the tags unload (March: s_cache_file_tag_interop_type_fixup, handed to
// cache_file_tag_interop_teardown(type_index, interop)).
struct s_cache_file_tag_interop_type_fixup
{
    TAG_POINTER(void) interop;
    long type_index;
};

struct s_cache_file_tags_header
{
    long group_count;
    TAG_POINTER(cache_file_tag_group) groups;
    long instance_count;
    TAG_POINTER(const cache_file_tag_instance) instances;
    long global_tag_count;
    TAG_POINTER(global_tag_index) global_tags;
    long interop_fixup_count;
    TAG_POINTER(s_cache_file_tag_interop_type_fixup) interop_fixups;
    unsigned long reserved20;
    unsigned long signature;
};

struct s_cache_file_digest
{
    s_network_http_request_hash hashes[3];
};

struct s_xenon_game_state_checksum_values
{
    void game_state_xenon_checksum_zero()
    {
        value[0] = 0;
        value[1] = 0;
        value[2] = 0;
        value[3] = 0;
        value[4] = 0;
        value[5] = 0;
        value[6] = 0;
        value[7] = 0;
        value[8] = 0;
        value[9] = 0;
        value[10] = 0;
    }
    long value[11];
};
struct s_xenon_game_state_checksums
{
    void game_state_xenon_checksum_zero()
    {
        count = 0;
        values.game_state_xenon_checksum_zero();
    }
    long count;
    s_xenon_game_state_checksum_values values;
};

// 0x3000 bytes; the 64-bit timestamp makes it 8-byte aligned.
struct s_cache_file_header
{
    unsigned long header_signature;
    long version;
    long file_size;
    unsigned long reserved0c;
    TAG_POINTER(s_cache_file_tags_header) tags_header;
    long tags_offset;
    long total_tags_size;
    char source_path[256];
    char build[32];
    short scenario_type;
    short flags;
    bool uncompressed;
    bool tracked_build;
    signed char saved_game_data;
    char reserved143;
    s_file_last_modification_date cached_map_date;
    long low_detail_texture_number;
    unsigned long low_detail_texture_offset;
    unsigned long low_detail_texture_byte_count;
    long string_id_count;
    long string_id_buffer_size;
    unsigned long string_id_indices_offset;
    unsigned long string_id_buffer_offset;
    unsigned char level_mask;
    long timestamp[2];
    s_file_last_modification_date level_timestamps[3];
    char name[32];
    long language;
    char scenario_path[256];
    long minor_version;
    long tag_name_count;
    unsigned long tag_name_buffer_offset;
    long tag_name_buffer_size;
    unsigned long tag_name_indices_offset;
    long cached_map_checksum;
    char copyright[32];
    unsigned long tag_memory_base;
    unsigned long xdk_version;
    union { cache_file_allocation_range ranges[6]; s_xenon_game_state_checksums checksums; };
    long checksum;
    unsigned __int64 timestamp328;
    s_cache_file_digest digest;
    unsigned char signature[256];
    union
    {
        long map_file_offsets[15];
        struct
        {
            long section_address_to_offsets[4];
            cache_file_allocation_range sections[4];
            long trailing_fields[4];
        };
    };
    char header_padding[0x2b50];
    unsigned long footer_signature;
};

typedef char cache_file_header_size_interop_type_fixup[(sizeof(s_cache_file_tag_interop_type_fixup) == 0x8) ? 1 : -1];
typedef char cache_file_header_size_tags_header[(sizeof(s_cache_file_tags_header) == 0x28) ? 1 : -1];
typedef char cache_file_header_size_digest[(sizeof(s_cache_file_digest) == 0x3c) ? 1 : -1];
typedef char cache_file_header_size_header[(sizeof(s_cache_file_header) == 0x3000) ? 1 : -1];
typedef char cache_file_header_offset_cached_map_date[(offsetof(s_cache_file_header, cached_map_date) == 0x144) ? 1 : -1];
typedef char cache_file_header_offset_level_mask[(offsetof(s_cache_file_header, level_mask) == 0x168) ? 1 : -1];
typedef char cache_file_header_offset_timestamp[(offsetof(s_cache_file_header, timestamp) == 0x16c) ? 1 : -1];
typedef char cache_file_header_offset_cached_map_checksum[(offsetof(s_cache_file_header, cached_map_checksum) == 0x2c4) ? 1 : -1];
typedef char cache_file_header_offset_map_file_offsets[(offsetof(s_cache_file_header, map_file_offsets) == 0x46c) ? 1 : -1];
typedef char cache_file_header_offset_tags_header[(offsetof(s_cache_file_header, tags_header) == 0x10) ? 1 : -1];
typedef char cache_file_header_offset_total_tags_size[(offsetof(s_cache_file_header, total_tags_size) == 0x18) ? 1 : -1];
typedef char cache_file_header_offset_scenario_type[(offsetof(s_cache_file_header, scenario_type) == 0x13c) ? 1 : -1];
typedef char cache_file_header_offset_tracked_build[(offsetof(s_cache_file_header, tracked_build) == 0x141) ? 1 : -1];
typedef char cache_file_header_offset_uncompressed[(offsetof(s_cache_file_header, uncompressed) == 0x140) ? 1 : -1];
typedef char cache_file_header_offset_language[(offsetof(s_cache_file_header, language) == 0x1ac) ? 1 : -1];
typedef char cache_file_header_offset_low_detail_texture_number[
    (offsetof(s_cache_file_header, low_detail_texture_number) == 0x14c) ? 1 : -1];
typedef char cache_file_header_offset_low_detail_texture_offset[
    (offsetof(s_cache_file_header, low_detail_texture_offset) == 0x150) ? 1 : -1];
typedef char cache_file_header_offset_low_detail_texture_byte_count[
    (offsetof(s_cache_file_header, low_detail_texture_byte_count) == 0x154) ? 1 : -1];
typedef char cache_file_header_offset_level_timestamps[(offsetof(s_cache_file_header, level_timestamps) == 0x174) ? 1 : -1];
typedef char cache_file_header_offset_name[(offsetof(s_cache_file_header, name) == 0x18c) ? 1 : -1];
typedef char cache_file_header_offset_scenario_path[(offsetof(s_cache_file_header, scenario_path) == 0x1b0) ? 1 : -1];
typedef char cache_file_header_offset_minor_version[(offsetof(s_cache_file_header, minor_version) == 0x2b0) ? 1 : -1];
typedef char cache_file_header_offset_copyright[(offsetof(s_cache_file_header, copyright) == 0x2c8) ? 1 : -1];
typedef char cache_file_header_offset_ranges[(offsetof(s_cache_file_header, ranges) == 0x2f0) ? 1 : -1];
typedef char cache_file_header_offset_checksum[(offsetof(s_cache_file_header, checksum) == 0x320) ? 1 : -1];
typedef char cache_file_header_offset_timestamp328[(offsetof(s_cache_file_header, timestamp328) == 0x328) ? 1 : -1];
typedef char cache_file_header_offset_signature[(offsetof(s_cache_file_header, signature) == 0x36c) ? 1 : -1];
typedef char cache_file_header_offset_trailing_fields[(offsetof(s_cache_file_header, trailing_fields) == 0x49c) ? 1 : -1];
typedef char cache_file_header_offset_footer[(offsetof(s_cache_file_header, footer_signature) == 0x2ffc) ? 1 : -1];

typedef char cache_file_header_offset_saved_game_data[(offsetof(s_cache_file_header, saved_game_data) == 0x142) ? 1 : -1];
typedef char cache_file_header_offset_checksums[(offsetof(s_cache_file_header, checksums) == 0x2f0) ? 1 : -1];

typedef char cache_file_header_offset_string_id_count[(offsetof(s_cache_file_header, string_id_count) == 0x158) ? 1 : -1];
typedef char cache_file_header_offset_string_id_buffer_size[(offsetof(s_cache_file_header, string_id_buffer_size) == 0x15c) ? 1 : -1];
typedef char cache_file_header_offset_string_id_indices[(offsetof(s_cache_file_header, string_id_indices_offset) == 0x160) ? 1 : -1];
typedef char cache_file_header_offset_string_id_buffer[(offsetof(s_cache_file_header, string_id_buffer_offset) == 0x164) ? 1 : -1];
typedef char cache_file_header_offset_tag_name_count[(offsetof(s_cache_file_header, tag_name_count) == 0x2b4) ? 1 : -1];
typedef char cache_file_header_offset_tag_name_buffer[(offsetof(s_cache_file_header, tag_name_buffer_offset) == 0x2b8) ? 1 : -1];
typedef char cache_file_header_offset_tag_name_size[(offsetof(s_cache_file_header, tag_name_buffer_size) == 0x2bc) ? 1 : -1];
typedef char cache_file_header_offset_tag_name_indices[(offsetof(s_cache_file_header, tag_name_indices_offset) == 0x2c0) ? 1 : -1];
typedef char cache_file_header_offset_section_addresses[(offsetof(s_cache_file_header, section_address_to_offsets) == 0x46c) ? 1 : -1];
typedef char cache_file_header_offset_sections[(offsetof(s_cache_file_header, sections) == 0x47c) ? 1 : -1];

#endif // CACHE_CACHE_FILE_HEADER_H
