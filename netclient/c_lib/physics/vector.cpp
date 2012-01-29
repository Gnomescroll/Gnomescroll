#include "vector.hpp"


float vector_dot(struct Vector* v1, struct Vector* v2) {
    float d;
    d = v1->x*v2->x + v1->y*v2->y + v1->z*+v2->z;
    //printf("dot= %f \n", d);
    return d;
}

float vector_dot(float a[3], float b[3]) {
    float d;
    d = a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
    //printf("dot= %f \n", d);
    return d;
}

float vector_dot(float x, float y, float z, float a, float b, float c) {
    float d;
    d = x*a + y*b + z*c;
    //printf("dot= %f \n", d);
    return d;
}

void print_vector_dot(struct Vector v1, struct Vector v2) {
    float d;
    d = v1.x*v2.x + v1.y*v2.y + v1.z*+v2.z;
    printf("dot= %f \n", d);
}

float vector_length(struct Vector *v) {
    float l;
    l = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x /= l; v->y /=l; v->z /=l;
    //printf("l= %fs \n", l);
    return l;
}

float vector_length1(float x, float y, float z) {
    float l;
    l = sqrt(x*x + y*y + z*z);
    x /= l; y /=l; z /=l;
    //printf("l= %fs \n", l);
    return l;
}

void print_vector_length(struct Vector *v) {
    float l;
    l = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x /= l; v->y /=l; v->z /=l;
    printf("l= %fs \n", l);
    }

//moved
static struct Vector _mult_vec_scalar_tmp = {0.0f,0.0f,0.0f};
static struct Vector* mult_vec_scalar_tmp = &_mult_vec_scalar_tmp;

struct Vector* mult_vec_scalar(struct Vector* a, float i) {
    mult_vec_scalar_tmp->x = a->x * i;
    mult_vec_scalar_tmp->y = a->y * i;
    mult_vec_scalar_tmp->z = a->z * i;
    return mult_vec_scalar_tmp;
}

void mult_vec_scalar(float a[3], float i, float b[3]) {
    b[0] = a[0] * i;
    b[1] = a[1] * i;
    b[2] = a[2] * i;
}

void mult_vec_scalar_ptr(struct Vector* a, float i) {
    a->x *= i;
    a->y *= i;
    a->z *= i;
}

struct Vector reflect(struct Vector* inc, struct Vector* nor) {
    struct Vector v;
    v = sub_vec(inc, mult_vec_scalar(nor, 2.0f*(vector_dot(nor,inc))));
    //v = inc - 2*(nor.inc)*nor   <<< pseudocode
    return v;
}

void print_vector(struct Vector * v) {
    printf("%f, %f, %f \n", v->x, v->y, v->z);
}

float distance(float x, float y, float z, float x1, float y1, float z1) {
    return sqrt((x1-x)*(x1-x) + (y1-y)*(y1-y) + (z1-z)*(z1-z));
}

float angle_between_vectors(float x, float y, float z, float a, float b, float c) {
    float angle;
    float dot = vector_dot(x,y,z, a,b,c);
    float len1 = vector_length1(x,y,z);
    float len2 = vector_length1(a,b,c);

    angle = acos(dot / (len1 * len2));
    return angle;
}

void reflect(float inc[3], float nor[3], float ref[3]) {
    float inter[3];
    mult_vec_scalar(nor, 2.0f*vector_dot(nor, inc), inter);
    sub_vec(inc, inter, ref);
}

void vector_cross(struct Vector* v1, struct Vector* v2, struct Vector* dest)
{
    dest->x = v1->y*v2->z - v1->z*v2->y;
    dest->y = v1->z*v2->x - v1->x*v2->z;
    dest->z = v1->x*v2->y - v1->y*v2->x;
}

void vector_scalar1(struct Vector* v, float scalar)
{
    v->x *= scalar;
    v->y *= scalar;
    v->z *= scalar;
}

inline Vector vector_add2(struct Vector* v1, struct Vector* v2)
{
    Vector vr;
    vr.x = v1->x + v2->x;
    vr.y = v1->y + v2->y;
    vr.z = v1->z + v2->z;
    return vr;
}

inline Vector vector_add3(struct Vector* v1, struct Vector* v2, struct Vector* v3)
{
    Vector vr;
    vr.x = v1->x + v2->x + v3->x;
    vr.y = v1->y + v2->y + v3->y;
    vr.z = v1->z + v2->z + v3->z;
    return vr;
}

struct Vector vector_scalar2(struct Vector* v1, float scalar)
{
    Vector vr;
    vr.x = v1->x * scalar;
    vr.y = v1->y * scalar;
    vr.z = v1->z * scalar;
    return vr;
}

inline Vector vector_add4(struct Vector* v1, struct Vector* v2, struct Vector* v3, struct Vector* v4)
{
    Vector vr;
    vr.x = v1->x + v2->x + v3->x + v4->x;
    vr.y = v1->y + v2->y + v3->y + v4->y;
    vr.z = v1->z + v2->z + v3->z + v4->z;
    return vr;
}

inline struct Vector Vector_init(float x, float y, float z) {
    struct Vector v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

inline void normalize_vector(struct Vector* v) {
    float l = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
    v->x /= l; v->y /=l; v->z /=l;
}

inline void normalize_vector_f(float* x, float *y, float *z) {
    float l = sqrt(*x**x + *y**y + *z**z);
    *x = *x/l;
    *y = *y/l;
    *z = *z/l;
}

inline struct Vector vector_cross(struct Vector v1, struct Vector v2) {
    struct Vector v0;
    v0.x = v1.y*v2.z - v1.z*v2.y;
    v0.y = v1.z*v2.x - v1.x*v2.z;
    v0.z = v1.x*v2.y - v1.y*v2.x;
    return v0;
}

inline void vector_cross_ptr(struct Vector* v0, struct Vector* v1, struct Vector* v2) {
    v0->x = v1->y*v2->z - v1->z*v2->y;
    v0->y = v1->z*v2->x - v1->x*v2->z;
    v0->z = v1->x*v2->y - v1->y*v2->x;
}

inline void vector_rotate_origin(struct Vector* in, struct Vector* out, float theta) {
    float cost = cos(theta);
    float sint = sin(theta);
    float x = in->x;
    float y = in->y;
    out->x = x*cost - y*sint;
    out->y = x*sint + y*cost;
}

inline struct Vector sub_vec(struct Vector* a, struct Vector* b) {
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

inline void normalize_vector(float v[3])
{
    float len = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}
