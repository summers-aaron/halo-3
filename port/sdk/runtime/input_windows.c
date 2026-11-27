// port/sdk/runtime (Windows host): hardware gamepads through the host's XInput, loaded at run time (xinput1_4.dll,
// else xinput1_3.dll, else xinput9_1_0.dll), so the port still starts where none is installed. XINPUT_GAMEPAD has the
// Xbox 360 layout already; the Guide button comes from XInputGetStateEx (ordinal 100 of xinput1_3 / 1_4, the one
// export that reports it) where the host has it. Interface: port/sdk/input/port_input.h.
#include "../input/port_input.h"
#include "../kernel/win32_host.h"

PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(int, FreeLibrary, 4, (void *module));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
PORT_HOST_IMPORT(unsigned long long, GetTickCount64, 0, (void));
PORT_HOST_IMPORT(int, InitOnceExecuteOnce, 16, (void **once, int (__stdcall *callback)(void **, void *, void **),
    void *parameter, void **context));
PORT_HOST_IMPORT(void, AcquireSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(unsigned char, TryAcquireSRWLockExclusive, 4, (void **lock));
PORT_HOST_IMPORT(void, ReleaseSRWLockExclusive, 4, (void **lock));

// XINPUT_STATE (xinput.h), with room for the reserved DWORD XInputGetStateEx writes after it, and XINPUT_VIBRATION
typedef struct {
    unsigned long packet_number;
    port_input_gamepad gamepad;
    unsigned long reserved;
} x_state;
typedef struct {
    unsigned short left, right;
} x_vibration;

static struct {
    int loaded; // 0: not tried, 1: loaded, -1: no XInput on this host
    unsigned long(__stdcall *XInputGetState)(unsigned long, x_state *); // XInputGetStateEx when the host has it
    unsigned long(__stdcall *XInputSetState)(unsigned long, x_vibration *);
} g_xinput;

typedef struct {
    __declspec(align(8)) unsigned long long next_probe;
    void *lock;
    int rumble_valid;
    int vibrating;
    x_vibration last_rumble;
} x_slot;
static x_slot g_slots[PORT_INPUT_USER_COUNT];

static int __stdcall init_xinput(void **once, void *parameter, void **context)
{
    (void)once;
    (void)parameter;
    (void)context;
    static const char *const k_names[] = { "xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll" };
    for (unsigned i = 0; i < sizeof(k_names) / sizeof(k_names[0]); i++) {
        void *module = host_LoadLibraryA(k_names[i]);
        if (module == 0)
            continue;
        g_xinput.XInputGetState = (unsigned long(__stdcall *)(unsigned long, x_state *))
            host_GetProcAddress(module, (const char *)100);
        if (g_xinput.XInputGetState == 0)
            g_xinput.XInputGetState = (unsigned long(__stdcall *)(unsigned long, x_state *))
                host_GetProcAddress(module, "XInputGetState");
        if (g_xinput.XInputGetState != 0) {
            g_xinput.XInputSetState = (unsigned long(__stdcall *)(unsigned long, x_vibration *))
                host_GetProcAddress(module, "XInputSetState");
            __atomic_store_n(&g_xinput.loaded, 1, __ATOMIC_RELEASE);
            return 1;
        }
        host_FreeLibrary(module);
    }
    __atomic_store_n(&g_xinput.loaded, -1, __ATOMIC_RELEASE);
    return 1;
}

static int load(void)
{
    static void *once;
    int loaded = __atomic_load_n(&g_xinput.loaded, __ATOMIC_ACQUIRE);
    if (loaded == 0) {
        host_InitOnceExecuteOnce(&once, init_xinput, 0, 0);
        loaded = __atomic_load_n(&g_xinput.loaded, __ATOMIC_ACQUIRE);
    }
    return loaded > 0;
}

static int probe_delayed(const x_slot *slot)
{
    unsigned long long next = __atomic_load_n(&slot->next_probe, __ATOMIC_RELAXED);
    return next != 0 && host_GetTickCount64() < next;
}

static int lock_slot(x_slot *slot)
{
    if (probe_delayed(slot))
        return 0;
    host_AcquireSRWLockExclusive(&slot->lock);
    if (!probe_delayed(slot))
        return 1;
    host_ReleaseSRWLockExclusive(&slot->lock);
    return 0;
}

static void note_disconnected(x_slot *slot)
{
    slot->rumble_valid = 0;
    slot->vibrating = 0;
    // Disconnected slots can trigger host HID enumeration. Retry after one second.
    __atomic_store_n(&slot->next_probe, host_GetTickCount64() + 1000ull, __ATOMIC_RELAXED);
}

#define PORT_XINPUT_DISCONNECTED 1167ul // ERROR_DEVICE_NOT_CONNECTED

int port_input_hardware_state(unsigned long user, port_input_gamepad *out, int poll)
{
    (void)poll; // XInput reports state only: no taps to keep between polls (port_input.h)
    __builtin_memset(out, 0, sizeof(*out));
    if (user >= PORT_INPUT_USER_COUNT || !load())
        return 0;
    x_slot *slot = &g_slots[user];
    if (!lock_slot(slot))
        return 0;
    x_state state;
    unsigned long result = g_xinput.XInputGetState(user, &state);
    if (result == 0) {
        __atomic_store_n(&slot->next_probe, 0, __ATOMIC_RELAXED);
        *out = state.gamepad;
    } else if (result == PORT_XINPUT_DISCONNECTED) {
        note_disconnected(slot);
    }
    host_ReleaseSRWLockExclusive(&slot->lock);
    return result == 0;
}

int port_input_hardware_vibration(unsigned long user, unsigned short left, unsigned short right)
{
    if (user >= PORT_INPUT_USER_COUNT || !load() || g_xinput.XInputSetState == 0)
        return 0;
    x_slot *slot = &g_slots[user];
    if (!lock_slot(slot))
        return 0;
    unsigned long result = 0;
    if (!slot->rumble_valid || slot->last_rumble.left != left || slot->last_rumble.right != right) {
        x_vibration vibration = { left, right };
        result = g_xinput.XInputSetState(user, &vibration);
        slot->rumble_valid = result == 0;
        if (result == 0) {
            __atomic_store_n(&slot->next_probe, 0, __ATOMIC_RELAXED);
            slot->last_rumble = (x_vibration){ left, right };
            slot->vibrating = left != 0 || right != 0;
        } else if (result == PORT_XINPUT_DISCONNECTED) {
            note_disconnected(slot);
        }
    }
    host_ReleaseSRWLockExclusive(&slot->lock);
    return result == 0;
}

void port_input_hardware_shutdown(void)
{
    if (__atomic_load_n(&g_xinput.loaded, __ATOMIC_ACQUIRE) != 1)
        return;
    x_vibration still = { 0, 0 };
    for (unsigned long user = 0; user < PORT_INPUT_USER_COUNT; user++) {
        x_slot *slot = &g_slots[user];
        // The crash and watchdog exit paths also run this while another call may hold the slot.
        if (!host_TryAcquireSRWLockExclusive(&slot->lock))
            continue;
        if (slot->vibrating) {
            unsigned long result = g_xinput.XInputSetState(user, &still);
            if (result == 0 || result == PORT_XINPUT_DISCONNECTED)
                slot->vibrating = 0;
        }
        slot->rumble_valid = 0;
        host_ReleaseSRWLockExclusive(&slot->lock);
    }
}
