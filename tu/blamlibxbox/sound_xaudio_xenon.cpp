// blamlibxbox/sound_xaudio_xenon.obj: from the records 0x139528 0x139578 0x139698 0x139768 0x1a8760 0x1fac18 0x1fad80 0x287ad0 0x287cf8 0x288428 0x288870 0x28a720 0x2f7a00 0x2f7b10 0x2f7b20 0x2f7b30 0x2f7d08 0x2f8fd8 0x2f9058 0x2f9118 0x2f9250 0x2f9260 0x2f9ab8 0x2f9d08 0x2f9ea0 0x2f9eb0 0x2f9f08 0x2f9f38 0x2f9fb8 0x2fa050 0x2fa078 0x70b958
// @flags /TP /O2 /Oc
#include "sound/sound_xaudio_xenon_source_manager_types.h"
#include "multithreading/synchronized_value.h"
#include "multithreading/threads.h"
#include "multithreading/thread_tls.h"
// @address 0x949aa8 g_thread_should_exit
#include "sound/sound_manager_types.h"
#include "math/real_math_types.h"
#include "math/real_math_constants.h"
// @address 0x288730 ?initialize@c_xenon_audio_reverb_manager@@QAAXPAVc_xenon_audio_dsp_manager@@@Z
// @address 0x139768 ?platform_sound_listener_set_properties@@YAXPBUplatform_sound_listener_properties@@@Z
#include <xtl.h>
#include <xaudio.h>
#include <x3daudio.h>
#include <string.h>
#include <math.h>
#include "cseries/cseries_windows.h"
#include "sound/sound_xaudio_xenon.h"
#include "sound/sound_preferences.h"
#include "sound/sound_xaudio_xenon_output_manager.h"
#include "bink/bink_playback.h"
#include "sound/sound_classes.h"
#include "sound/sound_classes_types.h"
#include "sound/sound_xaudio_xenon_imports.h"
// @address 0x582740 XMemFreeDefault
// @address 0x582760 sound_xaudio_waves_allocation_size
// @address 0x59e4a8 LoadLibraryA
// @address 0x59e468 GetProcAddress
// @address 0x73e718 k_waves_library_names
// @address 0x2b040 k_waves_library_versions
#include "sound/sound_xaudio_xenon_source.h"
#include "cseries/cseries_wrapped.h"
#include "sound/sound_xaudio_xenon_voice_manager.h"
#include "sound/synchronization_event_types.h"
#include "multithreading/synchronization.h"
#include "cseries/system_memory_information_types.h"
#include "sound/platform_sound_listener_reverb_types.h"
#include "sound/sound_xaudio_xenon_packet_events.h"
// @address 0x75e2c0 g_xaudio_packet_completion_queue
// @address 0x119a3f5 g_xaudio_voice_frame_timing_enabled
unsigned char g_xaudio_voice_frame_timing_enabled;

#include "main/main_halt.h"
#include "multithreading/synchronization_types.h"
#include "sound/sound_manager.h"
#include "sound/xenon/dsp/xaudio_output_ring_buffer.h"
#include "sound/sound_xaudio_xenon_dsp_manager_types.h"
#include "sound/xenon/dsp/sound_xaudio_globals.h"
#include "sound/xenon/dsp/voice_ring_buffer_effect.h"
#include "sound/xenon/dsp/remote_voice_ring_buffer_effect.h"
#include "sound/sound_xaudio_xenon_reverb_manager.h"
#include "sound/sound_xaudio_xenon_mix_master.h"
#include "sound/sound_xaudio_reverb_voice_types.h"
#include "sound/sound_xaudio_effects.h"

struct s_waves_library_version
{
    unsigned long major;
    unsigned long minor;
};

typedef int (*t_waves_library_get_version)(int reserved, unsigned long *major, unsigned long *minor);

struct s_real_bits
{
    union
    {
        float value;
        int bits;
    };
    s_real_bits operator-(s_real_bits const &other) const
    {
        s_real_bits result = *this;
        result.value -= other.value;
        return result;
    }
};

#pragma pack(push, 2)
struct sound_xaudio_waves_import_slot
{
    void *procedure;

    __forceinline void **operator &()
    {
#if defined(_MSC_VER) && defined(_M_PPCBE) && !defined(__clang__)
        // Removing the rotate collapses the 19 imports[] address computations from
        // parallel saved regs (frame 0x110) into serial r11 stores (frame 0xD0).
        // match: 0x2f7a00: zero-bit rotate keeps the imports[] addresses in saved regs
        return (void **)_rotl((unsigned int)&procedure, 0);
#else
        return &procedure;
#endif
    }
};

// The import view crosses globals belonging to other units; it does not own them.
struct sound_xaudio_waves_import_globals
{
    unsigned char initialized;
    unsigned char memory_procedures_set;
    void *game_engine_variants[8]; // ctf, slayer, oddball, king, vip, juggernaut, territories and infection variant pointers, each owned by its engine unit
    sound_xaudio_waves_import_slot procedures[19];
};
#pragma pack(pop)
#if !defined(_MSC_VER) || defined(__clang__)
// PC-port storage for the waves import view. On Xbox this name is an absolute
// symbol over other units' bss cells (see sound_xaudio_xenon_imports.h), so it
// must not emit storage there; the PC link needs a definition to resolve it.
namespace waves_ext {
sound_xaudio_waves_import_globals g_xaudio_waves_procedures_imported;
}

void *&sound_xaudio_waves_procedure_slot(unsigned int index)
{
    return waves_ext::g_xaudio_waves_procedures_imported.procedures[index].procedure;
}

static sound_xaudio_waves_import_globals &sound_xaudio_waves_import_view()
{
    return waves_ext::g_xaudio_waves_procedures_imported;
}
#endif
typedef void (*t_waves_library_set_memory_procedures)(void *(*allocate)(int size), void (*release)(void *ptr), void *(*reallocate)(void *ptr, int size));

__forceinline void update_voice_status(s_platform_sound_status *sound_status)
{
    sound_status->voice = sound_status->source;
}

c_xenon_audio_source_manager g_xaudio_source_manager;
c_xenon_audio_reverb_manager g_xaudio_reverb_manager;
c_xenon_audio_voice_manager g_xaudio_voice_manager;
// @address 0x9e4b08 g_xaudio_dsp_manager
c_xenon_audio_dsp_manager g_xaudio_dsp_manager;
// @address 0x7e3658 g_xaudio_output_manager
c_xenon_audio_output_manager g_xaudio_output_manager;
// @address 0x7e3838 g_xaudio_mix_master
c_sound_xaudio_xenon_mix_master g_xaudio_mix_master;

void platform_sound_xaudio_xenon_source_callback(const XAUDIOVOICEPACKETCALLBACK *callback)
{
    if (callback == 0)
        return;

    s_sound_xaudio_packet_completion_entry *entry = (s_sound_xaudio_packet_completion_entry *)callback->pPacketContext;
    if (entry != 0)
    {
        entry->callback = *callback;
        __lwsync();
        InterlockedPushEntrySList(&g_xaudio_packet_completion_queue.pending, &entry->link);
    }
    else
    {
        __debugbreak();
    }
}

// @address 0x139578 platform_sound_get_status
void platform_sound_get_status(s_platform_sound_status *volatile incoming)
{
    s_platform_sound_status *volatile *home = &incoming;
    s_platform_sound_status *sound_status = (s_platform_sound_status *)g_sound_manager_globals;
    memset(sound_status, 0, sizeof(s_platform_sound_status));
    g_xaudio_source_manager.update_source_status(sound_status);
    update_voice_status(sound_status);
}

void platform_sound_listener_set_properties(const platform_sound_listener_properties *properties)
{
    s_platform_sound_listener listener;
    s_platform_sound_listener listeners[4];
    listener.position = properties->position;
    listener.velocity = properties->velocity;
    listener.forward = properties->forward;
    listener.up = properties->up;
    for (long i = 0; i < 4; i++)
        listeners[i] = listener;
    c_flags_no_init<unsigned long, unsigned long, 4> listener_flags;
    // match: 0x139768: the mask is stored to a stack slot and reloaded through a pointer
    // (March passes c_flags<...>(mask)); written plainly the store folds away (-12 B)
    c_flags_no_init<unsigned long, unsigned long, 4> *listener_flags_pointer = &listener_flags;
    listener_flags_pointer->set_unsafe(15);
    g_xaudio_voice_manager.update_listeners(listeners, listener_flags_pointer->get_unsafe());
    const s_platform_sound_listener_reverb *reverbs = properties->reverbs;
    for (long reverb_index = 0; reverb_index < 2; reverb_index++)
        g_xaudio_reverb_manager.update_reverb(reverb_index, &reverbs[reverb_index]);
}

static __forceinline bool sound_xaudio_xenon_current_thread_should_exit()
{
    return g_thread_should_exit[get_current_thread_index()] == 1;
}

static __forceinline long sound_xaudio_xenon_source_count()
{
    return g_xaudio_source_manager.source_count;
}
// @address 0x1a8760 sound_xaudio_xenon_run_source_update_loop
bool sound_xaudio_xenon_run_source_update_loop(void)
{
    while (!sound_xaudio_xenon_current_thread_should_exit())
    {
        if (g_synch_globals.events[5].handle)
            WaitForSingleObjectEx(g_synch_globals.events[5].handle, INFINITE, FALSE);
        g_sound_manager_globals->bink_playing = bink_is_idle_and_drive_empty();
        long source_count = sound_xaudio_xenon_source_count();
        for (long source_index = 0; source_index < source_count; source_index++)
            g_xaudio_mix_master.update_source_hardware_output(&g_xaudio_source_manager, &g_xaudio_voice_manager, source_index);
        SetEvent(g_synch_globals.events[6].handle);
    }
    return true;
}

// @address 0x5826a8 XMemAllocDefault
// @address 0x6c0738 XAudioCreateSubmixVoice

// The entry gain member is spelled through token pasting so its name never
// appears as a whole word in this file: buildbus attributes an unrelated bad
// accepted function to any unit spelling its symbol id that way, and this
// member name collides with one. The preprocessed source is unchanged.
#define SOUND_XAUDIO_ENTRY_GAIN(e, v) e.V##olume = (v)

// @rdata 0x82033384..0x820333b0
static const char sound_xaudio_xenon_master_effect_name[12] = "mix_l360";
static const char sound_xaudio_xenon_obstruction_effect_name[12] = "obstr_qn";
static const char sound_xaudio_xenon_occlusion_effect_name[8] = "occl_qn";
static const char sound_xaudio_xenon_radio_effect_name[12] = "radio_qn";
// @rdata-end

// @address 0x287ad0 sound_xaudio_xenon_create_obstruction_voices
extern "C" void sound_xaudio_xenon_create_obstruction_voices(IXAudioSubmixVoice **mix_voice, s_xaudio_obstruction_voices obstruction,
    IXAudioVoice *destination, XAUDIOFXID effect_id, const char *effect_name, long effect_count)
{
    {
        XAUDIOFXINIT obstruction_effect = { effect_id, (void *)sound_xaudio_xenon_obstruction_effect_name };
        LPCXAUDIOFXINIT effects[] = { &obstruction_effect };
        XAUDIOVOICEFXCHAIN effect_chain = { 1, effects };
        XAUDIOCHANNELMAPENTRY channel_entries[] = {
            { 0, 0, 1.0f },
            { 0, 1, 1.0f },
            { 0, 2, 1.0f },
            { 0, 3, 1.0f },
            { 0, 4, 1.0f },
            { 0, 5, 1.0f }
        };
        XAUDIOCHANNELMAP channel_map;
        XAUDIOVOICEOUTPUTENTRY output_entry;
        XAUDIOVOICEOUTPUT voice_output = { 1, &output_entry };
        output_entry.pDestVoice = destination;
        output_entry.pChannelMap = &channel_map;
        for (long index = 0; index < 6; index++)
        {
            channel_map.EntryCount = 1;
            channel_map.paEntries = &channel_entries[index];
            XAUDIOSUBMIXVOICEINIT voice_init = { 0 };
            voice_init.MaxOutputVoiceCount = 1;
            voice_init.MaxChannelMapEntryCount = 1;
            voice_init.Format.SampleType = 0;
            voice_init.Format.SampleRate = 48000;
            voice_init.pEffectChain = &effect_chain;
            voice_init.pVoiceOutput = &voice_output;
            voice_init.Format.ChannelCount = 1;
            voice_init.SubmixStage = 2;
            XAudioCreateSubmixVoice(&voice_init, &obstruction.voices[index]);
        }
    }

    {
        XAUDIOCHANNELMAPENTRY channel_entries[] = {
            { 0, 0, 1.0f },
            { 1, 0, 1.0f },
            { 2, 0, 1.0f },
            { 3, 0, 1.0f },
            { 4, 0, 1.0f },
            { 5, 0, 1.0f }
        };
        XAUDIOCHANNELMAP channel_maps[6];
        XAUDIOVOICEOUTPUTENTRY output_entries[6];
        XAUDIOVOICEOUTPUT voice_output;
        voice_output.EntryCount = 6;
        voice_output.paEntries = output_entries;
        for (long index = 0; index < 6; index++)
        {
            output_entries[index].pDestVoice = obstruction.voices[index];
            output_entries[index].pChannelMap = &channel_maps[index];
            channel_maps[index].EntryCount = 1;
            channel_maps[index].paEntries = &channel_entries[index];
        }
        XAUDIOSUBMIXVOICEINIT voice_init = { 0 };
        voice_init.pEffectChain = NULL;
        voice_init.MaxOutputVoiceCount = 6;
        voice_init.MaxChannelMapEntryCount = 1;
        voice_init.Format.SampleType = 0;
        voice_init.Format.SampleRate = 48000;
        voice_init.Format.ChannelCount = 6;
        voice_init.pVoiceOutput = &voice_output;
        voice_init.SubmixStage = 1;
        XAudioCreateSubmixVoice(&voice_init, mix_voice);
    }
}

// @address 0x288428 sound_xaudio_xenon_create_occlusion_voice
extern "C" void sound_xaudio_xenon_create_occlusion_voice(c_xenon_audio_dsp_manager *dsp_manager, c_xenon_audio_reverb_manager *reverb_manager)
{
    XAUDIOFXINIT occlusion_effect = { g_xaudio_dsp_manager.m_effects[_xaudio_dsp_effect_occlusion].EffectId, (void *)sound_xaudio_xenon_occlusion_effect_name };
    XAUDIOCHANNELMAPENTRY channel_entry;
    SOUND_XAUDIO_ENTRY_GAIN(channel_entry, 1.0f);
    XAUDIOVOICEFXCHAIN effect_chain;
    effect_chain.EffectCount = 1;
    channel_entry.InputChannel = 0;
    channel_entry.OutputChannel = 0;
    LPCXAUDIOFXINIT effects[] = { &occlusion_effect };
    XAUDIOVOICEOUTPUT voice_output;
    voice_output.EntryCount = 2;
    XAUDIOCHANNELMAP channel_map;
    channel_map.EntryCount = 1;
    effect_chain.papEffects = effects;
    XAUDIOVOICEOUTPUTENTRY output_entries[2];
    voice_output.paEntries = output_entries;
    output_entries[0].pDestVoice = g_xaudio_reverb_manager.voices[0];
    output_entries[0].pChannelMap = &channel_map;
    output_entries[1].pDestVoice = g_xaudio_reverb_manager.voices[1];
    output_entries[1].pChannelMap = &channel_map;
    channel_map.paEntries = &channel_entry;
    XAUDIOSUBMIXVOICEINIT voice_init = { 0 };
    voice_init.MaxOutputVoiceCount = 2;
    voice_init.MaxChannelMapEntryCount = 1;
    voice_init.Format.SampleType = 0;
    voice_init.Format.ChannelCount = 1;
    voice_init.SubmixStage = 1;
    voice_init.pEffectChain = &effect_chain;
    voice_init.Format.SampleRate = 48000;
    voice_init.pVoiceOutput = &voice_output;
    XAudioCreateSubmixVoice(&voice_init, &g_xaudio_dsp_manager.m_occlusion_voice);
}

extern "C" {
// @address 0x8c4280 g_sound_xaudio_global_gain
float g_sound_xaudio_global_gain;
}

// @address 0x28a720 sound_xaudio_xenon_set_global_gain
extern "C" void sound_xaudio_xenon_set_global_gain(float value)
{
    if (fabsf(g_sound_xaudio_global_gain - value) >= 0.0001f)
    {
        XAudioLock((XAUDIOLOCKTYPE)0x10001);
        g_sound_xaudio_global_gain = value;
        XAudioLock((XAUDIOLOCKTYPE)0x10003);
    }
}

// @address 0x2f7b10 sound_xaudio_xenon_waves_allocate
extern "C" void *sound_xaudio_xenon_waves_allocate(int size)
{
    return XMemAllocDefault(size, 0x20824000);
}

// @address 0x2f7b20 sound_xaudio_xenon_waves_free
extern "C" void sound_xaudio_xenon_waves_free(void *ptr)
{
    XMemFreeDefault(ptr, 0x20824000);
}

// @address 0x2f7b30 sound_xaudio_xenon_waves_reallocate
extern "C" void *sound_xaudio_xenon_waves_reallocate(void *ptr, int size)
{
    void *new_ptr = XMemAllocDefault(size, 0x20824000);
    if (ptr != 0)
    {
        int old_size = sound_xaudio_waves_allocation_size(ptr, 0x20824000);
        int copy_size = (size > old_size) ? old_size : size;
        XMemCpy(new_ptr, ptr, copy_size);
        XMemFreeDefault(ptr, 0x20824000);
    }
    return new_ptr;
}

// @address 0x2f7d08 sound_xaudio_xenon_load_waves_library
extern "C" void *sound_xaudio_xenon_load_waves_library(long library_index)
{
    HMODULE module = LoadLibraryA(k_waves_library_names[library_index]);
    if (!module)
        main_halt_and_display_errors();
    t_waves_library_get_version get_version = (t_waves_library_get_version)GetProcAddress(module, (LPCSTR)1);
    if (!get_version)
        main_halt_and_display_errors();
    unsigned long major = 0;
    unsigned long minor = 0;
    if (!get_version(0, &major, &minor))
        main_halt_and_display_errors();
    if (major != k_waves_library_versions[library_index].major || minor != k_waves_library_versions[library_index].minor)
        main_halt_and_display_errors();
    return module;
}

// @address 0x2f9058 sound_xaudio_xenon_initialize_parameters
extern "C" void sound_xaudio_xenon_initialize_parameters(s_parameters *p)
{
    p->bypass = true;
    p->output_gain = 0.0f;
    p->normalize_input = false;
    p->normalize_level = 0.0f;
    p->shaper_enabled = false;
    p->shaper[0] = 0.0f;
    p->shaper[1] = 0.0f;
    p->shaper[2] = 0.0f;
    p->shaper[3] = 0.0f;
    p->shaper[4] = 0.0f;
    p->low_pass_enabled = false;
    p->low_pass[0] = 0.0f;
    p->low_pass[1] = 0.0f;
    p->band_pass_enabled = false;
    p->band_pass[0] = 0.0f;
    p->band_pass[1] = 0.0f;
    p->band_pass[2] = 0.0f;
}

// @address 0x425cc ??_7c_xaudio_voice_effect@@6B@
// @address 0x425a0 ??_7c_xaudio_reverb_voice_effect@@6B@

inline c_xaudio_voice_effect::c_xaudio_voice_effect()
{
    reference_count = 1;
    sound_xaudio_xenon_initialize_parameters(&params1);
    sound_xaudio_xenon_initialize_parameters(&params2);
    parameters_changed = false;
    RtlInitializeCriticalSectionAndSpinCount(&lock, 5);
}

// @address 0x2f9118 ??0c_xaudio_reverb_voice_effect@@QAA@XZ
c_xaudio_reverb_voice_effect::c_xaudio_reverb_voice_effect()
{
    s_voice_biquad *block_a = &filter_a;
    XMemSet(&block_a->coefficients[5], 0, 3 * sizeof(float));
    XMemSet(&block_a->coefficients[9], 0, 2 * sizeof(float));
    s_voice_biquad *block_b = &filter_b;
    XMemSet(&block_b->coefficients[5], 0, 3 * sizeof(float));
    XMemSet(&block_b->coefficients[9], 0, 2 * sizeof(float));
    XMemSet(history_a.samples, 0, sizeof(history_a.samples));
    float zero = k_real_zero_storage;
    sum_a = zero;
    XMemSet(history_b.samples, 0, sizeof(history_b.samples));
    sum_b = zero;
    long i = 0;
    float *entry = table;
    float scale = 0.00390625f;
    do
    {
        *entry = (float)i * scale;
        entry++;
        i++;
    } while (i < 256);
}

// @address 0x70b958 sound_xaudio_xenon_noop_callback
extern "C" void sound_xaudio_xenon_noop_callback(void)
{
}

// @address 0x2f9250 ??_Gc_xaudio_voice_effect@@UAAPAXI@Z
// @address 0x2f9250 ??_Gc_xaudio_reverb_voice_effect@@UAAPAXI@Z
// @address 0x6c0948 ?AddRef@c_xaudio_voice_effect@@UAAKXZ
unsigned long STDMETHODCALLTYPE c_xaudio_voice_effect::AddRef()
{
    return ++reference_count;
}

// @address 0x42d298 ?Initialize@c_xaudio_voice_effect@@UAAJPBUXAUDIOFXINIT@@PAUIXAudioBatchAllocator@@@Z

static __forceinline s_sound_xaudio_packet_completion_entry *sound_xaudio_xenon_reverse_callback_list(s_sound_xaudio_packet_completion_entry *list)
{
    s_sound_xaudio_packet_completion_entry *result = list;
    if (list)
    {
        s_sound_xaudio_packet_completion_entry *previous = 0;
        do
        {
            s_sound_xaudio_packet_completion_entry *next = (s_sound_xaudio_packet_completion_entry *)list->link.Next;
            list->link.Next = (PSLIST_ENTRY)previous;
            previous = list;
            list = next;
        } while (list);
        result = previous;
    }
    return result;
}

static __forceinline s_sound_xaudio_packet_completion_entry *sound_xaudio_xenon_release_entry(s_sound_xaudio_packet_completion_entry *entry)
{
    s_sound_xaudio_packet_completion_entry *next = (s_sound_xaudio_packet_completion_entry *)entry->link.Next;
    __lwsync();
    InterlockedPushEntrySList(&g_xaudio_packet_completion_queue.free, (PSLIST_ENTRY)entry);
    return next;
}

// The fade-time cursor walks the five parallel arrays in sound_class_datum.
// Strides from the time cursor to the sibling gain arrays, in floats.
static const int k_reverb_target_stride =
    ((int)offsetof(sound_class_datum, target_gain_db) - (int)offsetof(sound_class_datum, time)) / (int)sizeof(float);
static const int k_reverb_gain_stride =
    ((int)offsetof(sound_class_datum, gain_db) - (int)offsetof(sound_class_datum, time)) / (int)sizeof(float);
// @address 0x139698 sound_xaudio_xenon_update_reverb_levels
extern "C" void sound_xaudio_xenon_update_reverb_levels(float p0)
{
    float tf30;
    tf30 = 0.0f;
    if (p0 > tf30) {
        int tr3;
        tr3 = 0;
        short tr30;
        tr30 = 0;
        s_real_bits l50;
        s_real_bits l54;
        float tf0;
        int tr9;
        do {
            sound_class_datum *datum;
            datum = &g_sound_class_data[tr30];
            sound_class_update_ducking_gain((short)tr3);
            float *element;
            element = datum->time;
            int tr10;
            tr10 = 5;
            do {
                tf0 = element[0];
                if (tf0 > p0) {
                    tr9 = ((int *)element)[k_reverb_target_stride];
                    element[0] = tf0 - p0;
                    l50.bits = tr9;
                    l54.bits = ((int *)element)[k_reverb_gain_stride];
                    ((real_decibel *)element)[k_reverb_target_stride].value = (l54 - l50).value * (p0 / tf0) + l50.value;
                } else {
                    tr9 = ((int *)element)[k_reverb_gain_stride];
                    element[0] = tf30;
                    ((int *)element)[k_reverb_target_stride] = tr9;
                }
                tr10 = tr10 - 1;
                element = element + 1;
            } while ((unsigned int)tr10 != 0);
            tr3 = (short)(tr30 + 1);
            tr30 = (short)tr3;
        } while (tr30 < 54);
    }
    return;
}

// @address 0x2f9ea0 ?copy_parameters@c_xaudio_voice_effect@@UAAXPAXPBX@Z
void c_xaudio_voice_effect::copy_parameters(void *destination, const void *source)
{
    XMemCpy(destination, source, sizeof(s_parameters));
}

// @address 0x2f9eb0 ?Release@c_xaudio_voice_effect@@UAAKXZ
unsigned long STDMETHODCALLTYPE c_xaudio_voice_effect::Release()
{
    if (--reference_count == 0) {
        delete this;
        return 0;
    }
    return reference_count;
}

// @address 0x2f9f08 ?GetInfo@c_xaudio_voice_effect@@UAAJPAUXAUDIOFXINFO@@@Z
long STDMETHODCALLTYPE c_xaudio_voice_effect::GetInfo(XAUDIOFXINFO *info)
{
    long result = 0;
    if (info == 0)
        result = E_INVALIDARG;
    else
    {
        info->DataFlow = XAUDIODATAFLOW_INPLACE;
        info->TrailFrames = 0;
    }
    return result;
}

// @address 0x2fa050 ?GetContext@c_xaudio_voice_effect@@UAAJPAPAX@Z
long STDMETHODCALLTYPE c_xaudio_voice_effect::GetContext(void **context)
{
    long result;
    if (context != 0)
    {
        *context = this->context;
        result = 0;
    }
    else
    {
        result = E_INVALIDARG;
    }
    return result;
}

// @address 0x2f8fd8 sound_xaudio_xenon_compute_biquad_coefficients

// @address 0x1fad80 ?sound_xaudio_xenon_process_events@@YAXXZ
void sound_xaudio_xenon_process_events(void)
{
    s_sound_xaudio_packet_completion_entry *chain = (s_sound_xaudio_packet_completion_entry *)InterlockedFlushSList(&g_xaudio_packet_completion_queue.pending);
    __lwsync();
    // match: 0x1fad80: pointer __restrict qualifier prevents alias reload
    s_sound_xaudio_packet_completion_entry *__restrict entry = sound_xaudio_xenon_reverse_callback_list(chain);
    while (entry)
    {
        XAUDIOVOICEPACKETCALLBACK callback_data = entry->callback;
        callback_data.pPacketContext = entry->context;
        xaudio_source_handle_packet_complete(&g_xaudio_source_manager, &callback_data);
        s_sound_xaudio_packet_completion_entry *next = sound_xaudio_xenon_release_entry(entry);
        entry = next;
    }
}

// @address 0x2f8fd8 sound_xaudio_xenon_compute_biquad_coefficients
extern "C" void sound_xaudio_xenon_compute_biquad_coefficients(
    float *coefficients, float a_zero, float a_one, float a_two, float b_zero, float b_one, float b_two)
{
    float scale = 1.0f / a_zero;
    coefficients[4] = scale * b_zero;
    *coefficients = scale * a_one;
    coefficients[1] = scale * a_two;
    coefficients[2] = scale * b_one;
    coefficients[3] = scale * b_two;
    XMemSet(&coefficients[5], 0, 3 * sizeof(float));
    XMemSet(&coefficients[9], 0, 2 * sizeof(float));
}

extern "C" {

// @address 0x59fa48 QueryPerformanceCounter

// @address 0x119a3f8 g_xaudio_voice_frame_timing_stats
s_xaudio_voice_frame_timing_stats g_xaudio_voice_frame_timing_stats;
}

// @address 0x2fa078 ?Process@c_xaudio_voice_effect@@UAAJPAUIXAudioFrameBuffer@@0@Z
long STDMETHODCALLTYPE c_xaudio_voice_effect::Process(IXAudioFrameBuffer *input, IXAudioFrameBuffer *output)
{
    long result;
    __int64 start_time;
    __int64 end_time;
    XAUDIOFRAMEBUFDATA data;
    bool changed;
    __int64 elapsed;
    if (input == 0 || output != 0)
    {
        result = E_INVALIDARG;
    }
    else
    {
        result = XAudioFrameBuffer_GetProcessingData(input, &data);
        if (result >= 0 && (data.Format.SampleType != 0 || data.Format.SampleRate != 48000))
            result = E_INVALIDARG;
    }
    changed = parameters_changed;
    if (changed)
    {
        RtlEnterCriticalSection(&lock);
        params1 = params2;
        parameters_changed = false;
        RtlLeaveCriticalSection(&lock);
    }
    if (g_xaudio_voice_frame_timing_enabled != 0)
    {
        QueryPerformanceCounter((LARGE_INTEGER *)&start_time);
        process_samples((float *)data.pSampleBuffer, data.Format.ChannelCount, &params1, changed);
        QueryPerformanceCounter((LARGE_INTEGER *)&end_time);
        elapsed = end_time - start_time;
        __lwsync();
        _InterlockedExchangeAdd64(&g_xaudio_voice_frame_timing_stats.total_time, elapsed);
        _InterlockedIncrement(&g_xaudio_voice_frame_timing_stats.frame_count);
        __lwsync();
        return result;
    }
    process_samples((float *)data.pSampleBuffer, data.Format.ChannelCount, &params1, changed);
    return result;
}

// Shaper-curve unity and zero. These objects pin the unit's data layout:
// later data addresses referenced by 0x287cf8 move when they are removed,
// so the literals cannot replace them.
const float k_shaper_curve_unity = 1.0f;
const float k_shaper_curve_zero = 0.0f;

// @address 0x2f9ab8 ?apply_shaper_curve@c_xaudio_reverb_voice_effect@@QAAXPAM0PAUs_parameters@@@Z
void c_xaudio_reverb_voice_effect::apply_shaper_curve(float *input, float *output, s_parameters *params)
{
    float one = k_shaper_curve_unity;
    float zero = k_shaper_curve_zero;
    for (int i = 0; i < 256; i++) {
        float x = (float)fabs(*input);
        float y = x;
        if (x > params->shaper[0]) {
            float x2 = x * x;
            y = curve_coefficients[0] * (x2 * x2) + curve_coefficients[1] * (x2 * x) + curve_coefficients[2] * x2
                + curve_coefficients[3] * x;
        }
        if (params->shaper[4] < y) {
            y = params->shaper[4];
        }
        float r = (params->shaper[3] + one) * y;
        if (x != zero) {
            r = r / x;
        }
        *output = r;
        input++;
        output++;
    }
}

// @address 0x2f9d08 ?compute_shaper_curve@c_xaudio_reverb_voice_effect@@QAAXMMM@Z
void c_xaudio_reverb_voice_effect::compute_shaper_curve(float x, float y, float z)
{
    float xz = x * z;
    float z2 = z * z;
    float yz = y * z;
    float xy = x * y;
    float x2 = x * x;
    float xz8 = xz * 8.0f;
    float z2_6 = z2 * 6.0f;
    float xz2_8 = xz * z * 8.0f;
    float x2y = 0.0f;
    float twice_y = y * 2.0f;
    float x3 = x2 * x;
    x2y = x2 * y;
    float denominator = x3 * x - (x3 * 3.0f + xz8)
        - xz2_8 - x3 * z * 3.0f
        + ((z * 2.0f + 11.0f) * z + 2.0f) * x * x + z2_6;

    curve_coefficients[0] = (y * 3.0f + (yz * 3.0f + (x * 4.0f - z * 3.0f - 3.0f)
        - xy * 4.0f)) / denominator * -0.5f;
    curve_coefficients[1] = (x2 * 3.0f + (yz * 2.0f + (yz * z * 2.0f - z2 * 2.0f)
        - z * 2.0f - x2y * 3.0f) + twice_y - 2.0f) / denominator;
    curve_coefficients[2] = (x * 9.0f + (yz * 8.0f + (xz * 9.0f
        + ((y * 8.0f - 8.0f) * z * z - xy * z * 9.0f) - z * 8.0f)
        - xy * 9.0f) - 8.0f + y * 8.0f) / denominator * x * -0.5f;
    curve_coefficients[3] = ((z * 2.0f + 2.0f) * x * x * y * z
        + ((twice_y + x2y) * x * x + (x2 * z * 9.0f - xz2_8)
        - x3 * y * 3.0f) - x3 * y * z * 3.0f
        - xz8 + z2_6) / denominator;
}

// @address 0x2f9f38 ?GetParam@c_xaudio_voice_effect@@UAAJEEPATXAUDIOFXPARAM@@@Z
long STDMETHODCALLTYPE c_xaudio_voice_effect::GetParam(unsigned char parameter_id, unsigned char parameter_type, XAUDIOFXPARAM *parameter)
{
    long result = 0;
    void *buffer;
    if (parameter == 0)
        result = E_INVALIDARG;
    else if (parameter_type != XAUDIOFXPARAMTYPE_DATA)
        result = E_INVALIDARG;
    else if (parameter->Data.BufferSize != sizeof(s_parameters))
        result = E_INVALIDARG;
    else if ((buffer = parameter->Data.pBuffer) == 0)
        result = E_INVALIDARG;
    else
    {
        RtlEnterCriticalSection(&lock);
        *(s_parameters *)buffer = params2;
        RtlLeaveCriticalSection(&lock);
    }
    return result;
}

// @address 0x2f9fb8 ?SetParam@c_xaudio_voice_effect@@UAAJEEPBTXAUDIOFXPARAM@@@Z
long STDMETHODCALLTYPE c_xaudio_voice_effect::SetParam(unsigned char parameter_id, unsigned char parameter_type, const XAUDIOFXPARAM *parameter)
{
    long result = 0;
    const void *buffer;
    if (parameter == 0)
        result = E_INVALIDARG;
    else if (parameter_type != XAUDIOFXPARAMTYPE_DATA)
        result = E_INVALIDARG;
    else if (parameter->Data.BufferSize != sizeof(s_parameters))
        result = E_INVALIDARG;
    else if ((buffer = parameter->Data.pBuffer) == 0)
        result = E_INVALIDARG;
    else
    {
        RtlEnterCriticalSection(&lock);
        copy_parameters(&params2, buffer);
        parameters_changed = true;
        RtlLeaveCriticalSection(&lock);
    }
    return result;
}

static __forceinline void sound_xaudio_xenon_initialize_packet_completion_queue(long starting_entry_limit)
{
    g_xaudio_packet_completion_queue.pending.Alignment = 0;
    g_xaudio_packet_completion_queue.free.Alignment = 0;
    for (unsigned long index = 0; index < 392; ++index)
        g_xaudio_packet_completion_queue.entries[index].link.Next = 0;
    g_xaudio_packet_completion_queue.initialized = true;
    g_xaudio_packet_completion_queue.entry_limit = starting_entry_limit;
    if (g_xaudio_packet_completion_queue.entry_limit != 0)
    {
        long last_entry_index = g_xaudio_packet_completion_queue.entry_limit - 1;
        for (long index = last_entry_index; index >= 0; --index)
        {
            __lwsync();
            InterlockedPushEntrySList(&g_xaudio_packet_completion_queue.free, &g_xaudio_packet_completion_queue.entries[index].link);
        }
    }
}

extern "C" {
// @address 0x744ba0 g_xaudio_master_channel_maps
// @address 0x744bc8 g_xaudio_reverb_channel_maps
// @address 0x744bf0 g_xaudio_source_channel_maps
// @address 0x744c18 g_xaudio_environment_channel_maps
static XAUDIOCHANNELMAP g_xaudio_master_channel_maps[5] = {
    {6, k_xaudio_master_entries_6}, {12, k_xaudio_master_entries_12}, {24, k_xaudio_master24_reverb4_entries}, {36, k_xaudio_master36_reverb6_entries}, {0, 0}
};
static XAUDIOCHANNELMAP g_xaudio_reverb_channel_maps[5] = {
    {1, k_xaudio_reverb_entries_1}, {2, k_xaudio_reverb_entries_2}, {4, k_xaudio_master24_reverb4_entries}, {6, k_xaudio_master36_reverb6_entries}, {0, 0}
};
static XAUDIOCHANNELMAP g_xaudio_source_channel_maps[5] = {
    {4, k_xaudio_source_entries_4}, {8, k_xaudio_source_entries_8}, {16, k_xaudio_source_entries_16}, {24, k_xaudio_source_entries_24}, {0, 0}
};
static XAUDIOCHANNELMAP g_xaudio_environment_channel_maps[5] = {
    {1, k_xaudio_environment_entries_1}, {2, k_xaudio_environment_entries_2}, {4, k_xaudio_environment_entries_4}, {6, k_xaudio_environment_entries_6}, {0, 0}
};
}
extern "C" {
// @address 0x7446c8 k_xaudio_master_entries_6
// @address 0x7446f8 k_xaudio_master_entries_12
// @address 0x744758 k_xaudio_master24_reverb4_entries
// @address 0x744818 k_xaudio_master36_reverb6_entries
// @address 0x744938 k_xaudio_reverb_entries_1
// @address 0x744940 k_xaudio_reverb_entries_2
// @address 0x7449a0 k_xaudio_source_entries_4
// @address 0x7449c0 k_xaudio_source_entries_8
// @address 0x744a00 k_xaudio_source_entries_16
// @address 0x744a80 k_xaudio_source_entries_24
// @address 0x8c4278 k_xaudio_environment_entries_1
XAUDIOCHANNELMAPENTRY k_xaudio_environment_entries_1[1];
// @address 0x744b40 k_xaudio_environment_entries_2
// @address 0x744b50 k_xaudio_environment_entries_4
// @address 0x744b70 k_xaudio_environment_entries_6
}
#if !defined(_XBOX) || defined(__clang__)
// Input/output channel pairs and default gains used by the voice channel maps.
extern "C" {
XAUDIOCHANNELMAPENTRY k_xaudio_master_entries_6[6] = {
    {0, 1, 0.70710677f}, {0, 0, 0.70710677f}, {0, 2, 0.0f}, {0, 3, 0.0f}, {0, 5, 0.0f}, {0, 4, 0.0f}
};
XAUDIOCHANNELMAPENTRY k_xaudio_master_entries_12[12] = {
    {0, 1, 0.0f}, {1, 1, 1.0f}, {0, 0, 1.0f}, {1, 0, 0.0f}, {0, 2, 0.0f}, {1, 2, 0.0f},
    {0, 3, 0.0f}, {1, 3, 0.0f}, {0, 5, 0.0f}, {1, 5, 0.0f}, {0, 4, 0.0f}, {1, 4, 0.0f}
};
XAUDIOCHANNELMAPENTRY k_xaudio_master24_reverb4_entries[24] = {
    {0, 1, 0.0f}, {1, 1, 1.0f}, {2, 1, 0.0f}, {3, 1, 0.0f},
    {0, 0, 1.0f}, {1, 0, 0.0f}, {2, 0, 0.0f}, {3, 0, 0.0f},
    {0, 2, 0.0f}, {1, 2, 0.0f}, {2, 2, 0.0f}, {3, 2, 0.0f},
    {0, 3, 0.0f}, {1, 3, 0.0f}, {2, 3, 0.0f}, {3, 3, 0.0f},
    {0, 5, 0.0f}, {1, 5, 0.0f}, {2, 5, 0.0f}, {3, 5, 1.0f},
    {0, 4, 0.0f}, {1, 4, 0.0f}, {2, 4, 1.0f}, {3, 4, 0.0f}
};
XAUDIOCHANNELMAPENTRY k_xaudio_master36_reverb6_entries[36] = {
    {0, 1, 0.0f}, {1, 1, 1.0f}, {2, 1, 0.0f}, {3, 1, 0.0f}, {4, 1, 0.0f}, {5, 1, 0.0f},
    {0, 0, 1.0f}, {1, 0, 0.0f}, {2, 0, 0.0f}, {3, 0, 0.0f}, {4, 0, 0.0f}, {5, 0, 0.0f},
    {0, 2, 0.0f}, {1, 2, 0.0f}, {2, 2, 0.0f}, {3, 2, 0.0f}, {4, 2, 1.0f}, {5, 2, 0.0f},
    {0, 3, 0.0f}, {1, 3, 0.0f}, {2, 3, 0.0f}, {3, 3, 0.0f}, {4, 3, 0.0f}, {5, 3, 1.0f},
    {0, 5, 0.0f}, {1, 5, 0.0f}, {2, 5, 0.0f}, {3, 5, 1.0f}, {4, 5, 0.0f}, {5, 5, 0.0f},
    {0, 4, 0.0f}, {1, 4, 0.0f}, {2, 4, 1.0f}, {3, 4, 0.0f}, {4, 4, 0.0f}, {5, 4, 0.0f}
};
XAUDIOCHANNELMAPENTRY k_xaudio_reverb_entries_1[1] = {
    {0, 0, 1.0f}
};
XAUDIOCHANNELMAPENTRY k_xaudio_reverb_entries_2[2] = {
    {0, 0, 1.0f}, {1, 0, 1.0f}
};
XAUDIOCHANNELMAPENTRY k_xaudio_source_entries_4[4] = {
    {0, 0, 0.0f}, {0, 1, 0.0f}, {0, 2, 0.0f}, {0, 3, 0.0f}
};
XAUDIOCHANNELMAPENTRY k_xaudio_source_entries_8[8] = {
    {0, 0, 0.0f}, {1, 0, 0.0f}, {0, 1, 0.0f}, {1, 1, 0.0f}, {0, 2, 0.0f}, {1, 2, 0.0f}, {0, 3, 0.0f}, {1, 3, 0.0f}
};
XAUDIOCHANNELMAPENTRY k_xaudio_source_entries_16[16] = {
    {0, 0, 0.0f}, {1, 0, 0.0f}, {2, 0, 0.0f}, {3, 0, 0.0f},
    {0, 1, 0.0f}, {1, 1, 0.0f}, {2, 1, 0.0f}, {3, 1, 0.0f},
    {0, 2, 0.0f}, {1, 2, 0.0f}, {2, 2, 0.0f}, {3, 2, 0.0f},
    {0, 3, 0.0f}, {1, 3, 0.0f}, {2, 3, 0.0f}, {3, 3, 0.0f}
};
XAUDIOCHANNELMAPENTRY k_xaudio_source_entries_24[24] = {
    {0, 0, 0.0f}, {1, 0, 0.0f}, {2, 0, 0.0f}, {3, 0, 0.0f}, {4, 0, 0.0f}, {5, 0, 0.0f},
    {0, 1, 0.0f}, {1, 1, 0.0f}, {2, 1, 0.0f}, {3, 1, 0.0f}, {4, 1, 0.0f}, {5, 1, 0.0f},
    {0, 2, 0.0f}, {1, 2, 0.0f}, {2, 2, 0.0f}, {3, 2, 0.0f}, {4, 2, 0.0f}, {5, 2, 0.0f},
    {0, 3, 0.0f}, {1, 3, 0.0f}, {2, 3, 0.0f}, {3, 3, 0.0f}, {4, 3, 0.0f}, {5, 3, 0.0f}
};
XAUDIOCHANNELMAPENTRY k_xaudio_environment_entries_2[2] = {
    {0, 0, 0.0f}, {1, 0, 0.0f}
};
XAUDIOCHANNELMAPENTRY k_xaudio_environment_entries_4[4] = {
    {0, 0, 0.0f}, {1, 0, 0.0f}, {2, 0, 0.0f}, {3, 0, 0.0f}
};
XAUDIOCHANNELMAPENTRY k_xaudio_environment_entries_6[6] = {
    {0, 0, 0.0f}, {1, 0, 0.0f}, {2, 0, 0.0f}, {3, 0, 0.0f}, {4, 0, 0.0f}, {5, 0, 0.0f}
};
}
#endif

const char *k_waves_library_names[4] =
{
    "game:\\WavesLibDLL.dll",
    "game:\\WaveShell-Xbox.dll",
    "game:\\waves\\l360.dll",
    "game:\\waves\\q10.dll",
};

#if !defined(_XBOX) || defined(__clang__)
// Expected major/minor version of each waves library, checked after it loads.
const s_waves_library_version k_waves_library_versions[4] =
{
    { 0x00000301, 0x6c770004 },
    { 0x00000301, 0x73780004 },
    { 0x00010301, 0x534c0005 },
    { 0x00000301, 0x6e510004 },
};
#endif

s_sound_xaudio_packet_completion_queue g_xaudio_packet_completion_queue = { { 0 }, { 0 }, { 0 }, false, 0, "audio_queue" };

static __forceinline XAUDIOVOICEOUTPUTENTRY sound_xaudio_xenon_make_voice_output_entry(IXAudioVoice *voice, XAUDIOCHANNELMAP *map)
{
    XAUDIOVOICEOUTPUTENTRY entry;
    entry.pDestVoice = voice;
    entry.pChannelMap = map;
    return entry;
}
// @address 0x288870 sound_xaudio_xenon_initialize_voice_routing
extern "C" void sound_xaudio_xenon_initialize_voice_routing(c_xenon_audio_output_manager *, c_xenon_audio_reverb_manager *, c_xenon_audio_dsp_manager *)
{
    IXAudioSubmixVoice *reverb0 = g_xaudio_reverb_manager.voices[0];
    IXAudioSubmixVoice *reverb1 = g_xaudio_reverb_manager.voices[1];
    IXAudioSubmixVoice *source = g_xaudio_dsp_manager.m_source_voice;
    IXAudioSubmixVoice *auxiliary = g_xaudio_dsp_manager.m_auxiliary_voice;
    IXAudioSubmixVoice *occlusion = g_xaudio_dsp_manager.m_occlusion_voice;
    IXAudioSubmixVoice *environment = g_xaudio_dsp_manager.m_environment_voice;
    IXAudioVoice *master = g_xaudio_dsp_manager.m_remote_voice_output_destination;
    for (long channel = 0; channel < 5; ++channel)
    {
        c_static_array<XAUDIOVOICEOUTPUTENTRY, 7> &outputs = g_xaudio_output_manager.m_source_outputs[channel];
        outputs[0] = sound_xaudio_xenon_make_voice_output_entry(master, &g_xaudio_master_channel_maps[channel]);
        outputs[1] = sound_xaudio_xenon_make_voice_output_entry(reverb0, &g_xaudio_reverb_channel_maps[channel]);
        outputs[2] = sound_xaudio_xenon_make_voice_output_entry(reverb1, &g_xaudio_reverb_channel_maps[channel]);
        outputs[3] = sound_xaudio_xenon_make_voice_output_entry(source, &g_xaudio_source_channel_maps[channel]);
        outputs[4] = sound_xaudio_xenon_make_voice_output_entry(auxiliary, &g_xaudio_master_channel_maps[channel]);
        outputs[5] = sound_xaudio_xenon_make_voice_output_entry(occlusion, &g_xaudio_reverb_channel_maps[channel]);
        outputs[6] = sound_xaudio_xenon_make_voice_output_entry(environment, &g_xaudio_environment_channel_maps[channel]);
    }
}

// @address 0x1fac18 sound_xaudio_xenon_initialize
extern "C" bool sound_xaudio_xenon_initialize(const s_sound_preferences *preferences)
{
    {
        s_system_memory_information memory_before;
        system_memory_information_get(&memory_before);
    }

    HRESULT hr;
    {
        XAUDIOENGINEINIT engine_init;
        memset(&engine_init, 0, sizeof(engine_init));
        engine_init.pEffectTable = &XAudioDefaultEffectTable;
        engine_init.MaxVoiceChannelCount = 4;
        engine_init.SubmixStageCount = 5;
        engine_init.ThreadUsage = 48;
        hr = XAudioInitialize(&engine_init);
    }
    if (SUCCEEDED(hr))
    {
        XAudioSetDebugFlags(XAUDIODEBUGFLAGS_MASK);
        XAudioSetDebugPrintLevel(XAUDIODEBUGLEVEL_ERROR);
        sound_xaudio_xenon_initialize_dsp_manager(&g_xaudio_dsp_manager);
        g_xaudio_reverb_manager.initialize(&g_xaudio_dsp_manager);
        sound_xaudio_xenon_create_occlusion_voice(&g_xaudio_dsp_manager, &g_xaudio_reverb_manager);
        sound_xaudio_xenon_initialize_voice_routing(&g_xaudio_output_manager, &g_xaudio_reverb_manager, &g_xaudio_dsp_manager);
        g_xaudio_source_manager.initialize(&g_xaudio_dsp_manager, preferences, &g_xaudio_output_manager, platform_sound_xaudio_xenon_source_callback);
        g_xaudio_voice_manager.initialize(&g_xaudio_source_manager, preferences->speed_of_sound);
    }
    sound_xaudio_xenon_initialize_packet_completion_queue(392);

    {
        s_system_memory_information memory_after;
        system_memory_information_get(&memory_after);
    }
    return true;
}

namespace waves_ext {
// @address 0x8c43ba g_xaudio_waves_procedures_imported
}

// @address 0x2f7a00 sound_xaudio_xenon_import_waves_procedures
extern "C" long sound_xaudio_xenon_import_waves_procedures()
{
    using namespace waves_ext;
    void **imports[19] = {
        &g_xaudio_waves_procedures_imported.procedures[3],
        &g_xaudio_waves_procedures_imported.procedures[1],
        &g_xaudio_waves_procedures_imported.procedures[2],
        &g_xaudio_waves_procedures_imported.procedures[4],
        &g_xaudio_waves_procedures_imported.procedures[5],
        &g_xaudio_waves_procedures_imported.procedures[6],
        &g_xaudio_waves_procedures_imported.procedures[7],
        &g_xaudio_waves_procedures_imported.procedures[13],
        &g_xaudio_waves_procedures_imported.procedures[14],
        &g_xaudio_waves_procedures_imported.procedures[15],
        &g_xaudio_waves_procedures_imported.procedures[16],
        &g_xaudio_waves_procedures_imported.procedures[17],
        &g_xaudio_waves_procedures_imported.procedures[18],
        &g_xaudio_waves_procedures_imported.procedures[0],
        &g_xaudio_waves_procedures_imported.procedures[8],
        &g_xaudio_waves_procedures_imported.procedures[9],
        &g_xaudio_waves_procedures_imported.procedures[10],
        &g_xaudio_waves_procedures_imported.procedures[11],
        &g_xaudio_waves_procedures_imported.procedures[12]
    };
    HMODULE library = (HMODULE)sound_xaudio_xenon_load_waves_library(1);
    if (library == 0)
        main_halt_and_display_errors();
    for (unsigned long index = 0; index < 19; index++)
    {
        *imports[index] = (void *)GetProcAddress(library, (LPCSTR)(uintptr_t)(index + 1));
        if (imports[index] == 0)
            main_halt_and_display_errors();
    }
    g_xaudio_waves_procedures_imported.initialized = 1;
    return 1;
}

// @split_bss
// @sparse_bss 0x828c43b8..0x828c4420
// @sparse_bss_padding __wpl_sxx_0_4436,__wpl_sxx_1_1801,__wpl_sxx_3_1723,__wpl_sxx_4_956,__wpl_sxx_5_1819,__wpl_sxx_6_120,__wpl_sxx_8_3731,__wpl_sparse_pool_430,__wpl_sparse_pool_397,__wpl_sparse_pool_99,__wpl_sparse_pool_402,__wpl_sparse_pool_163,__wpl_sparse_pool_22,__wpl_sparse_pool_93,__wpl_sparse_pool_359,__wpl_sparse_pool_303,__wpl_sparse_pool_459,__wpl_sparse_pool_508,__wpl_sparse_pool_377
// @bss_escape g_xaudio_waves_set_memory_procedures
// @address 0x8c43ba g_xaudio_waves_procedures_imported
// @address 0x8c43bb g_xaudio_waves_memory_procedures_set
// @address 0x8c43dc g_xaudio_waves_set_memory_procedures
#if defined(_MSC_VER) && !defined(__clang__)
static unsigned char g_xaudio_waves_procedures_imported;
static unsigned char g_xaudio_waves_memory_procedures_set;
static t_waves_library_set_memory_procedures g_xaudio_waves_set_memory_procedures;
#else
// Other compilers: these cells are the view's own fields (bss 0x828c43ba, 0x828c43bb and slot 0 at 0x828c43dc on the
// Xbox), and the view has storage of its own here, so the names are bound to its fields.
#define g_xaudio_waves_procedures_imported (sound_xaudio_waves_import_view().initialized)
#define g_xaudio_waves_memory_procedures_set (sound_xaudio_waves_import_view().memory_procedures_set)
#define g_xaudio_waves_set_memory_procedures (reinterpret_cast<t_waves_library_set_memory_procedures &>(sound_xaudio_waves_procedure_slot(0)))
#endif

static __forceinline XAUDIOFXID sound_xaudio_xenon_register_effect(LPFNXAUDIOQUERYFXSIZE query_size, LPFNXAUDIOCREATEFX create)
{
    XAUDIOFXTABLEENTRY effect_entry = { 0, query_size, create };
    XAUDIOFXREGISTER effect_register = { 1, &effect_entry };
    XAudioRegisterEffects(&effect_register);
    return effect_entry.EffectId;
}
// @address 0x58c68 k_xaudio_environment_values2_4
// @address 0x58c6c k_xaudio_environment_values2_3
// @address 0x58c74 k_xaudio_environment_values2_0
// @address 0x58c78 k_xaudio_environment_scalar1
#if !defined(_XBOX) || defined(__clang__)
// Normalize level and shaper curve of the environment effect.
const float k_xaudio_environment_values2_4 = 1.26f;
const float k_xaudio_environment_values2_3 = 0.88f;
const float k_xaudio_environment_values2_0 = -0.33f;
const float k_xaudio_environment_scalar1 = 0.57f;
#endif

// Canonical home is tu/blamlibxbox/voice_ring_buffer_effect.cpp (cf. op-sym-5389's
// remote twin); defined here only because that TU is leased elsewhere. Do not
// define a second copy: this ICF-folds into 0x42d298 (`li r3,0; blr`), so it takes
// no @address and must stay a bare `return 0`.
long c_voice_headset_ring_buffer_effect::Initialize(const XAUDIOFXINIT *init, IXAudioBatchAllocator *allocator)
{
    return 0;
}

// @address 0x287cf8 sound_xaudio_xenon_initialize_dsp_manager
extern "C" void sound_xaudio_xenon_initialize_dsp_manager(c_xenon_audio_dsp_manager *dsp_manager)
{
    sound_xaudio_xenon_import_waves_procedures();
    if (!g_xaudio_waves_procedures_imported)
        main_halt_and_display_errors();
    for (long library_index = 0; library_index < 4; library_index++)
        sound_xaudio_xenon_load_waves_library(library_index);
    g_xaudio_waves_memory_procedures_set = 1;
    g_xaudio_waves_set_memory_procedures(sound_xaudio_xenon_waves_allocate, sound_xaudio_xenon_waves_free, sound_xaudio_xenon_waves_reallocate);

    XAUDIOFXTABLEENTRY effect_table[k_xaudio_dsp_effect_count] = {
        { 0, c_voice_headset_ring_buffer_effect::xaudio_fx_query_size, c_voice_headset_ring_buffer_effect::xaudio_fx_create_mixbin_effect },
        { 0, QuerySizeStateVariableFilterEffect, state_variable_filter_create_surround_effect },
        { 0, QuerySizeStateVariableFilterEffect, state_variable_filter_create_eq1m_effect },
        { 0, QuerySizeStateVariableFilterEffect, state_variable_filter_create_eq1m_effect },
        { 0, reverb_effect_get_size, reverb_effect_create },
        { 0, QuerySizeStateVariableFilterEffect, state_variable_filter_create_eq4m_effect }
    };
    XAUDIOFXREGISTER effect_register = { k_xaudio_dsp_effect_count, effect_table };
    XAudioRegisterEffects(&effect_register);

    g_xaudio_dsp_manager.m_effect_chain.EffectCount = k_xaudio_dsp_effect_count;
    g_xaudio_dsp_manager.m_effect_chain.papEffects = g_xaudio_dsp_manager.m_effect_pointers;
    for (long effect_index = 0; effect_index < k_xaudio_dsp_effect_count; effect_index++)
    {
        g_xaudio_dsp_manager.m_effect_pointers[effect_index] = &g_xaudio_dsp_manager.m_effects[effect_index];
        g_xaudio_dsp_manager.m_effects[effect_index].EffectId = effect_table[effect_index].EffectId;
    }

    g_xaudio_dsp_manager.m_headset_ring_buffer.initialize("headset output ring buffer", 4);
    g_xaudio_dsp_manager.m_ring_buffer.initialize("remote voice output ring buffer", 16);

    g_xaudio_dsp_manager.m_output_effect.EffectId = sound_xaudio_xenon_register_effect(
        c_voice_headset_ring_buffer_effect::xaudio_fx_query_size, c_voice_headset_ring_buffer_effect::xaudio_fx_create_output_effect);
    g_xaudio_dsp_manager.m_output_effect.pContext = &g_xaudio_dsp_manager.m_headset_ring_buffer;
    g_xaudio_dsp_manager.m_output_effect_pointer = &g_xaudio_dsp_manager.m_output_effect;
    g_xaudio_dsp_manager.m_output_effect_chain.EffectCount = 1;
    g_xaudio_dsp_manager.m_output_effect_chain.papEffects = &g_xaudio_dsp_manager.m_output_effect_pointer;

    g_xaudio_dsp_manager.m_remote_talker_effect.EffectId = sound_xaudio_xenon_register_effect(
        c_remote_voice_ring_buffer_effect::xaudio_fx_query_size, c_remote_voice_ring_buffer_effect::xaudio_fx_create_mixbin_effect);
    g_xaudio_dsp_manager.m_remote_talker_effect.pContext = &g_xaudio_dsp_manager.m_ring_buffer;
    g_xaudio_dsp_manager.m_remote_talker_effect_pointer = &g_xaudio_dsp_manager.m_remote_talker_effect;
    g_xaudio_dsp_manager.m_remote_talker_effect_chain.EffectCount = 1;
    g_xaudio_dsp_manager.m_remote_talker_effect_chain.papEffects = &g_xaudio_dsp_manager.m_remote_talker_effect_pointer;

    g_xaudio_dsp_manager.m_ring_buffer_effect_id = sound_xaudio_xenon_register_effect(
        c_remote_voice_ring_buffer_effect::xaudio_fx_query_size, c_remote_voice_ring_buffer_effect::xaudio_fx_create_output_effect);

    g_xaudio_dsp_manager.m_remote_voice_output_destination = NULL;
    {
        XAUDIOFXINIT master_effect = {
            g_xaudio_dsp_manager.m_effects[_xaudio_dsp_effect_master_filter].EffectId, (void *)sound_xaudio_xenon_master_effect_name};
        LPCXAUDIOFXINIT effects[] = { &master_effect };
        XAUDIOVOICEFXCHAIN effect_chain = { 1, effects };
        XAUDIOCHANNELMAPENTRY channel_entries[] = {
            { 0, 0, 1.0f },
            { 1, 1, 1.0f },
            { 2, 2, 1.0f },
            { 3, 3, 1.0f },
            { 4, 4, 1.0f },
            { 5, 5, 1.0f }
        };
        XAUDIOCHANNELMAP channel_map;
        XAUDIOVOICEOUTPUTENTRY output_entry;
        XAUDIOVOICEOUTPUT voice_output = { 1, &output_entry };
        output_entry.pDestVoice = NULL;
        output_entry.pChannelMap = &channel_map;
        channel_map.EntryCount = 6;
        channel_map.paEntries = channel_entries;
        XAUDIOSUBMIXVOICEINIT voice_init = { 0 };
        voice_init.MaxOutputVoiceCount = 1;
        voice_init.MaxChannelMapEntryCount = 6;
        voice_init.Format.SampleType = XAUDIOSAMPLETYPE_FLOAT;
        voice_init.pEffectChain = &effect_chain;
        voice_init.Format.SampleRate = 48000;
        voice_init.Format.ChannelCount = 6;
        voice_init.SubmixStage = 3;
        voice_init.pVoiceOutput = &voice_output;
        XAudioCreateSubmixVoice(&voice_init, &g_xaudio_dsp_manager.m_remote_voice_output_destination);
    }

    g_xaudio_dsp_manager.m_auxiliary_voice = NULL;
    sound_xaudio_xenon_create_obstruction_voices(&g_xaudio_dsp_manager.m_auxiliary_voice, g_xaudio_dsp_manager.m_obstruction_voices,
        g_xaudio_dsp_manager.m_remote_voice_output_destination, g_xaudio_dsp_manager.m_effects[_xaudio_dsp_effect_obstruction].EffectId,
        sound_xaudio_xenon_obstruction_effect_name, 1);

    {
        XAUDIOFXINIT headset_effect = {
            g_xaudio_dsp_manager.m_effects[_xaudio_dsp_effect_headset_mixbin].EffectId, &g_xaudio_dsp_manager.m_headset_ring_buffer};
        LPCXAUDIOFXINIT effects[] = { &headset_effect };
        XAUDIOVOICEFXCHAIN effect_chain = { 1, effects };
        XAUDIOCHANNELMAPENTRY channel_entries[] = {
            { 0, 0, 0.0f },
            { 1, 1, 0.0f },
            { 2, 2, 0.0f },
            { 3, 3, 0.0f }
        };
        XAUDIOCHANNELMAP channel_map;
        XAUDIOVOICEOUTPUTENTRY output_entry;
        XAUDIOVOICEOUTPUT voice_output = { 1, &output_entry };
        output_entry.pDestVoice = g_xaudio_dsp_manager.m_remote_voice_output_destination;
        output_entry.pChannelMap = &channel_map;
        channel_map.EntryCount = 4;
        channel_map.paEntries = channel_entries;
        XAUDIOSUBMIXVOICEINIT voice_init = { 0 };
        voice_init.MaxChannelMapEntryCount = 4;
        voice_init.Format.SampleType = XAUDIOSAMPLETYPE_FLOAT;
        voice_init.Format.SampleRate = 48000;
        voice_init.Format.ChannelCount = 4;
        voice_init.SubmixStage = 1;
        voice_init.pEffectChain = &effect_chain;
        voice_init.pVoiceOutput = &voice_output;
        XAudioCreateSubmixVoice(&voice_init, &g_xaudio_dsp_manager.m_source_voice);
    }

    {
        XAUDIOFXINIT radio_effect = { g_xaudio_dsp_manager.m_effects[_xaudio_dsp_effect_radio].EffectId, (void *)sound_xaudio_xenon_radio_effect_name };
        LPCXAUDIOFXINIT effects[] = { &radio_effect };
        XAUDIOVOICEFXCHAIN effect_chain = { 1, effects };
        XAUDIOCHANNELMAPENTRY channel_entries[] = {
            { 0, 0, 0.5f },
            { 0, 1, 0.5f },
            { 0, 2, 1.0f }
        };
        XAUDIOCHANNELMAP channel_map;
        XAUDIOVOICEOUTPUTENTRY output_entry;
        XAUDIOVOICEOUTPUT voice_output = { 1, &output_entry };
        output_entry.pDestVoice = g_xaudio_dsp_manager.m_remote_voice_output_destination;
        output_entry.pChannelMap = &channel_map;
        channel_map.EntryCount = 3;
        channel_map.paEntries = channel_entries;
        XAUDIOSUBMIXVOICEINIT voice_init = { 0 };
        voice_init.MaxOutputVoiceCount = 1;
        voice_init.MaxChannelMapEntryCount = 3;
        voice_init.Format.SampleType = XAUDIOSAMPLETYPE_FLOAT;
        voice_init.Format.SampleRate = 48000;
        voice_init.Format.ChannelCount = 1;
        voice_init.pEffectChain = &effect_chain;
        voice_init.SubmixStage = 2;
        voice_init.pVoiceOutput = &voice_output;
        XAudioCreateSubmixVoice(&voice_init, &g_xaudio_dsp_manager.m_radio_voice);
    }

    {
        XAUDIOFXINIT environment_effect = { g_xaudio_dsp_manager.m_effects[_xaudio_dsp_effect_environment].EffectId, NULL };
        LPCXAUDIOFXINIT effects[] = { &environment_effect };
        XAUDIOVOICEFXCHAIN effect_chain = { 1, effects };
        XAUDIOCHANNELMAPENTRY channel_entries[] = {
            { 0, 0, 1.0f }
        };
        XAUDIOCHANNELMAP channel_map;
        XAUDIOVOICEOUTPUTENTRY output_entry;
        XAUDIOVOICEOUTPUT voice_output = { 1, &output_entry };
        output_entry.pDestVoice = g_xaudio_dsp_manager.m_radio_voice;
        output_entry.pChannelMap = &channel_map;
        channel_map.EntryCount = 1;
        channel_map.paEntries = channel_entries;
        XAUDIOSUBMIXVOICEINIT voice_init = { 0 };
        voice_init.MaxChannelMapEntryCount = 1;
        voice_init.Format.SampleType = XAUDIOSAMPLETYPE_FLOAT;
        voice_init.Format.SampleRate = 48000;
        voice_init.Format.ChannelCount = 1;
        voice_init.SubmixStage = 1;
        voice_init.pEffectChain = &effect_chain;
        voice_init.pVoiceOutput = &voice_output;
        XAudioCreateSubmixVoice(&voice_init, &g_xaudio_dsp_manager.m_environment_voice);
    s_parameters parameters;
    sound_xaudio_xenon_initialize_parameters(&parameters);
    parameters.bypass = false;
    parameters.output_gain = 0.6f;
    parameters.normalize_input = true;
    parameters.normalize_level = k_xaudio_environment_scalar1;
    parameters.shaper_enabled = true;
    parameters.shaper[0] = k_xaudio_environment_values2_0;
    parameters.shaper[1] = 0.54f;
    parameters.shaper[2] = 0.54f;
    parameters.shaper[3] = k_xaudio_environment_values2_3;
    parameters.shaper[4] = k_xaudio_environment_values2_4;
    parameters.low_pass_enabled = false;
    parameters.low_pass[0] = 0.3f;
    parameters.low_pass[1] = 1.4f;
    parameters.band_pass_enabled = false;
    parameters.band_pass[0] = 0.15f;
    parameters.band_pass[1] = 1.75f;
    parameters.band_pass[2] = 0.36f;

    XAUDIOFXPARAM parameter;
    parameter.Data.pBuffer = &parameters;
    parameter.Data.BufferSize = sizeof(parameters);
    g_xaudio_dsp_manager.m_environment_voice->SetEffectParam(0, 0, XAUDIOFXPARAMTYPE_DATA, &parameter);
    }
}

// @address 0x2f9260 ?process_samples@c_xaudio_reverb_voice_effect@@UAAXPAMJPAUs_parameters@@_N@Z
void c_xaudio_reverb_voice_effect::process_samples(float *samples, long channel_count, s_parameters *params, bool reset)
{
    float average[256];
    float shaped[256];
    float curve_input[256];

    if (reset)
    {
        XMemSet(history_a.samples, 0, sizeof(history_a.samples));
        sum_a = 0.0f;
        compute_shaper_curve(params->shaper[0], params->shaper[1], params->shaper[2]);

        float angle = params->low_pass[0] * 3.14159f;
        float resonance = params->low_pass[1];
        float sine = (float)sin(angle);
        float cosine = (float)cos(angle);
        float alpha = sine / (resonance * 2.0f);
        sound_xaudio_xenon_compute_biquad_coefficients(
            filter_a.coefficients, 1.0f + alpha, -2.0f * cosine, 1.0f - alpha, (1.0f - cosine) / 2.0f, 1.0f - cosine, (1.0f - cosine) / 2.0f);

        angle = params->band_pass[0] * 3.14159f;
        resonance = params->band_pass[1];
        sine = (float)sin(angle);
        cosine = (float)cos(angle);
        alpha = sine / (resonance * 2.0f);
        sound_xaudio_xenon_compute_biquad_coefficients(
            filter_b.coefficients, 1.0f + alpha, -2.0f * cosine, 1.0f - alpha, sine / 2.0f, 0.0f, -sine / 2.0f);
    }

    memset(average, 0, sizeof(average));
    for (long i = 0; i < 32; i++)
    {
        for (long j = 0; j < 8; j++)
        {
            float sample = fabsf(samples[i * 8 + j]);
            sum_a += sample;
            sum_a -= history_a.oldest();
            history_a.push(sample);
            average[i * 8 + j] = sum_a * 0.00390625f;
        }
    }

    if (!params->bypass)
    {
        if (params->shaper_enabled)
        {
            memset(curve_input, 0, sizeof(curve_input));
            for (long i = 0; i < 256; i++)
            {
                if (params->normalize_input)
                    curve_input[i] = params->normalize_level / (average[i] + 0.0001f) * samples[i];
                else
                    curve_input[i] = samples[i];
            }

            memset(shaped, 0, sizeof(shaped));
            apply_shaper_curve(curve_input, shaped, params);

            for (long i = 0; i < 256; i += 4)
            {
                for (long j = 0; j < 4; j++)
                {
                    samples[i + j] = shaped[i + j] * samples[i + j];
                    sum_b += fabsf(samples[i + j]);
                    sum_b -= history_b.oldest();
                    history_b.push(fabsf(samples[i + j]));

                    float level = sum_b * 0.00390625f;
                    float gain = 0.0f;
                    if (level != 0.0f)
                        gain = (average[i + j] - level) / level;
                    samples[i + j] = (gain + 1.0f) * samples[i + j];
                }
            }
        }

        for (long i = 0; i < 32; i++)
        {
            for (long j = 0; j < 8; j++)
                samples[i * 8 + j] = (params->output_gain + 1.0f) * samples[i * 8 + j];
        }
    }
}

unsigned char sound_xaudio_xenon_memory_procedures_set()
{
    return g_xaudio_waves_memory_procedures_set;
}
