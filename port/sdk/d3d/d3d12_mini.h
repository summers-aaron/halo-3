#ifndef PORT_D3D_D3D12_MINI_H
#define PORT_D3D_D3D12_MINI_H

/*
 * The Direct3D 12 and DXGI 1.6 subset the PC port's Direct3D 12 backend (d3d12_backend.cpp) uses, self-contained: no
 * Windows SDK header, no d3d12.lib / dxgi.lib. The backend opens d3d12.dll and dxgi.dll at run time (LoadLibraryA)
 * and asks them for D3D12CreateDevice, D3D12GetDebugInterface and CreateDXGIFactory2, so the executable has no import
 * of either and starts on a machine without Direct3D 12 (the renderer then reports why and the other one is tried).
 *
 * Windows i686 C++ only (the port's i686-pc-windows-msvc build), inside namespace d3d12; the names are the SDK's.
 *
 * COM objects are declared in the C form of the SDK headers: a struct holding a pointer to its vtable, the vtable a
 * struct of __stdcall function pointers in slot order, `This` passed first. Two rules of the Windows i686 ABI make that
 * form the safe one:
 *   - every method is __stdcall (the callee pops its arguments; 64-bit arguments take two stack slots);
 *   - a method returning a structure by value (GetDesc, GetCPUDescriptorHandleForHeapStart, GetAdapterLuid, ...) is,
 *     in MSVC's C++ COM ABI, a member function returning an aggregate: the caller passes a hidden pointer to the
 *     result after `This`, and the method returns that pointer in EAX. A C++ declaration returning the structure
 *     would make the caller expect it in EDX:EAX (the free-function rule for 8-byte structures) and read garbage, so
 *     these slots are declared as the SDK's C headers declare them with WIDL_EXPLICIT_AGGREGATE_RETURNS: an explicit
 *     out pointer, returning it.
 * Every slot is named. Methods the backend does not call keep their name with a placeholder type (d3d12_unused_method),
 * so the slot numbering, which the verification checks name by name against the MinGW-w64 headers, stays exact
 * (tmp/d3d12_phase3_backend/tests: every struct size and field offset, every vtable slot, every enum value and IID).
 */

#if !defined(_WIN32) || !defined(__cplusplus)
#error "d3d12_mini.h: Windows C++ only (the Direct3D 12 backend)"
#endif

#include <stddef.h>
#include <stdint.h>

#define D3D12_MINI_STDCALL __stdcall

namespace d3d12 {

// ---- Windows base types ----

typedef int32_t HRESULT;
typedef int32_t BOOL;
typedef int32_t INT;
typedef uint32_t UINT;
typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint64_t UINT64;
typedef int64_t INT64;
typedef float FLOAT;
typedef uintptr_t SIZE_T;
typedef void *HANDLE;
typedef struct HWND__ *HWND;
typedef uint16_t WCHAR;
typedef const WCHAR *LPCWSTR;

struct GUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t Data4[8];
};
typedef const GUID &REFIID;

struct LUID {
    uint32_t LowPart;
    int32_t HighPart;
};

struct RECT {
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
};

inline bool SUCCEEDED(HRESULT hr) { return hr >= 0; }
inline bool FAILED(HRESULT hr) { return hr < 0; }

static const HRESULT S_OK = 0;
static const HRESULT S_FALSE = 1;
static const HRESULT E_FAIL = (HRESULT)0x80004005u;
static const HRESULT E_INVALIDARG = (HRESULT)0x80070057u;
static const HRESULT E_OUTOFMEMORY = (HRESULT)0x8007000Eu;
static const HRESULT E_NOINTERFACE = (HRESULT)0x80004002u;
static const HRESULT DXGI_STATUS_OCCLUDED = (HRESULT)0x087A0001u;
static const HRESULT DXGI_ERROR_NOT_FOUND = (HRESULT)0x887A0002u;
static const HRESULT DXGI_ERROR_UNSUPPORTED = (HRESULT)0x887A0004u;
static const HRESULT DXGI_ERROR_DEVICE_REMOVED = (HRESULT)0x887A0005u;
static const HRESULT DXGI_ERROR_DEVICE_HUNG = (HRESULT)0x887A0006u;
static const HRESULT DXGI_ERROR_DEVICE_RESET = (HRESULT)0x887A0007u;
static const HRESULT DXGI_ERROR_DRIVER_INTERNAL_ERROR = (HRESULT)0x887A0020u;
static const HRESULT D3D12_ERROR_ADAPTER_NOT_FOUND = (HRESULT)0x887E0001u;
static const HRESULT D3D12_ERROR_DRIVER_VERSION_MISMATCH = (HRESULT)0x887E0002u;

// A vtable slot the backend never calls (its name keeps the slot numbering checkable).
typedef void(D3D12_MINI_STDCALL *d3d12_unused_method)(void);

// ---- interface identifiers ----

static const GUID IID_ID3D12Device = { 0x189819f1, 0x1db6, 0x4b57, { 0xbe, 0x54, 0x18, 0x21, 0x33, 0x9b, 0x85, 0xf7 } };
static const GUID IID_ID3D12Device1 = { 0x77acce80, 0x638e, 0x4e65, { 0x88, 0x95, 0xc1, 0xf2, 0x33, 0x86, 0x86, 0x3e } };
static const GUID IID_ID3D12CommandQueue = { 0x0ec870a6, 0x5d7e, 0x4c22,
                                             { 0x8c, 0xfc, 0x5b, 0xaa, 0xe0, 0x76, 0x16, 0xed } };
static const GUID IID_ID3D12CommandAllocator = { 0x6102dee4, 0xaf59, 0x4b09,
                                                 { 0xb9, 0x99, 0xb4, 0x4d, 0x73, 0xf0, 0x9b, 0x24 } };
static const GUID IID_ID3D12GraphicsCommandList = { 0x5b160d0f, 0xac1b, 0x4185,
                                                    { 0x8b, 0xa8, 0xb3, 0xae, 0x42, 0xa5, 0xa4, 0x55 } };
static const GUID IID_ID3D12Resource = { 0x696442be, 0xa72e, 0x4059, { 0xbc, 0x79, 0x5b, 0x5c, 0x98, 0x04, 0x0f, 0xad } };
static const GUID IID_ID3D12DescriptorHeap = { 0x8efb471d, 0x616c, 0x4f49,
                                               { 0x90, 0xf7, 0x12, 0x7b, 0xb7, 0x63, 0xfa, 0x51 } };
static const GUID IID_ID3D12RootSignature = { 0xc54a6b66, 0x72df, 0x4ee8,
                                              { 0x8b, 0xe5, 0xa9, 0x46, 0xa1, 0x42, 0x92, 0x14 } };
static const GUID IID_ID3D12PipelineState = { 0x765a30f3, 0xf624, 0x4c6f,
                                              { 0xa8, 0x28, 0xac, 0xe9, 0x48, 0x62, 0x24, 0x45 } };
static const GUID IID_ID3D12PipelineLibrary = { 0xc64226a8, 0x9201, 0x46af,
                                                { 0xb4, 0xcc, 0x53, 0xfb, 0x9f, 0xf7, 0x41, 0x4f } };
static const GUID IID_ID3D12Fence = { 0x0a753dcf, 0xc4d8, 0x4b91, { 0xad, 0xf6, 0xbe, 0x5a, 0x60, 0xd9, 0x5a, 0x76 } };
static const GUID IID_ID3D12QueryHeap = { 0x0d9658ae, 0xed45, 0x469e, { 0xa6, 0x1d, 0x97, 0x0e, 0xc5, 0x83, 0xca, 0xb4 } };
static const GUID IID_ID3D12Debug = { 0x344488b7, 0x6846, 0x474b, { 0xb9, 0x89, 0xf0, 0x27, 0x44, 0x82, 0x45, 0xe0 } };
static const GUID IID_ID3D12Debug1 = { 0xaffaa4ca, 0x63fe, 0x4d8e, { 0xb8, 0xad, 0x15, 0x90, 0x00, 0xaf, 0x43, 0x04 } };
static const GUID IID_ID3D12InfoQueue = { 0x0742a90b, 0xc387, 0x483f, { 0xb9, 0x46, 0x30, 0xa7, 0xe4, 0xe6, 0x14, 0x58 } };
static const GUID IID_IDXGIFactory4 = { 0x1bc6ea02, 0xef36, 0x464f, { 0xbf, 0x0c, 0x21, 0xca, 0x39, 0xe5, 0x16, 0x8a } };
static const GUID IID_IDXGIFactory5 = { 0x7632e1f5, 0xee65, 0x4dca, { 0x87, 0xfd, 0x84, 0xcd, 0x75, 0xf8, 0x83, 0x8d } };
static const GUID IID_IDXGIFactory6 = { 0xc1b6694f, 0xff09, 0x44a9, { 0xb0, 0x3c, 0x77, 0x90, 0x0a, 0x0a, 0x1d, 0x17 } };
static const GUID IID_IDXGIAdapter1 = { 0x29038f61, 0x3839, 0x4626, { 0x91, 0xfd, 0x08, 0x68, 0x79, 0x01, 0x1a, 0x05 } };
static const GUID IID_IDXGISwapChain1 = { 0x790a45f7, 0x0d42, 0x4876, { 0x98, 0x3a, 0x0a, 0x55, 0xcf, 0xe6, 0xf4, 0xaa } };
static const GUID IID_IDXGISwapChain3 = { 0x94d99bdb, 0xf1f8, 0x4ab0, { 0xb2, 0x36, 0x7d, 0xa0, 0x17, 0x0e, 0xda, 0xb1 } };

// ---- DXGI formats and types ----

enum DXGI_FORMAT {
    DXGI_FORMAT_UNKNOWN = 0,
    DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
    DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
    DXGI_FORMAT_R32G32B32A32_UINT = 3,
    DXGI_FORMAT_R32G32B32A32_SINT = 4,
    DXGI_FORMAT_R32G32B32_TYPELESS = 5,
    DXGI_FORMAT_R32G32B32_FLOAT = 6,
    DXGI_FORMAT_R32G32B32_UINT = 7,
    DXGI_FORMAT_R32G32B32_SINT = 8,
    DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
    DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
    DXGI_FORMAT_R16G16B16A16_UNORM = 11,
    DXGI_FORMAT_R16G16B16A16_UINT = 12,
    DXGI_FORMAT_R16G16B16A16_SNORM = 13,
    DXGI_FORMAT_R16G16B16A16_SINT = 14,
    DXGI_FORMAT_R32G32_TYPELESS = 15,
    DXGI_FORMAT_R32G32_FLOAT = 16,
    DXGI_FORMAT_R32G32_UINT = 17,
    DXGI_FORMAT_R32G32_SINT = 18,
    DXGI_FORMAT_R32G8X24_TYPELESS = 19,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
    DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
    DXGI_FORMAT_R10G10B10A2_UNORM = 24,
    DXGI_FORMAT_R10G10B10A2_UINT = 25,
    DXGI_FORMAT_R11G11B10_FLOAT = 26,
    DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
    DXGI_FORMAT_R8G8B8A8_UNORM = 28,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
    DXGI_FORMAT_R8G8B8A8_UINT = 30,
    DXGI_FORMAT_R8G8B8A8_SNORM = 31,
    DXGI_FORMAT_R8G8B8A8_SINT = 32,
    DXGI_FORMAT_R16G16_TYPELESS = 33,
    DXGI_FORMAT_R16G16_FLOAT = 34,
    DXGI_FORMAT_R16G16_UNORM = 35,
    DXGI_FORMAT_R16G16_UINT = 36,
    DXGI_FORMAT_R16G16_SNORM = 37,
    DXGI_FORMAT_R16G16_SINT = 38,
    DXGI_FORMAT_R32_TYPELESS = 39,
    DXGI_FORMAT_D32_FLOAT = 40,
    DXGI_FORMAT_R32_FLOAT = 41,
    DXGI_FORMAT_R32_UINT = 42,
    DXGI_FORMAT_R32_SINT = 43,
    DXGI_FORMAT_R24G8_TYPELESS = 44,
    DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
    DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
    DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
    DXGI_FORMAT_R8G8_TYPELESS = 48,
    DXGI_FORMAT_R8G8_UNORM = 49,
    DXGI_FORMAT_R8G8_UINT = 50,
    DXGI_FORMAT_R8G8_SNORM = 51,
    DXGI_FORMAT_R8G8_SINT = 52,
    DXGI_FORMAT_R16_TYPELESS = 53,
    DXGI_FORMAT_R16_FLOAT = 54,
    DXGI_FORMAT_D16_UNORM = 55,
    DXGI_FORMAT_R16_UNORM = 56,
    DXGI_FORMAT_R16_UINT = 57,
    DXGI_FORMAT_R16_SNORM = 58,
    DXGI_FORMAT_R16_SINT = 59,
    DXGI_FORMAT_R8_TYPELESS = 60,
    DXGI_FORMAT_R8_UNORM = 61,
    DXGI_FORMAT_R8_UINT = 62,
    DXGI_FORMAT_R8_SNORM = 63,
    DXGI_FORMAT_R8_SINT = 64,
    DXGI_FORMAT_A8_UNORM = 65,
    DXGI_FORMAT_BC1_TYPELESS = 70,
    DXGI_FORMAT_BC1_UNORM = 71,
    DXGI_FORMAT_BC1_UNORM_SRGB = 72,
    DXGI_FORMAT_BC2_TYPELESS = 73,
    DXGI_FORMAT_BC2_UNORM = 74,
    DXGI_FORMAT_BC2_UNORM_SRGB = 75,
    DXGI_FORMAT_BC3_TYPELESS = 76,
    DXGI_FORMAT_BC3_UNORM = 77,
    DXGI_FORMAT_BC3_UNORM_SRGB = 78,
    DXGI_FORMAT_BC4_TYPELESS = 79,
    DXGI_FORMAT_BC4_UNORM = 80,
    DXGI_FORMAT_BC4_SNORM = 81,
    DXGI_FORMAT_BC5_TYPELESS = 82,
    DXGI_FORMAT_BC5_UNORM = 83,
    DXGI_FORMAT_BC5_SNORM = 84,
    DXGI_FORMAT_B5G6R5_UNORM = 85,
    DXGI_FORMAT_B5G5R5A1_UNORM = 86,
    DXGI_FORMAT_B8G8R8A8_UNORM = 87,
    DXGI_FORMAT_B8G8R8X8_UNORM = 88,
    DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
    DXGI_FORMAT_B4G4R4A4_UNORM = 115,
    DXGI_FORMAT_FORCE_UINT = 0x7fffffff,
};

struct DXGI_SAMPLE_DESC {
    UINT Count;
    UINT Quality;
};

typedef UINT DXGI_USAGE;
static const DXGI_USAGE DXGI_USAGE_RENDER_TARGET_OUTPUT = 0x20u;

enum DXGI_SCALING {
    DXGI_SCALING_STRETCH = 0,
    DXGI_SCALING_NONE = 1,
    DXGI_SCALING_ASPECT_RATIO_STRETCH = 2,
};

enum DXGI_SWAP_EFFECT {
    DXGI_SWAP_EFFECT_DISCARD = 0,
    DXGI_SWAP_EFFECT_SEQUENTIAL = 1,
    DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL = 3,
    DXGI_SWAP_EFFECT_FLIP_DISCARD = 4,
};

enum DXGI_ALPHA_MODE {
    DXGI_ALPHA_MODE_UNSPECIFIED = 0,
    DXGI_ALPHA_MODE_PREMULTIPLIED = 1,
    DXGI_ALPHA_MODE_STRAIGHT = 2,
    DXGI_ALPHA_MODE_IGNORE = 3,
};

enum DXGI_SWAP_CHAIN_FLAG {
    DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH = 2,
    DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT = 64,
    DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING = 2048,
};

enum DXGI_FEATURE {
    DXGI_FEATURE_PRESENT_ALLOW_TEARING = 0,
};

enum DXGI_GPU_PREFERENCE {
    DXGI_GPU_PREFERENCE_UNSPECIFIED = 0,
    DXGI_GPU_PREFERENCE_MINIMUM_POWER = 1,
    DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE = 2,
};

enum DXGI_ADAPTER_FLAG {
    DXGI_ADAPTER_FLAG_NONE = 0,
    DXGI_ADAPTER_FLAG_REMOTE = 1,
    DXGI_ADAPTER_FLAG_SOFTWARE = 2,
};

static const UINT DXGI_PRESENT_ALLOW_TEARING = 0x200u;
static const UINT DXGI_MWA_NO_ALT_ENTER = 0x2u;
static const UINT DXGI_CREATE_FACTORY_DEBUG = 0x1u;

struct DXGI_SWAP_CHAIN_DESC1 {
    UINT Width;
    UINT Height;
    DXGI_FORMAT Format;
    BOOL Stereo;
    DXGI_SAMPLE_DESC SampleDesc;
    DXGI_USAGE BufferUsage;
    UINT BufferCount;
    DXGI_SCALING Scaling;
    DXGI_SWAP_EFFECT SwapEffect;
    DXGI_ALPHA_MODE AlphaMode;
    UINT Flags;
};

struct DXGI_ADAPTER_DESC1 {
    WCHAR Description[128];
    UINT VendorId;
    UINT DeviceId;
    UINT SubSysId;
    UINT Revision;
    SIZE_T DedicatedVideoMemory;
    SIZE_T DedicatedSystemMemory;
    SIZE_T SharedSystemMemory;
    LUID AdapterLuid;
    UINT Flags;
};

// ---- Direct3D 12 enums ----

enum D3D_FEATURE_LEVEL {
    D3D_FEATURE_LEVEL_11_0 = 0xb000,
    D3D_FEATURE_LEVEL_11_1 = 0xb100,
    D3D_FEATURE_LEVEL_12_0 = 0xc000,
    D3D_FEATURE_LEVEL_12_1 = 0xc100,
};

enum D3D_SHADER_MODEL {
    D3D_SHADER_MODEL_5_1 = 0x51,
    D3D_SHADER_MODEL_6_0 = 0x60,
};

enum D3D_ROOT_SIGNATURE_VERSION {
    D3D_ROOT_SIGNATURE_VERSION_1_0 = 0x1,
    D3D_ROOT_SIGNATURE_VERSION_1_1 = 0x2,
};

enum D3D_PRIMITIVE_TOPOLOGY {
    D3D_PRIMITIVE_TOPOLOGY_UNDEFINED = 0,
    D3D_PRIMITIVE_TOPOLOGY_POINTLIST = 1,
    D3D_PRIMITIVE_TOPOLOGY_LINELIST = 2,
    D3D_PRIMITIVE_TOPOLOGY_LINESTRIP = 3,
    D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST = 4,
    D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP = 5,
    D3D_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST = 33,
};

enum D3D12_COMMAND_LIST_TYPE {
    D3D12_COMMAND_LIST_TYPE_DIRECT = 0,
    D3D12_COMMAND_LIST_TYPE_BUNDLE = 1,
    D3D12_COMMAND_LIST_TYPE_COMPUTE = 2,
    D3D12_COMMAND_LIST_TYPE_COPY = 3,
};

enum D3D12_COMMAND_QUEUE_FLAGS {
    D3D12_COMMAND_QUEUE_FLAG_NONE = 0,
    D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT = 1,
};

enum D3D12_COMMAND_QUEUE_PRIORITY {
    D3D12_COMMAND_QUEUE_PRIORITY_NORMAL = 0,
    D3D12_COMMAND_QUEUE_PRIORITY_HIGH = 100,
};

enum D3D12_DESCRIPTOR_HEAP_TYPE {
    D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV = 0,
    D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER = 1,
    D3D12_DESCRIPTOR_HEAP_TYPE_RTV = 2,
    D3D12_DESCRIPTOR_HEAP_TYPE_DSV = 3,
};

enum D3D12_DESCRIPTOR_HEAP_FLAGS {
    D3D12_DESCRIPTOR_HEAP_FLAG_NONE = 0,
    D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE = 1,
};

enum D3D12_HEAP_TYPE {
    D3D12_HEAP_TYPE_DEFAULT = 1,
    D3D12_HEAP_TYPE_UPLOAD = 2,
    D3D12_HEAP_TYPE_READBACK = 3,
    D3D12_HEAP_TYPE_CUSTOM = 4,
};

enum D3D12_CPU_PAGE_PROPERTY {
    D3D12_CPU_PAGE_PROPERTY_UNKNOWN = 0,
};

enum D3D12_MEMORY_POOL {
    D3D12_MEMORY_POOL_UNKNOWN = 0,
};

enum D3D12_HEAP_FLAGS {
    D3D12_HEAP_FLAG_NONE = 0,
};

enum D3D12_RESOURCE_DIMENSION {
    D3D12_RESOURCE_DIMENSION_UNKNOWN = 0,
    D3D12_RESOURCE_DIMENSION_BUFFER = 1,
    D3D12_RESOURCE_DIMENSION_TEXTURE1D = 2,
    D3D12_RESOURCE_DIMENSION_TEXTURE2D = 3,
    D3D12_RESOURCE_DIMENSION_TEXTURE3D = 4,
};

enum D3D12_TEXTURE_LAYOUT {
    D3D12_TEXTURE_LAYOUT_UNKNOWN = 0,
    D3D12_TEXTURE_LAYOUT_ROW_MAJOR = 1,
};

enum D3D12_RESOURCE_FLAGS {
    D3D12_RESOURCE_FLAG_NONE = 0,
    D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET = 0x1,
    D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL = 0x2,
    D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS = 0x4,
    D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE = 0x8,
};

enum D3D12_RESOURCE_STATES {
    D3D12_RESOURCE_STATE_COMMON = 0,
    D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
    D3D12_RESOURCE_STATE_INDEX_BUFFER = 0x2,
    D3D12_RESOURCE_STATE_RENDER_TARGET = 0x4,
    D3D12_RESOURCE_STATE_UNORDERED_ACCESS = 0x8,
    D3D12_RESOURCE_STATE_DEPTH_WRITE = 0x10,
    D3D12_RESOURCE_STATE_DEPTH_READ = 0x20,
    D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE = 0x80,
    D3D12_RESOURCE_STATE_STREAM_OUT = 0x100,
    D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT = 0x200,
    D3D12_RESOURCE_STATE_COPY_DEST = 0x400,
    D3D12_RESOURCE_STATE_COPY_SOURCE = 0x800,
    D3D12_RESOURCE_STATE_RESOLVE_DEST = 0x1000,
    D3D12_RESOURCE_STATE_RESOLVE_SOURCE = 0x2000,
    D3D12_RESOURCE_STATE_GENERIC_READ = 0x1 | 0x2 | 0x40 | 0x80 | 0x200 | 0x800,
    D3D12_RESOURCE_STATE_PRESENT = 0,
};

enum D3D12_RESOURCE_BARRIER_TYPE {
    D3D12_RESOURCE_BARRIER_TYPE_TRANSITION = 0,
    D3D12_RESOURCE_BARRIER_TYPE_ALIASING = 1,
    D3D12_RESOURCE_BARRIER_TYPE_UAV = 2,
};

enum D3D12_RESOURCE_BARRIER_FLAGS {
    D3D12_RESOURCE_BARRIER_FLAG_NONE = 0,
};

static const UINT D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES = 0xffffffffu;

enum D3D12_FENCE_FLAGS {
    D3D12_FENCE_FLAG_NONE = 0,
};

enum D3D12_SRV_DIMENSION {
    D3D12_SRV_DIMENSION_UNKNOWN = 0,
    D3D12_SRV_DIMENSION_BUFFER = 1,
    D3D12_SRV_DIMENSION_TEXTURE2D = 4,
    D3D12_SRV_DIMENSION_TEXTURE2DARRAY = 5,
    D3D12_SRV_DIMENSION_TEXTURE3D = 8,
    D3D12_SRV_DIMENSION_TEXTURECUBE = 9,
};

enum D3D12_UAV_DIMENSION {
    D3D12_UAV_DIMENSION_UNKNOWN = 0,
    D3D12_UAV_DIMENSION_BUFFER = 1,
    D3D12_UAV_DIMENSION_TEXTURE2D = 4,
    D3D12_UAV_DIMENSION_TEXTURE2DARRAY = 5,
    D3D12_UAV_DIMENSION_TEXTURE3D = 8,
};

enum D3D12_RTV_DIMENSION {
    D3D12_RTV_DIMENSION_UNKNOWN = 0,
    D3D12_RTV_DIMENSION_TEXTURE2D = 4,
    D3D12_RTV_DIMENSION_TEXTURE2DARRAY = 5,
    D3D12_RTV_DIMENSION_TEXTURE3D = 8,
};

enum D3D12_DSV_DIMENSION {
    D3D12_DSV_DIMENSION_UNKNOWN = 0,
    D3D12_DSV_DIMENSION_TEXTURE2D = 3,
    D3D12_DSV_DIMENSION_TEXTURE2DARRAY = 4,
};

enum D3D12_DSV_FLAGS {
    D3D12_DSV_FLAG_NONE = 0,
};

enum D3D12_BUFFER_SRV_FLAGS {
    D3D12_BUFFER_SRV_FLAG_NONE = 0,
    D3D12_BUFFER_SRV_FLAG_RAW = 1,
};

enum D3D12_BUFFER_UAV_FLAGS {
    D3D12_BUFFER_UAV_FLAG_NONE = 0,
    D3D12_BUFFER_UAV_FLAG_RAW = 1,
};

// Shader4ComponentMapping: per output component (r, g, b, a), 3 bits: a source component 0-3 or a forced 0 / 1.
enum D3D12_SHADER_COMPONENT_MAPPING {
    D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_0 = 0,
    D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_1 = 1,
    D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_2 = 2,
    D3D12_SHADER_COMPONENT_MAPPING_FROM_MEMORY_COMPONENT_3 = 3,
    D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0 = 4,
    D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1 = 5,
};
// D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING: bit 12 set marks the encoding (the SDK macro's
// D3D12_SHADER_COMPONENT_MAPPING_ALWAYS_SET_BIT_AVOIDING_ZEROMEM_MISTAKES).
inline UINT d3d12_shader_4_component_mapping(UINT r, UINT g, UINT b, UINT a)
{
    return (r & 7u) | (g & 7u) << 3 | (b & 7u) << 6 | (a & 7u) << 9 | 1u << 12;
}
static const UINT D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING = 0x1688u; // (0, 1, 2, 3)

enum D3D12_FILTER {
    D3D12_FILTER_MIN_MAG_MIP_POINT = 0,
    D3D12_FILTER_MIN_MAG_MIP_LINEAR = 0x15,
    D3D12_FILTER_ANISOTROPIC = 0x55,
    D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT = 0x80,
    D3D12_FILTER_COMPARISON_ANISOTROPIC = 0xd5,
};
// D3D12_ENCODE_BASIC_FILTER(min, mag, mip, reduction): 0 point / 1 linear each; reduction 0 standard, 1 comparison.
inline UINT d3d12_basic_filter(UINT min, UINT mag, UINT mip, UINT reduction)
{
    return (min & 3u) << 4 | (mag & 3u) << 2 | (mip & 3u) | (reduction & 3u) << 7;
}

enum D3D12_TEXTURE_ADDRESS_MODE {
    D3D12_TEXTURE_ADDRESS_MODE_WRAP = 1,
    D3D12_TEXTURE_ADDRESS_MODE_MIRROR = 2,
    D3D12_TEXTURE_ADDRESS_MODE_CLAMP = 3,
    D3D12_TEXTURE_ADDRESS_MODE_BORDER = 4,
    D3D12_TEXTURE_ADDRESS_MODE_MIRROR_ONCE = 5,
};

enum D3D12_COMPARISON_FUNC {
    D3D12_COMPARISON_FUNC_NEVER = 1,
    D3D12_COMPARISON_FUNC_LESS = 2,
    D3D12_COMPARISON_FUNC_EQUAL = 3,
    D3D12_COMPARISON_FUNC_LESS_EQUAL = 4,
    D3D12_COMPARISON_FUNC_GREATER = 5,
    D3D12_COMPARISON_FUNC_NOT_EQUAL = 6,
    D3D12_COMPARISON_FUNC_GREATER_EQUAL = 7,
    D3D12_COMPARISON_FUNC_ALWAYS = 8,
};

enum D3D12_BLEND {
    D3D12_BLEND_ZERO = 1,
    D3D12_BLEND_ONE = 2,
    D3D12_BLEND_SRC_COLOR = 3,
    D3D12_BLEND_INV_SRC_COLOR = 4,
    D3D12_BLEND_SRC_ALPHA = 5,
    D3D12_BLEND_INV_SRC_ALPHA = 6,
    D3D12_BLEND_DEST_ALPHA = 7,
    D3D12_BLEND_INV_DEST_ALPHA = 8,
    D3D12_BLEND_DEST_COLOR = 9,
    D3D12_BLEND_INV_DEST_COLOR = 10,
    D3D12_BLEND_SRC_ALPHA_SAT = 11,
    D3D12_BLEND_BLEND_FACTOR = 14,
    D3D12_BLEND_INV_BLEND_FACTOR = 15,
    D3D12_BLEND_ALPHA_FACTOR = 20,     // D3D12_FEATURE_DATA_D3D12_OPTIONS13::AlphaBlendFactorSupported
    D3D12_BLEND_INV_ALPHA_FACTOR = 21,
};

enum D3D12_BLEND_OP {
    D3D12_BLEND_OP_ADD = 1,
    D3D12_BLEND_OP_SUBTRACT = 2,
    D3D12_BLEND_OP_REV_SUBTRACT = 3,
    D3D12_BLEND_OP_MIN = 4,
    D3D12_BLEND_OP_MAX = 5,
};

enum D3D12_LOGIC_OP {
    D3D12_LOGIC_OP_CLEAR = 0,
    D3D12_LOGIC_OP_NOOP = 4,
};

enum D3D12_COLOR_WRITE_ENABLE {
    D3D12_COLOR_WRITE_ENABLE_RED = 1,
    D3D12_COLOR_WRITE_ENABLE_GREEN = 2,
    D3D12_COLOR_WRITE_ENABLE_BLUE = 4,
    D3D12_COLOR_WRITE_ENABLE_ALPHA = 8,
    D3D12_COLOR_WRITE_ENABLE_ALL = 15,
};

enum D3D12_FILL_MODE {
    D3D12_FILL_MODE_WIREFRAME = 2,
    D3D12_FILL_MODE_SOLID = 3,
};

enum D3D12_CULL_MODE {
    D3D12_CULL_MODE_NONE = 1,
    D3D12_CULL_MODE_FRONT = 2,
    D3D12_CULL_MODE_BACK = 3,
};

enum D3D12_CONSERVATIVE_RASTERIZATION_MODE {
    D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF = 0,
};

enum D3D12_DEPTH_WRITE_MASK {
    D3D12_DEPTH_WRITE_MASK_ZERO = 0,
    D3D12_DEPTH_WRITE_MASK_ALL = 1,
};

enum D3D12_STENCIL_OP {
    D3D12_STENCIL_OP_KEEP = 1,
    D3D12_STENCIL_OP_ZERO = 2,
    D3D12_STENCIL_OP_REPLACE = 3,
    D3D12_STENCIL_OP_INCR_SAT = 4,
    D3D12_STENCIL_OP_DECR_SAT = 5,
    D3D12_STENCIL_OP_INVERT = 6,
    D3D12_STENCIL_OP_INCR = 7,
    D3D12_STENCIL_OP_DECR = 8,
};

enum D3D12_INDEX_BUFFER_STRIP_CUT_VALUE {
    D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED = 0,
    D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF = 1,
    D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF = 2,
};

enum D3D12_PRIMITIVE_TOPOLOGY_TYPE {
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED = 0,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT = 1,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE = 2,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE = 3,
    D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH = 4,
};

enum D3D12_PIPELINE_STATE_FLAGS {
    D3D12_PIPELINE_STATE_FLAG_NONE = 0,
};

enum D3D12_CLEAR_FLAGS {
    D3D12_CLEAR_FLAG_DEPTH = 0x1,
    D3D12_CLEAR_FLAG_STENCIL = 0x2,
};

enum D3D12_TEXTURE_COPY_TYPE {
    D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX = 0,
    D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT = 1,
};

enum D3D12_QUERY_HEAP_TYPE {
    D3D12_QUERY_HEAP_TYPE_OCCLUSION = 0,
    D3D12_QUERY_HEAP_TYPE_TIMESTAMP = 1,
};

enum D3D12_QUERY_TYPE {
    D3D12_QUERY_TYPE_OCCLUSION = 0,
    D3D12_QUERY_TYPE_BINARY_OCCLUSION = 1,
    D3D12_QUERY_TYPE_TIMESTAMP = 2,
};

enum D3D12_FEATURE {
    D3D12_FEATURE_D3D12_OPTIONS = 0,
    D3D12_FEATURE_ARCHITECTURE = 1,
    D3D12_FEATURE_FEATURE_LEVELS = 2,
    D3D12_FEATURE_FORMAT_SUPPORT = 3,
    D3D12_FEATURE_SHADER_MODEL = 7,
    D3D12_FEATURE_ROOT_SIGNATURE = 12,
    D3D12_FEATURE_D3D12_OPTIONS8 = 36,
    D3D12_FEATURE_D3D12_OPTIONS13 = 42,
};

enum D3D12_SHADER_MIN_PRECISION_SUPPORT {
    D3D12_SHADER_MIN_PRECISION_SUPPORT_NONE = 0,
};

enum D3D12_TILED_RESOURCES_TIER {
    D3D12_TILED_RESOURCES_TIER_NOT_SUPPORTED = 0,
};

enum D3D12_RESOURCE_BINDING_TIER {
    D3D12_RESOURCE_BINDING_TIER_1 = 1,
    D3D12_RESOURCE_BINDING_TIER_2 = 2,
    D3D12_RESOURCE_BINDING_TIER_3 = 3,
};

enum D3D12_CONSERVATIVE_RASTERIZATION_TIER {
    D3D12_CONSERVATIVE_RASTERIZATION_TIER_NOT_SUPPORTED = 0,
};

enum D3D12_CROSS_NODE_SHARING_TIER {
    D3D12_CROSS_NODE_SHARING_TIER_NOT_SUPPORTED = 0,
};

enum D3D12_RESOURCE_HEAP_TIER {
    D3D12_RESOURCE_HEAP_TIER_1 = 1,
    D3D12_RESOURCE_HEAP_TIER_2 = 2,
};

enum D3D12_FORMAT_SUPPORT1 {
    D3D12_FORMAT_SUPPORT1_NONE = 0,
    D3D12_FORMAT_SUPPORT1_TEXTURE2D = 0x20,
    D3D12_FORMAT_SUPPORT1_TEXTURE3D = 0x40,
    D3D12_FORMAT_SUPPORT1_SHADER_LOAD = 0x100,
    D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE = 0x200,
    D3D12_FORMAT_SUPPORT1_RENDER_TARGET = 0x4000,
    D3D12_FORMAT_SUPPORT1_BLENDABLE = 0x8000,
    D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL = 0x10000,
    D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW = 0x2000000,
};

enum D3D12_FORMAT_SUPPORT2 {
    D3D12_FORMAT_SUPPORT2_NONE = 0,
    D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE = 0x80,
};

enum D3D12_MESSAGE_CATEGORY {
    D3D12_MESSAGE_CATEGORY_APPLICATION_DEFINED = 0,
};

enum D3D12_MESSAGE_SEVERITY {
    D3D12_MESSAGE_SEVERITY_CORRUPTION = 0,
    D3D12_MESSAGE_SEVERITY_ERROR = 1,
    D3D12_MESSAGE_SEVERITY_WARNING = 2,
    D3D12_MESSAGE_SEVERITY_INFO = 3,
    D3D12_MESSAGE_SEVERITY_MESSAGE = 4,
};

enum D3D12_MESSAGE_ID {
    D3D12_MESSAGE_ID_UNKNOWN = 0,
};

// D3D12_TEXTURE_DATA_PITCH_ALIGNMENT / _PLACEMENT_ALIGNMENT, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT
static const UINT D3D12_TEXTURE_DATA_PITCH_ALIGNMENT = 256u;
static const UINT D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT = 512u;
static const UINT D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT = 256u;
static const UINT D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE = 2048u;
static const UINT D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2 = 1000000u;
static const UINT D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT = 8u;
static const UINT D3D12_REQ_MAXANISOTROPY = 16u;

// ---- Direct3D 12 structures ----

struct D3D12_CPU_DESCRIPTOR_HANDLE {
    SIZE_T ptr;
};

struct D3D12_GPU_DESCRIPTOR_HANDLE {
    UINT64 ptr;
};

typedef UINT64 D3D12_GPU_VIRTUAL_ADDRESS;

struct D3D12_COMMAND_QUEUE_DESC {
    D3D12_COMMAND_LIST_TYPE Type;
    INT Priority;
    D3D12_COMMAND_QUEUE_FLAGS Flags;
    UINT NodeMask;
};

struct D3D12_DESCRIPTOR_HEAP_DESC {
    D3D12_DESCRIPTOR_HEAP_TYPE Type;
    UINT NumDescriptors;
    D3D12_DESCRIPTOR_HEAP_FLAGS Flags;
    UINT NodeMask;
};

struct D3D12_HEAP_PROPERTIES {
    D3D12_HEAP_TYPE Type;
    D3D12_CPU_PAGE_PROPERTY CPUPageProperty;
    D3D12_MEMORY_POOL MemoryPoolPreference;
    UINT CreationNodeMask;
    UINT VisibleNodeMask;
};

struct D3D12_RESOURCE_DESC {
    D3D12_RESOURCE_DIMENSION Dimension;
    UINT64 Alignment;
    UINT64 Width;
    UINT Height;
    UINT16 DepthOrArraySize;
    UINT16 MipLevels;
    DXGI_FORMAT Format;
    DXGI_SAMPLE_DESC SampleDesc;
    D3D12_TEXTURE_LAYOUT Layout;
    D3D12_RESOURCE_FLAGS Flags;
};

struct D3D12_DEPTH_STENCIL_VALUE {
    FLOAT Depth;
    UINT8 Stencil;
};

struct D3D12_CLEAR_VALUE {
    DXGI_FORMAT Format;
    union {
        FLOAT Color[4];
        D3D12_DEPTH_STENCIL_VALUE DepthStencil;
    };
};

struct D3D12_RANGE {
    SIZE_T Begin;
    SIZE_T End;
};

struct D3D12_BOX {
    UINT left;
    UINT top;
    UINT front;
    UINT right;
    UINT bottom;
    UINT back;
};

struct D3D12_VIEWPORT {
    FLOAT TopLeftX;
    FLOAT TopLeftY;
    FLOAT Width;
    FLOAT Height;
    FLOAT MinDepth;
    FLOAT MaxDepth;
};

typedef RECT D3D12_RECT;

struct ID3D12Resource;
struct ID3D12RootSignature;
struct ID3D12CommandList;

struct D3D12_RESOURCE_TRANSITION_BARRIER {
    ID3D12Resource *pResource;
    UINT Subresource;
    D3D12_RESOURCE_STATES StateBefore;
    D3D12_RESOURCE_STATES StateAfter;
};

struct D3D12_RESOURCE_ALIASING_BARRIER {
    ID3D12Resource *pResourceBefore;
    ID3D12Resource *pResourceAfter;
};

struct D3D12_RESOURCE_UAV_BARRIER {
    ID3D12Resource *pResource;
};

struct D3D12_RESOURCE_BARRIER {
    D3D12_RESOURCE_BARRIER_TYPE Type;
    D3D12_RESOURCE_BARRIER_FLAGS Flags;
    union {
        D3D12_RESOURCE_TRANSITION_BARRIER Transition;
        D3D12_RESOURCE_ALIASING_BARRIER Aliasing;
        D3D12_RESOURCE_UAV_BARRIER UAV;
    };
};

struct D3D12_BUFFER_SRV {
    UINT64 FirstElement;
    UINT NumElements;
    UINT StructureByteStride;
    D3D12_BUFFER_SRV_FLAGS Flags;
};

struct D3D12_TEX2D_SRV {
    UINT MostDetailedMip;
    UINT MipLevels;
    UINT PlaneSlice;
    FLOAT ResourceMinLODClamp;
};

struct D3D12_TEX2D_ARRAY_SRV {
    UINT MostDetailedMip;
    UINT MipLevels;
    UINT FirstArraySlice;
    UINT ArraySize;
    UINT PlaneSlice;
    FLOAT ResourceMinLODClamp;
};

struct D3D12_TEX3D_SRV {
    UINT MostDetailedMip;
    UINT MipLevels;
    FLOAT ResourceMinLODClamp;
};

struct D3D12_TEXCUBE_SRV {
    UINT MostDetailedMip;
    UINT MipLevels;
    FLOAT ResourceMinLODClamp;
};

struct D3D12_SHADER_RESOURCE_VIEW_DESC {
    DXGI_FORMAT Format;
    D3D12_SRV_DIMENSION ViewDimension;
    UINT Shader4ComponentMapping;
    union {
        D3D12_BUFFER_SRV Buffer;
        D3D12_TEX2D_SRV Texture2D;
        D3D12_TEX2D_ARRAY_SRV Texture2DArray;
        D3D12_TEX3D_SRV Texture3D;
        D3D12_TEXCUBE_SRV TextureCube;
    };
};

struct D3D12_BUFFER_UAV {
    UINT64 FirstElement;
    UINT NumElements;
    UINT StructureByteStride;
    UINT64 CounterOffsetInBytes;
    D3D12_BUFFER_UAV_FLAGS Flags;
};

struct D3D12_TEX2D_UAV {
    UINT MipSlice;
    UINT PlaneSlice;
};

struct D3D12_TEX2D_ARRAY_UAV {
    UINT MipSlice;
    UINT FirstArraySlice;
    UINT ArraySize;
    UINT PlaneSlice;
};

struct D3D12_TEX3D_UAV {
    UINT MipSlice;
    UINT FirstWSlice;
    UINT WSize;
};

struct D3D12_UNORDERED_ACCESS_VIEW_DESC {
    DXGI_FORMAT Format;
    D3D12_UAV_DIMENSION ViewDimension;
    union {
        D3D12_BUFFER_UAV Buffer;
        D3D12_TEX2D_UAV Texture2D;
        D3D12_TEX2D_ARRAY_UAV Texture2DArray;
        D3D12_TEX3D_UAV Texture3D;
    };
};

struct D3D12_TEX2D_RTV {
    UINT MipSlice;
    UINT PlaneSlice;
};

struct D3D12_TEX2D_ARRAY_RTV {
    UINT MipSlice;
    UINT FirstArraySlice;
    UINT ArraySize;
    UINT PlaneSlice;
};

struct D3D12_TEX3D_RTV {
    UINT MipSlice;
    UINT FirstWSlice;
    UINT WSize;
};

// The union is as large as its largest member in the SDK (D3D12_BUFFER_RTV, 16 bytes with 8-byte alignment).
struct D3D12_BUFFER_RTV {
    UINT64 FirstElement;
    UINT NumElements;
};

struct D3D12_RENDER_TARGET_VIEW_DESC {
    DXGI_FORMAT Format;
    D3D12_RTV_DIMENSION ViewDimension;
    union {
        D3D12_BUFFER_RTV Buffer;
        D3D12_TEX2D_RTV Texture2D;
        D3D12_TEX2D_ARRAY_RTV Texture2DArray;
        D3D12_TEX3D_RTV Texture3D;
    };
};

struct D3D12_TEX2D_DSV {
    UINT MipSlice;
};

struct D3D12_TEX2D_ARRAY_DSV {
    UINT MipSlice;
    UINT FirstArraySlice;
    UINT ArraySize;
};

struct D3D12_DEPTH_STENCIL_VIEW_DESC {
    DXGI_FORMAT Format;
    D3D12_DSV_DIMENSION ViewDimension;
    D3D12_DSV_FLAGS Flags;
    union {
        D3D12_TEX2D_DSV Texture2D;
        D3D12_TEX2D_ARRAY_DSV Texture2DArray;
    };
};

struct D3D12_SAMPLER_DESC {
    D3D12_FILTER Filter;
    D3D12_TEXTURE_ADDRESS_MODE AddressU;
    D3D12_TEXTURE_ADDRESS_MODE AddressV;
    D3D12_TEXTURE_ADDRESS_MODE AddressW;
    FLOAT MipLODBias;
    UINT MaxAnisotropy;
    D3D12_COMPARISON_FUNC ComparisonFunc;
    FLOAT BorderColor[4];
    FLOAT MinLOD;
    FLOAT MaxLOD;
};

struct D3D12_SUBRESOURCE_FOOTPRINT {
    DXGI_FORMAT Format;
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT RowPitch;
};

struct D3D12_PLACED_SUBRESOURCE_FOOTPRINT {
    UINT64 Offset;
    D3D12_SUBRESOURCE_FOOTPRINT Footprint;
};

struct D3D12_TEXTURE_COPY_LOCATION {
    ID3D12Resource *pResource;
    D3D12_TEXTURE_COPY_TYPE Type;
    union {
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT PlacedFootprint;
        UINT SubresourceIndex;
    };
};

struct D3D12_INDEX_BUFFER_VIEW {
    D3D12_GPU_VIRTUAL_ADDRESS BufferLocation;
    UINT SizeInBytes;
    DXGI_FORMAT Format;
};

struct D3D12_VERTEX_BUFFER_VIEW {
    D3D12_GPU_VIRTUAL_ADDRESS BufferLocation;
    UINT SizeInBytes;
    UINT StrideInBytes;
};

struct D3D12_SHADER_BYTECODE {
    const void *pShaderBytecode;
    SIZE_T BytecodeLength;
};

struct D3D12_SO_DECLARATION_ENTRY;
struct D3D12_STREAM_OUTPUT_DESC {
    const D3D12_SO_DECLARATION_ENTRY *pSODeclaration;
    UINT NumEntries;
    const UINT *pBufferStrides;
    UINT NumStrides;
    UINT RasterizedStream;
};

struct D3D12_RENDER_TARGET_BLEND_DESC {
    BOOL BlendEnable;
    BOOL LogicOpEnable;
    D3D12_BLEND SrcBlend;
    D3D12_BLEND DestBlend;
    D3D12_BLEND_OP BlendOp;
    D3D12_BLEND SrcBlendAlpha;
    D3D12_BLEND DestBlendAlpha;
    D3D12_BLEND_OP BlendOpAlpha;
    D3D12_LOGIC_OP LogicOp;
    UINT8 RenderTargetWriteMask;
};

struct D3D12_BLEND_DESC {
    BOOL AlphaToCoverageEnable;
    BOOL IndependentBlendEnable;
    D3D12_RENDER_TARGET_BLEND_DESC RenderTarget[8];
};

struct D3D12_RASTERIZER_DESC {
    D3D12_FILL_MODE FillMode;
    D3D12_CULL_MODE CullMode;
    BOOL FrontCounterClockwise;
    INT DepthBias;
    FLOAT DepthBiasClamp;
    FLOAT SlopeScaledDepthBias;
    BOOL DepthClipEnable;
    BOOL MultisampleEnable;
    BOOL AntialiasedLineEnable;
    UINT ForcedSampleCount;
    D3D12_CONSERVATIVE_RASTERIZATION_MODE ConservativeRaster;
};

struct D3D12_DEPTH_STENCILOP_DESC {
    D3D12_STENCIL_OP StencilFailOp;
    D3D12_STENCIL_OP StencilDepthFailOp;
    D3D12_STENCIL_OP StencilPassOp;
    D3D12_COMPARISON_FUNC StencilFunc;
};

struct D3D12_DEPTH_STENCIL_DESC {
    BOOL DepthEnable;
    D3D12_DEPTH_WRITE_MASK DepthWriteMask;
    D3D12_COMPARISON_FUNC DepthFunc;
    BOOL StencilEnable;
    UINT8 StencilReadMask;
    UINT8 StencilWriteMask;
    D3D12_DEPTH_STENCILOP_DESC FrontFace;
    D3D12_DEPTH_STENCILOP_DESC BackFace;
};

struct D3D12_INPUT_ELEMENT_DESC;
struct D3D12_INPUT_LAYOUT_DESC {
    const D3D12_INPUT_ELEMENT_DESC *pInputElementDescs;
    UINT NumElements;
};

struct D3D12_CACHED_PIPELINE_STATE {
    const void *pCachedBlob;
    SIZE_T CachedBlobSizeInBytes;
};

struct D3D12_GRAPHICS_PIPELINE_STATE_DESC {
    ID3D12RootSignature *pRootSignature;
    D3D12_SHADER_BYTECODE VS;
    D3D12_SHADER_BYTECODE PS;
    D3D12_SHADER_BYTECODE DS;
    D3D12_SHADER_BYTECODE HS;
    D3D12_SHADER_BYTECODE GS;
    D3D12_STREAM_OUTPUT_DESC StreamOutput;
    D3D12_BLEND_DESC BlendState;
    UINT SampleMask;
    D3D12_RASTERIZER_DESC RasterizerState;
    D3D12_DEPTH_STENCIL_DESC DepthStencilState;
    D3D12_INPUT_LAYOUT_DESC InputLayout;
    D3D12_INDEX_BUFFER_STRIP_CUT_VALUE IBStripCutValue;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE PrimitiveTopologyType;
    UINT NumRenderTargets;
    DXGI_FORMAT RTVFormats[8];
    DXGI_FORMAT DSVFormat;
    DXGI_SAMPLE_DESC SampleDesc;
    UINT NodeMask;
    D3D12_CACHED_PIPELINE_STATE CachedPSO;
    D3D12_PIPELINE_STATE_FLAGS Flags;
};

struct D3D12_COMPUTE_PIPELINE_STATE_DESC {
    ID3D12RootSignature *pRootSignature;
    D3D12_SHADER_BYTECODE CS;
    UINT NodeMask;
    D3D12_CACHED_PIPELINE_STATE CachedPSO;
    D3D12_PIPELINE_STATE_FLAGS Flags;
};

struct D3D12_QUERY_HEAP_DESC {
    D3D12_QUERY_HEAP_TYPE Type;
    UINT Count;
    UINT NodeMask;
};

struct D3D12_FEATURE_DATA_D3D12_OPTIONS {
    BOOL DoublePrecisionFloatShaderOps;
    BOOL OutputMergerLogicOp;
    D3D12_SHADER_MIN_PRECISION_SUPPORT MinPrecisionSupport;
    D3D12_TILED_RESOURCES_TIER TiledResourcesTier;
    D3D12_RESOURCE_BINDING_TIER ResourceBindingTier;
    BOOL PSSpecifiedStencilRefSupported;
    BOOL TypedUAVLoadAdditionalFormats;
    BOOL ROVsSupported;
    D3D12_CONSERVATIVE_RASTERIZATION_TIER ConservativeRasterizationTier;
    UINT MaxGPUVirtualAddressBitsPerResource;
    BOOL StandardSwizzle64KBSupported;
    D3D12_CROSS_NODE_SHARING_TIER CrossNodeSharingTier;
    BOOL CrossAdapterRowMajorTextureSupported;
    BOOL VPAndRTArrayIndexFromAnyShaderFeedingRasterizerSupportedWithoutGSEmulation;
    D3D12_RESOURCE_HEAP_TIER ResourceHeapTier;
};

struct D3D12_FEATURE_DATA_ARCHITECTURE {
    UINT NodeIndex;
    BOOL TileBasedRenderer;
    BOOL UMA;
    BOOL CacheCoherentUMA;
};

struct D3D12_FEATURE_DATA_FEATURE_LEVELS {
    UINT NumFeatureLevels;
    const D3D_FEATURE_LEVEL *pFeatureLevelsRequested;
    D3D_FEATURE_LEVEL MaxSupportedFeatureLevel;
};

struct D3D12_FEATURE_DATA_SHADER_MODEL {
    D3D_SHADER_MODEL HighestShaderModel;
};

struct D3D12_FEATURE_DATA_ROOT_SIGNATURE {
    D3D_ROOT_SIGNATURE_VERSION HighestVersion;
};

struct D3D12_FEATURE_DATA_FORMAT_SUPPORT {
    DXGI_FORMAT Format;
    D3D12_FORMAT_SUPPORT1 Support1;
    D3D12_FORMAT_SUPPORT2 Support2;
};

struct D3D12_FEATURE_DATA_D3D12_OPTIONS8 {
    BOOL UnalignedBlockTexturesSupported;
};

struct D3D12_FEATURE_DATA_D3D12_OPTIONS13 {
    BOOL UnrestrictedBufferTextureCopyPitchSupported;
    BOOL UnrestrictedVertexElementAlignmentSupported;
    BOOL InvertedViewportHeightFlipsYSupported;
    BOOL InvertedViewportDepthFlipsZSupported;
    BOOL TextureCopyBetweenDimensionsSupported;
    BOOL AlphaBlendFactorSupported;
};

struct D3D12_MESSAGE {
    D3D12_MESSAGE_CATEGORY Category;
    D3D12_MESSAGE_SEVERITY Severity;
    D3D12_MESSAGE_ID ID;
    const char *pDescription;
    SIZE_T DescriptionByteLength;
};

struct D3D12_INFO_QUEUE_FILTER_DESC {
    UINT NumCategories;
    D3D12_MESSAGE_CATEGORY *pCategoryList;
    UINT NumSeverities;
    D3D12_MESSAGE_SEVERITY *pSeverityList;
    UINT NumIDs;
    D3D12_MESSAGE_ID *pIDList;
};

struct D3D12_INFO_QUEUE_FILTER {
    D3D12_INFO_QUEUE_FILTER_DESC AllowList;
    D3D12_INFO_QUEUE_FILTER_DESC DenyList;
};

// ---- COM interfaces (C form: see the top) ----

#define D3D12_MINI_INTERFACE(name)                                                                                     \
    struct name##Vtbl;                                                                                                 \
    struct name {                                                                                                      \
        const struct name##Vtbl *lpVtbl;                                                                               \
    };

D3D12_MINI_INTERFACE(IUnknown)
D3D12_MINI_INTERFACE(ID3D12Device)
D3D12_MINI_INTERFACE(ID3D12Device1)
D3D12_MINI_INTERFACE(ID3D12CommandQueue)
D3D12_MINI_INTERFACE(ID3D12CommandAllocator)
D3D12_MINI_INTERFACE(ID3D12CommandList)
D3D12_MINI_INTERFACE(ID3D12GraphicsCommandList)
D3D12_MINI_INTERFACE(ID3D12Resource)
D3D12_MINI_INTERFACE(ID3D12DescriptorHeap)
D3D12_MINI_INTERFACE(ID3D12RootSignature)
D3D12_MINI_INTERFACE(ID3D12PipelineState)
D3D12_MINI_INTERFACE(ID3D12PipelineLibrary)
D3D12_MINI_INTERFACE(ID3D12Fence)
D3D12_MINI_INTERFACE(ID3D12QueryHeap)
D3D12_MINI_INTERFACE(ID3D12Debug)
D3D12_MINI_INTERFACE(ID3D12Debug1)
D3D12_MINI_INTERFACE(ID3D12InfoQueue)
D3D12_MINI_INTERFACE(IDXGIFactory6)
D3D12_MINI_INTERFACE(IDXGIAdapter1)
D3D12_MINI_INTERFACE(IDXGISwapChain3)

#define D3D12_MINI_IUNKNOWN(self)                                                                                      \
    HRESULT(D3D12_MINI_STDCALL *QueryInterface)(self * This, REFIID riid, void **object);                              \
    UINT(D3D12_MINI_STDCALL *AddRef)(self * This);                                                                     \
    UINT(D3D12_MINI_STDCALL *Release)(self * This);
// ID3D12Object
#define D3D12_MINI_OBJECT(self)                                                                                        \
    D3D12_MINI_IUNKNOWN(self)                                                                                          \
    d3d12_unused_method GetPrivateData;                                                                                \
    d3d12_unused_method SetPrivateData;                                                                                \
    d3d12_unused_method SetPrivateDataInterface;                                                                       \
    HRESULT(D3D12_MINI_STDCALL *SetName)(self * This, LPCWSTR name);
// ID3D12DeviceChild (ID3D12Pageable adds nothing)
#define D3D12_MINI_DEVICE_CHILD(self)                                                                                  \
    D3D12_MINI_OBJECT(self)                                                                                            \
    d3d12_unused_method GetDevice;

struct IUnknownVtbl {
    D3D12_MINI_IUNKNOWN(IUnknown)
};

#define D3D12_MINI_DEVICE_METHODS(self)                                                                                \
    D3D12_MINI_OBJECT(self)                                                                                            \
    UINT(D3D12_MINI_STDCALL *GetNodeCount)(self * This);                                                               \
    HRESULT(D3D12_MINI_STDCALL *CreateCommandQueue)(self * This, const D3D12_COMMAND_QUEUE_DESC *desc, REFIID riid,    \
                                                    void **queue);                                                     \
    HRESULT(D3D12_MINI_STDCALL *CreateCommandAllocator)(self * This, D3D12_COMMAND_LIST_TYPE type, REFIID riid,        \
                                                        void **allocator);                                             \
    HRESULT(D3D12_MINI_STDCALL *CreateGraphicsPipelineState)(self * This, const D3D12_GRAPHICS_PIPELINE_STATE_DESC *desc, \
                                                             REFIID riid, void **state);                               \
    HRESULT(D3D12_MINI_STDCALL *CreateComputePipelineState)(self * This, const D3D12_COMPUTE_PIPELINE_STATE_DESC *desc, \
                                                            REFIID riid, void **state);                                \
    HRESULT(D3D12_MINI_STDCALL *CreateCommandList)(self * This, UINT node_mask, D3D12_COMMAND_LIST_TYPE type,          \
                                                   ID3D12CommandAllocator *allocator, ID3D12PipelineState *initial,    \
                                                   REFIID riid, void **list);                                          \
    HRESULT(D3D12_MINI_STDCALL *CheckFeatureSupport)(self * This, D3D12_FEATURE feature, void *data, UINT size);       \
    HRESULT(D3D12_MINI_STDCALL *CreateDescriptorHeap)(self * This, const D3D12_DESCRIPTOR_HEAP_DESC *desc, REFIID riid, \
                                                      void **heap);                                                    \
    UINT(D3D12_MINI_STDCALL *GetDescriptorHandleIncrementSize)(self * This, D3D12_DESCRIPTOR_HEAP_TYPE type);          \
    HRESULT(D3D12_MINI_STDCALL *CreateRootSignature)(self * This, UINT node_mask, const void *blob, SIZE_T size,       \
                                                     REFIID riid, void **signature);                                   \
    d3d12_unused_method CreateConstantBufferView;                                                                      \
    void(D3D12_MINI_STDCALL *CreateShaderResourceView)(self * This, ID3D12Resource * resource,                         \
                                                       const D3D12_SHADER_RESOURCE_VIEW_DESC *desc,                    \
                                                       D3D12_CPU_DESCRIPTOR_HANDLE destination);                       \
    void(D3D12_MINI_STDCALL *CreateUnorderedAccessView)(self * This, ID3D12Resource * resource,                        \
                                                        ID3D12Resource * counter,                                      \
                                                        const D3D12_UNORDERED_ACCESS_VIEW_DESC *desc,                  \
                                                        D3D12_CPU_DESCRIPTOR_HANDLE destination);                      \
    void(D3D12_MINI_STDCALL *CreateRenderTargetView)(self * This, ID3D12Resource * resource,                           \
                                                     const D3D12_RENDER_TARGET_VIEW_DESC *desc,                        \
                                                     D3D12_CPU_DESCRIPTOR_HANDLE destination);                         \
    void(D3D12_MINI_STDCALL *CreateDepthStencilView)(self * This, ID3D12Resource * resource,                           \
                                                     const D3D12_DEPTH_STENCIL_VIEW_DESC *desc,                        \
                                                     D3D12_CPU_DESCRIPTOR_HANDLE destination);                         \
    void(D3D12_MINI_STDCALL *CreateSampler)(self * This, const D3D12_SAMPLER_DESC *desc,                               \
                                            D3D12_CPU_DESCRIPTOR_HANDLE destination);                                  \
    d3d12_unused_method CopyDescriptors;                                                                               \
    void(D3D12_MINI_STDCALL *CopyDescriptorsSimple)(self * This, UINT count, D3D12_CPU_DESCRIPTOR_HANDLE destination,  \
                                                    D3D12_CPU_DESCRIPTOR_HANDLE source,                                \
                                                    D3D12_DESCRIPTOR_HEAP_TYPE type);                                  \
    d3d12_unused_method GetResourceAllocationInfo; /* returns a structure: an explicit out pointer when used */        \
    d3d12_unused_method GetCustomHeapProperties;                                                                       \
    HRESULT(D3D12_MINI_STDCALL *CreateCommittedResource)(self * This, const D3D12_HEAP_PROPERTIES *heap,               \
                                                         D3D12_HEAP_FLAGS heap_flags, const D3D12_RESOURCE_DESC *desc, \
                                                         D3D12_RESOURCE_STATES initial_state,                          \
                                                         const D3D12_CLEAR_VALUE *clear, REFIID riid,                  \
                                                         void **resource);                                             \
    d3d12_unused_method CreateHeap;                                                                                    \
    d3d12_unused_method CreatePlacedResource;                                                                          \
    d3d12_unused_method CreateReservedResource;                                                                        \
    d3d12_unused_method CreateSharedHandle;                                                                            \
    d3d12_unused_method OpenSharedHandle;                                                                              \
    d3d12_unused_method OpenSharedHandleByName;                                                                        \
    d3d12_unused_method MakeResident;                                                                                  \
    d3d12_unused_method Evict;                                                                                         \
    HRESULT(D3D12_MINI_STDCALL *CreateFence)(self * This, UINT64 initial_value, D3D12_FENCE_FLAGS flags, REFIID riid,  \
                                             void **fence);                                                            \
    HRESULT(D3D12_MINI_STDCALL *GetDeviceRemovedReason)(self * This);                                                  \
    void(D3D12_MINI_STDCALL *GetCopyableFootprints)(self * This, const D3D12_RESOURCE_DESC *desc,                      \
                                                    UINT first_subresource, UINT subresource_count,                    \
                                                    UINT64 base_offset, D3D12_PLACED_SUBRESOURCE_FOOTPRINT *layouts,   \
                                                    UINT *row_counts, UINT64 *row_sizes, UINT64 *total_bytes);         \
    HRESULT(D3D12_MINI_STDCALL *CreateQueryHeap)(self * This, const D3D12_QUERY_HEAP_DESC *desc, REFIID riid,          \
                                                 void **heap);                                                         \
    d3d12_unused_method SetStablePowerState;                                                                           \
    d3d12_unused_method CreateCommandSignature;                                                                        \
    d3d12_unused_method GetResourceTiling;                                                                             \
    LUID *(D3D12_MINI_STDCALL *GetAdapterLuid)(self * This, LUID * result); /* aggregate return: explicit pointer */

struct ID3D12DeviceVtbl {
    D3D12_MINI_DEVICE_METHODS(ID3D12Device)
};

struct ID3D12Device1Vtbl {
    D3D12_MINI_DEVICE_METHODS(ID3D12Device1)
    HRESULT(D3D12_MINI_STDCALL *CreatePipelineLibrary)(ID3D12Device1 *This, const void *blob, SIZE_T size, REFIID riid,
                                                       void **library);
    d3d12_unused_method SetEventOnMultipleFenceCompletion;
    d3d12_unused_method SetResidencyPriority;
};

struct ID3D12CommandQueueVtbl {
    D3D12_MINI_DEVICE_CHILD(ID3D12CommandQueue)
    d3d12_unused_method UpdateTileMappings;
    d3d12_unused_method CopyTileMappings;
    void(D3D12_MINI_STDCALL *ExecuteCommandLists)(ID3D12CommandQueue *This, UINT count, ID3D12CommandList *const *lists);
    d3d12_unused_method SetMarker;
    d3d12_unused_method BeginEvent;
    d3d12_unused_method EndEvent;
    HRESULT(D3D12_MINI_STDCALL *Signal)(ID3D12CommandQueue *This, ID3D12Fence *fence, UINT64 value);
    d3d12_unused_method Wait;
    HRESULT(D3D12_MINI_STDCALL *GetTimestampFrequency)(ID3D12CommandQueue *This, UINT64 *frequency);
    d3d12_unused_method GetClockCalibration;
    D3D12_COMMAND_QUEUE_DESC *(D3D12_MINI_STDCALL *GetDesc)(ID3D12CommandQueue *This, D3D12_COMMAND_QUEUE_DESC *result);
};

struct ID3D12CommandAllocatorVtbl {
    D3D12_MINI_DEVICE_CHILD(ID3D12CommandAllocator)
    HRESULT(D3D12_MINI_STDCALL *Reset)(ID3D12CommandAllocator *This);
};

struct ID3D12CommandListVtbl {
    D3D12_MINI_DEVICE_CHILD(ID3D12CommandList)
    D3D12_COMMAND_LIST_TYPE(D3D12_MINI_STDCALL *GetType)(ID3D12CommandList *This);
};

typedef ID3D12GraphicsCommandList GCL;
struct ID3D12GraphicsCommandListVtbl {
    D3D12_MINI_DEVICE_CHILD(ID3D12GraphicsCommandList)
    D3D12_COMMAND_LIST_TYPE(D3D12_MINI_STDCALL *GetType)(GCL *This);
    HRESULT(D3D12_MINI_STDCALL *Close)(GCL *This);
    HRESULT(D3D12_MINI_STDCALL *Reset)(GCL *This, ID3D12CommandAllocator *allocator, ID3D12PipelineState *initial);
    d3d12_unused_method ClearState;
    void(D3D12_MINI_STDCALL *DrawInstanced)(GCL *This, UINT vertex_count, UINT instance_count, UINT first_vertex,
                                            UINT first_instance);
    void(D3D12_MINI_STDCALL *DrawIndexedInstanced)(GCL *This, UINT index_count, UINT instance_count, UINT first_index,
                                                   INT base_vertex, UINT first_instance);
    void(D3D12_MINI_STDCALL *Dispatch)(GCL *This, UINT x, UINT y, UINT z);
    void(D3D12_MINI_STDCALL *CopyBufferRegion)(GCL *This, ID3D12Resource *destination, UINT64 destination_offset,
                                               ID3D12Resource *source, UINT64 source_offset, UINT64 size);
    void(D3D12_MINI_STDCALL *CopyTextureRegion)(GCL *This, const D3D12_TEXTURE_COPY_LOCATION *destination, UINT x, UINT y,
                                                UINT z, const D3D12_TEXTURE_COPY_LOCATION *source, const D3D12_BOX *box);
    void(D3D12_MINI_STDCALL *CopyResource)(GCL *This, ID3D12Resource *destination, ID3D12Resource *source);
    d3d12_unused_method CopyTiles;
    d3d12_unused_method ResolveSubresource;
    void(D3D12_MINI_STDCALL *IASetPrimitiveTopology)(GCL *This, D3D_PRIMITIVE_TOPOLOGY topology);
    void(D3D12_MINI_STDCALL *RSSetViewports)(GCL *This, UINT count, const D3D12_VIEWPORT *viewports);
    void(D3D12_MINI_STDCALL *RSSetScissorRects)(GCL *This, UINT count, const D3D12_RECT *rects);
    void(D3D12_MINI_STDCALL *OMSetBlendFactor)(GCL *This, const FLOAT factor[4]);
    void(D3D12_MINI_STDCALL *OMSetStencilRef)(GCL *This, UINT reference);
    void(D3D12_MINI_STDCALL *SetPipelineState)(GCL *This, ID3D12PipelineState *state);
    void(D3D12_MINI_STDCALL *ResourceBarrier)(GCL *This, UINT count, const D3D12_RESOURCE_BARRIER *barriers);
    d3d12_unused_method ExecuteBundle;
    void(D3D12_MINI_STDCALL *SetDescriptorHeaps)(GCL *This, UINT count, ID3D12DescriptorHeap *const *heaps);
    void(D3D12_MINI_STDCALL *SetComputeRootSignature)(GCL *This, ID3D12RootSignature *signature);
    void(D3D12_MINI_STDCALL *SetGraphicsRootSignature)(GCL *This, ID3D12RootSignature *signature);
    void(D3D12_MINI_STDCALL *SetComputeRootDescriptorTable)(GCL *This, UINT parameter, D3D12_GPU_DESCRIPTOR_HANDLE table);
    void(D3D12_MINI_STDCALL *SetGraphicsRootDescriptorTable)(GCL *This, UINT parameter, D3D12_GPU_DESCRIPTOR_HANDLE table);
    d3d12_unused_method SetComputeRoot32BitConstant;
    d3d12_unused_method SetGraphicsRoot32BitConstant;
    void(D3D12_MINI_STDCALL *SetComputeRoot32BitConstants)(GCL *This, UINT parameter, UINT count, const void *data,
                                                           UINT offset);
    void(D3D12_MINI_STDCALL *SetGraphicsRoot32BitConstants)(GCL *This, UINT parameter, UINT count, const void *data,
                                                            UINT offset);
    void(D3D12_MINI_STDCALL *SetComputeRootConstantBufferView)(GCL *This, UINT parameter, D3D12_GPU_VIRTUAL_ADDRESS address);
    void(D3D12_MINI_STDCALL *SetGraphicsRootConstantBufferView)(GCL *This, UINT parameter,
                                                                D3D12_GPU_VIRTUAL_ADDRESS address);
    d3d12_unused_method SetComputeRootShaderResourceView;
    d3d12_unused_method SetGraphicsRootShaderResourceView;
    d3d12_unused_method SetComputeRootUnorderedAccessView;
    d3d12_unused_method SetGraphicsRootUnorderedAccessView;
    void(D3D12_MINI_STDCALL *IASetIndexBuffer)(GCL *This, const D3D12_INDEX_BUFFER_VIEW *view);
    void(D3D12_MINI_STDCALL *IASetVertexBuffers)(GCL *This, UINT start, UINT count, const D3D12_VERTEX_BUFFER_VIEW *views);
    d3d12_unused_method SOSetTargets;
    void(D3D12_MINI_STDCALL *OMSetRenderTargets)(GCL *This, UINT count, const D3D12_CPU_DESCRIPTOR_HANDLE *targets,
                                                 BOOL single_range, const D3D12_CPU_DESCRIPTOR_HANDLE *depth_stencil);
    void(D3D12_MINI_STDCALL *ClearDepthStencilView)(GCL *This, D3D12_CPU_DESCRIPTOR_HANDLE view, D3D12_CLEAR_FLAGS flags,
                                                    FLOAT depth, UINT8 stencil, UINT rect_count, const D3D12_RECT *rects);
    void(D3D12_MINI_STDCALL *ClearRenderTargetView)(GCL *This, D3D12_CPU_DESCRIPTOR_HANDLE view, const FLOAT color[4],
                                                    UINT rect_count, const D3D12_RECT *rects);
    d3d12_unused_method ClearUnorderedAccessViewUint;
    d3d12_unused_method ClearUnorderedAccessViewFloat;
    d3d12_unused_method DiscardResource;
    void(D3D12_MINI_STDCALL *BeginQuery)(GCL *This, ID3D12QueryHeap *heap, D3D12_QUERY_TYPE type, UINT index);
    void(D3D12_MINI_STDCALL *EndQuery)(GCL *This, ID3D12QueryHeap *heap, D3D12_QUERY_TYPE type, UINT index);
    void(D3D12_MINI_STDCALL *ResolveQueryData)(GCL *This, ID3D12QueryHeap *heap, D3D12_QUERY_TYPE type, UINT first,
                                               UINT count, ID3D12Resource *destination, UINT64 offset);
    d3d12_unused_method SetPredication;
    d3d12_unused_method SetMarker;
    d3d12_unused_method BeginEvent;
    d3d12_unused_method EndEvent;
    d3d12_unused_method ExecuteIndirect;
};

struct ID3D12ResourceVtbl {
    D3D12_MINI_DEVICE_CHILD(ID3D12Resource)
    HRESULT(D3D12_MINI_STDCALL *Map)(ID3D12Resource *This, UINT subresource, const D3D12_RANGE *read, void **data);
    void(D3D12_MINI_STDCALL *Unmap)(ID3D12Resource *This, UINT subresource, const D3D12_RANGE *written);
    D3D12_RESOURCE_DESC *(D3D12_MINI_STDCALL *GetDesc)(ID3D12Resource *This, D3D12_RESOURCE_DESC *result);
    D3D12_GPU_VIRTUAL_ADDRESS(D3D12_MINI_STDCALL *GetGPUVirtualAddress)(ID3D12Resource *This); /* a scalar: EDX:EAX */
    d3d12_unused_method WriteToSubresource;
    d3d12_unused_method ReadFromSubresource;
    d3d12_unused_method GetHeapProperties;
};

struct ID3D12DescriptorHeapVtbl {
    D3D12_MINI_DEVICE_CHILD(ID3D12DescriptorHeap)
    D3D12_DESCRIPTOR_HEAP_DESC *(D3D12_MINI_STDCALL *GetDesc)(ID3D12DescriptorHeap *This,
                                                               D3D12_DESCRIPTOR_HEAP_DESC *result);
    D3D12_CPU_DESCRIPTOR_HANDLE *(D3D12_MINI_STDCALL *GetCPUDescriptorHandleForHeapStart)(
        ID3D12DescriptorHeap *This, D3D12_CPU_DESCRIPTOR_HANDLE *result);
    D3D12_GPU_DESCRIPTOR_HANDLE *(D3D12_MINI_STDCALL *GetGPUDescriptorHandleForHeapStart)(
        ID3D12DescriptorHeap *This, D3D12_GPU_DESCRIPTOR_HANDLE *result);
};

struct ID3D12RootSignatureVtbl {
    D3D12_MINI_DEVICE_CHILD(ID3D12RootSignature)
};

struct ID3D12PipelineStateVtbl {
    D3D12_MINI_DEVICE_CHILD(ID3D12PipelineState)
    d3d12_unused_method GetCachedBlob;
};

struct ID3D12PipelineLibraryVtbl {
    D3D12_MINI_DEVICE_CHILD(ID3D12PipelineLibrary)
    HRESULT(D3D12_MINI_STDCALL *StorePipeline)(ID3D12PipelineLibrary *This, LPCWSTR name, ID3D12PipelineState *state);
    HRESULT(D3D12_MINI_STDCALL *LoadGraphicsPipeline)(ID3D12PipelineLibrary *This, LPCWSTR name,
                                                      const D3D12_GRAPHICS_PIPELINE_STATE_DESC *desc, REFIID riid,
                                                      void **state);
    HRESULT(D3D12_MINI_STDCALL *LoadComputePipeline)(ID3D12PipelineLibrary *This, LPCWSTR name,
                                                     const D3D12_COMPUTE_PIPELINE_STATE_DESC *desc, REFIID riid,
                                                     void **state);
    SIZE_T(D3D12_MINI_STDCALL *GetSerializedSize)(ID3D12PipelineLibrary *This);
    HRESULT(D3D12_MINI_STDCALL *Serialize)(ID3D12PipelineLibrary *This, void *data, SIZE_T size);
};

struct ID3D12FenceVtbl {
    D3D12_MINI_DEVICE_CHILD(ID3D12Fence)
    UINT64(D3D12_MINI_STDCALL *GetCompletedValue)(ID3D12Fence *This);
    HRESULT(D3D12_MINI_STDCALL *SetEventOnCompletion)(ID3D12Fence *This, UINT64 value, HANDLE event);
    HRESULT(D3D12_MINI_STDCALL *Signal)(ID3D12Fence *This, UINT64 value);
};

struct ID3D12QueryHeapVtbl {
    D3D12_MINI_DEVICE_CHILD(ID3D12QueryHeap)
};

struct ID3D12DebugVtbl {
    D3D12_MINI_IUNKNOWN(ID3D12Debug)
    void(D3D12_MINI_STDCALL *EnableDebugLayer)(ID3D12Debug *This);
};

struct ID3D12Debug1Vtbl {
    D3D12_MINI_IUNKNOWN(ID3D12Debug1)
    void(D3D12_MINI_STDCALL *EnableDebugLayer)(ID3D12Debug1 *This);
    void(D3D12_MINI_STDCALL *SetEnableGPUBasedValidation)(ID3D12Debug1 *This, BOOL enable);
    d3d12_unused_method SetEnableSynchronizedCommandQueueValidation;
};

struct ID3D12InfoQueueVtbl {
    D3D12_MINI_IUNKNOWN(ID3D12InfoQueue)
    HRESULT(D3D12_MINI_STDCALL *SetMessageCountLimit)(ID3D12InfoQueue *This, UINT64 limit);
    void(D3D12_MINI_STDCALL *ClearStoredMessages)(ID3D12InfoQueue *This);
    HRESULT(D3D12_MINI_STDCALL *GetMessage)(ID3D12InfoQueue *This, UINT64 index, D3D12_MESSAGE *message, SIZE_T *length);
    d3d12_unused_method GetNumMessagesAllowedByStorageFilter;
    d3d12_unused_method GetNumMessagesDeniedByStorageFilter;
    UINT64(D3D12_MINI_STDCALL *GetNumStoredMessages)(ID3D12InfoQueue *This);
    d3d12_unused_method GetNumStoredMessagesAllowedByRetrievalFilter;
    d3d12_unused_method GetNumMessagesDiscardedByMessageCountLimit;
    d3d12_unused_method GetMessageCountLimit;
    d3d12_unused_method AddStorageFilterEntries;
    d3d12_unused_method GetStorageFilter;
    d3d12_unused_method ClearStorageFilter;
    d3d12_unused_method PushEmptyStorageFilter;
    d3d12_unused_method PushCopyOfStorageFilter;
    HRESULT(D3D12_MINI_STDCALL *PushStorageFilter)(ID3D12InfoQueue *This, D3D12_INFO_QUEUE_FILTER *filter);
    d3d12_unused_method PopStorageFilter;
    d3d12_unused_method GetStorageFilterStackSize;
    d3d12_unused_method AddRetrievalFilterEntries;
    d3d12_unused_method GetRetrievalFilter;
    d3d12_unused_method ClearRetrievalFilter;
    d3d12_unused_method PushEmptyRetrievalFilter;
    d3d12_unused_method PushCopyOfRetrievalFilter;
    d3d12_unused_method PushRetrievalFilter;
    d3d12_unused_method PopRetrievalFilter;
    d3d12_unused_method GetRetrievalFilterStackSize;
    d3d12_unused_method AddMessage;
    d3d12_unused_method AddApplicationMessage;
    d3d12_unused_method SetBreakOnCategory;
    d3d12_unused_method SetBreakOnSeverity;
    d3d12_unused_method SetBreakOnID;
    d3d12_unused_method GetBreakOnCategory;
    d3d12_unused_method GetBreakOnSeverity;
    d3d12_unused_method GetBreakOnID;
    d3d12_unused_method SetMuteDebugOutput;
    d3d12_unused_method GetMuteDebugOutput;
};

// IDXGIObject
#define D3D12_MINI_DXGI_OBJECT(self)                                                                                   \
    D3D12_MINI_IUNKNOWN(self)                                                                                          \
    d3d12_unused_method SetPrivateData;                                                                                \
    d3d12_unused_method SetPrivateDataInterface;                                                                       \
    d3d12_unused_method GetPrivateData;                                                                                \
    d3d12_unused_method GetParent;

struct IDXGIFactory6Vtbl {
    D3D12_MINI_DXGI_OBJECT(IDXGIFactory6)
    // IDXGIFactory
    d3d12_unused_method EnumAdapters;
    HRESULT(D3D12_MINI_STDCALL *MakeWindowAssociation)(IDXGIFactory6 *This, HWND window, UINT flags);
    d3d12_unused_method GetWindowAssociation;
    d3d12_unused_method CreateSwapChain;
    d3d12_unused_method CreateSoftwareAdapter;
    // IDXGIFactory1
    HRESULT(D3D12_MINI_STDCALL *EnumAdapters1)(IDXGIFactory6 *This, UINT index, IDXGIAdapter1 **adapter);
    d3d12_unused_method IsCurrent;
    // IDXGIFactory2
    d3d12_unused_method IsWindowedStereoEnabled;
    HRESULT(D3D12_MINI_STDCALL *CreateSwapChainForHwnd)(IDXGIFactory6 *This, IUnknown *device, HWND window,
                                                        const DXGI_SWAP_CHAIN_DESC1 *desc, const void *fullscreen_desc,
                                                        void *restrict_to_output, IDXGISwapChain3 **swapchain);
    d3d12_unused_method CreateSwapChainForCoreWindow;
    d3d12_unused_method GetSharedResourceAdapterLuid;
    d3d12_unused_method RegisterStereoStatusWindow;
    d3d12_unused_method RegisterStereoStatusEvent;
    d3d12_unused_method UnregisterStereoStatus;
    d3d12_unused_method RegisterOcclusionStatusWindow;
    d3d12_unused_method RegisterOcclusionStatusEvent;
    d3d12_unused_method UnregisterOcclusionStatus;
    d3d12_unused_method CreateSwapChainForComposition;
    // IDXGIFactory3
    d3d12_unused_method GetCreationFlags;
    // IDXGIFactory4
    d3d12_unused_method EnumAdapterByLuid;
    HRESULT(D3D12_MINI_STDCALL *EnumWarpAdapter)(IDXGIFactory6 *This, REFIID riid, void **adapter);
    // IDXGIFactory5 (QueryInterface for IID_IDXGIFactory5 before use)
    HRESULT(D3D12_MINI_STDCALL *CheckFeatureSupport)(IDXGIFactory6 *This, DXGI_FEATURE feature, void *data, UINT size);
    // IDXGIFactory6 (QueryInterface for IID_IDXGIFactory6 before use)
    HRESULT(D3D12_MINI_STDCALL *EnumAdapterByGpuPreference)(IDXGIFactory6 *This, UINT index, DXGI_GPU_PREFERENCE preference,
                                                            REFIID riid, void **adapter);
};

struct IDXGIAdapter1Vtbl {
    D3D12_MINI_DXGI_OBJECT(IDXGIAdapter1)
    d3d12_unused_method EnumOutputs;
    d3d12_unused_method GetDesc;
    d3d12_unused_method CheckInterfaceSupport;
    HRESULT(D3D12_MINI_STDCALL *GetDesc1)(IDXGIAdapter1 *This, DXGI_ADAPTER_DESC1 *desc);
};

struct IDXGISwapChain3Vtbl {
    D3D12_MINI_DXGI_OBJECT(IDXGISwapChain3)
    d3d12_unused_method GetDevice; // IDXGIDeviceSubObject
    // IDXGISwapChain
    HRESULT(D3D12_MINI_STDCALL *Present)(IDXGISwapChain3 *This, UINT sync_interval, UINT flags);
    HRESULT(D3D12_MINI_STDCALL *GetBuffer)(IDXGISwapChain3 *This, UINT index, REFIID riid, void **surface);
    d3d12_unused_method SetFullscreenState;
    d3d12_unused_method GetFullscreenState;
    d3d12_unused_method GetDesc;
    HRESULT(D3D12_MINI_STDCALL *ResizeBuffers)(IDXGISwapChain3 *This, UINT count, UINT width, UINT height,
                                               DXGI_FORMAT format, UINT flags);
    d3d12_unused_method ResizeTarget;
    d3d12_unused_method GetContainingOutput;
    d3d12_unused_method GetFrameStatistics;
    d3d12_unused_method GetLastPresentCount;
    // IDXGISwapChain1
    HRESULT(D3D12_MINI_STDCALL *GetDesc1)(IDXGISwapChain3 *This, DXGI_SWAP_CHAIN_DESC1 *desc);
    d3d12_unused_method GetFullscreenDesc;
    d3d12_unused_method GetHwnd;
    d3d12_unused_method GetCoreWindow;
    d3d12_unused_method Present1;
    d3d12_unused_method IsTemporaryMonoSupported;
    d3d12_unused_method GetRestrictToOutput;
    d3d12_unused_method SetBackgroundColor;
    d3d12_unused_method GetBackgroundColor;
    d3d12_unused_method SetRotation;
    d3d12_unused_method GetRotation;
    // IDXGISwapChain2
    d3d12_unused_method SetSourceSize;
    d3d12_unused_method GetSourceSize;
    HRESULT(D3D12_MINI_STDCALL *SetMaximumFrameLatency)(IDXGISwapChain3 *This, UINT latency);
    d3d12_unused_method GetMaximumFrameLatency;
    d3d12_unused_method GetFrameLatencyWaitableObject;
    d3d12_unused_method SetMatrixTransform;
    d3d12_unused_method GetMatrixTransform;
    // IDXGISwapChain3
    UINT(D3D12_MINI_STDCALL *GetCurrentBackBufferIndex)(IDXGISwapChain3 *This);
    d3d12_unused_method CheckColorSpaceSupport;
    d3d12_unused_method SetColorSpace1;
    d3d12_unused_method ResizeBuffers1;
};

#undef D3D12_MINI_INTERFACE
#undef D3D12_MINI_IUNKNOWN
#undef D3D12_MINI_OBJECT
#undef D3D12_MINI_DEVICE_CHILD
#undef D3D12_MINI_DEVICE_METHODS
#undef D3D12_MINI_DXGI_OBJECT

// ---- the exports opened at run time (d3d12.dll, dxgi.dll; WINAPI = __stdcall) ----

typedef HRESULT(D3D12_MINI_STDCALL *PFN_D3D12_CREATE_DEVICE)(IUnknown *adapter, D3D_FEATURE_LEVEL minimum_level,
                                                             REFIID riid, void **device);
typedef HRESULT(D3D12_MINI_STDCALL *PFN_D3D12_GET_DEBUG_INTERFACE)(REFIID riid, void **debug);
typedef HRESULT(D3D12_MINI_STDCALL *PFN_CREATE_DXGI_FACTORY2)(UINT flags, REFIID riid, void **factory);

// ---- the ABI facts the backend relies on (the full field-by-field check is the test against the SDK headers) ----

static_assert(sizeof(void *) == 4 || sizeof(void *) == 8, "d3d12_mini.h: x86 or x64 layouts");
static_assert(sizeof(D3D12_CPU_DESCRIPTOR_HANDLE) == sizeof(void *) && sizeof(D3D12_GPU_DESCRIPTOR_HANDLE) == 8, "handles");
static_assert(sizeof(D3D12_RESOURCE_DESC) == 56 && offsetof(D3D12_RESOURCE_DESC, Alignment) == 8 &&
                  offsetof(D3D12_RESOURCE_DESC, Format) == 32,
              "D3D12_RESOURCE_DESC: UINT64 members 8-aligned (MSVC layout)");
static_assert(sizeof(D3D12_RESOURCE_BARRIER) == (sizeof(void *) == 8 ? 32 : 24), "D3D12_RESOURCE_BARRIER");
static_assert(sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC) == 40 && offsetof(D3D12_SHADER_RESOURCE_VIEW_DESC, Buffer) == 16,
              "D3D12_SHADER_RESOURCE_VIEW_DESC");
static_assert(sizeof(D3D12_UNORDERED_ACCESS_VIEW_DESC) == 40 && offsetof(D3D12_UNORDERED_ACCESS_VIEW_DESC, Buffer) == 8,
              "D3D12_UNORDERED_ACCESS_VIEW_DESC");
static_assert(sizeof(D3D12_RENDER_TARGET_VIEW_DESC) == 24, "D3D12_RENDER_TARGET_VIEW_DESC");
static_assert(sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC) == 24, "D3D12_DEPTH_STENCIL_VIEW_DESC");
static_assert(sizeof(D3D12_TEXTURE_COPY_LOCATION) == (sizeof(void *) == 8 ? 48 : 40) &&
                  offsetof(D3D12_TEXTURE_COPY_LOCATION, PlacedFootprint) == (sizeof(void *) == 8 ? 16 : 8),
              "D3D12_TEXTURE_COPY_LOCATION");
static_assert(sizeof(D3D12_SAMPLER_DESC) == 52, "D3D12_SAMPLER_DESC");
static_assert(sizeof(D3D12_INDEX_BUFFER_VIEW) == 16 && sizeof(D3D12_VERTEX_BUFFER_VIEW) == 16, "buffer views");
static_assert(sizeof(D3D12_RENDER_TARGET_BLEND_DESC) == 40 && sizeof(D3D12_BLEND_DESC) == 328, "D3D12_BLEND_DESC");
static_assert(sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC) == (sizeof(void *) == 8 ? 656 : 572),
              "D3D12_GRAPHICS_PIPELINE_STATE_DESC");
static_assert(sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC) == (sizeof(void *) == 8 ? 56 : 28),
              "D3D12_COMPUTE_PIPELINE_STATE_DESC");
static_assert(sizeof(DXGI_SWAP_CHAIN_DESC1) == 48, "DXGI_SWAP_CHAIN_DESC1");
static_assert(sizeof(DXGI_ADAPTER_DESC1) == (sizeof(void *) == 8 ? 312 : 296), "DXGI_ADAPTER_DESC1");
static_assert(offsetof(ID3D12DeviceVtbl, CreateCommittedResource) == 27 * sizeof(void *) &&
                  offsetof(ID3D12DeviceVtbl, GetAdapterLuid) == 43 * sizeof(void *) &&
                  offsetof(ID3D12Device1Vtbl, CreatePipelineLibrary) == 44 * sizeof(void *),
              "ID3D12Device slots");
static_assert(offsetof(ID3D12GraphicsCommandListVtbl, ResourceBarrier) == 26 * sizeof(void *) &&
                  offsetof(ID3D12GraphicsCommandListVtbl, OMSetRenderTargets) == 46 * sizeof(void *) &&
                  offsetof(ID3D12GraphicsCommandListVtbl, ExecuteIndirect) == 59 * sizeof(void *),
              "ID3D12GraphicsCommandList slots");
static_assert(offsetof(ID3D12DescriptorHeapVtbl, GetCPUDescriptorHandleForHeapStart) == 9 * sizeof(void *), "heap slots");
static_assert(offsetof(IDXGIFactory6Vtbl, CreateSwapChainForHwnd) == 15 * sizeof(void *) &&
                  offsetof(IDXGIFactory6Vtbl, EnumAdapterByGpuPreference) == 29 * sizeof(void *),
              "IDXGIFactory6 slots");
static_assert(offsetof(IDXGISwapChain3Vtbl, GetCurrentBackBufferIndex) == 36 * sizeof(void *), "IDXGISwapChain3 slots");

} // namespace d3d12

#endif /* PORT_D3D_D3D12_MINI_H */
