#ifndef GAME_GAME_SAVE_H
#define GAME_GAME_SAVE_H

// Functions defined in tu/blamlibxbox/game_save.cpp.

void game_save_initialize(void);

bool game_safe_to_save_internal(bool unused);
void game_save_update(void);
void game_save_initialize_for_new_map(void);

void game_save_dispose(void);
void game_save_dispose_from_old_map(void);

#endif // GAME_GAME_SAVE_H
