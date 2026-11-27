// port/sdk/runtime (Linux host): opens the Vulkan loader, libvulkan.so.1 (the soname every distribution ships; the
// unversioned libvulkan.so is only in development packages). RTLD_LOCAL keeps the loader's symbols out of the
// global namespace, where the game's own names live. Interface: port/sdk/d3d/vulkan_host.h.
#include "../d3d/vulkan_host.h"

#include <dlfcn.h>

void *port_vk_load_library(void)
{
    return dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
}

void *port_vk_get_proc_addr(void *library, const char *name)
{
    return library != 0 ? dlsym(library, name) : 0;
}

void port_vk_unload_library(void *library)
{
    if (library != 0)
        dlclose(library);
}
