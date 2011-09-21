#pragma once

//#define t_buff_size (32*8*2*((512*2) + (sizeof(int) * 3)))    // 256kB --  2*[columns * chunks/col * voxels/chunk * bytes/voxel].  Values below this are associated with the final call to deflate() segfaulting.

#define t_buff_size (8*((512*sizeof(unsigned short)) + (sizeof(int) * 3)))

unsigned char t_buff[t_buff_size];
int _t_bi=0, *t_buffer_index = &_t_bi;

int t_zlib_serialize_chunk_vox(unsigned short* vox, int vox_size, unsigned char* buffer, int* index, int buffer_size);
int t_zlib_serialize_chunk(int x, int y, int z, unsigned short* vox, unsigned char* buffer, int* index, int buffer_size);

int t_zlib_unserialize_chunk(unsigned char* buffer, int size);
