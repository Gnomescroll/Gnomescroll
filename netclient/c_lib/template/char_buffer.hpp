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

    OBJECT_POOL_OBJECT_MACRO
    
    Char_buffer() {}
};

class Char_buffer_pool: public Object_pool<Char_buffer, 128>  //64, 2048 byte buffers
{
    public:
    const char* name() { static const char x[] = "Generic Char_buffer_pool"; return x; } 
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
        cb_read->next = NULL;
        cb_write = cb_read;

        size=0;
        read_index = 0;
        write_index = 0;
    }

    //void * memcpy ( void * destination, const void * source, size_t num );
    void write(char* buff, int n)
    {
        //int _n = n; //debug
        int fb = CHAR_BUFFER_SIZE - write_index;

        //printf("Fifo_char_buffer: write \n");

        while(fb < n)
        {
            //printf("Fifo_char_buffer: write %i of %i bytes and new buffer\n", fb, n);  //debug

            memcpy(cb_write->buffer+write_index, buff, fb);
            buff += fb;
            n -= fb;
            size += fb;

            cb_write->next = char_buffer_pool.acquire();
            cb_write = cb_write->next;
            write_index = 0;

            fb = CHAR_BUFFER_SIZE - write_index;
        }
        //printf("Fifo_char_buffer: write %i bytes\n", n); //debug

        memcpy(cb_write->buffer+write_index, buff, n);
        size += n;
        write_index += n;

        //printf("Fifo_char_buffer: write end, size= %i \n", size);
    }

    void read(char* buff, int n)
    {
        //printf("Fifo_char_buffer: read \n" );
        //DEBUG
        if(n > size)
        {
          printf("Fifo_char_buffer: FATAL ERROR, n greater than size, n=%i, size=%i \n", n, size);   
          printf("segfault= %i \n", ((int*)NULL));
        } //debug
        //if(n == size) printf("Fifo_char_buffer: full read of buffer, n=%i, size=%i \n", n, size); //debug

        int rb = CHAR_BUFFER_SIZE - read_index;

        while(rb < n)
        {
            memcpy(buff, cb_read->buffer+read_index, rb);
            buff += rb;
            n -= rb;
            size -= rb;

            Char_buffer* tmp = cb_read;
            cb_read = cb_read->next;
            char_buffer_pool.retire(tmp);
            
            read_index = 0;

            rb = CHAR_BUFFER_SIZE - read_index;
        }

        memcpy(buff, cb_read->buffer+read_index, n);
        size -= n;
        read_index += n;

        //printf("Fifo_char_buffer: read finished \n");
    }
};

/*
    UNUSED BELOW LINE
*/


/*
reference counted, variable length string buffer

used for
-getting string/small object allocator
-reference counted string
*/

/*
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

class char_buffer_instance {
    private:
    public:

};

class Char_buffer2;

class char_buffer_inheritance_template {
    public:
    char_buffer_inheritance_template* next;
    char* buff;
    int length;
    Char_buffer_ref* cbf;

    void retire() {
        cbf->reference_count--;
        if(cbf->reference_count == 0) {
            char_buffer_ref_pool.retire(ctf);
        }
        //return this object to object pool
    }

    //static char_buffer_inheritance_template* acquire() { return object from pool }

};


//reference counted version

class char_buffer_ref_inheritance_template {
    public:
    int reference_count;
      
};

class Char_buffer2 {
    private:
    public:
    Char_buffer_ref* current;
    int remaining;
    char* offset;

    
        //Object can acquire a char buffer as long it decrements cbf reference count when its done
    
    void acquire(int length, char** b, Char_buffer_ref** cbf) {
        if(remaining < length) 
        {
            current = char_buffer_ref_pool.acquire();
            current.reference_count = 0;
            remaining = CHAR_BUFFER_REF_SIZE;
            offset = current->buffer;
        }
        remaining -= length;
        offset += length;
        current->reference_count++;
        
        *b = offset;
        *cbf = current;
        remaining -= length;
        offset += length;
        current->reference_count++; 
    }

    Char_buffer2 
    {
        current = NULL;
        offset = NULL;
        remaining = -1;
    }
};


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
*/
