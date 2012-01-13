#pragma once 

#include <voxel/common.h>
#include <physics/vector.hpp>

/*
optimization: compute matix and return matrix
*/
// possibly wrong
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

class Voxel_hitscan_list; //forward declaration

#ifdef DC_CLIENT

struct Voxel_normal
{
    union
    {
        char normal[4]; //16
        unsigned int n;
    };
};

struct Voxel_vertex
{
    float x,y,z;
    union
    {
        unsigned char rgba[4]; //12
        unsigned int color;
    };
    union
    {
        char normal[4]; //16
        unsigned int n;
    };
    //can compute normals from t
};

class Voxel_vertex_list
{
    public:
    Voxel_vertex* vertex_list;   //number of vertices
    int size;   //offset of vertices

    unsigned short vnum;   //number of vertices
    unsigned short voff;   //offset of vertices

    Voxel_vertex_list()
    :
    vertex_list(NULL),
    size(0), vnum(0), voff(0)
    {}
};
#endif

class Voxel_volume
{
    private:
#ifdef DC_CLIENT
    inline void push_voxel_quad(Voxel_vertex* scratch, int* index, int x, int y, int z, int side);
#endif

    inline Voxel* get(int x, int y, int z) __attribute((always_inline)) 
    {
        return &voxel[x+(y << index1)+(z << index1)];
    }


    inline unsigned int get_as_int(int x, int y, int z) __attribute((always_inline)) 
    {
        return voxel[x+(y << index1)+(z << index1)].color;
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

    public:

#ifdef DC_CLIENT
    Voxel_vertex_list vvl;
#endif
    int id;
    bool draw;

    Vector v[4]; // forward, up, right (x,y,z), offset

    float scale;    //size of voxels
    //bounding sphere
    Vector center;
    float radius2;  // unused

    int xdim,ydim,zdim;
    int _xdim,_ydim,_zdim;
    Voxel* voxel;
    int index1;
    int index12;    //unused

    float hdx,hdy,hdz;  //half of width, height, depth as floats

    bool needs_vbo_update;

#ifdef DC_CLIENT
    Voxel_render_list* voxel_render_list;
    void register_with_renderer(Voxel_render_list* vrl);
    void update_vertex_list();
#endif

    //hitscan registration
    Voxel_hitscan_list* voxel_hitscan_list;
    //void register_with_hitscan(Voxel_hitscan_list* vhl);

    //methods
    void set_unit_axis();

    //forward and up vector
    void set_axis(struct Vector* f, struct Vector* u);

    void set_rotated_unit_axis(float x_angle, float y_angle, float z_angle);

    void update_center();
    void set_center(float x, float y, float z);

    void set(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void set(int x, int y, int z, Voxel* v);

    void set_parameters(int xdim, int ydim, int zdim, float scale);

    void draw_bounding_box();

    void init(int xdim, int ydim, int zdim, float scale);

    Voxel_volume(int xdim, int ydim, int zdim, float scale);
    Voxel_volume();

    ~Voxel_volume();
};
