#pragma once

namespace Entities
{

// forward decl
class Entity;

void load_mob_bomb_data();
Entity* create_mob_bomb();
void ready_mob_bomb(Entity* object);
void die_mob_bomb(Entity* object);
void tick_mob_bomb(Entity* object);
void update_mob_bomb(Entity* object);

#if DC_SERVER
// TODO -- move
inline void send_mob_bomb_state_machine_to_client(ClientID client_id, class Entity* object);
#endif

} // Entities

