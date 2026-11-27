#ifndef SOUND_SOUND_XAUDIO_XENON_IMPORTS_H
#define SOUND_SOUND_XAUDIO_XENON_IMPORTS_H

// Library imports whose signatures differ from the public SDK views.
struct s_platform_sound_status;
struct sound_xaudio_waves_import_globals;
namespace waves_ext {
#if defined(_MSC_VER) && !defined(__clang__)
// match: 0x2f7a00: a view over the bss from 0x828c43ba, linked by C name to the byte there. It crosses globals
// of other units, so other compilers get a C++ name and storage of their own.
extern "C" sound_xaudio_waves_import_globals g_xaudio_waves_procedures_imported;
#else
extern sound_xaudio_waves_import_globals g_xaudio_waves_procedures_imported;
#endif
}

#if !defined(_MSC_VER) || defined(__clang__)
// Other compilers: the view has storage of its own (sound_xaudio_xenon.cpp), so the cells the Xbox lays over it are
// bound to its slots by name (sound_xaudio_xenon_import_waves_procedures fills them); the cells other units name are slots 0
// g_xaudio_waves_set_memory_procedures, 1 svf_new_filter_state_procedure, 3 g_voice_queue_get_effect_version, 6
// svf_set_frequency_proc, 7 svf_set_mode_proc, 16 svf_register_filter_state_proc, 18 g_state_variable_filter_proc.
void *&sound_xaudio_waves_procedure_slot(unsigned int index);
#endif

extern "C" {
void platform_sound_get_status(s_platform_sound_status *volatile incoming);
extern unsigned char g_xaudio_voice_frame_timing_enabled;
int sound_xaudio_waves_allocation_size(const void *ptr, unsigned long attrs);
}

#endif
