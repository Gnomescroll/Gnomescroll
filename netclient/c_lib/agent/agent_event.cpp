#include "agent_event.hpp"

#ifdef DC_CLIENT
#include <c_lib/state/client_state.hpp>
#include <c_lib/animations/animations.hpp>
#include <c_lib/sound/sound.hpp>

void Agent_event::fired_weapon(int weapon_id) {
    // play weapon animation & sound
    char soundfile[] = "laser_01.wav";
    Sound::play_2d_sound(soundfile);

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

// side effects of taking damage. dont modify health/death here
void Agent_event::took_damage(int dmg) {
    BillboardText* b = ClientState::billboard_text_list.create(a->s.x, a->s.y, a->s.z, 0.0f,0.0f, 7.0f);
    b->set_color(255,10,10, 255);   // red
    char txt[10+1];
    int n = sprintf(txt, "%d", dmg);
    b->set_text(txt, n);
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

#endif
