// port/sdk/runtime (Linux host): the environment, folders and files port_config.c asks about (config_host.h).
#include "config_host.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

const char *port_config_host_getenv(const char *name)
{
    return getenv(name);
}

int port_config_host_setenv(const char *name, const char *value)
{
    return setenv(name, value, 1) == 0;
}

int port_config_host_config_directory(char *path, unsigned long size)
{
    const char *xdg = getenv("XDG_CONFIG_HOME");
    int written = -1;
    if (xdg != 0 && xdg[0] == '/') { // the XDG base directory specification: a relative value is ignored
        written = snprintf(path, size, "%s/halo3", xdg);
    } else {
        const char *home = getenv("HOME");
        if (home != 0 && home[0] != 0)
            written = snprintf(path, size, "%s/.config/halo3", home);
    }
    return written > 0 && (unsigned long)written < size;
}

int port_config_host_current_directory(char *path, unsigned long size)
{
    return getcwd(path, size) != 0;
}

int port_config_host_display(void)
{
    const char *display = getenv("DISPLAY");
    return display != 0 && display[0] != 0;
}

int port_config_host_file_exists(const char *path)
{
    struct stat info;
    return stat(path, &info) == 0 && S_ISREG(info.st_mode);
}

int port_config_host_absolute(const char *path)
{
    return path[0] == '/';
}
