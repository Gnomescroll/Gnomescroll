#include "animations.hpp"

namespace Animations {

void block_crumble(float x, float y, float z, int n, int cube_id) {

    const float crumble_size = 0.17f;
    ClientState::minivox_list.set_size(crumble_size);

    const float _vx = 10.0f,
                  _vy = 10.0f,
                  _vz = 10.0f;

    unsigned char r,g,b,a;
    int side, ttl;
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
        get_random_pixel(cube_id, side, &r, &g, &b, &a);
        minivox = ClientState::minivox_list.create(nx,ny,nz, vx,vy,vz);
        minivox->set_color(r,g,b);
        minivox->set_ttl(ttl);
    }

    ClientState::minivox_list.unset_size();
}

void grenade_explode(float x, float y, float z) {
    int n = randrange(8,13);

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
