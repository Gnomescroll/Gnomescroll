#pragma once

namespace Objects
{

// forward decl
class Object;

void load_pickup_sprite_data(ObjectType type);
Object* create_pickup_sprite(ObjectType type);
void ready_pickup_sprite(Object* object);
void die_pickup_sprite(Object* object);
void tick_pickup_sprite(Object* object);
//void update_pickup_sprite(Object* object);

} // Objects


