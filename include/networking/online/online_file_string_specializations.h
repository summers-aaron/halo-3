#ifndef NETWORKING_ONLINE_ONLINE_FILE_STRING_SPECIALIZATIONS_H
#define NETWORKING_ONLINE_ONLINE_FILE_STRING_SPECIALIZATIONS_H

#include "memory/static_arrays.h"

template<>
bool c_static_string<1024>::starts_with(const char *prefix) const;
template<>
void c_static_string<1024>::trim_end(void);

extern template const char *c_static_string<256>::print(const char *format, ...);
extern template const char *c_static_string<48>::print(const char *format, ...);
template<> long c_static_wchar_string<256>::length() const;
extern template const char *c_static_string<65>::append(const char *source);
template<> void c_static_wchar_string<1024>::set(const wchar_t *string);

#endif
