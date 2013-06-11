#include "config.hpp"

#include <t_mob/sprites/properties.hpp>
#include <t_mob/sprites/mob.hpp>

namespace t_mob
{

static void verify_sprite_mob_config()
{
    for (size_t i=0; i<sprite_mob_properties->max; i++)
    {
        SpriteMobProperty* p = &sprite_mob_properties->properties[i];
        if (!p->loaded) continue;
        GS_ASSERT(p->radius > 0.0f && p->radius <= 4.0f);
        GS_ASSERT(p->width > 0 && p->width <= 1.0f);
        GS_ASSERT(p->height > 0 && p->height <= 1.0f);
        GS_ASSERT(p->size > 0 && p->size <= 8.0f);
        GS_ASSERT(p->animation_group_id != NULL_SPRITE_ANIMATION_GROUP);
        GS_ASSERT(p->type != NULL_SPRITE_MOB_TYPE);
    }

    for (size_t i=0; i<sprite_mob_properties->max-1; i++)
    for (size_t j=i+1; j<sprite_mob_properties->max; j++)
    {
        SpriteMobProperty* a = &sprite_mob_properties->properties[i];
        SpriteMobProperty* b = &sprite_mob_properties->properties[j];
        if (!a->loaded || !b->loaded) continue;
        GS_ASSERT(strcmp(a->name, b->name) != 0);
        GS_ASSERT(strcmp(a->pretty_name, b->pretty_name) != 0);
    }
}

static void apply_automatic_properties()
{
    // TODO compute width, height from the sprite in the animations
    // The texture sheet loader should do that actually

    for (size_t i=0; i<sprite_mob_properties->max; i++)
    {
        SpriteMobProperty* p = &sprite_mob_properties->properties[i];
        if (!p->loaded) continue;
        p->set_radius();
    }
}

SpriteMobProperty* s = NULL;
SpriteAnimation* a = NULL;
SpriteAnimationGroup* g = NULL;

static void add_mob(const char* name, const char* sheet_name)
{
    s = sprite_mob_properties->get_next();
    IF_ASSERT(s == NULL) return;
    s->set_name(name);
    g = animations->create(sheet_name);
    s->animation_group_id = g->id;
}

static void add_animation(const char* name)
{
    IF_ASSERT(s == NULL || g == NULL) return;
    a = g->create(name);
    GS_ASSERT(a != NULL);
}

void load_sprite_mob_config()
{
    add_mob("lizard_thief", "rdn");
    // TODO compute these from the sprite in the animations
    // The texture sheet loader should do that actually
    //s->width = 0.5f;
    //s->height = 0.5f;
    add_animation("walk");
    a->add_frame_strip(1, 1, 2);

    sprite_mob_properties->done_loading();
    sprite_mob_properties->set_pretty_names();
    apply_automatic_properties();
    verify_sprite_mob_config();
    #if DC_CLIENT
    animations->texture_loader.generate_texture();
    animations->texture_loader.save_texture("mobs");
    #endif
    animations->verify();
}

}   // t_mob
