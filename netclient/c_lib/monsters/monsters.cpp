#include "monsters.hpp"

#include <math.h>

#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>
#include <c_lib/common/random.h>
#include <c_lib/t_map/t_map.hpp>

namespace Monsters {

VoxDat slime_vox_dat;

Slime::Slime(int id)
:
id(id),
theta(0.0), phi(0.0),
type(OBJ_TYPE_SLIME)
#ifdef DC_SERVER
, changed(true),tick_num(0)
#endif
{
    this->init_vox();
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
    printf("Slime::Slime(float x, float y, float z, float vx, float vy, float vz), DEPRECATED, use vox_dat\n");
    //this->vox = new Voxel_model(SLIME_PART_NUM);
}
Slime::Slime(int id, float x, float y, float z, float vx, float vy, float vz)
:
id(id), x(x), y(y), z(z), vx(vx), vy(vy), vz(vz),
theta(0.0), phi(0.0),
health(SLIME_HEALTH),
type(OBJ_TYPE_SLIME)
#ifdef DC_SERVER
, changed(true),tick_num(0)
#endif
{
    this->init_vox();
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

void Slime::init_vox()
{
    this->vox = new Voxel_model(&slime_vox_dat, this->id, this->type);
    this->vox->set_hitscan(true);
    this->vox->register_hitscan();
}

int Slime::take_damage(int dmg)
{
    this->health -= dmg;
    if (this->health <= 0)
    {
        STATE::slime_list.destroy(this->id);
    }
    this->health = (this->health < 0) ? 0 : this->health;
    return this->health;
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
    int closest = STATE::agent_list.objects_within_sphere(this->x, this->y, this->z, r);
    int n_nearby = STATE::agent_list.n_filtered;
    if (n_nearby == 0) return;

    Agent_state* agent;
    if (closest >= 0 && this->vox != NULL)
    {
        float min_dist = STATE::agent_list.filtered_object_distances[closest];
        if (min_dist < this->vox->largest_radius()) {
            // damage and die if within range
            const int slime_dmg = 20; // TODO
            // blow up, damage player
            agent = STATE::agent_list.filtered_objects[closest];
            if (agent != NULL) {
                agent->status.apply_damage(slime_dmg, this->id, this->type);
                STATE::slime_list.destroy(this->id);    // die
                return;
            }
        }
    }

    // choose random player to target
    int i = randrange(0,n_nearby-1);
    agent = STATE::agent_list.filtered_objects[i];
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
        this->a[i]->vox->update(&slime_vox_dat, this->a[i]->x, this->a[i]->y, this->a[i]->z, this->a[i]->theta, this->a[i]->phi);
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

void test(int n) {
    int i;
    int x,y,z;
    for (i=0; i<n; i++) {
        x = randrange(0,127);
        y = randrange(0,127);
        z = _get_highest_open_block(x,y);
        
        #ifdef DC_CLIENT
        z = 127;
        STATE::slime_list.create(x+0.5,y+0.5,z, 0,0,0);
        #endif

        #ifdef DC_SERVER
        Slime* s = STATE::slime_list.create(x+0.5,y+0.5,z, 0,0,0);
        if (s==NULL) return;
        CreateSlime_StoC msg = CreateSlime_StoC();
        msg.id = s->id;
        msg.broadcast();
        #endif

    }
}

}
