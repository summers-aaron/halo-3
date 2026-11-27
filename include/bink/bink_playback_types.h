#ifndef BINK_BINK_PLAYBACK_TYPES_H
#define BINK_BINK_PLAYBACK_TYPES_H

#include <xtl.h>
#include <stddef.h>
#include "memory/static_arrays.h"
#include "multithreading/synchronized_value.h"

struct s_bink_rectangle
{
    int left, top, width, height;
};

struct s_bink_frame_buffers;
struct bink_io_context;
struct bink_read_state;
struct bink_sound_entry;

struct s_bink_info {
    unsigned long width;
    unsigned long height;
    unsigned long frame_count;
    unsigned long current_frame;
    unsigned long last_frame;
    unsigned long frame_rate_divisor;
    unsigned long frame_rate;
    unsigned long read_error, open_flags, bink_type;
    unsigned long file_size, frame_size, sound_size;
    s_bink_rectangle frame_rectangles[8];
    long rectangle_count;
    s_bink_frame_buffers *frame_buffers;
    void *mask_plane;
    unsigned long mask_pitch, mask_length;
    unsigned long largest_frame_size, internal_frames;
    long track_count;
    unsigned long highest_one_second_rate, highest_one_second_frame;
    long paused;
    unsigned long background_thread;
    char *data;
    char *audio0;
    char *audio1;
    char io_data[16];
    int (*io_read_callback)(void *);
    void (*io_close)(bink_io_context *);
    int (*io_background_control)(bink_io_context *, unsigned int);
    bink_read_state *io_bink;
    unsigned int io_suspended;
    volatile unsigned int io_doing_a_read;
    volatile unsigned int io_bytes_read;
    long seeking;
    volatile unsigned int io_total_time;
    volatile unsigned int io_foreground_time;
    volatile unsigned int io_idle_time;
    volatile unsigned int io_thread_time;
    volatile unsigned int io_buffer_capacity;
    volatile unsigned int io_buffer_high_used;
    unsigned int io_buffer_size;
    unsigned int io_buffer_used;
    volatile unsigned int io_background_suspended;
    volatile unsigned char io_private_data[160];
    void (*io_suspend_callback)(bink_io_context *);
    int (*io_try_suspend_callback)(bink_io_context *);
    void (*io_resume_callback)(bink_io_context *);
    void (*io_idle_callback)(bink_io_context *);
    int io_callback;
#ifdef HALO3_64BIT
    // The SDK's nested I/O context ends on a pointer-aligned boundary.
    unsigned int io_tail_padding;
#endif
    unsigned int io_callback_control[15];
    unsigned char *read_buffer;
    unsigned int read_buffer_size;
    unsigned int buffer_width, buffer_height;
    int *mix_bin_ids;
    char *plane0, *plane1, *plane2;
    unsigned int current_rectangle_count;
    unsigned int sound_track_enabled;
    unsigned int first_frame_time;
    unsigned int start_frame_time;
    unsigned int read_start;
    long frame_time;
    unsigned int start_sync_frame;
    unsigned int two_frames_time;
    volatile unsigned int frame_start;
    volatile unsigned int longest_frame;
    volatile unsigned int longest_frame_number;
    volatile unsigned int second_longest_frame;
    volatile unsigned int second_longest_frame_number;
    long sound_on, sound_enabled;
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
    unsigned long decoded_frame;
    unsigned int last_resync_time;
    unsigned int do_resync;
    unsigned int skip_count;
    unsigned int too_few_skips_to_mask;
    unsigned long has_sound;
    unsigned int sound_skips;
    bink_sound_entry *sounds;
    long frame_decoded, seek_time;
    void *bundle_type;
    void *bundle_type_16;
    void *bundle_color;
    void *bundle_bits_2;
    void *bundle_motion_x;
    void *bundle_motion_y;
    void *bundle_dct;
    void *bundle_motion_dct;
    void *bundle_pattern;
    unsigned int skipped_in_a_row;
    unsigned int big_sound_skip_adjustment;
    unsigned int big_sound_skip_reduce;
    long sound_time;
    unsigned int last_time_almost_empty;
    unsigned int io_priority;
    int sound_priority;
    long sound_callback;
};
typedef char bink_rectangle_size[(sizeof(s_bink_rectangle) == 16) ? 1 : -1];
typedef char bink_info_frame_buffers_offset[(offsetof(s_bink_info, frame_buffers) == 184) ? 1 : -1];
typedef char bink_info_data_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_info, data) == 0xe4))) ? 1 : -1];
typedef char bink_info_seeking_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_info, seeking) == 0x11c))) ? 1 : -1];
typedef char bink_info_plane_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_info, plane0) == 0x248))) ? 1 : -1];
typedef char bink_info_io_callback_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_info, io_callback) == 500))) ? 1 : -1];
typedef char bink_info_runtime_times_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_info, runtime_frame_times) == 692))) ? 1 : -1];
typedef char bink_info_decoded_frame_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_info, decoded_frame) == 0x2d4))) ? 1 : -1];
typedef char bink_info_sounds_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_info, sounds) == 752))) ? 1 : -1];
typedef char bink_info_bundle_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_info, bundle_type) == 764))) ? 1 : -1];
typedef char bink_info_sound_callback_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_info, sound_callback) == 0x33c))) ? 1 : -1];
enum
{
    _bink_prediction_message_none = 0,
    _bink_prediction_message_queued = 2,
};
struct s_bink_prediction_message
{
    c_static_string<256> name;
    long index;
    long state;
    bool recording;
    unsigned long flags;
    __forceinline s_bink_prediction_message &operator=(const s_bink_prediction_message &other)
    {
        memcpy(this, &other, sizeof(*this));
        return *this;
    }
};
typedef D3DTexture s_bink_texture;
struct BINKPLANE {
    long Allocate;
    void *Buffer;
    unsigned long BufferPitch;
};
struct s_bink_frame_planes { BINKPLANE y, cr, cb, alpha; };
struct s_bink_frame_buffers
{
    long count;
    unsigned long y_width, y_height, chroma_width, chroma_height;
    long current_frame;
    s_bink_frame_planes frames[2];
};
struct s_bink_prepare_texture_set
{
    s_bink_texture y, cr, cb, alpha;
    unsigned long fence;
    bool fence_pending;
};
struct s_bink_video_state
{
    bool started[2];
    s_bink_prepare_texture_set texture_sets[2];
    s_bink_frame_buffers buffers;
    void *texture_memory[2];
    unsigned long texture_memory_size[2];
};
struct s_bink_runtime
{
    bool thread_started;
    bool playing;
    float fade_start_time;
    float playback_rate;
    long frame_index;
    long frames_decoded;
    long frames_skipped;
    long thread_running;
    long thread_finished;
    long exit_requested;
    // March bink_thread_should_stop_playback reads this word: the worker thread stops when it is 0.
    long playback_requested;
    // March bink_thread_should_stop_forever reads this word: the worker thread exits when it is nonzero.
    long stop_forever_requested;
    s_bink_info *bink;
    s_bink_video_state video;
    bool have_decoded_texture;
    long decoded_texture;
    unsigned long next_frame;
    long movie_width;
    long movie_height;
    c_synchronized_long prediction_state_lock;
    s_bink_prediction_message queued_prediction_message;
};
struct _bink_globals {
    bool enabled;
    bool active;
    char spare_after_active[2];
    bool ready;
    char spare_after_ready;
    bool playing;
    unsigned long start_tick;
    long movie_tag_index;
    void *movie_data;
    unsigned __int64 movie_resource_flags;
    unsigned long control_flags;
    s_bink_prediction_message message;
    bool recording_enabled;
    s_bink_runtime runtime;
    void *pending_load_procs;
    unsigned long load_procs_pending;
    char *memory_pool;
    long memory_pool_offset, memory_pool_size;
};

typedef char bink_prediction_message_size[(sizeof(s_bink_prediction_message) == 0x110) ? 1 : -1];
typedef char bink_texture_set_size[(sizeof(s_bink_prepare_texture_set) == 0xd8) ? 1 : -1];
typedef char bink_runtime_size[(sizeof(void *) != 4 || ((sizeof(s_bink_runtime) == 0x394) )) ? 1 : -1];
typedef char bink_globals_size[(sizeof(void *) != 4 || ((sizeof(_bink_globals) == 0x4e0) )) ? 1 : -1];
typedef char bink_globals_enabled_offset[(offsetof(_bink_globals, enabled) == 0) ? 1 : -1];
typedef char bink_globals_active_offset[(offsetof(_bink_globals, active) == 1) ? 1 : -1];
typedef char bink_globals_runtime_offset[(sizeof(void *) != 4 || ((offsetof(_bink_globals, runtime) == 0x138) )) ? 1 : -1];
typedef char bink_globals_load_procs_offset[(sizeof(void *) != 4 || ((offsetof(_bink_globals, pending_load_procs) == 0x4cc) )) ? 1 : -1];


typedef char bink_runtime_texture_memory_size_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_runtime, video.texture_memory_size) == 0x264))) ? 1 : -1];
typedef char bink_runtime_next_frame_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_runtime, next_frame) == 0x274))) ? 1 : -1];

typedef char bink_runtime_video_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_runtime, video) == 0x30))) ? 1 : -1];
typedef char bink_prediction_flags_offset[(offsetof(s_bink_prediction_message, flags) == 0x10c) ? 1 : -1];
typedef char bink_texture_fence_pending_offset[(offsetof(s_bink_prepare_texture_set, fence_pending) == 0xd4) ? 1 : -1];
typedef char bink_video_texture_sets_offset[(offsetof(s_bink_video_state, texture_sets) == 4) ? 1 : -1];
typedef char bink_runtime_fade_time_offset[(offsetof(s_bink_runtime, fade_start_time) == 4) ? 1 : -1];
typedef char bink_runtime_decoded_texture_offset[(sizeof(void *) != 4 || ((offsetof(s_bink_runtime, decoded_texture) == 0x270))) ? 1 : -1];
typedef char bink_globals_resource_flags_offset[(offsetof(_bink_globals, movie_resource_flags) == 0x18) ? 1 : -1];
#endif // BINK_BINK_PLAYBACK_TYPES_H
