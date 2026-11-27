#ifndef PHYSICS_HAVOK_ACTIVE_ENTITY_H
#define PHYSICS_HAVOK_ACTIVE_ENTITY_H

// havok.cpp's accepted C entry point. Pointer signatures in older callers await a joint migration.
extern "C" unsigned char havok_component_has_active_entity(int component);

#endif // PHYSICS_HAVOK_ACTIVE_ENTITY_H
