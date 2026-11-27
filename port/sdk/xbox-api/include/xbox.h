// Halo 3 PC port: clean-room Xbox 360 SDK headers (zero-XDK build).
// xbox.h: Xbox 360 title APIs: XUIDs, memory (XMem/XPhysical), overlapped I/O, content, users, stats, guide UI, hashing.
#pragma once
#ifndef _XBOX_H_
#define _XBOX_H_

#define XBOXAPI

#ifdef __cplusplus
extern "C" {
#endif

#include <xconfig.h>
#include <xinputdefs.h>

#define XUSER_NAME_SIZE 16
#define XUSER_MAX_NAME_LENGTH (XUSER_NAME_SIZE - 1)

typedef ULONGLONG XUID;
typedef XUID *PXUID;
#define INVALID_XUID ((XUID)0)

XBOXAPI FORCEINLINE BOOL WINAPI IsEqualXUID(IN XUID xuid1, IN XUID xuid2)
{
    return xuid1 == xuid2;
}

// Team XUIDs carry 0xFE in their top byte.
XBOXAPI FORCEINLINE BOOL WINAPI IsTeamXUID(XUID xuid)
{
    return (xuid & 0xFF00000000000000) == 0xFE00000000000000;
}

#ifndef _NTOS_
#define XGetLanguage XTLGetLanguage
#endif
XBOXAPI DWORD WINAPI XGetLanguage(VOID);
XBOXAPI DWORD WINAPI XGetLocale(VOID);

typedef struct _XVIDEO_MODE {
    DWORD dwDisplayWidth;
    DWORD dwDisplayHeight;
    BOOL fIsInterlaced;
    BOOL fIsWideScreen;
    BOOL fIsHiDef;
    float RefreshRate;
    DWORD VideoStandard;
    DWORD Reserved[5];
} XVIDEO_MODE, *PXVIDEO_MODE;

#define XC_VIDEO_STANDARD_NTSC_M 1
#define XC_VIDEO_STANDARD_NTSC_J 2
#define XC_VIDEO_STANDARD_PAL_I 3

XBOXAPI VOID WINAPI XGetVideoMode(OUT PXVIDEO_MODE pVideoMode);

XBOXAPI DWORD WINAPI XMountUtilityDrive(IN BOOL fFormatClean, IN DWORD dwBytesPerCluster, IN SIZE_T dwFileCacheSize);
XBOXAPI DWORD WINAPI XFlushUtilityDrive(VOID);
XBOXAPI DWORD WINAPI XGetLaunchDataSize(OUT LPDWORD pdwLaunchDataSize);

typedef struct _XOVERLAPPED XOVERLAPPED, *PXOVERLAPPED;

XBOXAPI BOOL WINAPI XGetModuleSection(IN HANDLE ModuleHandle, IN LPCSTR szSectionName, OUT PVOID *pSectionData,
                                      OUT ULONG *pSectionSize);
XBOXAPI DWORD WINAPI XSetThreadProcessor(IN HANDLE hThread, IN DWORD dwHardwareThread);
XBOXAPI BOOL WINAPI XSetFileCacheSize(IN SIZE_T dwCacheSize);

XBOXAPI LPVOID WINAPI XPhysicalAlloc(IN SIZE_T dwSize, IN ULONG_PTR ulPhysicalAddress, IN ULONG_PTR ulAlignment,
                                     IN DWORD flProtect);
XBOXAPI VOID WINAPI XPhysicalProtect(IN LPVOID lpAddress, IN SIZE_T dwSize, IN DWORD flNewProtect);
XBOXAPI VOID WINAPI XPhysicalFree(IN LPVOID lpAddress);
XBOXAPI DWORD WINAPI XQueryMemoryProtect(IN LPVOID lpAddress);

// XMemAlloc attribute word: object type (13 bits), flags, allocator id (8), alignment (4), protection (2),
// zero-init and memory type in the top bit.
#define XALLOC_MEMTYPE_HEAP 0
#define XALLOC_MEMTYPE_PHYSICAL 1
#define XALLOC_MEMPROTECT_WRITECOMBINE_LARGE_PAGES 0
#define XALLOC_MEMPROTECT_NOCACHE 1
#define XALLOC_MEMPROTECT_READWRITE 2
#define XALLOC_MEMPROTECT_WRITECOMBINE 3
#define XALLOC_ALIGNMENT_DEFAULT 0x0
#define XALLOC_ALIGNMENT_4 0x1
#define XALLOC_ALIGNMENT_8 0x2
#define XALLOC_ALIGNMENT_16 0x4
#define XALLOC_PHYSICAL_ALIGNMENT_DEFAULT 0x0
#define XALLOC_PHYSICAL_ALIGNMENT_4 0x2
#define XALLOC_PHYSICAL_ALIGNMENT_8 0x3
#define XALLOC_PHYSICAL_ALIGNMENT_16 0x4
#define XALLOC_PHYSICAL_ALIGNMENT_32 0x5
#define XALLOC_PHYSICAL_ALIGNMENT_64 0x6
#define XALLOC_PHYSICAL_ALIGNMENT_128 0x7
#define XALLOC_PHYSICAL_ALIGNMENT_256 0x8
#define XALLOC_PHYSICAL_ALIGNMENT_512 0x9
#define XALLOC_PHYSICAL_ALIGNMENT_1K 0xA
#define XALLOC_PHYSICAL_ALIGNMENT_2K 0xB
#define XALLOC_PHYSICAL_ALIGNMENT_4K 0xC
#define XALLOC_PHYSICAL_ALIGNMENT_8K 0xD
#define XALLOC_PHYSICAL_ALIGNMENT_16K 0xE
#define XALLOC_PHYSICAL_ALIGNMENT_32K 0xF

typedef enum _XALLOC_ALLOCATOR_IDS {
    eXALLOCAllocatorId_GameMin = 0,
    eXALLOCAllocatorId_GameMax = 127,
    eXALLOCAllocatorId_MsReservedMin = 128,
    eXALLOCAllocatorId_D3D = 128,
    eXALLOCAllocatorId_D3DX,
    eXALLOCAllocatorId_XAUDIO,
    eXALLOCAllocatorId_XAPI,
    eXALLOCAllocatorId_XACT,
    eXALLOCAllocatorId_XBOXKERNEL,
    eXALLOCAllocatorId_XBDM,
    eXALLOCAllocatorId_XGRAPHICS,
    eXALLOCAllocatorId_XONLINE,
    eXALLOCAllocatorId_XVOICE,
    eXALLOCAllocatorId_XHV,
    eXALLOCAllocatorId_USB,
    eXALLOCAllocatorId_XMV,
    eXALLOCAllocatorId_SHADERCOMPILER,
    eXALLOCAllocatorId_XUI,
    eXALLOCAllocatorId_XASYNC,
    eXALLOCAllocatorId_XCAM,
    eXALLOCAllocatorId_XVIS,
    eXALLOCAllocatorId_XIME,
    eXALLOCAllocatorId_XFILECACHE,
    eXALLOCAllocatorId_XRN,
    eXALLOCAllocatorID_XMCORE,
    eXALLOCAllocatorId_MsReservedMax = 191,
    eXALLOCAllocatorId_MiddlewareReservedMin = 192,
    eXALLOCAllocatorId_MiddlewareReservedMax = 255
} XALLOC_ALLOCATOR_IDS;

#define MAKE_XALLOC_ATTRIBUTES(ObjectType, HeapTracksAttributes, MustSucceed, FixedSize, AllocatorId, Alignment, \
                               MemoryProtect, ZeroInitialize, MemoryType)                                         \
    ((DWORD)(ObjectType | (HeapTracksAttributes << 13) | (MustSucceed << 14) | (FixedSize << 15) |             \
             (AllocatorId << 16) | (Alignment << 24) | (MemoryProtect << 28) | (ZeroInitialize << 30) |          \
             (MemoryType << 31)))
#define XALLOC_IS_PHYSICAL(Attributes) ((BOOL)(Attributes & 0x80000000) != 0)

XBOXAPI LPVOID WINAPI XMemAlloc(IN SIZE_T dwSize, IN DWORD dwAllocAttributes);
XBOXAPI LPVOID WINAPI XMemAllocDefault(IN SIZE_T dwSize, IN DWORD dwAllocAttributes);
XBOXAPI VOID WINAPI XMemFree(IN OUT PVOID pAddress, IN DWORD dwAllocAttributes);
XBOXAPI VOID WINAPI XMemFreeDefault(IN OUT PVOID pAddress, IN DWORD dwAllocAttributes);
XBOXAPI SIZE_T WINAPI XMemSize(IN PVOID pAddress, IN DWORD dwAllocAttributes);
XBOXAPI SIZE_T WINAPI XMemSizeDefault(IN PVOID pAddress, IN DWORD dwAllocAttributes);
XBOXAPI PVOID WINAPI XMemSet128(OUT PVOID pDest, IN INT c, IN SIZE_T count);
XBOXAPI PVOID WINAPI XMemSet(OUT PVOID pDest, IN INT c, IN SIZE_T count);
XBOXAPI PVOID WINAPI XMemCpy128(OUT PVOID pDest, IN CONST VOID *pSrc, IN SIZE_T count);
XBOXAPI PVOID WINAPI XMemCpy(OUT PVOID pDest, IN CONST VOID *pSrc, IN SIZE_T count);
XBOXAPI DWORD WINAPI XMemGetPageSize(const VOID *address);

#include <xcompress.h>

typedef VOID (WINAPI *PXOVERLAPPED_COMPLETION_ROUTINE)(IN DWORD dwErrorCode, IN DWORD dwNumberOfBytesTransfered,
                                                       IN OUT PXOVERLAPPED pOverlapped);

// Asynchronous XAM/XOnline request block.
typedef struct _XOVERLAPPED {
    ULONG_PTR InternalLow;
    ULONG_PTR InternalHigh;
    ULONG_PTR InternalContext;
    HANDLE hEvent;
    PXOVERLAPPED_COMPLETION_ROUTINE pCompletionRoutine;
    DWORD_PTR dwCompletionContext;
    DWORD dwExtendedError;
} XOVERLAPPED, *PXOVERLAPPED;

#define XHasOverlappedIoCompleted(lpOverlapped) (*((volatile ULONG_PTR *)(&(lpOverlapped)->InternalLow)) != ERROR_IO_PENDING)

XBOXAPI DWORD WINAPI XGetOverlappedResult(IN PXOVERLAPPED pOverlapped, OUT LPDWORD pdwResult OPTIONAL, IN BOOL bWait);
XBOXAPI DWORD WINAPI XGetOverlappedExtendedError(IN PXOVERLAPPED pOverlapped OPTIONAL);

#define XCONTENT_MAX_DISPLAYNAME_LENGTH 128
#define XCONTENT_MAX_FILENAME_LENGTH 42
#define XCONTENTDEVICE_MAX_NAME_LENGTH 27

typedef DWORD XCONTENTDEVICEID, *PXCONTENTDEVICEID;

typedef struct _XCONTENT_DATA {
    XCONTENTDEVICEID DeviceID;
    DWORD dwContentType;
    WCHAR szDisplayName[XCONTENT_MAX_DISPLAYNAME_LENGTH];
    CHAR szFileName[XCONTENT_MAX_FILENAME_LENGTH];
} XCONTENT_DATA, *PXCONTENT_DATA;

typedef DWORD XCONTENTDEVICETYPE, *PXCONTENTDEVICETYPE;

typedef struct _XDEVICE_DATA {
    XCONTENTDEVICEID DeviceID;
    XCONTENTDEVICETYPE DeviceType;
    ULONGLONG ulDeviceBytes;
    ULONGLONG ulDeviceFreeBytes;
    WCHAR wszFriendlyName[XCONTENTDEVICE_MAX_NAME_LENGTH];
} XDEVICE_DATA, *PXDEVICE_DATA;

XBOXAPI DWORD WINAPI XContentCreateEx(IN DWORD dwUserIndex, IN LPCSTR szRootName, IN CONST XCONTENT_DATA *pContentData,
                                      IN DWORD dwContentFlags, OUT PDWORD pdwDisposition OPTIONAL,
                                      OUT PDWORD pdwLicenseMask OPTIONAL, IN SIZE_T dwFileCacheSize,
                                      IN ULARGE_INTEGER uliContentSize, IN OUT PXOVERLAPPED pOverlapped OPTIONAL);
XBOXAPI DWORD WINAPI XContentDelete(IN DWORD dwUserIndex OPTIONAL, IN CONST XCONTENT_DATA *pContentData,
                                    IN OUT PXOVERLAPPED pOverlapped OPTIONAL);
XBOXAPI DWORD WINAPI XContentClose(IN LPCSTR szRootName, IN OUT PXOVERLAPPED pOverlapped OPTIONAL);
XBOXAPI DWORD WINAPI XContentFlush(IN LPCSTR szRootName, IN OUT PXOVERLAPPED pOverlapped OPTIONAL);
XBOXAPI DWORD WINAPI XContentGetDeviceData(IN XCONTENTDEVICEID DeviceID, OUT PXDEVICE_DATA pDeviceData);
XBOXAPI DWORD WINAPI XContentCreateEnumerator(IN DWORD dwUserIndex, IN XCONTENTDEVICEID DeviceID, IN DWORD dwContentType,
                                              IN DWORD dwContentFlags, IN DWORD cItem, OUT PDWORD pcbBuffer OPTIONAL,
                                              OUT PHANDLE phEnum);
XBOXAPI DWORD WINAPI XEnumerate(IN HANDLE hEnum, IN OUT PVOID pvBuffer, IN DWORD cbBuffer,
                                OUT PDWORD pcItemsReturned OPTIONAL, IN OUT PXOVERLAPPED pOverlapped OPTIONAL);

typedef enum _XUSER_SIGNIN_STATE {
    eXUserSigninState_NotSignedIn,
    eXUserSigninState_SignedInLocally,
    eXUserSigninState_SignedInToLive
} XUSER_SIGNIN_STATE;

XBOXAPI DWORD WINAPI XUserGetName(IN DWORD dwUserIndex, OUT LPSTR szUserName, IN DWORD cchUserName);
XBOXAPI XUSER_SIGNIN_STATE WINAPI XUserGetSigninState(IN DWORD dwUserIndex);

typedef enum _XPRIVILEGE_TYPE {
    XPRIVILEGE_MULTIPLAYER_SESSIONS = 254,
    XPRIVILEGE_COMMUNICATIONS = 252,
    XPRIVILEGE_COMMUNICATIONS_FRIENDS_ONLY = 251,
    XPRIVILEGE_PROFILE_VIEWING = 249,
    XPRIVILEGE_PROFILE_VIEWING_FRIENDS_ONLY = 248,
    XPRIVILEGE_USER_CREATED_CONTENT = 247,
    XPRIVILEGE_USER_CREATED_CONTENT_FRIENDS_ONLY = 246,
    XPRIVILEGE_PURCHASE_CONTENT = 245,
    XPRIVILEGE_PRESENCE = 244,
    XPRIVILEGE_PRESENCE_FRIENDS_ONLY = 243,
    XPRIVILEGE_TRADE_CONTENT = 238,
    XPRIVILEGE_VIDEO_COMMUNICATIONS = 235,
    XPRIVILEGE_VIDEO_COMMUNICATIONS_FRIENDS_ONLY = 234,
} XPRIVILEGE_TYPE;

XBOXAPI DWORD WINAPI XUserAreUsersFriends(IN DWORD dwUserIndex, IN PXUID pXuids, IN DWORD dwXuidCount,
                                          OUT PBOOL pfResult OPTIONAL, IN OUT PXOVERLAPPED pOverlapped OPTIONAL);
XBOXAPI DWORD WINAPI XUserCheckPrivilege(IN DWORD dwUserIndex, IN XPRIVILEGE_TYPE PrivilegeType, OUT PBOOL pfResult);

// Tagged value used by properties, contexts and stats columns.
typedef struct _XUSER_DATA {
    BYTE type;
    union {
        LONG nData;
        LONGLONG i64Data;
        double dblData;
        struct {
            DWORD cbData;
            LPWSTR pwszData;
        } string;
        FLOAT fData;
        struct {
            DWORD cbData;
            PBYTE pbData;
        } binary;
        FILETIME ftData;
    };
} XUSER_DATA, *PXUSER_DATA;

typedef struct _XUSER_PROPERTY {
    DWORD dwPropertyId;
    XUSER_DATA value;
} XUSER_PROPERTY, *PXUSER_PROPERTY;

typedef struct _XUSER_CONTEXT {
    DWORD dwContextId;
    DWORD dwValue;
} XUSER_CONTEXT, *PXUSER_CONTEXT;

XBOXAPI DWORD WINAPI XUserSetPropertyEx(IN DWORD dwUserIndex, IN DWORD dwPropertyId, IN DWORD cbValue,
                                        IN CONST VOID *pvValue, IN OUT PXOVERLAPPED pOverlapped OPTIONAL);

typedef struct {
    DWORD dwUserIndex;
    DWORD dwAchievementId;
} XUSER_ACHIEVEMENT, *PXUSER_ACHIEVEMENT;

XBOXAPI DWORD WINAPI XUserWriteAchievements(IN DWORD dwNumAchievements, IN CONST XUSER_ACHIEVEMENT *pAchievements,
                                            IN OUT PXOVERLAPPED pOverlapped OPTIONAL);
XBOXAPI DWORD WINAPI XUserCreateAchievementEnumerator(IN DWORD dwTitleId, IN DWORD dwUserIndex, IN XUID xuid,
                                                      IN DWORD dwDetailFlags, IN DWORD dwStartingIndex, IN DWORD cItem,
                                                      OUT PDWORD pcbBuffer, OUT PHANDLE ph);

typedef struct _XUSER_STATS_COLUMN {
    WORD wColumnId;
    XUSER_DATA Value;
} XUSER_STATS_COLUMN, *PXUSER_STATS_COLUMN;

typedef struct _XUSER_STATS_ROW {
    XUID xuid;
    DWORD dwRank;
    LONGLONG i64Rating;
    CHAR szGamertag[XUSER_NAME_SIZE];
    DWORD dwNumColumns;
    PXUSER_STATS_COLUMN pColumns;
} XUSER_STATS_ROW, *PXUSER_STATS_ROW;

typedef struct _XUSER_STATS_VIEW {
    DWORD dwViewId;
    DWORD dwTotalViewRows;
    DWORD dwNumRows;
    PXUSER_STATS_ROW pRows;
} XUSER_STATS_VIEW, *PXUSER_STATS_VIEW;

typedef struct _XUSER_STATS_READ_RESULTS {
    DWORD dwNumViews;
    PXUSER_STATS_VIEW pViews;
} XUSER_STATS_READ_RESULTS, *PXUSER_STATS_READ_RESULTS;

#define XUSER_STATS_ATTRS_IN_SPEC 64

typedef struct _XUSER_STATS_SPEC {
    DWORD dwViewId;
    DWORD dwNumColumnIds;
    WORD rgwColumnIds[XUSER_STATS_ATTRS_IN_SPEC];
} XUSER_STATS_SPEC, *PXUSER_STATS_SPEC;

XBOXAPI DWORD WINAPI XUserReadStats(IN DWORD dwTitleId, IN DWORD dwNumXuids, IN CONST XUID *pXuids,
                                    IN DWORD dwNumStatsSpecs, IN CONST XUSER_STATS_SPEC *pSpecs, IN OUT PDWORD pcbResults,
                                    OUT PXUSER_STATS_READ_RESULTS pResults, IN OUT PXOVERLAPPED pOverlapped OPTIONAL);

XBOXAPI DWORD WINAPI XEnableGuestSignin(IN BOOL fEnable);
XBOXAPI DWORD WINAPI XShowSigninUI(IN DWORD cPanes, IN DWORD dwFlags);
XBOXAPI DWORD WINAPI XShowFriendsUI(IN DWORD dwUserIndex);
XBOXAPI DWORD WINAPI XShowMessageComposeUI(IN DWORD dwUserIndex, IN CONST XUID *pXuidRecipients OPTIONAL, IN DWORD cRecipients,
                                           IN LPCWSTR wszText OPTIONAL);

#define XMSG_MAX_CUSTOMACTION_TRANSLATIONS 11

typedef struct {
    DWORD dwActionId;
    WCHAR wszEnActionText[23];
    WORD wReserved;
    DWORD dwFlags;
    struct {
        DWORD dwLanguageId;
        WCHAR wszActionText[23];
        WORD wReserved;
    } rgTranslations[XMSG_MAX_CUSTOMACTION_TRANSLATIONS];
} XMSG_CUSTOMACTION;

XBOXAPI DWORD WINAPI XShowCustomMessageComposeUI(IN DWORD dwUserIndex, IN CONST XUID *pXuidRecipients OPTIONAL,
                                                 IN DWORD cRecipients, IN DWORD dwFlags, IN LPCWSTR wszTitle,
                                                 IN LPCWSTR wszGameMessage, IN LPCWSTR wszPlayerMessage OPTIONAL,
                                                 IN CONST BYTE *pbImage OPTIONAL, IN DWORD cbImage,
                                                 IN CONST XMSG_CUSTOMACTION *pCustomActions, IN DWORD cCustomActions,
                                                 IN CONST BYTE *pbCustomPayload OPTIONAL, IN DWORD cbCustomPayload,
                                                 IN DWORD dwExpireMinutes OPTIONAL, IN OUT XOVERLAPPED *pOverlapped OPTIONAL);
XBOXAPI DWORD WINAPI XShowFriendRequestUI(IN DWORD dwUserIndex, IN XUID xuidUser);
XBOXAPI DWORD WINAPI XShowKeyboardUI(IN DWORD dwUserIndex, IN DWORD dwFlags, IN LPCWSTR wseDefaultText, IN LPCWSTR wszTitleText,
                                     IN LPCWSTR wszDescriptionText, OUT LPWSTR wszResultText, IN DWORD cchResultText,
                                     IN OUT PXOVERLAPPED pOverlapped OPTIONAL);
XBOXAPI DWORD WINAPI XShowGamerCardUI(IN DWORD dwUserIndex, IN XUID XuidPlayer);
XBOXAPI DWORD WINAPI XShowMarketplaceUI(IN DWORD dwUserIndex, IN DWORD dwEntryPoint, IN ULONGLONG qwOfferID OPTIONAL,
                                        IN DWORD dwContentCategories);
XBOXAPI DWORD WINAPI XShowDeviceSelectorUI(IN DWORD dwUserIndex, IN DWORD dwContentType, IN DWORD dwContentFlags,
                                           IN ULARGE_INTEGER uliBytesRequested, OUT PXCONTENTDEVICEID pDeviceID,
                                           IN OUT PXOVERLAPPED pOverlapped OPTIONAL);
// Never returns: the dashboard takes over.
DECLSPEC_NORETURN XBOXAPI VOID WINAPI XShowDirtyDiscErrorUI(IN DWORD dwUserIndex);
XBOXAPI DWORD WINAPI XShowPlayerReviewUI(IN DWORD dwUserIndex, IN XUID XuidFeedbackTarget);

typedef struct {
    WORD wActionId;
    WCHAR wszActionText[23];
    DWORD dwFlags;
} XCUSTOMACTION;

XBOXAPI VOID WINAPI XCustomRegisterDynamicActions(VOID);
XBOXAPI VOID WINAPI XCustomUnregisterDynamicActions(VOID);
XBOXAPI BOOL WINAPI XCustomGetCurrentGamercard(OUT DWORD *pdwUserIndex, OUT XUID *pXuid);
XBOXAPI DWORD WINAPI XCustomSetDynamicActions(DWORD dwUserIndex, XUID xuid, CONST XCUSTOMACTION *pCustomActions,
                                              WORD cCustomActions);
XBOXAPI DWORD WINAPI XCustomGetLastActionPressEx(OUT DWORD *pdwUserIndex, OUT DWORD *pdwActionId, OUT XUID *pXuid,
                                                 OUT BYTE *pbPayload OPTIONAL, IN OUT WORD *pcbPayload OPTIONAL);

XBOXAPI DWORD WINAPI XEnableScreenSaver(IN BOOL fEnable);

typedef struct _XHASH_STATE {
    BYTE Internal[88];
} XHASH_STATE, *PXHASH_STATE;

#define XHASHMEMORY_DIGEST_SIZE (20)

XBOXAPI VOID WINAPI XHashMemoryBegin(OUT XHASH_STATE *pState);
XBOXAPI VOID WINAPI XHashMemoryUpdate(IN OUT XHASH_STATE *pState, IN CONST VOID *pvMemory, IN DWORD dwSize);
XBOXAPI VOID WINAPI XHashMemoryEnd(IN CONST XHASH_STATE *pState, OUT BYTE *pbDigest);

#ifdef __cplusplus
}
#endif

#endif // _XBOX_H_
