// port/sdk/runtime (Windows host): the game window through user32.dll, loaded at run time (the port links only
// kernel32 and msvcrt; see port/build/link-stage1.sh), with kernel32's host imports (port/sdk/kernel/win32_host.h).
// Interface: port_window.h.
//
// The window has a thread of its own. A window's messages reach only the thread that created it, and the game
// presents (and so calls port_window_pump) from whichever thread renders at the time, not always the one that opened
// the device: a pump on any other thread leaves the window's queue unread, and after five seconds of that Windows
// ghosts the window as "not responding" and offers to close it. The window thread blocks in GetMessage for the life
// of the window, answering the system at once (moves, resizes and the window modes run there too); the input it sees
// goes through a ring to the game input poll or the present-side pump, which hands it to port_input.h and the Guide.
#include "port_window.h"
#include "port_config.h"
#include "port_runtime.h"
#include "../input/port_input.h"
#include "../xam/guide_host.h"
#include "../xam/guide_ingame.h"
#include "../kernel/win32_host.h"

PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
PORT_HOST_IMPORT(void *, GetModuleHandleA, 4, (const char *name));
PORT_HOST_IMPORT(void *, CreateThread, 24,
    (void *security, __SIZE_TYPE__ stack_size, unsigned long(__stdcall *start)(void *), void *parameter,
        unsigned long flags, unsigned long *thread_id));
PORT_HOST_IMPORT(int, CloseHandle, 4, (void *handle));
PORT_HOST_IMPORT(void *, CreateEventA, 16, (void *security, int manual_reset, int initial_state, const char *name));
PORT_HOST_IMPORT(int, SetEvent, 4, (void *event));
PORT_HOST_IMPORT(unsigned long, WaitForSingleObject, 8, (void *handle, unsigned long milliseconds));
PORT_HOST_IMPORT(void, AcquireSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(void, ReleaseSRWLockExclusive, 4, (void **lock));

typedef __UINTPTR_TYPE__ w_wparam;
typedef __INTPTR_TYPE__ w_lparam;
typedef __INTPTR_TYPE__ w_lresult;
typedef w_lresult(__stdcall *window_procedure)(void *, unsigned int, w_wparam, w_lparam);

// WNDCLASSEXA, RECT and MSG (winuser.h), 32-bit and 64-bit.
typedef struct {
    unsigned int cbSize;
    unsigned int style;
    window_procedure lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    void *hInstance;
    void *hIcon;
    void *hCursor;
    void *hbrBackground;
    const char *lpszMenuName;
    const char *lpszClassName;
    void *hIconSm;
} w_class;
typedef struct {
    long left, top, right, bottom;
} w_rect;
typedef struct {
    void *hwnd;
    unsigned int message;
    w_wparam wParam;
    w_lparam lParam;
    unsigned long time;
    long x, y;
    unsigned long private_;
} w_message;

#define W_WM_CLOSE 0x0010
#define W_WM_DESTROY 0x0002
#define W_WM_SIZE 0x0005
#define W_WS_OVERLAPPEDWINDOW 0x00CF0000
#define W_WS_POPUP 0x80000000ul
#define W_WS_VISIBLE 0x10000000ul
#define W_GWL_STYLE (-16)
#define W_SWP_NOZORDER 0x0004u
#define W_SWP_FRAMECHANGED 0x0020u
#define W_SWP_SHOWWINDOW 0x0040u
#define W_SWP_NOOWNERZORDER 0x0200u
#define W_MONITOR_DEFAULTTONEAREST 2ul
#define W_CW_USEDEFAULT ((int)0x80000000)
#define W_SW_SHOW 5
#define W_CS_OWNDC 0x0020
#define W_WM_APP_CLOSE 0x8001 // WM_APP + 1: port_window_close asks the window thread to destroy the window and end
#define W_WM_APP_CURSOR 0x8003 // refresh a stationary client cursor on its owning thread
#define W_WM_APP_MODE 0x8002  // WM_APP + 2: apply window mode wParam (port_window_pump saw the settings change)
#define W_INFINITE 0xffffffffu
#define W_WM_SETFOCUS 0x0007
#define W_WM_KILLFOCUS 0x0008
#define W_WM_KEYDOWN 0x0100
#define W_WM_KEYUP 0x0101
#define W_WM_CHAR 0x0102
#define W_WM_SYSKEYDOWN 0x0104
#define W_WM_SYSKEYUP 0x0105
#define W_WM_MOUSEMOVE 0x0200
#define W_WM_LBUTTONDOWN 0x0201
#define W_WM_LBUTTONUP 0x0202
#define W_WM_RBUTTONDOWN 0x0204
#define W_WM_RBUTTONUP 0x0205
#define W_WM_MBUTTONDOWN 0x0207
#define W_WM_MBUTTONUP 0x0208
#define W_WM_MOUSEWHEEL 0x020A
#define W_WM_XBUTTONDOWN 0x020B // HIWORD(wParam): 1 XBUTTON1 (mouse4, back), 2 XBUTTON2 (mouse5, forward)
#define W_WM_XBUTTONUP 0x020C
#define W_WM_MOVE 0x0003
#define W_WM_NCLBUTTONDOWN 0x00A1
#define W_WM_ENTERSIZEMOVE 0x0231
#define W_WM_EXITSIZEMOVE 0x0232
#define W_WM_SETCURSOR 0x0020
#define W_WM_INPUT 0x00FF
#define W_WM_SYSCOMMAND 0x0112u
#define W_SC_SCREENSAVE 0xF140u
#define W_SC_MONITORPOWER 0xF170u
#define W_ES_SYSTEM_REQUIRED 0x00000001ul
#define W_ES_DISPLAY_REQUIRED 0x00000002ul
#define W_ES_CONTINUOUS 0x80000000ul
#define W_HTCLIENT 1
#define W_RID_INPUT 0x10000003u
#define W_RIM_TYPEMOUSE 0u
#define W_MOUSE_MOVE_ABSOLUTE 0x0001u
#define W_VK_SHIFT 0x10
#define W_VK_CONTROL 0x11
#define W_VK_MENU 0x12
#define W_VK_LSHIFT 0xA0
#define W_VK_RSHIFT 0xA1
#define W_PM_NOREMOVE 0x0000
#define W_IDC_ARROW ((const char *)32512)

typedef struct {
    long x, y;
} w_point;
// RAWINPUTDEVICE, and a mouse's RAWINPUT (RAWINPUTHEADER, RAWMOUSE) (winuser.h), 32-bit and 64-bit.
typedef struct {
    unsigned short usage_page, usage;
    unsigned long flags;
    void *target;
} w_raw_input_device;
typedef struct {
    unsigned long type, size; // RAWINPUTHEADER
    void *device;
    w_wparam wparam;
    unsigned short flags, unused; // RAWMOUSE (its button union is 4-aligned)
    unsigned short button_flags, button_data;
    unsigned long raw_buttons;
    long last_x, last_y;
    unsigned long extra;
} w_raw_mouse_input;
typedef char w_raw_input_device_size[sizeof(w_raw_input_device) == (sizeof(void *) == 8 ? 16 : 12) ? 1 : -1];
typedef char w_raw_mouse_input_layout[
    sizeof(w_raw_mouse_input) == (sizeof(void *) == 8 ? 48 : 40) &&
    __builtin_offsetof(w_raw_mouse_input, last_x) == (sizeof(void *) == 8 ? 36 : 28) ? 1 : -1];
// MONITORINFO (winuser.h)
typedef struct {
    unsigned long cbSize;
    w_rect rcMonitor, rcWork;
    unsigned long dwFlags;
} w_monitor_info;

static struct {
    unsigned short(__stdcall *RegisterClassExA)(const w_class *);
    void *(__stdcall *CreateWindowExA)(unsigned long, const char *, const char *, unsigned long, int, int, int, int,
                                       void *, void *, void *, void *);
    int(__stdcall *ShowWindow)(void *, int);
    int(__stdcall *GetMessageA)(w_message *, void *, unsigned int, unsigned int);
    int(__stdcall *PostMessageA)(void *, unsigned int, w_wparam, w_lparam);
    void(__stdcall *PostQuitMessage)(int);
    int(__stdcall *TranslateMessage)(const w_message *);
    w_lresult(__stdcall *DispatchMessageA)(const w_message *);
    w_lresult(__stdcall *DefWindowProcA)(void *, unsigned int, w_wparam, w_lparam);
    int(__stdcall *DestroyWindow)(void *);
    int(__stdcall *AdjustWindowRect)(w_rect *, unsigned long, int);
    int(__stdcall *ClientToScreen)(void *, w_point *);
    int(__stdcall *ScreenToClient)(void *, w_point *);
    int(__stdcall *GetCursorPos)(w_point *);
    int(__stdcall *SetCursorPos)(int, int);
    void *(__stdcall *GetFocus)(void);
    short(__stdcall *GetKeyState)(int);
    void *(__stdcall *LoadCursorA)(void *, const char *);
    // AltGr's made-up Left Ctrl (altgr_control): optional
    int(__stdcall *PeekMessageA)(w_message *, void *, unsigned int, unsigned int, unsigned int);
    long(__stdcall *GetMessageTime)(void);
    // raw mouse look (raw_input, mouse_capture_follow): optional, the pointer's own motion stands in without them
    int(__stdcall *RegisterRawInputDevices)(const w_raw_input_device *, unsigned int, unsigned int);
    unsigned int(__stdcall *GetRawInputData)(void *, unsigned int, void *, unsigned int *, unsigned int);
    int(__stdcall *ClipCursor)(const w_rect *);
    void *(__stdcall *SetCursor)(void *);
    int(__stdcall *GetClientRect)(void *, w_rect *);
    // the window modes (port_config.h): optional, the window stays framed without them
    long(__stdcall *GetWindowLongA)(void *, int);
    long(__stdcall *SetWindowLongA)(void *, int, long);
    int(__stdcall *SetWindowPos)(void *, void *, int, int, int, int, unsigned int);
    int(__stdcall *GetWindowRect)(void *, w_rect *);
    void *(__stdcall *MonitorFromWindow)(void *, unsigned long);
    int(__stdcall *GetMonitorInfoA)(void *, w_monitor_info *);
    // display/system sleep inhibition while the game window is open (kernel32.dll)
    unsigned long(__stdcall *SetThreadExecutionState)(unsigned long);
    unsigned int width, height;
    int pointer_x, pointer_y, pointer_known;
    int guide; // the in-game guide shows: the keys and mouse are its (guide_ingame.h)
    // A bit per virtual key: the keys down, and those down when the in-game guide closed (the Escape that closed it, the
    // Enter that chose Resume Game): their auto-repeats are not the game's, until let go.
    unsigned char keys_down[32], keys_held_over[32];
    int drop_char;      // the binding capture took the last key press: the character it types is not the guide's
    int wheel_residual; // wheel motion short of a notch (120), kept for the next (smooth-scrolling wheels)
    void *instance;
    void *window;
    void *thread; // the window thread: it created the window, and only it reads the window's messages
    __declspec(align(8)) unsigned long long raw_delta; // two 32-bit Raw Input counts, exchanged together
    int cursor_hidden;  // the pointer is hidden over the client area (the pump sets it; WM_SETCURSOR reads it)
    int captured;       // the pointer is confined to the client area (the pump's: mouse_capture_follow; -1 redo)
    int clip_stale;     // the window lost focus, moved or was resized (the window thread): the confinement is redone
    // the frame is held (the title bar, a border, a caption button) or the window is being moved or sized: the pointer
    // is the system's meanwhile, neither confined nor re-centred (the window thread sets it)
    int frame_held;
    int class_registered;
    int open;
    int focused;
    int close_requested;
    int mode;                    // the window mode applied (port_window_mode; the window thread's)
    unsigned long mode_revision; // port_config_revision when it was asked for (port_window_pump's)
    w_rect windowed;             // the framed window's place, kept while it is borderless or fullscreen
} g_w;

// The input the window thread saw, in order: one producer and one consumer at a time. A full ring drops new events.
typedef struct {
    unsigned int message;
    w_wparam w;
    w_lparam l;
    int shift; // Shift down as this message was read (GetKeyState holds for the window thread only)
    int sides; // a Shift key's message: the Shifts down as it was read (bit 0 the left, bit 1 the right)
} w_event;
#define W_QUEUE_SIZE 1024u // a power of two
static w_event g_queue[W_QUEUE_SIZE];
static unsigned int g_queue_head, g_queue_tail; // head: written by the window thread; tail: by the consumer

static void *g_window_lock; // SRWLOCK_INIT

void port_window_input_lock(void) { host_AcquireSRWLockExclusive(&g_window_lock); }
void port_window_input_unlock(void) { host_ReleaseSRWLockExclusive(&g_window_lock); }

// Virtual-key codes (winuser.h) of the keys port_input.h knows, with the WM_KEYDOWN lParam that tells a pair's left
// from its right: Shift by its scan code (0x36 the right one), Ctrl and Alt by the extended-key bit (the right ones).
static enum e_port_key key_of(unsigned int vk, w_lparam l)
{
    const int extended = ((w_wparam)l >> 24 & 1) != 0, scan = (int)((w_wparam)l >> 16 & 0xff);
    if (vk >= 'A' && vk <= 'Z')
        return (enum e_port_key)(k_port_key_a + (int)(vk - 'A'));
    if (vk >= '0' && vk <= '9')
        return (enum e_port_key)(k_port_key_0 + (int)(vk - '0'));
    if (vk >= 0x70 && vk <= 0x7b) // VK_F1 .. VK_F12
        return (enum e_port_key)(k_port_key_f1 + (int)(vk - 0x70));
    switch (vk) {
    case 0x1b: return k_port_key_escape;
    case 0x09: return k_port_key_tab;
    case 0x20: return k_port_key_space;
    case 0x0d: return k_port_key_return; // (the keypad's Enter too: the extended one)
    case 0x08: return k_port_key_backspace;
    case 0x26: return k_port_key_up;
    case 0x28: return k_port_key_down;
    case 0x25: return k_port_key_left;
    case 0x27: return k_port_key_right;
    case 0x10: return scan == 0x36 ? k_port_key_rshift : k_port_key_lshift; // VK_SHIFT
    case 0xa0: return k_port_key_lshift;
    case 0xa1: return k_port_key_rshift;
    case 0x11: return extended ? k_port_key_rctrl : k_port_key_lctrl; // VK_CONTROL
    case 0xa2: return k_port_key_lctrl;
    case 0xa3: return k_port_key_rctrl;
    case 0x12: return extended ? k_port_key_ralt : k_port_key_lalt; // VK_MENU
    case 0xa4: return k_port_key_lalt;
    case 0xa5: return k_port_key_ralt;
    case 0x14: return k_port_key_capslock;
    case 0x24: return k_port_key_home; // VK_HOME: the Guide button
    case 0x23: return k_port_key_end;
    case 0x2d: return k_port_key_insert;
    case 0x2e: return k_port_key_delete;
    case 0x21: return k_port_key_pageup;   // VK_PRIOR
    case 0x22: return k_port_key_pagedown; // VK_NEXT
    case 0xc0: return k_port_key_grave;     // VK_OEM_3
    case 0xbd: return k_port_key_minus;     // VK_OEM_MINUS
    case 0xbb: return k_port_key_equals;    // VK_OEM_PLUS
    case 0xdb: return k_port_key_lbracket;  // VK_OEM_4
    case 0xdd: return k_port_key_rbracket;  // VK_OEM_6
    case 0xdc: return k_port_key_backslash; // VK_OEM_5
    case 0xba: return k_port_key_semicolon; // VK_OEM_1
    case 0xde: return k_port_key_apostrophe; // VK_OEM_7
    case 0xbc: return k_port_key_comma;     // VK_OEM_COMMA
    case 0xbe: return k_port_key_period;    // VK_OEM_PERIOD
    case 0xbf: return k_port_key_slash;     // VK_OEM_2
    default: return k_port_key_none;
    }
}

int port_window_key_code(__UINTPTR_TYPE__ host_key, __INTPTR_TYPE__ host_flags)
{
    return key_of((unsigned int)host_key, host_flags);
}

// The in-game guide (guide_ingame.h) opened or closed: while it shows, the game's keys and buttons are all up (the
// guide has the keyboard and mouse) and the pointer is not re-centred.
static void guide_follow(void)
{
    const int guide = guide_ingame_input();
    if (guide == g_w.guide)
        return;
    __atomic_store_n(&g_w.guide, guide, __ATOMIC_RELEASE); // the window thread reads it (swallows)
    if (!guide)
        for (int i = 0; i < 32; i++)
            g_w.keys_held_over[i] = g_w.keys_down[i];
    port_input_release_all();
    __atomic_exchange_n(&g_w.raw_delta, 0, __ATOMIC_RELAXED);
    g_w.pointer_known = 0; // no look motion across the change
}

static int key_bit(const unsigned char *bits, unsigned int key) { return (bits[(key & 0xff) >> 3] >> (key & 7)) & 1; }
static void key_bit_set(unsigned char *bits, unsigned int key, int on)
{
    const unsigned char mask = (unsigned char)(1u << (key & 7));
    unsigned char *byte = &bits[(key & 0xff) >> 3];
    *byte = (unsigned char)(on ? *byte | mask : *byte & ~mask);
}

// A mouse button pressed while the in-game guide shows (1 left, 2 middle, 3 right).
static void guide_pointer_press(int button, w_lparam l)
{
    guide_ingame_pointer(GUIDE_POINTER_PRESS, button, (short)(l & 0xffff), (short)((w_wparam)l >> 16),
                         (int)g_w.width, (int)g_w.height);
}

// Keyboard, mouse buttons, the wheel, pointer motion and focus into port_input.h, or into the in-game guide while it
// shows (its key presses, typed characters and the pointer; releases still reach port_input.h, so nothing held before
// it opened stays down; a press goes first to the guide's binding capture, guide_capture). Runs under the window lock
// for the messages the window thread queued; `shift` is whether Shift was down as the window thread
// read this one, `sides` (a Shift key's message) which Shifts were. Returns 1 for a message it handled.
static int handle_input(unsigned int message, w_wparam w, w_lparam l, int shift, int sides)
{
    guide_follow(); // per event: a key that closed the guide hands the next ones back to the game
    if ((message >= W_WM_MOUSEMOVE && message <= W_WM_MBUTTONUP) ||
        message == W_WM_XBUTTONDOWN || message == W_WM_XBUTTONUP)
        port_input_mouse_position((short)(l & 0xffff), (short)((w_wparam)l >> 16), (int)g_w.width, (int)g_w.height);
    switch (message) {
    case W_WM_KEYDOWN:
    case W_WM_SYSKEYDOWN:
    case W_WM_KEYUP:
    case W_WM_SYSKEYUP: {
        const unsigned int vk = (unsigned int)w;
        const int down = message == W_WM_KEYDOWN || message == W_WM_SYSKEYDOWN;
        const int repeat = down && ((w_wparam)l >> 30 & 1) != 0; // the key was down already: auto-repeat
        key_bit_set(g_w.keys_down, vk, down);
        if (!down)
            key_bit_set(g_w.keys_held_over, vk, 0);
        if (down)
            g_w.drop_char = 0;
        if (g_w.guide) {
            if (down && !repeat && guide_capture(key_of(vk, l))) {
                g_w.drop_char = 1; // the character this press types (WM_CHAR, next) is the capture's too
                return 1;
            }
            // the guide's keys with Alt held too, as on Linux; other system keys (Alt+F4) stay the system's; a release
            // goes to both (nothing held before it opened stays down)
            const int guide_key = guide_host_virtual_key(vk, shift);
            if (guide_key >= 0)
                guide_ingame_key_action(guide_key, 0,
                                        !down ? GUIDE_KEY_RELEASED : repeat ? GUIDE_KEY_REPEATED : GUIDE_KEY_PRESSED);
            if (down)
                return guide_key >= 0; // typed characters follow as WM_CHAR (TranslateMessage)
        } else if (down && key_bit(g_w.keys_held_over, vk)) {
            return 1; // held since the guide closed: not the game's until let go
        }
        const enum e_port_key key = key_of(vk, l);
        if (key == k_port_key_none)
            return 0; // keys the game has no use for stay the system's
        if (vk == W_VK_SHIFT && !down) {
            // Windows sends no key-up for the first Shift let go of while both are down, and one for the last (with its
            // scan code): each Shift up as the window thread read this one is up
            if (!(sides & 1))
                port_input_key(k_port_key_lshift, 0);
            if (!(sides & 2))
                port_input_key(k_port_key_rshift, 0);
            return 1;
        }
        port_input_key(key, down);
        return 1;
    }
    case W_WM_CHAR: {
        if (g_w.drop_char) {
            g_w.drop_char = 0;
            return 1;
        }
        if (!g_w.guide)
            return 0;
        const unsigned int ch = (unsigned int)w;
        const int key = guide_host_typed(ch); // a character, or Ctrl+V's paste
        if (key >= 0) // (bit 30: its key was down already, an auto-repeat)
            guide_ingame_key_action(key, ch, ((w_wparam)l >> 30 & 1) != 0 ? GUIDE_KEY_REPEATED : GUIDE_KEY_PRESSED);
        return 1;
    }
    case W_WM_LBUTTONDOWN: case W_WM_LBUTTONUP:
        if (message == W_WM_LBUTTONDOWN && g_w.guide) {
            if (!guide_capture(PORT_KBM_MOUSE(k_port_mouse_left)))
                guide_pointer_press(1, l);
        } else {
            port_input_mouse_button(k_port_mouse_left, message == W_WM_LBUTTONDOWN);
        }
        return 1;
    case W_WM_RBUTTONDOWN: case W_WM_RBUTTONUP:
        if (message == W_WM_RBUTTONDOWN && g_w.guide) {
            if (!guide_capture(PORT_KBM_MOUSE(k_port_mouse_right)))
                guide_pointer_press(3, l);
        } else {
            port_input_mouse_button(k_port_mouse_right, message == W_WM_RBUTTONDOWN);
        }
        return 1;
    case W_WM_MBUTTONDOWN: case W_WM_MBUTTONUP:
        if (message == W_WM_MBUTTONDOWN && g_w.guide) {
            if (!guide_capture(PORT_KBM_MOUSE(k_port_mouse_middle)))
                guide_pointer_press(2, l);
        } else {
            port_input_mouse_button(k_port_mouse_middle, message == W_WM_MBUTTONDOWN);
        }
        return 1;
    case W_WM_XBUTTONDOWN: case W_WM_XBUTTONUP: {
        const enum e_port_mouse_button button = ((w >> 16) & 0xffff) == 1 ? k_port_mouse_4 : k_port_mouse_5;
        if (message == W_WM_XBUTTONDOWN && g_w.guide)
            guide_capture(PORT_KBM_MOUSE(button)); // (the guide has no use for them otherwise)
        else
            port_input_mouse_button(button, message == W_WM_XBUTTONDOWN);
        return 1;
    }
    case W_WM_MOUSEWHEEL: {
        const int delta = (short)(w >> 16); // a notch: 120, positive away from the user
        w_point point = { (short)(l & 0xffff), (short)((w_wparam)l >> 16) };
        const int positioned = g_w.ScreenToClient(g_w.window, &point);
        if (positioned)
            port_input_mouse_position((int)point.x, (int)point.y, (int)g_w.width, (int)g_w.height);
        if (!g_w.guide) {
            g_w.wheel_residual += delta;
            const int notches = g_w.wheel_residual / 120;
            g_w.wheel_residual -= notches * 120;
            port_input_mouse_wheel(notches);
            return 1;
        }
        if (delta != 0 && guide_capture(delta > 0 ? PORT_KBM_WHEEL_UP : PORT_KBM_WHEEL_DOWN))
            return 1;
        // the pointer in screen coordinates; the pane under it scrolls
        if (positioned)
            guide_ingame_pointer(GUIDE_POINTER_WHEEL, delta > 0 ? -1 : 1, (int)point.x, (int)point.y,
                                 (int)g_w.width, (int)g_w.height);
        return 1;
    }
    case W_WM_SIZE: // the client area's new size: the frame is stretched over it (the guide's pointer)
        if ((l & 0xffff) != 0 && (((w_wparam)l >> 16) & 0xffff) != 0) {
            g_w.width = (unsigned int)(l & 0xffff);
            g_w.height = (unsigned int)(((w_wparam)l >> 16) & 0xffff);
        }
        return 0;
    case W_WM_MOUSEMOVE: {
        const int x = (short)(l & 0xffff), y = (short)((w_wparam)l >> 16);
        if (g_w.guide) { // the pointer is the guide's: no look, no re-centring
            guide_ingame_pointer(GUIDE_POINTER_MOVE, 0, x, y, (int)g_w.width, (int)g_w.height);
            return 1;
        }
        const int cx = (int)(g_w.width / 2), cy = (int)(g_w.height / 2);
        // (GetFocus answers for the calling thread's own windows only: the window thread keeps `focused` instead)
        const int capture = port_input_mouse_capture() && __atomic_load_n(&g_w.focused, __ATOMIC_ACQUIRE) &&
                            !__atomic_load_n(&g_w.frame_held, __ATOMIC_ACQUIRE);
        if (g_w.pointer_known && !(capture && x == cx && y == cy)) // not our own re-centring
            port_input_mouse_motion(x - g_w.pointer_x, y - g_w.pointer_y);
        g_w.pointer_x = x;
        g_w.pointer_y = y;
        g_w.pointer_known = 1;
        if (capture && (x != cx || y != cy)) {
            w_point centre = { cx, cy };
            g_w.ClientToScreen(g_w.window, &centre);
            g_w.SetCursorPos((int)centre.x, (int)centre.y);
            g_w.pointer_x = cx;
            g_w.pointer_y = cy;
        }
        return 1;
    }
    case W_WM_SETFOCUS: // (`focused` is the window thread's: it sets it as the message arrives)
        return 0;
    case W_WM_KILLFOCUS: // what is let go while another window has the keys never reaches this one: nothing stays down
        g_w.pointer_known = 0;
        port_input_release_all();
        for (int i = 0; i < 32; i++)
            g_w.keys_down[i] = g_w.keys_held_over[i] = 0;
        guide_keys_released();
        return 0;
    default:
        return 0;
    }
}
// Whether handle_input will take this message (so DefWindowProc must not see it), decided on the window thread
// without its side effects: Alt+F4 and the other system keys stay the system's, as before.
static int swallows(unsigned int message, w_wparam w, w_lparam l, int shift)
{
    const int guide = __atomic_load_n(&g_w.guide, __ATOMIC_ACQUIRE);
    // Alt+F4 stays the system's (it closes the window), whatever the keys are bound to
    if ((message == W_WM_SYSKEYDOWN || message == W_WM_SYSKEYUP) && w == 0x73)
        return 0;
    switch (message) {
    case W_WM_KEYDOWN:
    case W_WM_SYSKEYDOWN:
        if (guide && guide_host_virtual_key((unsigned int)w, shift) >= 0)
            return 1;
        return key_of((unsigned int)w, l) != k_port_key_none; // (a key the guide has no use for may still be a binding's capture)
    case W_WM_KEYUP:
    case W_WM_SYSKEYUP:
        return key_of((unsigned int)w, l) != k_port_key_none;
    case W_WM_CHAR:
        return guide;
    case W_WM_MOUSEWHEEL:
    case W_WM_LBUTTONDOWN: case W_WM_LBUTTONUP:
    case W_WM_RBUTTONDOWN: case W_WM_RBUTTONUP:
    case W_WM_MBUTTONDOWN: case W_WM_MBUTTONUP:
    case W_WM_XBUTTONDOWN: case W_WM_XBUTTONUP:
    case W_WM_MOUSEMOVE:
        return 1;
    default:
        return 0;
    }
}

static int queued(unsigned int message)
{
    switch (message) {
    case W_WM_KEYDOWN: case W_WM_KEYUP: case W_WM_SYSKEYDOWN: case W_WM_SYSKEYUP: case W_WM_CHAR:
    case W_WM_LBUTTONDOWN: case W_WM_LBUTTONUP:
    case W_WM_RBUTTONDOWN: case W_WM_RBUTTONUP:
    case W_WM_MBUTTONDOWN: case W_WM_MBUTTONUP:
    case W_WM_XBUTTONDOWN: case W_WM_XBUTTONUP:
    case W_WM_MOUSEWHEEL: case W_WM_MOUSEMOVE:
    case W_WM_SIZE: case W_WM_SETFOCUS: case W_WM_KILLFOCUS:
        return 1;
    default:
        return 0;
    }
}

// Window thread: one message into the ring for the next input drain.
static void enqueue(unsigned int message, w_wparam w, w_lparam l, int shift, int sides)
{
    const unsigned int head = g_queue_head;
    if (head - __atomic_load_n(&g_queue_tail, __ATOMIC_ACQUIRE) >= W_QUEUE_SIZE)
        return; // nobody has pumped for a while (a long load): this one is dropped
    w_event *event = &g_queue[head & (W_QUEUE_SIZE - 1)];
    event->message = message;
    event->w = w;
    event->l = l;
    event->shift = shift;
    event->sides = sides;
    __atomic_store_n(&g_queue_head, head + 1, __ATOMIC_RELEASE);
}

static void window_mode_apply(int mode);

// Window thread: accumulate the relative WM_INPUT counts for the next input drain. An absolute device (a pen, a
// remote desktop) has none: its pointer motion stands in.
static void raw_input(w_lparam handle)
{
    if (!__atomic_load_n(&g_w.focused, __ATOMIC_RELAXED) ||
        __atomic_load_n(&g_w.frame_held, __ATOMIC_RELAXED) ||
        __atomic_load_n(&g_w.guide, __ATOMIC_ACQUIRE))
        return;
    w_raw_mouse_input input;
    unsigned int size = sizeof(input);
    const unsigned int header_size = sizeof(void *) == 8 ? 24u : 16u; // sizeof(RAWINPUTHEADER)
    if (g_w.GetRawInputData == 0 ||
        g_w.GetRawInputData((void *)handle, W_RID_INPUT, &input, &size, header_size) != sizeof(input))
        return;
    if (input.type != W_RIM_TYPEMOUSE || (input.flags & W_MOUSE_MOVE_ABSOLUTE) ||
        (input.last_x == 0 && input.last_y == 0))
        return;
    unsigned long long previous = __atomic_load_n(&g_w.raw_delta, __ATOMIC_RELAXED);
    unsigned long long next;
    do {
        const unsigned int x = (unsigned int)previous + (unsigned int)input.last_x;
        const unsigned int y = (unsigned int)(previous >> 32) + (unsigned int)input.last_y;
        next = ((unsigned long long)y << 32) | x;
    } while (!__atomic_compare_exchange_n(&g_w.raw_delta, &previous, next, 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED));
}

// Window thread: AltGr (the right Alt of many layouts) comes as a Left Ctrl the system makes up, then the right Alt,
// both at the same time, and goes up the same way: the made-up Ctrl (a Ctrl message without the extended-key bit whose
// next message is the right Alt's, at the same time) is not the player's Left Ctrl.
static int altgr_control(unsigned int message, w_wparam w, w_lparam l)
{
    if (w != W_VK_CONTROL || ((w_wparam)l >> 24 & 1) || g_w.PeekMessageA == 0 || g_w.GetMessageTime == 0)
        return 0;
    if (message != W_WM_KEYDOWN && message != W_WM_SYSKEYDOWN && message != W_WM_KEYUP && message != W_WM_SYSKEYUP)
        return 0;
    w_message next;
    if (!g_w.PeekMessageA(&next, 0, 0, 0, W_PM_NOREMOVE))
        return 0;
    const int down = message == W_WM_KEYDOWN || message == W_WM_SYSKEYDOWN;
    const int next_down = next.message == W_WM_KEYDOWN || next.message == W_WM_SYSKEYDOWN;
    const int next_up = next.message == W_WM_KEYUP || next.message == W_WM_SYSKEYUP;
    return (down ? next_down : next_up) && next.wParam == W_VK_MENU && ((w_wparam)next.lParam >> 24 & 1) &&
           next.time == (unsigned long)g_w.GetMessageTime();
}

// The window's procedure, on the window thread.
static w_lresult __stdcall procedure(void *window, unsigned int message, w_wparam w, w_lparam l)
{
    if (message == W_WM_CLOSE) {
        __atomic_store_n(&g_w.close_requested, 1, __ATOMIC_RELEASE);
        return 0;
    }
    if (message == W_WM_APP_CLOSE) {
        g_w.DestroyWindow(window);
        g_w.PostQuitMessage(0);
        return 0;
    }
    if (message == W_WM_APP_CURSOR) {
        w_point point;
        w_rect client;
        if (g_w.SetCursor && g_w.GetClientRect && g_w.GetFocus() == window &&
            g_w.GetCursorPos(&point) && g_w.ScreenToClient(window, &point) &&
            g_w.GetClientRect(window, &client) && point.x >= 0 && point.y >= 0 &&
            point.x < client.right && point.y < client.bottom)
            g_w.SetCursor(__atomic_load_n(&g_w.cursor_hidden, __ATOMIC_ACQUIRE) ? 0 : g_w.LoadCursorA(0, W_IDC_ARROW));
        return 0;
    }
    if (message == W_WM_APP_MODE) {
        window_mode_apply((int)w);
        return 0;
    }
    if (message == W_WM_SYSCOMMAND) {
        const unsigned int cmd = (unsigned int)(w & 0xfff0u);
        if (cmd == W_SC_SCREENSAVE || cmd == W_SC_MONITORPOWER)
            return 0;
    }
    if (message == W_WM_SETFOCUS || message == W_WM_KILLFOCUS) // at once, for port_window_is_focused
        __atomic_store_n(&g_w.focused, message == W_WM_SETFOCUS, __ATOMIC_RELEASE);
    if (message == W_WM_KILLFOCUS || message == W_WM_MOVE || message == W_WM_SIZE) {
        if (message == W_WM_KILLFOCUS) {
            __atomic_exchange_n(&g_w.raw_delta, 0, __ATOMIC_RELAXED);
            if (g_w.ClipCursor != 0)
                g_w.ClipCursor(0); // at once: the pointer is the next window's
        }
        __atomic_store_n(&g_w.clip_stale, 1, __ATOMIC_RELEASE);
    }
    // The frame held: the pointer let go at once. Confining it to the client area while the title bar is dragged pulls
    // it off the title bar, and the window follows it (it slides down, again with each move). WM_NCLBUTTONDOWN's
    // DefWindowProc runs the whole drag (or the caption button's press) before it returns; Alt+Space's Move and Size
    // come as WM_ENTERSIZEMOVE / WM_EXITSIZEMOVE alone.
    if (message == W_WM_NCLBUTTONDOWN || message == W_WM_ENTERSIZEMOVE) {
        __atomic_store_n(&g_w.frame_held, 1, __ATOMIC_RELEASE);
        __atomic_exchange_n(&g_w.raw_delta, 0, __ATOMIC_RELAXED);
        if (g_w.ClipCursor != 0)
            g_w.ClipCursor(0);
        __atomic_store_n(&g_w.clip_stale, 1, __ATOMIC_RELEASE);
    }
    if (message == W_WM_NCLBUTTONDOWN || message == W_WM_EXITSIZEMOVE) {
        const w_lresult result = message == W_WM_NCLBUTTONDOWN ? g_w.DefWindowProcA(window, message, w, l) : 0;
        __atomic_store_n(&g_w.frame_held, 0, __ATOMIC_RELEASE);
        __atomic_store_n(&g_w.clip_stale, 1, __ATOMIC_RELEASE); // confined again where the window now is
        return result;
    }
    if (message == W_WM_INPUT) {
        raw_input(l);
        return g_w.DefWindowProcA(window, message, w, l); // (WM_INPUT's own clean-up)
    }
    if (message == W_WM_SETCURSOR && (l & 0xffff) == W_HTCLIENT && g_w.SetCursor != 0 &&
        __atomic_load_n(&g_w.cursor_hidden, __ATOMIC_ACQUIRE)) {
        g_w.SetCursor(0);
        return 1;
    }
    if (queued(message)) {
        if (altgr_control(message, w, l))
            return g_w.DefWindowProcA(window, message, w, l);
        const int shift = g_w.GetKeyState(W_VK_SHIFT) < 0;
        const int sides =
            w == W_VK_SHIFT ? (g_w.GetKeyState(W_VK_LSHIFT) < 0) | (g_w.GetKeyState(W_VK_RSHIFT) < 0) << 1 : 0;
        enqueue(message, w, l, shift, sides);
        if (swallows(message, w, l, shift)) // (the X buttons' handled answer is TRUE: no WM_APPCOMMAND follows)
            return message == W_WM_XBUTTONDOWN || message == W_WM_XBUTTONUP;
    }
    return g_w.DefWindowProcA(window, message, w, l);
}

static int load(void)
{
    if (g_w.RegisterClassExA != 0)
        return 1;
    void *user32 = host_LoadLibraryA("user32.dll");
    if (user32 == 0) {
        port_write_error("window: user32.dll is not available\n");
        return 0;
    }
#define LOAD(name)                                                                                                  \
    do {                                                                                                            \
        *(void **)&g_w.name = host_GetProcAddress(user32, #name);                                                   \
        if (g_w.name == 0) {                                                                                        \
            port_write_error("window: user32.dll has no " #name "\n");                                              \
            return 0;                                                                                               \
        }                                                                                                           \
    } while (0)
    LOAD(RegisterClassExA);
    LOAD(CreateWindowExA);
    LOAD(ShowWindow);
    LOAD(GetMessageA);
    LOAD(PostMessageA);
    LOAD(PostQuitMessage);
    LOAD(TranslateMessage);
    LOAD(DispatchMessageA);
    LOAD(DefWindowProcA);
    LOAD(DestroyWindow);
    LOAD(AdjustWindowRect);
    LOAD(ClientToScreen);
    LOAD(ScreenToClient);
    LOAD(SetCursorPos);
    LOAD(GetCursorPos);
    LOAD(GetFocus);
    LOAD(GetKeyState);
    LOAD(LoadCursorA);
#undef LOAD
    *(void **)&g_w.GetWindowLongA = host_GetProcAddress(user32, "GetWindowLongA");
    *(void **)&g_w.SetWindowLongA = host_GetProcAddress(user32, "SetWindowLongA");
    *(void **)&g_w.SetWindowPos = host_GetProcAddress(user32, "SetWindowPos");
    *(void **)&g_w.GetWindowRect = host_GetProcAddress(user32, "GetWindowRect");
    *(void **)&g_w.MonitorFromWindow = host_GetProcAddress(user32, "MonitorFromWindow");
    *(void **)&g_w.GetMonitorInfoA = host_GetProcAddress(user32, "GetMonitorInfoA");
    *(void **)&g_w.PeekMessageA = host_GetProcAddress(user32, "PeekMessageA");
    *(void **)&g_w.GetMessageTime = host_GetProcAddress(user32, "GetMessageTime");
    *(void **)&g_w.RegisterRawInputDevices = host_GetProcAddress(user32, "RegisterRawInputDevices");
    *(void **)&g_w.GetRawInputData = host_GetProcAddress(user32, "GetRawInputData");
    *(void **)&g_w.ClipCursor = host_GetProcAddress(user32, "ClipCursor");
    *(void **)&g_w.SetCursor = host_GetProcAddress(user32, "SetCursor");
    *(void **)&g_w.GetClientRect = host_GetProcAddress(user32, "GetClientRect");
    void *kernel32 = host_GetModuleHandleA("kernel32.dll");
    if (kernel32 != 0)
        *(void **)&g_w.SetThreadExecutionState = host_GetProcAddress(kernel32, "SetThreadExecutionState");
    // Ensure native physical-pixel coordinates and DirectFlip on high-DPI laptops even without an embedded manifest.
    {
        typedef int(__stdcall *set_dpi_ctx_fn)(void *);
        typedef int(__stdcall *set_dpi_aware_fn)(void);
        set_dpi_ctx_fn set_ctx = (set_dpi_ctx_fn)host_GetProcAddress(user32, "SetProcessDpiAwarenessContext");
        if (set_ctx != 0) {
            set_ctx((void *)(__INTPTR_TYPE__)-4); // DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
        } else {
            set_dpi_aware_fn set_aware = (set_dpi_aware_fn)host_GetProcAddress(user32, "SetProcessDPIAware");
            if (set_aware != 0)
                set_aware();
        }
    }
    return 1;
}

// ---- the window modes (port_config.h's port_window_mode) ----
//
// Windowed is the framed window (WS_OVERLAPPEDWINDOW) where it was; borderless a frameless window (WS_POPUP) over the
// work area of the monitor it is on (the taskbar stays); fullscreen one over that whole monitor (the desktop's
// compositor then gives it the display, as an exclusive mode would, without changing the display mode). Applied on the
// window thread: at open, and for each W_WM_APP_MODE port_window_pump posts when the settings change.
static void window_mode_apply(int mode)
{
    if (g_w.GetWindowLongA == 0 || g_w.SetWindowLongA == 0 || g_w.SetWindowPos == 0 || g_w.GetWindowRect == 0 ||
        g_w.MonitorFromWindow == 0 || g_w.GetMonitorInfoA == 0 || mode == g_w.mode)
        return;
    if (g_w.mode == PORT_WINDOW_MODE_WINDOWED)
        g_w.GetWindowRect(g_w.window, &g_w.windowed);
    w_rect place = g_w.windowed;
    unsigned long style = W_WS_OVERLAPPEDWINDOW | W_WS_VISIBLE;
    if (mode != PORT_WINDOW_MODE_WINDOWED) {
        w_monitor_info monitor;
        __builtin_memset(&monitor, 0, sizeof(monitor));
        monitor.cbSize = sizeof(monitor);
        void *handle = g_w.MonitorFromWindow(g_w.window, W_MONITOR_DEFAULTTONEAREST);
        if (handle == 0 || !g_w.GetMonitorInfoA(handle, &monitor))
            return;
        place = mode == PORT_WINDOW_MODE_FULLSCREEN ? monitor.rcMonitor : monitor.rcWork;
        style = W_WS_POPUP | W_WS_VISIBLE;
    }
    g_w.SetWindowLongA(g_w.window, W_GWL_STYLE, (long)style);
    g_w.SetWindowPos(g_w.window, 0, (int)place.left, (int)place.top, (int)(place.right - place.left),
                     (int)(place.bottom - place.top), W_SWP_NOZORDER | W_SWP_NOOWNERZORDER | W_SWP_FRAMECHANGED |
                     W_SWP_SHOWWINDOW);
    g_w.mode = mode;
}

// The mode halo3.ini (or HALO3_WINDOW_MODE) asks for now, once the settings changed (Guide > Settings): handed to
// the window thread, which owns the window's style and place.
static void window_mode_follow(void)
{
    const unsigned long revision = port_config_revision();
    if (revision == g_w.mode_revision)
        return;
    g_w.mode_revision = revision;
    g_w.PostMessageA(g_w.window, W_WM_APP_MODE, (unsigned int)port_config_window_mode(), 0);
}

int port_window_system(void)
{
    return PORT_WINDOW_SYSTEM_WIN32;
}

typedef struct {
    const char *title;
    unsigned int width, height;
    int mode;    // the window mode it opens in
    void *ready; // set once the window is up (or failed to come up)
    int ok;
} w_open_request;

// The window thread: creates the window, then reads its messages until port_window_close.
static unsigned long __stdcall window_thread(void *parameter)
{
    w_open_request *request = (w_open_request *)parameter;
    if (!g_w.class_registered) {
        w_class wc = { 0 };
        wc.cbSize = sizeof(wc);
        wc.style = W_CS_OWNDC;
        wc.lpfnWndProc = procedure;
        wc.hInstance = g_w.instance;
        wc.hCursor = g_w.LoadCursorA(0, W_IDC_ARROW); // shown over the guide (and wherever the pointer is not captured)
        wc.lpszClassName = "halo3_port_window";
        if (g_w.RegisterClassExA(&wc) == 0) {
            port_write_error("window: RegisterClassExA failed\n");
            host_SetEvent(request->ready);
            return 1;
        }
        g_w.class_registered = 1;
    }
    w_rect rect = { 0, 0, (long)request->width, (long)request->height };
    g_w.AdjustWindowRect(&rect, W_WS_OVERLAPPEDWINDOW, 0);
    void *window = g_w.CreateWindowExA(0, "halo3_port_window", request->title != 0 ? request->title : "halo3",
                                      W_WS_OVERLAPPEDWINDOW, W_CW_USEDEFAULT, W_CW_USEDEFAULT, rect.right - rect.left,
                                      rect.bottom - rect.top, 0, 0, g_w.instance, 0);
    if (window == 0) {
        port_write_error("window: CreateWindowExA failed\n");
        host_SetEvent(request->ready);
        return 1;
    }
    __atomic_store_n(&g_w.window, window, __ATOMIC_RELEASE);
    g_w.ShowWindow(g_w.window, W_SW_SHOW);
    // Keep the display and system awake while the game window is open (gamepad input alone does not reset idle timers).
    if (g_w.SetThreadExecutionState != 0)
        g_w.SetThreadExecutionState(W_ES_CONTINUOUS | W_ES_DISPLAY_REQUIRED | W_ES_SYSTEM_REQUIRED);
    // raw mouse look's counts (WM_INPUT, raw_input): the mouse's own, before the pointer's acceleration
    const w_raw_input_device mouse = { 1, 2, 0, g_w.window }; // HID generic desktop: mouse
    if (g_w.RegisterRawInputDevices == 0 || !g_w.RegisterRawInputDevices(&mouse, 1, sizeof(mouse)))
        port_write_error("window: no Raw Input mouse: mouse look reads the pointer's motion\n");
    // the window mode it opens in (halo3.ini's window_mode, HALO3_WINDOW_MODE), then whatever the Guide makes it
    // (W_WM_APP_MODE); SetWindowPos sends WM_SIZE (the client area's new size), queued for the first pump
    g_w.mode = PORT_WINDOW_MODE_WINDOWED;
    window_mode_apply(request->mode);
    __atomic_store_n(&g_w.focused, g_w.GetFocus() == g_w.window ? 1 : 0, __ATOMIC_RELEASE);
    request->ok = 1;
    host_SetEvent(request->ready); // `request` is the opener's: not touched past here
    w_message message;
    while (g_w.GetMessageA(&message, 0, 0, 0) > 0) {
        g_w.TranslateMessage(&message);
        g_w.DispatchMessageA(&message);
    }
    if (g_w.SetThreadExecutionState != 0)
        g_w.SetThreadExecutionState(W_ES_CONTINUOUS);
    return 0;
}

static int window_open(const char *title, unsigned int width, unsigned int height)
{
    if (g_w.open)
        return 1;
    if (!load())
        return 0;
    g_w.instance = host_GetModuleHandleA(0);
    g_w.width = width;
    g_w.height = height;
    g_w.pointer_known = 0;
    g_w.close_requested = 0;
    g_w.captured = 0;
    g_w.cursor_hidden = g_w.clip_stale = 0;
    __atomic_store_n(&g_w.raw_delta, 0, __ATOMIC_RELAXED);
    g_queue_head = g_queue_tail = 0;
    g_w.mode_revision = port_config_revision();
    w_open_request request = { title, width, height, port_config_window_mode(), host_CreateEventA(0, 1, 0, 0), 0 };
    if (request.ready == 0) {
        port_write_error("window: CreateEventA failed\n");
        return 0;
    }
    g_w.thread = host_CreateThread(0, 0, window_thread, &request, 0, 0);
    if (g_w.thread == 0) {
        port_write_error("window: CreateThread failed\n");
        host_CloseHandle(request.ready);
        return 0;
    }
    host_WaitForSingleObject(request.ready, W_INFINITE);
    host_CloseHandle(request.ready);
    if (!request.ok) {
        host_WaitForSingleObject(g_w.thread, W_INFINITE);
        host_CloseHandle(g_w.thread);
        g_w.thread = 0;
        return 0;
    }
    port_input_keyboard_attach(1);
    __atomic_store_n(&g_w.open, 1, __ATOMIC_RELEASE);
    return 1;
}

int port_window_open(const char *title, unsigned int width, unsigned int height)
{
    port_window_input_lock();
    const int opened = window_open(title, width, height);
    port_window_input_unlock();
    return opened;
}

void *port_window_native_display(void)
{
    return g_w.instance;
}

__UINTPTR_TYPE__ port_window_native_window(void)
{
    return (__UINTPTR_TYPE__)__atomic_load_n(&g_w.window, __ATOMIC_ACQUIRE);
}

// The pointer while the mouse aims (port_input_mouse_capture: raw mouse look, or HALO3_MOUSE_CAPTURE=1), the window has
// focus and the in-game guide does not show: hidden over the client area and confined to it, as X11's grab; otherwise
// the system's. Then deliver the accumulated Raw Input counts to mouse look. The window lock is held.
static void mouse_capture_follow(void)
{
    const int want = __atomic_load_n(&g_w.focused, __ATOMIC_ACQUIRE) && !g_w.guide && port_input_mouse_capture() &&
                     !__atomic_load_n(&g_w.frame_held, __ATOMIC_ACQUIRE);
    if (__atomic_exchange_n(&g_w.clip_stale, 0, __ATOMIC_ACQ_REL))
        g_w.captured = -1; // focus lost, the window moved or resized: confined again (or not) below
    if (want != g_w.captured) {
        g_w.captured = want;
        if (__atomic_exchange_n(&g_w.cursor_hidden, want, __ATOMIC_ACQ_REL) != want)
            g_w.PostMessageA(g_w.window, W_WM_APP_CURSOR, 0, 0);
        w_rect client;
        if (g_w.ClipCursor != 0 && want && g_w.GetClientRect != 0 && g_w.GetClientRect(g_w.window, &client)) {
            w_point corners[2] = { { client.left, client.top }, { client.right, client.bottom } };
            g_w.ClientToScreen(g_w.window, &corners[0]);
            g_w.ClientToScreen(g_w.window, &corners[1]);
            const w_rect screen = { corners[0].x, corners[0].y, corners[1].x, corners[1].y };
            g_w.ClipCursor(&screen);
            if (!__atomic_load_n(&g_w.focused, __ATOMIC_ACQUIRE) ||
                __atomic_load_n(&g_w.frame_held, __ATOMIC_ACQUIRE))
                g_w.ClipCursor(0); // focus or ownership of the pointer changed while clipping
        } else if (g_w.ClipCursor != 0) {
            g_w.ClipCursor(0);
        }
    }
    const unsigned long long raw = __atomic_exchange_n(&g_w.raw_delta, 0, __ATOMIC_RELAXED);
    if (raw && want && port_input_mouse_raw())
        port_input_mouse_raw_motion((int)(unsigned int)raw, (int)(unsigned int)(raw >> 32));
}

void port_window_cursor_mode_changed(void)
{
    if (!g_w.open)
        return;
    g_w.pointer_known = 0;
    g_w.wheel_residual = 0;
    __atomic_exchange_n(&g_w.raw_delta, 0, __ATOMIC_RELAXED);
    mouse_capture_follow();
}

// The caller holds the window lock through dispatch and any resulting input sample.
void port_window_poll_input(void)
{
    if (!g_w.open)
        return;
    guide_follow();
    unsigned int tail = g_queue_tail;
    const unsigned int head = __atomic_load_n(&g_queue_head, __ATOMIC_ACQUIRE);
    while (tail != head) {
        const w_event event = g_queue[tail & (W_QUEUE_SIZE - 1)];
        __atomic_store_n(&g_queue_tail, ++tail, __ATOMIC_RELEASE);
        handle_input(event.message, event.w, event.l, event.shift, event.sides);
    }
    guide_follow();
    mouse_capture_follow();
}

int port_window_pump(void)
{
    port_window_input_lock();
    if (g_w.open) {
        window_mode_follow();
        port_window_poll_input();
    }
    const int keep_open = g_w.open && !__atomic_load_n(&g_w.close_requested, __ATOMIC_ACQUIRE);
    port_window_input_unlock();
    return keep_open;
}

int port_window_is_focused(void)
{
    return !__atomic_load_n(&g_w.open, __ATOMIC_ACQUIRE) || __atomic_load_n(&g_w.focused, __ATOMIC_ACQUIRE);
}

void port_window_close(void)
{
    port_window_input_lock();
    if (!g_w.open) {
        port_window_input_unlock();
        return;
    }
    port_input_keyboard_attach(0);
    __atomic_store_n(&g_w.open, 0, __ATOMIC_RELEASE);
    if (g_w.captured == 1 && g_w.ClipCursor != 0)
        g_w.ClipCursor(0); // the pointer free again
    g_w.captured = 0;
    __atomic_store_n(&g_w.cursor_hidden, 0, __ATOMIC_RELEASE);
    // DestroyWindow works for the window's own thread only: the window thread destroys it and ends
    g_w.PostMessageA(g_w.window, W_WM_APP_CLOSE, 0, 0);
    host_WaitForSingleObject(g_w.thread, 2000);
    host_CloseHandle(g_w.thread);
    g_w.thread = 0;
    __atomic_store_n(&g_w.window, 0, __ATOMIC_RELEASE);
    __atomic_store_n(&g_w.focused, 0, __ATOMIC_RELEASE);
    port_window_input_unlock();
}
