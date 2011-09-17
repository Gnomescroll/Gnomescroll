#include "t_serialize.h"

// 256kB buffer for compression stream
#define CHUNK_Z (1024*256)

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

unsigned short t_compress_buffer[CHUNK_Z / sizeof(unsigned short)];
int t_compress_buffer_size = 0;

void t_zerr(int ret);

void t_compress_buffer_reset() {
    // zero buffer
    int i;
    for (i=0; i < CHUNK_Z / sizeof(unsigned short); i++) {
        t_compress_buffer[i] = 0;
    }
    t_compress_buffer_size = 0;
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
    printf("copress finl\n");
    int ret = deflate(&t_strm, Z_FINISH);
    if (ret != Z_STREAM_END) return 1;        /* stream will be complete */
    (void)deflateEnd(&t_strm);
    fclose(t_zlib_dest_file);
    return 0;
}

int t_zlib_compress()    // level -1 to 9. -1 is 6; 9 is most compression, 0 is none
{
    int ret;
    unsigned have;
    unsigned char* in = (unsigned char*) t_compress_buffer;
    unsigned char out[CHUNK_Z / sizeof(unsigned short)];

        //return ret;

    /* compress until end of file */
    t_strm.avail_in = t_compress_buffer_size;
    t_strm.next_in = &in;

    /* run deflate() on input until output buffer not full, finish
       compression if all of source has been read in */
    do {
        t_strm.avail_out = t_compress_buffer_size;
        t_strm.next_out = out;
        printf("hey\n");
        ret = deflate(&t_strm, Z_NO_FLUSH);    /* no bad return value */
        //printf("%d\n", ret);
        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
        have = t_compress_buffer_size - t_strm.avail_out;
        //printf("%d\n", t_compress_buffer);
        //printf("%d\n", t_strm.avail_out);
        //printf("%d\n", have);
        //printf("^HAVE\n");
        if (fwrite(out, 1, have, t_zlib_dest_file) != have || ferror(t_zlib_dest_file)) {
            t_zlib_compress_final();
            return Z_ERRNO;
        }
    } while (t_strm.avail_out == 0);
    //assert(strm.avail_in == 0);     /* all input will be used */

    return Z_OK;
}

int t_zlib_compress_map_chunk(unsigned short *arr, int arr_size) {
    int i=0;
    int buffer_pos = 0;
    int ret = 0;
    if (CHUNK_Z - t_compress_buffer_size < arr_size) {  // cant fit in buffer, flush
        ret = t_zlib_compress();
        if (ret != Z_OK) {
            printf("zlib compress not ok\n");
            t_zerr(ret);
            t_compress_buffer_reset();
            return 1;
        }
        t_compress_buffer_reset();
    }

    // add to buffer
    for (i=0; i < arr_size; i++) {
        buffer_pos = t_compress_buffer_size + i; 
        t_compress_buffer[buffer_pos] = arr[i];
    }
    t_compress_buffer_size += arr_size;

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

int _save_to_disk(char* fn) {
    t_zlib_dest = fn;
    if (t_zlib_compress_init(Z_DEFAULT_COMPRESSION)) {
        printf("Error: zlib compression init failed.\n");
        t_compress_buffer_reset();
        return 1;
    }

    int max = vm_map_dim*vm_map_dim;
    int i,j;
    //int s = sizeof(unsigned short);
    struct vm_chunk* chunk;
    for (i=0; i < max; i++) {
        chunk = (struct vm_chunk*) map.column[i].chunk;
        for (j=0; j < vm_column_max; j++) {
            if (t_zlib_compress_map_chunk(chunk[i].voxel, 512)) {
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


