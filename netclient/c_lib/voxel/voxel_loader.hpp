#pragma once

#include <sys/stat.h>

off_t fsize(const char *filename);

class Voxel_loader
{
    private:
        //this function will always return on a new line or null
        void check_for_comments(char* s, int* index);

    public:

    Voxel_loader() {}

    void read_skeleton(char* file_name);
    void read_voxel_volume(char* file_name, class Voxel_volume* vox);
};

void test_voxel_skeleton();

    return;