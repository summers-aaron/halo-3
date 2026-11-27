#ifndef INTERFACE_SCREEN_TRANSITION_TYPES_H
#define INTERFACE_SCREEN_TRANSITION_TYPES_H

enum e_screen_transition_type {
    _screen_transition_none = 0,
    _screen_transition_normal = 1,
    _screen_transition_start_menu_previous = 3,
    _screen_transition_start_menu_next = 4,
    _screen_transition_custom0 = 5, // animates with custom transition in0 / out0
    _screen_transition_start_menu_back = 6,
};

#endif // INTERFACE_SCREEN_TRANSITION_TYPES_H
