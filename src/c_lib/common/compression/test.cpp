


typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint;

#define my_max(a,b) (((a) > (b)) ? (a) : (b))
#define my_min(a,b) (((a) < (b)) ? (a) : (b))

#define BUF_SIZE (1024 * 1024)
static uint8 s_inbuf[BUF_SIZE];
static uint8 s_outbuf[BUF_SIZE];

  uint infile_size;
  int level = Z_BEST_COMPRESSION;
  z_stream stream;
  int n = 1;

  // Init the z_stream
  memset(&stream, 0, sizeof(stream));
  stream.next_in = s_inbuf;
  stream.avail_in = 0;
  stream.next_out = s_outbuf;
  stream.avail_out = BUF_SIZE;


  // Compression.
  uint infile_remaining = infile_size;

  if (deflateInit(&stream, level) != Z_OK)
  {
    printf("deflateInit() failed!\n");
    return EXIT_FAILURE;
  }


  stream.next_in = s_inbuf;
  stream.avail_in = n;
  
  int status;
  stream.next_in = s_inbuf;
  stream.avail_in = n;

  //status = deflate(&stream, Z_FINISH); //

  status = deflate(&stream, Z_SYNC_FLUSH); //


  if ( status == Z_STREAM_END )
  {
    // Output buffer is full, or compression is done
    uint n = BUF_SIZE - stream.avail_out;  //number of bytes written
    stream.next_out = s_outbuf;   //reset output buffer
    stream.avail_out = BUF_SIZE;  //reset output buffer
  }

  else if (status != Z_OK)
  {
    printf("deflate() failed with status %i!\n", status);
    return EXIT_FAILURE;
  }


  /*
    Stuff
  */

  #define BUF_SIZE (1024 * 1024)
  static uint8 s_inbuf[BUF_SIZE];
  static uint8 s_outbuf[BUF_SIZE];

  uint infile_size;
  int level = Z_BEST_COMPRESSION;
  z_stream stream;
  int n = 1;

  // Init the z_stream
  memset(&stream, 0, sizeof(stream));
  stream.next_in = s_inbuf;
  stream.avail_in = 0;
  stream.next_out = s_outbuf;
  stream.avail_out = BUF_SIZE;


  if (inflateInit(&stream))
  {
    printf("inflateInit() failed!\n");
    return EXIT_FAILURE;
  }

  int status;

  stream.next_in = s_inbuf;
  stream.avail_in = n;

  status = inflate(&stream, Z_SYNC_FLUSH);

  if (status == Z_STREAM_END )
  {
    // Output buffer is full, or decompression is done, so write buffer to output file.
    uint n = BUF_SIZE - stream.avail_out;

    stream.next_out = s_outbuf;
    stream.avail_out = BUF_SIZE;
  }
  else if (status != Z_OK)
  {
    printf("inflate() failed with status %i!\n", status);
    return EXIT_FAILURE;
  }
