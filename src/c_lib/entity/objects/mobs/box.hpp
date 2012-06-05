#pragma once

namespace Objects
{

// forward decl
class Object;

void load_mob_robot_box_data();
Object* create_mob_robot_box();
void ready_mob_robot_box(Object* object);
void die_mob_robot_box(Object* object);
void tick_mob_robot_box(Object* object);
void update_mob_robot_box(Object* object);

} // Objects

