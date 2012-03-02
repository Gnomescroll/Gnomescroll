#include "common.hpp"

#include <stdlib.h>

void address_from_string(char* ip, int address[4])
{   // parse IP address string

    int address_index = 0;

    char c;
    int i=0, j=0;
    char tmp[3+1];
    while ((c = ip[i++]) != '\0')
    {
        if (c == '.')
        {
            tmp[j] = '\0';
            address[address_index++] = atoi(tmp);
            j = 0;
            continue;
        }
        tmp[j++] = c;
    }
    tmp[j] = '\0';
    address[address_index] = atoi(tmp);

}

// strcpy but dont add \0 to the end of a
void strcpy_no_null(char* dest, char* src)
{
    char c;
    int i = 0;
    while ((c = src[i]) != '\0')
        dest[i++] = c;
}
