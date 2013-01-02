#pragma once

namespace t_gen
{

const int PLASMAGEN_BLAST_RADIUS = 12;
const int PLASMAGEN_BLOCK_DAMAGE = 220; // max dmg on block is 254
const int PLASMAGEN_ENTITY_DAMAGE = 150;
const int LANDMINE_DAMAGE = 180;
const int LANDMINE_EXPLOSION_RADIUS = 6;

#if DC_SERVER
void create_explosion(const int x, const int y, const int z);
void init_explosives();
void explode_landmine_damage_players(float x, float y, float z);
#endif

bool isLandmine(int x, int y, int z);

}   // t_gen
