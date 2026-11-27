#ifndef CACHE_PREDICTED_RESOURCE_VISITOR_H
#define CACHE_PREDICTED_RESOURCE_VISITOR_H

struct s_tag_resource;

// Group-handle operations come before the resource/tag callbacks.
// March's c_simple_tag_resource_predictor identifies the final two callbacks.
struct c_visitor
{
    virtual long begin(long tag_index, long region_index, long permutation_index) = 0;
    virtual unsigned char has_handle(long handle) = 0;
    virtual void enter(long handle) = 0;
    virtual void leave() = 0;
    virtual void add_resource(const s_tag_resource *resource) = 0;
    virtual void visit(int index) = 0;
};

#endif
