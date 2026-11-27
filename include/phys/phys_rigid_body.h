#ifndef INCLUDE_PHYS_RIGID_BODY_H
#define INCLUDE_PHYS_RIGID_BODY_H

class phys_collider;
class phys_vec4;
class phys_type_desc;
void estimate_allowed_penetration(phys_collider *collidable, float depth, const phys_vec4 &extent);
void fixup_loaded_rigid_body(void *memory);
void destroy_loaded_rigid_body(void *object);
const void *get_rigid_body_vtable();
extern const phys_type_desc phys_rigid_body_type_desc;
extern "C" {
extern const void *phys_max_size_motion_vtable[];
}

#endif
