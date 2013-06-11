#include "mob.hpp"

#include <entity/constants.hpp>
#include <t_mob/sprites/textures.hpp>
#include <t_mob/sprites/properties.hpp>

#if DC_CLIENT
# include <common/draw/draw.hpp>
#endif

namespace t_mob
{

SpriteMobList* sprite_mob_list = NULL;

SpriteMob* _mob = NULL; // TODO - remove, debug only

void init_sprite_mob_list()
{
    GS_ASSERT(sprite_mob_list == NULL);
    sprite_mob_list = new SpriteMobList(MOBS_MAX, NULL_SPRITE_MOB);
}

void teardown_sprite_mob_list()
{
    delete _mob;
    delete sprite_mob_list;
}

#if DC_CLIENT
void draw_sprite_mobs()
{
    if (_mob == NULL)
    {
        _mob = new SpriteMob();
        _mob->init("lizard_thief");
    }

    using ClientState::player_agent;
    _mob->position = vec3_add(player_agent.camera_position(),
                              vec3_scalar_mult(player_agent.forward_vector(), 5.0f));
    _mob->position.z = t_map::get_nearest_surface_block(vec3i_init(_mob->position));

    IF_ASSERT(animations->texture_loader.texture == 0)
        return;

    glDisable(GL_BLEND);
    glColor4ub(255, 255, 255, 255);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, animations->texture_loader.texture);
    glBegin(GL_QUADS);

    for (size_t i=0; i<sprite_mob_list->max; i++)
        if (sprite_mob_list->objects[i] != NULL)
            sprite_mob_list->objects[i]->draw();

    glEnd();
    glDisable(GL_ALPHA_TEST);
    CHECK_GL_ERROR();
}
#endif

void tick_mob_sprites()
{
    for (size_t i=0; i<sprite_mob_list->max; i++)
        if (sprite_mob_list->objects[i] != NULL)
            sprite_mob_list->objects[i]->tick();
}

void hitscan_sprite_mobs(const Vec3& position, const Vec3& direction, float range)
{
    for (size_t i=0; i<sprite_mob_list->max; i++)
    {   // TODO
        // do a line-plane intersection test against the mob, if its in frustum
    }
}

const SpriteAnimationGroup* SpriteMob::_begin_animation()
{
    const SpriteAnimationGroup* g = get_mob_animation_group(this->type);
    IF_ASSERT(g == NULL) return NULL;
    this->animation_tick = 0;
    this->current_animation_frame = 0;
    return g;
}

void SpriteMob::begin_animation(const char* name)
{
    const SpriteAnimationGroup* g = this->_begin_animation();
    this->current_animation = g->get(name);
    GS_ASSERT(this->current_animation != NULL);
}

void SpriteMob::begin_animation(SpriteAnimationID id)
{
    const SpriteAnimationGroup* g = this->_begin_animation();
    this->current_animation = g->get(id);
    GS_ASSERT(this->current_animation != NULL);
}

void SpriteMob::begin_default_animation()
{
    this->begin_animation(SpriteAnimationID(0));
}

void SpriteMob::stop_animation()
{
    this->begin_default_animation();
}

void SpriteMob::tick()
{
    if (this->current_animation == NULL)
        return;
    this->animation_tick++;
    this->current_animation_frame = (this->animation_tick / ANIMATION_SPEED) %
                                     this->current_animation->count;
}

#if DC_CLIENT
void SpriteMob::draw()
{
    if (this->current_animation == NULL) return;
    int tex = this->current_animation->get_frame(this->current_animation_frame);
    IF_ASSERT(tex == NULL_SPRITE) return;
    Draw::drawAxialBillboardSprite(this->position, tex, get_mob_size(this->type));
    // TODO -- shader, with lighting
}
#endif

inline Vec3 SpriteMob::get_center()
{
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

void SpriteMob::init(SpriteMobType type)
{
    GS_ASSERT(this->id == NULL_SPRITE_MOB && this->type == NULL_SPRITE_MOB_TYPE);
    this->type = type;
    sprite_mob_list->add(this);
    this->begin_default_animation();
}

void SpriteMob::init(const char* name)
{
    this->init(get_mob_type(name));
}

SpriteMob::SpriteMob() :
    id(NULL_SPRITE_MOB), type(NULL_SPRITE_MOB_TYPE), position(vec3_init(0)),
    orientation(vec3_init(0)), animation_tick(0), current_animation(NULL),
    current_animation_frame(0)
{
}

SpriteMob::~SpriteMob()
{
    if (this->id != NULL_SPRITE_MOB)
        sprite_mob_list->remove(this->id);
}

}   // t_mob
