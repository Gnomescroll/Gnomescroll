#pragma once

#include <physics/vec3.hpp>
#include <physics/vec4.hpp>
#include <physics/mat3.hpp>
#include <physics/mat4.hpp>

//row times column, column major order

float row_mult_column(float* r, int i, float *c, int j)  __attribute((always_inline));
struct Mat4 mat4_mult(Mat4 a, Mat4 b) __attribute((always_inline));

class Voxel_skeleton
{
    private:
    public:

    struct Mat4 root_node; //parent world matrix for first volume

    Voxel_volume* voxel_volume_list;
    int num_volumes;

    int* skeleton_tree; //transversal order
    //struct Mat4* world_matrices; // put world matrix in the volumes

    //set offset and rotation
    void set_root_node(float x, float y, float z, float theta);

    void update_world_matrices();

    Voxel_skeleton(int volumes);
    ~Voxel_skeleton();
};

