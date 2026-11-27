#ifndef INTERFACE_FILE_SHARE_CATEGORY_DEPENDENCIES_H
#define INTERFACE_FILE_SHARE_CATEGORY_DEPENDENCIES_H

#include "shell/utility_drive.h"

static __forceinline bool content_file_share_enabled() { return g_utility_drive_mounted != 0; }

#endif
