// blamlibxbox/security_functions.obj: from the records 0x90020 0x900c8 0x19f058 0x19f0c0 0x19f1d0 0x211440
// @flags /TP /O2
#include <xtl.h>
#include "memory/static_arrays.h"
#include "cache/security_functions.h"
#include "cache/security_signature.h"
#include "networking/session/network_session_parameters.h"
#include "xbox/xboxkrnl.h"
#include "xapi/rtlerror.h"
// @address 0x19f0c0 security_initialize_hash
// @address 0x19f1d0 security_verify_signature
// @address 0x211440 ?length@?$c_static_string@$0EAA@@@QBAJXZ
// @address 0x59e4f0 XHashMemoryBegin
// @address 0x59e4f8 XHashMemoryUpdate
// @address 0x59e500 XHashMemoryEnd
// @address 0x70dd14 XeCryptBnDwLePkcs1Verify

// Offset and size of each signed block of the "01" module section, two words per block.
// @rdata 0x8203aa80 alias
static const unsigned long k_security_section_entries[4] = { 0, 0x3800, 0x3800, 0x6800 };

// match: 0x900c8: /Os joins both returns into one shared epilogue instead of an early blr
#pragma optimize("s", on)
// @address 0x900c8 security_get_module_handle
void *security_get_module_handle(const char *module_name)
{
    void *module;
    long status = XexGetModuleHandle((char *)module_name, &module);
    if (status < 0) {
        RtlSetLastNTError(status);
        return 0;
    }
    return module;
}
// match: 0x900c8: ends the /Os region opened for security_get_module_handle
#pragma optimize("", on)


// @address 0x19f058 ?security_calculate_hash@@YA_NPBDK_NPAUs_network_http_request_hash@@@Z
bool security_calculate_hash(const char *data, unsigned long size, bool salted, s_network_http_request_hash *hash)
{
    XHASH_STATE state;
    bool result = false;
    if (security_initialize_hash(&state, 88, salted)) {
        XHashMemoryUpdate(&state, data, size);
        XHashMemoryEnd(&state, hash->hash);
        result = true;
    }
    return result;
}

// 0x19f0c0
bool security_initialize_hash(XHASH_STATE *state, int size, bool salted)
{
    const void *section_data = 0;
    unsigned long section_data_size;
    void *module;
    bool result = false;

    {
        void *data = 0;
        section_data_size = 0;
        module = security_get_module_handle(0);
        if (module != 0 && XGetModuleSection(module, "00", &data, &section_data_size) != 0)
            section_data = data;
    }
    if (section_data != 0 && section_data_size == 0x138) {
        XHashMemoryBegin(state);
        if (salted)
            XHashMemoryUpdate(state, section_data, 34);
        result = true;
    }
    return result;
}

// 0x19f1d0
extern "C" bool security_verify_signature(const void *digest, void *signature)
{
    const unsigned char *section = 0;
    unsigned long section_data_size;
    void *module;
    bool result = false;

    {
        void *data = 0;
        section_data_size = 0;
        module = security_get_module_handle(0);
        if (module != 0 && XGetModuleSection(module, "00", &data, &section_data_size) != 0)
            section = (const unsigned char *)data;
    }
    if (section != 0 && section_data_size == 0x138) {
        XeCryptBnQw_SwapDwQwLeBe(signature, signature, 32);
        if (XeCryptBnQwNeRsaPubCrypt(signature, signature, section + 40) != 0) {
            XeCryptBnQw_SwapDwQwLeBe(signature, signature, 32);
            if (XeCryptBnDwLePkcs1Verify(digest, signature, 0x100) != 0)
                result = true;
        }
    }
    return result;
}


template<long k_maximum_count> long c_static_string<k_maximum_count>::length() const
{
    const char *string = m_string;
    unsigned long length;
    for (length = 0; length < (unsigned long)k_maximum_count; length++) {
        if (*string++ == '\0')
            break;
    }
    return length;
}
template long c_static_string<1024>::length() const;

// @address 0x90020 security_get_section_data
// match: 0x90020: pointer __restrict qualifier prevents alias reload
void security_get_section_data(long index, const void **address, unsigned long *__restrict size)
{
    void *section_data = 0;
    unsigned long section_data_size = 0;
    void *module = security_get_module_handle(0);
    if (module != 0)
        XGetModuleSection(module, "01", &section_data, &section_data_size);
    if (section_data != 0 && (int)section_data_size >= 0xa000) {
        const char *section_address = (const char *)section_data + k_security_section_entries[index * 2];
        *size = k_security_section_entries[index * 2 + 1];
        *address = section_address;
        return;
    }
    *address = 0;
    *size = 0;
}
