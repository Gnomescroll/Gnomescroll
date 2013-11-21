/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "properties.hpp"

#include <common/macros.hpp>
#include <t_mech/config/_interface.hpp>
#include <physics/ray_trace/ray_trace.hpp>

namespace t_mech
{

class DatNameMap* mech_name_map = NULL;
class MechAttributes* mech_attributes = NULL;  //index from type to attributes

void init_properties()
{
    GS_ASSERT(mech_name_map == NULL);
    mech_name_map = new class DatNameMap(256, DAT_NAME_MAX_LENGTH);

    GS_ASSERT(mech_attributes == NULL);
    mech_attributes = new class MechAttributes;
}

void teardown_properties()
{
    delete mech_name_map;
    delete mech_attributes;
}

const char* get_mech_name(MechType type)
{
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return NULL;
    return attr->name;
}

const char* get_mech_pretty_name(MechType type)
{
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return NULL;
    return attr->pretty_name;
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

inline class MechAttribute* get_mech_attributes(MechType type)
{
    return mech_attributes->get(type);
}

MechClassType get_mech_class(MechType type)
{
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return NULL_MECH_CLASS;
    return attr->class_type;
}

MechRenderType get_mech_render_type(MechType type)
{
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return MECH_RENDER_TYPE_NONE;
    return attr->render_type;
}

MechBehaviorType get_mech_behavior_type(MechType type)
{
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return MECH_BEHAVIOR_TYPE_DEFAULT;
    return attr->behavior_type;
}

inline bool type_in_use(MechType type)
{
    return (get_mech_attributes(type) != NULL);
}

inline bool is_valid_mech_name(const char* name)
{
    return is_valid_name(name);
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
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return false;
    return attr->plant;
}

inline bool has_item_drop(MechType type)
{
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return false;
    return attr->item_drop;
}

inline int get_mech_growth_ttl(MechType type)
{
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return -1;
    return attr->growth_ttl;
}

inline MechType get_mech_growth_stage(MechType type)
{
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return NULL_MECH_TYPE;
    return attr->growth_stage;
}

inline MechSpriteIndex get_mech_sprite(MechType type)
{
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return NULL_MECH_SPRITE;
    return attr->sprite;
}

Vec3 get_mech_box_dimensions(MechType type)
{
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return vec3_init(1);
    return attr->dimensions;
}

Vec3 get_mech_center(const struct Mech& mech)
{
    if (mech.render_type == MECH_RENDER_TYPE_3)
    {   // wall
        Vec3 offset = vec3_init(0.5f);
        offset.z *= 1.0f - get_mech_size(mech.type);
        Vec3 center = vec3_add(vec3_init(mech.position), offset);
        Vec3 side = vec3_init(get_sides_from_cube_side(mech.side));
        side = vec3_scalar_mult(side, -0.5f);
        center = vec3_add(center, side);
        return translate_position(center);
    }
    else
    {   // ground
        Vec3 offset = vec3_init(0.5f + mech.offset_x, 0.5f + mech.offset_y, 0.0f);
        Vec3 p = vec3_add(vec3_init(mech.position), offset);
        return translate_position(p);
    }
}

float get_mech_radius(MechType type)
{
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return 0.5f;
    return attr->radius;
}

float get_mech_size(MechType type)
{
    class MechAttribute* attr = get_mech_attributes(type);
    IF_ASSERT(attr == NULL) return 1.0f;
    return attr->size;
}

void update_dimensions()
{
    for (size_t i=0; i<mech_attributes->max; i++)
    {
        class MechAttribute* a = &mech_attributes->properties[i];
        if (!a->loaded) continue;
        float size = a->size;
        float size_w = size * 0.5f;
        float size_h = size;
        int tex_id = a->sprite;
        if (tex_id >= 0 && tex_id < MAX_MECHS)
        {
            GS_ASSERT(mech_sprite_width[tex_id] != -1)
            GS_ASSERT(mech_sprite_height[tex_id] != -1)
            size_w = 0.5f*size*mech_sprite_width_f[tex_id];
            size_h = size*mech_sprite_height_f[tex_id];
        }
        a->dimensions = vec3_init(size_w, size_w, size_h);
        a->radius = GS_MAX(size_w, size_h) * 0.5f;
    }
}

}   // t_mech
