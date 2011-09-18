#include "t_buffer.h"

void t_buffer_reset() {
    int i;
    for (i=0; i < t_buff_size; i++) {
        buff[i] = 0;
    }
    *t_buffer_index = 0;
}


/* Packing buffer */
inline void t_PACK_ushort(unsigned short d, unsigned char* buffer, int* n){
        *((unsigned short*)(buffer+*n)) = d;
        *n += sizeof(unsigned short);
}

inline void t_PACK_short(short d, unsigned char* buffer, int* n){
        *((short*)(buffer+*n)) = d;
        *n += sizeof(short);
}

inline void t_PACK_int(int d, unsigned char* buffer, int* n) {
    *((int*)(buffer+*n)) = d;
    *n += sizeof(int);
}

void t_zlib_serialize_chunk(int x, int y, int z, unsigned short* vox, unsigned char* buffer, int* t_buffer_index) {

    t_PACK_int(x, buffer, t_buffer_index); //pack x
    t_PACK_int(y, buffer, t_buffer_index); //pack y
    t_PACK_int(z, buffer, t_buffer_index); //pack z

    int i;
    for(i=0;i<vm_chunk_voxel_size;i++) {
        t_PACK_ushort(vox[i], buffer, t_buffer_index);
    }
}


/* unpacking */
inline unsigned short t_UNPACK_ushort(unsigned char* buffer, int*n) {
        unsigned short d = *((unsigned short*)(buffer+*n));
        *n += sizeof(unsigned short);
    return d;
}
 
inline short t_UNPACK_short(unsigned char* buffer, int*n) {
        short d = *((short*)(buffer+*n));
        *n += sizeof(short);
    return d;
}

inline int t_UNPACK_int(unsigned char* buffer, int*n) {
        int d = *((int*)(buffer+*n));
        *n += sizeof(int);
    return d;
}

void t_zlib_unserialize_chunk(unsigned char* buffer, int* t_buffer_index) {
    int x,y,z;
    unsigned short vox[vm_chunk_voxel_size];

    x = t_UNPACK_int(buffer, t_buffer_index); //unpack x
    y = t_UNPACK_int(buffer, t_buffer_index); //unpack y
    z = t_UNPACK_int(buffer, t_buffer_index); //unpack z

    int i, ret;
    for(i=0; i<vm_chunk_voxel_size; i++) {
        vox[i]= t_UNPACK_ushort(buffer, t_buffer_index);
    }

    ret = _set_chunk_voxels(x, y, z, vox);
    if (ret) printf("_set_chunk_voxels failed\n");
    
}
