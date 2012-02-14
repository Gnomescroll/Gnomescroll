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
