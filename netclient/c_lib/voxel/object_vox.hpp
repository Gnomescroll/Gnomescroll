#pragma once

#include <c_lib/physics/vector.hpp>
#include <c_lib/voxel/voxel_volume.hpp>
#include <c_lib/voxel/voxel_body.hpp>

template <class Obj, int NUM_PARTS>
class Object_vox {
    public:
        int n_parts;
        void right(Vector *f, float theta);
        void forward(Vector *f, float theta);
        void look(Vector *f, float theta, float phi);

        Obj* a;

        Voxel_volume vv[NUM_PARTS];

        void init_parts(VoxBody* vox_dat);
        void update(VoxBody* vox_dat);
        void set_draw(bool draw);

        explicit Object_vox(Obj* a, VoxBody* vox_dat);
        ~Object_vox();
};
