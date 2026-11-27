#ifndef SOUND_XENON_DSP_STATE_VARIABLE_FILTER_EFFECT_H
#define SOUND_XENON_DSP_STATE_VARIABLE_FILTER_EFFECT_H

// Functions defined in tu/blamlibxbox/state_variable_filter_effect.cpp.

struct s_string
{
    int allocator;
    union {
        char buffer[16];
        char *pointer;
    } bx;
    unsigned int size;
    unsigned int capacity;
};



extern "C" void state_variable_filter_string_reset(s_string *string);
extern "C" s_string *state_variable_filter_string_assign_substring(s_string *string, const s_string *right, unsigned int offset, unsigned int count);
extern "C" s_string *state_variable_filter_string_assign_buffer(s_string *string, const char *pointer, unsigned int count);
extern "C" s_string *state_variable_filter_string_erase(s_string *string, unsigned int offset, unsigned int count);
extern "C" unsigned char state_variable_filter_string_grow(s_string *string, unsigned int new_size, unsigned char trim);

#include <xaudfx.h>
#include "sound/sound_xaudio_effects.h"
#include <stddef.h>

class CStateVariableFilterEffect : public IXAudioEffect
{
public:
    virtual unsigned long AddRef();
    virtual unsigned long Release();
    virtual long GetInfo(XAUDIOFXINFO *info);
    virtual long GetParam(unsigned char id, unsigned char type, XAUDIOFXPARAM *out);
    virtual long SetParam(unsigned char id, unsigned char type, const XAUDIOFXPARAM *param);
    virtual long GetContext(void **context);
    virtual long Process(IXAudioFrameBuffer *input, IXAudioFrameBuffer *output);

    void *m_context;
    long m_reference_count;
    long channel_count;
    void *filter_state;
    s_string name;
    unsigned long mode;
    double frequency;
};

typedef char state_variable_filter_effect_size[(sizeof(void *) != 4 || ((sizeof(CStateVariableFilterEffect) == 64))) ? 1 : -1];
typedef char state_variable_filter_name_offset[(sizeof(void *) != 4 || ((offsetof(CStateVariableFilterEffect, name) == 20))) ? 1 : -1];
typedef char state_variable_filter_frequency_offset[(sizeof(void *) != 4 || ((offsetof(CStateVariableFilterEffect, frequency) == 56))) ? 1 : -1];

struct s_state_variable_filter_channel
{
    float *samples;
    int stride;
    // Xbox WaveShell export 13 copies the first two words and advances by 20 bytes.
    int reserved[3];
};


typedef int (*t_state_variable_filter_proc)(void *, int, int, s_state_variable_filter_channel *, int, int,
    int *output_sample_count, s_state_variable_filter_channel *, int);
#if defined(_MSC_VER) && !defined(__clang__)
extern "C" t_state_variable_filter_proc g_state_variable_filter_proc;
#endif
// Other compilers: the cell is bound to the Waves import view's slot in state_variable_filter_effect.cpp, its only user.

extern "C" IXAudioEffect *state_variable_filter_initialize_effect(
    void *memory, void *context, const char *name, long filter_type, unsigned long sample_rate, long channel_count);

#endif // SOUND_XENON_DSP_STATE_VARIABLE_FILTER_EFFECT_H
