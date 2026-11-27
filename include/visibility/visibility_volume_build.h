#ifndef VISIBILITY_VISIBILITY_VOLUME_BUILD_H
#define VISIBILITY_VISIBILITY_VOLUME_BUILD_H

struct visibility_projection;
struct visibility_volume;
union real_rectangle2d;

bool visibility_volume_build(
    const visibility_projection *projection, long index, const real_rectangle2d *rectangle, visibility_volume *volume);

#endif // VISIBILITY_VISIBILITY_VOLUME_BUILD_H
