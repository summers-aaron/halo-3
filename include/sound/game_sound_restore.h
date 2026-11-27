#ifndef SOUND_GAME_SOUND_RESTORE_H
#define SOUND_GAME_SOUND_RESTORE_H

struct game_sound_restore_iterator
{
    long index;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
};
struct game_sound_restore_state
{
    long definition_index;
    unsigned long mode;
    float volume;
    long sound_identifier;
    bool alternate;
};

extern "C" bool looping_sound_iterate_scripted(game_sound_restore_iterator *, game_sound_restore_state *);

#endif
