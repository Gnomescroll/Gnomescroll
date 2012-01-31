#pragma once

#include <math.h>
#include <stdio.h>

#define PI 3.14159265

struct Vector {
    float x,y,z;
};

inline void vector_cross(struct Vector* v1, struct Vector* v2, struct Vector* dest) __attribute((always_inline)); 

inline void vector_scalar1(struct Vector* v1, float scalar) __attribute((always_inline)); 
inline Vector vector_scalar2(struct Vector* v1, float scalar) __attribute((always_inline)); 

inline Vector vector_add2(struct Vector* v1, struct Vector* v2) __attribute((always_inline)); 
inline Vector vector_add3(struct Vector* v1, struct Vector* v2, struct Vector* v3) __attribute((always_inline)); 
inline Vector vector_add4(struct Vector* v1, struct Vector* v2, struct Vector* v3, struct Vector* v4) __attribute((always_inline)); 

float vector_dot(struct Vector* v1, struct Vector* v2);
void print_vector_dot(struct Vector v1, struct Vector v2);
float vector_length(struct Vector *v);
void print_vector_length(struct Vector *v);
void print_vector_length(float v[3]);
void print_vector(struct Vector * v);

inline struct Vector Vector_init(float x, float y, float z);

inline void normalize_vector(struct Vector* v);
inline void normalize_vector_f(float* x, float *y, float *z);
inline void normalize_vector(float v[3]);

inline struct Vector vector_cross(struct Vector v1, struct Vector v2);
inline void vector_cross_ptr(struct Vector* v0, struct Vector* v1, struct Vector* v2);

//rotate vector in xy plane around origin
inline void vector_rotate_origin(struct Vector* in, struct Vector* out, float theta);

inline struct Vector sub_vec(struct Vector* a, struct Vector* b);
inline void sub_vec(float a[3], float b[3], float c[3]);

struct Vector* mult_vec_scalar(struct Vector* a, float i);
void mult_vec_scalar_ptr(struct Vector *a, float i);
struct Vector reflect(struct Vector* inc, struct Vector* nor);
void reflect(float inc[3], float nor[3], float ref[3]);

void print_vector(struct Vector* v);
float distance(float x, float y, float z, float x1, float y1, float z1);
float angle_between_vectors(float x, float y, float z, float a, float b, float c);
