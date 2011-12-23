#pragma once


class Orientation_matrix
{
    
    float v[4][4];
    //x,y,z,offset

    //multiply current by other
    void multiply(Orientation_matrix* m)
    {
        return;
    }

    Orientation_matrix()
    {
        return;
    }

};

unsigned NextPow2( unsigned x ) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return ++x;
}

int pow2_1(int n)
{
    int x = 1;
    int i = 0;
    while(x < n) 
    {
        i++;
        x <<= 1;
    }
    return i;
} 

int pow2_2(int n)
{
    int x = 1;
    //int i = 0;
    while(x < n) 
    {
        //i++;
        x <<= 1;
    }
    return x;
} 

#include <voxel/common.h>
#include <physics/vector.hpp>

class Voxel_volume
{
    public:
    Vector v[4]; // forward, up, right (x,y,z), offset
    float scale;    //size of voxels
    //bounding sphere
    Vector center;
    float radius2;

    bool needs_vbo_update;

    int xdim,ydim,zdim;
    int _xdim,_ydim,_zdim;
    Voxel* voxel;
    int index1, index12;

    float hdx,hdy,hdz;  //half of width, height, depth as floats

    void update()
    {
        

    }

    //vector_cross(struct Vector* v1, struct Vector* v2, struct Vector* dest)
    //forward and up vector
    void set_axis(Vector* f, Vector* u)
    {
        v[0] = *f;
        v[2] = *u; 
        vector_cross(f, u, &v[1]);
    }

    //applies rotation
    void rotate_axis(float theta, float phi)
    {
        

    }

    // testing/tweaking
    void translate_center(float dx, float dy, float dz)
    {
        center.x += dx;
        center.y += dy;
        center.z += dz;

        Vector vx = vector_scalar2(&v[0],hdx*scale);
        Vector vy = vector_scalar2(&v[1],hdy*scale);
        Vector vz = vector_scalar2(&v[2],hdz*scale);
        v[4] = vector_add4(&vx,&vy,&vz,&center);
    }

    void set_center(float x, float y, float z)
    {
        center.x = x;
        center.y = y;
        center.z = z;

        Vector vx = vector_scalar2(&v[0],hdx*scale);
        Vector vy = vector_scalar2(&v[1],hdy*scale);
        Vector vz = vector_scalar2(&v[2],hdz*scale);
        v[4] = vector_add4(&vx,&vy,&vz,&center);
    }

    inline Voxel* get(int x, int y, int z) __attribute((always_inline)) 
    {
        //if(x+(y << index1)+(z << index1) != (x+_xdim*y+(_xdim*_ydim*z) ) ) printf("ERROR!\n");
        //printf("1: %i, %i, %i \n",x, (y << index1), (z << index1) );
        //printf("2: %i, %i, %i \n", x,_xdim*y, _xdim*_ydim*z );
        return &voxel[x+(y << index1)+(z << index1)];
    }

    inline void set(int x, int y, int z, Voxel* v) __attribute((always_inline))  __attribute((always_inline)) 
    {
        voxel[x+(y << index1)+(z << index1)] = *v;
    }

    inline void set(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a) __attribute((always_inline))  __attribute((always_inline)) 
    {
        Voxel* v = &voxel[x+(y << index1)+(z << index1)];
        v->r = r;
        v->g = g;
        v->b = b;
        v->a = a;
    }

    Voxel_volume(int __xdim, int __ydim, int __zdim, float _scale)
    {
        needs_vbo_update = false;
        scale = _scale;

        v[0] = Vector_init(1.0f,0.0f,0.0f);
        v[1] = Vector_init(0.0f,1.0f,0.0f);
        v[2] = Vector_init(0.0f,0.0f,1.0f);
        //v[3] = Vector_init(0,0,0);  //center
        this->set_center(0.0,0.0,0.0);

        xdim = __xdim;
        ydim = __ydim;
        zdim = __zdim;

        hdx = ((float) xdim) / 2;
        hdy = ((float) ydim) / 2;
        hdz = ((float) zdim) / 2;

        radius2 =  (hdx*hdz + hdy*hdy + hdz*hdz) * (scale*scale); //radius squared of bounding sphere

        _xdim = pow2_2(__xdim);
        _ydim = pow2_2(__ydim);
        _zdim = pow2_2(__zdim);
        
        index1 = pow2_1(__xdim);
        index12 = pow2_1(__xdim) + pow2_1(__ydim);

        printf("__xdim, __ydim, __zdim= %i, %i, %i \n", __xdim, __ydim,__zdim);
        printf("_xdim, _ydim, _zdim= %i, %i, %i \n", _xdim, _ydim, _zdim);
        printf("index1, index12= %i, %i \n", index1, index12);

        voxel = new Voxel[_xdim*_ydim*_zdim];
    }

    ~Voxel_volume()
    {
        delete voxel;
    }
};

/*
inline Voxel* Voxel_volume::get(int x, int y, int z) __attribute((always_inline)) = 0;
{
    if(x+(index1<<y)+(index12<<z) != (x+_xdim*y+(_xdim*_ydim*z) ) ) printf("ERROR!\n");
    return &voxel[x+(index1<<y)+(index12<<z)]   ;

}
*/

void voxel_test()
{
    return;
    Voxel_volume vv(9,9,9, 1.0);
    Voxel* v = vv.get(4,5,6);

    vv.set_center( -5.0, -5.0, 10.0);
}