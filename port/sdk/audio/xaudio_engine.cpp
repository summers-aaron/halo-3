// port/sdk/audio (both hosts): the XAudio engine with sound, and X3DAudio. Built like the game, against the XDK's
// xaudio.h / xaudfx.h / x3daudio.h, so every structure and signature is the Xbox's.
//
// An engine thread renders XAudio frames (256 samples at 48 kHz, 5.33 ms) through the voice graph the title builds
// and hands each finished frame of the mastering voice (6 channels: FL FR FC LFE BL BR) to the host output
// (port_audio.h: a ring drained into PulseAudio / ALSA / PipeWire / winmm; the output's free space paces the
// engine, so voices play in real time, and without a device a null sink keeps the same pace).
//
// A frame:
//   1. FRAMESTART engine callbacks.
//   2. Every source voice that is started renders 256 samples: its queued packets (SubmitPacket) and source buffers
//      (SubmitSourceBuffer) are read in order -- PCM converted to float, XMA decoded (xma_decoder.c: the WMA Pro
//      bitstream, one decoder per XMA stream, streams of a multi-stream voice interleaved by their packet skip
//      counts) -- from PlayBegin to PlayEnd with their loops, and resampled from the voice's rate to 48 kHz at
//      2^pitch * frequency scale (4-point cubic Hermite). Packet completion and loop callbacks run as each packet
//      is used up or loops, on this thread with the API lock held, as XAudio calls them. The voice's volume and
//      effect chain apply, then its output entries mix it into their destination voices through their channel
//      maps (XAUDIOVOICEOUTPUT; SetVoiceOutputVolume changes an entry's volume, ramped over the next frame).
//   3. Submix voices, in SubmixStage order, run their effect chains in place on what was mixed into them and mix
//      on into their destinations; a null destination is the mastering voice.
//   4. The mastering voice's effect chain, then the frame goes to the output.
//   5. FRAMEEND engine callbacks.
//
// Voice states (XAUDIOSOURCESTATE_*): Start sets STARTING (SYNCHRONIZED waits for XAudioSynchStart) and the voice
// renders from the next frame; Stop sets STOPPING and the next frame fades the voice out and stops it
// (XAUDIOSTOP_IMMEDIATE stops it at once). Queued packets stay queued across a stop until flushed (FlushPackets /
// FlushSourceBuffers complete them at once with XAUDIOPACKETSTATUS_FLUSHED). A voice with no packets left starves
// (silence, STARVED) unless Discontinuity was signalled, in which case it stops by itself once its last sample
// has played. READYPACKET is set while the queue has room (MaxPacketCount).
//
// Effects: XAudioRegisterEffects gives each table entry with EffectId 0 a dynamic id (0x80 up) and writes it back,
// as the game reads it; an entry with an id keeps it. A voice creates its chain's effects when it is created
// (query size, a batch allocator over voice-owned memory, create) and runs every frame, through a frame buffer
// (XAudioFrameBuffer_GetProcessingData: planar float, 256 samples a channel, 48 kHz), the ones that process in
// place (XAUDIODATAFLOW_INPLACE) and the ones that only read their input (XAUDIODATAFLOW_INPUT alone: taps, given
// no output buffer). That runs the game's own DSP (state_variable_filter_effect.cpp on the Waves stand-ins of
// waves_bypass.c, the environment shaper, the voice-chat ring buffers: the remote voice and headset rings are filled
// by input-only taps and read back by in-place ones) exactly where the Xbox runs it. The built-in effect the game
// uses, XAUDIOFXID_REVERB (the two I3DL2 reverb voices), is c_port_reverb below.
//
// Sample data: XMA is a byte stream, the same on both ends. 16-bit and float PCM are taken in host byte order: on
// the PC the only PCM producers are the port's own code (the game's sounds are all XMA).
#include <xtl.h>
#include <xaudio.h>
#include <xaudfx.h>
#include <x3daudio.h>
#include <math.h>
#include "port_audio.h"
#include "xma_decoder.h"

namespace {

enum e_voice_type { k_voice_source, k_voice_submix, k_voice_master };

const DWORD k_voice_magic = 0x58415644; // 'XAVD'
const DWORD k_max_packets = 64;
const DWORD k_frame_samples = 256;
const DWORD k_frame_rate = 48000;
const DWORD k_max_channels = 8;
const DWORD k_master_channels = 6;
const DWORD k_max_outputs = 8;
const DWORD k_max_map_entries = 64;
const DWORD k_max_effects = 8;
const DWORD k_max_streams = XAUDIOXMASTREAMCOUNT_MAX;

struct s_voice;

struct s_route_entry {
    BYTE input, output;
    float volume;  // the entry's volume
    float applied; // the volume the last frame ended at (a change ramps over one frame)
};

struct s_route {
    s_voice *destination;
    DWORD count;
    s_route_entry entries[k_max_map_entries];
};

struct s_frame_buffer {
    XAUDIOFRAMEBUFDATA data;
};

struct s_effect {
    IXAudioEffect *effect;
    void *memory;
    bool enabled;
    bool runs; // in place, or a tap that only reads its input
};

// One XMA stream of a source voice: its decoder and where it is in the current packet's data.
struct s_xma_stream {
    xma_decoder *decoder;
    xma_cursor cursor;
    BYTE channels, first_channel;
    long next_frame;    // the frame the cursor hands over next
    long decoded_frame; // the frame whose samples are in samples[] (-1: none)
    bool ended;         // the data has no more frames: past the flush frame comes silence
    // The voice's buffers are one XMA stream, as the Xbox decoder context runs them: when a buffer's data ends and
    // the next one is queued, the decoder goes on into the next buffer's first frame (its output is both the end of
    // this buffer, the decoder delay, and the start of the next). Then the cursor and the decoder are already on the
    // buffer here, its frame 0 in samples[], and prepare_packet takes them over instead of starting the data over.
    const BYTE *carried_data;
    DWORD carried_bytes;
    unsigned char bits[XMA_FRAME_BUFFER_BYTES];
    float samples[2 * XMA_SAMPLES_PER_FRAME];
};

// A queued packet or source buffer. Positions count samples (per channel) from the start of its data.
struct s_packet {
    void *context;
    const BYTE *data;
    DWORD bytes;
    DWORD end;          // one past the last sample played
    DWORD loop_begin;   // the loop region, when loop_end > loop_begin
    DWORD loop_end;
    DWORD loops_left;   // XAUDIOLOOPCOUNT_INFINITE for ever
    DWORD position;     // the next sample to read
    bool prepared;      // the voice's XMA streams are on this packet's data
};

struct s_voice {
    DWORD magic;
    e_voice_type type;
    LONG references;
    void *context;
    LPFNXAUDIOVOICEPROCESSCALLBACK on_process;
    LPFNXAUDIOVOICEPACKETCALLBACK on_complete, on_loop;
    DWORD channels;
    DWORD stage;        // submix stage; source voices 0, the mastering voice after every submix
    s_route outputs[k_max_outputs];
    DWORD output_count;
    s_effect effects[k_max_effects];
    DWORD effect_count;
    float *mix;         // channels x 256, planar: a source's rendered frame, or what was mixed into a submix
    // source voices
    XAUDIOSOURCEFORMAT format;
    DWORD sample_bytes; // PCM bytes per sample
    DWORD rate;
    double max_step;
    DWORD max_packets;
    BYTE state;         // XAUDIOSOURCESTATE_*
    BYTE start_hold_frames; // wait for post-SynchStart mix-master volume/pitch update before settling
    XAUDIOPITCH pitch, max_pitch;
    double pitch_scale;
    XAUDIOFREQSCALE frequency_scale;
    XAUDIOVOLUME volume, applied_volume;
    DWORD stream_position;
    s_packet packets[k_max_packets];
    DWORD packet_head, packet_count;
    // the resampler's input: frames of `channels` samples; `position` is the read position in it (>= 1: the frame
    // before it is kept for the interpolation)
    float *fifo;
    DWORD fifo_capacity, fifo_frames;
    double position;
    s_xma_stream *streams;
    DWORD stream_count;
    s_voice *next;
    // HALO3_AUDIO_TRACE: the voice's number, and what the current play (source) or the current second (submix)
    // rendered: frames, the peak and energy of the voice's own frame (after its volume and effects), and per output
    // entry the peak and energy it delivered to its destination and the largest sum of its channel-map volumes
    DWORD trace_id;
    DWORD trace_frames;
    float trace_peak, trace_max_volume;
    double trace_energy;
    float trace_out_peak[k_max_outputs], trace_out_gain[k_max_outputs];
    double trace_out_energy[k_max_outputs];
};

struct s_engine_callback {
    LPFNXAUDIOENGINECALLBACK callback;
    void *context;
    XAUDIOENGINECALLBACKTYPE type;
};

struct s_effect_type {
    LPFNXAUDIOQUERYFXSIZE query_size;
    LPFNXAUDIOCREATEFX create;
};

CRITICAL_SECTION g_api_lock;  // the XAudio API lock (XAUDIOLOCKTYPE_API), recursive
CRITICAL_SECTION g_spin_lock; // XAUDIOLOCKTYPE_SPINLOCK: the game's effect data lock
volatile LONG g_initialized;  // 0 none, 1 initializing, 2 ready
s_voice *g_voices;
s_voice *g_master;
s_engine_callback g_engine_callbacks[16];
s_effect_type g_effect_types[256];
HANDLE g_thread;
volatile LONG g_stop;
float g_output[k_frame_samples * k_master_channels];

// ---- HALO3_AUDIO_TRACE=1: a diagnostic log of what each voice plays (stderr, "audio: " lines) -----------------------
//
//   audio: start T v=ID src=CONTEXT fmt=xma|pcm ch=N rate=HZ samples=N loops=N vol=DB
//       a source voice starts playing (T: engine time in ms; CONTEXT: the voice context, the game's source index;
//       samples / loops: the head packet's)
//   audio: end T v=ID src=CONTEXT ms=N peak=DB rms=DB maxvol=DB o0=PEAK/RMS/GAIN ... how=stop|drain|immediate|release
//       the play ends: its frame's peak and RMS (after its volume), the largest SetVolume, and per output entry the
//       peak and RMS it delivered and the largest sum of the entry's channel-map volumes (dB)
//   audio: bus T v=ID stage=S ch=N fx=N peak=DB rms=DB     every submix voice and the master (v=0), once a second
// The game's side (sound tags, gains) can join on src= through port_audio_trace_time_ms / port_audio_log. Off, it
// costs one test a voice a frame.
bool g_trace;
DWORD g_trace_next_id = 1;
DWORD g_engine_frames; // frames rendered since the engine started (the trace's clock)

struct s_trace_line {
    char text[640];
    DWORD length;
    s_trace_line() : length(0) { text[0] = 0; }
    void add(const char *s)
    {
        while (*s && length + 1 < sizeof(text))
            text[length++] = *s++;
        text[length] = 0;
    }
    void add_unsigned(DWORD value)
    {
        char digits[24];
        DWORD n = 0;
        do {
            digits[n++] = (char)('0' + value % 10);
            value /= 10;
        } while (value != 0 && n < sizeof(digits));
        while (n != 0 && length + 1 < sizeof(text))
            text[length++] = digits[--n];
        text[length] = 0;
    }
    void add_int(LONG value)
    {
        if (value < 0) {
            add("-");
            value = -value;
        }
        add_unsigned((DWORD)value);
    }
    // a level in dB with one decimal; at or below -120 dB "-inf"
    void add_db(double amplitude)
    {
        if (!(amplitude > 1.0e-6)) {
            add("-inf");
            return;
        }
        double tenths = 200.0 * log10(amplitude);
        LONG t = (LONG)(tenths < 0.0 ? tenths - 0.5 : tenths + 0.5);
        if (t < 0) {
            add("-");
            t = -t;
        }
        add_unsigned((DWORD)(t / 10));
        add(".");
        add_unsigned((DWORD)(t % 10));
    }
    void field(const char *name, LONG value)
    {
        add(" ");
        add(name);
        add("=");
        add_int(value);
    }
    void field_db(const char *name, double amplitude)
    {
        add(" ");
        add(name);
        add("=");
        add_db(amplitude);
    }
    void emit()
    {
        add("\n");
        port_audio_log(text);
    }
};

// 256 samples at 48 kHz: 16 / 3 ms a frame (32-bit arithmetic: wraps after 16 days of audio)
DWORD trace_time_ms() { return g_engine_frames * 16 / 3; }
typedef char trace_frame_milliseconds[k_frame_samples * 1000 * 3 == k_frame_rate * 16 ? 1 : -1];

// HALO3_AUDIO_TRACE: the I3DL2 parameters a reverb effect is given, when they change ("audio: reverb T fx=N ...":
// room, room HF, reflections and reverb in mB, decay in ms).
void trace_reverb(const void *effect, const XAUDIOREVERBI3DL2SETTINGS &s)
{
    static const void *effects[4];
    static XAUDIOREVERBI3DL2SETTINGS last[4];
    DWORD slot = 0;
    while (slot < 4 && effects[slot] != 0 && effects[slot] != effect)
        slot++;
    if (slot == 4)
        slot = 3;
    if (effects[slot] == effect && memcmp(&last[slot], &s, sizeof(s)) == 0)
        return;
    effects[slot] = effect;
    last[slot] = s;
    s_trace_line line;
    line.add("audio: reverb ");
    line.add_unsigned(trace_time_ms());
    line.field("fx", slot);
    line.field("room", s.lRoom);
    line.field("room_hf", s.lRoomHF);
    line.field("reflections", s.lReflections);
    line.field("reverb", s.lReverb);
    line.field("decay_ms", (LONG)(s.flDecayTime * 1000.0f));
    line.field("hf_ratio_pct", (LONG)(s.flDecayHFRatio * 100.0f));
    line.field("diffusion", (LONG)s.flDiffusion);
    line.field("density", (LONG)s.flDensity);
    line.emit();
}

void trace_reset(s_voice *voice)
{
    voice->trace_frames = 0;
    voice->trace_peak = voice->trace_max_volume = 0.0f;
    voice->trace_energy = 0.0;
    for (DWORD r = 0; r < k_max_outputs; r++) {
        voice->trace_out_peak[r] = voice->trace_out_gain[r] = 0.0f;
        voice->trace_out_energy[r] = 0.0;
    }
}

void ensure_initialized();

struct s_api_lock {
    s_api_lock() { ensure_initialized(); EnterCriticalSection(&g_api_lock); }
    ~s_api_lock() { LeaveCriticalSection(&g_api_lock); }
};

void *allocate(DWORD bytes) { return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bytes); }
void release(void *memory)
{
    if (memory != 0)
        HeapFree(GetProcessHeap(), 0, memory);
}

s_voice *voice_of(void *pointer)
{
    s_voice *voice = (s_voice *)pointer;
    return voice != 0 && voice->magic == k_voice_magic ? voice : 0;
}

s_voice *source_of(void *pointer)
{
    s_voice *voice = voice_of(pointer);
    return voice != 0 && voice->type == k_voice_source ? voice : 0;
}

// ---- the built-in I3DL2 reverb (XAUDIOFXID_REVERB) ------------------------------------------------------------------
//
// A mono, in-place, wet-only reverb on the I3DL2 parameters the game sets (XAUDIOREVERBI3DL2SETTINGS, from its
// sound environment tags): the input, filtered by RoomHF at HFReference, feeds early reflections (four taps after
// ReflectionsDelay, at Reflections) and, after ReflectionsDelay + ReverbDelay, a late reverb of eight damped comb
// filters and four allpass diffusers (DecayTime, DecayHFRatio at HFReference, Diffusion, Density), at Reverb; all
// at Room. Until the game sets parameters it is the XDK's default (Room -10000 mB: silent).

const DWORD k_reverb_predelay = 32768; // samples (0.3 s reflections + 0.1 s reverb delay fit)
const DWORD k_reverb_combs = 8, k_reverb_allpasses = 4;
const DWORD k_reverb_comb_size = 2048, k_reverb_allpass_size = 1024;
const float k_reverb_comb_lengths[k_reverb_combs] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617}; // at 44.1 kHz
const float k_reverb_allpass_lengths[k_reverb_allpasses] = {556, 441, 341, 225};
const float k_reverb_tap_ms[4] = {0.0f, 3.1f, 7.3f, 11.9f};
const float k_reverb_tap_gains[4] = {0.62f, 0.50f, 0.37f, 0.28f};

float millibels_to_gain(float millibels) { return millibels <= -10000.0f ? 0.0f : powf(10.0f, millibels / 2000.0f); }

// One-pole low-pass coefficient a (y = x + a (y' - x)) whose gain at the frequency with cosine cw is g.
float lowpass_coefficient(float g, float cw)
{
    if (g >= 0.9999f)
        return 0.0f;
    if (g < 0.001f)
        g = 0.001f;
    float g2 = g * g;
    float d = 2.0f * g2 * (1.0f - cw) - g2 * g2 * (1.0f - cw * cw);
    float a = (1.0f - g2 * cw - sqrtf(d > 0.0f ? d : 0.0f)) / (1.0f - g2);
    return a < 0.0f ? 0.0f : a > 0.99f ? 0.99f : a;
}

class c_port_reverb : public IXAudioEffect
{
public:
    static void *operator new(size_t, void *memory) { return memory; }
    static void operator delete(void *) {}
    static void operator delete(void *, void *) {}

    c_port_reverb(void *context) : m_references(1), m_context(context), m_changed(true)
    {
        static const XAUDIOREVERBI3DL2SETTINGS k_default = {
            -10000, 0, 0.0f, 1.0f, 0.5f, -10000, 0.02f, -10000, 0.04f, 100.0f, 100.0f, 5000.0f};
        m_settings = k_default;
        memset(m_predelay, 0, sizeof(m_predelay));
        memset(m_combs, 0, sizeof(m_combs));
        memset(m_allpasses, 0, sizeof(m_allpasses));
        memset(m_comb_filter, 0, sizeof(m_comb_filter));
        m_predelay_at = 0;
        for (DWORD i = 0; i < k_reverb_combs; i++)
            m_comb_at[i] = 0;
        for (DWORD i = 0; i < k_reverb_allpasses; i++)
            m_allpass_at[i] = 0;
        m_input_filter = 0.0f;
        update();
    }

    ULONG STDMETHODCALLTYPE AddRef() { return (ULONG)InterlockedIncrement(&m_references); }
    ULONG STDMETHODCALLTYPE Release() { return (ULONG)InterlockedDecrement(&m_references); }
    HRESULT STDMETHODCALLTYPE GetInfo(LPXAUDIOFXINFO info)
    {
        if (info == 0)
            return E_INVALIDARG;
        info->DataFlow = XAUDIODATAFLOW_INPLACE;
        float trail = m_settings.flDecayTime * (float)k_frame_rate / (float)k_frame_samples;
        info->TrailFrames = (XAUDIOTRAILFRAME)(trail > 65535.0f ? 65535.0f : trail);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetParam(XAUDIOFXPARAMID, XAUDIOFXPARAMTYPE type, LPXAUDIOFXPARAM param)
    {
        if (param == 0 || type != XAUDIOFXPARAMTYPE_DATA || param->Data.pBuffer == 0 ||
            param->Data.BufferSize < sizeof(m_settings))
            return E_INVALIDARG;
        memcpy(param->Data.pBuffer, &m_settings, sizeof(m_settings));
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE SetParam(XAUDIOFXPARAMID, XAUDIOFXPARAMTYPE type, LPCXAUDIOFXPARAM param)
    {
        if (param == 0 || type != XAUDIOFXPARAMTYPE_DATA || param->Data.pBuffer == 0 ||
            param->Data.BufferSize < sizeof(m_settings))
            return E_INVALIDARG;
        memcpy(&m_settings, param->Data.pBuffer, sizeof(m_settings));
        m_changed = true;
        if (g_trace)
            trace_reverb(this, m_settings);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetContext(LPVOID *context)
    {
        if (context == 0)
            return E_INVALIDARG;
        *context = m_context;
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE Process(IXAudioFrameBuffer *input, IXAudioFrameBuffer *output);

private:
    void update();

    LONG m_references;
    void *m_context;
    XAUDIOREVERBI3DL2SETTINGS m_settings;
    bool m_changed;
    float m_room, m_reflections, m_reverb;
    float m_input_coefficient;
    DWORD m_tap_delay[4], m_late_delay;
    DWORD m_comb_length[k_reverb_combs], m_allpass_length[k_reverb_allpasses];
    float m_comb_feedback[k_reverb_combs], m_comb_damping[k_reverb_combs], m_comb_input[k_reverb_combs];
    float m_allpass_feedback;
    float m_input_filter;
    float m_comb_filter[k_reverb_combs];
    DWORD m_predelay_at, m_comb_at[k_reverb_combs], m_allpass_at[k_reverb_allpasses];
    float m_predelay[k_reverb_predelay];
    float m_combs[k_reverb_combs][k_reverb_comb_size];
    float m_allpasses[k_reverb_allpasses][k_reverb_allpass_size];
};

void c_port_reverb::update()
{
    const XAUDIOREVERBI3DL2SETTINGS &s = m_settings;
    const float rate = (float)k_frame_rate;
    m_room = millibels_to_gain((float)s.lRoom);
    m_reflections = millibels_to_gain((float)s.lReflections);
    m_reverb = millibels_to_gain((float)s.lReverb);
    float reference = s.flHFReference < 20.0f ? 20.0f : s.flHFReference > 20000.0f ? 20000.0f : s.flHFReference;
    float cw = cosf(6.28318531f * reference / rate);
    m_input_coefficient = lowpass_coefficient(millibels_to_gain((float)(s.lRoomHF > 0 ? 0 : s.lRoomHF)), cw);
    float reflections_delay = s.flReflectionsDelay < 0.0f ? 0.0f : s.flReflectionsDelay > 0.3f ? 0.3f : s.flReflectionsDelay;
    float reverb_delay = s.flReverbDelay < 0.0f ? 0.0f : s.flReverbDelay > 0.1f ? 0.1f : s.flReverbDelay;
    for (DWORD i = 0; i < 4; i++)
        m_tap_delay[i] = (DWORD)((reflections_delay + k_reverb_tap_ms[i] * 0.001f) * rate) + 1;
    m_late_delay = (DWORD)((reflections_delay + reverb_delay) * rate) + 1;
    float density = (s.flDensity < 0.0f ? 0.0f : s.flDensity > 100.0f ? 100.0f : s.flDensity) / 100.0f;
    float diffusion = (s.flDiffusion < 0.0f ? 0.0f : s.flDiffusion > 100.0f ? 100.0f : s.flDiffusion) / 100.0f;
    float size = (0.7f + 0.3f * density) * rate / 44100.0f;
    float decay = s.flDecayTime < 0.1f ? 0.1f : s.flDecayTime > 20.0f ? 20.0f : s.flDecayTime;
    float hf_ratio = s.flDecayHFRatio < 0.1f ? 0.1f : s.flDecayHFRatio > 2.0f ? 2.0f : s.flDecayHFRatio;
    for (DWORD i = 0; i < k_reverb_combs; i++) {
        DWORD length = (DWORD)(k_reverb_comb_lengths[i] * size);
        m_comb_length[i] = length < 16 ? 16 : length > k_reverb_comb_size ? k_reverb_comb_size : length;
        float seconds = (float)m_comb_length[i] / rate;
        float g = powf(10.0f, -3.0f * seconds / decay);
        float g_hf = powf(10.0f, -3.0f * seconds / (decay * hf_ratio));
        m_comb_feedback[i] = g;
        m_comb_damping[i] = lowpass_coefficient(g_hf < g ? g_hf / g : 1.0f, cw);
        // unit energy from each comb, eight of them summed
        m_comb_input[i] = sqrtf(1.0f - g * g) * 0.35355339f;
    }
    for (DWORD i = 0; i < k_reverb_allpasses; i++) {
        DWORD length = (DWORD)(k_reverb_allpass_lengths[i] * size);
        m_allpass_length[i] = length < 8 ? 8 : length > k_reverb_allpass_size ? k_reverb_allpass_size : length;
    }
    m_allpass_feedback = 0.15f + 0.55f * diffusion;
    m_changed = false;
}

HRESULT c_port_reverb::Process(IXAudioFrameBuffer *input, IXAudioFrameBuffer *output)
{
    if (input == 0 || output != 0)
        return E_INVALIDARG;
    XAUDIOFRAMEBUFDATA data;
    if (FAILED(XAudioFrameBuffer_GetProcessingData(input, &data)))
        return E_INVALIDARG;
    if (m_changed)
        update();
    float *samples = data.pSampleBuffer;
    for (DWORD i = 0; i < k_frame_samples; i++) {
        float x = samples[i];
        m_input_filter = x + m_input_coefficient * (m_input_filter - x);
        m_predelay[m_predelay_at] = m_input_filter;
        float early = 0.0f;
        for (DWORD t = 0; t < 4; t++)
            early += k_reverb_tap_gains[t] * m_predelay[(m_predelay_at + k_reverb_predelay - m_tap_delay[t]) % k_reverb_predelay];
        float late_in = m_predelay[(m_predelay_at + k_reverb_predelay - m_late_delay) % k_reverb_predelay];
        m_predelay_at = (m_predelay_at + 1) % k_reverb_predelay;
        float late = 0.0f;
        for (DWORD c = 0; c < k_reverb_combs; c++) {
            float *line = m_combs[c];
            DWORD at = m_comb_at[c];
            float y = line[at];
            m_comb_filter[c] = y + m_comb_damping[c] * (m_comb_filter[c] - y);
            line[at] = late_in * m_comb_input[c] + m_comb_filter[c] * m_comb_feedback[c];
            m_comb_at[c] = at + 1 >= m_comb_length[c] ? 0 : at + 1;
            late += y;
        }
        for (DWORD a = 0; a < k_reverb_allpasses; a++) {
            float *line = m_allpasses[a];
            DWORD at = m_allpass_at[a];
            float delayed = line[at];
            float v = late + m_allpass_feedback * delayed;
            line[at] = v;
            late = delayed - m_allpass_feedback * v;
            m_allpass_at[a] = at + 1 >= m_allpass_length[a] ? 0 : at + 1;
        }
        float y = m_room * (m_reflections * early + m_reverb * late);
        samples[i] = y == y ? y : 0.0f;
    }
    // the reverb voices are mono; any further channel carries the same wet signal
    for (DWORD c = 1; c < data.Format.ChannelCount; c++)
        memcpy(samples + c * k_frame_samples, samples, k_frame_samples * sizeof(float));
    return S_OK;
}

HRESULT STDMETHODCALLTYPE reverb_query_size(LPCXAUDIOFXINIT init, LPDWORD size)
{
    if (init == 0 || size == 0)
        return E_INVALIDARG;
    *size = sizeof(c_port_reverb);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE reverb_create(LPCXAUDIOFXINIT init, IXAudioBatchAllocator *allocator, IXAudioEffect **effect)
{
    if (init == 0 || allocator == 0 || effect == 0)
        return E_INVALIDARG;
    void *memory = allocator->Alloc(sizeof(c_port_reverb));
    if (memory == 0)
        return E_OUTOFMEMORY;
    *effect = new (memory) c_port_reverb(init->pContext);
    return S_OK;
}

const XAUDIOFXTABLEENTRY k_builtin_effects[] = {{XAUDIOFXID_REVERB, reverb_query_size, reverb_create}};

// ---- effects ----------------------------------------------------------------------------------------------------

// The allocator an effect's create function takes its memory from: one block owned by the voice.
class c_batch_allocator : public IXAudioBatchAllocator
{
public:
    c_batch_allocator(BYTE *memory, DWORD size) : m_memory(memory), m_size(size), m_used(0) {}
    DWORD STDMETHODCALLTYPE AddRef() { return 1; }
    DWORD STDMETHODCALLTYPE Release() { return 1; }
    void STDMETHODCALLTYPE GrowHeap(DWORD) {}
    HRESULT STDMETHODCALLTYPE CreateHeap(DWORD) { return S_OK; }
    DWORD STDMETHODCALLTYPE GetFreeHeapSize() { return m_size - m_used; }
    LPVOID STDMETHODCALLTYPE Alloc(DWORD bytes)
    {
        DWORD at = (m_used + 15) & ~15u;
        if (at + bytes > m_size)
            return 0;
        m_used = at + bytes;
        return m_memory + at;
    }

private:
    BYTE *m_memory;
    DWORD m_size, m_used;
};

void create_effects(s_voice *voice, LPCXAUDIOVOICEFXCHAIN chain)
{
    if (chain == 0)
        return;
    for (DWORD i = 0; i < chain->EffectCount && voice->effect_count < k_max_effects; i++) {
        LPCXAUDIOFXINIT init = chain->papEffects != 0 ? chain->papEffects[i] : 0;
        s_effect &slot = voice->effects[voice->effect_count++];
        slot.effect = 0;
        slot.enabled = true;
        if (init == 0)
            continue;
        const s_effect_type &type = g_effect_types[init->EffectId];
        if (type.create == 0)
            continue; // not registered: the slot keeps its index and does nothing
        DWORD size = 0;
        if (type.query_size == 0 || FAILED(type.query_size(init, &size)))
            size = 0;
        DWORD capacity = size + 1024; // the query size, alignment and room for small extra allocations
        slot.memory = allocate(capacity);
        if (slot.memory == 0)
            continue;
        c_batch_allocator allocator((BYTE *)(((uintptr_t)slot.memory + 15) & ~(uintptr_t)15), capacity - 16);
        IXAudioEffect *effect = 0;
        if (FAILED(type.create(init, &allocator, &effect)) || effect == 0) {
            release(slot.memory);
            slot.memory = 0;
            continue;
        }
        XAUDIOFXINFO info = {0, 0};
        slot.runs = SUCCEEDED(effect->GetInfo(&info)) && ((info.DataFlow & XAUDIODATAFLOW_INPLACE) != 0 ||
            (info.DataFlow & XAUDIODATAFLOW_MASK) == XAUDIODATAFLOW_INPUT);
        slot.effect = effect;
    }
}

void run_effects(s_voice *voice)
{
    if (voice->effect_count == 0)
        return;
    s_frame_buffer buffer;
    buffer.data.Format.UnionValue = 0;
    buffer.data.Format.SampleType = XAUDIOSAMPLETYPE_FLOAT;
    buffer.data.Format.ChannelCount = (XAUDIOCHANNEL)voice->channels;
    buffer.data.Format.SampleRate = k_frame_rate;
    buffer.data.pSampleBuffer = voice->mix;
    for (DWORD i = 0; i < voice->effect_count; i++) {
        s_effect &slot = voice->effects[i];
        if (slot.effect != 0 && slot.enabled && slot.runs)
            slot.effect->Process((IXAudioFrameBuffer *)&buffer, 0);
    }
}

void destroy_effects(s_voice *voice)
{
    for (DWORD i = 0; i < voice->effect_count; i++) {
        if (voice->effects[i].effect != 0)
            voice->effects[i].effect->Release();
        release(voice->effects[i].memory);
    }
    voice->effect_count = 0;
}

// ---- routing ------------------------------------------------------------------------------------------------------

void set_outputs(s_voice *voice, LPCXAUDIOVOICEOUTPUT output)
{
    voice->output_count = 0;
    if (output == 0 || output->paEntries == 0) {
        // no outputs given: the mastering voice, channel for channel (mono to the front pair)
        s_route &route = voice->outputs[voice->output_count++];
        route.destination = g_master;
        route.count = 0;
        if (voice->channels == 1) {
            s_route_entry left = {0, 0, 0.70710678f, 0.70710678f}, right = {0, 1, 0.70710678f, 0.70710678f};
            route.entries[route.count++] = left;
            route.entries[route.count++] = right;
        } else {
            for (DWORD c = 0; c < voice->channels && c < k_master_channels; c++) {
                s_route_entry entry = {(BYTE)c, (BYTE)c, 1.0f, 1.0f};
                route.entries[route.count++] = entry;
            }
        }
        return;
    }
    for (DWORD i = 0; i < output->EntryCount && voice->output_count < k_max_outputs; i++) {
        const XAUDIOVOICEOUTPUTENTRY &entry = output->paEntries[i];
        s_route &route = voice->outputs[voice->output_count++];
        s_voice *destination = entry.pDestVoice != 0 ? voice_of(entry.pDestVoice) : g_master;
        route.destination = destination;
        route.count = 0;
        if (entry.pChannelMap == 0 || entry.pChannelMap->paEntries == 0)
            continue;
        for (DWORD k = 0; k < entry.pChannelMap->EntryCount && k < k_max_map_entries; k++) {
            const XAUDIOCHANNELMAPENTRY &map = entry.pChannelMap->paEntries[k];
            s_route_entry routed = {map.InputChannel, map.OutputChannel, map.Volume, map.Volume};
            route.entries[route.count++] = routed;
        }
    }
}

// HALO3_AUDIO_TRACE: what one channel-map entry of output `r` delivers this frame (volume ramping from -> to).
void trace_entry(s_voice *voice, DWORD r, float from, float to, const float *in)
{
    if (from == 0.0f && to == 0.0f)
        return;
    float step = (to - from) / (float)k_frame_samples, peak = voice->trace_out_peak[r];
    double energy = 0.0;
    for (DWORD i = 0; i < k_frame_samples; i++) {
        float v = (from + step * (float)(i + 1)) * in[i];
        float a = v < 0.0f ? -v : v;
        if (a > peak)
            peak = a;
        energy += (double)v * v;
    }
    voice->trace_out_peak[r] = peak;
    voice->trace_out_energy[r] += energy;
}

// HALO3_AUDIO_TRACE: the frame a voice rendered (its own mix, after its volume and effects) and its output volumes.
void trace_frame(s_voice *voice)
{
    float peak = voice->trace_peak;
    double energy = 0.0;
    for (DWORD i = 0; i < voice->channels * k_frame_samples; i++) {
        float v = voice->mix[i], a = v < 0.0f ? -v : v;
        if (a > peak)
            peak = a;
        energy += (double)v * v;
    }
    voice->trace_peak = peak;
    voice->trace_energy += energy;
    voice->trace_frames++;
    if (voice->volume > voice->trace_max_volume)
        voice->trace_max_volume = voice->volume;
    for (DWORD r = 0; r < voice->output_count; r++) {
        float sum = 0.0f;
        for (DWORD k = 0; k < voice->outputs[r].count; k++)
            sum += voice->outputs[r].entries[k].volume;
        if (sum > voice->trace_out_gain[r])
            voice->trace_out_gain[r] = sum;
    }
}

void trace_start(s_voice *voice)
{
    trace_reset(voice);
    s_trace_line line;
    line.add("audio: start ");
    line.add_unsigned(trace_time_ms());
    line.field("v", voice->trace_id);
    line.field("src", (LONG)(LONG)(size_t)voice->context);
    line.add(voice->format.SampleType == XAUDIOSAMPLETYPE_XMA ? " fmt=xma" : " fmt=pcm");
    line.field("ch", voice->channels);
    line.field("rate", voice->rate);
    if (voice->packet_count != 0) {
        const s_packet &packet = voice->packets[voice->packet_head];
        line.field("samples", packet.end - packet.position);
        line.field("loops", packet.loops_left == XAUDIOLOOPCOUNT_INFINITE ? -1 : (LONG)packet.loops_left);
    }
    line.field("queued", voice->packet_count);
    line.field_db("vol", voice->volume);
    line.emit();
}

void trace_end(s_voice *voice, const char *how)
{
    s_trace_line line;
    line.add("audio: end ");
    line.add_unsigned(trace_time_ms());
    line.field("v", voice->trace_id);
    line.field("src", (LONG)(LONG)(size_t)voice->context);
    line.field("ms", (LONG)(voice->trace_frames * 16 / 3));
    double samples = (double)voice->trace_frames * k_frame_samples;
    line.field_db("peak", voice->trace_peak);
    line.field_db("rms", samples > 0.0 ? sqrt(voice->trace_energy / (samples * voice->channels)) : 0.0);
    line.field_db("maxvol", voice->trace_max_volume);
    for (DWORD r = 0; r < voice->output_count; r++) {
        if (voice->trace_out_peak[r] == 0.0f && voice->trace_out_gain[r] == 0.0f)
            continue;
        line.add(" o");
        line.add_unsigned(r);
        line.add("=");
        line.add_db(voice->trace_out_peak[r]);
        line.add("/");
        line.add_db(samples > 0.0 ? sqrt(voice->trace_out_energy[r] / samples) : 0.0);
        line.add("/");
        line.add_db(voice->trace_out_gain[r]);
    }
    line.add(" how=");
    line.add(how);
    line.emit();
}

// HALO3_AUDIO_TRACE: once a second, every submix voice's level (and the master's, v=0).
void trace_buses()
{
    for (s_voice *voice = g_voices; ; voice = voice->next) {
        if (voice == 0)
            voice = g_master;
        if (voice->type != k_voice_source && voice->trace_frames != 0) {
            s_trace_line line;
            line.add("audio: bus ");
            line.add_unsigned(trace_time_ms());
            line.field("v", voice == g_master ? 0 : voice->trace_id);
            line.field("stage", voice == g_master ? -1 : (LONG)voice->stage);
            line.field("ch", voice->channels);
            line.field("fx", voice->effect_count);
            double samples = (double)voice->trace_frames * k_frame_samples * voice->channels;
            line.field_db("peak", voice->trace_peak);
            line.field_db("rms", sqrt(voice->trace_energy / samples));
            line.emit();
        }
        if (voice->type != k_voice_source)
            trace_reset(voice);
        if (voice == g_master)
            break;
    }
}

// Mixes the voice's frame into its destinations.
void route(s_voice *voice)
{
    for (DWORD r = 0; r < voice->output_count; r++) {
        s_route &route = voice->outputs[r];
        s_voice *destination = route.destination;
        if (destination == 0 || destination->magic != k_voice_magic || destination->mix == 0)
            continue;
        for (DWORD k = 0; k < route.count; k++) {
            s_route_entry &entry = route.entries[k];
            DWORD input_channel = voice->channels == 1 ? 0u : entry.input;
            if (input_channel >= voice->channels || entry.output >= destination->channels) {
                entry.applied = entry.volume;
                continue;
            }
            const float *in = voice->mix + input_channel * k_frame_samples;
            float *out = destination->mix + entry.output * k_frame_samples;
            float from = entry.applied, to = entry.volume;
            if (g_trace)
                trace_entry(voice, r, from, to, in);
            if (from == to) {
                if (to != 0.0f)
                    for (DWORD i = 0; i < k_frame_samples; i++)
                        out[i] += to * in[i];
            } else {
                float step = (to - from) / (float)k_frame_samples;
                for (DWORD i = 0; i < k_frame_samples; i++)
                    out[i] += (from + step * (float)(i + 1)) * in[i];
            }
            entry.applied = to;
        }
    }
}

// The voice starts from silence: no ramp from volumes it had before.
void settle_volumes(s_voice *voice)
{
    voice->applied_volume = voice->volume;
    for (DWORD r = 0; r < voice->output_count; r++)
        for (DWORD k = 0; k < voice->outputs[r].count; k++)
            voice->outputs[r].entries[k].applied = voice->outputs[r].entries[k].volume;
}

// ---- packets ------------------------------------------------------------------------------------------------------

void call_packet_callback(s_voice *voice, LPFNXAUDIOVOICEPACKETCALLBACK callback, void *packet_context,
    XAUDIOPACKETSTATUS status)
{
    if (callback == 0)
        return;
    XAUDIOVOICEPACKETCALLBACK data;
    data.pVoiceContext = voice->context;
    data.pPacketContext = packet_context;
    data.Status = status;
    callback(&data);
}

bool is_xma(const s_voice *voice) { return voice->format.SampleType == XAUDIOSAMPLETYPE_XMA; }

// Samples per channel in the voice's format: PCM from the size, XMA 512 a frame of its (shortest) stream.
DWORD packet_samples(const s_voice *voice, const void *data, DWORD bytes)
{
    if (data == 0)
        return 0;
    if (!is_xma(voice))
        return bytes / (voice->sample_bytes * voice->channels);
    DWORD frames = 0xffffffffu;
    for (DWORD s = 0; s < voice->stream_count; s++) {
        xma_cursor cursor;
        xma_cursor_init(&cursor, data, bytes, s);
        DWORD count = 0;
        while (xma_cursor_skip_frame(&cursor) != 0)
            count++;
        if (count < frames)
            frames = count;
    }
    return frames == 0xffffffffu ? 0 : frames * XMA_SAMPLES_PER_FRAME;
}

// The sample where the XMA frame starting at an absolute bit offset of stream 0 begins, or `fallback`.
DWORD xma_frame_sample(const s_voice *voice, const void *data, DWORD bytes, DWORD bit_offset, DWORD fallback)
{
    xma_cursor cursor;
    xma_cursor_init(&cursor, data, bytes, 0);
    for (DWORD frame = 0; !cursor.done; frame++) {
        DWORD at = xma_cursor_tell_bits(&cursor);
        if (at == bit_offset)
            return frame * XMA_SAMPLES_PER_FRAME;
        if (at > bit_offset || xma_cursor_skip_frame(&cursor) == 0)
            break;
    }
    (void)voice;
    return fallback;
}

HRESULT queue_packet(s_voice *voice, const s_packet &packet, XAUDIOSUBMITPACKET flags)
{
    if (voice->packet_count >= voice->max_packets)
        return E_FAIL; // the queue holds MaxPacketCount packets
    voice->packets[(voice->packet_head + voice->packet_count) % k_max_packets] = packet;
    voice->packet_count++;
    voice->state &= ~XAUDIOSOURCESTATE_STARVED;
    if (flags & XAUDIOSUBMITPACKET_DISCONTINUITY)
        voice->state |= XAUDIOSOURCESTATE_DISCONTINUITY;
    return S_OK;
}

void reset_fifo(s_voice *voice)
{
    for (DWORD c = 0; c < voice->channels; c++)
        voice->fifo[c] = 0.0f;
    voice->fifo_frames = 1;
    voice->position = 1.0;
}

// Under the API lock: every queued packet completes as flushed, and what was read ahead of the play position is
// dropped.
void flush_packets(s_voice *voice)
{
    while (voice->packet_count != 0) {
        void *context = voice->packets[voice->packet_head].context;
        voice->packet_head = (voice->packet_head + 1) % k_max_packets;
        voice->packet_count--;
        call_packet_callback(voice, voice->on_complete, context, XAUDIOPACKETSTATUS_FLUSHED);
    }
    reset_fifo(voice);
    for (DWORD s = 0; s < voice->stream_count; s++) {
        // whatever comes next starts its data over
        voice->streams[s].carried_data = 0;
        voice->streams[s].carried_bytes = 0;
    }
    voice->state &= ~(XAUDIOSOURCESTATE_DISCONTINUITY | XAUDIOSOURCESTATE_STARVED);
}

// PCM samples [from, from + count) of a packet, interleaved float.
void read_pcm(const s_voice *voice, const s_packet &packet, DWORD from, DWORD count, float *out)
{
    const DWORD channels = voice->channels, n = count * channels;
    const DWORD first = from * channels;
    switch (voice->format.SampleType) {
    case XAUDIOSAMPLETYPE_FLOAT:
        memcpy(out, (const float *)packet.data + first, n * sizeof(float));
        break;
    case XAUDIOSAMPLETYPE_8BITPCM: {
        const BYTE *in = packet.data + first;
        for (DWORD i = 0; i < n; i++)
            out[i] = ((float)in[i] - 128.0f) * (1.0f / 128.0f);
        break;
    }
    default: {
        const SHORT *in = (const SHORT *)packet.data + first;
        for (DWORD i = 0; i < n; i++)
            out[i] = (float)in[i] * (1.0f / 32768.0f);
        break;
    }
    }
}

// The stream's data from the start of a buffer, the decoder cleared.
void xma_start(s_xma_stream &stream, const s_packet &packet, DWORD index)
{
    xma_decoder_reset(stream.decoder);
    xma_cursor_init(&stream.cursor, packet.data, packet.bytes, index);
    stream.next_frame = 0;
    stream.decoded_frame = -1;
    stream.ended = false;
    stream.carried_data = 0;
    stream.carried_bytes = 0;
}

// Decodes frames of one stream until frame `frame` of `packet` is in its samples[]. `next` is the buffer queued
// after it when the decoder may go on into that one at the end of this one's data (see s_xma_stream), else 0.
void xma_reach(s_xma_stream &stream, const s_packet &packet, const s_packet *next, DWORD index, long frame)
{
    if (stream.decoded_frame == frame)
        return;
    if (frame < stream.decoded_frame || stream.next_frame > frame || stream.carried_data != 0)
        xma_start(stream, packet, index); // back (a loop, a new position): start the data over
    if (frame - 1 > stream.next_frame && !stream.ended) {
        // far ahead: step over the frames before the one ahead of `frame` (it rebuilds the overlap)
        xma_decoder_reset(stream.decoder);
        while (stream.next_frame < frame - 1 && xma_cursor_skip_frame(&stream.cursor) != 0)
            stream.next_frame++;
        if (stream.next_frame < frame - 1)
            stream.ended = true;
    }
    while (stream.decoded_frame < frame) {
        if (stream.ended) {
            memset(stream.samples, 0, sizeof(stream.samples));
        } else {
            unsigned bits = xma_cursor_next_frame(&stream.cursor, stream.bits);
            if (bits == 0 && next != 0) {
                // the data ends and the next buffer is queued: on into its first frame
                xma_cursor_init(&stream.cursor, next->data, next->bytes, index);
                bits = xma_cursor_next_frame(&stream.cursor, stream.bits);
                if (bits != 0) {
                    stream.carried_data = next->data;
                    stream.carried_bytes = next->bytes;
                }
            }
            if (bits == 0) {
                xma_decoder_flush(stream.decoder, stream.samples); // the last frame's overlap tail
                stream.ended = true;
            } else {
                xma_decoder_decode_frame(stream.decoder, stream.bits, bits, stream.samples);
            }
            if (stream.carried_data != 0) {
                // the frame decoded is the next buffer's frame 0; past it this buffer has only silence left
                stream.decoded_frame = stream.next_frame;
                stream.next_frame++;
                stream.ended = true;
                return;
            }
        }
        stream.decoded_frame = stream.next_frame;
        stream.next_frame++;
    }
}

// XMA samples [from, from + count) of the voice's current packet, interleaved float, every stream into its channels.
void read_xma(s_voice *voice, const s_packet &packet, DWORD from, DWORD count, float *out)
{
    const DWORD channels = voice->channels;
    // the next buffer continues this one once this one has no loops left to play
    const s_packet *next = packet.loops_left == 0 && voice->packet_count > 1
                               ? &voice->packets[(voice->packet_head + 1) % k_max_packets]
                               : 0;
    for (DWORD s = 0; s < voice->stream_count; s++) {
        s_xma_stream &stream = voice->streams[s];
        DWORD done = 0;
        while (done < count) {
            DWORD decoder_sample = from + done + XMA_DECODER_DELAY;
            long frame = (long)(decoder_sample / XMA_SAMPLES_PER_FRAME);
            DWORD offset = decoder_sample % XMA_SAMPLES_PER_FRAME;
            DWORD run = XMA_SAMPLES_PER_FRAME - offset;
            if (run > count - done)
                run = count - done;
            xma_reach(stream, packet, next, s, frame);
            for (DWORD c = 0; c < stream.channels; c++) {
                const float *in = stream.samples + c * XMA_SAMPLES_PER_FRAME + offset;
                float *o = out + done * channels + stream.first_channel + c;
                for (DWORD i = 0; i < run; i++)
                    o[i * channels] = in[i];
            }
            done += run;
        }
    }
}

void prepare_packet(s_voice *voice, s_packet &packet)
{
    packet.prepared = true;
    if (!is_xma(voice))
        return;
    for (DWORD s = 0; s < voice->stream_count; s++) {
        s_xma_stream &stream = voice->streams[s];
        if (stream.carried_data == packet.data && stream.carried_bytes == packet.bytes && packet.data != 0) {
            // the decoder went on into this buffer at the end of the last one: its frame 0 is decoded
            stream.decoded_frame = 0;
            stream.next_frame = 1;
            stream.ended = false;
            stream.carried_data = 0;
            stream.carried_bytes = 0;
        } else {
            xma_start(stream, packet, s);
        }
    }
}

// Under the API lock: reads up to `count` frames from the voice's packets into `out` (interleaved), running the
// completion and loop callbacks as packets are used up or loop. Returns the frames read (fewer when the packets run
// out).
DWORD pull(s_voice *voice, float *out, DWORD count)
{
    DWORD produced = 0;
    while (produced < count && voice->packet_count != 0) {
        s_packet &packet = voice->packets[voice->packet_head];
        if (!packet.prepared)
            prepare_packet(voice, packet);
        // in the loop region (or at its end) with loops left: play to the loop end, then jump back
        bool looping = packet.loops_left != 0 && packet.loop_end > packet.loop_begin && packet.position <= packet.loop_end;
        DWORD limit = looping ? packet.loop_end : packet.end;
        if (packet.position >= limit) {
            if (looping) {
                if (packet.loops_left != XAUDIOLOOPCOUNT_INFINITE)
                    packet.loops_left--;
                packet.position = packet.loop_begin;
                call_packet_callback(voice, voice->on_loop, packet.context, XAUDIOPACKETSTATUS_SUCCESS);
                continue;
            }
            void *context = packet.context;
            voice->packet_head = (voice->packet_head + 1) % k_max_packets;
            voice->packet_count--;
            call_packet_callback(voice, voice->on_complete, context, XAUDIOPACKETSTATUS_SUCCESS);
            continue;
        }
        DWORD take = limit - packet.position;
        if (take > count - produced)
            take = count - produced;
        float *at = out + produced * voice->channels;
        if (is_xma(voice))
            read_xma(voice, packet, packet.position, take, at);
        else
            read_pcm(voice, packet, packet.position, take, at);
        packet.position += take;
        produced += take;
        voice->stream_position += take;
    }
    return produced;
}

// ---- source voices ------------------------------------------------------------------------------------------------

// Under the API lock: one frame of a started source voice into voice->mix, resampled to 48 kHz.
void render_source(s_voice *voice, float fade)
{
    const DWORD channels = voice->channels;
    double step = (double)voice->rate / (double)k_frame_rate * voice->pitch_scale *
                  (voice->frequency_scale > 0.0f ? voice->frequency_scale : 1.0f);
    if (step > voice->max_step)
        step = voice->max_step;
    if (step < 1.0 / 4096.0)
        step = 1.0 / 4096.0;
    // frames the interpolation reads: up to index floor(last position) + 2
    DWORD need = (DWORD)(voice->position + (double)(k_frame_samples - 1) * step) + 3;
    if (need > voice->fifo_capacity)
        need = voice->fifo_capacity;
    if (voice->fifo_frames < need)
        voice->fifo_frames += pull(voice, voice->fifo + voice->fifo_frames * channels, need - voice->fifo_frames);
    DWORD real = voice->fifo_frames;
    for (DWORD i = real * channels; i < need * channels; i++)
        voice->fifo[i] = 0.0f; // past the data: silence (not kept)

    double position = voice->position;
    float volume_from = voice->applied_volume, volume_to = voice->volume;
    for (DWORD j = 0; j < k_frame_samples; j++, position += step) {
        DWORD i = (DWORD)position;
        if (i + 2 >= need)
            i = need - 3;
        float t = (float)(position - (double)i);
        float gain = (volume_from + (volume_to - volume_from) * (float)(j + 1) / (float)k_frame_samples) *
                     (fade < 0.0f ? 1.0f : fade * (1.0f - (float)j / (float)k_frame_samples));
        const float *x = voice->fifo + (i - 1) * channels;
        for (DWORD c = 0; c < channels; c++) {
            float xm1 = x[c], x0 = x[channels + c], x1 = x[2 * channels + c], x2 = x[3 * channels + c];
            float c1 = 0.5f * (x1 - xm1);
            float c2 = xm1 - 2.5f * x0 + 2.0f * x1 - 0.5f * x2;
            float c3 = 0.5f * (x2 - xm1) + 1.5f * (x0 - x1);
            voice->mix[c * k_frame_samples + j] = (((c3 * t + c2) * t + c1) * t + x0) * gain;
        }
    }
    voice->applied_volume = volume_to;
    // drop what was played, keeping the frame before the new read position
    DWORD advance = (DWORD)position - 1;
    if (advance >= real) {
        // played past the data: a fresh start for whatever comes next
        reset_fifo(voice);
    } else {
        memmove(voice->fifo, voice->fifo + advance * channels, (real - advance) * channels * sizeof(float));
        voice->fifo_frames = real - advance;
        voice->position = position - (double)advance;
    }
}

// True when the voice has played everything it was given.
bool drained(const s_voice *voice) { return voice->packet_count == 0 && (DWORD)voice->position >= voice->fifo_frames; }

void process_source(s_voice *voice)
{
    if (voice->state & XAUDIOSOURCESTATE_STARTING) {
        if (voice->start_hold_frames > 0) {
            voice->start_hold_frames--;
            return;
        }
        voice->state = (BYTE)((voice->state & ~XAUDIOSOURCESTATE_STARTING) | XAUDIOSOURCESTATE_STARTED);
        settle_volumes(voice);
        if (g_trace)
            trace_start(voice);
        return; // runs from the next frame
    }
    if (!(voice->state & XAUDIOSOURCESTATE_STARTED))
        return;
    bool stopping = (voice->state & XAUDIOSOURCESTATE_STOPPING) != 0;
    render_source(voice, stopping ? 1.0f : -1.0f);
    run_effects(voice);
    if (g_trace)
        trace_frame(voice);
    route(voice);
    if (stopping) {
        voice->state &= ~(XAUDIOSOURCESTATE_STOPPING | XAUDIOSOURCESTATE_STARTED);
        if (g_trace)
            trace_end(voice, "stop");
        return;
    }
    if (voice->packet_count == 0) {
        if (voice->state & XAUDIOSOURCESTATE_DISCONTINUITY) {
            if (drained(voice)) {
                voice->state &= ~(XAUDIOSOURCESTATE_STARTED | XAUDIOSOURCESTATE_DISCONTINUITY);
                if (g_trace)
                    trace_end(voice, "drain");
            }
        } else {
            voice->state |= XAUDIOSOURCESTATE_STARVED;
        }
    }
}

// ---- the frame ----------------------------------------------------------------------------------------------------

void run_engine_callbacks(XAUDIOENGINECALLBACKTYPE type)
{
    for (DWORD i = 0; i < sizeof(g_engine_callbacks) / sizeof(g_engine_callbacks[0]); i++)
        if (g_engine_callbacks[i].callback != 0 && g_engine_callbacks[i].type == type)
            g_engine_callbacks[i].callback(g_engine_callbacks[i].context);
}

void call_process_callback(s_voice *voice)
{
    if (voice->on_process == 0)
        return;
    XAUDIOVOICEPROCESSCALLBACK data;
    data.pVoiceContext = voice->context;
    voice->on_process(&data);
}

// Under the API lock: one XAudio frame, into g_output (6 channels, interleaved).
void process_frame()
{
    run_engine_callbacks(XAUDIOENGINECALLBACKTYPE_FRAMESTART);
    memset(g_master->mix, 0, k_master_channels * k_frame_samples * sizeof(float));
    DWORD last_stage = 0;
    for (s_voice *voice = g_voices; voice != 0; voice = voice->next) {
        if (voice->type != k_voice_source) {
            memset(voice->mix, 0, voice->channels * k_frame_samples * sizeof(float));
            if (voice->type == k_voice_submix && voice->stage > last_stage)
                last_stage = voice->stage;
        }
    }
    for (s_voice *voice = g_voices; voice != 0; voice = voice->next) {
        if (voice->type != k_voice_source)
            continue;
        call_process_callback(voice);
        process_source(voice);
    }
    for (DWORD stage = 0; stage <= last_stage; stage++) {
        for (s_voice *voice = g_voices; voice != 0; voice = voice->next) {
            if (voice->type != k_voice_submix || voice->stage != stage)
                continue;
            call_process_callback(voice);
            run_effects(voice);
            if (g_trace)
                trace_frame(voice);
            route(voice);
        }
    }
    call_process_callback(g_master);
    run_effects(g_master);
    if (g_trace) {
        trace_frame(g_master);
        if (g_master->trace_frames >= k_frame_rate / k_frame_samples)
            trace_buses();
    }
    g_engine_frames++;
    for (DWORD i = 0; i < k_frame_samples; i++)
        for (DWORD c = 0; c < k_master_channels; c++)
            g_output[i * k_master_channels + c] = g_master->mix[c * k_frame_samples + i];
    run_engine_callbacks(XAUDIOENGINECALLBACKTYPE_FRAMEEND);
}

DWORD WINAPI engine_thread(LPVOID parameter)
{
    (void)parameter;
    port_audio_open();
    while (!g_stop) {
        if (port_audio_writable() < k_frame_samples) {
            Sleep(1);
            continue;
        }
        EnterCriticalSection(&g_api_lock);
        process_frame();
        LeaveCriticalSection(&g_api_lock);
        port_audio_write(g_output, k_frame_samples);
    }
    return 0;
}

s_voice *new_voice(e_voice_type type, DWORD channels, void *context, LPFNXAUDIOVOICEPROCESSCALLBACK on_process)
{
    s_voice *voice = (s_voice *)allocate(sizeof(s_voice));
    if (voice == 0)
        return 0;
    if (channels == 0)
        channels = 1;
    if (channels > k_max_channels)
        channels = k_max_channels;
    voice->mix = (float *)allocate(channels * k_frame_samples * sizeof(float));
    if (voice->mix == 0) {
        release(voice);
        return 0;
    }
    voice->magic = k_voice_magic;
    voice->type = type;
    voice->references = 1;
    voice->channels = channels;
    voice->context = context;
    voice->on_process = on_process;
    voice->frequency_scale = XAUDIOFREQSCALE_DEFAULT;
    voice->volume = voice->applied_volume = 1.0f;
    voice->trace_id = g_trace_next_id++;
    // the mastering voice is processed apart from the list
    if (type != k_voice_master) {
        voice->next = g_voices;
        g_voices = voice;
    }
    return voice;
}

void free_voice(s_voice *voice)
{
    destroy_effects(voice);
    if (voice->streams != 0) {
        for (DWORD s = 0; s < voice->stream_count; s++)
            release(voice->streams[s].decoder);
        release(voice->streams);
    }
    release(voice->fifo);
    release(voice->mix);
    voice->magic = 0;
    release(voice);
}

void ensure_initialized()
{
    if (g_initialized == 2)
        return;
    if (InterlockedCompareExchange(&g_initialized, 1, 0) == 0) {
        InitializeCriticalSection(&g_api_lock);
        InitializeCriticalSection(&g_spin_lock);
        char trace[8];
        g_trace = port_audio_getenv("HALO3_AUDIO_TRACE", trace, sizeof(trace)) && trace[0] == '1';
        g_master = new_voice(k_voice_master, k_master_channels, 0, 0);
        g_master->stage = 0xff;
        g_stop = 0;
        g_thread = CreateThread(0, 0, engine_thread, 0, 0, 0);
        InterlockedExchange(&g_initialized, 2);
    }
    while (g_initialized != 2)
        SwitchToThread();
}

void register_effects(const XAUDIOFXTABLEENTRY *entries, DWORD count, XAUDIOFXTABLEENTRY *write_back)
{
    for (DWORD i = 0; i < count; i++) {
        DWORD id = entries[i].EffectId;
        if (id == 0) {
            for (id = XAUDIOFXID_DYNMIN; id <= XAUDIOFXID_DYNMAX && g_effect_types[id].create != 0; id++)
                ;
            if (id > XAUDIOFXID_DYNMAX)
                continue;
        }
        g_effect_types[id].query_size = entries[i].pfnQueryEffectSize;
        g_effect_types[id].create = entries[i].pfnCreateEffect;
        if (write_back != 0)
            write_back[i].EffectId = (XAUDIOFXID)id;
    }
}

} // namespace

extern "C" {

// ---- engine ----

// The engine runs from the first XAudio call (voice chat and Bink use voices too); XAudioInitialize adds the
// title's built-in effects. HALO3_NO_AUDIO=1 declines it, as a console without audio would, and the game then
// runs without its DSP voices (as before this engine had sound).
int port_audio_trace_enabled(void)
{
    ensure_initialized();
    return g_trace ? 1 : 0;
}

unsigned port_audio_trace_time_ms(void) { return trace_time_ms(); }

HRESULT XAudioInitialize(LPCXAUDIOENGINEINIT pInit)
{
    if (port_audio_disabled())
        return E_FAIL;
    s_api_lock lock;
    register_effects(k_builtin_effects, sizeof(k_builtin_effects) / sizeof(k_builtin_effects[0]), 0);
    if (pInit != 0 && pInit->pEffectTable != 0 && pInit->pEffectTable->paEffects != 0)
        register_effects(pInit->pEffectTable->paEffects, pInit->pEffectTable->EffectCount, 0);
    if (pInit != 0 && pInit->pMasteringVoiceInit != 0) {
        destroy_effects(g_master);
        create_effects(g_master, pInit->pMasteringVoiceInit->pEffectChain);
        g_master->on_process = pInit->pMasteringVoiceInit->pfnProcessCallback;
        g_master->context = pInit->pMasteringVoiceInit->pContext;
    }
    return S_OK;
}

ULONG XAudioShutDown(void)
{
    if (g_initialized != 2)
        return 0;
    InterlockedExchange(&g_stop, 1);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = 0;
    port_audio_close();
    EnterCriticalSection(&g_api_lock);
    for (s_voice *voice = g_voices; voice != 0;) {
        s_voice *next = voice->next;
        free_voice(voice);
        voice = next;
    }
    g_voices = 0;
    free_voice(g_master);
    g_master = 0;
    memset(g_engine_callbacks, 0, sizeof(g_engine_callbacks));
    memset(g_effect_types, 0, sizeof(g_effect_types));
    LeaveCriticalSection(&g_api_lock);
    InterlockedExchange(&g_initialized, 0);
    return 0;
}

HRESULT XAudioRegisterEngineCallback(XAUDIOENGINECALLBACKTYPE CallbackType, LPFNXAUDIOENGINECALLBACK pfnCallback,
    LPVOID pContext, LPXAUDIOCALLBACKHANDLE phCallbackHandle)
{
    if (pfnCallback == 0 || CallbackType >= XAUDIOENGINECALLBACKTYPE_COUNT)
        return E_INVALIDARG;
    s_api_lock lock;
    for (DWORD i = 0; i < sizeof(g_engine_callbacks) / sizeof(g_engine_callbacks[0]); i++) {
        if (g_engine_callbacks[i].callback == 0) {
            g_engine_callbacks[i].callback = pfnCallback;
            g_engine_callbacks[i].context = pContext;
            g_engine_callbacks[i].type = CallbackType;
            if (phCallbackHandle != 0)
                *phCallbackHandle = &g_engine_callbacks[i];
            return S_OK;
        }
    }
    return E_OUTOFMEMORY;
}

HRESULT XAudioUnregisterEngineCallback(XAUDIOCALLBACKHANDLE hCallbackHandle)
{
    s_api_lock lock;
    s_engine_callback *entry = (s_engine_callback *)hCallbackHandle;
    if (entry < &g_engine_callbacks[0] || entry >= &g_engine_callbacks[16])
        return E_INVALIDARG;
    entry->callback = 0;
    return S_OK;
}

HRESULT XAudioRegisterEffects(LPCXAUDIOFXREGISTER pEffects)
{
    if (pEffects == 0 || (pEffects->EffectCount != 0 && pEffects->paEffects == 0))
        return E_INVALIDARG;
    s_api_lock lock;
    register_effects(pEffects->paEffects, pEffects->EffectCount, pEffects->paEffects);
    return S_OK;
}

HRESULT XAudioUnregisterEffects(LPCXAUDIOFXUNREGISTER pEffects)
{
    if (pEffects == 0 || (pEffects->EffectCount != 0 && pEffects->paEffectIds == 0))
        return E_INVALIDARG;
    s_api_lock lock;
    for (DWORD i = 0; i < pEffects->EffectCount; i++) {
        g_effect_types[pEffects->paEffectIds[i]].query_size = 0;
        g_effect_types[pEffects->paEffectIds[i]].create = 0;
    }
    return S_OK;
}

// The XDK's table of the built-in effects the title passes to XAudioInitialize: the reverb (XAUDIOFXID_REVERB).
// The source, router and render effects are the engine's own and are not created by id.
extern "C" const XAUDIOFXTABLE XAudioDefaultEffectTable = {
    (XAUDIOFXID)(sizeof(k_builtin_effects) / sizeof(k_builtin_effects[0])), k_builtin_effects};

HRESULT XAudioSynchStart(void)
{
    s_api_lock lock;
    for (s_voice *voice = g_voices; voice != 0; voice = voice->next) {
        if (voice->state & XAUDIOSOURCESTATE_SYNCHRONIZED)
            voice->state = (BYTE)((voice->state & ~XAUDIOSOURCESTATE_SYNCHRONIZED) | XAUDIOSOURCESTATE_STARTING);
    }
    return S_OK;
}

HRESULT XAudioQueryPIXCounters(LPXAUDIOPIXCOUNTERS pCounters)
{
    if (pCounters == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    memset(pCounters, 0, sizeof(*pCounters));
    for (s_voice *voice = g_voices; voice != 0; voice = voice->next) {
        if (voice->type == k_voice_submix) {
            pCounters->ActiveVoices_Submix++;
            pCounters->ActiveVoices_Total++;
        } else if (voice->state & XAUDIOSOURCESTATE_STARTED) {
            pCounters->ActiveVoices_Source++;
            pCounters->ActiveVoices_Total++;
            if (is_xma(voice))
                pCounters->ActiveVoices_XMA++;
            else
                pCounters->ActiveVoices_PCM++;
        }
    }
    pCounters->ActiveVoices_Total++; // the mastering voice
    return S_OK;
}

HRESULT XAudioLock(XAUDIOLOCKTYPE LockType)
{
    ensure_initialized();
    CRITICAL_SECTION *lock = (LockType & XAUDIOLOCKTYPE_LOCKMASK) == XAUDIOLOCKTYPE_SPINLOCK ? &g_spin_lock : &g_api_lock;
    switch (LockType & XAUDIOLOCKTYPE_OPERATIONMASK) {
    case XAUDIOLOCKTYPE_LOCK:
        EnterCriticalSection(lock);
        return S_OK;
    case XAUDIOLOCKTYPE_TRYLOCK:
        return TryEnterCriticalSection(lock) ? S_OK : S_FALSE;
    case XAUDIOLOCKTYPE_UNLOCK:
        LeaveCriticalSection(lock);
        return S_OK;
    default:
        return E_INVALIDARG;
    }
}

// The host's output: 5.1 (reported as the digital Dolby Digital output, with which the game uses its surround
// mixes) when the output was opened with six channels (HALO3_AUDIO_CHANNELS=6), else plain stereo.
HRESULT XAudioGetSpeakerConfig(LPXAUDIOSPEAKERCONFIG pConfig)
{
    if (pConfig == 0)
        return E_INVALIDARG;
    unsigned channels = port_audio_disabled() ? 2 : port_audio_open();
    *pConfig = channels == 6 ? (XAUDIOSPEAKERCONFIG_ANALOG_SURROUND | XAUDIOSPEAKERCONFIG_DIGITAL_DOLBYDIGITAL)
                             : XAUDIOSPEAKERCONFIG_ANALOG_STEREOBYPASS;
    return S_OK;
}

// The frame buffer an effect's Process gets (run_effects).
HRESULT XAudioFrameBuffer_GetProcessingData(IXAudioFrameBuffer *pFrameBuffer, LPXAUDIOFRAMEBUFDATA pProcessData)
{
    if (pProcessData == 0)
        return E_INVALIDARG;
    if (pFrameBuffer == 0) {
        memset(pProcessData, 0, sizeof(*pProcessData));
        return E_INVALIDARG;
    }
    *pProcessData = ((const s_frame_buffer *)pFrameBuffer)->data;
    return S_OK;
}

// The engine's frame buffers hold no prepared state to release.
HRESULT XAudioFrameBuffer_Unprepare(IXAudioFrameBuffer *pFrameBuffer) { return pFrameBuffer != 0 ? S_OK : E_INVALIDARG; }

// ---- voices ----

HRESULT XAudioCreateSourceVoice(LPCXAUDIOSOURCEVOICEINIT pInit, IXAudioSourceVoice **ppVoice)
{
    if (pInit == 0 || ppVoice == 0)
        return E_INVALIDARG;
    const XAUDIOSOURCEFORMAT &format = pInit->Format;
    DWORD channels = 0, rate = 0, streams = 0;
    if (format.SampleType == XAUDIOSAMPLETYPE_XMA) {
        streams = format.NumStreams != 0 ? format.NumStreams : 1;
        if (streams > k_max_streams)
            return E_INVALIDARG;
        for (DWORD s = 0; s < streams; s++) {
            if (format.Stream[s].ChannelCount < 1 || format.Stream[s].ChannelCount > 2)
                return E_INVALIDARG;
            channels += format.Stream[s].ChannelCount;
        }
        rate = format.Stream[0].SampleRate;
    } else if (format.SampleType <= XAUDIOSAMPLETYPE_16BITPCMLE) {
        channels = format.ChannelCount;
        rate = format.SampleRate;
    } else {
        return E_INVALIDARG;
    }
    if (channels == 0 || channels > XAUDIOCHANNEL_SOURCEMAX)
        return E_INVALIDARG;
    if (rate == 0)
        rate = k_frame_rate;
    s_api_lock lock;
    s_voice *voice = new_voice(k_voice_source, channels, pInit->pContext, pInit->pfnProcessCallback);
    if (voice == 0)
        return E_OUTOFMEMORY;
    voice->format = format;
    voice->rate = rate;
    voice->sample_bytes = format.SampleType == XAUDIOSAMPLETYPE_FLOAT ? 4 : format.SampleType == XAUDIOSAMPLETYPE_8BITPCM ? 1 : 2;
    voice->max_packets = pInit->MaxPacketCount != 0 && pInit->MaxPacketCount < k_max_packets ? pInit->MaxPacketCount
                                                                                             : k_max_packets;
    voice->on_complete = pInit->pfnPacketCompletionCallback;
    voice->on_loop = pInit->pfnPacketLoopCallback;
    // the pitch range the resampler's buffer is sized for: MaxPitchShift octaves up (XMA voices state it; PCM
    // voices get two), and a frequency scale up to 2
    XAUDIOPITCH octaves = pInit->MaxPitchShift > 0.0f ? pInit->MaxPitchShift : 2.0f;
    if (octaves > 6.0f)
        octaves = 6.0f;
    voice->max_pitch = octaves;
    voice->pitch_scale = 1.0;
    voice->max_step = (double)rate / (double)k_frame_rate * pow(2.0, (double)octaves) * 2.0;
    voice->fifo_capacity = (DWORD)(voice->max_step * k_frame_samples) + 8;
    voice->fifo = (float *)allocate(voice->fifo_capacity * channels * sizeof(float));
    if (voice->fifo == 0) {
        XAudioVoice_Release((IXAudioVoice *)voice);
        return E_OUTOFMEMORY;
    }
    reset_fifo(voice);
    if (streams != 0) {
        voice->streams = (s_xma_stream *)allocate(streams * sizeof(s_xma_stream));
        if (voice->streams == 0) {
            XAudioVoice_Release((IXAudioVoice *)voice);
            return E_OUTOFMEMORY;
        }
        voice->stream_count = streams;
        DWORD first = 0;
        for (DWORD s = 0; s < streams; s++) {
            s_xma_stream &stream = voice->streams[s];
            stream.channels = format.Stream[s].ChannelCount;
            stream.first_channel = (BYTE)first;
            first += stream.channels;
            stream.decoder = (xma_decoder *)allocate(xma_decoder_size());
            if (stream.decoder == 0 || !xma_decoder_init(stream.decoder, format.Stream[s].SampleRate != 0 ?
                    format.Stream[s].SampleRate : rate, stream.channels)) {
                XAudioVoice_Release((IXAudioVoice *)voice);
                return E_OUTOFMEMORY;
            }
            stream.decoded_frame = -1;
        }
    }
    create_effects(voice, pInit->pEffectChain);
    set_outputs(voice, pInit->pVoiceOutput);
    *ppVoice = (IXAudioSourceVoice *)voice;
    return S_OK;
}

HRESULT XAudioCreateSubmixVoice(LPCXAUDIOSUBMIXVOICEINIT pInit, IXAudioSubmixVoice **ppVoice)
{
    if (pInit == 0 || ppVoice == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = new_voice(k_voice_submix, pInit->Format.ChannelCount, pInit->pContext, pInit->pfnProcessCallback);
    if (voice == 0)
        return E_OUTOFMEMORY;
    voice->stage = pInit->SubmixStage;
    create_effects(voice, pInit->pEffectChain);
    set_outputs(voice, pInit->pVoiceOutput);
    *ppVoice = (IXAudioSubmixVoice *)voice;
    return S_OK;
}

HRESULT XAudioGetMasteringVoice(IXAudioMasteringVoice **ppVoice)
{
    if (ppVoice == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    *ppVoice = (IXAudioMasteringVoice *)g_master;
    return S_OK;
}

ULONG XAudioVoice_AddRef(IXAudioVoice *pVoice)
{
    s_api_lock lock;
    s_voice *voice = voice_of(pVoice);
    return voice != 0 ? (ULONG)++voice->references : 0;
}

ULONG XAudioVoice_Release(IXAudioVoice *pVoice)
{
    s_api_lock lock;
    s_voice *voice = voice_of(pVoice);
    if (voice == 0 || voice->type == k_voice_master)
        return 0;
    if (--voice->references != 0)
        return (ULONG)voice->references;
    if (g_trace && voice->type == k_voice_source && (voice->state & XAUDIOSOURCESTATE_STARTED))
        trace_end(voice, "release");
    for (s_voice **link = &g_voices; *link != 0; link = &(*link)->next) {
        if (*link == voice) {
            *link = voice->next;
            break;
        }
    }
    // outputs of other voices that went to this one go nowhere now
    for (s_voice *other = g_voices; other != 0; other = other->next)
        for (DWORD r = 0; r < other->output_count; r++)
            if (other->outputs[r].destination == voice)
                other->outputs[r].destination = 0;
    free_voice(voice);
    return 0;
}

HRESULT XAudioVoice_GetVoiceState(IXAudioVoice *pVoice, LPXAUDIOSOURCESTATE pState)
{
    if (pState == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    BYTE state = voice->state;
    if (voice->packet_count < voice->max_packets)
        state |= XAUDIOSOURCESTATE_READYPACKET;
    *pState = state;
    return S_OK;
}

HRESULT XAudioVoice_GetVoiceType(IXAudioVoice *pVoice, LPXAUDIOVOICETYPE pType)
{
    if (pType == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = voice_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    *pType = voice->type == k_voice_source ? XAUDIOVOICETYPE_SOURCE
             : voice->type == k_voice_submix ? XAUDIOVOICETYPE_SUBMIX : XAUDIOVOICETYPE_MASTERING;
    return S_OK;
}

HRESULT XAudioVoice_GetVoiceContext(IXAudioVoice *pVoice, LPVOID *ppContext)
{
    if (ppContext == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = voice_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    *ppContext = voice->context;
    return S_OK;
}

HRESULT XAudioVoice_GetEffectState(IXAudioVoice *pVoice, XAUDIOVOICEFXINDEX EffectIndex, LPXAUDIOVOICEFXSTATE pEffectState)
{
    if (pEffectState == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = voice_of(pVoice);
    if (voice == 0 || EffectIndex >= voice->effect_count)
        return E_INVALIDARG;
    *pEffectState = voice->effects[EffectIndex].enabled ? XAUDIOVOICEFXSTATE_ENABLED : 0;
    return S_OK;
}

HRESULT XAudioVoice_SetEffectState(IXAudioVoice *pVoice, XAUDIOVOICEFXINDEX EffectIndex, XAUDIOVOICEFXSTATE EffectState)
{
    s_api_lock lock;
    s_voice *voice = voice_of(pVoice);
    if (voice == 0 || EffectIndex >= voice->effect_count)
        return E_INVALIDARG;
    voice->effects[EffectIndex].enabled = (EffectState & XAUDIOVOICEFXSTATE_ENABLED) != 0;
    return S_OK;
}

HRESULT XAudioVoice_GetEffectParam(IXAudioVoice *pVoice, XAUDIOVOICEFXINDEX EffectIndex, XAUDIOFXPARAMID ParamId,
    XAUDIOFXPARAMTYPE ParamType, LPXAUDIOFXPARAM pParam)
{
    s_api_lock lock;
    s_voice *voice = voice_of(pVoice);
    if (voice == 0 || EffectIndex >= voice->effect_count)
        return E_INVALIDARG;
    IXAudioEffect *effect = voice->effects[EffectIndex].effect;
    return effect != 0 ? effect->GetParam(ParamId, ParamType, pParam) : S_OK;
}

HRESULT XAudioVoice_SetEffectParam(IXAudioVoice *pVoice, XAUDIOVOICEFXINDEX EffectIndex, XAUDIOFXPARAMID ParamId,
    XAUDIOFXPARAMTYPE ParamType, LPCXAUDIOFXPARAM pParam)
{
    s_api_lock lock;
    s_voice *voice = voice_of(pVoice);
    if (voice == 0 || EffectIndex >= voice->effect_count)
        return E_INVALIDARG;
    IXAudioEffect *effect = voice->effects[EffectIndex].effect;
    return effect != 0 ? effect->SetParam(ParamId, ParamType, pParam) : S_OK; // an unregistered effect takes any
}

HRESULT XAudioVoice_SetVoiceOutput(IXAudioVoice *pVoice, LPCXAUDIOVOICEOUTPUT pVoiceOutput)
{
    s_api_lock lock;
    s_voice *voice = voice_of(pVoice);
    if (voice == 0 || voice->type == k_voice_master)
        return E_INVALIDARG;
    set_outputs(voice, pVoiceOutput);
    return S_OK;
}

HRESULT XAudioVoice_SetVoiceOutputVolume(IXAudioVoice *pVoice, LPCXAUDIOVOICEOUTPUTVOLUME pVolume)
{
    if (pVolume == 0 || (pVolume->EntryCount != 0 && pVolume->paEntries == 0))
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = voice_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    voice->start_hold_frames = 0;
    for (DWORD i = 0; i < pVolume->EntryCount; i++) {
        const XAUDIOVOICEOUTPUTVOLUMEENTRY &entry = pVolume->paEntries[i];
        if (entry.OutputVoiceIndex >= voice->output_count || entry.pVolume == 0 || entry.pVolume->paEntries == 0)
            continue;
        s_route &route = voice->outputs[entry.OutputVoiceIndex];
        for (DWORD k = 0; k < entry.pVolume->EntryCount; k++) {
            const XAUDIOCHANNELVOLUMEENTRY &volume = entry.pVolume->paEntries[k];
            if (volume.EntryIndex < route.count)
                route.entries[volume.EntryIndex].volume = volume.Volume;
        }
    }
    return S_OK;
}

HRESULT XAudioSourceVoice_GetVoiceFormat(IXAudioSourceVoice *pVoice, LPXAUDIOSOURCEFORMAT pFormat)
{
    if (pFormat == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    *pFormat = voice->format;
    return S_OK;
}

HRESULT XAudioSubmixVoice_GetVoiceFormat(IXAudioSubmixVoice *pVoice, LPXAUDIOFORMAT pFormat)
{
    if (pFormat == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = voice_of(pVoice);
    if (voice == 0 || voice->type != k_voice_submix)
        return E_INVALIDARG;
    pFormat->UnionValue = 0;
    pFormat->SampleType = XAUDIOSAMPLETYPE_FLOAT;
    pFormat->ChannelCount = (XAUDIOCHANNEL)voice->channels;
    pFormat->SampleRate = k_frame_rate;
    return S_OK;
}

HRESULT XAudioMasteringVoice_GetVoiceFormat(IXAudioMasteringVoice *pVoice, LPXAUDIOFORMAT pFormat)
{
    if (pFormat == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    (void)pVoice;
    pFormat->UnionValue = 0;
    pFormat->SampleType = XAUDIOSAMPLETYPE_FLOAT;
    pFormat->ChannelCount = (XAUDIOCHANNEL)k_master_channels;
    pFormat->SampleRate = k_frame_rate;
    return S_OK;
}

HRESULT XAudioSourceVoice_SubmitPacket(IXAudioSourceVoice *pVoice, LPCXAUDIOPACKET pPacket, XAUDIOSUBMITPACKET Flags)
{
    if (pPacket == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    s_packet packet = {};
    packet.context = pPacket->pContext;
    packet.data = (const BYTE *)pPacket->pBuffer;
    packet.bytes = pPacket->BufferSize;
    packet.end = packet_samples(voice, pPacket->pBuffer, pPacket->BufferSize);
    packet.loops_left = pPacket->LoopCount;
    if (is_xma(voice)) {
        // XMA loop regions are bit offsets of frames (stream 0's), with subframes (128 samples) cut off the loop's
        // last frame and skipped at its first
        const XMALOOPREGION &loop = pPacket->XMALoop[0];
        packet.loop_begin = 0;
        packet.loop_end = packet.end;
        if (packet.loops_left != 0 && loop.LoopEnd > loop.LoopStart) {
            DWORD begin = xma_frame_sample(voice, packet.data, packet.bytes, loop.LoopStart, 0);
            DWORD end = xma_frame_sample(voice, packet.data, packet.bytes, loop.LoopEnd, packet.end);
            begin += loop.LoopSubframeSkip * XMA_SUBFRAME_SAMPLES;
            if (end < packet.end)
                end += (loop.LoopSubframeEnd + 1u) * XMA_SUBFRAME_SAMPLES;
            packet.loop_begin = begin;
            packet.loop_end = end < packet.end ? end : packet.end;
        }
    } else {
        packet.loop_begin = pPacket->LoopStart;
        packet.loop_end = pPacket->LoopLength != 0 ? pPacket->LoopStart + pPacket->LoopLength : packet.end;
        if (packet.loop_end > packet.end)
            packet.loop_end = packet.end;
    }
    return queue_packet(voice, packet, Flags);
}

HRESULT XAudioSourceVoice_SubmitSourceBuffer(IXAudioSourceVoice *pVoice, LPCXAUDIOSOURCEBUFFER pSourceBuffer,
    XAUDIOSUBMITPACKET Flags)
{
    if (pSourceBuffer == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    s_packet packet = {};
    packet.context = pSourceBuffer->pContext;
    packet.data = (const BYTE *)pSourceBuffer->pBuffer;
    packet.bytes = pSourceBuffer->BufferSize;
    DWORD total = packet_samples(voice, pSourceBuffer->pBuffer, pSourceBuffer->BufferSize);
    packet.end = pSourceBuffer->PlayEnd != 0 && pSourceBuffer->PlayEnd < total ? pSourceBuffer->PlayEnd : total;
    packet.position = pSourceBuffer->PlayBegin < packet.end ? pSourceBuffer->PlayBegin : packet.end;
    packet.loops_left = pSourceBuffer->LoopCount;
    packet.loop_begin = pSourceBuffer->LoopBegin;
    packet.loop_end = pSourceBuffer->LoopEnd < packet.end ? pSourceBuffer->LoopEnd : packet.end;
    return queue_packet(voice, packet, Flags);
}

HRESULT XAudioSourceVoice_FlushPackets(IXAudioSourceVoice *pVoice)
{
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    flush_packets(voice);
    return S_OK;
}

HRESULT XAudioSourceVoice_FlushSourceBuffers(IXAudioSourceVoice *pVoice) { return XAudioSourceVoice_FlushPackets(pVoice); }

HRESULT XAudioSourceVoice_Start(IXAudioSourceVoice *pVoice, XAUDIOSTART Flags)
{
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    voice->state &= ~(XAUDIOSOURCESTATE_STOPPING | XAUDIOSOURCESTATE_STARVED);
    if (voice->state & XAUDIOSOURCESTATE_STARTED)
        return S_OK;
    if (Flags & XAUDIOSTART_SYNCHRONIZED) {
        voice->state |= XAUDIOSOURCESTATE_SYNCHRONIZED;
        voice->start_hold_frames = 2;
    } else {
        voice->state |= XAUDIOSOURCESTATE_STARTING;
        voice->start_hold_frames = 0;
    }
    return S_OK;
}

HRESULT XAudioSourceVoice_Stop(IXAudioSourceVoice *pVoice, XAUDIOSTOP Flags)
{
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    voice->state &= ~(XAUDIOSOURCESTATE_STARTING | XAUDIOSOURCESTATE_SYNCHRONIZED | XAUDIOSOURCESTATE_STARVED);
    if (Flags & XAUDIOSTOP_IMMEDIATE) {
        if (g_trace && (voice->state & XAUDIOSOURCESTATE_STARTED))
            trace_end(voice, "immediate");
        voice->state &= ~(XAUDIOSOURCESTATE_STARTED | XAUDIOSOURCESTATE_STOPPING);
    } else if (voice->state & XAUDIOSOURCESTATE_STARTED)
        voice->state |= XAUDIOSOURCESTATE_STOPPING;
    return S_OK;
}

HRESULT XAudioSourceVoice_Discontinuity(IXAudioSourceVoice *pVoice)
{
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    voice->state |= XAUDIOSOURCESTATE_DISCONTINUITY;
    return S_OK;
}

// The packet at the head of the queue continues from a new position: a sample (PCM), or the frame at a bit offset
// plus a sample into it (XMA). What was read ahead is dropped.
HRESULT XAudioSourceVoice_SetPacketPosition(IXAudioSourceVoice *pVoice, LPCXAUDIOPACKETPOSITION pPosition)
{
    if (pPosition == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0 || voice->packet_count == 0)
        return voice == 0 ? E_INVALIDARG : E_FAIL;
    s_packet &packet = voice->packets[voice->packet_head];
    DWORD position;
    if (is_xma(voice)) {
        position = xma_frame_sample(voice, packet.data, packet.bytes, pPosition->XMAPosition[0].ReadOffset, 0) +
                   pPosition->XMAPosition[0].SampleIndex;
    } else {
        position = pPosition->Position;
    }
    packet.position = position < packet.end ? position : packet.end;
    reset_fifo(voice);
    return S_OK;
}

HRESULT XAudioSourceVoice_GetPacketPosition(IXAudioSourceVoice *pVoice, LPXAUDIOPACKETPOSITION pPosition)
{
    if (pPosition == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0 || voice->packet_count == 0)
        return voice == 0 ? E_INVALIDARG : E_FAIL;
    const s_packet &packet = voice->packets[voice->packet_head];
    memset(pPosition, 0, sizeof(*pPosition));
    if (is_xma(voice)) {
        DWORD frame = packet.position / XMA_SAMPLES_PER_FRAME;
        xma_cursor cursor;
        xma_cursor_init(&cursor, packet.data, packet.bytes, 0);
        for (DWORD f = 0; f < frame && xma_cursor_skip_frame(&cursor) != 0; f++)
            ;
        pPosition->XMAPosition[0].ReadOffset = xma_cursor_tell_bits(&cursor);
        pPosition->XMAPosition[0].SampleIndex = (WORD)(packet.position % XMA_SAMPLES_PER_FRAME);
    } else {
        pPosition->Position = packet.position;
    }
    return S_OK;
}

HRESULT XAudioSourceVoice_GetStreamPosition(IXAudioSourceVoice *pVoice, LPDWORD pPosition)
{
    if (pPosition == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    *pPosition = voice->stream_position;
    return S_OK;
}

HRESULT XAudioSourceVoice_GetPacketLoopCount(IXAudioSourceVoice *pVoice, LPDWORD pLoopCount)
{
    if (pLoopCount == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0 || voice->packet_count == 0)
        return voice == 0 ? E_INVALIDARG : E_FAIL;
    *pLoopCount = voice->packets[voice->packet_head].loops_left;
    return S_OK;
}

HRESULT XAudioSourceVoice_SetPacketLoopCount(IXAudioSourceVoice *pVoice, DWORD LoopCount)
{
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0 || voice->packet_count == 0)
        return voice == 0 ? E_INVALIDARG : E_FAIL;
    voice->packets[voice->packet_head].loops_left = LoopCount;
    return S_OK;
}

HRESULT XAudioSourceVoice_GetPacketContext(IXAudioSourceVoice *pVoice, LPVOID *ppContext)
{
    if (ppContext == 0)
        return E_INVALIDARG;
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0 || voice->packet_count == 0)
        return voice == 0 ? E_INVALIDARG : E_FAIL;
    *ppContext = voice->packets[voice->packet_head].context;
    return S_OK;
}

HRESULT XAudioSourceVoice_GetPitch(IXAudioSourceVoice *pVoice, LPXAUDIOPITCH pPitch)
{
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0 || pPitch == 0)
        return E_INVALIDARG;
    *pPitch = voice->pitch;
    return S_OK;
}

HRESULT XAudioSourceVoice_SetPitch(IXAudioSourceVoice *pVoice, XAUDIOPITCH Pitch)
{
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    XAUDIOPITCH clamped = Pitch > voice->max_pitch ? voice->max_pitch : Pitch;
    if (voice->pitch != clamped) {
        voice->pitch = clamped;
        voice->pitch_scale = pow(2.0, (double)clamped);
    }
    return S_OK;
}

HRESULT XAudioSourceVoice_GetFrequencyScale(IXAudioSourceVoice *pVoice, LPXAUDIOFREQSCALE pScale)
{
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0 || pScale == 0)
        return E_INVALIDARG;
    *pScale = voice->frequency_scale;
    return S_OK;
}

HRESULT XAudioSourceVoice_SetFrequencyScale(IXAudioSourceVoice *pVoice, XAUDIOFREQSCALE Scale)
{
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0 || !(Scale > 0.0f))
        return E_INVALIDARG;
    voice->frequency_scale = Scale > 2.0f ? 2.0f : Scale;
    return S_OK;
}

HRESULT XAudioSourceVoice_GetVolume(IXAudioSourceVoice *pVoice, LPXAUDIOVOLUME pVolume)
{
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0 || pVolume == 0)
        return E_INVALIDARG;
    *pVolume = voice->volume;
    return S_OK;
}

HRESULT XAudioSourceVoice_SetVolume(IXAudioSourceVoice *pVoice, XAUDIOVOLUME Volume)
{
    s_api_lock lock;
    s_voice *voice = source_of(pVoice);
    if (voice == 0)
        return E_INVALIDARG;
    voice->volume = Volume;
    return S_OK;
}

// ---- X3DAudio ----
//
// The handle (X3DAUDIO_HANDLE, 20 bytes) keeps the speaker mask, its channel count and the speed of sound. The
// calculation follows the X3DAudio model: the volume, LFE, LPF and reverb curves of the emitter (or the documented
// defaults) at the normalized distance, the cone of a single-channel emitter, equal-power panning of each emitter
// channel between the two output speakers around its direction in the listener's frame (the center speaker left out
// with X3DAUDIO_CALCULATE_ZEROCENTER, the LFE channel fed by the LFE curve), the emitter-to-listener angle, and the
// doppler factor from the velocity components along the line between them.

} // extern "C"

namespace {

struct s_x3daudio_instance {
    UINT32 speaker_mask;
    UINT32 channel_count;
    FLOAT32 speed_of_sound;
    UINT32 magic;
    UINT32 reserved;
};
typedef char x3daudio_instance_fits[sizeof(s_x3daudio_instance) <= X3DAUDIO_HANDLE_BYTESIZE ? 1 : -1];

const UINT32 k_x3daudio_magic = 0x58334441; // 'X3DA'

struct s_vec { float x, y, z; };
s_vec vec(const X3DAUDIO_VECTOR &v) { s_vec r = {v.x, v.y, v.z}; return r; }
s_vec sub(s_vec a, s_vec b) { s_vec r = {a.x - b.x, a.y - b.y, a.z - b.z}; return r; }
float dot(s_vec a, s_vec b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
s_vec cross(s_vec a, s_vec b) { s_vec r = {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; return r; }
float length(s_vec a) { return sqrtf(dot(a, a)); }

float curve_at(const X3DAUDIO_DISTANCE_CURVE *curve, float distance, float default0, float default1)
{
    if (curve == 0 || curve->pPoints == 0 || curve->PointCount < 2) {
        float t = distance < 0.0f ? 0.0f : distance > 1.0f ? 1.0f : distance;
        return default0 + (default1 - default0) * t;
    }
    const X3DAUDIO_DISTANCE_CURVE_POINT *p = curve->pPoints;
    if (distance <= p[0].Distance)
        return p[0].DSPSetting;
    for (UINT32 i = 1; i < curve->PointCount; i++) {
        if (distance <= p[i].Distance) {
            float span = p[i].Distance - p[i - 1].Distance;
            float t = span > 0.0f ? (distance - p[i - 1].Distance) / span : 1.0f;
            return p[i - 1].DSPSetting + (p[i].DSPSetting - p[i - 1].DSPSetting) * t;
        }
    }
    return p[curve->PointCount - 1].DSPSetting;
}

// Default volume / LFE curve: inverse distance (the inverse square law in intensity), no attenuation up to 1.
float inverse_distance(float distance) { return distance <= 1.0f ? 1.0f : 1.0f / distance; }

// Speaker azimuths (radians clockwise from front) in mask order; LFE is marked by -1.
UINT32 speaker_azimuths(UINT32 mask, float *azimuths, UINT32 capacity)
{
    static const struct { UINT32 bit; float azimuth; } k_speakers[] = {
        {SPEAKER_FRONT_LEFT, -0.523598776f}, {SPEAKER_FRONT_RIGHT, 0.523598776f}, {SPEAKER_FRONT_CENTER, 0.0f},
        {SPEAKER_LOW_FREQUENCY, -1.0f}, {SPEAKER_BACK_LEFT, -2.443460953f}, {SPEAKER_BACK_RIGHT, 2.443460953f},
        {SPEAKER_FRONT_LEFT_OF_CENTER, -0.261799388f}, {SPEAKER_FRONT_RIGHT_OF_CENTER, 0.261799388f},
        {SPEAKER_BACK_CENTER, 3.141592654f}, {SPEAKER_SIDE_LEFT, -1.570796327f}, {SPEAKER_SIDE_RIGHT, 1.570796327f},
    };
    UINT32 count = 0;
    for (UINT32 i = 0; i < sizeof(k_speakers) / sizeof(k_speakers[0]) && count < capacity; i++)
        if (mask & k_speakers[i].bit)
            azimuths[count++] = k_speakers[i].azimuth;
    return count;
}

float wrap_angle(float a)
{
    while (a > 3.141592654f)
        a -= 6.283185307f;
    while (a < -3.141592654f)
        a += 6.283185307f;
    return a;
}

// Equal-power gains for a source at `azimuth` over the speakers (center excluded when zero_center).
void pan(float azimuth, const float *speakers, UINT32 count, bool zero_center, float gain, float *out)
{
    int left = -1, right = -1;
    float left_gap = 7.0f, right_gap = 7.0f;
    for (UINT32 i = 0; i < count; i++) {
        if (speakers[i] == -1.0f || (zero_center && speakers[i] == 0.0f))
            continue;
        float d = wrap_angle(speakers[i] - azimuth);
        if (d >= 0.0f && d < right_gap) { right_gap = d; right = (int)i; }
        if (d <= 0.0f && -d < left_gap) { left_gap = -d; left = (int)i; }
    }
    if (left < 0 && right < 0)
        return;
    if (left < 0 || right < 0 || left == right) {
        out[left >= 0 ? left : right] += gain;
        return;
    }
    float t = left_gap / (left_gap + right_gap);
    out[left] += gain * cosf(t * 1.570796327f);
    out[right] += gain * sinf(t * 1.570796327f);
}

} // namespace

extern "C" {

void X3DAudioInitialize(UINT32 SpeakerChannelMask, FLOAT32 SpeedOfSound, X3DAUDIO_HANDLE Instance)
{
    s_x3daudio_instance *instance = (s_x3daudio_instance *)Instance;
    memset(Instance, 0, X3DAUDIO_HANDLE_BYTESIZE);
    instance->speaker_mask = SpeakerChannelMask;
    UINT32 channels = 0;
    for (UINT32 bit = SpeakerChannelMask; bit != 0; bit &= bit - 1)
        channels++;
    instance->channel_count = channels;
    instance->speed_of_sound = SpeedOfSound;
    instance->magic = k_x3daudio_magic;
}

void X3DAudioCalculate(const X3DAUDIO_HANDLE Instance, const X3DAUDIO_LISTENER *pListener,
    const X3DAUDIO_EMITTER *pEmitter, UINT32 Flags, X3DAUDIO_DSP_SETTINGS *pDSPSettings)
{
    const s_x3daudio_instance *instance = (const s_x3daudio_instance *)Instance;
    if (pListener == 0 || pEmitter == 0 || pDSPSettings == 0 || instance->magic != k_x3daudio_magic)
        return;
    s_vec to_listener = sub(vec(pListener->Position), vec(pEmitter->Position));
    float distance = length(to_listener);
    pDSPSettings->EmitterToListenerDistance = distance;
    const float scaler = pEmitter->CurveDistanceScaler > 0.0f ? pEmitter->CurveDistanceScaler : 1.0f;
    const float normalized = distance / scaler;
    s_vec direction = distance > 0.0f ? s_vec{to_listener.x / distance, to_listener.y / distance, to_listener.z / distance}
                                      : s_vec{0.0f, 0.0f, 0.0f};

    // the cone of a single-channel emitter, by the angle between its front and the listener
    float cone_volume = 1.0f, cone_lpf = 1.0f, cone_reverb = 1.0f;
    float emitter_angle = 0.0f;
    if (distance > 0.0f) {
        float c = dot(vec(pEmitter->OrientFront), direction);
        emitter_angle = acosf(c < -1.0f ? -1.0f : c > 1.0f ? 1.0f : c);
    }
    if (pEmitter->pCone != 0 && pEmitter->ChannelCount == 1) {
        const X3DAUDIO_CONE &cone = *pEmitter->pCone;
        float inner = cone.InnerAngle * 0.5f, outer = cone.OuterAngle * 0.5f;
        float t = emitter_angle <= inner ? 0.0f : emitter_angle >= outer ? 1.0f
                  : (emitter_angle - inner) / (outer - inner > 0.0f ? outer - inner : 1.0f);
        cone_volume = cone.InnerVolume + (cone.OuterVolume - cone.InnerVolume) * t;
        cone_lpf = cone.InnerLPF + (cone.OuterLPF - cone.InnerLPF) * t;
        cone_reverb = cone.InnerReverb + (cone.OuterReverb - cone.InnerReverb) * t;
    }
    if (Flags & X3DAUDIO_CALCULATE_EMITTER_ANGLE)
        pDSPSettings->EmitterToListenerAngle = emitter_angle;

    if (Flags & X3DAUDIO_CALCULATE_MATRIX) {
        const UINT32 sources = pDSPSettings->SrcChannelCount, outputs = pDSPSettings->DstChannelCount;
        float *matrix = pDSPSettings->pMatrixCoefficients;
        float speakers[32];
        UINT32 speaker_count = speaker_azimuths(instance->speaker_mask, speakers, 32);
        if (speaker_count > outputs)
            speaker_count = outputs;
        float volume = (pEmitter->pVolumeCurve != 0 ? curve_at(pEmitter->pVolumeCurve, normalized, 1.0f, 0.0f)
                                                    : inverse_distance(normalized)) * cone_volume;
        float lfe = pEmitter->pLFECurve != 0 ? curve_at(pEmitter->pLFECurve, normalized, 1.0f, 0.0f)
                                             : inverse_distance(normalized);
        // the source's direction in the listener's frame: azimuth clockwise from front about the top axis
        s_vec front = vec(pListener->OrientFront), top = vec(pListener->OrientTop);
        s_vec right = cross(top, front);
        s_vec from_listener = {-direction.x, -direction.y, -direction.z};
        float azimuth = distance > 0.0f ? atan2f(dot(from_listener, right), dot(from_listener, front)) : 0.0f;
        bool inside = distance <= pEmitter->InnerRadius;
        for (UINT32 s = 0; matrix != 0 && s < sources; s++) {
            float row[32] = {0};
            float channel_azimuth = pEmitter->pChannelAzimuths != 0 && sources > 1 ? pEmitter->pChannelAzimuths[s] : 0.0f;
            if (sources > 1 && channel_azimuth >= X3DAUDIO_2PI) {
                for (UINT32 d = 0; d < speaker_count; d++)
                    if (speakers[d] == -1.0f)
                        row[d] = lfe;
            } else if (inside) {
                // within the inner radius the sound surrounds the listener: equal power on every speaker
                UINT32 used = 0;
                for (UINT32 d = 0; d < speaker_count; d++)
                    if (speakers[d] != -1.0f && !((Flags & X3DAUDIO_CALCULATE_ZEROCENTER) && speakers[d] == 0.0f))
                        used++;
                for (UINT32 d = 0; used != 0 && d < speaker_count; d++)
                    if (speakers[d] != -1.0f && !((Flags & X3DAUDIO_CALCULATE_ZEROCENTER) && speakers[d] == 0.0f))
                        row[d] = volume / sqrtf((float)used);
            } else {
                pan(wrap_angle(azimuth + channel_azimuth), speakers, speaker_count,
                    (Flags & X3DAUDIO_CALCULATE_ZEROCENTER) != 0, volume, row);
            }
            for (UINT32 d = 0; d < outputs; d++)
                matrix[sources * d + s] = d < 32 ? row[d] : 0.0f;
        }
    }
    if ((Flags & X3DAUDIO_CALCULATE_DELAY) && pDSPSettings->pDelayTimes != 0)
        for (UINT32 d = 0; d < pDSPSettings->DstChannelCount; d++)
            pDSPSettings->pDelayTimes[d] = 0.0f;
    if (Flags & X3DAUDIO_CALCULATE_LPF_DIRECT)
        pDSPSettings->LPFDirectCoefficient = curve_at(pEmitter->pLPFDirectCurve, normalized, 1.0f, 0.75f) * cone_lpf;
    if (Flags & X3DAUDIO_CALCULATE_LPF_REVERB)
        pDSPSettings->LPFReverbCoefficient = curve_at(pEmitter->pLPFReverbCurve, normalized, 0.75f, 0.75f) * cone_lpf;
    if (Flags & X3DAUDIO_CALCULATE_REVERB)
        pDSPSettings->ReverbLevel = curve_at(pEmitter->pReverbCurve, normalized, 1.0f, 0.0f) * cone_reverb;
    if (Flags & X3DAUDIO_CALCULATE_DOPPLER) {
        float speed = instance->speed_of_sound > 0.0f ? instance->speed_of_sound : X3DAUDIO_SPEED_OF_SOUND;
        float emitter_component = dot(vec(pEmitter->Velocity), direction);
        float listener_component = dot(vec(pListener->Velocity), direction);
        pDSPSettings->EmitterVelocityComponent = emitter_component;
        pDSPSettings->ListenerVelocityComponent = listener_component;
        float scale = pEmitter->DopplerScaler;
        if (scale <= 0.0f || distance == 0.0f) {
            pDSPSettings->DopplerFactor = 1.0f;
        } else {
            // moving towards each other raises the pitch; components are clamped below the speed of sound
            float e = scale * emitter_component, l = scale * listener_component;
            float limit = speed * 0.999f;
            e = e > limit ? limit : e < -limit ? -limit : e;
            l = l > limit ? limit : l < -limit ? -limit : l;
            pDSPSettings->DopplerFactor = (speed - l) / (speed - e);
        }
    }
}

} // extern "C"
