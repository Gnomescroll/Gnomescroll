#pragma once

namespace Entities
{

void load_base_data();
//class Entity* create_base();
void ready_base(class Entity* entity);
void die_base(class Entity* entity);
void tick_base(class Entity* entity);
void update_base(class Entity* entity);

} // Entities
