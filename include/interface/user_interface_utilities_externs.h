#ifndef INTERFACE_USER_INTERFACE_UTILITIES_EXTERNS_H
#define INTERFACE_USER_INTERFACE_UTILITIES_EXTERNS_H

#include "networking/network_globals.h"

#include "interface/user_interface_shared_constants.h"

#include "cseries/cseries_unicode.h"

// External dependencies used by user_interface_utilities.
#include "math/real_math_types.h"
struct s_transport_security_globals;



extern "C" bool text_group_get_string(int tag_index, int handle, unsigned short *out);


#endif // INTERFACE_USER_INTERFACE_UTILITIES_EXTERNS_H
