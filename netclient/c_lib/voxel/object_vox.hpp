#pragma once

#include <c_lib/physics/vector.hpp>
#include <c_lib/voxel/voxel_volume.hpp>
#include <c_lib/voxel/voxel_body.hpp>

class Object_vox 
{
    public:


        //skeleton
        void init_skeleton(VoxBody* vox_dat);
        void Object_vox::set_skeleton_root(float x, float y, float z, float theta);
        void update_skeleton();  //recalculate world view matrices

        bool skeleton_needs_update;
        int n_skeleton_nodes;
        int* vox_skeleton_transveral_list;
        struct Mat4* vox_skeleton_local_matrix;
        struct Mat4* vox_skeleton_world_matrix;
        
        //parts list
        int n_parts;
        void right(Vector *f, float theta);
        void forward(Vector *f, float theta);
        void look(Vector *f, float theta, float phi);
        void look(float f[3], float theta, float phi);
        void look(double f[3], double theta, double phi);

        float largest_radius(); // temporary, until real body collision detection

        Voxel_volume* vv;

        bool inited;
        void init_parts(VoxBody* vox_dat, int id, int type);
        void init_parts(VoxBody* vox_dat, int id, int type, int team);
        void update(VoxBody* vox_dat, float x, float y, float z, float theta, float phi);
        void set_draw(bool draw);
        void set_hitscan(bool hitscan);

        void update_team_color(VoxBody* vox_dat, int team);


        Object_vox(int num_parts);
        Object_vox(int num_parts, VoxBody* vox_dat, int id, int type);
        Object_vox(int num_parts, VoxBody* vox_dat, int id, int type, int team);
        ~Object_vox();
};
