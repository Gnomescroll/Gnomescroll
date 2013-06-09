#include "properties.hpp"

#include <t_mob/sprites/common.hpp>
#include <common/dat/properties.hpp>

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

float get_mob_radius(SpriteMobType type)
{
    SpriteMobProperty* p = sprite_mob_properties->get(type);
    IF_ASSERT(p == NULL) return 0.5f;
    return p->radius;
}

float get_mob_height(SpriteMobType type)
{
    SpriteMobProperty* p = sprite_mob_properties->get(type);
    IF_ASSERT(p == NULL) return 1.0f;
    return p->height;
}

SpriteMobType get_mob_type(const char* name)
{
    SpriteMobProperty* p = sprite_mob_properties->get(name);
    IF_ASSERT(p == NULL) return NULL_SPRITE_MOB_TYPE;
    return p->type;
}

}   // t_mob
