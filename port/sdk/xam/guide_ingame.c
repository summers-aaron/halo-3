// port/sdk/xam (both hosts): the guide drawn inside the game's frame (guide_ingame.h): which opening of the guide shows
// in-game, the regions the renderer drew, and the game window's keys and mouse for it.
//
// Threads: the renderer calls guide_ingame_frame / guide_ingame_set_layout on the thread that presents the game's
// frames; the guide (guide.c) calls guide_ingame_show on whichever thread opens a request; the window's events arrive
// on the thread that pumps it (the presenting thread too) and HALO3_INPUT_SCRIPT's window entries on the game's input
// poll.
// The layout is under a lock of its own, taken briefly and never while calling into the guide (whose lock is taken by
// guide_click / guide_key); the guide takes it under its own for the rows shown (guide_ingame_rows_shown), so the
// layout's is always the inner one.
#ifdef _WIN32
__declspec(dllimport) char *__cdecl getenv(const char *name);
#else
char *getenv(const char *name);
#endif

#include "guide_ingame.h"
#include "../input/pointer_viewport.h"
#include "../online/invite.h"
#include "../runtime/port_runtime.h"

int guide_view_get_osk(guide_view *view, const char **grids, int *selected);
const char *guide_osk_label(char key, int compact, char literal[2]);

static struct {
    int wanted;                     // HALO3_GUIDE_RENDER is not "window"; -1 undecided
    int live;                       // the renderer could draw the guide in its last frame
    unsigned long session;          // the opening of the guide that shows in-game; 0 none
    volatile char busy;             // the layout's lock
    guide_ingame_layout layout;     // the regions the renderer last drew (under busy)
    int hover_pane, hover_item;     // the item the pointer selected last (the window's thread); -1 none
    unsigned long hover_session;
} g_ingame = { .wanted = -1, .hover_pane = -1, .hover_item = -1 };

static void layout_lock(void)
{
    while (__atomic_test_and_set(&g_ingame.busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void layout_unlock(void) { __atomic_clear(&g_ingame.busy, __ATOMIC_RELEASE); }

int guide_ingame_wanted(void)
{
    int wanted = __atomic_load_n(&g_ingame.wanted, __ATOMIC_ACQUIRE);
    if (wanted < 0) {
        const char *setting = getenv("HALO3_GUIDE_RENDER");
        wanted = !(setting != 0 && setting[0] == 'w'); // "window"; "ingame" or unset: in-game
        __atomic_store_n(&g_ingame.wanted, wanted, __ATOMIC_RELEASE);
    }
    return wanted;
}

int guide_ingame_live(void)
{
    return __atomic_load_n(&g_ingame.live, __ATOMIC_ACQUIRE);
}

int guide_ingame_show(unsigned long session)
{
    if (!guide_ingame_wanted())
        return 0;
    __atomic_store_n(&g_ingame.session, session, __ATOMIC_RELEASE);
    return 1;
}

// The opening that shows in-game now; 0 when the guide is closed or shows in its own window. The guide's lock is
// taken only while it is open (guide_showing): every frame asks.
static unsigned long showing_session(void)
{
    const unsigned long session = __atomic_load_n(&g_ingame.session, __ATOMIC_ACQUIRE);
    return session != 0 && guide_showing() && guide_showing_session() == session ? session : 0;
}

// ---- HALO3_GUIDE_SHOW: sample requests, for the pages Halo 3 never opens itself ----
//
// HALO3_GUIDE_SHOW="<page>@<ms>[,...]" (diagnostics): opens a sample request <ms> after the renderer's first frame, as
// a title's XShow* call would, on controller 1: messagebox (XShowMessageBoxUI: title, text, three buttons, the second
// focused), devices (XShowDeviceSelectorUI forced: a hard drive and a memory unit without the room asked for), or a
// blade: achievements, friends, players, messages. Its end is logged ("guide: sample ended: chosen <n>" /
// "cancelled").
#define GUIDE_SAMPLES 8

extern PORT_MS_ABI unsigned long GetTickCount(void);
void port_write_error(const char *text);

static struct {
    int parsed;
    int count;
    unsigned long start;
    struct { unsigned long at; int page, opened; } entries[GUIDE_SAMPLES];
} g_samples;

// (any other page: a guide_blade)
enum { k_sample_invite = -4, k_sample_achievement = -3, k_sample_message_box = -2, k_sample_devices = -1 };

static void sample_copy(char *to, unsigned long capacity, const char *text)
{
    unsigned long i = 0;
    for (; i + 1 < capacity && text[i] != 0; i++)
        to[i] = text[i];
    to[i] = 0;
}

static PORT_MS_ABI void sample_done(guide_request *request, int result, unsigned long value)
{
    char line[96] = "guide: sample ended: ";
    unsigned long length = 21;
    const char *what = result == GUIDE_RESULT_CHOSEN ? "chosen " : "cancelled ";
    for (; *what != 0 && length < sizeof(line) - 16; what++)
        line[length++] = *what;
    char digits[12];
    int count = 0;
    do {
        digits[count++] = (char)('0' + value % 10);
        value /= 10;
    } while (value != 0 && count < 11);
    while (count > 0)
        line[length++] = digits[--count];
    line[length++] = '\n';
    line[length] = 0;
    (void)request;
    port_write_error(line);
}

// XUSER_ACHIEVEMENT (xonline.h: two DWORDs, alike in both ABIs) and XUserWriteAchievements, synchronous (no
// XOVERLAPPED) (port/sdk/online/xlive_offline.cpp).
typedef struct {
    unsigned int user_index, achievement_id;
} sample_xuser_achievement;
PORT_MS_ABI unsigned int XUserWriteAchievements(unsigned int count, const sample_xuser_achievement *achievements,
    void *overlapped);

static void sample_log(const char *a, const char *b, unsigned long number)
{
    char line[GUIDE_LABEL * 3];
    sample_copy(line, sizeof(line), a);
    unsigned long n = 0;
    while (line[n] != 0)
        n++;
    sample_copy(line + n, sizeof(line) - n, b);
    while (line[n] != 0)
        n++;
    char digits[12];
    int count = 0;
    do {
        digits[count++] = (char)('0' + number % 10);
        number /= 10;
    } while (number != 0 && count < 11);
    if (n + (unsigned long)count + 3 < sizeof(line)) {
        line[n++] = ' ';
        while (count > 0)
            line[n++] = digits[--count];
    }
    line[n++] = '\n';
    line[n] = 0;
    port_write_error(line);
}

// "achievement": user 0 earns the first of the title's achievements its profile has not, as the game's own award
// would (the toast follows from XUserWriteAchievements).
static void sample_achievement(void)
{
    static guide_achievement list[GUIDE_ITEMS]; // the renderer's thread only
    const unsigned long count = port_guide_achievements(0, list, GUIDE_ITEMS);
    for (unsigned long i = 0; i < count; i++) {
        if (list[i].earned)
            continue;
        const sample_xuser_achievement earned = { 0, list[i].id };
        const unsigned long result = XUserWriteAchievements(1, &earned, 0);
        sample_log("guide: sample achievement: XUserWriteAchievements ", list[i].label, result);
        return;
    }
    sample_log("guide: sample achievement: user 0 has every achievement", "", 0);
}

// "invite": a game invite from "Sample Friend" for a System Link lobby at 10.0.0.2 (nobody is there: accepting it
// starts a join that fails as one to a lobby that has gone would), delivered as Live's poll delivers one.
static void sample_invite(void)
{
    static unsigned long number;
    port_invite invite;
    __builtin_memset(&invite, 0, sizeof(invite));
    port_host_random(invite.session, PORT_INVITE_SESSION_SIZE);
    invite.session[0] &= 0x1f;                       // a System Link XNKID
    unsigned char *xnaddr = invite.session + 8;
    __builtin_memset(xnaddr, 0, 36);
    xnaddr[0] = 10, xnaddr[1] = 0, xnaddr[2] = 0, xnaddr[3] = 2; // ina 10.0.0.2, as XNADDR holds it
    xnaddr[8] = 0x02, xnaddr[9] = 0x0c;              // wPortOnline 3074
    xnaddr[10] = 0x02, xnaddr[11] = 0x5a, xnaddr[12] = 0x11, xnaddr[13] = 0x22, xnaddr[14] = 0x33, xnaddr[15] = 0x44;
    invite.session_class = PORT_INVITE_CLASS_SYSTEM_LINK;
    sample_copy(invite.inviter, sizeof(invite.inviter), "Sample Friend");
    char id[PORT_INVITE_ID] = "sample-";
    id[7] = (char)('1' + number++ % 9);
    sample_copy(invite.id, sizeof(invite.id), id);
    sample_log("guide: sample invite: received ", invite.id, port_invite_received(&invite));
}

static void samples_parse(void)
{
    g_samples.parsed = 1;
    const char *text = getenv("HALO3_GUIDE_SHOW");
    static const struct { const char *name; int page; } k_pages[] = {
        { "messagebox", k_sample_message_box }, { "devices", k_sample_devices },
        { "achievements", GUIDE_BLADE_ACHIEVEMENTS }, { "friends", GUIDE_BLADE_FRIENDS },
        { "players", GUIDE_BLADE_PLAYERS }, { "messages", GUIDE_BLADE_MESSAGES }, { "settings", GUIDE_BLADE_SETTINGS },
        { "achievement", k_sample_achievement }, { "invite", k_sample_invite },
    };
    while (text != 0 && *text != 0 && g_samples.count < GUIDE_SAMPLES) {
        const char *name = text;
        while (*text != 0 && *text != '@' && *text != ',')
            text++;
        const unsigned long length = (unsigned long)(text - name);
        unsigned long at = 0;
        if (*text == '@')
            for (text++; *text >= '0' && *text <= '9'; text++)
                at = at * 10 + (unsigned long)(*text - '0');
        for (unsigned long i = 0; i < sizeof(k_pages) / sizeof(k_pages[0]); i++) {
            unsigned long j = 0;
            while (j < length && k_pages[i].name[j] == name[j])
                j++;
            if (j == length && k_pages[i].name[j] == 0) {
                g_samples.entries[g_samples.count].at = at;
                g_samples.entries[g_samples.count].page = k_pages[i].page;
                g_samples.count++;
            }
        }
        while (*text != 0 && *text != ',')
            text++;
        if (*text == ',')
            text++;
    }
    g_samples.start = GetTickCount();
}

// The samples due now (the renderer's thread: the one that parsed them).
static void samples_run(void)
{
    if (!g_samples.parsed)
        samples_parse();
    const unsigned long now = GetTickCount() - g_samples.start;
    for (int i = 0; i < g_samples.count; i++) {
        if (g_samples.entries[i].opened || now < g_samples.entries[i].at)
            continue;
        if (g_samples.entries[i].page == k_sample_achievement || g_samples.entries[i].page == k_sample_invite) {
            g_samples.entries[i].opened = 1; // neither opens the guide: whether it shows or not
            if (g_samples.entries[i].page == k_sample_achievement)
                sample_achievement();
            else
                sample_invite();
            continue;
        }
        if (guide_showing())
            continue;
        g_samples.entries[i].opened = 1;
        static guide_request request; // copied by guide_open
        __builtin_memset(&request, 0, sizeof(request));
        request.user = 0;
        request.done = sample_done;
        switch (g_samples.entries[i].page) {
        case k_sample_message_box:
            request.kind = GUIDE_KIND_MESSAGE_BOX;
            sample_copy(request.title, GUIDE_LABEL, "Quit Game");
            sample_copy(request.text, GUIDE_TEXT, "Are you sure you want to quit? Progress since the last checkpoint "
                                                  "will be lost, and the other players in this party will return to "
                                                  "the lobby.");
            sample_copy(request.buttons[0], GUIDE_LABEL, "Save and Quit");
            sample_copy(request.buttons[1], GUIDE_LABEL, "Quit");
            sample_copy(request.buttons[2], GUIDE_LABEL, "Keep Playing");
            request.button_count = 3;
            request.focus = 1;
            break;
        case k_sample_devices:
            request.kind = GUIDE_KIND_DEVICE_SELECTOR;
            request.device_count = 2;
            request.devices[0].id = 1;
            request.devices[0].free_bytes = 13207024435ull; // 12.3 GB
            request.devices[0].selectable = 1;
            sample_copy(request.devices[0].name, GUIDE_LABEL, "Hard Drive");
            request.devices[1].id = 2;
            request.devices[1].free_bytes = 3145728ull;
            request.devices[1].selectable = 0;
            sample_copy(request.devices[1].name, GUIDE_LABEL, "Memory Unit");
            break;
        default:
            request.kind = GUIDE_KIND_BLADE;
            request.blade = g_samples.entries[i].page;
            break;
        }
        guide_open(&request);
    }
}

// The game draws this view's body with its own font above the editable field. Keep one line per keyboard row,
// including in four-pane sign-in, and mark the exact key that A will press.
static void ingame_keyboard(guide_view_pane *pane, const char *grid, int selected, int columns, int narrow)
{
    char *out = pane->body;
    const char *end = out + sizeof(pane->body) - 1;
    for (int i = 0; grid[i]; i++) {
        char literal[2];
        const char *label = guide_osk_label(grid[i], narrow, literal);
        if (i && out < end)
            *out++ = i % columns == 0 ? '\n' : ' ';
        if (out < end && (i == selected || !narrow))
            *out++ = i == selected ? '[' : ' ';
        while (*label && out < end)
            *out++ = *label++;
        if (out < end && (i == selected || !narrow))
            *out++ = i == selected ? ']' : ' ';
    }
    *out = 0;
}

PORT_MS_ABI int guide_ingame_frame(guide_view *view, int drawable)
{
    drawable = drawable != 0;
    if (__atomic_load_n(&g_ingame.live, __ATOMIC_RELAXED) != drawable)
        __atomic_store_n(&g_ingame.live, drawable, __ATOMIC_RELEASE);
    samples_run();
    const unsigned long session = showing_session();
    if (session == 0)
        return 0;
    // the view the renderer keeps is rebuilt only when something shown changed
    if (!(view->open && view->session == session && view->revision == guide_revision())) {
        const char *grids[GUIDE_PANES];
        int selected[GUIDE_PANES];
        const int columns = guide_view_get_osk(view, grids, selected);
        for (int i = 0; i < view->pane_count; i++)
            if (grids[i])
                ingame_keyboard(&view->panes[i], grids[i], selected[i], columns, view->pane_count > 1);
    }
    return view->open && view->session == session;
}

PORT_MS_ABI void guide_ingame_set_layout(const guide_ingame_layout *layout)
{
    layout_lock();
    __builtin_memcpy(&g_ingame.layout, layout, sizeof(*layout));
    if (g_ingame.layout.hit_count > GUIDE_INGAME_HITS)
        g_ingame.layout.hit_count = GUIDE_INGAME_HITS;
    layout_unlock();
}

int guide_ingame_rows_shown(unsigned long session, int pane, int *first, int *last)
{
    int found = 0;
    *first = *last = -1;
    layout_lock();
    const guide_ingame_layout *layout = &g_ingame.layout;
    if (layout->session == session && layout->page_serial == guide_page_serial()) {
        for (int i = 0; i < layout->hit_count; i++) {
            const guide_ingame_hit *h = &layout->hits[i];
            if (h->target != GUIDE_TARGET_ITEM || h->pane != pane)
                continue;
            if (!found || h->item < *first)
                *first = h->item;
            if (!found || h->item > *last)
                *last = h->item;
            found = 1;
        }
    }
    layout_unlock();
    return found;
}

int guide_ingame_input(void) { return showing_session() != 0; }

void guide_ingame_key_action(int key, unsigned long character, int action)
{
    // a release always goes (a key pressed before the guide closed, let go after, holds nothing over)
    if (action == GUIDE_KEY_RELEASED || guide_ingame_input())
        guide_key_action(key, character, action);
}

void guide_ingame_key(int key, unsigned long character) { guide_ingame_key_action(key, character, GUIDE_KEY_PRESSED); }

// The topmost region of `session`'s layout at (x, y) window pixels of a width x height window: an item or a hint when
// there is one there (`*hit`), and the pane under it (`*pane`, -1 none); neither when the layout is another opening's
// or of pages no longer shown (a click between a page change and the next frame's layout hits nothing).
static void hit_test(unsigned long session, int x, int y, int width, int height, guide_ingame_hit *hit, int *pane)
{
    int found = 0;
    *pane = -1;
    layout_lock();
    const guide_ingame_layout *layout = &g_ingame.layout;
    if (layout->session == session && layout->page_serial == guide_page_serial() && layout->width > 0.0f &&
        layout->height > 0.0f) {
        // Match vulkan_cmd_blit_to_swapchain's centered aspect-preserving letterbox/pillarbox rect.
        float fx = (float)x + 0.5f;
        float fy = (float)y + 0.5f;
        int inside = 1;
        if (width > 0 && height > 0) {
            const int tw = (int)(layout->width + 0.5f);
            const int th = (int)(layout->height + 0.5f);
            float u, v;
            inside = port_pointer_in_frame(x, y, width, height, tw, th, &u, &v);
            if (inside) {
                fx = u * layout->width;
                fy = v * layout->height;
            }
        }
        for (int i = inside ? layout->hit_count - 1 : -1; i >= 0; i--) {
            const guide_ingame_hit *h = &layout->hits[i];
            if (fx < h->x0 || fx >= h->x1 || fy < h->y0 || fy >= h->y1)
                continue;
            if (h->target == GUIDE_TARGET_PANE) {
                if (*pane < 0)
                    *pane = h->pane;
            } else if (!found) {
                *hit = *h;
                found = 1;
                if (h->pane >= 0 && *pane < 0)
                    *pane = h->pane;
            }
        }
    }
    layout_unlock();
    if (!found)
        hit->target = -1;
}

void guide_ingame_pointer(int event, int button, int x, int y, int width, int height)
{
    const unsigned long session = showing_session();
    if (session == 0)
        return;
    if (g_ingame.hover_session != session) {
        g_ingame.hover_session = session;
        g_ingame.hover_pane = g_ingame.hover_item = -1;
    }
    guide_ingame_hit hit;
    int pane;
    hit_test(session, x, y, width, height, &hit, &pane);
    switch (event) {
    case GUIDE_POINTER_MOVE:
        // hovering an item (or a setting's marks, on it) selects it (once per item entered: the keys and controllers
        // move the selection meanwhile)
        if (hit.target == GUIDE_TARGET_ITEM || hit.target == GUIDE_TARGET_VALUE_PREVIOUS ||
            hit.target == GUIDE_TARGET_VALUE_NEXT) {
            if (hit.pane != g_ingame.hover_pane || hit.item != g_ingame.hover_item) {
                g_ingame.hover_pane = hit.pane;
                g_ingame.hover_item = hit.item;
                guide_hover(hit.pane, hit.item);
            }
        } else {
            g_ingame.hover_pane = g_ingame.hover_item = -1;
        }
        break;
    case GUIDE_POINTER_PRESS:
        if (button == 3) { // right: back, in the pane under the pointer
            if (pane >= 0)
                guide_click(pane, -1, 0);
            guide_key(GUIDE_KEY_BACK, 0);
            break;
        }
        if (button != 1)
            break;
        switch (hit.target) {
        case GUIDE_TARGET_ITEM: guide_click(hit.pane, hit.item, 1); break;
        case GUIDE_TARGET_ACCEPT: guide_key(GUIDE_KEY_ACCEPT, 0); break;
        case GUIDE_TARGET_BACK: guide_key(GUIDE_KEY_BACK, 0); break;
        case GUIDE_TARGET_NEXT_PANE: guide_key(GUIDE_KEY_NEXT_TAB, 0); break;
        case GUIDE_TARGET_PREVIOUS_TAB: guide_key(GUIDE_KEY_PREVIOUS_TAB, 0); break;
        case GUIDE_TARGET_NEXT_TAB: guide_key(GUIDE_KEY_NEXT_TAB, 0); break;
        case GUIDE_TARGET_CLOSE: guide_key(GUIDE_KEY_CLOSE, 0); break;
        case GUIDE_TARGET_VALUE_PREVIOUS:
        case GUIDE_TARGET_VALUE_NEXT: // the setting selected (silently), then its value stepped
            guide_click(hit.pane, hit.item, 0);
            guide_key(hit.target == GUIDE_TARGET_VALUE_PREVIOUS ? GUIDE_KEY_VALUE_PREVIOUS : GUIDE_KEY_VALUE_NEXT, 0);
            break;
        case GUIDE_TARGET_SCROLL_UP:
        case GUIDE_TARGET_SCROLL_DOWN:
            guide_click(hit.pane, -1, 0);
            guide_key(hit.target == GUIDE_TARGET_SCROLL_UP ? GUIDE_KEY_SCROLL_UP : GUIDE_KEY_SCROLL_DOWN, 0);
            break;
        default:
            if (pane >= 0)
                guide_click(pane, -1, 0); // focus the pane
            break;
        }
        break;
    case GUIDE_POINTER_WHEEL:
        if (pane >= 0) {
            guide_click(pane, -1, 0);
            guide_key(button < 0 ? GUIDE_KEY_SCROLL_UP : GUIDE_KEY_SCROLL_DOWN, 0); // the wheel stops at the ends
        }
        break;
    default:
        break;
    }
}

// ---- toasts ----
//
// A queue under a lock of its own (any thread posts; the renderer's thread takes them), never held while calling out.
// A toast's clock runs only while frames are drawable: the first toast starts with the first drawable frame after it
// is queued, and one that shows when the game stops drawing (a map load, say) waits, its time kept, until it draws
// again, so none passes unseen. Each next one starts GUIDE_TOAST_GAP_MILLISECONDS after the one before it has gone.

static struct {
    volatile char busy;
    guide_toast queue[GUIDE_TOAST_QUEUE]; // queue[0] is the one showing or next
    int count;
    int started;                          // queue[0] has begun to show
    unsigned long shown;                  // ... for this long (drawable frames' time only)
    unsigned long last;                   // GetTickCount at the frame before
    int last_drawable;                    // ... and whether it was drawable
    unsigned long ended;                  // GetTickCount when the one before went
    int gap;                              // the next waits for GUIDE_TOAST_GAP_MILLISECONDS after `ended`
    unsigned long serial;
    guide_toast last_posted;
} g_toasts;

static void toast_lock(void)
{
    while (__atomic_test_and_set(&g_toasts.busy, __ATOMIC_ACQUIRE))
        __builtin_ia32_pause();
}
static void toast_unlock(void) { __atomic_clear(&g_toasts.busy, __ATOMIC_RELEASE); }

// `text` into `to` (capacity bytes, NUL included), never cutting a UTF-8 sequence.
static void toast_copy(char *to, unsigned long capacity, const char *text)
{
    unsigned long n = 0;
    for (; text != 0 && *text != 0 && n + 1 < capacity; text++) {
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

PORT_MS_ABI int guide_toast_post(int kind, const char *title, const char *text, const char *detail, const char *hint)
{
    return guide_toast_post_icon(kind, 0, title, text, detail, hint);
}

PORT_MS_ABI int guide_toast_post_icon(int kind, unsigned int image_id, const char *title, const char *text,
    const char *detail, const char *hint)
{
    toast_lock();
    const int queued = g_toasts.count < GUIDE_TOAST_QUEUE;
    if (queued) {
        guide_toast *toast = &g_toasts.queue[g_toasts.count++];
        __builtin_memset(toast, 0, sizeof(*toast));
        toast->kind = kind;
        toast->serial = ++g_toasts.serial;
        toast->image_id = image_id;
        toast_copy(toast->title, sizeof(toast->title), title);
        toast_copy(toast->text, sizeof(toast->text), text);
        toast_copy(toast->detail, sizeof(toast->detail), detail);
        toast_copy(toast->hint, sizeof(toast->hint), hint);
        g_toasts.last_posted = *toast;
    }
    toast_unlock();
    if (!queued)
        port_write_error("guide: toast dropped: the toast queue is full\n");
    return queued;
}

PORT_MS_ABI unsigned int guide_toast_posted_count(guide_toast *last_out)
{
    toast_lock();
    const unsigned int count = (unsigned int)g_toasts.serial;
    if (last_out != 0 && count != 0)
        *last_out = g_toasts.last_posted;
    toast_unlock();
    return count;
}

PORT_MS_ABI int guide_ingame_toast_frame(unsigned long now, int drawable, guide_toast *toast)
{
    int showing = 0;
    toast_lock();
    // (tick differences are unsigned: right across the counter's wrap)
    if (g_toasts.started && drawable && g_toasts.last_drawable)
        g_toasts.shown += now - g_toasts.last;
    if (g_toasts.started && g_toasts.shown >= GUIDE_TOAST_MILLISECONDS) {
        // gone: the next after the gap
        for (int i = 1; i < g_toasts.count; i++)
            g_toasts.queue[i - 1] = g_toasts.queue[i];
        g_toasts.count--;
        g_toasts.started = 0;
        g_toasts.gap = 1;
        g_toasts.ended = now;
    }
    // once seen over, the gap is forgotten, however long the next toast takes to come
    if (g_toasts.gap && now - g_toasts.ended >= GUIDE_TOAST_GAP_MILLISECONDS)
        g_toasts.gap = 0;
    int chime = -1;
    if (!g_toasts.started && g_toasts.count > 0 && drawable && !g_toasts.gap) {
        g_toasts.started = 1;
        g_toasts.shown = 0;
        // it pops in with Halo 3's own fanfare: a promotion's for an achievement, the party-up for an invite
        if (g_toasts.queue[0].kind == GUIDE_TOAST_INVITE)
            chime = GUIDE_SOUND_PARTY_UP;
        else if (g_toasts.queue[0].kind == GUIDE_TOAST_ACHIEVEMENT)
            chime = GUIDE_SOUND_RANK_UP;
    }
    g_toasts.last = now;
    g_toasts.last_drawable = drawable;
    if (g_toasts.started && drawable) {
        *toast = g_toasts.queue[0];
        toast->elapsed = g_toasts.shown;
        showing = 1;
    }
    toast_unlock();
    guide_sound_post(chime);
    return showing;
}

int guide_toast_invite_showing(void)
{
    toast_lock();
    const int showing = g_toasts.started && g_toasts.last_drawable && g_toasts.queue[0].kind == GUIDE_TOAST_INVITE;
    toast_unlock();
    return showing;
}
