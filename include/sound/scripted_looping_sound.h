#ifndef SOUND_SCRIPTED_LOOPING_SOUND_H
#define SOUND_SCRIPTED_LOOPING_SOUND_H

enum e_looping_sound_fadeout_type {
    _looping_sound_fadeout_type_default,
    _looping_sound_fadeout_type_unknown1,
    _looping_sound_fadeout_type_immediate
};
void scripted_looping_sound_stop_internal(long definition_index, e_looping_sound_fadeout_type fadeout_type);

#endif // SOUND_SCRIPTED_LOOPING_SOUND_H
