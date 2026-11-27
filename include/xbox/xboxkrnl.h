#ifndef XBOX_XBOXKRNL_H
#define XBOX_XBOXKRNL_H

#ifdef __cplusplus
extern "C" {
#endif
long XexGetModuleHandle(char *module_name, void **module);
long XexGetModuleSection(void *module, const char *section_name, void **section_data, unsigned long *section_size);
void XeCryptBnQw_SwapDwQwLeBe(const void *input, void *output, unsigned long count);
int XeCryptBnQwNeRsaPubCrypt(const void *input, void *output, const void *key);
int XeCryptBnDwLePkcs1Verify(const void *hash, const void *signature, unsigned long size);
#ifdef __cplusplus
}
#endif

#endif // XBOX_XBOXKRNL_H
