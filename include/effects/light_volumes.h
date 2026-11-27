#ifndef EFFECTS_LIGHT_VOLUMES_H
#define EFFECTS_LIGHT_VOLUMES_H

// Functions defined in tu/blamlibxbox/light_volumes.cpp.

struct real_matrix4x3;
class c_light_volume_system;

void __tls_set_g_light_volume_system_allocator(void *object);
void __tls_set_g_light_volume_allocator(void *object);
extern "C" long light_volume_location_new(unsigned char type, unsigned long handle, const real_matrix4x3 *matrix, unsigned long value);
extern "C" void light_volume_system_render(c_light_volume_system *system, long user_index, long pass);

#endif // EFFECTS_LIGHT_VOLUMES_H
