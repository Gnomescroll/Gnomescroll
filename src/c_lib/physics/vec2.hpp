#pragma once


struct Vec2i;

struct Vec2
{
    union
    {
        float f[2];
        struct
        {
            float x, y;
        };
    };
};

struct Vec2 vec2_init(float x)
{
    Vec2 v;
    v.x = x;
    v.y = x;
    return v;
}

struct Vec2 vec2_init(float x, float y)
{
    Vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

struct Vec2 vec2_init(const struct Vec2i& a);

struct Vec2 vec2_add(const Vec2& a, const Vec2& b)
{
    Vec2 c = vec2_init(a.x + b.x, a.y + b.y);
    return c;
}

struct Vec2 vec2_sub(const Vec2& a, const Vec2& b)
{
    Vec2 c = vec2_init(a.x - b.x, a.y - b.y);
    return c;
}

void vec2_print(const Vec2& a)
{
    printf("%f, %f\n", a.x, a.y);
}

struct Vec2 vec2_scalar_mult(const Vec2& a, float k)
{
    Vec2 v = a;
    v.x *= k;
    v.y *= k;
    return v;
}

struct Vec2 vec2_scalar_sub(const Vec2& a, float b)
{
    Vec2 v = a;
    v.x -= b;
    v.y -= b;
    return v;
}

bool point_in_rect(const Vec2& point, const Vec2& position, const Vec2& dimension)
{
    return (point.x > position.x && point.x < position.x + dimension.x &&
            point.y > position.y && point.y < position.y + dimension.y);
}
