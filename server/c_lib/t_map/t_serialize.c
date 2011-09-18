#include "t_serialize.h"
#include "brandon_serializer.c"

// buffer size for compression stream
#define CHUNK_Z (sizeof(unsigned short[512])*16)

//int _save_to_disk(const char* fn) {
    //FILE* f = fopen(fn, "wb");
    //if (f != NULL) {
        //int max = vm_map_dim*vm_map_dim;
        //int i,j;
        //int s = sizeof(unsigned short);
        //struct vm_chunk* chunk;
        //for (i=0; i < max; i++) {
            //chunk = map.column[i].chunk;
            //for (j=0; j < vm_column_max; j++) {
                //fwrite(chunk->voxel, s, 512, f);
            //}
        //}
        //fclose(f);
    //} else {
        //printf("Failed to open %s for saving map\n", fn);
        //return 1;
    //}
    //return 0;
//}

char* t_zlib_dest;
FILE* t_zlib_dest_file;

//int t_compress_buffer_short_size = CHUNK_Z / sizeof(unsigned short);
#define t_compress_buffer_short_size (CHUNK_Z / sizeof(unsigned short))
unsigned short t_compress_buffer_short[t_compress_buffer_short_size];

unsigned char t_compress_buffer[CHUNK_Z];
int t_compress_buffer_index = 0;

void t_zerr(int ret);

void t_compress_buffer_reset() {
    //printf("reset buffer\n");
    // zero buffer
    int i;
    //for (i=0; i < CHUNK_Z; i++) {
    //for (i=0; i < t_compress_buffer_short_size; i++) {
        ////t_compress_buffer[i] = 0;
        //t_compress_buffer_short[i] = 0;
    //}
    for (i=0; i < buff_size; i++) {
        //t_compress_buffer[i] = 0;
        buff[i] = 0;
    }
    *buffer_index = 0;
}

/* Compress from file source to file dest until EOF on source.
   def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_STREAM_ERROR if an invalid compression
   level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
   version of the library linked do not match, or Z_ERRNO if there is
   an error reading or writing the files. */
//int defffff(FILE *source, FILE *dest, int level)
//{
    //int ret, flush;
    //unsigned have;
    //z_stream strm;
    //unsigned char in[CHUNK];
    //unsigned char out[CHUNK];

    /* allocate deflate state */
    //strm.zalloc = Z_NULL;
    //strm.zfree = Z_NULL;
    //strm.opaque = Z_NULL;
    //ret = deflateInit(&strm, level);
    //if (ret != Z_OK)
        //return ret;

    /* compress until end of file */
    //do {
        //strm.avail_in = fread(in, 1, CHUNK, source);
        //if (ferror(source)) {
            //(void)deflateEnd(&strm);
            //return Z_ERRNO;
        //}
        //flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        //strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
           //compression if all of source has been read in */
        //do {
            //strm.avail_out = CHUNK;
            //strm.next_out = out;
            //ret = deflate(&strm, flush);    /* no bad return value */
            //assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            //have = CHUNK - strm.avail_out;
            //if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                //(void)deflateEnd(&strm);
                //return Z_ERRNO;
            //}
        //} while (strm.avail_out == 0);
        //assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    //} while (flush != Z_FINISH);
    //assert(ret == Z_STREAM_END);        /* stream will be complete */

    /* clean up and return */
    //(void)deflateEnd(&strm);
    //return Z_OK;
//}


/* allocate deflate state */
z_stream t_strm;

int t_zlib_compress_init(int level) {
    printf("compress init\n");
    t_zlib_dest_file = fopen(t_zlib_dest, "wb");
    if (t_zlib_dest_file != NULL) {
        level = 6;
        t_strm.zalloc = Z_NULL;
        t_strm.zfree = Z_NULL;
        t_strm.opaque = Z_NULL;
        int ret = deflateInit(&t_strm, level);
        if (ret != Z_OK) {
            t_zerr(ret);
            return 1;
        }
        return 0;
    }
    printf("Could not open %s for writing\n", t_zlib_dest);
    return 1;
}

int t_zlib_compress_final() {
    printf("compress final\n");
    int ret = deflate(&t_strm, Z_FINISH);
    if (ret != Z_STREAM_END) { /* stream will be incomplete */
        printf("Z_FINISH deflate failed.\n");
        return 1;        
    }
    (void)deflateEnd(&t_strm);
    fclose(t_zlib_dest_file);
    return 0;
}

int _compress_calls = 0;
int t_zlib_compress()    // level -1 to 9. -1 is 6; 9 is most compression, 0 is none
{
    //return Z_OK;
    //printf("compress chunk\n");
    int ret;
    unsigned have;
    //unsigned char* in = (unsigned char*) t_compress_buffer;
    //unsigned char out[CHUNK_Z];
    unsigned char out[buff_size];

    //unsigned char* buff = (unsigned char*)t_compress_buffer_short;
    //int i=0;
    //int ibuff=0;
    //for (i=0; i < t_compress_buffer_short_size; i++) {
        //unsigned char* btmp = (unsigned char*)&t_compress_buffer_short[i];
        //buff[ibuff] = btmp[0];
        //ibuff++;
        //buff[ibuff] = btmp[1];
        //ibuff++;
    //}


    //int buff_size = t_compress_buffer_short_size * sizeof(unsigned short);
        //return ret;

    /* compress until end of file */
    //t_strm.avail_in = t_compress_buffer_index;
    //t_strm.avail_in = buff_size;
    t_strm.avail_in = *buffer_index;
    //t_strm.next_in = t_compress_buffer;
    t_strm.next_in = buff;
    //t_strm.next_in = &in;

    /* run deflate() on input until output buffer not full, finish
       compression if all of source has been read in */
    do {
        //t_strm.avail_out = CHUNK_Z;
        t_strm.avail_out = buff_size;
        t_strm.next_out = out;
        //printf("%d %d\n", t_strm.avail_in, t_strm.avail_out);
        ret = deflate(&t_strm, Z_NO_FLUSH);    /* no bad return value */
        //printf("%d\n", ret);
        _compress_calls++;
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
        //have = t_compress_buffer_index - t_strm.avail_out;
        //have = buff_size - t_strm.avail_out;
        have = *buffer_index - t_strm.avail_out;
        //printf("%d\n", t_strm.avail_out);
        //printf("%d\n", t_compress_buffer_index);
        //printf("%d\n", t_strm.avail_in);
        //printf("%d\n", have);
        //printf("^HAVE\n");
        if (fwrite(out, 1, have, t_zlib_dest_file) != have || ferror(t_zlib_dest_file)) {
            //printf("DEFLATE OR FILE FAIL\n");
            t_zlib_compress_final();
            return Z_ERRNO;
        }
    } while (t_strm.avail_out == 0);
    //assert(strm.avail_in == 0);     /* all input will be used */
//printf("%d\n", _compress_calls);
    return Z_OK;
}

int t_zlib_compress_map_chunk(int x, int y, int z, unsigned short *arr, int arr_size) {
    //arr_size *= sizeof(unsigned short);
    //int buff_arr_size = arr_size * sizeof(unsigned short);
    int ret = Z_OK;
    //if (CHUNK_Z - t_compress_buffer_index < buff_arr_size) {  // cant fit in buffer, flush
    //if (t_compress_buffer_short_size - t_compress_buffer_index < arr_size) {  // cant fit in buffer, flush
    if (buff_size - *buffer_index < (arr_size*sizeof(unsigned short))) {  // cant fit in buffer, flush
        //printf("%d %d %d %d\n", CHUNK_Z, t_compress_buffer_index, arr_size, CHUNK_Z - t_compress_buffer_index);
        ret = t_zlib_compress();
        t_compress_buffer_reset();
        //printf("RESET %d\n", t_compress_buffer_index);
    }

    if (ret != Z_OK) {
        printf("zlib compress not ok\n");
        t_zerr(ret);
        return 1;
    }

    // add to buffer
    int i=0;
    //int buffer_pos = t_compress_buffer_index;
    //printf("%d %d\n", buffer_pos, t_compress_buffer_index);
    //printf("buff size  %d\n", t_compress_buffer_index);
    //if (buffer_pos + buff_arr_size >= CHUNK_Z+1) {
        //printf("WARNING will segfault\n");
        //printf("%d %d %d %d\n", buffer_pos, buff_arr_size, buffer_pos + buff_arr_size, CHUNK_Z);
    //}
    //unsigned short k=0;
    //if (arr == NULL) {
        //printf("ARR NULL\n");
    //}
    //for (i=0; i < arr_size; i++) {
        ////if (t_compress_buffer == NULL) printf("t_compress_buffer NULL\n");
        ////t_compress_buffer[buffer_pos] = arr[i] & 0xff;
        ////buffer_pos++; 
        ////t_compress_buffer[buffer_pos] = (arr[i] >> 8) & 0xff;
        ////buffer_pos++;
        ////printf("buff pos  %d\n", buffer_pos);
        //k = arr[i];
        //t_compress_buffer_short[i] = k;
        ////t_compress_buffer_short[i] = arr[i];
    //}
    ////t_compress_buffer_index += buff_arr_size;//*sizeof(unsigned short);
    //t_compress_buffer_index += arr_size;//*sizeof(unsigned short);

    serialize_chunk(x,y,z, arr, buff, buffer_index);

    return 0;
}


/* report a zlib or i/o error */
void t_zerr(int ret)
{
    fputs("zpipe: ", stderr);
    switch (ret) {
    case Z_ERRNO:
        if (ferror(stdin))
            fputs("error reading stdin\n", stderr);
        if (ferror(stdout))
            fputs("error writing stdout\n", stderr);
        break;
    case Z_STREAM_ERROR:
        fputs("invalid compression level\n", stderr);
        break;
    case Z_DATA_ERROR:
        fputs("invalid or incomplete deflate data\n", stderr);
        break;
    case Z_MEM_ERROR:
        fputs("out of memory\n", stderr);
        break;
    case Z_VERSION_ERROR:
        fputs("zlib version mismatch!\n", stderr);
    }
}

int _processed_chunks = 0;
//int _save_to_disk(char* fn) {
    //t_zlib_dest = fn;
    //if (t_zlib_compress_init(Z_DEFAULT_COMPRESSION)) {
        //printf("Error: zlib compression init failed.\n");
        //t_compress_buffer_reset();
        //return 1;
    //}

    //int max = vm_map_dim*vm_map_dim;
    //int i,j;
    //short x,y,z;
    ////int s = sizeof(unsigned short);
    //struct vm_chunk* chunk;
    //for (i=0; i < max; i++) {
        ////chunk = (struct vm_chunk*) map.column[i].chunk;
        //chunk = map.column[i].chunk;
        //x = i % vm_map_dim;
        //y = i / vm_map_dim;
        //for (j=0; j < vm_column_max; j++) {
            //z = j;
            //printf("%d %d %d\n", i / vm_map_dim, i % vm_map_dim, j);

            //if (t_zlib_compress_map_chunk(x,y,z, &(chunk[i].voxel), 512)) {
                //printf("Error: Zlib map compression failed\n");
                //t_compress_buffer_reset();
                //return 1;
            //}
            //_processed_chunks++;
            ////printf("Processed chunks: %d\n", _processed_chunks);
        //}
    //}

    //if(t_zlib_compress_final()) {
        //printf("Error: zlib compression close failed.\n");
        //t_compress_buffer_reset();
        //return 1;
    //}
    //t_compress_buffer_reset();
    //return 0;
//}

int _save_to_disk(char *fn) {

    t_zlib_dest = fn;
    if (t_zlib_compress_init(Z_DEFAULT_COMPRESSION)) {
        printf("Error: zlib compression init failed.\n");
        t_compress_buffer_reset();
        return 1;
    }

    
    int t_max = vm_map_dim * vm_map_dim;
    int x,y,z;
    int i,j,k;
    unsigned short s;
    unsigned short* vox;
    struct vm_column col;
    struct vm_chunk* chunk;
    for (i=0; i < t_max;i++) {
        x = i % vm_map_dim;
        y = i / vm_map_dim;
        col = map.column[i];
        chunk = col.chunk;
        for (j=0; j < vm_column_max; j++) {
            z = j;
            printf("%d %d %d\n", x,y,z);
                vox = chunk->voxel;
            if (t_zlib_compress_map_chunk(x,y,z, vox, 512)) {
                printf("Error: Zlib map compression failed\n");
                t_compress_buffer_reset();
                return 1;
            }
        }
    }

    if(t_zlib_compress_final()) {
        printf("Error: zlib compression close failed.\n");
        t_compress_buffer_reset();
        return 1;
    }
    t_compress_buffer_reset();
    return 0;

}

int _load_from_disk(const char* fn) {
    return 0;
}


