// port/sdk/d3d: the Vulkan 1.3 backend (see vulkan_backend.h). Host C++: no exceptions, RTTI or dynamic
// initializers; the C library through gpu_crt.h; Vulkan through vulkan_mini.h and the loader opened at run time.
#include "vulkan_internal.h"
#include "vulkan_host.h"
#include "../runtime/port_runtime.h"

#include <stdarg.h>

using vulkan_recording::invalidate_command_state;
using vulkan_recording::record_descriptor_sets;
using vulkan_recording::record_dispatch;
using vulkan_recording::recording_command_buffer;

static void bindless_forget(s_vulkan_backend *backend, uint32_t binding, uint64_t identity);

struct s_memexport_sync_range {
    uint64_t base, end;
    bool write;
};

struct s_memexport_batch {
    static constexpr uint32_t capacity = 256;
    uint32_t count;
    bool active;
    s_memexport_sync_range ranges[capacity];
};

// Private recording state, allocated with each backend.
struct s_vulkan_backend_storage {
    s_vulkan_backend backend;
    s_memexport_batch memexport;
};
static_assert(__is_standard_layout(s_vulkan_backend_storage), "backend storage must share its first member's address");

static s_memexport_batch &memexport_batch(s_vulkan_backend *backend)
{
    return reinterpret_cast<s_vulkan_backend_storage *>(backend)->memexport;
}

// ---- logging ----

// The printf subset the backend's messages use: %s %c %d %i %u %x %X %p %%, the '0' flag and a width.
static void format_text(char *out, size_t size, const char *format, va_list args)
{
    size_t length = 0;
    auto put = [&](char c) {
        if (length + 1 < size)
            out[length++] = c;
    };
    auto put_padded = [&](const char *text, size_t text_length, size_t width, char pad) {
        for (size_t i = text_length; i < width; i++)
            put(pad);
        for (size_t i = 0; i < text_length; i++)
            put(text[i]);
    };
    for (const char *p = format; *p != 0; p++) {
        if (*p != '%') {
            put(*p);
            continue;
        }
        p++;
        char pad = ' ';
        size_t width = 0;
        if (*p == '0')
            pad = '0', p++;
        while (*p >= '0' && *p <= '9')
            width = width * 10 + (size_t)(*p++ - '0');
        while (*p == 'l' || *p == 'z')
            p++;
        char digits[24];
        size_t count = 0;
        switch (*p) {
        case 's': {
            const char *text = va_arg(args, const char *);
            if (text == 0)
                text = "(null)";
            put_padded(text, strlen(text), width, ' ');
            break;
        }
        case 'c':
            put((char)va_arg(args, int));
            break;
        case 'd':
        case 'i': {
            int value = va_arg(args, int);
            unsigned int magnitude = value < 0 ? 0u - (unsigned int)value : (unsigned int)value;
            do
                digits[sizeof(digits) - 1 - count++] = (char)('0' + magnitude % 10);
            while ((magnitude /= 10) != 0);
            if (value < 0)
                digits[sizeof(digits) - 1 - count++] = '-';
            put_padded(digits + sizeof(digits) - count, count, width, pad);
            break;
        }
        case 'u': {
            unsigned int value = va_arg(args, unsigned int);
            do
                digits[sizeof(digits) - 1 - count++] = (char)('0' + value % 10);
            while ((value /= 10) != 0);
            put_padded(digits + sizeof(digits) - count, count, width, pad);
            break;
        }
        case 'x':
        case 'X':
        case 'p': {
            const char *hex = *p == 'X' ? "0123456789ABCDEF" : "0123456789abcdef";
            uintptr_t value = *p == 'p' ? (uintptr_t)va_arg(args, void *) : (uintptr_t)va_arg(args, unsigned int);
            do
                digits[sizeof(digits) - 1 - count++] = hex[value & 15];
            while ((value >>= 4) != 0);
            if (*p == 'p')
                put('0'), put('x');
            put_padded(digits + sizeof(digits) - count, count, width, pad);
            break;
        }
        case '%':
            put('%');
            break;
        case 0:
            p--;
            break;
        default:
            put('%'), put(*p);
            break;
        }
    }
    out[length] = 0;
}

static void default_log(void *context, const char *text)
{
    (void)context;
    port_write_error(text);
}

void vulkan_log(s_vulkan_backend *backend, const char *format, ...)
{
    char text[4096];
    va_list args;
    va_start(args, format);
    format_text(text, sizeof(text) - 1, format, args);
    va_end(args);
    size_t length = strlen(text);
    text[length] = '\n';
    text[length + 1] = 0;
    backend->log(backend->log_context, text);
}

uint32_t vulkan_hash_bytes(uint32_t hash, const void *data, size_t size)
{
    const uint8_t *bytes = (const uint8_t *)data;
    for (size_t i = 0; i < size; i++)
        hash = (hash ^ bytes[i]) * 16777619u;
    return hash;
}

extern "C" const char *vulkan_result_string(VkResult result)
{
    switch (result) {
    case VK_SUCCESS: return "VK_SUCCESS";
    case VK_NOT_READY: return "VK_NOT_READY";
    case VK_TIMEOUT: return "VK_TIMEOUT";
    case VK_INCOMPLETE: return "VK_INCOMPLETE";
    case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
    case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
    case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
    case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
    case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
    case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
    case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
    case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
    case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
    case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
    case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
    case VK_ERROR_FRAGMENTED_POOL: return "VK_ERROR_FRAGMENTED_POOL";
    case VK_ERROR_UNKNOWN: return "VK_ERROR_UNKNOWN";
    case VK_ERROR_OUT_OF_POOL_MEMORY: return "VK_ERROR_OUT_OF_POOL_MEMORY";
    case VK_ERROR_FRAGMENTATION: return "VK_ERROR_FRAGMENTATION";
    case VK_PIPELINE_COMPILE_REQUIRED: return "VK_PIPELINE_COMPILE_REQUIRED";
    case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
    case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
    case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
    default: return "VkResult(?)";
    }
}

static const char *device_type_name(uint32_t type)
{
    switch (type) {
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "integrated GPU";
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return "discrete GPU";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return "virtual GPU";
    case VK_PHYSICAL_DEVICE_TYPE_CPU: return "CPU";
    default: return "other";
    }
}

// A decimal environment variable: *out and 1 when set and numeric.
static int environment_integer(const char *name, int32_t *out)
{
    const char *text = getenv(name);
    if (text == 0 || *text == 0)
        return 0;
    int32_t sign = 1, value = 0;
    if (*text == '-')
        sign = -1, text++;
    for (; *text != 0; text++) {
        if (*text < '0' || *text > '9')
            return 0;
        value = value * 10 + (*text - '0');
    }
    *out = sign * value;
    return 1;
}

// ---- debug messenger ----

static VkBool32 VKAPI_CALL debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                    VkDebugUtilsMessageTypeFlagsEXT types,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *data, void *user)
{
    (void)types;
    s_vulkan_backend *backend = (s_vulkan_backend *)user;
    const char *level = (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0     ? "error"
                        : (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0 ? "warning"
                                                                                             : "info";
    vulkan_log(backend, "vulkan: validation %s: %s", level, data != 0 && data->pMessage != 0 ? data->pMessage : "");
    return VK_FALSE;
}

// ---- entry points ----

static bool load_global_functions(s_vulkan_backend *backend)
{
    s_vulkan_functions &vk = backend->vk;
#define LOAD(name) vk.name = (PFN_##name)vk.vkGetInstanceProcAddr(0, #name);
    VULKAN_MINI_GLOBAL_FUNCTIONS(LOAD)
#undef LOAD
    // vkEnumerateInstanceVersion is missing from 1.0 loaders (which cannot run this backend anyway).
    return vk.vkCreateInstance != 0 && vk.vkEnumerateInstanceExtensionProperties != 0 &&
           vk.vkEnumerateInstanceLayerProperties != 0;
}

static bool load_instance_functions(s_vulkan_backend *backend)
{
    s_vulkan_functions &vk = backend->vk;
    bool complete = true;
#define LOAD(name)                                                                                                    \
    vk.name = (PFN_##name)vk.vkGetInstanceProcAddr(backend->instance, #name);                                         \
    if (vk.name == 0) {                                                                                               \
        vulkan_log(backend, "vulkan: the loader has no %s", #name);                                                   \
        complete = false;                                                                                             \
    }
    VULKAN_MINI_INSTANCE_FUNCTIONS(LOAD)
#undef LOAD
#define LOAD_OPTIONAL(name) vk.name = (PFN_##name)vk.vkGetInstanceProcAddr(backend->instance, #name);
    VULKAN_MINI_INSTANCE_OPTIONAL_FUNCTIONS(LOAD_OPTIONAL)
#undef LOAD_OPTIONAL
    return complete;
}

static void report_device_loss(s_vulkan_backend *backend);
static void destroy_retired(s_vulkan_backend *backend, s_vulkan_frame &frame);
static void buffer_pool_trim(s_vulkan_backend *backend, bool all);

// HALO3_VULKAN_CHECKPOINTS: a checkpoint naming the command recorded next, so a device loss can be placed between
// two commands (marker 0x80000000 | serial; the device layer's own markers are below 0x80000000).
static void command_checkpoint(s_vulkan_backend *backend, VkCommandBuffer command_buffer, const char *name)
{
    if (!backend->checkpoints)
        return;
    backend->command_serial = (backend->command_serial + 1u) & 0x7fffffffu;
    backend->command_names[backend->command_serial & 8191u] = name;
    backend->vk.vkCmdSetCheckpointNV(command_buffer, (const void *)(size_t)(0x80000000u | backend->command_serial));
}

static bool load_device_functions(s_vulkan_backend *backend)
{
    s_vulkan_functions &vk = backend->vk;
    bool complete = true;
#define LOAD(name)                                                                                                    \
    vk.name = (PFN_##name)vk.vkGetDeviceProcAddr(backend->device, #name);                                             \
    if (vk.name == 0) {                                                                                               \
        vulkan_log(backend, "vulkan: the device has no %s", #name);                                                   \
        complete = false;                                                                                             \
    }
    VULKAN_MINI_DEVICE_FUNCTIONS(LOAD)
#undef LOAD
#define LOAD_OPTIONAL(name) vk.name = (PFN_##name)vk.vkGetDeviceProcAddr(backend->device, #name);
    VULKAN_MINI_DEVICE_OPTIONAL_FUNCTIONS(LOAD_OPTIONAL)
#undef LOAD_OPTIONAL
    return complete;
}

// ---- instance ----

static bool has_extension(const VkExtensionProperties *extensions, uint32_t count, const char *name)
{
    for (uint32_t i = 0; i < count; i++)
        if (strcmp(extensions[i].extensionName, name) == 0)
            return true;
    return false;
}

static const char *window_system_extension(uint32_t window_system)
{
    switch (window_system) {
    case k_vulkan_window_system_win32: return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
    case k_vulkan_window_system_xlib: return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
    case k_vulkan_window_system_xcb: return VK_KHR_XCB_SURFACE_EXTENSION_NAME;
    case k_vulkan_window_system_headless: return VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME;
    default: return 0;
    }
}

static VkResult create_instance(s_vulkan_backend *backend, const s_vulkan_backend_create_info *info,
                                bool want_validation)
{
    s_vulkan_functions &vk = backend->vk;
    uint32_t loader_version = VK_API_VERSION_1_0;
    if (vk.vkEnumerateInstanceVersion != 0)
        vk.vkEnumerateInstanceVersion(&loader_version);
    if (loader_version < VK_API_VERSION_1_3) {
        vulkan_log(backend, "vulkan: the loader supports Vulkan %u.%u; 1.3 is required",
                   VK_API_VERSION_MAJOR(loader_version), VK_API_VERSION_MINOR(loader_version));
        return VK_ERROR_INCOMPATIBLE_DRIVER;
    }

    uint32_t extension_count = 0;
    vk.vkEnumerateInstanceExtensionProperties(0, &extension_count, 0);
    VkExtensionProperties *extensions =
        (VkExtensionProperties *)calloc(extension_count + 1, sizeof(VkExtensionProperties));
    if (extensions == 0)
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    vk.vkEnumerateInstanceExtensionProperties(0, &extension_count, extensions);

    const char *enabled[4];
    uint32_t enabled_count = 0;
    const char *surface_extension = window_system_extension(info->window_system);
    if (surface_extension != 0) {
        if (has_extension(extensions, extension_count, VK_KHR_SURFACE_EXTENSION_NAME) &&
            has_extension(extensions, extension_count, surface_extension)) {
            enabled[enabled_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
            enabled[enabled_count++] = surface_extension;
            backend->window_system = info->window_system;
        } else {
            vulkan_log(backend, "vulkan: %s is not available: no presentation", surface_extension);
        }
    }

    const char *layer = 0;
    if (want_validation) {
        uint32_t layer_count = 0;
        vk.vkEnumerateInstanceLayerProperties(&layer_count, 0);
        VkLayerProperties *layers = (VkLayerProperties *)calloc(layer_count + 1, sizeof(VkLayerProperties));
        if (layers != 0) {
            vk.vkEnumerateInstanceLayerProperties(&layer_count, layers);
            for (uint32_t i = 0; i < layer_count; i++)
                if (strcmp(layers[i].layerName, "VK_LAYER_KHRONOS_validation") == 0)
                    layer = "VK_LAYER_KHRONOS_validation";
            free(layers);
        }
        if (layer == 0)
            vulkan_log(backend, "vulkan: validation requested but VK_LAYER_KHRONOS_validation is not installed");
        else if (has_extension(extensions, extension_count, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
            enabled[enabled_count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }
    bool debug_utils = enabled_count > 0 && strcmp(enabled[enabled_count - 1], VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0;
    free(extensions);

    VkApplicationInfo application = {};
    application.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application.pApplicationName = info->application_name != 0 ? info->application_name : "halo3";
    application.applicationVersion = 1;
    application.pEngineName = "halo3-port";
    application.engineVersion = 1;
    application.apiVersion = VK_API_VERSION_1_3;

    // Validation messages from instance creation itself go through the same callback.
    VkDebugUtilsMessengerCreateInfoEXT messenger = {};
    messenger.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    messenger.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    messenger.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    messenger.pfnUserCallback = debug_messenger_callback;
    messenger.pUserData = backend;

    VkInstanceCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create.pNext = debug_utils ? &messenger : 0;
    create.pApplicationInfo = &application;
    create.enabledLayerCount = layer != 0 ? 1u : 0u;
    create.ppEnabledLayerNames = layer != 0 ? &layer : 0;
    create.enabledExtensionCount = enabled_count;
    create.ppEnabledExtensionNames = enabled;
    VkResult result = vk.vkCreateInstance(&create, 0, &backend->instance);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: vkCreateInstance failed: %s", vulkan_result_string(result));
        return result;
    }
    if (!load_instance_functions(backend))
        return VK_ERROR_INITIALIZATION_FAILED;
    backend->validation = layer != 0;
    if (debug_utils && vk.vkCreateDebugUtilsMessengerEXT != 0)
        vk.vkCreateDebugUtilsMessengerEXT(backend->instance, &messenger, 0, &backend->messenger);
    return VK_SUCCESS;
}

// ---- physical device ----

struct s_device_features {
    VkPhysicalDeviceFeatures2 features;
    VkPhysicalDeviceVulkan11Features vulkan11;
    VkPhysicalDeviceVulkan12Features vulkan12;
    VkPhysicalDeviceVulkan13Features vulkan13;
};

static void chain_features(s_device_features *features)
{
    memset(features, 0, sizeof(*features));
    features->features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    features->features.pNext = &features->vulkan11;
    features->vulkan11.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
    features->vulkan11.pNext = &features->vulkan12;
    features->vulkan12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    features->vulkan12.pNext = &features->vulkan13;
    features->vulkan13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
}

// Whether a device can run the backend: Vulkan 1.3, scalar block layout (the shaders' D3D buffer packing), dynamic
// rendering, and a queue family with graphics and compute. *out_reason says why not.
static bool device_is_suitable(s_vulkan_backend *backend, VkPhysicalDevice device,
                               const VkPhysicalDeviceProperties &properties, uint32_t *out_family,
                               const char **out_reason)
{
    s_vulkan_functions &vk = backend->vk;
    if (properties.apiVersion < VK_API_VERSION_1_3) {
        *out_reason = "Vulkan 1.3 not supported";
        return false;
    }
    s_device_features features;
    chain_features(&features);
    vk.vkGetPhysicalDeviceFeatures2(device, &features.features);
    if (!features.vulkan12.scalarBlockLayout) {
        *out_reason = "no scalarBlockLayout";
        return false;
    }
    if (!features.vulkan13.dynamicRendering) {
        *out_reason = "no dynamicRendering";
        return false;
    }
    uint32_t family_count = 0;
    vk.vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, 0);
    VkQueueFamilyProperties *families =
        (VkQueueFamilyProperties *)calloc(family_count + 1, sizeof(VkQueueFamilyProperties));
    if (families == 0) {
        *out_reason = "out of memory";
        return false;
    }
    vk.vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, families);
    bool found = false;
    for (uint32_t i = 0; i < family_count && !found; i++) {
        const uint32_t needed = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT;
        if ((families[i].queueFlags & needed) == needed && families[i].queueCount > 0) {
            *out_family = i;
            found = true;
        }
    }
    free(families);
    if (!found)
        *out_reason = "no graphics + compute queue";
    return found;
}

static uint32_t device_type_rank(uint32_t type)
{
    switch (type) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return 4;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return 3;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return 2;
    case VK_PHYSICAL_DEVICE_TYPE_CPU: return 1;
    default: return 0;
    }
}

static VkResult select_physical_device(s_vulkan_backend *backend, int32_t forced_index)
{
    s_vulkan_functions &vk = backend->vk;
    uint32_t count = 0;
    VkResult result = vk.vkEnumeratePhysicalDevices(backend->instance, &count, 0);
    if (result != VK_SUCCESS || count == 0) {
        vulkan_log(backend, "vulkan: no physical devices (%s)", vulkan_result_string(result));
        return result != VK_SUCCESS ? result : VK_ERROR_INITIALIZATION_FAILED;
    }
    VkPhysicalDevice *devices = (VkPhysicalDevice *)calloc(count, sizeof(VkPhysicalDevice));
    if (devices == 0)
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    vk.vkEnumeratePhysicalDevices(backend->instance, &count, devices);

    // The best suitable device by type, unless forced_index names a suitable one.
    int32_t chosen = -1, forced = -1;
    uint32_t chosen_rank = 0, chosen_family = 0, forced_family = 0;
    for (uint32_t i = 0; i < count; i++) {
        VkPhysicalDeviceProperties properties;
        vk.vkGetPhysicalDeviceProperties(devices[i], &properties);
        uint32_t family = 0;
        const char *reason = "";
        bool suitable = device_is_suitable(backend, devices[i], properties, &family, &reason);
        vulkan_log(backend, "vulkan: device %u: %s (%s, Vulkan %u.%u.%u)%s%s", i, properties.deviceName,
                   device_type_name(properties.deviceType), VK_API_VERSION_MAJOR(properties.apiVersion),
                   VK_API_VERSION_MINOR(properties.apiVersion), VK_API_VERSION_PATCH(properties.apiVersion),
                   suitable ? "" : ": unsuitable, ", reason);
        if (!suitable)
            continue;
        uint32_t rank = device_type_rank(properties.deviceType) + 1;
        if (rank > chosen_rank) {
            chosen = (int32_t)i;
            chosen_rank = rank;
            chosen_family = family;
        }
        if ((int32_t)i == forced_index) {
            forced = (int32_t)i;
            forced_family = family;
        }
    }
    if (forced >= 0) {
        chosen = forced;
        chosen_family = forced_family;
    } else if (forced_index >= 0) {
        vulkan_log(backend, "vulkan: device %d was requested but is missing or unsuitable", forced_index);
    }
    if (chosen >= 0) {
        backend->physical_device = devices[chosen];
        backend->queue_family = chosen_family;
    }
    free(devices);
    if (chosen < 0) {
        vulkan_log(backend, "vulkan: no suitable device");
        return VK_ERROR_INCOMPATIBLE_DRIVER;
    }
    vk.vkGetPhysicalDeviceProperties(backend->physical_device, &backend->properties);
    vk.vkGetPhysicalDeviceMemoryProperties(backend->physical_device, &backend->memory_properties);
    return VK_SUCCESS;
}

// ---- device ----

// Core features turned on when the device has them: what D3D9-era rendering and DXC's SPIR-V may use.
// robustBufferAccess (with vulkan13.robustImageAccess) gives out-of-range buffer and image reads the D3D11 result
// the shaders were written against (zero, never a fault): vertex fetches past a stream's end, structured buffer
// reads by vertex index.
static const size_t k_optional_core_features[] = {
    offsetof(VkPhysicalDeviceFeatures, robustBufferAccess),
    offsetof(VkPhysicalDeviceFeatures, fullDrawIndexUint32),
    offsetof(VkPhysicalDeviceFeatures, imageCubeArray),
    offsetof(VkPhysicalDeviceFeatures, independentBlend),
    offsetof(VkPhysicalDeviceFeatures, dualSrcBlend),
    offsetof(VkPhysicalDeviceFeatures, depthClamp),
    offsetof(VkPhysicalDeviceFeatures, depthBiasClamp),
    offsetof(VkPhysicalDeviceFeatures, fillModeNonSolid),
    offsetof(VkPhysicalDeviceFeatures, samplerAnisotropy),
    offsetof(VkPhysicalDeviceFeatures, textureCompressionBC),
    offsetof(VkPhysicalDeviceFeatures, occlusionQueryPrecise),
    offsetof(VkPhysicalDeviceFeatures, vertexPipelineStoresAndAtomics),
    offsetof(VkPhysicalDeviceFeatures, fragmentStoresAndAtomics),
    offsetof(VkPhysicalDeviceFeatures, shaderImageGatherExtended),
    offsetof(VkPhysicalDeviceFeatures, shaderStorageImageExtendedFormats),
    offsetof(VkPhysicalDeviceFeatures, shaderStorageImageReadWithoutFormat),
    offsetof(VkPhysicalDeviceFeatures, shaderStorageImageWriteWithoutFormat),
    offsetof(VkPhysicalDeviceFeatures, shaderClipDistance),
    offsetof(VkPhysicalDeviceFeatures, shaderInt16),
};

static uint32_t choose_depth_format(s_vulkan_backend *backend)
{
    static const uint32_t candidates[] = { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    for (uint32_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); i++) {
        VkFormatProperties properties;
        backend->vk.vkGetPhysicalDeviceFormatProperties(backend->physical_device, (VkFormat)candidates[i], &properties);
        if ((properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) != 0)
            return candidates[i];
    }
    return VK_FORMAT_UNDEFINED;
}

static bool supports_xenos_abi6(s_vulkan_backend *backend, const s_device_features &features)
{
    const VkPhysicalDeviceVulkan12Features &v12 = features.vulkan12;
    if (!v12.bufferDeviceAddress || !features.features.features.shaderSampledImageArrayDynamicIndexing ||
        !v12.runtimeDescriptorArray || !v12.descriptorBindingSampledImageUpdateAfterBind ||
        !v12.descriptorBindingPartiallyBound || !v12.descriptorBindingUpdateUnusedWhilePending)
        return false;

    VkPhysicalDeviceDescriptorIndexingProperties indexing = {};
    indexing.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES;
    VkPhysicalDeviceProperties2 query = {};
    query.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    query.pNext = &indexing;
    backend->vk.vkGetPhysicalDeviceProperties2(backend->physical_device, &query);
    const VkPhysicalDeviceLimits &limits = query.properties.limits;
    constexpr uint32_t constants = 4;
    return indexing.maxDescriptorSetUpdateAfterBindSampledImages >= XE_V6_IMAGES &&
           indexing.maxDescriptorSetUpdateAfterBindSamplers >= XE_V6_SAMPLERS &&
           indexing.maxPerStageDescriptorUpdateAfterBindSampledImages >= XE_V6_IMAGES &&
           indexing.maxPerStageDescriptorUpdateAfterBindSamplers >= XE_V6_SAMPLERS &&
           indexing.maxPerStageUpdateAfterBindResources >= XE_V6_IMAGES + XE_V6_SAMPLERS + constants &&
           indexing.maxUpdateAfterBindDescriptorsInAllPools >= XE_V6_IMAGES + XE_V6_SAMPLERS &&
           limits.maxBoundDescriptorSets >= 2 && limits.maxPerStageDescriptorUniformBuffers >= constants &&
           limits.maxDescriptorSetUniformBuffersDynamic >= constants && limits.maxPerStageResources >= constants &&
           limits.maxPushConstantsSize >= sizeof(xe_push_v6);
}

extern "C" uint32_t vulkan_backend_supports_xenos_abi6(const s_vulkan_backend *backend)
{
    return backend != 0 && backend->xenos_abi6_supported;
}

static VkResult create_device(s_vulkan_backend *backend)
{
    s_vulkan_functions &vk = backend->vk;
    s_device_features supported, enabled;
    chain_features(&supported);
    vk.vkGetPhysicalDeviceFeatures2(backend->physical_device, &supported.features);
    chain_features(&enabled);
    for (size_t i = 0; i < sizeof(k_optional_core_features) / sizeof(k_optional_core_features[0]); i++) {
        size_t offset = k_optional_core_features[i];
        *(VkBool32 *)((uint8_t *)&enabled.features.features + offset) =
            *(const VkBool32 *)((const uint8_t *)&supported.features.features + offset);
    }
    enabled.vulkan11.shaderDrawParameters = supported.vulkan11.shaderDrawParameters;
    enabled.vulkan12.scalarBlockLayout = VK_TRUE;
    enabled.vulkan12.uniformBufferStandardLayout = supported.vulkan12.uniformBufferStandardLayout;
    enabled.vulkan12.samplerMirrorClampToEdge = supported.vulkan12.samplerMirrorClampToEdge;
    enabled.vulkan12.shaderFloat16 = supported.vulkan12.shaderFloat16;
    enabled.vulkan13.dynamicRendering = VK_TRUE;
    enabled.vulkan13.synchronization2 = supported.vulkan13.synchronization2;
    enabled.vulkan13.maintenance4 = supported.vulkan13.maintenance4;
    enabled.vulkan13.shaderDemoteToHelperInvocation = supported.vulkan13.shaderDemoteToHelperInvocation;
    enabled.vulkan13.shaderTerminateInvocation = supported.vulkan13.shaderTerminateInvocation;
    enabled.vulkan13.robustImageAccess = supported.vulkan13.robustImageAccess;
    // HALO3_VULKAN_CHECKPOINTS also logs the buffers' device addresses, to place a GPU page fault (Xid 31)
    int32_t checkpoint_request = 0;
    if (environment_integer("HALO3_VULKAN_CHECKPOINTS", &checkpoint_request) && checkpoint_request != 0 &&
        supported.vulkan12.bufferDeviceAddress) {
        enabled.vulkan12.bufferDeviceAddress = VK_TRUE;
        backend->buffer_addresses = 1;
    }
    // translated Xenos shaders (shader pack version 3, xenos_spirv_abi.h): vertex streams and memexport by buffer device
    // address (core in 1.3), the water's tessellation, rectangle lists through a geometry shader, user clip planes
    if (backend->xenos_requested) {
        enabled.vulkan12.bufferDeviceAddress = supported.vulkan12.bufferDeviceAddress;
        enabled.features.features.tessellationShader = supported.features.features.tessellationShader;
        enabled.features.features.geometryShader = supported.features.features.geometryShader;
        if (!supported.vulkan12.bufferDeviceAddress)
            vulkan_log(backend, "vulkan: the device has no bufferDeviceAddress: translated Xenos shaders cannot fetch vertices");
        if (supports_xenos_abi6(backend, supported)) {
            enabled.features.features.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
            enabled.vulkan12.runtimeDescriptorArray = VK_TRUE;
            enabled.vulkan12.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
            enabled.vulkan12.descriptorBindingPartiallyBound = VK_TRUE;
            enabled.vulkan12.descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
            backend->xenos_abi6_supported = true;
        }
    }
    backend->device_address = enabled.vulkan12.bufferDeviceAddress != 0;
    backend->tessellation = enabled.features.features.tessellationShader != 0;
    backend->geometry = enabled.features.features.geometryShader != 0;
    backend->clip_distance = enabled.features.features.shaderClipDistance != 0;
    backend->full_draw_index = enabled.features.features.fullDrawIndexUint32 != 0;
    backend->synchronization2 = enabled.vulkan13.synchronization2 != 0;
    backend->sampler_anisotropy = enabled.features.features.samplerAnisotropy != 0;
    backend->sampler_mirror_clamp_to_edge = enabled.vulkan12.samplerMirrorClampToEdge != 0;
    backend->storage_write_without_format = enabled.features.features.shaderStorageImageWriteWithoutFormat != 0;
    backend->occlusion_precise = enabled.features.features.occlusionQueryPrecise != 0;

    const char *extensions[4];
    VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT pipeline_library = {};
    pipeline_library.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT;
    uint32_t extension_count = 0;
    int32_t checkpoint_setting = 0;
    const bool want_checkpoints =
        environment_integer("HALO3_VULKAN_CHECKPOINTS", &checkpoint_setting) && checkpoint_setting != 0;
    {
        uint32_t count = 0;
        vk.vkEnumerateDeviceExtensionProperties(backend->physical_device, 0, &count, 0);
        VkExtensionProperties *available = (VkExtensionProperties *)calloc(count + 1, sizeof(VkExtensionProperties));
        if (available == 0)
            return VK_ERROR_OUT_OF_HOST_MEMORY;
        vk.vkEnumerateDeviceExtensionProperties(backend->physical_device, 0, &count, available);
        if (backend->window_system != k_vulkan_window_system_none) {
            if (has_extension(available, count, VK_KHR_SWAPCHAIN_EXTENSION_NAME))
                extensions[extension_count++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
            else
                vulkan_log(backend, "vulkan: the device has no %s: no presentation", VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        }
        backend->presentation = extension_count != 0;
        if (want_checkpoints) {
            if (has_extension(available, count, VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME)) {
                extensions[extension_count++] = VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME;
                backend->checkpoints = 1;
            } else {
                vulkan_log(backend, "vulkan: HALO3_VULKAN_CHECKPOINTS: the device has no %s",
                           VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME);
            }
        }
        // translated Xenos shaders: graphics pipeline libraries (each shader compiled once, pipelines linked from the
        // parts; vulkan_pipeline_library_*), unless HALO3_PIPELINE_LIBRARY=0
        int32_t library_setting = 1;
        environment_integer("HALO3_PIPELINE_LIBRARY", &library_setting);
        if (backend->xenos_requested && library_setting != 0 &&
            has_extension(available, count, VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME) &&
            has_extension(available, count, VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME)) {
            VkPhysicalDeviceGraphicsPipelineLibraryFeaturesEXT supported_library = {};
            supported_library.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_FEATURES_EXT;
            VkPhysicalDeviceFeatures2 query = {};
            query.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            query.pNext = &supported_library;
            vk.vkGetPhysicalDeviceFeatures2(backend->physical_device, &query);
            VkPhysicalDeviceGraphicsPipelineLibraryPropertiesEXT library_properties = {};
            library_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_GRAPHICS_PIPELINE_LIBRARY_PROPERTIES_EXT;
            VkPhysicalDeviceProperties2 properties = {};
            properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
            properties.pNext = &library_properties;
            vk.vkGetPhysicalDeviceProperties2(backend->physical_device, &properties);
            if (supported_library.graphicsPipelineLibrary) {
                extensions[extension_count++] = VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME;
                extensions[extension_count++] = VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME;
                pipeline_library.graphicsPipelineLibrary = VK_TRUE;
                enabled.vulkan13.pNext = &pipeline_library;
                backend->pipeline_library = 1;
                backend->pipeline_library_fast_link = library_properties.graphicsPipelineLibraryFastLinking != 0;
            }
        }
        free(available);
    }

    const float priority = 1.0f;
    VkDeviceQueueCreateInfo queue = {};
    queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue.queueFamilyIndex = backend->queue_family;
    queue.queueCount = 1;
    queue.pQueuePriorities = &priority;

    VkDeviceCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create.pNext = &enabled.features;
    create.queueCreateInfoCount = 1;
    create.pQueueCreateInfos = &queue;
    create.enabledExtensionCount = extension_count;
    create.ppEnabledExtensionNames = extensions;
    VkResult result = vk.vkCreateDevice(backend->physical_device, &create, 0, &backend->device);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: vkCreateDevice failed: %s", vulkan_result_string(result));
        return result;
    }
    if (!load_device_functions(backend))
        return VK_ERROR_INITIALIZATION_FAILED;
    if (backend->checkpoints && (vk.vkCmdSetCheckpointNV == 0 || vk.vkGetQueueCheckpointDataNV == 0))
        backend->checkpoints = 0;
    vk.vkGetDeviceQueue(backend->device, backend->queue_family, 0, &backend->queue);
    backend->depth_format = choose_depth_format(backend);
    if (backend->depth_format == VK_FORMAT_UNDEFINED)
        vulkan_log(backend, "vulkan: no depth/stencil attachment format (D32_SFLOAT_S8_UINT, D24_UNORM_S8_UINT)");
    return VK_SUCCESS;
}

// ---- descriptor pools ----

static VkDescriptorPool create_descriptor_pool(s_vulkan_backend *backend, bool freeable, bool textures = false)
{
    // Sized for a frame of D3D9-style draws: mostly sampled images, samplers and constant buffers.
    static const VkDescriptorPoolSize sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 2048 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 256 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4096 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 256 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 256 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 256 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4096 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 512 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 256 },
    };
    // vulkan_descriptor_set_allocate_textures: sets of the universal Xenos texture layout (xenos_spirv_abi.h set 1:
    // 128 sampled images, 192 samplers), VULKAN_TEXTURE_SETS_PER_POOL a pool
    static const VkDescriptorPoolSize texture_sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 128 * VULKAN_TEXTURE_SETS_PER_POOL },
        { VK_DESCRIPTOR_TYPE_SAMPLER, 192 * VULKAN_TEXTURE_SETS_PER_POOL },
    };
    VkDescriptorPoolCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create.flags = freeable ? VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT : 0u;
    create.maxSets = textures ? VULKAN_TEXTURE_SETS_PER_POOL : 2048;
    create.poolSizeCount = textures ? sizeof(texture_sizes) / sizeof(texture_sizes[0]) : sizeof(sizes) / sizeof(sizes[0]);
    create.pPoolSizes = textures ? texture_sizes : sizes;
    VkDescriptorPool pool = VK_NULL_HANDLE;
    VkResult result = backend->vk.vkCreateDescriptorPool(backend->device, &create, 0, &pool);
    if (result != VK_SUCCESS)
        vulkan_log(backend, "vulkan: vkCreateDescriptorPool failed: %s", vulkan_result_string(result));
    return pool;
}

static void destroy_descriptor_pools(s_vulkan_backend *backend, s_vulkan_descriptor_pools *pools)
{
    for (uint32_t i = 0; i < pools->count; i++)
        backend->vk.vkDestroyDescriptorPool(backend->device, pools->pools[i], 0);
    free(pools->pools);
    memset(pools, 0, sizeof(*pools));
}

// Allocates from the chain's current pool, moving on to the next (created when needed) when it is exhausted.
static VkDescriptorSet allocate_from_pools(s_vulkan_backend *backend, s_vulkan_descriptor_pools *pools,
                                           VkDescriptorSetLayout layout, bool freeable, VkDescriptorPool *out_pool)
{
    for (;;) {
        if (pools->current == pools->count) {
            if (pools->count == pools->capacity) {
                uint32_t capacity = pools->capacity != 0 ? pools->capacity * 2 : 4;
                VkDescriptorPool *grown = (VkDescriptorPool *)realloc(pools->pools, capacity * sizeof(VkDescriptorPool));
                if (grown == 0)
                    return VK_NULL_HANDLE;
                pools->pools = grown;
                pools->capacity = capacity;
            }
            VkDescriptorPool pool = VK_NULL_HANDLE;
            if (pools->textures != 0 && !freeable && backend->free_texture_pools.count != 0)
                pool = backend->free_texture_pools.pools[--backend->free_texture_pools.count];
            else
                pool = create_descriptor_pool(backend, freeable, pools->textures != 0);
            if (pool == VK_NULL_HANDLE)
                return VK_NULL_HANDLE;
            pools->pools[pools->count++] = pool;
        }
        VkDescriptorSetAllocateInfo allocate = {};
        allocate.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocate.descriptorPool = pools->pools[pools->current];
        allocate.descriptorSetCount = 1;
        allocate.pSetLayouts = &layout;
        VkDescriptorSet set = VK_NULL_HANDLE;
        VkResult result = backend->vk.vkAllocateDescriptorSets(backend->device, &allocate, &set);
        if (result == VK_SUCCESS) {
            *out_pool = pools->pools[pools->current];
            return set;
        }
        if (result != VK_ERROR_OUT_OF_POOL_MEMORY && result != VK_ERROR_FRAGMENTED_POOL) {
            vulkan_log(backend, "vulkan: vkAllocateDescriptorSets failed: %s", vulkan_result_string(result));
            return VK_NULL_HANDLE;
        }
        pools->current++;
    }
}

// ---- synchronization ----

// Source scope when leaving `layout` (only write accesses need flushing; read-only layouts need execution order only).
// Only bits shared by legacy VkAccessFlagBits / VkPipelineStageFlagBits and synchronization2 are used.
static void layout_src_scope(const s_vulkan_backend *backend, uint32_t layout, VkPipelineStageFlags2 *stages,
                             VkAccessFlags2 *access)
{
    switch (layout) {
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        *stages = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        *access = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        *stages = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        *access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        *stages = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        *access = VK_ACCESS_2_NONE;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        *stages = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        *access = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        *stages = backend->shader_stages;
        *access = VK_ACCESS_2_NONE;
        break;
    case VK_IMAGE_LAYOUT_GENERAL:
        // Backend uses GENERAL exclusively for compute storage-image passes (image_pass / pack_depth_stencil).
        *stages = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        *access = VK_ACCESS_2_SHADER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_UNDEFINED:
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        // Nothing to wait for (presentation is ordered by the semaphores), nothing to make visible.
        *stages = VK_PIPELINE_STAGE_2_NONE;
        *access = VK_ACCESS_2_NONE;
        break;
    default:
        *stages = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        *access = VK_ACCESS_2_MEMORY_WRITE_BIT;
        break;
    }
}

// Destination scope when entering `layout`.
static void layout_dst_scope(const s_vulkan_backend *backend, uint32_t layout, VkPipelineStageFlags2 *stages,
                             VkAccessFlags2 *access)
{
    switch (layout) {
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        *stages = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        *access = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        *stages = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
        *access = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        *stages = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        *access = VK_ACCESS_2_TRANSFER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        *stages = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        *access = VK_ACCESS_2_TRANSFER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        *stages = backend->shader_stages;
        *access = VK_ACCESS_2_SHADER_READ_BIT;
        break;
    case VK_IMAGE_LAYOUT_GENERAL:
        *stages = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        *access = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT;
        break;
    case VK_IMAGE_LAYOUT_UNDEFINED:
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        *stages = VK_PIPELINE_STAGE_2_NONE;
        *access = VK_ACCESS_2_NONE;
        break;
    default:
        *stages = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        *access = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
        break;
    }
}

struct s_image_barrier {
    VkImage image;
    uint32_t aspect;
    uint32_t old_layout, new_layout;
    VkPipelineStageFlags2 src_stages, dst_stages;
    VkAccessFlags2 src_access, dst_access;
};

// One barrier command for `count` (at most k_max_image_barriers) image layout transitions of whole images.
const uint32_t k_max_image_barriers = VULKAN_MAX_COLOR_ATTACHMENTS + 1u;
static void cmd_image_barriers(s_vulkan_backend *backend, VkCommandBuffer command_buffer, const s_image_barrier *barriers,
                               uint32_t count)
{
    backend->bind_counters.barriers++;
    const VkImageSubresourceRange range = { 0, 0, VK_REMAINING_MIP_LEVELS, 0, VK_REMAINING_ARRAY_LAYERS };
    if (backend->synchronization2) {
        [[clang::uninitialized]] VkImageMemoryBarrier2 barrier[k_max_image_barriers];
        for (uint32_t i = 0; i < count; i++) {
            const s_image_barrier &b = barriers[i];
            barrier[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier[i].pNext = 0;
            barrier[i].srcStageMask = b.src_stages;
            barrier[i].srcAccessMask = b.src_access;
            barrier[i].dstStageMask = b.dst_stages;
            barrier[i].dstAccessMask = b.dst_access;
            barrier[i].oldLayout = (VkImageLayout)b.old_layout;
            barrier[i].newLayout = (VkImageLayout)b.new_layout;
            barrier[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier[i].image = b.image;
            barrier[i].subresourceRange = range;
            barrier[i].subresourceRange.aspectMask = b.aspect;
        }
        VkDependencyInfo dependency = {};
        dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependency.imageMemoryBarrierCount = count;
        dependency.pImageMemoryBarriers = barrier;
        backend->vk.vkCmdPipelineBarrier2(command_buffer, &dependency);
    } else {
        // one pair of stage masks for the command: the union of the barriers' (each waits on at least its own)
        [[clang::uninitialized]] VkImageMemoryBarrier barrier[k_max_image_barriers];
        VkPipelineStageFlags src = 0, dst = 0;
        for (uint32_t i = 0; i < count; i++) {
            const s_image_barrier &b = barriers[i];
            barrier[i].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
            barrier[i].pNext = 0;
            barrier[i].srcAccessMask = (VkAccessFlags)b.src_access;
            barrier[i].dstAccessMask = (VkAccessFlags)b.dst_access;
            barrier[i].oldLayout = (VkImageLayout)b.old_layout;
            barrier[i].newLayout = (VkImageLayout)b.new_layout;
            barrier[i].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier[i].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
            barrier[i].image = b.image;
            barrier[i].subresourceRange = range;
            barrier[i].subresourceRange.aspectMask = b.aspect;
            src |= (VkPipelineStageFlags)b.src_stages;
            dst |= (VkPipelineStageFlags)b.dst_stages;
        }
        src = src != 0 ? src : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst = dst != 0 ? dst : VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        backend->vk.vkCmdPipelineBarrier(command_buffer, src, dst, 0, 0, 0, 0, 0, count, barrier);
    }
}

static void cmd_image_barrier(s_vulkan_backend *backend, VkCommandBuffer command_buffer, VkImage image,
                              uint32_t aspect, uint32_t old_layout, uint32_t new_layout,
                              VkPipelineStageFlags2 src_stages, VkAccessFlags2 src_access,
                              VkPipelineStageFlags2 dst_stages, VkAccessFlags2 dst_access)
{
    s_image_barrier barrier;
    barrier.image = image;
    barrier.aspect = aspect;
    barrier.old_layout = old_layout;
    barrier.new_layout = new_layout;
    barrier.src_stages = src_stages;
    barrier.src_access = src_access;
    barrier.dst_stages = dst_stages;
    barrier.dst_access = dst_access;
    cmd_image_barriers(backend, command_buffer, &barrier, 1);
}

static void cmd_memory_barrier(s_vulkan_backend *backend, VkCommandBuffer command_buffer,
                               VkPipelineStageFlags2 src_stages, VkAccessFlags2 src_access,
                               VkPipelineStageFlags2 dst_stages, VkAccessFlags2 dst_access)
{
    backend->bind_counters.barriers++;
    if (backend->synchronization2) {
        VkMemoryBarrier2 barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
        barrier.srcStageMask = src_stages;
        barrier.srcAccessMask = src_access;
        barrier.dstStageMask = dst_stages;
        barrier.dstAccessMask = dst_access;
        VkDependencyInfo dependency = {};
        dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependency.memoryBarrierCount = 1;
        dependency.pMemoryBarriers = &barrier;
        backend->vk.vkCmdPipelineBarrier2(command_buffer, &dependency);
    } else {
        VkMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        barrier.srcAccessMask = (VkAccessFlags)src_access;
        barrier.dstAccessMask = (VkAccessFlags)dst_access;
        backend->vk.vkCmdPipelineBarrier(command_buffer, (VkPipelineStageFlags)src_stages,
                                         (VkPipelineStageFlags)dst_stages, 0, 1, &barrier, 0, 0, 0, 0);
    }
}

static constexpr VkAccessFlags2 k_memexport_shader_access = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT;
static constexpr VkPipelineStageFlags2 k_memexport_consumer_stages =
    VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT;
static constexpr VkAccessFlags2 k_memexport_consumer_access =
    VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_2_INDEX_READ_BIT |
    VK_ACCESS_2_TRANSFER_READ_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT;

static bool take_memexport_batch(s_vulkan_backend *backend)
{
    s_memexport_batch &batch = memexport_batch(backend);
    if (!batch.active)
        return false;
    const bool pending = batch.count != 0;
    batch.count = 0;
    batch.active = false;
    return pending;
}

void vulkan_cmd_flush_memexports(s_vulkan_backend *backend)
{
    if (take_memexport_batch(backend))
        cmd_memory_barrier(backend, backend->command_buffer, VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                           VK_ACCESS_2_SHADER_WRITE_BIT, backend->shader_stages | k_memexport_consumer_stages,
                           k_memexport_shader_access | k_memexport_consumer_access);
}

// One barrier command combining a memory barrier and an optional image layout transition.
static void cmd_memory_and_image_barrier(s_vulkan_backend *backend, VkCommandBuffer command_buffer,
                                         VkPipelineStageFlags2 mem_src_stages, VkAccessFlags2 mem_src_access,
                                         VkPipelineStageFlags2 mem_dst_stages, VkAccessFlags2 mem_dst_access,
                                         const s_image_barrier *img)
{
    if (img == 0) {
        cmd_memory_barrier(backend, command_buffer, mem_src_stages, mem_src_access, mem_dst_stages, mem_dst_access);
        return;
    }
    backend->bind_counters.barriers++;
    VkImageSubresourceRange range = {};
    range.aspectMask = img->aspect;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;
    if (backend->synchronization2) {
        VkMemoryBarrier2 mem = {};
        mem.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
        mem.srcStageMask = mem_src_stages;
        mem.srcAccessMask = mem_src_access;
        mem.dstStageMask = mem_dst_stages;
        mem.dstAccessMask = mem_dst_access;
        VkImageMemoryBarrier2 ib = {};
        ib.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
        ib.srcStageMask = img->src_stages;
        ib.srcAccessMask = img->src_access;
        ib.dstStageMask = img->dst_stages;
        ib.dstAccessMask = img->dst_access;
        ib.oldLayout = (VkImageLayout)img->old_layout;
        ib.newLayout = (VkImageLayout)img->new_layout;
        ib.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ib.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ib.image = img->image;
        ib.subresourceRange = range;
        VkDependencyInfo dependency = {};
        dependency.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dependency.memoryBarrierCount = 1;
        dependency.pMemoryBarriers = &mem;
        dependency.imageMemoryBarrierCount = 1;
        dependency.pImageMemoryBarriers = &ib;
        backend->vk.vkCmdPipelineBarrier2(command_buffer, &dependency);
    } else {
        VkMemoryBarrier mem = {};
        mem.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        mem.srcAccessMask = (VkAccessFlags)mem_src_access;
        mem.dstAccessMask = (VkAccessFlags)mem_dst_access;
        VkImageMemoryBarrier ib = {};
        ib.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        ib.srcAccessMask = (VkAccessFlags)img->src_access;
        ib.dstAccessMask = (VkAccessFlags)img->dst_access;
        ib.oldLayout = (VkImageLayout)img->old_layout;
        ib.newLayout = (VkImageLayout)img->new_layout;
        ib.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ib.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        ib.image = img->image;
        ib.subresourceRange = range;
        VkPipelineStageFlags src = (VkPipelineStageFlags)(mem_src_stages | img->src_stages);
        VkPipelineStageFlags dst = (VkPipelineStageFlags)(mem_dst_stages | img->dst_stages);
        src = src != 0 ? src : VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dst = dst != 0 ? dst : VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        backend->vk.vkCmdPipelineBarrier(command_buffer, src, dst, 0, 1, &mem, 0, 0, 1, &ib);
    }
}

// Moves a backend image (all subresources) to a new layout. discard: the old contents are not needed (they are about
// to be cleared), so the transition starts from UNDEFINED, still ordered after the image's earlier accesses.
// A transition from a read-only layout to itself orders a read after a read: there is nothing to wait for and nothing
// to make visible (the barrier that brought the image into the layout made the writes before it visible to every
// stage of the layout's scope). An attachment already in its layout also needs no barrier until a pass uses it.
static bool transition_barrier(s_vulkan_backend *backend, s_vulkan_image *image, uint32_t new_layout, bool discard,
                               s_image_barrier *out)
{
    if (image->layout == new_layout) {
        if (!discard && (new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ||
                         new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)) {
            backend->bind_counters.transitions_skipped++;
            return false;
        }
        if (!image->attachment_dirty &&
            (new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL ||
             new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)) {
            backend->bind_counters.transitions_skipped++;
            return false;
        }
    }
    layout_src_scope(backend, image->layout, &out->src_stages, &out->src_access);
    layout_dst_scope(backend, new_layout, &out->dst_stages, &out->dst_access);
    out->image = image->image;
    out->aspect = image->aspect;
    out->old_layout = discard ? (uint32_t)VK_IMAGE_LAYOUT_UNDEFINED : image->layout;
    out->new_layout = new_layout;
    image->layout = new_layout;
    image->attachment_dirty = 0;
    return true;
}

static void cmd_transition(s_vulkan_backend *backend, VkCommandBuffer command_buffer, s_vulkan_image *image,
                           uint32_t new_layout, bool discard = false)
{
    [[clang::uninitialized]] s_image_barrier barrier;
    if (transition_barrier(backend, image, new_layout, discard, &barrier))
        cmd_image_barriers(backend, command_buffer, &barrier, 1);
}

// A write at the origin of mip 0, layer 0 can discard the tracked whole-image layout only at this extent.
static bool covers_whole_2d_image(const s_vulkan_image *image, uint32_t width, uint32_t height)
{
    return width == image->desc.width && height == image->desc.height && image->desc.depth == 1 &&
           image->desc.mip_levels == 1 && image->desc.array_layers == 1;
}

// Two images' transitions (a copy's source and destination) in one barrier command.
static void cmd_transition_pair(s_vulkan_backend *backend, VkCommandBuffer command_buffer, s_vulkan_image *first,
                                uint32_t first_layout, s_vulkan_image *second, uint32_t second_layout,
                                bool second_discard = false)
{
    if (first == second) { // one image cannot be in both layouts: as cmd_transition, one after the other
        cmd_transition(backend, command_buffer, first, first_layout);
        cmd_transition(backend, command_buffer, second, second_layout, second_discard);
        return;
    }
    [[clang::uninitialized]] s_image_barrier barriers[2];
    uint32_t count = 0;
    count += transition_barrier(backend, first, first_layout, false, &barriers[count]) ? 1u : 0u;
    count += transition_barrier(backend, second, second_layout, second_discard, &barriers[count]) ? 1u : 0u;
    if (count != 0)
        cmd_image_barriers(backend, command_buffer, barriers, count);
}

// Submits command_buffer (after `first`, when not null: the frame's uploads, in the same batch so one fence covers both
// and they run first in submission order). The swapchain image is only touched by vulkan_cmd_blit_to_swapchain at the
// very end of the frame (TRANSFER stage), so waiting on `wait` at TRANSFER lets all scene rendering overlap acquire.
static VkResult queue_submit(s_vulkan_backend *backend, VkCommandBuffer command_buffer, VkSemaphore wait,
                             VkSemaphore signal, VkFence fence, VkCommandBuffer first = 0)
{
    const VkCommandBuffer buffers[2] = { first != 0 ? first : command_buffer, command_buffer };
    const uint32_t buffer_count = first != 0 ? 2u : 1u;
    if (backend->synchronization2) {
        VkCommandBufferSubmitInfo commands[2] = {};
        for (uint32_t i = 0; i < 2; i++) {
            commands[i].sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
            commands[i].commandBuffer = buffers[i];
        }
        VkSemaphoreSubmitInfo wait_info = {}, signal_info = {};
        wait_info.sType = signal_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        wait_info.semaphore = wait;
        wait_info.stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        signal_info.semaphore = signal;
        signal_info.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
        VkSubmitInfo2 submit = {};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        submit.waitSemaphoreInfoCount = wait != VK_NULL_HANDLE ? 1u : 0u;
        submit.pWaitSemaphoreInfos = &wait_info;
        submit.commandBufferInfoCount = buffer_count;
        submit.pCommandBufferInfos = commands;
        submit.signalSemaphoreInfoCount = signal != VK_NULL_HANDLE ? 1u : 0u;
        submit.pSignalSemaphoreInfos = &signal_info;
        return backend->vk.vkQueueSubmit2(backend->queue, 1, &submit, fence);
    }
    VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkSubmitInfo submit = {};
    submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.waitSemaphoreCount = wait != VK_NULL_HANDLE ? 1u : 0u;
    submit.pWaitSemaphores = &wait;
    submit.pWaitDstStageMask = &wait_stage;
    submit.commandBufferCount = buffer_count;
    submit.pCommandBuffers = buffers;
    submit.signalSemaphoreCount = signal != VK_NULL_HANDLE ? 1u : 0u;
    submit.pSignalSemaphores = &signal;
    return backend->vk.vkQueueSubmit(backend->queue, 1, &submit, fence);
}

static VkCommandBuffer immediate_begin(s_vulkan_backend *backend, const char *caller)
{
    backend->vk.vkResetCommandPool(backend->device, backend->immediate_pool, 0);
    VkCommandBufferBeginInfo begin = {};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    backend->vk.vkBeginCommandBuffer(backend->immediate_command_buffer, &begin);
    command_checkpoint(backend, backend->immediate_command_buffer, caller);
    return backend->immediate_command_buffer;
}

// Ends the immediate command buffer (making its transfer writes visible to all later commands), submits it and waits.
static VkResult immediate_submit(s_vulkan_backend *backend)
{
    s_vulkan_functions &vk = backend->vk;
    cmd_memory_barrier(backend, backend->immediate_command_buffer, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                       VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                       VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT);
    VkResult result = vk.vkEndCommandBuffer(backend->immediate_command_buffer);
    if (result == VK_SUCCESS)
        result = queue_submit(backend, backend->immediate_command_buffer, VK_NULL_HANDLE, VK_NULL_HANDLE,
                              backend->immediate_fence);
    if (result == VK_SUCCESS)
        result = vk.vkWaitForFences(backend->device, 1, &backend->immediate_fence, VK_TRUE, ~0ull);
    vk.vkResetFences(backend->device, 1, &backend->immediate_fence);
    backend->immediate_submits++;
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: upload submission failed: %s", vulkan_result_string(result));
        if (result == VK_ERROR_DEVICE_LOST)
            report_device_loss(backend);
    }
    return result;
}

// The open frame's upload command buffer, begun on first use; NULL outside a frame (the caller then submits at once).
// What is recorded here runs before every command of the frame (vulkan_frame_end submits it first in the frame's
// batch), so only uploads no earlier command of the frame can observe belong here: an image no command has used yet
// (layout UNDEFINED) and a buffer's first upload (vulkan_buffer_upload). Staging buffers are retired to the frame's
// slot and destroyed once its fence has signalled.
static VkCommandBuffer upload_begin(s_vulkan_backend *backend, const char *caller)
{
    if (!backend->recording)
        return 0;
    s_vulkan_frame &frame = backend->frames[backend->frame_index];
    if (frame.upload_command_buffer == 0)
        return 0;
    if (!frame.upload_recording) {
        VkCommandBufferBeginInfo begin = {};
        begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        if (backend->vk.vkBeginCommandBuffer(frame.upload_command_buffer, &begin) != VK_SUCCESS)
            return 0;
        frame.upload_recording = 1;
    }
    command_checkpoint(backend, frame.upload_command_buffer, caller);
    backend->deferred_uploads++;
    return frame.upload_command_buffer;
}

// Ends the frame's upload command buffer (its transfer writes made visible to every later command); NULL when the
// frame recorded no upload.
static VkCommandBuffer upload_end(s_vulkan_backend *backend, s_vulkan_frame &frame)
{
    if (!frame.upload_recording)
        return 0;
    frame.upload_recording = 0;
    cmd_memory_barrier(backend, frame.upload_command_buffer, VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                       VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                       VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT);
    if (backend->vk.vkEndCommandBuffer(frame.upload_command_buffer) != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: ending the frame's upload commands failed");
        return 0;
    }
    return frame.upload_command_buffer;
}

// ---- creation and destruction ----

static VkResult create_command_objects(s_vulkan_backend *backend)
{
    s_vulkan_functions &vk = backend->vk;
    VkCommandPoolCreateInfo pool = {};
    pool.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    pool.queueFamilyIndex = backend->queue_family;
    VkCommandBufferAllocateInfo allocate = {};
    allocate.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate.commandBufferCount = 1;
    VkFenceCreateInfo signaled_fence = {};
    signaled_fence.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    signaled_fence.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    VkSemaphoreCreateInfo semaphore = {};
    semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkResult result = VK_SUCCESS;
    for (uint32_t i = 0; i < backend->frame_count && result == VK_SUCCESS; i++) {
        s_vulkan_frame &frame = backend->frames[i];
        frame.acquired_image = VULKAN_NO_IMAGE;
        result = vk.vkCreateCommandPool(backend->device, &pool, 0, &frame.command_pool);
        if (result == VK_SUCCESS) {
            allocate.commandPool = frame.command_pool;
            result = vk.vkAllocateCommandBuffers(backend->device, &allocate, &frame.command_buffer);
        }
        if (result == VK_SUCCESS)
            result = vk.vkAllocateCommandBuffers(backend->device, &allocate, &frame.upload_command_buffer);
        if (result == VK_SUCCESS)
            result = vk.vkCreateFence(backend->device, &signaled_fence, 0, &frame.fence);
        if (result == VK_SUCCESS)
            result = vk.vkCreateSemaphore(backend->device, &semaphore, 0, &frame.acquire_semaphore);
    }
    for (uint32_t i = 0; i < backend->frame_count && result == VK_SUCCESS && vk.vkCreateQueryPool != 0; i++) {
        VkQueryPoolCreateInfo queries = {};
        queries.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
        queries.queryType = VK_QUERY_TYPE_OCCLUSION;
        queries.queryCount = VULKAN_OCCLUSION_SLOTS;
        if (vk.vkCreateQueryPool(backend->device, &queries, 0, &backend->frames[i].occlusion_pool) != VK_SUCCESS)
            backend->frames[i].occlusion_pool = VK_NULL_HANDLE; // occlusion queries then report no samples
        else
            backend->frames[i].occlusion_reset_count = VULKAN_OCCLUSION_SLOTS;
    }
    // GPU frame times (HALO3_FRAME_STATS): two timestamps per frame slot, when the queue family has timestamps
    // (or 256 timestamps when HALO3_GPU_PASS_TRACE is set for per-pass GPU timing).
    backend->gpu_frame_ms = -1.0f;
    int32_t pass_trace_setting = 0;
    if (environment_integer("HALO3_GPU_PASS_TRACE", &pass_trace_setting) && pass_trace_setting > 0)
        backend->pass_trace_period = pass_trace_setting == 1 ? 120u : (uint32_t)pass_trace_setting;
    if (result == VK_SUCCESS && vk.vkCreateQueryPool != 0 && vk.vkCmdWriteTimestamp != 0 &&
        backend->properties.limits.timestampPeriod > 0.0f) {
        uint32_t family_count = 0;
        vk.vkGetPhysicalDeviceQueueFamilyProperties(backend->physical_device, &family_count, 0);
        VkQueueFamilyProperties *families =
            (VkQueueFamilyProperties *)calloc(family_count + 1, sizeof(VkQueueFamilyProperties));
        if (families != 0) {
            vk.vkGetPhysicalDeviceQueueFamilyProperties(backend->physical_device, &family_count, families);
            if (backend->queue_family < family_count && families[backend->queue_family].timestampValidBits != 0)
                backend->timestamp_period = backend->properties.limits.timestampPeriod;
            free(families);
        }
        const uint32_t ts_count = backend->pass_trace_period != 0 ? VULKAN_GPU_PASS_TRACE_QUERIES : 2u;
        for (uint32_t i = 0; i < backend->frame_count && backend->timestamp_period > 0.0f; i++) {
            VkQueryPoolCreateInfo queries = {};
            queries.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
            queries.queryType = VK_QUERY_TYPE_TIMESTAMP;
            queries.queryCount = ts_count;
            if (vk.vkCreateQueryPool(backend->device, &queries, 0, &backend->frames[i].timestamp_pool) != VK_SUCCESS)
                backend->frames[i].timestamp_pool = VK_NULL_HANDLE;
            else if (backend->pass_trace_period != 0)
                backend->frames[i].pass_trace_spans =
                    (s_vulkan_gpu_pass_span *)calloc(VULKAN_GPU_PASS_TRACE_MAX_SPANS, sizeof(s_vulkan_gpu_pass_span));
        }
    }
    if (result == VK_SUCCESS)
        result = vk.vkCreateCommandPool(backend->device, &pool, 0, &backend->immediate_pool);
    if (result == VK_SUCCESS) {
        allocate.commandPool = backend->immediate_pool;
        result = vk.vkAllocateCommandBuffers(backend->device, &allocate, &backend->immediate_command_buffer);
    }
    if (result == VK_SUCCESS) {
        VkFenceCreateInfo fence = {};
        fence.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        result = vk.vkCreateFence(backend->device, &fence, 0, &backend->immediate_fence);
    }
    if (result != VK_SUCCESS)
        vulkan_log(backend, "vulkan: creating command buffers failed: %s", vulkan_result_string(result));
    return result;
}

static VkResult create_pipeline_cache(s_vulkan_backend *backend, const void *data, uint32_t size)
{
    VkPipelineCacheCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    create.initialDataSize = data != 0 ? size : 0;
    create.pInitialData = data;
    VkResult result = backend->vk.vkCreatePipelineCache(backend->device, &create, 0, &backend->pipeline_cache);
    if (result != VK_SUCCESS && data != 0) {
        // Data from another driver or version is rejected by some drivers instead of being ignored: start empty.
        create.initialDataSize = 0;
        create.pInitialData = 0;
        result = backend->vk.vkCreatePipelineCache(backend->device, &create, 0, &backend->pipeline_cache);
    }
    if (result != VK_SUCCESS)
        vulkan_log(backend, "vulkan: vkCreatePipelineCache failed: %s", vulkan_result_string(result));
    return result;
}

extern "C" VkResult vulkan_backend_create(const s_vulkan_backend_create_info *info, s_vulkan_backend **out_backend)
{
    *out_backend = 0;
    auto *storage = (s_vulkan_backend_storage *)calloc(1, sizeof(s_vulkan_backend_storage));
    if (storage == 0)
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    s_vulkan_backend *backend = &storage->backend;
    backend->log = info->log != 0 ? info->log : default_log;
    backend->log_context = info->log != 0 ? info->log_context : 0;
    backend->present_image = VULKAN_NO_IMAGE;
    backend->frame_count = info->frames_in_flight == 0 ? 2u
                           : info->frames_in_flight > VULKAN_MAX_FRAMES ? (uint32_t)VULKAN_MAX_FRAMES
                                                                        : info->frames_in_flight;
    const char *frames = getenv("HALO3_FRAMES_IN_FLIGHT");
    if (frames && frames[0] >= '2' && frames[0] <= '4' && frames[1] == '\0')
        backend->frame_count = (uint32_t)(frames[0] - '0');
    backend->frame_index = backend->frame_count - 1;
    backend->xenos_requested = info->xenos_features != 0;

    int32_t validation_setting = 0, device_index = info->device_index;
    bool validation = info->validation != 0 ||
                      (environment_integer("HALO3_VULKAN_VALIDATION", &validation_setting) && validation_setting != 0);
    environment_integer("HALO3_VULKAN_DEVICE", &device_index);

    VkResult result = VK_ERROR_INITIALIZATION_FAILED;
    backend->library = port_vk_load_library();
    if (backend->library == 0) {
        vulkan_log(backend, "vulkan: the Vulkan loader (%s) is not installed",
#ifdef _WIN32
                   "vulkan-1.dll"
#else
                   "libvulkan.so.1"
#endif
        );
    } else {
        backend->vk.vkGetInstanceProcAddr =
            (PFN_vkGetInstanceProcAddr)port_vk_get_proc_addr(backend->library, "vkGetInstanceProcAddr");
        if (backend->vk.vkGetInstanceProcAddr == 0 || !load_global_functions(backend))
            vulkan_log(backend, "vulkan: the Vulkan loader exports no usable vkGetInstanceProcAddr");
        else
            result = VK_SUCCESS;
    }
    if (result == VK_SUCCESS)
        result = create_instance(backend, info, validation);
    if (result == VK_SUCCESS)
        result = select_physical_device(backend, device_index);
    if (result == VK_SUCCESS)
        result = create_device(backend);
    if (result == VK_SUCCESS) {
        backend->shader_stages = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT |
                                 VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
        if (backend->tessellation)
            backend->shader_stages |= VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT |
                                      VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT;
        if (backend->geometry)
            backend->shader_stages |= VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT;
        // Detect full ReBAR (>= 1 GiB DEVICE_LOCAL | HOST_VISIBLE | HOST_COHERENT heap) so shader-read upload rings
        // can use VRAM bandwidth without risking 256 MiB BAR0 exhaustion on non-ReBAR laptops.
        const uint32_t rebar_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        for (uint32_t i = 0; i < backend->memory_properties.memoryTypeCount; i++) {
            const VkMemoryType &mt = backend->memory_properties.memoryTypes[i];
            if ((mt.propertyFlags & rebar_flags) == rebar_flags &&
                mt.heapIndex < backend->memory_properties.memoryHeapCount &&
                backend->memory_properties.memoryHeaps[mt.heapIndex].size >= (1024ull << 20)) {
                backend->rebar_upload = 1;
                break;
            }
        }
    }
    if (result == VK_SUCCESS)
        result = create_command_objects(backend);
    if (result == VK_SUCCESS)
        result = create_pipeline_cache(backend, info->pipeline_cache_data, info->pipeline_cache_size);
    if (result != VK_SUCCESS) {
        vulkan_backend_destroy(backend);
        return result;
    }
    // Pre-allocate each frame slot's staging ring up front so the first streaming burst on the draw thread does not
    // stall on vkAllocateMemory (staging_ring_alloc still falls back to lazy creation if allocation fails here).
    for (uint32_t i = 0; i < backend->frame_count; i++)
        backend->frames[i].staging_ring =
            vulkan_buffer_create(backend, VULKAN_STAGING_RING_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 k_vulkan_memory_upload);
    const VkPhysicalDeviceProperties &properties = backend->properties;
    vulkan_log(backend, "vulkan: using %s (%s, Vulkan %u.%u.%u)%s%s", properties.deviceName,
               device_type_name(properties.deviceType), VK_API_VERSION_MAJOR(properties.apiVersion),
               VK_API_VERSION_MINOR(properties.apiVersion), VK_API_VERSION_PATCH(properties.apiVersion),
               backend->validation ? ", validation on" : "", backend->presentation ? ", presentation" : "");
    *out_backend = backend;
    return VK_SUCCESS;
}

extern "C" void vulkan_backend_destroy(s_vulkan_backend *backend)
{
    if (backend == 0)
        return;
    s_vulkan_functions &vk = backend->vk;
    if (backend->device != 0) {
        vk.vkDeviceWaitIdle(backend->device);
        vulkan_bindless_destroy(backend);
        vulkan_pack_release(backend);
        for (s_vulkan_image_pass &pass : backend->image_passes) {
            if (pass.pipeline != 0)
                vulkan_pipeline_destroy(backend, pass.pipeline);
            if (pass.module != VK_NULL_HANDLE)
                vulkan_shader_module_destroy(backend, pass.module);
        }
        vulkan_swapchain_destroy(backend);
#ifdef HALO3_64BIT
        if (backend->depth_gather_buffer != 0)
            vulkan_buffer_destroy(backend, backend->depth_gather_buffer);
#endif
        if (backend->null_vertex_buffer != 0)
            vulkan_buffer_destroy(backend, backend->null_vertex_buffer);
        for (uint32_t i = 0; i < backend->pipeline_layout_count; i++)
            vk.vkDestroyPipelineLayout(backend->device, backend->pipeline_layouts[i].layout, 0);
        for (uint32_t i = 0; i < backend->set_layout_count; i++) {
            vk.vkDestroyDescriptorSetLayout(backend->device, backend->set_layouts[i].layout, 0);
            free(backend->set_layouts[i].bindings);
        }
        destroy_descriptor_pools(backend, &backend->persistent_pools);
        destroy_descriptor_pools(backend, &backend->texture_set_pools);
        for (uint32_t i = 0; i < backend->frame_count; i++) {
            s_vulkan_frame &frame = backend->frames[i];
            destroy_retired(backend, frame);
            vulkan_buffer_destroy(backend, frame.staging_ring);
            free(frame.retired.items);
            destroy_descriptor_pools(backend, &frame.transient_pools);
            destroy_descriptor_pools(backend, &frame.texture_pools);
            vk.vkDestroySemaphore(backend->device, frame.acquire_semaphore, 0);
            if (frame.occlusion_pool != VK_NULL_HANDLE)
                vk.vkDestroyQueryPool(backend->device, frame.occlusion_pool, 0);
            if (frame.timestamp_pool != VK_NULL_HANDLE)
                vk.vkDestroyQueryPool(backend->device, frame.timestamp_pool, 0);
            free(frame.pass_trace_spans);
            frame.pass_trace_spans = 0;
            vk.vkDestroyFence(backend->device, frame.fence, 0);
            vk.vkDestroyCommandPool(backend->device, frame.command_pool, 0);
        }
        destroy_descriptor_pools(backend, &backend->free_texture_pools);
        buffer_pool_trim(backend, true); // after every slot's retired buffers joined it
        vk.vkDestroyFence(backend->device, backend->immediate_fence, 0);
        vk.vkDestroyCommandPool(backend->device, backend->immediate_pool, 0);
        vk.vkDestroyPipelineCache(backend->device, backend->pipeline_cache, 0);
        vk.vkDestroyDevice(backend->device, 0);
#ifdef HALO3_64BIT
        // Device destruction also releases any images the client left alive.
        while (backend->texture_memory) {
            s_vulkan_texture_memory *block = backend->texture_memory;
            backend->texture_memory = block->next;
            free(block);
        }
#endif
    }
    if (backend->instance != 0) {
        vulkan_surface_destroy(backend);
        if (backend->messenger != VK_NULL_HANDLE && vk.vkDestroyDebugUtilsMessengerEXT != 0)
            vk.vkDestroyDebugUtilsMessengerEXT(backend->instance, backend->messenger, 0);
        vk.vkDestroyInstance(backend->instance, 0);
    }
    port_vk_unload_library(backend->library);
    free(backend->set_layouts);
    free(backend->set_layout_index.slots);
    free(backend->pipeline_layouts);
    free(backend->pipeline_layout_index.slots);
    free(backend->persistent_sets);
    free(reinterpret_cast<s_vulkan_backend_storage *>(backend));
}

extern "C" void vulkan_backend_get_device_info(const s_vulkan_backend *backend, s_vulkan_device_info *out)
{
    memset(out, 0, sizeof(*out));
    memcpy(out->name, backend->properties.deviceName, sizeof(out->name));
    out->name[sizeof(out->name) - 1] = 0;
    out->device_type = backend->properties.deviceType;
    out->api_version = backend->properties.apiVersion;
    out->driver_version = backend->properties.driverVersion;
    out->vendor_id = backend->properties.vendorID;
    out->device_id = backend->properties.deviceID;
    out->queue_family = backend->queue_family;
    out->synchronization2 = backend->synchronization2;
    out->presentation = backend->presentation;
    out->validation = backend->validation;
    out->depth_format = backend->depth_format;
    out->frames_in_flight = backend->frame_count;
    out->buffer_device_address = backend->device_address;
    out->tessellation = backend->tessellation;
    out->geometry = backend->geometry;
    out->clip_distance = backend->clip_distance;
    out->full_draw_index = backend->full_draw_index;
    out->pipeline_library = backend->pipeline_library;
    out->pipeline_library_fast_link = backend->pipeline_library_fast_link;
}

extern "C" VkResult vulkan_backend_wait_idle(s_vulkan_backend *backend)
{
    return backend->vk.vkDeviceWaitIdle(backend->device);
}

extern "C" VkResult vulkan_backend_get_pipeline_cache(s_vulkan_backend *backend, void *data, size_t *size)
{
    return backend->vk.vkGetPipelineCacheData(backend->device, backend->pipeline_cache, size, data);
}

extern "C" uint32_t vulkan_backend_pipelines_created(const s_vulkan_backend *backend)
{
    return backend != 0 ? backend->pipelines_created : 0u;
}

extern "C" uint32_t vulkan_backend_format_features(s_vulkan_backend *backend, uint32_t format, uint32_t buffer_features)
{
    VkFormatProperties properties;
    backend->vk.vkGetPhysicalDeviceFormatProperties(backend->physical_device, (VkFormat)format, &properties);
    return buffer_features ? properties.bufferFeatures : properties.optimalTilingFeatures;
}

extern "C" const s_vulkan_functions *vulkan_backend_functions(const s_vulkan_backend *backend) { return &backend->vk; }
extern "C" VkInstance vulkan_backend_instance(const s_vulkan_backend *backend) { return backend->instance; }
extern "C" VkPhysicalDevice vulkan_backend_physical_device(const s_vulkan_backend *backend) { return backend->physical_device; }
extern "C" VkDevice vulkan_backend_device(const s_vulkan_backend *backend) { return backend->device; }
extern "C" VkQueue vulkan_backend_queue(const s_vulkan_backend *backend) { return backend->queue; }

// ---- memory, buffers ----

static uint32_t find_memory_type_ex(const s_vulkan_backend *backend, uint32_t type_bits, uint32_t required,
                                    uint32_t preferred, uint32_t avoid)
{
    const VkPhysicalDeviceMemoryProperties &memory = backend->memory_properties;
    uint32_t pref_any = VULKAN_NO_MEMORY_TYPE;
    uint32_t fallback = VULKAN_NO_MEMORY_TYPE;
    for (uint32_t i = 0; i < memory.memoryTypeCount; i++) {
        uint32_t flags = memory.memoryTypes[i].propertyFlags;
        if ((type_bits & (1u << i)) == 0 || (flags & required) != required)
            continue;
        if ((flags & preferred) == preferred) {
            if (avoid == 0 || (flags & avoid) == 0)
                return i;
            if (pref_any == VULKAN_NO_MEMORY_TYPE)
                pref_any = i;
        }
        if (fallback == VULKAN_NO_MEMORY_TYPE)
            fallback = i;
    }
    return pref_any != VULKAN_NO_MEMORY_TYPE ? pref_any : fallback;
}

extern "C" uint32_t vulkan_find_memory_type(const s_vulkan_backend *backend, uint32_t type_bits, uint32_t required,
                                            uint32_t preferred)
{
    return find_memory_type_ex(backend, type_bits, required, preferred, 0);
}

// Allocates memory for a resource or a sampled-texture slab. Warn near the device's allocation-count limit.
static VkDeviceMemory allocate_memory(s_vulkan_backend *backend, const VkMemoryRequirements &requirements,
                                      uint32_t required, uint32_t preferred, uint32_t *out_flags,
                                      bool device_address = false,
#ifdef HALO3_64BIT
                                      const void *allocation_next = 0,
#endif
                                      uint32_t avoid = 0)
{
    uint32_t type = find_memory_type_ex(backend, requirements.memoryTypeBits, required, preferred, avoid);
    if (type == VULKAN_NO_MEMORY_TYPE) {
        vulkan_log(backend, "vulkan: no memory type with flags 0x%x for type bits 0x%x", required,
                   requirements.memoryTypeBits);
        return VK_NULL_HANDLE;
    }
    VkMemoryAllocateInfo allocate = {};
    allocate.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate.allocationSize = requirements.size;
    allocate.memoryTypeIndex = type;
#ifdef HALO3_64BIT
    allocate.pNext = allocation_next;
#endif
    VkMemoryAllocateFlagsInfo allocate_flags = {};
    if (device_address && backend->device_address) {
        allocate_flags.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
        allocate_flags.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
        allocate.pNext = &allocate_flags;
    }
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkResult result = backend->vk.vkAllocateMemory(backend->device, &allocate, 0, &memory);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: vkAllocateMemory(%u bytes) failed: %s", (uint32_t)requirements.size,
                   vulkan_result_string(result));
        return VK_NULL_HANDLE;
    }
    backend->allocation_count++;
    backend->memory_allocations++;
    uint32_t limit = backend->properties.limits.maxMemoryAllocationCount;
    if (backend->allocation_count == limit - limit / 10)
        vulkan_log(backend, "vulkan: %u memory allocations, near the device limit of %u", backend->allocation_count,
                   backend->properties.limits.maxMemoryAllocationCount);
    *out_flags = backend->memory_properties.memoryTypes[type].propertyFlags;
    return memory;
}

static void free_memory(s_vulkan_backend *backend, VkDeviceMemory memory)
{
    if (memory != VK_NULL_HANDLE) {
        backend->vk.vkFreeMemory(backend->device, memory, 0);
        backend->allocation_count--;
    }
}

#ifdef HALO3_64BIT
// Turning into an unseen room can create hundreds of textures on the draw thread.
// Share small, optimal sampled-image allocations instead of asking the driver for
// a new device allocation per texture. Pages become reusable only at image_destroy_now,
// after the same frame fence that already protects image/view destruction.
static VkDeviceMemory allocate_texture_memory(s_vulkan_backend *backend, s_vulkan_image *image,
                                               VkMemoryRequirements requirements)
{
    uint32_t flags = 0;
    VkMemoryDedicatedRequirements dedicated = {};
    dedicated.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS;
    PFN_vkGetImageMemoryRequirements2 get_requirements = backend->vk.vkGetImageMemoryRequirements2;
    if (get_requirements) {
        VkImageMemoryRequirementsInfo2 query = {};
        query.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2;
        query.image = image->image;
        VkMemoryRequirements2 result = {};
        result.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
        result.pNext = &dedicated;
        get_requirements(backend->device, &query, &result);
        requirements = result.memoryRequirements;
    }
    const uint32_t sampled_usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if (get_requirements && !dedicated.requiresDedicatedAllocation && !dedicated.prefersDedicatedAllocation &&
        image->desc.usage == sampled_usage && requirements.size <= (4u << 20) && requirements.alignment <= 65536u) {
        const uint32_t type = vulkan_find_memory_type(backend, requirements.memoryTypeBits,
                                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        if (type != VULKAN_NO_MEMORY_TYPE) {
            const uint32_t pages = (uint32_t)((requirements.size + 65535u) / 65536u);
            for (s_vulkan_texture_memory *block = backend->texture_memory; block; block = block->next) {
                if (block->type != type)
                    continue;
                uint32_t run = 0;
                for (uint32_t page = 0; page < 512u; ++page) {
                    run = (block->used[page / 32u] & (1u << (page % 32u))) ? 0u : run + 1u;
                    if (run != pages)
                        continue;
                    image->memory_page = page + 1u - pages;
                    image->memory_pages = pages;
                    image->texture_memory = block;
                    for (uint32_t p = image->memory_page; p <= page; ++p)
                        block->used[p / 32u] |= 1u << (p % 32u);
                    ++block->live;
                    return block->memory;
                }
            }
            s_vulkan_texture_memory *block = (s_vulkan_texture_memory *)calloc(1, sizeof(*block));
            if (block) {
                VkMemoryRequirements slab = requirements;
                slab.size = 32u << 20;
                slab.memoryTypeBits = 1u << type;
                block->memory = allocate_memory(backend, slab, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &flags);
                if (block->memory != VK_NULL_HANDLE) {
                    block->type = type;
                    block->live = 1;
                    for (uint32_t p = 0; p < pages; ++p)
                        block->used[p / 32u] |= 1u << (p % 32u);
                    block->next = backend->texture_memory;
                    backend->texture_memory = block;
                    image->texture_memory = block;
                    image->memory_pages = pages;
                    return block->memory;
                }
                free(block);
            }
        }
    }
    VkMemoryDedicatedAllocateInfo allocation = {};
    allocation.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
    allocation.image = image->image;
    return allocate_memory(backend, requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &flags, false,
                           dedicated.requiresDedicatedAllocation || dedicated.prefersDedicatedAllocation ? &allocation : 0);
}

static void release_texture_memory(s_vulkan_backend *backend, s_vulkan_image *image)
{
    s_vulkan_texture_memory *block = image->texture_memory;
    if (!block) {
        free_memory(backend, image->memory);
        return;
    }
    for (uint32_t p = image->memory_page; p < image->memory_page + image->memory_pages; ++p)
        block->used[p / 32u] &= ~(1u << (p % 32u));
    if (--block->live == 0) {
        s_vulkan_texture_memory **link = &backend->texture_memory;
        while (*link != block)
            link = &(*link)->next;
        *link = block->next;
        free_memory(backend, block->memory);
        free(block);
    }
}
#endif

// ---- buffer pool (s_vulkan_pooled_buffer) ----

static void buffer_pool_remove(s_vulkan_backend *backend, uint32_t index)
{
    backend->buffer_pool_bytes -= backend->buffer_pool[index].buffer->size;
    backend->buffer_pool_count--;
    for (uint32_t i = index; i < backend->buffer_pool_count; i++) // the order (oldest first) is kept
        backend->buffer_pool[i] = backend->buffer_pool[i + 1];
}

static void buffer_pool_destroy_oldest(s_vulkan_backend *backend)
{
    s_vulkan_buffer *oldest = backend->buffer_pool[0].buffer;
    buffer_pool_remove(backend, 0);
    vulkan_buffer_destroy(backend, oldest);
}

// A pooled buffer of exactly this size, usage and memory (the most recently pooled first), or NULL. It comes back as
// vulkan_buffer_create makes one: no upload has gone to it (its first may be deferred) and its contents are undefined.
static s_vulkan_buffer *buffer_pool_take(s_vulkan_backend *backend, uint32_t size, uint32_t usage,
                                         uint32_t memory_usage)
{
    for (uint32_t i = backend->buffer_pool_count; i-- > 0;) {
        s_vulkan_buffer *buffer = backend->buffer_pool[i].buffer;
        if (buffer->size == size && buffer->usage == usage && buffer->memory_usage == memory_usage) {
            buffer_pool_remove(backend, i);
            buffer->uploads = 0;
            backend->buffers_reused++;
            return buffer;
        }
    }
    return 0;
}

// A buffer no command will use again (its frame's fence or the immediate fence has signalled): into the pool, or
// destroyed when it is too large for it (or HALO3_VULKAN_CHECKPOINTS logs every buffer's life).
static void buffer_release(s_vulkan_backend *backend, s_vulkan_buffer *buffer)
{
    if (buffer == 0)
        return;
    if (buffer->size > VULKAN_BUFFER_POOL_BYTES / 4u || backend->buffer_addresses) {
        vulkan_buffer_destroy(backend, buffer);
        return;
    }
    while (backend->buffer_pool_count != 0 && (backend->buffer_pool_count == VULKAN_BUFFER_POOL_CAPACITY ||
                                               backend->buffer_pool_bytes + buffer->size > VULKAN_BUFFER_POOL_BYTES))
        buffer_pool_destroy_oldest(backend);
    s_vulkan_pooled_buffer &slot = backend->buffer_pool[backend->buffer_pool_count++];
    slot.buffer = buffer;
    slot.serial = backend->frame_serial;
    backend->buffer_pool_bytes += buffer->size;
}

// Destroys the pooled buffers unused for VULKAN_BUFFER_POOL_FRAMES frames (all: every one).
static void buffer_pool_trim(s_vulkan_backend *backend, bool all)
{
    while (backend->buffer_pool_count != 0 &&
           (all || backend->frame_serial - backend->buffer_pool[0].serial > VULKAN_BUFFER_POOL_FRAMES))
        buffer_pool_destroy_oldest(backend);
}

extern "C" s_vulkan_buffer *vulkan_buffer_create(s_vulkan_backend *backend, uint32_t size, uint32_t usage,
                                                 uint32_t memory_usage)
{
    s_vulkan_functions &vk = backend->vk;
    usage |= memory_usage == k_vulkan_memory_gpu ? (uint32_t)VK_BUFFER_USAGE_TRANSFER_DST_BIT : 0u;
    if (backend->device_address)
        usage |= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    if (s_vulkan_buffer *pooled = buffer_pool_take(backend, size, usage, memory_usage))
        return pooled;
    s_vulkan_buffer *buffer = (s_vulkan_buffer *)calloc(1, sizeof(s_vulkan_buffer));
    if (buffer == 0)
        return 0;
    buffer->size = size;
    buffer->memory_usage = memory_usage;
    buffer->usage = usage;
    VkBufferCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    create.size = size != 0 ? size : 4;
    create.usage = usage;
    create.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VkResult result = vk.vkCreateBuffer(backend->device, &create, 0, &buffer->buffer);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: vkCreateBuffer(%u bytes) failed: %s", size, vulkan_result_string(result));
        free(buffer);
        return 0;
    }
    VkMemoryRequirements requirements;
    vk.vkGetBufferMemoryRequirements(backend->device, buffer->buffer, &requirements);
    uint32_t required, preferred, avoid = 0;
    switch (memory_usage) {
    case k_vulkan_memory_upload:
        required = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        if (usage == VK_BUFFER_USAGE_TRANSFER_SRC_BIT) {
            // Pure DMA staging ring: stay out of DEVICE_LOCAL so 256 MiB BAR0 is never exhausted on laptops.
            preferred = required;
            avoid = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        } else if (backend->rebar_upload != 0) {
            // Shader-read upload rings on full ReBAR GPUs: prefer VRAM-backed HOST_VISIBLE memory.
            preferred = required | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        } else {
            // Non-ReBAR GPU: keep shader-read rings in WC sysmem so 256 MiB BAR0 stays free for driver use.
            preferred = required;
            avoid = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        }
        break;
    case k_vulkan_memory_readback:
        required = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        preferred = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
        break;
    default:
        required = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        preferred = required;
        break;
    }
    uint32_t flags = 0;
#ifdef HALO3_64BIT
    buffer->memory = allocate_memory(backend, requirements, required, preferred, &flags, true, 0, avoid);
#else
    buffer->memory = allocate_memory(backend, requirements, required, preferred, &flags, true, avoid);
#endif
    if (buffer->memory == VK_NULL_HANDLE) {
        vulkan_buffer_destroy(backend, buffer);
        return 0;
    }
    buffer->coherent = (flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
    result = vk.vkBindBufferMemory(backend->device, buffer->buffer, buffer->memory, 0);
    if (result == VK_SUCCESS && memory_usage != k_vulkan_memory_gpu)
        result = vk.vkMapMemory(backend->device, buffer->memory, 0, VK_WHOLE_SIZE, 0, &buffer->mapped);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: binding or mapping buffer memory failed: %s", vulkan_result_string(result));
        vulkan_buffer_destroy(backend, buffer);
        return 0;
    }
    if (backend->buffer_addresses && backend->vk.vkGetBufferDeviceAddress != 0) {
        VkBufferDeviceAddressInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.buffer = buffer->buffer;
        const VkDeviceAddress address = backend->vk.vkGetBufferDeviceAddress(backend->device, &info);
        vulkan_log(backend, "vulkan: buffer %p at 0x%x%08x, %u bytes, usage 0x%x", buffer, (uint32_t)(address >> 32),
                   (uint32_t)address, size, usage);
    }
    return buffer;
}

extern "C" void vulkan_buffer_device_address(const s_vulkan_backend *backend, const s_vulkan_buffer *buffer,
                                             uint32_t *out_lo, uint32_t *out_hi)
{
    VkDeviceAddress address = 0;
    if (buffer != 0 && backend->device_address && backend->vk.vkGetBufferDeviceAddress != 0) {
        VkBufferDeviceAddressInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
        info.buffer = buffer->buffer;
        address = backend->vk.vkGetBufferDeviceAddress(backend->device, &info);
    }
    *out_lo = (uint32_t)address;
    *out_hi = (uint32_t)(address >> 32);
}

extern "C" void vulkan_buffer_destroy(s_vulkan_backend *backend, s_vulkan_buffer *buffer)
{
    if (buffer == 0)
        return;
    if (backend->buffer_addresses)
        vulkan_log(backend, "vulkan: buffer %p destroyed", buffer);
    backend->vk.vkDestroyBuffer(backend->device, buffer->buffer, 0);
    free_memory(backend, buffer->memory);
    free(buffer);
}

// ---- deferred destruction ----

static void image_destroy_now(s_vulkan_backend *backend, s_vulkan_image *image);

static void retire(s_vulkan_backend *backend, s_vulkan_buffer *buffer, s_vulkan_image *image,
                   VkPipeline pipeline = VK_NULL_HANDLE, VkDescriptorPool descriptor_pool = VK_NULL_HANDLE)
{
    s_vulkan_retire_list &list = backend->frames[backend->frame_index].retired;
    if (list.count == list.capacity) {
        const uint32_t capacity = list.capacity != 0 ? list.capacity * 2u : 64u;
        s_vulkan_retired *items = (s_vulkan_retired *)realloc(list.items, capacity * sizeof(s_vulkan_retired));
        if (items == 0) {
            // no room to defer it: wait for every frame instead
            backend->vk.vkDeviceWaitIdle(backend->device);
            vulkan_buffer_destroy(backend, buffer);
            vulkan_image_destroy(backend, image);
            if (pipeline != VK_NULL_HANDLE)
                backend->vk.vkDestroyPipeline(backend->device, pipeline, 0);
            if (descriptor_pool != VK_NULL_HANDLE)
                backend->vk.vkDestroyDescriptorPool(backend->device, descriptor_pool, 0);
            return;
        }
        list.items = items;
        list.capacity = capacity;
    }
    list.items[list.count].buffer = buffer;
    list.items[list.count].image = image;
    list.items[list.count].pipeline = pipeline;
    list.items[list.count].descriptor_pool = descriptor_pool;
    list.count++;
}

// Destroys what a frame slot retired; its fence has been waited for.
static void destroy_retired(s_vulkan_backend *backend, s_vulkan_frame &frame)
{
    for (uint32_t i = 0; i < frame.retired.count; i++) {
        buffer_release(backend, frame.retired.items[i].buffer);
        image_destroy_now(backend, frame.retired.items[i].image); // the view epoch moved when it was retired
        if (frame.retired.items[i].pipeline != VK_NULL_HANDLE)
            backend->vk.vkDestroyPipeline(backend->device, frame.retired.items[i].pipeline, 0);
        if (frame.retired.items[i].descriptor_pool != VK_NULL_HANDLE) {
            VkDescriptorPool pool = frame.retired.items[i].descriptor_pool;
            s_vulkan_descriptor_pools &free_pools = backend->free_texture_pools;
            bool recycled = false;
            if (free_pools.count < 64u &&
                backend->vk.vkResetDescriptorPool(backend->device, pool, 0) == VK_SUCCESS) {
                if (free_pools.count == free_pools.capacity) {
                    const uint32_t cap = free_pools.capacity != 0 ? free_pools.capacity * 2u : 8u;
                    VkDescriptorPool *grown =
                        (VkDescriptorPool *)realloc(free_pools.pools, cap * sizeof(VkDescriptorPool));
                    if (grown != 0) {
                        free_pools.pools = grown;
                        free_pools.capacity = cap;
                    }
                }
                if (free_pools.count < free_pools.capacity) {
                    free_pools.pools[free_pools.count++] = pool;
                    recycled = true;
                }
            }
            if (!recycled)
                backend->vk.vkDestroyDescriptorPool(backend->device, pool, 0);
        }
    }
    frame.retired.count = 0;
}

extern "C" void vulkan_buffer_retire(s_vulkan_backend *backend, s_vulkan_buffer *buffer)
{
    if (buffer != 0)
        retire(backend, buffer, 0);
}

extern "C" void vulkan_image_retire(s_vulkan_backend *backend, s_vulkan_image *image)
{
    if (image != 0) {
        if ((image->desc.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0)
            backend->view_epoch++; // texture sets holding its views must not be bound again (vulkan_texture_sets_reset)
        retire(backend, 0, image);
    }
}

extern "C" VkBuffer vulkan_buffer_handle(const s_vulkan_buffer *buffer) { return buffer->buffer; }
extern "C" uint32_t vulkan_buffer_size(const s_vulkan_buffer *buffer) { return buffer->size; }
extern "C" void *vulkan_buffer_mapped(const s_vulkan_buffer *buffer) { return buffer->mapped; }

extern "C" void vulkan_buffer_invalidate(s_vulkan_backend *backend, s_vulkan_buffer *buffer)
{
    if (buffer->mapped == 0 || buffer->coherent)
        return;
    VkMappedMemoryRange range = {};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = buffer->memory;
    range.size = VK_WHOLE_SIZE;
    backend->vk.vkInvalidateMappedMemoryRanges(backend->device, 1, &range);
}

// `size` bytes at a multiple of `alignment` in the recording frame's staging ring (VULKAN_STAGING_RING_BYTES), for an
// upload recorded into the frame's upload command buffer (upload_begin); NULL when the ring has no room left.
static s_vulkan_buffer *staging_ring_alloc(s_vulkan_backend *backend, uint32_t size, uint32_t alignment,
                                           uint32_t *out_offset)
{
    s_vulkan_frame &frame = backend->frames[backend->frame_index];
    if (!backend->recording || size > VULKAN_STAGING_RING_BYTES)
        return 0;
    if (frame.staging_ring == 0) {
        frame.staging_ring = vulkan_buffer_create(backend, VULKAN_STAGING_RING_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                  k_vulkan_memory_upload);
        if (frame.staging_ring == 0)
            return 0;
    }
    const uint32_t start = (frame.staging_used + alignment - 1u) / alignment * alignment;
    if (start > VULKAN_STAGING_RING_BYTES || size > VULKAN_STAGING_RING_BYTES - start)
        return 0;
    frame.staging_used = start + size;
    *out_offset = start;
    return frame.staging_ring;
}

extern "C" VkResult vulkan_buffer_upload(s_vulkan_backend *backend, s_vulkan_buffer *buffer, uint32_t offset,
                                         const void *data, uint32_t size)
{
    if (offset > buffer->size || size > buffer->size - offset) {
        vulkan_log(backend, "vulkan: buffer upload of %u bytes at %u overruns the %u-byte buffer", size, offset,
                   buffer->size);
        return VK_ERROR_UNKNOWN;
    }
    if (size == 0)
        return VK_SUCCESS;
    if (buffer->mapped != 0) {
        memcpy((uint8_t *)buffer->mapped + offset, data, size);
        return VK_SUCCESS;
    }
    VkBufferCopy copy = {};
    copy.dstOffset = offset;
    copy.size = size;
    // a buffer's first upload: no command can have read it yet, so it may run ahead of the frame (upload_begin), its
    // bytes staged in the frame's staging ring when they fit
    VkCommandBuffer deferred = buffer->uploads++ == 0 ? upload_begin(backend, "vulkan_buffer_upload") : 0;
    uint32_t ring_offset = 0;
    if (s_vulkan_buffer *ring = deferred != 0 ? staging_ring_alloc(backend, size, 4u, &ring_offset) : 0) {
        memcpy((uint8_t *)ring->mapped + ring_offset, data, size);
        copy.srcOffset = ring_offset;
        backend->vk.vkCmdCopyBuffer(deferred, ring->buffer, buffer->buffer, 1, &copy);
        return VK_SUCCESS;
    }
    s_vulkan_buffer *staging = vulkan_buffer_create(backend, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, k_vulkan_memory_upload);
    if (staging == 0)
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    memcpy(staging->mapped, data, size);
    if (deferred != 0) {
        backend->vk.vkCmdCopyBuffer(deferred, staging->buffer, buffer->buffer, 1, &copy);
        retire(backend, staging, 0);
        return VK_SUCCESS;
    }
    VkCommandBuffer command_buffer = immediate_begin(backend, "vulkan_buffer_upload");
    backend->vk.vkCmdCopyBuffer(command_buffer, staging->buffer, buffer->buffer, 1, &copy);
    VkResult result = immediate_submit(backend);
    if (result == VK_SUCCESS)
        buffer_release(backend, staging);
    else
        vulkan_buffer_destroy(backend, staging);
    return result;
}

extern "C" VkBufferView vulkan_buffer_view_create(s_vulkan_backend *backend, s_vulkan_buffer *buffer, uint32_t format,
                                                  uint32_t offset, uint32_t range)
{
    VkBufferViewCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
    create.buffer = buffer->buffer;
    create.format = (VkFormat)format;
    create.offset = offset;
    create.range = range != 0 ? (VkDeviceSize)range : VK_WHOLE_SIZE;
    VkBufferView view = VK_NULL_HANDLE;
    VkResult result = backend->vk.vkCreateBufferView(backend->device, &create, 0, &view);
    if (result != VK_SUCCESS)
        vulkan_log(backend, "vulkan: vkCreateBufferView failed: %s", vulkan_result_string(result));
    return view;
}

extern "C" void vulkan_buffer_view_destroy(s_vulkan_backend *backend, VkBufferView view)
{
    backend->vk.vkDestroyBufferView(backend->device, view, 0);
}

// ---- images ----

extern "C" int vulkan_format_block_info(uint32_t format, uint32_t *out_block_width, uint32_t *out_block_height,
                                        uint32_t *out_bytes_per_block)
{
    uint32_t block = 1, bytes = 0;
    if (format == VK_FORMAT_R4G4_UNORM_PACK8 || (format >= VK_FORMAT_R8_UNORM && format <= VK_FORMAT_R8_SRGB) ||
        format == VK_FORMAT_S8_UINT)
        bytes = 1;
    else if ((format >= VK_FORMAT_R4G4B4A4_UNORM_PACK16 && format <= VK_FORMAT_A1R5G5B5_UNORM_PACK16) ||
             (format >= VK_FORMAT_R8G8_UNORM && format <= VK_FORMAT_R8G8_SRGB) ||
             (format >= VK_FORMAT_R16_UNORM && format <= VK_FORMAT_R16_SFLOAT) || format == VK_FORMAT_D16_UNORM)
        bytes = 2;
    else if ((format >= VK_FORMAT_R8G8B8A8_UNORM && format <= VK_FORMAT_A2B10G10R10_SINT_PACK32) ||
             (format >= VK_FORMAT_R16G16_UNORM && format <= VK_FORMAT_R16G16_SFLOAT) ||
             (format >= VK_FORMAT_R32_UINT && format <= VK_FORMAT_R32_SFLOAT) ||
             format == VK_FORMAT_B10G11R11_UFLOAT_PACK32 || format == VK_FORMAT_E5B9G9R9_UFLOAT_PACK32 ||
             format == VK_FORMAT_X8_D24_UNORM_PACK32 || format == VK_FORMAT_D32_SFLOAT)
        bytes = 4;
    else if ((format >= VK_FORMAT_R16G16B16A16_UNORM && format <= VK_FORMAT_R16G16B16A16_SFLOAT) ||
             (format >= VK_FORMAT_R32G32_UINT && format <= VK_FORMAT_R32G32_SFLOAT))
        bytes = 8;
    else if (format >= VK_FORMAT_R32G32B32_UINT && format <= VK_FORMAT_R32G32B32_SFLOAT)
        bytes = 12;
    else if (format >= VK_FORMAT_R32G32B32A32_UINT && format <= VK_FORMAT_R32G32B32A32_SFLOAT)
        bytes = 16;
    else if ((format >= VK_FORMAT_BC1_RGB_UNORM_BLOCK && format <= VK_FORMAT_BC1_RGBA_SRGB_BLOCK) ||
             format == VK_FORMAT_BC4_UNORM_BLOCK || format == VK_FORMAT_BC4_SNORM_BLOCK)
        block = 4, bytes = 8;
    else if (format >= VK_FORMAT_BC2_UNORM_BLOCK && format <= VK_FORMAT_BC7_SRGB_BLOCK)
        block = 4, bytes = 16;
    if (bytes == 0)
        return 0;
    *out_block_width = block;
    *out_block_height = block;
    *out_bytes_per_block = bytes;
    return 1;
}

static uint32_t format_aspect(uint32_t format)
{
    switch (format) {
    case VK_FORMAT_D16_UNORM:
    case VK_FORMAT_X8_D24_UNORM_PACK32:
    case VK_FORMAT_D32_SFLOAT:
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    case VK_FORMAT_S8_UINT:
        return VK_IMAGE_ASPECT_STENCIL_BIT;
    case VK_FORMAT_D16_UNORM_S8_UINT:
    case VK_FORMAT_D24_UNORM_S8_UINT:
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    default:
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

extern "C" s_vulkan_image *vulkan_image_create(s_vulkan_backend *backend, const s_vulkan_image_desc *desc)
{
    s_vulkan_functions &vk = backend->vk;
    s_vulkan_image *image = (s_vulkan_image *)calloc(1, sizeof(s_vulkan_image));
    if (image == 0)
        return 0;
    image->desc = *desc;
    if (image->desc.usage == 0)
        image->desc.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if (image->desc.depth == 0)
        image->desc.depth = 1;
    if (image->desc.mip_levels == 0)
        image->desc.mip_levels = 1;
    if (image->desc.array_layers == 0)
        image->desc.array_layers = desc->view_type == VK_IMAGE_VIEW_TYPE_CUBE ? 6u : 1u;
    image->aspect = format_aspect(desc->format);
    image->layout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkImageCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    create.flags = (desc->view_type == VK_IMAGE_VIEW_TYPE_CUBE || desc->view_type == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY)
                       ? (uint32_t)VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
                       : 0u;
    create.imageType = desc->view_type == VK_IMAGE_VIEW_TYPE_3D ? VK_IMAGE_TYPE_3D : VK_IMAGE_TYPE_2D;
    create.format = (VkFormat)desc->format;
    create.extent.width = desc->width;
    create.extent.height = desc->height;
    create.extent.depth = image->desc.depth;
    create.mipLevels = image->desc.mip_levels;
    create.arrayLayers = image->desc.array_layers;
    create.samples = VK_SAMPLE_COUNT_1_BIT;
    create.tiling = VK_IMAGE_TILING_OPTIMAL;
    create.usage = image->desc.usage;
    create.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    VkResult result = vk.vkCreateImage(backend->device, &create, 0, &image->image);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: vkCreateImage(%ux%u, format %u) failed: %s", desc->width, desc->height,
                   desc->format, vulkan_result_string(result));
        free(image);
        return 0;
    }
    VkMemoryRequirements requirements;
    vk.vkGetImageMemoryRequirements(backend->device, image->image, &requirements);
#ifdef HALO3_64BIT
    image->memory = allocate_texture_memory(backend, image, requirements);
#else
    uint32_t flags = 0;
    image->memory = allocate_memory(backend, requirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &flags);
#endif
    if (image->memory == VK_NULL_HANDLE) {
        vulkan_image_destroy(backend, image);
        return 0;
    }
    VkDeviceSize memory_offset = 0;
#ifdef HALO3_64BIT
    memory_offset = (VkDeviceSize)image->memory_page * 65536u;
#endif
    result = vk.vkBindImageMemory(backend->device, image->image, image->memory, memory_offset);
    if (result == VK_SUCCESS) {
        VkImageViewCreateInfo view = {};
        view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view.image = image->image;
        view.viewType = (VkImageViewType)desc->view_type;
        view.format = (VkFormat)desc->format;
        view.components.r = (VkComponentSwizzle)(desc->swizzle & 0xffu);
        view.components.g = (VkComponentSwizzle)((desc->swizzle >> 8) & 0xffu);
        view.components.b = (VkComponentSwizzle)((desc->swizzle >> 16) & 0xffu);
        view.components.a = (VkComponentSwizzle)(desc->swizzle >> 24);
        view.subresourceRange.aspectMask = image->aspect;
        view.subresourceRange.levelCount = image->desc.mip_levels;
        view.subresourceRange.layerCount = image->desc.array_layers;
        result = vk.vkCreateImageView(backend->device, &view, 0, &image->view);
        if (result == VK_SUCCESS && image->aspect != VK_IMAGE_ASPECT_COLOR_BIT &&
            (image->desc.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0 && (image->aspect & VK_IMAGE_ASPECT_DEPTH_BIT) != 0) {
            // shaders sample depth/stencil images by their depth (D3D's resolved depth textures); Xenos k_24_8 and
            // k_24_8_FLOAT replicate depth across all four channels (RRRR)
            view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
            view.components.r = view.components.g = view.components.b = view.components.a = VK_COMPONENT_SWIZZLE_R;
            result = vk.vkCreateImageView(backend->device, &view, 0, &image->sampled_view);
        }
    }
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: binding image memory or creating its view failed: %s", vulkan_result_string(result));
        vulkan_image_destroy(backend, image);
        return 0;
    }
    return image;
}

extern "C" void vulkan_image_destroy(s_vulkan_backend *backend, s_vulkan_image *image)
{
    if (image == 0)
        return;
    if ((image->desc.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0)
        backend->view_epoch++; // vulkan_image_retire
    image_destroy_now(backend, image);
}

static void image_destroy_now(s_vulkan_backend *backend, s_vulkan_image *image)
{
    if (image == 0)
        return;
    const VkImageView views[] = { image->view, image->sampled_view, image->array_view,
                                 image->stencil_view, image->swizzled_view };
    for (VkImageView view : views) {
        if (view != VK_NULL_HANDLE) {
            if (backend->bindless.set != VK_NULL_HANDLE)
                for (uint32_t binding = 0; binding < 4; ++binding)
                    bindless_forget(backend, binding, vulkan_gpu_detail::handle_cast<uint64_t>(view));
            backend->vk.vkDestroyImageView(backend->device, view, 0);
        }
    }
    backend->vk.vkDestroyImage(backend->device, image->image, 0);
#ifdef HALO3_64BIT
    release_texture_memory(backend, image);
#else
    free_memory(backend, image->memory);
#endif
    free(image);
}

extern "C" VkImage vulkan_image_handle(const s_vulkan_image *image) { return image->image; }
extern "C" VkImageView vulkan_image_view(const s_vulkan_image *image) { return image->view; }
extern "C" VkImageView vulkan_image_sampled_view(const s_vulkan_image *image)
{
    return image->sampled_view != VK_NULL_HANDLE ? image->sampled_view : image->view;
}
extern "C" VkImageView vulkan_image_array_view(s_vulkan_backend *backend, s_vulkan_image *image)
{
    if (image == 0)
        return VK_NULL_HANDLE;
    if (image->desc.view_type == VK_IMAGE_VIEW_TYPE_2D_ARRAY)
        return vulkan_image_sampled_view(image);
    if (image->desc.view_type != VK_IMAGE_VIEW_TYPE_2D)
        return VK_NULL_HANDLE;
    if (image->array_view != VK_NULL_HANDLE)
        return image->array_view;
    VkImageViewCreateInfo view = {};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.image = image->image;
    view.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    view.format = (VkFormat)image->desc.format;
    if (image->sampled_view != VK_NULL_HANDLE) {
        view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        view.components.r = view.components.g = view.components.b = view.components.a = VK_COMPONENT_SWIZZLE_R;
    } else {
        view.subresourceRange.aspectMask = image->aspect;
        view.components.r = (VkComponentSwizzle)(image->desc.swizzle & 0xffu);
        view.components.g = (VkComponentSwizzle)((image->desc.swizzle >> 8) & 0xffu);
        view.components.b = (VkComponentSwizzle)((image->desc.swizzle >> 16) & 0xffu);
        view.components.a = (VkComponentSwizzle)(image->desc.swizzle >> 24);
    }
    view.subresourceRange.levelCount = image->desc.mip_levels;
    view.subresourceRange.layerCount = image->desc.array_layers;
    if (backend->vk.vkCreateImageView(backend->device, &view, 0, &image->array_view) != VK_SUCCESS)
        image->array_view = VK_NULL_HANDLE;
    return image->array_view;
}

extern "C" VkImageView vulkan_image_swizzled_view(s_vulkan_backend *backend, s_vulkan_image *image, uint32_t swizzle)
{
    if (image == 0 || image->aspect != VK_IMAGE_ASPECT_COLOR_BIT)
        return VK_NULL_HANDLE;
    if (swizzle == image->desc.swizzle)
        return image->view;
    if (image->swizzled_view != VK_NULL_HANDLE) {
        if (image->swizzled_view_swizzle == swizzle)
            return image->swizzled_view;
        // one other swizzle for the image's life (draws recorded with a view may still use it)
        vulkan_log(backend, "vulkan: vulkan_image_swizzled_view: a second swizzle %08x of an image viewed as %08x",
                   swizzle, image->swizzled_view_swizzle);
        return VK_NULL_HANDLE;
    }
    VkImageViewCreateInfo view = {};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.image = image->image;
    view.viewType = (VkImageViewType)image->desc.view_type;
    view.format = (VkFormat)image->desc.format;
    view.components.r = (VkComponentSwizzle)(swizzle & 0xffu);
    view.components.g = (VkComponentSwizzle)((swizzle >> 8) & 0xffu);
    view.components.b = (VkComponentSwizzle)((swizzle >> 16) & 0xffu);
    view.components.a = (VkComponentSwizzle)(swizzle >> 24);
    view.subresourceRange.aspectMask = image->aspect;
    view.subresourceRange.levelCount = image->desc.mip_levels;
    view.subresourceRange.layerCount = image->desc.array_layers;
    if (backend->vk.vkCreateImageView(backend->device, &view, 0, &image->swizzled_view) != VK_SUCCESS)
        image->swizzled_view = VK_NULL_HANDLE;
    image->swizzled_view_swizzle = swizzle;
    return image->swizzled_view;
}

// The stencil aspect of a sampled depth/stencil image as its own view (a sampled view names one aspect), made on first
// use; VK_NULL_HANDLE for an image without stencil or SAMPLED usage.
static VkImageView image_stencil_view(s_vulkan_backend *backend, s_vulkan_image *image)
{
    if ((image->aspect & VK_IMAGE_ASPECT_STENCIL_BIT) == 0 || (image->desc.usage & VK_IMAGE_USAGE_SAMPLED_BIT) == 0)
        return VK_NULL_HANDLE;
    if (image->stencil_view != VK_NULL_HANDLE)
        return image->stencil_view;
    VkImageViewCreateInfo view = {};
    view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view.image = image->image;
    view.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view.format = (VkFormat)image->desc.format;
    view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
    view.subresourceRange.levelCount = 1;
    view.subresourceRange.layerCount = 1;
    if (backend->vk.vkCreateImageView(backend->device, &view, 0, &image->stencil_view) != VK_SUCCESS)
        image->stencil_view = VK_NULL_HANDLE;
    return image->stencil_view;
}

extern "C" VkResult vulkan_image_upload(s_vulkan_backend *backend, s_vulkan_image *image,
                                        const s_vulkan_image_region *regions, uint32_t region_count, const void *data,
                                        uint32_t size)
{
    uint32_t block_width, block_height, block_bytes;
    if (!vulkan_format_block_info(image->desc.format, &block_width, &block_height, &block_bytes)) {
        vulkan_log(backend, "vulkan: image upload: unknown block size of format %u", image->desc.format);
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }
    [[clang::uninitialized]] VkBufferImageCopy local_copies[16];
    [[clang::uninitialized]] uint32_t local_ends[16];
    VkBufferImageCopy *copies = region_count <= 16u
                                    ? local_copies
                                    : (VkBufferImageCopy *)calloc(region_count + 1, sizeof(VkBufferImageCopy));
    uint32_t *ends = region_count <= 16u ? local_ends : (uint32_t *)calloc(region_count + 1, sizeof(uint32_t));
    if (copies == 0 || ends == 0) {
        if (copies != local_copies)
            free(copies);
        if (ends != local_ends)
            free(ends);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    if (copies == local_copies)
        memset(local_copies, 0, region_count * sizeof(VkBufferImageCopy));
    // Staging offsets are multiples of 4 and of the block size (bufferOffset rules); a region the data has elsewhere
    // (the 1-byte tail levels of an 8-bit texture) is moved to the next such offset.
    uint32_t alignment = block_bytes;
    while (alignment % 4u != 0)
        alignment += block_bytes;
    bool repack = false;
    for (uint32_t i = 0; i < region_count; i++) {
        const s_vulkan_image_region &region = regions[i];
        uint32_t row_pitch = region.row_pitch != 0 ? region.row_pitch
                                                   : (region.width + block_width - 1) / block_width * block_bytes;
        uint32_t rows = (region.height + block_height - 1) / block_height;
        uint32_t slice_pitch = region.slice_pitch != 0 ? region.slice_pitch : row_pitch * rows;
        uint32_t layers = region.layer_count != 0 ? region.layer_count : 1u;
        uint32_t depth = region.depth != 0 ? region.depth : 1u;
        uint32_t row_bytes = (region.width + block_width - 1) / block_width * block_bytes;
        uint64_t end = (uint64_t)region.offset + (uint64_t)slice_pitch * (depth * layers - 1) +
                       (uint64_t)row_pitch * (rows - 1) + row_bytes;
        if (end > size || row_pitch % block_bytes != 0 || slice_pitch % row_pitch != 0 ||
            region.offset % block_bytes != 0) {
            vulkan_log(backend, "vulkan: image upload region %u (level %u, %ux%u at offset %u) is malformed or "
                                "outside the %u bytes of data", i, region.mip_level, region.width, region.height,
                       region.offset, size);
            if (copies != local_copies)
                free(copies);
            if (ends != local_ends)
                free(ends);
            return VK_ERROR_UNKNOWN;
        }
        ends[i] = (uint32_t)end;
        if (region.offset % alignment != 0)
            repack = true;
        // the region must lie inside its level and layers of the image (a copy outside them faults the device)
        const uint32_t level_width = image->desc.width >> region.mip_level != 0 ? image->desc.width >> region.mip_level : 1u;
        const uint32_t level_height =
            image->desc.height >> region.mip_level != 0 ? image->desc.height >> region.mip_level : 1u;
        const uint32_t image_depth = image->desc.depth != 0 ? image->desc.depth : 1u;
        const uint32_t level_depth = image_depth >> region.mip_level != 0 ? image_depth >> region.mip_level : 1u;
        const uint32_t image_layers = image->desc.array_layers != 0 ? image->desc.array_layers : 1u;
        if (region.mip_level >= (image->desc.mip_levels != 0 ? image->desc.mip_levels : 1u) || region.width == 0 ||
            region.height == 0 || region.width > level_width || region.height > level_height || depth > level_depth ||
            region.base_layer + layers > image_layers) {
            vulkan_log(backend, "vulkan: image upload region %u (level %u, %ux%ux%u, layers %u+%u) is outside the "
                                "%ux%ux%u image of %u levels and %u layers", i, region.mip_level, region.width,
                       region.height, depth, region.base_layer, layers, image->desc.width, image->desc.height,
                       image_depth, image->desc.mip_levels, image_layers);
            if (copies != local_copies)
                free(copies);
            if (ends != local_ends)
                free(ends);
            return VK_ERROR_UNKNOWN;
        }
        VkBufferImageCopy &copy = copies[i];
        copy.bufferOffset = region.offset;
        copy.bufferRowLength = row_pitch / block_bytes * block_width;
        copy.bufferImageHeight = slice_pitch / row_pitch * block_height;
        copy.imageSubresource.aspectMask = image->aspect;
        copy.imageSubresource.mipLevel = region.mip_level;
        copy.imageSubresource.baseArrayLayer = region.base_layer;
        copy.imageSubresource.layerCount = layers;
        copy.imageExtent.width = region.width;
        copy.imageExtent.height = region.height;
        copy.imageExtent.depth = depth;
    }
    uint32_t staging_size = size;
    if (repack) {
        staging_size = 0;
        for (uint32_t i = 0; i < region_count; i++)
            staging_size = (staging_size + alignment - 1u) / alignment * alignment + (ends[i] - regions[i].offset);
    }
    // an image no command has used yet (still UNDEFINED) may be filled ahead of the frame (upload_begin), its levels
    // staged in the frame's staging ring when they fit
    VkCommandBuffer deferred =
        image->layout == VK_IMAGE_LAYOUT_UNDEFINED ? upload_begin(backend, "vulkan_image_upload") : 0;
    uint32_t staging_base = 0;
    s_vulkan_buffer *ring = deferred != 0 ? staging_ring_alloc(backend, staging_size, alignment, &staging_base) : 0;
    s_vulkan_buffer *staging = ring != 0 ? ring
                                         : vulkan_buffer_create(backend, staging_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                                k_vulkan_memory_upload);
    if (staging == 0) {
        if (copies != local_copies)
            free(copies);
        if (ends != local_ends)
            free(ends);
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    }
    uint8_t *mapped = (uint8_t *)staging->mapped + staging_base;
    if (repack) {
        uint32_t offset = 0;
        for (uint32_t i = 0; i < region_count; i++) {
            offset = (offset + alignment - 1u) / alignment * alignment;
            const uint32_t bytes = ends[i] - regions[i].offset;
            memcpy(mapped + offset, (const uint8_t *)data + regions[i].offset, bytes);
            copies[i].bufferOffset = staging_base + offset;
            offset += bytes;
        }
    } else {
        memcpy(mapped, data, size);
        for (uint32_t i = 0; i < region_count; i++)
            copies[i].bufferOffset += staging_base;
    }
    if (ends != local_ends)
        free(ends);
    VkCommandBuffer command_buffer = deferred != 0 ? deferred : immediate_begin(backend, "vulkan_image_upload");
    cmd_transition(backend, command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    backend->vk.vkCmdCopyBufferToImage(command_buffer, staging->buffer, image->image,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, region_count, copies);
    cmd_transition(backend, command_buffer, image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    if (copies != local_copies)
        free(copies);
    if (deferred != 0) {
        if (ring == 0)
            retire(backend, staging, 0);
        return VK_SUCCESS;
    }
    VkResult result = immediate_submit(backend);
    if (result == VK_SUCCESS)
        buffer_release(backend, staging);
    else
        vulkan_buffer_destroy(backend, staging);
    return result;
}

extern "C" VkResult vulkan_image_upload_2d(s_vulkan_backend *backend, s_vulkan_image *image, uint32_t mip_level,
                                           uint32_t layer, const void *data, uint32_t row_pitch)
{
    uint32_t block_width, block_height, block_bytes;
    if (!vulkan_format_block_info(image->desc.format, &block_width, &block_height, &block_bytes))
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    s_vulkan_image_region region = {};
    region.mip_level = mip_level;
    region.base_layer = layer;
    region.layer_count = 1;
    region.width = image->desc.width >> mip_level != 0 ? image->desc.width >> mip_level : 1u;
    region.height = image->desc.height >> mip_level != 0 ? image->desc.height >> mip_level : 1u;
    region.depth = 1;
    region.row_pitch = row_pitch != 0 ? row_pitch : (region.width + block_width - 1) / block_width * block_bytes;
    uint32_t size = region.row_pitch * ((region.height + block_height - 1) / block_height);
    return vulkan_image_upload(backend, image, &region, 1, data, size);
}

// ---- samplers ----

extern "C" VkSampler vulkan_sampler_create(s_vulkan_backend *backend, const s_vulkan_sampler_desc *desc)
{
    VkSamplerCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create.magFilter = (VkFilter)desc->mag_filter;
    create.minFilter = (VkFilter)desc->min_filter;
    create.mipmapMode = (VkSamplerMipmapMode)desc->mipmap_mode;
    create.addressModeU = (VkSamplerAddressMode)desc->address_u;
    create.addressModeV = (VkSamplerAddressMode)desc->address_v;
    create.addressModeW = (VkSamplerAddressMode)desc->address_w;
    create.mipLodBias = desc->mip_lod_bias;
    if (desc->max_anisotropy > 1.0f && backend->sampler_anisotropy) {
        float limit = backend->properties.limits.maxSamplerAnisotropy;
        create.anisotropyEnable = VK_TRUE;
        create.maxAnisotropy = desc->max_anisotropy < limit ? desc->max_anisotropy : limit;
    } else {
        create.maxAnisotropy = 1.0f;
    }
    create.compareEnable = desc->compare_enable ? VK_TRUE : VK_FALSE;
    create.compareOp = (VkCompareOp)desc->compare_op;
    create.minLod = desc->min_lod;
    create.maxLod = desc->max_lod;
    create.borderColor = (VkBorderColor)desc->border_color;
    VkSampler sampler = VK_NULL_HANDLE;
    VkResult result = backend->vk.vkCreateSampler(backend->device, &create, 0, &sampler);
    if (result != VK_SUCCESS)
        vulkan_log(backend, "vulkan: vkCreateSampler failed: %s", vulkan_result_string(result));
    return sampler;
}

extern "C" void vulkan_sampler_destroy(s_vulkan_backend *backend, VkSampler sampler)
{
    backend->view_epoch++; // vulkan_image_retire
    bindless_forget(backend, 4, vulkan_gpu_detail::handle_cast<uint64_t>(sampler));
    backend->vk.vkDestroySampler(backend->device, sampler, 0);
}

// ---- shader modules ----

extern "C" VkShaderModule vulkan_shader_module_create(s_vulkan_backend *backend, const uint32_t *code, size_t size_bytes)
{
    if (code == 0 || size_bytes < 20 || size_bytes % 4 != 0 || code[0] != 0x07230203u) {
        vulkan_log(backend, "vulkan: shader module: not SPIR-V (%u bytes)", (uint32_t)size_bytes);
        return VK_NULL_HANDLE;
    }
    VkShaderModuleCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create.codeSize = size_bytes;
    create.pCode = code;
    VkShaderModule module = VK_NULL_HANDLE;
    VkResult result = backend->vk.vkCreateShaderModule(backend->device, &create, 0, &module);
    if (result != VK_SUCCESS)
        vulkan_log(backend, "vulkan: vkCreateShaderModule failed: %s", vulkan_result_string(result));
    return module;
}

extern "C" void vulkan_shader_module_destroy(s_vulkan_backend *backend, VkShaderModule module)
{
    backend->vk.vkDestroyShaderModule(backend->device, module, 0);
}

extern "C" const char *vulkan_spirv_entry_point(const uint32_t *code, size_t size_bytes, uint32_t stage)
{
    // SPIR-V execution models: Vertex 0, Fragment 4, GLCompute 5 (e_shader_pack_stage order: vertex, pixel, compute).
    static const uint32_t k_execution_models[3] = { 0, 4, 5 };
    const uint32_t k_op_entry_point = 15;
    if (code == 0 || size_bytes < 20 || code[0] != 0x07230203u || stage > 2)
        return 0;
    size_t word_count = size_bytes / 4;
    for (size_t i = 5; i < word_count;) {
        uint32_t length = code[i] >> 16, opcode = code[i] & 0xffffu;
        if (length == 0 || i + length > word_count)
            return 0;
        // OpEntryPoint: execution model, entry point <id>, name (NUL-terminated, padded to words), interface.
        if (opcode == k_op_entry_point && length >= 4 && code[i + 1] == k_execution_models[stage]) {
            const char *name = (const char *)&code[i + 3];
            size_t limit = (length - 3) * 4;
            for (size_t c = 0; c < limit; c++)
                if (name[c] == 0)
                    return name;
            return 0;
        }
        i += length;
    }
    return 0;
}

// The color output locations a fragment shader writes: bit L for each Output variable at Location L (an array of n
// outputs at L takes L .. L + n - 1). Built-ins (no Location: FragDepth) do not count. 0 for anything else.
extern "C" uint32_t vulkan_spirv_output_mask(const uint32_t *code, size_t size_bytes)
{
    enum {
        op_decorate = 71, op_type_array = 28, op_type_pointer = 32, op_constant = 43, op_variable = 59,
        decoration_location = 30, storage_output = 3,
    };
    if (code == 0 || size_bytes < 20 || (size_bytes & 3) != 0 || code[0] != 0x07230203u)
        return 0;
    const size_t word_count = size_bytes / 4;
    const uint32_t bound = code[3];
    if (bound == 0 || bound > (1u << 22))
        return 0;
    // per id: its Location (+1), the pointee of a pointer type, an array type's length constant, a constant's value
    uint32_t *location = (uint32_t *)calloc(bound, sizeof(uint32_t));
    uint32_t *pointee = (uint32_t *)calloc(bound, sizeof(uint32_t));
    uint32_t *array_length = (uint32_t *)calloc(bound, sizeof(uint32_t));
    uint32_t *constant = (uint32_t *)calloc(bound, sizeof(uint32_t));
    uint32_t mask = 0;
    if (location != 0 && pointee != 0 && array_length != 0 && constant != 0) {
        // two passes: declarations first, then the Output variables (decorations precede the types they name)
        for (int pass = 0; pass < 2; pass++) {
            for (size_t i = 5; i < word_count;) {
                const uint32_t length = code[i] >> 16, opcode = code[i] & 0xffffu;
                if (length == 0 || i + length > word_count)
                    break;
                const uint32_t *w = code + i;
                if (pass == 0) {
                    if (opcode == op_decorate && length >= 4 && w[2] == decoration_location && w[1] < bound)
                        location[w[1]] = w[3] + 1u;
                    else if (opcode == op_type_pointer && length >= 4 && w[1] < bound)
                        pointee[w[1]] = w[3];
                    else if (opcode == op_type_array && length >= 4 && w[1] < bound)
                        array_length[w[1]] = w[3];
                    else if (opcode == op_constant && length >= 4 && w[2] < bound)
                        constant[w[2]] = w[3];
                } else if (opcode == op_variable && length >= 4 && w[3] == storage_output && w[2] < bound &&
                           w[1] < bound && location[w[2]] != 0) {
                    const uint32_t type = pointee[w[1]] < bound ? pointee[w[1]] : 0;
                    uint32_t count = 1;
                    if (type != 0 && array_length[type] != 0 && array_length[type] < bound)
                        count = constant[array_length[type]];
                    for (uint32_t k = 0; k < count && location[w[2]] - 1u + k < 32u; k++)
                        mask |= 1u << (location[w[2]] - 1u + k);
                }
                i += length;
            }
        }
    }
    free(location);
    free(pointee);
    free(array_length);
    free(constant);
    return mask;
}

// The fragment shader specializations graphics pipelines set (see VULKAN_SPEC_COLOR_EXP_SCALE and
// VULKAN_SPEC_FRAGCOORD_OFFSET), added to a pack fragment shader:
//   - every store of a float color output at location L (0..3; an array of outputs takes L, L + 1, ..) is scaled by a
//     float specialization constant, SpecId VULKAN_SPEC_COLOR_EXP_SCALE + L, default 1. As the Xenos render backend
//     does, the whole color (alpha included) is scaled before blending;
//   - every load of the fragment coordinate (BuiltIn FragCoord, whole or its x / y through an access chain) has the
//     float specialization constants VULKAN_SPEC_FRAGCOORD_OFFSET + 0 / + 1 (default 0) added to x / y.
// Returns the new code (malloc'd, *out_size bytes), or NULL when the module is not a fragment shader, has nothing to
// patch, already has those constants, or is malformed (it is then used as it is).
extern "C" uint32_t *vulkan_spirv_patch_fragment(const uint32_t *code, size_t size_bytes, size_t *out_size)
{
    enum {
        op_decorate = 71, op_type_float = 22, op_type_vector = 23, op_type_array = 28, op_type_pointer = 32,
        op_constant = 43, op_variable = 59, op_load = 61, op_access_chain = 65, op_in_bounds_access_chain = 66,
        op_store = 62, op_spec_constant = 50, op_spec_constant_composite = 51, op_composite_construct = 80,
        op_composite_extract = 81, op_vector_times_scalar = 142, op_fadd = 129, op_fmul = 133, op_entry_point = 15,
        decoration_spec_id = 1, decoration_builtin = 11, decoration_location = 30, builtin_frag_coord = 15,
        storage_input = 1, storage_output = 3, model_fragment = 4, k_unresolved = 0x100u,
    };
    if (code == 0 || size_bytes < 20 || (size_bytes & 3) != 0 || code[0] != 0x07230203u)
        return 0;
    const size_t word_count = size_bytes / 4;
    const uint32_t bound = code[3];
    if (bound == 0 || bound > (1u << 22))
        return 0;
    // Per id. location: 1 + the location of an output variable or of a pointer into one (k_unresolved | location while
    // only decorated). components: 1 for the float type, n for a float vector, 0x10000 * length + element type count
    // for an array of them (element in element[]). pointee: of an Output pointer type. store_type: of what a store
    // through an output variable or chain writes. constant: an OpConstant's first word. frag_coord: 1 + the component
    // a pointer reaches (4: the whole FragCoord vector; 0x100 while only decorated).
    uint32_t *location = (uint32_t *)calloc(bound, sizeof(uint32_t));
    uint32_t *components = (uint32_t *)calloc(bound, sizeof(uint32_t));
    uint32_t *element = (uint32_t *)calloc(bound, sizeof(uint32_t));
    uint32_t *pointee = (uint32_t *)calloc(bound, sizeof(uint32_t));
    uint32_t *store_type = (uint32_t *)calloc(bound, sizeof(uint32_t));
    uint32_t *constant = (uint32_t *)calloc(bound, sizeof(uint32_t));
    uint32_t *frag_coord = (uint32_t *)calloc(bound, sizeof(uint32_t));
    uint8_t *known = (uint8_t *)calloc(bound, 1); // ids of OpConstants
    uint32_t *result = 0;
    bool fragment = false;
    bool ok = location != 0 && components != 0 && element != 0 && pointee != 0 && store_type != 0 && constant != 0 &&
              frag_coord != 0 && known != 0;
    uint32_t float_type = 0, float_at = 0, vec4_type = 0, vec4_at = 0, last_annotation = 0, stores = 0, loads = 0;
    uint32_t used = 0;
    for (size_t i = 5; ok && i < word_count;) {
        const uint32_t length = code[i] >> 16, opcode = code[i] & 0xffffu;
        if (length == 0 || i + length > word_count) {
            ok = false;
            break;
        }
        const uint32_t *w = &code[i];
        const uint32_t id1 = length >= 2 && w[1] < bound ? w[1] : 0, id2 = length >= 3 && w[2] < bound ? w[2] : 0;
        switch (opcode) {
        case op_entry_point:
            fragment |= length >= 3 && w[1] == model_fragment;
            break;
        case op_decorate:
            last_annotation = (uint32_t)(i + length);
            if (length >= 4 && w[2] == decoration_location && id1 != 0 && w[3] < 4)
                location[id1] = k_unresolved | w[3];
            if (length >= 4 && w[2] == decoration_builtin && w[3] == builtin_frag_coord && id1 != 0)
                frag_coord[id1] = k_unresolved;
            if (length >= 4 && w[2] == decoration_spec_id && w[3] >= VULKAN_SPEC_COLOR_EXP_SCALE &&
                w[3] < VULKAN_SPEC_FRAGCOORD_OFFSET + 2u)
                ok = false; // already patched
            break;
        case 72: case 73: case 74: case 75: case 332: case 5632: case 5633: // the other annotations
            last_annotation = (uint32_t)(i + length);
            break;
        case op_type_float:
            if (length >= 3 && w[2] == 32 && id1 != 0 && float_type == 0) {
                float_type = id1;
                float_at = (uint32_t)(i + length);
                components[id1] = 1;
            }
            break;
        case op_type_vector:
            if (length >= 4 && id1 != 0 && float_type != 0 && w[2] == float_type) {
                components[id1] = w[3];
                if (w[3] == 4 && vec4_type == 0) {
                    vec4_type = id1;
                    vec4_at = (uint32_t)(i + length);
                }
            }
            break;
        case op_constant:
            if (length >= 4 && id2 != 0) {
                constant[id2] = w[3];
                known[id2] = 1;
            }
            break;
        case op_type_array:
            if (length >= 4 && id1 != 0 && w[2] < bound && w[3] < bound && components[w[2]] != 0 &&
                components[w[2]] < 0x10000u && constant[w[3]] != 0 && constant[w[3]] <= 4) {
                components[id1] = constant[w[3]] << 16 | components[w[2]];
                element[id1] = w[2];
            }
            break;
        case op_type_pointer:
            if (length >= 4 && id1 != 0 && w[2] == storage_output && w[3] < bound)
                pointee[id1] = w[3];
            break;
        case op_variable:
            if (id2 == 0)
                break;
            if (length >= 4 && w[3] == storage_output && id1 != 0 && pointee[id1] != 0 &&
                (location[id2] & k_unresolved) != 0 && components[pointee[id1]] != 0) {
                location[id2] = 1u + (location[id2] & 3u);
                store_type[id2] = pointee[id1];
            } else {
                location[id2] = 0;
            }
            frag_coord[id2] = length >= 4 && w[3] == storage_input && frag_coord[id2] == k_unresolved ? 5u : 0u;
            break;
        case op_access_chain:
        case op_in_bounds_access_chain:
            if (length >= 4 && id2 != 0 && w[3] < bound && frag_coord[w[3]] == 5u) {
                // a component of FragCoord: x or y gets the offset
                if (length >= 5 && w[4] < bound && known[w[4]] && constant[w[4]] < 2u)
                    frag_coord[id2] = 1u + constant[w[4]];
                break;
            }
            // into an output: an element of an array of outputs (its location advances by the constant index), or
            // a component of one; the chain's own pointer type names what is stored
            if (length >= 4 && id1 != 0 && id2 != 0 && w[3] < bound && location[w[3]] != 0 &&
                (location[w[3]] & k_unresolved) == 0 && pointee[id1] != 0 && components[pointee[id1]] != 0) {
                uint32_t at = location[w[3]];
                if (components[store_type[w[3]]] >= 0x10000u) {
                    if (length < 5 || w[4] >= bound || at + constant[w[4]] > 4u) {
                        ok = false; // a dynamic or out-of-range index into an array of outputs
                        break;
                    }
                    at += constant[w[4]];
                }
                location[id2] = at;
                store_type[id2] = pointee[id1];
            }
            break;
        case op_load:
            if (length >= 4 && w[3] < bound && frag_coord[w[3]] != 0 && frag_coord[w[3]] != k_unresolved)
                loads++;
            break;
        case op_store:
            if (length >= 3 && id1 != 0 && location[id1] != 0 && (location[id1] & k_unresolved) == 0) {
                stores++;
                const uint32_t count = components[store_type[id1]] >= 0x10000u ? components[store_type[id1]] >> 16 : 1u;
                for (uint32_t k = 0; k < count && location[id1] - 1u + k < 4u; k++)
                    used |= 1u << (location[id1] - 1u + k);
            }
            break;
        default:
            break;
        }
        i += length;
    }
    if (loads != 0 && vec4_type == 0)
        loads = 0; // no vec4 type to build the offset with (cannot happen for a FragCoord load)
    if (ok && fragment && (stores != 0 || loads != 0) && float_type != 0 && last_annotation != 0 && float_at != 0) {
        const uint32_t added_words = 6u * 4u /* OpDecorate */ + 7u * 4u /* constants */ + 7u /* composite */ +
                                     stores * 48u + loads * 5u;
        result = (uint32_t *)malloc((word_count + added_words) * 4u);
        uint32_t spec[4] = {}, offset[2] = {}, zero = 0, offset_vec = 0;
        uint32_t next_id = bound;
        for (uint32_t l = 0; l < 4; l++)
            if ((used & (1u << l)) != 0)
                spec[l] = next_id++;
        if (loads != 0) {
            offset[0] = next_id++;
            offset[1] = next_id++;
            zero = next_id++;
            offset_vec = next_id++;
        }
        size_t o = 0;
        for (size_t i = 0; result != 0 && i < word_count;) {
            const uint32_t length = i < 5 ? 1u : code[i] >> 16, opcode = i < 5 ? 0u : code[i] & 0xffffu;
            const uint32_t *w = &code[i];
            uint32_t value = 0;
            if (i >= 5 && opcode == op_load && w[3] < bound && frag_coord[w[3]] != 0 &&
                frag_coord[w[3]] != k_unresolved && loads != 0) {
                // load into a new id, then the original id = loaded + offset
                const uint32_t loaded = next_id++, which = frag_coord[w[3]];
                memcpy(&result[o], w, length * 4u);
                result[o + 2] = loaded;
                o += length;
                result[o++] = 5u << 16 | op_fadd;
                result[o++] = w[1];
                result[o++] = w[2];
                result[o++] = loaded;
                result[o++] = which == 5u ? offset_vec : offset[which - 1u];
                i += length;
                continue;
            }
            if (i >= 5 && opcode == op_store && w[1] < bound && location[w[1]] != 0 &&
                (location[w[1]] & k_unresolved) == 0) {
                const uint32_t type = store_type[w[1]], first = location[w[1]] - 1u;
                if (components[type] >= 0x10000u) {
                    // an array of outputs: extract, scale and rebuild each element
                    const uint32_t count = components[type] >> 16, element_type = element[type];
                    uint32_t scaled[4];
                    for (uint32_t k = 0; k < count; k++) {
                        const uint32_t extracted = next_id++;
                        result[o++] = 5u << 16 | op_composite_extract;
                        result[o++] = element_type;
                        result[o++] = extracted;
                        result[o++] = w[2];
                        result[o++] = k;
                        scaled[k] = next_id++;
                        result[o++] = 5u << 16 | (components[element_type] == 1 ? (uint32_t)op_fmul
                                                                                 : (uint32_t)op_vector_times_scalar);
                        result[o++] = element_type;
                        result[o++] = scaled[k];
                        result[o++] = extracted;
                        result[o++] = spec[first + k < 4u ? first + k : 3u];
                    }
                    value = next_id++;
                    result[o++] = (3u + count) << 16 | op_composite_construct;
                    result[o++] = type;
                    result[o++] = value;
                    for (uint32_t k = 0; k < count; k++)
                        result[o++] = scaled[k];
                } else {
                    value = next_id++;
                    result[o++] = 5u << 16 | (components[type] == 1 ? (uint32_t)op_fmul : (uint32_t)op_vector_times_scalar);
                    result[o++] = type;
                    result[o++] = value;
                    result[o++] = w[2];
                    result[o++] = spec[first];
                }
            }
            memcpy(&result[o], w, length * 4u);
            if (value != 0)
                result[o + 2] = value;
            o += length;
            i += length;
            if (i == last_annotation) {
                for (uint32_t l = 0; l < 4; l++) {
                    if (spec[l] == 0)
                        continue;
                    result[o++] = 4u << 16 | op_decorate;
                    result[o++] = spec[l];
                    result[o++] = decoration_spec_id;
                    result[o++] = VULKAN_SPEC_COLOR_EXP_SCALE + l;
                }
                for (uint32_t c = 0; c < 2 && loads != 0; c++) {
                    result[o++] = 4u << 16 | op_decorate;
                    result[o++] = offset[c];
                    result[o++] = decoration_spec_id;
                    result[o++] = VULKAN_SPEC_FRAGCOORD_OFFSET + c;
                }
            }
            if (i == float_at) {
                for (uint32_t l = 0; l < 4; l++) {
                    if (spec[l] == 0)
                        continue;
                    result[o++] = 4u << 16 | op_spec_constant;
                    result[o++] = float_type;
                    result[o++] = spec[l];
                    result[o++] = 0x3f800000u; // 1.0
                }
                for (uint32_t c = 0; c < 2 && loads != 0; c++) {
                    result[o++] = 4u << 16 | op_spec_constant;
                    result[o++] = float_type;
                    result[o++] = offset[c];
                    result[o++] = 0; // 0.0
                }
                if (loads != 0) {
                    result[o++] = 4u << 16 | op_constant;
                    result[o++] = float_type;
                    result[o++] = zero;
                    result[o++] = 0;
                }
            }
            if (i == vec4_at && loads != 0) {
                result[o++] = 7u << 16 | op_spec_constant_composite;
                result[o++] = vec4_type;
                result[o++] = offset_vec;
                result[o++] = offset[0];
                result[o++] = offset[1];
                result[o++] = zero;
                result[o++] = zero;
            }
        }
        if (result != 0) {
            result[3] = next_id;
            *out_size = o * 4u;
        }
    }
    free(location);
    free(components);
    free(element);
    free(pointee);
    free(store_type);
    free(constant);
    free(frag_coord);
    free(known);
    return result;
}

// The shader pack's module cache is filled by vulkan_pack.cpp; releasing it needs no pack, so it lives here, where
// vulkan_backend_destroy uses it.
extern "C" void vulkan_pack_release(s_vulkan_backend *backend)
{
    for (uint32_t i = 0; i < backend->pack_module_count; i++)
        if (backend->pack_modules[i] != VK_NULL_HANDLE)
            backend->vk.vkDestroyShaderModule(backend->device, backend->pack_modules[i], 0);
    free(backend->pack_modules);
    free(backend->pack_entry_points);
    free(backend->pack_output_masks);
    backend->pack_modules = 0;
    backend->pack_entry_points = 0;
    backend->pack_output_masks = 0;
    backend->pack_module_count = 0;
    backend->pack = 0;
}

// ---- layout caches ----

// Finds the entry with this hash that `matches`; NULL if none.
template <typename T, typename F>
static T *cache_find(T *entries, const s_vulkan_cache_index &index, uint32_t hash, F matches)
{
    if (index.slot_count == 0)
        return 0;
    for (uint32_t slot = hash & (index.slot_count - 1); index.slots[slot] != 0; slot = (slot + 1) & (index.slot_count - 1)) {
        T &entry = entries[index.slots[slot] - 1];
        if (entry.hash == hash && matches(entry))
            return &entry;
    }
    return 0;
}

// Appends an entry and indexes it (growing the array, and the index to keep it at most half full).
template <typename T>
static T *cache_insert(T **entries, uint32_t *count, uint32_t *capacity, s_vulkan_cache_index *index, const T &entry)
{
    if (*count == *capacity) {
        uint32_t grown_capacity = *capacity != 0 ? *capacity * 2 : 16;
        T *grown = (T *)realloc(*entries, grown_capacity * sizeof(T));
        if (grown == 0)
            return 0;
        *entries = grown;
        *capacity = grown_capacity;
    }
    if ((*count + 1) * 2 > index->slot_count) {
        uint32_t slot_count = index->slot_count != 0 ? index->slot_count * 2 : 32;
        uint32_t *slots = (uint32_t *)calloc(slot_count, sizeof(uint32_t));
        if (slots == 0)
            return 0;
        for (uint32_t i = 0; i < *count; i++) {
            uint32_t slot = (*entries)[i].hash & (slot_count - 1);
            while (slots[slot] != 0)
                slot = (slot + 1) & (slot_count - 1);
            slots[slot] = i + 1;
        }
        free(index->slots);
        index->slots = slots;
        index->slot_count = slot_count;
    }
    uint32_t slot = entry.hash & (index->slot_count - 1);
    while (index->slots[slot] != 0)
        slot = (slot + 1) & (index->slot_count - 1);
    (*entries)[*count] = entry;
    index->slots[slot] = ++*count;
    return &(*entries)[*count - 1];
}

extern "C" VkDescriptorSetLayout vulkan_descriptor_set_layout_get(s_vulkan_backend *backend,
                                                                  const s_vulkan_layout_binding *bindings,
                                                                  uint32_t count)
{
    // Canonical form: sorted by binding number.
    s_vulkan_layout_binding *sorted = (s_vulkan_layout_binding *)calloc(count + 1, sizeof(s_vulkan_layout_binding));
    if (sorted == 0)
        return VK_NULL_HANDLE;
    for (uint32_t i = 0; i < count; i++) {
        uint32_t j = i;
        while (j > 0 && sorted[j - 1].binding > bindings[i].binding) {
            sorted[j] = sorted[j - 1];
            j--;
        }
        sorted[j] = bindings[i];
    }
    uint32_t hash = vulkan_hash_bytes(2166136261u, sorted, count * sizeof(s_vulkan_layout_binding));
    const s_vulkan_set_layout_entry *found =
        cache_find(backend->set_layouts, backend->set_layout_index, hash, [&](const s_vulkan_set_layout_entry &entry) {
            return entry.binding_count == count &&
                   memcmp(entry.bindings, sorted, count * sizeof(s_vulkan_layout_binding)) == 0;
        });
    if (found != 0) {
        free(sorted);
        return found->layout;
    }

    VkDescriptorSetLayoutBinding *vk_bindings =
        (VkDescriptorSetLayoutBinding *)calloc(count + 1, sizeof(VkDescriptorSetLayoutBinding));
    if (vk_bindings == 0) {
        free(sorted);
        return VK_NULL_HANDLE;
    }
    for (uint32_t i = 0; i < count; i++) {
        if (i > 0 && sorted[i].binding == sorted[i - 1].binding) {
            vulkan_log(backend, "vulkan: set layout: binding %u appears twice", sorted[i].binding);
            free(vk_bindings);
            free(sorted);
            return VK_NULL_HANDLE;
        }
        vk_bindings[i].binding = sorted[i].binding;
        vk_bindings[i].descriptorType = (VkDescriptorType)sorted[i].descriptor_type;
        vk_bindings[i].descriptorCount = sorted[i].count;
        vk_bindings[i].stageFlags = sorted[i].stage_flags;
    }
    VkDescriptorSetLayoutCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    create.bindingCount = count;
    create.pBindings = vk_bindings;
    s_vulkan_set_layout_entry entry = {};
    entry.hash = hash;
    entry.binding_count = count;
    entry.bindings = sorted;
    VkResult result = backend->vk.vkCreateDescriptorSetLayout(backend->device, &create, 0, &entry.layout);
    free(vk_bindings);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: vkCreateDescriptorSetLayout failed: %s", vulkan_result_string(result));
        free(sorted);
        return VK_NULL_HANDLE;
    }
    if (cache_insert(&backend->set_layouts, &backend->set_layout_count, &backend->set_layout_capacity,
                     &backend->set_layout_index, entry) == 0) {
        backend->vk.vkDestroyDescriptorSetLayout(backend->device, entry.layout, 0);
        free(sorted);
        return VK_NULL_HANDLE;
    }
    return entry.layout;
}

static const s_vulkan_pipeline_layout_entry *find_pipeline_layout(const s_vulkan_backend *backend,
                                                                   VkPipelineLayout layout)
{
    for (uint32_t i = 0; i < backend->pipeline_layout_count; i++)
        if (backend->pipeline_layouts[i].layout == layout)
            return &backend->pipeline_layouts[i];
    return 0;
}

VkPipelineLayout vulkan_pipeline_layout_with_push(s_vulkan_backend *backend,
                                                  const VkDescriptorSetLayout *set_layouts, uint32_t set_count,
                                                  const VkPushConstantRange &push)
{
    if (set_count > VULKAN_MAX_SETS) {
        vulkan_log(backend, "vulkan: pipeline layout: %u sets (at most %u)", set_count, (uint32_t)VULKAN_MAX_SETS);
        return VK_NULL_HANDLE;
    }
    s_vulkan_pipeline_layout_entry entry = {};
    entry.set_count = set_count;
    entry.push = push;
    for (uint32_t i = 0; i < set_count; i++) {
        entry.set_layouts[i] = set_layouts[i];
        if (entry.set_layouts[i] == VK_NULL_HANDLE)
            entry.set_layouts[i] = vulkan_descriptor_set_layout_get(backend, 0, 0);
        if (entry.set_layouts[i] == VK_NULL_HANDLE)
            return VK_NULL_HANDLE;
    }
    entry.hash = vulkan_hash_bytes(2166136261u, entry.set_layouts, set_count * sizeof(VkDescriptorSetLayout));
    entry.hash = vulkan_hash_bytes(entry.hash, &entry.push, sizeof(entry.push));
    const s_vulkan_pipeline_layout_entry *found = cache_find(
        backend->pipeline_layouts, backend->pipeline_layout_index, entry.hash,
        [&](const s_vulkan_pipeline_layout_entry &candidate) {
            return candidate.set_count == set_count &&
                   candidate.push.stageFlags == push.stageFlags && candidate.push.offset == push.offset &&
                   candidate.push.size == push.size &&
                   memcmp(candidate.set_layouts, entry.set_layouts, set_count * sizeof(VkDescriptorSetLayout)) == 0;
        });
    if (found != 0)
        return found->layout;
    VkPipelineLayoutCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    create.setLayoutCount = set_count;
    create.pSetLayouts = entry.set_layouts;
    create.pushConstantRangeCount = push.size != 0 ? 1u : 0u;
    create.pPushConstantRanges = push.size != 0 ? &entry.push : 0;
    VkResult result = backend->vk.vkCreatePipelineLayout(backend->device, &create, 0, &entry.layout);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: vkCreatePipelineLayout failed: %s", vulkan_result_string(result));
        return VK_NULL_HANDLE;
    }
    if (cache_insert(&backend->pipeline_layouts, &backend->pipeline_layout_count, &backend->pipeline_layout_capacity,
                     &backend->pipeline_layout_index, entry) == 0) {
        backend->vk.vkDestroyPipelineLayout(backend->device, entry.layout, 0);
        return VK_NULL_HANDLE;
    }
    return entry.layout;
}

extern "C" VkPipelineLayout vulkan_pipeline_layout_get(s_vulkan_backend *backend,
                                                       const VkDescriptorSetLayout *set_layouts, uint32_t set_count)
{
    return vulkan_pipeline_layout_with_push(backend, set_layouts, set_count, {});
}

// ---- pipelines ----

extern "C" void vulkan_graphics_state_default(s_vulkan_graphics_state *state, uint32_t color_format,
                                              uint32_t depth_stencil_format)
{
    memset(state, 0, sizeof(*state));
    state->topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    state->polygon_mode = VK_POLYGON_MODE_FILL;
    state->cull_mode = VK_CULL_MODE_NONE;
    state->front_face = VK_FRONT_FACE_CLOCKWISE;  // D3D: clockwise triangles face the viewer
    state->depth_compare = VK_COMPARE_OP_LESS_OR_EQUAL;
    s_vulkan_stencil_face keep = { VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS,
                                   0xff, 0xff, 0 };
    state->stencil_front = keep;
    state->stencil_back = keep;
    state->sample_count = VK_SAMPLE_COUNT_1_BIT;
    if (color_format != VK_FORMAT_UNDEFINED) {
        state->color_attachment_count = 1;
        state->color_formats[0] = color_format;
    }
    for (uint32_t i = 0; i < VULKAN_MAX_COLOR_ATTACHMENTS; i++) {
        s_vulkan_blend_attachment &blend = state->blend[i];
        blend.src_color = blend.src_alpha = VK_BLEND_FACTOR_ONE;
        blend.dst_color = blend.dst_alpha = VK_BLEND_FACTOR_ZERO;
        blend.color_op = blend.alpha_op = VK_BLEND_OP_ADD;
        blend.write_mask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                           VK_COLOR_COMPONENT_A_BIT;
    }
    state->depth_format = depth_stencil_format;
    if ((format_aspect(depth_stencil_format) & VK_IMAGE_ASPECT_STENCIL_BIT) != 0)
        state->stencil_format = depth_stencil_format;
}

static s_vulkan_pipeline *make_pipeline(s_vulkan_backend *backend, VkPipeline handle, VkPipelineLayout layout,
                                        uint32_t bind_point, uint32_t null_binding)
{
    s_vulkan_pipeline *pipeline = (s_vulkan_pipeline *)calloc(1, sizeof(s_vulkan_pipeline));
    if (pipeline == 0) {
        backend->vk.vkDestroyPipeline(backend->device, handle, 0);
        return 0;
    }
    pipeline->pipeline = handle;
    pipeline->layout = layout;
    pipeline->bind_point = bind_point;
    pipeline->null_binding = null_binding;
    const s_vulkan_pipeline_layout_entry *entry = find_pipeline_layout(backend, layout);
    if (entry != 0) {
        pipeline->set_count = entry->set_count;
        memcpy(pipeline->set_layouts, entry->set_layouts, sizeof(pipeline->set_layouts));
    }
    return pipeline;
}

static VkStencilOpState stencil_state(const s_vulkan_stencil_face &face)
{
    VkStencilOpState state = {};
    state.failOp = (VkStencilOp)face.fail_op;
    state.passOp = (VkStencilOp)face.pass_op;
    state.depthFailOp = (VkStencilOp)face.depth_fail_op;
    state.compareOp = (VkCompareOp)face.compare_op;
    state.compareMask = face.compare_mask;
    state.writeMask = face.write_mask;
    state.reference = face.reference;
    return state;
}

// The fixed-function part of a graphics pipeline, from shader stages (the vertex stage first; a tessellation control
// stage makes it a patch list of patch_control_points). vulkan_graphics_pipeline_create (pack modules) and
// vulkan_graphics_pipeline_create_stages (translated Xenos shaders) differ only in their stages.
static s_vulkan_pipeline *create_graphics_pipeline(s_vulkan_backend *backend, const VkPipelineShaderStageCreateInfo *stages,
                                                   uint32_t stage_count, VkPipelineLayout layout,
                                                   const s_xenos_vertex_input *vertex_input,
                                                   const s_vulkan_graphics_state *state, uint32_t patch_control_points)
{
    // the declaration's bindings and attributes (xenos_vertex.h: host formats as e_gpu_format)
    VkVertexInputBindingDescription bindings[XENOS_VERTEX_MAX_STREAMS];
    VkVertexInputAttributeDescription attributes[XENOS_VERTEX_MAX_ATTRIBUTES];
    VkPipelineVertexInputStateCreateInfo input = {};
    input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    if (vertex_input != 0) {
        const uint32_t binding_count = vertex_input->binding_count < XENOS_VERTEX_MAX_STREAMS ? vertex_input->binding_count
                                                                                              : XENOS_VERTEX_MAX_STREAMS;
        const uint32_t attribute_count = vertex_input->attribute_count < XENOS_VERTEX_MAX_ATTRIBUTES
                                             ? vertex_input->attribute_count : XENOS_VERTEX_MAX_ATTRIBUTES;
        for (uint32_t i = 0; i < binding_count; i++) {
            const s_xenos_vertex_binding &b = vertex_input->bindings[i];
            bindings[i].binding = b.binding;
            bindings[i].stride = b.stride;
            bindings[i].inputRate =
                b.input_rate == k_xenos_vertex_rate_instance ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
        }
        for (uint32_t i = 0; i < attribute_count; i++) {
            const s_xenos_vertex_attribute &a = vertex_input->attributes[i];
            attributes[i].location = a.location;
            attributes[i].binding = a.binding;
            attributes[i].format = (VkFormat)vulkan_format_from_gpu(a.format);
            attributes[i].offset = a.offset;
        }
        input.vertexBindingDescriptionCount = binding_count;
        input.pVertexBindingDescriptions = bindings;
        input.vertexAttributeDescriptionCount = attribute_count;
        input.pVertexAttributeDescriptions = attributes;
    }
    VkPipelineInputAssemblyStateCreateInfo assembly = {};
    assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assembly.topology = patch_control_points != 0 ? VK_PRIMITIVE_TOPOLOGY_PATCH_LIST : (VkPrimitiveTopology)state->topology;
    assembly.primitiveRestartEnable = state->primitive_restart ? VK_TRUE : VK_FALSE;
    VkPipelineViewportStateCreateInfo viewport = {};
    viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport.viewportCount = 1;
    viewport.scissorCount = 1;
    VkPipelineRasterizationStateCreateInfo raster = {};
    raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster.polygonMode = (VkPolygonMode)state->polygon_mode;
    raster.cullMode = state->cull_mode;
    raster.frontFace = (VkFrontFace)state->front_face;
    raster.depthBiasEnable = state->depth_bias_enable ? VK_TRUE : VK_FALSE;
    raster.depthBiasConstantFactor = state->depth_bias_constant;
    raster.depthBiasSlopeFactor = state->depth_bias_slope;
    raster.lineWidth = 1.0f;
    VkPipelineMultisampleStateCreateInfo multisample = {};
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples =
        state->sample_count != 0 ? (VkSampleCountFlagBits)state->sample_count : VK_SAMPLE_COUNT_1_BIT;
    multisample.alphaToCoverageEnable = state->alpha_to_coverage ? VK_TRUE : VK_FALSE;
    VkPipelineDepthStencilStateCreateInfo depth = {};
    depth.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth.depthTestEnable = state->depth_test ? VK_TRUE : VK_FALSE;
    depth.depthWriteEnable = state->depth_write ? VK_TRUE : VK_FALSE;
    depth.depthCompareOp = (VkCompareOp)state->depth_compare;
    depth.stencilTestEnable = state->stencil_test ? VK_TRUE : VK_FALSE;
    depth.front = stencil_state(state->stencil_front);
    depth.back = stencil_state(state->stencil_back);
    depth.maxDepthBounds = 1.0f;
    VkPipelineColorBlendAttachmentState attachments[VULKAN_MAX_COLOR_ATTACHMENTS] = {};
    for (uint32_t i = 0; i < state->color_attachment_count; i++) {
        const s_vulkan_blend_attachment &blend = state->blend[i];
        attachments[i].blendEnable = blend.enable ? VK_TRUE : VK_FALSE;
        attachments[i].srcColorBlendFactor = (VkBlendFactor)blend.src_color;
        attachments[i].dstColorBlendFactor = (VkBlendFactor)blend.dst_color;
        attachments[i].colorBlendOp = (VkBlendOp)blend.color_op;
        attachments[i].srcAlphaBlendFactor = (VkBlendFactor)blend.src_alpha;
        attachments[i].dstAlphaBlendFactor = (VkBlendFactor)blend.dst_alpha;
        attachments[i].alphaBlendOp = (VkBlendOp)blend.alpha_op;
        attachments[i].colorWriteMask = blend.write_mask;
    }
    VkPipelineColorBlendStateCreateInfo blend = {};
    blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend.attachmentCount = state->color_attachment_count;
    blend.pAttachments = attachments;
    memcpy(blend.blendConstants, state->blend_constants, sizeof(blend.blendConstants));
    static const VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE,  // D3D sets the stride per stream at bind time (core 1.3)
    };
    VkPipelineDynamicStateCreateInfo dynamic = {};
    dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic.dynamicStateCount = sizeof(dynamic_states) / sizeof(dynamic_states[0]);
    dynamic.pDynamicStates = dynamic_states;
    VkFormat color_formats[VULKAN_MAX_COLOR_ATTACHMENTS];
    for (uint32_t i = 0; i < state->color_attachment_count; i++)
        color_formats[i] = (VkFormat)state->color_formats[i];
    VkPipelineRenderingCreateInfo rendering = {};
    rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering.colorAttachmentCount = state->color_attachment_count;
    rendering.pColorAttachmentFormats = color_formats;
    rendering.depthAttachmentFormat = (VkFormat)state->depth_format;
    rendering.stencilAttachmentFormat = (VkFormat)state->stencil_format;

    VkGraphicsPipelineCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create.pNext = &rendering;
    create.stageCount = stage_count;
    create.pStages = stages;
    create.pVertexInputState = &input;
    create.pInputAssemblyState = &assembly;
    VkPipelineTessellationStateCreateInfo tessellation = {};
    tessellation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellation.patchControlPoints = patch_control_points;
    create.pTessellationState = patch_control_points != 0 ? &tessellation : 0;
    create.pViewportState = &viewport;
    create.pRasterizationState = &raster;
    create.pMultisampleState = &multisample;
    create.pDepthStencilState = &depth;
    create.pColorBlendState = &blend;
    create.pDynamicState = &dynamic;
    create.layout = layout;
    create.basePipelineIndex = -1;
    VkPipeline handle = VK_NULL_HANDLE;
    VkResult result =
        backend->vk.vkCreateGraphicsPipelines(backend->device, backend->pipeline_cache, 1, &create, 0, &handle);
    if (result == VK_SUCCESS)
        __atomic_fetch_add(&backend->pipelines_created, 1u, __ATOMIC_RELAXED); // pipeline libraries: any thread
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: vkCreateGraphicsPipelines (%s, %s) failed: %s", stages[0].pName,
                   stages[stage_count - 1].stage == VK_SHADER_STAGE_FRAGMENT_BIT ? stages[stage_count - 1].pName
                                                                                  : "no fragment shader",
                   vulkan_result_string(result));
        return 0;
    }
    s_vulkan_pipeline *pipeline = make_pipeline(backend, handle, layout, VK_PIPELINE_BIND_POINT_GRAPHICS,
                                                vertex_input != 0 ? vertex_input->null_binding : 0xffffffffu);
    if (pipeline != 0) {
        pipeline->dynamic_viewport_scissor = true;
        pipeline->dynamic_vertex_stride = true;
    }
    for (uint32_t b = 0; pipeline != 0 && vertex_input != 0 && b < vertex_input->binding_count; b++)
        if (vertex_input->bindings[b].binding != vertex_input->null_binding)
            pipeline->vertex_streams |= 1u << vertex_input->bindings[b].binding;
    return pipeline;
}

extern "C" s_vulkan_pipeline *vulkan_graphics_pipeline_create(s_vulkan_backend *backend, VkShaderModule vertex,
                                                              const char *vertex_entry, VkShaderModule fragment,
                                                              const char *fragment_entry, VkPipelineLayout layout,
                                                              const s_xenos_vertex_input *vertex_input,
                                                              const s_vulkan_graphics_state *state)
{
    if (state->color_attachment_count > VULKAN_MAX_COLOR_ATTACHMENTS) {
        vulkan_log(backend, "vulkan: graphics pipeline: %u color attachments", state->color_attachment_count);
        return 0;
    }
    VkPipelineShaderStageCreateInfo stages[2] = {};
    stages[0].sType = stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = vertex;
    stages[0].pName = vertex_entry != 0 ? vertex_entry : "main";
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = fragment;
    stages[1].pName = fragment_entry != 0 ? fragment_entry : "main";
    // the fragment shader specializations (vulkan_spirv_patch_fragment): the render targets' exponent biases
    // (VULKAN_SPEC_COLOR_EXP_SCALE) and the tile's screen origin (VULKAN_SPEC_FRAGCOORD_OFFSET); constants a module
    // lacks are ignored
    const uint32_t k_specializations = VULKAN_MAX_COLOR_ATTACHMENTS + 2u;
    float spec_values[k_specializations];
    VkSpecializationMapEntry spec_entries[k_specializations];
    for (uint32_t i = 0; i < k_specializations; i++) {
        if (i < VULKAN_MAX_COLOR_ATTACHMENTS) {
            int32_t bias = state->color_exp_bias[i];
            bias = bias < -32 ? -32 : bias > 31 ? 31 : bias;
            const uint32_t bits = (uint32_t)(127 + bias) << 23; // 2^bias
            memcpy(&spec_values[i], &bits, sizeof(float));
            spec_entries[i].constantID = VULKAN_SPEC_COLOR_EXP_SCALE + i;
        } else {
            spec_values[i] = (float)state->fragcoord_offset[i - VULKAN_MAX_COLOR_ATTACHMENTS];
            spec_entries[i].constantID = VULKAN_SPEC_FRAGCOORD_OFFSET + (i - VULKAN_MAX_COLOR_ATTACHMENTS);
        }
        spec_entries[i].offset = i * (uint32_t)sizeof(float);
        spec_entries[i].size = sizeof(float);
    }
    VkSpecializationInfo specialization = {};
    specialization.mapEntryCount = k_specializations;
    specialization.pMapEntries = spec_entries;
    specialization.dataSize = sizeof(spec_values);
    specialization.pData = spec_values;
    stages[1].pSpecializationInfo = &specialization;

    return create_graphics_pipeline(backend, stages, fragment != VK_NULL_HANDLE ? 2u : 1u, layout, vertex_input, state, 0);
}

extern "C" s_vulkan_pipeline *vulkan_graphics_pipeline_create_stages(s_vulkan_backend *backend,
                                                                     const s_vulkan_shader_stage *stages,
                                                                     uint32_t stage_count, VkPipelineLayout layout,
                                                                     const s_vulkan_graphics_state *state,
                                                                     uint32_t patch_control_points)
{
    if (state->color_attachment_count > VULKAN_MAX_COLOR_ATTACHMENTS || stage_count == 0 || stage_count > 5 ||
        stages[0].stage != VK_SHADER_STAGE_VERTEX_BIT) {
        vulkan_log(backend, "vulkan: graphics pipeline: %u color attachments, %u stages", state->color_attachment_count,
                   stage_count);
        return 0;
    }
    VkPipelineShaderStageCreateInfo infos[5] = {};
    VkSpecializationMapEntry alpha_test_map = { 0u, 0u, sizeof(uint32_t) };
    uint32_t alpha_test_val = state->fragcoord_offset[0] != 0 ? 1u : 0u;
    VkSpecializationInfo alpha_test_spec = {};
    alpha_test_spec.mapEntryCount = 1;
    alpha_test_spec.pMapEntries = &alpha_test_map;
    alpha_test_spec.dataSize = sizeof(alpha_test_val);
    alpha_test_spec.pData = &alpha_test_val;
    bool tessellation = false;
    for (uint32_t i = 0; i < stage_count; i++) {
        infos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        infos[i].stage = (VkShaderStageFlagBits)stages[i].stage;
        infos[i].module = stages[i].module;
        infos[i].pName = stages[i].entry != 0 ? stages[i].entry : "main";
        if (stages[i].stage == VK_SHADER_STAGE_FRAGMENT_BIT)
            infos[i].pSpecializationInfo = &alpha_test_spec;
        tessellation |= stages[i].stage == VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    }
    return create_graphics_pipeline(backend, infos, stage_count, layout, 0, state,
                                    tessellation ? (patch_control_points != 0 ? patch_control_points : 3u) : 0u);
}

// ---- pipeline libraries (VK_EXT_graphics_pipeline_library) ----

// The dynamic state of each library part (the linked pipeline has their union; vulkan_cmd_bind_pipeline sets it).
static const VkDynamicState k_pre_rasterization_dynamic[] = {
    VK_DYNAMIC_STATE_VIEWPORT,          VK_DYNAMIC_STATE_SCISSOR,           VK_DYNAMIC_STATE_CULL_MODE,
    VK_DYNAMIC_STATE_FRONT_FACE,        VK_DYNAMIC_STATE_DEPTH_BIAS_ENABLE, VK_DYNAMIC_STATE_DEPTH_BIAS,
};
static const VkDynamicState k_fragment_shader_dynamic[] = {
    VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,    VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE, VK_DYNAMIC_STATE_DEPTH_COMPARE_OP,
    VK_DYNAMIC_STATE_STENCIL_TEST_ENABLE,  VK_DYNAMIC_STATE_STENCIL_OP,         VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
    VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,   VK_DYNAMIC_STATE_STENCIL_REFERENCE,
};

// One library part: create (flags, stages, layout and the part's state already filled in) as a library of `part`.
static VkPipeline create_library(s_vulkan_backend *backend, VkGraphicsPipelineCreateInfo *create, uint32_t part,
                                 const char *what)
{
    VkGraphicsPipelineLibraryCreateInfoEXT library = {};
    library.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_LIBRARY_CREATE_INFO_EXT;
    library.flags = part;
    // dynamic rendering: no view mask; the formats count only in the fragment output part (create->pNext)
    VkPipelineRenderingCreateInfo rendering = {};
    rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    if (create->pNext != 0)
        rendering = *(const VkPipelineRenderingCreateInfo *)create->pNext;
    rendering.pNext = &library;
    create->sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create->pNext = &rendering;
    create->flags |= VK_PIPELINE_CREATE_LIBRARY_BIT_KHR | VK_PIPELINE_CREATE_RETAIN_LINK_TIME_OPTIMIZATION_INFO_BIT_EXT;
    create->basePipelineIndex = -1;
    VkPipeline handle = VK_NULL_HANDLE;
    const VkResult result =
        backend->vk.vkCreateGraphicsPipelines(backend->device, backend->pipeline_cache, 1, create, 0, &handle);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: pipeline library (%s) failed: %s", what, vulkan_result_string(result));
        return VK_NULL_HANDLE;
    }
    __atomic_fetch_add(&backend->pipelines_created, 1u, __ATOMIC_RELAXED);
    return handle;
}

extern "C" VkPipeline vulkan_pipeline_library_vertex_input(s_vulkan_backend *backend,
                                                           const s_vulkan_graphics_state *state,
                                                           uint32_t patch_control_points)
{
    VkPipelineVertexInputStateCreateInfo input = {};
    input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VkPipelineInputAssemblyStateCreateInfo assembly = {};
    assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assembly.topology = patch_control_points != 0 ? VK_PRIMITIVE_TOPOLOGY_PATCH_LIST : (VkPrimitiveTopology)state->topology;
    assembly.primitiveRestartEnable = state->primitive_restart ? VK_TRUE : VK_FALSE;
    VkGraphicsPipelineCreateInfo create = {};
    create.pVertexInputState = &input;
    create.pInputAssemblyState = &assembly;
    return create_library(backend, &create, VK_GRAPHICS_PIPELINE_LIBRARY_VERTEX_INPUT_INTERFACE_BIT_EXT, "vertex input");
}

extern "C" VkPipeline vulkan_pipeline_library_pre_rasterization(s_vulkan_backend *backend,
                                                                const s_vulkan_shader_stage *stages,
                                                                uint32_t stage_count, VkPipelineLayout layout,
                                                                const s_vulkan_graphics_state *state,
                                                                uint32_t patch_control_points)
{
    if (stage_count == 0 || stage_count > 4 || stages[0].stage != VK_SHADER_STAGE_VERTEX_BIT)
        return VK_NULL_HANDLE;
    VkPipelineShaderStageCreateInfo infos[4] = {};
    for (uint32_t i = 0; i < stage_count; i++) {
        infos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        infos[i].stage = (VkShaderStageFlagBits)stages[i].stage;
        infos[i].module = stages[i].module;
        infos[i].pName = stages[i].entry != 0 ? stages[i].entry : "main";
        if (infos[i].module == VK_NULL_HANDLE)
            return VK_NULL_HANDLE;
    }
    VkPipelineViewportStateCreateInfo viewport = {};
    viewport.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport.viewportCount = 1;
    viewport.scissorCount = 1;
    VkPipelineRasterizationStateCreateInfo raster = {};
    raster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    raster.polygonMode = (VkPolygonMode)state->polygon_mode;
    raster.lineWidth = 1.0f;
    VkPipelineTessellationStateCreateInfo tessellation = {};
    tessellation.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellation.patchControlPoints = patch_control_points;
    VkPipelineDynamicStateCreateInfo dynamic = {};
    dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic.dynamicStateCount = sizeof(k_pre_rasterization_dynamic) / sizeof(k_pre_rasterization_dynamic[0]);
    dynamic.pDynamicStates = k_pre_rasterization_dynamic;
    VkGraphicsPipelineCreateInfo create = {};
    create.stageCount = stage_count;
    create.pStages = infos;
    create.pViewportState = &viewport;
    create.pRasterizationState = &raster;
    create.pTessellationState = patch_control_points != 0 ? &tessellation : 0;
    create.pDynamicState = &dynamic;
    create.layout = layout;
    return create_library(backend, &create, VK_GRAPHICS_PIPELINE_LIBRARY_PRE_RASTERIZATION_SHADERS_BIT_EXT,
                          "pre-rasterization");
}

extern "C" VkPipeline vulkan_pipeline_library_fragment_shader(s_vulkan_backend *backend, VkShaderModule fragment,
                                                              VkPipelineLayout layout, uint32_t alpha_test_enable)
{
    VkSpecializationMapEntry alpha_test_map = { 0u, 0u, sizeof(uint32_t) };
    uint32_t alpha_test_val = alpha_test_enable != 0 ? 1u : 0u;
    VkSpecializationInfo alpha_test_spec = {};
    alpha_test_spec.mapEntryCount = 1;
    alpha_test_spec.pMapEntries = &alpha_test_map;
    alpha_test_spec.dataSize = sizeof(alpha_test_val);
    alpha_test_spec.pData = &alpha_test_val;
    VkPipelineShaderStageCreateInfo stage = {};
    stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stage.module = fragment;
    stage.pName = "main";
    stage.pSpecializationInfo = &alpha_test_spec;
    // every depth / stencil test value is dynamic; no sample shading, so no multisample state here (dynamic
    // rendering: the fragment output part has it)
    VkPipelineDepthStencilStateCreateInfo depth = {};
    depth.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth.maxDepthBounds = 1.0f;
    VkPipelineDynamicStateCreateInfo dynamic = {};
    dynamic.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic.dynamicStateCount = sizeof(k_fragment_shader_dynamic) / sizeof(k_fragment_shader_dynamic[0]);
    dynamic.pDynamicStates = k_fragment_shader_dynamic;
    VkGraphicsPipelineCreateInfo create = {};
    create.stageCount = fragment != VK_NULL_HANDLE ? 1u : 0u;
    create.pStages = &stage;
    create.pDepthStencilState = &depth;
    create.pDynamicState = &dynamic;
    create.layout = layout;
    return create_library(backend, &create, VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_SHADER_BIT_EXT, "fragment shader");
}

extern "C" VkPipeline vulkan_pipeline_library_fragment_output(s_vulkan_backend *backend,
                                                              const s_vulkan_graphics_state *state)
{
    if (state->color_attachment_count > VULKAN_MAX_COLOR_ATTACHMENTS)
        return VK_NULL_HANDLE;
    VkPipelineMultisampleStateCreateInfo multisample = {};
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.rasterizationSamples =
        state->sample_count != 0 ? (VkSampleCountFlagBits)state->sample_count : VK_SAMPLE_COUNT_1_BIT;
    multisample.alphaToCoverageEnable = state->alpha_to_coverage ? VK_TRUE : VK_FALSE;
    VkPipelineColorBlendAttachmentState attachments[VULKAN_MAX_COLOR_ATTACHMENTS] = {};
    for (uint32_t i = 0; i < state->color_attachment_count; i++) {
        const s_vulkan_blend_attachment &blend = state->blend[i];
        attachments[i].blendEnable = blend.enable ? VK_TRUE : VK_FALSE;
        attachments[i].srcColorBlendFactor = (VkBlendFactor)blend.src_color;
        attachments[i].dstColorBlendFactor = (VkBlendFactor)blend.dst_color;
        attachments[i].colorBlendOp = (VkBlendOp)blend.color_op;
        attachments[i].srcAlphaBlendFactor = (VkBlendFactor)blend.src_alpha;
        attachments[i].dstAlphaBlendFactor = (VkBlendFactor)blend.dst_alpha;
        attachments[i].alphaBlendOp = (VkBlendOp)blend.alpha_op;
        attachments[i].colorWriteMask = blend.write_mask;
    }
    VkPipelineColorBlendStateCreateInfo blend = {};
    blend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend.attachmentCount = state->color_attachment_count;
    blend.pAttachments = attachments;
    memcpy(blend.blendConstants, state->blend_constants, sizeof(blend.blendConstants));
    VkFormat color_formats[VULKAN_MAX_COLOR_ATTACHMENTS];
    for (uint32_t i = 0; i < state->color_attachment_count; i++)
        color_formats[i] = (VkFormat)state->color_formats[i];
    VkPipelineRenderingCreateInfo rendering = {};
    rendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering.colorAttachmentCount = state->color_attachment_count;
    rendering.pColorAttachmentFormats = color_formats;
    rendering.depthAttachmentFormat = (VkFormat)state->depth_format;
    rendering.stencilAttachmentFormat = (VkFormat)state->stencil_format;
    VkGraphicsPipelineCreateInfo create = {};
    create.pNext = &rendering;
    create.pMultisampleState = &multisample;
    create.pColorBlendState = &blend;
    return create_library(backend, &create, VK_GRAPHICS_PIPELINE_LIBRARY_FRAGMENT_OUTPUT_INTERFACE_BIT_EXT,
                          "fragment output");
}

extern "C" void vulkan_pipeline_library_destroy(s_vulkan_backend *backend, VkPipeline library)
{
    if (library != VK_NULL_HANDLE)
        backend->vk.vkDestroyPipeline(backend->device, library, 0);
}

extern "C" VkPipeline vulkan_pipeline_link_handle(s_vulkan_backend *backend, const VkPipeline libraries[4],
                                                  VkPipelineLayout layout, uint32_t optimize)
{
    for (uint32_t i = 0; i < 4; i++)
        if (libraries[i] == VK_NULL_HANDLE)
            return VK_NULL_HANDLE;
    VkPipelineLibraryCreateInfoKHR link = {};
    link.sType = VK_STRUCTURE_TYPE_PIPELINE_LIBRARY_CREATE_INFO_KHR;
    link.libraryCount = 4;
    link.pLibraries = libraries;
    VkGraphicsPipelineCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    create.pNext = &link;
    create.flags = optimize ? VK_PIPELINE_CREATE_LINK_TIME_OPTIMIZATION_BIT_EXT : 0u;
    create.layout = layout;
    create.basePipelineIndex = -1;
    VkPipeline handle = VK_NULL_HANDLE;
    const VkResult result =
        backend->vk.vkCreateGraphicsPipelines(backend->device, backend->pipeline_cache, 1, &create, 0, &handle);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: linking a pipeline from libraries%s failed: %s", optimize ? " (optimized)" : "",
                   vulkan_result_string(result));
        return VK_NULL_HANDLE;
    }
    if (optimize)
        __atomic_fetch_add(&backend->pipelines_created, 1u, __ATOMIC_RELAXED);
    return handle;
}

static s_vulkan_dynamic_state pipeline_dynamic_state(const s_vulkan_graphics_state &state)
{
    return { state.cull_mode, (VkFrontFace)state.front_face, state.depth_test, state.depth_write,
             (VkCompareOp)state.depth_compare, state.depth_bias_enable, state.depth_bias_constant,
             state.depth_bias_slope, state.stencil_test, state.stencil_front, state.stencil_back };
}

extern "C" s_vulkan_pipeline *vulkan_pipeline_link(s_vulkan_backend *backend, const VkPipeline libraries[4],
                                                   VkPipelineLayout layout, const s_vulkan_graphics_state *state,
                                                   uint32_t optimize)
{
    VkPipeline handle = vulkan_pipeline_link_handle(backend, libraries, layout, optimize);
    if (handle == VK_NULL_HANDLE)
        return 0;
    s_vulkan_pipeline *pipeline = make_pipeline(backend, handle, layout, VK_PIPELINE_BIND_POINT_GRAPHICS, 0xffffffffu);
    if (pipeline != 0) {
        pipeline->dynamic = 1;
        pipeline->dynamic_viewport_scissor = true;
        pipeline->dynamic_state = pipeline_dynamic_state(*state);
    }
    return pipeline;
}

extern "C" void vulkan_pipeline_replace(s_vulkan_backend *backend, s_vulkan_pipeline *pipeline, VkPipeline handle)
{
    if (pipeline == 0 || handle == VK_NULL_HANDLE)
        return;
    retire(backend, 0, 0, pipeline->pipeline);
    pipeline->pipeline = handle;
}

// The dynamic state of a pipeline linked from libraries (k_pre_rasterization_dynamic, k_fragment_shader_dynamic), as
// its s_vulkan_graphics_state has it: the values create_graphics_pipeline would have built in. Set only when they
// differ from what the command buffer has.
// The dynamic fields run from cull_mode through stencil_back. Blend constants belong to the static blend state.
const size_t k_dynamic_state_first = offsetof(s_vulkan_graphics_state, cull_mode);
const size_t k_dynamic_state_end = offsetof(s_vulkan_graphics_state, sample_count);
static_assert(offsetof(s_vulkan_graphics_state, front_face) > k_dynamic_state_first &&
                  offsetof(s_vulkan_graphics_state, depth_test) > k_dynamic_state_first &&
                  offsetof(s_vulkan_graphics_state, depth_write) > k_dynamic_state_first &&
                  offsetof(s_vulkan_graphics_state, depth_compare) > k_dynamic_state_first &&
                  offsetof(s_vulkan_graphics_state, depth_bias_enable) > k_dynamic_state_first &&
                  offsetof(s_vulkan_graphics_state, depth_bias_constant) > k_dynamic_state_first &&
                  offsetof(s_vulkan_graphics_state, depth_bias_slope) > k_dynamic_state_first &&
                  offsetof(s_vulkan_graphics_state, stencil_test) > k_dynamic_state_first &&
                  offsetof(s_vulkan_graphics_state, stencil_front) > k_dynamic_state_first &&
                  offsetof(s_vulkan_graphics_state, stencil_back) + sizeof(s_vulkan_stencil_face) == k_dynamic_state_end &&
                  offsetof(s_vulkan_graphics_state, polygon_mode) < k_dynamic_state_first,
              "the library pipelines' dynamic state (k_pre_rasterization_dynamic, k_fragment_shader_dynamic) is the "
              "contiguous cull_mode .. stencil_back of s_vulkan_graphics_state");

extern "C" void vulkan_graphics_state_static_key(const s_vulkan_graphics_state *state, s_vulkan_graphics_state *out)
{
    *out = *state;
    memset((uint8_t *)out + k_dynamic_state_first, 0, k_dynamic_state_end - k_dynamic_state_first);
}

static_assert(offsetof(s_vulkan_dynamic_state, stencil_front) + sizeof(s_vulkan_stencil_face) ==
                  offsetof(s_vulkan_dynamic_state, stencil_back) &&
                  sizeof(s_vulkan_stencil_face) == 7u * sizeof(uint32_t),
              "stencil_front and stencil_back are 56 contiguous bytes with no padding");

enum e_dynamic_state_valid {
    k_dynamic_common = 1u << 0,
    k_dynamic_depth_bias = 1u << 1,
    k_dynamic_depth_test = 1u << 2,
    k_dynamic_stencil = 1u << 3,
};

static bool same_stencil_ops(const s_vulkan_stencil_face &a, const s_vulkan_stencil_face &b)
{
    return a.fail_op == b.fail_op && a.pass_op == b.pass_op && a.depth_fail_op == b.depth_fail_op &&
           a.compare_op == b.compare_op;
}

static void set_stencil_ops(VkCommandBuffer command_buffer, PFN_vkCmdSetStencilOp set, VkStencilFaceFlags faces,
                            const s_vulkan_stencil_face &state)
{
    set(command_buffer, faces, (VkStencilOp)state.fail_op, (VkStencilOp)state.pass_op,
        (VkStencilOp)state.depth_fail_op, (VkCompareOp)state.compare_op);
}

static void set_stencil_value(VkCommandBuffer command_buffer, PFN_vkCmdSetStencilReference set, bool unset,
                              uint32_t front, uint32_t back, uint32_t last_front, uint32_t last_back)
{
    const bool diff_front = unset || front != last_front;
    const bool diff_back = unset || back != last_back;
    if (front == back && (diff_front || diff_back)) {
        set(command_buffer, VK_STENCIL_FACE_FRONT_AND_BACK, front);
    } else {
        if (diff_front)
            set(command_buffer, VK_STENCIL_FACE_FRONT_BIT, front);
        if (diff_back)
            set(command_buffer, VK_STENCIL_FACE_BACK_BIT, back);
    }
}

static void set_dynamic_state(s_vulkan_backend *backend, VkCommandBuffer command_buffer, const s_vulkan_dynamic_state &s)
{
    using vulkan_recording::same_float;
    s_vulkan_functions &vk = backend->vk;
    s_vulkan_dynamic_state &last = backend->dynamic_state;
    uint32_t valid = backend->untracked_commands ? 0 : backend->dynamic_valid;
    const bool all = (valid & k_dynamic_common) == 0;
    const bool bias = s.depth_bias_enable != 0;
    const bool depth = s.depth_test != 0;
    const bool stencil = s.stencil_test != 0;

    const bool set_cull = all || last.cull_mode != s.cull_mode;
    const bool set_front = all || last.front_face != s.front_face;
    const bool set_bias_enable = all || last.depth_bias_enable != bias;
    const bool set_depth_enable = all || last.depth_test != depth;
    const bool set_stencil_enable = all || last.stencil_test != stencil;

    // The draw only consumes these values when their test or bias is enabled. Keep the last values actually
    // emitted while disabled, and distinguish an unset value from a set value that happens to match.
    const bool set_bias = bias && ((valid & k_dynamic_depth_bias) == 0 ||
                                   !same_float(last.depth_bias_constant, s.depth_bias_constant) ||
                                   !same_float(last.depth_bias_slope, s.depth_bias_slope));
    const bool depth_unset = (valid & k_dynamic_depth_test) == 0;
    const bool set_depth_write = depth && (depth_unset || last.depth_write != (s.depth_write != 0));
    const bool set_depth_compare = depth && (depth_unset || last.depth_compare != s.depth_compare);
    const bool stencil_unset = (valid & k_dynamic_stencil) == 0;
    const size_t stencil_offset = offsetof(s_vulkan_dynamic_state, stencil_front);
    const bool set_stencil =
        stencil && (stencil_unset ||
                    memcmp((const uint8_t *)&last + stencil_offset, (const uint8_t *)&s + stencil_offset,
                           2 * sizeof(s_vulkan_stencil_face)) != 0);

    if (!(set_cull || set_front || set_bias_enable || set_depth_enable || set_stencil_enable ||
          set_bias || set_depth_write || set_depth_compare || set_stencil))
        return;
    backend->bind_counters.dynamic_states++;
    if (set_cull) {
        vk.vkCmdSetCullMode(command_buffer, s.cull_mode);
        last.cull_mode = s.cull_mode;
    }
    if (set_front) {
        vk.vkCmdSetFrontFace(command_buffer, s.front_face);
        last.front_face = s.front_face;
    }
    if (set_bias_enable) {
        vk.vkCmdSetDepthBiasEnable(command_buffer, bias);
        last.depth_bias_enable = bias;
    }
    if (set_bias) {
        vk.vkCmdSetDepthBias(command_buffer, s.depth_bias_constant, 0.0f, s.depth_bias_slope);
        last.depth_bias_constant = s.depth_bias_constant;
        last.depth_bias_slope = s.depth_bias_slope;
        valid |= k_dynamic_depth_bias;
    }
    if (set_depth_enable) {
        vk.vkCmdSetDepthTestEnable(command_buffer, depth);
        last.depth_test = depth;
    }
    if (set_depth_write) {
        const bool write = s.depth_write != 0;
        vk.vkCmdSetDepthWriteEnable(command_buffer, write);
        last.depth_write = write;
    }
    if (set_depth_compare) {
        vk.vkCmdSetDepthCompareOp(command_buffer, s.depth_compare);
        last.depth_compare = s.depth_compare;
        valid |= k_dynamic_depth_test;
    }
    if (set_stencil_enable) {
        vk.vkCmdSetStencilTestEnable(command_buffer, stencil);
        last.stencil_test = stencil;
    }
    if (set_stencil) {
        const s_vulkan_stencil_face &front = s.stencil_front, &back = s.stencil_back;
        const s_vulkan_stencil_face &last_front = last.stencil_front, &last_back = last.stencil_back;
        const bool diff_front = stencil_unset || !same_stencil_ops(last_front, front);
        const bool diff_back = stencil_unset || !same_stencil_ops(last_back, back);
        if ((diff_front || diff_back) && same_stencil_ops(front, back)) {
            set_stencil_ops(command_buffer, vk.vkCmdSetStencilOp, VK_STENCIL_FACE_FRONT_AND_BACK, front);
        } else {
            if (diff_front)
                set_stencil_ops(command_buffer, vk.vkCmdSetStencilOp, VK_STENCIL_FACE_FRONT_BIT, front);
            if (diff_back)
                set_stencil_ops(command_buffer, vk.vkCmdSetStencilOp, VK_STENCIL_FACE_BACK_BIT, back);
        }
        set_stencil_value(command_buffer, vk.vkCmdSetStencilCompareMask, stencil_unset,
                          front.compare_mask, back.compare_mask, last_front.compare_mask, last_back.compare_mask);
        set_stencil_value(command_buffer, vk.vkCmdSetStencilWriteMask, stencil_unset,
                          front.write_mask, back.write_mask, last_front.write_mask, last_back.write_mask);
        set_stencil_value(command_buffer, vk.vkCmdSetStencilReference, stencil_unset,
                          front.reference, back.reference, last_front.reference, last_back.reference);
        last.stencil_front = front;
        last.stencil_back = back;
        valid |= k_dynamic_stencil;
    }
    backend->dynamic_valid = valid | k_dynamic_common;
}

extern "C" VkPipeline vulkan_compute_pipeline_create_handle(s_vulkan_backend *backend, VkShaderModule module,
                                                            const char *entry, VkPipelineLayout layout)
{
    VkComputePipelineCreateInfo create = {};
    create.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    create.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    create.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    create.stage.module = module;
    create.stage.pName = entry != 0 ? entry : "main";
    create.layout = layout;
    create.basePipelineIndex = -1;
    VkPipeline handle = VK_NULL_HANDLE;
    VkResult result =
        backend->vk.vkCreateComputePipelines(backend->device, backend->pipeline_cache, 1, &create, 0, &handle);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: vkCreateComputePipelines (%s) failed: %s", create.stage.pName,
                   vulkan_result_string(result));
        return VK_NULL_HANDLE;
    }
    __atomic_fetch_add(&backend->pipelines_created, 1u, __ATOMIC_RELAXED); // any thread (pipeline prewarm)
    return handle;
}

extern "C" s_vulkan_pipeline *vulkan_pipeline_adopt(s_vulkan_backend *backend, VkPipeline handle, VkPipelineLayout layout,
                                                    uint32_t bind_point)
{
    return handle != VK_NULL_HANDLE ? make_pipeline(backend, handle, layout, bind_point, 0xffffffffu) : 0;
}

extern "C" s_vulkan_pipeline *vulkan_compute_pipeline_create(s_vulkan_backend *backend, VkShaderModule module,
                                                             const char *entry, VkPipelineLayout layout)
{
    return vulkan_pipeline_adopt(backend, vulkan_compute_pipeline_create_handle(backend, module, entry, layout), layout,
                                 VK_PIPELINE_BIND_POINT_COMPUTE);
}

extern "C" void vulkan_pipeline_destroy(s_vulkan_backend *backend, s_vulkan_pipeline *pipeline)
{
    if (pipeline == 0)
        return;
    backend->vk.vkDestroyPipeline(backend->device, pipeline->pipeline, 0);
    free(pipeline);
}

extern "C" VkPipeline vulkan_pipeline_handle(const s_vulkan_pipeline *pipeline) { return pipeline->pipeline; }
extern "C" VkPipelineLayout vulkan_pipeline_layout(const s_vulkan_pipeline *pipeline) { return pipeline->layout; }

extern "C" VkDescriptorSetLayout vulkan_pipeline_set_layout(const s_vulkan_pipeline *pipeline, uint32_t set)
{
    return set < pipeline->set_count ? pipeline->set_layouts[set] : VK_NULL_HANDLE;
}

// ---- descriptors ----

extern "C" VkDescriptorSet vulkan_descriptor_set_allocate(s_vulkan_backend *backend, VkDescriptorSetLayout layout,
                                                          uint32_t transient)
{
    VkDescriptorPool pool = VK_NULL_HANDLE;
    if (transient) {
        if (!backend->recording) {
            vulkan_log(backend, "vulkan: transient descriptor sets are allocated within a frame");
            return VK_NULL_HANDLE;
        }
        return allocate_from_pools(backend, &backend->frames[backend->frame_index].transient_pools, layout, false, &pool);
    }
    if (backend->persistent_set_count == backend->persistent_set_capacity) {
        uint32_t capacity = backend->persistent_set_capacity != 0 ? backend->persistent_set_capacity * 2 : 64;
        s_vulkan_persistent_set *grown =
            (s_vulkan_persistent_set *)realloc(backend->persistent_sets, capacity * sizeof(s_vulkan_persistent_set));
        if (grown == 0)
            return VK_NULL_HANDLE;
        backend->persistent_sets = grown;
        backend->persistent_set_capacity = capacity;
    }
    // Freed sets leave holes in earlier pools: start from the first pool each time.
    backend->persistent_pools.current = 0;
    VkDescriptorSet set = allocate_from_pools(backend, &backend->persistent_pools, layout, true, &pool);
    if (set != VK_NULL_HANDLE) {
        s_vulkan_persistent_set &record = backend->persistent_sets[backend->persistent_set_count++];
        record.set = set;
        record.pool = pool;
    }
    return set;
}

extern "C" VkDescriptorSet vulkan_descriptor_set_allocate_textures(s_vulkan_backend *backend, VkDescriptorSetLayout layout)
{
    if (!backend->recording) {
        vulkan_log(backend, "vulkan: transient descriptor sets are allocated within a frame");
        return VK_NULL_HANDLE;
    }
    s_vulkan_descriptor_pools &pools = backend->frames[backend->frame_index].texture_pools;
    pools.textures = 1;
    VkDescriptorPool pool = VK_NULL_HANDLE;
    return allocate_from_pools(backend, &pools, layout, false, &pool);
}

extern "C" VkDescriptorSet vulkan_texture_set_allocate(s_vulkan_backend *backend, VkDescriptorSetLayout layout)
{
    s_vulkan_descriptor_pools &pools = backend->texture_set_pools;
    pools.textures = 1;
    VkDescriptorPool pool = VK_NULL_HANDLE;
    return allocate_from_pools(backend, &pools, layout, false, &pool);
}

// The sets may still be in use by frames in flight: their pools are destroyed with the frame now recorded (or last
// recorded), after its fence, like a retired image.
extern "C" void vulkan_texture_sets_reset(s_vulkan_backend *backend)
{
    s_vulkan_descriptor_pools &pools = backend->texture_set_pools;
    for (uint32_t i = 0; i < pools.count; i++)
        retire(backend, 0, 0, VK_NULL_HANDLE, pools.pools[i]);
    pools.count = pools.current = 0;
}

extern "C" uint32_t vulkan_backend_view_epoch(const s_vulkan_backend *backend)
{
    return backend->view_epoch;
}

extern "C" void vulkan_descriptor_set_free(s_vulkan_backend *backend, VkDescriptorSet set)
{
    for (uint32_t i = 0; i < backend->persistent_set_count; i++) {
        if (backend->persistent_sets[i].set != set)
            continue;
        backend->vk.vkFreeDescriptorSets(backend->device, backend->persistent_sets[i].pool, 1, &set);
        backend->persistent_sets[i] = backend->persistent_sets[--backend->persistent_set_count];
        return;
    }
    vulkan_log(backend, "vulkan: freeing a descriptor set that is not a persistent one");
}

static void write_descriptor(s_vulkan_backend *backend, VkDescriptorSet set, uint32_t binding, uint32_t array_element,
                             uint32_t descriptor_type, const VkDescriptorBufferInfo *buffer,
                             const VkDescriptorImageInfo *image, const VkBufferView *texel_buffer)
{
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set;
    write.dstBinding = binding;
    write.dstArrayElement = array_element;
    write.descriptorCount = 1;
    write.descriptorType = (VkDescriptorType)descriptor_type;
    write.pBufferInfo = buffer;
    write.pImageInfo = image;
    write.pTexelBufferView = texel_buffer;
    backend->vk.vkUpdateDescriptorSets(backend->device, 1, &write, 0, 0);
}

extern "C" void vulkan_descriptor_write_buffer(s_vulkan_backend *backend, VkDescriptorSet set, uint32_t binding,
                                               uint32_t array_element, uint32_t descriptor_type,
                                               s_vulkan_buffer *buffer, uint32_t offset, uint32_t range)
{
    VkDescriptorBufferInfo info = {};
    info.buffer = buffer->buffer;
    info.offset = offset;
    info.range = range != 0 ? (VkDeviceSize)range : VK_WHOLE_SIZE;
    write_descriptor(backend, set, binding, array_element, descriptor_type, &info, 0, 0);
}

extern "C" void vulkan_descriptor_write_image(s_vulkan_backend *backend, VkDescriptorSet set, uint32_t binding,
                                              uint32_t array_element, uint32_t descriptor_type, VkImageView view)
{
    VkDescriptorImageInfo info = {};
    info.imageView = view;
    info.imageLayout = descriptor_type == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ? VK_IMAGE_LAYOUT_GENERAL
                                                                           : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    write_descriptor(backend, set, binding, array_element, descriptor_type, 0, &info, 0);
}

extern "C" void vulkan_descriptor_write_sampler(s_vulkan_backend *backend, VkDescriptorSet set, uint32_t binding,
                                                uint32_t array_element, VkSampler sampler)
{
    VkDescriptorImageInfo info = {};
    info.sampler = sampler;
    write_descriptor(backend, set, binding, array_element, VK_DESCRIPTOR_TYPE_SAMPLER, 0, &info, 0);
}

static void bindless_invalidate(s_vulkan_backend *backend)
{
    const uint64_t revision = ++backend->bindless_revision;
    // Disable token hits if the 32-bit epoch is exhausted.
    backend->bindless_epoch = revision <= UINT32_MAX ? (uint32_t)revision : 0u;
}

bool vulkan_bindless_create(s_vulkan_backend *backend)
{
    s_vulkan_bindless &bindless = backend->bindless;
    if (bindless.set != VK_NULL_HANDLE)
        return true;
    VkDescriptorSetLayoutBinding bindings[5] = {};
    VkDescriptorBindingFlags flags[5];
    for (uint32_t i = 0; i < 5; ++i) {
        bindings[i] = { i, i == 4 ? VK_DESCRIPTOR_TYPE_SAMPLER : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                        k_xe_v6_slots[i], k_vulkan_xenos_push_stages, 0 };
        flags[i] = VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT | VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                   VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
    }
    const VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO, 0, 5, flags
    };
    const VkDescriptorSetLayoutCreateInfo layout = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, &binding_flags,
        VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT, 5, bindings
    };
    const VkDescriptorPoolSize sizes[] = {
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, XE_V6_IMAGES }, { VK_DESCRIPTOR_TYPE_SAMPLER, XE_V6_SAMPLERS },
    };
    const VkDescriptorPoolCreateInfo pool = {
        VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO, 0, VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT, 1, 2, sizes
    };
    auto &vk = backend->vk;
    VkDescriptorSetLayoutSupport support = {};
    support.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_SUPPORT;
    vk.vkGetDescriptorSetLayoutSupport(backend->device, &layout, &support);
    if (!support.supported) {
        vulkan_log(backend, "vulkan: the ABI 6 bindless descriptor layout is unsupported");
        return false;
    }
    for (uint32_t i = 0; i < 5; ++i) {
        auto &b = bindless.bindings[i];
        b.slots = (s_vulkan_bindless_slot *)calloc(k_xe_v6_slots[i], sizeof(*b.slots));
        b.buckets = (uint16_t *)calloc(k_xe_v6_slots[i], sizeof(*b.buckets));
        b.next_unused = 1;
        if (!b.slots || !b.buckets) {
            vulkan_bindless_destroy(backend);
            return false;
        }
    }
    VkResult result = vk.vkCreateDescriptorSetLayout(backend->device, &layout, 0, &bindless.layout);
    if (result == VK_SUCCESS)
        result = vk.vkCreateDescriptorPool(backend->device, &pool, 0, &bindless.pool);
    if (result == VK_SUCCESS) {
        const VkDescriptorSetAllocateInfo allocate = {
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, 0, bindless.pool, 1, &bindless.layout
        };
        result = vk.vkAllocateDescriptorSets(backend->device, &allocate, &bindless.set);
    }
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: ABI 6 bindless descriptor setup failed: %s", vulkan_result_string(result));
        vulkan_bindless_destroy(backend);
    } else {
        bindless_invalidate(backend);
    }
    return result == VK_SUCCESS;
}

void vulkan_bindless_destroy(s_vulkan_backend *backend)
{
    s_vulkan_bindless &bindless = backend->bindless;
    if (bindless.set != VK_NULL_HANDLE)
        bindless_invalidate(backend);
    if (bindless.pool != VK_NULL_HANDLE)
        backend->vk.vkDestroyDescriptorPool(backend->device, bindless.pool, 0);
    if (bindless.layout != VK_NULL_HANDLE)
        backend->vk.vkDestroyDescriptorSetLayout(backend->device, bindless.layout, 0);
    for (auto &binding : bindless.bindings) {
        free(binding.slots);
        free(binding.buckets);
    }
    bindless = {};
}

static void bindless_write(s_vulkan_backend *backend, uint32_t binding, uint32_t slot, uint64_t identity)
{
    using vulkan_gpu_detail::handle_cast;
    const VkDescriptorSet set = backend->bindless.set;
    if (binding == 4)
        vulkan_descriptor_write_sampler(backend, set, binding, slot, handle_cast<VkSampler>(identity));
    else
        vulkan_descriptor_write_image(backend, set, binding, slot, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                                      handle_cast<VkImageView>(identity));
}

void vulkan_bindless_set_dummy(s_vulkan_backend *backend, uint32_t binding, uint64_t identity)
{
    backend->bindless.bindings[binding].slots[0].identity = identity;
    bindless_write(backend, binding, 0, identity);
}

static uint16_t *bindless_find(s_vulkan_bindless_binding &table, uint32_t capacity, uint64_t identity)
{
    const uint32_t hash = (uint32_t)((identity ^ (identity >> 32)) * 0x9e3779b1u);
    const uint32_t bucket = (hash ^ (hash >> 16)) & (capacity - 1u);
    uint16_t *link = &table.buckets[bucket];
    while (*link && table.slots[*link].identity != identity)
        link = &table.slots[*link].next;
    return link;
}

static void bindless_forget(s_vulkan_backend *backend, uint32_t binding, uint64_t identity)
{
    auto &table = backend->bindless.bindings[binding];
    if (!table.slots || !identity)
        return;
    uint16_t *link = bindless_find(table, k_xe_v6_slots[binding], identity);
    if (const uint16_t slot = *link) {
        *link = table.slots[slot].next;
        table.slots[slot].identity = 0;
        bindless_invalidate(backend);
        // Retired images are destroyed after the fence for their last use.
        bindless_write(backend, binding, slot, table.slots[0].identity);
    }
}

uint32_t vulkan_bindless_slot(s_vulkan_backend *backend, uint32_t binding, uint64_t identity)
{
    auto &table = backend->bindless.bindings[binding];
    if (!identity || identity == table.slots[0].identity)
        return 0;
    const uint32_t capacity = k_xe_v6_slots[binding];
    if (const uint16_t slot = *bindless_find(table, capacity, identity)) {
        table.slots[slot].last_use = backend->bindless_serial;
        return slot;
    }
    uint32_t slot = 0;
    if (table.next_unused < capacity) {
        slot = table.next_unused++;
    } else {
        for (uint32_t i = 1; i < capacity; ++i) {
            if (table.slots[i].last_use < backend->bindless_serial &&
                (slot == 0 || !table.slots[i].identity || table.slots[i].last_use < table.slots[slot].last_use)) {
                slot = i;
                if (!table.slots[i].identity)
                    break;
            }
        }
        if (!slot)
            return UINT32_MAX;
        if (table.slots[slot].last_use > backend->bindless_completed) {
            // The queue is ordered. Drain only if the oldest usable slot is still pending.
            if (backend->vk.vkDeviceWaitIdle(backend->device) != VK_SUCCESS)
                return UINT32_MAX;
            backend->bindless_completed = backend->bindless_submitted;
        }
        if (const uint64_t old = table.slots[slot].identity) {
            uint16_t *link = bindless_find(table, capacity, old);
            *link = table.slots[slot].next;
            bindless_invalidate(backend);
        }
    }
    table.slots[slot] = { identity, backend->bindless_serial, 0 };
    *bindless_find(table, capacity, identity) = (uint16_t)slot;
    bindless_write(backend, binding, slot, identity);
    return slot;
}

extern "C" void vulkan_descriptor_write_textures(s_vulkan_backend *backend, VkDescriptorSet set, uint32_t count,
                                                 const uint32_t *binding, const uint64_t *handle,
                                                 uint32_t first_sampler)
{
    // each entry written before use (not zeroed whole on every call: -ftrivial-auto-var-init)
    [[clang::uninitialized]] VkWriteDescriptorSet writes[96];
    [[clang::uninitialized]] VkDescriptorImageInfo infos[96];
    while (count != 0) {
        const uint32_t n = count < 96u ? count : 96u;
        for (uint32_t i = 0; i < n; i++) {
            const bool sampler = binding[i] >= first_sampler;
            VkDescriptorImageInfo &info = infos[i];
            info.sampler = sampler ? (VkSampler)handle[i] : VK_NULL_HANDLE;
            info.imageView = sampler ? VK_NULL_HANDLE : (VkImageView)handle[i];
            info.imageLayout = sampler ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            VkWriteDescriptorSet &write = writes[i];
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.pNext = 0;
            write.dstSet = set;
            write.dstBinding = binding[i];
            write.dstArrayElement = 0;
            write.descriptorCount = 1;
            write.descriptorType = sampler ? VK_DESCRIPTOR_TYPE_SAMPLER : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            write.pImageInfo = &info;
            write.pBufferInfo = 0;
            write.pTexelBufferView = 0;
        }
        backend->vk.vkUpdateDescriptorSets(backend->device, n, writes, 0, 0);
        binding += n;
        handle += n;
        count -= n;
    }
}

extern "C" void vulkan_descriptor_write_texel_buffer(s_vulkan_backend *backend, VkDescriptorSet set, uint32_t binding,
                                                     uint32_t array_element, uint32_t descriptor_type,
                                                     VkBufferView view)
{
    write_descriptor(backend, set, binding, array_element, descriptor_type, 0, 0, &view);
}

// ---- frames ----

// ---- occlusion queries ----

// The open query takes a slot in the current rendering (queries begun in a rendering must end in it).
static void occlusion_resume(s_vulkan_backend *backend, VkCommandBuffer command_buffer)
{
    s_vulkan_frame &frame = backend->frames[backend->frame_index];
    if (backend->occlusion_active == 0 || backend->occlusion_running != 0 || frame.occlusion_pool == VK_NULL_HANDLE)
        return;
    s_vulkan_occlusion_record &record = frame.records[backend->occlusion_active - 1u];
    if (frame.occlusion_slots >= VULKAN_OCCLUSION_SLOTS || record.slot_count >= VULKAN_OCCLUSION_RECORD_SLOTS)
        return; // the rest of its draws go uncounted
    const uint32_t slot = frame.occlusion_slots++;
    record.slots[record.slot_count++] = slot;
    backend->vk.vkCmdBeginQuery(command_buffer, frame.occlusion_pool, slot, backend->occlusion_precise ? 1u : 0u);
    backend->occlusion_running = slot + 1u;
}

static void occlusion_suspend(s_vulkan_backend *backend, VkCommandBuffer command_buffer)
{
    if (backend->occlusion_running == 0)
        return;
    backend->vk.vkCmdEndQuery(command_buffer, backend->frames[backend->frame_index].occlusion_pool,
                              backend->occlusion_running - 1u);
    backend->occlusion_running = 0;
}

// Sums a record's slots of a submitted frame; 0 while any is not available yet.
static int occlusion_read(s_vulkan_backend *backend, s_vulkan_frame &frame, const s_vulkan_occlusion_record &record,
                          uint32_t wait, uint64_t *out_samples)
{
    uint64_t total = 0;
    for (uint32_t i = 0; i < record.slot_count; i++) {
        uint64_t value[2] = { 0, 0 };
        const VkResult result = backend->vk.vkGetQueryPoolResults(
            backend->device, frame.occlusion_pool, record.slots[i], 1, sizeof(value), value, sizeof(value),
            VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WITH_AVAILABILITY_BIT | (wait ? VK_QUERY_RESULT_WAIT_BIT : 0u));
        if (result != VK_SUCCESS && result != VK_NOT_READY)
            return 0;
        if (value[1] == 0)
            return 0;
        total += value[0];
    }
    *out_samples = total;
    return 1;
}

extern "C" int vulkan_occlusion_begin(s_vulkan_backend *backend, uint32_t *out_serial, uint32_t *out_record)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_occlusion_begin");
    s_vulkan_frame &frame = backend->frames[backend->frame_index];
    if (command_buffer == 0 || frame.occlusion_pool == VK_NULL_HANDLE || frame.occlusion_records >= VULKAN_OCCLUSION_RECORDS)
        return 0;
    vulkan_occlusion_end(backend);
    const uint32_t index = frame.occlusion_records++;
    frame.records[index].slot_count = 0;
    backend->occlusion_active = index + 1u;
    if (backend->rendering)
        occlusion_resume(backend, command_buffer);
    *out_serial = frame.serial;
    *out_record = index;
    return 1;
}

extern "C" void vulkan_occlusion_end(s_vulkan_backend *backend)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_occlusion_end");
    if (command_buffer != 0)
        occlusion_suspend(backend, command_buffer);
    backend->occlusion_active = 0;
}

extern "C" int vulkan_occlusion_result(s_vulkan_backend *backend, uint32_t serial, uint32_t record,
                                       uint64_t *out_samples)
{
    *out_samples = 0;
    for (uint32_t i = 0; i < backend->frame_count; i++) {
        s_vulkan_frame &frame = backend->frames[i];
        if (frame.results_serial == serial && frame.serial != serial)
            return record < frame.result_count ? (*out_samples = frame.results[record], 1) : 1;
        if (frame.serial != serial)
            continue;
        if (backend->recording && i == backend->frame_index)
            return 0; // still being recorded
        return record < frame.occlusion_records ? occlusion_read(backend, frame, frame.records[record], 0, out_samples) : 1;
    }
    return 1; // older than every frame slot: nothing to report
}

extern "C" VkResult vulkan_frame_begin(s_vulkan_backend *backend)
{
    s_vulkan_functions &vk = backend->vk;
    if (backend->recording) {
        vulkan_log(backend, "vulkan: vulkan_frame_begin inside a frame");
        return VK_ERROR_UNKNOWN;
    }
    backend->frame_index = (backend->frame_index + 1) % backend->frame_count;
    s_vulkan_frame &frame = backend->frames[backend->frame_index];
    VkResult result = vk.vkWaitForFences(backend->device, 1, &frame.fence, VK_TRUE, ~0ull);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: waiting for frame %u failed: %s", backend->frame_index, vulkan_result_string(result));
        if (result == VK_ERROR_DEVICE_LOST)
            report_device_loss(backend);
        return result;
    }
    if (frame.bindless_serial > backend->bindless_completed)
        backend->bindless_completed = frame.bindless_serial;
    destroy_retired(backend, frame);
    buffer_pool_trim(backend, false);
    if (frame.occlusion_pool != VK_NULL_HANDLE) {
        // the slot's fence has signalled: read all used occlusion slots in one call before the pool is reset
        if (frame.occlusion_records != 0) {
            [[clang::uninitialized]] uint64_t slot_values[VULKAN_OCCLUSION_SLOTS * 2u];
            const uint32_t sc = frame.occlusion_slots <= VULKAN_OCCLUSION_SLOTS ? frame.occlusion_slots
                                                                                 : VULKAN_OCCLUSION_SLOTS;
            bool batch_ok = false;
            if (sc != 0) {
                const VkResult qr = vk.vkGetQueryPoolResults(
                    backend->device, frame.occlusion_pool, 0, sc, sc * 2u * sizeof(uint64_t), slot_values,
                    2u * sizeof(uint64_t), VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WITH_AVAILABILITY_BIT);
                batch_ok = (qr == VK_SUCCESS || qr == VK_NOT_READY);
            }
            for (uint32_t r = 0; r < frame.occlusion_records; r++) {
                const s_vulkan_occlusion_record &rec = frame.records[r];
                uint64_t total = 0;
                bool ok = batch_ok || rec.slot_count == 0;
                for (uint32_t i = 0; ok && i < rec.slot_count; i++) {
                    const uint32_t s = rec.slots[i];
                    if (s >= sc || slot_values[s * 2u + 1u] == 0)
                        ok = false;
                    else
                        total += slot_values[s * 2u];
                }
                if (!ok && !occlusion_read(backend, frame, rec, 1, &total))
                    total = 0;
                frame.results[r] = total;
            }
        }
        frame.result_count = frame.occlusion_records;
        frame.results_serial = frame.serial;
    }
    if (frame.timestamps_written) {
        // the slot's fence has signalled: its frame command buffer's GPU time
        if (backend->pass_trace_period == 0) {
            uint64_t stamps[2] = { 0, 0 };
            if (vk.vkGetQueryPoolResults(backend->device, frame.timestamp_pool, 0, 2, sizeof(stamps), stamps,
                                         sizeof(stamps[0]), VK_QUERY_RESULT_64_BIT) == VK_SUCCESS && stamps[1] >= stamps[0])
                backend->gpu_frame_ms = (float)((double)(stamps[1] - stamps[0]) * backend->timestamp_period / 1.0e6);
        } else {
            const uint32_t qcount = frame.pass_trace_next_query >= 2u && frame.pass_trace_next_query <= VULKAN_GPU_PASS_TRACE_QUERIES
                                        ? frame.pass_trace_next_query : 2u;
            [[clang::uninitialized]] uint64_t stamps[VULKAN_GPU_PASS_TRACE_QUERIES];
            memset(stamps, 0, sizeof(stamps[0]) * qcount);
            if (vk.vkGetQueryPoolResults(backend->device, frame.timestamp_pool, 0, qcount, sizeof(stamps[0]) * qcount,
                                         stamps, sizeof(stamps[0]), VK_QUERY_RESULT_64_BIT) == VK_SUCCESS) {
                if (stamps[1] >= stamps[0])
                    backend->gpu_frame_ms = (float)((double)(stamps[1] - stamps[0]) * backend->timestamp_period / 1.0e6);
                if (frame.serial != 0 && (frame.serial % backend->pass_trace_period) == 0 &&
                    frame.pass_trace_spans != 0 && frame.pass_trace_span_count != 0) {
                    const double tick_us = (double)backend->timestamp_period / 1.0e3;
                    const uint32_t total_us = stamps[1] >= stamps[0] ? (uint32_t)((double)(stamps[1] - stamps[0]) * tick_us + 0.5) : 0u;
                    [[clang::uninitialized]] uint32_t span_us[VULKAN_GPU_PASS_TRACE_MAX_SPANS];
                    [[clang::uninitialized]] uint32_t order[VULKAN_GPU_PASS_TRACE_MAX_SPANS];
                    uint32_t sum_us = 0;
                    const uint32_t n = frame.pass_trace_span_count;
                    for (uint32_t i = 0; i < n; i++) {
                        const s_vulkan_gpu_pass_span &sp = frame.pass_trace_spans[i];
                        uint32_t us = 0;
                        if (sp.q_end < qcount && sp.q_begin < qcount && stamps[sp.q_end] >= stamps[sp.q_begin])
                            us = (uint32_t)((double)(stamps[sp.q_end] - stamps[sp.q_begin]) * tick_us + 0.5);
                        span_us[i] = us;
                        order[i] = i;
                        sum_us += us;
                    }
                    vulkan_log(backend, "gpass: frame %u total=%u.%03ums sum_spans=%u.%03ums spans=%u",
                               frame.serial, total_us / 1000u, total_us % 1000u, sum_us / 1000u, sum_us % 1000u, n);
                    for (uint32_t i = 0; i < n; i++) {
                        const s_vulkan_gpu_pass_span &sp = frame.pass_trace_spans[i];
                        vulkan_log(backend, "gpass:   #%02u %22s %4ux%4u fmt=%3u draws=%4u %6u us",
                                   i, sp.kind ? sp.kind : "?", sp.w, sp.h, sp.fmt, sp.draws, span_us[i]);
                    }
                    // Top 10 heaviest spans (insertion sort on indices)
                    for (uint32_t i = 1; i < n; i++) {
                        const uint32_t key = order[i];
                        uint32_t j = i;
                        while (j > 0 && span_us[order[j - 1u]] < span_us[key]) {
                            order[j] = order[j - 1u];
                            j--;
                        }
                        order[j] = key;
                    }
                    const uint32_t top = n < 10u ? n : 10u;
                    vulkan_log(backend, "gpass: top %u:", top);
                    for (uint32_t k = 0; k < top; k++) {
                        const uint32_t idx = order[k];
                        const s_vulkan_gpu_pass_span &sp = frame.pass_trace_spans[idx];
                        vulkan_log(backend, "gpass:   top%u #%02u %22s %4ux%4u fmt=%3u draws=%4u %6u us",
                                   k + 1u, idx, sp.kind ? sp.kind : "?", sp.w, sp.h, sp.fmt, sp.draws, span_us[idx]);
                    }
                }
            }
        }
        frame.timestamps_written = 0;
    }
    vk.vkResetFences(backend->device, 1, &frame.fence);
    vk.vkResetCommandPool(backend->device, frame.command_pool, 0);
    frame.upload_recording = 0;
    frame.staging_used = 0; // the slot's fence has signalled: its uploads have run
    for (uint32_t i = 0; i < frame.transient_pools.count; i++)
        vk.vkResetDescriptorPool(backend->device, frame.transient_pools.pools[i], 0);
    frame.transient_pools.current = 0;
    for (uint32_t i = 0; i < frame.texture_pools.count; i++)
        vk.vkResetDescriptorPool(backend->device, frame.texture_pools.pools[i], 0);
    frame.texture_pools.current = 0;
    frame.texture_pools.textures = 1;
    frame.acquired_image = VULKAN_NO_IMAGE;
    VkCommandBufferBeginInfo begin = {};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    result = vk.vkBeginCommandBuffer(frame.command_buffer, &begin);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: vkBeginCommandBuffer failed: %s", vulkan_result_string(result));
        return result;
    }
    backend->recording = 1;
    backend->command_buffer = frame.command_buffer;
    backend->rendering_dirty = 0;
    backend->rendering_color_count = 0;
    backend->rendering_depth = 0;
    backend->untracked_commands = false;
    invalidate_command_state(backend);
    frame.serial = ++backend->frame_serial;
    frame.bindless_serial = ++backend->bindless_serial;
    const uint32_t occ_reset = frame.occlusion_reset_count > frame.occlusion_slots ? frame.occlusion_reset_count
                                                                                    : frame.occlusion_slots;
    frame.occlusion_reset_count = 0;
    frame.occlusion_slots = 0;
    frame.occlusion_records = 0;
    backend->occlusion_active = 0;
    backend->occlusion_running = 0;
    frame.pass_trace_next_query = 2u;
    frame.pass_trace_span_count = 0;
    backend->pass_trace_active_span = 0;
    if (frame.occlusion_pool != VK_NULL_HANDLE && occ_reset != 0)
        vk.vkCmdResetQueryPool(frame.command_buffer, frame.occlusion_pool, 0, occ_reset);
    if (frame.timestamp_pool != VK_NULL_HANDLE) {
        const uint32_t ts_count =
            (backend->pass_trace_period != 0 && (frame.serial % backend->pass_trace_period) == 0)
                ? VULKAN_GPU_PASS_TRACE_QUERIES
                : 2u;
        vk.vkCmdResetQueryPool(frame.command_buffer, frame.timestamp_pool, 0, ts_count);
        vk.vkCmdWriteTimestamp(frame.command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, frame.timestamp_pool, 0);
    }
    return VK_SUCCESS;
}

extern "C" VkResult vulkan_frame_end(s_vulkan_backend *backend, uint32_t wait)
{
    s_vulkan_functions &vk = backend->vk;
    if (!backend->recording) {
        vulkan_log(backend, "vulkan: vulkan_frame_end outside a frame");
        return VK_ERROR_UNKNOWN;
    }
    s_vulkan_frame &frame = backend->frames[backend->frame_index];
    if (backend->rendering)
        vulkan_cmd_end_rendering(backend);
    vulkan_cmd_flush_memexports(backend);
    if (frame.timestamp_pool != VK_NULL_HANDLE) {
        vk.vkCmdWriteTimestamp(frame.command_buffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, frame.timestamp_pool, 1);
        frame.timestamps_written = 1;
    }
    backend->recording = 0;
    backend->command_buffer = 0;
    VkResult result = vk.vkEndCommandBuffer(frame.command_buffer);
    VkSemaphore wait_semaphore = VK_NULL_HANDLE, signal_semaphore = VK_NULL_HANDLE;
    if (frame.acquired_image != VULKAN_NO_IMAGE) {
        wait_semaphore = frame.acquire_semaphore;
        signal_semaphore = backend->present_semaphores[frame.acquired_image];
    }
    const VkCommandBuffer uploads = upload_end(backend, frame);
    if (result == VK_SUCCESS)
        result = queue_submit(backend, frame.command_buffer, wait_semaphore, signal_semaphore, frame.fence, uploads);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: frame submission failed: %s", vulkan_result_string(result));
        if (result == VK_ERROR_DEVICE_LOST)
            report_device_loss(backend);
        // Leave the fence signaled so the slot can be begun again.
        VkFenceCreateInfo create = {};
        create.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        create.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        vk.vkDestroyFence(backend->device, frame.fence, 0);
        vk.vkCreateFence(backend->device, &create, 0, &frame.fence);
        return result;
    }
    backend->bindless_submitted = frame.bindless_serial;
    backend->present_image = frame.acquired_image;
    if (wait) {
        result = vk.vkWaitForFences(backend->device, 1, &frame.fence, VK_TRUE, ~0ull);
        if (result != VK_SUCCESS)
            vulkan_log(backend, "vulkan: waiting for the frame failed: %s", vulkan_result_string(result));
        else
            backend->bindless_completed = frame.bindless_serial;
    }
    return result;
}

extern "C" VkCommandBuffer vulkan_frame_command_buffer(const s_vulkan_backend *backend)
{
    // The caller can retain the handle and record commands (including vkCmdExecuteCommands) at any point this frame.
    backend->untracked_commands = backend->command_buffer != 0;
    return backend->command_buffer;
}

// The slow recording path logs commands outside a frame or records a checkpoint.
VkCommandBuffer vulkan_recording::command_buffer_diagnostics(s_vulkan_backend *backend, const char *caller)
{
    VkCommandBuffer command_buffer = backend->command_buffer;
    if (__builtin_expect(command_buffer == 0, 0)) {
        vulkan_log(backend, "vulkan: %s outside vulkan_frame_begin / vulkan_frame_end", caller);
        return 0;
    }
    command_checkpoint(backend, command_buffer, caller);
    return command_buffer;
}

// ---- diagnostics ----

extern "C" void vulkan_cmd_checkpoint(s_vulkan_backend *backend, const void *marker)
{
    if (!backend->checkpoints || !backend->recording)
        return;
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_checkpoint");
    if (command_buffer != 0) {
        backend->command_markers[backend->command_serial & 8191u] = marker;
        backend->vk.vkCmdSetCheckpointNV(command_buffer, marker);
    }
}

extern "C" void vulkan_backend_set_checkpoint_describe(s_vulkan_backend *backend,
                                                       vulkan_checkpoint_describe_function describe, void *context)
{
    backend->checkpoint_describe = describe;
    backend->checkpoint_context = context;
}

// After a device loss (once): the last checkpoints the queue reached, per pipeline stage.
static void report_device_loss(s_vulkan_backend *backend)
{
    if (!backend->checkpoints || backend->loss_reported)
        return;
    backend->loss_reported = 1;
    uint32_t count = 0;
    backend->vk.vkGetQueueCheckpointDataNV(backend->queue, &count, 0);
    VkCheckpointDataNV *data = (VkCheckpointDataNV *)calloc(count + 1, sizeof(VkCheckpointDataNV));
    if (data == 0)
        return;
    for (uint32_t i = 0; i < count; i++)
        data[i].sType = VK_STRUCTURE_TYPE_CHECKPOINT_DATA_NV;
    backend->vk.vkGetQueueCheckpointDataNV(backend->queue, &count, data);
    vulkan_log(backend, "vulkan: device lost; %u checkpoints reached:", count);
    for (uint32_t i = 0; i < count; i++) {
        vulkan_log(backend, "vulkan:   stage 0x%x marker %p", (uint32_t)data[i].stage, data[i].pCheckpointMarker);
        const uint32_t marker = (uint32_t)(size_t)data[i].pCheckpointMarker;
        if ((marker & 0x80000000u) != 0) {
            const uint32_t serial = marker & 0x7fffffffu;
            vulkan_log(backend, "vulkan:     backend command %u: %s (latest %u)", serial,
                       backend->command_serial - serial < 8192u ? backend->command_names[serial & 8191u] : "?",
                       backend->command_serial);
            for (uint32_t next = serial + 1u; next <= serial + 4u && next <= backend->command_serial; next++)
                vulkan_log(backend, "vulkan:     then backend command %u: %s", next,
                           backend->command_serial - next < 8192u ? backend->command_names[next & 8191u] : "?");
            // the caller's last checkpoint at or before it: the work this command belongs to
            for (uint32_t back = serial; back + 64u > serial && back > 0 && backend->command_serial - back < 8192u; back--) {
                if (backend->command_names[back & 8191u] != 0 &&
                    strcmp(backend->command_names[back & 8191u], "vulkan_cmd_checkpoint") == 0) {
                    if (backend->checkpoint_describe != 0)
                        backend->checkpoint_describe(backend->checkpoint_context, backend->command_markers[back & 8191u]);
                    break;
                }
            }
            continue;
        }
        if (backend->checkpoint_describe != 0)
            backend->checkpoint_describe(backend->checkpoint_context, data[i].pCheckpointMarker);
    }
    free(data);
}

// ---- render targets ----

extern "C" s_vulkan_render_target *vulkan_render_target_create(s_vulkan_backend *backend, uint32_t width,
                                                               uint32_t height, uint32_t color_format,
                                                               uint32_t flags)
{
    uint32_t block_width, block_height, block_bytes;
    const bool with_depth = (flags & k_vulkan_render_target_depth) != 0;
    if (!vulkan_format_block_info(color_format, &block_width, &block_height, &block_bytes) || block_width != 1 ||
        format_aspect(color_format) != VK_IMAGE_ASPECT_COLOR_BIT) {
        vulkan_log(backend, "vulkan: render target: format %u is not a color format", color_format);
        return 0;
    }
    if (with_depth && backend->depth_format == VK_FORMAT_UNDEFINED) {
        vulkan_log(backend, "vulkan: render target: the device has no depth/stencil format");
        return 0;
    }
    s_vulkan_render_target *target = (s_vulkan_render_target *)calloc(1, sizeof(s_vulkan_render_target));
    if (target == 0)
        return 0;
    target->width = width;
    target->height = height;
    target->readback_row_pitch = width * block_bytes;
    s_vulkan_image_desc desc = {};
    desc.view_type = VK_IMAGE_VIEW_TYPE_2D;
    desc.format = color_format;
    desc.width = width;
    desc.height = height;
    desc.depth = 1;
    desc.mip_levels = 1;
    desc.array_layers = 1;
    desc.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                 VK_IMAGE_USAGE_SAMPLED_BIT;
    if ((flags & k_vulkan_render_target_storage) != 0)
        desc.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    target->color = vulkan_image_create(backend, &desc);
    if (target->color != 0 && with_depth) {
        desc.format = backend->depth_format;
        desc.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        target->depth = vulkan_image_create(backend, &desc);
    }
    target->readback = vulkan_buffer_create(backend, target->readback_row_pitch * height, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                            k_vulkan_memory_readback);
    if (target->color == 0 || (with_depth && target->depth == 0) || target->readback == 0) {
        vulkan_render_target_destroy(backend, target);
        return 0;
    }
    return target;
}

extern "C" void vulkan_render_target_destroy(s_vulkan_backend *backend, s_vulkan_render_target *target)
{
    if (target == 0)
        return;
    vulkan_image_destroy(backend, target->color);
    vulkan_image_destroy(backend, target->depth);
    vulkan_buffer_destroy(backend, target->readback);
    free(target);
}

extern "C" void vulkan_render_target_get_info(const s_vulkan_render_target *target, s_vulkan_render_target_info *out)
{
    out->width = target->width;
    out->height = target->height;
    out->color_format = target->color->desc.format;
    out->depth_format = target->depth != 0 ? target->depth->desc.format : (uint32_t)VK_FORMAT_UNDEFINED;
    out->readback_row_pitch = target->readback_row_pitch;
}

extern "C" VkImageView vulkan_render_target_color_view(const s_vulkan_render_target *target)
{
    return target->color->view;
}

extern "C" const void *vulkan_render_target_read(s_vulkan_backend *backend, s_vulkan_render_target *target)
{
    vulkan_buffer_invalidate(backend, target->readback);
    return target->readback->mapped;
}

static size_t format_decimal(char *out, uint32_t value)
{
    char digits[12];
    size_t count = 0;
    do
        digits[count++] = (char)('0' + value % 10);
    while ((value /= 10) != 0);
    for (size_t i = 0; i < count; i++)
        out[i] = digits[count - 1 - i];
    return count;
}

extern "C" int vulkan_write_ppm(const char *path, const void *pixels, uint32_t width, uint32_t height,
                                uint32_t row_pitch, uint32_t format)
{
    bool bgra = format >= VK_FORMAT_B8G8R8A8_UNORM && format <= VK_FORMAT_B8G8R8A8_SRGB;
    bool rgba = format >= VK_FORMAT_R8G8B8A8_UNORM && format <= VK_FORMAT_R8G8B8A8_SRGB;
    if (!bgra && !rgba)
        return 0;
    FILE *file = fopen(path, "wb");
    if (file == 0)
        return 0;
    char header[48] = "P6\n";
    size_t length = 3;
    length += format_decimal(header + length, width);
    header[length++] = ' ';
    length += format_decimal(header + length, height);
    memcpy(header + length, "\n255\n", 5);
    length += 5;
    uint8_t *row = (uint8_t *)malloc(width * 3u + 1u);
    bool ok = row != 0 && fwrite(header, 1, length, file) == length;
    for (uint32_t y = 0; ok && y < height; y++) {
        const uint8_t *source = (const uint8_t *)pixels + (size_t)y * row_pitch;
        for (uint32_t x = 0; x < width; x++) {
            row[x * 3 + 0] = source[x * 4 + (bgra ? 2 : 0)];
            row[x * 3 + 1] = source[x * 4 + 1];
            row[x * 3 + 2] = source[x * 4 + (bgra ? 0 : 2)];
        }
        ok = fwrite(row, 1, width * 3u, file) == width * 3u;
    }
    free(row);
    return fclose(file) == 0 && ok;
}

// ---- commands ----

extern "C" void vulkan_cmd_set_viewport(s_vulkan_backend *backend, float x, float y, float width, float height,
                                        float min_depth, float max_depth)
{
    vulkan_cmd_set_viewport_inline(backend, x, y, width, height, min_depth, max_depth);
}

extern "C" void vulkan_cmd_set_scissor(s_vulkan_backend *backend, int32_t x, int32_t y, uint32_t width, uint32_t height)
{
    vulkan_cmd_set_scissor_inline(backend, x, y, width, height);
}

static uint32_t pass_trace_begin(s_vulkan_backend *backend, VkCommandBuffer command_buffer, const char *kind,
                                 uint32_t w, uint32_t h, uint32_t fmt)
{
    if (__builtin_expect(backend->pass_trace_period == 0, 1))
        return 0;
    s_vulkan_frame &frame = backend->frames[backend->frame_index];
    if ((frame.serial % backend->pass_trace_period) != 0)
        return 0;
    if (frame.timestamp_pool == VK_NULL_HANDLE || frame.pass_trace_spans == 0 ||
        frame.pass_trace_span_count >= VULKAN_GPU_PASS_TRACE_MAX_SPANS ||
        frame.pass_trace_next_query + 2u > VULKAN_GPU_PASS_TRACE_QUERIES)
        return 0;
    const uint32_t idx = frame.pass_trace_span_count++;
    s_vulkan_gpu_pass_span &sp = frame.pass_trace_spans[idx];
    sp.kind = kind;
    sp.w = w;
    sp.h = h;
    sp.fmt = fmt;
    sp.draws = 0;
    sp.q_begin = frame.pass_trace_next_query++;
    sp.q_end = frame.pass_trace_next_query++;
    backend->vk.vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, frame.timestamp_pool, sp.q_begin);
    return idx + 1u;
}

static void pass_trace_end(s_vulkan_backend *backend, VkCommandBuffer command_buffer, uint32_t span_handle)
{
    if (__builtin_expect(span_handle == 0, 1))
        return;
    s_vulkan_frame &frame = backend->frames[backend->frame_index];
    if (frame.timestamp_pool == VK_NULL_HANDLE || frame.pass_trace_spans == 0 ||
        span_handle > frame.pass_trace_span_count)
        return;
    const s_vulkan_gpu_pass_span &sp = frame.pass_trace_spans[span_handle - 1u];
    backend->vk.vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, frame.timestamp_pool, sp.q_end);
}

extern "C" void vulkan_cmd_begin_rendering(s_vulkan_backend *backend, s_vulkan_render_target *target,
                                           const s_vulkan_clear *clear)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_begin_rendering");
    if (command_buffer == 0)
        return;
    if (backend->rendering)
        vulkan_cmd_end_rendering(backend);
    vulkan_cmd_flush_memexports(backend);
    uint32_t flags = clear != 0 ? clear->flags : 0u;
    bool has_stencil = false;
    bool depth_discard = false;
    if (target->depth != 0) {
        uint32_t depth_stencil_clear = flags & (k_vulkan_clear_depth | k_vulkan_clear_stencil);
        has_stencil = (target->depth->aspect & VK_IMAGE_ASPECT_STENCIL_BIT) != 0;
        depth_discard = depth_stencil_clear == (has_stencil ? (uint32_t)(k_vulkan_clear_depth | k_vulkan_clear_stencil)
                                                            : (uint32_t)k_vulkan_clear_depth);
    }
    // Always a barrier when dirty/changing layout, batched across color + depth into one command.
    [[clang::uninitialized]] s_image_barrier barriers[2];
    uint32_t barrier_count = 0;
    if (transition_barrier(backend, target->color, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                           (flags & k_vulkan_clear_color) != 0, &barriers[barrier_count]))
        barrier_count++;
    if (target->depth != 0 &&
        transition_barrier(backend, target->depth, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, depth_discard,
                           &barriers[barrier_count]))
        barrier_count++;
    if (barrier_count != 0)
        cmd_image_barriers(backend, command_buffer, barriers, barrier_count);

    [[clang::uninitialized]] VkRenderingAttachmentInfo color;
    color.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color.pNext = 0;
    color.imageView = target->color->view;
    color.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color.resolveMode = VK_RESOLVE_MODE_NONE;
    color.resolveImageView = VK_NULL_HANDLE;
    color.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color.loadOp = (flags & k_vulkan_clear_color) != 0 ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    color.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    if (clear != 0)
        memcpy(color.clearValue.color.float32, clear->color, sizeof(clear->color));
    else
        memset(&color.clearValue, 0, sizeof(color.clearValue));

    [[clang::uninitialized]] VkRenderingAttachmentInfo depth, stencil;
    if (target->depth != 0) {
        depth.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depth.pNext = 0;
        depth.imageView = target->depth->view;
        depth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth.resolveMode = VK_RESOLVE_MODE_NONE;
        depth.resolveImageView = VK_NULL_HANDLE;
        depth.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth.loadOp = (flags & k_vulkan_clear_depth) != 0 ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        depth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth.clearValue.depthStencil.depth = clear != 0 ? clear->depth : 1.0f;
        depth.clearValue.depthStencil.stencil = 0;
        if (has_stencil) {
            stencil = depth;
            stencil.loadOp = (flags & k_vulkan_clear_stencil) != 0 ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            stencil.clearValue.depthStencil.stencil = clear != 0 ? clear->stencil : 0u;
        }
    }
    VkRenderingInfo rendering = {};
    rendering.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering.renderArea.extent.width = target->width;
    rendering.renderArea.extent.height = target->height;
    rendering.layerCount = 1;
    rendering.colorAttachmentCount = 1;
    rendering.pColorAttachments = &color;
    rendering.pDepthAttachment = target->depth != 0 ? &depth : 0;
    rendering.pStencilAttachment = has_stencil ? &stencil : 0;
    backend->pass_trace_active_span = pass_trace_begin(backend, command_buffer, "render_target", target->width,
                                                       target->height, target->color->desc.format);
    backend->vk.vkCmdBeginRendering(command_buffer, &rendering);
    backend->rendering = 1;
    ++backend->rendering_epoch;
    // STORE can write even when there are no draws or clears.
    backend->rendering_dirty = 1;
    backend->rendering_colors[0] = target->color;
    backend->rendering_depth = target->depth;
    target->color->attachment_dirty = 1;
    if (target->depth != 0)
        target->depth->attachment_dirty = 1;
    occlusion_resume(backend, command_buffer);
    backend->rendering_width = target->width;
    backend->rendering_height = target->height;
    backend->rendering_color_count = 1;
    backend->rendering_has_depth = target->depth != 0;
    backend->rendering_has_stencil = has_stencil;
    vulkan_cmd_set_viewport(backend, 0.0f, 0.0f, (float)target->width, (float)target->height, 0.0f, 1.0f);
    vulkan_cmd_set_scissor(backend, 0, 0, target->width, target->height);
}

extern "C" void vulkan_cmd_end_rendering(s_vulkan_backend *backend)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_end_rendering");
    if (command_buffer == 0 || !backend->rendering)
        return;
    occlusion_suspend(backend, command_buffer);
    backend->vk.vkCmdEndRendering(command_buffer);
    if (__builtin_expect(backend->pass_trace_active_span != 0, 0)) {
        pass_trace_end(backend, command_buffer, backend->pass_trace_active_span);
        backend->pass_trace_active_span = 0;
    }
    backend->rendering = 0;
    backend->rendering_dirty = 0;
    backend->rendering_color_count = 0;
    backend->rendering_depth = 0;
}

// Single-view rendering boundaries and transfer commands preserve command-buffer bindings and dynamic state.
static void begin_transfer(s_vulkan_backend *backend, bool flush_memexports = true)
{
    if (backend->rendering)
        vulkan_cmd_end_rendering(backend);
    if (flush_memexports)
        vulkan_cmd_flush_memexports(backend);
}

static void cmd_bind_pipeline(s_vulkan_backend *backend, VkCommandBuffer command_buffer,
                              const s_vulkan_pipeline *pipeline, uint32_t point)
{
    if (vulkan_recording::pipeline_is_bound(backend, pipeline, point)) {
        backend->bind_counters.pipelines_skipped++;
        return;
    }
    // Descriptor layout changes are tracked by record_descriptor_sets when sets are actually bound.
    backend->vk.vkCmdBindPipeline(command_buffer, (VkPipelineBindPoint)pipeline->bind_point, pipeline->pipeline);
    backend->bind_counters.pipelines++;
    backend->bound_pipelines[point] = pipeline->pipeline;
    backend->bound_pipeline_layouts[point] = pipeline->layout;
    if (point == 0) {
        // A pipeline bind invalidates the dynamic values that the new pipeline specifies statically.
        if (!pipeline->dynamic)
            backend->dynamic_valid = 0;
        if (!pipeline->dynamic_vertex_stride)
            backend->bound_vertex_mask = 0;
        if (!pipeline->dynamic_viewport_scissor)
            backend->viewport_valid = backend->scissor_valid = 0;
    }
}

void vulkan_recording::bind_pipeline_with_state(s_vulkan_backend *backend, VkCommandBuffer command_buffer,
                                                const s_vulkan_pipeline *pipeline, uint32_t point,
                                                const s_vulkan_dynamic_state *state)
{
    cmd_bind_pipeline(backend, command_buffer, pipeline, point);
    if (pipeline->dynamic)
        set_dynamic_state(backend, command_buffer, state != 0 ? *state : pipeline->dynamic_state);
    if (pipeline->null_binding != 0xffffffffu) {
        if (backend->null_vertex_buffer == 0) {
            backend->null_vertex_buffer = vulkan_buffer_create(backend, XENOS_VERTEX_NULL_BUFFER_SIZE,
                                                               VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, k_vulkan_memory_upload);
            if (backend->null_vertex_buffer == 0)
                return;
            memcpy(backend->null_vertex_buffer->mapped, k_xenos_vertex_null_buffer_data, XENOS_VERTEX_NULL_BUFFER_SIZE);
        }
        vulkan_cmd_bind_vertex_buffer_native(backend, pipeline->null_binding, backend->null_vertex_buffer, 0, 0);
    }
}

extern "C" void vulkan_cmd_bind_pipeline_state(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline,
                                               const s_vulkan_graphics_state *state)
{
    if (pipeline->dynamic) {
        const s_vulkan_dynamic_state dynamic_state = pipeline_dynamic_state(*state);
        vulkan_cmd_bind_pipeline_native(backend, pipeline, &dynamic_state);
    } else {
        vulkan_cmd_bind_pipeline_native(backend, pipeline);
    }
}

extern "C" void vulkan_cmd_bind_pipeline(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline)
{
    vulkan_cmd_bind_pipeline_native(backend, pipeline);
}

// All descriptor binds use this recorder. A coalesced bind has dynamic offsets for its first set only.
void vulkan_recording::record_descriptor_sets(s_vulkan_backend *backend, VkCommandBuffer command_buffer,
                                              uint32_t bind_point, VkPipelineLayout layout, uint32_t first,
                                              uint32_t count, const VkDescriptorSet *sets, uint32_t offset_count,
                                              const uint32_t *offsets)
{
    backend->vk.vkCmdBindDescriptorSets(command_buffer, (VkPipelineBindPoint)bind_point, layout, first, count, sets,
                                        offset_count, offsets);
    backend->bind_counters.sets++;
    s_vulkan_bound_sets &bound = backend->bound_sets[bind_point_index(bind_point)];
    if (bound.layout != layout) {
        bound.valid_mask = 0;
        bound.layout = layout;
    }
    if (first >= VULKAN_MAX_SETS || count > VULKAN_MAX_SETS - first || offset_count > VULKAN_MAX_DYNAMIC_OFFSETS) {
        bound.valid_mask = 0;
        return;
    }
    for (uint32_t i = 0; i < count; i++) {
        const uint32_t index = first + i;
        bound.sets[index] = sets[i];
        bound.offset_count[index] = i == 0 ? offset_count : 0;
        bound.valid_mask |= 1u << index;
    }
    if (offset_count != 0)
        memcpy(bound.offsets[first], offsets, offset_count * sizeof(*offsets));
}

extern "C" void vulkan_cmd_bind_descriptor_set(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline,
                                               uint32_t set_index, VkDescriptorSet set)
{
    vulkan_recording::bind_group(backend, pipeline, set_index, set, 0, 0, "vulkan_cmd_bind_descriptor_set");
}

extern "C" void vulkan_cmd_bind_descriptor_set_dynamic(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline,
                                                       uint32_t set_index, VkDescriptorSet set, uint32_t offset_count,
                                                       const uint32_t *offsets)
{
    vulkan_cmd_bind_group_native(backend, pipeline, set_index, set, offset_count, offsets);
}

extern "C" void vulkan_cmd_bind_descriptor_sets_pair(s_vulkan_backend *backend, const s_vulkan_pipeline *pipeline,
                                                     VkDescriptorSet set0, uint32_t offset_count0,
                                                     const uint32_t *offsets0, VkDescriptorSet set1)
{
    vulkan_cmd_bind_group_pair_native(backend, pipeline, set0, offset_count0, offsets0, set1);
}

extern "C" void vulkan_cmd_bind_vertex_buffer(s_vulkan_backend *backend, uint32_t binding, const s_vulkan_buffer *buffer,
                                              uint32_t offset, uint32_t stride)
{
    vulkan_cmd_bind_vertex_buffer_native(backend, binding, buffer, offset, stride);
}

extern "C" void vulkan_cmd_bind_index_buffer(s_vulkan_backend *backend, const s_vulkan_buffer *buffer, uint32_t offset,
                                             uint32_t index_type)
{
    vulkan_cmd_bind_index_buffer_native(backend, buffer, offset, (VkIndexType)index_type);
}

extern "C" void vulkan_backend_bind_counters(const s_vulkan_backend *backend, s_vulkan_bind_counters *out)
{
    *out = backend->bind_counters;
}

extern "C" void vulkan_cmd_draw(s_vulkan_backend *backend, uint32_t vertex_count, uint32_t instance_count,
                                uint32_t first_vertex, uint32_t first_instance)
{
    vulkan_cmd_draw_inline(backend, vertex_count, instance_count, first_vertex, first_instance);
}

extern "C" void vulkan_cmd_draw_indexed(s_vulkan_backend *backend, uint32_t index_count, uint32_t instance_count,
                                        uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
{
    vulkan_cmd_draw_indexed_inline(backend, index_count, instance_count, first_index, vertex_offset, first_instance);
}

void vulkan_recording::dispatch_traced(s_vulkan_backend *backend, VkCommandBuffer command_buffer, uint32_t groups_x,
                                       uint32_t groups_y, uint32_t groups_z)
{
    const uint32_t span = pass_trace_begin(backend, command_buffer, "dispatch", groups_x, groups_y, groups_z);
    record_dispatch(backend, command_buffer, groups_x, groups_y, groups_z);
    pass_trace_end(backend, command_buffer, span);
}

extern "C" void vulkan_cmd_dispatch(s_vulkan_backend *backend, uint32_t groups_x, uint32_t groups_y, uint32_t groups_z)
{
    vulkan_cmd_flush_memexports(backend);
    vulkan_cmd_dispatch_inline(backend, groups_x, groups_y, groups_z);
}

void vulkan_cmd_dispatch_memexport(s_vulkan_backend *backend, uint32_t groups,
                                   const s_memexport_sync_range *ranges, uint32_t count)
{
    auto &batch = memexport_batch(backend);
    constexpr VkPipelineStageFlags2 compute = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;
    if (!batch.active) {
        // Order the first dispatch against earlier graphics and transfers, including vertex/index fetches
        // from buffers it may overwrite. Later dispatches in the batch have only compute dependencies.
        cmd_memory_barrier(backend, backend->command_buffer,
                           (backend->shader_stages & ~compute) | k_memexport_consumer_stages,
                           VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT,
                           compute, k_memexport_shader_access);
        batch.active = true;
    }
    bool hazard = batch.count + count > s_memexport_batch::capacity;
    for (uint32_t i = 0; i < count && !hazard; ++i) {
        const auto &range = ranges[i];
        for (uint32_t j = 0; j < batch.count; ++j) {
            const auto &pending = batch.ranges[j];
            if ((range.write || pending.write) && range.base < pending.end && pending.base < range.end) {
                hazard = true;
                break;
            }
        }
    }
    if (hazard) {
        // Publish to compute here. The next boundary still publishes all earlier
        // compute writes to graphics and transfer, including writes before this barrier.
        cmd_memory_barrier(backend, backend->command_buffer, compute, VK_ACCESS_2_SHADER_WRITE_BIT,
                           compute, k_memexport_shader_access);
        batch.count = 0;
    }
    vulkan_cmd_dispatch_inline(backend, groups, 1, 1);
    memcpy(batch.ranges + batch.count, ranges, count * sizeof(ranges[0]));
    batch.count += count;
}

extern "C" void vulkan_cmd_memory_barrier(s_vulkan_backend *backend)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_memory_barrier");
    if (command_buffer == 0)
        return;
    // This barrier already publishes all pending exports to graphics, compute and transfer.
    take_memexport_batch(backend);
    // Narrow to shader, vertex-input, and transfer stages (skip color/depth ROP stalls and host cache flushes).
    // Include VERTEX_INPUT in srcStages for WAR ordering when memexport overwrites a previously drawn vertex buffer.
    const VkPipelineStageFlags2 rw_stages =
        backend->shader_stages | VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    cmd_memory_barrier(backend, command_buffer, rw_stages,
                       VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT, rw_stages,
                       VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT |
                           VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_2_INDEX_READ_BIT |
                           VK_ACCESS_2_TRANSFER_READ_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT);
}

extern "C" void vulkan_cmd_copy_buffer(s_vulkan_backend *backend, s_vulkan_buffer *source, uint32_t source_offset,
                                       s_vulkan_buffer *destination, uint32_t destination_offset, uint32_t size)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_copy_buffer");
    if (command_buffer == 0 || source == 0 || destination == 0 || size == 0)
        return;
    begin_transfer(backend, false);
    const bool exports = take_memexport_batch(backend);
    const VkPipelineStageFlags2 rw_stages =
        backend->shader_stages | VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    cmd_memory_barrier(backend, command_buffer, rw_stages,
                       VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT,
                       exports ? rw_stages : VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                       exports ? k_memexport_shader_access | k_memexport_consumer_access
                               : VK_ACCESS_2_TRANSFER_READ_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT);
    const uint32_t span = pass_trace_begin(backend, command_buffer, "copy_buffer", size, 1u, 0u);
    VkBufferCopy copy = {};
    copy.srcOffset = source_offset;
    copy.dstOffset = destination_offset;
    copy.size = size;
    backend->vk.vkCmdCopyBuffer(command_buffer, source->buffer, destination->buffer, 1, &copy);
    pass_trace_end(backend, command_buffer, span);
    VkPipelineStageFlags2 post_stages = rw_stages;
    VkAccessFlags2 post_access = VK_ACCESS_2_SHADER_READ_BIT | VK_ACCESS_2_SHADER_WRITE_BIT |
                                 VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_2_INDEX_READ_BIT |
                                 VK_ACCESS_2_TRANSFER_READ_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT;
    if (destination->memory_usage == k_vulkan_memory_readback) {
        post_stages |= VK_PIPELINE_STAGE_2_HOST_BIT;
        post_access |= VK_ACCESS_2_HOST_READ_BIT;
    }
    cmd_memory_barrier(backend, command_buffer, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
                       post_stages, post_access);
}

extern "C" void vulkan_cmd_copy_buffer_to_image(s_vulkan_backend *backend, s_vulkan_buffer *buffer, uint32_t offset,
                                                uint32_t row_length, s_vulkan_image *image, uint32_t width,
                                                uint32_t height)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_copy_buffer_to_image");
    if (command_buffer == 0 || buffer == 0 || image == 0 || width == 0 || height == 0)
        return;
    begin_transfer(backend, false);
    const bool exports = take_memexport_batch(backend);
    const uint32_t span = pass_trace_begin(backend, command_buffer, "copy_buf_to_img", width, height,
                                           image->desc.format);
    // Combine prior shader/transfer writes to `buffer` with the image transition into TRANSFER_DST.
    [[clang::uninitialized]] s_image_barrier pre_img;
    const bool whole_image = covers_whole_2d_image(image, width, height);
    const bool has_pre = transition_barrier(backend, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, whole_image, &pre_img);
    cmd_memory_and_image_barrier(backend, command_buffer, backend->shader_stages | VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                 VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT,
                                 exports ? backend->shader_stages | k_memexport_consumer_stages
                                         : VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                 exports ? k_memexport_shader_access | k_memexport_consumer_access
                                         : VK_ACCESS_2_TRANSFER_READ_BIT,
                                 has_pre ? &pre_img : 0);
    VkBufferImageCopy copy = {};
    copy.bufferOffset = offset;
    copy.bufferRowLength = row_length;
    copy.imageSubresource.aspectMask = image->aspect;
    copy.imageSubresource.layerCount = 1;
    copy.imageExtent.width = width;
    copy.imageExtent.height = height;
    copy.imageExtent.depth = 1;
    backend->vk.vkCmdCopyBufferToImage(command_buffer, buffer->buffer, image->image,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    // Combine image transition to SHADER_READ_ONLY with WAR ordering before subsequent writes to `buffer`.
    [[clang::uninitialized]] s_image_barrier post_img;
    const bool has_post = transition_barrier(backend, image, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, false, &post_img);
    cmd_memory_and_image_barrier(backend, command_buffer, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_NONE,
                                 backend->shader_stages | VK_PIPELINE_STAGE_2_TRANSFER_BIT,
                                 VK_ACCESS_2_SHADER_WRITE_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT,
                                 has_post ? &post_img : 0);
    pass_trace_end(backend, command_buffer, span);
}

extern "C" void vulkan_cmd_readback(s_vulkan_backend *backend, s_vulkan_render_target *target)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_readback");
    if (command_buffer == 0)
        return;
    begin_transfer(backend);
    cmd_transition(backend, command_buffer, target->color, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    VkBufferImageCopy copy = {};
    copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copy.imageSubresource.layerCount = 1;
    copy.imageExtent.width = target->width;
    copy.imageExtent.height = target->height;
    copy.imageExtent.depth = 1;
    backend->vk.vkCmdCopyImageToBuffer(command_buffer, target->color->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                       target->readback->buffer, 1, &copy);
    // The copy's writes, made available to the host once the frame's fence is waited for.
    cmd_memory_barrier(backend, command_buffer, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
                       VK_PIPELINE_STAGE_2_HOST_BIT, VK_ACCESS_2_HOST_READ_BIT);
}

// ---- presentation ----

static VkResult surface_created(s_vulkan_backend *backend, VkResult result, const char *function)
{
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: %s failed: %s", function, vulkan_result_string(result));
        return result;
    }
    VkBool32 supported = VK_FALSE;
    backend->vk.vkGetPhysicalDeviceSurfaceSupportKHR(backend->physical_device, backend->queue_family, backend->surface,
                                                     &supported);
    if (!supported) {
        vulkan_log(backend, "vulkan: the graphics queue cannot present to this surface");
        vulkan_surface_destroy(backend);
        return VK_ERROR_INCOMPATIBLE_DISPLAY_KHR;
    }
    return VK_SUCCESS;
}

// Whether a surface of this window system can be created now.
static bool surface_available(s_vulkan_backend *backend, uint32_t window_system, bool has_create_function)
{
    if (!backend->presentation || backend->window_system != window_system || !has_create_function) {
        vulkan_log(backend, "vulkan: no presentation for this window system (the backend was created for %u)",
                   backend->window_system);
        return false;
    }
    if (backend->surface != VK_NULL_HANDLE) {
        vulkan_log(backend, "vulkan: the backend already has a surface");
        return false;
    }
    return true;
}

extern "C" VkResult vulkan_surface_create_win32(s_vulkan_backend *backend, void *hinstance, void *hwnd)
{
    if (!surface_available(backend, k_vulkan_window_system_win32, backend->vk.vkCreateWin32SurfaceKHR != 0))
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    VkWin32SurfaceCreateInfoKHR create = {};
    create.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create.hinstance = hinstance;
    create.hwnd = hwnd;
    return surface_created(backend, backend->vk.vkCreateWin32SurfaceKHR(backend->instance, &create, 0, &backend->surface),
                           "vkCreateWin32SurfaceKHR");
}

extern "C" VkResult vulkan_surface_create_xlib(s_vulkan_backend *backend, void *display, unsigned long window)
{
    if (!surface_available(backend, k_vulkan_window_system_xlib, backend->vk.vkCreateXlibSurfaceKHR != 0))
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    VkXlibSurfaceCreateInfoKHR create = {};
    create.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    create.dpy = (struct _XDisplay *)display;
    create.window = window;
    return surface_created(backend, backend->vk.vkCreateXlibSurfaceKHR(backend->instance, &create, 0, &backend->surface),
                           "vkCreateXlibSurfaceKHR");
}

extern "C" VkResult vulkan_surface_create_xcb(s_vulkan_backend *backend, void *connection, uint32_t window)
{
    if (!surface_available(backend, k_vulkan_window_system_xcb, backend->vk.vkCreateXcbSurfaceKHR != 0))
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    VkXcbSurfaceCreateInfoKHR create = {};
    create.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    create.connection = (struct xcb_connection_t *)connection;
    create.window = window;
    return surface_created(backend, backend->vk.vkCreateXcbSurfaceKHR(backend->instance, &create, 0, &backend->surface),
                           "vkCreateXcbSurfaceKHR");
}

extern "C" VkResult vulkan_surface_create_headless(s_vulkan_backend *backend)
{
    if (!surface_available(backend, k_vulkan_window_system_headless,
                           backend->vk.vkCreateHeadlessSurfaceEXT != 0))
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    VkHeadlessSurfaceCreateInfoEXT create = {};
    create.sType = VK_STRUCTURE_TYPE_HEADLESS_SURFACE_CREATE_INFO_EXT;
    return surface_created(backend, backend->vk.vkCreateHeadlessSurfaceEXT(backend->instance, &create, 0, &backend->surface),
                           "vkCreateHeadlessSurfaceEXT");
}

extern "C" void vulkan_surface_destroy(s_vulkan_backend *backend)
{
    if (backend->surface == VK_NULL_HANDLE)
        return;
    vulkan_swapchain_destroy(backend);
    backend->vk.vkDestroySurfaceKHR(backend->instance, backend->surface, 0);
    backend->surface = VK_NULL_HANDLE;
}

static void release_swapchain_images(s_vulkan_backend *backend)
{
    for (uint32_t i = 0; i < backend->swapchain_image_count; i++)
        backend->vk.vkDestroySemaphore(backend->device, backend->present_semaphores[i], 0);
    free(backend->swapchain_images);
    free(backend->swapchain_layouts);
    free(backend->present_semaphores);
    backend->swapchain_images = 0;
    backend->swapchain_layouts = 0;
    backend->present_semaphores = 0;
    backend->swapchain_image_count = 0;
}

extern "C" VkResult vulkan_swapchain_create(s_vulkan_backend *backend, uint32_t width, uint32_t height, uint32_t vsync)
{
    s_vulkan_functions &vk = backend->vk;
    if (backend->surface == VK_NULL_HANDLE || vk.vkCreateSwapchainKHR == 0) {
        vulkan_log(backend, "vulkan: vulkan_swapchain_create without a surface");
        return VK_ERROR_SURFACE_LOST_KHR;
    }
    VkSurfaceCapabilitiesKHR capabilities;
    VkResult result = vk.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(backend->physical_device, backend->surface, &capabilities);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: surface capabilities: %s", vulkan_result_string(result));
        return result;
    }
    if ((capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0) {
        vulkan_log(backend, "vulkan: the surface's images cannot be blitted to (no TRANSFER_DST usage)");
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }
    // Format: B8G8R8A8 or R8G8B8A8 UNORM (the game does its own gamma), else whatever comes first.
    uint32_t format_count = 0;
    vk.vkGetPhysicalDeviceSurfaceFormatsKHR(backend->physical_device, backend->surface, &format_count, 0);
    VkSurfaceFormatKHR *formats = (VkSurfaceFormatKHR *)calloc(format_count + 1, sizeof(VkSurfaceFormatKHR));
    if (formats == 0)
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    vk.vkGetPhysicalDeviceSurfaceFormatsKHR(backend->physical_device, backend->surface, &format_count, formats);
    if (format_count == 0) {
        free(formats);
        vulkan_log(backend, "vulkan: the surface reports no formats");
        return VK_ERROR_FORMAT_NOT_SUPPORTED;
    }
    VkSurfaceFormatKHR surface_format = formats[0];
    for (uint32_t i = 0; i < format_count; i++)
        if ((formats[i].format == VK_FORMAT_B8G8R8A8_UNORM || formats[i].format == VK_FORMAT_R8G8B8A8_UNORM) &&
            formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surface_format = formats[i];
            break;
        }
    free(formats);
    // Present mode: FIFO (always there) with vsync; otherwise IMMEDIATE (avoids Windows DWM/Optimus MAILBOX caps
    // and minimizes latency), else MAILBOX, else FIFO.
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    if (!vsync) {
        uint32_t mode_count = 0;
        vk.vkGetPhysicalDeviceSurfacePresentModesKHR(backend->physical_device, backend->surface, &mode_count, 0);
        VkPresentModeKHR *modes = (VkPresentModeKHR *)calloc(mode_count + 1, sizeof(VkPresentModeKHR));
        if (modes != 0) {
            vk.vkGetPhysicalDeviceSurfacePresentModesKHR(backend->physical_device, backend->surface, &mode_count, modes);
            for (uint32_t i = 0; i < mode_count; i++)
                if (modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR ||
                    (modes[i] == VK_PRESENT_MODE_MAILBOX_KHR && present_mode == VK_PRESENT_MODE_FIFO_KHR))
                    present_mode = modes[i];
            free(modes);
        }
    }
    VkExtent2D extent = capabilities.currentExtent;
    if (extent.width == 0xffffffffu) {
        extent.width = width < capabilities.minImageExtent.width ? capabilities.minImageExtent.width
                       : width > capabilities.maxImageExtent.width ? capabilities.maxImageExtent.width
                                                                   : width;
        extent.height = height < capabilities.minImageExtent.height ? capabilities.minImageExtent.height
                        : height > capabilities.maxImageExtent.height ? capabilities.maxImageExtent.height
                                                                      : height;
    }
    uint32_t image_count = capabilities.minImageCount + 1;
    if (image_count < backend->frame_count + 1u)
        image_count = backend->frame_count + 1u;
    if (capabilities.maxImageCount != 0 && image_count > capabilities.maxImageCount)
        image_count = capabilities.maxImageCount;
    VkCompositeAlphaFlagBitsKHR composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    if ((capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) == 0)
        composite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;

    VkSwapchainCreateInfoKHR create = {};
    create.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create.surface = backend->surface;
    create.minImageCount = image_count;
    create.imageFormat = surface_format.format;
    create.imageColorSpace = surface_format.colorSpace;
    create.imageExtent = extent;
    create.imageArrayLayers = 1;
    create.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create.preTransform = (capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) != 0
                              ? VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR
                              : capabilities.currentTransform;
    create.compositeAlpha = composite;
    create.presentMode = present_mode;
    create.clipped = VK_TRUE;
    create.oldSwapchain = backend->swapchain;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    if (backend->swapchain != VK_NULL_HANDLE)
        vk.vkDeviceWaitIdle(backend->device);
    result = vk.vkCreateSwapchainKHR(backend->device, &create, 0, &swapchain);
    vulkan_swapchain_destroy(backend);
    if (result != VK_SUCCESS) {
        vulkan_log(backend, "vulkan: vkCreateSwapchainKHR failed: %s", vulkan_result_string(result));
        return result;
    }
    backend->swapchain = swapchain;
    backend->swapchain_format = surface_format.format;
    backend->swapchain_width = extent.width;
    backend->swapchain_height = extent.height;

    uint32_t count = 0;
    vk.vkGetSwapchainImagesKHR(backend->device, swapchain, &count, 0);
    backend->swapchain_images = (VkImage *)calloc(count + 1, sizeof(VkImage));
    backend->swapchain_layouts = (uint32_t *)calloc(count + 1, sizeof(uint32_t));
    backend->present_semaphores = (VkSemaphore *)calloc(count + 1, sizeof(VkSemaphore));
    if (backend->swapchain_images == 0 || backend->swapchain_layouts == 0 || backend->present_semaphores == 0) {
        vulkan_swapchain_destroy(backend);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }
    vk.vkGetSwapchainImagesKHR(backend->device, swapchain, &count, backend->swapchain_images);
    VkSemaphoreCreateInfo semaphore = {};
    semaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    for (uint32_t i = 0; i < count; i++) {
        backend->swapchain_layouts[i] = VK_IMAGE_LAYOUT_UNDEFINED;
        result = vk.vkCreateSemaphore(backend->device, &semaphore, 0, &backend->present_semaphores[i]);
        backend->swapchain_image_count = i + 1;
        if (result != VK_SUCCESS) {
            vulkan_swapchain_destroy(backend);
            return result;
        }
    }
    vulkan_log(backend, "vulkan: swapchain %ux%u, %u images, format %u, present mode %u", extent.width, extent.height,
               count, surface_format.format, present_mode);
    return VK_SUCCESS;
}

extern "C" void vulkan_swapchain_destroy(s_vulkan_backend *backend)
{
    if (backend->swapchain == VK_NULL_HANDLE && backend->swapchain_image_count == 0)
        return;
    backend->vk.vkDeviceWaitIdle(backend->device);
    release_swapchain_images(backend);
    if (backend->swapchain != VK_NULL_HANDLE)
        backend->vk.vkDestroySwapchainKHR(backend->device, backend->swapchain, 0);
    backend->swapchain = VK_NULL_HANDLE;
    backend->present_image = VULKAN_NO_IMAGE;
    for (uint32_t i = 0; i < backend->frame_count; i++)
        backend->frames[i].acquired_image = VULKAN_NO_IMAGE;
}

extern "C" VkResult vulkan_swapchain_acquire(s_vulkan_backend *backend, uint32_t *out_image_index)
{
    if (!backend->recording || backend->swapchain == VK_NULL_HANDLE) {
        vulkan_log(backend, "vulkan: vulkan_swapchain_acquire needs a swapchain and a frame being recorded");
        return VK_ERROR_UNKNOWN;
    }
    s_vulkan_frame &frame = backend->frames[backend->frame_index];
    if (frame.acquired_image != VULKAN_NO_IMAGE) {
        if (out_image_index != 0)
            *out_image_index = frame.acquired_image;
        return VK_SUCCESS;
    }
    uint32_t image = 0;
    VkResult result = backend->vk.vkAcquireNextImageKHR(backend->device, backend->swapchain, ~0ull,
                                                        frame.acquire_semaphore, VK_NULL_HANDLE, &image);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        return result;
    frame.acquired_image = image;
    if (out_image_index != 0)
        *out_image_index = image;
    return result;
}

extern "C" void vulkan_cmd_blit_to_swapchain(s_vulkan_backend *backend, s_vulkan_render_target *target)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_blit_to_swapchain");
    if (command_buffer == 0)
        return;
    uint32_t image = backend->frames[backend->frame_index].acquired_image;
    if (image == VULKAN_NO_IMAGE) {
        vulkan_log(backend, "vulkan: vulkan_cmd_blit_to_swapchain before vulkan_swapchain_acquire");
        return;
    }
    const uint32_t sw = backend->swapchain_width;
    const uint32_t sh = backend->swapchain_height;
    const uint32_t tw = target->width != 0 ? target->width : 1u;
    const uint32_t th = target->height != 0 ? target->height : 1u;
    if (sw == 0 || sh == 0)
        return;
    begin_transfer(backend);
    [[clang::uninitialized]] s_image_barrier barriers[2];
    uint32_t barrier_count = 0;
    if (transition_barrier(backend, target->color, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, false, &barriers[barrier_count]))
        barrier_count++;
    // The acquire semaphore is waited on at TRANSFER_BIT, which this barrier's first scope chains to.
    s_image_barrier &sb = barriers[barrier_count++];
    sb.image = backend->swapchain_images[image];
    sb.aspect = VK_IMAGE_ASPECT_COLOR_BIT;
    sb.old_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    sb.new_layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    sb.src_stages = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    sb.src_access = VK_ACCESS_2_NONE;
    sb.dst_stages = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    sb.dst_access = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    cmd_image_barriers(backend, command_buffer, barriers, barrier_count);

    // Preserve display aspect ratio when the window is resized (Linux & Windows):
    // Halo 3 renders 1152x640 (9:5) for a 16:9 screen on Xbox 360. Keep exact 1:1 when the window matches the target,
    // and fit centered 16:9 (else target's own aspect) with solid black letterbox/pillarbox bars otherwise.
    uint32_t dst_x = 0, dst_y = 0, dst_w = sw, dst_h = sh;
    if (sw != tw || sh != th) {
        const uint64_t aspect_num = ((uint64_t)tw * 5u == (uint64_t)th * 9u) ? 16u : (uint64_t)tw;
        const uint64_t aspect_den = ((uint64_t)tw * 5u == (uint64_t)th * 9u) ? 9u : (uint64_t)th;
        if ((uint64_t)sw * aspect_den > (uint64_t)sh * aspect_num) {
            dst_h = sh;
            dst_w = (uint32_t)(((uint64_t)sh * aspect_num + aspect_den / 2u) / aspect_den);
            if (dst_w == 0)
                dst_w = 1;
            if (dst_w > sw)
                dst_w = sw;
            dst_x = (sw - dst_w) / 2u;
        } else if ((uint64_t)sw * aspect_den < (uint64_t)sh * aspect_num) {
            dst_w = sw;
            dst_h = (uint32_t)(((uint64_t)sw * aspect_den + aspect_num / 2u) / aspect_num);
            if (dst_h == 0)
                dst_h = 1;
            if (dst_h > sh)
                dst_h = sh;
            dst_y = (sh - dst_h) / 2u;
        }
    }

    if (dst_w < sw || dst_h < sh) {
        const VkClearColorValue black = { { 0.0f, 0.0f, 0.0f, 1.0f } };
        VkImageSubresourceRange range = {};
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.levelCount = 1;
        range.layerCount = 1;
        backend->vk.vkCmdClearColorImage(command_buffer, backend->swapchain_images[image],
                                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &black, 1, &range);
        cmd_image_barrier(backend, command_buffer, backend->swapchain_images[image], VK_IMAGE_ASPECT_COLOR_BIT,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
                          VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT);
    }

    VkImageBlit blit = {};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.layerCount = 1;
    blit.srcOffsets[1].x = (int32_t)tw;
    blit.srcOffsets[1].y = (int32_t)th;
    blit.srcOffsets[1].z = 1;
    blit.dstSubresource = blit.srcSubresource;
    blit.dstOffsets[0].x = (int32_t)dst_x;
    blit.dstOffsets[0].y = (int32_t)dst_y;
    blit.dstOffsets[0].z = 0;
    blit.dstOffsets[1].x = (int32_t)(dst_x + dst_w);
    blit.dstOffsets[1].y = (int32_t)(dst_y + dst_h);
    blit.dstOffsets[1].z = 1;
    backend->vk.vkCmdBlitImage(command_buffer, target->color->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                               backend->swapchain_images[image], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                               VK_FILTER_LINEAR);
    cmd_image_barrier(backend, command_buffer, backend->swapchain_images[image], VK_IMAGE_ASPECT_COLOR_BIT,
                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                      VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_2_NONE,
                      VK_ACCESS_2_NONE);
    backend->swapchain_layouts[image] = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
}

extern "C" VkResult vulkan_swapchain_present(s_vulkan_backend *backend)
{
    uint32_t image = backend->present_image;
    if (image == VULKAN_NO_IMAGE || backend->swapchain == VK_NULL_HANDLE) {
        vulkan_log(backend, "vulkan: vulkan_swapchain_present without an acquired image in the last frame");
        return VK_ERROR_UNKNOWN;
    }
    backend->present_image = VULKAN_NO_IMAGE;
    VkPresentInfoKHR present = {};
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores = &backend->present_semaphores[image];
    present.swapchainCount = 1;
    present.pSwapchains = &backend->swapchain;
    present.pImageIndices = &image;
    return backend->vk.vkQueuePresentKHR(backend->queue, &present);
}

// ---- generic attachments and image copies (the D3D device's render targets and resolves) ----

extern "C" s_vulkan_image *vulkan_render_target_color_image(s_vulkan_render_target *target)
{
    return target != 0 ? target->color : 0;
}

extern "C" void vulkan_image_get_desc(const s_vulkan_image *image, s_vulkan_image_desc *out)
{
    *out = image->desc;
}

extern "C" void vulkan_cmd_begin_rendering_images(s_vulkan_backend *backend, s_vulkan_image *const *colors,
                                                  uint32_t color_count, s_vulkan_image *depth_image,
                                                  const s_vulkan_clear *clear)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_begin_rendering_images");
    if (command_buffer == 0)
        return;
    if (backend->rendering)
        vulkan_cmd_end_rendering(backend);
    vulkan_cmd_flush_memexports(backend);
    if (color_count > VULKAN_MAX_COLOR_ATTACHMENTS)
        color_count = VULKAN_MAX_COLOR_ATTACHMENTS;
    uint32_t flags = clear != 0 ? clear->flags : 0u;
    // The render area is what every attachment covers: Xenos surfaces of different sizes can be bound together
    // (EDRAM holds them side by side), and draws may only touch their common area.
    uint32_t width = 0, height = 0;
    bool sized = false;
    auto cover = [&](const s_vulkan_image *image) {
        if (!sized || image->desc.width < width)
            width = image->desc.width;
        if (!sized || image->desc.height < height)
            height = image->desc.height;
        sized = true;
    };
    if (depth_image != 0)
        cover(depth_image);
    for (uint32_t i = 0; i < color_count; i++) {
        backend->rendering_colors[i] = colors[i];
        if (colors[i] != 0)
            cover(colors[i]);
    }
    if (width == 0) {
        vulkan_log(backend, "vulkan: begin rendering without attachments");
        return;
    }
    // the attachments' transitions, in one barrier command (an image bound twice: its first transition first)
    [[clang::uninitialized]] s_image_barrier barriers[k_max_image_barriers];
    uint32_t barrier_count = 0;
    auto transition = [&](s_vulkan_image *image, uint32_t layout, bool discard) {
        for (uint32_t i = 0; i < barrier_count; i++) {
            if (barriers[i].image == image->image) {
                cmd_image_barriers(backend, command_buffer, barriers, barrier_count);
                barrier_count = 0;
                break;
            }
        }
        barrier_count += transition_barrier(backend, image, layout, discard, &barriers[barrier_count]) ? 1u : 0u;
    };
    [[clang::uninitialized]] VkRenderingAttachmentInfo color[VULKAN_MAX_COLOR_ATTACHMENTS];
    for (uint32_t i = 0; i < color_count; i++) {
        VkRenderingAttachmentInfo &ci = color[i];
        ci.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        ci.pNext = 0;
        ci.resolveMode = VK_RESOLVE_MODE_NONE;
        ci.resolveImageView = VK_NULL_HANDLE;
        ci.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        s_vulkan_image *image = colors[i];
        if (image == 0) {
            ci.imageView = VK_NULL_HANDLE;
            ci.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            ci.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            ci.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            memset(&ci.clearValue, 0, sizeof(ci.clearValue));
            continue;
        }
        const bool cleared = (flags & k_vulkan_clear_color) != 0 && (clear->color_mask & (1u << i)) != 0;
        transition(image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                   cleared && covers_whole_2d_image(image, width, height));
        image->attachment_dirty = 1; // STORE, including an empty pass
        ci.imageView = image->view;
        ci.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        ci.loadOp = cleared ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        ci.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        if (cleared)
            memcpy(ci.clearValue.color.float32, clear->color, sizeof(clear->color));
    }
    [[clang::uninitialized]] VkRenderingAttachmentInfo depth, stencil;
    bool has_stencil = false;
    if (depth_image != 0) {
        uint32_t depth_stencil_clear = flags & (k_vulkan_clear_depth | k_vulkan_clear_stencil);
        has_stencil = (depth_image->aspect & VK_IMAGE_ASPECT_STENCIL_BIT) != 0;
        bool discard = depth_stencil_clear == (has_stencil ? (uint32_t)(k_vulkan_clear_depth | k_vulkan_clear_stencil)
                                                           : (uint32_t)k_vulkan_clear_depth);
        transition(depth_image, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                   discard && covers_whole_2d_image(depth_image, width, height));
        depth_image->attachment_dirty = 1;
        depth.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depth.pNext = 0;
        depth.imageView = depth_image->view;
        depth.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth.resolveMode = VK_RESOLVE_MODE_NONE;
        depth.resolveImageView = VK_NULL_HANDLE;
        depth.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth.loadOp = (flags & k_vulkan_clear_depth) != 0 ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
        depth.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth.clearValue.depthStencil.depth = clear != 0 ? clear->depth : 1.0f;
        depth.clearValue.depthStencil.stencil = 0;
        if (has_stencil) {
            stencil = depth;
            stencil.loadOp = (flags & k_vulkan_clear_stencil) != 0 ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            stencil.clearValue.depthStencil.stencil = clear != 0 ? clear->stencil : 0u;
        }
    }
    if (barrier_count != 0)
        cmd_image_barriers(backend, command_buffer, barriers, barrier_count);
    VkRenderingInfo rendering = {};
    rendering.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering.renderArea.extent.width = width;
    rendering.renderArea.extent.height = height;
    rendering.layerCount = 1;
    rendering.colorAttachmentCount = color_count;
    rendering.pColorAttachments = color_count != 0 ? color : 0;
    rendering.pDepthAttachment = depth_image != 0 ? &depth : 0;
    rendering.pStencilAttachment = has_stencil ? &stencil : 0;
    if (__builtin_expect(backend->pass_trace_period != 0, 0)) {
        uint32_t primary_fmt = 0;
        for (uint32_t i = 0; i < color_count && primary_fmt == 0; i++)
            if (colors[i] != 0)
                primary_fmt = colors[i]->desc.format;
        if (primary_fmt == 0 && depth_image != 0)
            primary_fmt = depth_image->desc.format;
        backend->pass_trace_active_span = pass_trace_begin(backend, command_buffer,
                                                           color_count != 0 ? "render_pass" : "depth_pass",
                                                           width, height, primary_fmt);
    }
    backend->vk.vkCmdBeginRendering(command_buffer, &rendering);
    backend->rendering = 1;
    ++backend->rendering_epoch;
    backend->rendering_dirty = 1;
    backend->rendering_depth = depth_image;
    occlusion_resume(backend, command_buffer);
    backend->rendering_width = width;
    backend->rendering_height = height;
    backend->rendering_color_count = color_count;
    backend->rendering_has_depth = depth_image != 0;
    backend->rendering_has_stencil = has_stencil;
    vulkan_cmd_set_viewport(backend, 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f);
    vulkan_cmd_set_scissor(backend, 0, 0, width, height);
}

extern "C" void vulkan_cmd_clear_attachments(s_vulkan_backend *backend, const s_vulkan_clear *clear,
                                             const int32_t *rects, uint32_t rect_count)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_clear_attachments");
    if (command_buffer == 0 || clear == 0)
        return;
    if (!backend->rendering) {
        vulkan_log(backend, "vulkan: vulkan_cmd_clear_attachments outside rendering");
        return;
    }
    VkClearAttachment attachments[VULKAN_MAX_COLOR_ATTACHMENTS + 1] = {};
    uint32_t count = 0;
    if ((clear->flags & k_vulkan_clear_color) != 0) {
        for (uint32_t i = 0; i < backend->rendering_color_count; i++) {
            if ((clear->color_mask & (1u << i)) == 0)
                continue;
            attachments[count].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            attachments[count].colorAttachment = i;
            memcpy(attachments[count].clearValue.color.float32, clear->color, sizeof(clear->color));
            count++;
        }
    }
    uint32_t aspect = 0;
    if ((clear->flags & k_vulkan_clear_depth) != 0 && backend->rendering_has_depth)
        aspect |= VK_IMAGE_ASPECT_DEPTH_BIT;
    if ((clear->flags & k_vulkan_clear_stencil) != 0 && backend->rendering_has_stencil)
        aspect |= VK_IMAGE_ASPECT_STENCIL_BIT;
    if (aspect != 0) {
        attachments[count].aspectMask = aspect;
        attachments[count].clearValue.depthStencil.depth = clear->depth;
        attachments[count].clearValue.depthStencil.stencil = clear->stencil;
        count++;
    }
    if (count == 0)
        return;
    if (rects == 0 || rect_count == 0) {
        VkClearRect whole = {};
        whole.rect.extent.width = backend->rendering_width;
        whole.rect.extent.height = backend->rendering_height;
        whole.layerCount = 1;
        backend->vk.vkCmdClearAttachments(command_buffer, count, attachments, 1, &whole);
        return;
    }
    [[clang::uninitialized]] VkClearRect local_rects[16];
    uint32_t n = 0;
    for (uint32_t i = 0; i < rect_count; i++) {
        int32_t x0 = rects[4 * i] < 0 ? 0 : rects[4 * i];
        int32_t y0 = rects[4 * i + 1] < 0 ? 0 : rects[4 * i + 1];
        int32_t x1 = rects[4 * i + 2] > (int32_t)backend->rendering_width ? (int32_t)backend->rendering_width : rects[4 * i + 2];
        int32_t y1 = rects[4 * i + 3] > (int32_t)backend->rendering_height ? (int32_t)backend->rendering_height : rects[4 * i + 3];
        if (x1 <= x0 || y1 <= y0)
            continue;
        VkClearRect &rect = local_rects[n++];
        rect.rect.offset.x = x0;
        rect.rect.offset.y = y0;
        rect.rect.extent.width = (uint32_t)(x1 - x0);
        rect.rect.extent.height = (uint32_t)(y1 - y0);
        rect.baseArrayLayer = 0;
        rect.layerCount = 1;
        if (n == 16u) {
            backend->vk.vkCmdClearAttachments(command_buffer, count, attachments, n, local_rects);
            n = 0;
        }
    }
    if (n != 0)
        backend->vk.vkCmdClearAttachments(command_buffer, count, attachments, n, local_rects);
}

extern "C" void vulkan_cmd_copy_image(s_vulkan_backend *backend, s_vulkan_image *source, int32_t source_x,
                                      int32_t source_y, uint32_t width, uint32_t height, s_vulkan_image *destination,
                                      uint32_t destination_level, uint32_t destination_layer, int32_t destination_x,
                                      int32_t destination_y, uint32_t destination_width, uint32_t destination_height)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_copy_image");
    if (command_buffer == 0 || source == 0 || destination == 0 || width == 0 || height == 0)
        return;
    begin_transfer(backend);
    const uint32_t dst_w = destination_width != 0 ? destination_width : width;
    const uint32_t dst_h = destination_height != 0 ? destination_height : height;
    const uint32_t dst_mip_w = (destination->desc.width >> destination_level) != 0
                                   ? (destination->desc.width >> destination_level) : 1u;
    const uint32_t dst_mip_h = (destination->desc.height >> destination_level) != 0
                                   ? (destination->desc.height >> destination_level) : 1u;
    const bool source_in_bounds = source_x >= 0 && source_y >= 0 &&
                                  (uint64_t)source_x + width <= source->desc.width &&
                                  (uint64_t)source_y + height <= source->desc.height;
    const bool exact_copy = source->desc.format == destination->desc.format && source->aspect == destination->aspect &&
                            dst_w == width && dst_h == height && source_in_bounds &&
                            destination_x >= 0 && destination_y >= 0 &&
                            (uint64_t)destination_x + dst_w <= dst_mip_w &&
                            (uint64_t)destination_y + dst_h <= dst_mip_h;
    const uint32_t span = pass_trace_begin(backend, command_buffer, exact_copy ? "copy_image" : "blit_image",
                                           width, height, destination->desc.format);
    const bool discard_dst = source != destination && source_in_bounds && destination_level == 0 &&
                             destination_layer == 0 && destination_x == 0 && destination_y == 0 &&
                             covers_whole_2d_image(destination, dst_w, dst_h);
    cmd_transition_pair(backend, command_buffer, source, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, destination,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, discard_dst);
    if (exact_copy) {
        VkImageCopy copy = {};
        copy.srcSubresource.aspectMask = source->aspect;
        copy.srcSubresource.layerCount = 1;
        copy.srcOffset.x = source_x;
        copy.srcOffset.y = source_y;
        copy.dstSubresource.aspectMask = destination->aspect;
        copy.dstSubresource.mipLevel = destination_level;
        copy.dstSubresource.baseArrayLayer = destination_layer;
        copy.dstSubresource.layerCount = 1;
        copy.dstOffset.x = destination_x;
        copy.dstOffset.y = destination_y;
        copy.extent.width = width;
        copy.extent.height = height;
        copy.extent.depth = 1;
        backend->vk.vkCmdCopyImage(command_buffer, source->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                   destination->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
    } else {
        VkImageBlit blit = {};
        blit.srcSubresource.aspectMask = source->aspect;
        blit.srcSubresource.layerCount = 1;
        blit.srcOffsets[0].x = source_x;
        blit.srcOffsets[0].y = source_y;
        blit.srcOffsets[1].x = source_x + (int32_t)width;
        blit.srcOffsets[1].y = source_y + (int32_t)height;
        blit.srcOffsets[1].z = 1;
        blit.dstSubresource.aspectMask = destination->aspect;
        blit.dstSubresource.mipLevel = destination_level;
        blit.dstSubresource.baseArrayLayer = destination_layer;
        blit.dstSubresource.layerCount = 1;
        blit.dstOffsets[0].x = destination_x;
        blit.dstOffsets[0].y = destination_y;
        blit.dstOffsets[1].x = destination_x + (int32_t)dst_w;
        blit.dstOffsets[1].y = destination_y + (int32_t)dst_h;
        blit.dstOffsets[1].z = 1;
        // blit for scaling, format conversion, or out-of-bounds/flipped rects
        backend->vk.vkCmdBlitImage(command_buffer, source->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                   destination->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                                   VK_FILTER_NEAREST);
    }
    if ((destination->desc.usage & (VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)) != 0)
        cmd_transition(backend, command_buffer, destination, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    pass_trace_end(backend, command_buffer, span);
}

extern "C" void vulkan_cmd_copy_depth(s_vulkan_backend *backend, s_vulkan_image *source, int32_t source_x,
                                      int32_t source_y, uint32_t width, uint32_t height, s_vulkan_image *destination,
                                      int32_t destination_x, int32_t destination_y)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_copy_depth");
    if (command_buffer == 0 || source == 0 || destination == 0 || width == 0 || height == 0)
        return;
    if ((source->aspect & VK_IMAGE_ASPECT_DEPTH_BIT) == 0 || source->desc.format != destination->desc.format) {
        vulkan_log(backend, "vulkan: vulkan_cmd_copy_depth needs two depth images of one format (%u, %u)",
                   source->desc.format, destination->desc.format);
        return;
    }
    begin_transfer(backend);
    const uint32_t span = pass_trace_begin(backend, command_buffer, "copy_depth", width, height,
                                           destination->desc.format);
    cmd_transition_pair(backend, command_buffer, source, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, destination,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    // depth, and stencil where both images have it (a depth resolve keeps the stencil the surface held, which a
    // restore through EDRAM brings back: vulkan_device.cpp edram_sync)
    VkImageCopy copies[2] = {};
    uint32_t count = 0;
    const uint32_t aspects[2] = { VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_STENCIL_BIT };
    for (uint32_t a = 0; a < 2; a++) {
        if ((source->aspect & aspects[a]) == 0 || (destination->aspect & aspects[a]) == 0)
            continue;
        VkImageCopy &copy = copies[count++];
        copy.srcSubresource.aspectMask = aspects[a];
        copy.srcSubresource.layerCount = 1;
        copy.srcOffset.x = source_x;
        copy.srcOffset.y = source_y;
        copy.dstSubresource.aspectMask = aspects[a];
        copy.dstSubresource.layerCount = 1;
        copy.dstOffset.x = destination_x;
        copy.dstOffset.y = destination_y;
        copy.extent.width = width;
        copy.extent.height = height;
        copy.extent.depth = 1;
    }
    backend->vk.vkCmdCopyImage(command_buffer, source->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, destination->image,
                               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, count, copies);
    if ((destination->desc.usage & (VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)) != 0)
        cmd_transition(backend, command_buffer, destination, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    pass_trace_end(backend, command_buffer, span);
}

extern "C" void vulkan_cmd_copy_rects(s_vulkan_backend *backend, s_vulkan_image *source, s_vulkan_image *destination,
                                      const s_vulkan_copy_rect *rects, uint32_t count)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_copy_rects");
    if (command_buffer == 0 || source == 0 || destination == 0 || source == destination || count == 0)
        return;
    if (source->desc.format != destination->desc.format) {
        vulkan_log(backend, "vulkan: vulkan_cmd_copy_rects needs two images of one format (%u, %u)",
                   source->desc.format, destination->desc.format);
        return;
    }
    // each aspect both images have is a region of its own (a depth/stencil copy names one aspect per region)
    const uint32_t aspects[3] = { VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_ASPECT_STENCIL_BIT };
    uint32_t aspect_count = 0, copied[3];
    for (uint32_t a = 0; a < 3; a++)
        if ((source->aspect & destination->aspect & aspects[a]) != 0)
            copied[aspect_count++] = aspects[a];
    [[clang::uninitialized]] VkImageCopy local_copies[16];
    const size_t total = (size_t)count * aspect_count;
    VkImageCopy *copies = total <= 16u ? local_copies : (VkImageCopy *)calloc(total, sizeof(VkImageCopy));
    if (copies == 0 || aspect_count == 0) {
        if (copies != local_copies)
            free(copies);
        return;
    }
    if (copies == local_copies)
        memset(local_copies, 0, total * sizeof(VkImageCopy));
    uint32_t n = 0;
    for (uint32_t i = 0; i < count; i++) {
        if (rects[i].width == 0 || rects[i].height == 0)
            continue;
        for (uint32_t a = 0; a < aspect_count; a++) {
            VkImageCopy &copy = copies[n++];
            copy.srcSubresource.aspectMask = copied[a];
            copy.srcSubresource.layerCount = 1;
            copy.srcOffset.x = rects[i].source_x;
            copy.srcOffset.y = rects[i].source_y;
            copy.dstSubresource.aspectMask = copied[a];
            copy.dstSubresource.layerCount = 1;
            copy.dstOffset.x = rects[i].destination_x;
            copy.dstOffset.y = rects[i].destination_y;
            copy.extent.width = rects[i].width;
            copy.extent.height = rects[i].height;
            copy.extent.depth = 1;
        }
    }
    if (n != 0) {
        begin_transfer(backend);
        cmd_transition_pair(backend, command_buffer, source, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, destination,
                            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        backend->vk.vkCmdCopyImage(command_buffer, source->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                   destination->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, n, copies);
        // a texture is left for sampling; an attachment or a scratch image as it is (its next use moves it)
        if ((destination->desc.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0)
            cmd_transition(backend, command_buffer, destination, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    if (copies != local_copies)
        free(copies);
}

extern "C" VkResult vulkan_buffer_download(s_vulkan_backend *backend, s_vulkan_buffer *buffer, uint32_t offset,
                                           void *data, uint32_t size)
{
    if (offset > buffer->size || size > buffer->size - offset) {
        vulkan_log(backend, "vulkan: buffer download of %u bytes at %u overruns the %u-byte buffer", size, offset,
                   buffer->size);
        return VK_ERROR_UNKNOWN;
    }
    if (size == 0)
        return VK_SUCCESS;
    if (buffer->mapped != 0) {
        vulkan_buffer_invalidate(backend, buffer);
        memcpy(data, (const uint8_t *)buffer->mapped + offset, size);
        return VK_SUCCESS;
    }
    s_vulkan_buffer *staging = vulkan_buffer_create(backend, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                                    k_vulkan_memory_readback);
    if (staging == 0)
        return VK_ERROR_OUT_OF_DEVICE_MEMORY;
    VkCommandBuffer command_buffer = immediate_begin(backend, "vulkan_buffer_download");
    VkBufferCopy copy = {};
    copy.srcOffset = offset;
    copy.size = size;
    backend->vk.vkCmdCopyBuffer(command_buffer, buffer->buffer, staging->buffer, 1, &copy);
    VkResult result = immediate_submit(backend);
    if (result == VK_SUCCESS) {
        vulkan_buffer_invalidate(backend, staging);
        memcpy(data, staging->mapped, size);
    }
    if (result == VK_SUCCESS)
        buffer_release(backend, staging);
    else
        vulkan_buffer_destroy(backend, staging);
    return result;
}

// ---- image passes: EDRAM aliasing, display gamma, resolve exponent bias ----

#include "edram_7e3_spirv.h"
#include "display_gamma_spirv.h"
#include "resolve_scale_spirv.h"
#include "resolve_scale_any_spirv.h"
#include "pwl_convert_spirv.h"
#include "resolve_downscale_spirv.h"
#include "depth_pack_spirv.h"
#ifdef HALO3_64BIT
#include "depth_gather_spirv.h"
#endif

extern "C" uint32_t vulkan_format_storage(s_vulkan_backend *backend, uint32_t format)
{
    VkFormatProperties properties;
    backend->vk.vkGetPhysicalDeviceFormatProperties(backend->physical_device, (VkFormat)format, &properties);
    return (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT) != 0;
}

extern "C" uint32_t vulkan_format_blendable_filterable(s_vulkan_backend *backend, uint32_t format)
{
    VkFormatProperties properties;
    backend->vk.vkGetPhysicalDeviceFormatProperties(backend->physical_device, (VkFormat)format, &properties);
    const VkFormatFeatureFlags needed =
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT;
    return (properties.optimalTilingFeatures & needed) == needed;
}

// One of the backend's own compute passes (s_vulkan_backend::image_passes): a sampled source image (binding 0), a
// storage destination image (binding 1) and, with a uniform buffer, its parameters (binding 2). The pipeline is made
// on first use. Each group covers 64x4 pixels for PWL and display gamma, 8x8 for the other passes.
// The compute pipeline of one of the backend's image passes, made on first use (a pass that failed once stays failed).
static bool image_pass_pipeline(s_vulkan_backend *backend, const char *name, s_vulkan_image_pass *state,
                                const uint32_t *spirv, size_t spirv_size, const s_vulkan_layout_binding *bindings,
                                uint32_t binding_count)
{
    if (state->pipeline != 0)
        return true;
    if (state->failed)
        return false;
    if (state->module == VK_NULL_HANDLE)
        state->module = vulkan_shader_module_create(backend, spirv, spirv_size);
    VkDescriptorSetLayout set_layout = vulkan_descriptor_set_layout_get(backend, bindings, binding_count);
    VkPipelineLayout layout = set_layout != VK_NULL_HANDLE ? vulkan_pipeline_layout_get(backend, &set_layout, 1)
                                                           : VK_NULL_HANDLE;
    if (state->module != VK_NULL_HANDLE && layout != VK_NULL_HANDLE)
        state->pipeline = vulkan_compute_pipeline_create(backend, state->module, "main", layout);
    if (state->pipeline == 0) {
        vulkan_log(backend, "vulkan: the %s pipeline could not be made", name);
        state->failed = 1;
        return false;
    }
    return true;
}

static void image_pass(s_vulkan_backend *backend, const char *name, uint32_t pass, const uint32_t *spirv,
                       size_t spirv_size, s_vulkan_image *source, s_vulkan_image *destination, s_vulkan_buffer *uniforms,
                       uint32_t uniform_offset, uint32_t uniform_size, uint32_t width, uint32_t height)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, name);
    if (command_buffer == 0 || source == 0 || destination == 0)
        return;
    if ((destination->desc.usage & VK_IMAGE_USAGE_STORAGE_BIT) == 0) {
        vulkan_log(backend, "vulkan: %s into an image without storage use", name);
        return;
    }
    const s_vulkan_layout_binding bindings[3] = {
        { 0, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { 2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
    };
    s_vulkan_image_pass &state = backend->image_passes[pass];
    if (!image_pass_pipeline(backend, name, &state, spirv, spirv_size, bindings, uniforms != 0 ? 3u : 2u))
        return;
    VkDescriptorSet set = vulkan_descriptor_set_allocate(backend, vulkan_pipeline_set_layout(state.pipeline, 0), 1);
    if (set == VK_NULL_HANDLE)
        return;
    VkDescriptorImageInfo img_infos[2] = {};
    img_infos[0].imageView = source->view;
    img_infos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    img_infos[1].imageView = destination->view;
    img_infos[1].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    VkDescriptorBufferInfo buf_info = {};
    VkWriteDescriptorSet writes[3] = {};
    writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[0].dstSet = set;
    writes[0].dstBinding = 0;
    writes[0].descriptorCount = 1;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    writes[0].pImageInfo = &img_infos[0];
    writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writes[1].dstSet = set;
    writes[1].dstBinding = 1;
    writes[1].descriptorCount = 1;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    writes[1].pImageInfo = &img_infos[1];
    uint32_t write_count = 2;
    if (uniforms != 0) {
        buf_info.buffer = uniforms->buffer;
        buf_info.offset = uniform_offset;
        buf_info.range = uniform_size != 0 ? (VkDeviceSize)uniform_size : VK_WHOLE_SIZE;
        writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[2].dstSet = set;
        writes[2].dstBinding = 2;
        writes[2].descriptorCount = 1;
        writes[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writes[2].pBufferInfo = &buf_info;
        write_count = 3;
    }
    backend->vk.vkUpdateDescriptorSets(backend->device, write_count, writes, 0, 0);
    if (backend->rendering)
        vulkan_cmd_end_rendering(backend);
    vulkan_cmd_flush_memexports(backend);
    const uint32_t span = pass_trace_begin(backend, command_buffer, name, width, height, destination->desc.format);
    // the pass writes the whole destination unless it is a rectangle of it (the resolve scale keeps the rest)
    cmd_transition_pair(backend, command_buffer, source, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, destination,
                        VK_IMAGE_LAYOUT_GENERAL, width == destination->desc.width && height == destination->desc.height);
    cmd_bind_pipeline(backend, command_buffer, state.pipeline, 1);
    record_descriptor_sets(backend, command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, state.pipeline->layout, 0, 1, &set,
                           0, 0);
    if (pass == k_vulkan_image_pass_pwl_encode || pass == k_vulkan_image_pass_pwl_decode)
        record_dispatch(backend, command_buffer, (width + 15u) / 16u, (height + 7u) / 8u, 1);
    else if (pass == k_vulkan_image_pass_display_gamma)
        record_dispatch(backend, command_buffer, (width + 63u) / 64u, (height + 3u) / 4u, 1);
    else
        record_dispatch(backend, command_buffer, (width + 7u) / 8u, (height + 7u) / 8u, 1);
    pass_trace_end(backend, command_buffer, span);
}

extern "C" void vulkan_cmd_reinterpret_7e3(s_vulkan_backend *backend, s_vulkan_image *source,
                                           s_vulkan_image *destination)
{
    if (destination == 0)
        return;
    image_pass(backend, "vulkan_cmd_reinterpret_7e3", k_vulkan_image_pass_7e3, k_edram_7e3_spirv,
               sizeof(k_edram_7e3_spirv), source, destination, 0, 0, 0, destination->desc.width,
               destination->desc.height);
    if (backend->image_passes[k_vulkan_image_pass_7e3].failed == 0 &&
        (destination->desc.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0 &&
        (destination->desc.usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) == 0) {
        VkCommandBuffer cb = recording_command_buffer(backend, "vulkan_cmd_reinterpret_7e3");
        if (cb != 0)
            cmd_transition(backend, cb, destination, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
}

extern "C" void vulkan_cmd_apply_display_gamma(s_vulkan_backend *backend, s_vulkan_image *source,
                                               s_vulkan_image *destination, s_vulkan_buffer *ramp, uint32_t ramp_offset)
{
    if (destination == 0 || ramp == 0)
        return;
    if (destination->desc.format != VK_FORMAT_R8G8B8A8_UNORM) {
        vulkan_log(backend, "vulkan: vulkan_cmd_apply_display_gamma into an image that is not R8G8B8A8_UNORM");
        return;
    }
    image_pass(backend, "vulkan_cmd_apply_display_gamma", k_vulkan_image_pass_display_gamma, k_display_gamma_spirv,
               sizeof(k_display_gamma_spirv), source, destination, ramp, ramp_offset, k_vulkan_display_gamma_ramp_size,
               destination->desc.width, destination->desc.height);
}

extern "C" uint32_t vulkan_cmd_scale_image_rect(s_vulkan_backend *backend, s_vulkan_image *source,
                                                s_vulkan_image *destination, s_vulkan_buffer *parameters,
                                                uint32_t parameters_offset, uint32_t width, uint32_t height)
{
    if (source == 0 || destination == 0 || parameters == 0 ||
        (destination->desc.usage & VK_IMAGE_USAGE_STORAGE_BIT) == 0)
        return 0;
    if (destination->desc.format == VK_FORMAT_R16G16B16A16_SFLOAT) {
        image_pass(backend, "vulkan_cmd_scale_image_rect", k_vulkan_image_pass_resolve_scale, k_resolve_scale_spirv,
                   sizeof(k_resolve_scale_spirv), source, destination, parameters, parameters_offset,
                   k_vulkan_scale_image_parameters_size, width, height);
        if (backend->image_passes[k_vulkan_image_pass_resolve_scale].failed == 0) {
            if ((destination->desc.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0) {
                VkCommandBuffer cb = recording_command_buffer(backend, "vulkan_cmd_scale_image_rect");
                if (cb != 0)
                    cmd_transition(backend, cb, destination, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            }
            return 1;
        }
        return 0;
    }
    const uint32_t format = destination->desc.format;
    if ((format != VK_FORMAT_R16G16_SFLOAT && format != VK_FORMAT_R32G32_SFLOAT &&
         format != VK_FORMAT_R32G32B32A32_SFLOAT) || !backend->storage_write_without_format)
        return 0;
    image_pass(backend, "vulkan_cmd_scale_image_rect", k_vulkan_image_pass_resolve_scale_any, k_resolve_scale_any_spirv,
               sizeof(k_resolve_scale_any_spirv), source, destination, parameters, parameters_offset,
               k_vulkan_scale_image_parameters_size, width, height);
    if (backend->image_passes[k_vulkan_image_pass_resolve_scale_any].failed == 0) {
        if ((destination->desc.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0) {
            VkCommandBuffer cb = recording_command_buffer(backend, "vulkan_cmd_scale_image_rect");
            if (cb != 0)
                cmd_transition(backend, cb, destination, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
        return 1;
    }
    return 0;
}

extern "C" uint32_t vulkan_cmd_convert_pwl(s_vulkan_backend *backend, s_vulkan_image *source,
                                           s_vulkan_image *destination, uint32_t encode)
{
    if (source == 0 || destination == 0 || !backend->storage_write_without_format ||
        (destination->desc.usage & VK_IMAGE_USAGE_STORAGE_BIT) == 0)
        return 0;
    const uint32_t pass = encode ? k_vulkan_image_pass_pwl_encode : k_vulkan_image_pass_pwl_decode;
    if (encode)
        image_pass(backend, "vulkan_cmd_convert_pwl", k_vulkan_image_pass_pwl_encode, k_pwl_encode_spirv,
                   sizeof(k_pwl_encode_spirv), source, destination, 0, 0, 0, destination->desc.width,
                   destination->desc.height);
    else
        image_pass(backend, "vulkan_cmd_convert_pwl", k_vulkan_image_pass_pwl_decode, k_pwl_decode_spirv,
                   sizeof(k_pwl_decode_spirv), source, destination, 0, 0, 0, destination->desc.width,
                   destination->desc.height);
    if (backend->image_passes[pass].failed != 0)
        return 0;
    if ((destination->desc.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0 &&
        (destination->desc.usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) == 0) {
        VkCommandBuffer cb = recording_command_buffer(backend, "vulkan_cmd_convert_pwl");
        if (cb != 0)
            cmd_transition(backend, cb, destination, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    return 1;
}

extern "C" uint32_t vulkan_cmd_downscale_image(s_vulkan_backend *backend, s_vulkan_image *source,
                                               s_vulkan_image *destination)
{
    if (source == 0 || destination == 0 || !backend->storage_write_without_format ||
        (destination->desc.usage & VK_IMAGE_USAGE_STORAGE_BIT) == 0)
        return 0;
    image_pass(backend, "vulkan_cmd_downscale_image", k_vulkan_image_pass_downscale, k_resolve_downscale_spirv,
               sizeof(k_resolve_downscale_spirv), source, destination, 0, 0, 0, destination->desc.width,
               destination->desc.height);
    return backend->image_passes[k_vulkan_image_pass_downscale].failed == 0;
}

extern "C" uint32_t vulkan_cmd_pack_depth_stencil(s_vulkan_backend *backend, s_vulkan_image *source,
                                                  s_vulkan_image *destination, uint32_t float_depth)
{
    static const char name[] = "vulkan_cmd_pack_depth_stencil";
    VkCommandBuffer command_buffer = recording_command_buffer(backend, name);
    if (command_buffer == 0 || source == 0 || destination == 0)
        return 0;
    const VkImageView depth = source->sampled_view, stencil = image_stencil_view(backend, source);
    if (depth == VK_NULL_HANDLE || stencil == VK_NULL_HANDLE || destination->desc.format != VK_FORMAT_R8G8B8A8_UNORM ||
        (destination->desc.usage & VK_IMAGE_USAGE_STORAGE_BIT) == 0 || destination->desc.swizzle != 0) {
        vulkan_log(backend, "vulkan: %s needs a sampled depth/stencil image and an R8G8B8A8_UNORM storage image with "
                            "the identity view", name);
        return 0;
    }
    const uint32_t pass = float_depth ? k_vulkan_image_pass_depth_pack_20e4 : k_vulkan_image_pass_depth_pack_unorm24;
    const s_vulkan_layout_binding bindings[3] = {
        { 0, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { 2, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
    };
    s_vulkan_image_pass &state = backend->image_passes[pass];
    if (!image_pass_pipeline(backend, name, &state, float_depth ? k_depth_pack_20e4_spirv : k_depth_pack_unorm24_spirv,
                             float_depth ? sizeof(k_depth_pack_20e4_spirv) : sizeof(k_depth_pack_unorm24_spirv),
                             bindings, 3))
        return 0;
    VkDescriptorSet set = vulkan_descriptor_set_allocate(backend, vulkan_pipeline_set_layout(state.pipeline, 0), 1);
    if (set == VK_NULL_HANDLE)
        return 0;
    VkDescriptorImageInfo img_infos[3] = {};
    img_infos[0].imageView = depth;
    img_infos[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    img_infos[1].imageView = destination->view;
    img_infos[1].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    img_infos[2].imageView = stencil;
    img_infos[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    VkWriteDescriptorSet writes[3] = {};
    for (uint32_t i = 0; i < 3; i++) {
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = set;
        writes[i].dstBinding = i;
        writes[i].descriptorCount = 1;
        writes[i].descriptorType = i == 1 ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        writes[i].pImageInfo = &img_infos[i];
    }
    backend->vk.vkUpdateDescriptorSets(backend->device, 3, writes, 0, 0);
    if (backend->rendering)
        vulkan_cmd_end_rendering(backend);
    vulkan_cmd_flush_memexports(backend);
    const uint32_t span = pass_trace_begin(backend, command_buffer, name, destination->desc.width,
                                           destination->desc.height, destination->desc.format);
    cmd_transition_pair(backend, command_buffer, source, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, destination,
                        VK_IMAGE_LAYOUT_GENERAL, true); // every texel is written
    cmd_bind_pipeline(backend, command_buffer, state.pipeline, 1);
    record_descriptor_sets(backend, command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, state.pipeline->layout, 0, 1, &set,
                           0, 0);
    record_dispatch(backend, command_buffer, (destination->desc.width + 7u) / 8u, (destination->desc.height + 7u) / 8u, 1);
    cmd_transition(backend, command_buffer, destination, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    pass_trace_end(backend, command_buffer, span);
    return 1;
}

#ifdef HALO3_64BIT
// Gather sample zero in one dispatch instead of copying alternating pairs of columns and rows.
// The two linear planes preserve the source float depth and stencil bytes in the destination image.
extern "C" uint32_t vulkan_cmd_gather_depth(s_vulkan_backend *backend, s_vulkan_image *source,
                                            s_vulkan_image *destination, s_vulkan_buffer *parameters,
                                            uint32_t offset, uint32_t width, uint32_t height)
{
    static const char name[] = "vulkan_cmd_gather_depth";
    VkCommandBuffer command_buffer = recording_command_buffer(backend, name);
    if (command_buffer == 0 || source == 0 || destination == 0 || parameters == 0 || width == 0 || height == 0 ||
        width > destination->desc.width || height > destination->desc.height ||
        source->desc.format != VK_FORMAT_D32_SFLOAT_S8_UINT || destination->desc.format != source->desc.format ||
        (destination->desc.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == 0)
        return 0;
    const VkImageView depth = source->sampled_view, stencil = image_stencil_view(backend, source);
    const uint64_t pitch = ((uint64_t)width + 3u) & ~3ull;
    const uint64_t pixels = pitch * height;
    if (depth == VK_NULL_HANDLE || stencil == VK_NULL_HANDLE ||
        pixels > backend->properties.limits.maxStorageBufferRange / sizeof(float))
        return 0;
    // Bind disjoint views of the scratch buffer so each lane can store four depth words as one aligned vector.
    const VkDeviceSize depth_bytes = pixels * sizeof(float);
    const VkDeviceSize mask = backend->properties.limits.minStorageBufferOffsetAlignment - 1u;
    const VkDeviceSize stencil_offset = (depth_bytes + mask) & ~mask;
    if (stencil_offset + pixels > UINT32_MAX)
        return 0;
    const uint32_t size = (uint32_t)(stencil_offset + pixels);
    const s_vulkan_layout_binding bindings[] = {
        { 0, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { 1, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
        { 4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT },
    };
    s_vulkan_image_pass &state = backend->image_passes[k_vulkan_image_pass_depth_gather];
    if (!image_pass_pipeline(backend, name, &state, k_depth_gather_spirv, sizeof(k_depth_gather_spirv), bindings, 5))
        return 0;
    s_vulkan_buffer *&buffer = backend->depth_gather_buffer;
    if (buffer == 0 || buffer->size < size) {
        s_vulkan_buffer *grown = vulkan_buffer_create(backend, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                                      VK_BUFFER_USAGE_TRANSFER_SRC_BIT, k_vulkan_memory_gpu);
        if (grown == 0)
            return 0;
        vulkan_buffer_retire(backend, buffer);
        buffer = grown;
    }
    VkDescriptorSet set = vulkan_descriptor_set_allocate(backend, vulkan_pipeline_set_layout(state.pipeline, 0), 1);
    if (set == VK_NULL_HANDLE)
        return 0;
    const VkDescriptorImageInfo images[] = {
        { VK_NULL_HANDLE, depth, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
        { VK_NULL_HANDLE, stencil, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
    };
    const VkDescriptorBufferInfo planes[] = {
        { buffer->buffer, 0, depth_bytes }, { buffer->buffer, stencil_offset, pixels },
    };
    const VkDescriptorBufferInfo uniform = { parameters->buffer, offset, 16 };
    // Consecutive bindings of the same type and stage flags can be updated in one write.
    VkWriteDescriptorSet writes[3] = {};
    for (uint32_t i = 0; i < 3; i++) {
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = set;
    }
    writes[0].dstBinding = 0;
    writes[0].descriptorCount = 2;
    writes[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    writes[0].pImageInfo = images;
    writes[1].dstBinding = 2;
    writes[1].descriptorCount = 2;
    writes[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    writes[1].pBufferInfo = planes;
    writes[2].dstBinding = 4;
    writes[2].descriptorCount = 1;
    writes[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writes[2].pBufferInfo = &uniform;
    backend->vk.vkUpdateDescriptorSets(backend->device, 3, writes, 0, 0);
    if (backend->rendering)
        vulkan_cmd_end_rendering(backend);
    vulkan_cmd_flush_memexports(backend);
    const uint32_t span = pass_trace_begin(backend, command_buffer, name, width, height, destination->desc.format);
    s_image_barrier pre_img;
    const bool has_pre = transition_barrier(backend, source, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, false, &pre_img);
    // Both full and cropped gathers consume the shared scratch buffer with vkCmdCopyBufferToImage.
    const VkPipelineStageFlags2 copy_stage =
        backend->synchronization2 ? VK_PIPELINE_STAGE_2_COPY_BIT : VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    cmd_memory_and_image_barrier(backend, command_buffer, copy_stage, VK_ACCESS_2_NONE,
                                 VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT, VK_ACCESS_2_SHADER_WRITE_BIT,
                                 has_pre ? &pre_img : 0);
    cmd_bind_pipeline(backend, command_buffer, state.pipeline, 1);
    record_descriptor_sets(backend, command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, state.pipeline->layout, 0, 1, &set,
                           0, 0);
    // 32 x 2 lanes each gather a 4 x 2 tile.
    record_dispatch(backend, command_buffer, (width + 127u) / 128u, (height + 3u) / 4u, 1);
    s_image_barrier dst_img;
    const bool discard_dst = covers_whole_2d_image(destination, width, height);
    const bool has_dst = transition_barrier(backend, destination, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, discard_dst,
                                            &dst_img);
    cmd_memory_and_image_barrier(backend, command_buffer, VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
                                 VK_ACCESS_2_SHADER_WRITE_BIT, copy_stage,
                                 VK_ACCESS_2_TRANSFER_READ_BIT, has_dst ? &dst_img : 0);

    VkBufferImageCopy copies[2] = {};
    copies[0].bufferRowLength = (uint32_t)pitch;
    copies[0].imageSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    copies[0].imageSubresource.layerCount = 1;
    copies[0].imageExtent = { width, height, 1 };
    copies[1] = copies[0];
    copies[1].bufferOffset = stencil_offset;
    copies[1].imageSubresource.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
    backend->vk.vkCmdCopyBufferToImage(command_buffer, buffer->buffer, destination->image,
                                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 2, copies);
    if ((destination->desc.usage & VK_IMAGE_USAGE_SAMPLED_BIT) != 0)
        cmd_transition(backend, command_buffer, destination, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    pass_trace_end(backend, command_buffer, span);
    return 1;
}
#endif

// ---- reading resolved images back ----

extern "C" void vulkan_cmd_copy_image_to_buffer(s_vulkan_backend *backend, s_vulkan_image *image, uint32_t level,
                                                uint32_t layer, int32_t x, int32_t y, uint32_t width, uint32_t height,
                                                s_vulkan_buffer *buffer, uint32_t offset)
{
    VkCommandBuffer command_buffer = recording_command_buffer(backend, "vulkan_cmd_copy_image_to_buffer");
    if (command_buffer == 0 || image == 0 || buffer == 0 || width == 0 || height == 0)
        return;
    begin_transfer(backend);
    const uint32_t layout = image->layout;
    cmd_transition(backend, command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    VkBufferImageCopy copy = {};
    copy.bufferOffset = offset;
    copy.imageSubresource.aspectMask = image->aspect;
    copy.imageSubresource.mipLevel = level;
    copy.imageSubresource.baseArrayLayer = layer;
    copy.imageSubresource.layerCount = 1;
    copy.imageOffset.x = x;
    copy.imageOffset.y = y;
    copy.imageExtent.width = width;
    copy.imageExtent.height = height;
    copy.imageExtent.depth = 1;
    backend->vk.vkCmdCopyImageToBuffer(command_buffer, image->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                                       buffer->buffer, 1, &copy);
    cmd_memory_barrier(backend, command_buffer, VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_ACCESS_2_TRANSFER_WRITE_BIT,
                       VK_PIPELINE_STAGE_2_HOST_BIT, VK_ACCESS_2_HOST_READ_BIT);
    // back to where it was (a resolved texture stays ready for sampling)
    if (layout != VK_IMAGE_LAYOUT_UNDEFINED)
        cmd_transition(backend, command_buffer, image, layout);
}

extern "C" VkResult vulkan_frame_wait(s_vulkan_backend *backend, uint32_t slot)
{
    if (slot >= backend->frame_count)
        return VK_ERROR_UNKNOWN;
    if (backend->recording && slot == backend->frame_index)
        return VK_NOT_READY; // not submitted yet
    VkResult result = backend->vk.vkWaitForFences(backend->device, 1, &backend->frames[slot].fence, VK_TRUE, ~0ull);
    if (result == VK_ERROR_DEVICE_LOST)
        report_device_loss(backend);
    return result;
}
