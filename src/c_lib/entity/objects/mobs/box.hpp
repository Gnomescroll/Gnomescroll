#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_mob_robot_box_data();
Entity* create_mob_robot_box();
void ready_mob_robot_box(Entity* object);
void die_mob_robot_box(Entity* object);
void tick_mob_robot_box(Entity* object);
void update_mob_robot_box(Entity* object);

} // Entities

