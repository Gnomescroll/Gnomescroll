#pragma once

#define t_buff_size (((32*8*512*2) *2) * sizeof(int) * 3)    // 256kB --  2*[columns * chunks/col * voxels/chunk * bytes/voxel].  Values below this are associated with the final call to deflate() segfaulting.

unsigned char buff[t_buff_size];
int _t_bi=0, *t_buffer_index = &_t_bi;

void t_zlib_serialize_chunk(int x, int y, int z, unsigned short* vox, unsigned char* buffer, int* t_buffer_index);
void t_zlib_unserialize_chunk(unsigned char* buffer, int* t_buffer_index);
