#include "animations.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <common/random.hpp>
#include <state/client_state.hpp>

#include <physics/vec3.hpp>
#include <physics/mat3.hpp>

#include <particle/_include.hpp>
#include <particle/shrapnel.hpp>
#include <particle/blood.hpp>

#include <animations/config/_interface.hpp>

namespace Animations
{

void block_crumble(struct Vec3 pos, int n, CubeType cube_type, float momentum)
{
    if (Options::animation_level <= 0) return;
    const float crumble_size = 0.17f;
    float scale = float(Options::animation_level)/3.0f;
    n = scale*float(n);
    int side;
    int ttl;
    int tex_id;
    struct Vec3 next;
    struct Vec3 forward;
    float theta, phi;
    class Particle::TexturedMinivox* minivox;
    for (int i=0; i < n; i++)
    {
        next = vec3_add(pos, vec3_rand_center());
        next = translate_position(next);
        forward = vec3_scalar_mult(vec3_rand_center(), momentum);
        theta = randf() * PI * 2;
        phi = randf() * PI * 2;
        side = randrange(0,5);
        tex_id = t_map::get_cube_side_texture(cube_type, side);
        minivox = Particle::textured_minivox_list->create();
        if (minivox == NULL) return;
        ttl = randrange(60,95);
        minivox->set_ttl(ttl);
        minivox->voxel.texture_index = tex_id;
        minivox->voxel.pixel_width = 2;
        minivox->voxel.set_texture();
        minivox->set_size(crumble_size);
        minivox->voxel.set_rotation_delta((randf()* 0.04f) - 0.02f, (randf()* 0.04f) - 0.02f);
        minivox->voxel.set_rotation(theta, phi);
        minivox->set_state(next, forward);
    }
}

void block_crumble(struct Vec3 pos, int n, CubeType cube_type, TerrainModificationAction action)
{
    float momentum = 5.0f;
    switch (action)
    {
        case TMA_PICK:
            momentum = 2.0f;
            break;
        case TMA_GRENADE:
        case TMA_PLASMAGEN:
            momentum = 10.0f;
            break;
        case TMA_LASER:
            momentum = 0.5f;
            break;
        case TMA_MONSTER_BOX:
            momentum = 0.5f;
            break;
        case TMA_MONSTER_BOMB:
            momentum = 10.0f;
            break;
        case TMA_NONE:
            break;
        case TMA_APPLIER:
        case TMA_TURRET:
        case TMA_PLACE_BLOCK:
            GS_ASSERT(false);
            break;
    }
    block_crumble(pos, n, cube_type, momentum);
}

// surface block dmg
// takes encoded side int and translates to side[3];
// collision point, incident vector, cube_type, side
void block_damage(struct Vec3 pos, struct Vec3 incident, CubeType cube_type, int cube_side)
{
    int side[3];
    get_side_array_from_cube_side(cube_side, side);
    block_damage(pos, incident, cube_type, side, cube_side);
}

// surface block dmg
// collision point, incident vector, cube_type, side
void block_damage(struct Vec3 pos, struct Vec3 incident, CubeType cube_type, int side[3])
{
    int cube_side = get_cube_side_from_side_array(side);
    block_damage(pos, incident, cube_type, side, cube_side);
}

void block_damage(struct Vec3 pos, struct Vec3 incident, CubeType cube_type, int _side[3], int cube_side)
{
    if (Options::animation_level <= 0) return;
    int n = randrange(10,15);
    float scale = float(Options::animation_level)/3.0f;
    n = scale*float(n);
    int tex_id = t_map::get_cube_side_texture(cube_type, cube_side);
    float theta, phi;
    // reflection bias
    Vec3 nor = vec3_init(_side);
    Vec3 ref = vec3_reflect(incident, nor);
    ref = vec3_normalize(ref);
    // compute initial base velocities
    const float momentum = 0.2f;
    // "invert" the normal for perturbing the initial positions along the plane
    // this flips the 0 vectors to 1, which will be centered to be 1,-1 in the perturbation
    _side[0] = (_side[0]) ? 0 : 1;
    _side[1] = (_side[1]) ? 0 : 1;
    _side[2] = (_side[2]) ? 0 : 1;
    struct Vec3 side = vec3_init(_side);
    class Particle::TexturedMinivox* minivox;
    struct Vec3 next;
    struct Vec3 forward;
    const float vox_size = 0.05f;
    int ttl;
    for (int i=0; i<n; i++)
    {
        forward = vec3_add(ref, vec3_rand_center());
        forward = vec3_scalar_mult(forward, momentum);
        // perturb the initial positions along the side's plane
        next = vec3_scalar_mult(vec3_mult(vec3_rand_center(), side), 0.4f);
        next = translate_position(vec3_add(pos, next));
        theta = randf() * PI * 2;
        phi = randf() * PI * 2;
        minivox = Particle::textured_minivox_list->create();
        if (minivox == NULL) return;
        ttl = randrange(75,85);
        minivox->set_ttl(ttl);
        minivox->voxel.texture_index = tex_id;
        minivox->voxel.pixel_width = 2;
        minivox->voxel.set_texture();
        minivox->set_size(vox_size);
        minivox->voxel.set_rotation_delta((randf()* 0.04f) - 0.02f, (randf()* 0.04f) - 0.02f);
        minivox->voxel.set_rotation(theta, phi);
        minivox->set_state(next, forward);
    }
}

void grenade_explode(struct Vec3 pos)
{
    if (Options::animation_level <= 0) return;
    int n = randrange(25,35);
    float scale = float(Options::animation_level)/3.0f;
    n = scale*float(n);
    const float vel = 20.0f;
    const float displacement = 1.0f/20.0f;
    struct Vec3 c;
    struct Vec3 cv;
    Particle::Shrapnel *s;
    for (int i=0; i<n; i++)
    {
        c = vec3_add(pos, vec3_scalar_mult(vec3_rand_center(), displacement));
        c = translate_position(c);
        cv = vec3_scalar_mult(vec3_rand_center(), vel);
        s = Particle::create_shrapnel(c, cv);
        if (s == NULL) return;
        s->ttl = randrange(15,25);
    }
}

void particle_explode(struct Vec3 pos, int min, int max, float vel, float scale)
{
    if (Options::animation_level <= 0) return;
    int n = randrange(min,max);
    float anim_scale = float(Options::animation_level)/3.0f;
    n = anim_scale*float(n);
    const float displacement = 1.0f/20.0f;
    struct Vec3 c;
    struct Vec3 cv;
    Particle::Shrapnel *s;
    for (int i=0; i<n; i++)
    {
        c = vec3_add(pos, vec3_scalar_mult(vec3_rand_center(), displacement));
        c = translate_position(c);
        cv = vec3_scalar_mult(vec3_rand_center(), vel);
        s = Particle::create_shrapnel(c, cv);
        if (s==NULL) return;
        s->ttl = randrange(8,15);
        s->scale = scale;
        s->texture_index = 54;
    }
}

void particle_explode(struct Vec3 pos)
{
    particle_explode(pos, 15, 20, 3.5f, 0.05f);
}

void voxel_explode(Vec3 position, int count, float size, float force, Color color)
{
    if (Options::animation_level <= 0) return;
    float scale = float(Options::animation_level)/3.0f;
    count = scale*float(count);
    struct Vec3 c;
    struct Vec3 cv;
    float theta, phi;
    float dtheta, dphi;
    Particle::ColoredMinivox* minivox;
    int ttl;
    const float displacement = 1.0f/20.0f;
    for (int i=0; i<count; i++)
    {
        ttl = randrange(20,50);
        c = vec3_add(position, vec3_scalar_mult(vec3_rand_center(), displacement));
        c = translate_position(c);
        cv = vec3_scalar_mult(vec3_rand_center(), force);
        theta = randf() * PI * 2;
        phi = randf() * PI * 2;
        dtheta = randf() * 0.01f;
        dphi = randf() * 0.01f;
        minivox = Particle::colored_minivox_list->create();
        if (minivox == NULL) return;
        minivox->set_color(color.r, color.g, color.b);//sky blue
        minivox->set_ttl(ttl);
        minivox->voxel.set_rotation_delta(dtheta, dphi);
        minivox->voxel.set_rotation(theta, phi);
        minivox->set_size(size);
        minivox->set_state(c, cv);
    }
}

// will generate random count between count_min and count_max
void voxel_explode(Vec3 position, int count_min, int count_max, float size, float force, Color color)
{
    int count = randrange(count_min, count_max);
    voxel_explode(position, count, size, force, color);
}

void agent_bleed(struct Vec3 pos)
{
    if (Options::animation_level <= 0) return;
    const float scale = float(Options::animation_level)/3.0f;
    const int n = float(randrange(50,70)) * scale;
    const float momentum = 1.0f;
    for (int i=0; i<n; i++)
    {
        struct Vec3 next = vec3_add(pos, vec3_rand_center());
        next = translate_position(next);
        struct Vec3 forward = vec3_scalar_mult(vec3_rand_center(), momentum);
        Particle::Blood* b = Particle::blood_list->create();
        if (b == NULL) return;
        b->set_state(next, forward);
        int ttl = randrange(0,10) - 5;
        b->set_ttl(b->ttl + ttl);
    }
}

void blood_spray(struct Vec3 pos, struct Vec3 incident)  // pos, incident vector
{
    if (Options::animation_level <= 0) return;
    int n = randrange(140,170);
    float scale = float(Options::animation_level)/3.0f;
    n = scale*float(n);
    incident = vec3_normalize(incident);
    float theta, phi, gamma;
    struct Vec3 v;
    const float base_speed = 1.0f;
    float speed;
    const float arc = 1.0f/48.0f;
    Particle::Blood *b;
    int ttl;
    for (int i=0; i<n; i++)
    {
        theta = randf() * PI * 2;
        phi = randf() * PI * 2;
        gamma = randf() * PI * 2;
        v = vec3_euler_rotation(incident, theta*arc, phi*arc, gamma*arc);
        speed = (randf() + 0.5f) * randrange(1,2);
        speed *= base_speed;
        v = vec3_scalar_mult(v, speed);
        b = Particle::blood_list->create();
        if (b == NULL) return;
        b->set_state(pos, v);
        ttl = randrange(-5, 5);
        b->set_ttl(b->ttl + ttl);
    }
}

// DEBUG
void confetti_stream()
{
    if (agent_camera == NULL) return;
    struct Vec3 p = agent_camera->get_position();
    struct Vec3 f = agent_camera->forward_vector();
    f = vec3_scalar_mult(f, (randf()+1.0f)*3.0f);

    static const int n = 1;
    for (int i=0; i<n; i++)
    {
        Particle::ColoredMinivox* minivox = Particle::colored_minivox_list->create();
        if (minivox == NULL) return;
        minivox->set_color(Color(randrange(0,255), randrange(0,255), randrange(0,255)));
        minivox->set_ttl(100);
        minivox->voxel.set_rotation_delta(0.1f, 0.0f);
        minivox->voxel.set_rotation(randf(), randf());
        minivox->set_size(0.1f);
        minivox->set_state(p.x, p.y, p.z, f.x, f.y, f.z);

    }
}

// new style callback

void create_shrapnel(AnimationType animation_type, void* metadata)
{
    if (Options::animation_level <= 0) return;
    IF_ASSERT(metadata == NULL) return;
    class AnimationStateMetadata* data = (class AnimationStateMetadata*)metadata;
    class AnimationProperty* anim_data = get_animation_data(animation_type);
    IF_ASSERT(anim_data == NULL) return;

    int n = anim_data->count;
    if (anim_data->use_rand_range)
        n = randrange(anim_data->count, anim_data->max_count);
    float scale = float(Options::animation_level)/3.0f;
    n = scale*float(n);
    struct Vec3 p = data->position;
    struct Vec3 v = data->velocity;
    const float jitter = anim_data->jitter_scale;
    struct Vec3 c;
    struct Vec3 cv;
    Particle::Shrapnel *s;
    for (int i=0; i<n; i++)
    {
        c = vec3_add(p, vec3_scalar_mult(vec3_rand_center(), jitter));
        c = translate_position(c);
        cv = vec3_mult(v, vec3_rand_center());
        s = Particle::create_shrapnel(c, cv);
        if (s == NULL) return;
        s->ttl = randrange(anim_data->ttl_min, anim_data->ttl_max);
    }
}

}   // Animations
