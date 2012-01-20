#pragma once

#include <physics/vec3.hpp>
#include <physics/vec4.hpp>
#include <physics/mat3.hpp>
#include <physics/mat4.hpp>

#include <stdio.h>

static const char* test_string = "3\n"

class Voxel_skeleton
{
    public:

    Voxel_volume** voxel_volume_list;
    int voxel_volume_num;
    int* voxel_volume


    Voxel_skeleton(char* string)
    {
        init_from_string(string);
    }

    void init_from_string(char string)
    {
        scanf ("%d %n", &voxel_volume_num);
        printf("voxel_volume_num= %i \n", voxel_volume_num);

    }

};

void test_voxel_skeleton()
{
    class Voxel_skeleton(test_string);
}