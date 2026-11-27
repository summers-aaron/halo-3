// blamlibxbox/files.obj: from the records 0x1aec40 0x1aecc0 0x1aed20 0x1aee60 0x228248
// @flags /TP /O2 /Oc
// @address 0x1aec40 ?file_reference_create_from_path@@YAPAUs_file_reference@@PAU1@PBD_N@Z
// @address 0x1aecc0 ?file_reference_set_name@@YAPAUs_file_reference@@PAU1@PBD@Z
// @address 0x1aed20 ?file_reference_get_name@@YAPADPBUs_file_reference@@KPADJ@Z
// @address 0x1aee60 ?file_create_parent_directories_if_not_present@@YA_NPBUs_file_reference@@@Z
// @address 0x228248 file_initialize_save_directory
#include <string.h>
#include "tag_files/files.h"
#include "tag_files/files_windows.h"

// @address 0x58e5c0 memset

#include "tag_files/file_reference.h"
#include "interface/gui_pregame_selection_transfer_storage.h"

extern "C" {
char g_pregame_selection_file_transfer_storage[800];
}

// @address 0x3e674 k_saved_game_autosave_directory
// @rdata 0x8203e674..0x8203e688
extern "C" {
const char k_saved_game_autosave_directory[20] = "cache1:\\autosave";
}
// @rdata-end

s_file_reference *file_reference_create_from_path(s_file_reference *reference, const char *path, bool is_directory)
{
    memset(reference, 0, sizeof(*reference));
    reference->signature = 0x66696c6f;
    reference->location = -1;
    reference->handle = (void *)-1;
    if (is_directory) {
        file_path_add_name(reference->path, 0x100, path);
        return reference;
    }
    file_reference_set_name(reference, path);
    return reference;
}

s_file_reference *file_reference_set_name(s_file_reference *reference, const char *name)
{
    unsigned short *flags = &reference->flags;
    if ((*flags & 1) != 0) {
        file_path_remove_name(reference->path);
    }
    file_path_add_name(reference->path, 0x100, name);
    *flags |= 1;
    return reference;
}

char *file_reference_get_name(const s_file_reference *reference, unsigned long parts, char *buffer, long size)
{
    struct { char *root; char *drive; char *extension; char *directory; } p;
    char path[256] = {0};
    file_location_get_full_path(reference->location, reference->path, path, 256);
    file_path_split(path, &p.drive, &p.directory, &p.root, &p.extension, reference->flags & 1);
    buffer[0] = 0;
    if (parts & 1)
        file_path_add_name(buffer, 256, p.drive);
    if (parts & 2)
        file_path_add_name(buffer, 256, p.directory);
    if (parts & 4)
        file_path_add_name(buffer, 256, p.root);
    if ((parts & 8) && *p.extension) {
        int length = strlen(buffer);
        char *extension = p.extension;
        char *end = buffer + length;
        if (end != buffer) {
            *end++ = '.';
            *end = 0;
            length++;
        }
        int remaining = 256 - length;
        strncpy(end, extension, remaining);
        end[remaining - 1] = 0;
    }
    return buffer;
}

bool file_create_parent_directories_if_not_present(const s_file_reference *reference)
{
    char directory_path[256];
    char full_path[256];
    s_file_reference directory_reference;
    bool success = true;
    file_reference_get_name(reference, 1, full_path, sizeof(full_path));
    char *component_start = full_path;
    for (;;)
    {
        bool is_network_root = false;
        char *component_end = component_start;
        while (*component_end == '\\')
            ++component_end;
        if (component_end == component_start + 2
            && component_end[-2] == '\\' && component_end[-1] == '\\')
            is_network_root = true;
        while (*component_end && *component_end != '\\')
            ++component_end;
        while (*component_end == '\\')
            ++component_end;
        if (component_end == component_start)
            break;
        component_start = component_end;
        if (is_network_root)
            continue;
        int directory_length = component_end - full_path;
        unsigned int copy_length = directory_length + 1;
        strncpy(directory_path, full_path, copy_length);
        directory_path[copy_length - 1] = 0;
        if (directory_path[directory_length - 1] != '\\')
        {
            directory_path[directory_length] = '\\';
            directory_path[directory_length + 1] = 0;
        }
        file_reference_create_from_path(&directory_reference, directory_path, true);
        if (!file_exists(&directory_reference))
        {
            success = file_create(&directory_reference);
            if (!success)
                break;
        }
    }
    return success;
}

// 0x228248
void file_initialize_save_directory(void)
{
    s_file_reference reference;

    file_create_parent_directories_if_not_present(file_reference_create_from_path(&reference, k_saved_game_autosave_directory, true));
    g_pregame_selection_file_transfer_storage[0] = false;
}

