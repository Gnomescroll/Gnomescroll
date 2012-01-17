#include "monsters.hpp"

#include <c_lib/state/client_state.hpp>
#include <c_lib/common/random.h>
#include <c_lib/t_map/t_map.hpp>

namespace Monsters {

VoxBody slime_vox_dat;

Slime::Slime()
:
theta(0.0f), phi(0.0f),
type(OBJ_TYPE_SLIME)
{
    this->vox = new Slime_vox(this, &slime_vox_dat);
}
Slime::Slime(int id)
:
id(id),
theta(0.0f), phi(0.0f),
type(OBJ_TYPE_SLIME)
{
    this->vox = new Slime_vox(this, &slime_vox_dat);
}
Slime::Slime(float x, float y, float z, float vx, float vy, float vz)
:
x(x), y(y), z(z), vx(vx), vy(vy), vz(vz),
theta(0.0f), phi(0.0f),
type(OBJ_TYPE_SLIME)
{
    this->vox = new Slime_vox(this, &slime_vox_dat);
}
Slime::Slime(int id, float x, float y, float z, float vx, float vy, float vz)
:
id(id), x(x), y(y), z(z), vx(vx), vy(vy), vz(vz),
theta(0.0f), phi(0.0f),
type(OBJ_TYPE_SLIME)
{
    this->vox = new Slime_vox(this, &slime_vox_dat);
}

Slime::~Slime()
{
    if (this->vox != NULL) {
        delete this->vox;
    }
}

#ifdef DC_CLIENT
void test() {
    //int n = 30;
    int n = 1;
    int i;
    int x,y,z;
    for (i=0; i<n; i++) {
        x = randrange(0,127);
        y = randrange(0,127);
        z = _get_highest_open_block(x,y);
        Slime* s = ClientState::slime_list.create(x,y,z+1, 0,0,0);
        s->vox->set_draw(true);
        s->vox->update(&slime_vox_dat);
    }
}
#endif


}
