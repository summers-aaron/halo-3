#ifndef BINK_BINKREAD_H
#define BINK_BINKREAD_H

#include "bink/bink_playback_types.h"

struct bink_sound_entry
{
    int (*ready)(bink_sound_entry *);
    int (*lock)(bink_sound_entry *, unsigned char **, unsigned int *);
    int (*unlock)(bink_sound_entry *, unsigned int);
    void (*set_volume)(bink_sound_entry *, int);
    void (*set_pan)(bink_sound_entry *, int);
    void (*pause)(bink_sound_entry *, int);
    int (*set_on_off)(bink_sound_entry *, int);
    void (*close)(bink_sound_entry *);
    void (*set_mix_bins)(bink_sound_entry *, unsigned int *, int);
    void (*set_mix_bin_volumes)(bink_sound_entry *, unsigned int *, int *, unsigned int);
    unsigned int sound_buffer_size;
    unsigned char *sound_buffer;
    unsigned char *sound_end;
    unsigned char *sound_write_position;
    unsigned char *sound_read_position;
    unsigned int sound_compression;
    unsigned int sound_amount;
    unsigned int sound_convert_8;
    unsigned int sound_end_frame;
    unsigned int sound_prime;
    unsigned int sound_padding;
    unsigned int best_size_in_16;
    unsigned int best_size_mask;
    unsigned int sound_dropped_out;
    int no_thread_service;
    int on_off;
    unsigned int latency;
    unsigned int video_scale;
    unsigned int frequency;
    int bits, channels;
    unsigned char sound_data[256];
};

struct bink_read_state;

struct bink_io_context
{
    char io_data[16];
    int (*read_callback)(void *);
    void (*close)(bink_io_context *);
    int (*background_control)(bink_io_context *, unsigned int);
    bink_read_state *bink;
    unsigned int suspended;
    volatile unsigned int doing_a_read;
    volatile unsigned int bytes_read;
    volatile unsigned int working;
    volatile unsigned int total_time;
    volatile unsigned int foreground_time;
    volatile unsigned int idle_time;
    volatile unsigned int thread_time;
    volatile unsigned int io_buffer_size;
    volatile unsigned int buffer_high_used;
    unsigned int buffer_size;
    unsigned int buffer_used;
    volatile unsigned int io_suspended;
    volatile unsigned char io_private_data[160];
    void (*suspend_callback)(bink_io_context *);
    int (*try_suspend_callback)(bink_io_context *);
    void (*resume_callback)(bink_io_context *);
    void (*idle_callback)(bink_io_context *);
    long callback;
};

struct s_bink_bundle_pointers
{
    void *type;
    void *type_16;
    void *color;
    void *bits_2;
    void *motion_x;
    void *motion_y;
    void *dct;
    void *motion_dct;
    void *pattern;
};

struct bink_read_state
{
    unsigned int movie_width, movie_height, frame_count;
    volatile unsigned int frame;
    unsigned int last_frame;
    unsigned int frame_rate, frame_rate_divisor;
    unsigned int read_error;
    unsigned int flags;
    unsigned int bink_type;
    unsigned int file_size, frame_size, sound_size;
    s_bink_rectangle frame_rectangles[8];
    int rectangle_count;
    s_bink_frame_buffers *buffers;
    void *mask_plane;
    unsigned int mask_pitch, mask_length;
    unsigned int largest_frame_size, internal_frames;
    int track_count;
    unsigned int highest_one_second_rate, highest_one_second_frame;
    int paused;
    unsigned int background_thread;
    void *compressed_frame;
    void *preloaded_data;
    unsigned int *frame_offsets;
    bink_io_context io;
    unsigned int io_callback_control[15];
    unsigned char *io_buffer;
    unsigned int io_buffer_size;
    unsigned int width, height;
    int *mix_bin_ids;
    unsigned int *track_sizes;
    unsigned int *track_types;
    int *mix_bin_map;
    unsigned int current_rectangle_count;
    unsigned int sound_enabled;
    unsigned int first_frame_time;
    unsigned int start_frame_time;
    unsigned int read_start;
    unsigned int playback_start;
    unsigned int start_sync_frame;
    unsigned int two_frames_time;
    volatile unsigned int frame_start;
    volatile unsigned int longest_frame;
    volatile unsigned int longest_frame_number;
    volatile unsigned int second_longest_frame;
    volatile unsigned int second_longest_frame_number;
    unsigned int sound_on, video_on;
    unsigned int allocated_bytes;
    unsigned int video_decompression_time, audio_decompression_time;
    unsigned int read_time;
    unsigned int open_time;
    unsigned int file_frame_rate, file_frame_rate_divisor;
    unsigned int runtime_frames;
    int runtime_index;
    unsigned int *runtime_frame_times;
    unsigned int *runtime_audio_decompression_times;
    unsigned int *runtime_video_decompression_times;
    unsigned int *runtime_blit_times;
    unsigned int *runtime_read_times;
    unsigned int *runtime_idle_read_times;
    unsigned int *runtime_thread_read_times;
    unsigned int last_blit_flags;
    unsigned int last_decompressed_frame;
    unsigned int last_resync_time;
    unsigned int do_resync;
    unsigned int skip_count;
    unsigned int too_few_skips_to_mask;
    unsigned int sound_count;
    unsigned int sound_skips;
    bink_sound_entry *sounds;
    unsigned int skipped_last_blit;
    unsigned int skipped_blits;
    s_bink_bundle_pointers bundle_pointers;
    unsigned int skipped_in_a_row;
    unsigned int big_sound_skip_adjustment;
    unsigned int big_sound_skip_reduce;
    unsigned int paused_time;
    unsigned int last_time_almost_empty;
    unsigned int io_priority;
    int sound_priority;
    long sound_callback;
    unsigned int sound_callback_control[15];
    int single_buffer;
};

struct BINKSND;
struct BINK;
typedef int (*binkread_sound_open)(BINKSND *, unsigned int, int, int, unsigned int, BINK *);
typedef binkread_sound_open (*binkread_sound_system_factory)(unsigned int);
struct binkread_sound_system_state
{
    binkread_sound_system_factory factory;
    binkread_sound_open sound_open;
    int active_sounds;
};

typedef char bink_bundle_pointers_size[(sizeof(void *) != 4 || ((sizeof(s_bink_bundle_pointers) == 36))) ? 1 : -1];
typedef char bink_read_bundle_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, bundle_pointers) == 764))) ? 1 : -1];
typedef char bink_read_big_sound_skip_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, big_sound_skip_reduce) == 808))) ? 1 : -1];
typedef char bink_read_callback_control_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, sound_callback_control) == 832))) ? 1 : -1];
typedef char bink_sound_data_offset[(sizeof(void *) != 4 || ((offsetof(bink_sound_entry, sound_data) == 124))) ? 1 : -1];
typedef char bink_read_frame_rectangles_offset[(offsetof(bink_read_state, frame_rectangles) == 52) ? 1 : -1];
typedef char bink_read_mask_plane_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, mask_plane) == 188))) ? 1 : -1];
typedef char bink_read_frame_offsets_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, frame_offsets) == 236))) ? 1 : -1];
typedef char bink_read_io_buffer_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, io_buffer) == 564))) ? 1 : -1];
typedef char bink_read_runtime_times_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, runtime_frame_times) == 692))) ? 1 : -1];
typedef char bink_io_callback_offset[(sizeof(void *) != 4 || ((offsetof(bink_io_context, callback) == 0x104))) ? 1 : -1];
typedef char bink_io_context_size[(sizeof(void *) != 4 || ((sizeof(bink_io_context) == 0x108))) ? 1 : -1];
typedef char bink_read_io_context_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, io) == 240))) ? 1 : -1];
typedef char bink_read_frame_offset[(offsetof(bink_read_state, frame) == 12) ? 1 : -1];
typedef char bink_read_buffers_offset[(offsetof(bink_read_state, buffers) == 184) ? 1 : -1];
typedef char bink_read_io_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, io.callback) == 500))) ? 1 : -1];
typedef char bink_read_width_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, width) == 572))) ? 1 : -1];
typedef char bink_read_ids_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, mix_bin_ids) == 580))) ? 1 : -1];
typedef char bink_read_timer_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, frame_start) == 628))) ? 1 : -1];
typedef char bink_read_sounds_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, sounds) == 752))) ? 1 : -1];
typedef char bink_read_sound_callback_offset[(sizeof(void *) != 4 || ((offsetof(bink_read_state, sound_callback) == 828))) ? 1 : -1];
typedef char bink_read_size[(sizeof(void *) != 4 || ((sizeof(bink_read_state) == 896))) ? 1 : -1];
typedef char bink_sound_entry_size[(sizeof(void *) != 4 || ((sizeof(bink_sound_entry) == 380))) ? 1 : -1];

extern "C" {
int checksound(...);
unsigned high1secrate(int frames, unsigned *ends, unsigned rate, int *hs, int *cont);
extern int g_bink_sound_tracks[];
extern char g_bink_error_string[];
extern int g_bink_io_thread;
extern long g_bink_sound_thread;
extern binkread_sound_system_state g_bink_sound_system;
}

#endif // BINK_BINKREAD_H
