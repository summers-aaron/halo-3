// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// malloc.h: heap allocation (stdlib.h's allocators) plus alloca and the aligned-heap entry points.
#pragma once
#ifndef _INC_MALLOC
#define _INC_MALLOC
#include <crtdefs.h>

#pragma pack(push, _CRT_PACKING)
#ifdef __cplusplus
extern "C" {
#endif

#define _HEAP_MAXREQ 0xFFFFFFE0
#define _HEAPEMPTY (-1)
#define _HEAPOK (-2)
#define _HEAPBADBEGIN (-3)
#define _HEAPBADNODE (-4)
#define _HEAPEND (-5)
#define _HEAPBADPTR (-6)
#define _FREEENTRY 0
#define _USEDENTRY 1
typedef struct _heapinfo
{
    int *_pentry;
    size_t _size;
    int _useflag;
} _HEAPINFO;

#ifndef _CRT_ALLOCATION_DEFINED
#define _CRT_ALLOCATION_DEFINED
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl calloc(size_t _NumOfElements, size_t _SizeOfElements);
_CRTIMP _CRTNOALIAS void __cdecl free(void *_Memory);
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl malloc(size_t _Size);
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl realloc(void *_Memory, size_t _NewSize);
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl _recalloc(void *_Memory, size_t _Count, size_t _Size);
_CRTIMP _CRTNOALIAS void __cdecl _aligned_free(void *_Memory);
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl _aligned_malloc(size_t _Size, size_t _Alignment);
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl _aligned_realloc(void *_Memory, size_t _Size, size_t _Alignment);
#endif
_CRTIMP _CRTNOALIAS _CRTRESTRICT void *__cdecl _aligned_offset_malloc(size_t _Size, size_t _Alignment, size_t _Offset);
_CRTIMP size_t __cdecl _msize(void *_Memory);
_CRTIMP void *__cdecl _expand(void *_Memory, size_t _NewSize);
_CRTIMP int __cdecl _heapchk(void);
_CRTIMP int __cdecl _heapmin(void);
int __cdecl _heapwalk(_HEAPINFO *_EntryInfo);
void *__cdecl _alloca(size_t _Size);
#define _mm_free(a) _aligned_free(a)
#define _mm_malloc(a, b) _aligned_malloc(a, b)
#define _resetstkoflw() (1)
#if !__STDC__
#define alloca _alloca
#endif

#ifdef __cplusplus
}
#endif
#pragma pack(pop)
#endif // _INC_MALLOC
