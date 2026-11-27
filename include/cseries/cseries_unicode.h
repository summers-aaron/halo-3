#ifndef CSERIES_CSERIES_UNICODE_H
#define CSERIES_CSERIES_UNICODE_H

int usnzprintf(wchar_t *buffer, long size, const wchar_t *format, ...);


// Shared wide format strings.
extern "C" const wchar_t k_wide_integer_format[];

#endif // CSERIES_CSERIES_UNICODE_H
