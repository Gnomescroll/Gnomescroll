#include "grenade.hpp"

#if DC_CLIENT
#include <animations/animations.hpp>
#include <sound/triggers.hpp>
#endif

#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

#include <entity/constants.hpp>
#include <common/random.hpp>

#include <particle/_interface.hpp>

/* properties */


namespace Particle
{


const int GRENADE_TTL = 100;
const float GRENADE_DAMP = 0.5f;
const int GRENADE_TYPE = 1;
const int GRENADE_TEXTURE_ID = 5;
const float GRENADE_TEXTURE_SCALE = 1.0f;
const int GRENADE_BOUNCE_EXPLODE_LIMIT = 2;
const float GRENADE_MASS = 0.5f;

class grenade_StoC: public FixedSizeReliableNetPacketToClient<grenade_StoC>
{
    public:

        struct Vec3 p;
        struct Vec3 m;
        uint8_t owner;
        uint16_t ttl_max;
        uint8_t type;

    inline void packet(char* buff, unsigned int* buff_n, bool pack) 
    {
        pack_vec3(&p, buff, buff_n, pack);
        pack_vec3(&m, buff, buff_n, pack);
        pack_u8(&owner, buff, buff_n, pack);
        pack_u16(&ttl_max, buff, buff_n, pack);
        pack_u8(&type, buff, buff_n, pack);
    }
    inline void handle();
};

inline void grenade_StoC::handle()
{
    #if DC_CLIENT
    if (owner == ClientState::playerAgent_state.agent_id) return;
    Grenade* g = Particle::grenade_list->create();
    if (g == NULL) return;
    g->set_state(p, m);
    g->owner = (AgentID)this->owner;
    g->ttl_max = (int)ttl_max;
    g->type = (int)type;
    #endif
}

void Grenade::reset()
{
    this->ttl_max = GRENADE_TTL;
    this->type = GRENADE_TYPE;
    this->verlet.dampening = GRENADE_DAMP;
    this->scale = GRENADE_TEXTURE_SCALE;
    #if DC_CLIENT
    this->texture_index = GRENADE_TEXTURE_ID;
    #endif
}

Grenade::Grenade(int id) :
    ParticleMotion(id, 0,0,0,0,0,0, GRENADE_MASS),
    bounce_count(0),
    owner(NULL_AGENT)
{
    this->reset();
}

Grenade::~Grenade()
{
    #if DC_CLIENT
    Vec3 position = this->get_position();
    Sound::plasma_grenade_explode(position.x, position.y, position.z, 0,0,0);
    #endif
}

#if DC_SERVER
void Grenade::broadcast()
{
    grenade_StoC msg;
    msg.p = this->get_position();
    msg.m = this->get_momentum();
    msg.ttl_max = this->ttl_max;
    msg.owner = this->owner;
    msg.type = this->type;
    msg.broadcast();
}
#endif

void Grenade::tick()
{

    bool bounced = this->verlet.bounce();
    if (bounced)
    {
        this->bounce_count++;
        #if DC_CLIENT
        Vec3 position = this->get_position();
        Sound::grenade_bounce(position.x, position.y, position.z, 0,0,0);
        #endif
    }
    if (this->bounce_count >= GRENADE_BOUNCE_EXPLODE_LIMIT)
        this->ttl = this->ttl_max;
    this->ttl++;
}

void Grenade::explode(int multiplier)
{
    Vec3 position = this->get_position();
    #if DC_CLIENT
    const int ANIM_CT_MAX = 6;
    int anim_ct = Options::animation_level * multiplier;
    if (ANIM_CT_MAX > 6) anim_ct = ANIM_CT_MAX;
    for (int i=0; i<anim_ct; i++)
        Animations::grenade_explode(position.x, position.y, position.z);
    #endif

    #if DC_SERVER
    // this has to be called before damage_blocks(), unless you want the blast to go through blocks AND hit players newly exposed
    ServerState::damage_objects_within_sphere(
        position, GRENADE_DAMAGE_RADIUS, GRENADE_SPLASH_DAMAGE,
        this->owner, OBJECT_GRENADE, this->id
    );

    // apply block damage
    damage_blocks(multiplier);
    #endif
}

inline int Grenade::block_damage(int dist)
{
    int max_dist = GRENADE_BLOCK_DESTROY_RADIUS*3;
    if (dist >= max_dist) return 0;
    float ratio = (float)(max_dist - dist) / (float)(max_dist);
    float dmg = ratio * (float)(GRENADE_BLOCK_DAMAGE);
    int idmg = (int)(dmg);
    return idmg;
}

void Grenade::damage_blocks(int multiplier)
{
    #if DC_SERVER
    const TerrainModificationAction action = TMA_GRENADE;
    const int ir = GRENADE_BLOCK_DESTROY_RADIUS;

    Vec3 position = this->get_position();
    int mx = position.x;
    int my = position.y;
    int mz = position.z;
    
    for (int i=mx-ir; i<mx+ir; i++)
    for (int j=my-ir; j<my+ir; j++)
    for (int k=mz-ir; k<mz+ir; k++)
    {
        if (k <= 0 || k >= t_map::map_dim.z) continue;

        int x = translate_point(i);
        int y = translate_point(j);
        int z = k;

        int dmg = this->block_damage(abs(x-mx)+abs(y-my)+abs(z-mz));
        if (dmg <= 0) continue;

        t_map::apply_damage_broadcast(x,y,z, dmg*multiplier, action);
    }
    #endif
}

/* Grenade list */
void GrenadeList::tick()
{
    if (this->ct <= 0) return;
    for (unsigned int i=0; i<this->max; i++)
    {
        if (this->objects[i].id == -1) continue;
        this->objects[i].tick();
        if(this->objects[i].ttl >= this->objects[i].ttl_max)
        {
            this->objects[i].explode();
            this->destroy(this->objects[i].id);
        }
    }
}

void GrenadeList::draw()
{
    #if DC_CLIENT
    if (this->ct <= 0) return;
    for (unsigned int i=0; i<this->max; i++)
        if (this->objects[i].id != -1)
            this->objects[i].draw(this->objects[i].get_position());
    #endif
}

}   // Particle
