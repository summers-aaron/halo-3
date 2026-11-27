// port/sdk/xam (Windows host): the guide's window (guide_host.h) through user32.dll and gdi32.dll, loaded at run time
// (the port links only kernel32 and msvcrt: port/build/link-stage1.sh), with kernel32's host imports
// (port/sdk/kernel/win32_host.h), as the virtual keyboard's dialog (port/sdk/online/keyboard_ui_windows.c).
//
// The window is one of its own, centred over the game window and kept above it (topmost: an owned window would tie its
// input queue to the game thread's), made and run on a thread of its own so the game's frames go on while the guide
// shows. It paints the guide's panes side by side (one column a controller), each with its controller and player, the
// page's title and text, a text field when the page has one, the list (the selected item highlighted, the focused
// pane's brightest; the list scrolls to keep it in view) and the hint; the keys along the bottom. guide_host_wake posts
// the newest window a message, and a timer catches the rest (a window whose guide closed, or was replaced, goes). Keys:
//   Up, Down, Page Up, Page Down   the selection       Left, Right, Tab, Shift+Tab   the pane the keys drive
//   Enter, Space                   choose              Escape                        back (closing from a first page)
//   Backspace                      erase / back        Home, the window's close button  close the guide
//   letters, digits, space         typed into a text field
//   Ctrl+V, Shift+Insert           the clipboard into a text field (clipboard.h)
// The mouse: a click chooses the item under it, the wheel moves the selection. The font is the system's message font
// (NONCLIENTMETRICS), DEFAULT_GUI_FONT without it.
#ifdef _WIN32
#include "guide.h"
#include "guide_host.h"
#include "../kernel/win32_host.h"
#include "../runtime/port_runtime.h"
#include "../runtime/port_window.h"
#include "../input/kbm_bindings.h"

int guide_view_get_osk(guide_view *view, const char **grids, int *selected);
const char *guide_osk_label(char key, int compact, char literal[2]);

PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
PORT_HOST_IMPORT(void *, GetModuleHandleA, 4, (const char *name));
PORT_HOST_IMPORT(void *, CreateThread, 24,
    (void *security, __SIZE_TYPE__ stack_size, unsigned long(__stdcall *start)(void *), void *parameter,
        unsigned long flags, unsigned long *thread_id));
PORT_HOST_IMPORT(int, CloseHandle, 4, (void *handle));
PORT_HOST_IMPORT(void *, HeapAlloc, 12, (void *heap, unsigned long flags, __SIZE_TYPE__ bytes));
PORT_HOST_IMPORT(int, HeapFree, 12, (void *heap, unsigned long flags, void *memory));
PORT_HOST_IMPORT(void *, GetProcessHeap, 0, (void));

typedef unsigned short utf16;
typedef __UINTPTR_TYPE__ w_wparam;
typedef __INTPTR_TYPE__ w_lparam;
typedef __INTPTR_TYPE__ w_lresult;
typedef w_lresult(__stdcall *w_procedure)(void *, unsigned int, w_wparam, w_lparam);

// WNDCLASSEXW, RECT, MSG, CREATESTRUCTW (its first field), PAINTSTRUCT, SIZE, POINT, LOGFONTW, NONCLIENTMETRICSW
// (winuser.h, wingdi.h), 32-bit and 64-bit.
typedef struct {
    unsigned int cbSize, style;
    w_procedure lpfnWndProc;
    int cbClsExtra, cbWndExtra;
    void *hInstance, *hIcon, *hCursor, *hbrBackground;
    const utf16 *lpszMenuName, *lpszClassName;
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
typedef struct {
    void *lpCreateParams;
} w_create;
typedef struct {
    void *hdc;
    int fErase;
    w_rect rcPaint;
    int fRestore, fIncUpdate;
    unsigned char rgbReserved[32];
} w_paint;
typedef struct {
    long cx, cy;
} w_size;
typedef struct {
    long x, y;
} w_point;
typedef struct {
    long lfHeight, lfWidth, lfEscapement, lfOrientation, lfWeight;
    unsigned char lfItalic, lfUnderline, lfStrikeOut, lfCharSet, lfOutPrecision, lfClipPrecision, lfQuality,
        lfPitchAndFamily;
    utf16 lfFaceName[32];
} w_logfont;
typedef struct {
    unsigned int cbSize;
    int iBorderWidth, iScrollWidth, iScrollHeight, iCaptionWidth, iCaptionHeight;
    w_logfont lfCaptionFont;
    int iSmCaptionWidth, iSmCaptionHeight;
    w_logfont lfSmCaptionFont;
    int iMenuWidth, iMenuHeight;
    w_logfont lfMenuFont, lfStatusFont, lfMessageFont;
    int iPaddedBorderWidth;
} w_nonclient_metrics;
typedef char w_logfont_size[sizeof(w_logfont) == 92 ? 1 : -1];
typedef char w_nonclient_metrics_size[sizeof(w_nonclient_metrics) == 504 ? 1 : -1];
typedef char w_paint_size[sizeof(w_paint) == (sizeof(void *) == 8 ? 72 : 64) ? 1 : -1];

#define W_WS_POPUP 0x80000000ul
#define W_WS_CAPTION 0x00C00000ul
#define W_WS_SYSMENU 0x00080000ul
#define W_WS_EX_DLGMODALFRAME 0x00000001ul
#define W_WS_EX_TOPMOST 0x00000008ul
#define W_WS_EX_APPWINDOW 0x00040000ul
#define W_WM_DESTROY 0x0002
#define W_WM_PAINT 0x000F
#define W_WM_CLOSE 0x0010
#define W_WM_ERASEBKGND 0x0014
#define W_WM_NCCREATE 0x0081
#define W_WM_KEYDOWN 0x0100
#define W_WM_KEYUP 0x0101
#define W_WM_SYSKEYDOWN 0x0104
#define W_WM_CHAR 0x0102
#define W_WM_KILLFOCUS 0x0008
#define W_WM_TIMER 0x0113
#define W_WM_LBUTTONDOWN 0x0201
#define W_WM_MOUSEWHEEL 0x020A
#define W_WM_RBUTTONDOWN 0x0204
#define W_WM_MBUTTONDOWN 0x0207
#define W_WM_XBUTTONDOWN 0x020B
#define W_WM_APP 0x8000
#define W_GWL_USERDATA (-21)
#define W_SW_SHOW 5
#define W_SPI_GETNONCLIENTMETRICS 0x0029u
#define W_DEFAULT_GUI_FONT 17
#define W_SM_CXSCREEN 0
#define W_SM_CYSCREEN 1
#define W_IDC_ARROW ((const utf16 *)32512)
#define W_TRANSPARENT 1
#define W_SRCCOPY 0x00CC0020ul
#define W_DT_LEFT 0x0000u
#define W_DT_CENTER 0x0001u
#define W_DT_RIGHT 0x0002u
#define W_DT_WORDBREAK 0x0010u
#define W_DT_SINGLELINE 0x0020u
#define W_DT_NOPREFIX 0x0800u
#define W_DT_CALCRECT 0x0400u
#define W_DT_END_ELLIPSIS 0x8000u
#define W_DT_EDITCONTROL 0x2000u
#define W_VK_BACK 0x08
#define W_VK_TAB 0x09
#define W_VK_RETURN 0x0D
#define W_VK_SHIFT 0x10
#define W_VK_ESCAPE 0x1B
#define W_VK_PRIOR 0x21
#define W_VK_NEXT 0x22
#define W_VK_HOME 0x24
#define W_VK_INSERT 0x2d
#define W_VK_LEFT 0x25
#define W_VK_UP 0x26
#define W_VK_RIGHT 0x27
#define W_VK_DOWN 0x28
#define W_RGB(rgb) ((unsigned long)(((rgb) >> 16 & 0xff) | ((rgb) & 0xff00) | ((rgb) & 0xff) << 16))

static struct {
    unsigned short(__stdcall *RegisterClassExW)(const w_class *);
    void *(__stdcall *CreateWindowExW)(unsigned long, const utf16 *, const utf16 *, unsigned long, int, int, int, int,
                                       void *, void *, void *, void *);
    w_lresult(__stdcall *DefWindowProcW)(void *, unsigned int, w_wparam, w_lparam);
    int(__stdcall *DestroyWindow)(void *);
    int(__stdcall *ShowWindow)(void *, int);
    int(__stdcall *GetMessageW)(w_message *, void *, unsigned int, unsigned int);
    int(__stdcall *TranslateMessage)(const w_message *);
    w_lresult(__stdcall *DispatchMessageW)(const w_message *);
    void(__stdcall *PostQuitMessage)(int);
    int(__stdcall *PostMessageW)(void *, unsigned int, w_wparam, w_lparam);
    int(__stdcall *SetForegroundWindow)(void *);
    int(__stdcall *GetWindowRect)(void *, w_rect *);
    int(__stdcall *ScreenToClient)(void *, w_point *);
    int(__stdcall *AdjustWindowRectEx)(w_rect *, unsigned long, int, unsigned long);
    int(__stdcall *GetSystemMetrics)(int);
    w_lparam(__stdcall *SetWindowLongPtrW)(void *, int, w_lparam);
    w_lparam(__stdcall *GetWindowLongPtrW)(void *, int);
    void *(__stdcall *LoadCursorW)(void *, const utf16 *);
    int(__stdcall *SystemParametersInfoW)(unsigned int, unsigned int, void *, unsigned int);
    void *(__stdcall *BeginPaint)(void *, w_paint *);
    int(__stdcall *EndPaint)(void *, const w_paint *);
    int(__stdcall *InvalidateRect)(void *, const w_rect *, int);
    int(__stdcall *FillRect)(void *, const w_rect *, void *);
    int(__stdcall *FrameRect)(void *, const w_rect *, void *);
    int(__stdcall *DrawTextW)(void *, const utf16 *, int, w_rect *, unsigned int);
    w_wparam(__stdcall *SetTimer)(void *, w_wparam, unsigned int, void *);
    short(__stdcall *GetKeyState)(int);
    void *(__stdcall *GetStockObject)(int);
    void *(__stdcall *SelectObject)(void *, void *);
    void *(__stdcall *CreateSolidBrush)(unsigned long);
    int(__stdcall *DeleteObject)(void *);
    unsigned long(__stdcall *SetTextColor)(void *, unsigned long);
    int(__stdcall *SetBkMode)(void *, int);
    void *(__stdcall *CreateCompatibleDC)(void *);
    void *(__stdcall *CreateCompatibleBitmap)(void *, int, int);
    int(__stdcall *BitBlt)(void *, int, int, int, int, void *, int, int, unsigned long);
    int(__stdcall *DeleteDC)(void *);
    int(__stdcall *GetTextExtentPoint32W)(void *, const utf16 *, int, w_size *);
    void *(__stdcall *CreateFontIndirectW)(const w_logfont *); // optional
    void *instance;
    volatile long state; // 0 not loaded, 1 loading, 2 loaded, 3 unavailable
} g_wg;

static const utf16 k_class[] = u"halo3_port_guide";

static int guide_w_load(void)
{
    long expected = 0;
    if (__atomic_compare_exchange_n(&g_wg.state, &expected, 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        long state = 3;
        void *user32 = host_LoadLibraryA("user32.dll"), *gdi32 = host_LoadLibraryA("gdi32.dll");
        int loaded = user32 != 0 && gdi32 != 0;
        if (!loaded)
            port_write_error("guide: user32.dll or gdi32.dll is not available\n");
#define LOAD(module, name)                                                                                          \
    do {                                                                                                            \
        if (loaded) {                                                                                               \
            *(void **)&g_wg.name = host_GetProcAddress(module, #name);                                              \
            if (g_wg.name == 0) {                                                                                   \
                port_write_error("guide: " #module ".dll has no " #name "\n");                                      \
                loaded = 0;                                                                                         \
            }                                                                                                       \
        }                                                                                                           \
    } while (0)
        LOAD(user32, RegisterClassExW);
        LOAD(user32, CreateWindowExW);
        LOAD(user32, DefWindowProcW);
        LOAD(user32, DestroyWindow);
        LOAD(user32, ShowWindow);
        LOAD(user32, GetMessageW);
        LOAD(user32, TranslateMessage);
        LOAD(user32, DispatchMessageW);
        LOAD(user32, PostQuitMessage);
        LOAD(user32, PostMessageW);
        LOAD(user32, SetForegroundWindow);
        LOAD(user32, GetWindowRect);
        LOAD(user32, ScreenToClient);
        LOAD(user32, AdjustWindowRectEx);
        LOAD(user32, GetSystemMetrics);
#ifdef _WIN64
        LOAD(user32, SetWindowLongPtrW);
        LOAD(user32, GetWindowLongPtrW);
#else
        if (loaded) {
            *(void **)&g_wg.SetWindowLongPtrW = host_GetProcAddress(user32, "SetWindowLongW");
            *(void **)&g_wg.GetWindowLongPtrW = host_GetProcAddress(user32, "GetWindowLongW");
            loaded = g_wg.SetWindowLongPtrW != 0 && g_wg.GetWindowLongPtrW != 0;
        }
#endif
        LOAD(user32, LoadCursorW);
        LOAD(user32, SystemParametersInfoW);
        LOAD(user32, BeginPaint);
        LOAD(user32, EndPaint);
        LOAD(user32, InvalidateRect);
        LOAD(user32, FillRect);
        LOAD(user32, FrameRect);
        LOAD(user32, DrawTextW);
        LOAD(user32, SetTimer);
        LOAD(user32, GetKeyState);
        LOAD(gdi32, GetStockObject);
        LOAD(gdi32, SelectObject);
        LOAD(gdi32, CreateSolidBrush);
        LOAD(gdi32, DeleteObject);
        LOAD(gdi32, SetTextColor);
        LOAD(gdi32, SetBkMode);
        LOAD(gdi32, CreateCompatibleDC);
        LOAD(gdi32, CreateCompatibleBitmap);
        LOAD(gdi32, BitBlt);
        LOAD(gdi32, DeleteDC);
        LOAD(gdi32, GetTextExtentPoint32W);
#undef LOAD
        if (loaded) {
            *(void **)&g_wg.CreateFontIndirectW = host_GetProcAddress(gdi32, "CreateFontIndirectW");
            g_wg.instance = host_GetModuleHandleA(0);
            state = 2;
        }
        __atomic_store_n(&g_wg.state, state, __ATOMIC_RELEASE);
    }
    while (__atomic_load_n(&g_wg.state, __ATOMIC_ACQUIRE) == 1)
        __builtin_ia32_pause();
    return __atomic_load_n(&g_wg.state, __ATOMIC_ACQUIRE) == 2;
}

// ---- the window ----

#define GUIDE_MARGIN 14
#define GUIDE_PAD 6
#define GUIDE_PANE_WIDTH 300
#define GUIDE_SINGLE_WIDTH 560
#define GUIDE_HEIGHT 540
#define GUIDE_BODY_LINES 6
#define GUIDE_HINT_LINES 4
#define GUIDE_TIMER_MS 250

typedef struct guide_window {
    unsigned long session;
    void *window, *font;
    int owns_font;
    int width, height, line_height;
    guide_view view;
    const char *osk[GUIDE_PANES];
    int osk_selected[GUIDE_PANES], osk_columns;
    unsigned long drawn;
    struct {
        int x, width, rows_y, row_height, first, rows;
    } layout[GUIDE_PANES];
} guide_window;

static void *volatile g_newest; // the newest window, which guide_host_wake posts to

static const unsigned long k_background = 0x16191d, k_panel = 0x22272d, k_foreground = 0xf2f2f2, k_dim = 0xa4acb4,
                           k_disabled = 0x646b72, k_accent = 0x6cc417, k_accent_dim = 0x3c4650, k_field = 0x0d0f12;

// UTF-8 into UTF-16 (to has `capacity` units, NUL-terminated); returns the units written.
static int guide_utf16(const char *from, utf16 *to, int capacity)
{
    const unsigned char *s = (const unsigned char *)from;
    int n = 0;
    while (*s != 0 && n + 2 < capacity) {
        unsigned long code = *s, extra = code >= 0xf0 ? 3 : code >= 0xe0 ? 2 : code >= 0xc0 ? 1 : 0;
        code &= extra == 3 ? 0x07 : extra == 2 ? 0x0f : extra == 1 ? 0x1f : 0x7f;
        s++;
        for (unsigned long k = 0; k < extra && (*s & 0xc0) == 0x80; k++)
            code = code << 6 | (*s++ & 0x3f);
        if (code >= 0x10000) {
            code -= 0x10000;
            to[n++] = (utf16)(0xd800 | (code >> 10));
            to[n++] = (utf16)(0xdc00 | (code & 0x3ff));
        } else {
            to[n++] = (utf16)code;
        }
    }
    to[n] = 0;
    return n;
}

static void guide_fill(void *dc, unsigned long rgb, int x, int y, int width, int height)
{
    void *brush = g_wg.CreateSolidBrush(W_RGB(rgb));
    w_rect rect = { x, y, x + width, y + height };
    g_wg.FillRect(dc, &rect, brush);
    g_wg.DeleteObject(brush);
}

static void guide_frame(void *dc, unsigned long rgb, int x, int y, int width, int height)
{
    void *brush = g_wg.CreateSolidBrush(W_RGB(rgb));
    w_rect rect = { x, y, x + width, y + height };
    g_wg.FrameRect(dc, &rect, brush);
    g_wg.DeleteObject(brush);
}

// Draws UTF-8 `text` in `rect` with DrawTextW `format`; returns the height drawn (or measured, W_DT_CALCRECT).
static int guide_text(void *dc, unsigned long rgb, const char *text, w_rect *rect, unsigned int format)
{
    static utf16 units[GUIDE_TEXT]; // the window threads draw one at a time (g_paint_busy)
    const int n = guide_utf16(text, units, GUIDE_TEXT);
    if (n == 0)
        return 0;
    g_wg.SetTextColor(dc, W_RGB(rgb));
    return g_wg.DrawTextW(dc, units, n, rect, format | W_DT_NOPREFIX);
}

static int guide_text_width(void *dc, const char *text)
{
    utf16 units[GUIDE_FIELD];
    const int n = guide_utf16(text, units, GUIDE_FIELD);
    w_size size = { 0, 0 };
    if (n > 0)
        g_wg.GetTextExtentPoint32W(dc, units, n, &size);
    return (int)size.cx;
}

static volatile char g_paint_busy;

static int guide_draw_osk(guide_window *w, void *dc, int pane, int left, int y, int width)
{
    const char *grid = w->osk[pane];
    const int columns = w->osk_columns, height = w->line_height + 2 * GUIDE_PAD;
    for (int i = 0; grid[i]; i++) {
        const int col = i % columns;
        const int x = left + col * width / columns, right = left + (col + 1) * width / columns;
        const int selected = i == w->osk_selected[pane];
        char literal[2];
        const char *label = guide_osk_label(grid[i], w->view.pane_count > 1, literal);
        guide_fill(dc, selected ? k_accent : k_field, x, y, right - x - 2, height - 2);
        w_rect cell = { x, y + GUIDE_PAD, right - 2, y + height - 2 };
        guide_text(dc, selected ? k_background : k_foreground, label, &cell, W_DT_CENTER | W_DT_SINGLELINE);
        if (col + 1 == columns)
            y += height;
    }
    return y + GUIDE_PAD;
}

static void guide_draw_pane(guide_window *w, void *dc, int index, int x, int width)
{
    const guide_view_pane *pane = &w->view.panes[index];
    const int inner = width - 2 * GUIDE_PAD, left = x + GUIDE_PAD, line = w->line_height;
    const int bottom = w->height - GUIDE_MARGIN - line - GUIDE_PAD;
    guide_fill(dc, k_panel, x, GUIDE_MARGIN, width, bottom - GUIDE_MARGIN);
    if (pane->focused && w->view.pane_count > 1)
        guide_frame(dc, k_accent, x, GUIDE_MARGIN, width, bottom - GUIDE_MARGIN);
    int y = GUIDE_MARGIN + GUIDE_PAD;
    w_rect rect = { left, y, left + inner, y + line };
    guide_text(dc, k_accent, pane->heading, &rect, W_DT_LEFT | W_DT_SINGLELINE);
    rect.left = left + inner / 2;
    guide_text(dc, k_dim, pane->status, &rect, W_DT_RIGHT | W_DT_SINGLELINE | W_DT_END_ELLIPSIS);
    y += line + GUIDE_PAD;
    rect.left = left;
    rect.top = y;
    rect.bottom = y + line;
    guide_text(dc, k_foreground, pane->title, &rect, W_DT_LEFT | W_DT_SINGLELINE | W_DT_END_ELLIPSIS);
    rect.left++;
    rect.right++;
    guide_text(dc, k_foreground, pane->title, &rect, W_DT_LEFT | W_DT_SINGLELINE | W_DT_END_ELLIPSIS); // bold
    rect.left--;
    rect.right--;
    y += line + GUIDE_PAD;
    if (pane->body[0] != 0) {
        w_rect body = { left, y, left + inner, y + GUIDE_BODY_LINES * line };
        w_rect measure = body;
        int height = guide_text(dc, k_dim, pane->body, &measure, W_DT_WORDBREAK | W_DT_EDITCONTROL | W_DT_CALCRECT);
        if (height > GUIDE_BODY_LINES * line)
            height = GUIDE_BODY_LINES * line;
        guide_text(dc, k_dim, pane->body, &body, W_DT_WORDBREAK | W_DT_EDITCONTROL);
        y += height + GUIDE_PAD;
    }
    if (pane->has_field) {
        const int height = line + 2 * GUIDE_PAD;
        guide_fill(dc, k_field, left, y, inner, height);
        w_rect field = { left + GUIDE_PAD, y + GUIDE_PAD, left + inner - GUIDE_PAD, y + GUIDE_PAD + line };
        const char *shown = pane->field;
        if (w->osk[index])
            while (*shown && guide_text_width(dc, shown) > inner - 2 * GUIDE_PAD - 2)
                shown++;
        guide_text(dc, k_foreground, shown, &field, W_DT_LEFT | W_DT_SINGLELINE);
        guide_fill(dc, k_accent, left + GUIDE_PAD + guide_text_width(dc, shown), y + GUIDE_PAD, 2, line);
        y += height + GUIDE_PAD;
    }
    if (w->osk[index])
        y = guide_draw_osk(w, dc, index, left, y, inner);
    int hint_height = 0;
    w_rect hint = { left, 0, left + inner, GUIDE_HINT_LINES * line };
    if (pane->hint[0] != 0) {
        w_rect measure = hint;
        hint_height = guide_text(dc, k_dim, pane->hint, &measure, W_DT_WORDBREAK | W_DT_EDITCONTROL | W_DT_CALCRECT);
        if (hint_height > GUIDE_HINT_LINES * line)
            hint_height = GUIDE_HINT_LINES * line;
    }
    const int hint_y = bottom - GUIDE_PAD - hint_height;
    const int row_height = line + 2 * GUIDE_PAD;
    int rows = (hint_y - GUIDE_PAD - y) / row_height;
    if (rows < 1)
        rows = 1;
    int first = 0;
    if (pane->selected >= rows)
        first = pane->selected - rows + 1;
    if (first + rows > pane->item_count)
        first = pane->item_count > rows ? pane->item_count - rows : 0;
    w->layout[index].x = x;
    w->layout[index].width = width;
    w->layout[index].rows_y = y;
    w->layout[index].row_height = row_height;
    w->layout[index].first = first;
    w->layout[index].rows = rows;
    const int mark = guide_text_width(dc, "* ");
    for (int row = 0; row < rows && first + row < pane->item_count; row++) {
        const guide_view_item *item = &pane->items[first + row];
        const int top = y + row * row_height, selected = first + row == pane->selected;
        if (selected)
            guide_fill(dc, pane->focused ? k_accent : k_accent_dim, left, top, inner, row_height - 2);
        const unsigned long color = !item->enabled ? k_disabled : selected ? k_background : k_foreground;
        w_rect text = { left + GUIDE_PAD, top + GUIDE_PAD, left + inner - GUIDE_PAD, top + GUIDE_PAD + line };
        if (item->checked)
            guide_text(dc, color, "*", &text, W_DT_LEFT | W_DT_SINGLELINE);
        // the text first, the detail (right-aligned) in the room it leaves; either cut with an ellipsis
        text.left += mark;
        const int room = text.right - text.left, label = guide_text_width(dc, item->text);
        const int detail_room = room - (label < room ? label : room) - GUIDE_PAD;
        if (item->detail[0] != 0 && detail_room >= guide_text_width(dc, "W...")) {
            w_rect detail = text;
            detail.left = detail.right - detail_room;
            guide_text(dc, color, item->detail, &detail, W_DT_RIGHT | W_DT_SINGLELINE | W_DT_END_ELLIPSIS);
            const int detail_width = guide_text_width(dc, item->detail);
            text.right -= (detail_width < detail_room ? detail_width : detail_room) + GUIDE_PAD;
        }
        guide_text(dc, color, item->text, &text, W_DT_LEFT | W_DT_SINGLELINE | W_DT_END_ELLIPSIS);
    }
    w_rect arrow = { left, y - GUIDE_PAD - line / 2, left + inner, y - GUIDE_PAD + line / 2 };
    if (first > 0)
        guide_text(dc, k_dim, "^", &arrow, W_DT_RIGHT | W_DT_SINGLELINE);
    arrow.top = y + rows * row_height - line / 2;
    arrow.bottom = arrow.top + line;
    if (first + rows < pane->item_count)
        guide_text(dc, k_dim, "v", &arrow, W_DT_RIGHT | W_DT_SINGLELINE);
    if (pane->hint[0] != 0) {
        hint.top = hint_y;
        hint.bottom = hint_y + hint_height;
        guide_text(dc, k_dim, pane->hint, &hint, W_DT_WORDBREAK | W_DT_EDITCONTROL);
    }
}

static void guide_paint(guide_window *w, void *target)
{
    while (__atomic_test_and_set(&g_paint_busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
    void *dc = g_wg.CreateCompatibleDC(target);
    void *bitmap = g_wg.CreateCompatibleBitmap(target, w->width, w->height);
    void *previous_bitmap = g_wg.SelectObject(dc, bitmap);
    void *previous_font = g_wg.SelectObject(dc, w->font);
    g_wg.SetBkMode(dc, W_TRANSPARENT);
    guide_fill(dc, k_background, 0, 0, w->width, w->height);
    const int count = w->view.pane_count > 0 ? w->view.pane_count : 1;
    const int width = (w->width - 2 * GUIDE_MARGIN - (count - 1) * GUIDE_PAD) / count;
    for (int i = 0; i < w->view.pane_count; i++)
        guide_draw_pane(w, dc, i, GUIDE_MARGIN + i * (width + GUIDE_PAD), width);
    w_rect keys = { GUIDE_MARGIN, w->height - GUIDE_MARGIN - w->line_height, w->width - GUIDE_MARGIN,
                    w->height - GUIDE_MARGIN };
    guide_text(dc, k_dim,
               w->view.pane_count > 1 ? "Enter: Select   Esc: Back   Tab: Next controller   Home: Close"
                                      : "Enter: Select   Esc: Back   Home: Close",
               &keys, W_DT_LEFT | W_DT_SINGLELINE);
    g_wg.BitBlt(target, 0, 0, w->width, w->height, dc, 0, 0, W_SRCCOPY);
    g_wg.SelectObject(dc, previous_font);
    g_wg.SelectObject(dc, previous_bitmap);
    g_wg.DeleteObject(bitmap);
    g_wg.DeleteDC(dc);
    __atomic_clear(&g_paint_busy, __ATOMIC_RELEASE);
}

// Whether the guide this window shows is still open: if not, the window goes.
static int guide_window_current(guide_window *w)
{
    w->osk_columns = guide_view_get_osk(&w->view, w->osk, w->osk_selected);
    if (!w->view.open || w->view.session != w->session) {
        g_wg.DestroyWindow(w->window);
        return 0;
    }
    if (w->view.revision != w->drawn)
        g_wg.InvalidateRect(w->window, 0, 0);
    return 1;
}

// The pane under client x, -1 when none.
static int guide_window_pane_at(const guide_window *w, int x)
{
    for (int i = 0; i < w->view.pane_count; i++)
        if (x >= w->layout[i].x && x < w->layout[i].x + w->layout[i].width)
            return i;
    return -1;
}

static void guide_window_click(guide_window *w, int x, int y)
{
    const int i = guide_window_pane_at(w, x);
    if (i < 0)
        return;
    const int row = y >= w->layout[i].rows_y ? (y - w->layout[i].rows_y) / w->layout[i].row_height : -1;
    const int item = row >= 0 && row < w->layout[i].rows ? w->layout[i].first + row : -1;
    guide_click(i, item < w->view.panes[i].item_count ? item : -1, 1);
}

int guide_host_virtual_key(unsigned int virtual_key, int shift)
{
    switch (virtual_key) {
    case W_VK_UP: return GUIDE_KEY_UP;
    case W_VK_DOWN: return GUIDE_KEY_DOWN;
    case W_VK_LEFT: return GUIDE_KEY_LEFT;
    case W_VK_RIGHT: return GUIDE_KEY_RIGHT;
    case W_VK_PRIOR: return GUIDE_KEY_PAGE_UP;
    case W_VK_NEXT: return GUIDE_KEY_PAGE_DOWN;
    case W_VK_RETURN: return GUIDE_KEY_ACCEPT;
    case W_VK_ESCAPE: return GUIDE_KEY_BACK;
    case W_VK_BACK: return GUIDE_KEY_ERASE;
    case W_VK_TAB: return shift ? GUIDE_KEY_PREVIOUS_TAB : GUIDE_KEY_NEXT_TAB; // Tab: RB, Shift+Tab: LB
    case W_VK_HOME: return GUIDE_KEY_CLOSE;                            // the Guide button
    case W_VK_INSERT: return shift ? GUIDE_KEY_PASTE : -1;             // Shift+Insert
    default: return -1;
    }
}

// The keys above send control characters as WM_CHAR too; those are theirs. Ctrl+V's (0x16, SYN) is the paste.
int guide_host_typed(unsigned int character)
{
    if (character == 0x16)
        return GUIDE_KEY_PASTE;
    return character >= 0x20 && character < 0x7f ? GUIDE_KEY_CHARACTER : -1;
}

// The binding capture took the last key press: the character it types (WM_CHAR, next) is not the guide's (the window
// thread's).
static int g_drop_char;

static w_lresult __stdcall guide_procedure(void *window, unsigned int message, w_wparam wparam, w_lparam lparam)
{
    if (message == W_WM_NCCREATE)
        g_wg.SetWindowLongPtrW(window, W_GWL_USERDATA, (w_lparam)((const w_create *)lparam)->lpCreateParams);
    guide_window *w = (guide_window *)g_wg.GetWindowLongPtrW(window, W_GWL_USERDATA);
    if (w == 0)
        return g_wg.DefWindowProcW(window, message, wparam, lparam);
    switch (message) {
    case W_WM_PAINT: {
        w_paint paint;
        void *dc = g_wg.BeginPaint(window, &paint);
        w->drawn = w->view.revision;
        guide_paint(w, dc);
        g_wg.EndPaint(window, &paint);
        return 0;
    }
    case W_WM_ERASEBKGND:
        return 1; // painted whole, double-buffered
    case W_WM_TIMER:
    case W_WM_APP:
        guide_window_current(w);
        return 0;
    case W_WM_KEYDOWN:
    case W_WM_KEYUP: { // (lparam bit 30: the key was down already, an auto-repeat)
        g_drop_char = 0;
        // a press offered first to the binding capture (guide_capture), which takes the character it types too
        if (message == W_WM_KEYDOWN && !((w_wparam)lparam >> 30 & 1) &&
            guide_capture(port_window_key_code(wparam, lparam))) {
            g_drop_char = 1;
            guide_window_current(w);
            return 0;
        }
        const int key = guide_host_virtual_key((unsigned int)wparam, g_wg.GetKeyState(W_VK_SHIFT) < 0);
        if (key >= 0)
            guide_key_action(key, 0,
                             message == W_WM_KEYUP             ? GUIDE_KEY_RELEASED
                             : ((w_wparam)lparam >> 30 & 1)    ? GUIDE_KEY_REPEATED
                                                               : GUIDE_KEY_PRESSED);
        guide_window_current(w);
        return 0;
    }
    case W_WM_CHAR: {
        if (g_drop_char) {
            g_drop_char = 0;
            return 0;
        }
        const int key = guide_host_typed((unsigned int)wparam);
        if (key >= 0)
            guide_key_action(key, (unsigned int)wparam, ((w_wparam)lparam >> 30 & 1) ? GUIDE_KEY_REPEATED : GUIDE_KEY_PRESSED);
        guide_window_current(w);
        return 0;
    }
    case W_WM_SYSKEYDOWN: // Alt, F10, a key with Alt held: offered to the binding capture, else the system's (Alt+F4)
        if (!((w_wparam)lparam >> 30 & 1) && guide_capture(port_window_key_code(wparam, lparam))) {
            guide_window_current(w);
            return 0;
        }
        break;
    case W_WM_KILLFOCUS: // what is let go meanwhile never reaches this window: nothing stays down
        guide_keys_released();
        break;
    case W_WM_RBUTTONDOWN:
    case W_WM_MBUTTONDOWN:
    case W_WM_XBUTTONDOWN: // only the binding capture has a use for these here
        guide_capture(message == W_WM_RBUTTONDOWN   ? PORT_KBM_MOUSE(k_port_mouse_right)
                      : message == W_WM_MBUTTONDOWN ? PORT_KBM_MOUSE(k_port_mouse_middle)
                      : (wparam >> 16) == 1         ? PORT_KBM_MOUSE(k_port_mouse_4)
                                                    : PORT_KBM_MOUSE(k_port_mouse_5));
        guide_window_current(w);
        return message == W_WM_XBUTTONDOWN;
    case W_WM_LBUTTONDOWN:
        if (guide_capture(PORT_KBM_MOUSE(k_port_mouse_left))) {
            guide_window_current(w);
            return 0;
        }
        guide_window_click(w, (short)(lparam & 0xffff), (short)((unsigned long)lparam >> 16));
        guide_window_current(w);
        return 0;
    case W_WM_MOUSEWHEEL: {
        if ((short)(wparam >> 16) != 0 &&
            guide_capture((short)(wparam >> 16) > 0 ? PORT_KBM_WHEEL_UP : PORT_KBM_WHEEL_DOWN)) {
            guide_window_current(w);
            return 0;
        }
        // the pane under the pointer (its position in screen coordinates) scrolls
        w_point point = { (short)(lparam & 0xffff), (short)((unsigned long)lparam >> 16) };
        const int pane = g_wg.ScreenToClient(window, &point) ? guide_window_pane_at(w, (int)point.x) : -1;
        if (pane >= 0)
            guide_click(pane, -1, 0);
        guide_key((short)(wparam >> 16) > 0 ? GUIDE_KEY_SCROLL_UP : GUIDE_KEY_SCROLL_DOWN, 0);
        guide_window_current(w);
        return 0;
    }
    case W_WM_CLOSE:
        guide_key(GUIDE_KEY_CLOSE, 0); // the guide closes; the timer or the wake takes the window down
        guide_window_current(w);
        return 0;
    case W_WM_DESTROY:
        __atomic_compare_exchange_n(&g_newest, &(void *){ window }, (void *)0, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
        g_wg.PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    return g_wg.DefWindowProcW(window, message, wparam, lparam);
}

static int guide_register(void)
{
    static volatile long registered; // 0 not yet, 1 registering, 2 registered, 3 failed
    long expected = 0;
    if (__atomic_compare_exchange_n(&registered, &expected, 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
        w_class wc;
        __builtin_memset(&wc, 0, sizeof(wc));
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = guide_procedure;
        wc.hInstance = g_wg.instance;
        wc.hCursor = g_wg.LoadCursorW(0, W_IDC_ARROW);
        wc.lpszClassName = k_class;
        const int ok = g_wg.RegisterClassExW(&wc) != 0;
        if (!ok)
            port_write_error("guide: RegisterClassExW failed\n");
        __atomic_store_n(&registered, ok ? 2 : 3, __ATOMIC_RELEASE);
    }
    while (__atomic_load_n(&registered, __ATOMIC_ACQUIRE) == 1)
        __builtin_ia32_pause();
    return __atomic_load_n(&registered, __ATOMIC_ACQUIRE) == 2;
}

// Makes the window on the calling thread (the window's): 1 on success.
static int guide_window_create(guide_window *w)
{
    w_nonclient_metrics metrics;
    __builtin_memset(&metrics, 0, sizeof(metrics));
    metrics.cbSize = sizeof(metrics);
    if (g_wg.CreateFontIndirectW != 0 &&
        g_wg.SystemParametersInfoW(W_SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0))
        w->font = g_wg.CreateFontIndirectW(&metrics.lfMessageFont);
    w->owns_font = w->font != 0;
    if (w->font == 0)
        w->font = g_wg.GetStockObject(W_DEFAULT_GUI_FONT);
    // a line's height in that font
    void *dc = g_wg.CreateCompatibleDC(0);
    void *previous = g_wg.SelectObject(dc, w->font);
    w_size size = { 0, 16 };
    g_wg.GetTextExtentPoint32W(dc, u"Ag", 2, &size);
    g_wg.SelectObject(dc, previous);
    g_wg.DeleteDC(dc);
    w->line_height = size.cy > 0 ? (int)size.cy : 16;
    w->width = w->view.pane_count > 1 ? 2 * GUIDE_MARGIN + w->view.pane_count * (GUIDE_PANE_WIDTH + GUIDE_PAD)
                                      : GUIDE_SINGLE_WIDTH;
    w->height = GUIDE_HEIGHT;

    const unsigned long style = W_WS_POPUP | W_WS_CAPTION | W_WS_SYSMENU;
    const unsigned long ex_style = W_WS_EX_DLGMODALFRAME | W_WS_EX_TOPMOST | W_WS_EX_APPWINDOW;
    w_rect frame = { 0, 0, w->width, w->height };
    g_wg.AdjustWindowRectEx(&frame, style, 0, ex_style);
    const int width = (int)(frame.right - frame.left), height = (int)(frame.bottom - frame.top);
    w_rect over = { 0, 0, g_wg.GetSystemMetrics(W_SM_CXSCREEN), g_wg.GetSystemMetrics(W_SM_CYSCREEN) };
    void *game = (void *)port_window_native_window();
    if (game != 0)
        g_wg.GetWindowRect(game, &over);
    const int x = (int)(over.left + over.right) / 2 - width / 2, top = (int)(over.top + over.bottom) / 2 - height / 2;
    w->window = g_wg.CreateWindowExW(ex_style, k_class, u"System Guide", style, x, top, width, height, 0, 0,
                                     g_wg.instance, w);
    if (w->window == 0) {
        port_write_error("guide: CreateWindowExW failed\n");
        return 0;
    }
    g_wg.SetTimer(w->window, 1, GUIDE_TIMER_MS, 0);
    __atomic_store_n(&g_newest, w->window, __ATOMIC_RELEASE);
    g_wg.ShowWindow(w->window, W_SW_SHOW);
    g_wg.SetForegroundWindow(w->window);
    return 1;
}

static unsigned long __stdcall guide_window_main(void *argument)
{
    guide_window *w = (guide_window *)argument;
    if (guide_window_create(w)) {
        w_message message;
        while (g_wg.GetMessageW(&message, 0, 0, 0) > 0) {
            g_wg.TranslateMessage(&message);
            g_wg.DispatchMessageW(&message);
        }
    }
    if (w->owns_font)
        g_wg.DeleteObject(w->font); // the window that used it is gone
    host_HeapFree(host_GetProcessHeap(), 0, w);
    return 0;
}

void guide_host_wake(void)
{
    void *window = __atomic_load_n(&g_newest, __ATOMIC_ACQUIRE);
    if (window != 0)
        g_wg.PostMessageW(window, W_WM_APP, 0, 0);
}

int guide_host_show(unsigned long session)
{
    if (port_window_native_window() == 0)
        return 0; // no game window (HALO3_WINDOW=0): nobody would see the guide
    if (!guide_w_load() || !guide_register())
        return 0;
    const unsigned long zeroed = 8; // HEAP_ZERO_MEMORY
    guide_window *w = (guide_window *)host_HeapAlloc(host_GetProcessHeap(), zeroed, sizeof(guide_window));
    if (w == 0)
        return 0;
    w->session = session;
    w->osk_columns = guide_view_get_osk(&w->view, w->osk, w->osk_selected); // the opening panes set its width
    if (!w->view.open || w->view.session != session) {
        host_HeapFree(host_GetProcessHeap(), 0, w);
        return 0;
    }
    void *thread = host_CreateThread(0, 0, guide_window_main, w, 0, 0);
    if (thread == 0) {
        port_write_error("guide: cannot start the window's thread\n");
        host_HeapFree(host_GetProcessHeap(), 0, w);
        return 0;
    }
    host_CloseHandle(thread);
    return 1;
}

// ---- the guide's worker threads (guide_host.h) ----

typedef struct {
    void (*entry)(void *argument);
    void *argument;
} guide_thread_start;

static unsigned long __stdcall guide_thread_main(void *start_argument)
{
    guide_thread_start start = *(guide_thread_start *)start_argument;
    host_HeapFree(host_GetProcessHeap(), 0, start_argument);
    start.entry(start.argument);
    return 0;
}

int guide_host_thread_start(void (*entry)(void *argument), void *argument)
{ // (kernel32 only: no window library needed)
    guide_thread_start *start =
        (guide_thread_start *)host_HeapAlloc(host_GetProcessHeap(), 0, sizeof(guide_thread_start));
    if (start == 0)
        return 0;
    start->entry = entry;
    start->argument = argument;
    void *thread = host_CreateThread(0, 0, guide_thread_main, start, 0, 0);
    if (thread == 0) {
        host_HeapFree(host_GetProcessHeap(), 0, start);
        return 0;
    }
    host_CloseHandle(thread);
    return 1;
}
#endif // _WIN32
