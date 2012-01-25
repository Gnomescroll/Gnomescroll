#pragma once

#include <physics/vec3.hpp>
#include <physics/vec4.hpp>
#include <physics/mat3.hpp>
#include <physics/mat4.hpp>

#include <stdio.h>

//row times column, column major order

float row_mult_column(float* r, int i, float *c, int j)  __attribute((always_inline));

float row_mult_column(float* r, int i, float *c, int j)
{
    return r[4*0+i]*c[4*j+0] + r[4*1+i]*c[4*j+1] + r[4*2+i]*c[4*j+2] + r[4*3+i]*c[4*j+3];
}

/*
    r[4*0+i]
    r[4*1+i]
    r[4*2+i]
    r[4*3+i]

    c[4*j+0]
    c[4*j+1]
    c[4*j+2]
    c[4*j+3]
*/

struct Mat4 mat4_mult(Mat4 a, Mat4 b) __attribute((always_inline));

struct Mat4 mat4_mult(Mat4 a, Mat4 b)
{
    struct Mat4 c;
    int i,j;
    for(i=0; i<4; i++) //row
    {
        for(j=0; j<4; j++) 
        {
            c._f[4*j+i] = row_mult_column( a._f, i, b._f, j );
        }
    }
    return c;
}

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
    void set_root_node(float x, float y, float z, float theta)
    {
        root_node = mat4_euler_rotation( 0.0,0.0,1.0);
        root_node.v[3] = vec4_init(x,y,z, 1.0);
    }

    void update_world_matrices()
    {
        struct Mat4* local_mat;
        struct Mat4* world_mat;
        struct Mat4* parent_world_mat;

        //root node
        voxel_volume_list[0].world_matrix = mat4_mult( root_node, voxel_volume_list[0].local_matrix );

        for(int i=1; i<num_volumes; i++)
        {
            local_mat =  &voxel_volume_list[ skeleton_tree[ i*2+0 ]].local_matrix;  //current node
            world_mat = &voxel_volume_list[ skeleton_tree[ i*2+0 ]].world_matrix;
            parent_world_mat = &voxel_volume_list[ skeleton_tree[ i*2+1 ]].world_matrix; //parent world matrix

            *world_mat = mat4_mult( *parent_world_mat, *local_mat );
        }    

    }

    Voxel_skeleton(int volumes)
    {
        num_volumes = volumes;
        skeleton_tree = new int[2*volumes];

        voxel_volume_list = new Voxel_volume[volumes];
    }

    ~Voxel_skeleton()
    {
        delete[] skeleton_tree;
        delete[] voxel_volume_list;
    }



};

