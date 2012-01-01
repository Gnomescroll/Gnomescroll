#pragma once

#include <math.h>
#include <stdio.h>

#define PI 3.14159265

#include <physics/vector.hpp>

struct Vector4 {
    union
    {
        struct 
        {
            float x,y,z,w;
        };
        struct 
        {
            Vector v3;    
        };
    };
};


inline static void vector_cross(struct Vector4* v1, struct Vector4* v2, struct Vector4* dest) __attribute((always_inline)); 

void vector_cross(struct Vector4* v1, struct Vector4* v2, struct Vector4* dest)
{
    dest->x = v1->y*v2->z - v1->z*v2->y;
    dest->y = v1->z*v2->x - v1->x*v2->z;
    dest->z = v1->x*v2->y - v1->y*v2->x;
}

inline static void vector_scalar1(struct Vector4* v1, float scalar) __attribute((always_inline)); 

void vector_scalar1(struct Vector4* v, float scalar)
{
    v->x *= scalar;
    v->y *= scalar;
    v->z *= scalar;
}

inline static Vector4 vector_scalar2(struct Vector4* v1, float scalar) __attribute((always_inline)); 

struct Vector4 vector_scalar2(struct Vector4* v1, float scalar)
{
    Vector4 vr;
    vr.x = v1->x * scalar;
    vr.y = v1->y * scalar;
    vr.z = v1->z * scalar;
    return vr;
}



inline static Vector4 vector_add2(struct Vector4* v1, struct Vector4* v2) __attribute((always_inline)); 

inline static Vector4 vector_add2(struct Vector4* v1, struct Vector4* v2)
{
    Vector4 vr;
    vr.x = v1->x + v2->x;
    vr.y = v1->y + v2->y;
    vr.z = v1->z + v2->z;
    return vr;
}

inline static Vector4 vector_add3(struct Vector4* v1, struct Vector4* v2, struct Vector4* v3) __attribute((always_inline)); 

inline static Vector4 vector_add3(struct Vector4* v1, struct Vector4* v2, struct Vector4* v3)
{
    Vector4 vr;
    vr.x = v1->x + v2->x + v3->x;
    vr.y = v1->y + v2->y + v3->y;
    vr.z = v1->z + v2->z + v3->z;
    return vr;
}

inline static Vector4 vector_add4(struct Vector4* v1, struct Vector4* v2, struct Vector4* v3, struct Vector4* v4) __attribute((always_inline)); 

inline static Vector4 vector_add4(struct Vector4* v1, struct Vector4* v2, struct Vector4* v3, struct Vector4* v4)
{
    Vector4 vr;
    vr.x = v1->x + v2->x + v3->x + v4->x;
    vr.y = v1->y + v2->y + v3->y + v4->y;
    vr.z = v1->z + v2->z + v3->z + v4->z;
    return vr;
}


inline static struct Vector4 Vector4_init(float x, float y, float z) {
    struct Vector4 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

inline static struct Vector4 Vector4_init(float x, float y, float z, float w) {
    struct Vector4 v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}