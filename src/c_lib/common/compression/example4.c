// example4.c - Uses tinfl.c to decompress a zlib stream in memory to an output file
// Public domain, May 15 2011, Rich Geldreich, richgel99@gmail.com. See "unlicense" statement at the end of tinfl.c.
#include "tinfl.c"
#include <stdio.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

#define my_max(a,b) (((a) > (b)) ? (a) : (b))
#define my_min(a,b) (((a) < (b)) ? (a) : (b))

static int tinfl_put_buf_func(const void* pBuf, int len, void *pUser)
{
  return len == (int)fwrite(pBuf, 1, len, (FILE*)pUser);
}

int main(int argc, char *argv[])
{
  int status;
  FILE *pInfile, *pOutfile;
  uint infile_size, outfile_size;
  size_t in_buf_size;
  uint8 *pCmp_data;
          
  if (argc != 3)
  {
    printf("Usage: example4 infile outfile\n");
    printf("Decompresses zlib stream in file infile to file outfile.\n");
    printf("Input file must be able to fit entirely in memory.\n");
    return EXIT_FAILURE;
  }

  // Open input file.
  pInfile = fopen(argv[1], "rb");
  if (!pInfile)
  {
    printf("Failed opening input file!\n");
    return EXIT_FAILURE;
  }
  
  // Determine input file's size.
  fseek(pInfile, 0, SEEK_END);
  infile_size = ftell(pInfile);
  fseek(pInfile, 0, SEEK_SET);

  pCmp_data = (uint8 *)malloc(infile_size);
  if (!pCmp_data)
  {
    fclose(pInfile);
    printf("Out of memory!\n");
    return EXIT_FAILURE;
  }
  if (fread(pCmp_data, 1, infile_size, pInfile) != infile_size)
  {
    fclose(pInfile);
    free(pCmp_data);
    printf("Failed reading input file!\n");
    return EXIT_FAILURE;
  }

  // Open output file.
  pOutfile = fopen(argv[2], "wb");
  if (!pOutfile)
  {
    fclose(pInfile);
    free(pCmp_data);
    printf("Failed opening output file!\n");
    return EXIT_FAILURE;
  }

  printf("Input file size: %u\n", infile_size);
    
  in_buf_size = infile_size;
  status = tinfl_decompress_mem_to_callback(pCmp_data, &in_buf_size, tinfl_put_buf_func, pOutfile, TINFL_FLAG_PARSE_ZLIB_HEADER);
  free(pCmp_data);
  if (!status)
  {
    fclose(pInfile);
    fclose(pOutfile);
    printf("tinfl_decompress_mem_to_callback() failed with status %i!\n", status);
    return EXIT_FAILURE;
  }

  outfile_size = ftell(pOutfile);

  fclose(pInfile);
  if (EOF == fclose(pOutfile))
  {
    printf("Failed writing to output file!\n");
    return EXIT_FAILURE;
  }

  printf("Total input bytes: %u\n", (uint)in_buf_size);
  printf("Total output bytes: %u\n", outfile_size);
  printf("Success.\n");
  return EXIT_SUCCESS;
}
