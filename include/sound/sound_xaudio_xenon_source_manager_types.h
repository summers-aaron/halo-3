#ifndef SOUND_SOUND_XAUDIO_XENON_SOURCE_MANAGER_TYPES_H
#define SOUND_SOUND_XAUDIO_XENON_SOURCE_MANAGER_TYPES_H

#include <xtl.h>
#include <xaudio.h>
#include <string.h>
#include "sound/sound_xaudio_xenon.h"
#include "sound/sound_permutation_chunk.h"


class c_platform_source_packet_controller
{
public:
    void source_added(void *source);
};


struct s_source_status_flags
{
    unsigned long bits[7];

    __forceinline void set(long index, bool value)
    {
        if (value)
            bits[index >> 5] |= 1UL << (index & 31);
        else
            bits[index >> 5] &= ~(1UL << (index & 31));
    }
};

struct s_platform_sound_status
{
    s_source_status_flags source;
    long count[5];
    long peak[5];
    s_source_status_flags voice;
};

class c_xenon_audio_source_manager;

class c_platform_source_datum
{
public:
    short state : 4;
    unsigned short sound_channel_count : 3;
    signed short packet_count : 3;
    signed short current_packet : 3;
    unsigned short remaining : 3;
    unsigned short flags;
    s_sound_permutation_chunk *packets[2];
    IXAudioSourceVoice *voice;
    long channel;
    long sample_offset;
    c_platform_source_packet_controller *controller;
    long pad; // Tail padding in the 0x20-byte Xbox 360 source datum.

    __forceinline bool is_start_pending() const { return ((flags >> 7) & 1) != 0; }
    __forceinline long get_sound_channel_count() const { return sound_channel_count; }
    __forceinline void signal_complete()
    {
        if (state == 3)
            state = 1;
        flags |= 1 << 3;
    }
    __forceinline void dispose()
    {
        if (voice)
        {
            voice->Release();
            voice = 0;
        }
    }
    void fill();
    bool update_stopped_status(bool force_stop);
    void initialize_channel(long channel);
    void stop();
    void queue_sound(s_sound_permutation_chunk *sound_permutation_chunk);
    void dequeue_sound(s_sound_permutation_chunk *finished_sound);

    // March 82e4e048: stop(), then flush_xaudio_source_internal().
    __forceinline void stop_immediately()
    {
        stop();
        flush_xaudio_source_internal();
    }

    __forceinline bool flushing() const
    {
        return ((flags >> 2) & 1) != 0;
    }

    __forceinline bool is_playing() const
    {
        return (flags & 1) != 0;
    }

    __forceinline bool no_more_packets() const { return flags_test_inline(3); }

    __forceinline void flush()
    {
        if (state != 0)
        {
            while (!update_stopped_status(true))
                ;
        }
        if (current_packet != 0)
            current_packet = 0;
        if (packet_count != 0)
            packet_count = 0;
    }

    __forceinline void prepare_for_unpause()
    {
        if (state == 1)
        {
            while (!update_stopped_status(true))
                ;
        }
        initialize_channel(channel);
    }

    __forceinline bool initialize(const XAUDIOSOURCEVOICEINIT *init, c_platform_source_packet_controller *packet_controller, long type, long channels)
    {
        state = 0;
        sample_offset = 0;
        remaining = type;
        controller = packet_controller;
        bool success = false;
        if (XAudioCreateSourceVoice(init, &voice) >= 0)
            success = true;
        channels &= 7;
        sound_channel_count = channels;
        current_packet = 0;
        packet_count = 0;
        channel = -1;
        flags = 0;
        memset(packets, 0, sizeof(packets));
        return success;
    }

private:
    bool queue_packet(bool force);
    void flush_xaudio_source_internal();
    __forceinline void release_sound_data();
    __forceinline void resync_internal_state_due_to_faked_completions();
    __forceinline bool can_queue_packets();
    __forceinline bool can_acquire_packet(bool force) const { return packet_count > current_packet; }
    __forceinline bool flags_test_inline(long bit) const { return ((flags >> bit) & 1) != 0; }
    __forceinline void packet_submitted() { sample_offset = 0; current_packet++; }
    __forceinline void packet_failed();
    void fake_finish_packets_due_to_stop();
    friend class c_xenon_audio_source_manager;
};

class c_xenon_audio_source_manager
{
public:
    long first_source[5];
    long source_counts[5];
    c_platform_source_datum sources[196];
    long source_count;
    c_platform_source_packet_controller packet_controller;
    bool any_active;

public:
    __forceinline void source_signal_complete(long source_index) { sources[source_index].signal_complete(); }
    __forceinline void source_stop(long source_index) { sources[source_index].stop(); }
    __forceinline void dispose()
    {
        for (long source_index = 0; source_index < source_count; source_index++)
            sources[source_index].dispose();
        source_count = 0;
    }
    __forceinline void update_source_status(s_platform_sound_status *sound_status)
    {
        for (long index = 0; index < source_count; index++)
        {
            c_platform_source_datum *source = &sources[index];
            sound_status->source.set(index, source->state == 0);
        }
        for (long index = 0; index < 5; index++)
        {
            sound_status->count[index] = first_source[index];
            sound_status->peak[index] = source_counts[index];
        }
    }
    // March 82d1e418/82d1e468: indexed source and the active source count.
    __forceinline c_platform_source_datum *get_source(long source_index) { return &sources[source_index]; }
    __forceinline long get_source_count() const { return source_count; }
    void initialize(c_xenon_audio_dsp_manager *dsp_manager, const s_sound_preferences *preferences,
        c_xenon_audio_output_manager *output_manager, void (*packet_callback)(const XAUDIOVOICEPACKETCALLBACK *));
    void begin_scene();
    void end_scene();
    void source_unpause(long source_index);
};


#if defined(_M_PPCBE) || defined(_XBOX)
typedef char platform_source_datum_size[(sizeof(void *) != 4 || (sizeof(c_platform_source_datum) == 0x20)) ? 1 : -1];
typedef char xenon_audio_source_manager_size[(sizeof(void *) != 4 || (sizeof(c_xenon_audio_source_manager) == 0x18b0)) ? 1 : -1];
#endif

#endif // SOUND_SOUND_XAUDIO_XENON_SOURCE_MANAGER_TYPES_H
