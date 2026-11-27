#ifndef GAME_PLAYER_TRAINING_EXTERNS_H
#define GAME_PLAYER_TRAINING_EXTERNS_H
#include "interface/chud/chud_manager.h"
#include "cutscene/cinematic_globals_types.h"
#include "game/player_training_tls.h"

#include "math/real_math_types.h"
#include "game/player_mapping_globals.h"
#include "objects/object_header_data.h"
#include "game/player_data.h"
enum e_training_data_save;
struct s_player_training_user;
struct s_player_training_entry_data;
struct s_player_training_state;

bool increment_training_completion(long, struct s_player_training_user *, struct s_player_training_entry_data const *, struct s_player_training_state *);
void player_training_save_changes_to_profiles(e_training_data_save training_data_save);

struct s_data_array;
#include "game/game_time_types.h"
#include "game/game_globals_storage.h"
struct s_cinematic_globals_definition;
struct s_user_globals;

#endif // GAME_PLAYER_TRAINING_EXTERNS_H
