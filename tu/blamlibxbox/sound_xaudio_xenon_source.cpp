// blamlibxbox/sound_xaudio_xenon_source.obj: from the records 0x1378b8 0x137990 0x137b38 0x137d18 0x137fe8 0x1394c0 0x13d6d0 0x2fa1e0 0x2fa260 0x2fa310
// @flags /TP /O2 /Oc
#include "memory/data_types.h"
// @address 0x137b38 ?fill@c_platform_source_datum@@QAAXXZ

#include <string.h>
#include <stdlib.h>
#include "cache/xbox_sound_cache.h"
#include "sound/sound_xaudio_xenon_source.h"
#include "sound/sound_xaudio_xenon.h"
#include "sound/sound_cache_inline.h"
#include "sound/sound_xaudio_xenon_source_manager_types.h"
#include "sound/sound_xaudio_xenon_packet_controller_default.h"
#include "game/platform_source_packet_types.h"
static __forceinline void sound_xaudio_xenon_source_flags_set(unsigned short *flags, long bit, bool value)
{
    if (value)
        *flags |= 1 << bit;
    else
        *flags &= ~(1 << bit);
}

// match: 0x13d6d0: the stripped assert call keeps queue_sound's empty cases, so its jump table stays
static __forceinline void sound_xaudio_xenon_source_assert(bool condition) { (void)condition; }

static __forceinline unsigned long sound_xaudio_xenon_source_uses_xma2(s_sound_permutation_chunk *chunk)
{
    return (unsigned long)chunk->flags_and_sample_size >> 31;
}

__forceinline void c_platform_source_datum::release_sound_data()
{
    while (packet_count > 0)
    {
        sound_cache_sound_release(packets[packet_count - 1]);
        --packet_count;
    }
}

__forceinline void c_platform_source_datum::resync_internal_state_due_to_faked_completions()
{
    long states[3] = { 1, 4, 5 };
    state = states[packet_count];
}

__forceinline void c_platform_source_datum::packet_failed()
{
    sound_cache_sound_hardware_unlock(packets[current_packet]);
}
__forceinline bool c_platform_source_datum::can_queue_packets()
{
    bool result = false;
    if (!flushing() && ((1 << state) & 0x3c) != 0)
    {
        unsigned char status;
        long hr = XAudioVoice_GetVoiceState(voice, &status);
        if (hr >= 0)
            result = (bool)(_rotl(status, 27) & 1);
    }
    return result;
}

// @address 0x1378b8 xaudio_source_handle_packet_complete
extern "C" void xaudio_source_handle_packet_complete(
    c_xenon_audio_source_manager *source_manager, const XAUDIOVOICEPACKETCALLBACK *callback)
{
    if (callback)
    {
        long source_index = address_from_pointer(callback->pVoiceContext);
        if (source_index >= 0 && source_index < g_xaudio_source_manager.get_source_count())
        {
            s_sound_permutation_chunk *finished_sound = (s_sound_permutation_chunk *)callback->pPacketContext;
            c_platform_source_datum *source = g_xaudio_source_manager.get_source(source_index);
            source->dequeue_sound(finished_sound);
            sound_cache_sound_hardware_unlock(finished_sound);
            if (!source->no_more_packets() && !source->flushing())
                source->fill();
        }
    }
}

// @address 0x137990 ?dequeue_sound@c_platform_source_datum@@QAAXPAUs_sound_permutation_chunk@@@Z
void c_platform_source_datum::dequeue_sound(s_sound_permutation_chunk *finished_sound)
{
    --current_packet;
    --packet_count;
    sound_cache_sound_release(finished_sound);
    memmove(packets, packets + 1, packet_count * sizeof(packets[0]));
    memset(packets + packet_count, 0, (2 - packet_count) * sizeof(packets[0]));
    switch (packet_count)
    {
    case 0:
        if (no_more_packets())
            state = 1;
        else
            state = 3;
        break;
    case 1:
        state = 4;
        break;
    default:
        break;
    }
}

void c_platform_source_datum::fill()
{
    bool force = false;
    while (can_queue_packets())
    {
        if (queue_packet(force))
            force = true;
        else
            break;
    }
}

// @address 0x137d18 ?queue_packet@c_platform_source_datum@@AAA_N_N@Z
bool c_platform_source_datum::queue_packet(bool force)
{
    bool result = false;
    s_platform_source_packet packet;
    memset(&packet, 0, sizeof(packet));
    if (can_acquire_packet(force) && c_platform_source_packet_controller_default::acquire_packet(this, &packet))
    {
        long hr;
        if ((unsigned long)packet.type >= 1)
        {
            unsigned long submit_flags = 0;
            if (!can_acquire_packet(true) && no_more_packets())
                submit_flags |= XAUDIOSUBMITPACKET_DISCONTINUITY;
            hr = XAudioSourceVoice_SubmitSourceBuffer(voice, &packet.source_buffer, (XAUDIOSUBMITPACKET)submit_flags);
        }
        else
        {
            hr = XAudioSourceVoice_SubmitPacket(voice, &packet.packet, 0);
        }
        result = hr == 0;
        if (result)
        {
            packet_submitted();
            if (flags_test_inline(6))
            {
                XAudioSourceVoice_Start(voice, 1);
                sound_xaudio_xenon_source_flags_set(&flags, 6, false);
            }
            if (!can_acquire_packet(true) && no_more_packets())
            {
                XAudioSourceVoice_Discontinuity(voice);
                sound_xaudio_xenon_source_flags_set(&flags, 5, true);
            }
        }
        else
            packet_failed();
        sound_xaudio_xenon_source_flags_set(&flags, 4, false);
    }
    return result;
}

// @address 0x137fe8 ?update_stopped_status@c_platform_source_datum@@QAA_N_N@Z
bool c_platform_source_datum::update_stopped_status(bool force_stop)
{
    bool stopped = false;
    unsigned char voice_state;
    XAudioVoice_GetVoiceState(voice, &voice_state);
    stopped = (voice_state & 7) == 0;
    if (stopped || force_stop)
    {
        flush_xaudio_source_internal();
        state = 0;
        release_sound_data();
        sound_xaudio_xenon_source_flags_set(&flags, 2, false);
        sound_xaudio_xenon_source_flags_set(&flags, 0, false);
        sound_xaudio_xenon_source_flags_set(&flags, 6, false);
    }
    return stopped || force_stop;
}

void c_platform_source_datum::stop()
{
    if (state)
    {
        if (state == 3)
            state = 1;
        sound_xaudio_xenon_source_flags_set(&flags, 3, true);
        XAudioSourceVoice_Stop(voice, 0);
        sound_xaudio_xenon_source_flags_set(&flags, 2, true);
        sound_xaudio_xenon_source_flags_set(&flags, 7, false);
    }
}

void c_platform_source_datum::initialize_channel(long channel)
{
    this->channel = channel;
    sound_xaudio_xenon_source_flags_set(&flags, 7, channel != -1);
    if (!flags_test_inline(7))
        sound_xaudio_xenon_source_flags_set(&flags, 6, true);
    else
        sound_xaudio_xenon_source_flags_set(&flags, 6, false);
    sound_xaudio_xenon_source_flags_set(&flags, 2, false);
    sound_xaudio_xenon_source_flags_set(&flags, 3, false);
    sound_xaudio_xenon_source_flags_set(&flags, 5, false);
    sound_xaudio_xenon_source_flags_set(&flags, 0, true);
    sound_xaudio_xenon_source_flags_set(&flags, 8, false);
    sample_offset = 0;
    state = 2;
}

// @address 0x2fa310 ?fake_finish_packets_due_to_stop@c_platform_source_datum@@AAAXXZ
void c_platform_source_datum::fake_finish_packets_due_to_stop()
{
    if (current_packet < packet_count)
    {
        while (packet_count > current_packet)
        {
            sound_cache_sound_release(packets[packet_count - 1]);
            --packet_count;
        }
    }
    resync_internal_state_due_to_faked_completions();
    unsigned char voice_state;
    XAudioVoice_GetVoiceState(voice, &voice_state);
    bool stopped = (voice_state & 7) == 0;
    if (stopped && state > 1)
        flush_xaudio_source_internal();
}

// @address 0x1394c0 ?flush_xaudio_source_internal@c_platform_source_datum@@AAAXXZ
void c_platform_source_datum::flush_xaudio_source_internal()
{
    XAudioSourceVoice_Stop(voice, 1);
    if (flags_test_inline(8)) {
        XAudioSourceVoice_FlushSourceBuffers(voice);
        return;
    }
    XAudioSourceVoice_FlushPackets(voice);
}

// @address 0x13d6d0 ?queue_sound@c_platform_source_datum@@QAAXPAUs_sound_permutation_chunk@@@Z
void c_platform_source_datum::queue_sound(s_sound_permutation_chunk *sound_permutation_chunk)
{
    switch (state)
    {
    case 0:
    case 1:
        sound_xaudio_xenon_source_assert(false);
        break;
    case 2:
        sound_xaudio_xenon_source_flags_set(&flags, 8, (unsigned char)sound_xaudio_xenon_source_uses_xma2(sound_permutation_chunk) != 0);
    case 3:
        state = 4;
        break;
    case 4:
        state = 5;
        break;
    case 5:
        sound_xaudio_xenon_source_assert(false);
        break;
    }
    if (packet_count < 2)
    {
        _sound_cache_sound_request(-1, sound_permutation_chunk, 2);
        packets[packet_count] = sound_permutation_chunk;
        packet_count++;
    }
}
