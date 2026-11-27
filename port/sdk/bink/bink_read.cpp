// port/sdk/bink (both hosts): the rest of binkread.obj, the Bink library's player (the half the game has decompiled is
// tu/binkxenon/binkread.cpp: BinkSetSoundSystem, BinkSetSoundTrack, BinkGetFrameBuffersInfo,
// bink_register_frame_buffers, BinkPause, bink_set_volume, BinkSetMixBins, the RADCB priority / callback hooks),
// over the port's own decoders (bink_codec.c). Built like the game, against the game's Bink headers: the BINK is
// include/bink/binkread.h's bink_read_state, which the game reads as s_bink_info (bink_playback.cpp) and the
// decompiled functions read by name.
//
// What the game and those functions rely on (offsets in bink_read_state):
//   - movie_width / movie_height, frame_count, frame (the 1-based frame BinkDoFrame decodes next), frame_rate /
//     frame_rate_divisor, width / height (BinkGetFrameBuffersInfo sizes the planes from these), flags bit 20
//     (BINKALPHA: an alpha plane to allocate; set only if the open asked for it and the file has one), single_buffer;
//   - buffers: the game's BINKFRAMEBUFFERS (bink_register_frame_buffers). BinkDoFrame decodes into the set after
//     buffers->current_frame (the one before is the reference) and makes it current, which is the set the game
//     shows (bink_try_to_decompress_frame: its next set is current + 1, else 0, as here). The planes are the game's
//     linear L8 textures (D3DFMT_LIN_L8: Y, Cr, Cb, alpha; rows BufferPitch apart): the CPU writes them, so every
//     decoded plane is reported to the renderer (port_vk_texels_written), the step the console's GPU did not need;
//   - last_decompressed_frame (the game decodes when frame differs from it), video_on (0: BinkDoFrame skips the
//     video; the game clears it to pre-roll sound from before a key frame), sound_on, playback_start (the clock:
//     the game zeroes it after a seek, BinkPause moves it across a pause), paused;
//   - sounds / sound_count, with mix_bin_ids (a sound's track index) and mix_bin_map (the file's track id table)
//     for bink_set_volume and BinkSetMixBins to find a track by id; sound_callback, the s_registered_callback of
//     the sound thread (RADCB_suspend_callback on it stops the sound service, as the game does around seeks);
//   - memory movies (BINKFROMMEMORY, the game's bink tag resources): when the resource moves, bink_playback.cpp
//     moves compressed_frame, preloaded_data, frame_offsets, track_sizes, track_types and mix_bin_map by the same
//     distance. They point into the movie (preloaded_data is its start), and the movie is only ever reached
//     through them, so a move is followed. For a file movie they point into this BINK's copy of the header.
//
// Sound: every selected track (BinkSetSoundTrack's ids with BINKSNDTRACK, else the first track) gets a decoder, a
// FIFO of decoded 16-bit samples and a BINKSND opened through the sound system (BinkSetSoundSystem: the XAudio
// driver, bink_xaudio.cpp). checksound -- on RAD's callback thread every 76 ms (bink_sound_callback), and from
// BinkPause -- decodes the audio packets of the frames ahead of the video while every FIFO has room for its next
// packet (Bink files carry audio ahead of the video: intro_60's first frame holds 0.78 s), then writes the FIFOs
// into the drivers' rings (Ready / Lock / Unlock); past the movie's audio it writes silence. The clock starts when
// the first ring starts streaming (or at once without sound), and BinkWait holds each frame until its time:
// (frame - start_sync_frame) * rate_divisor / rate seconds after playback_start.
//
// Open flags: 0x00000400 BINKNOFRAMEBUFFERS (the game's planes; else this BINK allocates two sets),
// 0x00002000 BINKPRELOADALL (the whole file read into memory at open), 0x00004000 BINKSNDTRACK,
// 0x00100000 BINKALPHA, 0x00800000 BINKFILEHANDLE (data is an open file handle, which stays the caller's),
// 0x04000000 BINKFROMMEMORY (data is the movie). Paths are the game's (d:\bink\...), opened through the kernel's
// CreateFileA. Tracks are played as 16-bit PCM.
#include <xtl.h>
#include <string.h>

#include "bink/binkread.h"
#include "bink/binkxa.h"
#include "bink/radmem.h"
#include "bink/radcb.h"
#include "bink/rad_timer.h"
#include "bink_codec.h"
#include "../d3d/vulkan_device.h"
#include "../audio/port_audio.h"

static_assert(offsetof(s_bink_info, io_callback_control) == offsetof(bink_read_state, io_callback_control));
static_assert(offsetof(s_bink_info, sound_track_enabled) == offsetof(bink_read_state, sound_enabled));
static_assert(offsetof(s_bink_info, sound_on) == offsetof(bink_read_state, sound_on));
static_assert(offsetof(s_bink_info, decoded_frame) == offsetof(bink_read_state, last_decompressed_frame));
static_assert(offsetof(s_bink_info, has_sound) == offsetof(bink_read_state, sound_count));
static_assert(offsetof(s_bink_info, sound_callback) == offsetof(bink_read_state, sound_callback));

extern "C" void port_write_error(const char *text);

// tu/binkxenon/binkread.cpp, radcb.cpp; bink_radcb.cpp
extern "C" {
void bink_set_error_string(const char *text);
void BinkGetFrameBuffersInfo(bink_read_state *bink, s_bink_frame_buffers *buffers);
void endframe(bink_read_state *state, unsigned int frame);
int bink_get_priority_sound(int *callback, unsigned int priority);
void bink_sound_callback(int context, unsigned int value);
s_callback_thread *RADCB_allocate_handler(int rate);
void RADCB_suspend_callback(s_callback_thread *thread, s_registered_callback *registration);
void RADCB_resume_callback(s_callback_thread *thread, s_registered_callback *registration);
int do_unregister(s_callback_thread *thread, s_registered_callback *registration, unsigned int flags);
}

// ---- binkread.obj's data ----

extern "C" {
// @address 0x11db800 g_bink_sound_system
binkread_sound_system_state g_bink_sound_system;
// @address 0x11db80c g_bink_io_thread
int g_bink_io_thread;
// @address 0x11db810 g_bink_sound_thread
long g_bink_sound_thread;
}

// @address 0x11db820 g_bink_sound_track_list
// BinkSetSoundTrack (binkread.cpp) writes the count two words before g_bink_sound_tracks (retail 0x11db828): the
// list is one object and g_bink_sound_tracks names its tracks.
struct s_bink_sound_track_list {
    unsigned int count;
    unsigned int reserved;
    int tracks[32];
};
extern "C" s_bink_sound_track_list g_bink_sound_track_list;
s_bink_sound_track_list g_bink_sound_track_list;
#define BINK_STRING2(x) #x
#define BINK_STRING(x) BINK_STRING2(x)
#define BINK_SYMBOL(name) BINK_STRING(__USER_LABEL_PREFIX__) #name
__asm__(".globl " BINK_SYMBOL(g_bink_sound_tracks) "\n\t.set " BINK_SYMBOL(g_bink_sound_tracks) ", "
        BINK_SYMBOL(g_bink_sound_track_list) " + 8\n");

namespace {

typedef char track_list_tracks_offset[(offsetof(s_bink_sound_track_list, tracks) == 8) ? 1 : -1];

const unsigned long k_open_noframebuffers = 0x00000400;
const unsigned long k_open_preloadall = 0x00002000;
const unsigned long k_open_sndtrack = 0x00004000;
const unsigned long k_open_alpha = 0x00100000;
const unsigned long k_open_filehandle = 0x00800000;
const unsigned long k_open_frommemory = 0x04000000;
const unsigned k_max_sounds = 8;
const unsigned k_sound_wait_ms = 1000; // BinkWait waits this long for the sound to start before it runs without
const unsigned k_stream_lead_ms = 50;  // start_streaming returns once its first chunk has played

struct s_track {
    unsigned file_track, id, rate, flags, channels;
    struct bink_audio *decoder;
    short *fifo;       // decoded samples not yet in the ring, interleaved
    unsigned capacity; // in sample frames
    unsigned read, count;
};

struct s_bink {
    bink_read_state bink; // first: the BINK the game holds
    bink_codec_header header;
    unsigned long open_flags;
    HANDLE file;
    int own_file;
    HANDLE io_lock;    // the file position and reads (video on the game's thread, audio on the sound thread)
    HANDLE sound_lock; // the FIFOs and decoders (checksound, the seeks)
    void *preload;     // BINKPRELOADALL: the file in memory
    unsigned max_frame;
    unsigned char *video_io, *audio_io;
    struct bink_video *video;
    int decoded_any;   // a frame was decoded: the current set is a reference
    int gray;
    s_bink_frame_buffers own_buffers;
    unsigned track_total;
    s_track tracks[k_max_sounds];
    unsigned audio_frame;      // 0-based: the next frame whose audio packets are decoded
    unsigned audio_base_frame; // 1-based: the frame the sound (re)started at
    volatile unsigned sound_started, sound_start_time;
    unsigned clock_started;
    unsigned wait_begin;
    unsigned registered;
    unsigned dropouts;        // rings that ran dry (Ready's sound_dropped_out)
    unsigned decode_ms, decode_max_ms, decoded; // HALO3_BINK_TRACE
    float block[BINK_CODEC_AUDIO_MAX_BLOCK * 2];
};

// ---- HALO3_BINK_TRACE=1: "bink: " lines on stderr (open, close, once a second of video: how late the frame is
// against the clock, the decode time, the sound FIFOs and drop-outs) ----

int g_trace = -1;

bool trace_on()
{
    if (g_trace < 0) {
        char value[8];
        g_trace = port_audio_getenv("HALO3_BINK_TRACE", value, sizeof(value)) && value[0] == '1';
    }
    return g_trace != 0;
}

struct s_trace_line {
    char text[256];
    unsigned length;
    s_trace_line() : length(0) { add("bink: "); }
    s_trace_line &add(const char *s)
    {
        while (*s && length + 2 < sizeof(text))
            text[length++] = *s++;
        return *this;
    }
    s_trace_line &add(int v)
    {
        char digits[12];
        unsigned n = 0, u = v < 0 ? 0u - (unsigned)v : (unsigned)v;
        do {
            digits[n++] = (char)('0' + u % 10u);
            u /= 10u;
        } while (u);
        if (v < 0)
            add("-");
        while (n && length + 2 < sizeof(text))
            text[length++] = digits[--n];
        return *this;
    }
    void write()
    {
        text[length++] = '\n';
        text[length] = 0;
        port_write_error(text);
    }
};

inline s_bink *private_of(bink_read_state *bink) { return (s_bink *)bink; }
inline s_registered_callback *sound_registration(bink_read_state *bink)
{
    return (s_registered_callback *)&bink->sound_callback;
}
inline s_callback_thread *sound_thread() { return (s_callback_thread *)(size_t)g_bink_sound_thread; }

typedef char registration_fits[(sizeof(s_registered_callback) <=
    sizeof(((bink_read_state *)0)->sound_callback) + sizeof(((bink_read_state *)0)->sound_callback_control)) ? 1 : -1];

unsigned align_up(unsigned value, unsigned alignment) { return (value + alignment - 1) & ~(alignment - 1); }
unsigned rd32(const unsigned char *p) { return p[0] | (unsigned)p[1] << 8 | (unsigned)p[2] << 16 | (unsigned)p[3] << 24; }

void lock(HANDLE mutex)
{
    if (mutex)
        WaitForSingleObject(mutex, INFINITE);
}
void unlock(HANDLE mutex)
{
    if (mutex)
        ReleaseMutex(mutex);
}

// Reads `bytes` at `offset` of a file movie.
int read_at(s_bink *b, unsigned offset, void *buffer, unsigned bytes)
{
    lock(b->io_lock);
    DWORD got = 0;
    int ok = SetFilePointer(b->file, (LONG)offset, 0, FILE_BEGIN) != INVALID_SET_FILE_POINTER &&
             ReadFile(b->file, buffer, bytes, &got, 0) && got == bytes;
    unlock(b->io_lock);
    return ok;
}

// A frame's bytes (0-based frame): in the movie for memory movies, else read into `io`.
const unsigned char *frame_data(s_bink *b, unsigned frame, unsigned char *io, unsigned *size)
{
    unsigned offset, bytes;
    int key;
    if (!bink_codec_frame_extent(b->bink.frame_offsets, b->header.frame_count, frame, &offset, &bytes, &key) ||
        offset > b->header.file_size || bytes > b->header.file_size - offset)
        return 0;
    *size = bytes;
    if (b->bink.preloaded_data)
        return (const unsigned char *)b->bink.preloaded_data + offset;
    if (bytes > b->max_frame || !read_at(b, offset, io, bytes))
        return 0;
    return io;
}

// ---- sound ----

void fifo_reset(s_track *track)
{
    track->read = track->count = 0;
}

void fifo_push(s_track *track, const float *samples, unsigned frames)
{
    const unsigned channels = track->channels;
    for (unsigned i = 0; i < frames && track->count < track->capacity; i++) {
        short *slot = track->fifo + ((track->read + track->count) % track->capacity) * channels;
        for (unsigned c = 0; c < channels; c++) {
            // full scale +-1 to 16 bits, rounded to nearest and clamped (Bink's own float-to-16-bit step)
            float v = samples[i * channels + c] * 32768.0f;
            v = v > 32767.0f ? 32767.0f : v < -32768.0f ? -32768.0f : v;
            slot[c] = (short)(v >= 0.0f ? v + 0.5f : v - 0.5f);
        }
        track->count++;
    }
}

// Decodes the audio of the frames ahead while every track has room for its packet of the next frame.
void decode_ahead(s_bink *b)
{
    while (b->audio_frame < b->header.frame_count) {
        unsigned size;
        const unsigned char *frame = frame_data(b, b->audio_frame, b->audio_io, &size);
        if (!frame) {
            b->audio_frame = b->header.frame_count; // unreadable: the sound ends here
            return;
        }
        for (unsigned i = 0; i < b->track_total; i++) {
            s_track *track = &b->tracks[i];
            const void *packet;
            unsigned packet_size;
            if (!bink_codec_frame_part(frame, size, b->header.track_count, track->file_track, &packet, &packet_size) ||
                packet_size < 4)
                continue;
            const unsigned frames = rd32((const unsigned char *)packet) / (2u * track->channels);
            if (track->capacity - track->count < frames + 2u * bink_audio_block_frames(track->decoder))
                return; // no room yet: on the next service
        }
        for (unsigned i = 0; i < b->track_total; i++) {
            s_track *track = &b->tracks[i];
            const void *packet;
            unsigned packet_size;
            if (!bink_codec_frame_part(frame, size, b->header.track_count, track->file_track, &packet, &packet_size) ||
                packet_size < 4 || bink_audio_begin(track->decoder, packet, packet_size) < 0)
                continue;
            int got;
            while ((got = bink_audio_next_block(track->decoder, b->block)) > 0)
                fifo_push(track, b->block, (unsigned)got);
        }
        b->audio_frame++;
    }
}

// The FIFOs into the rings.
void feed_rings(s_bink *b)
{
    bink_read_state *bink = &b->bink;
    for (unsigned i = 0; i < bink->sound_count; i++) {
        bink_sound_entry *sound = &bink->sounds[i];
        s_track *track = &b->tracks[i];
        const unsigned align = 2u * track->channels;
        for (int pass = 0; pass < 2; pass++) {
            if (sound->sound_dropped_out) {
                sound->sound_dropped_out = 0;
                b->dropouts++;
            }
            if (!sound->ready(sound))
                break;
            unsigned char *at;
            unsigned bytes;
            sound->lock(sound, &at, &bytes);
            unsigned frames = bytes / align;
            if (frames > track->count)
                frames = track->count;
            if (frames == 0) {
                if (b->audio_frame < b->header.frame_count || bytes == 0)
                    break; // more is coming
                memset(at, 0, bytes); // past the movie's audio: silence
                sound->unlock(sound, bytes);
                continue;
            }
            for (unsigned f = 0; f < frames; f++) {
                memcpy(at + f * align, track->fifo + track->read * track->channels, align);
                track->read = track->read + 1 == track->capacity ? 0 : track->read + 1;
            }
            track->count -= frames;
            sound->unlock(sound, frames * align);
        }
    }
    // the clock: the first ring that started streaming (start_streaming returned once its first chunk had played)
    if (!b->sound_started) {
        for (unsigned i = 0; i < bink->sound_count; i++) {
            if (((BINKSND *)&bink->sounds[i])->channel.paused) {
                b->sound_start_time = RADTimerRead() - k_stream_lead_ms;
                b->sound_started = 1;
                if (trace_on())
                    s_trace_line().add("sound started at ").add((int)b->sound_start_time).add(" ms, frame ")
                        .add((int)bink->frame).add(", ").add((int)bink->sound_count).add(" tracks").write();
                break;
            }
        }
    }
}

} // namespace

// The sound service (checksound, bink_checksound.cpp).
extern "C" int bink_service_sound(void *movie)
{
    bink_read_state *bink = (bink_read_state *)movie;
    if (bink == 0 || bink->sound_count == 0)
        return 0;
    s_bink *b = private_of(bink);
    lock(b->sound_lock);
    if (bink->sound_on && !bink->paused) {
        decode_ahead(b);
        feed_rings(b);
    }
    unlock(b->sound_lock);
    return 1;
}

namespace {

// Stops the sound service (the callback's mutex, as RADCB_suspend_callback; the game's seek takes it the same way)
// and the decoders.
void suspend_sound(bink_read_state *bink)
{
    s_bink *b = private_of(bink);
    if (b->registered)
        RADCB_suspend_callback(sound_thread(), sound_registration(bink));
    lock(b->sound_lock);
}

void resume_sound(bink_read_state *bink)
{
    s_bink *b = private_of(bink);
    unlock(b->sound_lock);
    if (b->registered)
        RADCB_resume_callback(sound_thread(), sound_registration(bink));
}

// The sound starts over at `frame` (1-based): decoders, FIFOs and rings.
void restart_sound(s_bink *b, unsigned frame)
{
    bink_read_state *bink = &b->bink;
    b->audio_frame = frame - 1;
    b->audio_base_frame = frame;
    for (unsigned i = 0; i < b->track_total; i++) {
        bink_audio_reset(b->tracks[i].decoder);
        fifo_reset(&b->tracks[i]);
    }
    if (bink->sound_on) {
        for (unsigned i = 0; i < bink->sound_count; i++) {
            bink->sounds[i].set_on_off(&bink->sounds[i], 0);
            bink->sounds[i].set_on_off(&bink->sounds[i], 1);
        }
    }
    b->sound_started = 0;
}

int find_track(const unsigned char *ids, unsigned count, int id)
{
    for (unsigned t = 0; t < count; t++)
        if ((int)rd32(ids + 4 * t) == id)
            return (int)t;
    return -1;
}

// The planes of one frame buffer set, for the decoder (alpha only when the set has its own alpha plane).
void planes_of(const s_bink_frame_planes *set, int alpha, bink_video_planes *planes)
{
    planes->data[0] = (unsigned char *)set->y.Buffer;
    planes->pitch[0] = set->y.BufferPitch;
    planes->data[1] = (unsigned char *)set->cb.Buffer;
    planes->pitch[1] = set->cb.BufferPitch;
    planes->data[2] = (unsigned char *)set->cr.Buffer;
    planes->pitch[2] = set->cr.BufferPitch;
    planes->data[3] = alpha && set->alpha.Allocate ? (unsigned char *)set->alpha.Buffer : 0;
    planes->pitch[3] = set->alpha.BufferPitch;
}

void texels_written(const s_bink_frame_buffers *buffers, const bink_video_planes *planes)
{
    const unsigned heights[4] = { buffers->y_height, buffers->chroma_height, buffers->chroma_height, buffers->y_height };
    for (int p = 0; p < 4; p++)
        if (planes->data[p] != 0)
            port_vk_texels_written(planes->data[p], planes->pitch[p] * heights[p]);
}

void decode_video(s_bink *b, unsigned frame)
{
    bink_read_state *bink = &b->bink;
    s_bink_frame_buffers *buffers = bink->buffers;
    if (buffers == 0 || buffers->count < 1 || buffers->frames[0].y.Buffer == 0)
        return;
    const long count = buffers->count > 2 ? 2 : buffers->count;
    const long current = buffers->current_frame >= 0 && buffers->current_frame < count ? buffers->current_frame : 0;
    const long next = count > 1 ? (current + 1) % count : 0;
    const int alpha = (bink->flags & k_open_alpha) != 0;
    bink_video_planes out, previous;
    planes_of(&buffers->frames[next], alpha, &out);
    planes_of(&buffers->frames[current], alpha, &previous);
    if (out.data[1] == 0 || out.data[2] == 0)
        return;
    unsigned size;
    const unsigned char *data = frame_data(b, frame - 1, b->video_io, &size);
    const void *packet;
    unsigned packet_size;
    if (data == 0 || !bink_codec_frame_part(data, size, b->header.track_count, b->header.track_count, &packet,
                         &packet_size)) {
        bink->read_error = 1;
        return;
    }
    bink->compressed_frame = (void *)data;
    if (b->gray && !b->decoded_any) {
        // a grayscale movie has no colour planes: neutral chroma in every set
        for (long s = 0; s < count; s++) {
            bink_video_planes set;
            planes_of(&buffers->frames[s], 0, &set);
            for (unsigned y = 0; y < buffers->chroma_height; y++) {
                memset(set.data[1] + y * set.pitch[1], 0x80, buffers->chroma_width);
                memset(set.data[2] + y * set.pitch[2], 0x80, buffers->chroma_width);
            }
        }
    }
    const unsigned start = RADTimerRead();
    bink_video_decode(b->video, packet, packet_size, &out, b->decoded_any ? &previous : 0, alpha);
    const unsigned took = RADTimerRead() - start;
    bink->video_decompression_time += took;
    b->decode_ms = took;
    if (took > b->decode_max_ms)
        b->decode_max_ms = took;
    b->decoded++;
    b->decoded_any = 1;
    buffers->current_frame = next;
    texels_written(buffers, &out);
}

} // namespace

// @address 0x70ef70 BinkOpen
extern "C" bink_read_state *BinkOpen(const void *data, unsigned long flags)
{
    if (data == 0)
        return 0;
    const unsigned open_time = RADTimerRead();
    const unsigned char *movie = 0;
    HANDLE file = INVALID_HANDLE_VALUE;
    int own_file = 0;
    unsigned char fixed[48];
    bink_codec_header header;
    if (flags & k_open_frommemory) {
        movie = (const unsigned char *)data;
        memcpy(fixed, movie, sizeof(fixed));
    } else {
        if (flags & k_open_filehandle) {
            file = (HANDLE)data;
        } else {
            file = CreateFileA((const char *)data, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL, 0);
            own_file = 1;
        }
        DWORD got = 0;
        if (file == INVALID_HANDLE_VALUE || SetFilePointer(file, 0, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER ||
            !ReadFile(file, fixed, sizeof(fixed), &got, 0) || got != sizeof(fixed)) {
            if (own_file && file != INVALID_HANDLE_VALUE)
                CloseHandle(file);
            bink_set_error_string("Error opening file.");
            return 0;
        }
    }
    if (!bink_codec_parse_header(fixed, sizeof(fixed), &header)) {
        if (own_file)
            CloseHandle(file);
        bink_set_error_string("Not a Bink file.");
        return 0;
    }
    // the track arrays, to choose the tracks
    unsigned char tracks_bytes[BINK_CODEC_MAX_TRACKS * 12];
    const unsigned tracks_size = header.track_count * 12;
    if (movie) {
        memcpy(tracks_bytes, movie + header.tracks_offset, tracks_size);
    } else if (tracks_size) {
        DWORD got = 0;
        if (SetFilePointer(file, (LONG)header.tracks_offset, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER ||
            !ReadFile(file, tracks_bytes, tracks_size, &got, 0) || got != tracks_size) {
            if (own_file)
                CloseHandle(file);
            bink_set_error_string("Error reading the file header.");
            return 0;
        }
    }
    bink_codec_header local = header;
    local.tracks_offset = 0; // bink_codec_track over tracks_bytes
    int chosen[k_max_sounds];
    unsigned chosen_count = 0;
    if (header.track_count && g_bink_sound_system.sound_open) {
        const unsigned char *ids = tracks_bytes + 8 * header.track_count;
        if (flags & k_open_sndtrack) {
            const unsigned requested = g_bink_sound_track_list.count < 32 ? g_bink_sound_track_list.count : 32;
            for (unsigned i = 0; i < requested && chosen_count < k_max_sounds; i++) {
                const int t = find_track(ids, header.track_count, g_bink_sound_track_list.tracks[i]);
                int seen = t < 0;
                for (unsigned k = 0; k < chosen_count && !seen; k++)
                    seen = chosen[k] == t;
                if (!seen)
                    chosen[chosen_count++] = t;
            }
        } else {
            chosen[chosen_count++] = 0;
        }
    }
    unsigned fifo_frames[k_max_sounds], track_channels[k_max_sounds];
    for (unsigned i = 0; i < chosen_count; i++) {
        unsigned max_bytes, rate, track_flags;
        bink_codec_track(tracks_bytes, &local, (unsigned)chosen[i], &max_bytes, &rate, &track_flags, 0);
        track_channels[i] = (track_flags & BINK_CODEC_AUDIO_STEREO) ? 2u : 1u;
        unsigned frames = max_bytes / (2u * track_channels[i]);
        if (frames == 0 || frames > rate * 4u)
            frames = rate; // no usable figure: a second
        fifo_frames[i] = frames + 4u * BINK_CODEC_AUDIO_MAX_BLOCK;
    }
    const int preload = movie == 0 && (flags & k_open_preloadall) != 0;
    const int own_planes = (flags & k_open_noframebuffers) == 0;
    const int alpha = (flags & k_open_alpha) && (header.video_flags & BINK_CODEC_FLAG_ALPHA);

    // one allocation: the BINK, the sound entries and track ids, the header copy, the decoders, the read buffers,
    // the FIFOs and (without BINKNOFRAMEBUFFERS) two frame buffer sets
    const unsigned header_copy = movie || preload ? 0 : header.header_size;
    const unsigned io = movie || preload ? 0 : align_up(header.largest_frame + 16, 16);
    const unsigned y_width = (header.width + 7) & ~7u, y_height = (header.height + 7) & ~7u;
    const unsigned c_width = (((header.width + 1) >> 1) + 7) & ~7u, c_height = (((header.height + 1) >> 1) + 7) & ~7u;
    const unsigned plane_set = own_planes ? y_width * y_height * (alpha ? 2 : 1) + 2 * c_width * c_height : 0;
    unsigned size = align_up(sizeof(s_bink), 64);
    const unsigned sounds_at = size;
    size += align_up(chosen_count * sizeof(bink_sound_entry), 64);
    const unsigned ids_at = size;
    size += align_up(chosen_count * 4 + 4, 64);
    const unsigned header_at = size;
    size += align_up(header_copy, 64);
    const unsigned video_at = size;
    size += align_up(bink_video_memory(header.width, header.height), 64);
    const unsigned audio_at = size;
    size += chosen_count * align_up(bink_audio_memory(), 64);
    const unsigned io_at = size;
    size += 2 * io;
    const unsigned fifo_at = size;
    for (unsigned i = 0; i < chosen_count; i++)
        size += align_up(fifo_frames[i] * track_channels[i] * 2, 64);
    const unsigned planes_at = size;
    size += 2 * align_up(plane_set, 64);
    unsigned char *memory = (unsigned char *)radmalloc((int)size, 0, 0, 0, 0, 0);
    if (memory == 0) {
        if (own_file)
            CloseHandle(file);
        bink_set_error_string("Out of memory.");
        return 0;
    }
    memset(memory, 0, size);
    s_bink *b = (s_bink *)memory;
    bink_read_state *bink = &b->bink;
    b->header = header;
    b->open_flags = flags;
    b->file = file;
    b->own_file = own_file;
    b->max_frame = header.largest_frame;
    if (preload) {
        b->preload = radmalloc((int)header.file_size, 0, 0, 0, 0, 0);
        if (b->preload == 0 || SetFilePointer(file, 0, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
            if (b->preload)
                radfree(b->preload);
            if (own_file)
                CloseHandle(file);
            radfree(memory);
            bink_set_error_string("Out of memory.");
            return 0;
        }
        DWORD got = 0;
        if (!ReadFile(file, b->preload, header.file_size, &got, 0) || got != header.file_size) {
            radfree(b->preload);
            if (own_file)
                CloseHandle(file);
            radfree(memory);
            bink_set_error_string("Error reading the file.");
            return 0;
        }
        if (own_file)
            CloseHandle(file);
        b->file = INVALID_HANDLE_VALUE;
        b->own_file = 0;
        movie = (const unsigned char *)b->preload;
    }
    if (movie) {
        bink->preloaded_data = (void *)movie;
        bink->frame_offsets = (unsigned int *)(movie + header.index_offset);
        bink->track_sizes = (unsigned int *)(movie + header.tracks_offset);
    } else {
        unsigned char *copy = memory + header_at;
        b->io_lock = CreateMutexA(0, 0, 0);
        if (!read_at(b, 0, copy, header.header_size)) {
            if (own_file)
                CloseHandle(file);
            CloseHandle(b->io_lock);
            radfree(memory);
            bink_set_error_string("Error reading the file header.");
            return 0;
        }
        bink->frame_offsets = (unsigned int *)(copy + header.index_offset);
        bink->track_sizes = (unsigned int *)(copy + header.tracks_offset);
        b->video_io = memory + io_at;
        b->audio_io = memory + io_at + io;
    }
    bink->track_types = bink->track_sizes + header.track_count;
    bink->mix_bin_map = (int *)(bink->track_sizes + 2 * header.track_count);

    bink->movie_width = bink->width = header.width;
    bink->movie_height = bink->height = header.height;
    bink->frame_count = header.frame_count;
    bink->frame = 1;
    bink->last_frame = 0;
    bink->frame_rate = header.rate;
    bink->frame_rate_divisor = header.rate_divisor;
    bink->file_frame_rate = header.rate;
    bink->file_frame_rate_divisor = header.rate_divisor;
    bink->flags = (flags & ~k_open_alpha) | (alpha ? k_open_alpha : 0);
    bink->bink_type = header.revision;
    bink->file_size = header.file_size;
    bink->largest_frame_size = header.largest_frame;
    bink->track_count = (int)header.track_count;
    bink->video_on = 1;
    bink->sound_on = 1;
    bink->open_time = open_time;
    {
        int frame = 0, continuous = 0;
        const unsigned per_second = (header.rate + header.rate_divisor - 1) / header.rate_divisor;
        if (header.frame_count > per_second)
            bink->highest_one_second_rate =
                high1secrate((int)header.frame_count, bink->frame_offsets, per_second, &frame, &continuous);
        bink->highest_one_second_frame = (unsigned)frame;
    }
    b->gray = (header.video_flags & BINK_CODEC_FLAG_GRAY) != 0;
    b->video = bink_video_create(memory + video_at, header.width, header.height, header.revision, header.video_flags);
    if (own_planes) {
        // Bink's own frame buffers: two sets, linear, pitch the 8-aligned width
        s_bink_frame_buffers *buffers = &b->own_buffers;
        BinkGetFrameBuffersInfo(bink, buffers);
        for (long s = 0; s < buffers->count && s < 2; s++) {
            unsigned char *at = memory + planes_at + s * align_up(plane_set, 64);
            s_bink_frame_planes *set = &buffers->frames[s];
            set->y.Buffer = at;
            set->y.BufferPitch = y_width;
            at += y_width * y_height;
            set->cr.Buffer = at;
            set->cr.BufferPitch = c_width;
            at += c_width * c_height;
            set->cb.Buffer = at;
            set->cb.BufferPitch = c_width;
            at += c_width * c_height;
            if (alpha) {
                set->alpha.Buffer = at;
                set->alpha.BufferPitch = y_width;
            }
        }
        bink->buffers = buffers;
    }

    // the sound tracks
    b->sound_lock = CreateMutexA(0, 0, 0);
    bink->sounds = (bink_sound_entry *)(memory + sounds_at);
    bink->mix_bin_ids = (int *)(memory + ids_at);
    unsigned char *fifo = memory + fifo_at;
    unsigned audio = audio_at;
    for (unsigned i = 0; i < chosen_count; i++) {
        unsigned rate, track_flags, id;
        bink_codec_track(tracks_bytes, &local, (unsigned)chosen[i], 0, &rate, &track_flags, &id);
        const unsigned fifo_bytes = align_up(fifo_frames[i] * track_channels[i] * 2, 64);
        unsigned char *track_fifo = fifo;
        fifo += fifo_bytes;
        bink_sound_entry *entry = &bink->sounds[bink->sound_count];
        if (!g_bink_sound_system.sound_open((BINKSND *)entry, rate, 16, (int)track_channels[i], 0, (BINK *)bink)) {
            memset(entry, 0, sizeof(*entry)); // the track plays muted (no voice could be opened)
            continue;
        }
        s_track *track = &b->tracks[b->track_total++];
        track->file_track = (unsigned)chosen[i];
        track->id = id;
        track->rate = rate;
        track->flags = track_flags;
        track->channels = track_channels[i];
        track->decoder = bink_audio_create(memory + audio, rate, track_flags, header.revision);
        audio += align_up(bink_audio_memory(), 64);
        track->fifo = (short *)track_fifo;
        track->capacity = fifo_frames[i];
        bink->mix_bin_ids[bink->sound_count] = chosen[i];
        bink->sound_count++;
        g_bink_sound_system.active_sounds++;
    }
    bink->sound_enabled = bink->sound_count != 0;
    b->audio_frame = 0;
    b->audio_base_frame = 1;
    if (bink->sound_count) {
        // the sound service: RAD's callback thread, as the retail BinkOpen sets it up
        s_callback_thread *thread = sound_thread();
        if (thread == 0) {
            thread = RADCB_allocate_handler(-13);
            if (thread)
                RADCB_resume_handler(thread);
            g_bink_sound_thread = (long)(size_t)thread;
        }
        if (thread) {
            RADCB_register_callback(thread, sound_registration(bink), (void (*)(void *))bink_get_priority_sound,
                (void *)bink_sound_callback);
            b->registered = 1;
        }
    }
    if (trace_on()) {
        s_trace_line line;
        line.add("open ").add(flags & k_open_frommemory ? "(memory)" : flags & k_open_filehandle ? "(file handle)" : (const char *)data)
            .add(" ").add((int)header.width).add("x").add((int)header.height).add(header.revision == 'i' ? " BIKi" : " BIK")
            .add(" frames ").add((int)header.frame_count).add(" rate ").add((int)header.rate).add("/").add((int)header.rate_divisor)
            .add(" tracks ").add((int)header.track_count).add(" sounds ").add((int)bink->sound_count)
            .add(alpha ? " alpha" : "").add(preload ? " preloaded" : "").add(" memory ").add((int)size).write();
    }
    return bink;
}

// @address 0x711240 BinkClose
extern "C" void BinkClose(bink_read_state *bink)
{
    if (bink == 0)
        return;
    s_bink *b = private_of(bink);
    if (b->registered) {
        s_callback_thread *thread = sound_thread();
        s_registered_callback *registration = sound_registration(bink);
        // off the thread's list while it is between rounds, then unregistered (the last one frees the thread)
        WaitForSingleObject(thread->mutex, INFINITE);
        remove_cb_from_list(thread, registration);
        ReleaseMutex(thread->mutex);
        if (do_unregister(thread, registration, 1))
            g_bink_sound_thread = 0;
        b->registered = 0;
    }
    for (unsigned i = 0; i < bink->sound_count; i++) {
        if (bink->sounds[i].close)
            bink->sounds[i].close(&bink->sounds[i]);
        g_bink_sound_system.active_sounds--;
    }
    bink->sound_count = 0;
    if (b->sound_lock)
        CloseHandle(b->sound_lock);
    if (b->io_lock)
        CloseHandle(b->io_lock);
    if (b->own_file && b->file != INVALID_HANDLE_VALUE)
        CloseHandle(b->file);
    if (b->preload)
        radfree(b->preload);
    if (trace_on())
        s_trace_line().add("close at frame ").add((int)bink->frame).add(", decoded ").add((int)b->decoded)
            .add(", longest decode ").add((int)b->decode_max_ms).add(" ms, sound drop-outs ").add((int)b->dropouts).write();
    radfree(b);
}

// @address 0x710858 BinkDoFrame
extern "C" void BinkDoFrame(bink_read_state *bink)
{
    if (bink == 0 || bink->frame < 1 || bink->frame > bink->frame_count)
        return;
    s_bink *b = private_of(bink);
    bink->frame_start = RADTimerRead() | 1u;
    if (bink->video_on)
        decode_video(b, bink->frame);
    bink->last_decompressed_frame = bink->frame;
    endframe(bink, RADTimerRead());
    if (trace_on() && b->clock_started && bink->frame_rate != 0) {
        const unsigned per_second = (bink->frame_rate + bink->frame_rate_divisor - 1) / bink->frame_rate_divisor;
        if (per_second != 0 && bink->frame % per_second == 1) {
            const unsigned now = RADTimerRead();
            const int due = (int)((unsigned __int64)(bink->frame - bink->start_sync_frame) * 1000u *
                                  bink->frame_rate_divisor / bink->frame_rate);
            s_trace_line line;
            line.add("frame ").add((int)bink->frame).add(" late ").add((int)(now - bink->playback_start) - due)
                .add(" ms, decode ").add((int)b->decode_ms).add(" ms (max ").add((int)b->decode_max_ms)
                .add("), audio frame ").add((int)b->audio_frame);
            for (unsigned i = 0; i < b->track_total; i++)
                line.add(i ? "/" : ", fifo ").add((int)b->tracks[i].count);
            line.add(", drop-outs ").add((int)b->dropouts).write();
        }
    }
}

// @address 0x710e58 BinkNextFrame
extern "C" void BinkNextFrame(bink_read_state *bink)
{
    if (bink == 0)
        return;
    bink->last_frame = bink->frame;
    if (bink->frame < bink->frame_count) {
        bink->frame++;
        return;
    }
    // past the last frame: the movie starts over (sound and clock too)
    s_bink *b = private_of(bink);
    suspend_sound(bink);
    bink->frame = 1;
    restart_sound(b, 1);
    resume_sound(bink);
    bink->playback_start = 0;
    b->clock_started = 0;
}

// @address 0x710228 BinkWait
// 1 while the current frame's time has not come (or the movie is paused), else 0.
extern "C" long BinkWait(bink_read_state *bink)
{
    if (bink == 0)
        return 0;
    if (bink->paused)
        return 1;
    s_bink *b = private_of(bink);
    unsigned now = RADTimerRead();
    if (now == 0)
        now = 1;
    if (!b->clock_started) {
        if (b->wait_begin == 0)
            b->wait_begin = now;
        if (b->track_total && !b->sound_started && now - b->wait_begin < k_sound_wait_ms)
            return bink->frame > b->audio_base_frame ? 1 : 0; // the first frame waits for nothing
        if (b->sound_started) {
            bink->playback_start = b->sound_start_time ? b->sound_start_time : 1;
            bink->start_sync_frame = b->audio_base_frame;
        } else {
            bink->playback_start = now;
            bink->start_sync_frame = bink->frame;
        }
        b->clock_started = 1;
    } else if (bink->playback_start == 0) {
        // the game restarted the clock (after its seek): from this frame, now
        bink->playback_start = now;
        bink->start_sync_frame = bink->frame;
    }
    if (bink->frame <= bink->start_sync_frame)
        return 0;
    const unsigned __int64 due = (unsigned __int64)(bink->frame - bink->start_sync_frame) * 1000u *
                                 bink->frame_rate_divisor / bink->frame_rate;
    return (unsigned __int64)(now - bink->playback_start) < due ? 1 : 0;
}

// @address 0x70eab0 GotoFrame
// The movie continues at `frame` (1-based; decoded by the next BinkDoFrame, from a key frame for a clean picture).
extern "C" void GotoFrame(bink_read_state *bink, unsigned long frame)
{
    if (bink == 0)
        return;
    if (frame < 1)
        frame = 1;
    if (frame > bink->frame_count)
        frame = bink->frame_count;
    s_bink *b = private_of(bink);
    suspend_sound(bink);
    bink->last_frame = bink->frame;
    bink->frame = frame;
    bink->last_decompressed_frame = 0;
    restart_sound(b, (unsigned)frame);
    resume_sound(bink);
    bink->playback_start = 0;
    b->clock_started = 0;
    b->wait_begin = 0;
}

// @address 0x710070 BinkGetKeyFrame
// flags & 0xf: 0 the key frame at or before `frame`, 1 at or after, 2 the closest; flags & 0x80: not `frame` itself.
extern "C" unsigned long BinkGetKeyFrame(bink_read_state *bink, unsigned long frame, long flags)
{
    if (bink == 0 || bink->frame_count == 0)
        return 0;
    if (frame < 1)
        frame = 1;
    if (frame > bink->frame_count)
        frame = bink->frame_count;
    const unsigned *index = bink->frame_offsets;
    const int not_equal = (flags & 0x80) != 0;
    // frame 1 is always a key frame
    long previous = 0, next = 0;
    for (unsigned long f = frame; f >= 1; f--) {
        if ((f == 1 || (index[f - 1] & 1)) && !(not_equal && f == frame)) {
            previous = (long)f;
            break;
        }
    }
    for (unsigned long f = frame; f <= bink->frame_count; f++) {
        if ((f == 1 || (index[f - 1] & 1)) && !(not_equal && f == frame)) {
            next = (long)f;
            break;
        }
    }
    switch (flags & 0xf) {
    case 1:
        return next ? (unsigned long)next : 0;
    case 2:
        if (previous == 0)
            return (unsigned long)next;
        if (next == 0)
            return (unsigned long)previous;
        return (unsigned long)((long)frame - previous <= next - (long)frame ? previous : next);
    default:
        return (unsigned long)previous;
    }
}

// @address 0x710698 BinkSetSoundOnOff
extern "C" long BinkSetSoundOnOff(bink_read_state *bink, long on)
{
    if (bink == 0 || bink->sound_count == 0)
        return 0;
    on = on != 0;
    suspend_sound(bink);
    for (unsigned i = 0; i < bink->sound_count; i++)
        bink->sounds[i].set_on_off(&bink->sounds[i], (int)on);
    bink->sound_on = (unsigned)on;
    resume_sound(bink);
    return on;
}
