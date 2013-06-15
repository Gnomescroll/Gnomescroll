#pragma once

#include <entity/constants.hpp>
#include <voxel/voxel_volume.hpp>
#include <voxel/voxel_dat.hpp>

namespace Voxels
{

class VoxelModel
{
    public:
        //skeleton
        void init_skeleton();
        void reset_skeleton(); // reset skeleton properties from voxdat
        void set_skeleton_root(float x, float y, float z, float theta);
        void set_skeleton_root(float *data);
        void set_biaxial_nodes(float phi);
        void update_skeleton();  //recalculate world view matrices

        void draw_skeleton();

        //skeleton
        bool skeleton_inited;

        bool skeleton_needs_update;
        int n_skeleton_nodes;
        int* vox_skeleton_transveral_list;
        struct Affine* vox_skeleton_local_matrix;
        struct Affine* vox_skeleton_world_matrix;
        bool* biaxial_nodes;

        // lifecycle / update state
        bool vox_inited;
        bool was_updated;   // NOTE: This should be reset to false every frame. TODO: change was_updated to was_updated_this_frame
        bool frozen;    // if true, refuse to update no matter what
        void freeze();
        void thaw();

        VoxDat* vox_dat;

        int n_parts;
        void init_parts(EntityID id, EntityType type);

        void set_draw(bool draw);
        void set_hitscan(bool hitscan);
        void set_vox_dat(VoxDat* vox_dat);
        void register_hitscan();
        void unregister_hitscan();

        void set_part_color(int part_num);
        void set_colors();

        void fill_part_color(int part_num, Color color);
        void fill_color(Color color);

        void update(float x, float y, float z, float theta, float phi);

        Affine* get_node(int node);

        int get_parent_node_index(int part);
        void set_node_rotation_by_part(int part, float theta, float phi, float rho);
        void set_node_rotation(int node, float theta, float phi, float rho);

        bool in_sight_of(Vec3 source, Vec3* sink);
        bool in_sight_of(Vec3 source, Vec3* sink, float failure_rate);

        VoxelVolume* vv;

        Vec3 get_center();
        Vec3 get_center(int part);
        float get_radius();
        float get_radius(int part);
        VoxelVolume* get_part(int part);

        float largest_radius(); // temporary, until real body collision detection

        VoxelModel(VoxDat* vox_dat, EntityID id, EntityType type);
        ~VoxelModel();
};

}   // Voxels
