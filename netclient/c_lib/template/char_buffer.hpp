#pragma once

#include <stdio.h>

#include "object_pool.hpp"

static const int CHAR_BUFFER_SIZE=2048;

//non-reference counted char buffer
class Char_buffer {
    private:
    public:
    //int reference_count;
    Char_buffer* next;
    char buffer[CHAR_BUFFER_SIZE];

    Char_buffer() {}
};

class Char_buffer_pool: public Object_pool<Char_buffer, 128>  //64, 2048 byte buffers
{
    public:
    char* name() { static char x[] = "Generic Char_buffer_pool"; return x; } 
};

Char_buffer_pool char_buffer_pool;

/*
Fifo_char_buffer
*/

class Fifo_char_buffer
{
    public:

    int size;

    Char_buffer* cb_read;
    int read_index;
    
    Char_buffer* cb_write;
    int write_index;

    Fifo_char_buffer()
    {
        cb_read = char_buffer_pool.acquire();
        cb_read.next = NULL;
        cb_write = cb_read;

        size=0;
        read_index = 0;
        write_index = 0;
    }

    //void * memcpy ( void * destination, const void * source, size_t num );
    void write(char* buff, int n)
    {
        int fb = CHAR_BUFFER_SIZE - write_index;

        while(fb < n)
        {
            printf("write %i bytes and new buffer\n", fb);  //debug

            memcpy(cb_write.buffer+write_index, buff, fb);
            buff += fb;
            n -= fb;
            size += fb;

            cb_write.next = char_buffer_pool.acquire();
            cb_write = cb_write.next;
            write_index = 0;

            fb = CHAR_BUFFER_SIZE - write_index;
        }
        printf("write %i bytes and new buffer\n", n); //debug

        memcpy(cb_write_buffer+write_index, buff, n);
        size += n;
        write_index += n;
    }

    void read(char* buff, int n)
    {

        if(n > size) printf("n greater than size\n"); //debug
        if(n==size) printf("full read of buffer\n"); //debug

        int rb = CHAR_BUFFER_SIZE - read_index;

        while(rb < n)
        {
            memcpy(buff, cb_read.buffer+read_index, rb);
            buff += rb;
            n -= rb;
            size -= rb;

            char_buffer_pool.retire(cb_read);
            cb_read = cb_read.next;
            read_index = 0;

            rb = CHAR_BUFFER_SIZE - read_index;
        }

        memcpy(buff, cb_read.buff+read_index, n);
        size -= n
        read_index += n;
    }
};

/*
reference counted, variable length string buffer
*/

static const int CHAR_BUFFER_REF_SIZE=2048;

class Char_buffer_ref {
    private:
    public:
    int reference_count;
    Char_buffer* next;
    char buffer[CHAR_BUFFER_REF_SIZE];

    Char_buffer() {}
};

class Char_buffer: public Object_pool<Char_buffer_ref, 32>  //64, 2048 byte buffers
{
    public:
    char* name() { static char x[] = "Generic Char_buffer_ref_pool"; return x; } 
};

Char_buffer_ref_pool char_buffer_ref_pool;


inline void get_char_buffer(int length, char** b, Net_message_buffer** nmb) 
{
    static Net_message_buffer* current = NULL;
    static int remaining = 0;
    static char* offset = NULL;
    if(remaining < length) 
    {
        current = net_message_buffer_pool.acquire();
        remaining = NET_MESSAGE_BUFFER_SIZE;
        offset = current->buffer;
    }
    *b = offset;
    *nmb = current;
    remaining -= length;
    offset += length;
    current->reference_count++;
}