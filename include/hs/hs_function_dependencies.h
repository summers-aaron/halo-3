#ifndef HS_HS_FUNCTION_DEPENDENCIES_H
#define HS_HS_FUNCTION_DEPENDENCIES_H



extern "C" void camera_fx_animate_exposure(float a, float b, float c, float d, float e);
extern "C" unsigned char first_person_weapon_raise(unsigned int user_index, int slot_index);
extern "C" int ai_scripting_set_objective(int, int);
extern "C" void actor_set_berserk(long actor_index, unsigned char berserk, unsigned char latch);

#endif // HS_HS_FUNCTION_DEPENDENCIES_H
