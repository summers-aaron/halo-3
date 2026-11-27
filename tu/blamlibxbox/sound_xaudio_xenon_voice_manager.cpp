// blamlibxbox/sound_xaudio_xenon_voice_manager.obj: from the records 0x139880
// @flags /TP /O2 /Oc
#include "math/real_math_types.h"
#include "cseries/cseries_wrapped.h"
#include "sound/sound_xaudio_xenon_voice_manager.h"
#include "sound/sound_xaudio_xenon.h"

namespace {
static __forceinline void convert_vector(X3DAUDIO_VECTOR *destination, const real_vector3d *source) {
    destination->x = source->i;
    destination->y = source->j;
    destination->z = source->k;
}
static __forceinline void convert_point(X3DAUDIO_VECTOR *destination, const real_point3d *source) {
    destination->x = source->x;
    destination->y = source->y;
    destination->z = source->z;
}
}
void c_xenon_audio_voice_manager::update_listeners(const s_platform_sound_listener *source, c_flags<unsigned long, unsigned long, 4> flags) {
    // match: 0x139880: folds this to the global address so member stores use absolute lis offsets, not r3-relative.
    __assume(this == &g_xaudio_voice_manager);
    listener_flags = flags;
    for (long listener_index = 0; listener_index < 4; ++listener_index) {
        if (listener_flags.test(listener_index)) {
            X3DAUDIO_LISTENER *listener = &listeners[listener_index];
            convert_vector(&listener->OrientFront, &source[listener_index].forward);
            convert_vector(&listener->OrientTop, &source[listener_index].up);
            convert_point(&listener->Position, &source[listener_index].position);
            convert_vector(&listener->Velocity, &source[listener_index].velocity);
        }
    }
}
