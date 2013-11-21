/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once


struct Vec2;

struct Vec2i
{
    union
    {
        int f[2];
        struct
        {
            int x, y;
        };
    };
};

struct Vec2i vec2i_init(int x)
{
    Vec2i v;
    v.x = x;
    v.y = x;
    return v;
}

struct Vec2i vec2i_init(int x, int y)
{
    Vec2i v;
    v.x = x;
    v.y = y;
    return v;
}

struct Vec2i vec2i_init(const Vec2& a);

struct Vec2i vec2i_add(const Vec2i& a, const Vec2i& b)
{
    Vec2i c = vec2i_init(a.x + b.x, a.y + b.y);
    return c;
}

struct Vec2i vec2i_sub(const Vec2i& a, const Vec2i& b)
{
    Vec2i c = vec2i_init(a.x - b.x, a.y - b.y);
    return c;
}

void vec2i_print(const Vec2i& a)
{
    printf("%d, %d\n", a.x, a.y);
}

struct Vec2i vec2i_scalar_multf(const Vec2i& a, float k)
{
    Vec2i v = a;
    v.x *= k;
    v.y *= k;
    return v;
}

struct Vec2i vec2i_scalar_mult(const Vec2i& a, int k)
{
    Vec2i v = a;
    v.x *= k;
    v.y *= k;
    return v;
}

bool point_in_rect(const Vec2i& point, const Vec2i& position, const Vec2i& dimension)
{
    return (point.x > position.x && point.x < position.x + dimension.x &&
            point.y > position.y && point.y < position.y + dimension.y);
}
