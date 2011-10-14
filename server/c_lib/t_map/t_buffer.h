#pragma once

static const int t_buff_size = (8*((512*sizeof(unsigned short)) + (sizeof(int) * 3)));

//unsigned char t_buff[t_buff_size];
unsigned char *t_buff = NULL;

int t_buffer_init();
int t_buffer_destroy();

int _t_bi=0, *t_buffer_index = &_t_bi;

int t_zlib_serialize_chunk_vox(unsigned short* vox, int vox_size, unsigned char* buffer, int* index, int buffer_size);
int t_zlib_serialize_chunk(int x, int y, int z, unsigned short* vox, unsigned char* buffer, int* index, int buffer_size);

int t_zlib_unserialize_chunk(unsigned char* buffer, int size);
