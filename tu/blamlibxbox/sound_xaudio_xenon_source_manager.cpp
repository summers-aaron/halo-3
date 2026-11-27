// blamlibxbox/sound_xaudio_xenon_source_manager.obj: from the records 0x137f40 0x13e350 0x288960 0x288bb8
// @flags /TP /O2 /Oc
#include <xtl.h>
#include <xaudio.h>
#include <string.h>
#include "cseries/pointer_utilities.h"
#include "memory/wrapped_arrays.h"
#include "sound/sound_xaudio_xenon.h"
#include "sound/sound_preferences.h"
#include "sound/sound_xaudio_xenon_output_manager.h"

#include "sound/sound_xaudio_xenon_source_manager_types.h"
#include "sound/sound_xaudio_xenon_dsp_manager_types.h"

// @address 0x288960 ?initialize@c_xenon_audio_source_manager@@QAAXPAVc_xenon_audio_dsp_manager@@PBUs_sound_preferences@@PAVc_xenon_audio_output_manager@@P6AXPBUXAUDIOVOICEPACKETCALLBACK@@@Z@Z
void c_xenon_audio_source_manager::initialize(c_xenon_audio_dsp_manager *dsp_manager, const s_sound_preferences *preferences,
    c_xenon_audio_output_manager *output_manager, void (*packet_callback)(const XAUDIOVOICEPACKETCALLBACK *))
{
    // match: 0x288960: value range assumption guides branch lowering
    __assume(this == &g_xaudio_source_manager);
    // match: 0x288960: value range assumption guides branch lowering
    __assume(output_manager == &g_xaudio_output_manager);
    packet_callback = platform_sound_xaudio_xenon_source_callback;

    unsigned char channel_counts[5] = { 1, 2, 4, 6, 0 };

    for (unsigned long channel_type = 0; channel_type < 5; channel_type++)
    {
        first_source[channel_type] = source_count;
        unsigned char channel_count = channel_counts[channel_type];
        if (channel_count > 0)
        {
            XAUDIOVOICEOUTPUT voice_output;
            XAUDIOSOURCEVOICEINIT init;
            memset(&init, 0, sizeof(init));
            init.Format.SampleType = XAUDIOSAMPLETYPE_XMA;
            unsigned long stream_count = channel_count / 2;
            init.Format.NumStreams = stream_count > 1 ? stream_count : 1;
            for (long stream_index = 0; stream_index < init.Format.NumStreams; stream_index++)
            {
                init.Format.Stream[stream_index].ChannelCount = channel_count > 2 ? 2 : channel_count;
                init.Format.Stream[stream_index].SampleRate = 44100;
                init.Format.Stream[stream_index].DecodeBufferSize = 0;
            }
            init.pEffectChain = dsp_manager->get_per_source_effect_chain(channel_count);
            init.MaxPacketCount = 2;
            init.pfnPacketCompletionCallback = packet_callback;
            init.MaxPitchShift = 5.0f;
            c_wrapped_array<XAUDIOVOICEOUTPUTENTRY> source_output = output_manager->get_source_output((e_sound_channel_type)channel_type);
            voice_output.EntryCount = source_output.count();
            voice_output.paEntries = source_output.get_elements();
            init.MaxOutputVoiceCount = source_output.count() + 1;
            init.MaxChannelMapEntryCount = channel_counts[channel_type] * 6;
            init.pVoiceOutput = &voice_output;
            for (long source_index = 0; source_index < preferences->source_counts[channel_type]; source_index++)
            {
                init.pContext = pointer_from_address(source_count);
                c_platform_source_datum datum;
                if (datum.initialize(&init, &packet_controller, channel_type, channel_count))
                {
                    long new_index = source_count;
                    source_count++;
                    sources[new_index] = datum;
                }
            }
        }
        source_counts[channel_type] = source_count;
    }
}

void c_xenon_audio_source_manager::begin_scene()
{
    long count = g_xaudio_source_manager.source_count;
    g_xaudio_source_manager.any_active = false;
    if (count <= 0)
        return;
    unsigned int remaining = count;
    c_platform_source_datum *source = g_xaudio_source_manager.sources;
    do {
        if (source->state == 1)
            source->update_stopped_status(false);
        if (source->flushing())
            source->fake_finish_packets_due_to_stop();
        g_xaudio_source_manager.any_active = g_xaudio_source_manager.any_active || source->is_playing();
        source++;
        remaining--;
    } while (remaining != 0);
}

void c_xenon_audio_source_manager::end_scene()
{
    long count = g_xaudio_source_manager.source_count;
    if (count <= 0)
        return;
    unsigned int remaining = count;
    c_platform_source_datum *source = g_xaudio_source_manager.sources;
    do {
        source->fill();
        source++;
        remaining--;
    } while (remaining != 0);
}

void c_xenon_audio_source_manager::source_unpause(long source_index)
{
    c_platform_source_datum *source = &g_xaudio_source_manager.sources[source_index];
    source->flush();
    source->prepare_for_unpause();
}
