#ifndef CACHE_SECURITY_SIGNATURE_H
#define CACHE_SECURITY_SIGNATURE_H

// Defined in security_functions.cpp; used by the cache file and BLF verifiers.
extern "C" bool security_verify_signature(const void *digest, void *signature);

#endif
