#pragma once

namespace t_gen
{

const int PLASMAGEN_BLAST_RADIUS = 8;
const int PLASMAGEN_ENTITY_DAMAGE = 75;
const int PLASMAGEN_BLOCK_DAMAGE = 220; // max dmg on block is 254

#if DC_SERVER
void create_explosion(const int x, const int y, const int z);
void init_explosives();
#endif

}   // t_gen
