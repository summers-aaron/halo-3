// port/sdk/kernel (Windows host): reaching the host's Win32 from the port's implementation of the Xbox API.
// The game calls the Xbox API without __stdcall (PowerPC has one convention; the port headers drop it), so the
// port defines those names as cdecl functions; the host functions of the same name are __stdcall imports
// (__imp__Name@bytes). PORT_HOST_IMPORT declares a pointer to one under host_Name.
#ifndef PORT_KERNEL_WIN32_HOST_H
#define PORT_KERNEL_WIN32_HOST_H

#ifdef _WIN64
#define PORT_HOST_IMPORT(ret, name, bytes, params) \
    extern ret(*host_##name) params __asm__("__imp_" #name)
#else
#define PORT_HOST_IMPORT(ret, name, bytes, params) \
    extern ret(__stdcall *host_##name) params __asm__("__imp__" #name "@" #bytes)
#endif

#endif
