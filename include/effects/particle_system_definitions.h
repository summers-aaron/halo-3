#ifndef EFFECTS_PARTICLE_SYSTEM_DEFINITIONS_H
#define EFFECTS_PARTICLE_SYSTEM_DEFINITIONS_H

#include <stddef.h>
#include "tag_files/tag_groups.h"

class c_particle_definition;
class c_particle_emitter_definition;

// Particle system definition layout and flag accessors, including the
// coordinate system, location, environment, disposition, and camera mode.
class c_particle_system_definition {
public:
    union { unsigned char particle_system_type; unsigned char limit_type; };
private:
    // 'prt3' reference; March c_particle_system_definition::get_particle_definition reads its index.
    s_tag_reference particle_definition;
public:
    long location_index;
    unsigned short m_coordinate_system;
    short environment;
    short disposition;
    union { unsigned short appearance; unsigned short camera_mode; };
    short m_sort_bias;
    unsigned short flags;
private:
    char spare_after_flags[4];
public:
    float far_fade;
    float near_fade;
    float override_fade;
public:
    float near_cutoff_distance;
    float near_fade_range;
    float near_fade_scale;
    float far_cutoff_distance;
    float far_fade_range;
    float far_fade_scale;
private:
    c_typed_tag_block<c_particle_emitter_definition> emitters;
    char spare_after_emitters[0x5c - 0x58];
public:
    __forceinline unsigned short get_coordinate_system() const { return m_coordinate_system; }
    __forceinline unsigned short get_camera_mode() const { return camera_mode; }
    __forceinline long get_particle_definition_index() const { return particle_definition.index; }
    __forceinline long get_emitter_count() const { return emitters.count(); }
    __forceinline const c_particle_emitter_definition *get_emitter(long index) const;
    __forceinline float get_sort_bias() const { return (float)m_sort_bias * 0.05f; }
    __forceinline unsigned char get_type() const { return particle_system_type; }
    __forceinline bool get_allowed_to_create() const {
        unsigned char result = (flags >> 5) & 1;
        return !result;
    }
    __forceinline bool get_valid() const {
        bool result;
        if (particle_definition.index == -1 ||
            !TAG_GET('prt3', const c_particle_definition, particle_definition.index) ||
            emitters.count() <= 0)
            result = false;
        else
            result = true;
        return result;
    }
    __forceinline bool get_cinematics() const { return (flags & 1) != 0; }
    __forceinline bool get_distance_fade_disabled() const {
        unsigned char result = (flags >> 2) & 1;
        return result != 0;
    }
    __forceinline bool get_distance_in_world_units() const {
        unsigned char result = (flags >> 3) & 1;
        return result != 0;
    }
    __forceinline unsigned char get_bit8() const {
        unsigned char result = (flags >> 8) & 1;
        return result;
    }
    __forceinline unsigned char get_bit12() const {
        unsigned char result = (flags >> 12) & 1;
        return result;
    }
    __forceinline bool get_bit7() const {
        unsigned char result = (flags >> 7) & 1;
        return result != 0;
    }
    __forceinline bool get_bit13() const {
        unsigned char result = (flags >> 13) & 1;
        return result != 0;
    }
    __forceinline const c_particle_definition *get_particle_definition() const {
        if (particle_definition.index != -1)
            return TAG_GET('prt3', const c_particle_definition, particle_definition.index);
        else
            return 0;
    }
};

typedef char particle_system_definition_size[(sizeof(c_particle_system_definition) == 0x5c) ? 1 : -1];
typedef char particle_system_definition_coordinate_offset[(offsetof(c_particle_system_definition, m_coordinate_system) == 0x18) ? 1 : -1];
typedef char particle_system_definition_location_offset[(offsetof(c_particle_system_definition, location_index) == 0x14) ? 1 : -1];
typedef char particle_system_definition_environment_offset[(offsetof(c_particle_system_definition, environment) == 0x1a) ? 1 : -1];
typedef char particle_system_definition_disposition_offset[(offsetof(c_particle_system_definition, disposition) == 0x1c) ? 1 : -1];
typedef char particle_system_definition_camera_offset[(offsetof(c_particle_system_definition, camera_mode) == 0x1e) ? 1 : -1];

typedef char particle_system_definition_far_fade_offset[(offsetof(c_particle_system_definition, far_fade) == 0x28) ? 1 : -1];
#endif // EFFECTS_PARTICLE_SYSTEM_DEFINITIONS_H
