// port/sdk/online (Windows host): the virtual keyboard's dialog (keyboard_ui_host.h) through user32.dll and gdi32.dll,
// loaded at run time (the port links only kernel32 and msvcrt: port/build/link-stage1.sh), with kernel32's host
// imports (port/sdk/kernel/win32_host.h).
//
// The dialog is a window of its own, centred over the game window and kept above it (topmost: an owned window would
// tie the dialog's input queue to the game thread's), made and run on a thread of its own so the game's frames go on
// while the player types. It holds the description, a single-line edit control (EM_LIMITTEXT: at most max_length UTF-16
// units, the system beeping past it; ES_PASSWORD for VKBD_LATIN_PASSWORD) opened with the default text (selected for
// VKBD_HIGHLIGHT_TEXT, the caret at its end otherwise), and OK / Cancel; the caption is the title. IsDialogMessageW
// gives it the dialog keys: Enter is OK, Escape is Cancel, Tab moves between the controls; closing the window cancels.
// The font is the system's message font (NONCLIENTMETRICS), DEFAULT_GUI_FONT without it.
#include "keyboard_ui_host.h"
#include "../kernel/win32_host.h"
#include "../runtime/port_runtime.h"
#include "../runtime/port_window.h"

__declspec(dllimport) char *__cdecl getenv(const char *name);

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

// WNDCLASSEXW, RECT, MSG, CREATESTRUCTW (its first field), LOGFONTW, NONCLIENTMETRICSW (winuser.h, wingdi.h), 32-bit and 64-bit.
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

#define W_WS_POPUP 0x80000000ul
#define W_WS_CHILD 0x40000000ul
#define W_WS_VISIBLE 0x10000000ul
#define W_WS_CAPTION 0x00C00000ul
#define W_WS_SYSMENU 0x00080000ul
#define W_WS_TABSTOP 0x00010000ul
#define W_WS_GROUP 0x00020000ul
#define W_WS_EX_DLGMODALFRAME 0x00000001ul
#define W_WS_EX_TOPMOST 0x00000008ul
#define W_WS_EX_CLIENTEDGE 0x00000200ul
#define W_WS_EX_APPWINDOW 0x00040000ul
#define W_ES_PASSWORD 0x0020ul
#define W_ES_AUTOHSCROLL 0x0080ul
#define W_ES_READONLY 0x0800ul
#define W_BS_PUSHBUTTON 0x0000ul
#define W_BS_DEFPUSHBUTTON 0x0001ul
#define W_SS_LEFT 0x0000ul
#define W_SS_NOPREFIX 0x0080ul
#define W_WM_CREATE 0x0001
#define W_WM_DESTROY 0x0002
#define W_WM_ACTIVATE 0x0006
#define W_WM_SETFOCUS 0x0007
#define W_WM_CLOSE 0x0010
#define W_WM_SETFONT 0x0030
#define W_WM_NCCREATE 0x0081
#define W_WM_COMMAND 0x0111
#define W_DM_GETDEFID 0x0400
#define W_DC_HASDEFID 0x534B
#define W_EM_SETSEL 0x00B1
#define W_EM_LIMITTEXT 0x00C5
#define W_IDOK 1
#define W_IDCANCEL 2
#define W_ID_EDIT 100
#define W_GWL_USERDATA (-21)
#define W_SW_SHOW 5
#define W_COLOR_BTNFACE 15
#define W_DT_WORDBREAK 0x0010u
#define W_DT_CALCRECT 0x0400u
#define W_DT_NOPREFIX 0x0800u
#define W_DT_EDITCONTROL 0x2000u
#define W_SPI_GETNONCLIENTMETRICS 0x0029u
#define W_DEFAULT_GUI_FONT 17
#define W_SM_CXSCREEN 0
#define W_SM_CYSCREEN 1
#define W_IDC_ARROW ((const utf16 *)32512)

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
    int(__stdcall *IsDialogMessageW)(void *, w_message *);
    void(__stdcall *PostQuitMessage)(int);
    w_lresult(__stdcall *SendMessageW)(void *, unsigned int, w_wparam, w_lparam);
    void *(__stdcall *SetFocus)(void *);
    int(__stdcall *SetForegroundWindow)(void *);
    int(__stdcall *GetWindowRect)(void *, w_rect *);
    int(__stdcall *AdjustWindowRectEx)(w_rect *, unsigned long, int, unsigned long);
    int(__stdcall *GetSystemMetrics)(int);
    void *(__stdcall *GetDC)(void *);
    int(__stdcall *ReleaseDC)(void *, void *);
    int(__stdcall *DrawTextW)(void *, const utf16 *, int, w_rect *, unsigned int);
    int(__stdcall *GetWindowTextW)(void *, utf16 *, int);
    w_lparam(__stdcall *SetWindowLongPtrW)(void *, int, w_lparam);
    w_lparam(__stdcall *GetWindowLongPtrW)(void *, int);
    void *(__stdcall *GetDlgItem)(void *, int);
    void *(__stdcall *LoadCursorW)(void *, const utf16 *);
    int(__stdcall *SystemParametersInfoW)(unsigned int, unsigned int, void *, unsigned int);
    void *(__stdcall *GetStockObject)(int);
    void *(__stdcall *SelectObject)(void *, void *);
    void *(__stdcall *CreateFontIndirectW)(const w_logfont *); // optional, with DeleteObject
    int(__stdcall *DeleteObject)(void *);
    void *instance;
    int registered;
} g_wk;

static const utf16 k_class[] = u"halo3_port_keyboard";

typedef struct kbd_dialog {
    keyboard_ui_host_request *request;
    void *window, *edit, *font;
    int owns_font; // font is the message font, made for this dialog (not a stock object)
    int accepted;
} kbd_dialog;

// The edit control's text into the request: at most max_length units, never ending in half a surrogate pair.
static void kbd_take_text(kbd_dialog *dialog)
{
    keyboard_ui_host_request *request = dialog->request;
    int length = g_wk.GetWindowTextW(dialog->edit, request->text, (int)request->max_length + 1);
    if (length < 0)
        length = 0;
    if ((unsigned long)length > request->max_length)
        length = (int)request->max_length;
    if (length > 0 && request->text[length - 1] >= 0xd800 && request->text[length - 1] <= 0xdbff)
        length--;
    request->text[length] = 0;
}

static w_lresult __stdcall kbd_procedure(void *window, unsigned int message, w_wparam w, w_lparam l)
{
    if (message == W_WM_NCCREATE)
        g_wk.SetWindowLongPtrW(window, W_GWL_USERDATA, (w_lparam)((const w_create *)l)->lpCreateParams);
    kbd_dialog *dialog = (kbd_dialog *)g_wk.GetWindowLongPtrW(window, W_GWL_USERDATA);
    switch (message) {
    case W_DM_GETDEFID:
        return (w_lresult)((unsigned long)W_DC_HASDEFID << 16 | W_IDOK);
    case W_WM_ACTIVATE:
        if ((w & 0xffff) != 0 && dialog != 0 && dialog->edit != 0)
            g_wk.SetFocus(dialog->edit); // back to the text whenever the dialog is activated
        return 0;
    case W_WM_COMMAND:
        if (dialog != 0 && ((w & 0xffff) == W_IDOK || (w & 0xffff) == W_IDCANCEL)) {
            dialog->accepted = (w & 0xffff) == W_IDOK;
            if (dialog->accepted)
                kbd_take_text(dialog);
            g_wk.DestroyWindow(window);
            return 0;
        }
        break;
    case W_WM_CLOSE:
        if (dialog != 0)
            dialog->accepted = 0;
        g_wk.DestroyWindow(window);
        return 0;
    case W_WM_DESTROY:
        g_wk.PostQuitMessage(0);
        return 0;
    default:
        break;
    }
    return g_wk.DefWindowProcW(window, message, w, l);
}

static int kbd_load(void)
{
    if (g_wk.registered)
        return 1;
    void *user32 = host_LoadLibraryA("user32.dll"), *gdi32 = host_LoadLibraryA("gdi32.dll");
    if (user32 == 0 || gdi32 == 0) {
        port_write_error("keyboard ui: user32.dll or gdi32.dll is not available\n");
        return 0;
    }
#define LOAD(module, name)                                                                                          \
    do {                                                                                                            \
        *(void **)&g_wk.name = host_GetProcAddress(module, #name);                                                  \
        if (g_wk.name == 0) {                                                                                       \
            port_write_error("keyboard ui: " #module ".dll has no " #name "\n");                                    \
            return 0;                                                                                               \
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
    LOAD(user32, IsDialogMessageW);
    LOAD(user32, PostQuitMessage);
    LOAD(user32, SendMessageW);
    LOAD(user32, SetFocus);
    LOAD(user32, SetForegroundWindow);
    LOAD(user32, GetWindowRect);
    LOAD(user32, AdjustWindowRectEx);
    LOAD(user32, GetSystemMetrics);
    LOAD(user32, GetDC);
    LOAD(user32, ReleaseDC);
    LOAD(user32, DrawTextW);
    LOAD(user32, GetWindowTextW);
#ifdef _WIN64
    LOAD(user32, SetWindowLongPtrW);
    LOAD(user32, GetWindowLongPtrW);
#else
    *(void **)&g_wk.SetWindowLongPtrW = host_GetProcAddress(user32, "SetWindowLongW");
    *(void **)&g_wk.GetWindowLongPtrW = host_GetProcAddress(user32, "GetWindowLongW");
    if (g_wk.SetWindowLongPtrW == 0 || g_wk.GetWindowLongPtrW == 0)
        return 0;
#endif
    LOAD(user32, GetDlgItem);
    LOAD(user32, LoadCursorW);
    LOAD(user32, SystemParametersInfoW);
    LOAD(gdi32, GetStockObject);
    LOAD(gdi32, SelectObject);
#undef LOAD
    *(void **)&g_wk.CreateFontIndirectW = host_GetProcAddress(gdi32, "CreateFontIndirectW");
    *(void **)&g_wk.DeleteObject = host_GetProcAddress(gdi32, "DeleteObject");
    if (g_wk.DeleteObject == 0)
        g_wk.CreateFontIndirectW = 0;
    g_wk.instance = host_GetModuleHandleA(0);
    w_class wc;
    __builtin_memset(&wc, 0, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = kbd_procedure;
    wc.hInstance = g_wk.instance;
    wc.hCursor = g_wk.LoadCursorW(0, W_IDC_ARROW);
    wc.hbrBackground = (void *)(W_COLOR_BTNFACE + 1);
    wc.lpszClassName = k_class;
    if (g_wk.RegisterClassExW(&wc) == 0) {
        port_write_error("keyboard ui: RegisterClassExW failed\n");
        return 0;
    }
    g_wk.registered = 1;
    return 1;
}

#define KBD_MARGIN 12
#define KBD_GAP 8
#define KBD_CLIENT_WIDTH 420
#define KBD_BUTTON_WIDTH 88

// Makes the dialog's windows on the calling thread (the dialog's): 1 on success.
static int kbd_dialog_create(kbd_dialog *dialog)
{
    const keyboard_ui_host_request *request = dialog->request;
    w_nonclient_metrics metrics;
    __builtin_memset(&metrics, 0, sizeof(metrics));
    metrics.cbSize = sizeof(metrics);
    if (g_wk.CreateFontIndirectW != 0 &&
        g_wk.SystemParametersInfoW(W_SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0))
        dialog->font = g_wk.CreateFontIndirectW(&metrics.lfMessageFont);
    dialog->owns_font = dialog->font != 0;
    if (dialog->font == 0)
        dialog->font = g_wk.GetStockObject(W_DEFAULT_GUI_FONT);

    // measure the description (wrapped) and a line in that font
    const int inner = KBD_CLIENT_WIDTH - 2 * KBD_MARGIN;
    w_rect line = { 0, 0, inner, 0 }, description = { 0, 0, inner, 0 };
    void *dc = g_wk.GetDC(0);
    void *previous = g_wk.SelectObject(dc, dialog->font);
    g_wk.DrawTextW(dc, u"Ag", -1, &line, W_DT_CALCRECT | W_DT_NOPREFIX);
    if (request->description[0] != 0)
        g_wk.DrawTextW(dc, request->description, -1, &description,
                       W_DT_CALCRECT | W_DT_WORDBREAK | W_DT_NOPREFIX | W_DT_EDITCONTROL);
    g_wk.SelectObject(dc, previous);
    g_wk.ReleaseDC(0, dc);
    const int line_height = line.bottom > 0 ? (int)line.bottom : 16;
    int description_height = (int)description.bottom;
    if (description_height > 8 * line_height)
        description_height = 8 * line_height;
    const int edit_height = line_height + 10, button_height = line_height + 12;
    int y = KBD_MARGIN;
    const int description_y = y;
    if (description_height > 0)
        y += description_height + KBD_GAP;
    const int edit_y = y;
    y += edit_height + KBD_GAP + 4;
    const int button_y = y;
    const int client_height = y + button_height + KBD_MARGIN;

    const unsigned long style = W_WS_POPUP | W_WS_CAPTION | W_WS_SYSMENU;
    const unsigned long ex_style = W_WS_EX_DLGMODALFRAME | W_WS_EX_TOPMOST | W_WS_EX_APPWINDOW;
    w_rect frame = { 0, 0, KBD_CLIENT_WIDTH, client_height };
    g_wk.AdjustWindowRectEx(&frame, style, 0, ex_style);
    const int width = (int)(frame.right - frame.left), height = (int)(frame.bottom - frame.top);
    // centred over the game window (the screen without one)
    w_rect over = { 0, 0, g_wk.GetSystemMetrics(W_SM_CXSCREEN), g_wk.GetSystemMetrics(W_SM_CYSCREEN) };
    void *game = (void *)port_window_native_window();
    if (game != 0)
        g_wk.GetWindowRect(game, &over);
    const int x = (int)(over.left + over.right) / 2 - width / 2, top = (int)(over.top + over.bottom) / 2 - height / 2;
    const utf16 *caption = request->title[0] != 0 ? request->title : u"Keyboard";
    dialog->window = g_wk.CreateWindowExW(ex_style, k_class, caption, style, x, top, width, height, 0, 0, g_wk.instance,
                                          dialog);
    if (dialog->window == 0) {
        port_write_error("keyboard ui: CreateWindowExW failed\n");
        return 0;
    }
    void *window = dialog->window;
    void *label = 0;
    if (description_height > 0)
        label = g_wk.CreateWindowExW(0, u"STATIC", request->description,
                                     W_WS_CHILD | W_WS_VISIBLE | W_SS_LEFT | W_SS_NOPREFIX, KBD_MARGIN, description_y,
                                     inner, description_height, window, 0, g_wk.instance, 0);
    unsigned long edit_style = W_WS_CHILD | W_WS_VISIBLE | W_WS_TABSTOP | W_WS_GROUP | W_ES_AUTOHSCROLL;
    if (request->password)
        edit_style |= W_ES_PASSWORD;
    if (request->max_length == 0)
        edit_style |= W_ES_READONLY; // EM_LIMITTEXT 0 would mean the default limit: nothing fits
    dialog->edit = g_wk.CreateWindowExW(W_WS_EX_CLIENTEDGE, u"EDIT", request->default_text, edit_style, KBD_MARGIN,
                                        edit_y, inner, edit_height, window, (void *)W_ID_EDIT, g_wk.instance, 0);
    const unsigned long button = W_WS_CHILD | W_WS_VISIBLE | W_WS_TABSTOP;
    const int ok_x = KBD_CLIENT_WIDTH - KBD_MARGIN - 2 * KBD_BUTTON_WIDTH - KBD_GAP;
    const int cancel_x = KBD_CLIENT_WIDTH - KBD_MARGIN - KBD_BUTTON_WIDTH;
    void *ok = g_wk.CreateWindowExW(0, u"BUTTON", u"OK", button | W_WS_GROUP | W_BS_DEFPUSHBUTTON, ok_x, button_y,
                                    KBD_BUTTON_WIDTH, button_height, window, (void *)W_IDOK, g_wk.instance, 0);
    void *cancel = g_wk.CreateWindowExW(0, u"BUTTON", u"Cancel", button | W_BS_PUSHBUTTON, cancel_x, button_y,
                                        KBD_BUTTON_WIDTH, button_height, window, (void *)W_IDCANCEL, g_wk.instance, 0);
    if (dialog->edit == 0 || ok == 0 || cancel == 0) {
        port_write_error("keyboard ui: the dialog's controls could not be made\n");
        g_wk.DestroyWindow(window);
        return 0;
    }
    void *controls[4] = { label, dialog->edit, ok, cancel };
    for (int i = 0; i < 4; i++)
        if (controls[i] != 0)
            g_wk.SendMessageW(controls[i], W_WM_SETFONT, (w_wparam)dialog->font, 0);
    if (request->max_length > 0)
        g_wk.SendMessageW(dialog->edit, W_EM_LIMITTEXT, (w_wparam)request->max_length, 0);
    unsigned long length = 0;
    while (request->default_text[length] != 0)
        length++;
    if (request->highlight)
        g_wk.SendMessageW(dialog->edit, W_EM_SETSEL, 0, -1);
    else
        g_wk.SendMessageW(dialog->edit, W_EM_SETSEL, (w_wparam)length, (w_lparam)length);
    g_wk.ShowWindow(window, W_SW_SHOW);
    g_wk.SetForegroundWindow(window);
    g_wk.SetFocus(dialog->edit);
    return 1;
}

static unsigned long __stdcall kbd_dialog_main(void *argument)
{
    kbd_dialog *dialog = (kbd_dialog *)argument;
    keyboard_ui_host_request *request = dialog->request;
    if (kbd_dialog_create(dialog)) {
        w_message message;
        while (g_wk.GetMessageW(&message, 0, 0, 0) > 0) {
            if (!g_wk.IsDialogMessageW(dialog->window, &message)) {
                g_wk.TranslateMessage(&message);
                g_wk.DispatchMessageW(&message);
            }
        }
    }
    const int accepted = dialog->accepted;
    if (dialog->owns_font)
        g_wk.DeleteObject(dialog->font); // the windows that used it are gone
    host_HeapFree(host_GetProcessHeap(), 0, dialog);
    request->done(request, accepted); // last: the request is the caller's again
    return 0;
}

int keyboard_ui_host_show(keyboard_ui_host_request *request)
{
    if (port_window_native_window() == 0)
        return 0; // no game window (HALO3_WINDOW=0): nobody would see the dialog
    if (!kbd_load())
        return 0;
    const unsigned long zeroed = 8; // HEAP_ZERO_MEMORY
    kbd_dialog *dialog = (kbd_dialog *)host_HeapAlloc(host_GetProcessHeap(), zeroed, sizeof(kbd_dialog));
    if (dialog == 0)
        return 0;
    dialog->request = request;
    void *thread = host_CreateThread(0, 0, kbd_dialog_main, dialog, 0, 0);
    if (thread == 0) {
        port_write_error("keyboard ui: cannot start the dialog's thread\n");
        host_HeapFree(host_GetProcessHeap(), 0, dialog);
        return 0;
    }
    host_CloseHandle(thread);
    return 1;
}

int keyboard_ui_host_scripted_text(char *utf8, unsigned int capacity)
{
    const char *text = getenv("HALO3_KBD_TEXT");
    if (text == 0 || capacity == 0)
        return 0;
    unsigned long n = 0;
    for (; text[n] != 0 && n + 1 < capacity; n++)
        utf8[n] = text[n];
    utf8[n] = 0;
    return 1;
}
