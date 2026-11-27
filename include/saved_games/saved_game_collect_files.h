#ifndef SAVED_GAMES_SAVED_GAME_COLLECT_FILES_H
#define SAVED_GAMES_SAVED_GAME_COLLECT_FILES_H

#include "tag_files/files_windows_types.h"
#include "multithreading/synchronized_value.h"
#include <stddef.h>
#include "tag_files/file_reference.h"
#include "tag_files/file_last_modification_date.h"
#include "saved_games/saved_game_item_metadata_packed.h"

struct s_saved_game_queue_item
{
    s_file_reference file;
    long valid;
    s_saved_game_item_metadata_packed metadata;
    s_file_last_modification_date modification_date;
};

typedef char saved_game_queue_metadata_offset[(sizeof(void *) != 4 || ((offsetof(s_saved_game_queue_item, metadata) == 0x118))) ? 1 : -1];

typedef char saved_game_queue_item_size[(sizeof(void *) != 4 || ((sizeof(s_saved_game_queue_item) == 0x218))) ? 1 : -1];

struct s_saved_game_queue
{
    s_saved_game_queue() : state(0), write_count(0), read_count(0), done(1), cancel(0) {}
    s_saved_game_queue(long initial_category, long initial_thread_id) :
        category(initial_category), state(0), write_count(0), read_count(0),
        thread_id(initial_thread_id), done(1), cancel(0) {}

    long category;
    c_synchronized_long state;
    s_saved_game_queue_item items[26];
    c_synchronized_long write_count;
    c_synchronized_long read_count;
    s_find_file_data find_file_data;
    long thread_id;
    c_synchronized_long done;
    c_synchronized_long cancel;

    __forceinline bool has_items() const { return write_count.peek() > read_count.peek(); }
};

extern "C" void saved_game_queue_start_enumeration(s_saved_game_queue *queue, long category);
extern "C" unsigned char saved_game_queue_try_and_get_next(s_saved_game_queue *queue, s_saved_game_queue_item *item);

// match: 0x228998: write_map_autosave keeps count in r27 across the call; without it count spills to 80(r1), reloads into r11, frame saves r27.
extern "C" { __declspec(noalias) bool saved_game_collect_files( long category,
    s_saved_game_queue_item *items, long *file_sizes, long maximum_count, long *count, long *total_size); }

#endif // SAVED_GAMES_SAVED_GAME_COLLECT_FILES_H
