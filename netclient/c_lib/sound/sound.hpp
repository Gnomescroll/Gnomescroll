#include <fmod.h>
#include <fmod_errors.h>
#include <c_lib/sound/wincompat.h>
#include <stdio.h>

#define USE_FMOD 0
#define USE_OPENAL 1

namespace Sound {

void init();
void close();

void set_volume(float vol);

void load_sound(char* file);

int play_2d_sound(char* file);
int play_3d_sound(char* file, float x, float y, float z, float vx, float vy, float vz);

void update_listener(float x, float y, float z, float vx, float vy, float vz, float fx, float fy, float fz, float ux, float uy, float uz);
void update();

int test();

}

//short audioFormat = file_read_int16_le(xbuffer, file);
//short channels = file_read_int16_le(xbuffer, file);
//int sampleRate = file_read_int32_le(xbuffer, file);
//int byteRate = file_read_int32_le(xbuffer, file);
//file_read_int16_le(xbuffer, file);
//short bitsPerSample = file_read_int16_le(xbuffer, file);

//if (audioFormat != 16) {
    //short extraParams = file_read_int16_le(xbuffer, file);
    //file_ignore_bytes(file, extraParams);
//}

//if (fread(xbuffer, sizeof(char), 4, file) != 4 || strcmp(xbuffer, "data") != 0)
    //throw "Invalid WAV file";

//int dataChunkSize = file_read_int32_le(xbuffer, file);
//unsigned char* bufferData = file_allocate_and_read_bytes(file, (size_t) dataChunkSize);

//float duration = float(dataChunkSize) / byteRate;
//alBufferData(buffer, GetFormatFromInfo(channels, bitsPerSample), bufferData, dataChunkSize, sampleRate);
//free(bufferData);

//typedef struct {
  //ID             chunkID;
  //long           chunkSize;

  //short          wFormatTag;
  //unsigned short wChannels;
  //unsigned long  dwSamplesPerSec;
  //unsigned long  dwAvgBytesPerSec;
  //unsigned short wBlockAlign;
  //unsigned short wBitsPerSample;

///* Note: there may be additional fields here, depending upon wFormatTag. */

//} FormatChunk;
