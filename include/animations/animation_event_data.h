#ifndef ANIMATIONS_ANIMATION_EVENT_DATA_H
#define ANIMATIONS_ANIMATION_EVENT_DATA_H

struct s_animation_event_data
{
    long frame;
    long event_type;
    union {
        long definition_index;
        short frame_event_type;
        unsigned short dialogue_index;
    };
    long flags;
    long marker_name;
};

#endif
