// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// xcompress.h: the XMem LZX compression / decompression context API.
#pragma once
#ifndef _XCOMPRESS_H_
#define _XCOMPRESS_H_

#ifndef XBOXAPI
#define XBOXAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _XMEMCODEC_TYPE {
    XMEMCODEC_DEFAULT = 0,
    XMEMCODEC_LZX = 1
} XMEMCODEC_TYPE;

#define XMEMCOMPRESS_STREAM 0x00000001

typedef VOID *XMEMCOMPRESSION_CONTEXT;
typedef VOID *XMEMDECOMPRESSION_CONTEXT;

XBOXAPI HRESULT WINAPI XMemCreateDecompressionContext(IN XMEMCODEC_TYPE CodecType, IN CONST VOID *pCodecParams, IN DWORD Flags,
                                                      OUT XMEMDECOMPRESSION_CONTEXT *pContext);
XBOXAPI VOID WINAPI XMemDestroyDecompressionContext(IN OUT XMEMDECOMPRESSION_CONTEXT Context);
XBOXAPI HRESULT WINAPI XMemResetDecompressionContext(IN XMEMDECOMPRESSION_CONTEXT Context);
XBOXAPI HRESULT WINAPI XMemDecompress(IN XMEMDECOMPRESSION_CONTEXT Context, OUT VOID *pDestination, IN OUT SIZE_T *pDestSize,
                                      IN CONST VOID *pSource, IN SIZE_T SrcSize);
XBOXAPI HRESULT WINAPI XMemDecompressStream(IN XMEMDECOMPRESSION_CONTEXT Context, OUT VOID *pDestination,
                                            IN OUT SIZE_T *pDestSize, IN CONST VOID *pSource, IN OUT SIZE_T *pSrcSize);

typedef struct _XMEMCODEC_PARAMETERS_LZX {
    DWORD Flags;
    DWORD WindowSize;
    DWORD CompressionPartitionSize;
} XMEMCODEC_PARAMETERS_LZX;

#ifdef __cplusplus
}
#endif

#endif // _XCOMPRESS_H_
