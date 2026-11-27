#ifndef AI_STYLES_H
#define AI_STYLES_H

// Functions defined in tu/ailibxbox/styles.cpp that other units call.

struct style_definition;

style_definition *actor_get_task_style(long actor_index);

// Combat status for each style_definition::combat_status value.
extern short global_style_status_mapping[];

#endif // AI_STYLES_H
