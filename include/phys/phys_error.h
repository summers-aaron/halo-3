#ifndef INCLUDE_PHYS_ERROR_H
#define INCLUDE_PHYS_ERROR_H

#include "phys/phys_singleton.h"

// March 2007: hkError's constructor calls hkSingleton<hkError>; its section hooks are empty.
class phys_error : public phys_singleton<phys_error> {
public:
    enum error_message {
        MESSAGE_REPORT,
        MESSAGE_WARNING,
        MESSAGE_ASSERT,
        MESSAGE_ERROR
    };

    virtual int message(error_message severity, int id, const char *description, const char *file, int line) = 0;
    virtual void error_set_enabled(int id, phys_bool enabled) = 0;
    virtual phys_bool is_error_enabled(int id) = 0;
    virtual void enable_all_reports() = 0;
    virtual void begin_section(int id, const char *section_name) {}
    virtual void end_section() {}
};

typedef char havok_size_hkError[(sizeof(void *) != 4 || ((sizeof(phys_error) == 8) )) ? 1 : -1];

#endif // INCLUDE_HAVOK_HK_BASE_ERROR_H
