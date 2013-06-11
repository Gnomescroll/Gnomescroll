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

void init_sprite_mob_list()
{
    GS_ASSERT(sprite_mob_list == NULL);
    sprite_mob_list = new SpriteMobList(MOBS_MAX, NULL_SPRITE_MOB);
}

void teardown_sprite_mob_list()
{
    delete sprite_mob_list;
}

#if DC_CLIENT
void draw_sprite_mobs()
{
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

bool hitscan_sprite_mobs(const Vec3& position, const Vec3& direction, float range,
                         SpriteMobID& id, float& distance)
{
    SpriteMobID nearest_mob = NULL_SPRITE_MOB;
    float nearest_distance = 100000.0f;
    const float range_sq = range * range;
    const Vec3 up = vec3_init(0, 0, 1);
    Vec3 line_point = vec3_init(0);
    for (size_t i=0, j=0; i<sprite_mob_list->max && j<sprite_mob_list->count; i++)
    {   // TODO
        // do a line-plane intersection test against the mob, if its in frustum
        SpriteMob* m = sprite_mob_list->objects[i];
        if (m == NULL) continue;
        j++;

        Vec3 p = m->get_center();
        p = quadrant_translate_position(position, p);

        if (vec3_distance_squared(position, p) > range_sq)
            continue;

        float rad_sq = 10000.0f;
        if (!sphere_line_distance(position, direction, p, line_point, rad_sq))
            continue;

        Vec3 forward = vec3_sub(position, p);
        forward.z = 0.0f;
        forward = vec3_normalize(forward);
        const Vec3 right = vec3_normalize(vec3_cross(forward, up));

        float d = 1000000.0f;
        float width = get_mob_width(m->type);
        float height = get_mob_height(m->type);
        if (!line_plane_intersection(position, direction, p, width, height,
                                     forward, right, up, d))
            continue;

        if (d >= nearest_distance)
            continue;

        nearest_distance = d;
        nearest_mob = m->id;
    }

    id = nearest_mob;
    distance = nearest_distance;
    return (nearest_mob != NULL_SPRITE_MOB);
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
