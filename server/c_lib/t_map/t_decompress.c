#include "t_decompress.h"

char* t_zlib_src;
FILE* t_zlib_src_file;

z_stream t_strm_decompress;

int t_zlib_decompress_init(char* fn) {

    t_zlib_src = fn;
    t_zlib_src_file = fopen(t_zlib_src, "rb");
    if (t_zlib_src_file == NULL) {
        printf("Failed to open map file %s\n", fn);
        return 1;
    }

    int ret;
    /* allocate inflate state */
    t_strm_decompress.zalloc = Z_NULL;
    t_strm_decompress.zfree = Z_NULL;
    t_strm_decompress.opaque = Z_NULL;
    t_strm_decompress.avail_in = 0;
    t_strm_decompress.next_in = Z_NULL;
    ret = inflateInit(&t_strm_decompress);
    if (ret != Z_OK) {
        printf("Map decompress init fail.\n");
        return ret;
    }
    return 0;
}

int t_zlib_decompress_final() {

    (void)inflateEnd(&t_strm_decompress);
    fclose(t_zlib_src_file);
    t_buffer_reset();

    return 0;
}

int t_zlib_decompress_update_buffer(unsigned char* out, int n) {

    if (t_zlib_unserialize_chunk(out, n)) {
        return 1;
    }
    
    return 0;
}

int t_zlib_decompress() {

    int ret = Z_OK;
    unsigned int have;
    unsigned char in[t_buff_size];

    /* decompress until deflate stream ends or end of file */
    do {
        t_strm_decompress.avail_in = fread(in, 1, t_buff_size, t_zlib_src_file);

        if (ferror(t_zlib_src_file)) {
            printf("Error reading map file: %s!\n", t_zlib_src);
            return Z_ERRNO;
        }
        
        if (t_strm_decompress.avail_in == 0) {
            break;
        }

        t_strm_decompress.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {

            t_strm_decompress.avail_out = t_buff_size;
            t_strm_decompress.next_out = t_buff;

            ret = inflate(&t_strm_decompress, Z_NO_FLUSH);

            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                return ret;
            }

            have = t_buff_size - t_strm_decompress.avail_out;

            if (t_zlib_decompress_update_buffer(t_buff, have)) {
                return Z_ERRNO;
            }

        } while (t_strm_decompress.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}


int _map_load_from_disk(char* fn) {
    printf("Loading map %s\n", fn);
    
    const char* errmsg = "Map decompress failed.\n";
    int ret;

    ret = t_zlib_decompress_init(fn);
    if (ret) {
        printf("%s", errmsg);
        return 1;
    }
    
    ret = t_zlib_decompress();
    if (ret != Z_OK) {
        t_zerr(ret);
        printf("%s", errmsg);
    }

    ret = t_zlib_decompress_final();
    if (ret) {
        printf("%s", errmsg);
        return 1;
    }

    printf("Map decompression end.\n");

    return 0;
}

int map_load_from_disk(char* fn) {
    t_buffer_init();
    int ret = _map_load_from_disk(fn);
    t_buffer_destroy();
    return ret;
}


////
// http://zlib.net/zpipe.c
////

/* Decompress from file source to file dest until stream ends or EOF.
   inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_DATA_ERROR if the deflate data is
   invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
   the version of the library linked do not match, or Z_ERRNO if there
   is an error reading or writing the files. */
//int inf(FILE *source, FILE *dest)
//{
    //int ret;
    //unsigned have;
    //z_stream strm;
    //unsigned char in[CHUNK];
    //unsigned char out[CHUNK];

    ///* allocate inflate state */
    //strm.zalloc = Z_NULL;
    //strm.zfree = Z_NULL;
    //strm.opaque = Z_NULL;
    //strm.avail_in = 0;
    //strm.next_in = Z_NULL;
    //ret = inflateInit(&strm);
    //if (ret != Z_OK)
        //return ret;

    ///* decompress until deflate stream ends or end of file */
    //do {
        //strm.avail_in = fread(in, 1, CHUNK, source);
        //if (ferror(source)) {
            //(void)inflateEnd(&strm);
            //return Z_ERRNO;
        //}
        //if (strm.avail_in == 0)
            //break;
        //strm.next_in = in;

        ///* run inflate() on input until output buffer not full */
        //do {
            //strm.avail_out = CHUNK;
            //strm.next_out = out;
            //ret = inflate(&strm, Z_NO_FLUSH);
            //assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            //switch (ret) {
            //case Z_NEED_DICT:
                //ret = Z_DATA_ERROR;     /* and fall through */
            //case Z_DATA_ERROR:
            //case Z_MEM_ERROR:
                //(void)inflateEnd(&strm);
                //return ret;
            //}
            //have = CHUNK - strm.avail_out;
            //if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                //(void)inflateEnd(&strm);
                //return Z_ERRNO;
            //}
        //} while (strm.avail_out == 0);

        ///* done when inflate() says it's done */
    //} while (ret != Z_STREAM_END);

    ///* clean up and return */
    //(void)inflateEnd(&strm);
    //return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
//}
