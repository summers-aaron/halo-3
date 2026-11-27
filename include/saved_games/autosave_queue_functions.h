#ifndef SAVED_GAMES_AUTOSAVE_QUEUE_FUNCTIONS_H
#define SAVED_GAMES_AUTOSAVE_QUEUE_FUNCTIONS_H

#include "shell/shell_types.h"

struct s_autosave_queue;
long autosave_queue_save_current_game_variant_to_queue(s_autosave_queue *queue, e_controller_index controller_index);

#endif // SAVED_GAMES_AUTOSAVE_QUEUE_FUNCTIONS_H
