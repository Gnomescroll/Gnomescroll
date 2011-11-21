#include "weapons.hpp"

namespace Weapons {

    bool isBlockPick(int weapon_type) {
        if (weapon_type == WEAPON_TYPE_BLOCK_PICK) return true;
        return false;
    }
}
