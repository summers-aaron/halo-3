#ifndef CACHE_CACHE_FILE_BULK_READER_CALLBACKS_H
#define CACHE_CACHE_FILE_BULK_READER_CALLBACKS_H

template<typename T> class c_basic_buffer;
extern "C" bool tag_resource_cache_get_page_data(void *owner, long p1, long p2, long p3, long p4, c_basic_buffer<void> buffer);
extern "C" void lruv_complete_request(long result_a, long result_b, void *context);

#endif // CACHE_CACHE_FILE_BULK_READER_CALLBACKS_H
