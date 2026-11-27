#ifndef INTERFACE_GUI_HEAP_H
#define INTERFACE_GUI_HEAP_H

// Integer heap-handle view used by the GUI widget allocation paths.
#if !defined(_XTL_) && !defined(_WINBASE_)
extern "C" int GetProcessHeap();
extern "C" unsigned char RtlFreeHeap(void *heap, unsigned long flags, void *memory);

#endif

#endif // INTERFACE_GUI_HEAP_H
