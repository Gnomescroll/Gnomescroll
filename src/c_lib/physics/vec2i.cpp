#include "vec2i.hpp"

#include <physics/vec2.hpp>


struct Vec2i vec2i_init(const Vec2& a)
{
    Vec2i b;
    b.x = a.x;
    b.y = a.y;
    return b;
}
