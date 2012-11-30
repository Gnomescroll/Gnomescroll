#pragma once

#if DC_CLIENT
dont_include_this_file_in_client
#endif

namespace t_gen
{

const int PLASMAGEN_BLAST_RADIUS = 8;
const int PLASMAGEN_ENTITY_DAMAGE = 75;
const int PLASMAGEN_BLOCK_DAMAGE = 220; // max dmg on block is 254

void create_explosion(const int x, const int y, const int z);

void init_explosives();

}   // t_gen
