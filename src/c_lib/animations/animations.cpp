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

void block_crumble(float x, float y, float z, int n, int cube_id, float momentum)
{
    if (Options::animation_level <= 0) return;
    const float crumble_size = 0.17f;

    float scale = ((float)Options::animation_level)/3.0f;
    n = scale*((float)n);

    //unsigned char r,g,b,a;
    int side;
    int ttl;
    int tex_id;
    float nx,ny,nz;
    float vx,vy,vz;

    float theta,phi;

    class Particle::TexturedMinivox* minivox;
    
    for (int i=0; i < n; i++)
    {
        nx = x + randf() -0.5f;
        ny = y + randf() -0.5f;
        nz = z + randf() -0.5f;

        vx = momentum*(randf() -0.5f);
        vy = momentum*(randf() -0.5f);
        vz = momentum*(randf() -0.5f);

        nx = translate_point(nx);
        ny = translate_point(ny);

        theta = randf() * PI * 2;
        phi = randf() * PI * 2;

        side = randrange(0,5);
        tex_id = t_map::get_cube_side_texture(cube_id, side);
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
        minivox->set_state(nx,ny,nz, vx,vy,vz);
    }
}

void block_crumble(float x, float y, float z, int n, int cube_id, TerrainModificationAction action)
{
    float momentum = 5.0f;
    switch (action)
    {
        case t_map::TMA_PICK:
            momentum = 2.0f;
            break;
        case t_map::TMA_GRENADE:
            momentum = 10.0f;
            break;
        case t_map::TMA_LASER:
            momentum = 0.5f;
            break;
        case t_map::TMA_MONSTER_BOX:
            momentum = 0.5f;
            break;
        case t_map::TMA_MONSTER_BOMB:
            momentum = 10.0f;
            break;
        case t_map::TMA_NONE:
            break;
        default:
            GS_ASSERT(false);
            break;
    }
    block_crumble(x,y,z, n, cube_id, momentum);
}

// surface block dmg
// takes encoded side int and translates to side[3];
// collision point, incident vector, cube_id, side
void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_id, int cube_side)
{
    int side[3];
    get_side_array_from_cube_side(cube_side, side);
    block_damage(x,y,z,ix,iy,iz,cube_id,side, cube_side);
}

// surface block dmg
// collision point, incident vector, cube_id, side
void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_id, int *side)
{
    int cube_side = get_cube_side_from_side_array(side);
    block_damage(x,y,z,ix,iy,iz,cube_id, side, cube_side);
}

void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_id, int *side, int cube_side)
{
    if (Options::animation_level <= 0) return;

    int n = randrange(10,15);
    float scale = ((float)Options::animation_level)/3.0f;
    n = scale*((float)n);

    int tex_id = t_map::get_cube_side_texture(cube_id, cube_side);

    float theta,phi;

    // reflection bias
    Vec3 inc = vec3_init(ix,iy,iz);
    Vec3 nor = vec3_init(side[0], side[1], side[2]);
    Vec3 ref = vec3_reflect(inc, nor);
    normalize_vector(&ref);

    // compute initial base velocities
    const float momentum = 0.2f;

    // "invert" the normal for perturbing the initial positions along the plane
    side[0] = (side[0]) ? 0 : 1;
    side[1] = (side[1]) ? 0 : 1;
    side[2] = (side[2]) ? 0 : 1;

    class Particle::TexturedMinivox* minivox;

    float vx,vy,vz;
    float nx,ny,nz;

    const float vox_size = 0.05f;
    int ttl;
    for (int i=0; i<n; i++)
    {
        vx = ref.x + (randf() -0.5f);
        vy = ref.y + (randf() -0.5f);
        vz = ref.z + (randf() -0.5f);
        vx *= momentum;
        vy *= momentum;
        vz *= momentum;

        // perturb the initial positions along the side's plane
        nx = x + ((randf() -0.5f)*0.4f*side[0]);
        ny = y + ((randf() -0.5f)*0.4f*side[1]);
        nz = z + ((randf() -0.5f)*0.4f*side[2]);

        theta = randf() * PI * 2;
        phi = randf() * PI * 2;
        
        nx = translate_point(nx);
        ny = translate_point(ny);
        
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
        minivox->set_state(nx,ny,nz, vx,vy,vz);
    }
}

//const Vec3 _gvset[26] =
//{
    //{  0.57735, 0.57735, 0.57735 },
    //{  0.57735, 0.57735,-0.57735 },
    //{  0.57735,-0.57735, 0.57735 },
    //{ -0.57735, 0.57735, 0.57735 },
    //{  0.57735,-0.57735,-0.57735 },
    //{ -0.57735, 0.57735,-0.57735 },
    //{ -0.57735,-0.57735, 0.57735 },
    //{ -0.57735,-0.57735,-0.57735 },   //8

    //{ 0,0,1 },
    //{ 0,1,0 },
    //{ 1,0,0 },
    //{ 0,0,-1 },
    //{ 0,-1,0 },
    //{ -1,0,0 }, // 6

    //{ 0,0.70711,0.70711 },
    //{ 0,-0.70711,0.70711 },
    //{ 0,0.70711,-0.70711 },
    //{ 0,-0.70711,-0.70711 },    // 4
    
    //{ 0.70711,0,0.70711 },
    //{ -0.70711,0,0.70711 },
    //{ 0.70711,0,-0.70711 },
    //{ -0.70711,0,-0.70711 },    // 4

    //{ 0.70711,0.70711,0 },
    //{ -0.70711,0.70711,0 },
    //{ 0.70711,-0.70711,0 },
    //{ -0.70711,-0.70711,0 },    // 4

//};


void grenade_explode(float x, float y, float z)
{
    if (Options::animation_level <= 0) return;
    int n = randrange(25,35);
    float scale = ((float)Options::animation_level)/3.0f;
    n = scale*((float)n);

    float vx = 20.0f;
    float vy = 20.0f;
    float vz = 20.0f;

    float cx,cy,cz;
    float cvx,cvy,cvz;

    Particle::Shrapnel *s;
    for (int i=0; i<n; i++)
    {
        cx = x + ((randf() - 0.5f) / 20.0f);
        cy = y + ((randf() - 0.5f) / 20.0f);
        cz = z + ((randf() - 0.5f) / 20.0f);
        cvx = vx * (randf() - 0.5f);
        cvy = vy * (randf() - 0.5f);
        cvz = vz * (randf() - 0.5f);

        cx = translate_point(cx);
        cy = translate_point(cy);

        s = Particle::create_shrapnel(cx, cy, cz, cvx, cvy, cvz);
        if (s == NULL) return;
        s->ttl = randrange(15,25);
    }
}

void terrain_sparks(float x, float y, float z)
{
    if (Options::animation_level <= 0) return;
    int n = randrange(15,20);
    float scale = ((float)Options::animation_level)/3.0f;
    n = scale*((float)n);

    float vx = 3.5f;
    float vy = 3.5f;
    float vz = 3.5f;

    float cx,cy,cz;
    float cvx,cvy,cvz;

    Particle::Shrapnel *s;
    for (int i=0; i<n; i++)
    {
        cx = x + ((randf() - 0.5f) / 20.0f);
        cy = y + ((randf() - 0.5f) / 20.0f);
        cz = z + ((randf() - 0.5f) / 20.0f);
        cvx = vx * (randf() - 0.5f);
        cvy = vy * (randf() - 0.5f);
        cvz = vz * (randf() - 0.5f);

        cx = translate_point(cx);
        cy = translate_point(cy);

        s = Particle::create_shrapnel(cx, cy, cz, cvx, cvy, cvz);
        if (s==NULL) return;
        s->ttl = randrange(8,15);
        s->scale = 0.05f;
        s->texture_index = 54;
    }
}

void voxel_explode(Vec3 position, int count, float size, float force, struct Color color)
{
    if (Options::animation_level <= 0) return;

    float scale = ((float)Options::animation_level)/3.0f;
    count = scale*((float)count);
    
    float cx,cy,cz;
    float cvx,cvy,cvz;
    float theta, phi;
    float dtheta, dphi;

    Particle::ColoredMinivox* minivox;
    int ttl;
    for (int i=0; i<count; i++)
    {
        ttl = randrange(20,50);
        cx = position.x + ((randf() - 0.5f) / 20.0f);
        cy = position.y + ((randf() - 0.5f) / 20.0f);
        cz = position.z + ((randf() - 0.5f) / 20.0f);
        cvx = force * (randf() - 0.5f);
        cvy = force * (randf() - 0.5f);
        cvz = force * (randf() - 0.5f);

        theta = randf() * PI * 2;
        phi = randf() * PI * 2;
        dtheta = randf() * 0.01f;
        dphi = randf() * 0.01f;

        cx = translate_point(cx);
        cy = translate_point(cy);

        minivox = Particle::colored_minivox_list->create();
        if (minivox == NULL) return;
        minivox->set_color(color.r, color.g, color.b);//sky blue
        minivox->set_ttl(ttl);
        minivox->voxel.set_rotation_delta(dtheta, dphi);
        minivox->voxel.set_rotation(theta, phi);
        minivox->set_size(size);
        minivox->set_state(cx,cy,cz, cvx,cvy,cvz);
    }
}

// will generate random count between count_min and count_max
void voxel_explode(Vec3 position, int count_min, int count_max, float size, float force, struct Color color)
{
    int count = randrange(count_min, count_max);
    voxel_explode(position, count, size, force, color);
}

void agent_bleed(float x, float y, float z)
{
    if (Options::animation_level <= 0) return;

    int n = randrange(50,70);
    float scale = ((float)Options::animation_level)/3.0f;
    n = scale*((float)n);

    const float momentum = 1.0f;
    const float _vx = momentum,
                  _vy = momentum,
                  _vz = momentum;

    float nx,ny,nz;
    float vx,vy,vz;

    int ttl;
    Particle::Blood *b;
    for (int i=0; i<n; i++)
    {
        nx = x + randf() -0.5f;
        ny = y + randf() -0.5f;
        nz = z + randf() -0.5f;

        vx = _vx*(randf() -0.5f);
        vy = _vy*(randf() -0.5f);
        vz = _vz*(randf() -0.5f);

        nx = translate_point(nx);
        ny = translate_point(ny);

        b = Particle::blood_list->create();
        if (b==NULL) return;
        b->set_state(nx,ny,nz, vx,vy,vz);
        ttl = randrange(0,10) - 5;
        b->set_ttl(b->ttl + ttl);
    }
}

void blood_spray(float x, float y, float z, float ix, float iy, float iz)  // pos, incident vector
{
    if (Options::animation_level <= 0) return;

    int n = randrange(140,170);
    float scale = ((float)Options::animation_level)/3.0f;
    n = scale*((float)n);
    
    float len = sqrtf(ix*ix + iy*iy + iz*iz);
    ix /= len;
    iy /= len;
    iz /= len;

    float theta,phi,gamma;

    struct Vec3 iv = vec3_init(ix,iy,iz);
    struct Vec3 v;
    const float base_speed = 1.0f;
    float speed;
    const float arc = 48.0f;
    Particle::Blood *b;
    int ttl;
    for (int i=0; i<n; i++)
    {
        theta = randf() * 3.14159f * 2;
        phi = randf() * 3.14159f * 2;
        gamma = randf() * 3.14159f * 2;

        v = vec3_euler_rotation(iv, theta/arc, phi/arc, gamma/arc);
        normalize_vector(&v);

        speed = (randf() + 0.5f) * randrange(1,2);
        speed *= base_speed;
        b = Particle::blood_list->create();
        if (b == NULL) return;
        b->set_state(x,y,z, v.x*speed, v.y*speed, v.z*speed);
        ttl = randrange(0,10) - 5;
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
        minivox->set_color(color_init(randrange(0,255), randrange(0,255), randrange(0,255)));
        minivox->set_ttl(100);
        minivox->voxel.set_rotation_delta(0.1f, 0.0f);
        minivox->voxel.set_rotation(randf(), randf());
        minivox->set_size(0.1f);
        minivox->set_state(p.x, p.y, p.z, f.x, f.y, f.z);

    }
}

// new style callback

void create_shrapnel(int animation_id, void* metadata)
{
    if (Options::animation_level <= 0) return;

    GS_ASSERT(metadata != NULL);
    if (metadata == NULL) return;
    class AnimationStateMetadata* data = (class AnimationStateMetadata*)metadata;

    class AnimationData* anim_data = get_animation_data(animation_id);
    GS_ASSERT(anim_data != NULL);

    int n = anim_data->count;
    if (anim_data->use_rand_range)
        n = randrange(anim_data->count, anim_data->max_count);
        
    const float scale = ((float)Options::animation_level)/3.0f;
    n = scale*((float)n);

    struct Vec3 p = data->position;
    struct Vec3 v = data->velocity;

    const float jitter = anim_data->jitter_scale;

    float cx,cy,cz;
    float cvx,cvy,cvz;
    Particle::Shrapnel *s;
    for (int i=0; i<n; i++)
    {
        cx = p.x + ((randf() - 0.5f) * jitter);
        cy = p.y + ((randf() - 0.5f) * jitter);
        cz = p.z + ((randf() - 0.5f) * jitter);
        cvx = v.x * (randf() - 0.5f);
        cvy = v.y * (randf() - 0.5f);
        cvz = v.z * (randf() - 0.5f);

        cx = translate_point(cx);
        cy = translate_point(cy);

        s = Particle::create_shrapnel(cx, cy, cz, cvx, cvy, cvz);
        if (s == NULL) return;
        s->ttl = randrange(anim_data->ttl_min, anim_data->ttl_max);
    }

}

}   // Animations
