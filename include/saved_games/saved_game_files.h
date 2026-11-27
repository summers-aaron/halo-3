#ifndef SAVED_GAMES_SAVED_GAME_FILES_H
#define SAVED_GAMES_SAVED_GAME_FILES_H

#include "cseries/async_types.h"
#include "game/game_types.h"
#include "saved_games/saved_game_file_types.h"

// Functions defined in tu/blamlibxbox/saved_game_files.cpp.

struct game_state_header;
struct s_file_reference;
struct s_saved_game_item_metadata;
struct s_saved_game_item_metadata_packed;

bool saved_game_read_metadata_from_file(s_file_reference *file, s_saved_game_item_metadata_packed *out_metadata);
void saved_game_files_write_saved_game_state(const game_state_header *header, long header_size, const void *buffer, long buffer_size);

class c_content_catalogue;
class c_map_variant;
struct s_buffered_file_writer;
struct s_player_marketplace_user;

extern "C" void game_state_handle_after_load(long arg);
extern "C" void saved_game_files_delete_personal_storage(long controller_index);
extern "C" bool saved_game_write_metadata_to_open_file_internal(s_buffered_file_writer *writer);
extern "C" void map_variant_set_metadata(c_map_variant *variant, const s_saved_game_item_metadata_packed *metadata);
bool content_catalogue_set_item_metadata(const c_content_catalogue *catalogue, long content_item_index, const s_saved_game_item_metadata_packed *metadata);
extern "C" bool content_catalogue_get_item_file_path(const c_content_catalogue *catalogue, long content_item_index, char *path, long path_size);
extern "C" bool marketplace_consume_assets(s_player_marketplace_user *user, unsigned __int64 offer_id);
extern "C" void saved_game_files_find_unique_name(long controller_index, long file_type, wchar_t *name, unsigned long name_size);
extern "C" void saved_game_files_write_map_autosave(bool);
extern "C" bool saved_game_metadata_is_valid(const s_saved_game_item_metadata_packed *metadata);

class c_bitstream;
struct s_saved_game_files_context;

extern "C" bool saved_game_files_find_latest_film_path(char *path);
extern "C" void saved_game_files_initialize(void);
extern "C" void saved_game_files_reset(void);
extern "C" void saved_game_files_reset_film_state(void);
extern "C" void saved_game_files_update(void);
extern "C" void saved_game_files_delete_content_item(long controller_index, long content_item_index);
extern "C" int saved_game_write_metadata_to_file(s_file_reference *reference, const s_saved_game_item_metadata_packed *metadata, int file_type);
extern "C" bool saved_game_files_get_film_description(long controller_index, unsigned long type, const wchar_t *first_string, const wchar_t *second_string,
    long campaign_difficulty, wchar_t *output, long output_capacity);
extern "C" bool saved_game_files_try_and_get_metadata_property(const s_saved_game_item_metadata *metadata, long property, long *out_value);
extern "C" long saved_game_files_compute_film_type(const s_saved_game_item_metadata_packed *metadata);
extern "C" void saved_game_files_encode_metadata(const s_saved_game_item_metadata_packed *metadata, c_bitstream *stream);
extern "C" void saved_game_files_decode_metadata(s_saved_game_item_metadata_packed *metadata, c_bitstream *stream);
extern "C" long saved_game_files_set_async_result(unsigned char success, long unused, volatile long *result, volatile long *complete);
bool saved_game_read_metadata_from_open_file_internal(s_file_reference *reference, s_saved_game_item_metadata_packed *metadata);
extern "C" int saved_game_write_metadata_to_open_file(s_file_reference *reference, const s_saved_game_item_metadata_packed *metadata, int file_type);
extern "C" void saved_game_files_set_text_parse_context(const s_saved_game_files_context *context);
extern "C" void saved_game_files_reset_text_parse_context(void);
extern "C" bool saved_game_files_parse_first_string(void *context, wchar_t *output, long capacity);
extern "C" bool saved_game_files_parse_second_string(void *context, wchar_t *output, long capacity);
extern "C" bool saved_game_files_parse_third_string(void *context, wchar_t *output, long capacity);
extern "C" bool saved_game_files_parse_difficulty_string(void *context, wchar_t *output, long capacity);
extern "C" bool saved_game_files_has_unique_id(long controller_index, unsigned __int64 unique_id);
extern "C" int saved_game_files_update_variant_metadata(s_file_reference *file, const s_saved_game_item_metadata_packed *metadata, int file_type);

class c_synchronized_long;

extern "C" long saved_game_files_delete_content_item_async(long catalogue_index, long content_item_index, c_synchronized_long *completion,
    c_synchronized_long *done);
extern "C" long saved_game_files_copy_screenshot(long catalogue_index, long content_type, const wchar_t *display_name, const char *source_file_name,
    c_synchronized_long *done, c_synchronized_long *completion, long *out_content_item_index);

struct c_controller_interface;
struct s_async_task;
struct s_controller_update;

extern "C" e_async_completion saved_game_files_delete_content_item_callback(s_async_task *task);
extern "C" e_async_completion saved_game_files_update_metadata_callback(s_async_task *task);
extern "C" e_async_completion saved_game_files_create_content_item_callback(s_async_task *task);
int saved_game_files_update_controller(s_controller_update *update, c_controller_interface *controller, int state);
e_async_completion saved_game_files_enumerate_from_content_catalogue_callback(s_async_task *task);
const char *saved_game_filename_extension_by_game_file_type(e_saved_game_file_type file_type, bool without_dot);
e_saved_game_file_type game_engine_index_to_saved_game_file_type(e_game_engine_type engine_type);
extern "C" void saved_game_files_get_file_type_display_name(e_saved_game_file_type file_type, wchar_t *output, long capacity);
char const * saved_game_files_get_saved_game_file_name(enum e_saved_game_file_type index);

enum e_controller_index;

long saved_game_files_get_personal_storage_content_item(e_controller_index controller_index, bool create);

template<long> class c_static_wchar_string;

bool saved_game_files_create_saved_game_file_reference(enum e_controller_index, wchar_t const *, char const *, struct s_file_reference *, bool, bool);
extern "C" bool saved_game_files_get_game_state_options(e_controller_index controller_index, void *buffer, long buffer_size);
bool saved_game_files_get_saved_game_state_header(e_controller_index controller_index, game_state_header *header, long header_size);
extern "C" bool saved_game_files_get_content_item_file_path(long catalogue_index, long content_item_index, char *path, long max_path);
extern "C" bool saved_game_files_try_and_get_metadata_string(unsigned __int64 unique_id, const wchar_t *name, const wchar_t *description, const char *author,
    long file_type, bool author_is_online, unsigned __int64 author_id, unsigned __int64 size_in_bytes,
    unsigned __int64 date, long length_seconds, long campaign_id, long map_id, long game_engine_type,
    long campaign_difficulty, short campaign_insertion_point, unsigned __int64 game_id,
    long string_id, c_static_wchar_string<1024> *string);

void saved_game_files_dispose();

#endif // SAVED_GAMES_SAVED_GAME_FILES_H
