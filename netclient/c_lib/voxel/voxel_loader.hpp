#pragma once

void read_skeleton(char* file_name, VoxDat* vox_dat);
void read_voxel_volume(char* file_name, int part_num, int skeleton_parent_matrix, VoxDat* vox_dat);
