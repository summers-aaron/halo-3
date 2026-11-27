#ifndef VISIBILITY_VISIBILITY_COLLECTION_FRAME_TYPES_H
#define VISIBILITY_VISIBILITY_COLLECTION_FRAME_TYPES_H

struct s_visibility_collection_frame
{
    bool active;
    long light_mark;
};

typedef char visibility_collection_frame_size[(sizeof(void *) != 4 || sizeof(s_visibility_collection_frame) == 8) ? 1 : -1];

extern "C" s_visibility_collection_frame g_visibility_collection_frame;

#endif // VISIBILITY_VISIBILITY_COLLECTION_FRAME_TYPES_H
