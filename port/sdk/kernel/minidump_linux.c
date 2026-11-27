// port/sdk/kernel (Linux host): the loaded-module and address-space queries of the game's minidump writer and tag
// resource thread lock (cseries/xenon_minidump.cpp, cache/tag_resource_cache_thread_lock.cpp). Retail answers them
// from the Xbox kernel (0x826f2c18..0x826f33f0: the loader's module list, each module's PE section table, the page
// protections of the title's address ranges); here the process's loaded objects (dl_iterate_phdr: the executable,
// then its shared libraries), their PT_LOAD segments, and /proc/self/maps.
#define _GNU_SOURCE
#include <link.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef unsigned int port_dword;

typedef struct port_minidump_module_info { // s_xenon_minidump_module_info, 280 bytes
    port_dword image_base, image_size, timestamp, checksum, flags; // flags bit 0: a shared library (a DLL)
    char filename[260];
} port_minidump_module_info;

typedef struct port_module_section { // the PE section record retail reads: address, size, name
    port_dword address, size;
    char name[8];
} port_module_section;

typedef struct port_memory_range {
    port_dword address, size, protection; // protection: 4 (PAGE_READWRITE) as retail reports
} port_memory_range;

typedef struct port_module_query {
    long wanted, index;
    port_minidump_module_info *info;
    long section;               // >= 0: the section wanted
    port_module_section *section_out;
    long section_count;
    int found;
} port_module_query;

static const char *port_module_name(const struct dl_phdr_info *module, char *buffer, size_t size)
{
    if (module->dlpi_name != 0 && module->dlpi_name[0] != '\0')
        return module->dlpi_name;
    ssize_t length = readlink("/proc/self/exe", buffer, size - 1);
    buffer[length > 0 ? length : 0] = '\0';
    return buffer;
}

static int port_module_visit(struct dl_phdr_info *module, size_t size, void *parameter)
{
    (void)size;
    port_module_query *query = (port_module_query *)parameter;
    if (query->index++ != query->wanted)
        return 0;
    query->found = 1;
    unsigned long low = ~0ul, high = 0;
    long loads = 0;
    for (int i = 0; i < module->dlpi_phnum; i++) {
        const ElfW(Phdr) *segment = &module->dlpi_phdr[i];
        if (segment->p_type != PT_LOAD)
            continue;
        const unsigned long start = module->dlpi_addr + segment->p_vaddr, end = start + segment->p_memsz;
        if (start < low)
            low = start;
        if (end > high)
            high = end;
        if (query->section_out != 0 && loads == query->section) {
            query->section_out->address = start;
            query->section_out->size = segment->p_memsz;
            const char *name = (segment->p_flags & PF_X) ? ".text" : (segment->p_flags & PF_W) ? ".data" : ".rdata";
            memset(query->section_out->name, 0, sizeof(query->section_out->name));
            memcpy(query->section_out->name, name, strlen(name));
        }
        loads++;
    }
    query->section_count = loads;
    if (query->info != 0) {
        memset(query->info, 0, sizeof(*query->info));
        query->info->image_base = low;
        query->info->image_size = high > low ? high - low : 0;
        query->info->flags = query->wanted != 0 ? 1u : 0u;
        char path[260];
        snprintf(query->info->filename, sizeof(query->info->filename), "%s", port_module_name(module, path, sizeof(path)));
    }
    return 1;
}

static int port_module_query_run(port_module_query *query)
{
    query->index = 0;
    query->found = 0;
    dl_iterate_phdr(port_module_visit, query);
    return query->found;
}

static int port_module_count_visit(struct dl_phdr_info *module, size_t size, void *parameter)
{
    (void)module; (void)size;
    ++*(long *)parameter;
    return 0;
}

long long PORT_MS_ABI xenon_minidump_get_loaded_module_count(void)
{
    long count = 0;
    dl_iterate_phdr(port_module_count_visit, &count);
    return count;
}

int PORT_MS_ABI xenon_minidump_get_module_info(int module, port_minidump_module_info *info)
{
    memset(info, 0, sizeof(*info));
    port_module_query query = { module, 0, info, -1, 0, 0, 0 };
    return module >= 0 && port_module_query_run(&query);
}

int PORT_MS_ABI tag_resource_cache_module_section_count(int module)
{
    port_module_query query = { module, 0, 0, -1, 0, 0, 0 };
    return module >= 0 && port_module_query_run(&query) ? (int)query.section_count : 0;
}

int PORT_MS_ABI tag_resource_cache_module_get_section(int module, unsigned int section_index, port_module_section *section)
{
    port_module_query query = { module, 0, 0, (long)section_index, section, 0, 0 };
    return module >= 0 && port_module_query_run(&query) && (long)section_index < query.section_count;
}

// The next readable range at or above `address` (a handle: 0 starts the walk) in the 32-bit address space: fills
// *range and returns where the walk continues (the range's end), or 0 when no readable memory is left.
int PORT_MS_ABI tag_resource_cache_next_readable_memory_range(int address, port_memory_range *range)
{
    range->address = range->size = range->protection = 0;
    FILE *maps = fopen("/proc/self/maps", "r");
    if (maps == 0)
        return 0;
    const unsigned long from = (unsigned long)(unsigned int)address;
    unsigned long start, end, result = 0;
    char permissions[8], line[512];
    while (fgets(line, sizeof(line), maps) != 0) {
        if (sscanf(line, "%lx-%lx %7s", &start, &end, permissions) != 3 || permissions[0] != 'r' || end <= from)
            continue;
        if (start < from)
            start = from;
        range->address = start;
        range->size = end - start;
        range->protection = 4;
        result = end;
        break;
    }
    fclose(maps);
    return (int)result;
}
