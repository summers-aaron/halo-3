// blamlibxbox/sort.obj: from the records 0xf58a8 0x115a00 0x163258 0x1f0ac0 0x1f0dd8
// @flags /TP /O2 /Oc
#include <stddef.h>
#include "cseries/sort.h"
#include "cseries/long_sort.h"
#include "ai/ai_releasable_entry.h"
// @address 0x115a00 ?qsort@?$templated_sort@J@@SAXPAJKP6A_NJJPBX@Z1@Z
// @address 0x163258 ?qsort@?$templated_sort@F@@SAXPAFKP6A_NFFPBX@Z1@Z

// shortsort of 12-byte records; its caller 0x1f0ac0 passes width 12 and the
// ai priority compare

template <class T> static __forceinline void shortsort(T *lo, T *hi, bool (*compare)(T, T, const void *), const void *context) {
 while(hi>lo) {
  T *maximum=lo;
  for(T *candidate=lo+1;candidate<=hi;++candidate) if(compare(*candidate,*maximum,context)) maximum=candidate;
  T swap_value=*maximum; *maximum=*hi; *hi=swap_value;
  --hi;
 }
}

// @address 0xf58a8 ?bsearch_elements@@YAJPBX0JJP6AJ000@Z0@Z
long bsearch_elements(const void *key, const void *base, long count, long element_size,
                      long (*compare)(const void *, const void *, const void *), const void *context)
{
    const char *lower = (const char *)base;
    for (long remaining = count; remaining != 0; remaining >>= 1) {
        const char *middle = lower + (remaining >> 1) * element_size;
        long comparison = compare(key, middle, 0);
        if (comparison == 0) {
            return (middle - (const char *)base) / element_size;
        } else if (comparison > 0) {
            lower = middle + element_size;
            --remaining;
        }
    }
    return -1;
}

template<> void templated_sort<long>::qsort(long *base, unsigned long count, bool (*compare)(long, long, const void *), const void *context) {
 long stack_index=0;
 long *lo=base;
 long *hi=base+count-1;
 long *lower_stack[30],*upper_stack[30];
 long swap_value;
 for(;;) {
  unsigned long size=hi-lo+1;
  if(size<=8) shortsort(lo,hi,compare,context);
  else {
   long *pivot=lo+size/2;
   swap_value=*pivot; *pivot=*lo; *lo=swap_value;
   long *low=lo;
   long *high=hi+1;
   for(;;) {
    do { ++low; } while(low<=hi && !compare(*low,*lo,context));
    do { --high; } while(high>lo && !compare(*lo,*high,context));
    if(high<low) break;
    swap_value=*low; *low=*high; *high=swap_value;
   }
   swap_value=*lo; *lo=*high; *high=swap_value;
   if(high-1-lo>=hi-low) {
    if(lo+1<high) { lower_stack[stack_index]=lo; upper_stack[stack_index]=high-1; ++stack_index; }
    if(low<hi) { lo=low; continue; }
   } else {
    if(low<hi) { lower_stack[stack_index]=low; upper_stack[stack_index]=hi; ++stack_index; }
    if(lo+1<high) { hi=high-1; continue; }
   }
  }
  --stack_index;
  if(stack_index<0) break;
  lo=lower_stack[stack_index]; hi=upper_stack[stack_index];
 }
}

template<> void templated_sort<short>::qsort(short *base, unsigned long count, bool (*compare)(short, short, const void *), const void *context) {
 long stack_index=0;
 short *lo=base;
 short *hi=base+count-1;
 short *lower_stack[30],*upper_stack[30];
 short swap_value;
 for(;;) {
  unsigned long size=hi-lo+1;
  if(size<=8) shortsort(lo,hi,compare,context);
  else {
   short *pivot=lo+size/2;
   swap_value=*pivot; *pivot=*lo; *lo=swap_value;
   short *low=lo;
   short *high=hi+1;
   for(;;) {
    do { ++low; } while(low<=hi && !compare(*low,*lo,context));
    do { --high; } while(high>lo && !compare(*lo,*high,context));
    if(high<low) break;
    swap_value=*low; *low=*high; *high=swap_value;
   }
   swap_value=*lo; *lo=*high; *high=swap_value;
   if(high-1-lo>=hi-low) {
    if(lo+1<high) { lower_stack[stack_index]=lo; upper_stack[stack_index]=high-1; ++stack_index; }
    if(low<hi) { lo=low; continue; }
   } else {
    if(low<hi) { lower_stack[stack_index]=low; upper_stack[stack_index]=hi; ++stack_index; }
    if(lo+1<high) { hi=high-1; continue; }
   }
  }
  --stack_index;
  if(stack_index<0) break;
  lo=lower_stack[stack_index]; hi=upper_stack[stack_index];
 }
}

// @address 0x1f0ac0 ?qsort_elements@@YAXPAXIIP6A_NPBX11@Z1@Z
static __forceinline bool sort_compare_entries(const void *first, const void *second, const void *context) {
 const s_ai_releasable_entry *a=(const s_ai_releasable_entry *)first,*b=(const s_ai_releasable_entry *)second;
 long pa=a->release_index,pb=b->release_index;
 if(pb<pa)return true;
 if(pb>pa)return false;
 return (unsigned char)a->is_actor<(unsigned char)b->is_actor;
}
typedef void (*shortsort6_t)(char *,char *,unsigned int,bool,bool (*)(const void *, const void *, const void *),const void *);
static __forceinline void sort_swap_elements(char *first,char *second,unsigned int width,bool aligned) {
 if(aligned) {
  int count=width/4;
  long *a=(long *)first,*b=(long *)second;
  do {long temp=*a;*a++=*b;*b++=temp;} while(--count>0);
 } else {
  int count=width;
  char *a=first,*b=second;
  do {char temp=*a;*a++=*b;*b++=temp;} while(--count>0);
 }
}
void qsort_elements(void *base,unsigned int count,unsigned int width,bool (*compare)(const void *, const void *, const void *),const void *context) {
 // match: 0x1f0ac0: width==12 folds span divwu to mulhwu magic + li r5,12; removal emits divwu, mr args, width==0/align guard branches.
 __assume(width==12);
 bool (*volatile *home_compare)(const void *, const void *, const void *)=&compare;
 *home_compare=compare;
 compare=(bool (*)(const void *, const void *, const void *))compare_releasable_inactive_squads;
 if(count<2 || width==0)return;
 long stack=0;
 bool aligned=(width%4==0 && (size_t)base%4==0);
 char *lo=(char *)base;
 char *hi=lo+(count-1)*width;
 char *lo_stack[32],*hi_stack[32];
 for(;;) {
  unsigned int size=(unsigned int)(hi-lo)/width+1;
  if(size<=8) ((shortsort6_t)shortsort_elements)(lo,hi,width,aligned,compare,0);
  else {
   char *mid=lo+(size/2)*width;
   sort_swap_elements(mid,lo,width,aligned);
   char *low=lo,*high=hi+width;
   for(;;) {
    do {low+=width;} while(low<=hi && !sort_compare_entries(low,lo,context));
    do {high-=width;} while(high>lo && !sort_compare_entries(lo,high,context));
    if(high<low)break;
    sort_swap_elements(low,high,width,aligned);
   }
   sort_swap_elements(lo,high,width,aligned);
   if(high-1-lo>=hi-low) {
    if(lo+width<high) {lo_stack[stack]=lo;hi_stack[stack]=high-width;++stack;}
    if(low<hi) {lo=low;continue;}
   } else {
    if(low<hi) {lo_stack[stack]=low;hi_stack[stack]=hi;++stack;}
    if(lo+width<high) {hi=high-width;continue;}
   }
  }
  --stack;
  if(stack<0) break;
  lo=lo_stack[stack]; hi=hi_stack[stack];
 }
}

// @address 0x1f0dd8 shortsort_elements
extern "C" void shortsort_elements(char *first,char *last,unsigned int width,bool aligned) {
 while(last>first) {
  char *it=first+12;
  char *best=first;
  for(;it<=last;it+=12) {
   unsigned char before;
   long it_priority=((s_ai_releasable_entry *)it)->release_index;
   long best_priority=((s_ai_releasable_entry *)best)->release_index;
   if(best_priority<it_priority) before=1;
   else if(best_priority>it_priority) before=0;
   else {
    unsigned char best_key=(unsigned char)((s_ai_releasable_entry *)best)->is_actor;
    unsigned char it_key=(unsigned char)((s_ai_releasable_entry *)it)->is_actor;
    before=it_key<best_key;
   }
   if(before) best=it;
  }
  if(aligned) {
   int count=3;
   long *a=(long *)best,*b=(long *)last;
   do { long temp=*a; *a++=*b; *b++=temp; } while(--count>0);
  } else {
   int count=12;
   char *a=best,*b=last;
   do { char temp=*a; *a++=*b; *b++=temp; } while(--count>0);
  }
  last-=12;
 }
}
