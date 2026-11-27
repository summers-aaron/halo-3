#ifndef SOUND_SOUND_XAUDIO_XENON_H
#define SOUND_SOUND_XAUDIO_XENON_H

// Functions defined in tu/blamlibxbox/sound_xaudio_xenon.cpp.

#include <stddef.h>
#include "math/real_math_types.h"

struct s_platform_sound_listener_reverb;

struct platform_sound_listener_properties
{
    real_point3d position;
    real_vector3d forward;
    real_vector3d up;
    real_vector3d velocity;
    long reverb_count;
    const s_platform_sound_listener_reverb *reverbs;
};

typedef char platform_sound_listener_properties_size[
    (sizeof(void *) != 4 || sizeof(platform_sound_listener_properties) == 0x38) ? 1 : -1];
typedef char platform_sound_listener_properties_velocity_offset[
    (offsetof(platform_sound_listener_properties, velocity) == 0x24) ? 1 : -1];
typedef char platform_sound_listener_properties_reverbs_offset[
    (sizeof(void *) != 4 || offsetof(platform_sound_listener_properties, reverbs) == 0x34) ? 1 : -1];

void platform_sound_listener_set_properties(const platform_sound_listener_properties *properties);
void sound_xaudio_xenon_process_events(void);
unsigned char sound_xaudio_xenon_memory_procedures_set();

struct s_sound_preferences;

extern "C" void sound_xaudio_xenon_set_global_gain(float value);
extern "C" void sound_xaudio_xenon_noop_callback(void);
extern "C" void sound_xaudio_xenon_update_reverb_levels(float p0);
extern "C" bool sound_xaudio_xenon_initialize(const s_sound_preferences *preferences);

class c_xenon_audio_dsp_manager;
class c_xenon_audio_output_manager;
class c_xenon_audio_reverb_manager;
class c_xenon_audio_source_manager;
struct XAUDIOVOICEPACKETCALLBACK;

void platform_sound_xaudio_xenon_source_callback(const XAUDIOVOICEPACKETCALLBACK *callback);

extern "C" bool sound_xaudio_xenon_run_source_update_loop(void);
extern "C" void sound_xaudio_xenon_compute_biquad_coefficients(
    float *coefficients, float a_zero, float a_one, float a_two, float b_zero, float b_one, float b_two);
extern "C" void sound_xaudio_xenon_initialize_voice_routing(c_xenon_audio_output_manager *, c_xenon_audio_reverb_manager *, c_xenon_audio_dsp_manager *);
extern "C" void sound_xaudio_xenon_initialize_dsp_manager(c_xenon_audio_dsp_manager *dsp_manager);

// Data defined in tu/blamlibxbox/sound_xaudio_xenon.cpp that other units use.
extern c_xenon_audio_source_manager g_xaudio_source_manager;
extern c_xenon_audio_output_manager g_xaudio_output_manager;

class c_xenon_audio_voice_manager;

extern c_xenon_audio_reverb_manager g_xaudio_reverb_manager;
extern c_xenon_audio_voice_manager g_xaudio_voice_manager;
extern c_xenon_audio_dsp_manager g_xaudio_dsp_manager;


struct XAUDIOCHANNELMAPENTRY;

// Shared declarations used by sound_xaudio_xenon.cpp.
struct s_waves_library_version;
extern "C" const char *k_waves_library_names[];
extern "C" const s_waves_library_version k_waves_library_versions[];
// Frame count and total processing time of the voice effects, recorded while frame timing is enabled.
struct s_xaudio_voice_frame_timing_stats
{
    long frame_count;
    __int64 total_time;
};
extern "C" s_xaudio_voice_frame_timing_stats g_xaudio_voice_frame_timing_stats;
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_master_entries_6[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_master_entries_12[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_master24_reverb4_entries[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_master36_reverb6_entries[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_reverb_entries_1[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_reverb_entries_2[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_source_entries_4[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_source_entries_8[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_source_entries_16[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_source_entries_24[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_environment_entries_1[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_environment_entries_2[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_environment_entries_4[];
extern "C" XAUDIOCHANNELMAPENTRY k_xaudio_environment_entries_6[];
extern "C" const float k_xaudio_environment_values2_4, k_xaudio_environment_values2_3, k_xaudio_environment_values2_0, k_xaudio_environment_scalar1;

#endif // SOUND_SOUND_XAUDIO_XENON_H
