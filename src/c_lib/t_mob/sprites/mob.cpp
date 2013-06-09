#include "mob.hpp"

#include <t_mob/sprites/textures.hpp>
#include <t_mob/sprites/properties.hpp>

namespace t_mob
{

SpriteMobList* sprite_mob_list = NULL;

void init_sprite_mob_list()
{
    GS_ASSERT(sprite_mob_list == NULL);
    sprite_mob_list = new SpriteMobList(MAX_SPRITE_MOBS, NULL_SPRITE_MOB);
}

void teardown_sprite_mob_list()
{
    delete sprite_mob_list;
}


void draw_sprite_mobs()
{
    for (size_t i=0; i<sprite_mob_list->max; i++)
        if (sprite_mob_list->objects[i] != NULL)
            sprite_mob_list->objects[i]->draw();
}

void hitscan_sprite_mobs(const Vec3& position, const Vec3& direction, float range)
{
    for (size_t i=0; i<sprite_mob_list->max; i++)
    {   // TODO
        // do a line-plane intersection test against the mob, if its in frustum
    }
}

void SpriteMob::draw()
{
    // TODO
}

inline Vec3 SpriteMob::get_center()
{   // TODO -- correct centering
    Vec3 c = this->position;
    c.z += get_mob_height(this->type) * 0.5f;
    return c;
}

inline float SpriteMob::get_radius()
{
    return get_mob_radius(this->type);
}

void SpriteMob::set_type(const char* name)
{
    this->type = get_mob_type(name);
}

SpriteMob::SpriteMob() :
    position(vec3_init(0)), orientation(vec3_init(0))
{
    sprite_mob_list->add(this);
}

SpriteMob::~SpriteMob()
{
    sprite_mob_list->remove(this->id);
}


}   // t_mob
