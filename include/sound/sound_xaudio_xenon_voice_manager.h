#ifndef SOUND_SOUND_XAUDIO_XENON_VOICE_MANAGER_H
#define SOUND_SOUND_XAUDIO_XENON_VOICE_MANAGER_H

// The XAudio voice manager (tu/blamlibxbox/sound_xaudio_xenon_voice_manager.cpp): the four listeners and the 3D
// state of every voice's speakers, and the X3DAudio instance.

#include <xtl.h>
#include <x3daudio.h>
#include <stddef.h>
#include "math/real_math_types.h"
#include "cseries/cseries_wrapped.h"
#include "sound/sound_xaudio_xenon_3d_voice.h"

class c_xenon_audio_source_manager;

struct s_platform_sound_listener
{
    real_point3d position;
    real_vector3d velocity;
    real_vector3d forward;
    real_vector3d up;
};

class c_xenon_audio_voice_manager
{
public:
    // Initialize the 3D audio instance and remember the source manager.
    __forceinline void initialize(c_xenon_audio_source_manager *new_source_manager, float speed_of_sound)
    {
        X3DAudioInitialize(SPEAKER_XBOX, speed_of_sound, x3d_instance);
        source_manager = new_source_manager;
    }
    __forceinline void dispose() { source_manager = 0; }
    void update_listeners(const s_platform_sound_listener *source, c_flags<unsigned long, unsigned long, 4> flags);

    c_xenon_audio_source_manager *source_manager;
    X3DAUDIO_LISTENER listeners[4];
    c_flags_no_init<unsigned long, unsigned long, 4> listener_flags;
    c_xaudio_voice_speaker_datum speakers[196];
    X3DAUDIO_HANDLE x3d_instance;
};
typedef char xenon_audio_voice_manager_listener_flags_offset[(sizeof(void *) != 4
    || ((offsetof(c_xenon_audio_voice_manager, listener_flags) == 0xd4) )) ? 1 : -1];
typedef char xenon_audio_voice_manager_speakers_offset[(sizeof(void *) != 4 || ((offsetof(c_xenon_audio_voice_manager, speakers) == 0xd8) )) ? 1 : -1];
typedef char xenon_audio_voice_manager_x3d_instance_offset[(sizeof(void *) != 4
    || ((offsetof(c_xenon_audio_voice_manager, x3d_instance) == 0x34e8) )) ? 1 : -1];

#endif // SOUND_SOUND_XAUDIO_XENON_VOICE_MANAGER_H
