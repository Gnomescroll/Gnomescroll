#pragma once

#include <c_lib/voxel/voxel_volume.hpp>
#include <c_lib/voxel/voxel_dat.hpp>

class Voxel_model 
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
        //parts voxel volume parst
        bool vox_inited;
        bool was_updated;

        VoxDat* vox_dat;

        int n_parts;
        void init_parts(int id, int type);

        void set_draw(bool draw);
        void set_hitscan(bool hitscan);
        void set_vox_dat(VoxDat* vox_dat);
        void register_hitscan();
        void unregister_hitscan();

        void set_part_color(int part_num);
        void set_part_team_color(int part_num, unsigned char team_r, unsigned char team_g, unsigned char team_b);
        void set_colors();
        void update_team_color(int team);

        void update(float x, float y, float z, float theta, float phi);
        void restore(int team=-1);

        void set_arm(float theta, float phi);
        Affine* get_node(int node);

        bool in_sight_of(Vec3 source, Vec3* sink);

        Voxel_volume* vv;

        Voxel_volume* get_part(int part);

        float largest_radius(); // temporary, until real body collision detection

        Voxel_model(VoxDat* vox_dat, int id, int type);
        Voxel_model(VoxDat* vox_dat, int id, int type, int team);
        ~Voxel_model();
};
