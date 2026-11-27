#ifndef CSERIES_SORT_H
#define CSERIES_SORT_H

// Functions defined in tu/blamlibxbox/sort.cpp.

long bsearch_elements(const void *key, const void *base, long count, long element_size,
    long (*compare)(const void *, const void *, const void *), const void *context);
extern "C" void shortsort_elements(char *first,char *last,unsigned int width,bool aligned);

void qsort_elements(void *base, unsigned int count, unsigned int width, bool (*compare)(const void *, const void *, const void *), const void *context);

#endif // CSERIES_SORT_H
