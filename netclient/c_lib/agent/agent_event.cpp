#include "agent_event.hpp"

#ifdef DC_CLIENT
#include <c_lib/state/client_state.hpp>
#include <c_lib/animations/animations.hpp>
#include <c_lib/animations/hitscan.hpp>
#include <c_lib/sound/sound.hpp>

void Agent_event::fired_weapon(int type) {
    switch (type) {
        case Weapons::TYPE_block_applier:
            break;
        case Weapons::TYPE_block_pick:
            break;
        case Weapons::TYPE_grenade_thrower:
            break;
        case Weapons::TYPE_hitscan_laser:
            fired_laser();
            break;
        default:
            break;
    }
}

void Agent_event::fired_laser() {
    // raycast from current aiming direction to nearest block
    // get side of block hit, point of collision
    // send to animations

    int collision[3];
    int pre_collision[3];
    int side[3];
    int _cube=0,*cube=&_cube;
    const float max_l = 500.0f;
    float _distance=0.0f,*distance=&_distance;

    float f[3];
    a->s.forward_vector(f);

    float
        x = a->s.x,
        y = a->s.y,
        z = a->s.z + a->camera_height();

    // if not player agent
    if (a->id != ClientState::playerAgent_state.agent_id) { // only play for other agents
        // play sound
        char soundfile[] = "laser_01.wav";
        Sound::play_3d_sound(soundfile, x,y,z, a->s.vx, a->s.vy, a->s.vz);

        // play laser anim
        const float hitscan_speed = 200.0f;
        ClientState::hitscan_effect_list.create(
            x,y,z,
            f[0]*hitscan_speed, f[1]*hitscan_speed, f[2]*hitscan_speed
        );
    }

    int collided = _ray_cast6(x,y,z, f[0], f[1], f[2], max_l, distance, collision, pre_collision, cube, side);
    if (!collided) {
        return;
    }

    // pt of collision
    x += f[0] * _distance;
    y += f[1] * _distance;
    z += f[2] * _distance;

    Animations::block_damage(x,y,z, f[0], f[1], f[2], _cube, side);
}

void Agent_event::name_changed()
{
    if (this->bb != NULL)
    {
        this->bb->set_text(this->a->status.name);
    }
}

void Agent_event::hide_name()
{
    if (this->bb != NULL)
    {
        this->bb->set_draw(false);
    }
}

void Agent_event::display_name()
{
    if (this->a->status.dead) return;
    if (this->bb == NULL)
    {
        this->bb = ClientState::billboard_text_list.create(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        if (this->bb == NULL) return;
        this->bb->set_gravity(false);   // stay put
        this->bb->set_ttl(-1000);          // dont die
        this->bb->set_text(this->a->status.name);
        unsigned char r,g,b,a=255;
        ClientState::get_team_color(this->a->status.team, &r, &g, &b);
        this->bb->set_color(r,g,b,a);
        this->bb->projection_type = Billboard::HUD;
        this->bb->set_size(0.7);
    }
    this->bb->set_state(a->s.x, a->s.y, a->s.z + a->box.b_height, 0.0f, 0.0f, 0.0f);
    this->bb->set_draw(true);
}

// side effects of taking damage. dont modify health/death here
void Agent_event::took_damage(int dmg) {
    BillboardText* b = ClientState::billboard_text_list.create(a->s.x, a->s.y, a->s.z, 0.0f,0.0f, 7.0f);
    if (b==NULL) return;
    b->set_color(255,10,10, 255);   // red
    char txt[10+1];
    sprintf(txt, "%d", dmg);
    b->set_text(txt);
}

void Agent_event::died() {
    a->status.dead = true;
}

void Agent_event::born() {
    a->status.dead = false;
}

void Agent_event::life_changing(bool dead) {
    if (dead) died();
    else born();
}

void Agent_event::reload_weapon(int type) {
    if (! a->weapons.is_active(type)) return;
    // play reload animation/sound for the weapon
}

void Agent_event::joined_team(int team)
{
    this->a->status.team = team;
    this->a->vox->update_team_color(&agent_vox_dat, team);
}

void Agent_event::picked_up_flag()
{
    this->a->status.has_flag = true;
}

void Agent_event::dropped_flag()
{
    this->a->status.has_flag = false;
}

void Agent_event::scored_flag()
{
    this->a->status.has_flag = false;
    this->a->status.flag_captures++;
}

void Agent_event::coins_changed(unsigned int coins)
{
    if (coins == this->a->status.coins) return;
    this->a->status.coins = coins;
}

void Agent_event::fired_weapon_at_object(int id, int type, int part, float x, float y, float z)
{
    // animate laser to target

    // get vector from player to target's part
    // pick random piece of the part
    // animate to that

    // TODO:
    // need function get part position
    // currently goes straight to the target point, which is usually the feet

//// Gets x,y,z from packet;
//// reasons for this stated in agent_hit_object_StoC class declaration

    //void *obj;
    //float x,y,z;
    //switch (type)
    //{
        //case OBJ_TYPE_AGENT:
            //obj = ClientState::agent_list.get(id);
            //if (obj == NULL) return;
            //x = ((Agent_state*)obj)->s.x;
            //y = ((Agent_state*)obj)->s.y;
            //z = ((Agent_state*)obj)->s.z;
            //break;

        //case OBJ_TYPE_SLIME:
            //obj = ClientState::slime_list.get(id);
            //if (obj==NULL) return;
            //printf("Slime not null\n");
            //x = ((Monsters::Slime*)obj)->x;
            //y = ((Monsters::Slime*)obj)->y;
            //z = ((Monsters::Slime*)obj)->z;
            //break;
            
        //case OBJ_TYPE_SPAWNER:
            //obj = ClientState::spawner_list.get(id);
            //if (obj==NULL) return;
            //x = ((Spawner*)obj)->x;
            //y = ((Spawner*)obj)->y;
            //z = ((Spawner*)obj)->z;
            //break;

        //case OBJ_TYPE_TURRET:
            ////obj = ClientState::turret_list.get(id);
            ////if (obj==NULL) return;
            ////x = ((Turret*)obj)->x;
            ////y = ((Turret*)obj)->y;
            ////z = ((Turret*)obj)->z;
            ////break;
            //printf("Agent_event::fired_weapon_at_object -- turret type not implemented\n");
            //return;

        //default:
            //printf("Hitscan against invalid or non-hitscanned type %d\n", type);
            //return;
    //}

    float sx,sy,sz;
    sx = this->a->s.x;
    sy = this->a->s.y;
    sz = this->a->s.z + this->a->camera_height();

    float f[3];
    f[0] = x - sx;
    f[1] = y - sy;
    f[2] = z - sz;

    // animate
    const float hitscan_speed = 200.0f;
    ClientState::hitscan_effect_list.create(
        sx,sy,sz,
        f[0]*hitscan_speed, f[1]*hitscan_speed, f[2]*hitscan_speed
    );

    // play sound
    char soundfile[] = "laser_01.wav";
    Sound::play_3d_sound(soundfile, sx,sy,sz, this->a->s.vx, this->a->s.vy, this->a->s.vz);
}

void Agent_event::fired_weapon_at_block(float x, float y, float z, int cube, int side)
{
    float sx,sy,sz;
    sx = this->a->s.x;
    sy = this->a->s.y;
    sz = this->a->s.z + this->a->camera_height();

    // animate laser to target
    float f[3];
    f[0] = x - sx;
    f[1] = y - sy;
    f[2] = z - sz;

    const float hitscan_speed = 200.0f;
    ClientState::hitscan_effect_list.create(
        sx,sy,sz,
        f[0]*hitscan_speed, f[1]*hitscan_speed, f[2]*hitscan_speed
    );

    // play block surface crumble
    Animations::block_damage(x,y,z, f[0], f[1], f[2], cube, side);

    // play sound
    char soundfile[] = "laser_01.wav";
    Sound::play_3d_sound(soundfile, sx,sy,sz, this->a->s.vx, this->a->s.vy, this->a->s.vz);
}

void Agent_event::fired_weapon_at_nothing()
{
    float sx,sy,sz;
    sx = this->a->s.x;
    sy = this->a->s.y;
    sz = this->a->s.z + this->a->camera_height();

    float f[3];
    this->a->s.forward_vector(f);
    
    const float hitscan_speed = 200.0f;
    ClientState::hitscan_effect_list.create(
        sx,sy,sz,
        f[0]*hitscan_speed, f[1]*hitscan_speed, f[2]*hitscan_speed
    );

    // play sound
    char soundfile[] = "laser_01.wav";
    Sound::play_3d_sound(soundfile, sx,sy,sz, this->a->s.vx, this->a->s.vy, this->a->s.vz);
}

void Agent_event::hit_block()
{
    // play pick swing
    // play block damage animation
}

void Agent_event::threw_grenade()
{
    // play throw grenade animation
    // might need to sync grenades with this?
}

void Agent_event::placed_block()
{
    // player agent block placement animation
}


Agent_event::~Agent_event()
{
    if (this->bb != NULL)
    {
        this->bb->set_ttl(10000);   // let it die (will be culled next tick)
    }
}

Agent_event::Agent_event(Agent_state* owner)
:
a(owner),
bb(NULL)
{}

#endif
