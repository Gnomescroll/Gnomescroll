#pragma once

#include <sys/stat.h>

off_t fsize(const char *filename);

class Voxel_loader
{
    private:
        //this function will always return on a new line or null
        void check_for_comments(char* s, int* index);

    public:

        void read_skeleton(char* file_name, VoxBody* vox_dat);
        void read_voxel_volume(char* file_name, int part_num, VoxBody* vox_dat);

        Voxel_loader() {}
};

void test_voxel_skeleton();
