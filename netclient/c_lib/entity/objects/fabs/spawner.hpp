#pragma once

namespace Objects
{

// forward decl
class Object;

void load_agent_spawner_data();
Object* create_agent_spawner();
void ready_agent_spawner(Object* object);
void die_agent_spawner(Object* object);
void tick_agent_spawner(Object* object);
void update_agent_spawner(Object* object);


} // Objects
