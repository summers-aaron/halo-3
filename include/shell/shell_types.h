#ifndef SHELL_SHELL_TYPES_H
#define SHELL_SHELL_TYPES_H

enum e_controller_index {
    _controller_none = -1,
    _controller_index_none = -1,
    k_no_controller = -1,
    k_controller_index_none = -1,
    _download_controller_none = -1,
    _controller0 = 0,
    _controller_index0 = 0,
    _controller_index_0 = 0,
    _controller1 = 1,
    _controller_index1 = 1,
    _controller_index_1 = 1,
    _controller2 = 2,
    _controller_index2 = 2,
    _controller_index_2 = 2,
    _controller3 = 3,
    _controller_index3 = 3,
    _controller_index_3 = 3,
    k_number_of_controllers = 4,
    k_any_controller = 255,
    _saved_film_no_controller = 255,
    _gui_screen_take_screenshot_no_controller = 255
};

enum e_window_index {
    _window_none = -1,
    _window_index_none = -1,
    k_no_window = -1,
    _window0 = 0,
    _saved_film_window_zero = 0,
    _window_dummy = 0,
    k_window_index_dummy = 0,
    _window1 = 1,
    _window2 = 2,
    _window3 = 3,
    _console_window = 4,
    _screenshot_console_window = 4,
    k_number_of_windows = 5,
    k_any_window = 255
};

typedef char shell_size_controller_index[(sizeof(e_controller_index) == 4) ? 1 : -1];
typedef char shell_size_window_index[(sizeof(e_window_index) == 4) ? 1 : -1];

#endif // SHELL_SHELL_TYPES_H
