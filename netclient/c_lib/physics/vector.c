#include "vector.h"


float vector_dot(struct Vector* v1, struct Vector* v2) {
    float d;
    d = v1->x*v2->x + v1->y*v2->y + v1->z*+v2->z;
    //printf("dot= %f \n", d);
    return d;
}

float vector_dot1(float x, float y, float z, float a, float b, float c) {
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
    float dot = vector_dot1(x,y,z, a,b,c);
    float len1 = vector_length1(x,y,z);
    float len2 = vector_length1(a,b,c);

    angle = acos(dot / (len1 * len2));
    return angle;
}
