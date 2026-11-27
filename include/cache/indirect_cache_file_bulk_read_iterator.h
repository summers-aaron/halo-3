#ifndef CACHE_INDIRECT_CACHE_FILE_BULK_READ_ITERATOR_H
#define CACHE_INDIRECT_CACHE_FILE_BULK_READ_ITERATOR_H

struct s_indirect_cache_file_read_request;

class c_indirect_cache_file_bulk_read_iterator
{
public:
    virtual void reset() = 0;
    virtual bool next(s_indirect_cache_file_read_request *out_read_request, int *out_request_identifier) = 0;
    virtual void finished(unsigned long request_identifier, long result) = 0;
    virtual void read_progress(unsigned long bytes_read, bool complete) = 0;
};

#endif // CACHE_INDIRECT_CACHE_FILE_BULK_READ_ITERATOR_H
