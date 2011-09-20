#include "t_buffer.h"

void t_buffer_reset() {
    //printf("buffer reset\n");
    int i;
    for (i=0; i < t_buff_size; i++) {
        t_buff[i] = 0;
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


// store a partially decompressed chunk here
#define t_chunk_buff_size (vm_chunk_voxel_size * sizeof(unsigned short)) + (3 * sizeof(int))
unsigned char t_chunk_buff[t_chunk_buff_size];
int t_chunk_buff_index = 0;

int nz_vox = 0;
int t_zlib_unserialize_chunk(unsigned char* buffer, int* index) {

    // buffer is filled by zlib inflate
    // index is how much of the buffer was filled

    // 1. check previous partial chunk buffer

    int i, j=0;
    int x,y,z;

    unsigned short vox_i;

    printf("unserialize chunk\n");
    //printf("%d < %d\n", j ,*index);
    
    while (j < *index) {

        // available buffer will not fill the chunk, copy remains
        if (t_chunk_buff_size - t_chunk_buff_index > t_buff_size - (*index + j)) {
            for (j; j < *index; j++) {
                t_chunk_buff[t_chunk_buff_index] = buffer[j];
                t_chunk_buff_index++;
            }
            break;
        }

        // fill chunk buffer
        for (i=t_chunk_buff_index; i < t_chunk_buff_size; i++) {
            t_chunk_buff[i] = buffer[j];
            j++;
        }

        // set chunk
        t_chunk_buff_index = 0; // rewind index for reading
        x = t_UNPACK_int(t_chunk_buff, &t_chunk_buff_index); //unpack x
        y = t_UNPACK_int(t_chunk_buff, &t_chunk_buff_index); //unpack y
        z = t_UNPACK_int(t_chunk_buff, &t_chunk_buff_index); //unpack z

        printf("%d %d %d\n", x,y,z);
        for (i=0; i < vm_chunk_voxel_size; i++) {
            vox_i = t_UNPACK_ushort(t_chunk_buff, &t_chunk_buff_index);
            if (vox_i) {
                nz_vox++;
                //printf("VOX_I:  %d\n", vox_i);
            }
            _set_chunk_voxel(x,y,z, vox_i, i);
        }

        // reset chunk buffer
        for (i=0; i < t_chunk_buff_size; i++) {
            t_chunk_buff[i] = 0;
        }
        t_chunk_buff_index = 0;
        
    }
    
    //int x,y,z;
    //x = t_UNPACK_int(buffer, t_buffer_index); //unpack x
    //y = t_UNPACK_int(buffer, t_buffer_index); //unpack y
    //z = t_UNPACK_int(buffer, t_buffer_index); //unpack z

    //printf("%d %d %d\n", x,y,z);

    //int i, ret=0;
    //for(i=0; i<vm_chunk_voxel_size; i++) {
        ////vox[i]= t_UNPACK_ushort(buffer, t_buffer_index);
        //ret = _set_chunk_voxel(x, y, z, t_UNPACK_ushort(buffer, t_buffer_index), i);
    //}

    //ret = _set_chunk_voxels(x, y, z, vox);
    //ret = _set_chunk_voxel(x, y, z, t_UNPACK_ushort(buffer, t_buffer_index), i);
    //if (ret) {
        //printf("_set_chunk_voxels failed\n");
    //}
    int ret = 0;
    //printf("NZ_VOX: %d\n", nz_vox);
    return ret;
}
