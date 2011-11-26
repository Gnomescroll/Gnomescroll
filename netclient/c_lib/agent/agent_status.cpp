#include "agent_status.hpp"

int Base_status::apply_damage(int dmg) {
    health -= dmg;
    if (health <= 0) {
        health = 0;
        dead = true;
    }
    return health;
}
