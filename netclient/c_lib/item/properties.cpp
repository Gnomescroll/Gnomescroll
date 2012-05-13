#include "properties.hpp"

#include <item/_interface.hpp>
#include <item/common/constant.hpp>
#include <item/common/struct.hpp>

#include <item/config/item_attribute.hpp>

namespace Item
{


int sprite_array[MAX_ITEMS]; //maps item id to sprite
int group_array[MAX_ITEMS];
class ItemAttribute* item_attribute_array = NULL;
class NaniteStoreItem* nanite_store_item_array = NULL;
class CraftingRecipe* crafting_recipe_array = NULL;

void init_properties()
{
    for (int i=0; i<MAX_ITEMS; sprite_array[i++] = ERROR_SPRITE);
    for (int i=0; i<MAX_ITEMS; group_array[i++] = IG_ERROR);

    assert(item_attribute_array == NULL);
    item_attribute_array = new ItemAttribute[MAX_ITEMS];
    assert(nanite_store_item_array == NULL);
    nanite_store_item_array = new NaniteStoreItem[MAX_ITEMS];

    crafting_recipe_array = new CraftingRecipe[MAX_CRAFTING_RECIPE];
}

void tear_down_properties()
{
    if (item_attribute_array    != NULL) delete[] item_attribute_array;
    if (nanite_store_item_array != NULL) delete[] nanite_store_item_array;
    if (nanite_store_item_array != NULL) delete[] crafting_recipe_array;
}

class ItemAttribute* get_item_attributes(int item_type)
{
    return &item_attribute_array[item_type];
}

int get_item_fire_rate(int item_type)
{
    // TODO
    return 5;
}

int get_sprite_index_for_id(ItemID id)
{
    assert(id < MAX_ITEMS && id >= 0);
    int type = get_item_type(id);
    if (type == NULL_ITEM_TYPE) return ERROR_SPRITE;
    assert(type >= 0 && type < MAX_ITEMS);
    return sprite_array[type];
}

int get_sprite_index_for_type(int type)
{
    if (type == NULL_ITEM_TYPE) return ERROR_SPRITE;
    assert(type >= 0 && type < MAX_ITEMS);
    return sprite_array[type];
}

/*
Names
*/


const int ITEM_NAME_MAX_LENGTH = 64;
char item_names[MAX_ITEMS*ITEM_NAME_MAX_LENGTH];
int item_name_index[MAX_ITEMS];

void set_item_name(int id, char* name, int length)
{
    assert(length > 0);
    assert(id >= 0 || id < MAX_ITEMS);
    
    if (length >= ITEM_NAME_MAX_LENGTH)
    {
        printf("Error: %s, name length greater than 63 characters \n", __func__ );
        assert(length < ITEM_NAME_MAX_LENGTH);
    }

    static int index = 0;

    item_name_index[id] = index;

    memcpy(item_names+index, name, length);
    index += length;
    item_names[index] = '\0';
    index++;
}

void set_item_name(int id, char* name)
{
    int length = strlen(name);
    set_item_name(id, name, length);
}


char* get_item_name(int type)
{
    assert(type >= 0 || type < MAX_ITEMS);
    return (item_names + item_name_index[type]);
}

int get_item_type(char* name)
{
    for (int i=0; i<MAX_ITEMS; i++)
        if (strcmp(name, get_item_name(i)) == 0)
            return i;
    return NULL_ITEM_TYPE;
}

int get_item_group_for_type(int item_type)
{
    return group_array[item_type];
}

int dat_get_item_type(const char* name)
{
    int type = get_item_type((char*) name);
    if (type == NULL_ITEM_TYPE)
    {
        printf("Dat Loading Failure:item_type, dat failure, item '%s' does not exist! \n", name);
        GS_ABORT();
    }
    return type;
}

int get_max_stack_size(int item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    assert(attr != NULL);
    return attr->max_stack_size;
}

int get_max_energy(int item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    assert(attr != NULL);
    return attr->max_energy;
}

int get_max_durability(int item_type)
{
    ItemAttribute* attr = get_item_attributes(item_type);
    assert(attr != NULL);
    return attr->max_durability;
}

void get_nanite_store_item(int level, int xslot, int yslot, int* item_type, int* cost)
{
    for(int i=0; i<MAX_ITEMS; i++)
    {
        class NaniteStoreItem* n = &nanite_store_item_array[i];
        if(n->level == level && n->xslot == xslot && n->yslot == yslot)
        {
            *item_type = n->item_type;
            *cost = n->nanite_cost;
            return;
        }
    }
    *item_type = NULL_ITEM_TYPE;
    *cost = 0;
}


}
