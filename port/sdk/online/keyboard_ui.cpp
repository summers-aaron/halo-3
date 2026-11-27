// port/sdk/online (both hosts): XShowKeyboardUI, the guide's virtual keyboard. The game opens it to name a Service
// Tag, a custom game variant, a Forge map or a saved film, and reads the text back when the XOVERLAPPED completes.
// Built like the game, against the XDK headers, so the signature is the XDK's.
//
// As on the console the call returns ERROR_IO_PENDING at once and the dialog completes the XOVERLAPPED when it closes:
// ERROR_SUCCESS with the text entered in wszResultText (NUL-terminated, at most cchResultText - 1 UTF-16 units), or
// ERROR_CANCELLED with the default text left there. The guide shows one dialog at a time (a second call while one
// shows fails with ERROR_ACCESS_DENIED), and listeners hear XN_SYS_UI (1) when it opens and XN_SYS_UI (0) when it
// closes, before the XOVERLAPPED completes (port/sdk/xam/xam_ui.h). Where the text comes from:
//   - HALO3_KBD_TEXT set (scripted and headless runs): that text (UTF-8), accepted at once;
//   - a game window open (HALO3_WINDOW=1): a host dialog (keyboard_ui_host.h) over it, on a thread of its own, with
//     the title, the description and the default text; Enter accepts, Escape cancels;
//   - no window (HALO3_WINDOW=0), or a host that cannot show the dialog: accepted at once with the default text, or
//     cancelled when there is none, so an unattended run never waits on a dialog nobody can see.
// Without an XOVERLAPPED the call waits for the dialog and returns its result.
#include <xtl.h>

#include "../kernel/xapi_io.h"
#include "../runtime/port_runtime.h"
#include "../xam/xam_ui.h"
#include "keyboard_ui_host.h"

namespace {

// XShowKeyboardUI dwFlags (the XDK's VKBD_ values) the dialog follows; the layout choices (VKBD_LATIN_FULL,
// VKBD_LATIN_GAMERTAG, ...) pick the console's on-screen keys and mean nothing to a host keyboard.
const DWORD k_vkbd_latin_password = 0x00000080;
const DWORD k_vkbd_highlight_text = 0x10000000;

const unsigned long k_text_max = KEYBOARD_UI_HOST_TEXT_MAX;
typedef unsigned short utf16;

bool is_high_surrogate(utf16 unit) { return unit >= 0xd800 && unit <= 0xdbff; }

// The UTF-16 length of `text` cut to at most `max` units, never between the two halves of a surrogate pair.
unsigned long bounded_length(const utf16 *text, unsigned long max)
{
    unsigned long length = 0;
    while (text != 0 && length < max && text[length] != 0)
        length++;
    if (length > 0 && length == max && is_high_surrogate(text[length - 1]) && text[length] != 0)
        length--;
    return length;
}

// Copies `from` (null: empty) cut to `max` units (bounded_length) into `to`, NUL-terminated (to has max + 1 units).
void copy_text(utf16 *to, const utf16 *from, unsigned long max)
{
    const unsigned long length = bounded_length(from, max);
    for (unsigned long i = 0; i < length; i++)
        to[i] = from[i];
    to[length] = 0;
}

// UTF-8 into UTF-16 (to has max + 1 units, NUL-terminated): a malformed sequence becomes U+FFFD, one the text ends
// inside is left out; a
// character that does not fit whole is left out, with everything after it.
void utf8_to_utf16(const char *from, utf16 *to, unsigned long max)
{
    const unsigned char *s = (const unsigned char *)from;
    unsigned long length = 0;
    while (*s != 0) {
        unsigned long code = 0xfffd;
        unsigned int extra = *s >= 0xf0 && *s <= 0xf4 ? 3 : *s >= 0xe0 ? 2 : *s >= 0xc2 && *s < 0xe0 ? 1 : 0;
        if (*s < 0x80) {
            code = *s++;
        } else if (extra == 0) {
            s++; // a continuation byte, 0xc0, 0xc1 or 0xf5..0xff on its own
        } else {
            unsigned long value = *s++ & (0x3fu >> extra);
            unsigned int n = 0;
            while (n < extra && (*s & 0xc0) == 0x80) {
                value = value << 6 | (*s++ & 0x3fu);
                n++;
            }
            if (n < extra && *s == 0)
                break; // the text ends inside the character (cut short): left out
            const unsigned long least = extra == 1 ? 0x80 : extra == 2 ? 0x800 : 0x10000;
            if (n == extra && value >= least && value <= 0x10ffff && (value < 0xd800 || value > 0xdfff))
                code = value;
        }
        const unsigned long units = code >= 0x10000 ? 2 : 1;
        if (length + units > max)
            break;
        if (units == 2) {
            to[length++] = (utf16)(0xd800 + ((code - 0x10000) >> 10));
            to[length++] = (utf16)(0xdc00 + ((code - 0x10000) & 0x3ff));
        } else {
            to[length++] = (utf16)code;
        }
    }
    to[length] = 0;
}

// UTF-16 into UTF-8 for the log (to has `capacity` bytes, NUL-terminated); a lone surrogate becomes U+FFFD.
void utf16_to_utf8(const utf16 *from, char *to, unsigned long capacity)
{
    unsigned long length = 0;
    for (unsigned long i = 0; from[i] != 0; i++) {
        unsigned long code = from[i];
        if (is_high_surrogate(from[i]) && from[i + 1] >= 0xdc00 && from[i + 1] <= 0xdfff) {
            code = 0x10000 + ((code - 0xd800) << 10) + (from[i + 1] - 0xdc00);
            i++;
        } else if (code >= 0xd800 && code <= 0xdfff) {
            code = 0xfffd;
        }
        char bytes[4];
        unsigned long n;
        if (code < 0x80) {
            bytes[0] = (char)code;
            n = 1;
        } else if (code < 0x800) {
            bytes[0] = (char)(0xc0 | code >> 6);
            bytes[1] = (char)(0x80 | (code & 0x3f));
            n = 2;
        } else if (code < 0x10000) {
            bytes[0] = (char)(0xe0 | code >> 12);
            bytes[1] = (char)(0x80 | (code >> 6 & 0x3f));
            bytes[2] = (char)(0x80 | (code & 0x3f));
            n = 3;
        } else {
            bytes[0] = (char)(0xf0 | code >> 18);
            bytes[1] = (char)(0x80 | (code >> 12 & 0x3f));
            bytes[2] = (char)(0x80 | (code >> 6 & 0x3f));
            bytes[3] = (char)(0x80 | (code & 0x3f));
            n = 4;
        }
        if (length + n + 1 > capacity)
            break;
        for (unsigned long k = 0; k < n; k++)
            to[length++] = bytes[k];
    }
    to[length] = 0;
}

void append(char *line, unsigned long capacity, const char *text)
{
    unsigned long length = 0;
    while (line[length] != 0)
        length++;
    while (*text != 0 && length + 1 < capacity)
        line[length++] = *text++;
    line[length] = 0;
}

// The one request the guide can have open (port_xam_ui_open admits one at a time).
struct s_keyboard_request {
    keyboard_ui_host_request host;
    PXOVERLAPPED overlapped;
    LPWSTR result;
    DWORD result_capacity; // cchResultText
    DWORD thread;          // the caller's thread, which runs the completion routine
};
s_keyboard_request g_request;

// "port: XShowKeyboardUI: "<title>" <what>[ "<text>"]" (a password shows as stars).
void log_request(const char *what, const utf16 *text)
{
    char line[3 * 256 + 128] = "port: XShowKeyboardUI: \"";
    char utf8[2 * 256];
    utf16 title[64 + 1];
    copy_text(title, g_request.host.title, 64);
    utf16_to_utf8(title, utf8, sizeof(utf8));
    append(line, sizeof(line), utf8);
    append(line, sizeof(line), "\" ");
    append(line, sizeof(line), what);
    if (text != 0) {
        utf16 shown[128 + 1];
        copy_text(shown, text, 128);
        if (g_request.host.password)
            for (unsigned long i = 0; shown[i] != 0; i++)
                shown[i] = '*';
        utf16_to_utf8(shown, utf8, sizeof(utf8));
        append(line, sizeof(line), " \"");
        append(line, sizeof(line), utf8);
        append(line, sizeof(line), "\"");
    }
    append(line, sizeof(line), "\n");
    port_write_error(line);
}

// The dialog closed: the result text into the game's buffer, XN_SYS_UI (0), then the XOVERLAPPED (InternalLow last,
// which is what XHasOverlappedIoCompleted watches), its event and its completion routine. Any thread.
void finish(bool accepted)
{
    const DWORD result = accepted ? ERROR_SUCCESS : ERROR_CANCELLED;
    log_request(accepted ? "accepted" : "cancelled", accepted ? g_request.host.text : 0);
    copy_text((utf16 *)g_request.result, accepted ? g_request.host.text : g_request.host.default_text,
        g_request.result_capacity - 1);
    PXOVERLAPPED overlapped = g_request.overlapped;
    const DWORD thread = g_request.thread;
    port_xam_ui_close(); // from here a new request may reuse g_request
    // Once InternalLow is stored the game may recycle the XOVERLAPPED (its task slot is cleared and reused, or a
    // waiting call returns and its stack goes): everything still needed from it is read first.
    HANDLE event = overlapped->hEvent;
    PXOVERLAPPED_COMPLETION_ROUTINE routine = overlapped->pCompletionRoutine;
    overlapped->InternalHigh = 0;
    overlapped->dwExtendedError = accepted ? (DWORD)S_OK : (DWORD)HRESULT_FROM_WIN32(result);
    __atomic_store_n(&overlapped->InternalLow, (ULONG_PTR)result, __ATOMIC_RELEASE);
    if (event != 0)
        SetEvent(event);
    if (routine != 0 && !port_apc_queue_thread(thread, (void *)routine, result, 0, overlapped))
        port_write_error("port: XShowKeyboardUI: the completion routine queue is full; the routine is dropped\n");
}

void host_done(keyboard_ui_host_request *request, int accepted)
{
    (void)request;
    finish(accepted != 0);
}

} // namespace

extern "C" DWORD XShowKeyboardUI(DWORD dwUserIndex, DWORD dwFlags, LPCWSTR wseDefaultText, LPCWSTR wszTitleText,
    LPCWSTR wszDescriptionText, LPWSTR wszResultText, DWORD cchResultText, PXOVERLAPPED pOverlapped)
{
    if (wszResultText == 0 || cchResultText == 0 || (dwUserIndex >= XUSER_MAX_COUNT && dwUserIndex != XUSER_INDEX_ANY))
        return ERROR_INVALID_PARAMETER;
    if (!port_xam_ui_open())
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

    keyboard_ui_host_request *host = &g_request.host;
    g_request.overlapped = overlapped;
    g_request.result = wszResultText;
    g_request.result_capacity = cchResultText;
    g_request.thread = GetCurrentThreadId();
    host->max_length = cchResultText - 1 < k_text_max ? cchResultText - 1 : k_text_max;
    copy_text(host->title, (const utf16 *)wszTitleText, k_text_max);
    copy_text(host->description, (const utf16 *)wszDescriptionText, k_text_max);
    copy_text(host->default_text, (const utf16 *)wseDefaultText, host->max_length);
    host->password = (dwFlags & k_vkbd_latin_password) != 0;
    host->highlight = (dwFlags & k_vkbd_highlight_text) != 0;
    host->text[0] = 0;
    host->done = host_done;

    static char scripted[4 * KEYBOARD_UI_HOST_TEXT_MAX + 1];
    if (keyboard_ui_host_scripted_text(scripted, sizeof(scripted))) {
        utf8_to_utf16(scripted, host->text, host->max_length);
        log_request("shown (HALO3_KBD_TEXT)", 0);
        finish(true);
    } else {
        // logged first: once the dialog's thread runs, the request is its until it closes
        log_request("shown", host->default_text);
        if (!keyboard_ui_host_show(host)) {
            // nobody can see a dialog: the player takes the default text, or backs out when there is none
            copy_text(host->text, host->default_text, host->max_length);
            finish(host->text[0] != 0);
        }
    }

    if (pOverlapped != 0)
        return ERROR_IO_PENDING;
    while (!XHasOverlappedIoCompleted(&wait))
        Sleep(10);
    return (DWORD)wait.InternalLow;
}
