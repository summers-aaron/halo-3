#ifndef XBOX_XECRYPT_H
#define XBOX_XECRYPT_H

// Kernel crypto exports (xboxkrnl.exe) the game calls through its import stubs. The AES state is the kernel's
// XECRYPT_AES_STATE (352 bytes); the game keeps it as raw bytes.

#ifdef __cplusplus
extern "C" {
#endif
void XeCryptRandom(void *buffer, unsigned long size);
void XeCryptBnDwLePkcs1Format(const void *hash, long salt, void *buffer, long size);
void XeCryptAesKey(void *aes_state, const void *key);
void XeCryptAesCbc(void *aes_state, const void *input, unsigned long size, void *output, unsigned char *feedback,
                   int encrypt);
#ifdef __cplusplus
}
#endif

#endif // XBOX_XECRYPT_H
