// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// d3d9gpu.h: Xenos GPU facts used by D3D: memory map, constant-file split, register and fetch-constant layouts.
#ifndef _D3D9GPU_H_
#define _D3D9GPU_H_

#ifdef __cplusplus
extern "C" {
#endif

// Bit-fields below are listed from the least significant bit up (the XDK switches its PPC compiler to that order;
// clang allocates bit-fields LSB first anyway).

// ---- Memory ----------------------------------------------------------------------------------------------------

#define GPU_EDRAM_SIZE                              (10*1024*1024)
#define GPU_MEMORY_SIZE                             (512*1024*1024)
#define GPU_ADDRESS_MASK                            (GPU_MEMORY_SIZE - 1)

// CPU views of physical memory: 64 KB pages at 0xA0000000, 16 MB pages at 0xC0000000, 4 KB pages at 0xE0000000
// (whose GPU offsets start 0x1000 low); the 0xC0000000 window doubles as the cached read-only view.
#define GPU_CPU_VIRTUAL_ADDRESS_64KB_START          ((DWORD) 0xA0000000)
#define GPU_CPU_VIRTUAL_ADDRESS_16MB_START          ((DWORD) 0xC0000000)
#define GPU_CPU_VIRTUAL_ADDRESS_4KB_START           ((DWORD) 0xDFFFF000)
#define GPU_CPU_VIRTUAL_ADDRESS                     ((DWORD) 0xA0000000)
#define GPU_CPU_CACHED_READONLY_VIRTUAL_ADDRESS     ((DWORD) 0xC0000000)

// Physical (GPU) address of a physical-memory CPU pointer: the low 512 MB, plus 4 KB for the 0xE0000000+ range.
static __forceinline DWORD GPU_CONVERT_CPU_TO_GPU_ADDRESS(CONST void* CpuAddress)
{
    UINT_PTR address = (UINT_PTR) CpuAddress;
    return ((DWORD) address & GPU_ADDRESS_MASK) + (DWORD) (((address >> 20) + 0x200) & 0x1000);
}

static __forceinline void* GPU_CONVERT_GPU_TO_CPU_CACHED_READONLY_ADDRESS(DWORD GpuAddress)
{
    return (void*) (UINT_PTR) (GpuAddress + GPU_CPU_CACHED_READONLY_VIRTUAL_ADDRESS);
}

static __forceinline void* GPU_CONVERT_CPU_TO_CPU_CACHED_READONLY_ADDRESS(CONST void* CpuAddress)
{
    return GPU_CONVERT_GPU_TO_CPU_CACHED_READONLY_ADDRESS(GPU_CONVERT_CPU_TO_GPU_ADDRESS(CpuAddress));
}

// ---- Constant files -------------------------------------------------------------------------------------------

#define GPU_ALU_CONSTANTS                           512
#define GPU_BOOLEAN_CONSTANTS                       256
#define GPU_INTEGER_CONSTANTS                       32
#define GPU_FLOW_CONSTANTS                          (GPU_BOOLEAN_CONSTANTS/32 + GPU_INTEGER_CONSTANTS)
#define GPU_FETCH_CONSTANTS                         32
#define GPU_VERTEX_FETCH_CONSTANTS                  (3*GPU_FETCH_CONSTANTS)

// D3D's split: vertex shader constants first, pixel shader constants after them; texture fetch constants 0..25,
// vertex fetch constants (three per fetch slot) in slots 26..31, stream 0 in the last one.
#define GPU_D3D_VERTEX_CONSTANTF_BASE               0
#define GPU_D3D_VERTEX_CONSTANTF_COUNT              256
#define GPU_D3D_VERTEX_CONSTANTI_BASE               0
#define GPU_D3D_VERTEX_CONSTANTI_COUNT              16
#define GPU_D3D_VERTEX_CONSTANTB_BASE               0
#define GPU_D3D_VERTEX_CONSTANTB_COUNT              128
#define GPU_D3D_VERTEX_FETCH_CONSTANT_BASE          26
#define GPU_D3D_VERTEX_FETCH_CONSTANT_COUNT         6
#define GPU_D3D_VERTEX_TEXTURE_FETCH_CONSTANT_BASE  16
#define GPU_D3D_VERTEX_TEXTURE_FETCH_CONSTANT_COUNT 10
#define GPU_D3D_PIXEL_CONSTANTF_BASE                GPU_D3D_VERTEX_CONSTANTF_COUNT
#define GPU_D3D_PIXEL_CONSTANTF_COUNT               256
#define GPU_D3D_PIXEL_CONSTANTI_BASE                GPU_D3D_VERTEX_CONSTANTI_COUNT
#define GPU_D3D_PIXEL_CONSTANTI_COUNT               16
#define GPU_D3D_PIXEL_CONSTANTB_BASE                GPU_D3D_VERTEX_CONSTANTB_COUNT
#define GPU_D3D_PIXEL_CONSTANTB_COUNT               128
#define GPU_D3D_PIXEL_TEXTURE_FETCH_CONSTANT_BASE   0
#define GPU_D3D_PIXEL_TEXTURE_FETCH_CONSTANT_COUNT  26
#define GPU_D3D_TEXTURE_FETCH_CONSTANT_BASE         0
#define GPU_D3D_TEXTURE_FETCH_CONSTANT_COUNT        26

#define GPU_CONVERT_D3D_TO_HARDWARE_TEXTUREFETCHCONSTANT(X) ((X) + GPU_D3D_PIXEL_TEXTURE_FETCH_CONSTANT_BASE)
#define GPU_CONVERT_D3D_TO_HARDWARE_VERTEXFETCHCONSTANT(X)  ((GPU_VERTEX_FETCH_CONSTANTS - 1) - (X))

// ---- Textures, tiles, EDRAM -----------------------------------------------------------------------------------

#define GPU_TEXTURE_ALIGNMENT                       4096
#define GPU_TEXTURE_TEXEL_PITCH_ALIGNMENT           32
#define GPU_LINEAR_TEXTURE_PITCH_BYTE_ALIGNMENT     256
#define GPU_TEXTURE_TILE_DIMENSION                  32
#define GPU_TEXTURE_TILE_DEPTH_DIMENSION            4
#define GPU_RESOLVE_ALIGNMENT                       8

#define GPU_EDRAM_TILE_WIDTH_1X                     80
#define GPU_EDRAM_TILE_HEIGHT_1X                    16
#define GPU_EDRAM_TILE_WIDTH_2X                     80
#define GPU_EDRAM_TILE_HEIGHT_2X                    8
#define GPU_EDRAM_TILE_WIDTH_4X                     40
#define GPU_EDRAM_TILE_HEIGHT_4X                    8
#define GPU_EDRAM_TILE_WIDTH_IN_FRAGMENTS           GPU_EDRAM_TILE_WIDTH_1X
#define GPU_EDRAM_TILE_HEIGHT_IN_FRAGMENTS          GPU_EDRAM_TILE_HEIGHT_1X
#define GPU_EDRAM_TILE_SIZE                         5120
#define GPU_EDRAM_TILES                             (GPU_EDRAM_SIZE / GPU_EDRAM_TILE_SIZE)

#define GPU_HIERARCHICAL_Z_TILE_WIDTH_1X            32
#define GPU_HIERARCHICAL_Z_TILE_HEIGHT_1X           16
#define GPU_HIERARCHICAL_Z_TILE_WIDTH_2X            32
#define GPU_HIERARCHICAL_Z_TILE_HEIGHT_2X           8
#define GPU_HIERARCHICAL_Z_TILE_WIDTH_4X            16
#define GPU_HIERARCHICAL_Z_TILE_HEIGHT_4X           8
#define GPU_HIERARCHICAL_Z_TILE_WIDTH_IN_FRAGMENTS  GPU_HIERARCHICAL_Z_TILE_WIDTH_1X
#define GPU_HIERARCHICAL_Z_TILE_HEIGHT_IN_FRAGMENTS GPU_HIERARCHICAL_Z_TILE_HEIGHT_1X
#define GPU_HIERARCHICAL_Z_TILE_SIZE                512
#define GPU_HIERARCHICAL_Z_TILES                    3600

#define GPU_VERTEXBUFFER_ADDRESS_SHIFT              2
#define GPU_VERTEXBUFFER_SIZE_SHIFT                 2
#define GPU_TEXTURE_ADDRESS_SHIFT                   12

// ---- Field encodings ------------------------------------------------------------------------------------------

typedef enum
{
    GPUENDIAN_NONE = 0, GPUENDIAN_8IN16 = 1, GPUENDIAN_8IN32 = 2, GPUENDIAN_16IN32 = 3,
} GPUENDIAN;

typedef enum
{
    GPUTEXTUREFORMAT_1_REVERSE = 0, GPUTEXTUREFORMAT_1 = 1, GPUTEXTUREFORMAT_8 = 2,
    GPUTEXTUREFORMAT_1_5_5_5 = 3, GPUTEXTUREFORMAT_5_6_5 = 4, GPUTEXTUREFORMAT_6_5_5 = 5,
    GPUTEXTUREFORMAT_8_8_8_8 = 6, GPUTEXTUREFORMAT_2_10_10_10 = 7, GPUTEXTUREFORMAT_8_A = 8,
    GPUTEXTUREFORMAT_8_B = 9, GPUTEXTUREFORMAT_8_8 = 0xA, GPUTEXTUREFORMAT_Cr_Y1_Cb_Y0_REP = 0xB,
    GPUTEXTUREFORMAT_Y1_Cr_Y0_Cb_REP = 0xC, GPUTEXTUREFORMAT_16_16_EDRAM = 0xD,
    GPUTEXTUREFORMAT_8_8_8_8_A = 0xE, GPUTEXTUREFORMAT_4_4_4_4 = 0xF, GPUTEXTUREFORMAT_10_11_11 = 0x10,
    GPUTEXTUREFORMAT_11_11_10 = 0x11, GPUTEXTUREFORMAT_DXT1 = 0x12, GPUTEXTUREFORMAT_DXT2_3 = 0x13,
    GPUTEXTUREFORMAT_DXT4_5 = 0x14, GPUTEXTUREFORMAT_16_16_16_16_EDRAM = 0x15, GPUTEXTUREFORMAT_24_8 = 0x16,
    GPUTEXTUREFORMAT_24_8_FLOAT = 0x17, GPUTEXTUREFORMAT_16 = 0x18, GPUTEXTUREFORMAT_16_16 = 0x19,
    GPUTEXTUREFORMAT_16_16_16_16 = 0x1A, GPUTEXTUREFORMAT_16_EXPAND = 0x1B,
    GPUTEXTUREFORMAT_16_16_EXPAND = 0x1C, GPUTEXTUREFORMAT_16_16_16_16_EXPAND = 0x1D,
    GPUTEXTUREFORMAT_16_FLOAT = 0x1E, GPUTEXTUREFORMAT_16_16_FLOAT = 0x1F,
    GPUTEXTUREFORMAT_16_16_16_16_FLOAT = 0x20, GPUTEXTUREFORMAT_32 = 0x21, GPUTEXTUREFORMAT_32_32 = 0x22,
    GPUTEXTUREFORMAT_32_32_32_32 = 0x23, GPUTEXTUREFORMAT_32_FLOAT = 0x24, GPUTEXTUREFORMAT_32_32_FLOAT = 0x25,
    GPUTEXTUREFORMAT_32_32_32_32_FLOAT = 0x26, GPUTEXTUREFORMAT_32_AS_8 = 0x27,
    GPUTEXTUREFORMAT_32_AS_8_8 = 0x28, GPUTEXTUREFORMAT_16_MPEG = 0x29, GPUTEXTUREFORMAT_16_16_MPEG = 0x2A,
    GPUTEXTUREFORMAT_8_INTERLACED = 0x2B, GPUTEXTUREFORMAT_32_AS_8_INTERLACED = 0x2C,
    GPUTEXTUREFORMAT_32_AS_8_8_INTERLACED = 0x2D, GPUTEXTUREFORMAT_16_INTERLACED = 0x2E,
    GPUTEXTUREFORMAT_16_MPEG_INTERLACED = 0x2F, GPUTEXTUREFORMAT_16_16_MPEG_INTERLACED = 0x30,
    GPUTEXTUREFORMAT_DXN = 0x31, GPUTEXTUREFORMAT_8_8_8_8_AS_16_16_16_16 = 0x32,
    GPUTEXTUREFORMAT_DXT1_AS_16_16_16_16 = 0x33, GPUTEXTUREFORMAT_DXT2_3_AS_16_16_16_16 = 0x34,
    GPUTEXTUREFORMAT_DXT4_5_AS_16_16_16_16 = 0x35, GPUTEXTUREFORMAT_2_10_10_10_AS_16_16_16_16 = 0x36,
    GPUTEXTUREFORMAT_10_11_11_AS_16_16_16_16 = 0x37, GPUTEXTUREFORMAT_11_11_10_AS_16_16_16_16 = 0x38,
    GPUTEXTUREFORMAT_32_32_32_FLOAT = 0x39, GPUTEXTUREFORMAT_DXT3A = 0x3A, GPUTEXTUREFORMAT_DXT5A = 0x3B,
    GPUTEXTUREFORMAT_CTX1 = 0x3C, GPUTEXTUREFORMAT_DXT3A_AS_1_1_1_1 = 0x3D,
    GPUTEXTUREFORMAT_8_8_8_8_GAMMA_EDRAM = 0x3E, GPUTEXTUREFORMAT_2_10_10_10_FLOAT_EDRAM = 0x3F,
} GPUTEXTUREFORMAT;

typedef enum
{
    GPUVERTEXFORMAT_8_8_8_8 = 6, GPUVERTEXFORMAT_2_10_10_10 = 7, GPUVERTEXFORMAT_10_11_11 = 0x10,
    GPUVERTEXFORMAT_11_11_10 = 0x11, GPUVERTEXFORMAT_16_16 = 0x19, GPUVERTEXFORMAT_16_16_16_16 = 0x1A,
    GPUVERTEXFORMAT_16_16_FLOAT = 0x1F, GPUVERTEXFORMAT_16_16_16_16_FLOAT = 0x20, GPUVERTEXFORMAT_32 = 0x21,
    GPUVERTEXFORMAT_32_32 = 0x22, GPUVERTEXFORMAT_32_32_32_32 = 0x23, GPUVERTEXFORMAT_32_FLOAT = 0x24,
    GPUVERTEXFORMAT_32_32_FLOAT = 0x25, GPUVERTEXFORMAT_32_32_32_32_FLOAT = 0x26,
    GPUVERTEXFORMAT_32_32_32_FLOAT = 0x39,
} GPUVERTEXFORMAT;

typedef enum
{
    GPUSIGN_UNSIGNED = 0, GPUSIGN_SIGNED = 1, GPUSIGN_BIAS = 2, GPUSIGN_GAMMA = 3,
} GPUSIGN;

// Sign, packed per channel X..W (two bits each).
#define GPUSIGN_ALL_UNSIGNED                        0x00
#define GPUSIGN_ALL_SIGNED                          0x55

typedef enum
{
    GPUSWIZZLE_X = 0, GPUSWIZZLE_Y = 1, GPUSWIZZLE_Z = 2, GPUSWIZZLE_W = 3, GPUSWIZZLE_0 = 4, GPUSWIZZLE_1 = 5,
    GPUSWIZZLE_KEEP = 7,
} GPUSWIZZLE;

// Channel selectors packed X..W, three bits each (GPUSWIZZLE values).
#define GPUSWIZZLE_ARGB                             0x60A
#define GPUSWIZZLE_ABGR                             0x688

typedef enum
{
    GPUNUMFORMAT_FRACTION = 0, GPUNUMFORMAT_INTEGER = 1,
} GPUNUMFORMAT;

typedef enum
{
    GPUCONSTANTTYPE_INVALID_TEXTURE = 0, GPUCONSTANTTYPE_INVALID_VERTEX = 1, GPUCONSTANTTYPE_TEXTURE = 2,
    GPUCONSTANTTYPE_VERTEX = 3,
} GPUCONSTANTTYPE;

typedef enum
{
    GPUCLAMP_WRAP = 0, GPUCLAMP_MIRROR = 1, GPUCLAMP_CLAMP_TO_LAST = 2, GPUCLAMP_MIRROR_ONCE_TO_LAST = 3,
    GPUCLAMP_CLAMP_HALFWAY = 4, GPUCLAMP_MIRROR_ONCE_HALFWAY = 5, GPUCLAMP_CLAMP_TO_BORDER = 6,
    GPUCLAMP_MIRROR_TO_BORDER = 7,
} GPUCLAMP;

typedef enum
{
    GPUDIMENSION_1D = 0, GPUDIMENSION_2D = 1, GPUDIMENSION_3D = 2, GPUDIMENSION_CUBEMAP = 3,
} GPUDIMENSION;

typedef enum
{
    GPUREQUESTSIZE_256BIT = 0, GPUREQUESTSIZE_512BIT = 1,
} GPUREQUESTSIZE;

typedef enum
{
    GPUCLAMPPOLICY_D3D = 0, GPUCLAMPPOLICY_OGL = 1,
} GPUCLAMPPOLICY;

typedef enum
{
    GPUMINMAGFILTER_POINT = 0, GPUMINMAGFILTER_LINEAR = 1, GPUMINMAGFILTER_KEEP = 3,
} GPUMINMAGFILTER;

typedef enum
{
    GPUMIPFILTER_POINT = 0, GPUMIPFILTER_LINEAR = 1, GPUMIPFILTER_BASEMAP = 2, GPUMIPFILTER_KEEP = 3,
} GPUMIPFILTER;

typedef enum
{
    GPUANISOFILTER_DISABLED = 0, GPUANISOFILTER_MAX1TO1 = 1, GPUANISOFILTER_MAX2TO1 = 2,
    GPUANISOFILTER_MAX4TO1 = 3, GPUANISOFILTER_MAX8TO1 = 4, GPUANISOFILTER_MAX16TO1 = 5,
    GPUANISOFILTER_KEEP = 7,
} GPUANISOFILTER;

typedef enum
{
    GPUBORDERCOLOR_ABGR_BLACK = 0, GPUBORDERCOLOR_ABGR_WHITE = 1, GPUBORDERCOLOR_ACBYCR_BLACK = 2,
    GPUBORDERCOLOR_ACBCRY_BLACK = 3,
} GPUBORDERCOLOR;

typedef enum
{
    GPUTRICLAMP_NORMAL = 0, GPUTRICLAMP_ONE_SIXTH = 1, GPUTRICLAMP_ONE_FOURTH = 2,
    GPUTRICLAMP_THREE_EIGHTHS = 3,
} GPUTRICLAMP;

typedef enum
{
    GPUVERTEXSIGN_UNSIGNED = 0, GPUVERTEXSIGN_SIGNED = 1,
} GPUVERTEXSIGN;

// ---- Context registers ----------------------------------------------------------------------------------------

typedef union
{
    struct {
        DWORD SurfacePitch : 14, : 2, MsaaSamples : 2, HiZPitch : 14;
    };
    DWORD dword;
} GPU_SURFACEINFO;

typedef union
{
    struct {
        DWORD ColorBase : 12, : 4, ColorFormat : 4, ColorExpBias : 6, : 6;
    };
    DWORD dword;
} GPU_COLORINFO;

typedef union
{
    struct {
        DWORD DepthBase : 12, : 4, DepthFormat : 1, DisableHZClamp : 1, : 14;
    };
    DWORD dword;
} GPU_DEPTHINFO;

typedef union
{
    struct {
        int X : 15; DWORD : 1; int Y : 15; DWORD : 1;
    };
    DWORD dword;
} GPU_POINT;

typedef union
{
    struct {
        DWORD Write0 : 4, Write1 : 4, Write2 : 4, Write3 : 4, : 16;
    };
    DWORD dword;
} GPU_COLORMASK;

typedef union
{
    struct {
        DWORD Ref : 8, Mask : 8, WriteMask : 8, : 8;
    };
    DWORD dword;
} GPU_STENCILREFMASK;

typedef union
{
    struct {
        DWORD VsMaxReg : 6, : 2, PsMaxReg : 6, : 2, VsResource : 1, PsResource : 1, ParamGen : 1;
        DWORD GenIndexPix : 1, VsExportCount : 4, VsExportMode : 3, PsExportZ : 1, PsExportColorCount : 3;
        DWORD GenIndexVtx : 1;
    };
    DWORD dword;
} GPU_PROGRAMCONTROL;

typedef union
{
    struct {
        DWORD InstPredOptimize : 1, OutputScreenXY : 1, SampleControl : 2, : 4, ParamGenPos : 8;
        DWORD PerfCounterRef : 1, YieldOptimize : 1, TxCacheSelect : 1, : 13;
    };
    DWORD dword;
} GPU_CONTEXTMISC;

typedef union
{
    struct {
        DWORD ParamShade : 16, SamplingPattern : 16;
    };
    DWORD dword;
} GPU_INTERPOLATORCONTROL;

typedef union
{
    struct {
        DWORD Wrap0 : 4, Wrap1 : 4, Wrap2 : 4, Wrap3 : 4, Wrap4 : 4, Wrap5 : 4, Wrap6 : 4, Wrap7 : 4;
    };
    DWORD dword;
} GPU_WRAPPING1;

typedef union
{
    struct {
        DWORD Wrap8 : 4, Wrap9 : 4, Wrap10 : 4, Wrap11 : 4, Wrap12 : 4, Wrap13 : 4, Wrap14 : 4, Wrap15 : 4;
    };
    DWORD dword;
} GPU_WRAPPING0;

typedef union
{
    struct {
        DWORD StencilEnable : 1, ZEnable : 1, ZWriteEnable : 1, : 1, ZFunc : 3, BackFaceEnable : 1;
        DWORD StencilFunc : 3, StencilFail : 3, StencilZPass : 3, StencilZFail : 3, StencilFuncBF : 3;
        DWORD StencilFailBF : 3, StencilZPassBF : 3, StencilZFailBF : 3;
    };
    DWORD dword;
} GPU_DEPTHCONTROL;

typedef union
{
    struct {
        DWORD ColorSrcBlend : 5, ColorBlendOp : 3, ColorDestBlend : 5, : 3, AlphaSrcBlend : 5;
        DWORD AlphaBlendOp : 3, AlphaDestBlend : 5, : 3;
    };
    DWORD dword;
} GPU_BLENDCONTROL;

typedef union
{
    struct {
        DWORD AlphaFunc : 3, AlphaTestEnable : 1, AlphaToMaskEnable : 1, : 19, AlphaToMaskOffset0 : 2;
        DWORD AlphaToMaskOffset1 : 2, AlphaToMaskOffset2 : 2, AlphaToMaskOffset3 : 2;
    };
    DWORD dword;
} GPU_COLORCONTROL;

typedef union
{
    struct {
        DWORD HiZWriteEnable : 1, HiZEnable : 1, HiStencilWriteEnable : 1, HiStencilEnable : 1, HiZFunc : 1;
        DWORD HiStencilFunc : 1, : 2, HiStencilRef : 8, : 1, HiBaseAddr : 15;
    };
    DWORD dword;
} GPU_HICONTROL;

typedef union
{
    struct {
        DWORD ClipPlaneEnable0 : 1, ClipPlaneEnable1 : 1, ClipPlaneEnable2 : 1, ClipPlaneEnable3 : 1;
        DWORD ClipPlaneEnable4 : 1, ClipPlaneEnable5 : 1, : 8, ClipPlaneMode : 2, ClipDisable : 1;
        DWORD ClipPlaneCullOnlyEnable : 1, BoundaryEdgeFlagEnable : 1, DxClipSpaceDef : 1;
        DWORD DisableClipErrDetect : 1, VtxKillOr : 1, XyNanRetain : 1, ZNanRetain : 1, WNanRetain : 1, : 7;
    };
    DWORD dword;
} GPU_CLIPCONTROL;

typedef union
{
    struct {
        DWORD CullMode : 3, PolyMode : 2, PolyModeFrontPType : 3, PolyModeBackPType : 3;
        DWORD PolyOffsetFrontEnable : 1, PolyOffsetBackEnable : 1, PolyOffsetParaEnable : 1, : 1;
        DWORD MsaaEnable : 1, VtxWindowOffsetEnable : 1, : 2, ProvokingVtxLast : 1, PerspCorrDisable : 1;
        DWORD MultiPrimIbEnable : 1, : 1, QuadOrderEnable : 1, ScOneQuadPerClock : 1, : 7;
    };
    DWORD dword;
} GPU_MODECONTROL;

typedef union
{
    struct {
        DWORD VportXScaleEnable : 1, VportXOffsetEnable : 1, VportYScaleEnable : 1, VportYOffsetEnable : 1;
        DWORD VportZScaleEnable : 1, VportZOffsetEnable : 1, : 2, VtxXyFmt : 1, VtxZFmt : 1, VtxW0Fmt : 1;
        DWORD PerfCounterRef : 1, : 20;
    };
    DWORD dword;
} GPU_VTECONTROL;

typedef union
{
    struct {
        DWORD EdramMode : 3, ColorDepthMacro : 1, : 28;
    };
    DWORD dword;
} GPU_EDRAMMODECONTROL;

typedef union
{
    struct {
        DWORD Height : 16, Width : 16;
    };
    DWORD dword;
} GPU_POINTSIZE;

typedef union
{
    struct {
        DWORD MinSize : 16, MaxSize : 16;
    };
    DWORD dword;
} GPU_POINTMINMAX;

typedef union
{
    struct {
        DWORD Width : 16, : 16;
    };
    DWORD dword;
} GPU_LINECONTROL;

typedef union
{
    struct {
        DWORD PathSelect : 2, : 30;
    };
    DWORD dword;
} GPU_OUTPUTPATHCONTROL;

typedef union
{
    struct {
        DWORD TessMode : 2, : 30;
    };
    DWORD dword;
} GPU_HOSCONTROL;

typedef union
{
    struct {
        DWORD GroupPrimType : 4, : 10, GroupRetainOrder : 1, GroupRetainQuads : 1, GroupPrimOrder : 3, : 13;
    };
    DWORD dword;
} GPU_GROUPPRIMTYPE;

typedef union
{
    struct {
        DWORD CompXEnable : 1, CompYEnable : 1, CompZEnable : 1, CompWEnable : 1, : 4, Stride : 8, Shift : 8;
        DWORD : 8;
    };
    DWORD dword;
} GPU_GROUPVECTCONTROL;

typedef union
{
    struct {
        DWORD XConv : 4, XOffset : 4, YConv : 4, YOffset : 4, ZConv : 4, ZOffset : 4, WConv : 4, WOffset : 4;
    };
    DWORD dword;
} GPU_GROUPVECTFMTCONTROL;

typedef union
{
    struct {
        DWORD MPassPixVecPerPass : 20, : 11, MPassPsEnable : 1;
    };
    DWORD dword;
} GPU_MPASSPSCONTROL;

typedef union
{
    struct {
        DWORD VizQueryEnable : 1, VizQueryId : 6, KillPixPostHiZ : 1, KillPixPostDetailMask : 1, : 23;
    };
    DWORD dword;
} GPU_VIZQUERY;

typedef union
{
    struct {
        DWORD Misc : 16, : 16;
    };
    DWORD dword;
} GPU_ENHANCE;

typedef union
{
    struct {
        DWORD BresControl : 8, UseBresControl : 1, ExpandLineWidth : 1, LastPixel : 1, : 21;
    };
    DWORD dword;
} GPU_SCLINECONTROL;

typedef union
{
    struct {
        DWORD MsaaNumSamples : 3, : 10, MaxSampleDist : 4, : 15;
    };
    DWORD dword;
} GPU_AACONFIG;

typedef union
{
    struct {
        DWORD PixCenter : 1, RoundMode : 2, QuantMode : 3, : 26;
    };
    DWORD dword;
} GPU_VTXCONTROL;

typedef union
{
    struct {
        DWORD Base : 9, : 3, Size : 9, : 11;
    };
    DWORD dword;
} GPU_CONST;

typedef union
{
    struct {
        DWORD DbProgOn : 1, : 3, DbProbBreak : 1, : 3, DbProbAddr : 11, : 5, DbProbCount : 8;
    };
    DWORD dword;
} GPU_DEBUGMISC0;

typedef union
{
    struct {
        DWORD DbOnPix : 1, DbOnVtx : 1, : 6, DbInstCount : 8, DbBreakAddr : 11, : 5;
    };
    DWORD dword;
} GPU_DEBUGMISC1;

typedef union
{
    struct {
        DWORD VtxReuseDepth : 8, : 24;
    };
    DWORD dword;
} GPU_VERTEXREUSEBLOCKCONTROL;

typedef union
{
    struct {
        DWORD DeallocDist : 7, : 25;
    };
    DWORD dword;
} GPU_OUTDEALLOCCONTROL;

typedef union
{
    struct {
        DWORD CopySrcSelect : 3, : 1, CopySampleSelect : 3, : 1, ColorClearEnable : 1, DepthClearEnable : 1;
        DWORD : 10, CopyCommand : 2, : 10;
    };
    DWORD dword;
} GPU_COPYCONTROL;

typedef union
{
    struct {
        DWORD CopyDestPitch : 14, : 2, CopyDestHeight : 14, : 2;
    };
    DWORD dword;
} GPU_COPYDESTPITCH;

typedef union
{
    struct {
        DWORD CopyDestEndian : 3, CopyDestArray : 1, CopyDestSlice : 3, CopyDestFormat : 6, CopyDestNumber : 3;
        DWORD CopyDestExpBias : 6, : 2, CopyDestSwap : 1, : 7;
    };
    DWORD dword;
} GPU_COPYDESTINFO;

typedef union
{
    struct {
        DWORD CopyFuncRed : 3, : 1, CopyFuncGreen : 3, : 1, CopyFuncBlue : 3, : 1, CopyFuncAlpha : 3, : 17;
    };
    DWORD dword;
} GPU_COPYFUNC;

typedef union
{
    struct {
        DWORD CopyRefRed : 8, CopyRefGreen : 8, CopyRefBlue : 8, CopyRefAlpha : 8;
    };
    DWORD dword;
} GPU_COPYREF;

typedef union
{
    struct {
        DWORD CopyMaskRed : 8, CopyMaskGreen : 8, CopyMaskBlue : 8, CopyMaskAlpha : 8;
    };
    DWORD dword;
} GPU_COPYMASK;

typedef union
{
    struct {
        DWORD ResetSampleCount : 1, CopySampleCount : 1, : 30;
    };
    DWORD dword;
} GPU_SAMPLECOUNTCONTROL;

// ---- The register packets D3D shadows (D3DDevice::m_*Packet) ---------------------------------------------------

typedef struct
{
    GPU_SURFACEINFO SurfaceInfo;
    GPU_COLORINFO Color0Info;
    GPU_DEPTHINFO DepthInfo;
    GPU_COLORINFO Color1Info, Color2Info, Color3Info;
    DWORD CoherDestBase0, CoherDestBase1, CoherDestBase2, CoherDestBase3, CoherDestBase4, CoherDestBase5;
    DWORD CoherDestBase6, CoherDestBase7;
    GPU_POINT ScreenScissorTL, ScreenScissorBR;
} GPU_DESTINATIONPACKET;

typedef struct
{
    GPU_POINT WindowOffset, WindowScissorTL, WindowScissorBR;
} GPU_WINDOWPACKET;

typedef struct
{
    DWORD MaxVtxIndx, MinVtxIndx, IndxOffset, MultiPrimIbResetIndx;
    GPU_COLORMASK ColorMask;
    float BlendRed, BlendGreen, BlendBlue, BlendAlpha;
    DWORD Unused[3];
    GPU_STENCILREFMASK StencilRefMaskBF, StencilRefMask;
    float AlphaRef, VportXScale, VportXOffset, VportYScale, VportYOffset, VportZScale, VportZOffset;
} GPU_VALUESPACKET;

typedef struct
{
    GPU_PROGRAMCONTROL ProgramControl;
    GPU_CONTEXTMISC ContextMisc;
    GPU_INTERPOLATORCONTROL InterpolatorControl;
    GPU_WRAPPING0 Wrapping0;
    GPU_WRAPPING1 Wrapping1;
} GPU_PROGRAMPACKET;

typedef struct
{
    GPU_DEPTHCONTROL DepthControl;
    GPU_BLENDCONTROL BlendControl0;
    GPU_COLORCONTROL ColorControl;
    GPU_HICONTROL HiControl;
    GPU_CLIPCONTROL ClipControl;
    GPU_MODECONTROL ModeControl;
    GPU_VTECONTROL VteControl;
    DWORD Unused;
    GPU_EDRAMMODECONTROL EdramModeControl;
    GPU_BLENDCONTROL BlendControl1, BlendControl2, BlendControl3;
} GPU_CONTROLPACKET;

typedef struct
{
    GPU_POINTSIZE PointSize;
    GPU_POINTMINMAX PointMinMax;
    GPU_LINECONTROL LineControl;
    DWORD Unused1;
    GPU_OUTPUTPATHCONTROL OutputPathControl;
    GPU_HOSCONTROL HosControl;
    float HosMaxTessLevel, HosMinTessLevel;
    DWORD HosReuseDepth;
    GPU_GROUPPRIMTYPE GroupPrimType;
    DWORD GroupFirstDecr, GroupDecr;
    GPU_GROUPVECTCONTROL GroupVect0Control, GroupVect1Control;
    GPU_GROUPVECTFMTCONTROL GroupVect0FmtControl, GroupVect1FmtControl;
    DWORD Unused2[2];
    GPU_MPASSPSCONTROL MPassPsControl;
    GPU_VIZQUERY VizQuery;
    GPU_ENHANCE Enhance;
} GPU_TESSELLATORPACKET;

typedef struct
{
    GPU_SCLINECONTROL ScLineControl;
    GPU_AACONFIG AaConfig;
    GPU_VTXCONTROL VtxControl;
    float GbVertClipAdj, GbVertDiscAdj, GbHorzClipAdj, GbHorzDiscAdj;
    GPU_CONST VsConst, PsConst;
    GPU_DEBUGMISC0 DebugMisc0;
    GPU_DEBUGMISC1 DebugMisc1;
    DWORD Unused1[5], Unused2[2], AaMask, Unused3[3];
    GPU_VERTEXREUSEBLOCKCONTROL VertexReuseBlockControl;
    GPU_OUTDEALLOCCONTROL OutDeallocControl;
    GPU_COPYCONTROL CopyControl;
    DWORD CopyDestBase;
    GPU_COPYDESTPITCH CopyDestPitch;
    GPU_COPYDESTINFO CopyDestInfo;
    DWORD HiClear, DepthClear, ColorClear, ColorClearLo;
    GPU_COPYFUNC CopyFunc;
    GPU_COPYREF CopyRef;
    GPU_COPYMASK CopyMask;
    DWORD CopySurfaceSlice;
    GPU_SAMPLECOUNTCONTROL SampleCountControl;
    DWORD SampleCountAddress;
} GPU_MISCPACKET;

typedef struct
{
    float PolyOffsetFrontScale, PolyOffsetFrontOffset, PolyOffsetBackScale, PolyOffsetBackOffset, PointXRad;
    float PointYRad, PointConstantSize, PointCullRad;
} GPU_POINTPACKET;

// ---- Flow and fetch constants ---------------------------------------------------------------------------------

typedef struct
{
    union {
        DWORD FlowConstant[GPU_FLOW_CONSTANTS];
        struct {
            DWORD BooleanConstant[GPU_BOOLEAN_CONSTANTS / 32], IntegerConstant[GPU_INTEGER_CONSTANTS];
        };
    };
} GPUFLOW_CONSTANTS;

typedef struct
{
    DWORD Width : 24, : 8;
} GPUTEXTURESIZE_1D;

typedef struct
{
    DWORD Width : 13, Height : 13, : 6;
} GPUTEXTURESIZE_2D;

typedef struct
{
    DWORD Width : 13, Height : 13, Depth : 6;
} GPUTEXTURESIZE_STACK;

typedef struct
{
    DWORD Width : 11, Height : 11, Depth : 10;
} GPUTEXTURESIZE_3D;

typedef union
{
    struct {
        DWORD Type : 2, SignX : 2, SignY : 2, SignZ : 2, SignW : 2, ClampX : 3, ClampY : 3, ClampZ : 3, : 2;
        DWORD : 1, Pitch : 9, Tiled : 1;
        DWORD DataFormat : 6, Endian : 2, RequestSize : 2, Stacked : 1, ClampPolicy : 1, BaseAddress : 20;
        union {
            GPUTEXTURESIZE_1D OneD;
            GPUTEXTURESIZE_2D TwoD;
            GPUTEXTURESIZE_3D ThreeD;
            GPUTEXTURESIZE_STACK Stack;
        } Size;
        DWORD NumFormat : 1, SwizzleX : 3, SwizzleY : 3, SwizzleZ : 3, SwizzleW : 3; INT ExpAdjust : 6;
        DWORD MagFilter : 2, MinFilter : 2, MipFilter : 2, AnisoFilter : 3, : 3, BorderSize : 1;
        DWORD VolMagFilter : 1, VolMinFilter : 1, MinMipLevel : 4, MaxMipLevel : 4, MagAnisoWalk : 1;
        DWORD MinAnisoWalk : 1; INT LODBias : 10, GradExpAdjustH : 5, GradExpAdjustV : 5;
        DWORD BorderColor : 2, ForceBCWToMax : 1, TriClamp : 2; INT AnisoBias : 4; DWORD Dimension : 2;
        DWORD PackedMips : 1, MipAddress : 20;
    };
    DWORD dword[6];
} GPUTEXTURE_FETCH_CONSTANT;

typedef union
{
    struct {
        DWORD Type : 2, BaseAddress : 30;
        DWORD Endian : 2, Size : 24, AddressClamp : 1, : 1, RequestSize : 2, ClampDisable : 2;
    };
    DWORD dword[2];
} GPUVERTEX_FETCH_CONSTANT;

typedef union
{
    GPUTEXTURE_FETCH_CONSTANT Texture;
    GPUVERTEX_FETCH_CONSTANT Vertex[3];
} GPUFETCH_CONSTANT;

#ifdef __cplusplus
}
#endif

#endif // _D3D9GPU_H_
