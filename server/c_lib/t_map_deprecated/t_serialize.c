#include "t_serialize.h"

#include "t_map.hpp"

#include <zlib.h>

#include <assert.h>
#include <stdio.h>

/* report a zlib or i/o error */
void t_zerr(int ret)
{
    printf("zpipe: errno[%d] :: ", ret);
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
        break;
    case Z_BUF_ERROR:
        fputs("zlib buffer error\n", stderr);
    }
}

#include "t_buffer.c"
#include "t_compress.c"
#include "t_decompress.c"
