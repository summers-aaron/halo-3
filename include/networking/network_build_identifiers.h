#ifndef NETWORKING_NETWORK_BUILD_IDENTIFIERS_H
#define NETWORKING_NETWORK_BUILD_IDENTIFIERS_H

#include "networking/network_utilities.h"

static inline void network_get_build_identifiers(
    long *executable_type, long *executable_version, long *compatible_version)
{
    *executable_type = 5;
    *executable_version = 11855;
    *compatible_version = k_network_compatible_version;
}
static inline bool network_build_is_compatible(
    long executable_type, long executable_version, long compatible_version)
{
    bool compatible = true;
    long local_executable_type;
    long local_executable_version;
    long local_compatible_version;
    network_get_build_identifiers(
        &local_executable_type, &local_executable_version, &local_compatible_version);
    compatible = compatible && (executable_type == local_executable_type);
    compatible = compatible && (executable_version >= local_compatible_version);
    compatible = compatible && (local_executable_version >= compatible_version);
    return compatible;
}

#endif
