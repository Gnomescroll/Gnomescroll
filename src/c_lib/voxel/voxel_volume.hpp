#pragma once

#include <voxel/common.hpp>
#include <physics/affine.hpp>
#include <voxel/voxel_hitscan.hpp>

#if DC_CLIENT
# include <voxel/voxel_render.hpp>
#endif

namespace Voxels
{

ALWAYS_INLINE int get_ao_weight(int side_1, int side_2, int corner);
// Sets AO strength values

class VoxelVolume
{
    public:

    struct Affine local_matrix;
    struct Affine world_matrix;

    struct Affine* parent_world_matrix;

    #if DC_CLIENT
    VoxelVertexList vvl;
    #endif

    int render_id;
    bool draw;
    bool hitscan;

    float scale;    //size of voxels
    //bounding sphere
    float radius;   //update when changing scale

    Vec3i dimensions;
    Voxel* voxel;
    int index1;
    int index12;
    int index_max;

    float hdx,hdy,hdz;  //half of width, height, depth as floats

    bool needs_vbo_update;
    bool damaged;

    class VoxelHitscanElement vhe;

    //hitscan registration
    VoxelHitscanList* voxel_hitscan_list;

    #if DC_CLIENT
    class VoxelRenderList* voxel_render_list;
    int voxel_render_list_id;
    void update_vertex_list();
    #endif

    int hitscan_test(const Vec3& p, const Vec3& f, float r2, Vec3i& voxel);

    inline int voxel_ray_cast(const Vec3& position, const Vec3& direction,
                              float max_len, float* distance, Vec3i& collision);

    inline struct Vec3 get_center()
    {
        return world_matrix.c;
    }

    void set_color(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void set_color(int x, int y, int z, unsigned char rgba[4]);

    void set(int x, int y, int z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void set(int x, int y, int z, Voxel* v);

    void set_parameters(const Vec3i& dimensions, float scale);

    void draw_bounding_box();
    int point_collision_test(const Vec3& p, Vec3i& voxel);

    void init(const Vec3i& dimensions, float scale);
    void set_hitscan_properties(EntityID entity_id, EntityType entity_type, short part_id);

    VoxelVolume(const Vec3i& dimensions, float scale);
    VoxelVolume();

    ~VoxelVolume();

    private:
    #if DC_CLIENT
    ALWAYS_INLINE void push_voxel_quad(VoxelVertex* scratch, int* index,
                                       int x, int y,
                                       int z, int side, float* vset,
                                       float ox, float oy, float oz);
    #endif

    ALWAYS_INLINE Voxel* get(int x, int y, int z);

    ALWAYS_INLINE int get_as_int(int x, int y, int z);

    // Tests whether a voxel is occupied, for AO
    ALWAYS_INLINE bool _test_occludes_safe(int x, int y, int z);
    ALWAYS_INLINE void _set(int x, int y, int z, Voxel* v);
    ALWAYS_INLINE void _set(int x, int y, int z,
                            unsigned char r, unsigned char g, unsigned char b,
                            unsigned char a);

};

void init_voxel_volume();
void teardown_voxel_volume();

}   // Voxels
