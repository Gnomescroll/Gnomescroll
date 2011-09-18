#include "t_serialize.h"
#include "brandon_serializer.c"


/* Map serializer using zlib
 * Fails under some conditions:
 *  Tiny map (only a few chunks)
 *  sizeof Buffer feeding zlib < 128kB
 *                              */

char* t_zlib_dest;
FILE* t_zlib_dest_file;

/* allocate deflate state */
z_stream t_strm;

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

void t_zlib_compress_buffer_reset() {
    int i;
    for (i=0; i < t_buff_size; i++) {
        buff[i] = 0;
    }
    *t_buffer_index = 0;
}

int t_zlib_compress_init(int level) {

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

    int ret = deflate(&t_strm, Z_FINISH);
    if (ret != Z_STREAM_END) { /* stream will be incomplete */
        printf("Z_FINISH deflate failed.\n");
        return 1;        
    }

    (void)deflateEnd(&t_strm);

    fclose(t_zlib_dest_file);

    return 0;
}

int t_zlib_compress()    // level -1 to 9. -1 is 6; 9 is most compression, 0 is none
{
    int ret;
    unsigned have;
    unsigned char out[t_buff_size];

    t_strm.avail_in = *t_buffer_index;
    t_strm.next_in = buff;

    /* run deflate() on input until output buffer not full */
    do {

        t_strm.avail_out = t_buff_size;
        t_strm.next_out = out;

        ret = deflate(&t_strm, Z_NO_FLUSH);    /* no bad return value */
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

        have = t_buff_size - t_strm.avail_out;
        if (fwrite(out, 1, have, t_zlib_dest_file) != have || ferror(t_zlib_dest_file)) {
            printf("DEFLATE OR FILE FAIL\n");
            printf("%d\n", have);
            t_zlib_compress_final();
            return Z_ERRNO;
        }

    } while (t_strm.avail_out == 0);

    assert(t_strm.avail_in == 0);     /* all input will be used */
    return Z_OK;
}

// entry point for map compression -- decides to compress buffer, and copies voxels to buffer
int t_zlib_compress_map_chunk(int x, int y, int z, unsigned short *arr, int arr_size) {
    int ret = Z_OK;
    if (t_buff_size - *t_buffer_index < (arr_size*sizeof(*arr))) {  // cant fit in buffer, flush
        ret = t_zlib_compress();
        t_zlib_compress_buffer_reset();
    }

    if (ret != Z_OK) {
        printf("zlib compress not ok\n");
        t_zerr(ret);
        return 1;
    }

    t_zlib_serialize_chunk(x,y,z, arr, buff, t_buffer_index);

    return 0;
}


int map_save_to_disk(char *fn) {

    printf("Map compression begin\n");

    t_zlib_dest = fn;
    if (t_zlib_compress_init(Z_DEFAULT_COMPRESSION)) {
        printf("Error: zlib compression init failed.\n");
        t_zlib_compress_buffer_reset();
        return 1;
    }
    
    int t_columns = vm_map_dim * vm_map_dim;
    int x,y,z;
    int i,j,k;

    unsigned short* vox;
    struct vm_column col;
    struct vm_chunk* chunk;

    for (i=0; i < t_columns;i++) {
        x = i % vm_map_dim;
        y = i / vm_map_dim;
        col = map.column[i];
        chunk = col.chunk;
        for (j=0; j < vm_column_max; j++) {
            z = j;
            vox = chunk->voxel;
            if (t_zlib_compress_map_chunk(x,y,z, vox, 512)) {
                printf("Error: Zlib map compression failed\n");
                t_zlib_compress_buffer_reset();
                return 1;
            }
        }
    }

    if(t_zlib_compress_final()) {
        printf("Error: zlib compression close failed.\n");
        t_zlib_compress_buffer_reset();
        return 1;
    }
    t_zlib_compress_buffer_reset();

    printf("Map compression end\n");

    return 0;
}

int map_load_from_disk(const char* fn) {
    return 0;
}


////
// http://zlib.net/zpipe.c
////

/* Compress from file source to file dest until EOF on source.
   def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_STREAM_ERROR if an invalid compression
   level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
   version of the library linked do not match, or Z_ERRNO if there is
   an error reading or writing the files. */
//int compress_file(FILE *source, FILE *dest, int level)
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

