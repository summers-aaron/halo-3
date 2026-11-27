#ifndef NETWORKING_TOOLS_BLF_AUTHENTICATION_TYPES_H
#define NETWORKING_TOOLS_BLF_AUTHENTICATION_TYPES_H

enum e_blf_file_authentication_type
{
    _blf_file_authentication_none = 0,
    _blf_file_authentication_crc = 1,
    _blf_file_authentication_hash = 2,
    _blf_file_authentication_signature = 3,
};

#endif // NETWORKING_TOOLS_BLF_AUTHENTICATION_TYPES_H
