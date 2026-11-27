#ifndef CAMERA_SAVED_FILM_DIRECTOR_STORAGE_H
#define CAMERA_SAVED_FILM_DIRECTOR_STORAGE_H
#include "game/player_control_globals.h"

#include "game/player_control_types.h"

struct s_controllers;
extern "C" {
extern __declspec(thread) bool *g_director_camera_scripted;
extern __declspec(thread) s_controllers *g_director_globals;
}

#endif
