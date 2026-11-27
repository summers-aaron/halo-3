#ifndef OBJECTS_OBJECT_AI_PROPERTIES_H
#define OBJECTS_OBJECT_AI_PROPERTIES_H

#include <stddef.h>
// Shared tag element: path_obstacle_avoidance reads flags and size;
// actor_moving_ground reads the leap-jump speed at 0x0e.
struct object_ai_properties
{
    long flags;
    long ai_type_name;
    // TagTool GameObject.AiProperty Padding0: H3 keeps 4 pad bytes here (Reach reuses them as InteractionName).
    char ai_type_name_padding[4];
    short size;
    short leap_jump_speed;
};
typedef char object_ai_properties_size[(sizeof(object_ai_properties) == 0x10) ? 1 : -1];
typedef char object_ai_properties_jump_offset[(offsetof(object_ai_properties, leap_jump_speed) == 0x0e) ? 1 : -1];
#endif
