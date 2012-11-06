#pragma once

namespace Entities
{

void load_base_data();
class Entity* create_base();
void ready_base(class Entity* object);
void die_base(class Entity* object);
void tick_base(class Entity* object);
void update_base(class Entity* object);

} // Entities
