#include "animations.hpp"

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
void block_damage(float x, float y, float z, int cube_id, int *side) {

    // determine which side was hit
    int cube_side = 0;
          if (side[0] ==  1) cube_side = 2;
    else if (side[0] == -1) cube_side = 3;
    else if (side[1] ==  1) cube_side = 4;
    else if (side[1] == -1) cube_side = 5;
    else if (side[2] ==  1) cube_side = 0;
    else if (side[2] == -1) cube_side = 1;

    int tex_id = _get_cube_side_texture(cube_id, cube_side);

    // compute initial base velocities
    const float base_v = 1.0f;
    float _vx = base_v*side[0],
          _vy = base_v*side[1],
          _vz = base_v*side[2];

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
        nx = x + ((randf() -0.5f)*0.2f*side[0]);
        ny = y + ((randf() -0.5f)*0.2f*side[1]);
        nz = z + ((randf() -0.5f)*0.2f*side[2]);
        
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

}
