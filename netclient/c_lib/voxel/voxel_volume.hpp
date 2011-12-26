#pragma once 

#include <voxel/common.h>
#include <physics/vector.hpp>

//z vector rotated in x,y plane by theta degrees
//theta from 0 to 1
static inline struct Vector unit_z_rot(float theta) __attribute((always_inline));
struct Vector unit_z_rot(float theta)
{   
    Vector u;
    u.x = 0.0;
    u.y = sin(theta * 2*PI);
    u.z = cos(theta * 2*PI);
    return u;
}

/*
optimization: compute matix and return matrix
*/
struct Vector euler_rotation(Vector v, float theta) __attribute((always_inline));
struct Vector euler_rotation(Vector v, float x, float y, float z)
{   
    x *= 2*PI;
    y *= 2*PI;
    z *= 2*PI;

    double cx = cos(x);
    double sx = sin(x);
    double cy = cos(y);
    double sy = sin(y);
    double cz = cos(z);
    double sz = sin(z);
    
    Vector m[3];
    Vector u;

    m[0].x = (cy*cz); 
    m[0].y = (cy*sz);
    m[0].z = (-sy);

    double sxsy = sx*sy;
    double cxsy = cx*sy;
    
    m[1].x = (sxsy*cz-cx*sz);
    m[1].y = (sxsy*sz+cx*cz);
    m[1].z = (sx*cy);

    m[2].x = (cxsy*cz+sx*sz);
    m[2].y = (cxsy*sz-sx*cz);
    m[2].z = (cx*cy);


    u.x = v.x*m[0].x + v.y*m[1].x + v.z*m[2].x, 
    u.y = v.x*m[0].y + v.y*m[1].y + v.z*m[2].y, 
    u.z = v.x*m[0].z + v.y*m[1].z + v.z*m[2].z;

    return u;
}

#ifdef DC_CLIENT
    class Voxel_render_list; //forward declaration
#endif

class VBO_t
{
    public:
    unsigned short vnum;   //number of vertices
    unsigned short voff;   //offset of vertices

    VBO_t()
    {
        vnum = 0;
        voff = 0;
    }
};



class Voxel_volume
{
    public:

#ifdef DC_CLIENT
    VBO_t vbo_t[2]; //VBO for drawing properties
#endif

    Vector v[4]; // forward, up, right (x,y,z), offset
    float scale;    //size of voxels
    //bounding sphere
    Vector center;
    float radius2;

    int xdim,ydim,zdim;
    int _xdim,_ydim,_zdim;
    Voxel* voxel;
    int index1, index12;

    float hdx,hdy,hdz;  //half of width, height, depth as floats

    bool needs_vbo_update;

#ifdef DC_CLIENT
    Voxel_render_list* voxel_render_list;
    void register_with_renderer(Voxel_render_list* vrl);
#endif

    void set_unit_axis();

    //forward and up vector
    void set_axis(Vector* f, Vector* u);

    void set_rotated_unit_axis(float x_angle, float y_angle, float z_angle);

    void update_center();
    void set_center(float x, float y, float z);

    void set(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void set(int x, int y, int z, Voxel* v);

    inline Voxel* get(int x, int y, int z) __attribute((always_inline)) 
    {
        return &voxel[x+(y << index1)+(z << index1)];
    }

    //internal methods
    inline void _set(int x, int y, int z, Voxel* v) __attribute((always_inline))
    {
        voxel[x+(y << index1)+(z << index1)] = *v;
    }

    inline void _set(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a) __attribute((always_inline))
    {
        Voxel* v = &voxel[x+(y << index1)+(z << index1)];
        v->r = r;v->g = g;v->b = b;v->a = a;
    }

    Voxel_volume(int __xdim, int __ydim, int __zdim, float _scale);

    ~Voxel_volume();

    void draw_bounding_box();

};



void voxel_test()
{
    //static Voxel_volume vv(1,1,1, 1.0);
    static Voxel_volume vv(4,4,4, 1.0);
    //Voxel* v = vv.get(4,5,6);

    //static float c = 0.0;
    //c -= 0.02;
    
    //vv.set_center( -5.0, -5.0, 10.0);


    static float c0 = 0.0;
    static float c1 = 0.0;
    static float c2 = 0.0;
    c0 += 0.0050 / (2*PI);
    c1 += 0.0025 / (2*PI);
    c2 += 0.0100 / (2*PI);

    //printf("start\n");
    //printf("1 v[3] x,y,z= %f, %f, %f \n", vv.v[3].x, vv.v[3].y, vv.v[3].z);
    
    vv.set_rotated_unit_axis(c0, c1, c2);

    vv.set_center( 2.0, 2.0, 2.0);

    vv.draw_bounding_box();
}

