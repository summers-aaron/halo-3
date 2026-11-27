#ifndef SOUND_SOUND_XAUDIO_XENON_OUTPUT_MANAGER_H
#define SOUND_SOUND_XAUDIO_XENON_OUTPUT_MANAGER_H

#include <xtl.h>
#include <xaudio.h>
#include "memory/static_arrays.h"
#include "memory/wrapped_arrays.h"

enum e_sound_channel_type
{
    k_sound_channel_type_count = 5
};

class c_xenon_audio_output_manager
{
public:
    // Seven outputs per source channel type.
    __forceinline c_wrapped_array<XAUDIOVOICEOUTPUTENTRY> get_source_output(e_sound_channel_type channel_type)
    {
        return make_wrapped_array(m_source_outputs[channel_type].get_elements(), m_source_outputs[channel_type].get_count());
    }

    c_static_array<c_static_array<XAUDIOVOICEOUTPUTENTRY, 7>, k_sound_channel_type_count> m_source_outputs;
};
typedef char xaudio_output_manager_size[(sizeof(void *) != 4 || ((sizeof(c_xenon_audio_output_manager) == 0x118) )) ? 1 : -1];

#endif // SOUND_SOUND_XAUDIO_XENON_OUTPUT_MANAGER_H
