// blamlibxbox/state_variable_filter_effect.obj: from the records 0x28a708 0x2f7de0 0x2f7f38 0x2f8048 0x2f8060 0x2f80f0 0x2f81c0 0x2f81d0 0x2f8258 0x2f82e0 0x2f8368 0x2f83a8 0x2f8418 0x2f8528 0x2f85b0 0x2f86d8 0x2f87b0 0x2f8890 0x2f8990
// @flags /TP /O2 /Oc
// @split_bss
// @bss_escape svf_new_filter_state_procedure,svf_register_filter_state_proc
// @address 0x28a708 ?Release@c_remote_voice_ring_buffer_effect@@UAAKXZ
// @address 0x2f7f38 ?Process@CStateVariableFilterEffect@@UAAJPAUIXAudioFrameBuffer@@0@Z
// @address 0x2f8368 ?Release@CStateVariableFilterEffect@@UAAKXZ
// @address 0x2f83a8 state_variable_filter_string_reset
// @address 0x2f8418 state_variable_filter_string_assign_substring
// @address 0x2f8528 state_variable_filter_string_shrink_to_short
// @address 0x2f87b0 state_variable_filter_string_grow
// @address 0x8c43e0 svf_new_filter_state_procedure
// @address 0x8c4424 g_state_variable_filter_proc
// @address 0x42564 ??_7CStateVariableFilterEffect@@6B@
// @address 0x58dbc0 ?_Xran@_String_base@std@@SAXXZ
// @address 0x58dac8 ?_Xlen@_String_base@std@@SAXXZ
// @address 0x58cb80 RtlFreeHeap
// @address 0x58c298 RtlAllocateHeap
// @address 0x6c07e0 XAudioFrameBuffer_GetProcessingData
#include "sound/xenon/dsp/state_variable_filter_effect_externs.h"
#include <string.h>
#include <xaudfx.h>
#include "sound/sound_xaudio_xenon.h"
#include "networking/online/voice_queue.h"
#include "cache/security_functions.h"
#include "sound/xenon/dsp/state_variable_filter_effect.h"
#include "main/main_halt.h"
#include "xbox/xmodule.h"
#if !defined(_MSC_VER) || defined(__clang__)
#include "sound/sound_xaudio_xenon_imports.h"
#endif

// Empty the message macro around this include so no "bad allocation" literal is kept in this object
#include <xstddef>
#undef _MESG
#define _MESG(str) 0
#include <new>
#include <xstring>
#undef _MESG
#define _MESG(str) str

typedef void (*t_svf_set_frequency_proc)(void *state, unsigned long mode, double frequency);
typedef void (*t_svf_set_mode_proc)(void *state, unsigned long mode, double *frequency);
union svf_callback_entry
{
    t_svf_set_frequency_proc frequency;
    t_svf_set_mode_proc mode;
};

// The simple Release at 0x28a708 is the ICF-shared voice implementation.
#include "sound/xenon/dsp/remote_voice_ring_buffer_effect.h"
unsigned long c_remote_voice_ring_buffer_effect::Release()
{
    return --m_reference_count;
}

// DSP filter procedure, initially null; the registered filter module sets it.
#if defined(_MSC_VER) && !defined(__clang__)
t_state_variable_filter_proc g_state_variable_filter_proc = 0;
#else
// Other compilers: on the Xbox this cell is the Waves import view's slot 18 (bss 0x828c4424); the view has storage of
// its own here, so the name is bound to the slot (sound_xaudio_xenon_imports.h).
#define g_state_variable_filter_proc (reinterpret_cast<t_state_variable_filter_proc &>(sound_xaudio_waves_procedure_slot(18)))
#endif

long CStateVariableFilterEffect::Process(IXAudioFrameBuffer *input, IXAudioFrameBuffer *output)
{
    XAUDIOFRAMEBUFDATA data;
    s_state_variable_filter_channel channels[6];
    int output_sample_count;

    XAudioFrameBuffer_GetProcessingData(input, &data);
    if (this->filter_state != 0) {
        output_sample_count = 0;

        channels[0].samples = data.pSampleBuffer + 0;
        channels[0].stride = 1;
        channels[0].reserved[0] = 0;
        channels[0].reserved[1] = 0;
        channels[0].reserved[2] = 0;

        channels[1].samples = data.pSampleBuffer + 256;
        channels[1].stride = 1;
        channels[1].reserved[0] = 0;
        channels[1].reserved[1] = 0;
        channels[1].reserved[2] = 0;

        channels[2].samples = data.pSampleBuffer + 512;
        channels[2].stride = 1;
        channels[2].reserved[0] = 0;
        channels[2].reserved[1] = 0;
        channels[2].reserved[2] = 0;

        channels[3].samples = data.pSampleBuffer + 768;
        channels[3].stride = 1;
        channels[3].reserved[0] = 0;
        channels[3].reserved[1] = 0;
        channels[3].reserved[2] = 0;

        channels[4].samples = data.pSampleBuffer + 1024;
        channels[4].stride = 1;
        channels[4].reserved[0] = 0;
        channels[4].reserved[1] = 0;
        channels[4].reserved[2] = 0;

        channels[5].samples = data.pSampleBuffer + 1280;
        channels[5].stride = 1;
        channels[5].reserved[0] = 0;
        channels[5].reserved[1] = 0;
        channels[5].reserved[2] = 0;

        g_state_variable_filter_proc(this->filter_state, this->channel_count, 256, channels,
            this->channel_count, 256, &output_sample_count, channels, 0);
    }
    return 0;
}

// @address 0x2f8048 ?GetInfo@CStateVariableFilterEffect@@UAAJPAUXAUDIOFXINFO@@@Z
long CStateVariableFilterEffect::GetInfo(struct XAUDIOFXINFO *info)
{
    info->DataFlow = 1;
    info->TrailFrames = 1;
    return 0;
}

// @address 0x2f8060 ?GetParam@CStateVariableFilterEffect@@UAAJEEPATXAUDIOFXPARAM@@@Z
long CStateVariableFilterEffect::GetParam(unsigned char id, unsigned char type, XAUDIOFXPARAM *out)
{
    XAUDIOFXPARAM value = {0.0f};
    long result = 0;
    XAudioLock((XAUDIOLOCKTYPE)0x10001);
    switch (id) {
    case 0: value.Value = (float)this->mode; break;
    case 1: value.Value = (float)this->frequency; break;
    default: break;
    }
    *out = value;
    XAudioLock((XAUDIOLOCKTYPE)0x10003);
    return result;
}

#if defined(_MSC_VER) && !defined(__clang__)
static svf_callback_entry svf_set_mode_proc, svf_set_frequency_proc;
#else
// the view's slots 7 and 6 (bss 0x828c43f8, 0x828c43f4 on the Xbox)
#define svf_set_mode_proc (reinterpret_cast<svf_callback_entry &>(sound_xaudio_waves_procedure_slot(7)))
#define svf_set_frequency_proc (reinterpret_cast<svf_callback_entry &>(sound_xaudio_waves_procedure_slot(6)))
#endif
// @address 0x2f81c0 QuerySizeStateVariableFilterEffect
HRESULT QuerySizeStateVariableFilterEffect(const XAUDIOFXINIT *init, unsigned long *value)
{
    *value = sizeof(CStateVariableFilterEffect);
    return 0;
}


// @address 0x2f81d0 state_variable_filter_create_surround_effect
extern "C" long state_variable_filter_create_surround_effect(const XAUDIOFXINIT *init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect)
{
    void *memory = allocator->Alloc(sizeof(CStateVariableFilterEffect));
    IXAudioEffect *created;
    if (memory)
        created = state_variable_filter_initialize_effect(memory, init->pContext, "L360,L3M6", 5, 48000, 6);
    else
        created = 0;
    *effect = created;
    return 0;
}

// @address 0x2f8258 state_variable_filter_create_eq1m_effect
extern "C" long state_variable_filter_create_eq1m_effect(const XAUDIOFXINIT *init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect)
{
    void *memory = allocator->Alloc(sizeof(CStateVariableFilterEffect));
    IXAudioEffect *created;
    if (memory)
        created = state_variable_filter_initialize_effect(memory, init->pContext, "Q10,EQ1M", 102, 48000, 1);
    else
        created = 0;
    *effect = created;
    return 0;
}

// @address 0x2f82e0 state_variable_filter_create_eq4m_effect
extern "C" long state_variable_filter_create_eq4m_effect(const XAUDIOFXINIT *init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect)
{
    void *memory = allocator->Alloc(sizeof(CStateVariableFilterEffect));
    IXAudioEffect *created;
    if (memory)
        created = state_variable_filter_initialize_effect(memory, init->pContext, "Q10,EQ4M", 102, 48000, 1);
    else
        created = 0;
    *effect = created;
    return 0;
}

unsigned long CStateVariableFilterEffect::Release()
{
    if (--m_reference_count == 0) {
        state_variable_filter_string_reset(&name);
        return 0;
    }
    return m_reference_count;
}

void state_variable_filter_string_reset(s_string *string)
{
    if (16 <= string->capacity && string->bx.pointer != 0) {
        RtlFreeHeap(GetProcessHeap(), 0, string->bx.pointer);
    }
    string->size = 0;
    string->capacity = 15;
    string->bx.buffer[0] = 0;
}

static __forceinline char *svf_string_pointer(s_string *string)
{
    return 16 <= string->capacity ? string->bx.pointer : string->bx.buffer;
}

static __forceinline const char *svf_string_pointer_const(const s_string *string)
{
    return 16 <= string->capacity ? string->bx.pointer : string->bx.buffer;
}

static __forceinline bool svf_string_inside(s_string *string, const char *pointer)
{
    if (pointer < svf_string_pointer(string) || svf_string_pointer(string) + string->size <= pointer)
        return false;
    else
        return true;
}

static __forceinline void svf_string_eos(s_string *string, unsigned int size)
{
    svf_string_pointer(string)[string->size = size] = 0;
}

s_string *state_variable_filter_string_assign_substring(s_string *string, const s_string *right, unsigned int offset, unsigned int count)
{
    if (right->size < offset)
        std::_String_base::_Xran();
    unsigned int number = right->size - offset;
    if (count < number)
        number = count;
    if (string == right) {
        state_variable_filter_string_erase(string, offset + number, (unsigned int)-1);
        state_variable_filter_string_erase(string, 0, offset);
    } else if (state_variable_filter_string_grow(string, number, false)) {
        memcpy_s(svf_string_pointer(string), string->capacity, svf_string_pointer_const(right) + offset, number);
        svf_string_eos(string, number);
    }
    return string;
}

// @address 0x2f85b0 state_variable_filter_string_assign_buffer
s_string *state_variable_filter_string_assign_buffer(s_string *string, const char *pointer, unsigned int count)
{
    if (svf_string_inside(string, pointer))
        return state_variable_filter_string_assign_substring(string, string, pointer - svf_string_pointer(string), count);
    if (state_variable_filter_string_grow(string, count, false)) {
        memcpy_s(svf_string_pointer(string), string->capacity, pointer, count);
        svf_string_eos(string, count);
    }
    return string;
}

// @address 0x2f86d8 state_variable_filter_string_erase
extern "C" s_string *state_variable_filter_string_erase(s_string *string, unsigned int offset, unsigned int count)
{
    if (string->size < offset)
        std::_String_base::_Xran();
    unsigned int number = string->size - offset;
    if (number < count)
        count = number;
    if (count != 0) {
        memmove_s(svf_string_pointer(string) + offset, string->capacity - offset,
            svf_string_pointer(string) + offset + count, number - count);
        svf_string_eos(string, string->size - count);
    }
    return string;
}

extern "C" void state_variable_filter_string_shrink_to_short(s_string *string, bool built, unsigned int new_size)
{
    char *pointer;
    if ((unsigned char)built != 0 && 16 <= string->capacity) {
        pointer = string->bx.pointer;
        if (new_size != 0)
            memcpy_s(string->bx.buffer, 16, pointer, new_size);
        if (pointer != 0)
            RtlFreeHeap(GetProcessHeap(), 0, pointer);
    }
    string->size = new_size;
    string->capacity = 15;
    svf_string_eos(string, new_size);
}

// @address 0x2f8890 state_variable_filter_string_reallocate
extern "C" void state_variable_filter_string_reallocate(s_string *string, unsigned int new_size, unsigned int old_size)
{
    unsigned int new_capacity = new_size | 15;
    if (new_capacity > (unsigned int)-2) {
        new_capacity = new_size;
    } else {
        unsigned int current = string->capacity;
        unsigned int half = current >> 1;
        if (new_capacity / 3 < half && current <= (unsigned int)-2 - half)
            new_capacity = half + current;
    }
    char *new_pointer = (char *)state_variable_filter_allocate(new_capacity + 1, 0);
    if (old_size != 0) {
        char *source;
        if (string->capacity >= 16)
            source = string->bx.pointer;
        else
            source = string->bx.buffer;
        memcpy_s(new_pointer, new_capacity + 1, source, old_size);
    }
    if (string->capacity >= 16) {
        char *old_pointer = string->bx.pointer;
        if (old_pointer != 0) {
            RtlFreeHeap(GetProcessHeap(), 0, old_pointer);
        }
    }
    char *base = string->bx.buffer;
    *base = 0;
    string->bx.pointer = new_pointer;
    string->capacity = new_capacity;
    string->size = old_size;
    if (new_capacity >= 16)
        new_pointer[old_size] = 0;
    else
        base[old_size] = 0;
}

extern "C" unsigned char state_variable_filter_string_grow(s_string *string, unsigned int new_size, unsigned char trim)
{
    unsigned int capacity;
    char *eos;
    if ((unsigned int)-2 < new_size)
        std::_String_base::_Xlen();
    capacity = string->capacity;
    if (capacity < new_size) {
        state_variable_filter_string_reallocate(string, new_size, string->size);
    } else if (trim != 0 && new_size < 16) {
        state_variable_filter_string_shrink_to_short(string, true, new_size < string->size ? new_size : string->size);
    } else if (new_size == 0) {
        string->size = 0;
        if (16 <= capacity)
            eos = string->bx.pointer;
        else
            eos = string->bx.buffer;
        eos[0] = 0;
    }
    return 0 < new_size;
}



// @address 0x7c8bb4 ?_Raise_handler@std@@3P6AXABVexception@1@@ZA

// @address 0x2f8990 state_variable_filter_allocate
extern "C" void *state_variable_filter_allocate(unsigned int count, const void *hint)
{
    if (count > 0 && ((unsigned int)(-1) / count) < 1)
        _THROW_NCEE(std::bad_alloc, 0);
    return RtlAllocateHeap(GetProcessHeap(), 0, count);
}
// @address 0x2f80f0 ?SetParam@CStateVariableFilterEffect@@UAAJEEPBTXAUDIOFXPARAM@@@Z
long CStateVariableFilterEffect::SetParam(unsigned char id, unsigned char type, const XAUDIOFXPARAM *param)
{
    long result = 0;
    switch (id) {
    case 1:
        this->frequency = param->Value;
        if (this->filter_state)
            svf_set_frequency_proc.frequency(this->filter_state, this->mode, this->frequency);
        break;
    case 0: {
        unsigned long mode = (unsigned long)param->Value;
        this->mode = mode;
        svf_set_mode_proc.mode(this->filter_state, mode, &this->frequency);
        break;
    }
    default:
        result = 0x80004005;
        break;
    }
    return result;
}

typedef void *(*voice_queue_create_filter_proc)(const char *, unsigned long, unsigned long);
typedef long (*voice_queue_register_filter_proc)(void *, void *, unsigned long);
#if defined(_MSC_VER) && !defined(__clang__)
extern "C"
{
    static voice_queue_create_filter_proc svf_new_filter_state_procedure;
    static voice_queue_register_filter_proc svf_register_filter_state_proc;
}
#else
// the view's slots 1 and 16 (bss 0x828c43e0, 0x828c441c on the Xbox)
#define svf_new_filter_state_procedure (reinterpret_cast<voice_queue_create_filter_proc &>(sound_xaudio_waves_procedure_slot(1)))
#define svf_register_filter_state_proc (reinterpret_cast<voice_queue_register_filter_proc &>(sound_xaudio_waves_procedure_slot(16)))
#endif

static __forceinline voice_queue_register_filter_proc state_variable_filter_effect_register_filter_state()
{
    return svf_register_filter_state_proc;
}

// @address 0x2f7de0 state_variable_filter_initialize_effect
extern "C" IXAudioEffect *state_variable_filter_initialize_effect(
    void *memory, void *effect_context, const char *name, long filter_type, unsigned long sample_rate, long channel_count)
{
    CStateVariableFilterEffect *effect = (CStateVariableFilterEffect *)memory;
    // match: 0x2f7de0: pointer __restrict qualifier prevents alias reload
    CStateVariableFilterEffect *__restrict initialize;
    initialize = effect;
    const char *context = (const char *)effect_context;
    // match: 0x2f7de0: memory is non-null from the guarded Alloc callers; default construction stores only the vfptr
    __assume(initialize != 0);
    new (initialize) CStateVariableFilterEffect;
    s_string *effect_name = &initialize->name;
    effect_name->capacity = 15;
    svf_string_eos(effect_name, 0);
    if (!sound_xaudio_xenon_memory_procedures_set())
        main_halt_and_display_errors();
    effect->m_context = (void *)context;
    effect->channel_count = channel_count;
    effect->m_reference_count = 1;
    state_variable_filter_string_assign_buffer(effect_name, name, strlen(name));
    effect->mode = filter_type;
    effect->frequency = 500.0;
    effect->filter_state = svf_new_filter_state_procedure(name, 48000, 0);
    if (!effect->filter_state)
        main_halt_and_display_errors();
    voice_filter_validate_version(effect->filter_state, name);
    if (effect->filter_state && context)
    {
        void *module = security_get_module_handle(0);
        if (module)
        {
            void *section_data;
            unsigned long section_size;
            if (XGetModuleSection(module, context, &section_data, &section_size) != 1)
                main_halt_and_display_errors();
            if (state_variable_filter_effect_register_filter_state()(effect->filter_state, section_data, 0) != 1)
                main_halt_and_display_errors();
        }
        else
            main_halt_and_display_errors();
    }
    else
    {
        // match: 0x2f7de0: calling through the pointer keeps the two remaining halt calls before the return epilogue.
        void (*error_handler)() = main_halt_and_display_errors;
        error_handler();
    }
    return effect;
}

// End of state variable filter unit.

// @address 0x6cea98 ?AddRef@CStateVariableFilterEffect@@UAAKXZ
unsigned long CStateVariableFilterEffect::AddRef() { return ++m_reference_count; }
// @address 0x2f81b0 ?GetContext@CStateVariableFilterEffect@@UAAJPAPAX@Z
long CStateVariableFilterEffect::GetContext(void **context) { *context = m_context; return 0; }
