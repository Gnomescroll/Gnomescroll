#include "common.hpp"

#include <stdlib.h>
#include <ctype.h>

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

static bool is_valid_name_character(char c)
{
    // range of solid ascii chars
    if (c < '!' /* 33 */ || c > '~' /* 126 */)
        return false;
    // additional banned characters
    if (c == '/' || c == '\\')
        return false;
    return true;
}

unsigned int sanitize_player_name(char* name)
{
    // remove whitespace at beginning and end
    // remove non-approved chars
    // can't be all whitespace

    int len = strlen(name);
    if (len == 0) return 0;
    char* new_name = (char*)calloc(len+1, sizeof(char));

    char c;
    unsigned int i = 0, j = 0;
    //  filter valid characters
    while ((c = name[i++]) != '\0')
        if (is_valid_name_character(c))
            new_name[j++] = c;

    new_name[j] = '\0';   // ignore trailing whitespace
    strcpy(name, new_name);
    free(new_name);
    return j;
}
