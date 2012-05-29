#pragma once

#include <t_map/common/constants.hpp>
#include <physics/vec3.hpp>


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


static inline int Min_2f(float x, float y)
{
    return abs(x)<abs(y) ? x*x : y*y;
}

static inline int Min_2i(int x, int y)
{
    return abs(x)<abs(y) ? x*x : y*y;
}

//camera x and position x
int quadrant_distance2i(int cx, int px)
{
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
int quadrant_translate_f(float cx, float px)
{
    if(cx < 0.0 || cx >= 512.0)
    {
        printf("WARNING: quadrant_distance_f, cx= %f \n", cx);
    }

    if(px < 0.0 || px >= 512.0)
    {
        printf("WARNING: quadrant_distance_f, px= %f \n", px);
    }

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
            return Min_f(px-cx, px-cx-512.0, px, px-512.0);
        }
    }
    else
    {
        //camera is in second half
        if(px < QUADRANT_DIVIDEf)
        {
            return Min_f(px-cx, px-cx+512.0, px, px+512.0);
        }
        else
        {
            return px;
        }
    }
}

struct Vec3 translate_position(struct Vec3 pos)
{
    return pos;

    if(pos.x <  0.0) pos.x += 512.0;
    if(pos.x >= 512.0) pos.x -= 512.0;

    if(pos.y <  0.0) pos.y += 512.0;
    if(pos.y >= 512.0) pos.y -= 512.0;

    return pos;
}

struct Vec3 quadrant_translate_position(struct Vec3 pos)
{
    return pos;

    if(pos.x <  0.0) pos.x += 512.0;
    if(pos.x >= 512.0) pos.x -= 512.0;

    if(pos.y <  0.0) pos.y += 512.0;
    if(pos.y >= 512.0) pos.y -= 512.0;

    return pos;
}