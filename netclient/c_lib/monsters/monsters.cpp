#include "monsters.hpp"

#include <math.h>

#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/common/random.h>
#include <c_lib/t_map/t_map.hpp>

namespace Monsters {

VoxBody slime_vox_dat;

Slime::Slime()
:
theta(0.0), phi(0.0),
type(OBJ_TYPE_SLIME)
#ifdef DC_SERVER
, changed(true),tick_num(0)
#endif
{
    this->vox = new Slime_vox(this, &slime_vox_dat);
}
Slime::Slime(int id)
:
id(id),
theta(0.0), phi(0.0),
type(OBJ_TYPE_SLIME)
#ifdef DC_SERVER
, changed(true),tick_num(0)
#endif
{
    #ifdef DC_CLIENT
    this->vox = new Slime_vox(this, &slime_vox_dat);
    #endif

    #ifdef DC_SERVER
    this->vox = NULL;
    #endif
}
Slime::Slime(float x, float y, float z, float vx, float vy, float vz)
:
x(x), y(y), z(z), vx(vx), vy(vy), vz(vz),
theta(0.0), phi(0.0),
type(OBJ_TYPE_SLIME)
#ifdef DC_SERVER
, changed(true),tick_num(0)
#endif
{
    #ifdef DC_CLIENT
    this->vox = new Slime_vox(this, &slime_vox_dat);
    #endif

    #ifdef DC_SERVER
    this->vox = NULL;
    #endif
}
Slime::Slime(int id, float x, float y, float z, float vx, float vy, float vz)
:
id(id), x(x), y(y), z(z), vx(vx), vy(vy), vz(vz),
theta(0.0), phi(0.0),
type(OBJ_TYPE_SLIME)
#ifdef DC_SERVER
, changed(true),tick_num(0)
#endif
{
    #ifdef DC_CLIENT
    this->vox = new Slime_vox(this, &slime_vox_dat);
    #endif

    #ifdef DC_SERVER
    this->vox = NULL;
    #endif
}

Slime::~Slime()
{
    if (this->vox != NULL) {
        delete this->vox;
    }
    #ifdef DC_CLIENT
    Animations::slime_melt(this->x, this->y, this->z);
    #endif
    
    #ifdef DC_SERVER
    DestroySlime_StoC msg = DestroySlime_StoC();
    msg.id = this->id;
    msg.broadcast();
    #endif
}

void Slime::tick() {
    #ifdef DC_CLIENT
    return;
    #endif

    #ifdef DC_SERVER
    if (changed) {
        SlimeState_StoC msg;
        msg.id = this->id;
        msg.seq = 0;
        msg.x = this->x;
        msg.y = this->y;
        msg.z = this->z;
        msg.vx = this->vx;
        msg.vy = this->vy;
        msg.vz = this->vz;
        msg.theta = this->theta;
        msg.phi = this->phi;
        msg.broadcast();
    }
    tick_num++;
    if (tick_num == 30) {
        changed = true;
    }
    tick_num %= 6;
    #endif
    
    // find nearby players
    // if nearby, move toward it

    const float r = 10.0f;
    const float speed = 0.1f;

    // choose a random agent in range
    int n_nearby = STATE::agent_list.agents_within_sphere(this->x, this->y, this->z, r);
    if (n_nearby == 0) return;

    // damage and die if within range
    int i;
    float min_dist = 10000.0f;
    int closest = -1;
    for (i=0; i<n_nearby; i++) {
        if (STATE::agent_list.filtered_agent_distances[i] < min_dist) {
            min_dist = STATE::agent_list.filtered_agent_distances[i];
            closest = STATE::agent_list.filtered_agents[i]->id;
        }
    }

    // slime sphere intersects agent sphere?
    const int slime_dmg = 20; // TODO
    Agent_state* agent;
    if (min_dist < this->vox->vv->radius && closest >= 0) { // get real radius
        // blow up, damage player
        agent = STATE::agent_list.get(closest);
        if (agent != NULL) {
            agent->status.apply_damage(slime_dmg, this->id, this->type);
            STATE::slime_list.destroy(this->id);    // die
            return;
        }
    }

    // choose random player to target
    i = randrange(0,n_nearby-1);
    agent = STATE::agent_list.filtered_agents[i];
    if (agent == NULL) return;
    
    // determine velocity tick
    float a,b,c;
    float vec[2];
    a = agent->s.x - this->x;
    b = agent->s.y - this->y;
    c = agent->s.z - this->z;
    float len = sqrt(a*a + b*b + c*c);
    a /= len;
    b /= len;
    c /= len;
    vec[0] = a;
    vec[1] = b;
    a *= speed;
    b *= speed;
    c *= speed;

    // apply velocity
    this->x += a;
    this->y += b;
    this->z += c;

    // calculate rotation deltas
    // THETA = acos( (A.B) / (|A|*|B|) )
    float dtheta;
    float dot, alen, blen;

    float ftmp[2], vtmp[2];
    ftmp[0] = 1.0;
    ftmp[1] = 0.0;
    vtmp[0] = vec[0];
    vtmp[1] = vec[1];

    // calculate theta
    dot = ftmp[0]*vtmp[0] + ftmp[1]*vtmp[1];
    alen = ftmp[0]*ftmp[0] + ftmp[1]*ftmp[1];
    blen = vtmp[0]*vtmp[0] + vtmp[1]*vtmp[1];
    dtheta = acos ( dot / sqrt(alen*blen) );

    // orient towards player
    this->theta = dtheta;

    #ifdef DC_SERVER
    changed = true;
    #endif

    //SlimeState_StoC msg;
    //msg.id = this->id;
    //msg.seq = 0;
    //msg.x = this->x;
    //msg.y = this->y;
    //msg.z = this->z;
    //msg.vx = this->vx;
    //msg.vy = this->vy;
    //msg.vz = this->vz;
    //msg.theta = this->theta;
    //msg.phi = this->phi;
    //msg.broadcast();
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

void Slime::set_state(float x, float y, float z, float vx, float vy, float vz)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->vx = vx;
    this->vy = vy;
    this->vz = vz;
}

void Slime::set_angles(float theta, float phi)
{
    this->theta = theta;
    this->phi = phi;
}

void Slime::set_state(float x, float y, float z, float vx, float vy, float vz, float theta, float phi)
{
    this->set_state(x,y,z,vx,vy,vz);
    this->set_angles(theta, phi);
}

void Slime_list::send_to_client(int client_id) {
    int i;
    Slime* s;
    for (i=0; i<n_max; i++) {
        s = this->a[i];
        if (s==NULL) continue;
        CreateSlime_StoC msg;
        msg.id = s->id;
        msg.sendToClient(client_id);
    }
}

void test() {
    int n = 1;
    int i;
    int x,y,z;
    for (i=0; i<n; i++) {
        x = randrange(0,127);
        y = randrange(0,127);
        z = _get_highest_open_block(x,y);
        
        #ifdef DC_CLIENT
        STATE::slime_list.create(x,y,z+1, 0,0,0);
        #endif
        //s->vox->set_draw(true);
        //s->vox->update(&slime_vox_dat);

        #ifdef DC_SERVER
        Slime* s = STATE::slime_list.create(x,y,z+1, 0,0,0);
        CreateSlime_StoC msg = CreateSlime_StoC();
        msg.id = s->id;
        msg.broadcast();
        #endif

    }
}

}
