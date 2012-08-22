#include "properties.hpp"

#include <common/macros.hpp>

/*
struct MECH_ATTRIBUTE
{
    int mech_type;
    int mech_type_class;
    int render_type;
    int sprite_index;
};
*/

namespace t_mech
{
    
struct MECH_ATTRIBUTE* mech_attribute = NULL;  //index from type to attributes

const int MAX_MECHS = 256;
const int MECH_NAME_MAX_LENGTH = 64;

char mech_names[MAX_MECHS*MECH_NAME_MAX_LENGTH];
int mech_name_index[MAX_MECHS] = {-1};

void init_properties()
{
    GS_ASSERT(mech_attribute == NULL);
    mech_attribute = (struct MECH_ATTRIBUTE*) malloc(MAX_MECHS* sizeof(struct MECH_ATTRIBUTE));
    for(int i=0; i<MAX_MECHS; i++) mech_attribute[i].mech_type = -1;
}

void tear_down_properties()
{
    if (mech_attribute != NULL) free(mech_attribute);
}

void set_mech_name(int id, const char* name)
{
    int length = (int)strlen(name);

    GS_ASSERT(length > 0);
    GS_ASSERT(length < MECH_NAME_MAX_LENGTH);
    GS_ASSERT(id >= 0 || id < MAX_MECHS);    
    
    if (length <= 0 || length >= MECH_NAME_MAX_LENGTH) return;
    if (id < 0 || id >= MAX_MECHS) return;

    static int str_index = 0;

    for (int i=0; i<MAX_MECHS; i++)    // no duplicate names
        if (mech_name_index[i] >= 0)
            GS_ASSERT(strcmp(mech_names+mech_name_index[i], name));

    mech_name_index[id] = str_index;

    memcpy(mech_names+str_index, name, length);
    str_index += length;
    mech_names[str_index++] = '\0';
}


const char* get_mech_name(int type)
{
    ASSERT_VALID_MECH_TYPE(type);
    IF_INVALID_MECH_TYPE(type) return NULL;
    GS_ASSERT(mech_name_index[type] >= 0);
    if (mech_name_index[type] < 0) return NULL;
    GS_ASSERT(mech_name_index[type] < MAX_MECHS*MECH_NAME_MAX_LENGTH);
    if (mech_name_index[type] >= MAX_MECHS*MECH_NAME_MAX_LENGTH) return NULL;
    
    return (mech_names + mech_name_index[type]);
}

int get_mech_type(const char* name)
{
    for (int i=0; i<MAX_MECHS; i++)
    {
        const char* cmp_name = get_mech_name(i);
        if (cmp_name != NULL && strcmp(name, cmp_name) == 0)
            return i;
    }
    GS_ASSERT_LIMIT(false, 1);
    printf("In function %s:%d -- No mech for name %s\n", __FUNCTION__, __LINE__, name);
    return -1;
}

struct MECH_ATTRIBUTE* get_mech_attribute(int mech_type)
{
    GS_ASSERT(mech_attribute[mech_type].mech_type != -1);
    return &mech_attribute[mech_type];
}

}   // t_mech
