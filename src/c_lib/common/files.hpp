#pragma once

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <common/macros.hpp>

int GS_MKDIR(const char* dir, int permissions);

char* read_file_to_buffer(const char* file_name, int* size);
char* fast_read_file_to_buffer(const char* file_name, int* size);

off_t fsize(const char *filename);

int count_lines(char* buffer);
int count_lines(char* buffer, int* longest);

void free_read_lines(char** readlines, int lines);
char** read_lines(char* buffer);

void create_path(const char* path);
void create_path_to_file(const char* fn);

bool file_exists(const char* filename);

char* strip_whitespace(const char* str)
{
    int i=0;
    while (isspace(str[i])) i++;
    int start = i;
    i = strlen(str)-1;
    while (isspace(str[i])) i--;
    int end = i;
    int len = end-start+1;
    char* nstr = (char*)malloc((len+1) * sizeof(char));
    strncpy(nstr, &str[start], len);
    nstr[len] = '\0';
    return nstr;
}

unsigned char* hexstr_to_char(const char* hexstr)
{
    unsigned int len = (unsigned int)strlen(hexstr);
    GS_ASSERT(len%2 == 0);
    unsigned int final_len = len / 2;
    unsigned char* chrs = (unsigned char*)malloc((final_len+1) * sizeof(unsigned char));
    unsigned int i=0,j=0;
    for (; i<len; i+=2, j++)
        chrs[j] = (hexstr[i] % 32 + 9) % 25 * 16 + (hexstr[i+1] % 32 + 9) % 25;
    return chrs;
}

static unsigned int strhash(const char* s)
{
    unsigned int highorder;
    unsigned int h = 0;
    int i;
    for (i=0; s[i] != '\0'; i++)
    {
         highorder = h & 0xf8000000;    // extract high-order 5 bits from h
                                        // 0xf8000000 is the hexadecimal representation
                                        //   for the 32-bit number with the first five 
                                        //   bits = 1 and the other bits = 0   
         h = h << 5;                    // shift h left by 5 bits
         h = h ^ (highorder >> 27);     // move the highorder 5 bits to the low-order
                                        //   end and XOR into h
         h = h ^ (unsigned int)s[i];                  // XOR h and ki
    }
    return h;
}
