// port/sdk/xam (both hosts): the clipboard (clipboard.h): the port's own copy, and the host's clipboard when there is
// one.
#include "clipboard.h"

static struct {
    volatile char busy;
    char text[PORT_CLIPBOARD_MAX];
    int set;
} g_clipboard;

static void clipboard_lock(void)
{
    while (__atomic_test_and_set(&g_clipboard.busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void clipboard_unlock(void) { __atomic_clear(&g_clipboard.busy, __ATOMIC_RELEASE); }

// `text` into `to` (capacity bytes, NUL included), never cutting a UTF-8 sequence.
static void clipboard_copy(char *to, unsigned long capacity, const char *text)
{
    unsigned long n = 0;
    for (; *text != 0 && n + 1 < capacity; text++) {
        const unsigned char c = (unsigned char)*text;
        const unsigned long size = c < 0x80 ? 1 : c >= 0xf0 ? 4 : c >= 0xe0 ? 3 : c >= 0xc0 ? 2 : 1;
        if (n + size >= capacity)
            break;
        to[n++] = *text;
        for (unsigned long k = 1; k < size && text[1] != 0; k++)
            to[n++] = *++text;
    }
    to[n] = 0;
}

int port_clipboard_set(const char *text)
{
    if (text == 0)
        text = "";
    clipboard_lock();
    clipboard_copy(g_clipboard.text, sizeof(g_clipboard.text), text);
    g_clipboard.set = 1;
    clipboard_unlock();
    return port_clipboard_host_set(text);
}

int port_clipboard_get(char *text, unsigned long capacity)
{
    if (text == 0 || capacity == 0)
        return 0;
    text[0] = 0;
    char host[PORT_CLIPBOARD_MAX];
    if (port_clipboard_host_get(host, sizeof(host))) {
        clipboard_copy(text, capacity, host);
        return 1;
    }
    clipboard_lock();
    const int set = g_clipboard.set;
    if (set)
        clipboard_copy(text, capacity, g_clipboard.text);
    clipboard_unlock();
    return set;
}
