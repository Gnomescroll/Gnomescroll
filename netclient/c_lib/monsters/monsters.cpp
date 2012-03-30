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
    if (this->vox != NULL)
        delete this->vox;

    #if DC_CLIENT
    Animations::slime_melt(this->x, this->y, this->z);
    #endif
    
    #if DC_SERVER
    DestroySlime_StoC msg;
    msg.id = this->id;
    msg.broadcast();

    const float mom = 5.0f;
    float p = randf();
    Object_types obj_type;
    if (p < 1.0f/3.0f)
        obj_type = OBJ_TYPE_LASER_REFILL;
    else if (p < 2.0f/3.0f)
        obj_type = OBJ_TYPE_GRENADE_REFILL;
    else
        return;
    ObjectPolicyInterface* obj = ServerState::object_list->create(x,y,z+1.0f, (randf()-0.5f)*mom, (randf()-0.5f)*mom, mom, obj_type);
    obj->born();
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
    this->health = (this->health < 0) ? 0 : this->health;
    return this->health;
}

void Slime::tick() {

    if (health <= 0)
    {
        STATE::slime_list->destroy(this->id);
        return;
    }

    #if DC_SERVER
    tick_num++;
    if (tick_num == 30)
    {
        tick_num = 0;
        changed = true;
    }
    if (changed)
    {
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
        changed = false;
    }
    #endif
    
    // find nearby players
    // if nearby, move toward it

    const float r = 15.0f;
    const float speed = 0.25f;

    if (this->vox == NULL) return;
    // update nearby agents
    STATE::agent_list->objects_within_sphere(this->x, this->y, this->z, r);
    int n_nearby = STATE::agent_list->n_filtered;
    if (n_nearby == 0) return;

    // check if any agent in explode radius
    int i = 0;
    Agent_state* agent = STATE::agent_list->filtered_objects[i++];
    while (agent->status.team == 0) // skip viewer agents
        agent = STATE::agent_list->filtered_objects[i++];
    float dist = STATE::agent_list->filtered_object_distances[i-1];
    if (dist < this->vox->largest_radius()*0.5f)
    {
        agent = STATE::agent_list->filtered_objects[0];
        const int slime_dmg = 20; // TODO
        // blow up, damage player
        agent->status.apply_damage(slime_dmg, this->id, this->type);
        this->health = 0;
    }
    
    // target random nearby player
    i = randrange(0,n_nearby-1);
    agent = STATE::agent_list->filtered_objects[i];
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
}

void Slime_list::update()
{
    Slime *s;
    for (int i=0; i<this->n_max; i++)
    {
        s = this->a[i];
        if (s == NULL) continue;
        if (s->vox == NULL) continue;
        
        s->vox->was_updated = false;
        #if DC_CLIENT
        if (current_camera == NULL || !current_camera->in_view(s->x, s->y, s->z))
        {
            s->vox->set_draw(false);
            s->vox->set_hitscan(false);
            continue;
        }
        s->vox->set_draw(true);
        #endif
        
        s->vox->set_hitscan(true);
        s->vox->update(s->x, s->y, s->z, s->theta, s->phi);
    }
}

void Slime_list::tick()
{
    for (int i=0; i<this->n_max; i++)
        if (this->a[i] != NULL)
            this->a[i]->tick();
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

void Slime_list::send_to_client(int client_id)
{
    for (int i=0; i<n_max; i++)
    {
        Slime* s = this->a[i];
        if (s==NULL) continue;
        CreateSlime_StoC msg;
        msg.id = s->id;
        msg.sendToClient(client_id);
    }
}

void test(int n)
{
    int x,y,z;
    for (int i=0; i<n; i++)
    {
        x = randrange(0,map_dim.x-1);
        y = randrange(0,map_dim.y-1);
        z = _get_highest_open_block(x,y);
        
        #if DC_CLIENT
        z = map_dim.z-1;
        STATE::slime_list->create(x+0.5,y+0.5,z, 0,0,0);
        #endif

        #if DC_SERVER
        Slime* s = STATE::slime_list->create(x+0.5,y+0.5,z, 0,0,0);
        if (s==NULL) return;
        CreateSlime_StoC msg;
        msg.id = s->id;
        msg.broadcast();
        #endif
    }
}

void populate_slimes(int n_max)
{   // regenerates slimes up to a maximum
    int n_slimes = STATE::slime_list->num;
    n_max -= n_slimes;
    if (n_max > 0)
        test(n_max);
}

}
