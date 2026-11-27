// blamlibxbox/sound_xaudio_xenon_packet_controller_default.obj: from the records 0x13e3b0
// @flags /TP /O2 /Oc
#include <xtl.h>
#include "cseries/cseries.h"
#include "sound/sound_xaudio_xenon_source_manager_types.h"
#include "sound/sound_xaudio_xenon_packet_controller_default.h"
#include "sound/sound_cache_inline.h"
#include "sound/sound_xaudio_xenon_packet_events.h"
#include "game/platform_source_packet_types.h"

// @address 0x13e3b0 ?acquire_packet@c_platform_source_packet_controller_default@@SA_NPAVc_platform_source_datum@@PAUs_platform_source_packet@@@Z
bool c_platform_source_packet_controller_default::acquire_packet(c_platform_source_datum *owner, s_platform_source_packet *out_packet) {
    s_sound_permutation_chunk *permutation = (s_sound_permutation_chunk *)owner->packets[owner->current_packet];
    unsigned long size;
    void *data = sound_cache_sound_hardware_lock(permutation, &size);
    void *context = sound_xaudio_xenon_allocate_packet_completion_event(permutation);
    if (sound_cache_sound_uses_xma2(permutation)) {
        s_xma2_packet_info info;
        sound_cache_sound_get_xma2_info(permutation, &info);
        long sample_offset = owner->sample_offset;
        unsigned long first_sample = permutation->first_sample;
        unsigned long last_sample = permutation->last_sample;
        out_packet->type = 1;
        out_packet->source_buffer.pBuffer = data;
        out_packet->source_buffer.pContext = context;
        out_packet->source_buffer.BufferSize = size;
        out_packet->source_buffer.LoopCount = 0;
        out_packet->source_buffer.PlayBegin = info.first_sample +
            (MIN((unsigned long)
                 MAX(sample_offset, 0), last_sample - first_sample - 128) &
                ~127UL);
        out_packet->source_buffer.PlayEnd = info.last_sample;
    } else {
        out_packet->type = 0;
        out_packet->packet.pBuffer = data;
        out_packet->packet.pContext = context;
        out_packet->packet.BufferSize = size;
        out_packet->packet.LoopCount = 0;
    }
    return true;
}
