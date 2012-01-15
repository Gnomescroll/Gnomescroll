#pragma once 

#include <voxel/common.h>
#include <physics/vector.hpp>
#include <physics/matrix.hpp>

#ifdef DC_CLIENT
class Voxel_render_list; //forward declaration
#endif
class Voxel_hitscan_list; //forward declaration


class Voxel_volume
{
    private:
#ifdef DC_CLIENT
    inline void push_voxel_quad(Voxel_vertex* scratch, int* index, int x, int y, int z, int side);
#endif

    inline Voxel* get(int x, int y, int z) __attribute((always_inline)) 
    { return &voxel[x+(y << index1)+(z << index12)]; }

    inline unsigned int get_as_int(int x, int y, int z) __attribute((always_inline)) 
    { return voxel[x+(y << index1)+(z << index12)].color; }

/*
    Tests whether a voxel is occupied, for AO
*/
    inline unsigned int _test_occludes_safe(int x, int y, int z) __attribute((always_inline)) 
    { 
        unsigned int index= x+(y << index1)+(z << index12);
        if(index >= index_max) return 0;
        if(voxel[x+(y << index1)+(z << index12)].color == 0) return 0;
        return 1;
    }

    inline void _set(int x, int y, int z, Voxel* v) __attribute((always_inline))
    { voxel[x+(y << index1)+(z << index12)] = *v; }

    inline void _set(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a) __attribute((always_inline))
    {  Voxel* v = &voxel[x+(y << index1)+(z << index12)]; v->r = r;v->g = g;v->b = b;v->a = a; }

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
    float radius;   //update when changing scale

    int xdim,ydim,zdim;
    int _xdim,_ydim,_zdim;
    Voxel* voxel;
    int index1;
    int index12;
    unsigned int index_max;

    float hdx,hdy,hdz;  //half of width, height, depth as floats

    bool needs_vbo_update;

#ifdef DC_CLIENT
    Voxel_render_list* voxel_render_list;
    void register_with_renderer(Voxel_render_list* vrl);
    void update_vertex_list();
#endif

    void set_color(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void set_color(int x, int y, int z, unsigned char rgba[4]);
    //void set_colors(int start, int end, unsigned char rgba[][4]);

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
