#ifndef INTERFACE_USER_INTERFACE_TIME_H
#define INTERFACE_USER_INTERFACE_TIME_H

template<long k_maximum_count> class c_static_wchar_string;

extern "C" bool user_interface_get_timespan_string(long seconds, long format, bool leading_zero, c_static_wchar_string<1024> *output);

#endif // INTERFACE_USER_INTERFACE_TIME_H
