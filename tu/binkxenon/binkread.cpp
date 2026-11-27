// binkxenon/binkread.obj: from the records 0x70e600 0x70e658 0x70e6a0 0x70ebe0 0x70ec28 0x70ed08 0x70ed60 0x70eda8 0x70edb8 0x70edc8 0x70edd8 0x70ede0 0x70edf8 0x70ee48 0x70ef20 0x7101c8 0x7103e8 0x710578 0x710608 0x710850
// @flags /TP /O2 /Oc
// @address 0x70e600 bpopmalloc
// @address 0x70ebe0 BinkSetSoundTrack
// @address 0x70ec28 high1secrate
// @address 0x70ed60 bink_IO_callback
// @address 0x70edf8 bink_sound_callback
// @address 0x7101c8 endframe
// @address 0x7103e8 BinkPause
// @address 0x710608 BinkSetMixBins

#include <xtl.h>
#include "bink/radmem.h"
#include "bink/popmal.h"
#include "bink/rad_timer.h"
#include <string.h>
#include <stddef.h>
#include "bink/binkread.h"

// @address 0x7ceca0 g_bink_error_string
// @address 0x11db810 g_bink_sound_thread
// @address 0x11db800 g_bink_sound_system
// @address 0x11db80c g_bink_io_thread
// @address 0x11db828 g_bink_sound_tracks

#include "bink/radcb_abi.h"

char g_bink_error_string[256];

extern "C" void *bpopmalloc(bink_read_state *bink, int size)
{
    bink->allocated_bytes += popmalloctotal() + size;
    return ((void *(*)(int))popmalloc)(size);
}

// @address 0x70e658 bink_set_error_string
extern "C" void bink_set_error_string(const char *text)
{
    char *destination = g_bink_error_string;
    *destination = *text;
    do {
        if (!*text)
            return;
        ++text;
        ++destination;
        *destination = *text;
    } while (destination - g_bink_error_string + 1 != 255);
    destination[1] = 0;
}

// @address 0x70e6a0 BinkSetSoundSystem
extern "C" int BinkSetSoundSystem(binkread_sound_system_factory factory, unsigned int parameter)
{
    int result = 0;
    if (!factory)
        return result;
    if (g_bink_sound_system.factory) {
        if (g_bink_sound_system.factory != factory) {
            if (g_bink_sound_system.active_sounds)
                return 0;
            g_bink_sound_system.factory = factory;
        }
    } else {
        g_bink_sound_system.factory = factory;
    }
    binkread_sound_open sound_open = g_bink_sound_system.factory(parameter);
    if (sound_open)
        g_bink_sound_system.sound_open = sound_open;
    result = g_bink_sound_system.sound_open != 0;
    return result;
}

extern "C" void BinkSetSoundTrack(unsigned int count, const int *tracks)
{
    if (count > 32) count = 32;
    g_bink_sound_tracks[-2] = count;
    for (unsigned int i = 0; i < count; ++i) {
        g_bink_sound_tracks[i] = tracks[i];
    }
}

unsigned high1secrate(int frames, unsigned *ends, unsigned rate, int *hs, int *cont)
{
    unsigned high = 0; int pos = 0; int c = 1; int i;
    for (i = 0; i < frames - (int)rate; i++) {
        unsigned d = ends[i + rate] - ends[i];
        if (d > high) { high = d; pos = i; }
    }
    if (high == 0) {
        high = (unsigned)((((unsigned __int64)ends[frames] - (unsigned __int64)ends[0]) * rate) / (unsigned)frames);
    }
    for (i = 0; i < frames; i++) {
        if (!(ends[i] & 1)) { c = 0; break; }
    }
    *cont = c; *hs = pos;
    return high;
}

// @address 0x70ed08 bink_get_priority_IO
extern "C" int bink_get_priority_IO(bink_io_context *io, unsigned int priority)
{
    bink_read_state *bink = (bink_read_state *)
        ((char *)io - offsetof(bink_read_state, io.callback));
    if (!bink->io.suspended) {
        unsigned int percent = (bink->io.buffer_used + 1) * 100 / (bink->io.buffer_size + 1);
        if (percent < 50)
            return -1 - percent;
        if (bink->io_priority != priority)
            return 0x80000000u - bink->io_priority;
    }
    return 0;
}

extern "C" int bink_IO_callback(int context, int value)
{
    bink_read_state *b = (bink_read_state *)(context - offsetof(bink_read_state, io.callback));
    int r = b->io.read_callback(b->io.io_data);
    b->io_priority = value;
    return r;
}

// @address 0x70eda8 bink_suspend_io
extern "C" void bink_suspend_io(bink_io_context *io)
{
    RADCB_suspend_callback(g_bink_io_thread, &io->callback);
}

// @address 0x70edb8 bink_try_suspend_io
extern "C" void bink_try_suspend_io(bink_io_context *io)
{
    RADCB_try_to_suspend_callback(g_bink_io_thread, &io->callback);
}

// @address 0x70edc8 bink_resume_io
extern "C" void bink_resume_io(bink_io_context *io)
{
    RADCB_resume_callback(g_bink_io_thread, &io->callback);
}

// @address 0x70edd8 bink_idle_on_io
extern "C" void bink_idle_on_io(void)
{
    RADCB_idle_on_callbacks();
}

// @address 0x70ede0 bink_get_priority_sound
extern "C" int bink_get_priority_sound(int *callback, unsigned int priority)
{
    bink_read_state *bink = (bink_read_state *)
        ((char *)callback - offsetof(bink_read_state, sound_callback));
    if ((unsigned int)bink->sound_priority != priority)
        return -1 - bink->sound_priority;
    return 0;
}
extern "C" void bink_sound_callback(char *context, unsigned int value)
{
  bink_read_state *b = (bink_read_state *)(context - offsetof(bink_read_state, sound_callback));
  if (b->sound_count != 0U) {
    checksound(b);
  }
  b->sound_priority = value;
  return;
}

// @address 0x70ee48 BinkGetFrameBuffersInfo
extern "C" void BinkGetFrameBuffersInfo(bink_read_state *bink, s_bink_frame_buffers *buffers)
{
    if (!bink || !buffers)
        return;
    s_bink_frame_planes *frame = &buffers->frames[0];
    buffers->current_frame = 0;
    buffers->y_width = (bink->width + 7) & ~7;
    buffers->y_height = (bink->height + 7) & ~7;
    buffers->chroma_width = (((bink->width + 1) >> 1) + 7) & ~7;
    unsigned int height = bink->height;
    frame->y.Allocate = 1;
    buffers->frames[0].cr.Allocate = 1;
    buffers->frames[0].cb.Allocate = 1;
    buffers->chroma_height = (((height + 1) >> 1) + 7) & ~7;
    buffers->frames[0].alpha.Allocate = (bink->flags >> 20) & 1;
    buffers->frames[0].y.Buffer = 0;
    buffers->frames[0].cr.Buffer = 0;
    buffers->frames[0].cb.Buffer = 0;
    buffers->frames[0].alpha.Buffer = 0;
    buffers->frames[0].y.BufferPitch = 0;
    buffers->frames[0].cr.BufferPitch = 0;
    buffers->frames[0].cb.BufferPitch = 0;
    buffers->frames[0].alpha.BufferPitch = 0;
    if (bink->single_buffer) {
        buffers->count = 1;
    } else {
        buffers->count = 2;
        memcpy(&buffers->frames[1], frame, sizeof(s_bink_frame_planes));
    }
}

// @address 0x70ef20 bink_register_frame_buffers
extern "C" void bink_register_frame_buffers(bink_read_state *bink, s_bink_frame_buffers *buffers)
{
    if (bink && buffers) {
        bink->buffers = buffers;
        if (XQueryMemoryProtect(buffers->frames[0].y.Buffer) & 0x400)
            OutputDebugStringA(
                "\nERROR!! You are passing a write combined memory address to Bink!\n"
                "Playback will be *extremely* slow!  You must pass in read-write\n"
                "cached memory for the Bink texture pointers - see BinkTextures.cpp\n"
                "for an example.\n\n");
    }
}

extern "C" void endframe(bink_read_state *state, unsigned int frame)
{
    unsigned int start = state->frame_start;
    if (start == 0)
        return;

    unsigned int elapsed = frame - start;
    state->frame_start = 0;
    unsigned int previous = state->longest_frame;
    if (elapsed > previous)
    {
        state->longest_frame = elapsed;
        unsigned int old_value = state->longest_frame_number;
        unsigned int current = state->frame;
        state->second_longest_frame = previous;
        state->second_longest_frame_number = old_value;
        state->longest_frame_number = current;
        return;
    }

    unsigned int max_value = state->second_longest_frame;
    if (elapsed <= max_value)
        return;

    unsigned int current = state->frame;
    state->second_longest_frame = elapsed;
    state->second_longest_frame_number = current;
}

extern "C" int BinkPause(bink_read_state *bink, int pause)
{
    int resume = 0;
    if (!bink)
        return 0;
    unsigned int now = RADTimerRead();
    if (bink->read_start) {
        bink->read_time += now - bink->read_start;
        bink->read_start = 0;
    }
    unsigned int start = bink->frame_start;
    if (start) {
        unsigned int elapsed = now - start;
        unsigned int previous = bink->longest_frame;
        bink->frame_start = 0;
        if (elapsed > previous) {
            bink->longest_frame = elapsed;
            unsigned int old_number = bink->longest_frame_number;
            unsigned int current = bink->frame;
            bink->second_longest_frame = previous;
            bink->second_longest_frame_number = old_number;
            bink->longest_frame_number = current;
        } else if (elapsed > bink->second_longest_frame) {
            unsigned int current = bink->frame;
            bink->second_longest_frame = elapsed;
            bink->second_longest_frame_number = current;
        }
    }
    unsigned int previously_paused = bink->paused;
    bink->paused = pause;
    RADCB_suspend_callback(g_bink_sound_thread, &bink->sound_callback);
    for (unsigned int i = 0; i < bink->sound_count; ++i)
        bink->sounds[i].pause(&bink->sounds[i], pause);
    now = RADTimerRead();
    if (!pause) {
        if (previously_paused)
            resume = 1;
    } else if (!previously_paused) {
        if (bink->playback_start)
            bink->paused_time = now - bink->playback_start;
        else
            bink->paused_time = 0;
    }
    RADCB_resume_callback(g_bink_sound_thread, &bink->sound_callback);
    if (bink->sound_enabled && bink->sound_count)
        checksound(bink);
    if (resume)
        bink->playback_start = now - bink->paused_time;
    return bink->paused;
}

static __forceinline int binkread_find_sound(bink_read_state *bink, int channel)
{
    int count = bink->sound_count;
    int index = 0;
    if (count > 0) {
        int *map = bink->mix_bin_map;
        int *ids = bink->mix_bin_ids;
        do {
            if (map[*ids] == channel)
                return index;
            ++index;
            ++ids;
        } while (index < count);
    }
    return -1;
}

// @address 0x710578 bink_set_volume
extern "C" void bink_set_volume(bink_read_state *bink, int channel, int volume)
{
    if (bink && (unsigned int)bink->sound_count != 0) {
        int index = binkread_find_sound(bink, channel);
        if (index != -1) {
            // match: 0x710578: recompute the byte offset per use; folding it loses the address reload.
            unsigned int offset = (unsigned int)index * sizeof(bink_sound_entry);
            if (((bink_sound_entry *)((char *)bink->sounds + offset))->set_volume)
                ((bink_sound_entry *)((char *)bink->sounds + offset))->set_volume(
                    (bink_sound_entry *)((char *)bink->sounds + offset), volume);
        }
    }
}

extern "C" void BinkSetMixBins(bink_read_state *bink, int mix_bin_id, unsigned int *mix_bins, int mix_bin_count)
{
    if (bink == 0)
        return;
    if ((unsigned int)bink->sound_count == 0)
        return;
    int index = binkread_find_sound(bink, mix_bin_id);
    if (index == -1)
        return;
    // match: 0x710608: recompute the byte offset per use; folding it loses the address reload.
    unsigned int offset = (unsigned int)index * sizeof(bink_sound_entry);
    if (((bink_sound_entry *)((char *)bink->sounds + offset))->set_mix_bins == 0)
        return;
    ((bink_sound_entry *)((char *)bink->sounds + offset))->set_mix_bins(
        (bink_sound_entry *)((char *)bink->sounds + offset), mix_bins, mix_bin_count);
}
// @address 0x710850 BinkSetMemory
extern "C" void BinkSetMemory(void *(*allocator)(unsigned int), void (*deallocator)(void *))
{
    RADSetMemory(allocator, deallocator);
}
