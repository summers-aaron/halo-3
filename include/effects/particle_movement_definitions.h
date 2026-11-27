#ifndef EFFECTS_PARTICLE_MOVEMENT_DEFINITIONS_H
#define EFFECTS_PARTICLE_MOVEMENT_DEFINITIONS_H

// Functions defined in tu/blamlibxbox/particle_movement_definitions.cpp.

class c_particle;
union real_vector3d;
struct s_fullframe_codec_header;
struct real_orientation;

extern "C" void animation_fullframe_get_translation(s_fullframe_codec_header *data, long channel, int time,
    int, int, int, real_orientation *out);
extern "C" void animation_fullframe_get_scale(s_fullframe_codec_header *data, long channel, int time,
    int, int, int, real_orientation *out);
void accumulate_avoidance_force(const c_particle *particle, float distance, real_vector3d *force);

#endif // EFFECTS_PARTICLE_MOVEMENT_DEFINITIONS_H
