#ifndef SOUND_SOUND_CHANNEL_SUMMARY_TYPES_H
#define SOUND_SOUND_CHANNEL_SUMMARY_TYPES_H

struct sound_channel_summary
{
    short like_channel_count;
    short like_channels[16];
    short maximum_like_channels;
    bool like_sound_started_this_tick;
    short object_channel_count;
    short object_channels[16];
    short maximum_object_channels;
};

#endif // SOUND_SOUND_CHANNEL_SUMMARY_TYPES_H
