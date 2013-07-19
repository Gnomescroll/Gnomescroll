#include "vec2.hpp"

#include <physics/vec2i.hpp>

struct Vec2 vec2_init(const struct Vec2i& a)
{
    Vec2 b;
    b.x = a.x;
    b.y = a.y;
    return b;
}
