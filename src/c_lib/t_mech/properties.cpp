#include "properties.hpp"

#include <common/macros.hpp>
#include <t_mech/config/_interface.hpp>

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
    delete mech_name_map;
    delete[] mech_attributes;
}

const char* get_mech_name(MechType type)
{
    class MechAttribute* attr = get_mech_attribute(type);
    IF_ASSERT(attr == NULL) return NULL;
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
    GS_ASSERT(false);
    printf("In function %s:%d -- No mech for name %s\n", __FUNCTION__, __LINE__, name);
    return NULL_MECH_TYPE;
}

class MechAttribute* get_mech_attribute(MechType type)
{
    IF_ASSERT(!isValid(type)) return NULL;
    if (!mech_attributes[type].loaded) return NULL;
    return &mech_attributes[type];
}

MechClassType get_mech_class(MechType type)
{
    class MechAttribute* attr = get_mech_attribute(type);
    IF_ASSERT(attr == NULL) return NULL_MECH_CLASS;
    return attr->class_type;
}

MechRenderType get_mech_render_type(MechType type)
{
    class MechAttribute* attr = get_mech_attribute(type);
    IF_ASSERT(attr == NULL) return MECH_RENDER_TYPE_NONE;
    return attr->render_type;
}

MechBehaviorType get_mech_behavior_type(MechType type)
{
    class MechAttribute* attr = get_mech_attribute(type);
    IF_ASSERT(attr == NULL) return MECH_BEHAVIOR_TYPE_DEFAULT;
    return attr->behavior_type;
}

inline bool type_in_use(MechType type)
{
    return (get_mech_attribute(type) != NULL);
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

const char* get_compatible_mech_name(const char* name)
{
    const char* mapname = mech_name_map->get_mapped_name(name);
    if (mapname != NULL) return mapname;
    if (get_mech_type(name) != NULL_MECH_TYPE) return name;
    return NULL;
}

inline bool is_plant(MechType type)
{
    class MechAttribute* attr = get_mech_attribute(type);
    IF_ASSERT(attr == NULL) return false;
    return attr->plant;
}

Vec3 get_mech_box_dimensions(const struct Mech& mech)
{
    IF_ASSERT(!isValid(mech.type)) return vec3_init(1);
    float size = mech.size * 0.5f;
    int tex_id = mech_attributes[mech.type].sprite;
    GS_ASSERT(mech_sprite_width[mech_attributes[mech.type].sprite] != -1)
    GS_ASSERT(mech_sprite_height[mech_attributes[mech.type].sprite] != -1)
    float size_w = size*mech_sprite_width_f[tex_id];
    float size_h = 2.0f*size*mech_sprite_height_f[tex_id];
    return vec3_init(size_w, size_w, size_h);
}

Vec3 get_mech_center(const struct Mech& mech)
{
    Vec3 offset = vec3_init(0.5f + mech.offset_x, 0.5f + mech.offset_y, 0.0f);
    Vec3 p = vec3_add(vec3_init(mech.position), offset);
    return translate_position(p);
}


}   // t_mech
