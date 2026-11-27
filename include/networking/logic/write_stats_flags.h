#ifndef NETWORKING_LOGIC_WRITE_STATS_FLAGS_H
#define NETWORKING_LOGIC_WRITE_STATS_FLAGS_H

extern "C" unsigned int _rotl(unsigned int value, int shift);

class c_write_stats_flags
{
public:
    c_write_stats_flags() : m_storage(0) {}
    short m_storage;
    __forceinline void clear() { m_storage = 0; }
    __forceinline bool test(long bit) const
    {
        bool result = (bool)(_rotl(m_storage, 32 - bit) & 1);
        return result;
    }
    __forceinline void set(long bit, bool enabled)
    {
        if (enabled)
            m_storage |= (short)(1 << bit);
        else
            m_storage &= (short)~(1 << bit);
    }
};

typedef char write_stats_flags_size[(sizeof(c_write_stats_flags) == 2) ? 1 : -1];

#endif // NETWORKING_LOGIC_WRITE_STATS_FLAGS_H
