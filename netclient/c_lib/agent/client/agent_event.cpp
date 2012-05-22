#include "agent_event.hpp"

#if DC_SERVER
dont_include_this_file_in_server
#endif

#include <particle/billboard_text_hud.hpp>

#include <state/client_state.hpp>
#include <sound/triggers.hpp>
#include <chat/client.hpp>
#include <hud/map.hpp>

#include <particle/_interface.hpp>
#include <particle/constants.hpp>

#include <animations/_interface.hpp>
#include <animations/animations.hpp>

#include <item/properties.hpp>

void Agent_event::name_changed(char* old_name)
{
    if (this->bb != NULL)
        this->bb->set_text(this->a->status.name);

    if (a->is_you())
    {
        const char fmt[] = "You are identified as %s";
        char* msg = (char*)calloc(strlen(fmt) + strlen(this->a->status.name) - 2 + 1, sizeof(char));
        sprintf(msg, fmt, this->a->status.name);
        chat_client->send_system_message(msg);
        free(msg);
    }
    else
    {
        if (strcmp(AGENT_UNDEFINED_NAME, old_name))
        {
            const char fmt[] = "%s is now known as %s";
            char* msg = (char*)calloc(strlen(fmt) + strlen(old_name) + strlen(this->a->status.name) - 4 + 1, sizeof(char));
            sprintf(msg, fmt, old_name, this->a->status.name);
            chat_client->send_system_message(msg);
            free(msg);
        }
        else
        {
            const char fmt[] = "%s has joined the game";
            char* msg = (char*)calloc(strlen(fmt) + strlen(this->a->status.name) - 2 + 1, sizeof(char));
            sprintf(msg, fmt, this->a->status.name);
            chat_client->send_system_message(msg);
            free(msg);
            #if DC_CLIENT && PRODUCTION_DEV
            Sound::player_joined_server();  // move this to a better place, along with the other dis/connect events
            #endif
        }
    }

    this->a->status.identified = true;
}

void Agent_event::hide_name()
{
    if (this->bb != NULL)
        this->bb->set_draw(false);
}

void Agent_event::display_name()
{
    if (this->a->status.dead) return;
    if (this->bb == NULL)
    {
        this->bb = Particle::billboard_text_hud_list->create();
        if (this->bb == NULL) return;
        this->bb->set_ttl(-1000);          // dont die
        this->bb->set_text(this->a->status.name);
        this->bb->set_color(r,g,b,255);
        this->bb->set_size(0.7);
    }
    const float z_margin = 0.4;
    this->bb->set_state(a->s.x, a->s.y, a->s.z + a->box.b_height + z_margin, 0.0f, 0.0f, 0.0f);
    this->bb->set_draw(true);
}

// side effects of taking damage. dont modify health/death here
void Agent_event::took_damage(int dmg)
{
    Particle::BillboardText* b = Particle::billboard_text_list->create();
    if (b==NULL) return;
    b->set_state(
        a->s.x + (randf()*(a->box.box_r*2) - a->box.box_r),
        a->s.y + (randf()*(a->box.box_r*2) - a->box.box_r),
        a->s.z + a->current_height(),
        0.0f,0.0f, BB_PARTICLE_DMG_VELOCITY_Z
    );
    b->set_color(BB_PARTICLE_DMG_COLOR);   // red
    char txt[10+1];
    sprintf(txt, "%d", dmg);
    b->set_text(txt);
    b->set_size(1.0f);
    b->set_ttl(5);

    if (a->is_you())
        Sound::agent_took_damage();
    // TODO: attenuated damage sound
    //else
        //Sound::agent_took_damage(a->s.x, a->s.y, a->s.z, 0,0,0);
}

void Agent_event::healed(int health)
{
    bool healed = (a->status.health < health);
    a->status.health = health;

    if (a->is_you())
    {
        if (healed)
        {
            Sound::restore_health();
            chat_client->send_system_message((char*) "You healed.");
        }
    }
    else
    {
        if (healed)
            Sound::restore_health(
                a->s.x,
                a->s.y,
                a->s.z,
                0,0,0
            );
    }
}

void Agent_event::died()
{
    if (!this->a->status.dead)
    {
        this->a->status.dead = true;
        if (a->is_you())
            Sound::died();
        else
            Sound::died(a->s.x, a->s.y, a->s.z, 0,0,0);
        this->a->vox->set_vox_dat(&VoxDats::agent_dead);
        this->a->vox->reset_skeleton();
    }
    Toolbelt::agent_died(this->a->id);
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

        // reset skeleton
        VoxDat* vd = (this->a->crouched()) ? &VoxDats::agent_crouched : &VoxDats::agent;
        this->a->vox->set_vox_dat(vd);
        this->a->vox->reset_skeleton();
        // regenerate model
        if (this->a->vox != NULL)
            this->a->vox->restore(this->a->status.team);

    }
}

void Agent_event::life_changing(bool dead)
{
    if (dead) died();
    else born();
}

void Agent_event::set_spawner(int pt)
{
    this->a->status.spawner = pt;
    //printf("Agent_event:: set spawner to %d\n", pt);
}

void Agent_event::crouched()
{
    printf("crouched\n");
    this->model_was_changed = true;
    this->a->vox->set_vox_dat(&VoxDats::agent_crouched);
    this->a->vox->reset_skeleton();
}

void Agent_event::uncrouched()
{
    printf("uncrouched\n");
    this->model_was_changed = true;
    this->a->vox->set_vox_dat(&VoxDats::agent);
    this->a->vox->reset_skeleton();
}

bool Agent_event::model_changed()
{
    bool changed = this->model_was_changed;
    this->model_was_changed = false;
    return changed;
}

void Agent_event::set_agent_vox_status(AgentVoxStatus status)
{
    if (this->vox_status != status) this->model_was_changed = true;
    this->vox_status = status;
}

void Agent_event::reload_weapon(int type) {
    //if (! a->weapons.is_active(type)) return;
    Sound::reload(a->s.x, a->s.y, a->s.z, 0,0,0);
    // play reload animation/sound for the weapon
}

void Agent_event::update_team_color(unsigned char r, unsigned char g, unsigned char b)
{
    if (r == this->r && g == this->g && b == this->b)
        return;
    this->r = r;
    this->g = g;
    this->b = b;
    if (this->a->vox != NULL)
        this->a->vox->update_team_color(this->a->status.team);
    if (this->bb != NULL)
        this->bb->set_color(r,g,b);
}

void Agent_event::update_team_color()
{
    unsigned char r,g,b;
    int ret = ClientState::ctf->get_team_color(this->a->status.team, &r, &g, &b);
    if (ret) return;
    this->update_team_color(r,g,b);
}

void Agent_event::joined_team(int team)
{
    int old_team = this->a->status.team;
    this->a->status.team = team;
    if (old_team != team)
        this->update_team_color();
    chat_client->subscribe_channels();
    if (this->a->is_you() && old_team != team)
        HudMap::update_team(team);
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

void Agent_event::tick_mining_laser()
{
    int weapon_type = Item::get_item_type((char*)"mining_laser");
    float range = Item::get_weapon_range(weapon_type);

    Animations::mining_laser_beam(this->a->arm_center(), this->a->s.forward_vector(), range);
}

void Agent_event::fired_mining_laser()
{
    //Sound::fire_mining_laser();
}

void Agent_event::fired_weapon_at_object(int id, int type, int part)
{
    float sx,sy,sz;
    sx = this->a->s.x;
    sy = this->a->s.y;
    sz = this->a->camera_z();

    Sound::fire_laser(sx,sy,sz, this->a->s.vx, this->a->s.vy, this->a->s.vz);

    float f[3];
    this->a->s.forward_vector(f);

    if (type == OBJECT_AGENT)
    {
        Agent_state* agent = ClientState::agent_list->get(id);
        if (agent != NULL && agent->vox != NULL)
        {
            Voxel_volume* vv = agent->vox->get_part(part);
            if (vv != NULL)
            {
                float c[3];
                vv->get_center(c);
                Animations::blood_spray(c[0], c[1], c[2], f[0], f[1], f[2]);
                Sound::pick_hit_agent(  // TODO: switch (weapon) {}
                    c[0], c[1], c[2],
                    0,0,0
                );
            }
        }
    }

    if (this->a->vox == NULL) return;
    // play laser anim out of arm
    const float hitscan_speed = 200.0f;
    Vec3 arm_center = this->a->arm_center();
    //f[0] = x - arm_center.x;
    //f[1] = y - arm_center.y;
    //f[2] = z - arm_center.z;
    
    Animations::create_hitscan_effect(
        arm_center.x, arm_center.y, arm_center.z,
        f[0]*hitscan_speed, f[1]*hitscan_speed, f[2]*hitscan_speed
    );

}

void Agent_event::fired_weapon_at_block(float x, float y, float z, int cube, int side)
{
    float sx,sy,sz;
    sx = this->a->s.x;
    sy = this->a->s.y;
    sz = this->a->camera_z();

    Sound::fire_laser(sx,sy,sz, this->a->s.vx, this->a->s.vy, this->a->s.vz);

    if (this->a->vox == NULL) return;

    // animate laser to target

    // play laser anim out of arm
    const float hitscan_speed = 200.0f;
    //Vec3 arm_center = this->a->vox->get_part(AGENT_PART_RARM)->world_matrix.c;
    Vec3 arm_center = this->a->vox->get_node(5)->c;
    
    // vector from arm center to collision point
    Vec3 p = vec3_init(x,y,z);
    Vec3 f = vec3_sub(p, arm_center);
    normalize_vector(&f);

    //Vec3 look = this->a->s.forward_vector();    // wrong thing
    //// NEED THE FKN FORWARD VECTOR OF THE NODE
    //// ROTATE FROM DEFAULT FORWARD VECTOR OF NODE (READ MATRIX OFF VOX DAT)
    //// TO LASER
    ////Vec3 look = vec3_scalar_mult(this->a->vox->get_node(5)->vz, -1);
    //// need the look vector for the ARM
    ////float theta = this->a->s.theta;
    ////float _x,_y;
    ////rotate_point(1,0, theta, &_x, &_y);
    ////Vec3 look = vec3_init(_x, _y, 0);
    ////look.z = 0;
    ////normalize_vector(&look);

    ////printf("f:");
    ////vec3_print(f);
    ////printf("look:");
    ////vec3_print(look);
    ////printf("\n");
    //Vec3 f_cache = vec3_copy(f);
    //Vec3 look_cache = vec3_copy(look);

    //float theta, phi;

    //// xy plane
    //f.z = 0;
    //look.z = 0;
    //normalize_vector(&f);
    //normalize_vector(&look);
    //theta = acos(vec3_dot(f, look));

    //f = vec3_copy(f_cache);
    //look = vec3_copy(look_cache);

    //// xz plane
    //f.y = 0;
    //look.y = 0;
    //normalize_vector(&f);
    //normalize_vector(&look);
    //phi = acos(vec3_dot(f, look));

    //f = vec3_copy(f_cache);
    //look = vec3_copy(look_cache);

    //ax /= kPI;
    //ay /= kPI;
    //this->a->vox->set_arm(-phi, -theta);

    f = vec3_scalar_mult(f, hitscan_speed);
    Animations::create_hitscan_effect(
        arm_center.x, arm_center.y, arm_center.z,
        f.x, f.y, f.z
    );

    // play block surface crumble
    Animations::block_damage(x,y,z, f.x, f.y, f.z, cube, side);
    Animations::terrain_sparks(x,y,z);
    Sound::laser_hit_block(x,y,z, 0,0,0);
}

void Agent_event::fired_weapon_at_nothing()
{
    float sx,sy,sz;
    sx = this->a->s.x;
    sy = this->a->s.y;
    sz = this->a->camera_z();

    Sound::fire_laser(sx,sy,sz, this->a->s.vx, this->a->s.vy, this->a->s.vz);

    if (this->a->vox == NULL) return;
    
    float f[3];
    this->a->s.forward_vector(f);
    
    // play laser anim out of arm
    const float hitscan_speed = 200.0f;
    Vec3 arm_center = this->a->vox->get_part(AGENT_PART_RARM)->world_matrix.c;
    Animations::create_hitscan_effect(
        arm_center.x, arm_center.y, arm_center.z,
        f[0]*hitscan_speed, f[1]*hitscan_speed, f[2]*hitscan_speed
    );
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

void Agent_event::melee_attack_object(int id, int type, int part)
{
    // play pick swing animation
    // play blood animation
    // play swing sound
    // play object's hurt sound
    //Sound::pick_swung(a->s.x,a->s.y,a->s.z,0,0,0);
    //Sound::pick_hit_agent(a->s.x, a->s.y, a->s.z,0,0,0);
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
    {   // BUG -- particle list dtor might be called before this, on close
        this->bb->set_ttl(10000);   // let it die (will be culled next tick)
    }
}

Agent_event::Agent_event(Agent_state* owner)
:
a(owner),
r(0),g(0),b(0),
model_was_changed(true),
bb(NULL)
{}
