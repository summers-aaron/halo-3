#ifndef INCLUDE_SAVED_GAMES_SAVED_GAME_FILES_OPERATIONS_H
#define INCLUDE_SAVED_GAMES_SAVED_GAME_FILES_OPERATIONS_H

#include "cseries/async_types.h"
#include "saved_games/game_content_types.h"
#include "saved_games/saved_game_file_category_types.h"
class c_synchronized_long;
struct s_file_reference;
#include "saved_games/saved_game_item_metadata.h"
struct s_saved_game_item_metadata_packed;

struct s_saved_game_file_copy_task
{
    long state;
    long controller_index;
    s_file_reference *source;
    s_file_reference *destination;
    s_saved_game_item_metadata_packed *metadata;
    long content_item_index;
    unsigned long file_size;
    c_synchronized_long *completion;
};

typedef char saved_game_file_copy_task_size[(sizeof(void *) != 4 || ((sizeof(s_saved_game_file_copy_task) == 0x20))) ? 1 : -1];

extern "C" bool saved_game_files_get_film_name(long controller_index, unsigned long type, const wchar_t *first_string, const wchar_t *second_string,
    long campaign_difficulty, long unused, wchar_t *output);
extern "C" long saved_game_files_read(long catalogue_index, long content_item_index, void *buffer, unsigned long size,
    c_synchronized_long *completion, c_synchronized_long *done);
extern "C" long saved_game_files_write_content_item(long catalogue_index, long content_item_index, long file_type, const void *buffer,
    unsigned long size, c_synchronized_long *completion, void *done);
extern "C" bool saved_game_files_create_content_item_file(long controller_index, e_game_content_type content_type,
    const wchar_t *display_name, const char *file_name, s_file_reference *reference, long *out_content_item_index);
extern "C" void saved_game_files_initialize_metadata(s_saved_game_item_metadata_record *out, unsigned int index, int value, const wchar_t *name,
    const wchar_t *description, unsigned __int64 identifier, unsigned __int64 size_in_bytes);
#include "saved_games/saved_game_files_metadata.h"
extern "C" e_async_completion saved_game_files_copy_file_callback(s_saved_game_file_copy_task *task);


extern "C" long saved_game_files_create_content_item_async(int controller_index, const unsigned short *path, int file_type,
    c_synchronized_long *completion, c_synchronized_long *done, c_synchronized_long *status);

#endif // INCLUDE_SAVED_GAMES_SAVED_GAME_FILES_OPERATIONS_H
