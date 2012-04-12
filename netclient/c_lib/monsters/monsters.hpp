#pragma once

#include <c_lib/monsters/constants.hpp>
#include <c_lib/monsters/slime.hpp>
#include <c_lib/monsters/box.hpp>
#include <c_lib/monsters/monster_spawner.hpp>

namespace Monsters
{
#if DC_SERVER
void spawn_monsters(int max_monsters);
void create_monsters_spawners(int max_spawners);
#endif
} // Monsters
