#ifndef SAVED_GAMES_AUTOSAVE_QUEUE_H
#define SAVED_GAMES_AUTOSAVE_QUEUE_H

#include "cseries/async_types.h"
#include "shell/shell_types.h"
#include "saved_games/autosave_queue_functions.h"

// Functions defined in tu/blamlibxbox/autosave_queue.cpp.

struct s_autosave_queue;

extern "C" long g_saved_game_category_size_limits[];

extern "C" void levels_update_enumeration(void);
extern "C" bool autosave_queue_delete_item(s_autosave_queue *queue, long item_index);
extern "C" bool autosave_queue_set_item_text(s_autosave_queue *queue, long item_index, const wchar_t *name, const wchar_t *description);
extern "C" void autosave_queue_update(s_autosave_queue *queue);

struct s_autosave_queue_enumeration_task;
struct s_file_reference;

#if defined(_XBOX) && !defined(__clang__)
// The Xbox symbol is mangled with a bool return; elsewhere this returns e_async_completion.
bool saved_game_files_enumerate_from_autosave_queue_find_next_file(s_autosave_queue_enumeration_task *task);
#else
e_async_completion saved_game_files_enumerate_from_autosave_queue_find_next_file(s_autosave_queue_enumeration_task *task);
#endif
extern "C" bool saved_game_is_file_in_category(long category, const s_file_reference *reference);
extern "C" void autosave_queue_populate_from_catalogue(s_autosave_queue *queue);

extern "C" void saved_game_cleanup_storage();

#endif // SAVED_GAMES_AUTOSAVE_QUEUE_H
