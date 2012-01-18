#include "animations.hpp"

#include <c_lib/particles/particle_lib.hpp>
#include <c_lib/common/random.h>
#include <c_lib/state/client_state.hpp>

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

        tex_id = _get_cube_side_texture(cube_id, side);
        minivox = ClientState::minivox_list.create(nx,ny,nz, vx,vy,vz);
        //minivox->set_color(r,g,b);
        minivox->set_ttl(ttl);
        minivox->set_texture(tex_id);
    }

    ClientState::minivox_list.unset_size();
}

// surface block dmg
void block_damage(float x, float y, float z, float ix, float iy, float iz, int cube_id, int *side) {

    // determine which side was hit
    int cube_side = 0;
          if (side[0] ==  1) cube_side = 2;
    else if (side[0] == -1) cube_side = 3;
    else if (side[1] ==  1) cube_side = 4;
    else if (side[1] == -1) cube_side = 5;
    else if (side[2] ==  1) cube_side = 0;
    else if (side[2] == -1) cube_side = 1;

    int tex_id = _get_cube_side_texture(cube_id, cube_side);

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
        ClientState::shrapnel_list.create(cx, cy, cz, cvx, cvy, cvz);
    }
}

void slime_melt(float x, float y, float z) {

    ClientState::minivox_list.set_size(0.7f);
    
     int n = randrange(50,100);
     float vx,vy,vz;
     vx=vy=vz=20.0f;
    
    float cx,cy,cz;
    float cvx,cvy,cvz;

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
        
        minivox = ClientState::minivox_list.create(cx,cy,cz, cvx,cvy,cvz);
        minivox->set_color(128,223,31);
        //minivox->set_size(0.7);
        minivox->set_ttl(ttl);
    }

    ClientState::minivox_list.unset_size();
}

}
