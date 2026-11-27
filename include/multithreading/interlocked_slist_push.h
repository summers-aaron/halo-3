#ifndef INCLUDE_MULTITHREADING_INTERLOCKED_SLIST_PUSH_H
#define INCLUDE_MULTITHREADING_INTERLOCKED_SLIST_PUSH_H



#if defined(_WIN64)
struct _SLIST_HEADER;
struct _SLIST_ENTRY;
typedef _SLIST_HEADER *slist_push_header_ptr;
typedef _SLIST_ENTRY *slist_push_entry_ptr;
#else
union _SLIST_HEADER;
struct _SINGLE_LIST_ENTRY;
typedef _SLIST_HEADER *slist_push_header_ptr;
typedef _SINGLE_LIST_ENTRY *slist_push_entry_ptr;
#endif

#ifndef _XTL_
extern "C" void *__stdcall InterlockedPopEntrySList(void *);
extern "C" slist_push_entry_ptr __stdcall InterlockedPushEntrySList(slist_push_header_ptr, slist_push_entry_ptr);
#endif

extern "C++" {
template <typename t_header, typename t_entry>
__forceinline void *InterlockedPushEntrySList(t_header *header, t_entry *entry)
{
    return ::InterlockedPushEntrySList((slist_push_header_ptr)header, (slist_push_entry_ptr)entry);
}

}

#endif // INCLUDE_MULTITHREADING_INTERLOCKED_SLIST_PUSH_H
