#ifndef ANIMATIONS_ANIMATION_DEFINITIONS_INTERNAL_H
#define ANIMATIONS_ANIMATION_DEFINITIONS_INTERNAL_H

#include "math/real_math_constants.h"

#include "animations/animation_definitions.h"

struct s_animation_range_event_data {
  long frame;
  long type;
  union {
    struct { short type; short index; } key;
    struct { long asset; long flags; long marker; } asset;
    short dialogue;
    char data[24];
  } u;
};
typedef void (*s_animation_event_callback)(const s_animation_range_event_data *, long);
struct s_animation_playback_flags {
  unsigned short storage;
  __forceinline bool test(long bit) const { return (storage >> bit) & 1; }
};

extern "C" unsigned long check_animation_frame_range_for_events(const c_model_animation_graph *g, const c_model_animation *a,
    s_animation_playback_flags *play, float start, float end, s_animation_event_callback callback, long context);

#endif
