#include "weapons.hpp"

namespace Weapons {

bool HitscanLaser::fire() {
    if (clip == 0) return false;
    clip--;
    return true;
}

bool BlockApplier::fire() {
    if (ammo == 0) return false;
    ammo--;
    return true;
}

bool BlockPick::fire() {
    return true;
}

bool GrenadeThrower::fire() {
    if (ammo == 0) return false;
    ammo--;
    return true;
}


}
