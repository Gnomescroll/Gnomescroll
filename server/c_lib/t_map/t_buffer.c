#include "t_buffer.h"

int t_buffer_init() {
    if (t_buff == NULL) {
        t_buff = (unsigned char*)malloc(sizeof(unsigned char)*t_buff_size);
        return 0;
    }
    return 1;
}

int t_buffer_destroy() {
    if (t_buff != NULL) {
        free(t_buff);
        t_buff = NULL;
        return 0;
    }
    return 1;
}

void t_buffer_reset() {
    if (t_buff != NULL) {
        int i=0;
        for (i=0; i < t_buff_size; i++) {
            t_buff[i] = (unsigned char)0;
        }
        *t_buffer_index = 0;
    }
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

// serialize just the voxel array
int t_zlib_serialize_chunk_vox(unsigned short* vox, int vox_size, unsigned char* buffer, int* index, int buffer_size) {

    int s = (int)(sizeof(*vox));
    int n_can_fit = (buffer_size - *index) / s;
    int i;
    int n = (n_can_fit < vox_size) ? n_can_fit : vox_size;

    for (i=0; i < n; i++) {
        t_PACK_ushort(vox[i], buffer, index);
    }
    return n;
}

// wrap voxel chunk coordinates
int t_zlib_serialize_chunk(int x, int y, int z, unsigned short* vox, int vox_size, unsigned char* buffer, int* index, int buffer_size) {

    int free_space = buffer_size - *index;
    int req_space = (int)(3*sizeof(x));
    int ret = -1;
    
    if (free_space < req_space) {  // cant fit vox chunk coordinate
        return ret;
    }

    t_PACK_int(x, buffer, index); //pack x
    t_PACK_int(y, buffer, index); //pack y
    t_PACK_int(z, buffer, index); //pack z

    ret = t_zlib_serialize_chunk_vox(vox, vox_size, buffer, index, buffer_size);
    return ret;
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
static const int t_chunk_buff_size = (int)(vm_chunk_voxel_size * sizeof(unsigned short)) + (3 * sizeof(int));
//unsigned char t_chunk_buff[t_chunk_buff_size];
/*@null@*/ unsigned char* t_chunk_buff = NULL;
static int t_chunk_buff_index = 0;

int chunk_buffer_init() {
    if (t_chunk_buff == NULL) {
        t_chunk_buff = (unsigned char*)malloc(sizeof(unsigned char)*t_chunk_buff_size);
        return 0;
    }
    return 1;
}

int chunk_buffer_destroy() {
    if (t_chunk_buff != NULL) {
        free(t_chunk_buff);
        t_chunk_buff = NULL;
        return 0;
    }
    return 1;
}
        

int t_zlib_unserialize_chunk(unsigned char* buffer, int size) {

    int i,j=0;
    int x,y,z;
    unsigned short vox_i;

    while (j < size) {

        // available buffer will not fill the chunk, copy to chunk buffer and abort
        if (t_chunk_buff_size - t_chunk_buff_index > size - j) {
            for (j=j; j < size; j++) {
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

        for (i=0; i < vm_chunk_voxel_size; i++) {
            vox_i = t_UNPACK_ushort(t_chunk_buff, &t_chunk_buff_index);
            if (_set_chunk_voxel(x,y,z, vox_i, i)) return 1;
        }

        // reset chunk buffer
        for (i=0; i < t_chunk_buff_size; i++) {
            t_chunk_buff[i] = 0;
        }
        t_chunk_buff_index = 0;
        
    }
    
    return 0;
}
