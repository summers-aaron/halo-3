// xapilib/pix.obj: from the records 0x192688
// @flags /TP /O1 /Oc

#include "xapi/pix.h"

// A marker as the debug monitor receives it: one header word (word count << 24 | 22), then the text.
union s_marker_packet
{
    int words[12];
    struct
    {
        int header;
        char text[44];
    };
};

// @address 0x192688 pix_set_marker
void pix_set_marker(const char *format, va_list *arguments)
{
    s_marker_packet marker_packet;
    char *out = marker_packet.text;
    int literal_count = 0;
    ptrdiff_t source_offset = format - marker_packet.text;
    unsigned char ch;
    int words;
    int remaining;
    int *word;
    pix_debug_monitor_callback *monitor;

    do {
        ch = (unsigned char)out[source_offset];
        if (ch == 0) break;
        if (ch == '%') {
            out = marker_packet.text + RtlVsnprintf(marker_packet.text, 31, format, *arguments);
            if (out < marker_packet.text) out = marker_packet.text + 31;
            break;
        }
        literal_count = literal_count + 1;
        *out = (char)ch;
        out = out + 1;
    } while (literal_count < 31);

    *out = 0;
    // words in the header plus the text and its terminator, rounded up
    words = (unsigned int)(out - (marker_packet.text - sizeof(marker_packet.header)) + 4) >> 2;
    marker_packet.header = (words << 24) | 22;
    if (words == 0) return;

    word = marker_packet.words;
    remaining = words;
    do {
        monitor = KeDebugMonitorData;
        if (monitor != 0)
            monitor[6](39, *word);
        remaining = remaining - 1;
        word = word + 1;
    } while (remaining != 0);
}
