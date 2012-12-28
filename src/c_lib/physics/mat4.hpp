#pragma once

#ifdef __GNUC
    #pragma GCC diagnostic ignored "-Wunused-function"
#endif


#include <physics/vec3.hpp>
#include <physics/vec4.hpp>
#include <physics/mat3.hpp>

//note that c uses row major order
//glsl uses column major order
 
struct Mat4 {
    union
    {
        float f[4][4];
        float _f[16];

        struct Vec4 v[4];

        struct 
        {
            struct Vec4 vx;
            struct Vec4 vy;
            struct Vec4 vz;
            struct Vec4 c;
        };
    };
};

//9.005 Are OpenGL matrices column-major or row-major?
// http://www.opengl.org/archives/resources/faq/technical/transformations.htm
void mat4_print(struct Mat4 m)
{
/*
    printf("%.2f %.2f %.2f %.2f \n", m.v[0].x,m.v[0].y,m.v[0].z,m.v[0].w );
    printf("%.2f %.2f %.2f %.2f \n", m.v[1].x,m.v[1].y,m.v[1].z,m.v[1].w );
    printf("%.2f %.2f %.2f %.2f \n", m.v[2].x,m.v[2].y,m.v[2].z,m.v[2].w );
    printf("%.2f %.2f %.2f %.2f \n", m.v[3].x,m.v[3].y,m.v[3].z,m.v[3].w );
*/

    printf("%.2f %.2f %.2f %.2f \n", m.v[0].x,m.v[1].x,m.v[2].x,m.v[3].x );
    printf("%.2f %.2f %.2f %.2f \n", m.v[0].y,m.v[1].y,m.v[2].y,m.v[3].y );
    printf("%.2f %.2f %.2f %.2f \n", m.v[0].z,m.v[1].z,m.v[2].z,m.v[3].z );
    printf("%.2f %.2f %.2f %.2f \n", m.v[0].w,m.v[1].w,m.v[2].w,m.v[3].w );
/*
    printf("\n");

    float *f = m._f;

    printf("%.2f %.2f %.2f %.2f \n", f[0*4+0],f[0*4+1],f[0*4+2],f[0*4+3] );
    printf("%.2f %.2f %.2f %.2f \n", f[1*4+0],f[1*4+1],f[1*4+2],f[1*4+3] );
    printf("%.2f %.2f %.2f %.2f \n", f[2*4+0],f[2*4+1],f[2*4+2],f[2*4+3] );
    printf("%.2f %.2f %.2f %.2f \n", f[3*4+0],f[3*4+1],f[3*4+2],f[3*4+3] );
    
    printf("\n");

    printf("%f %f %f %f \n", f[0*4+0],f[0*4+1],f[0*4+2],f[0*4+3] );
    printf("%f %f %f %f \n", f[1*4+0],f[1*4+1],f[1*4+2],f[1*4+3] );
    printf("%f %f %f %f \n", f[2*4+0],f[2*4+1],f[2*4+2],f[2*4+3] );
    printf("%f %f %f %f \n", f[3*4+0],f[3*4+1],f[3*4+2],f[3*4+3] );
*/
}

ALWAYS_INLINE
float mat4_row_mult_column(float* r, int i, float *c, int j)
{
    return r[4*0+i]*c[4*j+0] + r[4*1+i]*c[4*j+1] + r[4*2+i]*c[4*j+2] + r[4*3+i]*c[4*j+3];
}

ALWAYS_INLINE
struct Mat4 mat4_mult(Mat4 a, Mat4 b)
{
    struct Mat4 c;

/*
    int i,j;
    for(i=0; i<4; i++) //row
    {
        for(j=0; j<4; j++) 
        {
            c._f[4*j+i] = mat4_row_mult_column( a._f, i, b._f, j );
        }
    }
*/


    for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
        c._f[4*j+i] = a._f[4*0+i]*b._f[4*j+0] + a._f[4*1+i]*b._f[4*j+1] + a._f[4*2+i]*b._f[4*j+2] + a._f[4*3+i]*b._f[4*j+3];
        //matC[i][j] += matA[i][x] * matB[x][j];
    }
    }


/*
    for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
        c._f[4*i+j] = 0.0f;
    }}

    for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
    for(int k = 0; k < 4; k++) {
        //c._f[4*i+j] += a._f[4*i+k] * b._f[4*k+j];

        c._f[4*i+j] += a._f[4*i+k] * b._f[4*k+j];

    }
    }
    }
*/
    return c;
}


struct Mat4 mat4_inverse(Mat4 m)
{
    Mat4 n;

    n._f[0] = m._f[5]  * m._f[10] * m._f[15] - 
             m._f[5]  * m._f[11] * m._f[14] - 
             m._f[9]  * m._f[6]  * m._f[15] + 
             m._f[9]  * m._f[7]  * m._f[14] +
             m._f[13] * m._f[6]  * m._f[11] - 
             m._f[13] * m._f[7]  * m._f[10];

    n._f[4] = -m._f[4]  * m._f[10] * m._f[15] + 
              m._f[4]  * m._f[11] * m._f[14] + 
              m._f[8]  * m._f[6]  * m._f[15] - 
              m._f[8]  * m._f[7]  * m._f[14] - 
              m._f[12] * m._f[6]  * m._f[11] + 
              m._f[12] * m._f[7]  * m._f[10];

    n._f[8] = m._f[4]  * m._f[9] * m._f[15] - 
             m._f[4]  * m._f[11] * m._f[13] - 
             m._f[8]  * m._f[5] * m._f[15] + 
             m._f[8]  * m._f[7] * m._f[13] + 
             m._f[12] * m._f[5] * m._f[11] - 
             m._f[12] * m._f[7] * m._f[9];

    n._f[12] = -m._f[4]  * m._f[9] * m._f[14] + 
               m._f[4]  * m._f[10] * m._f[13] +
               m._f[8]  * m._f[5] * m._f[14] - 
               m._f[8]  * m._f[6] * m._f[13] - 
               m._f[12] * m._f[5] * m._f[10] + 
               m._f[12] * m._f[6] * m._f[9];

    n._f[1] = -m._f[1]  * m._f[10] * m._f[15] + 
              m._f[1]  * m._f[11] * m._f[14] + 
              m._f[9]  * m._f[2] * m._f[15] - 
              m._f[9]  * m._f[3] * m._f[14] - 
              m._f[13] * m._f[2] * m._f[11] + 
              m._f[13] * m._f[3] * m._f[10];

    n._f[5] = m._f[0]  * m._f[10] * m._f[15] - 
             m._f[0]  * m._f[11] * m._f[14] - 
             m._f[8]  * m._f[2] * m._f[15] + 
             m._f[8]  * m._f[3] * m._f[14] + 
             m._f[12] * m._f[2] * m._f[11] - 
             m._f[12] * m._f[3] * m._f[10];

    n._f[9] = -m._f[0]  * m._f[9] * m._f[15] + 
              m._f[0]  * m._f[11] * m._f[13] + 
              m._f[8]  * m._f[1] * m._f[15] - 
              m._f[8]  * m._f[3] * m._f[13] - 
              m._f[12] * m._f[1] * m._f[11] + 
              m._f[12] * m._f[3] * m._f[9];

    n._f[13] = m._f[0]  * m._f[9] * m._f[14] - 
              m._f[0]  * m._f[10] * m._f[13] - 
              m._f[8]  * m._f[1] * m._f[14] + 
              m._f[8]  * m._f[2] * m._f[13] + 
              m._f[12] * m._f[1] * m._f[10] - 
              m._f[12] * m._f[2] * m._f[9];

    n._f[2] = m._f[1]  * m._f[6] * m._f[15] - 
             m._f[1]  * m._f[7] * m._f[14] - 
             m._f[5]  * m._f[2] * m._f[15] + 
             m._f[5]  * m._f[3] * m._f[14] + 
             m._f[13] * m._f[2] * m._f[7] - 
             m._f[13] * m._f[3] * m._f[6];

    n._f[6] = -m._f[0]  * m._f[6] * m._f[15] + 
              m._f[0]  * m._f[7] * m._f[14] + 
              m._f[4]  * m._f[2] * m._f[15] - 
              m._f[4]  * m._f[3] * m._f[14] - 
              m._f[12] * m._f[2] * m._f[7] + 
              m._f[12] * m._f[3] * m._f[6];

    n._f[10] = m._f[0]  * m._f[5] * m._f[15] - 
              m._f[0]  * m._f[7] * m._f[13] - 
              m._f[4]  * m._f[1] * m._f[15] + 
              m._f[4]  * m._f[3] * m._f[13] + 
              m._f[12] * m._f[1] * m._f[7] - 
              m._f[12] * m._f[3] * m._f[5];

    n._f[14] = -m._f[0]  * m._f[5] * m._f[14] + 
               m._f[0]  * m._f[6] * m._f[13] + 
               m._f[4]  * m._f[1] * m._f[14] - 
               m._f[4]  * m._f[2] * m._f[13] - 
               m._f[12] * m._f[1] * m._f[6] + 
               m._f[12] * m._f[2] * m._f[5];

    n._f[3] = -m._f[1] * m._f[6] * m._f[11] + 
              m._f[1] * m._f[7] * m._f[10] + 
              m._f[5] * m._f[2] * m._f[11] - 
              m._f[5] * m._f[3] * m._f[10] - 
              m._f[9] * m._f[2] * m._f[7] + 
              m._f[9] * m._f[3] * m._f[6];

    n._f[7] = m._f[0] * m._f[6] * m._f[11] - 
             m._f[0] * m._f[7] * m._f[10] - 
             m._f[4] * m._f[2] * m._f[11] + 
             m._f[4] * m._f[3] * m._f[10] + 
             m._f[8] * m._f[2] * m._f[7] - 
             m._f[8] * m._f[3] * m._f[6];

    n._f[11] = -m._f[0] * m._f[5] * m._f[11] + 
               m._f[0] * m._f[7] * m._f[9] + 
               m._f[4] * m._f[1] * m._f[11] - 
               m._f[4] * m._f[3] * m._f[9] - 
               m._f[8] * m._f[1] * m._f[7] + 
               m._f[8] * m._f[3] * m._f[5];

    n._f[15] = m._f[0] * m._f[5] * m._f[10] - 
              m._f[0] * m._f[6] * m._f[9] - 
              m._f[4] * m._f[1] * m._f[10] + 
              m._f[4] * m._f[2] * m._f[9] + 
              m._f[8] * m._f[1] * m._f[6] - 
              m._f[8] * m._f[2] * m._f[5];

    double det = m._f[0] * n._f[0] + m._f[1] * n._f[4] + m._f[2] * n._f[8] + m._f[3] * n._f[12];

    GS_ASSERT(det != 0);
    //if (det == 0)
    //    return false;

    det = 1.0 / det;

    for(int i = 0; i < 16; i++)
        n._f[i] *= det;

    return n;

}

ALWAYS_INLINE
struct Mat4 mat4_transpose(Mat4 a)
{
    struct Mat4 b;
    
    for(int i = 0; i < 4; i++){
    for(int j = 0; j < 4; j++){
        b._f[4*i+j] = a._f[4*j+i];
    }}
    return b;
}

ALWAYS_INLINE
struct Mat4 mat4_identity()
{   
    struct Mat4 m;

    m.v[0] = vec4_init(1.0f, 0.0f, 0.0f, 0.0f);
    m.v[1] = vec4_init(0.0f, 1.0f, 0.0f, 0.0f);
    m.v[2] = vec4_init(0.0f, 0.0f, 1.0f, 0.0f);
    m.v[3] = vec4_init(0.0f, 0.0f, 0.0f, 1.0f);

    return m;
}

ALWAYS_INLINE
struct Mat4 mat4_euler_rotation(float x, float y, float z)
{   
    x *= PI;
    y *= PI;
    z *= PI;

    double cx = cos(x);
    double sx = sin(x);
    double cy = cos(y);
    double sy = sin(y);
    double cz = cos(z);
    double sz = sin(z);
    
    struct Mat4 m;

    m.v[0].x = (float)(cy*cx); 
    m.v[0].y = (float)(cy*sx);
    m.v[0].z = (float)(-sy);
    m.v[0].w = 0.0f;

    double szsy = sz*sy;
    double czsy = cz*sy;
    
    m.v[1].x = (float)(szsy*cx-cz*sx);
    m.v[1].y = (float)(szsy*sx+cz*cx);
    m.v[1].z = (float)(sz*cy);
    m.v[1].w = 0.0f;

    m.v[2].x = (float)(czsy*cx+sz*sx);
    m.v[2].y = (float)(czsy*sx-sz*cx);
    m.v[2].z = (float)(cz*cy);
    m.v[2].w = 0.0f;

    m.v[3] = vec4_init(0.0f, 0.0f, 0.0f, 1.0f);

    return m;
}

ALWAYS_INLINE
struct Mat4 mat4_euler_rotation_and_translation(float _x, float _y, float _z, float x, float y, float z)
{   
    x *= PI;
    y *= PI;
    z *= PI;

    double cx = cos(x);
    double sx = sin(x);
    double cy = cos(y);
    double sy = sin(y);
    double cz = cos(z);
    double sz = sin(z);
    
    struct Mat4 m;

    m.v[0].x = (float)(cy*cx); 
    m.v[0].y = (float)(cy*sx);
    m.v[0].z = (float)(-sy);
    m.v[0].w = 0.0f;

    double szsy = sz*sy;
    double czsy = cz*sy;
    
    m.v[1].x = (float)(szsy*cx-cz*sx);
    m.v[1].y = (float)(szsy*sx+cz*cx);
    m.v[1].z = (float)(sz*cy);
    m.v[1].w = 0.0f;

    m.v[2].x = (float)(czsy*cx+sz*sx);
    m.v[2].y = (float)(czsy*sx-sz*cx);
    m.v[2].z = (float)(cz*cy);
    m.v[2].w = 0.0f;

    m.v[3] = vec4_init(_x, _y, _z, 1.0f);

    return m;
}

/*
ALWAYS_INLINE
struct Mat4 mat4_apply_mat3_rotation_matrix(struct Mat3 m1, struct Mat3 m2)
{   
    struct Mat4 m;
    //not used/implemented yet
    return m;
}
*/

ALWAYS_INLINE
struct Vec3 vec3_apply_rotation(struct Vec3 v, struct Mat4 m)
{   
    struct Vec3 u;

    u.x = v.x*m.v[0].x + v.y*m.v[1].x + v.z*m.v[2].x, 
    u.y = v.x*m.v[0].y + v.y*m.v[1].y + v.z*m.v[2].y, 
    u.z = v.x*m.v[0].z + v.y*m.v[1].z + v.z*m.v[2].z;

    return u;
}

ALWAYS_INLINE
struct Vec4 vec4_apply_rotation(struct Vec4 v, struct Mat4 m)
{   
    struct Vec4 u;

    u.x = v.x*m.v[0].x + v.y*m.v[1].x + v.z*m.v[2].x, 
    u.y = v.x*m.v[0].y + v.y*m.v[1].y + v.z*m.v[2].y, 
    u.z = v.x*m.v[0].z + v.y*m.v[1].z + v.z*m.v[2].z;
    u.w = 0;

    return u;
}

/*
    Rotation and translation
*/

//translation and rotation by mat4
ALWAYS_INLINE
struct Vec3 vec3_mat3_apply(struct Vec3 v, struct Mat4 m)
{   
    struct Vec3 u;
/*
    u.x = v.x*m.v[0].x + v.y*m.v[1].x + v.z*m.v[2].x + m.v[3].x, 
    u.y = v.x*m.v[0].y + v.y*m.v[1].y + v.z*m.v[2].y + m.v[3].y, 
    u.z = v.x*m.v[0].z + v.y*m.v[1].z + v.z*m.v[2].z + m.v[3].z;
*/

    u.x = v.x*m.v[0].x + v.y*m.v[1].x + v.z*m.v[2].x, 
    u.y = v.x*m.v[0].y + v.y*m.v[1].y + v.z*m.v[2].y, 
    u.z = v.x*m.v[0].z + v.y*m.v[1].z + v.z*m.v[2].z;

    u.x += m.v[3].x;
    u.y += m.v[3].y;
    u.z += m.v[3].z;

    return u;
}

void mat4_from_vec3(struct Mat4& m, struct Vec3 vx, struct Vec3 vy, struct Vec3 vz, struct Vec3 c)
{
    m.vx = vec4_init(vx, 0.0f);
    m.vy = vec4_init(vy, 0.0f);
    m.vz = vec4_init(vz, 0.0f);
    m.c = vec4_init(c, 1.0f);
}
