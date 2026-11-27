#ifndef SAVED_GAMES_SAVED_FILM_RECORDING_TYPES_H
#define SAVED_GAMES_SAVED_FILM_RECORDING_TYPES_H
#include <stddef.h>
#include "camera/observer_globals_types.h"
#include "cseries/async_double_buffer.h"
#include "memory/static_arrays.h"
#include "game/game_options.h"
#include "networking/tools/blf_chunk_author_types.h"
#include "saved_games/saved_game_item_metadata_packed.h"
#include "saved_games/saved_film_runtime.h"
#include "tag_files/file_reference.h"
#include "camera/camera_mode_types.h"
#include "math/real_math_types.h"
enum e_map_memory_configuration;
class c_overlapped_task;
class c_saved_film;


struct s_blf_chunk_start_of_file
{
    s_blf_header header;
    short byte_order_marker;
    char name[32];
    char pad[2];

    __forceinline s_blf_chunk_start_of_file()
    {
        header.setup('_blf', sizeof(*this), 1, 2);
        byte_order_marker = -2;
        memset(name, 0, sizeof(name));
        for (long i = 0; i < 2; i++)
            pad[i] = 0;
    }
};


#pragma pack(push, 4)
struct s_blf_chunk_content_header
{
    s_blf_header header;
    unsigned short build_number;
    unsigned short scenario_slot;
    s_saved_game_item_metadata_packed metadata;

    s_blf_chunk_content_header();
};
#pragma pack(pop)

typedef char saved_film_content_header_size[(sizeof(s_blf_chunk_content_header) == 0x108) ? 1 : -1];
typedef char saved_film_content_header_metadata_offset[(offsetof(s_blf_chunk_content_header, metadata) == 0x10) ? 1 : -1];

extern "C" void saved_film_encode_screenshot_header(char *buffer, long, e_controller_index, long, const wchar_t *, const wchar_t *, __int64, long);

class s_blffile_saved_game_file
{
public:
    s_blffile_saved_game_file();
    s_saved_game_item_metadata_packed &get_metadata() { return m_content_header.metadata; }
    const s_saved_game_item_metadata_packed &get_metadata() const { return m_content_header.metadata; }
private:
    friend class c_saved_film;
    friend void saved_film_encode_screenshot_header(char *, long, e_controller_index, long, const wchar_t *, const wchar_t *, __int64, long);
    friend bool saved_film_new_recording(c_saved_film *, const char *, const s_game_options *, e_controller_index);
    friend bool saved_film_load_game_state(long index);
    s_blf_chunk_start_of_file m_start_of_file;
    s_blf_chunk_content_header m_content_header;
};




// blf chunks are packed to 4 (the game options inside sit at 0x2e4 in the saved-film globals, life_cycle_handler_in_game)
#pragma pack(push, 4)
struct s_blf_chunk_saved_film_header
{
    s_blf_header header;
    char pad_0c[4];
    char build_string[32];
    long network_version;
    long build_number;
    long compatible_version;
    long language;
    long minor_version;
    bool minor_version_tracked;
    char pad_45[0x50 - 0x45];
    unsigned long content_name_size;
    char content_name[60];
    bool authoritative;
    bool contains_gamestate;
    bool is_snippet;
    char pad_93[5];
    char session_id[128];
    game_options options;
    unsigned long system_seconds;
    long length_in_ticks;
    long snippet_start_tick;
    char pad_f934[0xfe6c - 0xf934];

    __forceinline s_blf_chunk_saved_film_header()
    {
        header.setup('flmh', sizeof(*this), 10, 1);
    }
};
#pragma pack(pop)

typedef char saved_film_header_size[(sizeof(s_blf_chunk_saved_film_header) == 0xfe6c) ? 1 : -1];
typedef char saved_film_header_build_string_offset[(offsetof(s_blf_chunk_saved_film_header, build_string) == 0x10) ? 1 : -1];
typedef char saved_film_header_content_name_size_offset[(offsetof(s_blf_chunk_saved_film_header, content_name_size) == 0x50) ? 1 : -1];
typedef char saved_film_header_session_id_offset[(offsetof(s_blf_chunk_saved_film_header, session_id) == 0x98) ? 1 : -1];
typedef char saved_film_header_options_offset[(offsetof(s_blf_chunk_saved_film_header, options) == 0x118) ? 1 : -1];
typedef char saved_film_header_system_seconds_offset[(offsetof(s_blf_chunk_saved_film_header, system_seconds) == 0xf928) ? 1 : -1];
typedef char saved_film_header_snippet_start_tick_offset[(offsetof(s_blf_chunk_saved_film_header, snippet_start_tick) == 0xf930) ? 1 : -1];

struct s_blf_chunk_saved_film_data
{
    s_blf_header header;

    __forceinline s_blf_chunk_saved_film_data()
    {
        header.setup('flmd', sizeof(*this), 1, 1);
    }
};

class s_blf_saved_film
{
public:
    s_blf_saved_film();
private:
    friend class c_saved_film;
    friend bool saved_film_new_recording(c_saved_film *, const char *, const s_game_options *, e_controller_index);
    friend bool saved_film_load_game_state(long index);
    s_blffile_saved_game_file m_start;
    s_blf_chunk_author m_author;
    s_blf_chunk_saved_film_header m_header;
    s_blf_chunk_saved_film_data m_data;
};


class c_saved_film;

extern "C" bool saved_film_encode_game_state(long zone_set_index, const void *data, long data_size);

extern "C" {
bool saved_film_try_and_get_update_header(long *output);
unsigned char saved_film_try_and_get_verified_game_state(c_saved_film *, long *, void *, long, long *, long *);
bool saved_film_update_storage_bytes(c_saved_film *film, const void *data, long size);
bool saved_film_initialize_from_file(c_saved_film *film, const char *file_path, long controller_index, bool ignore_version);
bool saved_film_initialize_from_name(long controller_index, const char *name);
bool saved_film_try_and_get_game_state(void);
void saved_film_initialize_history_storage(e_map_memory_configuration configuration);
void saved_film_update_recording(const simulation_update *update);
}

class c_saved_film
{
public:
    c_saved_film();
    void initialize();
    __forceinline void dispose();
    bool close();
    long get_mode() const { return m_mode; }
    const s_saved_game_item_metadata_packed *get_metadata() const
    {
        return &m_blf_saved_film.m_start.m_content_header.metadata;
    }
    const game_options *get_options() const { return &m_blf_saved_film.m_header.options; }
    __forceinline unsigned long get_content_name_size() const { return m_blf_saved_film.m_header.content_name_size; }
    __forceinline const unsigned char *get_content_name() const { return reinterpret_cast<const unsigned char *>(m_blf_saved_film.m_header.content_name); }
    __forceinline bool allocate_storage();
    __forceinline void write_game_state();
    bool header_valid(const s_blf_saved_film *film, bool ignore_version);
    bool read_simulation_update(const struct s_saved_film_update *update_header, simulation_update *update);
    long get_current_tick() const
    {
        long tick = 0;
        if (m_mode == 0)
            tick = m_blf_saved_film.m_header.length_in_ticks - m_storage_used;
        return tick;
    }
    long get_storage_used() const { return m_storage_used; }
    c_async_double_buffer *get_async_buffer() { return &m_async_buffer; }
    bool contains_gamestate() const { return m_blf_saved_film.m_header.contains_gamestate; }
    void set_snippet_start_tick(long start_tick)
    {
        m_blf_saved_film.m_header.is_snippet = true;
        m_blf_saved_film.m_header.snippet_start_tick = start_tick;
    }
private:
    friend bool saved_film_new_recording(c_saved_film *, const char *, const s_game_options *, e_controller_index);
    friend bool saved_film_load_game_state(long index);
    friend bool saved_film_try_and_get_update_header(long *output);
    friend unsigned char saved_film_try_and_get_verified_game_state(c_saved_film *, long *, void *, long, long *, long *);
    friend bool saved_film_update_storage_bytes(c_saved_film *film, const void *data, long size);
    friend bool saved_film_initialize_from_file(c_saved_film *film, const char *file_path, long controller_index, bool ignore_version);
    friend bool saved_film_initialize_from_name(long controller_index, const char *name);
#ifdef HALO3_64BIT
    friend bool saved_film_is_recording_to_file(const s_file_reference *file);
#endif
    friend bool saved_film_try_and_get_game_state(void);
    friend void saved_film_initialize_history_storage(e_map_memory_configuration configuration);
    friend bool saved_film_update_storage(c_saved_film *film, const simulation_update *update);
    friend void saved_film_update_recording(const simulation_update *update);
    friend void saved_film_close(void);
    friend bool saved_film_encode_game_state(long, const void *, long);
    bool write_game_state_internal(long zone_set_index, void *buffer, long size);
    bool write_finish();
    bool read_data(void *data, long size);
    long m_mode;
    s_blf_saved_film m_blf_saved_film;
    long m_storage_size;
    long m_storage_position;
    long m_storage_used;
    c_async_double_buffer m_async_buffer;
    s_file_reference m_file_reference;
    long m_cached_controller_index;
    bool m_error_occurred;
    bool m_fatal_error_occurred;
};

struct s_saved_film_snippet_player_state
{
    long absolute_index;
    bool valid;
};
struct s_saved_film_snippet_camera
{
    union {
        s_observer_result camera_result;
        struct {
            real_point3d position;
            s_location location;
            real_vector3d velocity, angular_velocity, forward, up;
            float horizontal_field_of_view;
            s_observer_depth_of_field depth_of_field;
            float aspect_ratio;
            s_observer_view_offset view_offset;
            float direction_error, vertical_field_of_view, field_of_view_scale;
        };
    };
    e_camera_mode mode;
    long camera_index;
    s_saved_film_snippet_player_state player;
};

struct s_saved_film_recording_globals
{
    __forceinline s_saved_film_recording_globals() : async_buffer(2) {}
    ~s_saved_film_recording_globals() {}

    c_async_double_buffer async_buffer;
    c_saved_film film;
    union { char buffer[0x200]; s_saved_film_snippet_camera camera_states[4]; };
    long first_tick;
    long last_tick;
    c_static_wchar_string<16> film_name;
    c_static_wchar_string<128> film_description;
    c_overlapped_task *overlapped_task;
    long slot_index_a;
    long slot_index_b;
    long slot_index_c;
    c_synchronized_long completion_a;
    c_synchronized_long completion_b;
    unsigned char pending_screen_update;
    long state;
};
typedef char saved_film_snippet_camera_result_offset[(offsetof(s_saved_film_snippet_camera, camera_result) == 0) ? 1 : -1];
typedef char saved_film_snippet_camera_mode_offset[(offsetof(s_saved_film_snippet_camera, mode) == 0x70) ? 1 : -1];
typedef char saved_film_snippet_camera_player_offset[(offsetof(s_saved_film_snippet_camera, player) == 0x78) ? 1 : -1];
typedef char saved_film_recording_camera_size[(sizeof(s_saved_film_snippet_camera) == 0x80) ? 1 : -1];
typedef char saved_film_recording_first_tick_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_film_recording_globals, first_tick) == 0x10438))) ? 1 : -1];
typedef char saved_film_recording_camera_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_film_recording_globals, camera_states) == 0x10238))) ? 1 : -1];
typedef char saved_film_recording_state_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_film_recording_globals, state) == 0x1057c))) ? 1 : -1];
extern "C" s_saved_film_recording_globals g_saved_film_recording_globals;
#endif
