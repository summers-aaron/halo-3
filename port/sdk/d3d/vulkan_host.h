#ifndef PORT_D3D_VULKAN_HOST_H
#define PORT_D3D_VULKAN_HOST_H

/*
 * Opening the host's Vulkan loader at run time (port/sdk/runtime/vulkan_host_linux.c, vulkan_host_windows.c), so the
 * port links against no Vulkan library and a machine without one still starts (the backend then reports that Vulkan
 * is unavailable). Linux: libvulkan.so.1 through dlopen; Windows: vulkan-1.dll through LoadLibraryA.
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Opens the Vulkan loader. NULL when it is not installed. */
void *port_vk_load_library(void);

/* An exported symbol of the opened loader (the backend asks only for vkGetInstanceProcAddr), or NULL. The result is
 * a function address in the host's calling convention: VKAPI_PTR (__stdcall) on Windows, cdecl on Linux. */
void *port_vk_get_proc_addr(void *library, const char *name);

/* Closes the loader opened by port_vk_load_library (NULL is ignored). */
void port_vk_unload_library(void *library);

#ifdef __cplusplus
}
#endif

#endif /* PORT_D3D_VULKAN_HOST_H */
