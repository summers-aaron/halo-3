#ifndef CUTSCENE_RECORDED_ANIMATIONS_H
#define CUTSCENE_RECORDED_ANIMATIONS_H

// Functions defined in tu/blamlibxbox/recorded_animations.cpp.

#include <stddef.h>
#include "tag_files/tag_types.h"
#include "units/unit_control.h"
#include "cutscene/recorded_animation_playback.h"

// Scenario recorded-animations block element.
struct recorded_animation_definition
{
    char name[32];
    unsigned char version;
    unsigned char raw_animation_data;
    unsigned char unit_control_data_version;
    char pad;
    short length_of_animation;
    short pad1;
    long pad2;
    s_tag_data recorded_animation_event_stream;
};

// Datum of the animation_threads data array.
struct animation_thread
{
    unsigned short identifier;
    short pad;
    long unit_index;
    unsigned short ticks;
    unsigned short flags;
    long relative_ticks;
    unsigned char *event_stream;
    unit_control_data control_data;
    animation_playback_controller playback_controller;
    short codec_index;
};

typedef char recorded_animation_definition_size[(sizeof(recorded_animation_definition) == 64) ? 1 : -1];
typedef char recorded_animation_definition_ticks_offset[(offsetof(recorded_animation_definition, length_of_animation) == 36) ? 1 : -1];
typedef char recorded_animation_definition_events_offset[(offsetof(recorded_animation_definition, recorded_animation_event_stream) == 44) ? 1 : -1];
typedef char animation_thread_size[(sizeof(void *) != 4 || ((sizeof(animation_thread) == 160) )) ? 1 : -1];
typedef char animation_thread_flags_offset[(offsetof(animation_thread, flags) == 10) ? 1 : -1];
typedef char animation_thread_stream_offset[(offsetof(animation_thread, event_stream) == 16) ? 1 : -1];
typedef char animation_thread_control_offset[(sizeof(void *) != 4 || ((offsetof(animation_thread, control_data) == 20) )) ? 1 : -1];
typedef char animation_thread_controller_offset[(sizeof(void *) != 4 || ((offsetof(animation_thread, playback_controller) == 144) )) ? 1 : -1];
typedef char animation_thread_codec_offset[(sizeof(void *) != 4 || ((offsetof(animation_thread, codec_index) == 156) )) ? 1 : -1];

bool recorded_animation_play_internal(long unit_index, short animation_index, unsigned short play_flags);
bool recorded_animation_controlling_unit(long unit_index);
animation_thread *get_controlling_thread(long unit_index, long *thread_index);
void recorded_animations_update();
void __tls_set_g_animation_threads_allocator(void *object);
void __tls_set_g_animation_threads_data_allocator(void *allocation);

void recorded_animations_initialize();
void recorded_animations_dispose();
void recorded_animations_initialize_for_new_map();
void recorded_animations_dispose_from_old_map();

#endif // CUTSCENE_RECORDED_ANIMATIONS_H
