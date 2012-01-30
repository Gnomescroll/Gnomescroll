#pragma once 

#include <voxel/common.h>

//#include <physics/vector.hpp>
//#include <physics/matrix.hpp>

//#include <physics/vec3.hpp>
//#include <physics/vec4.hpp>
//#include <physics/mat3.hpp>
//#include <physics/mat4.hpp>
#include <physics/affine.hpp>

#include <voxel/voxel_hitscan.hpp>

class Voxel_render_list; //forward declarations

class Voxel_volume
{
    public:

    struct Affine local_matrix;
    struct Affine world_matrix;

    struct Affine* parent_world_matrix;

    //Vec4 v[4]; // forward, up, right, offset

#ifdef DC_CLIENT
    Voxel_vertex_list vvl;
#endif

    int id;
    bool draw;
    bool hitscan;

    float scale;    //size of voxels
    //bounding sphere
    Vec3 center;
    float radius;   //update when changing scale

    int xdim,ydim,zdim;
    int _xdim,_ydim,_zdim;
    Voxel* voxel;
    int index1;
    int index12;
    unsigned int index_max;

    float hdx,hdy,hdz;  //half of width, height, depth as floats

    bool needs_vbo_update;

    struct Voxel_hitscan_element vhe;

#ifdef DC_CLIENT
    class Voxel_render_list* voxel_render_list;
    void register_with_renderer(Voxel_render_list* vrl);
    void update_vertex_list();
#endif

    void set_color(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void set_color(int x, int y, int z, unsigned char rgba[4]);

    //hitscan registration
    Voxel_hitscan_list* voxel_hitscan_list;

    void set(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void set(int x, int y, int z, Voxel* v);

    void set_parameters(int xdim, int ydim, int zdim, float scale);

    void draw_bounding_box();

    void init(int xdim, int ydim, int zdim, float scale);
    void set_hitscan_properties(short entity_id, short entity_type, short part_id);

    Voxel_volume(int xdim, int ydim, int zdim, float scale);
    Voxel_volume();

    ~Voxel_volume();

    private:
#ifdef DC_CLIENT
    inline void push_voxel_quad(Voxel_vertex* scratch, int* index, int x, int y, int z, int side, float* vset, float ox,float oy,float oz) __attribute((always_inline));
#endif

    inline Voxel* get(unsigned int x, unsigned int y, unsigned int z) __attribute((always_inline));

    inline unsigned int get_as_int(unsigned int x, unsigned int y, unsigned int z) __attribute((always_inline));

/*
    Tests whether a voxel is occupied, for AO
*/
    inline unsigned int _test_occludes_safe(unsigned int x, unsigned int y, unsigned int z) __attribute((always_inline));
    inline void _set(unsigned int x, unsigned int y, unsigned int z, Voxel* v) __attribute((always_inline));
    inline void _set(unsigned int x, unsigned int y, unsigned int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a) __attribute((always_inline));

};
