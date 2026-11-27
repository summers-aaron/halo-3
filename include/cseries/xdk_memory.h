#ifndef CSERIES_XDK_MEMORY_H
#define CSERIES_XDK_MEMORY_H

struct _XHASH_STATE;

// Size of the digest XHashMemoryEnd writes (xbox.h).
#ifndef XHASHMEMORY_DIGEST_SIZE
#define XHASHMEMORY_DIGEST_SIZE (20)
#endif

extern "C" {
void XHashMemoryUpdate(_XHASH_STATE *state, const void *data, unsigned long size);
void XHashMemoryEnd(const _XHASH_STATE *state, unsigned char *digest);
#if defined(_WIN64)
void *__stdcall XPhysicalAlloc(unsigned __int64 size, unsigned __int64 address, unsigned __int64 alignment, unsigned long protect);
void __stdcall XPhysicalProtect(void *address, unsigned __int64 size, unsigned long protect);
#else
void *__stdcall XPhysicalAlloc(unsigned long size, unsigned long address, unsigned long alignment, unsigned long protect);
void __stdcall XPhysicalProtect(void *address, unsigned long size, unsigned long protect);
#endif
unsigned long __stdcall XMemGetPageSize(const void *address);
unsigned long __stdcall XQueryMemoryProtect(void *address);
}

#endif // CSERIES_XDK_MEMORY_H
