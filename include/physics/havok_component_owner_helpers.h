#ifndef PHYSICS_HAVOK_COMPONENT_OWNER_HELPERS_H
#define PHYSICS_HAVOK_COMPONENT_OWNER_HELPERS_H

// Owner-only declarations; include after the complete component definition.
extern "C" void *havok_component_delete_phantom_bodies(phys_array<c_havok_component::s_body_in_phantom> *array, unsigned int flags);

#endif
