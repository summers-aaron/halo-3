#ifndef OBJECTS_OBJECT_FUNCTION_PREVIEW_H
#define OBJECTS_OBJECT_FUNCTION_PREVIEW_H

struct s_objects_function_preview
{
    unsigned char primary_ammunition;
    unsigned char secondary_ammunition;
    unsigned char flags;
    signed char primary_trigger;
    signed char secondary_trigger;
    signed char heat;
};

extern "C" s_objects_function_preview g_objects_function_preview;

#endif // OBJECTS_OBJECT_FUNCTION_PREVIEW_H
