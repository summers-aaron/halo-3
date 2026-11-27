#ifndef UNITS_SLIDER_MOVEMENT_PATTERN_TYPES_H
#define UNITS_SLIDER_MOVEMENT_PATTERN_TYPES_H
// Easing curve applied by c_mixing_board_slider::update; none is linear, ease_in is fraction squared.
enum e_slider_movement_pattern
{
    _slider_movement_pattern_none,
    _slider_movement_pattern_light_ease_in = 1,
    _slider_movement_pattern_full_ease_in = 2,
    _slider_movement_pattern_ease_in = 3,
    _slider_movement_pattern_light_ease_in_and_out = 5,
    _slider_movement_pattern_full_ease_in_and_out = 6
};
#endif // UNITS_SLIDER_MOVEMENT_PATTERN_TYPES_H
