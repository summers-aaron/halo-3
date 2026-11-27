#ifndef MULTITHREADING_SYNCHRONIZED_VALUE_H
#define MULTITHREADING_SYNCHRONIZED_VALUE_H

#include <stddef.h>
#include <ppcintrinsics.h>

extern "C"
{
    long _InterlockedExchange(volatile long *, long);
    long _InterlockedCompareExchange(volatile long *, long, long);
    long _InterlockedExchangeAdd(volatile long *, long);
    long _InterlockedIncrement(volatile long *);
    long _InterlockedDecrement(volatile long *);
    long _InterlockedAnd(volatile long *, long);
    long _InterlockedOr(volatile long *, long);
    long _InterlockedXor(volatile long *, long);
}
// No #pragma intrinsic here: this compiler already emits every _Interlocked*
// call below inline (lwarx/stwcx. retry loops) at /O1 and /O2, so the pragma
// spellings changed no standalone listing with or without them.

// March synchronized_value.obj emits these operations; unlike c_synchronized_long,
// c_interlocked_long does not add an lwsync around the atomic operation.
class c_interlocked_long
{
public:
    c_interlocked_long() : value(0) {}
    c_interlocked_long(long initial_value) : value(initial_value) {}
    ~c_interlocked_long() {}

    long set(long new_value) { return _InterlockedExchange(&value, new_value); }
    long set_if_equal(long new_value, long comparison_value)
    {
        return _InterlockedCompareExchange(&value, new_value, comparison_value);
    }
    long peek() const { return value; }
    long add(long amount) { return _InterlockedExchangeAdd(&value, amount); }
    long increment() volatile { return _InterlockedIncrement(&value); }
    long decrement() volatile { return _InterlockedDecrement(&value); }
    long bitwise_and(long mask) { return _InterlockedAnd(&value, mask); }
    long bitwise_or(long mask) { return _InterlockedOr(&value, mask); }
    long bitwise_xor(long mask) { return _InterlockedXor(&value, mask); }
    long set_bit(long bit, bool enabled)
    {
        return enabled ? bitwise_or(1 << bit) : bitwise_and(~(1 << bit));
    }
    bool test_bit(long bit) const
    {
        long current_value = peek();
        return (current_value & (1 << bit)) != 0;
    }
    operator long() const { return peek(); }

    volatile long value;
};

typedef char c_interlocked_long_size[(sizeof(c_interlocked_long) == 4) ? 1 : -1];
typedef char c_interlocked_long_value_offset[(offsetof(c_interlocked_long, value) == 0) ? 1 : -1];

// March's header string names multithreading/synchronized_value.h.
// Methods and ordering within their bodies are from the March debug build.
class c_synchronized_long
{
public:
    c_synchronized_long() : value(0) {}
    struct s_defer_initialization {};
    explicit c_synchronized_long(s_defer_initialization) {}
    c_synchronized_long(long initial_value) : value(initial_value) {}
    ~c_synchronized_long() {}

    long set(long new_value)
    {
        __lwsync();
        return _InterlockedExchange(&value, new_value);
    }

    long set_if_equal(long new_value, long comparison_value)
    {
        __lwsync();
        return _InterlockedCompareExchange(&value, new_value, comparison_value);
    }

    long peek() const
    {
        long result = value;
        __lwsync();
        return result;
    }

    long add(long amount)
    {
        __lwsync();
        return _InterlockedExchangeAdd(&value, amount);
    }

    long increment()
    {
        __lwsync();
        return _InterlockedIncrement(&value);
    }

    long decrement()
    {
        __lwsync();
        return _InterlockedDecrement(&value);
    }

    // March spells these methods and/or/xor, which are standard C++ keywords.
    long bitwise_and(long mask)
    {
        __lwsync();
        return _InterlockedAnd(&value, mask);
    }

    long bitwise_or(long mask)
    {
        __lwsync();
        return _InterlockedOr(&value, mask);
    }

    long bitwise_xor(long mask)
    {
        __lwsync();
        return _InterlockedXor(&value, mask);
    }

    // Defined out of line in synchronized_value.cpp.
    // March: return enabled ? bitwise_or(1 << bit) : bitwise_and(~(1 << bit));
    long set_bit(long bit, bool enabled);

    bool test_bit(long bit) const
    {
        long current_value = peek();
        return (current_value & (1 << bit)) != 0;
    }

    operator long() const { return peek(); }
    c_synchronized_long &operator=(long new_value) { set(new_value); return *this; }
    c_synchronized_long &operator=(bool new_value) { set(new_value != 0); return *this; }
    c_synchronized_long &operator=(c_synchronized_long &other) { set(other.value); return *this; }
    c_synchronized_long &operator+=(long amount) { add(amount); return *this; }
    c_synchronized_long &operator-=(long amount) { add(-amount); return *this; }
    c_synchronized_long &operator&=(long mask) { bitwise_and(mask); return *this; }
    c_synchronized_long &operator|=(long mask) { bitwise_or(mask); return *this; }

    volatile long value; // synchronized 32-bit integer
};

typedef char c_synchronized_long_size[(sizeof(c_synchronized_long) == 4) ? 1 : -1];
typedef char c_synchronized_long_value_offset[(offsetof(c_synchronized_long, value) == 0) ? 1 : -1];

extern "C"
{
    __int64 _InterlockedExchange64(volatile __int64 *, __int64);
    __int64 _InterlockedCompareExchange64(volatile __int64 *, __int64, __int64);
    __int64 _InterlockedExchangeAdd64(volatile __int64 *, __int64);
    __int64 _InterlockedIncrement64(volatile __int64 *);
    __int64 _InterlockedDecrement64(volatile __int64 *);
}

// March cache_file_tag_resource_runtime.obj, constructor 0x8298ea00, clears the
// single 64-bit value. set (0x8293c7b8) and peek (0x82e600e0) add no lwsync.
class c_interlocked_int64
{
public:
    __forceinline c_interlocked_int64() : value(0) {}
    __forceinline __int64 set(__int64 new_value) { return _InterlockedExchange64(&value, new_value); }
    __forceinline __int64 peek() const { return value; }

    volatile __int64 value;
};

typedef char c_interlocked_int64_size[(sizeof(c_interlocked_int64) == 8) ? 1 : -1];
typedef char c_interlocked_int64_value_offset[(offsetof(c_interlocked_int64, value) == 0) ? 1 : -1];

// March synchronized_value.h: the 64-bit twin of c_synchronized_long (set/peek/add/increment each lwsync, then the
// ldarx/stdcx. sequence; operator= reads the other value raw and calls set; operator __int64 is peek). March also
// has and/or/xor methods, omitted here as unused (and/or/xor are standard C++ keywords).
class c_synchronized_int64
{
public:
    c_synchronized_int64() : value(0) {}
    ~c_synchronized_int64() {}

    __int64 set(__int64 new_value)
    {
        __lwsync();
        return _InterlockedExchange64(&value, new_value);
    }

    __int64 set_if_equal(__int64 new_value, __int64 comparison_value)
    {
        __lwsync();
        return _InterlockedCompareExchange64(&value, new_value, comparison_value);
    }

    __int64 peek() const
    {
        __int64 result = value;
        __lwsync();
        return result;
    }

    __int64 add(__int64 amount)
    {
        __lwsync();
        return _InterlockedExchangeAdd64(&value, amount);
    }

    __int64 increment()
    {
        __lwsync();
        return _InterlockedIncrement64(&value);
    }

    __int64 decrement()
    {
        __lwsync();
        return _InterlockedDecrement64(&value);
    }

    operator __int64() const { return peek(); }
    c_synchronized_int64 &operator=(c_synchronized_int64 &other) { set(other.value); return *this; }

    volatile __int64 value; // synchronized 64-bit integer
};

typedef char c_synchronized_int64_size[(sizeof(c_synchronized_int64) == 8) ? 1 : -1];

#endif // MULTITHREADING_SYNCHRONIZED_VALUE_H
