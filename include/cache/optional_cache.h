#ifndef CACHE_OPTIONAL_CACHE_H
#define CACHE_OPTIONAL_CACHE_H

enum e_map_memory_configuration;

struct s_optional_cache_user_memory_configuration
{
    long maximum_priority;
    unsigned long maximum_size;
};

class c_optional_cache_user_callback
{
public:
    virtual void get_memory_configuration(e_map_memory_configuration configuration,
        s_optional_cache_user_memory_configuration *memory_configuration) = 0;
    virtual void idle(const void *memory) = 0;
    virtual void terminate(const void *memory) = 0;
};

typedef char optional_cache_user_memory_configuration_size[
    (sizeof(s_optional_cache_user_memory_configuration) == 8) ? 1 : -1];
typedef char optional_cache_user_callback_size[
    (sizeof(c_optional_cache_user_callback) == sizeof(void *)) ? 1 : -1];

#endif // CACHE_OPTIONAL_CACHE_H
