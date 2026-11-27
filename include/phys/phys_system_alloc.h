#ifndef INCLUDE_PHYS_SYSTEM_ALLOC_H
#define INCLUDE_PHYS_SYSTEM_ALLOC_H

extern void *(__cdecl *phys_system_malloc)(int size, int alignment);
extern void *(__cdecl *phys_system_malloc)(int size, int alignment);
extern void (__cdecl *phys_system_free)(void *);

#endif // HAVOK_HK_SYSTEM_ALLOC_H
