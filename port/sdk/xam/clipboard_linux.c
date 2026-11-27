// port/sdk/xam (Linux host): the host's clipboard (clipboard.h) as X11's CLIPBOARD selection, through Xlib opened at
// run time from libX11.so.6 (RTLD_LOCAL, as the game window and the guide's window: no toolkit). Under Wayland,
// XWayland bridges it to the compositor's clipboard.
//
// X11 has no clipboard store: the client that copied owns the selection and answers every paste itself. So a thread
// of the port's keeps a display connection and an unmapped window of its own (no Xlib state shared with the game
// window's connection or the guide window's) and, while it owns CLIPBOARD, answers SelectionRequest with the text
// (UTF8_STRING, STRING, TEXT, and TARGETS listing them; ICCCM 2.2), until another client takes the selection
// (SelectionClear). Copying takes ownership with a server timestamp (a zero-length property append, as ICCCM 2.1 asks,
// not CurrentTime); pasting converts CLIPBOARD to UTF8_STRING (STRING if the owner has no UTF-8) into a property of the
// window and reads it, answering from the thread's own copy when it is the owner. The calls hand their request to the
// thread and wait for it (at most PORT_CLIPBOARD_WAIT_MILLISECONDS). With no display (a headless run) there is no host
// clipboard: clipboard.c keeps the port's own.
#include "clipboard.h"
#include "../runtime/port_runtime.h"

#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include <unistd.h>

typedef unsigned long x_window;
typedef unsigned long x_atom;
typedef unsigned long x_time;
typedef struct x_display x_display;

#define X_NONE 0ul
#define X_CURRENT_TIME 0ul
#define X_ATOM_ATOM 4ul
#define X_ATOM_STRING 31ul
#define X_ANY_PROPERTY_TYPE 0ul
#define X_PROPERTY_CHANGE_MASK (1L << 22)
#define X_PROP_MODE_REPLACE 0
#define X_PROP_MODE_APPEND 2
#define X_PROPERTY_NOTIFY 28
#define X_SELECTION_CLEAR 29
#define X_SELECTION_REQUEST 30
#define X_SELECTION_NOTIFY 31

// XEvent (Xlib.h): a union of 24 longs. XPropertyEvent, XSelectionClearEvent, XSelectionRequestEvent, XSelectionEvent.
typedef union {
    int type;
    struct {
        int type;
        unsigned long serial;
        int send_event;
        x_display *display;
        x_window window;
        x_atom atom;
        x_time time;
        int state;
    } property;
    struct {
        int type;
        unsigned long serial;
        int send_event;
        x_display *display;
        x_window window;
        x_atom selection;
        x_time time;
    } clear;
    struct {
        int type;
        unsigned long serial;
        int send_event;
        x_display *display;
        x_window owner, requestor;
        x_atom selection, target, property;
        x_time time;
    } request;
    struct {
        int type;
        unsigned long serial;
        int send_event;
        x_display *display;
        x_window requestor;
        x_atom selection, target, property;
        x_time time;
    } selection;
    long pad[24];
} x_event;

typedef struct {
    int type;
    x_display *display;
    unsigned long resource_id, serial;
    unsigned char error_code, request_code, minor_code;
} x_error_event;
typedef int (*x_error_handler)(x_display *, x_error_event *);

static struct {
    x_display *(*XOpenDisplay)(const char *);
    x_error_handler (*XSetErrorHandler)(x_error_handler);
    int (*XConnectionNumber)(x_display *);
    int (*XDefaultScreen)(x_display *);
    x_window (*XRootWindow)(x_display *, int);
    x_window (*XCreateSimpleWindow)(x_display *, x_window, int, int, unsigned int, unsigned int, unsigned int,
                                    unsigned long, unsigned long);
    int (*XSelectInput)(x_display *, x_window, long);
    x_atom (*XInternAtom)(x_display *, const char *, int);
    int (*XChangeProperty)(x_display *, x_window, x_atom, x_atom, int, int, const unsigned char *, int);
    int (*XGetWindowProperty)(x_display *, x_window, x_atom, long, long, int, x_atom, x_atom *, int *,
                              unsigned long *, unsigned long *, unsigned char **);
    int (*XFree)(void *);
    int (*XSetSelectionOwner)(x_display *, x_atom, x_window, x_time);
    x_window (*XGetSelectionOwner)(x_display *, x_atom);
    int (*XConvertSelection)(x_display *, x_atom, x_atom, x_atom, x_window, x_time);
    int (*XSendEvent)(x_display *, x_window, int, long, x_event *);
    int (*XFlush)(x_display *);
    int (*XPending)(x_display *);
    int (*XNextEvent)(x_display *, x_event *);
} g_xc;

enum { k_clip_stopped, k_clip_running, k_clip_unavailable };

static struct {
    pthread_mutex_t lock;
    pthread_cond_t changed;
    int state;                 // k_clip_*
    int wake[2];               // a byte written: a request waits
    // requests (under lock)
    int set_request, set_done, set_result;
    char set_text[PORT_CLIPBOARD_MAX];
    int get_request, get_done, get_result;
    char get_text[PORT_CLIPBOARD_MAX];
    // the thread's own
    x_display *display;
    x_window window;
    x_atom clipboard, utf8, targets, text, incr, property, stamp;
    int owned;                 // the thread owns CLIPBOARD
    x_time owned_since;
    char owned_text[PORT_CLIPBOARD_MAX];
    int converting;            // a paste waits for SelectionNotify
    x_atom converting_target;
    struct timespec deadline;
} g_clip = { .lock = PTHREAD_MUTEX_INITIALIZER, .changed = PTHREAD_COND_INITIALIZER, .state = k_clip_stopped,
              .wake = { -1, -1 } };

// What XSetErrorHandler returned when the clipboard's was set: never 0 (Xlib gives its default handler when none was
// set), so another connection's error still reaches the game window's handler or Xlib's default. Shared by every
// thread that gets an X error, hence atomic.
static x_error_handler g_previous_error_handler;

// Protocol errors on the clipboard's connection (a requestor's window gone before its answer, say) are its own: logged
// and ignored, where Xlib's default handler would end the process; every other connection's go on unchanged.
static int clipboard_x_error(x_display *display, x_error_event *error)
{
    if (display == __atomic_load_n(&g_clip.display, __ATOMIC_ACQUIRE)) {
        (void)error;
        port_write_error("clipboard: X error on the clipboard's connection, ignored\n");
        return 0;
    }
    const x_error_handler previous = __atomic_load_n(&g_previous_error_handler, __ATOMIC_ACQUIRE);
    return previous != 0 ? previous(display, error) : 0; // (0 only in the instant before it is stored)
}

static int clipboard_x_load(void)
{
    void *library = dlopen("libX11.so.6", RTLD_NOW | RTLD_LOCAL);
    if (library == 0)
        return 0;
#define LOAD(name)                                                                                                  \
    do {                                                                                                            \
        *(void **)&g_xc.name = dlsym(library, #name);                                                               \
        if (g_xc.name == 0) {                                                                                       \
            dlclose(library);                                                                                       \
            return 0;                                                                                               \
        }                                                                                                           \
    } while (0)
    LOAD(XOpenDisplay);
    LOAD(XSetErrorHandler);
    LOAD(XConnectionNumber);
    LOAD(XDefaultScreen);
    LOAD(XRootWindow);
    LOAD(XCreateSimpleWindow);
    LOAD(XSelectInput);
    LOAD(XInternAtom);
    LOAD(XChangeProperty);
    LOAD(XGetWindowProperty);
    LOAD(XFree);
    LOAD(XSetSelectionOwner);
    LOAD(XGetSelectionOwner);
    LOAD(XConvertSelection);
    LOAD(XSendEvent);
    LOAD(XFlush);
    LOAD(XPending);
    LOAD(XNextEvent);
#undef LOAD
    return 1;
}

// ---- text ----

// UTF-8 `text` into `to` (capacity bytes, NUL included), never cutting a sequence.
static void utf8_copy(char *to, unsigned long capacity, const char *text, unsigned long length)
{
    unsigned long n = 0, i = 0;
    while (i < length && text[i] != 0 && n + 1 < capacity) {
        const unsigned char c = (unsigned char)text[i];
        const unsigned long size = c < 0x80 ? 1 : c >= 0xf0 ? 4 : c >= 0xe0 ? 3 : c >= 0xc0 ? 2 : 1;
        if (n + size >= capacity || i + size > length)
            break;
        for (unsigned long k = 0; k < size; k++)
            to[n++] = text[i++];
    }
    to[n] = 0;
}

// ISO Latin-1 (STRING) `text` as UTF-8.
static void latin1_to_utf8(char *to, unsigned long capacity, const unsigned char *text, unsigned long length)
{
    unsigned long n = 0;
    for (unsigned long i = 0; i < length && text[i] != 0; i++) {
        const unsigned char c = text[i];
        if (c < 0x80) {
            if (n + 2 > capacity)
                break;
            to[n++] = (char)c;
        } else {
            if (n + 3 > capacity)
                break;
            to[n++] = (char)(0xc0 | c >> 6);
            to[n++] = (char)(0x80 | (c & 0x3f));
        }
    }
    to[n] = 0;
}

// UTF-8 `text` as ISO Latin-1 (STRING), '?' for what it cannot hold. Returns the length.
static unsigned long utf8_to_latin1(unsigned char *to, unsigned long capacity, const char *text)
{
    unsigned long n = 0;
    const unsigned char *c = (const unsigned char *)text;
    while (*c != 0 && n + 1 < capacity) {
        unsigned long code = *c++;
        if (code >= 0xc0) {
            int extra = code >= 0xf0 ? 3 : code >= 0xe0 ? 2 : 1;
            code &= extra == 3 ? 0x07 : extra == 2 ? 0x0f : 0x1f;
            for (; extra > 0 && (*c & 0xc0) == 0x80; extra--)
                code = (code << 6) | (*c++ & 0x3f);
        }
        to[n++] = (unsigned char)(code < 0x100 ? code : '?');
    }
    to[n] = 0;
    return n;
}

// ---- the thread ----

static void deadline_after(struct timespec *at, unsigned long milliseconds)
{
    clock_gettime(CLOCK_MONOTONIC, at);
    at->tv_sec += (time_t)(milliseconds / 1000);
    at->tv_nsec += (long)(milliseconds % 1000) * 1000000L;
    if (at->tv_nsec >= 1000000000L) {
        at->tv_sec++;
        at->tv_nsec -= 1000000000L;
    }
}

static int deadline_passed(const struct timespec *at)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec > at->tv_sec || (now.tv_sec == at->tv_sec && now.tv_nsec >= at->tv_nsec);
}

static void get_finish(int result, const char *text)
{
    pthread_mutex_lock(&g_clip.lock);
    g_clip.get_result = result;
    g_clip.get_text[0] = 0;
    if (result)
        utf8_copy(g_clip.get_text, sizeof(g_clip.get_text), text, PORT_CLIPBOARD_MAX);
    g_clip.get_done = 1;
    pthread_cond_broadcast(&g_clip.changed);
    pthread_mutex_unlock(&g_clip.lock);
    g_clip.converting = 0;
}

// A paste's answer arrived (SelectionNotify on the thread's window).
static void selection_arrived(const x_event *event)
{
    if (!g_clip.converting || event->selection.selection != g_clip.clipboard)
        return;
    if (event->selection.property == X_NONE) {
        if (g_clip.converting_target == g_clip.utf8) { // the owner has no UTF-8: Latin-1
            g_clip.converting_target = X_ATOM_STRING;
            g_xc.XConvertSelection(g_clip.display, g_clip.clipboard, X_ATOM_STRING, g_clip.property, g_clip.window,
                                   X_CURRENT_TIME);
            g_xc.XFlush(g_clip.display);
            return;
        }
        get_finish(0, "");
        return;
    }
    x_atom type = X_NONE;
    int format = 0;
    unsigned long items = 0, after = 0;
    unsigned char *data = 0;
    const int status = g_xc.XGetWindowProperty(g_clip.display, g_clip.window, g_clip.property, 0,
                                               (long)(PORT_CLIPBOARD_MAX / 4), 1, X_ANY_PROPERTY_TYPE, &type, &format,
                                               &items, &after, &data);
    char text[PORT_CLIPBOARD_MAX];
    text[0] = 0;
    const int ok = status == 0 && data != 0 && format == 8 && type != g_clip.incr; // INCR: far longer than a code
    if (ok && type == X_ATOM_STRING)
        latin1_to_utf8(text, sizeof(text), data, items);
    else if (ok)
        utf8_copy(text, sizeof(text), (const char *)data, items);
    if (data != 0)
        g_xc.XFree(data);
    get_finish(ok, text);
}

// Another client asks for the selection the thread owns: the text, or the targets it has (ICCCM 2.2).
static void selection_requested(const x_event *event)
{
    x_event reply;
    memset(&reply, 0, sizeof(reply));
    reply.selection.type = X_SELECTION_NOTIFY;
    reply.selection.display = event->request.display;
    reply.selection.requestor = event->request.requestor;
    reply.selection.selection = event->request.selection;
    reply.selection.target = event->request.target;
    reply.selection.time = event->request.time;
    reply.selection.property = X_NONE;
    const x_atom property = event->request.property != X_NONE ? event->request.property : event->request.target;
    const int ours = g_clip.owned && event->request.selection == g_clip.clipboard &&
                     (event->request.time == X_CURRENT_TIME || event->request.time >= g_clip.owned_since);
    if (ours && event->request.target == g_clip.targets) {
        const x_atom targets[4] = { g_clip.targets, g_clip.utf8, X_ATOM_STRING, g_clip.text };
        g_xc.XChangeProperty(g_clip.display, event->request.requestor, property, X_ATOM_ATOM, 32, X_PROP_MODE_REPLACE,
                             (const unsigned char *)targets, 4);
        reply.selection.property = property;
    } else if (ours && (event->request.target == g_clip.utf8 || event->request.target == g_clip.text)) {
        g_xc.XChangeProperty(g_clip.display, event->request.requestor, property, g_clip.utf8, 8, X_PROP_MODE_REPLACE,
                             (const unsigned char *)g_clip.owned_text, (int)strlen(g_clip.owned_text));
        reply.selection.property = property;
    } else if (ours && event->request.target == X_ATOM_STRING) {
        unsigned char latin1[PORT_CLIPBOARD_MAX];
        const unsigned long length = utf8_to_latin1(latin1, sizeof(latin1), g_clip.owned_text);
        g_xc.XChangeProperty(g_clip.display, event->request.requestor, property, X_ATOM_STRING, 8, X_PROP_MODE_REPLACE,
                             latin1, (int)length);
        reply.selection.property = property;
    }
    g_xc.XSendEvent(g_clip.display, event->request.requestor, 0, 0, &reply);
    g_xc.XFlush(g_clip.display);
}

static void clipboard_event(const x_event *event)
{
    switch (event->type) {
    case X_SELECTION_REQUEST: selection_requested(event); break;
    case X_SELECTION_NOTIFY:
        if (event->selection.requestor == g_clip.window)
            selection_arrived(event);
        break;
    case X_SELECTION_CLEAR:
        if (event->clear.selection == g_clip.clipboard)
            g_clip.owned = 0; // another client copied: its text is the clipboard's now
        break;
    default: break;
    }
}

// A server timestamp: a zero-length append to a property of the thread's window, and its PropertyNotify's time.
static x_time server_time(void)
{
    g_xc.XChangeProperty(g_clip.display, g_clip.window, g_clip.stamp, X_ATOM_STRING, 8, X_PROP_MODE_APPEND,
                         (const unsigned char *)"", 0);
    g_xc.XFlush(g_clip.display);
    for (;;) {
        x_event event;
        memset(&event, 0, sizeof(event));
        g_xc.XNextEvent(g_clip.display, &event);
        if (event.type == X_PROPERTY_NOTIFY && event.property.window == g_clip.window &&
            event.property.atom == g_clip.stamp)
            return event.property.time;
        clipboard_event(&event);
    }
}

static void requests_run(void)
{
    char text[PORT_CLIPBOARD_MAX];
    pthread_mutex_lock(&g_clip.lock);
    const int set = g_clip.set_request, get = g_clip.get_request;
    if (set)
        memcpy(text, g_clip.set_text, sizeof(text));
    g_clip.set_request = g_clip.get_request = 0;
    pthread_mutex_unlock(&g_clip.lock);
    if (set) {
        const x_time stamp = server_time();
        memcpy(g_clip.owned_text, text, sizeof(g_clip.owned_text));
        g_xc.XSetSelectionOwner(g_clip.display, g_clip.clipboard, g_clip.window, stamp);
        g_clip.owned = g_xc.XGetSelectionOwner(g_clip.display, g_clip.clipboard) == g_clip.window;
        g_clip.owned_since = stamp;
        pthread_mutex_lock(&g_clip.lock);
        g_clip.set_result = g_clip.owned;
        g_clip.set_done = 1;
        pthread_cond_broadcast(&g_clip.changed);
        pthread_mutex_unlock(&g_clip.lock);
    }
    if (get) {
        if (g_clip.owned && g_xc.XGetSelectionOwner(g_clip.display, g_clip.clipboard) == g_clip.window) {
            get_finish(1, g_clip.owned_text);
        } else {
            g_clip.converting = 1;
            g_clip.converting_target = g_clip.utf8;
            deadline_after(&g_clip.deadline, PORT_CLIPBOARD_WAIT_MILLISECONDS);
            g_xc.XConvertSelection(g_clip.display, g_clip.clipboard, g_clip.utf8, g_clip.property, g_clip.window,
                                   X_CURRENT_TIME);
        }
    }
    g_xc.XFlush(g_clip.display);
}

static void *clipboard_main(void *unused)
{
    (void)unused;
    x_display *display = g_xc.XOpenDisplay(0);
    if (display != 0) {
        __atomic_store_n(&g_clip.display, display, __ATOMIC_RELEASE);
        __atomic_store_n(&g_previous_error_handler, g_xc.XSetErrorHandler(clipboard_x_error), __ATOMIC_RELEASE);
        g_clip.window = g_xc.XCreateSimpleWindow(display, g_xc.XRootWindow(display, g_xc.XDefaultScreen(display)), 0, 0,
                                                 1, 1, 0, 0, 0);
        g_xc.XSelectInput(display, g_clip.window, X_PROPERTY_CHANGE_MASK);
        g_clip.clipboard = g_xc.XInternAtom(display, "CLIPBOARD", 0);
        g_clip.utf8 = g_xc.XInternAtom(display, "UTF8_STRING", 0);
        g_clip.targets = g_xc.XInternAtom(display, "TARGETS", 0);
        g_clip.text = g_xc.XInternAtom(display, "TEXT", 0);
        g_clip.incr = g_xc.XInternAtom(display, "INCR", 0);
        g_clip.property = g_xc.XInternAtom(display, "HALO3_CLIPBOARD", 0);
        g_clip.stamp = g_xc.XInternAtom(display, "HALO3_CLIPBOARD_TIME", 0);
        g_xc.XFlush(display);
    }
    pthread_mutex_lock(&g_clip.lock);
    g_clip.state = display != 0 ? k_clip_running : k_clip_unavailable;
    pthread_cond_broadcast(&g_clip.changed);
    pthread_mutex_unlock(&g_clip.lock);
    if (display == 0)
        return 0;
    const int x_fd = g_xc.XConnectionNumber(display);
    for (;;) {
        requests_run();
        while (g_xc.XPending(display) > 0) {
            x_event event;
            memset(&event, 0, sizeof(event));
            g_xc.XNextEvent(display, &event);
            clipboard_event(&event);
        }
        if (g_clip.converting && deadline_passed(&g_clip.deadline))
            get_finish(0, ""); // the owner did not answer
        struct pollfd fds[2];
        fds[0].fd = x_fd;
        fds[0].events = POLLIN;
        fds[0].revents = 0;
        fds[1].fd = g_clip.wake[0];
        fds[1].events = POLLIN;
        fds[1].revents = 0;
        if (poll(fds, 2, 50) > 0 && (fds[1].revents & POLLIN) != 0) {
            char drained[16];
            while (read(g_clip.wake[0], drained, sizeof(drained)) > 0) {
            }
        }
    }
    return 0;
}

// Starts the thread once; whether the host has a clipboard (a display). Under the lock.
static int clipboard_running_locked(void)
{
    if (g_clip.state == k_clip_stopped) {
        g_clip.state = k_clip_unavailable;
        pthread_t thread;
        if (clipboard_x_load() && pipe(g_clip.wake) == 0) {
            fcntl(g_clip.wake[0], F_SETFL, fcntl(g_clip.wake[0], F_GETFL) | O_NONBLOCK); // drained without blocking
            g_clip.state = -1; // starting
            pthread_attr_t attributes;
            pthread_attr_init(&attributes);
            pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);
            if (pthread_create(&thread, &attributes, clipboard_main, 0) != 0)
                g_clip.state = k_clip_unavailable;
            pthread_attr_destroy(&attributes);
            while (g_clip.state == -1)
                pthread_cond_wait(&g_clip.changed, &g_clip.lock);
        }
        if (g_clip.state == k_clip_unavailable)
            port_write_error("clipboard: no X display: lobby codes stay in the game's own clipboard\n");
    }
    return g_clip.state == k_clip_running;
}

static void wake_thread(void)
{
    const char byte = 1;
    while (write(g_clip.wake[1], &byte, 1) < 0 && errno == EINTR) {
    }
}

// Waits (the lock held) until *done or the wait is over.
static void wait_done(const volatile int *done)
{
    struct timespec until;
    clock_gettime(CLOCK_REALTIME, &until);
    const unsigned long wait = PORT_CLIPBOARD_WAIT_MILLISECONDS + 100; // the thread's own deadline first
    until.tv_sec += (time_t)(wait / 1000);
    until.tv_nsec += (long)(wait % 1000) * 1000000L;
    if (until.tv_nsec >= 1000000000L) {
        until.tv_sec++;
        until.tv_nsec -= 1000000000L;
    }
    while (!*done)
        if (pthread_cond_timedwait(&g_clip.changed, &g_clip.lock, &until) == ETIMEDOUT)
            break;
}

int port_clipboard_host_set(const char *text)
{
    pthread_mutex_lock(&g_clip.lock);
    int result = 0;
    if (clipboard_running_locked()) {
        utf8_copy(g_clip.set_text, sizeof(g_clip.set_text), text, PORT_CLIPBOARD_MAX);
        g_clip.set_request = 1;
        g_clip.set_done = 0;
        wake_thread();
        wait_done(&g_clip.set_done);
        result = g_clip.set_done && g_clip.set_result;
    }
    pthread_mutex_unlock(&g_clip.lock);
    return result;
}

int port_clipboard_host_get(char *text, unsigned long capacity)
{
    pthread_mutex_lock(&g_clip.lock);
    int result = 0;
    if (clipboard_running_locked()) {
        g_clip.get_request = 1;
        g_clip.get_done = 0;
        wake_thread();
        wait_done(&g_clip.get_done);
        result = g_clip.get_done && g_clip.get_result;
        if (result)
            utf8_copy(text, capacity, g_clip.get_text, PORT_CLIPBOARD_MAX);
    }
    pthread_mutex_unlock(&g_clip.lock);
    return result;
}
