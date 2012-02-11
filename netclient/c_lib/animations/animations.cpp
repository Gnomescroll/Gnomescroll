#include "animations.hpp"

#include <c_lib/particles/particle_lib.hpp>
#include <c_lib/common/random.h>
#include <c_lib/state/client_state.hpp>

//#include <c_lib/physics/matrix.hpp>

#include <c_lib/physics/vec3.hpp>
#include <c_lib/physics/mat3.hpp>


namespace Animations {

void block_crumble(float x, float y, float z, int n, int cube_id) {

    const float crumble_size = 0.17f;
    ClientState::minivox_list.set_size(crumble_size);

    const float _vx = 10.0f,
                  _vy = 10.0f,
                  _vz = 10.0f;

    //unsigned char r,g,b,a;
    int side, ttl;
    int tex_id;
    float nx,ny,nz;
    float vx,vy,vz;

    Minivox* minivox;
    
    int i;
    for (i=0; i < n; i++) {

        nx = x + randf() -0.5f;
        ny = y + randf() -0.5f;
        nz = z + randf() -0.5f;

        vx = _vx*(randf() -0.5f);
        vy = _vy*(randf() -0.5f);
        vz = _vz*(randf() -0.5f);

        side = randrange(0,5);
        ttl = randrange(20,40);
        //get_random_pixel(cube_id, side, &r, &g, &b, &a);

        tex_id = t_map::get_cube_side_texture(cube_id, side);
        minivox = ClientState::minivox_list.create(nx,ny,nz, vx,vy,vz);
        if (minivox == NULL) return;
        //minivox->set_color(r,g,b);
        minivox->set_ttl(ttl);
        minivox->set_texture(tex_id);
    }

    ClientState::minivox_list.unset_size();
}

// surface block dmg
// takes encoded side int and translates to side[3];
void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_id, int cube_side) {
    int side[3];
    get_side_array_from_cube_side(cube_side, side);
    block_damage(x,y,z,ix,iy,iz,cube_id,side, cube_side);
}

// surface block dmg    
void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_id, int *side) {
    int cube_side = get_cube_side_from_side_array(side);
    block_damage(x,y,z,ix,iy,iz,cube_id, side, cube_side);
}

void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_id, int *side, int cube_side) {
    //printf("block damage: %0.2f %0.2f %0.2f %0.2f %0.2f %0.2f cube_id=%d, cube_side=%d\n", x,y,z,ix,iy,iz, cube_id, cube_side);

    int tex_id = t_map::get_cube_side_texture(cube_id, cube_side);

    // reflection bias
    float ref[3];
    float inc[3];   inc[0]=ix;inc[1]=iy;inc[2]=iz;
    float nor[3];   nor[0]=(float)side[0];nor[1]=(float)side[1];nor[2]=(float)side[2];
    reflect(inc, nor, ref);
    normalize_vector_f(&ref[0], &ref[1], &ref[2]);

    // compute initial base velocities
    const float base_v = 2.5f;
    float _vx = base_v*ref[0],
          _vy = base_v*ref[1],
          _vz = base_v*ref[2];

    // "invert" the normal for perturbing the initial positions along the plane
    side[0] = (side[0]) ? 0 : 1;
    side[1] = (side[1]) ? 0 : 1;
    side[2] = (side[2]) ? 0 : 1;

    Minivox* minivox;

    float vx,vy,vz;
    float nx,ny,nz;

    int i;
    int n = randrange(5,10);
    int ttl;
    for (i=0; i<n; i++) {

        ttl = randrange(15,25);

        vx = _vx + (randf() -0.5f);
        vy = _vy + (randf() -0.5f);
        vz = _vz + (randf() -0.5f);

        // perturb the initial positions along the side's plane
        nx = x + ((randf() -0.5f)*0.4f*side[0]);
        ny = y + ((randf() -0.5f)*0.4f*side[1]);
        nz = z + ((randf() -0.5f)*0.4f*side[2]);
        
        minivox = ClientState::minivox_list.create(nx,ny,nz, vx,vy,vz);
        if (minivox == NULL) return;
        minivox->set_texture(tex_id, 2);
        minivox->set_ttl(ttl);
    }
}

void grenade_explode(float x, float y, float z) {
    int n = randrange(15,25);

    float vx = 20.0f;
    float vy = 20.0f;
    float vz = 20.0f;

    float cx,cy,cz;
    float cvx,cvy,cvz;

    int i;
    for (i=0; i<n; i++) {
        cx = x + ((randf() - 0.5f) / 20.0f);
        cy = y + ((randf() - 0.5f) / 20.0f);
        cz = z + ((randf() - 0.5f) / 20.0f);
        cvx = vx * (randf() - 0.5f);
        cvy = vy * (randf() - 0.5f);
        cvz = vz * (randf() - 0.5f);
        if (ClientState::shrapnel_list.create(cx, cy, cz, cvx, cvy, cvz) == NULL) return;
    }
}

void slime_melt(float x, float y, float z) {

    ClientState::minivox_list.set_size(0.7f);
    
     int n = randrange(50,100);
     float vx,vy,vz;
     vx=vy=vz=20.0f;
    
    float cx,cy,cz;
    float cvx,cvy,cvz;
    float theta, phi;
    float dtheta, dphi;

    Minivox* minivox;
    int ttl;
    
    int i;
    for (i=0; i<n; i++) {
        ttl = randrange(50,80);
        cx = x + ((randf() - 0.5f) / 20.0f);
        cy = y + ((randf() - 0.5f) / 20.0f);
        cz = z + ((randf() - 0.5f) / 20.0f);
        cvx = vx * (randf() - 0.5f);
        cvy = vy * (randf() - 0.5f);
        cvz = vz * (randf() - 0.5f);

        theta = randf() * PI * 2;
        phi = randf() * PI * 2;
        dtheta = randf() * 0.01f;
        dphi = randf() * 0.01f;
        
        minivox = ClientState::minivox_list.create(cx,cy,cz, cvx,cvy,cvz);
        if (minivox == NULL) return;
        minivox->set_color(128,223,31);
        //minivox->set_size(0.7);
        minivox->set_ttl(ttl);
        minivox->set_spin(dtheta, dphi);
        minivox->set_angles(theta, phi);
    }

    ClientState::minivox_list.unset_size();
}

void agent_bleed(float x, float y, float z)
{
    const float _vx = 10.0f,
                  _vy = 10.0f,
                  _vz = 10.0f;

    //int ttl;
    float nx,ny,nz;
    float vx,vy,vz;

    //Blood* blood;

    int n = randrange(50,70);
    for (int i=0; i<n; i++) {

        nx = x + randf() -0.5f;
        ny = y + randf() -0.5f;
        nz = z + randf() -0.5f;

        vx = _vx*(randf() -0.5f);
        vy = _vy*(randf() -0.5f);
        vz = _vz*(randf() -0.5f);

        //ttl = randrange(10,25);

        //blood = ClientState::blood_list.create(nx,ny,nz, vx,vy,vz);
        //if (blood == NULL) return;
        if (ClientState::blood_list.create(nx,ny,nz, vx,vy,vz) == NULL) return;
        //blood->ttl = ttl;
    }
}

void blood_spray(float x, float y, float z, float ix, float iy, float iz)  // pos, incident vector
{

    float len = sqrt(ix*ix + iy*iy + iz*iz);
    ix /= len;
    iy /= len;
    iz /= len;

    float theta,phi,gamma;

    struct Vec3 iv = vec3_init(ix,iy,iz);
    struct Vec3 v;
    int n = randrange(200,250);
    const float base_speed = 10.0f;
    float speed;
    const float arc = 48.0f;
    for (int i=0; i<n; i++)
    {
        theta = randf() * 3.14159 * 2;
        phi = randf() * 3.14159 * 2;
        gamma = randf() * 3.14159 * 2;

        v = vec3_euler_rotation(iv, theta/arc, phi/arc, gamma/arc);

        speed = (randf() + 0.5) * randrange(0,2);
        speed *= base_speed;
        if (ClientState::blood_list.create(x,y,z, v.x*speed, v.y*speed, v.z*speed) == NULL) return;
    }
}

void animations_tick()
{
    ClientState::ctf.animate_flags();
    ClientState::hitscan_effect_list.tick();
    ClientState::hitscan_laser_effect_list.tick();
}

void animations_draw()
{
    ClientState::hitscan_effect_list.draw();
    ClientState::hitscan_laser_effect_list.draw();
}

}
