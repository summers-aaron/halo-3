// port/sdk/xhv (both hosts): the voice codec of the port's XHV engine (xhv_codec.c), host C with no library.
//
// Halo 3 carries voice in packets of exactly XHV_VOICECHAT_MODE_PACKET_SIZE (10) bytes: voice_xenon.cpp asks the
// engine for at most 10 of them a poll (a 100-byte buffer), its voice queue keeps 16 a player and clears itself when
// full, and voice_queue_received_packet drops anything that is not 10 bytes. The codec is sized for that channel:
// narrowband speech (8 kHz) in frames of 20 ms, each frame two packets, 100 packets (1000 bytes) a second -- about
// the share of the game's voice bandwidth (stream_voice_shared_desired_bps, 12288 bit/s) one talker gets.
//
// The coder is an algebraic CELP (the family of ITU-T G.729, written here from the textbook structure, with its own
// tables): a 10th-order LPC envelope as line spectral frequencies (MA-predicted, scalar quantized), four 5 ms
// subframes each with an adaptive (pitch) codebook at half-sample resolution, a 4-pulse algebraic codebook with pitch
// sharpening, and a joint pitch / innovation gain with MA-predicted innovation energy. The decoder adds an adaptive
// postfilter (long-term, short-term, tilt, gain control) and conceals lost frames.
//
// The interface is the engine's 16 kHz (XHV_PCM_SAMPLE_RATE) 16-bit mono: the encoder band-limits and decimates, the
// decoder interpolates back. Packet layout (bits MSB first): packet A = 0, seq:3, reset:1, payload[0..74];
// packet B = 1, seq:3, payload[75..150]. seq counts frames modulo 8; reset marks the first frame of a talk spurt (the
// encoder started from its initial state, so the decoder resets too).
#ifndef PORT_XHV_CODEC_H
#define PORT_XHV_CODEC_H

#ifdef __cplusplus
extern "C" {
#endif

#define XHV_CODEC_PACKET_BYTES 10
#define XHV_CODEC_FRAME_PACKETS 2
#define XHV_CODEC_FRAME_BYTES (XHV_CODEC_PACKET_BYTES * XHV_CODEC_FRAME_PACKETS)
#define XHV_CODEC_PCM_RATE 16000
#define XHV_CODEC_FRAME_SAMPLES 320 // 20 ms at 16 kHz
#define XHV_CODEC_SEQ_MODULO 8

// Internal sizes, here so callers can hold the state by value (no allocator).
#define XHV_CODEC_ORDER 10
#define XHV_CODEC_NB_FRAME 160    // 20 ms at 8 kHz
#define XHV_CODEC_NB_SUBFRAME 40
#define XHV_CODEC_PITCH_MAX 147
#define XHV_CODEC_EXC_HISTORY 160 // PITCH_MAX + interpolation taps, rounded up
#define XHV_CODEC_RESAMPLE_TAPS 80

typedef struct xhv_resampler {
    float history[XHV_CODEC_RESAMPLE_TAPS];
} xhv_resampler;

typedef struct xhv_encoder {
    xhv_resampler decimator;
    float hpf_x[2], hpf_y[2];
    float speech[XHV_CODEC_ORDER + 240]; // M of history, then the 240-sample analysis window (40 old, 160, 40 ahead)
    float wsp[XHV_CODEC_PITCH_MAX + XHV_CODEC_NB_FRAME];
    float exc[XHV_CODEC_EXC_HISTORY + XHV_CODEC_NB_FRAME];
    float mem_syn[XHV_CODEC_ORDER], mem_err[XHV_CODEC_ORDER], mem_w0[XHV_CODEC_ORDER], mem_w[XHV_CODEC_ORDER];
    float lsf_old[XHV_CODEC_ORDER];   // unquantized, last frame
    float lsfq_old[XHV_CODEC_ORDER];  // quantized, last frame
    float lsf_residual[XHV_CODEC_ORDER]; // the MA predictor's memory (quantized residual of the last frame)
    float past_energy[4];             // the innovation gain predictor's memory (dB)
    float sharp;                      // pitch sharpening factor (the last quantized pitch gain, clamped)
    unsigned seq;
    int fresh; // the next frame is the first since a reset
} xhv_encoder;

typedef struct xhv_decoder {
    xhv_resampler interpolator;
    float exc[XHV_CODEC_EXC_HISTORY + XHV_CODEC_NB_FRAME];
    float mem_syn[XHV_CODEC_ORDER];
    float lsfq_old[XHV_CODEC_ORDER];
    float lsf_residual[XHV_CODEC_ORDER];
    float past_energy[4];
    float sharp;
    // postfilter
    float pf_res[XHV_CODEC_PITCH_MAX + 2 + XHV_CODEC_NB_SUBFRAME]; // short-term residual history (long-term filter)
    float pf_syn_hist[XHV_CODEC_ORDER];  // synthesized speech (input of the residual filter)
    float pf_mem_stp[XHV_CODEC_ORDER];   // 1/A(z/gd)
    float pf_tilt_mem, pf_gain;
    // concealment
    int last_lag;     // integer pitch lag of the last good subframe
    float last_gp, last_gc;
    unsigned erased;  // consecutive concealed frames
    unsigned noise;   // LCG state
    int postfilter;   // 1 normally; 0 for the codec core alone (tests)
} xhv_decoder;

void xhv_encoder_init(xhv_encoder *encoder);
// Encodes one frame (XHV_CODEC_FRAME_SAMPLES at 16 kHz) into two packets. reset != 0 starts a talk spurt: the encoder
// returns to its initial state first and marks the frame so the decoder does the same.
void xhv_encoder_encode(xhv_encoder *encoder, const short *pcm, int reset, unsigned char *packets);

void xhv_decoder_init(xhv_decoder *decoder);
// Decodes a frame (the two packets of one seq, A then B) to XHV_CODEC_FRAME_SAMPLES at 16 kHz.
void xhv_decoder_decode(xhv_decoder *decoder, const unsigned char *packets, short *pcm);
// Synthesizes a frame for one that never arrived: the last frame's spectrum and pitch, fading.
void xhv_decoder_conceal(xhv_decoder *decoder, short *pcm);

// A packet's header fields (every 10 bytes parse as a packet; the size is the caller's check). Returns 1.
int xhv_codec_packet_info(const unsigned char *packet, unsigned *half, unsigned *seq, unsigned *reset);

#ifdef __cplusplus
}
#endif

#endif
