#pragma once

namespace Objects
{

// forward decl
class Object;

void load_health_refill_data();
Object* create_health_refill();
void ready_health_refill(Object* object);
void die_health_refill(Object* object);
void tick_health_refill(Object* object);
//void update_health_refill(Object* object);

} // Objects

