#include "grenade.hpp"

#ifdef DC_CLIENT
#include <compat_gl.h>
#include <c_lib/animations/animations.hpp>
#include <sound/triggers.hpp>
#endif

#include <ray_trace/ray_trace.hpp>
#include <t_map/t_map.hpp>
#include <t_map/t_properties.hpp>

#include <c_lib/state/client_state.hpp>
#include <c_lib/state/server_state.hpp>

#include <common/enum_types.hpp>

/*
 *  Networking; spawn packet from server to client
 */

#include <net_lib/net.hpp>

/* properties */

const int GRENADE_BOUNCE_EXPLODE_LIMIT = 2;
const float GRENADE_MASS = 0.5f;

class grenade_StoC: public FixedSizeNetPacketToClient<grenade_StoC>
{
    public:

        float x,y,z;
        float mx,my,mz; // send initial impulse, not velocity
        uint8_t owner;
        uint16_t ttl_max;
        uint8_t type;

        inline void packet(char* buff, int* buff_n, bool pack) 
        {
            pack_float(&x, buff, buff_n, pack);
            pack_float(&y, buff, buff_n, pack);
            pack_float(&z, buff, buff_n, pack);

            pack_float(&mx, buff, buff_n, pack);
            pack_float(&my, buff, buff_n, pack);
            pack_float(&mz, buff, buff_n, pack);

            pack_u8(&owner, buff, buff_n, pack);
            pack_u16(&ttl_max, buff, buff_n, pack);
            pack_u8(&type, buff, buff_n, pack);
        }
        inline void handle();
};


inline void grenade_StoC::handle() {
    #ifdef DC_CLIENT
    if (owner == ClientState::playerAgent_state.agent_id) return;
    Grenade* g = ClientState::grenade_list->create(x, y, z, mx, my, mz);
    g->owner = owner;
    g->ttl_max = (int)ttl_max;
    g->type = (int)type;
    #endif
}

Grenade::Grenade(int id)
:
Particle(id, 0,0,0,0,0,0, GRENADE_MASS),
bounce_count(0),
owner(-1)
{
    this->ttl_max = GRENADE_TTL;
    this->type = GRENADE_TYPE;
}

Grenade::Grenade(int id, float x, float y, float z, float mx, float my, float mz)
:
Particle(id, x,y,z,mx,my,mz, GRENADE_MASS),
bounce_count(0),
owner(-1)
{
    this->ttl_max = GRENADE_TTL;
    this->type = GRENADE_TYPE;
}

Grenade::~Grenade()
{
    #ifdef DC_CLIENT
    Sound::grenade_explode(
        this->vp->p.x, this->vp->p.y, this->vp->p.z,
        0,0,0
    );
    #endif
}

#ifdef DC_SERVER
void Grenade::broadcast()
{
    grenade_StoC msg;
    msg.x = this->vp->p.x;
    msg.y = this->vp->p.y;
    msg.z = this->vp->p.z;
    Vec3 mom = this->vp->get_momentum();
    msg.mx = mom.x;
    msg.my = mom.y;
    msg.mz = mom.z;
    msg.ttl_max = this->ttl_max;
    msg.owner = this->owner;
    msg.type = this->type;
    msg.broadcast();
}
#endif

void Grenade::tick() {

    bool bounced = Verlet::bounce(this->vp, GRENADE_DAMP);
    if (bounced)
    {
        bounce_count++;
        #ifdef DC_CLIENT
        Sound::grenade_bounce(
            this->vp->p.x,
            this->vp->p.y,
            this->vp->p.z,
            0,0,0
        );
        #endif
    }
    if (bounce_count==GRENADE_BOUNCE_EXPLODE_LIMIT)
        this->ttl = this->ttl_max;
    this->ttl++;
}

void Grenade::draw() {
#ifdef DC_CLIENT
    if (current_camera == NULL || !current_camera->in_view(this->vp->p.x, this->vp->p.y, this->vp->p.z)) return;


    float up[3] = {
        model_view_matrix[0]*GRENADE_TEXTURE_SCALE,
        model_view_matrix[4]*GRENADE_TEXTURE_SCALE,
        model_view_matrix[8]*GRENADE_TEXTURE_SCALE
    };
    float right[3] = {
        model_view_matrix[1]*GRENADE_TEXTURE_SCALE,
        model_view_matrix[5]*GRENADE_TEXTURE_SCALE,
        model_view_matrix[9]*GRENADE_TEXTURE_SCALE
    };

    float tx_min, tx_max, ty_min, ty_max;
    float x,y,z;

    tx_min = (float)(GRENADE_TEXTURE_ID%16)* (1.0/16.0);
    tx_max = tx_min + (1.0/16.0);
    ty_min = (float)(GRENADE_TEXTURE_ID/16)* (1.0/16.0);
    ty_max = ty_min + (1.0/16.0);

    x=this->vp->p.x; y=this->vp->p.y; z=this->vp->p.z;

    glTexCoord2f(tx_min,ty_max );
    glVertex3f(x+(-right[0]-up[0]), y+(-right[1]-up[1]), z+(-right[2]-up[2]));  // Bottom left

    glTexCoord2f(tx_min,ty_min );
    glVertex3f(x+(up[0]-right[0]), y+(up[1]-right[1]), z+(up[2]-right[2]));  // Top left

    glTexCoord2f(tx_max,ty_min);
    glVertex3f(x+(up[0]+right[0]), y+(up[1]+right[1]), z+(up[2]+right[2]));  // Top right

    glTexCoord2f(tx_max,ty_max );
    glVertex3f(x+(right[0]-up[0]), y+(right[1]-up[1]), z+(right[2]-up[2]));  // Bottom right
#endif    
}

void Grenade::explode()
{
#ifdef DC_CLIENT
    Animations::grenade_explode(this->vp->p.x, this->vp->p.y, this->vp->p.z);
#endif

#ifdef DC_SERVER
    // this has to be called before damage_blocks(), unless you want the blast to go through blocks AND hit players newly exposed
    ServerState::damage_objects_within_sphere(
        this->vp->p.x, this->vp->p.y, this->vp->p.z,
        GRENADE_DAMAGE_RADIUS,
        GRENADE_SPLASH_DAMAGE, this->owner, OBJ_TYPE_GRENADE
    );
    
    damage_blocks();
#endif
}

int Grenade::block_damage(int dist) {
    int max_dist = GRENADE_BLOCK_DESTROY_RADIUS*3;
    float ratio = (float)(max_dist - dist) / (float)(max_dist);
    float dmg = ratio * (float)(GRENADE_BLOCK_DAMAGE);
    int idmg = (int)(dmg);
    return idmg;
}

void Grenade::damage_blocks() {
#ifdef DC_SERVER
    const t_map::TerrainModificationAction action = t_map::TMA_GRENADE;
    using t_map::apply_damage_broadcast;
    
    float x = this->vp->p.x;
    float y = this->vp->p.y;
    float z = this->vp->p.z;

    int ir = GRENADE_BLOCK_DESTROY_RADIUS;

    int mx = (int)x;
    int my = (int)y;
    int mz = (int)z;
    
    int i,j,k;
    int bx,by,bz;
    int dmg=0;
    for (i=0; i<ir; i++)
        for (j=0; j<ir; j++)
            for (k=0; k<ir; k++)
            {
                dmg = block_damage(i+j+k);

                bx = mx + i;
                by = my + j;
                bz = mz + k;
                apply_damage_broadcast(bx,by,bz, dmg, action);
                bx = mx - i;
                apply_damage_broadcast(bx,by,bz, dmg, action);
                by = my - j;
                apply_damage_broadcast(bx,by,bz, dmg, action);
                by = my + j;
                bz = mz - k;
                apply_damage_broadcast(bx,by,bz, dmg, action);
                bx = mx + i;
                by = my - j;
                apply_damage_broadcast(bx,by,bz, dmg, action);
                bx = mx - i;
                apply_damage_broadcast(bx,by,bz, dmg, action);
            }
#endif
}

/* Grenade list */
void Grenade_list::tick() {
    int i;
    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->tick();

        if(a[i]->ttl >= a[i]->ttl_max) {
            a[i]->explode();
            destroy(a[i]->id);
            num--;
        }
    }
}

void Grenade_list::draw() {
#ifdef DC_CLIENT
    if(num == 0) { return; }

    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glEnable (GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBindTexture( GL_TEXTURE_2D, particle_texture_id );
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE);

    glBegin( GL_QUADS );

    int i;
    for(i=0; i<n_max; i++) {
        if (a[i] == NULL) continue;
        a[i]->draw();
    }

    glEnd();
    glDepthMask(GL_TRUE);
    glDisable(GL_TEXTURE_2D);
    glDisable (GL_DEPTH_TEST);
    glDisable(GL_BLEND);

#endif
}

inline void print_grenade(Grenade *g) {
    printf("Print Grenade -\n");
    printf("ID: %d\n", g->id);
    printf("Pos: %0.2f %0.2f %0.2f\n", g->vp->p.x, g->vp->p.y, g->vp->p.z);
    printf("Vel: %0.2f %0.2f %0.2f\n", g->vp->v.x, g->vp->v.y, g->vp->v.z);
    printf("TTL max: %d\n", g->ttl_max);
    printf("Type: %d\n", g->type);
}
