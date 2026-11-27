// port/sdk/runtime (Windows host): opens the Vulkan loader, vulkan-1.dll, through kernel32's host imports (the
// game declares its own cdecl Win32 names; see port/sdk/kernel/win32_host.h). Interface:
// port/sdk/d3d/vulkan_host.h.
#include "../d3d/vulkan_host.h"
#include "../kernel/win32_host.h"

PORT_HOST_IMPORT(void *, LoadLibraryA, 4, (const char *name));
PORT_HOST_IMPORT(void *, GetProcAddress, 8, (void *module, const char *name));
PORT_HOST_IMPORT(int, FreeLibrary, 4, (void *module));

void *port_vk_load_library(void)
{
    return host_LoadLibraryA("vulkan-1.dll");
}

void *port_vk_get_proc_addr(void *library, const char *name)
{
    return library != 0 ? host_GetProcAddress(library, name) : 0;
}

void port_vk_unload_library(void *library)
{
    if (library != 0)
        host_FreeLibrary(library);
}
