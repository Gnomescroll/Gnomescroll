#pragma once

#include <sys/stat.h>

void read_skeleton(char* file_name, VoxBody* vox_dat);
void read_voxel_volume(char* file_name, int part_num, VoxBody* vox_dat);
