/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "vec3.hpp"

#include <physics/vec3i.hpp>

ALWAYS_INLINE struct Vec3 vec3_init(struct Vec3i pos)
{
    return vec3_init(pos.x, pos.y, pos.z);
}
