#pragma once

#include <math.h>

#include <t_map/common/constants.hpp>
#include <physics/vec3.hpp>

#define ASSERT_BOXED_POINT(p) GS_ASSERT(is_boxed_point(p))
#define ASSERT_BOXED_POSITION(p) GS_ASSERT(is_boxed_position(p))

ALWAYS_INLINE bool is_boxed_point(float p)
{
    return (p >= 0.0f && p < 512.0f);
}

ALWAYS_INLINE bool is_boxed_point(int p)
{
    return (p >= 0 && p < 512);
}

ALWAYS_INLINE bool is_boxed_position(const struct Vec3& p)
{
    return (is_boxed_point(p.x) && is_boxed_point(p.y));
}

ALWAYS_INLINE bool is_boxed_position(const struct MapPos& p)
{
    return (is_boxed_point(p.x) && is_boxed_point(p.y));
}

ALWAYS_INLINE int min_i(int x, int y, int x1, int y1)
{
    return abs(x) < abs(y) ? x1 : y1;
}

ALWAYS_INLINE float min_f(float x, float y, float x1, float y1)
{
    return fabsf(x) < fabsf(y) ? x1 : y1;
}

ALWAYS_INLINE int min_2i(int x, int y)
{
    return abs(x) < abs(y) ? x*x : y*y;
}

ALWAYS_INLINE float min_2f(float x, float y)
{
    return fabsf(x) < fabsf(y) ? x*x : y*y;
}

inline int quadrant_translate_i(int cx, int px)
{   //camera x and position x
    ASSERT_BOXED_POINT(cx);
    ASSERT_BOXED_POINT(px);

    if (cx < QUADRANT_DIVIDEi)
    {
        //camera in first half
        if (px < QUADRANT_DIVIDEi)
        {
            //chunk is in first half
            return px;
        }
        else
        {
            return min_i(px-cx, px-cx-512, px, px-512);
        }
    }
    else
    {
        //camera is in second half
        if (px < QUADRANT_DIVIDEi)
        {
            return min_i(px-cx, px-cx+512, px, px+512);
        }
        else
        {
            return px;
        }
    }
}


inline float quadrant_translate_f(float cx, float px)
{   //camera x and position x
    #ifdef NAN
    GS_ASSERT(!isnan(cx));
    GS_ASSERT(!isnan(px));
    #endif

    ASSERT_BOXED_POINT(cx);
    ASSERT_BOXED_POINT(px);

    if (cx < QUADRANT_DIVIDEf)
    {
        //camera in first half
        if (px < QUADRANT_DIVIDEf)
        {
            //chunk is in first half
            return px;
        }
        else
        {
            return min_f(px-cx, px-cx-512.0f, px, px-512.0f);
        }
    }
    else
    {
        //camera is in second half
        if (px < QUADRANT_DIVIDEf)
        {
            return min_f(px-cx, px-cx+512.0f, px, px+512.0f);
        }
        else
        {
            return px;
        }
    }
}

ALWAYS_INLINE int quadrant_translate(int cx, int px)
{
    return quadrant_translate_i(cx, px);
}

ALWAYS_INLINE float quadrant_translate(float cx, float px)
{
    return quadrant_translate_f(cx, px);
}

ALWAYS_INLINE float translate_point(float pt)
{
    #ifdef NAN
    GS_ASSERT(!isnan(pt));
    #endif
    if (pt <  0.0f) pt += 512.0f;
    if (pt >= 512.0f) pt -= 512.0f;
    ASSERT_BOXED_POINT(pt);
    return pt;
}

ALWAYS_INLINE int translate_point(int pt)
{
    if (pt < 0) pt += 512;
    if (pt >= 512) pt -= 512;
    ASSERT_BOXED_POINT(pt);
    return pt;
}

ALWAYS_INLINE struct Vec3 translate_position(struct Vec3 pos)
{
    pos.x = translate_point(pos.x);
    pos.y = translate_point(pos.y);
    return pos;
}

inline struct Vec3 quadrant_translate_position(struct Vec3 pos1, struct Vec3 pos2)
{
    ASSERT_BOXED_POSITION(pos1);
    ASSERT_BOXED_POSITION(pos2);

    pos2.x = quadrant_translate_f(pos1.x, pos2.x);
    pos2.y = quadrant_translate_f(pos1.y, pos2.y);

    return pos2;
}
