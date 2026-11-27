// port/sdk/kernel (Linux host): time. The performance counter runs at the Xbox 360's time-base rate, 50 MHz, so
// the counter, its frequency and __mftb agree with each other and with the console.
#include <errno.h>
#include <sys/prctl.h>
#include <time.h>

#define PORT_TIME_BASE_HZ 50000000ull
#define PORT_SPIN_TICKS (PORT_TIME_BASE_HZ * 150ull / 1000000ull)

static unsigned long long port_time_base(void)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (unsigned long long)now.tv_sec * PORT_TIME_BASE_HZ + (unsigned long long)now.tv_nsec / 20;
}

int PORT_MS_ABI QueryPerformanceCounter(long long *count)
{
    *count = (long long)port_time_base();
    return 1;
}

int PORT_MS_ABI QueryPerformanceFrequency(long long *frequency)
{
    *frequency = (long long)PORT_TIME_BASE_HZ;
    return 1;
}

unsigned long PORT_MS_ABI GetTickCount(void)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (unsigned long)((unsigned long long)now.tv_sec * 1000ull + (unsigned long long)now.tv_nsec / 1000000ull);
}

unsigned long long PORT_MS_ABI __mftb(void)
{
    return port_time_base();
}

// FILETIME: 100 ns units since 1601-01-01, as two 32-bit words (low, high).
void PORT_MS_ABI GetSystemTimeAsFileTime(unsigned int *file_time)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    unsigned long long ticks = (unsigned long long)now.tv_sec * 10000000ull + (unsigned long long)now.tv_nsec / 100
        + 116444736000000000ull;
    file_time[0] = (unsigned int)ticks;
    file_time[1] = (unsigned int)(ticks >> 32);
}

int PORT_MS_ABI CompareFileTime(const unsigned int *first, const unsigned int *second)
{
    unsigned long long a = ((unsigned long long)first[1] << 32) | first[0];
    unsigned long long b = ((unsigned long long)second[1] << 32) | second[0];
    return a < b ? -1 : a > b ? 1 : 0;
}

// SYSTEMTIME: year, month, day of week, day, hour, minute, second, milliseconds (16 bits each).
void PORT_MS_ABI GetLocalTime(unsigned short *system_time)
{
    struct timespec now;
    struct tm local;
    clock_gettime(CLOCK_REALTIME, &now);
    localtime_r(&now.tv_sec, &local);
    system_time[0] = (unsigned short)(local.tm_year + 1900);
    system_time[1] = (unsigned short)(local.tm_mon + 1);
    system_time[2] = (unsigned short)local.tm_wday;
    system_time[3] = (unsigned short)local.tm_mday;
    system_time[4] = (unsigned short)local.tm_hour;
    system_time[5] = (unsigned short)local.tm_min;
    system_time[6] = (unsigned short)local.tm_sec;
    system_time[7] = (unsigned short)(now.tv_nsec / 1000000);
}

// Wait on the same monotonic clock as the 50 MHz performance counter. Wake 150 us early to absorb scheduler latency,
// then spin to the deadline. Keep the thread's timer slack at 1 us.
void PORT_MS_ABI port_sleep_until_counter(long long counter)
{
    if (counter <= 0)
        return;
    const unsigned long long deadline = (unsigned long long)counter;
    unsigned long long now = port_time_base();
    static __thread int slack_set;
    if (now < deadline && deadline - now > PORT_SPIN_TICKS) {
        if (!slack_set) {
            prctl(PR_SET_TIMERSLACK, 1000ul, 0ul, 0ul, 0ul);
            slack_set = 1;
        }
        const unsigned long long wake = deadline - PORT_SPIN_TICKS;
        const struct timespec when = {
            (time_t)(wake / PORT_TIME_BASE_HZ), (long)(wake % PORT_TIME_BASE_HZ * 20ull)
        };
        while (clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &when, 0) == EINTR) {
        }
        now = port_time_base();
    }
    while (now < deadline) {
        __builtin_ia32_pause();
        now = port_time_base();
    }
}
