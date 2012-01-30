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
        const float hitscan_speed = 120.0f;
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
    printf("You now have %d coins\n", coins);
    this->a->status.coins = coins;
}

Agent_event::~Agent_event()
{
    if (this->bb != NULL)
    {
        this->bb->set_ttl(10000);    // let billboard die
    }
}

Agent_event::Agent_event(Agent_state* owner)
:
a(owner),
bb(NULL)
{}

#endif
