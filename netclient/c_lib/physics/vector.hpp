#pragma once

#include <math.h>
#include <stdio.h>

#define PI 3.14159265

struct Vector {
    float x,y,z;
};


inline static void vector_cross(struct Vector* v1, struct Vector* v2, struct Vector* dest) __attribute((always_inline)); 

void vector_cross(struct Vector* v1, struct Vector* v2, struct Vector* dest)
{
    dest->x = v1->y*v2->z - v1->z*v2->y;
    dest->y = v1->z*v2->x - v1->x*v2->z;
    dest->z = v1->x*v2->y - v1->y*v2->x;
}

inline static void vector_scalar1(struct Vector* v1, float scalar) __attribute((always_inline)); 

void vector_scalar1(struct Vector* v, float scalar)
{
    v->x *= scalar;
    v->y *= scalar;
    v->z *= scalar;
}

inline static Vector vector_scalar2(struct Vector* v1, float scalar) __attribute((always_inline)); 

struct Vector vector_scalar2(struct Vector* v1, float scalar)
{
    Vector vr;
    vr.x = v1->x * scalar;
    vr.y = v1->y * scalar;
    vr.z = v1->z * scalar;
    return vr;
}



inline static Vector vector_add2(struct Vector* v1, struct Vector* v2) __attribute((always_inline)); 

inline static Vector vector_add2(struct Vector* v1, struct Vector* v2)
{
    Vector vr;
    vr.x = v1->x + v2->x;
    vr.y = v1->y + v2->y;
    vr.z = v1->z + v2->z;
    return vr;
}

inline static Vector vector_add3(struct Vector* v1, struct Vector* v2, struct Vector* v3) __attribute((always_inline)); 

inline static Vector vector_add3(struct Vector* v1, struct Vector* v2, struct Vector* v3)
{
    Vector vr;
    vr.x = v1->x + v2->x + v3->x;
    vr.y = v1->y + v2->y + v3->y;
    vr.z = v1->z + v2->z + v3->z;
    return vr;
}

inline static Vector vector_add4(struct Vector* v1, struct Vector* v2, struct Vector* v3, struct Vector* v4) __attribute((always_inline)); 

inline static Vector vector_add4(struct Vector* v1, struct Vector* v2, struct Vector* v3, struct Vector* v4)
{
    Vector vr;
    vr.x = v1->x + v2->x + v3->x + v4->x;
    vr.y = v1->y + v2->y + v3->y + v4->y;
    vr.z = v1->z + v2->z + v3->z + v4->z;
    return vr;
}

//

float vector_dot(struct Vector* v1, struct Vector* v2);
void print_vector_dot(struct Vector v1, struct Vector v2);
float vector_length(struct Vector *v);
void print_vector_length(struct Vector *v);
void print_vector(struct Vector * v);

inline static struct Vector Vector_init(float x, float y, float z) {
    struct Vector v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

inline static void normalize_vector(struct Vector* v) {
float l = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
v->x /= l; v->y /=l; v->z /=l;
}

inline static void normalize_vector_f(float* x, float *y, float *z) {
    float l = sqrt(*x**x + *y**y + *z**z);
    *x = *x/l;
    *y = *y/l;
    *z = *z/l;
}

inline static struct Vector vector_cross(struct Vector v1, struct Vector v2) {
    struct Vector v0;
    v0.x = v1.y*v2.z - v1.z*v2.y;
    v0.y = v1.z*v2.x - v1.x*v2.z;
    v0.z = v1.x*v2.y - v1.y*v2.x;
    return v0;
}

inline static void vector_cross_ptr(struct Vector* v0, struct Vector* v1, struct Vector* v2) {
    v0->x = v1->y*v2->z - v1->z*v2->y;
    v0->y = v1->z*v2->x - v1->x*v2->z;
    v0->z = v1->x*v2->y - v1->y*v2->x;
}

//rotate vector in xy plane around origin
inline static void vector_rotate_origin(struct Vector* in, struct Vector* out, float theta) {
    float cost = cos(theta);
    float sint = sin(theta);
    float x = in->x;
    float y = in->y;
    out->x = x*cost - y*sint;
    out->y = x*sint + y*cost;
}


inline static struct Vector sub_vec(struct Vector* a, struct Vector* b) {
    struct Vector v;
    v.x = a->x - b->x;
    v.y = a->y - b->y;
    v.z = a->z - b->z;
    return v;
}

inline void sub_vec(float a[3], float b[3], float c[3]) {
    c[0] = a[0] - b[0];
    c[1] = a[1] - b[1];
    c[2] = a[2] - b[2];
}

struct Vector* mult_vec_scalar(struct Vector* a, float i);
void mult_vec_scalar_ptr(struct Vector *a, float i);
struct Vector reflect(struct Vector* inc, struct Vector* nor);
void reflect(float inc[3], float nor[3], float ref[3]);

void print_vector(struct Vector* v);
float distance(float x, float y, float z, float x1, float y1, float z1);
float angle_between_vectors(float x, float y, float z, float a, float b, float c);
