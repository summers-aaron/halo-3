#ifndef CAMERA_CAMERA_H
#define CAMERA_CAMERA_H

#include "camera/camera_mode_types.h"
#include "shell/shell.h"

// Functions defined in tu/blamlibxbox/camera.cpp that other units call.

extern "C" void camera_input_inhibited(void);
unsigned char camera_responding_gate(e_controller_index controller_index);

#endif // CAMERA_CAMERA_H
