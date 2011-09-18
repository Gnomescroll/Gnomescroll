#define t_buff_size ((131072*2) * sizeof(int) * 3)    // 256kB -- 32*8*512*2  [columns * chunks/col * voxels/chunk * bytes/voxel].  Values below this are associated with the final call to deflate() segfaulting.
unsigned char buff[t_buff_size];
int* _t_bi=0, *t_buffer_index = &_t_bi;

/* Functions */


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

    int _n = *t_buffer_index;
    t_PACK_int(x, buffer, t_buffer_index); //pack x
    t_PACK_int(y, buffer, t_buffer_index); //pack y
    t_PACK_int(z, buffer, t_buffer_index); //pack z

    int i;
    for(i=0;i<512;i++) {
        t_PACK_ushort(vox[i], buffer, t_buffer_index);
    }
    //printf("packed map chunk into %i bytes\n", *t_buffer_index-_n);
    //printf("remaining buffer space: %d\n", t_buff_size-*t_buffer_index);
    //printf("buff size: %d\n", t_buff_size);
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

void t_zlib_unserialize_chunk(unsigned char* buffer, int* t_buffer_index) {
    short x,y,z;
    unsigned short vox[512];

    //x = t_UNPACK_short(buffer, t_buffer_index); //unpack x
    //y = t_UNPACK_short(buffer, t_buffer_index); //unpack y
    //z = t_UNPACK_short(buffer, t_buffer_index); //unpack z

    int i;
    for(i=0; i<512; i++) {
        vox[i]= t_UNPACK_ushort(buffer, t_buffer_index);
    }
    //now do something with vox and x,y,z
}
