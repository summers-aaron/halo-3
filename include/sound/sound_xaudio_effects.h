#ifndef SOUND_SOUND_XAUDIO_EFFECTS_H
#define SOUND_SOUND_XAUDIO_EFFECTS_H

#include <xtl.h>
#include <xaudio.h>

extern "C" {
HRESULT QuerySizeStateVariableFilterEffect(LPCXAUDIOFXINIT init, LPDWORD effect_size);
HRESULT state_variable_filter_create_surround_effect(LPCXAUDIOFXINIT init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect);
HRESULT state_variable_filter_create_eq1m_effect(LPCXAUDIOFXINIT init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect);
HRESULT state_variable_filter_create_eq4m_effect(LPCXAUDIOFXINIT init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect);
HRESULT reverb_effect_get_size(LPCXAUDIOFXINIT init, LPDWORD effect_size);
HRESULT reverb_effect_create(LPCXAUDIOFXINIT init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect);
}

#endif // SOUND_SOUND_XAUDIO_EFFECTS_H
