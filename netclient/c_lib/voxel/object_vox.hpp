#pragma once

#include <c_lib/physics/vector.hpp>
#include <c_lib/voxel/voxel_volume.hpp>
#include <c_lib/voxel/voxel_body.hpp>

class Object_vox {
    public:
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
