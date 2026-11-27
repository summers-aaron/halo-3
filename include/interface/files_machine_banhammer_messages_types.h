#ifndef INTERFACE_FILES_MACHINE_BANHAMMER_MESSAGES_TYPES_H
#define INTERFACE_FILES_MACHINE_BANHAMMER_MESSAGES_TYPES_H

// 'mapm' chunk payload (major version 1) of rsa_manifest.bin, fetched by the banhammer machine downloader.
// network_banhammer_update_map_manifest bounds count by 128 and copies the whole payload into
// s_network_banhammer_map_signatures::count.
struct s_files_machine_banhammer_messages
{
    long count;
    unsigned char signatures[128][256];
};
typedef char files_machine_banhammer_messages_size_check[(sizeof(s_files_machine_banhammer_messages) == 32772) ? 1 : -1];

#endif // INTERFACE_FILES_MACHINE_BANHAMMER_MESSAGES_TYPES_H
