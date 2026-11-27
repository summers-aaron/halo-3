#ifndef SOUND_GAME_SOUND_IMPULSE_H
#define SOUND_GAME_SOUND_IMPULSE_H

#include "sound/game_sound_deterministic_permutation.h"
#include "math/real_math_types.h"

struct s_sound_location;
struct s_game_sound_impulse_internal_parameters;

class c_game_sound_impulse_parameters {
public:
    long sound_impulse_index;
    long definition_index;
    float scale;
    const s_game_sound_deterministic_permutation *deterministic;
    __forceinline c_game_sound_impulse_parameters(long index, float value) {
        sound_impulse_index = -1;
        definition_index = index;
        scale = value;
        deterministic = 0;
    }
    __forceinline c_game_sound_impulse_parameters &set_deterministic_permutation(const s_game_sound_deterministic_permutation *value) {
        deterministic = value;
        return *this;
    }
    __forceinline c_game_sound_impulse_parameters &set_playback_label(long label) {
        sound_impulse_index = label;
        return *this;
    }
    long submit_attached_sound(long object_index, short node_index, const real_point3d *point, const real_vector3d *forward);
    long submit_attached_sound_from_marker(long object_index, long marker_name);
    __forceinline long submit_unattached_sound(const s_sound_location *location, s_game_sound_impulse_internal_parameters *storage);
    long submit_unattached_object_sound(long object_index, long sound_type);
    long submit_unspatialized_sound(void);
};

typedef char game_sound_impulse_size[(sizeof(void *) != 4 || ((sizeof(c_game_sound_impulse_parameters) == 0x10))) ? 1 : -1];

#endif
