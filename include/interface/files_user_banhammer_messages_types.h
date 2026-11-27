#ifndef INTERFACE_FILES_USER_BANHAMMER_MESSAGES_TYPES_H
#define INTERFACE_FILES_USER_BANHAMMER_MESSAGES_TYPES_H

// 'bhms' chunk payload (major version 1). The banhammer reader bounds
// message_count by 32 and reads 256-byte UTF-8 messages from offset 4.
struct s_files_user_banhammer_messages
{
    long message_count;
    char messages[32][256];
};
typedef char files_user_banhammer_messages_size_check[(sizeof(s_files_user_banhammer_messages) == 8196) ? 1 : -1];

#endif // INTERFACE_FILES_USER_BANHAMMER_MESSAGES_TYPES_H
