#include "properties.hpp"

#include <common/macros.hpp>

namespace t_mech
{
    
class DatNameMap* mech_name_map = NULL;
class MechAttribute* mech_attributes = NULL;  //index from type to attributes

void init_properties()
{
    GS_ASSERT(mech_name_map == NULL);
    mech_name_map = new class DatNameMap(256, DAT_NAME_MAX_LENGTH);
    
    GS_ASSERT(mech_attributes == NULL);
    mech_attributes = new class MechAttribute[MAX_MECHS];
}

void teardown_properties()
{
    if (mech_name_map != NULL) delete mech_name_map;
    if (mech_attributes != NULL) delete[] mech_attributes;
}

const char* get_mech_name(MechType type)
{
    class MechAttribute* attr = get_mech_attribute(type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return NULL;
    return attr->name;
}

MechType get_mech_type(const char* name)
{
    for (int i=0; i<MAX_MECHS; i++)
    {
        const char* cmp_name = get_mech_name((MechType)i);
        if (cmp_name != NULL && strcmp(name, cmp_name) == 0)
            return (MechType)i;
    }
    GS_ASSERT_LIMIT(false, 1);
    printf("In function %s:%d -- No mech for name %s\n", __FUNCTION__, __LINE__, name);
    return NULL_MECH_TYPE;
}

class MechAttribute* get_mech_attribute(MechType mech_type)
{
    ASSERT_VALID_MECH_TYPE(mech_type);
    IF_INVALID_MECH_TYPE(mech_type) return NULL;
    if (!mech_attributes[mech_type].loaded) return NULL;
    return &mech_attributes[mech_type];
}

MechClass get_mech_class(MechType mech_type)
{
    class MechAttribute* attr = get_mech_attribute(mech_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return NULL_MECH_CLASS;
    return attr->mech_type_class;
}

int get_mech_render_type(MechType mech_type)
{
    class MechAttribute* attr = get_mech_attribute(mech_type);
    GS_ASSERT(attr != NULL);
    if (attr == NULL) return MECH_RENDER_TYPE_NONE;
    return attr->render_type;
}

bool get_mech_type_in_use(MechType mech_type)
{
    return (get_mech_attribute(mech_type) != NULL);
}

bool is_valid_mech_name(const char* name)
{
    size_t len = strlen(name);
    if (len <= 0 || len > DAT_NAME_MAX_LENGTH) return false;
    for (size_t i=0; i<len; i++)
        if (!is_valid_name_char(name[i]))
            return false;
    return true;
}

MechType get_compatible_mech_type(const char* name)
{
    const char* mapname = mech_name_map->get_mapped_name(name);
    if (mapname != NULL)
        return get_mech_type(mapname);
    return get_mech_type(name);
}


}   // t_mech
