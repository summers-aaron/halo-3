#ifndef SOUND_GAME_SOUND_DETERMINISTIC_PERMUTATION_H
#define SOUND_GAME_SOUND_DETERMINISTIC_PERMUTATION_H

struct s_game_sound_deterministic_permutation
{
    char pitch_range;
    char permutation;
};
typedef char deterministic_permutation_size[(sizeof(s_game_sound_deterministic_permutation) == 2) ? 1 : -1];

#endif
