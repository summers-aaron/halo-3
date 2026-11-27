// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// vectorintrinsics.h: the 128-bit __vector4 register type and the VMX / VMX128 vector intrinsics (implemented on
// the PC by port/sdk/vmx/vmx128.cpp; port/sdk/include/vectorintrinsics.h wraps this file and replaces __lvx/__stvx).
#ifndef __VECTORINTRINSICS_H__
#define __VECTORINTRINSICS_H__

#ifndef _NOALIGNMENT_
#define _DECLSPEC_ALIGN_16_ __declspec(align(16))
#else
#define _DECLSPEC_ALIGN_16_
#endif

// One vector register: four floats, also addressable as an array of floats or of 32-bit words.
typedef struct _DECLSPEC_ALIGN_16_ __vector4 {
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        float v[4];
        unsigned int u[4];
    };
} __vector4;

// Data formats of the D3D pack / unpack instructions (vpkd3d128 / vupkd3d128).
typedef enum __VECTOR_PACK_TYPES {
    VPACK_D3DCOLOR = 0,
    VPACK_NORMSHORT2 = 1,
    VPACK_NORMPACKED32 = 2,
    VPACK_FLOAT16_2 = 3,
    VPACK_NORMSHORT4 = 4,
    VPACK_FLOAT16_4 = 5,
    VPACK_NORMPACKED64 = 6
} __VECTOR_PACK_TYPES;

// Which words of the destination a vpkd3d128 writes (positioned by its rotate count).
typedef enum __VECTOR_PACK_MASK {
    VPACK_32 = 1,
    VPACK_64LO = 2,
    VPACK_64HI = 3
} __VECTOR_PACK_MASK;

#ifdef __cplusplus
extern "C" {
#endif

// Whole-vector load / store: a plain 16-byte access at base + offset (bytes).
#ifdef __cplusplus
#define __lvx(base, offset) \
    (*(reinterpret_cast<const __vector4 *>(reinterpret_cast<const char *>(base) + (offset))))
#define __stvx(vSrc, base, offset) \
    *(reinterpret_cast<__vector4 *>(reinterpret_cast<char *>(base) + (offset))) = (vSrc);
#else
#define __lvx(base, offset) (*((__vector4 *)((char *)(base) + (offset))))
#define __stvx(vSrc, base, offset) *((__vector4 *)((char *)(base) + (offset))) = (vSrc);
#endif

// Element and left / right partial loads and stores.
__vector4 __lvewx(const void *base, int offset);
void __stvehx(__vector4 vSrc, void *base, int offset);
void __stvewx(__vector4 vSrc, void *base, int offset);
__vector4 __lvlx(const void *base, int offset);
__vector4 __lvrx(const void *base, int offset);
void __stvlx(__vector4 vSrc, void *base, int offset);
void __stvrx(__vector4 vSrc, void *base, int offset);

// Floating-point arithmetic.
__vector4 __vaddfp(__vector4 VRA, __vector4 VRB);
__vector4 __vsubfp(__vector4 VRA, __vector4 VRB);
__vector4 __vmulfp(__vector4 VRA, __vector4 VRB);
__vector4 __vmaddfp(__vector4 VRA, __vector4 VRC, __vector4 VRB);
__vector4 __vnmsubfp(__vector4 VRA, __vector4 VRB, __vector4 VRC);
__vector4 __vmaxfp(__vector4 VRA, __vector4 VRB);
__vector4 __vminfp(__vector4 VRA, __vector4 VRB);
__vector4 __vmsum3fp(__vector4 VRA, __vector4 VRB);
__vector4 __vmsum4fp(__vector4 VRA, __vector4 VRB);
__vector4 __vrefp(__vector4 VRB);
__vector4 __vrsqrtefp(__vector4 VRB);
__vector4 __vrfin(__vector4 VRB);
__vector4 __vrfiz(__vector4 VRB);

// Fixed <-> float conversions (scale = power-of-two exponent).
__vector4 __vcfsx(__vector4 VRB, unsigned short scale);
__vector4 __vcfux(__vector4 VRB, unsigned short scale);
__vector4 __vctsxs(__vector4 VRB, unsigned int scale);
__vector4 __vctuxs(__vector4 VRB, unsigned int scale);
#define __vcuxwfp __vcfux

// Compares; the R forms also return the CR6 summary through CR.
__vector4 __vcmpeqfp(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpgefp(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpgtfp(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpequw(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpgtuh(__vector4 VRA, __vector4 VRB);
__vector4 __vcmpeqfpR(__vector4 VRA, __vector4 VRB, unsigned int *CR);
__vector4 __vcmpequwR(__vector4 VRA, __vector4 VRB, unsigned int *CR);
__vector4 __vcmpgefpR(__vector4 VRA, __vector4 VRB, unsigned int *CR);
__vector4 __vcmpgtfpR(__vector4 VRA, __vector4 VRB, unsigned int *CR);

// Integer arithmetic, logic, shifts.
__vector4 __vadduhm(__vector4 VRA, __vector4 VRB);
__vector4 __vsubuhm(__vector4 VRA, __vector4 VRB);
__vector4 __vand(__vector4 VRA, __vector4 VRB);
__vector4 __vandc(__vector4 VRA, __vector4 VRB);
__vector4 __vnor(__vector4 VRA, __vector4 VRB);
__vector4 __vor(__vector4 VRA, __vector4 VRB);
__vector4 __vxor(__vector4 VRA, __vector4 VRB);
__vector4 __vsel(__vector4 VRA, __vector4 VRB, __vector4 VRC);
__vector4 __vslw(__vector4 VRA, __vector4 VRB);
__vector4 __vsraw(__vector4 VRA, __vector4 VRB);
__vector4 __vsrw(__vector4 VRA, __vector4 VRB);

// Splats, merges, permutes, rotates.
__vector4 __vspltisw(int SIM);
__vector4 __vsplth(__vector4 VRB, unsigned int UIM);
__vector4 __vspltw(__vector4 VRB, unsigned int UIM);
__vector4 __vmrghh(__vector4 VRA, __vector4 VRB);
__vector4 __vmrghw(__vector4 VRA, __vector4 VRB);
__vector4 __vmrglw(__vector4 VRA, __vector4 VRB);
__vector4 __vperm(__vector4 VRA, __vector4 VRB, __vector4 VRC);
__vector4 __vpermwi(__vector4 VRB, unsigned int UIM);
__vector4 __vsldoi(__vector4 VRA, __vector4 VRB, unsigned int SHB);
__vector4 __vrlimi(__vector4 VRT, __vector4 VRB, unsigned int WMASK, unsigned int SHW);

// Packs / unpacks.
__vector4 __vpkuwus(__vector4 VRA, __vector4 VRB);
__vector4 __vpkd3d(__vector4 VRT, __vector4 VRB, __VECTOR_PACK_TYPES DT, __VECTOR_PACK_MASK MS, unsigned int SHW);
__vector4 __vupkd3d(__vector4 VRB, __VECTOR_PACK_TYPES DT);

// The /QVMXReserve register file (VR64..VR95); the register number must be a constant.
__vector4 __VMXGetReg(const int reg);
void __VMXSetReg(const int reg, __vector4 value);

// A zero vector.
#define __vzero() __vspltisw(0)

// __vpermwi control: the source element (0..3) for each of x, y, z, w.
#define VPERMWI_CONST(x, y, z, w) ((((x) & 3) << 6) | (((y) & 3) << 4) | (((z) & 3) << 2) | ((w) & 3))

// Unaligned 16-byte access built from the left / right partial forms (the right form takes the address 16 on).
#define __loadunalignedvector(ptr) __vor(__lvlx(ptr, 0), __lvrx(ptr, 16))
#define __storeunalignedvector(v, ptr) (__stvlx(v, ptr, 0), __stvrx(v, ptr, 16))

// Load one 32-bit word from ptr into element dstelem of v.
#define __loadunalignedvectorelement(v, ptr, dstelem) { \
    __vector4 port_vmx_element_ = __lvlx(ptr, 0); \
    v = __vrlimi(v, port_vmx_element_, 0x8 >> (dstelem), (4 - (dstelem)) % 4); \
}

#ifdef __cplusplus
}
#endif

#endif
