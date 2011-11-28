#include "agent_event.hpp"

#ifdef DC_CLIENT
#include <c_lib/state/client_state.hpp>
#include <c_lib/animations/animations.hpp>

void Agent_event::fired_weapon(int weapon_id) {
    // play weapon animation & sound
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
    printf("UR DEAD\n");
}

#endif
