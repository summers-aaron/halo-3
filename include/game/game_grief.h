#ifndef GAME_GAME_GRIEF_H
#define GAME_GAME_GRIEF_H

extern "C" bool game_grief_can_boot_betrayer(long player_index);

void game_grief_record_betrayal(long player_index);

void game_grief_initialize(void);
void game_grief_dispose(void);
void game_grief_reset_betrayals(void);

#endif // GAME_GAME_GRIEF_H
