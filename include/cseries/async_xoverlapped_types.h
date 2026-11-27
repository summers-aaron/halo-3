#ifndef CSERIES_ASYNC_XOVERLAPPED_TYPES_H
#define CSERIES_ASYNC_XOVERLAPPED_TYPES_H

#include "cseries/cseries_wrapped.h"
#include "cseries/async_xoverlapped.h"

enum e_overlapped_task_state
{
    _overlapped_task_state_none,
    _overlapped_task_state_starting,
    _overlapped_task_state_pending,
    _overlapped_task_state_completing,
    _overlapped_task_state_success,
    _overlapped_task_state_failure
};

// March c_overlapped_task: methods and wrapper types read from all owned bodies.
// The base holds only flags and task state (8 bytes). Subclasses provide the
// context string and the start routine.
// Kept in a leaf until held users of async_xoverlapped.h shed their private copies.
class c_overlapped_task
{
public:
    __forceinline c_overlapped_task(const char *file, long line) : m_flags(0)
    {
        set_task_state_internal(_overlapped_task_state_none);
    }

    __forceinline virtual ~c_overlapped_task()
    {
        if (busy())
            overlapped_task_block_until_finished(this);
    }
    virtual const char *get_context_string() const = 0;
    virtual unsigned long start(void *overlapped) = 0;
    // Default callbacks do nothing unless overridden.
    virtual void update(unsigned long return_result);
    virtual void success(unsigned long return_result);
    virtual void failure(unsigned long calling_result, unsigned long overlapped_error,
        unsigned long overlapped_extended_error);
    virtual void complete();
    virtual void reset();
    virtual bool is_result_successful(unsigned long calling_result, unsigned long overlapped_error,
        unsigned long overlapped_extended_error);

    __forceinline bool busy() const
    {
        return m_task_state == _overlapped_task_state_starting
            || m_task_state == _overlapped_task_state_pending
            || m_task_state == _overlapped_task_state_completing;
    }
    __forceinline e_overlapped_task_state get_task_state() const { return m_task_state; }
    __forceinline void set_task_state_internal(e_overlapped_task_state state) { m_task_state = state; }
    __forceinline bool task_was_recycled_during_completion() const
    {
        return m_flags.test(_overlapped_task_flag_recycled_during_completion_bit);
    }
    __forceinline void task_recycled_during_completion(bool value)
    {
        m_flags.set(_overlapped_task_flag_recycled_during_completion_bit, value);
    }

    // March's debug-only metadata accessors read/write fields at 8 and 12. The
    // base holds no such fields; keep the accessors declared until a known
    // user establishes their release behaviour. operator= is compiler-generated.
    const char *get_file() const;
    long get_line() const;
    void set_file(const char *file);
    void set_line(long line);

protected:
    enum e_overlapped_task_flags
    {
        _overlapped_task_flag_recycled_during_completion_bit = 1
    };
    c_flags<e_overlapped_task_flags, short, 2> m_flags;
    c_enum<e_overlapped_task_state, short, 0, 6> m_task_state;
};

#if defined(_XBOX)
typedef char overlapped_task_size[(sizeof(void *) != 4 || ((sizeof(c_overlapped_task) == 8) )) ? 1 : -1];
#endif

#endif // CSERIES_ASYNC_XOVERLAPPED_TYPES_H
