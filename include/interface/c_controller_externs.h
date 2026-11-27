#ifndef INTERFACE_C_CONTROLLER_EXTERNS_H
#define INTERFACE_C_CONTROLLER_EXTERNS_H

// External dependencies used by c_controller.
class c_content_catalogue;

#include "networking/network_configuration.h"
#include "interface/user_interface_session.h"
extern "C" void content_catalogue_update_enumeration_task(const c_content_catalogue *catalogue);

#endif // INTERFACE_C_CONTROLLER_EXTERNS_H
