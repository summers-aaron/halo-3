#ifndef INCLUDE_TAG_FILES_TAG_INDEX_H
#define INCLUDE_TAG_FILES_TAG_INDEX_H

// March 2007: c_tag_index(), c_tag_index(long), operator long() const.
// Its methods contain no header assert; this leaf location is inferred.
class c_tag_index {
    long m_index;
public:
    c_tag_index() {}
    c_tag_index(long index) : m_index(index) {}
    operator long() const { return m_index; }
};

typedef char tag_index_size[(sizeof(c_tag_index) == 4) ? 1 : -1];

#endif // INCLUDE_TAG_FILES_TAG_INDEX_H
