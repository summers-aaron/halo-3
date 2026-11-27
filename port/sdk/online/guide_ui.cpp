// port/sdk/online (both hosts): the XShow* calls of the guide (port/sdk/xam/guide.h), with the XDK's signatures (built
// like the game, against the SDK headers): the sign-in panes, message boxes, the device selector, the Guide's menu and
// its pages (achievements, friends, recent players, messages, message compose, game invite, friend request, gamercard,
// player review). The marketplace and the dirty-disc screen are port/sdk/online/xlive_offline.cpp's, the virtual
// keyboard is keyboard_ui.cpp's.
//
// Each opens the guide for the user it names (XUSER_INDEX_ANY: the controller used last) and returns at once, as XAM's
// do: the guide's pages return ERROR_SUCCESS; a message box or a device selector that shows completes its XOVERLAPPED
// when the player answers (without one the call waits for the answer). The guide shows one dialog at a time: a call
// while one shows fails with ERROR_ACCESS_DENIED. A user who is not signed in has no guide pages
// (ERROR_NOT_LOGGED_ON); a guest has no gamercard (ERROR_INVALID_PARAMETER). When nobody operates the guide (guide.h,
// HALO3_GUIDE) a message box takes its focus button, a device selector the hard drive, and a page opens and closes.
#include <xtl.h>

#include "../kernel/content.h"
#include "../kernel/xapi_io.h"
#include "../runtime/port_runtime.h"
#include "../xam/guide.h"
#include "../xam/xam_profile.h"
#include "../xam/xam_ui.h"

// The message box's flags and result (xbox.h; the clean-room headers declare only what the game calls, which is not
// this, and the XDK's declare it).
#ifndef XMB_MAXBUTTONS
#define XMB_NOICON 0x00000000
#define XMB_ERRORICON 0x00000001
#define XMB_WARNINGICON 0x00000002
#define XMB_ALERTICON 0x00000003
#define XMB_PASSCODEMODE 0x00010000
#define XMB_VERIFYPASSCODEMODE 0x00020000
#define XMB_MAXBUTTONS 3
#define XMB_CANCELID -1
typedef struct _MESSAGEBOX_RESULT {
    union {
        DWORD dwButtonPressed;
        WORD rgwPasscode[4];
    };
} MESSAGEBOX_RESULT, *PMESSAGEBOX_RESULT;
#endif
#ifndef XCONTENTFLAG_FORCE_SHOW_UI
#define XCONTENTFLAG_FORCE_SHOW_UI 0x00000200
#endif

namespace {

const DWORD k_xuser_index_any = 0xff;
const DWORD k_error_no_such_user = 1317;

typedef unsigned short utf16;

// The user a call names: 0..3, or the controller used last for XUSER_INDEX_ANY; XUSER_MAX_COUNT for neither.
DWORD call_user(DWORD user_index)
{
    if (user_index == k_xuser_index_any)
        return port_xam_last_used_controller();
    return user_index < XUSER_MAX_COUNT ? user_index : XUSER_MAX_COUNT;
}

// UTF-16 (null: empty) into UTF-8 (`capacity` bytes, NUL-terminated); a character that does not fit is left out with
// everything after it, a lone surrogate becomes U+FFFD.
void utf16_to_utf8(const utf16 *from, char *to, unsigned long capacity)
{
    unsigned long length = 0;
    for (unsigned long i = 0; from != 0 && from[i] != 0; i++) {
        unsigned long code = from[i];
        if (code >= 0xd800 && code <= 0xdbff && from[i + 1] >= 0xdc00 && from[i + 1] <= 0xdfff) {
            code = 0x10000 + ((code - 0xd800) << 10) + (from[i + 1] - 0xdc00);
            i++;
        } else if (code >= 0xd800 && code <= 0xdfff) {
            code = 0xfffd;
        }
        const unsigned long n = code < 0x80 ? 1 : code < 0x800 ? 2 : code < 0x10000 ? 3 : 4;
        if (length + n + 1 > capacity)
            break;
        static const unsigned char lead[5] = { 0, 0x00, 0xc0, 0xe0, 0xf0 };
        to[length++] = (char)(lead[n] | code >> (6 * (n - 1)));
        for (unsigned long k = 1; k < n; k++)
            to[length++] = (char)(0x80 | (code >> (6 * (n - 1 - k)) & 0x3f));
    }
    to[length] = 0;
}

// What a pending message box or device selector completes once the guide answers. One per call in flight (the guide
// holds one request; a few records so that one finishing never races the next call's).
struct s_pending {
    volatile LONG used;
    PXOVERLAPPED overlapped;
    DWORD thread;                // the caller's thread, which runs the completion routine
    PMESSAGEBOX_RESULT result;   // a message box
    PXCONTENTDEVICEID device_id; // a device selector
};
s_pending g_pending[4];

s_pending *pending_claim()
{
    for (int i = 0; i < 4; i++)
        if (InterlockedCompareExchange(&g_pending[i].used, 1, 0) == 0)
            return &g_pending[i];
    return 0;
}

void pending_release(s_pending *pending) { InterlockedExchange(&pending->used, 0); }

// The XOVERLAPPED's completion: InternalHigh 0, dwExtendedError `extended`, InternalLow `result` (last: what
// XHasOverlappedIoCompleted watches; the title may recycle the XOVERLAPPED from then on, so what is still needed from
// it is read first), its event, and its completion routine queued to `thread`.
void complete_overlapped(PXOVERLAPPED overlapped, DWORD result, DWORD extended, DWORD thread)
{
    const HANDLE event = overlapped->hEvent;
    const PXOVERLAPPED_COMPLETION_ROUTINE routine = overlapped->pCompletionRoutine;
    overlapped->InternalHigh = 0;
    overlapped->dwExtendedError = extended;
    __atomic_store_n(&overlapped->InternalLow, (ULONG_PTR)result, __ATOMIC_RELEASE);
    if (event != 0)
        SetEvent(event);
    if (routine != 0 && !port_apc_queue_thread(thread, (void *)routine, result, 0, overlapped))
        port_write_error("port: guide: an XOVERLAPPED completion routine queue is full; the routine is dropped\n");
}

// A message box answered: the button (XMB_CANCELID for B), then ERROR_SUCCESS / ERROR_CANCELLED.
void message_box_done(guide_request *request, int result, unsigned long value)
{
    s_pending *pending = (s_pending *)request->context;
    const bool chosen = result == GUIDE_RESULT_CHOSEN;
    pending->result->dwButtonPressed = chosen ? (DWORD)value : (DWORD)XMB_CANCELID;
    PXOVERLAPPED overlapped = pending->overlapped;
    const DWORD thread = pending->thread;
    pending_release(pending);
    complete_overlapped(overlapped, chosen ? ERROR_SUCCESS : ERROR_CANCELLED,
        chosen ? (DWORD)S_OK : (DWORD)HRESULT_FROM_WIN32(ERROR_CANCELLED), thread);
}

// A device chosen; backing out is XAM's ERROR_FUNCTION_FAILED whose extended error is ERROR_CANCELLED itself (what the
// game's selector task tests for), the device id left alone.
void device_selector_done(guide_request *request, int result, unsigned long value)
{
    s_pending *pending = (s_pending *)request->context;
    PXOVERLAPPED overlapped = pending->overlapped;
    const DWORD thread = pending->thread;
    if (result == GUIDE_RESULT_CHOSEN)
        *pending->device_id = (XCONTENTDEVICEID)value;
    pending_release(pending);
    if (result == GUIDE_RESULT_CHOSEN)
        complete_overlapped(overlapped, ERROR_SUCCESS, S_OK, thread);
    else
        complete_overlapped(overlapped, ERROR_FUNCTION_FAILED, ERROR_CANCELLED, thread);
}

// Waits for an XOVERLAPPED of this thread's own (a call made without one), operating the guide by the controllers
// meanwhile: the title's thread that polls them may be this one.
DWORD wait_for(XOVERLAPPED *overlapped)
{
    while (!XHasOverlappedIoCompleted(overlapped)) {
        port_xam_guide_poll();
        Sleep(10);
    }
    return (DWORD)overlapped->InternalLow;
}

DWORD show_blade(DWORD user_index, int blade, XUID xuid)
{
    const DWORD user = call_user(user_index);
    if (user == XUSER_MAX_COUNT)
        return ERROR_INVALID_PARAMETER;
    if (!port_profile_signed_in(user))
        return ERROR_NOT_LOGGED_ON;
    guide_request request;
    memset(&request, 0, sizeof(request));
    request.kind = GUIDE_KIND_BLADE;
    request.user = user;
    request.blade = blade;
    request.xuid = xuid;
    return guide_open(&request);
}

} // namespace

extern "C" {

// ---- sign-in ----

// The sign-in panes (port/sdk/xam/xam.c builds the request: the controllers connected, the one used last).
DWORD XShowSigninUI(DWORD cPanes, DWORD dwFlags)
{
    return port_profile_show_signin(cPanes, dwFlags);
}

// Whether the sign-in panes offer guests whatever the title's XShowSigninUI flags say.
DWORD XEnableGuestSignin(BOOL fEnable)
{
    guide_enable_guest_signin(fEnable != FALSE);
    return ERROR_SUCCESS;
}

// ---- message box, device selector ----

DWORD XShowMessageBoxUI(DWORD dwUserIndex, LPCWSTR wszTitle, LPCWSTR wszText, DWORD cButtons, LPCWSTR *pwszButtons,
    DWORD dwFocusButton, DWORD dwFlags, PMESSAGEBOX_RESULT pResult, PXOVERLAPPED pOverlapped)
{
    const DWORD user = call_user(dwUserIndex);
    if (pResult == 0 || user == XUSER_MAX_COUNT || cButtons > XMB_MAXBUTTONS || (cButtons != 0 && pwszButtons == 0))
        return ERROR_INVALID_PARAMETER;
    if ((dwFlags & (XMB_PASSCODEMODE | XMB_VERIFYPASSCODEMODE)) != 0) {
        // a passcode is four controller buttons entered blind: the guide's pages have no such entry
        port_write_error("port: XShowMessageBoxUI: passcode mode is not supported\n");
        return ERROR_INVALID_PARAMETER;
    }
    s_pending *pending = pending_claim();
    if (pending == 0)
        return ERROR_ACCESS_DENIED;
    XOVERLAPPED wait; // no XOVERLAPPED: the call completes this one and waits for it
    PXOVERLAPPED overlapped = pOverlapped;
    if (overlapped == 0) {
        memset(&wait, 0, sizeof(wait));
        overlapped = &wait;
    }
    overlapped->InternalHigh = 0;
    overlapped->dwExtendedError = 0;
    overlapped->InternalLow = ERROR_IO_PENDING;
    pending->overlapped = overlapped;
    pending->thread = GetCurrentThreadId();
    pending->result = pResult;
    pending->device_id = 0;

    guide_request request;
    memset(&request, 0, sizeof(request));
    request.kind = GUIDE_KIND_MESSAGE_BOX;
    request.user = user;
    utf16_to_utf8((const utf16 *)wszTitle, request.title, sizeof(request.title));
    utf16_to_utf8((const utf16 *)wszText, request.text, sizeof(request.text));
    for (DWORD i = 0; i < cButtons; i++)
        utf16_to_utf8((const utf16 *)pwszButtons[i], request.buttons[i], sizeof(request.buttons[i]));
    request.button_count = cButtons;
    request.focus = dwFocusButton;
    request.icon = dwFlags & 0xf;
    request.done = message_box_done;
    request.context = pending;
    const DWORD result = guide_open(&request);
    if (result != ERROR_SUCCESS) {
        pending_release(pending);
        overlapped->InternalLow = result;
        return result;
    }
    if (pOverlapped != 0)
        return ERROR_IO_PENDING;
    return wait_for(&wait);
}

// The hard drive is the only device, and with one device the guide chooses it without asking, unless the title forces
// the selector (XCONTENTFLAG_FORCE_SHOW_UI: the game's "change storage device"). Its free space is not weighed against
// uliBytesRequested: the game builds that from big-endian halves (c_storage_device.cpp's
// online_guide_show_device_selector_ui), so on the PC it reads as the request times 2^32; a drive that fills up fails
// the writes themselves, which the game reports. With no drive at all (no data directory) the user can only back out
// of the guide: XAM completes that as ERROR_FUNCTION_FAILED whose extended error is ERROR_CANCELLED itself (what the
// game's selector task tests for), and leaves the device id alone.
DWORD XShowDeviceSelectorUI(DWORD dwUserIndex, DWORD dwContentType, DWORD dwContentFlags,
    ULARGE_INTEGER uliBytesRequested, PXCONTENTDEVICEID pDeviceID, PXOVERLAPPED pOverlapped)
{
    (void)dwContentType;
    (void)uliBytesRequested;
    const DWORD user = call_user(dwUserIndex);
    if (pDeviceID == 0 || pOverlapped == 0 || user == XUSER_MAX_COUNT)
        return ERROR_INVALID_PARAMETER;
    if (!port_profile_signed_in(user))
        return k_error_no_such_user;
    XDEVICE_DATA device;
    const bool attached = XContentGetDeviceData(PORT_CONTENT_DEVICE_HARD_DRIVE, &device) == ERROR_SUCCESS;
    if (!attached) {
        complete_overlapped(pOverlapped, ERROR_FUNCTION_FAILED, ERROR_CANCELLED, GetCurrentThreadId());
        return ERROR_IO_PENDING;
    }
    if ((dwContentFlags & XCONTENTFLAG_FORCE_SHOW_UI) == 0) {
        *pDeviceID = PORT_CONTENT_DEVICE_HARD_DRIVE;
        complete_overlapped(pOverlapped, ERROR_SUCCESS, S_OK, GetCurrentThreadId());
        return ERROR_IO_PENDING;
    }
    s_pending *pending = pending_claim();
    if (pending == 0)
        return ERROR_ACCESS_DENIED;
    pOverlapped->InternalHigh = 0;
    pOverlapped->dwExtendedError = 0;
    pOverlapped->InternalLow = ERROR_IO_PENDING;
    pending->overlapped = pOverlapped;
    pending->thread = GetCurrentThreadId();
    pending->result = 0;
    pending->device_id = pDeviceID;
    guide_request request;
    memset(&request, 0, sizeof(request));
    request.kind = GUIDE_KIND_DEVICE_SELECTOR;
    request.user = user;
    request.device_count = 1;
    request.devices[0].id = device.DeviceID;
    utf16_to_utf8((const utf16 *)device.wszFriendlyName, request.devices[0].name, sizeof(request.devices[0].name));
    if (request.devices[0].name[0] == 0)
        memcpy(request.devices[0].name, "Hard Drive", 11);
    memcpy(&request.devices[0].free_bytes, &device.ulDeviceFreeBytes, sizeof(request.devices[0].free_bytes));
    request.devices[0].selectable = 1;
    request.done = device_selector_done;
    request.context = pending;
    const DWORD result = guide_open(&request);
    if (result != ERROR_SUCCESS) {
        pending_release(pending);
        pOverlapped->InternalLow = result;
        return result;
    }
    return ERROR_IO_PENDING;
}

// ---- the guide and its pages ----

// XShowGuideUI (an XAM call later XDKs declare): the Guide's menu, as the Guide button opens it.
DWORD XShowGuideUI(DWORD dwUserIndex)
{
    const DWORD user = call_user(dwUserIndex);
    if (user == XUSER_MAX_COUNT)
        return ERROR_INVALID_PARAMETER;
    guide_request request;
    memset(&request, 0, sizeof(request));
    request.kind = GUIDE_KIND_MENU;
    request.user = user;
    return guide_open(&request);
}

DWORD XShowAchievementsUI(DWORD dwUserIndex)
{
    if (port_profile_user_is_guest(call_user(dwUserIndex)))
        return ERROR_INVALID_PARAMETER; // guests have no achievements
    return show_blade(dwUserIndex, GUIDE_BLADE_ACHIEVEMENTS, 0);
}

DWORD XShowFriendsUI(DWORD dwUserIndex) { return show_blade(dwUserIndex, GUIDE_BLADE_FRIENDS, 0); }
DWORD XShowPlayersUI(DWORD dwUserIndex) { return show_blade(dwUserIndex, GUIDE_BLADE_PLAYERS, 0); }
DWORD XShowMessagesUI(DWORD dwUserIndex) { return show_blade(dwUserIndex, GUIDE_BLADE_MESSAGES, 0); }

DWORD XShowMessageComposeUI(DWORD dwUserIndex, const XUID *pXuidRecipients, DWORD cRecipients, LPCWSTR wszText)
{
    (void)pXuidRecipients; (void)cRecipients; (void)wszText;
    return show_blade(dwUserIndex, GUIDE_BLADE_COMPOSE, 0);
}

// The custom message compose completes its XOVERLAPPED once the page is shown: nothing can be sent (no Xbox Live
// message service), which is the page's text.
DWORD XShowCustomMessageComposeUI(DWORD dwUserIndex, const XUID *pXuidRecipients, DWORD cRecipients, DWORD dwFlags,
    LPCWSTR wszTitle, LPCWSTR wszGameMessage, LPCWSTR wszPlayerMessage, const BYTE *pbImage, DWORD cbImage,
    const XMSG_CUSTOMACTION *pCustomActions, DWORD cCustomActions, const BYTE *pbCustomPayload, DWORD cbCustomPayload,
    DWORD dwExpireMinutes, XOVERLAPPED *pOverlapped)
{
    (void)pXuidRecipients; (void)cRecipients; (void)dwFlags; (void)wszTitle; (void)wszGameMessage;
    (void)wszPlayerMessage; (void)pbImage; (void)cbImage; (void)pCustomActions; (void)cCustomActions;
    (void)pbCustomPayload; (void)cbCustomPayload; (void)dwExpireMinutes;
    const DWORD result = show_blade(dwUserIndex, GUIDE_BLADE_COMPOSE, 0);
    if (pOverlapped == 0)
        return result;
    complete_overlapped(pOverlapped, result, result == ERROR_SUCCESS ? (DWORD)S_OK : (DWORD)HRESULT_FROM_WIN32(result),
        GetCurrentThreadId());
    return ERROR_IO_PENDING;
}

DWORD XShowGameInviteUI(DWORD dwUserIndex, const XUID *pXuidRecipients, DWORD cRecipients, LPCWSTR wszUnused)
{
    (void)pXuidRecipients; (void)cRecipients; (void)wszUnused;
    return show_blade(dwUserIndex, GUIDE_BLADE_INVITE, 0);
}

DWORD XShowFriendRequestUI(DWORD dwUserIndex, XUID xuidUser)
{
    return show_blade(dwUserIndex, GUIDE_BLADE_FRIEND_REQUEST, xuidUser);
}

// A guest's XUID (the high 16 bits 0x?9 with bits 6..7 set) has no gamercard.
DWORD XShowGamerCardUI(DWORD dwUserIndex, XUID XuidPlayer)
{
    const unsigned int high = (unsigned int)(XuidPlayer >> 48);
    if ((high & 0xf) == 9 && (high & 0xc0) != 0)
        return ERROR_INVALID_PARAMETER;
    return show_blade(dwUserIndex, GUIDE_BLADE_GAMERCARD, XuidPlayer);
}

DWORD XShowPlayerReviewUI(DWORD dwUserIndex, XUID XuidFeedbackTarget)
{
    return show_blade(dwUserIndex, GUIDE_BLADE_REVIEW, XuidFeedbackTarget);
}

} // extern "C"
