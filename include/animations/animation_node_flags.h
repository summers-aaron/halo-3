#ifndef ANIMATIONS_ANIMATION_NODE_FLAGS_H
#define ANIMATIONS_ANIMATION_NODE_FLAGS_H
#include "cseries/cseries.h"
bool combine_node_flag_masks(const c_static_flags_no_init<255> *a,
    const c_static_flags_no_init<255> *b, c_static_flags_no_init<255> *result);
#endif
