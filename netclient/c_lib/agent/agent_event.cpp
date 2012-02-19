#include "agent_event.hpp"

#ifdef DC_CLIENT
#include <c_lib/state/client_state.hpp>
#include <c_lib/animations/animations.hpp>
#include <c_lib/animations/hitscan.hpp>
#include <c_lib/sound/triggers.hpp>
#include <chat/client.hpp>

void Agent_event::name_changed(char* old_name)
{
    printf("name changed from %s to %s\n", old_name, this->a->status.name);
    if (this->bb != NULL)
    {
        this->bb->set_text(this->a->status.name);
    }

    if (a->is_you())
    {
        char fmt[] = "You are identified as %s";
        char msg[strlen(fmt) + strlen(this->a->status.name) + 1];
        sprintf(msg, fmt, this->a->status.name);
        chat_client.send_system_message(msg);
    }
    else
    {
        if (strcmp(AGENT_UNDEFINED_NAME, old_name))
        {
            char fmt[] = "%s is now known as %s";
            char msg[strlen(fmt) + strlen(old_name) + strlen(this->a->status.name) + 1];
            sprintf(msg, fmt, old_name, this->a->status.name);
            chat_client.send_system_message(msg);
        }
        else
        {
            char fmt[] = "%s has joined the game";
            char msg[strlen(fmt) + strlen(this->a->status.name) + 1];
            sprintf(msg, fmt, this->a->status.name);
            chat_client.send_system_message(msg);
        }
    }

    this->a->status.identified = true;
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
        ClientState::ctf.get_team_color(this->a->status.team, &r, &g, &b);
        this->bb->set_color(r,g,b,a);
        this->bb->projection_type = Billboard::HUD;
        this->bb->set_size(0.7);
    }
    this->bb->set_state(a->s.x, a->s.y, a->s.z + a->box.b_height, 0.0f, 0.0f, 0.0f);
    this->bb->set_draw(true);
}

// side effects of taking damage. dont modify health/death here
void Agent_event::took_damage(int dmg)
{
    BillboardText* b = ClientState::billboard_text_list.create(
        a->s.x + (randf()*(a->box.box_r*2) - a->box.box_r),
        a->s.y + (randf()*(a->box.box_r*2) - a->box.box_r),
        a->s.z + a->current_height(),
        0.0f,0.0f, 8.0f
    );
    if (b==NULL) return;
    b->set_color(255,10,10, 255);   // red
    char txt[10+1];
    sprintf(txt, "%d", dmg);
    b->set_text(txt);
    b->set_size(1.0f);
    b->set_ttl(5);

    if (a->is_you())
        Sound::agent_took_damage();
    else
        Sound::agent_took_damage(a->s.x, a->s.y, a->s.z, 0,0,0);
}

void Agent_event::healed(int health)
{
    a->status.health = health;

    if (a->is_you())
        Sound::restore_health();
    else
        Sound::restore_health(
            a->s.x,
            a->s.y,
            a->s.z,
            0,0,0
        );
}

void Agent_event::died() {
    if (!this->a->status.dead)
    {
        this->a->status.dead = true;
        if (a->is_you())
            Sound::died();
        else
            Sound::died(a->s.x, a->s.y, a->s.z, 0,0,0);
        this->a->vox->reset_skeleton(&agent_vox_dat_dead);
    }
}

void Agent_event::born()
{
    if (this->a->status.dead)
    {
        if (a->is_you())
            Sound::respawned();
        //else
            //Sound::respawned(a->s.x, a->s.y, a->s.z, 0,0,0);
        this->a->status.dead = false;
        // regenerate model
        if (this->a->vox != NULL)
            this->a->vox->restore(&agent_vox_dat, this->a->status.team);

        VoxDat* vd = (this->a->crouched()) ? &agent_vox_dat_crouched : &agent_vox_dat;
        this->a->vox->reset_skeleton(vd);
    }
}

void Agent_event::life_changing(bool dead) {
    if (dead) died();
    else born();
}

void Agent_event::crouched()
{
    this->a->vox->reset_skeleton(&agent_vox_dat_crouched);
}

void Agent_event::uncrouched()
{
    this->a->vox->reset_skeleton(&agent_vox_dat);
}

void Agent_event::reload_weapon(int type) {
    if (! a->weapons.is_active(type)) return;
    Sound::reload(a->s.x, a->s.y, a->s.z, 0,0,0);
    // play reload animation/sound for the weapon
}

void Agent_event::joined_team(int team)
{
    this->a->status.team = team;
    this->a->vox->update_team_color(&agent_vox_dat, team);
    chat_client.subscribe_channels();
}

void Agent_event::picked_up_flag()
{
    Sound::flag_picked_up();
    this->a->status.has_flag = true;
}

void Agent_event::dropped_flag()
{
    this->a->status.has_flag = false;
}

void Agent_event::scored_flag()
{
    Sound::flag_scored();
    this->a->status.has_flag = false;
    this->a->status.flag_captures++;
}

void Agent_event::coins_changed(unsigned int coins)
{
    if (coins == this->a->status.coins) return;
    this->a->status.coins = coins;
}

void Agent_event::fired_weapon_at_object(int id, int type, int part)
{
    float sx,sy,sz;
    sx = this->a->s.x;
    sy = this->a->s.y;
    sz = this->a->s.z + this->a->camera_height();

    float f[3];
    this->a->s.forward_vector(f);
    
    // animate
    const float hitscan_speed = 200.0f;
    ClientState::hitscan_effect_list.create(
        sx,sy,sz,
        f[0]*hitscan_speed, f[1]*hitscan_speed, f[2]*hitscan_speed
    );


    if (type == OBJ_TYPE_AGENT)
    {
        Agent_state* agent = ClientState::agent_list.get(id);
        if (agent != NULL && agent->vox != NULL)
        {
            Voxel_volume* vv = agent->vox->get_part(part);
            if (vv != NULL)
            {
                float c[3];
                vv->get_center(c);
                Animations::blood_spray(c[0], c[1], c[2], f[0], f[1], f[2]);
                Sound::pick_hit_agent(
                    c[0], c[1], c[2],
                    0,0,0
                );
            }
        }
    }

    Sound::fire_laser(sx,sy,sz, this->a->s.vx, this->a->s.vy, this->a->s.vz);
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
    Sound::laser_hit_block(x,y,z, 0,0,0);

    Sound::fire_laser(sx,sy,sz, this->a->s.vx, this->a->s.vy, this->a->s.vz);
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

    Sound::fire_laser(sx,sy,sz, this->a->s.vx, this->a->s.vy, this->a->s.vz);
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

void Agent_event::hit_block()
{
    // play pick swing
    // play block damage animation
    //Sound::pick_hit_block(collision_point[0], collision_point[1], collision_point[2], 0,0,0);
    //Sound::pick_swung(a->s.x,a->s.y,a->s.z,0,0,0);
}

void Agent_event::melee_attack_object(int id, int type, int part, float x, float y, float z)
{
    // play pick swing animation
    // play blood animation
    // play swing sound
    // play object's hurt sound
    //Sound::pick_swung(a->s.x,a->s.y,a->s.z,0,0,0);
    //Sound::pick_hit_agent(x,y,z,0,0,0);
}

void Agent_event::melee_attack_nothing()
{
    // play pick swing animation
    //Sound::pick_swung(a->s.x,a->s.y,a->s.z,0,0,0);
}

void Agent_event::fire_empty_weapon(int weapon_type)
{
    Sound::out_of_ammo(a->s.x, a->s.y, a->s.z, 0,0,0);
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
