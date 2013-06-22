#pragma once

namespace Entities
{

class Entity;  // forward decl

void load_mob_robot_box_data();
void ready_mob_robot_box(Entity* entity);
void die_mob_robot_box(Entity* entity);
void tick_mob_robot_box(Entity* entity);
void update_mob_robot_box(Entity* entity);

} // Entities

