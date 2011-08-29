#ifndef vector_h
#define vector_h

#include <math.h>

#define PI 3.14159265

struct Vector {
    float x,y,z;
};

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

inline static struct Vector vector_cross(struct Vector v1, struct Vector v2) {
    struct Vector v0;
    v0.x = v1.y*v2.z - v1.z*v2.y;
    v0.y = v1.z*v2.x - v1.x*v2.z;
    v0.z = v1.x*v2.y - v1.y*v2.x;
    return v0;
}

inline static struct Vector vector_rotate_origin(struct Vector v, float theta) {
    float x = v.x;
    float y = v.y;
    float cost = cos(theta);
    float sint = sin(theta);
    v.x = x*cost - y*sint;
    v.y = x*sint + y*cost;
    return v;
}

void print_vector_dot(struct Vector v1, struct Vector v2) {
    float d;
    d = v1.x*v2.x + v1.y*v2.y + v1.z*+v2.z;
    printf("dot= %f \n", d);
}

void print_vector_length(struct Vector *v) {
    float l;
    l = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x /= l; v->y /=l; v->z /=l;
    printf("l= %fs \n", l);
    }

#endif
