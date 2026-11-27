#ifndef PHYSICS_HAVOK_COMPONENT_ACTIVE_ENTITY_H
#define PHYSICS_HAVOK_COMPONENT_ACTIVE_ENTITY_H

// havok.cpp currently exposes the component address as an integer. Keep its
// accepted C ABI until the pointer-based callers and definition migrate together.
extern "C" unsigned char havok_component_has_active_entity(int component_address);

#endif // PHYSICS_HAVOK_COMPONENT_ACTIVE_ENTITY_H
