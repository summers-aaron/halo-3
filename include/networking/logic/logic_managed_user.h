#ifndef NETWORKING_LOGIC_LOGIC_MANAGED_USER_H
#define NETWORKING_LOGIC_LOGIC_MANAGED_USER_H

#include "networking/online_property_id_types.h"
#include "shell/shell_types.h"

// Data defined in tu/networkinglibxbox/logic_managed_user.cpp that other units use.

struct s_managed_user_globals;

extern s_managed_user_globals managed_user_globals;

// Functions defined in tu/networkinglibxbox/logic_managed_user.cpp that other units call.

struct s_online_data;

void managed_user_set_property(e_controller_index controller_index, e_online_property_id property_id, const s_online_data *value);

#endif // NETWORKING_LOGIC_LOGIC_MANAGED_USER_H
