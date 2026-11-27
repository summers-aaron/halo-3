#ifndef SOUND_SOUND_XAUDIO_REVERB_VOICE_TYPES_H
#define SOUND_SOUND_XAUDIO_REVERB_VOICE_TYPES_H
#include <xtl.h>
#include <xaudio.h>
#include <xaudfx.h>
#include <stddef.h>

#pragma pack(push, 1)
struct s_parameters
{
    bool bypass;
    float output_gain;
    bool normalize_input;
    float normalize_level;
    bool shaper_enabled;
    float shaper[5];
    bool low_pass_enabled;
    float low_pass[2];
    bool band_pass_enabled;
    float band_pass[3];
};
#pragma pack(pop)
typedef char xaudio_reverb_parameters_size[(sizeof(s_parameters) == 53) ? 1 : -1];

struct s_voice_biquad
{
    float coefficients[12];
};
typedef char voice_biquad_size[(sizeof(s_voice_biquad) == 0x30) ? 1 : -1];
struct s_voice_sample_history
{
    float samples[256];
    long index;

    __forceinline float oldest() const
    {
        return samples[(index + 1) & 0xff];
    }
    __forceinline void push(float sample)
    {
        index = (index + 1) & 0xff;
        samples[index] = sample;
    }
};

class c_xaudio_voice_effect : public IXAudioEffect
{
public:
    c_xaudio_voice_effect();

    static void operator delete(void *) {}

    virtual unsigned long STDMETHODCALLTYPE AddRef();
    virtual unsigned long STDMETHODCALLTYPE Release();
    virtual long STDMETHODCALLTYPE GetInfo(XAUDIOFXINFO *info);
    virtual long STDMETHODCALLTYPE GetParam(unsigned char parameter_id, unsigned char parameter_type, XAUDIOFXPARAM *parameter);
    virtual long STDMETHODCALLTYPE SetParam(unsigned char parameter_id, unsigned char parameter_type, const XAUDIOFXPARAM *parameter);
    virtual long STDMETHODCALLTYPE GetContext(void **context);
    virtual long STDMETHODCALLTYPE Process(IXAudioFrameBuffer *input, IXAudioFrameBuffer *output);
    virtual ~c_xaudio_voice_effect() {}
    virtual long Initialize(const XAUDIOFXINIT *init, IXAudioBatchAllocator *allocator) { return 0; }
    virtual void copy_parameters(void *destination, const void *source);
    virtual void process_samples(float *samples, long channel_count, s_parameters *parameters, bool reset) = 0;

    long reference_count;
    void *context;
    __declspec(align(8)) RTL_CRITICAL_SECTION lock;
    s_parameters params1;
    s_parameters params2;
    bool parameters_changed;
};

typedef char xaudio_voice_lock_offset[(sizeof(void *) != 4 || ((offsetof(c_xaudio_voice_effect, lock) == 0x10))) ? 1 : -1];

class c_xaudio_reverb_voice_effect : public c_xaudio_voice_effect
{
public:
    c_xaudio_reverb_voice_effect();

    static void *operator new(size_t size, IXAudioBatchAllocator *allocator) { return allocator->Alloc(size); }

    virtual void process_samples(float *samples, long channel_count, s_parameters *parameters, bool reset);

    void apply_shaper_curve(float *input, float *output, s_parameters *params);
    void compute_shaper_curve(float x, float y, float z);

    float table[256];
    float curve_coefficients[4];
    s_voice_biquad filter_a;
    s_voice_biquad filter_b;
    s_voice_sample_history history_a;
    float sum_a;
    s_voice_sample_history history_b;
    float sum_b;
};

typedef char xaudio_reverb_filter_offset[(sizeof(void *) != 4 || ((offsetof(c_xaudio_reverb_voice_effect, filter_a) == 1192))) ? 1 : -1];
typedef char xaudio_reverb_history_offset[(sizeof(void *) != 4 || ((offsetof(c_xaudio_reverb_voice_effect, history_a) == 1288))) ? 1 : -1];
typedef char xaudio_reverb_table_offset[(sizeof(void *) != 4 || ((offsetof(c_xaudio_reverb_voice_effect, table) == 152))) ? 1 : -1];
typedef char xaudio_reverb_voice_size[(sizeof(void *) != 4 || ((sizeof(c_xaudio_reverb_voice_effect) == 3352))) ? 1 : -1];
#endif
