/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "properties.hpp"

#include <common/dat/properties.hpp>
#include <t_mob/sprites/common.hpp>
#include <t_mob/sprites/textures.hpp>

namespace t_mob
{

void init_sprite_mob_properties()
{
    GS_ASSERT(sprite_mob_properties == NULL);
    sprite_mob_properties = new SpriteMobProperties(MAX_SPRITE_MOB_TYPES);
}

void teardown_sprite_mob_properties()
{
    delete sprite_mob_properties;
}

#define GET_MOB_PROPERTY(return_type, name, default_value) \
    return_type get_mob_##name(SpriteMobType type) \
    { \
        SpriteMobProperty* p = sprite_mob_properties->get(type); \
        IF_ASSERT(p == NULL) return default_value; \
        return p->name; \
    }

GET_MOB_PROPERTY(SpriteAnimationGroupID, animation_group_id, NULL_SPRITE_ANIMATION_GROUP);
GET_MOB_PROPERTY(float, radius, 0.5f);
GET_MOB_PROPERTY(float, height, 1.0f);
GET_MOB_PROPERTY(float, width, 1.0f);
GET_MOB_PROPERTY(float, size, 1.0f);
GET_MOB_PROPERTY(const char*, name, "UNDEFINED");
GET_MOB_PROPERTY(const char*, pretty_name, "UNDEFINED");


const SpriteAnimationGroup* get_mob_animation_group(SpriteMobType type)
{
    SpriteAnimationGroupID id = get_mob_animation_group_id(type);
    if (!isValid(id)) return NULL;
    return animations->get(id);
}

SpriteMobType get_mob_type(const char* name)
{
    SpriteMobProperty* p = sprite_mob_properties->get(name);
    IF_ASSERT(p == NULL) return NULL_SPRITE_MOB_TYPE;
    return p->type;
}

#undef GET_MOB_PROPERTY

}   // t_mob
