#include "t_compress.h"

/* Map serializer using zlib
 * Fails under some conditions:
 *  Tiny map (only a few chunks)
 *  sizeof Buffer feeding zlib < 128kB
 *                              */

char* t_zlib_dest;
FILE* t_zlib_dest_file;

/* allocate deflate state */
z_stream t_strm_compress;


int t_zlib_compress()    // level -1 to 9. -1 is 6; 9 is most compression, 0 is none
{
    int ret;
    unsigned int have;
    unsigned char out[t_buff_size];

    t_strm_compress.avail_in = *t_buffer_index;
    t_strm_compress.next_in = t_buff;

    /* run deflate() on input until output buffer not full */
    do {

        t_strm_compress.avail_out = t_buff_size;
        t_strm_compress.next_out = out;

        ret = deflate(&t_strm_compress, Z_NO_FLUSH);    /* no bad return value */
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

        have = t_buff_size - t_strm_compress.avail_out;

        if (fwrite(out, 1, have, t_zlib_dest_file) != have || ferror(t_zlib_dest_file)) {
            return 1;
        }

    } while (t_strm_compress.avail_out == 0);

    assert(t_strm_compress.avail_in == 0);     /* all input will be used */
    return Z_OK;
}


int t_zlib_compress_init(char* fn, int level) {

    t_zlib_dest = fn;
    t_zlib_dest_file = fopen(t_zlib_dest, "wb");

    if (t_zlib_dest_file != NULL) {

        level = 6;
        t_strm_compress.zalloc = Z_NULL;
        t_strm_compress.zfree = Z_NULL;
        t_strm_compress.opaque = Z_NULL;

        int ret = deflateInit(&t_strm_compress, level);
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

    if (t_zlib_compress()) return 1;
    
    unsigned char out[t_buff_size];
    int ret;
    unsigned int have = 0;

    // write remaining deflate data
   do {

        t_strm_compress.avail_out = t_buff_size;
        t_strm_compress.next_out = out;

        ret = deflate(&t_strm_compress, Z_FINISH);    /* no bad return value */
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */

        have = t_buff_size - t_strm_compress.avail_out;
        if (fwrite(out, 1, have, t_zlib_dest_file) != have || ferror(t_zlib_dest_file)) {
            return 1;
        }

    } while (t_strm_compress.avail_out == 0);

    if (ret != Z_STREAM_END) { /* stream will be incomplete */
        t_zerr(ret);
        return 1;        
    }

    (void)deflateEnd(&t_strm_compress);

    fclose(t_zlib_dest_file);

    return 0;
}


int t_zlib_flush_buffer() {
    int ret = t_zlib_compress();

    if (ret != Z_OK) {
        t_zerr(ret);
        ret = 0;
    }

    t_buffer_reset();

    return ret;
}

// entry point for map compression
int t_zlib_compress_map_chunk(int x, int y, int z, unsigned short *arr, int arr_size) {

    int vox_index, vox_inc;

    vox_index = t_zlib_serialize_chunk(x,y,z, arr, arr_size, t_buff, t_buffer_index, t_buff_size);

    if (vox_index < 0) {  // x,y,z couldnt fit
        if (t_zlib_flush_buffer()) return 1;
        vox_index = t_zlib_serialize_chunk(x,y,z, arr, arr_size, t_buff, t_buffer_index, t_buff_size);
    }

    vox_inc = vox_index;  // incremental serialize return value
    while (vox_index < arr_size) { // x,y,z packed, not but all voxels
        
        if (t_zlib_flush_buffer()) return 1;

        arr = &arr[vox_inc];
        vox_inc = t_zlib_serialize_chunk_vox(arr, arr_size - vox_index, t_buff, t_buffer_index, t_buff_size);
        vox_index += vox_inc;
    }

    return 0;
}


int map_save_to_disk(char *fn) {
    printf("Map compression begin\n");

    if (t_zlib_compress_init(fn, Z_DEFAULT_COMPRESSION)) {
        printf("Error: zlib compression init failed.\n");
        t_buffer_reset();
        return 1;
    }
    
    int t_columns = vm_map_dim * vm_map_dim;
    int x,y;
    int i,j;

    struct vm_column* col;
    struct vm_chunk* chunk;

    for (i=0; i < t_columns; i++) {
        x = i % vm_map_dim;
        y = i / vm_map_dim;

        col = &map.column[i];
        for (j=0; j < vm_column_max; j++) {
            chunk = col->chunk[j];
            if (chunk == NULL || chunk->voxel == NULL) continue;
            if (t_zlib_compress_map_chunk(x,y,j, chunk->voxel, vm_chunk_voxel_size)) {
                t_buffer_reset();
                return 1;
            }
        }
    }

    if(t_zlib_compress_final()) {
        t_buffer_reset();
        return 1;
    }

    t_buffer_reset();

    printf("Map compression end\n");

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

