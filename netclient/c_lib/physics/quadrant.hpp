#pragma once

#include <t_map/common/constants.hpp>
#include <physics/vec3.hpp>

#define ASSERT_BOXED_POINT(p) GS_ASSERT(p >= 0 && p < 512)
#define ASSERT_BOXED_POSITION(p) GS_ASSERT(p.x >= 0 && p.x < 512 && p.y >= 0 && p.y < 512)

static inline int Min_i(int x, int y, int x1, int y1)
{
    //float x2 = x*x;
    //float y2 = y*y;
    return abs(x)<abs(y) ? x1 : y1;
}

static inline float Min_f(float x, float y, float x1, float y1)
{
    //float x2 = x*x;
    //float y2 = y*y;
    return abs(x)<abs(y) ? x1 : y1;
}

static inline int Min_2i(int x, int y)
{
    return abs(x)<abs(y) ? x*x : y*y;
}

static inline float Min_2f(float x, float y)
{
    return abs(x)<abs(y) ? x*x : y*y;
}

//camera x and position x
int quadrant_distance2i(int cx, int px)
{
    ASSERT_BOXED_POINT(cx);
    ASSERT_BOXED_POINT(px);

    if(cx < 0 || cx >= 512)
    {
        printf("WARNING: quadrant_distance2i, cx= %i \n", cx);
    }

    if(px < 0 || px >= 512)
    {
        printf("WARNING: quadrant_distance2i, px= %i \n", px);
    }

    int distance;

    if(cx < QUADRANT_DIVIDEi)
    {
        //camera in first half
        if(px < QUADRANT_DIVIDEi)
        {
            //chunk is in first half
            distance = px - cx;
            distance = distance*distance;
        }
        else
        {
            distance = Min_2i(px-cx, px-cx-512);
        }
    }
    else
    {
        //camera is in second half
        if(px < QUADRANT_DIVIDEi)
        {
            distance = Min_2i(px-cx, px-cx+512);
        }
        else
        {
            distance = px - cx;
            distance = distance*distance;
        }
    }

    return distance;
}


//camera x and position x
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

#if DC_CLIENT

#include <camera/camera.hpp>

void quadrant_translate_draw_position(float* x, float* y)
{
    *x = quadrant_translate_f(current_camera_position.x, *x);
    *y = quadrant_translate_f(current_camera_position.y, *y);
}

#endif

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
