// port/sdk/runtime: what port_config.c needs from the host (config_linux.c, config_windows.c) besides the storage
// layer's files and folders (port/sdk/kernel/storage_host.h).
#ifndef PORT_RUNTIME_CONFIG_HOST_H
#define PORT_RUNTIME_CONFIG_HOST_H

#ifdef __cplusplus
extern "C" {
#endif

// The process environment: the C library's getenv, and setting a variable for every later getenv (on Windows also
// the process's own environment, which GetEnvironmentVariable reads). Setting is for start-up, before other threads
// run; returns 1 when it was set.
const char *port_config_host_getenv(const char *name);
int port_config_host_setenv(const char *name, const char *value);
// The host's per-user settings folder with the game's appended: $XDG_CONFIG_HOME/halo3 (else ~/.config/halo3) on
// Linux, %APPDATA%\Halo3 on Windows. Returns 1, or 0 when the host does not say.
int port_config_host_config_directory(char *path, unsigned long size);
// The current folder. Returns 1, or 0 when it does not fit.
int port_config_host_current_directory(char *path, unsigned long size);
// Whether the launch is in a desktop session a window can open in (Linux: DISPLAY is set, as Xlib needs; Windows:
// always).
int port_config_host_display(void);
// Whether `path` is a file (not a folder).
int port_config_host_file_exists(const char *path);
// Whether `path` is absolute ("/..." on Linux; "C:\...", "\\server\..." or "\..." on Windows).
int port_config_host_absolute(const char *path);

#ifdef __cplusplus
}
#endif

#endif
