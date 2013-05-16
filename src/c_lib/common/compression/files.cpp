#include "files.hpp"

#include <common/macros.hpp>

/* WARNING: Make sure all of these are thread safe, as they are used by the
 * map compressor. This means no asserts.
 *
 * miniz itself is probably not threadsafe, so dont use it in the server unless
 * you add locks
 */

static const size_t OUT_BUF_SIZE = 1024 * 1024 * 16;
static const size_t IN_BUF_SIZE = 1024 * 512;
static uint8_t outbuf[OUT_BUF_SIZE];
static uint8_t inbuf[IN_BUF_SIZE];


int compress_buffer_to_file(const char* filename, const char* inbuf, const size_t buffer_size)
{   // modified version of miniz.c's example5.c
    static const int COMPRESSION_FAIL = 1;
    int level = Z_BEST_COMPRESSION;
    void* next_out = outbuf;
    const void* next_in = (const void*)inbuf;
    size_t avail_in = buffer_size;
    size_t avail_out = OUT_BUF_SIZE;
    int error = 0;

    size_t total_in = 0, total_out = 0;

    size_t comp_flags = tdefl_create_comp_flags_from_zip_params(
        level, MZ_DEFAULT_WINDOW_BITS, MZ_DEFAULT_STRATEGY) | TDEFL_WRITE_ZLIB_HEADER;

    // Initialize the low-level compressor.
    tdefl_compressor deflator;
    tdefl_status status = tdefl_init(&deflator, NULL, NULL, comp_flags);
    if (status != TDEFL_STATUS_OKAY)
    {
        printf("tdefl_init() failed!\n");
        return COMPRESSION_FAIL;
    }

    FILE* outfile = fopen(filename, "wb");
    if (outfile == NULL)
    {
        printf("Failed opening output file!\n");
        return COMPRESSION_FAIL;
    }

    // Compression.
    for (;;)
    {
        size_t in_bytes = avail_in;
        size_t out_bytes = avail_out;
        // Compress as much of the input as possible (or all of it) to the output inbuf.
        status = tdefl_compress(&deflator, next_in, &in_bytes, next_out, &out_bytes,
                                in_bytes ? TDEFL_NO_FLUSH : TDEFL_FINISH);

        next_in = (const char*)next_in + in_bytes;
        avail_in -= in_bytes;
        total_in += in_bytes;

        next_out = (char*)next_out + out_bytes;
        avail_out -= out_bytes;
        total_out += out_bytes;

        if ((status != TDEFL_STATUS_OKAY) || (!avail_out))
        {
            // Output inbuf is full, or compression is done or failed, so write inbuf to output file.
            size_t n = OUT_BUF_SIZE - (size_t)avail_out;
            if (fwrite(outbuf, 1, n, outfile) != n)
            {
                printf("Failed writing to output file!\n");
                error = COMPRESSION_FAIL;
                break;
            }
            next_out = outbuf;
            avail_out = OUT_BUF_SIZE;
        }

        if (status == TDEFL_STATUS_DONE)
        {
            // Compression completed successfully.
            break;
        }
        else if (status != TDEFL_STATUS_OKAY)
        {
            // Compression somehow failed.
            printf("tdefl_compress() failed with status %d!\n", status);
            error = COMPRESSION_FAIL;
            break;
        }
    }

    if (outfile != NULL)
    {
        int closed = fclose(outfile);
        if (closed)
            printf("failed to close file \"%s\"\n", filename);
    }

    return error;
}


char* decompress_file_to_buffer(const char* filename, size_t& len)
{
    len = 0;
    const size_t OUTPUT_START_SIZE = 1024 * 1024;   // 1MB
    char* output = (char*)malloc(OUTPUT_START_SIZE);
    size_t output_size = OUTPUT_START_SIZE;
    size_t output_len = 0;
    const int DECOMPRESS_FAIL = 1;
    int error = 0;
    size_t avail_in = 0;
    size_t avail_out = OUT_BUF_SIZE;
    const void* next_in = inbuf;
    void* next_out = outbuf;
    size_t total_out = 0;
    size_t total_in = 0;

    // Open input file.
    FILE* infile = fopen(filename, "rb");
    if (infile == NULL)
    {
        printf("Failed opening input file \"%s\"\n", filename);
        return NULL;
    }
    // Determine input file's size.
    fseek(infile, 0, SEEK_END);
    size_t infile_size = ftell(infile);
    fseek(infile, 0, SEEK_SET);

    // Decompression.
    size_t infile_remaining = infile_size;

    tinfl_decompressor inflator;
    tinfl_init(&inflator);

    for (;;)
    {
        size_t in_bytes, out_bytes;
        tinfl_status status;
        if (!avail_in)
        {
            // Input buffer is empty, so read more bytes from input file.
            size_t n = GS_MIN(IN_BUF_SIZE, infile_remaining);

            if (fread(inbuf, 1, n, infile) != n)
            {
                printf("Failed reading from input file \"%s\"\n", filename);
                return NULL;
            }

            next_in = inbuf;
            avail_in = n;

            infile_remaining -= n;
        }

        in_bytes = avail_in;
        out_bytes = avail_out;
        status = tinfl_decompress(&inflator, (const mz_uint8*)next_in, &in_bytes,
                                  outbuf, (mz_uint8*)next_out, &out_bytes,
                                  (infile_remaining ? TINFL_FLAG_HAS_MORE_INPUT : 0) | TINFL_FLAG_PARSE_ZLIB_HEADER);

        avail_in -= in_bytes;
        next_in = (const mz_uint8*)next_in + in_bytes;
        total_in += in_bytes;

        avail_out -= out_bytes;
        next_out = (mz_uint8*)next_out + out_bytes;
        total_out += out_bytes;

        if ((status <= TINFL_STATUS_DONE) || (!avail_out))
        {
            // Output buffer is full, or decompression is done, so write buffer to output file.
            size_t n = OUT_BUF_SIZE - (size_t)avail_out;
            // TODO -- convert this to a memcpy + resize
            size_t new_size = output_size;
            while ((output_len + n) >= new_size)
                new_size *= 2;
            if (new_size != output_size)
            {
                char* _output = (char*)realloc(output, new_size);
                if (_output == NULL)
                {
                    printf("Failed to realloc output buffer\n");
                    error = DECOMPRESS_FAIL;
                    break;
                }
                output = _output;
                output_size = new_size;
            }
            memcpy(&output[output_len], outbuf, n);
            output_len += n;
            next_out = outbuf;
            avail_out = OUT_BUF_SIZE;
        }

        // If status is <= TINFL_STATUS_DONE then either decompression is done or something went wrong.
        if (status <= TINFL_STATUS_DONE)
        {
            if (status == TINFL_STATUS_DONE)
            {
                // Decompression completed successfully.
                break;
            }
            else
            {
                // Decompression failed.
                printf("tinfl_decompress() failed with status %i!\n", status);
                error = DECOMPRESS_FAIL;
                break;
            }
        }
    }

    int closed = fclose(infile);
    if (closed)
        printf("Failed to close file \"%s\"\n", filename);

    if (error)
    {
        free(output);
        len = 0;
        return NULL;
    }

    len = output_len;
    if (!output_len)
    {
        free(output);
        return NULL;
    }
    else if (output_len != output_size)
    {
        char* _output = (char*)realloc(output, output_len * sizeof(*output));
        if (_output == NULL)
            printf("Failed to decrease output buffer size\n");
        else
            output = _output;
    }
    return output;
}
