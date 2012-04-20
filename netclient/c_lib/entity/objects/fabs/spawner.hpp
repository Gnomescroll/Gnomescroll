#pragma once

namespace Objects
{

// forward decl
class Object;

Object* create_agent_spawner();
void ready_agent_spawner(Object* object);
void die_agent_spawner(Object* object);
void tick_agent_spawner(Object* object);
void update_agent_spawner(Object* object);


} // Objects
