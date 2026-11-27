#ifndef SHELL_SHELL_H
#define SHELL_SHELL_H

struct shell_name_entry {
    int build_number;
    const char *name;
};

struct shell_name_table {
    shell_name_entry entries[1];
};

extern "C" {
extern shell_name_table g_shell_compatible_builds;
}

#include "shell/shell_types.h"

extern "C" bool g_shell_application_paused;

#endif // SHELL_SHELL_H
