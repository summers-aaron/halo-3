// port/sdk/audio (both hosts): a self-contained XMA (Xbox WMA Pro) decoder. Host C with no library dependency (no
// libavcodec, no libm, no allocation: callers own the memory), so it builds the same for i686 Linux and Windows.
//
// Two parts:
//   * xma_decoder: one XMA stream (1 or 2 channels). It decodes one XMA frame at a time (512 samples per channel),
//     the frame-at-a-time model of FFmpeg's XMAFRAMES decoder that xenia / rexglue feed from the hardware context.
//     The bitstream decode is a C port of FFmpeg 4.4 libavcodec/wmaprodec.c (LGPL 2.1+, see xma_decoder.c).
//   * xma_cursor: walks the frames of one stream through a buffer of 2 KB XMA packets (the XAudio packet / source
//     buffer data), following each packet's header: the 15-bit offset of the first frame that starts in it, and the
//     packet skip count that interleaves the packets of a multi-stream buffer. A frame may cross packet boundaries;
//     the cursor hands it over as one contiguous bit run.
//
// Sample timeline: an XMA stream's sample t is decoder output sample t + XMA_DECODER_DELAY (the decoder's output
// runs XMA_DECODER_DELAY samples ahead of the bitstream's sample numbering; rexglue's kDecoderStartPadding, measured
// on Halo 3's own data). Frame n's 512 output samples are decoder samples [512 n, 512 n + 512).
//
// Interface types are ints and pointers only: the engine calling it (xaudio_engine.cpp) is built with the game's
// MS-ABI flags, this file as host C.
#ifndef PORT_AUDIO_XMA_DECODER_H
#define PORT_AUDIO_XMA_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#define XMA_PACKET_BYTES 2048u
#define XMA_PACKET_BITS (XMA_PACKET_BYTES * 8u)
#define XMA_PACKET_HEADER_BITS 32u
#define XMA_SAMPLES_PER_FRAME 512u
#define XMA_SUBFRAME_SAMPLES 128u
#define XMA_DECODER_DELAY 192u
#define XMA_MAX_FRAME_BITS 0x7fffu
#define XMA_FRAME_BUFFER_BYTES (XMA_MAX_FRAME_BITS / 8u + 1u + 16u) // a frame's bits plus read padding

typedef struct xma_decoder xma_decoder;

// Bytes of memory one decoder needs (any 16-byte aligned block of this size).
unsigned PORT_MS_ABI xma_decoder_size(void);
// Sets up a decoder in `memory` for one stream. Returns 0 for an unsupported format (rate 0, channels not 1-2).
int PORT_MS_ABI xma_decoder_init(xma_decoder *decoder, unsigned sample_rate, unsigned channels);
// Forgets the overlap history: the next frame starts a new stream.
void PORT_MS_ABI xma_decoder_reset(xma_decoder *decoder);
// Decodes one frame whose bits (from its 15-bit length field) start at bit 0 of `frame` (XMA_FRAME_BUFFER_BYTES,
// zero padded past the frame). Writes 512 samples per channel, planar (out[c * 512 + i]), in [-1, 1] give or take
// the codec's overshoot. Returns 1, or 0 for a frame the bitstream rejects (out is then silence and the overlap
// history is reset, as the hardware drops a bad frame).
int PORT_MS_ABI xma_decoder_decode_frame(xma_decoder *decoder, const unsigned char *frame, unsigned frame_bits, float *out);
// The samples still held back after the last frame: the second half of its overlap (256 samples), then silence.
void PORT_MS_ABI xma_decoder_flush(xma_decoder *decoder, float *out);

// The frames of one stream through a packet buffer.
typedef struct xma_cursor {
    const unsigned char *data;
    unsigned packet_count;
    unsigned packet;        // packet holding the next frame's first bit
    unsigned bit;           // bit offset of the next frame in that packet (XMA_PACKET_HEADER_BITS .. XMA_PACKET_BITS)
    unsigned frame_index;   // frames handed out so far
    int done;
} xma_cursor;

// Positions the cursor on the first frame of stream `stream` (0-based) of a buffer of `bytes` bytes. The first
// packet of stream s is packet s; after that each stream follows its packets' skip counts.
void PORT_MS_ABI xma_cursor_init(xma_cursor *cursor, const void *data, unsigned bytes, unsigned stream);
// Positions the cursor at an absolute bit offset of the buffer (XAudio loop and packet positions), which must be a
// frame start. Returns 0 if it is outside the buffer.
int PORT_MS_ABI xma_cursor_seek_bits(xma_cursor *cursor, unsigned bit_offset);
// The absolute bit offset of the next frame (for XAudio's GetPacketPosition and loop regions).
unsigned PORT_MS_ABI xma_cursor_tell_bits(const xma_cursor *cursor);
// Copies the next frame to `frame` (XMA_FRAME_BUFFER_BYTES, zero padded) and returns its length in bits, or 0 when
// the stream has no more frames (or the data is malformed: the cursor stops rather than guess).
unsigned PORT_MS_ABI xma_cursor_next_frame(xma_cursor *cursor, unsigned char *frame);
// Steps over the next frame without copying it (seeks, length scans); returns its length in bits as above.
unsigned PORT_MS_ABI xma_cursor_skip_frame(xma_cursor *cursor);

#ifdef __cplusplus
}
#endif

#endif
