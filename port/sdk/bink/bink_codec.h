// port/sdk/bink (both hosts): the Bink 1 container, video and audio decoders (bink_codec.c) under the port's Bink
// library (bink_read.cpp). Host code: no C library, no allocator, no Xbox SDK headers; the interface is unsigned
// ints, floats and pointers only, so the MS-ABI library and the host decoder agree on it on both targets. Every
// context lives in memory the caller provides (the *_memory size), as the library allocates through the game's
// Bink pool (BinkSetMemory).
//
// The file (all fields little-endian):
//   0   'B' 'I' 'K' revision         ('i' on the Halo 3 disc; 'f' .. 'i' and 'k' decode, 'b' does not)
//   4   file size - 8
//   8   frame count
//   12  largest frame, bytes
//   16  frame count again
//   20  width, 24 height
//   28  frame rate, 32 frame rate divisor (frames per second = rate / divisor)
//   36  video flags (0x00100000 an alpha plane, 0x00020000 grayscale)
//   40  audio track count T ('k' adds a dword after it)
//   then T dwords (largest decoded audio of a frame, bytes of 16-bit samples), T (u16 sample rate, u16 flags:
//   0x1000 DCT, 0x2000 stereo, 0x4000 16-bit), T dwords (track ids), and frame_count + 1 dwords: frame offsets
//   (bit 0: key frame), the last one the file size.
// A frame: per track a dword size and that many bytes of audio packet (a packet starts with a dword, the bytes of
// 16-bit samples it decodes to), then the video packet to the end of the frame.
#ifndef PORT_BINK_CODEC_H
#define PORT_BINK_CODEC_H

#ifdef __cplusplus
extern "C" {
#endif

#define BINK_CODEC_FLAG_ALPHA 0x00100000u
#define BINK_CODEC_FLAG_GRAY 0x00020000u
#define BINK_CODEC_AUDIO_DCT 0x1000u
#define BINK_CODEC_AUDIO_STEREO 0x2000u
#define BINK_CODEC_AUDIO_16BITS 0x4000u
#define BINK_CODEC_MAX_TRACKS 256u
// The decoder's largest audio block (samples per channel): 2048-point transform, 1/16 overlap.
#define BINK_CODEC_AUDIO_MAX_BLOCK 4096u

typedef struct bink_codec_header {
    unsigned revision;     // the fourth signature byte
    unsigned file_size;    // the whole file
    unsigned frame_count;
    unsigned largest_frame;
    unsigned width, height;
    unsigned rate, rate_divisor;
    unsigned video_flags;
    unsigned track_count;
    unsigned tracks_offset; // the track arrays (track_count x 12 bytes)
    unsigned index_offset;  // the frame offsets (frame_count + 1 dwords)
    unsigned header_size;   // bytes up to the end of the frame offsets
} bink_codec_header;

// Reads the fixed header from the first `size` bytes of a file (44 bytes, 48 for 'k'). 0: not a Bink 1 file this
// decoder plays, or too short. The track arrays and the index need header_size bytes.
int PORT_MS_ABI bink_codec_parse_header(const void *data, unsigned size, bink_codec_header *header);
// One track's entries of the track arrays (`header_bytes`: the file from offset 0, at least header->index_offset).
void PORT_MS_ABI bink_codec_track(const void *header_bytes, const bink_codec_header *header, unsigned track, unsigned *max_bytes,
    unsigned *sample_rate, unsigned *flags, unsigned *id);
// A frame's offset and size from the index (`index`: header->frame_count + 1 dwords; frame 0-based). 0 if the index
// is out of order there.
int PORT_MS_ABI bink_codec_frame_extent(const void *index, unsigned frame_count, unsigned frame, unsigned *offset, unsigned *size,
    int *key);
// The parts of a frame: track `track`'s audio packet (size 0 if the frame has none for it), or the video packet
// (track == track_count). 0 if the frame's sizes overrun it.
int PORT_MS_ABI bink_codec_frame_part(const void *frame, unsigned frame_size, unsigned track_count, unsigned track,
    const void **part, unsigned *part_size);

// ---- video ----
struct bink_video;
// Plane pointers and pitches: 0 Y, 1 U (Cb), 2 V (Cr), 3 alpha. Luma planes are the width x height rounded up to
// 8, chroma planes half that rounded up to 8 (whole 8x8 blocks are written).
typedef struct bink_video_planes {
    unsigned char *data[4];
    unsigned pitch[4];
} bink_video_planes;
unsigned PORT_MS_ABI bink_video_memory(unsigned width, unsigned height);
struct bink_video * PORT_MS_ABI bink_video_create(void *memory, unsigned width, unsigned height, unsigned revision,
    unsigned video_flags);
// Decodes a video packet into `out`, `previous` the frame before it (NULL for the first frame of a sequence:
// motion then copies within `out`). With `alpha` 0 a file's alpha plane is skipped. 0 decoded; < 0 a corrupt
// packet (the planes hold what was decoded up to the error).
int PORT_MS_ABI bink_video_decode(struct bink_video *video, const void *packet, unsigned size, const bink_video_planes *out,
    const bink_video_planes *previous, int alpha);

// ---- audio ----
struct bink_audio;
unsigned PORT_MS_ABI bink_audio_memory(void);
// A decoder for one track (`flags`: the track's BINK_CODEC_AUDIO_* flags).
struct bink_audio * PORT_MS_ABI bink_audio_create(void *memory, unsigned sample_rate, unsigned flags, unsigned revision);
unsigned PORT_MS_ABI bink_audio_channels(const struct bink_audio *audio);
// Samples per channel of every block.
unsigned PORT_MS_ABI bink_audio_block_frames(const struct bink_audio *audio);
// The next block starts a new stream (no overlap with what was decoded before): after a seek.
void PORT_MS_ABI bink_audio_reset(struct bink_audio *audio);
// Starts on a packet (which must stay in place until its blocks are taken): its declared decoded size in bytes of
// 16-bit samples, or -1 if it is too short.
int PORT_MS_ABI bink_audio_begin(struct bink_audio *audio, const void *packet, unsigned size);
// The packet's next block, interleaved, into out (bink_audio_block_frames x channels floats, full scale +-1).
// Returns the frames written, 0 at the end of the packet, -1 for a corrupt block (the rest of the packet is dropped).
int PORT_MS_ABI bink_audio_next_block(struct bink_audio *audio, float *out);

#ifdef __cplusplus
}
#endif

#endif
