#include "properties.hpp"

namespace Item
{

const int MAX_ITEMS = 256;

int sprite_array[MAX_ITEMS]; //maps item id to sprite

int id_to_sprite(int id)
{
    assert(id < MAX_ITEMS && id >= 0);

    return id;
    //return sprite_array[id];
}
/*
Names
*/


char item_names[MAX_ITEMS*64];
int item_name_index[MAX_ITEMS];


/*
struct cubeProperties* get_cube(int id)
{
    if (id < 0) printf("get_cube: error id less than zero \n");
    if (id >= MAX_ITEMS ) printf("get_cube: error id exceeds MAX_ITEMS \n");
    return &cube_list[id];
}
*/

void set_item_name(int id, char* name, int length)
{
    static int index = 0;

    if (length >= 64)
    {
        printf("Error: %s, name length greater than 63 characters \n", __func__ );
        return;
    }

    if (id < 0 || id >= MAX_ITEMS)
    {
        printf("Error: out of bounds, %s \n", __func__);
        return;
    }

    item_name_index[id] = index;

    memcpy(item_names+index, name, length);
    index += length;
    item_names[index] = 0x00;
    index++;
}

char* get_item_name(int id)
{
    if (id < 0 || id >= MAX_ITEMS)
    {
        printf("Error:  %s, item id error \n",  __func__);
        return NULL;
    }

    return (item_names + item_name_index[id]);
}

int get_item_id(char* name)
{
    for (int i=0; i<MAX_ITEMS; i++)
        if (strcmp(name, get_item_name(i)) == 0)
            return i;
    return -1;
}

}
