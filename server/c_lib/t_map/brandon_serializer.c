inline void PACK_ushort(unsigned short d, unsigned char* buffer, int* n){
        *((unsigned short*)(buffer+*n)) = d;
        *n += sizeof(unsigned short);
}

inline void PACK_short(short d, unsigned char* buffer, int* n){
        *((short*)(buffer+*n)) = d;
        *n += sizeof(short);
}

//#define buff_size (512*sizeof(unsigned short)+ 3*sizeof(short))
//#define buff_size (512*sizeof(unsigned short))
#define buff_size 131072
unsigned char buff[buff_size];
int _buffer_index=0;
int *buffer_index = &_buffer_index;
void serialize_chunk(short x, short y, short z, unsigned short* vox, unsigned char* buffer, int* buffer_index) {

    int _n = *buffer_index;
    //PACK_short(x, buffer, buffer_index); //pack x
    //PACK_short(y, buffer, buffer_index); //pack y
    //PACK_short(z, buffer, buffer_index); //pack z

    int i;
    for(i=0;i<512;i++) {
        PACK_ushort(vox[i], buffer, buffer_index);
    }
    //printf("packed map chunk into %i bytes\n", *buffer_index-_n);
    //printf("remaining buffer space: %d\n", buff_size-*buffer_index);
    //printf("buff size: %d\n", buff_size);
}


/* unpacking */
inline unsigned short UNPACK_ushort(unsigned char* buffer, int*n) {
        unsigned short d = *((unsigned short*)(buffer+*n));
        *n += sizeof(unsigned short);
    return d;
}
 
inline short UNPACK_short(unsigned char* buffer, int*n) {
        short d = *((short*)(buffer+*n));
        *n += sizeof(short);
    return d;
}

void unserialize_chunk(unsigned char* buffer, int* buffer_index) {
    short x,y,z;
    unsigned short vox[512];

    //x = UNPACK_short(buffer, buffer_index); //unpack x
    //y = UNPACK_short(buffer, buffer_index); //unpack y
    //z = UNPACK_short(buffer, buffer_index); //unpack z

    int i;
    for(i=0; i<512; i++) {
        vox[i]= UNPACK_ushort(buffer, buffer_index);
    }
    //now do something with vox and x,y,z
}
