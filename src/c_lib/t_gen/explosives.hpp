/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

namespace t_gen
{

const int PLASMAGEN_BLOCK_BLAST_RADIUS = 12;
const float PLASMAGEN_ENTITY_BLAST_RADIUS = 6.0f;
const int PLASMAGEN_BLOCK_DAMAGE = 220; // max dmg on block is 254
const int PLASMAGEN_ENTITY_DAMAGE = 150;
const int LANDMINE_DAMAGE = 180;
const int LANDMINE_EXPLOSION_RADIUS = 6;

#if DC_SERVER
void create_explosion(const Vec3i& position);
void init_explosives();
void explode_landmine_damage_players(const Vec3& position);
#endif

//bool is_landmine(const Vec3i& position);

}   // t_gen
