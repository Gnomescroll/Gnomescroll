#include "quadrant.hpp"

#include <math.h>

#include <t_map/common/constants.hpp>
#include <physics/vec3.hpp>
#include <physics/vec3i.hpp>

ALWAYS_INLINE bool is_boxed_point(float p)
{
    return (p >= 0.0f && p < float(map_dim.x));
}

ALWAYS_INLINE bool is_boxed_point(int p)
{
    return (p >= 0 && p < map_dim.x);
}

ALWAYS_INLINE bool is_boxed_position(const struct Vec3& p)
{
    return (is_boxed_point(p.x) && is_boxed_point(p.y));
}

ALWAYS_INLINE bool is_boxed_position(const struct Vec3i& p)
{
    return (is_boxed_point(p.x) && is_boxed_point(p.y));
}

ALWAYS_INLINE static int min_i(int x, int y, int x1, int y1)
{
    return abs(x) < abs(y) ? x1 : y1;
}

ALWAYS_INLINE static float min_f(float x, float y, float x1, float y1)
{
    return fabsf(x) < fabsf(y) ? x1 : y1;
}

ALWAYS_INLINE static int min_2i(int x, int y)
{
    return abs(x) < abs(y) ? x*x : y*y;
}

ALWAYS_INLINE static float min_2f(float x, float y)
{
    return fabsf(x) < fabsf(y) ? x*x : y*y;
}

inline int quadrant_translate_i(int cx, int px)
{   //camera x and position x
    GS_ASSERT(is_boxed_point(cx));
    GS_ASSERT(is_boxed_point(px));

    if (cx < QUADRANT_DIVIDEi)
    {   //camera in first half
        if (px < QUADRANT_DIVIDEi)
        {   //chunk is in first half
            return px;
        }
        else
        {
            return min_i(px-cx, px-cx-map_dim.x, px, px-map_dim.x);
        }
    }
    else
    {   //camera is in second half
        if (px < QUADRANT_DIVIDEi)
        {
            return min_i(px-cx, px-cx+map_dim.x, px, px+map_dim.x);
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

    GS_ASSERT(is_boxed_point(cx));
    GS_ASSERT(is_boxed_point(px));

    const float dx = float(map_dim.x);

    if (cx < QUADRANT_DIVIDEf)
    {   //camera in first half
        if (px < QUADRANT_DIVIDEf)
        {   //chunk is in first half
            return px;
        }
        else
        {
            return min_f(px-cx, px-cx-dx, px, px-dx);
        }
    }
    else
    {   //camera is in second half
        if (px < QUADRANT_DIVIDEf)
        {
            return min_f(px-cx, px-cx+dx, px, px+dx);
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
    if (pt <  0.0f) pt += float(map_dim.x);
    if (pt >= float(map_dim.x)) pt -= float(map_dim.x);
    GS_ASSERT(is_boxed_point(pt));
    return pt;
}

ALWAYS_INLINE int translate_point(int pt)
{
    if (pt < 0) pt += map_dim.x;
    if (pt >= map_dim.x) pt -= map_dim.x;
    GS_ASSERT(is_boxed_point(pt));
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
    GS_ASSERT(is_boxed_position(pos1));
    GS_ASSERT(is_boxed_position(pos2));

    pos2.x = quadrant_translate_f(pos1.x, pos2.x);
    pos2.y = quadrant_translate_f(pos1.y, pos2.y);

    return pos2;
}

ALWAYS_INLINE struct Vec3i translate_position(struct Vec3i pos)
{
    pos.x = translate_point(pos.x);
    pos.y = translate_point(pos.y);
    return pos;
}

inline struct Vec3i quadrant_translate_position(struct Vec3i pos1, struct Vec3i pos2)
{
    GS_ASSERT(is_boxed_position(pos1));
    GS_ASSERT(is_boxed_position(pos2));
    pos2.x = quadrant_translate_i(pos1.x, pos2.x);
    pos2.y = quadrant_translate_i(pos1.y, pos2.y);
    return pos2;
}
