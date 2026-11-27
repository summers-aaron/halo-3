// port/sdk/xam (Windows host): the host's clipboard (clipboard.h) as the Win32 clipboard's CF_UNICODETEXT, through
// user32.dll and kernel32.dll's global memory functions, found at run time (the port links only kernel32 and msvcrt:
// port/build/link-stage1.sh), as the guide's window (guide_windows.c).
//
// Windows keeps what is copied itself, so copying is one SetClipboardData. The clipboard is opened for the game window
// (port_window.h): opened for no window, EmptyClipboard leaves it without an owner and SetClipboardData fails. Without
// a game window (a headless run) there is no host clipboard: clipboard.c keeps the port's own. Another program may
// hold the clipboard open for a moment, so opening it is tried a few times.
#include "clipboard.h"
#include "../kernel/win32_host.h"
#include "../runtime/port_window.h"

PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
PORT_HOST_IMPORT(void *, GetModuleHandleA, 4, (const char *name));

#define W_CF_UNICODETEXT 13u
#define W_GMEM_MOVEABLE 0x0002u
#define CLIPBOARD_OPEN_TRIES 10
#define CLIPBOARD_OPEN_RETRY_MILLISECONDS 20ul

typedef unsigned short utf16;

static struct {
    volatile long state; // 0 not loaded, 1 loading, 2 loaded, 3 unavailable
    int(__stdcall *OpenClipboard)(void *window);
    int(__stdcall *CloseClipboard)(void);
    int(__stdcall *EmptyClipboard)(void);
    void *(__stdcall *SetClipboardData)(unsigned int format, void *memory);
    void *(__stdcall *GetClipboardData)(unsigned int format);
    int(__stdcall *IsClipboardFormatAvailable)(unsigned int format);
    void *(__stdcall *GlobalAlloc)(unsigned int flags, __SIZE_TYPE__ bytes);
    void *(__stdcall *GlobalLock)(void *memory);
    int(__stdcall *GlobalUnlock)(void *memory);
    void *(__stdcall *GlobalFree)(void *memory);
    __SIZE_TYPE__(__stdcall *GlobalSize)(void *memory);
    void(__stdcall *Sleep)(unsigned long milliseconds);
} g_wc;

static int clipboard_w_load(void)
{
    long expected = 0;
    if (__atomic_compare_exchange_n(&g_wc.state, &expected, 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        void *user32 = host_LoadLibraryA("user32.dll"), *kernel32 = host_GetModuleHandleA("kernel32.dll");
        int loaded = user32 != 0 && kernel32 != 0;
#define LOAD(module, name)                                                                                          \
    do {                                                                                                            \
        if (loaded) {                                                                                               \
            *(void **)&g_wc.name = host_GetProcAddress(module, #name);                                              \
            loaded = g_wc.name != 0;                                                                                \
        }                                                                                                           \
    } while (0)
        LOAD(user32, OpenClipboard);
        LOAD(user32, CloseClipboard);
        LOAD(user32, EmptyClipboard);
        LOAD(user32, SetClipboardData);
        LOAD(user32, GetClipboardData);
        LOAD(user32, IsClipboardFormatAvailable);
        LOAD(kernel32, GlobalAlloc);
        LOAD(kernel32, GlobalLock);
        LOAD(kernel32, GlobalUnlock);
        LOAD(kernel32, GlobalFree);
        LOAD(kernel32, GlobalSize);
        LOAD(kernel32, Sleep);
#undef LOAD
        __atomic_store_n(&g_wc.state, loaded ? 2 : 3, __ATOMIC_RELEASE);
    }
    while (__atomic_load_n(&g_wc.state, __ATOMIC_ACQUIRE) == 1)
        __builtin_ia32_pause();
    return __atomic_load_n(&g_wc.state, __ATOMIC_ACQUIRE) == 2;
}

static int clipboard_open(void *window)
{
    for (int i = 0; i < CLIPBOARD_OPEN_TRIES; i++) {
        if (g_wc.OpenClipboard(window))
            return 1;
        g_wc.Sleep(CLIPBOARD_OPEN_RETRY_MILLISECONDS);
    }
    return 0;
}

// UTF-8 `text` as UTF-16 into `out` (capacity units, NUL included). Returns the units written, NUL included.
static unsigned long utf8_to_utf16(const char *text, utf16 *out, unsigned long capacity)
{
    unsigned long n = 0;
    const unsigned char *c = (const unsigned char *)text;
    while (*c != 0 && n + 2 < capacity) {
        unsigned long code = *c++;
        if (code >= 0xc0) {
            int extra = code >= 0xf0 ? 3 : code >= 0xe0 ? 2 : 1;
            code &= extra == 3 ? 0x07 : extra == 2 ? 0x0f : 0x1f;
            for (; extra > 0 && (*c & 0xc0) == 0x80; extra--)
                code = (code << 6) | (*c++ & 0x3f);
        } else if (code >= 0x80) {
            code = '?';
        }
        if (code >= 0x10000 && code < 0x110000) {
            code -= 0x10000;
            out[n++] = (utf16)(0xd800 | (code >> 10));
            out[n++] = (utf16)(0xdc00 | (code & 0x3ff));
        } else {
            out[n++] = (utf16)(code < 0x110000 ? code : '?');
        }
    }
    out[n++] = 0;
    return n;
}

// UTF-16 `text` (at most `length` units) as UTF-8 into `out` (capacity bytes, NUL included), never cutting a character.
static void utf16_to_utf8(const utf16 *text, unsigned long length, char *out, unsigned long capacity)
{
    unsigned long n = 0;
    for (unsigned long i = 0; i < length && text[i] != 0; i++) {
        unsigned long code = text[i];
        if (code >= 0xd800 && code < 0xdc00 && i + 1 < length && text[i + 1] >= 0xdc00 && text[i + 1] < 0xe000)
            code = 0x10000 + ((code - 0xd800) << 10) + (text[++i] - 0xdc00);
        else if (code >= 0xd800 && code < 0xe000)
            code = '?';
        const unsigned long size = code < 0x80 ? 1 : code < 0x800 ? 2 : code < 0x10000 ? 3 : 4;
        if (n + size >= capacity)
            break;
        if (size == 1) {
            out[n++] = (char)code;
        } else {
            out[n++] = (char)((size == 2 ? 0xc0 : size == 3 ? 0xe0 : 0xf0) | (code >> (6 * (size - 1))));
            for (unsigned long k = size - 1; k > 0; k--)
                out[n++] = (char)(0x80 | ((code >> (6 * (k - 1))) & 0x3f));
        }
    }
    out[n] = 0;
}

int port_clipboard_host_set(const char *text)
{
    void *window = (void *)port_window_native_window();
    if (window == 0 || !clipboard_w_load())
        return 0;
    // the text converted straight into the memory the clipboard will own (no buffer shared between threads)
    void *memory = g_wc.GlobalAlloc(W_GMEM_MOVEABLE, PORT_CLIPBOARD_MAX * sizeof(utf16));
    if (memory == 0)
        return 0;
    utf16 *to = (utf16 *)g_wc.GlobalLock(memory);
    if (to == 0) {
        g_wc.GlobalFree(memory);
        return 0;
    }
    utf8_to_utf16(text, to, PORT_CLIPBOARD_MAX);
    g_wc.GlobalUnlock(memory);
    if (!clipboard_open(window)) {
        g_wc.GlobalFree(memory);
        return 0;
    }
    int result = 0;
    if (g_wc.EmptyClipboard() && g_wc.SetClipboardData(W_CF_UNICODETEXT, memory) != 0)
        result = 1; // the clipboard owns the memory now
    else
        g_wc.GlobalFree(memory);
    g_wc.CloseClipboard();
    return result;
}

int port_clipboard_host_get(char *text, unsigned long capacity)
{
    if (!clipboard_w_load() || !g_wc.IsClipboardFormatAvailable(W_CF_UNICODETEXT))
        return 0;
    if (!clipboard_open((void *)port_window_native_window()))
        return 0;
    int result = 0;
    void *memory = g_wc.GetClipboardData(W_CF_UNICODETEXT);
    const utf16 *wide = memory != 0 ? (const utf16 *)g_wc.GlobalLock(memory) : 0;
    if (wide != 0) {
        utf16_to_utf8(wide, g_wc.GlobalSize(memory) / sizeof(utf16), text, capacity);
        g_wc.GlobalUnlock(memory);
        result = 1;
    }
    g_wc.CloseClipboard();
    return result;
}
