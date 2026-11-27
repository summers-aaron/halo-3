// port/sdk/input (both hosts): XInput, the Xbox controller API, as xapilib's xinpapi implements it: through XAM's
// XamInputGetState, which merges the window's keyboard and mouse with hardware gamepads, and XamInputSetState, which
// passes XINPUT_VIBRATION to the gamepad's motors (port/sdk/xam/xam.c, port_input.h).
typedef unsigned int port_dword;

port_dword XamInputGetState(port_dword user_index, port_dword flags, void *state);
PORT_MS_ABI port_dword XamInputGetCapabilities(port_dword user_index, port_dword flags, void *capabilities);
PORT_MS_ABI port_dword XamInputSetState(port_dword user_index, port_dword flags, void *vibration, unsigned char amplitude,
    unsigned char frequency, unsigned char offset);

PORT_MS_ABI port_dword XInputGetState(port_dword user_index, void *state)
{
    return XamInputGetState(user_index, 0, state);
}

port_dword XInputGetCapabilities(port_dword user_index, port_dword flags, void *capabilities)
{
    return XamInputGetCapabilities(user_index, flags, capabilities);
}

port_dword XInputSetState(port_dword user_index, void *vibration)
{
    return XamInputSetState(user_index, 0, vibration, 0, 0, 0);
}
