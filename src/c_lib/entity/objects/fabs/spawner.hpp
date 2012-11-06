#pragma once

namespace Entities
{

void load_agent_spawner_data();
class Entity* create_agent_spawner();
void ready_agent_spawner(Entity* object);
void die_agent_spawner(Entity* object);
void tick_agent_spawner(Entity* object);
void update_agent_spawner(Entity* object);

} // Entities
