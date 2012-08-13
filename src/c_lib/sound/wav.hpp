#pragma once

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <common/compat_al.h>

namespace Sound
{

const int MAX_WAV_BUFFERS = 64;
typedef struct WavFile
{
    short format;
    short channels;
    int sample_rate;
    int byte_rate;
    short bits_per_sample;
    float duration;
    int size;
    bool in_use;
} WavData;

ALenum get_openal_wav_format(WavData* data);

void print_wav_data(WavData* data);

// returns buffer id.  make sure to free *buffer after binding to an ALbuffer
int load_wav_file(char *fn, unsigned char** buffer);

void release_wav_data(int buffer_id);
WavData* get_loaded_wav_data(int buffer_id);

void init_wav_buffers();
void teardown_wav_buffers();


}
