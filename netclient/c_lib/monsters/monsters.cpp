#include "monsters.hpp"

#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>
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

void Slime::tick() {
    // find nearby players
    // if nearby, move toward it

    const float r = 10.0f;
    const float speed = 0.2f;

    // choose a random agent in range
    int n_nearby = STATE::agent_list.agents_within_sphere(this->x, this->y, this->z, r);
    if (n_nearby == 0) return;
    int i = randrange(0,n_nearby-1);
    Agent_state* agent = STATE::agent_list.filtered_agents[i];
    if (agent == NULL) return;
    
    // determine velocity tick
    float a,b,c;
    a = agent->s.x - this->x;
    b = agent->s.y - this->y;
    c = agent->s.z - this->z;
    float len = sqrt(a*a + b*b + c*c);
    a /= len;
    b /= len;
    c /= len;
    a *= speed;
    b *= speed;
    c *= speed;

    // apply velocity
    this->x += a;
    this->y += b;
    this->z += c;
}

void Slime_list::update() {
    int i;
    for (i=0; i<this->n_max; i++) {
        if (this->a[i] == NULL) continue;
        if (this->a[i]->vox == NULL) continue;
        this->a[i]->vox->update(&slime_vox_dat);
    }
}

void Slime_list::tick() {
    int i;
    for (i=0; i<this->n_max; i++) {
        if (this->a[i] == NULL) continue;
        this->a[i]->tick();
    }
}

//#ifdef DC_CLIENT
void test() {
    //int n = 30;
    int n = 1;
    int i;
    int x,y,z;
    for (i=0; i<n; i++) {
        x = randrange(0,127);
        y = randrange(0,127);
        z = _get_highest_open_block(x,y);
        Slime* s = STATE::slime_list.create(x,y,z+1, 0,0,0);
        s->vox->set_draw(true);
        s->vox->update(&slime_vox_dat);
    }
}
//#endif


}
