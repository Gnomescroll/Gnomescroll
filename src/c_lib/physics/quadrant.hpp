#pragma once

#include <t_map/common/constants.hpp>
#include <physics/vec3.hpp>

#define ASSERT_BOXED_POINT(p) GS_ASSERT(p >= 0 && p < 512)
#define ASSERT_BOXED_POSITION(p) GS_ASSERT(p.x >= 0.0f && p.x < 512.0f && p.y >= 0.0f && p.y < 512.0f)

__attribute((always_inline))
int Min_i(int x, int y, int x1, int y1)
{
    return abs(x)<abs(y) ? x1 : y1;
}

__attribute((always_inline))
float Min_f(float x, float y, float x1, float y1)
{
    return abs(x)<abs(y) ? x1 : y1;
}

__attribute((always_inline))
int Min_2i(int x, int y)
{
    return abs(x)<abs(y) ? x*x : y*y;
}

__attribute((always_inline))
float Min_2f(float x, float y)
{
    return abs(x)<abs(y) ? x*x : y*y;
}

//camera x and position x
__attribute((always_inline))
int quadrant_translate_i(int cx, int px)
{
    ASSERT_BOXED_POINT(cx);
    ASSERT_BOXED_POINT(px);

    if(cx < QUADRANT_DIVIDEi)
    {
        //camera in first half
        if(px < QUADRANT_DIVIDEi)
        {
            //chunk is in first half
            return px;
        }
        else
        {
            return Min_i(px-cx, px-cx-512, px, px-512);
        }
    }
    else
    {
        //camera is in second half
        if(px < QUADRANT_DIVIDEi)
        {
            return Min_i(px-cx, px-cx+512, px, px+512);
        }
        else
        {
            return px;
        }
    }
}


//camera x and position x
__attribute((always_inline))
float quadrant_translate_f(float cx, float px)
{
    //ASSERT_BOXED_POINT(cx);
    //ASSERT_BOXED_POINT(px);

    if(cx < QUADRANT_DIVIDEf)
    {
        //camera in first half
        if(px < QUADRANT_DIVIDEf)
        {
            //chunk is in first half
            return px;
        }
        else
        {
            return Min_f(px-cx, px-cx-512.0f, px, px-512.0f);
        }
    }
    else
    {
        //camera is in second half
        if(px < QUADRANT_DIVIDEf)
        {
            return Min_f(px-cx, px-cx+512.0f, px, px+512.0f);
        }
        else
        {
            return px;
        }
    }
}

__attribute((always_inline))
inline float translate_point(float pt)
{
    if(pt <  0.0f) pt += 512.0f;
    if(pt >= 512.0f) pt -= 512.0f;
    ASSERT_BOXED_POINT(pt);
    return pt;
}

__attribute((always_inline))
inline int translate_point(int pt)
{
    if(pt <  0) pt += 512;
    if(pt >= 512) pt -= 512;
    ASSERT_BOXED_POINT(pt);
    return pt;
}

struct Vec3 translate_position(struct Vec3 pos)
{
    pos.x = translate_point(pos.x);
    pos.y = translate_point(pos.y);

    //pos.x = quadrant_translate_f(current_camera_position.x, pos.x);
    //pos.y = quadrant_translate_f(current_camera_position.y, pos.y);

    ASSERT_BOXED_POSITION(pos);

    return pos;
}

struct Vec3 quadrant_translate_position(struct Vec3 pos1, struct Vec3 pos2)
{
    //ASSERT_BOXED_POSITION(pos1);
    //ASSERT_BOXED_POSITION(pos2);
    
    pos2.x = quadrant_translate_f(pos1.x, pos2.x);
    pos2.y = quadrant_translate_f(pos1.y, pos2.y);

    return pos2;
}
