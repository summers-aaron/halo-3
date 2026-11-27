// networkinglibxbox/online_files.obj: from the records 0x96650 0x1e4f80 0x2113d0 0x2281d0 0x281be8 0x281c60 0x281db0 0x2febf8 0x2fecf0 0x2fedb8 0x2fee30 0x2feec0 0x2ff258 0x2ff2e8 0x3a84e0 0x3a8620 0x3b6e90 0x3b6ee8 0x3b6f48 0x3b6f98 0x3b70e8 0x3b71f8 0x3b75f0 0x3b7708 0x3b7850 0x3b7938 0x3b7a18 0x3b7aa0 0x3b7b30 0x3b7c30 0x3b7d10 0x3b7dc0 0x3b7e20 0x3b7f30 0x3b8038 0x3b8168 0x3b8260 0x3b8768 0x3b90b0 0x3ba368 0x3bae68 0x3baf28 0x3bb008 0x3bb120 0x3bb1f0 0x3bb3a0 0x3bb528 0x3bb638 0x3bb708 0x3bb7c0 0x3bb830 0x3bb960 0x3bc240 0x3bc338 0x3bc3a8 0x3bc440 0x3bc5c8 0x3bc670 0x3bc708 0x3bc748 0x3bc878 0x3bc8e0 0x3c3820 0x3c38b8 0x3c39a8 0x3c3ac8 0x3c3bf0 0x3c3d10 0x3c3e78 0x3c3ff0 0x3c4068 0x4b0b68 0x3b9b00
#include "saved_games/saved_game_item_metadata.h"
#include "interface/c_player_marketplace.h"
#include "networking/online/online_file_string_specializations.h"
#include "cache/cache_files.h"
#include "saved_games/saved_game_files_io.h"
#include "memory/physical_memory_map.h"
#include "tag_files/files_windows_types.h"
#include "tag_files/files.h"
#include "scenario/scenario.h"
#include "networking/online/online_files_globals.h"
#include "interface/file_share_category_dependencies.h"
#include "networking/online/online_presence_accessors.h"
#include "interface/controller_inline.h"
#include "shell/shell.h"
#include "math/random_math.h"
#include "cseries/async_helpers.h"
#include "multithreading/synchronized_value.h"
#include <stdio.h>
#include "game/player_types.h"
#include "interface/gui_widget_types.h"
#include "multithreading/synchronization.h"
#include "simulation/simulation.h"
#include "text/unicode.h"
#include "saved_games/saved_game_files.h"
#include "tag_files/files_windows.h"
#include "text/font_cache.h"
#include "networking/online/online_files.h"
#include "networking/online/minidump_file_output_secure.h"
#include "networking/online/online_file_work_item_types.h"
#include "networking/tools/network_debug_dump.h"
#include "game/game_globals.h"
#include "cseries/async.h"
#include "memory/data_compress.h"
#include "zlib/zlib.h"
#include "tag_files/file_reference.h"
#include "memory/zlib_compression_level_types.h"
#include "saved_games/text_stream_types.h"
#include "networking/online_lsp_server_acquire_result_types.h"
#include "networking/http_request_types.h"
#include "saved_games/buffered_file_writer_types.h"
#include "interface/queued_work_item_status_state_types.h"
#include "interface/gui_screens/start_menu/panes/hq_service_record_file_share/gui_start_menu_hq_service_record_file_share.h"
#include "networking/online/online_lsp.h"
#include "interface/c_signed_in_users_watcher.h"
#include "zlib/blam_zlib_memory_types.h"
#include "networking/tools/blf_header_types.h"
#include "cache/cache_file_globals.h"
#include "networking/logic/storage/network_http_request_hash.h"
#include "cseries/async_types.h"
#include "cseries/minidump_writer.h"
#include "cseries/byte_swapping.h"
#include "scenario/scenario_tag.h"
#include "saved_games/game_content_types.h"
#include "saved_games/content_item_status_types.h"
#include "interface/c_gui_list_widget.h"
#include "saved_games/content_item_types.h"
#include <ppcintrinsics.h>
#include <stddef.h>
#include "cseries/placement_new.h"
#include <math.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "memory/static_arrays.h"
#include "cseries/async_block.h"
#include "cseries/read_write_barrier.h"
#include "interface/user_interface_error_manager.h"
#include "interface/user_interface_networking.h"
#include "networking/tools/http_client.h"
#include "interface/gui_screens/start_menu/gui_start_menu_hq_transfers_item_selected.h"
#include "multithreading/critical_section_scope.h"
#include "networking/online/online_file_game_api_parser.h"
#include "memory/data_compress_encrypted_writer_types.h"
#include "xbox/xecrypt.h"
#include "networking/online/online_files_services.h"
#include "game/game_globals_storage.h"
#include "memory/tls_globals.h"
#include "multithreading/synchronization_types.h"
#include "cseries/crt_xdk.h"
#include "networking/online/online_lsp_manager.h"
#include "networking/logic/storage/network_http_request_queue.h"
#include "networking/online/online_files_enumerator.h"
#include "networking/online/online_files_deleter.h"
#include "networking/session/network_session_interface_types.h"
#include "interface/controller_types.h"
#include "saved_games/content_catalogue_types.h"
#include "networking/online/online_globals_types.h"
#include "networking/tools/blf_end_of_file_types.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "interface/gui_widget.h"
#include "interface/c_gui_screen_widget.h"
#include "interface/gui_screens/start_menu/start_menu_pane_screen_widget.h"
#include "saved_games/saved_game_file_types.h"
#include "saved_games/saved_film_recording_types.h"
#include "math/random_math_range.h"
#include "cseries/cseries.h"

// @compiler xdk6130
// @flags /TP /O2 /Oc

enum
{
    _transfer_state_none = 0,
    _transfer_state_enumerating_files = 1,
};

enum
{
    _tracker_state_none = 0,
};

enum memory_stage;
enum e_network_http_request_result;
enum e_network_http_request_queue_failure_reason;
struct z_stream_s;
enum e_zlib_compression_level;
struct utf8;
// @address 0x3b6f98 ??0c_online_file_manager@@QAA@XZ
// @address 0x3b8768 ?async_do_compression@c_online_file_manager@@CA?AW4e_async_completion@@PAUs_async_task@@@Z
// @address 0x3b9b00 ?async_do_download@c_online_file_manager@@CA?AW4e_async_completion@@PAUs_async_task@@@Z
// @address 0x80608 GetTickCount
// @address 0x281be8 ?set@?$c_static_wchar_string@$0BA@@@QAAXPB_W@Z
// @address 0x281c60 ?trim@?$c_static_wchar_string@$0BA@@@QAAXXZ
// @address 0x281db0 ?length@?$c_static_wchar_string@$0BA@@@QBAJXZ
// @address 0x3b6f48 online_files_memory_dispose
// @address 0x3b7a18 online_files_is_transfer_ready
// @address 0x3b7708 ?enqueue_for_transfer@c_online_file_manager@@QAAXW4e_queued_work_item_type@@PBUs_online_file_description@@PBD2_K3J_N@Z
// @address 0x3b7aa0 ?post_connection_error@c_online_file_manager@@QAAX_NPBUs_queued_work_item_status_tracker@@@Z
// @address 0x3b7b30 ?start_enumerate_files@c_online_file_manager@@AAAXXZ
// @address 0x3b7c30 ?end_enumerate_files@c_online_file_manager@@AAAXXZ
// @address 0x3b7f30 ?start_uploading_file@c_online_file_manager@@AAAXPBUs_queued_work_item_status_tracker@@_N@Z
// @address 0x3b8168 ?start_decompressing_file@c_online_file_manager@@AAAXPBUs_queued_work_item_status_tracker@@@Z
// @address 0x3bb120 ?start_enumerate_files@c_online_files_active_transfer_tracker@@AAAXXZ
// @address 0x3bb708 ?get_transfer_status_by_slot@c_online_files_active_transfer_tracker@@QBA_N_KJPAUs_queued_work_item_status@@@Z
// @address 0x3bb528 ?update_transfer_status@c_online_files_active_transfer_tracker@@QAAXPBDW4e_queued_work_item_status_state@@M@Z
// @address 0x3b6e90 ?get_base_filename@s_work_item_transfer_task@@QAAXPAV?$c_static_string@$0BB@@@@Z
// @address 0x3b6ee8 ?online_files_memory_initialize@@YAXW4e_map_memory_configuration@@@Z
// @address 0x3b7d10 ?setup_task@c_online_file_manager@@AAAXPAUs_work_item_transfer_task@@@Z
// @address 0x3b7dc0 ?create_file_reference_with_extension@c_online_file_manager@@CAXPAUs_file_reference@@PBD1@Z
// @address 0x3bb3a0 ?set_transfers@c_online_files_active_transfer_tracker@@QAAXPAUs_queued_work_item_status_tracker@@JPA_N@Z
// @address 0x3bb638 ?get_transfer_status@c_online_files_active_transfer_tracker@@QBA_NJPAUs_queued_work_item_status@@@Z
// @address 0x3bc5c8 ?get_controller_index_from_xuid@@YA?AW4e_controller_index@@_K@Z
// @address 0x3bc708 ?work_item_tracker_sort_proc@@YAHPBX0@Z
// @address 0x3bc748 buffered_file_writer_set_position
// @address 0x3bc878 ?print@?$c_static_string@$0BB@@@QAAPBDPBDZZ
// @address 0x3c3820 online_file_game_api_parser_advance_to_next_line
// @address 0x3c38b8 online_file_game_api_parser_skip_to_next_section
// @address 0x3c39a8 ?read_long@c_online_file_game_api_parser@@QAA_NPBDPAJ@Z
// @address 0x3c3ac8 ?read_dword@c_online_file_game_api_parser@@QAA_NPBDPAK@Z
// @address 0x3c3bf0 ?read_qword@c_online_file_game_api_parser@@QAA_NPBDPA_K@Z
// @address 0x3c3d10 ?read_string@c_online_file_game_api_parser@@QAA_NPBDPA_WJ@Z
// @address 0x3c3e78 ?read_string@c_online_file_game_api_parser@@QAA_NPBDPADJ@Z
// @address 0x3c3ff0 ?trim_end@?$c_static_string@$0EAA@@@QAAXXZ
// @address 0x3c4068 ?starts_with@?$c_static_string@$0EAA@@@QBA_NPBD@Z


enum e_map_memory_configuration { _map_memory_configuration_main_menu = 1 };

class c_online_file_manager;
struct s_online_files_writer_statistics {
    long state;
    long file_size;
    long checksum;
    long compressed_size;
    long uncompressed_size;
    long written_size;
};

struct s_minidump_file_output_secure_header {
    unsigned long signature;
    unsigned long flags;
    unsigned char encrypted_key[256];
    unsigned long file_count;
    s_online_files_writer_statistics files[10];
};

struct s_file_share_screen
{
    c_start_menu_pane_screen_widget m_pane;
    unsigned __int64 m_owner_xuid;
    unsigned __int64 m_player_xuid;
    s_file_share_enumerator_state m_transfer;
    long m_quota_used;
    long m_file_count;
    long m_controller_state;
    bool m_flag_7b0c;
};

STATIC_ASSERT(offsetof(s_queued_work_item, description) == 8, queued_item_description_offset);
STATIC_ASSERT(offsetof(s_online_file_description, metadata) == 16, online_file_description_metadata_offset);

namespace {
class c_online_files_buffered_writer : public s_buffered_file_writer
{
public:
    __forceinline c_online_files_buffered_writer(s_file_reference *reference)
    {
        file = reference;
        buffer = m_buffer;
        buffer_size = sizeof(m_buffer);
        buffer_position = 0;
    }
private:
    char m_buffer[4096];
};
}

namespace {
class c_online_files_extension_string
{
public:
    __forceinline c_online_files_extension_string() { m_string[0] = 0; }
    __forceinline void set(const char *source)
    {
        strncpy(m_string, source, sizeof(m_string));
        m_string[255] = 0;
    }
    __forceinline bool is_equal(const char *other) const
    {
        bool result = strncmp(m_string, other, sizeof(m_string)) == 0;
        return result;
    }
private:
    char m_string[256];
};
}

enum e_online_file_slot_state
{
    _online_file_slot_state_empty = 0,
    _online_file_slot_state_partial,
    _online_file_slot_state_ready,
    _online_file_slot_state_invalid,
};

#pragma pack(push, 1)
struct s_blf_chunk_compressed_data {
    s_blf_header header;
    unsigned char compression_type;
    long uncompressed_size;
    __forceinline void initialize()
    {
        header.setup('_cmp', sizeof(*this), 1, 1);
        compression_type = 0;
        uncompressed_size = 0;
    }
};
#pragma pack(pop)

union s_online_file_compress_header_union {
    char lock_storage[sizeof(c_critical_section_scope)];
    s_blf_header header;
    s_blf_chunk_compressed_data compressed_state4;
    s_online_file_compress_header_union() {}
    ~s_online_file_compress_header_union() {}
};

static __forceinline bool map_memory_configuration_is_main_menu(e_map_memory_configuration configuration)
{
    return configuration == _map_memory_configuration_main_menu;
}

template<>
__forceinline void c_static_string<33>::set_wchar(const wchar_t *source)
    {
        const wchar_t *read = source;
        char *write = m_string;
        long remaining = 33;
        wchar_t character;
        do
        {
            character = *read;
            if (remaining == 1)
                *write = 0;
            else if (character <= 127)
                *write = (char)character;
            else
                *write = '?';
            read++;
            write++;
            remaining--;
        } while (character != 0 && remaining > 0);
    }


class c_http_client;


static __forceinline void online_files_synchronized_set(volatile long *value, long new_value)
{
    __lwsync();
    _InterlockedExchange(value, new_value);
}


static __forceinline long online_files_synchronized_get(const volatile long *value)
{
    long result = *value;
    __lwsync();
    return result;
}


__forceinline void c_online_files_active_transfer_tracker::update()
{
        switch (m_state)
        {
        case _tracker_state_none:
            if (!m_status_dirty)
                m_status_dirty = m_signed_in_users_watcher.have_users_changed();
            if (m_status_dirty)
            {
                start_enumerate_files();
                m_status_dirty = false;
                m_state = 1;
            }
            break;
        case 1:
            if (online_files_synchronized_get(&m_task_done))
            {
                m_task_id = -1;
                m_state = _tracker_state_none;
                if (!online_files_synchronized_get(&m_enumeration_done))
                    m_status_dirty = true;
            }
            break;
        }
    }

__forceinline void c_online_file_manager::memory_initialize(e_map_memory_configuration configuration)
{
        if (map_memory_configuration_is_main_menu(configuration))
        {
            long buffer_size = 307200;
            m_buffer = _physical_memory_malloc((memory_stage)4, 0, buffer_size, 0, 0, -1);
            m_buffer_size = buffer_size;
        }
    }

__forceinline s_online_file_manager_self::s_online_file_manager_self()
{
    // match: 0x3b6f98: identify the singleton for alias analysis.
    __assume((c_online_file_manager *)this == &g_online_file_manager);
}

static __forceinline int csstrnicmp(const char *first, const char *second, unsigned int count)
{
    for (long index = 0; ; ++index)
    {
        if (index == count)
            return 0;
        long first_character = towlower(first[index]);
        long second_character = towlower(second[index]);
        if (first_character == 0)
            return second_character != 0 ? -1 : 0;
        else if (second_character == 0)
            return first_character != 0;
        else if (first_character != second_character)
            return first_character > second_character ? 1 : -1;
    }
}
__forceinline void c_online_files_active_transfer_tracker::sort_transfers()
{
    qsort(m_transfers, m_transfer_count, sizeof(m_transfers[0]), work_item_tracker_sort_proc);
}

static __forceinline unsigned int string_length(const char *string, unsigned int maximum)
{
    unsigned int length;
    const char *p = string;
    for (length = 0; length < maximum; length++) {
        if (!*p++)
            break;
    }
    return length;
}


// @address 0x1e4f80 buffered_file_writer_flush
extern "C" bool buffered_file_writer_flush(s_buffered_file_writer *writer)
{
    bool success = true;
    if (writer->buffer_position > 0)
    {
        success = file_write(writer->file, writer->buffer_size, writer->buffer);
        writer->buffer_position = 0;
    }
    return success;
}


// @address 0x2113d0 ?append_print@?$c_static_string@$0EAA@@@QAAPADPBDZZ
template<long k_capacity>
char *c_static_string<k_capacity>::append_print(const char *format, ...)
{
    long length = this->length();
    long size = k_capacity - length;
    char *end = m_string + length;
    va_list args;
    va_start(args, format);
    _vsnprintf(end, size - 1, format, args);
    end[size - 1] = 0;
    return m_string;
}
template char *c_static_string<1024>::append_print(const char *format, ...);

// 0x281be8
template<>
void c_static_wchar_string<16>::set(const wchar_t *string)
{
    ustrnzcpy_inline(m_string, string, 16);
}

// 0x281c60
template<>
void c_static_wchar_string<16>::trim(void)
{
    long index = 0;
    long length = this->length();
    if (length > 0)
    {
        while (index < length)
        {
            wchar_t c = m_string[index];
            if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
                break;
            index++;
        }
        if (index != 0)
        {
            memmove(m_string, m_string + index, (length - index) * sizeof(wchar_t));
            m_string[length - index] = 0;
        }
    }

    length = this->length();
    while (length < 16)
    {
        wchar_t c = m_string[length - 1];
        if (c != ' ' && c != '\r' && c != '\n')
            break;
        m_string[length - 1] = 0;
        length--;
    }
}

// 0x281db0
template<long k_maximum_count> long c_static_wchar_string<k_maximum_count>::length() const
{
    unsigned long count;

    for (count = 0; count < k_maximum_count; count++) {
        if (m_string[count] == 0)
            break;
    }
    return count;
}
#if !defined(_XBOX) || defined(__clang__)
template long c_static_wchar_string<16>::length() const;
#endif
extern "C" {
// @address 0x7f4220 g_online_files_writer_statistics
s_online_files_writer_statistics g_online_files_writer_statistics[10];
}


// @address 0x2fedb8 ?is_valid@c_minidump_file_output_secure@@UAA_NXZ
bool c_minidump_file_output_secure::is_valid()
{
    return m_open;
}


extern "C" {
// @address 0x74b5a8 k_online_files_rsa_public_key
unsigned char k_online_files_rsa_public_key[272] = {
    0x00, 0x00, 0x00, 0x20, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xa1, 0xfe, 0x18, 0x03, 0x20, 0x2f, 0xd3, 0xff, 0x38, 0x98, 0x2d, 0x54, 0xe8, 0xad, 0x97, 0xf9,
    0xa1, 0xef, 0x7c, 0x28, 0x36, 0x6d, 0x2e, 0x11, 0x96, 0x52, 0x10, 0x50, 0x6b, 0x4e, 0x76, 0xb2,
    0x11, 0xb2, 0xa7, 0x70, 0x81, 0x88, 0xed, 0x3d, 0x0f, 0x7e, 0xaf, 0xcc, 0x6d, 0x1b, 0x46, 0x2c,
    0xba, 0x14, 0x85, 0x9b, 0xd9, 0x21, 0x38, 0x94, 0xf7, 0x6b, 0xea, 0xf8, 0xbb, 0x72, 0x14, 0x33,
    0x0f, 0xc3, 0x05, 0xac, 0x95, 0x69, 0x9b, 0xe7, 0xb8, 0xc0, 0x9e, 0x52, 0x32, 0xd3, 0x86, 0x29,
    0xab, 0x03, 0xa1, 0xfc, 0x38, 0xc2, 0xae, 0x68, 0x56, 0xa5, 0x91, 0x58, 0x47, 0x97, 0xda, 0x20,
    0x22, 0x99, 0xf6, 0xbd, 0x59, 0xfd, 0xa1, 0x1f, 0xa4, 0x5a, 0xdd, 0x0f, 0x39, 0x91, 0x4e, 0x9d,
    0x56, 0x10, 0xf4, 0x04, 0xe3, 0x25, 0xc7, 0x0c, 0xd0, 0x7c, 0x8e, 0xe4, 0x4f, 0x45, 0xc8, 0xf8,
    0xc1, 0x4b, 0xab, 0x5b, 0xe7, 0x1e, 0x50, 0x31, 0x17, 0x14, 0x32, 0x50, 0xa1, 0x0d, 0xde, 0xc6,
    0xc2, 0x27, 0xec, 0x0b, 0x4b, 0x79, 0x29, 0xf6, 0xbe, 0x6d, 0xa6, 0x3f, 0xcc, 0x57, 0x8f, 0xda,
    0x08, 0x93, 0xa9, 0x60, 0xa5, 0x7b, 0xe7, 0x67, 0x68, 0x2d, 0x8f, 0x20, 0x30, 0x28, 0x80, 0xcd,
    0x52, 0x94, 0xbc, 0x5d, 0xf5, 0x15, 0xec, 0xaa, 0xd1, 0xc9, 0xeb, 0x3f, 0x66, 0xaf, 0x62, 0x25,
    0x19, 0xd8, 0x53, 0x2e, 0x61, 0xa3, 0x82, 0xec, 0x46, 0xdb, 0x76, 0x15, 0x43, 0x02, 0x15, 0xa5,
    0xe8, 0x08, 0x8a, 0x34, 0x21, 0x22, 0x83, 0x97, 0x84, 0xd0, 0x80, 0xe0, 0x6b, 0x83, 0x95, 0x81,
    0x29, 0xb3, 0xf1, 0x89, 0x4d, 0xb8, 0x8c, 0x50, 0x2e, 0x02, 0x98, 0xf7, 0x8e, 0xd7, 0x47, 0x98,
    0x29, 0x48, 0xc1, 0xf6, 0xf6, 0xec, 0x6c, 0x4e, 0x9b, 0xd9, 0xec, 0xca, 0x5c, 0xcf, 0xb7, 0x5b,
};


}
static __forceinline void online_files_initialize_header(s_minidump_file_output_secure_header *header, const c_minidump_file_output_secure *object)
{
    header->signature = 'mdsc';
    header->flags |= 1;
    header->flags |= 2;
    header->file_count = object->m_count + 1;
}
// @address 0x2feec0 online_files_write_archive_header
extern "C" unsigned char online_files_write_archive_header(c_minidump_file_output_secure *object, long value)
{
    bool success;
    unsigned char crypto_buffer[268];
    if ((unsigned char)value)
    {
        memset(crypto_buffer, 0, sizeof(crypto_buffer));
        success = buffered_file_write(&object->buffered_writer, crypto_buffer, sizeof(crypto_buffer)) &&
            buffered_file_write(&object->buffered_writer, g_online_files_writer_statistics, 240);
    }
    else
    {
        union online_files_metadata_buffer {
            char bytes[4096];
            s_minidump_file_output_secure_header header;
        } file_buffer = {0};
        s_file_reference *file = &object->file;
        success = file_set_position(file, 0, false) && file_read(file, sizeof(file_buffer), false, file_buffer.bytes);
        s_minidump_file_output_secure_header *header = &file_buffer.header;
        online_files_initialize_header(header, object);
        struct online_files_hash {
            unsigned char data[20];
        } hash = {};
        XeCryptBnDwLePkcs1Format(&hash, 0, crypto_buffer, 256);
        memcpy(crypto_buffer, object->encrypted_writer.key, sizeof(object->encrypted_writer.key));
        XeCryptBnQw_SwapDwQwLeBe(crypto_buffer, crypto_buffer, 32);
        int encrypted = XeCryptBnQwNeRsaPubCrypt(crypto_buffer, crypto_buffer, k_online_files_rsa_public_key);
        XeCryptBnQw_SwapDwQwLeBe(crypto_buffer, crypto_buffer, 32);
        if (encrypted)
            memcpy(header->encrypted_key, crypto_buffer, sizeof(header->encrypted_key));
        header->flags = byte_swap_long(header->flags);
        header->signature = byte_swap_long(header->signature);
        header->file_count = byte_swap_long(header->file_count);
        for (unsigned long index = 0; index < 10; index++)
        {
            unsigned long state = g_online_files_writer_statistics[index].state;
            unsigned long file_size = g_online_files_writer_statistics[index].file_size;
            unsigned long checksum = g_online_files_writer_statistics[index].checksum;
            unsigned long written_size = g_online_files_writer_statistics[index].written_size;
            unsigned long compressed_size = g_online_files_writer_statistics[index].compressed_size;
            unsigned long uncompressed_size = g_online_files_writer_statistics[index].uncompressed_size;
            header->files[index].file_size = byte_swap_long(file_size);
            header->files[index].checksum = byte_swap_long(checksum);
            s_minidump_file_output_secure_header *output_header = &file_buffer.header;
            output_header->files[index].state = byte_swap_long(state);
            header->files[index].written_size = byte_swap_long(written_size);
            header->files[index].compressed_size = byte_swap_long(compressed_size);
            header->files[index].uncompressed_size = byte_swap_long(uncompressed_size);
        }
        success = success && file_set_position(file, 0, false);
        success = success && file_write(file, sizeof(file_buffer), file_buffer.bytes);
        unsigned long remaining = 256;
        unsigned char volatile *key = header->encrypted_key;
        do
        {
            *key++ = 0;
        } while (--remaining);
    }
    return success;
}
// @address 0x2ff258 online_files_writer_begin_file
extern "C" unsigned char online_files_writer_begin_file(c_minidump_file_output_secure *object)
{
    bool success = false;
    long index = object->m_count;
    if (index < 10)
    {
        s_online_files_writer_statistics *statistics = &g_online_files_writer_statistics[index];
        statistics->state = 0;
        statistics->file_size = object->buffered_writer.file->position + object->buffered_writer.buffer_position;
        statistics->checksum = object->tell();
        if (data_compress_encrypted_writer_initialize(&object->encrypted_writer))
            success = 1;
    }
    return success;
}
// @address 0x2ff2e8 online_files_writer_end_file
extern "C" unsigned char online_files_writer_end_file(c_minidump_file_output_secure *object)
{
    s_online_files_encrypted_writer *writer = &object->encrypted_writer;
    s_online_files_writer_statistics *statistics = &g_online_files_writer_statistics[object->m_count];
    bool success = false;
    unsigned char result = data_compress_encrypted_writer_finish(writer);
    statistics->compressed_size = writer->compressed_size;
    statistics->uncompressed_size = writer->uncompressed_size;
    statistics->written_size = writer->written_size;
    if (result)
        success = 1;
    return success;
}
// @address 0x2fee30 ?seek@c_minidump_file_output_secure@@UAA_NJ@Z
bool c_minidump_file_output_secure::seek(long value)
{
    bool success = false;
    m_value = value;
    if (online_files_writer_end_file(this))
    {
        m_count++;
        if (online_files_writer_begin_file(this))
            success = 1;
    }
    if (!success)
        close();
    return success;
}
static __forceinline bool online_files_file_is_open(const c_minidump_file_output_secure *object)
{
    return object->file.handle != 0 && object->file.handle != (void *)-1;
}
static __forceinline bool flush_and_write_header(c_minidump_file_output_secure *object)
{
    return buffered_file_writer_flush(&object->buffered_writer) && online_files_write_archive_header(object, false);
}
// @address 0x2fecf0 ?close@c_minidump_file_output_secure@@UAA_NXZ
bool c_minidump_file_output_secure::close()
{
    bool success = true;
    if (online_files_file_is_open(this))
    {
        if (online_files_writer_end_file(this))
            success = flush_and_write_header(this);
        data_compress_encrypted_writer_dispose(&encrypted_writer);
        file_close(&file);
    }
    m_open = false;
    return success;
}

// match: 0x3b8768, 0x3b90b0, 0x3b9b00, 0x3ba368: inline the filename logic while retaining the addressed method.
static __forceinline void online_files_compress_get_base_filename(
    s_work_item_transfer_task *task, c_static_string<17> *filename)
{
    char name[256];
    file_reference_get_name(task->file_reference, 4, name, sizeof(name));
    filename->set(name);
}

void s_work_item_transfer_task::get_base_filename(c_static_string<17> *filename)
{
    online_files_compress_get_base_filename(this, filename);
}

void online_files_memory_initialize(e_map_memory_configuration configuration)
{
    c_online_file_manager::get()->memory_initialize(configuration);
}

// 0x3b6f48
extern "C" void online_files_memory_dispose(void)
{
    c_online_file_manager::get()->stop();
    c_online_file_manager *manager = c_online_file_manager::get();
    if (manager->m_buffer)
    {
        manager->m_buffer = 0;
        manager->m_buffer_size = 0;
    }
}

// @address 0x3b70e8 ?stop@c_online_file_manager@@QAAXXZ
void c_online_file_manager::stop()
{
    if (m_task_id != -1)
    {
        online_files_synchronized_set(&m_task_cancelled, 1);
        async_task_change_priority(m_task_id, _async_priority_blocking_generic);
        async_block_until_done(&m_task_done, simple_yield_function, true, false, 0, 0);
        online_files_synchronized_set(&m_task_done.value, -1);
    }
    if (m_upload_id != -1)
        async_block_until_done(&m_upload_status, simple_yield_function, true, false, 0, 0);
    if (m_download_id != -1)
        async_block_until_done(&m_download_status, simple_yield_function, true, false, 0, 0);
    m_active = false;
}

void c_online_file_manager::enqueue_for_transfer(e_queued_work_item_type type, const s_online_file_description *description,
    const char *name, const char *hash, unsigned __int64 owner_xuid, unsigned __int64 requester_xuid, long slot, bool user_initiated)
{
    c_static_string<256> filename(name);
    s_queued_work_item work_item;
    memset(&work_item, 0, sizeof(work_item));
    work_item.type = type;
    work_item.description = *description;
    work_item.priority = m_transfer_priority.decrement();
    work_item.user_initiated = user_initiated;
    filename.copy_to(work_item.name, sizeof(work_item.name));
    if (hash)
        memcpy(work_item.hash, hash, sizeof(work_item.hash));
    work_item.owner_xuid = owner_xuid;
    work_item.requester_xuid = requester_xuid;
    work_item.slot = slot;
    m_enqueuer.enqueue(&work_item);
}

// 0x3b7a18
extern "C" unsigned char online_files_is_transfer_ready(c_online_file_manager *manager)
{
    bool result = manager->m_active && simulation_starting_up() && tls_game_globals()->options.game_mode == 3 && online_files_is_catalogue_ready() &&
        manager->m_http_client.get_upstream_quota() > 0;
    return result;
}

// @address 0x3b71f8 ?update@c_online_file_manager@@QAAXXZ
void c_online_file_manager::update()
{
    if (m_active && simulation_starting_up() && tls_game_globals()->options.game_mode == 3 && online_files_is_catalogue_ready())
    {
        switch (m_state)
        {
        case _transfer_state_none:
        {
            long time = GetTickCount();
            if (m_refresh_queue)
            {
                memset(m_transfer_data.enumeration.transfers, 0, sizeof(m_transfer_data.enumeration));
                start_enumerate_files();
                m_status_known = false;
                m_refresh_queue = false;
                float refresh_scale = _real_random_range(get_local_random_seed_address(), 0.5f, 2.0f);
                m_next_refresh_time = (long)(refresh_scale * 30000.0f + (float)time);
            }
            else if (time >= m_next_refresh_time)
            {
                m_refresh_queue = true;
                m_active_transfer_tracker.mark_dirty();
            }
            break;
        }
        case _transfer_state_enumerating_files:
            if (online_files_synchronized_get(&m_task_done.value))
            {
                m_task_id = -1;
                if (online_files_synchronized_get(&m_task_succeeded))
                {
                    end_enumerate_files();
                }
                else
                {
                    if (!online_files_synchronized_get(&m_task_cancelled))
                        post_connection_error(false, NULL);
                    m_state = _transfer_state_none;
                }
            }
            break;
        case 3:
            if (online_files_synchronized_get(&m_task_done.value))
            {
                m_task_id = -1;
                m_state = _transfer_state_none;
                if (online_files_synchronized_get(&m_task_succeeded))
                {
                    m_error_posted = false;
                    m_refresh_queue = true;
                    m_active_transfer_tracker.refresh();
                }
                else
                {
                    if (!online_files_synchronized_get(&m_task_cancelled))
                        post_connection_error(m_task_error_forced, m_transfer_data.enumeration.transfers);
                    m_refresh_queue = false;
                    m_active_transfer_tracker.refresh();
                }
            }
            break;
        case 4:
            if (online_files_synchronized_get(&m_task_done.value))
            {
                m_task_id = -1;
                m_state = _transfer_state_none;
                if (online_files_synchronized_get(&m_task_succeeded))
                {
                    m_error_posted = false;
                    m_refresh_queue = true;
                    m_active_transfer_tracker.mark_dirty();
                }
                else
                {
                    if (!online_files_synchronized_get(&m_task_cancelled))
                        post_connection_error(m_task_error_forced, m_transfer_data.enumeration.transfers);
                    m_refresh_queue = false;
                    m_active_transfer_tracker.refresh();
                }
            }
            break;
        case 2:
        case 5:
            if (online_files_synchronized_get(&m_task_done.value))
            {
                m_refresh_queue = true;
                m_active_transfer_tracker.mark_dirty();
                m_task_id = -1;
                m_state = _transfer_state_none;
                if (!online_files_synchronized_get(&m_task_succeeded) && !online_files_synchronized_get(&m_task_cancelled))
                    post_connection_error(m_task_error_forced, m_transfer_data.enumeration.transfers);
            }
            break;
        }
    }
    bool changed = false;
    m_enqueuer.update(&changed);
    if (changed || m_signed_in_users_watcher.have_users_changed())
    {
        m_refresh_queue = true;
        m_active_transfer_tracker.mark_dirty();
    }
    m_active_transfer_tracker.update();
    if (m_upload_id != -1 && online_files_synchronized_get(&m_upload_status.value))
    {
        if (m_active)
        {
            stop();
            start();
        }
        else
        {
            m_refresh_queue = true;
            m_active_transfer_tracker.mark_dirty();
        }
        m_upload_id = -1;
    }
}

void c_online_file_manager::post_connection_error(bool forced, const s_queued_work_item_status_tracker *transfers)
{
    if (!m_error_posted && !forced)
        g_gui_error_manager.post_error_with_custom_message(0x20043, 0, 0, k_any_controller, false, false);
    else if (forced)
        g_gui_error_manager.post_error_with_custom_message(0x20044, 0, 0, k_any_controller, false, false);
    m_status_known = true;
    m_error_posted = true;
}

void c_online_file_manager::start_enumerate_files()
{
    memset(&m_transfer_data, 0, sizeof(m_transfer_data));
    m_transfer_data.enumeration.transfer_count = 0;
    m_transfer_data.enumeration.upstream_quota = 0;
    m_transfer_data.enumeration.transfer_priority = 0;
    s_async_task task;
    memset(&task, 0, sizeof(task));
    task.enumerate_files.transfers = m_transfer_data.enumeration.transfers;
    task.enumerate_files.transfer_count = &m_transfer_data.enumeration.transfer_count;
    task.enumerate_files.maximum_transfer_count = 16;
    task.enumerate_files.upstream_quota = &m_transfer_data.enumeration.upstream_quota;
    task.enumerate_files.transfer_priority = &m_transfer_data.enumeration.transfer_priority;
    task.enumerate_files.succeeded = &m_task_succeeded;
    task.enumerate_files.cancelled = &m_task_cancelled;
    online_files_synchronized_set(&m_task_done.value, 0);
    online_files_synchronized_set(&m_task_succeeded, 0);
    online_files_synchronized_set(&m_task_cancelled, 0);
    m_task_id = ::async_task_add_ex(_async_priority_predicted, &task, _async_category_online_files, async_do_enumerate_and_or_cleanup_files,
        &m_task_done, true);
    m_state = _transfer_state_enumerating_files;
}

void c_online_file_manager::end_enumerate_files()
{
    m_upstream_quota = m_transfer_data.enumeration.upstream_quota;
    m_transfer_priority = m_transfer_data.enumeration.transfer_priority;
    if (m_transfer_data.enumeration.transfer_count > 0)
    {
        switch (m_transfer_data.enumeration.transfers[0].status.state)
        {
        case _queued_work_item_status_state_upload_queued:
            start_compressing_file(&m_transfer_data.enumeration.transfers[0]);
            break;
        case _queued_work_item_status_state_compressed:
            start_uploading_file(&m_transfer_data.enumeration.transfers[0], false);
            break;
        case _queued_work_item_status_state_uploading:
            start_uploading_file(&m_transfer_data.enumeration.transfers[0], true);
            break;
        case _queued_work_item_status_state_download_queued:
            start_downloading_file(&m_transfer_data.enumeration.transfers[0], false);
            break;
        case _queued_work_item_status_state_downloading:
            start_downloading_file(&m_transfer_data.enumeration.transfers[0], true);
            break;
        case _queued_work_item_status_state_downloaded:
            start_decompressing_file(&m_transfer_data.enumeration.transfers[0]);
            break;
        default:
            // match: 0x3b7c30: default is unreachable; omits the state range check.
            __assume(0);
        }
    }
    else
    {
        m_state = _transfer_state_none;
    }
}

void c_online_file_manager::setup_task(s_work_item_transfer_task *task)
{
    c_online_file_manager *manager = this;
#ifdef HALO3_64BIT
    memset(task, 0, sizeof(*task));
#else
    long *task_word = (long *)task;
    int words_remaining = 10;
    do { *task_word = 0; task_word++; words_remaining--; } while (words_remaining != 0);
#endif
    task->flag_c = 0;
    task->flag_d = 0;
    task->second = -1;
    task->first = -1;
    task->succeeded = &manager->m_task_succeeded;
    task->cancelled = &manager->m_task_cancelled;
    task->area = (char *)&manager->m_active_transfer_tracker;
    task->error_forced = &manager->m_task_error_forced;
    __lwsync();
    _InterlockedExchange(&manager->m_task_done.value, 0);
    __lwsync();
    _InterlockedExchange(&manager->m_task_succeeded, 0);
    __lwsync();
    _InterlockedExchange(&manager->m_task_cancelled, 0);
    manager->m_task_error_forced = 0;
}

void c_online_file_manager::create_file_reference_with_extension(s_file_reference *reference, const char *name, const char *extension)
{
    char path[256];
    path[0] = 0;
    ((c_static_string<256> *)path)->print("%s\\%s.%s", "cache1:\\upload_queue", name, extension);
    file_reference_create_from_path(reference, path, false);
}

static __forceinline void online_files_initialize_buffered_writer(s_online_file_buffered_writer *writer, s_file_reference *file)
{
    writer->file = file;
    writer->buffer_size = sizeof(writer->write_buffer);
    writer->buffer_position = 0;
    writer->buffer = writer->write_buffer;
}

// @address 0x2febf8 ?open@c_minidump_file_output_secure@@UAAXPBD@Z
void c_minidump_file_output_secure::open(const char *path)
{
    bool success = false;
    unsigned long error = 0;
    file_reference_create_from_path(&file, path, false);
    if (file_create(&file) && file_open(&file, 3, &error))
    {
        online_files_initialize_buffered_writer(&buffered_writer, &file);
        if (online_files_write_archive_header(this, 1))
        {
            s_online_files_encrypted_writer *writer = &encrypted_writer;
            XeCryptRandom(writer, 16);
            writer->writer = &buffered_writer;
            m_count = 0;
            if (online_files_writer_begin_file(this))
                success = 1;
        }
    }
    if (success)
        m_open = true;
    else
        close();
}

// @address 0x2fedc0 ?write@c_minidump_file_output_secure@@UAA_NPBXJ@Z
bool c_minidump_file_output_secure::write(const void *data, long size)
{
    bool success = false;
    if (data_compress_encrypted_writer_write(&encrypted_writer, data, size, false))
    {
        success = true;
        m_value += size;
    }
    if (!success)
        close();
    return success;
}

// @address 0x60d148 ?tell@c_minidump_file_output_secure@@UAAJXZ
long c_minidump_file_output_secure::tell()
{
    return m_value;
}

// @address 0x3b7e20 ?start_compressing_file@c_online_file_manager@@AAAXPBUs_queued_work_item_status_tracker@@@Z
void c_online_file_manager::start_compressing_file(const s_queued_work_item_status_tracker *tracker)
{
    s_queued_work_item_status_tracker transfer = *tracker;
    memset(&m_transfer_data, 0, sizeof(m_transfer_data));
    create_file_reference_with_extension(&m_transfer_data.compression.queued_file, transfer.filename, "queued");
    create_file_reference_with_extension(&m_transfer_data.compression.decompressed_file, transfer.filename,
        "compressing");
    s_async_task task;
    setup_task(&task.compress_file.transfer);
    task.compress_file.transfer.file_reference = &m_transfer_data.compression.queued_file;
    task.compress_file.transfer.source_file = &m_transfer_data.compression.transfer_file;
    task.compress_file.transfer.destination_file = &m_transfer_data.compression.decompressed_file;
    task.compress_file.chunk_size = -1;
    task.compress_file.chunk_position = -1;
    task.compress_file.compressed_position = -1;
    task.compress_file.buffer = (char *)m_buffer;
    task.compress_file.buffer_size = m_buffer_size;
    task.compress_file.work_item = &m_transfer_data.compression.work_item;
    task.compress_file.zlib_memory = &m_transfer_data.compression.zlib_memory;
    task.compress_file.stream = &m_transfer_data.compression.stream;
    task.compress_file.writer = &m_writer;
    online_files_initialize_buffered_writer(task.compress_file.writer, task.compress_file.transfer.destination_file);
    task.compress_file.state = 0;
    m_task_id = ::async_task_add_ex(_async_priority_predicted, &task, _async_category_online_files, async_do_compression, &m_task_done, true);
    m_state = 2;
}


void c_online_file_manager::start_uploading_file(const s_queued_work_item_status_tracker *tracker, bool resume)
{
    s_queued_work_item_status_tracker transfer = *tracker;
    memset(&m_transfer_data, 0, sizeof(m_transfer_data));
    create_file_reference_with_extension(&m_transfer_data.upload.queued_file, transfer.filename, "queued");
    create_file_reference_with_extension(&m_transfer_data.upload.transfer_file, transfer.filename,
        resume ? "uploading" : "compressed");
    s_async_task task;
    setup_task(&task.upload_file.transfer);
    task.upload_file.transfer.file_reference = &m_transfer_data.upload.queued_file;
    task.upload_file.transfer.source_file = &m_transfer_data.upload.transfer_file;
    task.upload_file.upload_buffer = (char *)&m_http_client;
    task.upload_file.response_buffer = (char *)&m_post_stream;
    task.upload_file.bytes_uploaded = 0;
    task.upload_file.flags = resume ? 2 : 0;
    task.upload_file.request_cookie = -1;
    task.upload_file.work_item = &m_transfer_data.upload.work_item;
    task.upload_file.url = m_transfer_data.upload.url;
    task.upload_file.url_size = sizeof(m_transfer_data.upload.url);
    m_task_id = ::async_task_add_ex(_async_priority_predicted, &task, _async_category_online_files, async_do_upload, &m_task_done, true);
    m_state = 3;
}

// @address 0x3b8038 ?start_downloading_file@c_online_file_manager@@AAAXPBUs_queued_work_item_status_tracker@@_N@Z
void c_online_file_manager::start_downloading_file(const s_queued_work_item_status_tracker *tracker, bool shared)
{
    s_queued_work_item_status_tracker transfer = *tracker;
    memset(&m_transfer_data, 0, sizeof(m_transfer_data));
    create_file_reference_with_extension(&m_transfer_data.download.queued_file, transfer.filename, "queued");
    create_file_reference_with_extension(&m_transfer_data.download.transfer_file, transfer.filename, "downloading");
    s_async_task task;
    setup_task(&task.download_file.transfer);
    task.download_file.transfer.file_reference = &m_transfer_data.download.queued_file;
    task.download_file.transfer.destination_file = &m_transfer_data.download.transfer_file;
    task.download_file.client = &m_http_client;
    task.download_file.stream = (char *)&m_get_stream;
    task.download_file.flags = 0;
    task.download_file.mode = shared ? 1 : 0;
    task.download_file.request_cookie = -1;
    task.download_file.work_item = &m_transfer_data.download.work_item;
    task.download_file.input = m_transfer_data.download.input;
    task.download_file.output = m_transfer_data.download.output;
    task.download_file.output_capacity = sizeof(m_transfer_data.download.output);
    task.download_file.writer = &m_writer;
    task.download_file.start_time = GetTickCount();
    online_files_initialize_buffered_writer(task.download_file.writer, task.download_file.transfer.destination_file);
    m_task_id = ::async_task_add_ex(_async_priority_predicted, &task, _async_category_online_files, async_do_download, &m_task_done, true);
    m_state = 4;
}


void c_online_file_manager::start_decompressing_file(const s_queued_work_item_status_tracker *tracker)
{
    s_queued_work_item_status_tracker transfer = *tracker;
    memset(&m_transfer_data, 0, sizeof(m_transfer_data));
    create_file_reference_with_extension(&m_transfer_data.compression.queued_file, transfer.filename, "queued");
    create_file_reference_with_extension(&m_transfer_data.compression.transfer_file, transfer.filename,
        "downloaded");
    s_async_task task;
    setup_task(&task.decompress_file.transfer);
    task.decompress_file.transfer.file_reference = &m_transfer_data.compression.queued_file;
    task.decompress_file.transfer.source_file = &m_transfer_data.compression.transfer_file;
    task.decompress_file.transfer.destination_file = &m_transfer_data.compression.decompressed_file;
    task.decompress_file.transfer.first = -1;
    task.decompress_file.bytes_decompressed = 0;
    task.decompress_file.source_handle = -1;
    task.decompress_file.destination_handle = -1;
    task.decompress_file.decompressor = -1;
    task.decompress_file.buffer = m_buffer;
    task.decompress_file.buffer_size = m_buffer_size;
    task.decompress_file.work_item = &m_transfer_data.compression.work_item;
    task.decompress_file.zlib_memory = &m_transfer_data.compression.zlib_memory;
    task.decompress_file.stream = &m_transfer_data.compression.stream;
    task.decompress_file.bytes_written = 0;
    task.decompress_file.bytes_read = 0;
    m_task_id = ::async_task_add_ex(_async_priority_predicted, &task, _async_category_online_files, async_do_decompression, &m_task_done, true);
    m_state = 5;
}


// @address 0x3bae68 ?async_do_bump_transfer_priority@c_online_file_manager@@CA?AW4e_async_completion@@PAUs_async_task@@@Z
e_async_completion c_online_file_manager::async_do_bump_transfer_priority(s_async_task *task)
{
    bool lock_acquired = false;
    c_critical_section_scope critical_section(29, 1000, &lock_acquired);
    if (lock_acquired)
    {
        s_file_reference reference;
        s_queued_work_item item;
        create_file_reference_with_extension(&reference, task->bump_transfer_priority.filename, "queued");
        if (read_queued_file(&reference, &item))
        {
            item.priority = task->bump_transfer_priority.priority;
            write_queued_file(&reference, &item);
        }
    }
    return (e_async_completion)(lock_acquired != false);
}


// @address 0x3baf28 online_files_async_delete_transfer_files
extern "C" e_async_completion online_files_async_delete_transfer_files(s_async_task *task)
{
    const char *extensions[] = {"queued", "compressed", "uploading",
        "downloading", "downloaded"};
    s_file_reference reference;
    const char *format = "%s\\%s.%s";
    const char *directory = "cache1:\\upload_queue";
    const char **extension = extensions;
    unsigned long remaining = 5;
    do
    {
        char path[256];
        path[0] = 0;
        ((c_static_string<256> *)path)->print(format, directory, task->delete_transfer_files.filename, *extension);
        file_reference_create_from_path(&reference, path, false);
        if (file_exists(&reference))
            file_delete(&reference);
        remaining--;
        extension++;
    } while (remaining);
    c_online_file_manager::get()->m_refresh_queue = true;
    c_online_file_manager::get()->m_active_transfer_tracker.mark_dirty();
    return _async_completion_done;
}

// @address 0x3bb008 ?async_do_enqueue_for_transfer@c_online_files_enqueuer@@CA?AW4e_async_completion@@PAUs_async_task@@@Z
e_async_completion c_online_files_enqueuer::async_do_enqueue_for_transfer(s_async_task *task)
{
    bool success = false;
    s_file_reference reference;
    file_reference_create_from_path(&reference, task->enqueue_transfer.filename, false);
    file_create_parent_directories_if_not_present(&reference);
    if (file_create(&reference))
    {
        unsigned long error;
        if (file_open(&reference, 2, &error))
        {
            c_online_files_buffered_writer writer(&reference);
            if (buffered_file_write(&writer, task->enqueue_transfer.item,
                sizeof(s_queued_work_item)) && buffered_file_writer_flush(&writer))
                success = true;
            file_close(&reference);
        }
        if (!success)
            file_delete(&reference);
    }
    online_files_synchronized_set(task->enqueue_transfer.success, success);
    return _async_completion_done;
}

void c_online_files_active_transfer_tracker::start_enumerate_files()
{
    c_critical_section_scope lock(29);
    s_async_task task;
    memset(&task, 0, sizeof(task));
    task.enumerate_transfers.tracker = this;
    task.enumerate_transfers.success = &m_enumeration_succeeded;
    task.enumerate_transfers.done = &m_enumeration_done;
    m_enumeration_succeeded = false;
    online_files_synchronized_set(&m_task_done, 0);
    online_files_synchronized_set(&m_enumeration_done, 0);
    m_task_id = ::async_task_add_ex(_async_priority_blocking_generic, &task, _async_category_online_files, async_do_enumerate_files,
        (c_synchronized_long *)&m_task_done, true);
    m_state = 1;
}


template<int k_size>
static __forceinline char *file_reference_get_name(const s_file_reference *reference, unsigned long parts, char (&name)[k_size])
{
    return file_reference_get_name(reference, parts, name, k_size);
}

// @address 0x3bb1f0 ?async_do_enumerate_files@c_online_files_active_transfer_tracker@@CA?AW4e_async_completion@@PAUs_async_task@@@Z
e_async_completion c_online_files_active_transfer_tracker::async_do_enumerate_files(s_async_task *task)
{
    bool lock_acquired = false;
    c_critical_section_scope lock(29, 1000, &lock_acquired);
    if (lock_acquired)
    {
        s_online_files_tracker_enumerate_task *enumeration = &task->enumerate_transfers;
        unsigned long transfer_count = 0;
        s_find_file_data find_data;
        s_file_reference directory;
        s_queued_work_item_status_tracker transfers[16];
        file_reference_create_from_path(&directory, "cache1:\\upload_queue", true);
        find_files_start(&find_data, 0, &directory, "*.*");
        s_file_reference reference;
        while (find_files_next(&find_data, &reference, 0) && transfer_count < 16)
        {
            c_online_files_extension_string extension;
            char name[256];
            file_reference_get_name(&reference, 8, name);
            extension.set(name);
            if (extension.is_equal("queued"))
            {
                bool found = false;
                char filename[17];
                if (get_work_item_status_from_queued_file(&reference, &transfers[transfer_count].status, filename, &found))
                {
                    if (found)
                    {
                        memcpy(transfers[transfer_count].filename, filename, sizeof(filename));
                        transfer_count++;
                    }
                }
                else
                    file_delete(&reference);
            }
        }
        find_files_end(&find_data);
        enumeration->tracker->set_transfers(transfers, transfer_count, enumeration->success);
        online_files_synchronized_set(enumeration->done, 1);
    }
    return _async_completion_done;
}

void c_online_files_active_transfer_tracker::set_transfers(
    s_queued_work_item_status_tracker *new_transfers, long new_count, bool *changed)
{
    long matching_count = 0;
    s_queued_work_item_status_tracker old_transfers[16];
    memcpy(old_transfers, m_transfers, m_transfer_count * sizeof(old_transfers[0]));
    long old_count = m_transfer_count;
    memcpy(m_transfers, new_transfers, new_count * sizeof(new_transfers[0]));
    m_transfer_count = new_count;
    for (long index = 0; index < m_transfer_count; ++index)
    {
        for (long old_index = 0; old_index < old_count; ++old_index)
        {
            if (csstrnicmp(m_transfers[index].filename, old_transfers[old_index].filename, 16) == 0)
            {
                m_transfers[index].status.state = old_transfers[old_index].status.state;
                m_transfers[index].status.progress = old_transfers[old_index].status.progress;
                ++matching_count;
                break;
            }
        }
    }
    if (changed)
        *changed = matching_count != new_count || matching_count != old_count;
    sort_transfers();
}

// @address 0x3bb528 ?update_transfer_status@c_online_files_active_transfer_tracker@@QAAXPBDW4e_queued_work_item_status_state@@M@Z
void c_online_files_active_transfer_tracker::update_transfer_status(const char *filename,
    e_queued_work_item_status_state state, float progress)
{
    bool lock_acquired = false;
    c_critical_section_scope lock(29, 1000, &lock_acquired);
    if (lock_acquired)
    {
        for (long index = 0; index < m_transfer_count; ++index)
        {
            char transfer_filename[256];
            strncpy(transfer_filename, m_transfers[index].filename, 256);
            transfer_filename[255] = 0;
            if (strncmp(transfer_filename, filename, 256) == 0)
            {
                if (index >= 0 && index < m_transfer_count)
                {
                    m_transfers[index].status.state = state;
                    m_transfers[index].status.progress = progress;
                }
                break;
            }
        }
        sort_transfers();
    }
    else
        m_status_dirty = true;
}

// @address 0x3bb638 ?get_transfer_status@c_online_files_active_transfer_tracker@@QBA_NJPAUs_queued_work_item_status@@@Z
bool c_online_files_active_transfer_tracker::get_transfer_status(long transfer_index, s_queued_work_item_status *status) const
{
    bool result = false;
    c_critical_section_scope lock(29);
    if (transfer_index >= 0 && transfer_index < 16)
    {
        *status = m_transfers[transfer_index].status;
        if (transfer_index != 0)
        {
            switch (status->state)
            {
            case _queued_work_item_status_state_compressing:
            case _queued_work_item_status_state_compressed:
            case _queued_work_item_status_state_uploading:
                status->state = _queued_work_item_status_state_upload_queued;
                break;
            case _queued_work_item_status_state_downloading:
            case _queued_work_item_status_state_downloaded:
            case _queued_work_item_status_state_decompressing:
                status->state = _queued_work_item_status_state_download_queued;
                break;
            case _queued_work_item_status_state_download_queued:
            default:
                break;
            }
            status->progress = 0.0f;
        }
        result = true;
    }
    return result;
}

static __forceinline void online_files_critical_section_leave(long index)
{
    _InterlockedDecrement(&g_synch_globals.critical_sections[index].lock_count.value);
    RtlLeaveCriticalSection((_RTL_CRITICAL_SECTION *)&g_synch_globals.critical_sections[index].critical_section);
}
bool c_online_files_active_transfer_tracker::get_transfer_status_by_slot(unsigned __int64 owner_xuid, long slot,
    s_queued_work_item_status *status) const
{
    bool result = false;
    internal_critical_section_enter(29);
    for (long index = 0; index < m_transfer_count; index++)
    {
        if (m_transfers[index].status.owner_xuid == owner_xuid && m_transfers[index].status.slot == slot)
        {
            result = get_transfer_status(index, status);
            break;
        }
    }
    online_files_critical_section_leave(29);
    return result;
}


// @address 0x3bb7c0 ?stop@c_online_files_enumerator@@QAAXXZ
void c_online_files_enumerator::stop()
{
    if (is_busy())
    {
        c_network_http_request_queue *queue = c_network_http_request_queue::get(_network_http_request_queue_files);
        queue->cancel_request(m_request_cookie);
        m_request_cookie = 0;
        m_xuid = 0;
        m_owner_xuid = 0;
        m_state = 3;
        m_quota_bytes = 0;
        m_quota_slots = 0;
        m_visible_slots = 0;
    }
}

// @address 0x3bb830 ?update@c_online_files_enumerator@@QAAXXZ
void c_online_files_enumerator::update()
{
    if (m_owner_xuid != 0 && m_state == 0)
    {
        if (m_request_cookie == 0)
        {
            c_static_string<256> url;
            url.m_string[0] = 0;
            url.print("/gameapi/FilesGetCatalog.ashx?userId=%016I64x&shareId=%016I64x&locale=%s", m_xuid, m_owner_xuid, get_current_language_suffix(0));
            c_network_http_request_description description(url.m_string, _http_request_type_get);
            c_network_http_request_queue *queue = c_network_http_request_queue::get(_network_http_request_queue_files);
            m_request_cookie = queue->start_request_into_buffer(online_lsp_service_shr, &description, m_results, sizeof(m_results));
            return;
        }

        c_network_http_request_queue *queue = c_network_http_request_queue::get(_network_http_request_queue_files);
        long position;
        long result = queue->is_fill_buffer_complete(m_request_cookie, &position, 0,
                    0);
        m_state = result;
        if (result == 1)
        {
            m_request_cookie = 0;
            if (position > 0)
            {
                process_request_result();
                return;
            }
        }
        else if (result == 2)
        {
            m_request_cookie = 0;
        }
    }
}


// @address 0x3bc240 ?update@c_online_files_deleter@@QAAXXZ
void c_online_files_deleter::update()
{
    if (m_owner_xuid != 0 && m_status == 0)
    {
        if (m_request_cookie == 0)
        {
            c_static_string<256> url;
            url.m_string[0] = 0;
            url.print("/gameapi/FilesDelete.ashx?userId=%016I64x&shareId=%016I64x&slot=%d&serverId=%s", m_xuid, m_owner_xuid, m_slot_index, m_name);
            c_network_http_request_description description(url.m_string, _http_request_type_get);
            m_request_cookie = c_network_http_request_queue::get(_network_http_request_queue_files)
                                   ->start_request_into_buffer(online_lsp_service_shr, &description, m_results, sizeof(m_results));
            return;
        }
        long position;
        long result = c_network_http_request_queue::get(_network_http_request_queue_files)->is_fill_buffer_complete(m_request_cookie, &position, 0,
                    0);
        m_status = result;
        if (result == 1 || result == 2)
            m_request_cookie = 0;
    }
}

// @address 0x3bc338 ?read_queued_file@@YA_NPAUs_file_reference@@PAUs_queued_work_item@@@Z
bool read_queued_file(s_file_reference *reference, s_queued_work_item *item)
{
    bool success = false;
    unsigned long error = 0;
    if (file_open(reference, 1, &error))
    {
        if (file_read(reference, sizeof(s_queued_work_item), true, item))
            success = true;
        file_close(reference);
    }
    return success;
}

// @address 0x3bc3a8 ?write_queued_file@@YA_NPAUs_file_reference@@PBUs_queued_work_item@@@Z
bool write_queued_file(s_file_reference *reference, const s_queued_work_item *item)
{
    bool success = false;
    unsigned long error = 0;
    if (file_open(reference, 3, &error))
    {
        char buffer[4096];
        s_buffered_file_writer writer;
        writer.file = reference;
        writer.buffer = buffer;
        writer.buffer_size = sizeof(buffer);
        writer.buffer_position = 0;
        if (buffered_file_write(&writer, item, sizeof(s_queued_work_item)) && buffered_file_writer_flush(&writer))
            success = true;
        file_close(reference);
    }
    return success;
}

extern "C" {

// @address 0x3a8620 netdebug_build_upload_text
void netdebug_build_upload_text(c_http_post_stream *text_owner, const s_netdebug_upload_task *settings)
{
    c_static_string<1024> *text;
    unsigned short ch;
    c_controller_interface *controller;
    int remaining;
    int cache_index;
    const wchar_t *name_src;
    char *dst;
    const char *p;
    int len;
    char insecure_address_string[16];
    char gamertags[80];
    c_static_string<33> cache_index_string;
    c_static_string<33> file_crc_string;
    char gamertag[80];
    char secure_address_string[128];
    insecure_address_string[0] = 0;
    secure_address_string[0] = 0;
    // match: 0x3a8620: these clears keep the two zero stores after secure_address_string's; the
    // constructor stores alone are scheduled ahead of insecure_address_string's.
    file_crc_string.clear();
    cache_index_string.clear();
    gamertags[0] = 0;
    transport_security_get_address_strings((c_static_string<16> *)insecure_address_string,
        (c_static_string<128> *)secure_address_string);
    text_owner->get_headers().m_string[0] = 0;
    text = &text_owner->get_headers();
    file_crc_string.print("%u", settings->file_crc);
    text->append_print("%s: \"%s\"\r\n", "digest", file_crc_string.get_string());
    text->append_print("%s: \"%s\"\r\n", "title", g_netdebug_globals.project);
    text->append_print("%s: \"%s\"\r\n", "build",
        g_netdebug_globals.build_version);
    cache_index = -1;
    if (global_scenario) {
        cache_index = g_cache_file_globals.header.minor_version;
    }
    cache_index_string.print("%i", cache_index);
    text->append_print("%s: \"%s\"\r\n", "map_minor_version", cache_index_string.get_string());
    text->append_print("%s: \"%s\"\r\n", "systemid",
        g_netdebug_globals.system_name);
    text->append_print("%s: \"%s\"\r\n", "sessionid", g_netdebug_globals.session_id);
    text->append_print("%s: \"%s\"\r\n", "test_option", "none");
    text->append_print("%s: \"%s\"\r\n", "insecure_ip", insecure_address_string);
    text->append_print("%s: \"%s\"\r\n", "secure_ip", secure_address_string);
    text->append_print("%s: \"%s\"\r\n", "platform",
        "xenon");
    text->append_print("%s: \"%s\"\r\n", "flash", g_netdebug_globals.flash_version);
    text->append_print("%s: \"%s\"\r\n", "xdk", g_netdebug_globals.xdk_version);
    for (long controller_index = _controller0; controller_index < k_number_of_controllers; controller_index++) {
        controller = controller_get((e_controller_index)controller_index);
        if (controller && controller->is_signed_in_to_machine()) {
            gamertag[0] = 0;
            name_src = controller->get_display_name();
            dst = gamertag;
            remaining = 65;
            do {
                ch = *name_src;
                if (remaining == 1) {
                    *dst = 0;
                } else if ((unsigned short)ch <= 127) {
                    *dst = ch;
                } else {
                    *dst = 63;
                }
                ++name_src;
                dst = dst + 1;
                remaining = remaining - 1;
            } while ((unsigned short)ch != 0 && remaining > 0);
            p = gamertags;
            for (len = 0; (unsigned int)len < 65; len++) {
                if (*p++ == 0)
                    break;
            }
            if (len > 0) {
                ((c_static_string<65> *)gamertags)->append(",");
            }
            ((c_static_string<65> *)gamertags)->append((const char *)gamertag);
        }
    }
    if (!gamertags[0]) {
        strncpy((char *)gamertags, "[unknown]", 65);
        gamertags[64] = 0;
    }
    text->append_print("%s: \"%s\"\r\n", "profiles", gamertags);
    text->append_print("%s: \"%s\"\r\n", "mission_segment", "");
    return;
}


}
static __forceinline unsigned __int64 online_files_controller_get_player_xuid(const c_controller_interface *controller)
{
    unsigned __int64 xuid = 0;
    if (controller->get_user_index() >= 0 && controller->get_user_index() < 4)
        xuid = online_presence_user_slot(controller->get_user_index()).xuid;
    return xuid;
}
// @address 0x3bc440 ?get_work_item_status_from_queued_file@@YA_NPAUs_file_reference@@PAUs_queued_work_item_status@@QADPA_N@Z
bool get_work_item_status_from_queued_file(s_file_reference *reference, s_queued_work_item_status *status,
    char *const filename, bool *found)
{
    bool valid = false;
    s_queued_work_item item;
    *found = false;
    if (read_queued_file(reference, &item) && (item.type == 1 || item.type == 2))
        valid = true;
    if (valid)
    {
        for (long controller_index = 0; controller_index < 4; controller_index++)
        {
            c_controller_interface *controller = controller_get((e_controller_index)controller_index);
            if (controller && controller->is_signed_in_to_live())
            {
                unsigned __int64 xuid = online_files_controller_get_player_xuid(controller);
                if (xuid == item.description.owner_xuid)
                {
                    c_static_string<17> base_filename;
                    status->type = item.type;
                    status->description = item.description;
                    status->priority = item.priority;
                    status->state = item.type == _queued_work_item_type_upload
                        ? _queued_work_item_status_state_upload_queued
                        : _queued_work_item_status_state_download_queued;
                    status->owner_xuid = item.requester_xuid;
                    status->slot = item.slot;
                    status->progress = 0.0f;
                    char name[256];
                    file_reference_get_name(reference, 4, name, sizeof(name));
                    base_filename.set(name);
                    base_filename.copy_to(filename, 17);
                    *found = true;
                    break;
                }
            }
        }
    }
    return valid;
}

enum e_controller_index get_controller_index_from_xuid(unsigned __int64 xuid)
{
    e_controller_index result = k_no_controller;
    for (long controller_index = _controller_index0; controller_index < k_number_of_controllers; controller_index++)
    {
        c_controller_interface *controller = controller_get((e_controller_index)controller_index);
        bool signed_in_to_live = controller->is_signed_in_to_live();
        if (controller && signed_in_to_live && online_files_controller_get_player_xuid(controller) == xuid)
        {
            result = (e_controller_index)controller_index;
            break;
        }
    }
    return result;
}

// 0x3bc670
// @address 0x2281d0 content_catalogue_has_pending_tasks
// match: 0x3b75f0: inline keeps is_catalogue_ready folded and the async_do_compression sum in r30; out of line it moves to r10
#if defined(_XBOX) && !defined(__clang__)
extern "C" inline bool content_catalogue_has_pending_tasks(const c_content_catalogue *storage)
#else
extern "C" bool content_catalogue_has_pending_tasks(const c_content_catalogue *storage)
#endif
{
    const c_content_catalogue *catalogue = (const c_content_catalogue *)storage;
    return catalogue->m_enumeration_task.busy() || catalogue->m_content_item_task.busy();
}

// @address 0x3bc670 online_files_cleanup_transfer_task
extern "C" void online_files_cleanup_transfer_task(s_work_item_transfer_task *task)
{
    if (task->flag_c)
    {
        file_close(task->source_file);
        task->flag_c = 0;
    }
    if (task->flag_d)
    {
        file_close(task->destination_file);
        task->flag_d = 0;
    }
    if (task->second != -1)
    {
        content_catalogue_close_item(&content_catalogues().catalogues[task->first], task->second, false);
        task->second = -1;
    }
}

// @address 0x3b75f0 online_files_is_catalogue_ready

static __forceinline bool online_files_catalogue_flag_1_set(const c_content_catalogue *catalogue)
{
    return (bool)(_rotl(catalogue->m_flags.value, 31) & 1);
}
extern "C" unsigned char online_files_is_catalogue_ready(void)
{
    bool any_catalogue = false;
    bool all_ok = true;
    for (long controller_index = _controller_index0; controller_index != k_number_of_controllers; controller_index++)
    {
        c_controller_interface *controller = controller_get((e_controller_index)controller_index);
        if (controller == 0 || !controller->is_signed_in_to_live())
            continue;
        const c_content_catalogue *catalogue =
            &content_catalogues().catalogues[controller - controller_get(_controller0)];
        if (!content_catalogue_has_storage_device(catalogue))
            continue;
        any_catalogue = 1;
        int ok = (catalogue != 0) && content_catalogue_has_storage_device(catalogue) &&
            online_files_catalogue_flag_1_set(catalogue) && !content_catalogue_has_pending_tasks(catalogue);
        all_ok = all_ok & ok;
    }
    int result = any_catalogue != 0 && all_ok != 0;
    return (unsigned char)result;
}


int work_item_tracker_sort_proc(const void *a, const void *b)
{
    int result = 0;
    const s_queued_work_item_status_tracker *first = (const s_queued_work_item_status_tracker *)a;
    const s_queued_work_item_status_tracker *second = (const s_queued_work_item_status_tracker *)b;
    if (first->status.priority > second->status.priority)
        result = -1;
    else if (first->status.priority < second->status.priority)
        result = 1;
    else if (first->status.description.metadata.date > second->status.description.metadata.date)
        result = 1;
    else if (first->status.description.metadata.date < second->status.description.metadata.date)
        result = -1;
    return result;
}

// 0x3bc748
extern "C" bool buffered_file_writer_set_position(s_buffered_file_writer *writer, unsigned long position)
{
    bool success = false;
    bool write_ok = true;
    if (writer->buffer_position > 0)
    {
        write_ok = file_write(writer->file, writer->buffer_size, writer->buffer);
        writer->buffer_position = 0;
    }
    if (write_ok)
    {
        unsigned long aligned_position = position & ~(writer->buffer_size - 1);
        if (writer->file->position != position)
        {
            if (file_set_position(writer->file, aligned_position, true))
            {
                if (aligned_position == position)
                {
                    success = true;
                }
                else if (saved_game_write_metadata_to_open_file_internal(writer))
                {
                    writer->buffer_position = position - aligned_position;
                    success = true;
                }
            }
        }
        else
        {
            success = true;
        }
    }
    return success;
}


extern "C" {
// @address 0x73a924 k_online_files_incompressible_blf_types
const long k_online_files_incompressible_blf_types[4] = { '_blf', '_eof', 'athr', 'chdr' };
}


static __forceinline void online_files_compress_stream_end(z_stream_s *stream, s_blam_zlib_memory *memory)
{
    z_deflateEnd(stream);
    memory->heap = 0;
}


static __forceinline void online_files_compress_set_truth(long *value, long new_value)
{
    online_files_synchronized_set(value, (unsigned char)new_value != 0);
}
// @address 0x3b8768 ?async_do_compression@c_online_file_manager@@CA?AW4e_async_completion@@PAUs_async_task@@@Z
e_async_completion c_online_file_manager::async_do_compression(s_async_task *task)
{
    bool success = false;
    bool finished = false;
    float progress = 0.0f;
    char buffer[0x2000];
    char input[0x2000];
    char output[0x2000];
    long cancelled_value = *task->compress_file.transfer.cancelled;
    __lwsync();
    bool cancelled = cancelled_value != 0;
    if (task->compress_file.state == 0)
    {
        if (!cancelled)
        {
            bool lock_acquired = false;
            c_critical_section_scope lock(29, 1000, &lock_acquired);
            if (lock_acquired)
            {
                if (read_queued_file(task->compress_file.transfer.file_reference, task->compress_file.work_item))
                {
                    task->compress_file.transfer.first = ::get_controller_index_from_xuid(
                        task->compress_file.work_item->description.owner_xuid);
                    file_reference_create_from_path(
                        task->compress_file.transfer.source_file, task->compress_file.work_item->name, false);
                    if (task->compress_file.transfer.first >= 0 && task->compress_file.transfer.first < 4)
                    {
                        task->compress_file.transfer.second = content_catalogue_find_item_by_file_reference(
                            &content_catalogues().catalogues[task->compress_file.transfer.first], task->compress_file.transfer.source_file);
                        if (task->compress_file.transfer.second != -1)
                        {
                            task->compress_file.state = 1;
                            success = true;
                        }
                    }
                }
            }
            else
                success = true;
        }
    }
    else if (task->compress_file.state == 1)
    {
        c_content_catalogue *catalogue = &content_catalogues().catalogues[task->compress_file.transfer.first];
        if (content_catalogue_has_pending_tasks(catalogue))
            success = true;
        else if (((s_content_item *)catalogue)->make_ready_for_access(task->compress_file.transfer.second, false))
        {
            success = true;
            task->compress_file.state = 2;
        }
    }
    else if (task->compress_file.state == 2)
    {
        if (!cancelled)
        {
            if (content_catalogue_has_pending_tasks(&content_catalogues().catalogues[task->compress_file.transfer.first]))
                success = true;
            else
            {
                unsigned long error;
                if (file_open(task->compress_file.transfer.source_file, 1, &error))
                {
                    task->compress_file.transfer.flag_c = 1;
                    if (file_get_size(task->compress_file.transfer.source_file, &task->compress_file.source_size))
                    {
                        if (file_create(task->compress_file.transfer.destination_file))
                        {
                            if (file_open(task->compress_file.transfer.destination_file, 3, &error))
                            {
                                task->compress_file.transfer.flag_d = 1;
                                success = true;
                                task->compress_file.state = 3;
                            }
                            else
                                file_delete(task->compress_file.transfer.destination_file);
                        }
                    }
                }
            }
        }
    }
    else if (task->compress_file.state == 3)
    {
        if (!cancelled)
        {
            unsigned long position = task->compress_file.transfer.source_file->position;
            s_blf_header header;
            if (task->compress_file.source_size - position >= sizeof(s_blf_header))
            {
                if (file_read(task->compress_file.transfer.source_file, sizeof(header), true, &header))
                {
                    if (header.chunk_type == '_eof')
                    {
                        success = true;
                        task->compress_file.state = 5;
                    }
                    else
                    {
                        bool uncompressible = false;
                        long *signature = (long *)k_online_files_incompressible_blf_types;
                        unsigned long offset = 0;
                        do
                        {
                            if (*signature == header.chunk_type)
                            {
                                uncompressible = true;
                                break;
                            }
                            offset += 4;
                            signature++;
                        } while (offset < 16 && !uncompressible);
                        if (!uncompressible && header.chunk_size >= 0x1000)
                        {
                            s_online_file_buffered_writer *writer = task->compress_file.writer;
                            s_blf_chunk_compressed_data compressed_state3;
                            compressed_state3.initialize();
                            task->compress_file.compressed_position = writer->file->position + writer->buffer_position;
                            if (file_set_position(task->compress_file.transfer.source_file, position, true))
                            {
                                task->compress_file.chunk_position = position;
                                task->compress_file.chunk_size = header.chunk_size;
                                if (buffered_file_write(task->compress_file.writer, &compressed_state3, sizeof(compressed_state3)) &&
                                    ::agnostic_stream_start(true, task->compress_file.zlib_memory,
                                        task->compress_file.buffer, task->compress_file.buffer_size,
                                        task->compress_file.stream, _zlib_compression_level_fast))
                                {
                                    success = true;
                                    task->compress_file.state = 4;
                                }
                            }
                        }
                        else if (buffered_file_write(task->compress_file.writer, &header, sizeof(header)))
                        {
                            success = true;
                            long remaining = header.chunk_size - sizeof(header);
                            while (remaining > 0)
                            {
                                long count;
                                if ((unsigned long)remaining < 0x2000)
                                    count = remaining;
                                else
                                    count = 0x2000;
                                if (task->compress_file.source_size - task->compress_file.transfer.source_file->position < (unsigned long)count ||
                                    !file_read(task->compress_file.transfer.source_file, count, true, buffer) ||
                                    !buffered_file_write(task->compress_file.writer, buffer, count))
                                {
                                    success = false;
                                    break;
                                }
                                remaining -= count;
                            }
                        }
                    }
                }
            }
        }
        if (success)
            progress = (float)task->compress_file.transfer.source_file->position / (float)task->compress_file.source_size;
    }
    else if (task->compress_file.state == 4)
    {
        if (!cancelled)
        {
            bool stream_finished = false;
            if (online_guide_is_active())
                success = true;
            else
            {
                long bytes_consumed = 0;
                long bytes_written = 0;
                unsigned long position = task->compress_file.transfer.source_file->position;
                long remaining = task->compress_file.chunk_position + task->compress_file.chunk_size - position;
                long count = 0x2000;
                if ((unsigned long)remaining <= 0x2000)
                    count = remaining;
                success = true;
                if (count > 0 && !file_read(task->compress_file.transfer.source_file, count, false, input))
                    success = false;
                if (success &&
                    !::agnostic_stream_write(true, task->compress_file.zlib_memory,
                        task->compress_file.stream, input, count, &bytes_consumed, output,
                        0x2000, (long *)(&bytes_written), remaining == count, (bool *)(&stream_finished)))
                    success = false;
                if (success && bytes_consumed < count && !file_set_position(task->compress_file.transfer.source_file,
                    position + bytes_consumed, true))
                    success = false;
                if (success && bytes_written > 0 && !buffered_file_write(task->compress_file.writer, output, bytes_written))
                    success = false;
            }
            if (success)
                progress = (float)task->compress_file.transfer.source_file->position / (float)task->compress_file.source_size;
            if (!success || stream_finished)
            {
                online_files_compress_stream_end(task->compress_file.stream, task->compress_file.zlib_memory);
                if (success && stream_finished)
                {
                    s_blf_chunk_compressed_data compressed_state4 = {{'_cmp', 17, 1, 1}, 0, 0};
                    s_online_file_buffered_writer *writer = task->compress_file.writer;
                    unsigned long end_position = writer->file->position + writer->buffer_position;
                    success = false;
                    if (buffered_file_writer_set_position(task->compress_file.writer, task->compress_file.compressed_position))
                    {
                        compressed_state4.header.chunk_size = task->compress_file.stream->total_out + sizeof(compressed_state4);
                        compressed_state4.uncompressed_size = task->compress_file.chunk_size;
                        if (buffered_file_write(task->compress_file.writer, &compressed_state4, sizeof(compressed_state4)) &&
                            buffered_file_writer_set_position(task->compress_file.writer, end_position))
                        {
                            success = true;
                            task->compress_file.state = 3;
                        }
                    }
                }
            }
        }
    }
    else if (task->compress_file.state == 5 && !cancelled)
    {
        s_online_file_buffered_writer *writer = task->compress_file.writer;
        s_blf_chunk_end_of_file end_of_file;
        end_of_file.initialize();
        end_of_file.total_file_size = writer->file->position + writer->buffer_position;
        if (buffered_file_write(task->compress_file.writer, &end_of_file,
            sizeof(end_of_file)) && buffered_file_writer_flush(task->compress_file.writer))
        {
            success = true;
            finished = true;
        }
        success = file_close(task->compress_file.transfer.destination_file) & success;
        task->compress_file.transfer.flag_d = 0;
        if (success)
        {
            c_static_string<256> new_filename;
            new_filename.m_string[0] = 0;
            char name[256];
            file_reference_get_name(task->compress_file.transfer.destination_file, 4, name, sizeof(name));
            new_filename.print("%s.compressed", name);
            success = file_rename(task->compress_file.transfer.destination_file, new_filename.m_string) & success;
            progress = 1.0f;
        }
    }
    if (finished || !success)
    {
        online_files_cleanup_transfer_task(&task->compress_file.transfer);
        if (!success && !cancelled)
            file_delete(task->compress_file.transfer.file_reference);
        if (finished)
            progress = 1.0f;
    }
    c_static_string<17> filename;
    online_files_compress_get_base_filename(&task->compress_file.transfer, &filename);
    if (success)
        ((c_online_files_active_transfer_tracker *)task->compress_file.transfer.area)
            ->update_transfer_status(filename.m_string, _queued_work_item_status_state_compressing, progress);
    else
    {
        ((c_online_files_active_transfer_tracker *)task->compress_file.transfer.area)
            ->update_transfer_status(filename.m_string, _queued_work_item_status_state_upload_queued, 0.0f);
        *task->compress_file.transfer.error_forced = 1;
    }
    online_files_compress_set_truth(task->compress_file.transfer.succeeded, success && finished);
    return (e_async_completion)(finished || !success || cancelled);
}


static __forceinline c_http_client *online_files_upload_client(s_online_file_upload_task *upload)
{
    return (c_http_client *)upload->upload_buffer;
}
static __forceinline c_static_string<1024> *online_files_upload_headers(s_online_file_upload_task *upload)
{
    return &((c_http_post_stream *)upload->response_buffer)->get_headers();
}
static __forceinline const float &online_files_min(const float &a, const float &b)
{
    return a < b ? a : b;
}
static __forceinline const float &online_files_max(const float &a, const float &b)
{
    return a > b ? a : b;
}
static __forceinline const float &online_files_pin(const float &value, const float &lo, const float &hi)
{
    return online_files_min(online_files_max(value, lo), hi);
}
static __forceinline c_static_string<1024> *online_files_download_headers(s_online_file_download_task *download)
{
    return &((c_http_get_stream *)download->stream)->get_headers();
}


// @address 0x3b90b0 ?async_do_upload@c_online_file_manager@@CA?AW4e_async_completion@@PAUs_async_task@@@Z
e_async_completion c_online_file_manager::async_do_upload(s_async_task *task)
{
    s_online_file_upload_task *upload = &task->upload_file;
    bool success = false;
    bool finished = false;
    float progress = 0.0f;
    long cancelled_value = online_files_synchronized_get(upload->transfer.cancelled);
    bool cancelled = cancelled_value != 0;
    if (upload->flags == 0)
    {
        if (!cancelled)
        {
            bool lock_acquired = false;
            c_critical_section_scope lock(29, 1000, &lock_acquired);
            if (lock_acquired)
            {
                s_saved_game_item_metadata_packed metadata;
                if (read_queued_file(upload->transfer.file_reference, upload->work_item) &&
                    saved_game_read_metadata_from_file(upload->transfer.source_file, &metadata))
                {
                    c_static_string<256> url;
                    url.m_string[0] = 0;
                    unsigned long size = 0;
                    c_static_string<17> base_filename;
                    upload->transfer.get_base_filename(&base_filename);
                    if (file_get_size(upload->transfer.source_file, &size))
                    {
                        upload->file_size = size;
                        url.print("/gameapi/FilesNewUpload.ashx?userId=%016I64x&shareId=%016I64x&slot=%d"
                            "&uniqueId=%016I64x&fileType=%d&uncompressedSize=%d&compressedSize=%d",
                            upload->work_item->owner_xuid,
                            upload->work_item->requester_xuid, upload->work_item->slot, metadata.unique_id, metadata.file_type,
                            metadata.size_in_bytes, size);
                        c_network_http_request_description description(url.m_string, _http_request_type_get);
                        upload->request_id = c_network_http_request_queue::get(_network_http_request_queue_files)
                                                 ->start_request_into_buffer(
                                                     online_lsp_service_shr, &description, upload->url, upload->url_size);
                        upload->flags = 1;
                        success = true;
                    }
                    else
                        *upload->transfer.error_forced = 1;
                }
                else
                    *upload->transfer.error_forced = 1;
            }
            else
                success = true;
        }
    }
    else if (upload->flags == 1)
    {
        c_network_http_request_queue *queue = c_network_http_request_queue::get(_network_http_request_queue_files);
        if (!cancelled)
        {
            long position = 0;
            e_network_http_request_queue_failure_reason failure_reason = e_network_http_request_queue_failure_reason();
            long result = queue->is_fill_buffer_complete(upload->request_id, &position, 0,
                        &failure_reason);
            if (result == 0)
                success = true;
            else if (result == 1)
            {
                bool lock_acquired = false;
                c_critical_section_scope lock(29, 1000, &lock_acquired);
                if (lock_acquired)
                {
                    c_static_string<33> hash(upload->url);
                    hash.copy_to(upload->work_item->hash, 33);
                    if (write_queued_file(upload->transfer.file_reference, upload->work_item))
                    {
                        c_static_string<256> new_filename;
                        new_filename.m_string[0] = 0;
                        c_static_string<17> base_filename;
                        upload->transfer.get_base_filename(&base_filename);
                        new_filename.print("%s.uploading", base_filename.m_string);
                        if (file_rename(upload->transfer.source_file, new_filename.m_string))
                        {
                            create_file_reference_with_extension(upload->transfer.source_file, base_filename.m_string,
                                "uploading");
                            success = true;
                            upload->flags = 4;
                        }
                    }
                }
            }
            else
                *upload->transfer.error_forced = failure_reason == 2;
        }
        else
            queue->cancel_request(upload->request_id);
    }
    else if (upload->flags == 2)
    {
        if (!cancelled)
        {
            bool lock_acquired = false;
            c_critical_section_scope lock(29, 1000, &lock_acquired);
            if (lock_acquired)
            {
                if (read_queued_file(upload->transfer.file_reference, upload->work_item))
                {
                    unsigned long size = 0;
                    if (file_get_size(upload->transfer.source_file, &size))
                    {
                        c_static_string<256> url;
                        url.m_string[0] = 0;
                        upload->file_size = size;
                        url.print(
                            "/gameapi/FilesGetUploadProgress.ashx?userId=%016I64x&shareId=%016I64x&slot=%d&serverId=%s",
                            upload->work_item->owner_xuid, upload->work_item->requester_xuid,
                            upload->work_item->slot, upload->work_item->hash);
                        c_network_http_request_description description(url.m_string, _http_request_type_get);
                        upload->request_id = c_network_http_request_queue::get(_network_http_request_queue_files)
                                                 ->start_request_into_buffer(
                                                     online_lsp_service_shr, &description, upload->url, upload->url_size);
                        upload->flags = 3;
                        success = true;
                    }
                    else
                        *upload->transfer.error_forced = 1;
                }
                else
                    *upload->transfer.error_forced = 1;
            }
            else
                success = true;
        }
    }
    else if (upload->flags == 3)
    {
        c_network_http_request_queue *queue = c_network_http_request_queue::get(_network_http_request_queue_files);
        if (!cancelled)
        {
            long position = 0;
            e_network_http_request_queue_failure_reason failure_reason = e_network_http_request_queue_failure_reason();
            long result = queue->is_fill_buffer_complete(upload->request_id, &position, 0,
                        &failure_reason);
            if (result == 0)
                success = true;
            else if (result == 1)
            {
                // match: 0x3b90b0: the bound and the clamped length are separate locals.
                long maximum_length = upload->url_size - 1;
                long length = position <= maximum_length ? position : maximum_length;
                upload->url[length] = 0;
                upload->bytes_uploaded = atoi(upload->url);
                if ((unsigned long)upload->bytes_uploaded <= upload->file_size)
                {
                    success = true;
                    upload->flags = 4;
                }
            }
            else
                *upload->transfer.error_forced = failure_reason == 2;
        }
        else
            queue->cancel_request(upload->request_id);
    }
    else if (upload->flags == 4)
    {
        unsigned long error;
        if (!cancelled && file_open(upload->transfer.source_file, 1, &error))
        {
            c_static_string<256> filename;
            filename.m_string[0] = 0;
            c_http_post_source source;
            char name[256];
            upload->transfer.flag_c = 1;
            file_reference_get_name(upload->transfer.source_file, 4, name, 256);
            filename.print(k_custom_map_image_filename_format, name);
            source.set_source_as_file(upload->transfer.source_file);
            source.set_filename(filename.m_string);
            source.set_content_type("application/x-halo3sharedfile");
            source.set_start_position(upload->bytes_uploaded);
            ((c_http_post_stream *)upload->response_buffer)->set_source(&source);
            c_static_string<17> owner;
            c_static_string<17> requester;
            c_static_string<256> slot;
            slot.m_string[0] = 0;
            owner.print("%016I64x", upload->work_item->owner_xuid);
            requester.print("%016I64x", upload->work_item->requester_xuid);
            slot.print("%d", upload->work_item->slot);
            online_files_upload_headers(upload)->m_string[0] = 0;
            online_files_upload_headers(upload)->append_print( "%s: \"%s\"\r\n", "serverId",
                upload->work_item->hash);
            online_files_upload_headers(upload)->append_print( "%s: \"%s\"\r\n",
                "userID", owner.m_string);
            online_files_upload_headers(upload)->append_print( "%s: \"%s\"\r\n", "shareID",
                requester.m_string);
            online_files_upload_headers(upload)->append_print( "%s: \"%s\"\r\n", "slot",
                slot.m_string);
            if (upload->bytes_uploaded != 0)
            {
                c_static_string<48> start_position;
                start_position.print("%d", upload->bytes_uploaded);
                online_files_upload_headers(upload)->append_print( "%s: \"%s\"\r\n", "startPosition",
                    start_position.m_string);
            }
            success = true;
            upload->flags = 5;
        }
    }
    else if (upload->flags == 5)
    {
        if (!cancelled)
        {
            long address = 0;
            unsigned short port = 0;
            e_online_lsp_server_acquire_result acquire_result = g_online_lsp_manager.acquire_server(
                online_lsp_service_shr, &upload->request_cookie, &address, &port, "shared file upload");
            if (acquire_result == acquire_success)
            {
                if (((c_http_client *)upload->upload_buffer)
                        ->start((c_http_stream *)upload->response_buffer, address, port, "/gameapi/FilesUpload.ashx", true))
                {
                    success = true;
                    upload->flags = 6;
                }
            }
            else if (acquire_result == acquire_pending)
                success = true;
        }
    }
    else if (upload->flags == 6)
    {
        bool done = false;
        long status = 0;
        
        if (!cancelled && online_files_upload_client(upload)->do_work(&done, 0, 0, &status))
        {
            success = true;
            if (done)
            {
                g_online_lsp_manager.disconnect_from_server(upload->request_cookie, true);
                upload->request_cookie = -1;
                bool closed = (file_close(upload->transfer.source_file) & 1) != 0;
                upload->transfer.flag_c = 0;
                success = file_delete(upload->transfer.source_file) & closed;
                finished = true;
            }
            if (online_files_upload_client(upload)->online_files_is_connected())
                g_online_lsp_manager.server_connected(upload->request_cookie);
        }
        else
        {
            g_online_lsp_manager.disconnect_from_server(upload->request_cookie, false);
            upload->request_cookie = -1;
            online_files_upload_client(upload)->stop();
            file_close(upload->transfer.source_file);
            upload->transfer.flag_c = 0;
            if (status == 400)
                *upload->transfer.error_forced = 1;
        }
        if (success)
        {
            c_http_post_stream *stream = (c_http_post_stream *)upload->response_buffer;
            progress = PIN((float)(long)stream->get_source()->get_contents_file()->position / (float)upload->file_size, 0.0f, 1.0f);
        }
    }
    c_static_string<17> filename;
    online_files_compress_get_base_filename(&upload->transfer, &filename);
    if (success)
        ((c_online_files_active_transfer_tracker *)upload->transfer.area)
            ->update_transfer_status(filename.m_string, _queued_work_item_status_state_uploading, progress);
    else
        ((c_online_files_active_transfer_tracker *)upload->transfer.area)->update_transfer_status(filename.m_string,
            _queued_work_item_status_state_upload_queued, 0.0f);
    if (!success || finished)
        online_files_cleanup_transfer_task(&upload->transfer);
    if (finished || (!success && *upload->transfer.error_forced != 0))
        file_delete(upload->transfer.file_reference);
    online_files_compress_set_truth(upload->transfer.succeeded, success && finished);
    if (success && finished)
    {
        s_saved_game_item_metadata *metadata = &upload->work_item->description.metadata;
        long string_id;
        switch (s_saved_game_item_metadata::get_file_type_from_saved_game_type((e_saved_game_file_type)metadata->file_type))
        {
        case _metadata_file_type_film:
        case _metadata_file_type_clip:
            string_id = _user_interface_networking_string_film_clip_uploaded_to_file_share;
            break;
        case _metadata_file_type_game_variant:
            string_id = _user_interface_networking_string_game_variant_uploaded_to_file_share;
            break;
        case _metadata_file_type_map:
            string_id = _user_interface_networking_string_map_uploaded_to_file_share;
            break;
        case _metadata_file_type_screenshot:
            string_id = _user_interface_networking_string_screenshot_uploaded_to_file_share;
            break;
        default:
            string_id = _user_interface_networking_string_file_successfully_added_to_file_share;
            break;
        }
        user_interface_networking_post_error_message(metadata, string_id);
    }
    return (e_async_completion)(finished || !success || cancelled);
}

// @address 0x3b9b00 ?async_do_download@c_online_file_manager@@CA?AW4e_async_completion@@PAUs_async_task@@@Z
e_async_completion c_online_file_manager::async_do_download(s_async_task *task)
{
    s_online_file_download_task *download = &task->download_file;
    bool success = false;
    bool finished = false;
    float progress = 0.0f;
    long cancelled_value = online_files_synchronized_get(download->transfer.cancelled);
    bool cancelled = cancelled_value != 0;
    if (download->mode == 0 || download->mode == 1)
    {
        if (!cancelled)
        {
            bool lock_acquired = false;
            c_critical_section_scope lock(29, 1000, &lock_acquired);
            if (lock_acquired)
            {
                if (read_queued_file(download->transfer.file_reference, download->work_item))
                {
                    if (download->mode == 0)
                    {
                        if (file_create(download->transfer.destination_file))
                            success = true;
                    }
                    else if (file_get_size(download->transfer.destination_file, &download->flags))
                    {
                        download->flags &= ~0xFFF;
                        success = true;
                    }
                    if (success)
                    {
                        unsigned long error;
                        success = false;
                        if (file_open(download->transfer.destination_file, 3, &error))
                        {
                            download->transfer.flag_d = true;
                            if (download->flags == 0 || buffered_file_writer_set_position(download->writer, download->flags))
                            {
                                c_static_string<17> base_filename;
                                download->transfer.get_base_filename(&base_filename);
                                c_static_string<256> url;
                                url.m_string[0] = 0;
                                url.print("/gameapi/FilesStageForDownload.ashx?userId=%016I64x&shareId=%016I64x&slot=%d&serverId=%s"
                                    "&startPosition=%d&fromAutoQueue=%d&preview=%d",
                                    download->work_item->owner_xuid, download->work_item->requester_xuid,
                                    download->work_item->slot, download->work_item->hash, download->flags,
                                    download->work_item->user_initiated != 0, 0);
                                c_network_http_request_description description(url.m_string, _http_request_type_get);
                                download->request_id = c_network_http_request_queue::get(_network_http_request_queue_files)
                                                           ->start_request_into_buffer(online_lsp_service_shr, &description,
                                                               download->output, download->output_capacity);
                                download->mode = 2;
                                success = true;
                            }
                        }
                    }
                }
                else
                    *download->transfer.error_forced = true;
            }
            else
                success = true;
        }
    }
    else if (download->mode == 2)
    {
        c_network_http_request_queue *queue = c_network_http_request_queue::get(_network_http_request_queue_files);
        if (!cancelled)
        {
            long position = 0;
            e_network_http_request_queue_failure_reason failure_reason = e_network_http_request_queue_failure_reason();
            long result = queue->is_fill_buffer_complete(download->request_id, &position, 0,
                        &failure_reason);
            if (result == 0)
                success = true;
            else if (result == 1)
            {
                c_online_file_game_api_parser parser(download->output, download->output_capacity);
                parser.read_dword("Size: ", &download->compressed_size);
                parser.read_dword("FullSize: ", &download->full_size);
                parser.read_string("InitialUrl: ", download->input, 256);
                if (download->compressed_size > 0 && download->full_size != 0)
                {
                    if (download->flags > 0)
                    {
                        char *input = download->input;
                        strncpy(input, "/gameapi/FilesResumeDownload.ashx", 256);
                        input[255] = 0;
                    }
                    success = true;
                    download->mode = 4;
                }
            }
            else
                *download->transfer.error_forced = (failure_reason == 2);
        }
        else
            queue->cancel_request(download->request_id);
    }
    else if (download->mode == 4)
    {
        if (!cancelled)
        {
            long address = 0;
            unsigned short port = 0;
            e_online_lsp_server_acquire_result acquire_result = g_online_lsp_manager.acquire_server(
                online_lsp_service_shr, &download->request_cookie, &address, &port, "shared file download");
            if (acquire_result == acquire_success)
            {
                char url[256];
                c_static_string<17> owner_id;
                c_static_string<17> share_id;
                c_static_string<256> slot;
                slot.m_string[0] = 0;
                strncpy(url, download->input, 256);
                url[255] = 0;
                owner_id.print("%016I64x", download->work_item->owner_xuid);
                share_id.print("%016I64x", download->work_item->requester_xuid);
                slot.print("%d", download->work_item->slot);
                online_files_download_headers(download)->m_string[0] = 0;
                online_files_download_headers(download)->append_print("%s: \"%s\"\r\n",
                    "serverId", download->work_item->hash);
                online_files_download_headers(download)->append_print("%s: \"%s\"\r\n",
                    "userID", owner_id.m_string);
                online_files_download_headers(download)->append_print("%s: \"%s\"\r\n",
                    "shareID", share_id.m_string);
                online_files_download_headers(download)->append_print("%s: \"%s\"\r\n",
                    "slot", slot.m_string);
                if (download->flags != 0)
                {
                    c_static_string<48> start_position;
                    start_position.print("%d", download->flags);
                    online_files_download_headers(download)->append_print(
                        "%s: \"%s\"\r\n", "startPosition", start_position.m_string);
                }
                if (download->client->start((c_http_stream *)download->stream, address, port, url, true))
                {
                    success = true;
                    download->mode = 5;
                }
            }
            else if (acquire_result == acquire_pending)
                success = true;
        }
    }
    else if (download->mode == 5)
    {
        bool done = false;
        long status = 0;
        long count = 4096;
        char buffer[4096];
        if (!cancelled && download->client->do_work(&done, buffer, &count, &status))
        {
            if (count == 0 || buffered_file_write(download->writer, buffer, count))
            {
                long now = GetTickCount();
                if (now - (long)download->start_time >= 10000)
                {
                    if (content_file_share_enabled())
                        XFlushUtilityDrive();
                    download->start_time = now;
                }
                success = true;
            }
            if (download->client->online_files_is_connected())
                g_online_lsp_manager.server_connected(download->request_cookie);
            if (done)
            {
                g_online_lsp_manager.disconnect_from_server(download->request_cookie, true);
                download->request_cookie = -1;
                finished = true;
                success = false;
                if (buffered_file_writer_flush(download->writer) && file_close(download->transfer.destination_file))
                {
                    c_static_string<256> new_filename;
                    c_static_string<17> base_filename;
                    download->transfer.flag_d = false;
                    download->transfer.get_base_filename(&base_filename);
                    new_filename.print("%s.downloaded", base_filename.m_string);
                    if (file_rename(download->transfer.destination_file, new_filename.m_string))
                    {
                        progress = 1.0f;
                        success = true;
                    }
                }
            }
        }
        else
        {
            g_online_lsp_manager.disconnect_from_server(download->request_cookie, false);
            download->request_cookie = -1;
            download->client->stop();
            if (status == 400)
                *download->transfer.error_forced = true;
        }
        if (success)
        {
            s_online_file_buffered_writer *writer = download->writer;
            progress = online_files_pin((float)(writer->file->position + writer->buffer_position) / (float)download->compressed_size, 0.0f, 1.0f);
        }
    }
    c_static_string<17> filename;
    online_files_compress_get_base_filename(&download->transfer, &filename);
    if (success)
        ((c_online_files_active_transfer_tracker *)download->transfer.area)
            ->update_transfer_status(filename.m_string, _queued_work_item_status_state_downloading, progress);
    else
        ((c_online_files_active_transfer_tracker *)download->transfer.area)
            ->update_transfer_status(filename.m_string, _queued_work_item_status_state_download_queued, 0.0f);
    if (!success || finished)
        online_files_cleanup_transfer_task(&download->transfer);
    if (!success && *download->transfer.error_forced)
    {
        file_delete(download->transfer.file_reference);
        if (file_exists(download->transfer.destination_file))
            file_delete(download->transfer.destination_file);
    }
    online_files_compress_set_truth(download->transfer.succeeded, success && finished);
    return (e_async_completion)(finished || !success || cancelled);
}

/* Formats up to 16 characters and always terminates the buffer. */
template<long k_capacity>
const char *c_static_string<k_capacity>::print(const char *format, ...)
{
  char *buffer = m_string;
  va_list args;
  va_start(args, format);
  _vsnprintf(buffer, k_capacity - 1, format, args);
  buffer[k_capacity - 1] = 0;
  return buffer;
}
template const char *c_static_string<17>::print(const char *format, ...);

// 0x3bc8e0
// @address 0x3bc8e0 online_file_game_api_parser_read_wstring
extern "C" bool online_file_game_api_parser_read_wstring(
    c_online_file_game_api_parser *parser, const char *key, c_static_wchar_string<256> *message)
{
    bool result = false;
    wchar_t buffer[256];
    if (parser->read_string(key, buffer, 256))
    {
        message->set(buffer);
        result = true;
    }
    return result;
}

extern "C" unsigned char online_file_game_api_parser_advance_to_next_line(s_text_stream *stream)
{
  bool result = false;
  while (stream->next < stream->begin + stream->size) {
    unsigned char flag = result;
    if (flag) {
      char c = *stream->next;
      if (c != 13 && c != 10)
        return result;
    }
    if (!flag) {
      char c = *stream->next;
      if (c == 13 || c == 10)
        result = 1;
    }
    stream->next++;
  }
  return result;
}

extern "C" bool online_file_game_api_parser_skip_to_next_section(s_text_stream *stream, const char *prefix)
{
    bool result = false;
    c_static_string<1024> key("EndSlot");
    c_static_string<1024> line;
    line.set(stream->next);
    while (!line.starts_with(key.m_string))
    {
        if (!online_file_game_api_parser_advance_to_next_line(stream))
            break;
        line.set(stream->next);
    }
    if (line.starts_with(key.m_string))
    {
        result = true;
        online_file_game_api_parser_advance_to_next_line(stream);
    }
    return result;
}

bool c_online_file_game_api_parser::read_long(const char *prefix, long *value)
{
    bool result = false;
    c_static_string<1024> key(prefix);
    c_static_string<1024> line;
    line.set(next);
    while (!line.starts_with(key.m_string))
    {
        if (!online_file_game_api_parser_advance_to_next_line(this))
            break;
        line.set(next);
    }
    if (line.starts_with(key.m_string))
    {
        result = true;
        char *start = next;
        char *end = begin + size;
        char *limit;
        if (start + key.length() > end)
            limit = end;
        else
            limit = start + key.length();
        next = limit;
        *value = atoi(limit);
        online_file_game_api_parser_advance_to_next_line(this);
    }
    return result;
}

bool c_online_file_game_api_parser::read_dword(const char *prefix, unsigned long *value)
{
    bool result = false;
    c_static_string<1024> key(prefix);
    c_static_string<1024> line;
    line.set(next);
    while (!line.starts_with(key.m_string))
    {
        if (!online_file_game_api_parser_advance_to_next_line(this))
            break;
        line.set(next);
    }
    if (line.starts_with(key.m_string))
    {
        result = true;
        char *start = next;
        char *end = begin + size;
        char *limit;
        if (start + key.length() > end)
            limit = end;
        else
            limit = start + key.length();
        next = limit;
        *value = strtoul(limit, 0, 10);
        online_file_game_api_parser_advance_to_next_line(this);
    }
    return result;
}

bool c_online_file_game_api_parser::read_qword(const char *prefix, unsigned __int64 *value)
{
    bool result = false;
    c_static_string<1024> key(prefix);
    c_static_string<1024> line;
    line.set(next);
    while (!line.starts_with(key.m_string))
    {
        if (!online_file_game_api_parser_advance_to_next_line(this))
            break;
        line.set(next);
    }
    if (line.starts_with(key.m_string))
    {
        result = true;
        char *start = next;
        char *end = begin + size;
        char *limit;
        if (start + key.length() > end)
            limit = end;
        else
            limit = start + key.length();
        next = limit;
        *value = _atoi64(limit);
        online_file_game_api_parser_advance_to_next_line(this);
    }
    return result;
}

static __forceinline bool online_files_text_starts_with(const char *text, const char *prefix)
{
    return ((const c_static_string<1024> *)text)->starts_with(prefix);
}
static __forceinline void online_files_text_trim_end(char *text)
{
    ((c_static_string<1024> *)text)->trim_end();
}
bool c_online_file_game_api_parser::read_string(const char *prefix, wchar_t *value, long count)
{
    s_text_stream *stream = this;
    char line[1024];
    char prefix_copy[1024];
    char text[1024];
    bool result = false;
    strncpy(prefix_copy, prefix, 1024);
    prefix_copy[1023] = 0;
    line[0] = 0;
    strncpy(line, stream->next, 1024);
    line[1023] = 0;
    while (!online_files_text_starts_with(line, prefix_copy)) {
        if (!online_file_game_api_parser_advance_to_next_line(stream))
            break;
        strncpy(line, stream->next, 1024);
        line[1023] = 0;
    }
    if (online_files_text_starts_with(line, prefix_copy)) {
        char *end;
        result = true;
        char *start = stream->next;
        text[0] = 0;
        end = stream->begin + stream->size;
        char *limit;
        if (((const c_static_string<1024> *)prefix_copy)->length() + start > end)
            limit = end;
        else
            limit = ((const c_static_string<1024> *)prefix_copy)->length() + start;
        stream->next = limit;
        online_file_game_api_parser_advance_to_next_line(stream);
        unsigned int length = stream->next - limit + 1;
        if (length >= 1024)
            length = 1024;
        strncpy(text, limit, length);
        text[length - 1] = 0;
        online_files_text_trim_end(text);
        utf8_string_to_wchar_string((const utf8 *)text, value, count, 0);
    }
    return result;
}

bool c_online_file_game_api_parser::read_string(char const * prefix, char * out, long capacity)
{
    char line[1024];
    char prefix_copy[1024];
    char text[1024];
    bool result = false;
    strncpy(prefix_copy, prefix, 1024);
    prefix_copy[1023] = 0;
    line[0] = 0;
    strncpy(line, this->next, 1024);
    line[1023] = 0;
    while (!online_files_text_starts_with(line, prefix_copy)) {
        if (!online_file_game_api_parser_advance_to_next_line(this))
            break;
        strncpy(line, this->next, 1024);
        line[1023] = 0;
    }
    if (online_files_text_starts_with(line, prefix_copy)) {
        char *end;
        result = true;
        char *start = this->next;
        text[0] = 0;
        end = this->begin + this->size;
        char *limit;
        if (((const c_static_string<1024> *)prefix_copy)->length() + start > end)
            limit = end;
        else
            limit = ((const c_static_string<1024> *)prefix_copy)->length() + start;
        this->next = limit;
        online_file_game_api_parser_advance_to_next_line(this);
        unsigned int length = this->next - limit + 1;
        if (length >= 1024)
            length = 1024;
        strncpy(text, limit, length);
        text[length - 1] = 0;
        online_files_text_trim_end(text);
        unsigned int count = ((unsigned int)capacity);
        if (count > 1024)
            count = 1024;
        strncpy(out, text, count);
        out[count - 1] = 0;
    }
    return result;
}

template<>
void c_static_string<1024>::trim_end(void)
{
    long length = this->length();
    while (length < 1024)
    {
        char c = m_string[length - 1];
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
            break;
        m_string[length - 1] = 0;
        length--;
    }
}

template<>
bool c_static_string<1024>::starts_with(const char *prefix) const
{
    const char *string = m_string;
    return memcmp(prefix, string, string_length(prefix, 1024)) == 0;
}

static __forceinline void online_files_copy_guid(wchar_t *dest, const wchar_t *source, long size)
{
    for (long i = 0; i <= size - 1; source++, i++)
    {
        if ((dest[i] = *source) == 0)
            break;
    }
    dest[size - 1] = 0;
}
template<>
inline void c_static_wchar_string<33>::set(const wchar_t *string)
{ online_files_copy_guid(m_string, string, 33); }

template<long k_count>
static __forceinline bool online_files_parser_read_string(
    c_online_file_game_api_parser *parser, const char *key, c_static_wchar_string<k_count> *value)
{
    bool result = false;
    wchar_t buffer[k_count];
    if (parser->read_string(key, buffer, k_count))
    {
        value->set(buffer);
        result = true;
    }
    return result;
}


// @address 0x3bb960 ?process_request_result@c_online_files_enumerator@@AAA_NXZ
bool c_online_files_enumerator::process_request_result()
{
    c_online_file_game_api_parser parser(m_results, sizeof(m_results));
    long slot_count = 0;
    memset(m_slots, 0, sizeof(m_slots));

    bool success = true;
    unsigned __int64 quota_bytes;
    long quota_slots;
    long visible_slots;
    long subscription_hash;
    success &= parser.read_qword("QuotaBytes: ", &quota_bytes);
    success &= parser.read_long("QuotaSlots: ", &quota_slots);
    success &= parser.read_long("SlotCount: ", &slot_count);
    success &= parser.read_long("VisibleSlots: ", &visible_slots);
    success &= parser.read_long("SubscriptionHash: ", &subscription_hash);
    success &= online_file_game_api_parser_read_wstring(&parser, "Message: ", &m_message);

    for (long slot_number = 0; slot_number < slot_count; slot_number++)
    {
        long slot_index = -1;
        success &= parser.read_long("StartSlot: ", &slot_index);
        if (success && slot_index >= 0 && (unsigned long)slot_index < (sizeof(m_slots) / sizeof((m_slots)[0])))
        {
            c_static_wchar_string<33> guid;
            c_static_wchar_string<16> state;

            success &= online_files_parser_read_string(&parser, "  Guid: ", &guid);
            if (success)
                c_static_string<33>::copy_wchar_string_to_buffer(guid.get_string(), m_slots[slot_index].filename);

            success &= online_files_parser_read_string(&parser, "  State: ", &state);
            if (success)
            {
                if (state.is_equal(L"Partial"))
                {
                    m_slots[slot_index].state = _online_file_slot_state_partial;
                    success &= parser.read_qword("  SizeBytes: ",
                        &m_slots[slot_index].description.metadata.size_in_bytes);
                }
                else if (state.is_equal(L"Ready"))
                {
                    c_static_wchar_string<1024> file_type_string;
                    long map_id = -1;
                    long campaign_id = -1;
                    long game_engine_type = -1;
                    long campaign_difficulty = -1;
                    long author_xuid_is_online = 0;
                    wchar_t description[128];
                    wchar_t file_type[1024];

                    m_slots[slot_index].state = _online_file_slot_state_ready;
                    success &= parser.read_string("  Name: ", m_slots[slot_index].description.metadata.name,
                        (sizeof(m_slots[slot_index].description.metadata.name) / sizeof((m_slots[slot_index].description.metadata.name)[0])));
                    success &= parser.read_string("  Description: ", description,
                        (sizeof(description) / sizeof((description)[0])));
                    success &= parser.read_string("  Author: ", m_slots[slot_index].description.metadata.author,
                        (sizeof(m_slots[slot_index].description.metadata.author) / sizeof((m_slots[slot_index].description.metadata.author)[0])));
                    success &= parser.read_qword("  AuthorXuid: ", &m_slots[slot_index].description.metadata.author_id);
                    success &= parser.read_long("  AuthorXuidIsOnline: ", &author_xuid_is_online);
                    success &= parser.read_qword("  SizeBytes: ",
                        &m_slots[slot_index].description.metadata.size_in_bytes);
                    success &= parser.read_string("  FileType: ", file_type,
                        (sizeof(file_type) / sizeof((file_type)[0])));
                    success &= parser.read_qword("  SecondsPast19700101: ", &m_slots[slot_index].description.metadata.date);
                    success &= parser.read_long("  LengthSeconds: ", &m_slots[slot_index].description.metadata.length_seconds);
                    success &= parser.read_long("  CampaignID: ", &campaign_id);
                    success &= parser.read_long("  MapID: ", &map_id);
                    success &= parser.read_long("  GameEngineType: ", &game_engine_type);
                    success &= parser.read_long("  CampaignDifficulty: ", &campaign_difficulty);
                    success &= parser.read_qword("  GameID: ", &m_slots[slot_index].description.metadata.game_id);

                    wchar_string_to_utf8_string((const unsigned short *)description,
                        (unsigned char *)m_slots[slot_index].description.metadata.description,
                        (sizeof(m_slots[slot_index].description.metadata.description) / sizeof((m_slots[slot_index].description.metadata.description)[0])), 0);
                    m_slots[slot_index].description.metadata.author_is_online = author_xuid_is_online != 0;
                    m_slots[slot_index].description.metadata.campaign_id = campaign_id;
                    m_slots[slot_index].description.metadata.map_id = map_id;
                    m_slots[slot_index].description.metadata.game_engine_type = game_engine_type;
                    m_slots[slot_index].description.metadata.campaign_difficulty = campaign_difficulty;

                    file_type_string.set(file_type);
                    if (file_type_string.is_equal(L"GameVariantCtf"))
                        m_slots[slot_index].description.metadata.file_type = 1;
                    else if (file_type_string.is_equal(L"GameVariantSlayer"))
                        m_slots[slot_index].description.metadata.file_type = 2;
                    else if (file_type_string.is_equal(L"GameVariantOddball"))
                        m_slots[slot_index].description.metadata.file_type = 3;
                    else if (file_type_string.is_equal(L"GameVariantKing"))
                        m_slots[slot_index].description.metadata.file_type = 4;
                    else if (file_type_string.is_equal(L"GameVariantJuggernaut"))
                        m_slots[slot_index].description.metadata.file_type = 5;
                    else if (file_type_string.is_equal(L"GameVariantTerritories"))
                        m_slots[slot_index].description.metadata.file_type = 6;
                    else if (file_type_string.is_equal(L"GameVariantAssault"))
                        m_slots[slot_index].description.metadata.file_type = 7;
                    else if (file_type_string.is_equal(L"GameVariantInfection"))
                        m_slots[slot_index].description.metadata.file_type = 8;
                    else if (file_type_string.is_equal(L"GameVariantVip"))
                        m_slots[slot_index].description.metadata.file_type = 9;
                    else if (file_type_string.is_equal(L"MapVariant"))
                        m_slots[slot_index].description.metadata.file_type = 10;
                    else if (file_type_string.is_equal(L"Film"))
                        m_slots[slot_index].description.metadata.file_type = 11;
                    else if (file_type_string.is_equal(L"FilmClip"))
                        m_slots[slot_index].description.metadata.file_type = 12;
                    else if (file_type_string.is_equal(L"Screenshot"))
                        m_slots[slot_index].description.metadata.file_type = 13;
                    else
                        m_slots[slot_index].description.metadata.file_type = -1;
                }
                else if (state.is_equal(L"Invalid"))
                {
                    m_slots[slot_index].state = _online_file_slot_state_invalid;
                }

                success &= online_file_game_api_parser_skip_to_next_section(&parser, "EndSlot");
            }
        }
    }

    if (success)
    {
        m_quota_bytes = quota_bytes;
        m_quota_slots = quota_slots;
        m_visible_slots = visible_slots;
        m_subscription_hash = subscription_hash;
    }
    else
    {
        m_quota_bytes = 0;
        m_quota_slots = 0;
        m_visible_slots = 0;
        m_subscription_hash = 0;
    }

    return success;
}

// File-share transfer and enumerator state.


class c_gui_start_menu_hq_service_record_file_share;

// @address 0x4b0b68 gui_start_menu_hq_service_record_file_share_update_enumerator
extern "C" void gui_start_menu_hq_service_record_file_share_update_enumerator(c_gui_start_menu_hq_service_record_file_share *screen)

{
    s_file_share_screen *file_share_screen = (s_file_share_screen *)screen;
    ((c_online_files_enumerator *)&file_share_screen->m_transfer)->update();
    if (file_share_screen->m_transfer.started && file_share_screen->m_transfer.m_request_cookie == 0)
    {
        wchar_t *custom_message = file_share_screen->m_transfer.m_message.m_string;
        if ((unsigned char)gui_start_menu_hq_service_record_file_share_is_owner((void *)file_share_screen) != 0
            && file_share_screen->m_transfer.m_visible_slots > 0)
        {
            long saved_value = file_share_screen->m_transfer.m_subscription_hash;
            e_controller_index controller_index = ((c_gui_widget *)file_share_screen)->get_any_responding_controller();
            s_player_marketplace_user *user = &g_player_marketplace_users[controller_index];
            user->controller_index = controller_index;
            _ReadWriteBarrier();
            if (saved_value != user->catalogue_version)
                user->refresh_requested = true;
        }
        if (file_share_screen->m_flag_7b0c)
        {
            c_gui_widget *list = ((c_gui_widget *)file_share_screen)->get_child_widget(_gui_list_widget, 0x1010a);
            if (list)
                ((c_gui_screen_widget *)file_share_screen)
                    ->transfer_focus_to_list((c_gui_list_widget *)list, 0, false, false);
            file_share_screen->m_flag_7b0c = false;
        }
        if (custom_message)
        {
            wchar_t message[256];
            ((c_static_wchar_string<256> *)message)->set(custom_message);
            content_catalogue_trim_whitespace((unsigned short *)message);
            if (((const c_static_wchar_string<256> *)message)->length() > 0)
            {
                e_controller_index controller_index = ((c_gui_widget *)file_share_screen)->get_any_responding_controller();
                g_gui_error_manager.post_error_with_custom_message(0x20089, 0, custom_message, controller_index, false, true);
            }
        }
        file_share_screen->m_transfer.started = false;
    }
}

class c_http_stream;
struct s_netdebug_upload_task;

// @address 0x3a84e0 netdebug_upload_file
extern "C" int netdebug_upload_file(s_netdebug_upload_task *task)
{
    s_file_reference *reference;
    int success;
    unsigned long error;
    char filename[256];
    char failed_filename[256];
    success = 0;
    reference = &task->file;
    error = success;
    if (file_open(reference, 1, &error))
    {
        c_http_post_source source;
        file_reference_get_name(reference, 12, filename, sizeof(filename));
        c_static_string<256> path;
        path.print("%s%s%s", g_netdebug_globals.session_id,
            "#", filename);
        netdebug_build_upload_text(&g_netdebug_globals.post_stream, task);
        source.set_source_as_file(reference);
        source.set_filename(path.get_string());
        source.set_content_type("application/x-halo2upload");
        g_netdebug_globals.post_stream.set_source(&source);
        if ((unsigned char)network_debug_dump_handle_upload_task(
                &g_netdebug_globals.http_client, &g_netdebug_globals.post_stream, 0x708, task) != 0)
            success = 1;
        file_close(reference);
    }
    else
    {
        file_reference_get_name(reference, 12, (char *)failed_filename, 0x100);
    }
    return success;
}

c_online_file_manager::c_online_file_manager() :
    m_active(false),
    m_state(_transfer_state_none),
    m_refresh_queue(false),
    m_status_known(false),
    m_error_posted(false),
    m_upstream_quota(0),
    m_transfer_priority(0),
    m_next_refresh_time(0),
    m_http_client(),
    m_post_stream(),
    m_get_stream(),
    m_task_id(-1),
    m_task_done(0),
    m_task_succeeded(),
    m_task_cancelled(),
    m_upload_id(-1),
    m_upload_status(0),
    m_download_id(-1),
    m_download_status(0),
    m_enqueuer()
{
    m_signed_in_users_watcher.signin_state = 0;
    memset(m_signed_in_users_watcher.users, 0, sizeof(m_signed_in_users_watcher.users));
    m_active_transfer_tracker.m_refresh_count = 1;
    m_active_transfer_tracker.m_status_dirty = false;
    m_active_transfer_tracker.m_state = _tracker_state_none;
    m_active_transfer_tracker.m_task_id = -1;
    m_active_transfer_tracker.m_task_done = 0;
    m_active_transfer_tracker.m_enumeration_done = 0;
    m_active_transfer_tracker.m_signed_in_users_watcher.signin_state = 0;
    memset(m_active_transfer_tracker.m_signed_in_users_watcher.users, 0, sizeof(m_active_transfer_tracker.m_signed_in_users_watcher.users));
    m_writer.buffer = 0;
    m_writer.buffer_size = 0;
    m_writer.buffer_position = 0;
}

__forceinline void c_critical_section_scope::release()
{
    if (m_locked && m_critical_section != -1)
        online_files_critical_section_leave(m_critical_section);
    m_locked = false;
}
// @address 0x3b8260 ?async_do_enumerate_and_or_cleanup_files@c_online_file_manager@@CA?AW4e_async_completion@@PAUs_async_task@@@Z
e_async_completion c_online_file_manager::async_do_enumerate_and_or_cleanup_files(s_async_task *task)
{
    bool lock_acquired = false;
    c_critical_section_scope lock(29, 1000, &lock_acquired);
    if (lock_acquired)
    {
        s_online_file_enumerate_task *enumeration = &task->enumerate_files;
        s_file_reference directory;
        s_find_file_data find_data;
        s_file_reference reference;
        file_reference_create_from_path(&directory, "cache1:\\upload_queue", true);
        find_files_start(&find_data, 0, &directory, "*.*");
        while (find_files_next(&find_data, &reference, 0) && *enumeration->transfer_count < enumeration->maximum_transfer_count)
        {
            c_online_files_extension_string extension;
            char name[256];
            file_reference_get_name(&reference, 8, name);
            extension.set(name);
            if (extension.is_equal("compressing"))
                file_delete(&reference);
            else if (extension.is_equal("queued"))
            {
                bool found = false;
                char filename[17];
                if (get_work_item_status_from_queued_file(&reference,
                    &enumeration->transfers[*enumeration->transfer_count].status, filename, &found))
                {
                    if (found)
                    {
                        memcpy(enumeration->transfers[*enumeration->transfer_count].filename, filename, sizeof(filename));
                        *enumeration->upstream_quota = *enumeration->upstream_quota > enumeration->transfers[*enumeration->transfer_count].status.priority
                            ? *enumeration->upstream_quota
                            : enumeration->transfers[*enumeration->transfer_count].status.priority;
                        *enumeration->transfer_priority = *enumeration->transfer_priority <=
                            enumeration->transfers[*enumeration->transfer_count].status.priority
                            ? *enumeration->transfer_priority
                            : enumeration->transfers[*enumeration->transfer_count].status.priority;
                        ++*enumeration->transfer_count;
                    }
                }
                else
                    file_delete(&reference);
            }
        }
        find_files_end(&find_data);
        qsort(enumeration->transfers, *enumeration->transfer_count, sizeof(enumeration->transfers[0]), work_item_tracker_sort_proc);
        find_files_start(&find_data, 0, &directory, "*.*");
        while (find_files_next(&find_data, &reference, 0))
        {
            c_online_files_extension_string extension;
            char extension_name[256];
            file_reference_get_name(&reference, 8, extension_name);
            extension.set(extension_name);
            if (!extension.is_equal("queued"))
            {
                bool found = false;
                char name[256];
                file_reference_get_name(&reference, 4, name);
                c_static_string<256> filename(name);
                for (long index = 0; index < *enumeration->transfer_count; ++index)
                {
                    if (strncmp(filename.m_string, enumeration->transfers[index].filename, 256) == 0)
                    {
                        if (extension.is_equal("compressed"))
                            enumeration->transfers[index].status.state = _queued_work_item_status_state_compressed;
                        else if (extension.is_equal("uploading"))
                            enumeration->transfers[index].status.state = _queued_work_item_status_state_uploading;
                        else if (extension.is_equal("downloading"))
                            enumeration->transfers[index].status.state = _queued_work_item_status_state_downloading;
                        else if (extension.is_equal("downloaded"))
                            enumeration->transfers[index].status.state = _queued_work_item_status_state_downloaded;
                        found = true;
                        break;
                    }
                }
                if (!found)
                    file_delete(&reference);
            }
        }
        find_files_end(&find_data);
        online_files_synchronized_set(enumeration->succeeded, 1);
    }
    e_async_completion result = (e_async_completion)(lock_acquired != false);
    lock.release();
    return result;
}


static __forceinline long online_files_enqueue_peek(const volatile long *value)
{
    long result = *value;
    __lwsync();
    return result;
}
// @address 0x96650 ?update@c_online_files_enqueuer@@QAAXPA_N@Z
void c_online_files_enqueuer::update(bool *changed)
{
    long task_id = m_task_id;
    *changed = false;
    if (task_id != -1)
    {
        if (online_files_enqueue_peek(&m_done.value))
        {
            m_task_id = -1;
            volatile long *success_value = &m_success.value;
            long success = *success_value;
            *changed = true;
            __lwsync();
            if (!success)
            {
                if (m_current_item.type == 1)
                g_gui_error_manager.post_error_with_custom_message(0x20084, 0, 0, k_any_controller, false, true);
                else
                g_gui_error_manager.post_error_with_custom_message(0x20085, 0, 0, k_any_controller, false, true);
            }
        }
    }
    else
    {
        for (unsigned long index = 0; index < sizeof(m_items) / sizeof(m_items[0]); ++index)
        {
            if (m_items[index].type == _queued_work_item_type_none)
                continue;
            c_online_files_active_transfer_tracker *tracker = &c_online_file_manager::get()->m_active_transfer_tracker;
            if (tracker->m_transfer_count < 16)
            {
                s_queued_work_item_status status;
                s_async_task task;
                bool duplicate = false;
                for (long transfer = 0; transfer < tracker->m_transfer_count; ++transfer)
                {
                    if (tracker->get_transfer_status(transfer, &status))
                    {
                        const s_queued_work_item *item = &m_items[index];
                        if (status.type == item->type && status.owner_xuid == item->requester_xuid &&
                            status.slot == item->slot && status.description.owner_xuid == item->description.owner_xuid)
                        {
                            duplicate = true;
                            break;
                        }
                    }
                }
                if (!duplicate)
                {
                    c_static_string<256> filename;
                    memset(&task, 0, sizeof(task));
                    memcpy(&m_current_item, &m_items[index], sizeof(s_queued_work_item));
                    task.enqueue_transfer.item = &m_current_item;
                    filename.print("%s\\%04x%04x%04x%04x.queued", "cache1:\\upload_queue", rand(), rand(), rand(), rand());
                    strncpy(task.enqueue_transfer.filename, filename.m_string, 256);
                    task.enqueue_transfer.filename[255] = 0;
                    task.enqueue_transfer.success = &m_success.value;
                    online_files_synchronized_set(&m_success.value, 0);
                    m_task_id = ::async_task_add_ex(_async_priority_blocking_generic, &task, _async_category_online_files,
                        c_online_files_enqueuer::enqueue_callback(), &m_done, true);
                }
            }
            memmove(&m_items[index], &m_items[index + 1], sizeof(m_items) - 1 - index);
            memset(&m_items[7], 0, sizeof(s_queued_work_item));
            return;
        }
    }
}

// @address 0x3b7938 online_file_manager_delete_transfer
extern "C" void online_file_manager_delete_transfer(void *p0, const char *p1)
{
    c_online_file_manager *control = (c_online_file_manager *)p0;

    if (control->m_task_id != -1)
    {
        online_files_synchronized_set(&control->m_task_cancelled, 1);
        async_task_change_priority(control->m_task_id, _async_priority_blocking_generic);
        async_block_until_done(&control->m_task_done, simple_yield_function, 1, 0, 0, 0);
        control->m_task_id = -1;
    }

    s_async_task task;
    memset(&task, 0, sizeof(task));
    strncpy(task.delete_transfer_files.filename, p1, sizeof(task.delete_transfer_files.filename));
    control->m_download_id = ::async_task_add_ex(_async_priority_blocking_generic, &task, _async_category_online_files,
        online_files_async_delete_transfer_files, &control->m_download_status, true);
}

// @address 0x3b7850 ?bump_transfer_priority@c_online_file_manager@@QAAXPBD@Z
void c_online_file_manager::bump_transfer_priority(char const *p0)
{
    if (this->m_upload_id != -1)
    {
        async_block_until_done(&this->m_upload_status, simple_yield_function, 1, 0, 0, 0);
        this->m_upload_id = -1;
    }

    s_async_task task;
    memset(&task, 0, sizeof(task));
    strncpy(task.bump_transfer_priority.filename, p0, sizeof(task.bump_transfer_priority.filename));

    this->m_upstream_quota = this->m_upstream_quota + 1;
    task.bump_transfer_priority.priority = this->m_upstream_quota;

    this->m_upload_id = ::async_task_add_ex(_async_priority_blocking_generic, &task, _async_category_online_files,
        c_online_file_manager::async_do_bump_transfer_priority, &m_upload_status, true);
}


#pragma pack(push, 1)

#pragma pack(pop)
#pragma pack(push, 4)
#pragma pack(pop)
template<typename T> static __forceinline const T &online_files_decompression_min(const T &a, const T &b)
{
    return a < b ? a : b;
}
template<typename T> static __forceinline const T &online_files_decompression_max(const T &a, const T &b)
{
    return a > b ? a : b;
}
static __forceinline void online_files_decompression_copy_name(wchar_t *destination, const wchar_t *source, long length)
{
    unsigned __int64 *words = (unsigned __int64 *)destination;
    unsigned __int64 *end = words + 4;
    do
    {
        *words++ = 0;
    } while (words < end);
    for (long index = 0; index <= length; ++index)
    {
        if (!(destination[index] = *source))
            break;
        ++source;
    }
}
static __forceinline void online_files_decompression_stream_end(z_stream_s *stream, s_blam_zlib_memory *memory)
{
    z_inflateEnd(stream);
    memory->heap = 0;
}
static __forceinline bool online_files_decompression_keep_name(bool preserve_name, bool exists)
{
    bool result = true;
    result = result && preserve_name;
    return result || !exists;
}
// @address 0x3ba368 ?async_do_decompression@c_online_file_manager@@CA?AW4e_async_completion@@PAUs_async_task@@@Z
e_async_completion c_online_file_manager::async_do_decompression(s_async_task *task)
{
    bool success = false;
    bool finished = false;
    float progress = 0.0f;
    char buffer[0x2000];
    char input[0x2000];
    char output[0x2000];
    long cancelled_value = *task->decompress_file.transfer.cancelled;
    __lwsync();
    bool cancelled = cancelled_value != 0;
    if (task->decompress_file.state == 0)
    {
        if (!cancelled)
        {
            unsigned long error;
            bool lock_acquired = false;
            c_critical_section_scope lock(29, 1000, &lock_acquired);
            if (lock_acquired)
            {
                if (read_queued_file(task->decompress_file.transfer.file_reference, task->decompress_file.work_item))
                {
                    task->decompress_file.expected_size =
                        (unsigned long)task->decompress_file.work_item->description.metadata.size_in_bytes;
                    if (file_open(task->decompress_file.transfer.source_file, 1, &error))
                    {
                        task->decompress_file.transfer.flag_c = 1;
                        if (file_get_size(task->decompress_file.transfer.source_file, &task->decompress_file.source_size))
                        {
                            task->decompress_file.transfer.first = ::get_controller_index_from_xuid(
                                task->decompress_file.work_item->description.owner_xuid);
                            long controller_index = task->decompress_file.transfer.first;
                            if (task->decompress_file.transfer.first >= 0 && task->decompress_file.transfer.first < 4)
                            {
                                s_queued_work_item *item = task->decompress_file.work_item;
                                e_saved_game_file_type file_type = (e_saved_game_file_type)item->description.metadata.file_type;
                                const char *filename = saved_game_files_get_saved_game_file_name(file_type);
                                bool created = false;
                                e_game_content_type content_type = saved_game_file_type_to_game_content_type(file_type);
                                if ((unsigned long)content_type <= 14)
                                {
                                    const wchar_t *display_name = task->decompress_file.work_item->description.metadata.name;
                                    c_content_catalogue *catalogue = (c_content_catalogue *)&content_catalogues().catalogues[controller_index];
                                    bool exists = catalogue->try_and_get_personal_storage_item(display_name,
                                        (e_content_item_status)content_type) != -1;
                                    item = task->decompress_file.work_item;
                                    if (!online_files_decompression_keep_name(item->description.named != 0, exists))
                                        saved_game_files_find_unique_name(
                                            task->decompress_file.transfer.first, item->description.metadata.file_type,
                                                item->description.metadata.name, 16);
                                    created = saved_game_files_create_content_item_file(task->decompress_file.transfer.first, content_type,
                                        task->decompress_file.work_item->description.metadata.name, filename,
                                        task->decompress_file.transfer.destination_file, &task->decompress_file.transfer.second);
                                }
                                if (created && task->decompress_file.transfer.second != -1)
                                {
                                    task->decompress_file.state = 1;
                                    success = true;
                                }
                            }
                        }
                    }
                }
            }
            else
                success = true;
        }
    }
    else if (task->decompress_file.state == 1)
    {
        if (!cancelled)
        {
            c_content_catalogue *catalogue = &content_catalogues().catalogues[task->decompress_file.transfer.first];
            if (content_catalogue_has_pending_tasks(catalogue))
                success = true;
            else if (task->decompress_file.transfer.second != -1 &&
                ((s_content_item *)catalogue)->make_ready_for_access(task->decompress_file.transfer.second, false))
            {
                task->decompress_file.state = 2;
                success = true;
            }
        }
    }
    else if (task->decompress_file.state == 2)
    {
        if (!cancelled)
        {
            if (content_catalogue_has_pending_tasks(&content_catalogues().catalogues[task->decompress_file.transfer.first]))
                success = true;
            else
            {
                unsigned long error;
                if (file_create(task->decompress_file.transfer.destination_file) && file_open(task->decompress_file.transfer.destination_file, 2, &error))
                {
                    task->decompress_file.transfer.flag_d = 1;
                    task->decompress_file.state = 3;
                    success = true;
                }
            }
        }
    }
    else if (task->decompress_file.state == 3)
    {
        if (!cancelled)
        {
            s_blf_header header;
            if (task->decompress_file.source_size - task->decompress_file.transfer.source_file->position >= sizeof(header) &&
                file_read(task->decompress_file.transfer.source_file, sizeof(header), true, &header))
            {
                if (header.chunk_type == '_eof' && header.major_version == 1)
                {
                    task->decompress_file.state = 5;
                    success = true;
                }
                else if (header.chunk_type == 'chdr' && header.major_version == 9)
                {
                    s_blf_chunk_content_header content_header;
                    memset(&content_header, 0, sizeof(content_header));
                    memcpy(&content_header, &header, sizeof(header));
                    if ((unsigned long)header.chunk_size >= 264 &&
                        file_read(task->decompress_file.transfer.source_file, 252, true, &content_header.build_number))
                    {
                        const wchar_t *display_name = task->decompress_file.work_item->description.metadata.name;
                        c_content_catalogue *catalogue = &content_catalogues().catalogues[task->decompress_file.transfer.first];
                        if (wcsncmp(content_header.metadata.name, display_name, 16))
                        {
                            online_files_decompression_copy_name(content_header.metadata.name,
                                task->decompress_file.work_item->description.metadata.name, 16);
                            unsigned __int64 id;
                            XNetRandom((unsigned char *)&id, sizeof(id));
                            content_header.metadata.unique_id = id;
                        }
                        long item_index = content_catalogue_find_item_by_file_reference(catalogue,
                            task->decompress_file.transfer.destination_file);
                        if (item_index != -1)
                        {
                            content_catalogue_set_item_metadata((c_content_catalogue *)catalogue, item_index,
                                (const s_saved_game_item_metadata_packed *)&content_header.metadata);
                            if (file_write(task->decompress_file.transfer.destination_file, 264, &content_header))
                                success = true;
                        }
                    }
                }
                else
                {
                    bool compressed = header.chunk_type == '_cmp' && header.major_version == 1;
                    if (!compressed)
                    {
                        if (file_write(task->decompress_file.transfer.destination_file, sizeof(header), &header))
                        {
                            success = true;
                            long remaining = header.chunk_size - sizeof(header);
                            while (remaining > 0)
                            {
                                long count = (unsigned long)remaining < sizeof(buffer) ? remaining : sizeof(buffer);
                                if (task->decompress_file.source_size - task->decompress_file.transfer.source_file->position < (unsigned long)count ||
                                    !file_read(task->decompress_file.transfer.source_file, count, true,
                                        buffer) || !file_write(task->decompress_file.transfer.destination_file, count,
                                        buffer))
                                {
                                    success = false;
                                    break;
                                }
                                remaining -= count;
                            }
                        }
                    }
                    else
                    {
                        task->decompress_file.output_position = task->decompress_file.transfer.destination_file->position;
                        task->decompress_file.chunk_position = task->decompress_file.transfer.source_file->position +
                            sizeof(s_blf_chunk_compressed_data) - sizeof(header);
                        task->decompress_file.chunk_size = header.chunk_size - sizeof(s_blf_chunk_compressed_data);
                        if (file_set_position(task->decompress_file.transfer.source_file, task->decompress_file.chunk_position, true) &&
                            agnostic_stream_start(false, task->decompress_file.zlib_memory,
                                (char *)task->decompress_file.buffer, task->decompress_file.buffer_size,
                                task->decompress_file.stream, _zlib_compression_level_default))
                        {
                            task->decompress_file.state = 4;
                            success = true;
                        }
                    }
                }
            }
        }
        if (success)
            progress = online_files_decompression_min(
                online_files_decompression_max(
                    (float)task->decompress_file.transfer.destination_file->position / (float)task->decompress_file.expected_size, 0.0f),
                1.0f);
    }
    else if (task->decompress_file.state == 4)
    {
        bool stream_finished = false;
        if (!cancelled)
        {
            if (online_guide_is_active())
                success = true;
            else
            {
                long bytes_consumed = 0;
                long bytes_written = 0;
                unsigned long position = task->decompress_file.transfer.source_file->position;
                long remaining = task->decompress_file.chunk_position + task->decompress_file.chunk_size - position;
                long count = (unsigned long)remaining > sizeof(input) ? sizeof(input) : remaining;
                success = true;
                if (count > 0 && !file_read(task->decompress_file.transfer.source_file, count, false, input))
                    success = false;
                if (success &&
                    !agnostic_stream_write(false, task->decompress_file.zlib_memory,
                        task->decompress_file.stream, input, count, &bytes_consumed, output, sizeof(output),
                        &bytes_written, remaining == count, &stream_finished))
                    success = false;
                if (success && bytes_consumed < count && !file_set_position(task->decompress_file.transfer.source_file,
                    position + bytes_consumed, true))
                    success = false;
                if (success && bytes_written > 0 && !file_write(task->decompress_file.transfer.destination_file, bytes_written, output))
                    success = false;
            }
        }
        if (success)
            progress = online_files_decompression_min(
                online_files_decompression_max(
                    (float)task->decompress_file.transfer.destination_file->position / (float)task->decompress_file.expected_size, 0.0f),
                1.0f);
        if (!success || stream_finished)
        {
            online_files_decompression_stream_end(
                task->decompress_file.stream, task->decompress_file.zlib_memory);
            if (success)
                task->decompress_file.state = 3;
        }
    }
    else if (task->decompress_file.state == 5)
    {
        s_blf_chunk_end_of_file end_of_file;
        end_of_file.header.setup('_eof', sizeof(end_of_file), 1, 1);
        end_of_file.total_file_size = 0;
        end_of_file.authentication_type = 0;
        end_of_file.total_file_size = file_get_eof(task->decompress_file.transfer.destination_file);
        if (file_write(task->decompress_file.transfer.destination_file, sizeof(end_of_file), &end_of_file))
        {
            finished = true;
            success = true;
        }
    }
    if (!success || finished)
        online_files_cleanup_transfer_task(&task->decompress_file.transfer);
    if (!success || finished)
    {
        if (task->decompress_file.transfer.second != -1 && !success)
            content_catalogue_delete_item(&content_catalogues().catalogues[task->decompress_file.transfer.first],
                task->decompress_file.transfer.second, true);
        if (finished || (!success && !cancelled))
        {
            if (!file_delete(task->decompress_file.transfer.file_reference))
                success = false;
            if (!file_delete(task->decompress_file.transfer.source_file))
                success = false;
        }
        if (finished)
            progress = 1.0f;
    }
    c_static_string<17> filename;
    online_files_compress_get_base_filename(&task->decompress_file.transfer, &filename);
    if (success)
        ((c_online_files_active_transfer_tracker *)task->decompress_file.transfer.area)
            ->update_transfer_status(filename.m_string, _queued_work_item_status_state_decompressing, progress);
    else
    {
        ((c_online_files_active_transfer_tracker *)task->decompress_file.transfer.area)
            ->update_transfer_status(filename.m_string, _queued_work_item_status_state_download_queued, 0.0f);
        *task->decompress_file.transfer.error_forced = 1;
    }
    online_files_compress_set_truth(task->decompress_file.transfer.succeeded, success && finished);
    if (success && finished)
    {
        s_saved_game_item_metadata *metadata = &task->decompress_file.work_item->description.metadata;
        long string_id;
        switch (s_saved_game_item_metadata::get_file_type_from_saved_game_type((e_saved_game_file_type)metadata->file_type))
        {
        case _metadata_file_type_film:
        case _metadata_file_type_clip: string_id = _user_interface_networking_string_film_clip_available_in_theater; break;
        case _metadata_file_type_game_variant: string_id = _user_interface_networking_string_game_variant_available_in_custom_games; break;
        case _metadata_file_type_map: string_id = _user_interface_networking_string_map_available_in_custom_games_and_forge; break;
        case _metadata_file_type_screenshot: string_id = _user_interface_networking_string_screenshot_available_in_screenshots; break;
        default: string_id = _user_interface_networking_string_file_successfully_added_to_library; break;
        }
        user_interface_networking_post_error_message(metadata, string_id);
    }
    return (e_async_completion)(finished || !success || cancelled);
}

c_online_file_manager g_online_file_manager;

