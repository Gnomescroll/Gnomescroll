#pragma once

#include <c_lib/physics/vector.hpp>
#include <c_lib/voxel/voxel_volume.hpp>
#include <c_lib/voxel/voxel_dat.hpp>

class Voxel_model 
{
    public:
        //skeleton
        void init_skeleton(VoxDat* vox_dat);
        void set_skeleton_root(float x, float y, float z, float theta);
        void update_skeleton();  //recalculate world view matrices

        //skeleton
        bool skeleton_inited;

        bool skeleton_needs_update;
        int n_skeleton_nodes;
        int* vox_skeleton_transveral_list;
        struct Affine* vox_skeleton_local_matrix;
        struct Affine* vox_skeleton_world_matrix;
        
        //parts voxel volume parst
        bool vox_inited;

        int n_parts;
        void init_parts(VoxDat* vox_dat, int id, int type);

        void set_draw(bool draw);
        void set_hitscan(bool hitscan);
        void register_hitscan();
        void update_team_color(VoxDat* vox_dat, int team);

        void update(VoxDat* vox_dat, float x, float y, float z, float theta, float phi);

        Voxel_volume* vv;

        float largest_radius(); // temporary, until real body collision detection

        Voxel_model(VoxDat* vox_dat, int id, int type);
        Voxel_model(VoxDat* vox_dat, int id, int type, int team);
        ~Voxel_model();
};
