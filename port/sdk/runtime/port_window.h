#ifndef PORT_RUNTIME_PORT_WINDOW_H
#define PORT_RUNTIME_PORT_WINDOW_H

/*
 * The game's one OS window (HALO3_WINDOW=1), for the Vulkan renderer to present into
 * (port/sdk/d3d/vulkan_device.cpp). Linux: Xlib, loaded at run time from libX11.so.6 (window_linux.c); Windows:
 * user32.dll, loaded at run time (window_windows.c). Neither adds a link-time library dependency, and a machine with
 * no display still starts (port_window_open fails and the renderer stays offscreen).
 */

#ifdef __cplusplus
extern "C" {
#endif

/* e_vulkan_window_system values */
#define PORT_WINDOW_SYSTEM_NONE 0
#define PORT_WINDOW_SYSTEM_WIN32 1
#define PORT_WINDOW_SYSTEM_XLIB 2

/* The window system this build opens windows with. */
int port_window_system(void);
/* Opens and shows a window with a client area of width x height, in the window mode halo3.ini asks for (window_mode,
 * HALO3_WINDOW_MODE: port_config.h). Returns 1 on success (the reason for a failure is logged through
 * port_write_error). One window at a time. */
int port_window_open(const char *title, unsigned int width, unsigned int height);
/* Native handles for the Vulkan surface: Xlib Display * and Window; Win32 HINSTANCE and HWND. */
void *port_window_native_display(void);
__UINTPTR_TYPE__ port_window_native_window(void);
/* Handles pending window events without blocking, and puts the window in the mode the settings ask for once the Guide
 * changed it (windowed, borderless, fullscreen). Returns 0 once the user has asked to close the window. */
int port_window_pump(void);
/* The keyboard key (port/sdk/input/kbm_bindings.h e_port_key; k_port_key_none for none) a host key is: an X11 keysym
 * (host_flags unused) on Linux, a virtual-key code and its WM_KEYDOWN lParam (which of a left / right pair) on Windows.
 * The guide's own window reads its keys with it too (its binding capture: guide_capture). */
int port_window_key_code(__UINTPTR_TYPE__ host_key, __INTPTR_TYPE__ host_flags);
/* Returns 1 when no OS window is open (headless) or when the open OS window is currently active/focused; 0 when unfocused. */
int port_window_is_focused(void);
void port_window_close(void);

#ifdef __cplusplus
}
#endif

#endif /* PORT_RUNTIME_PORT_WINDOW_H */
