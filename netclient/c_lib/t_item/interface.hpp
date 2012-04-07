#pragma once



namespace t_item
{


extern class Free_item_list* free_item_list;

void state_init();
void state_teardown();

void tick();
void draw();


void create_free_item(int id, float f, float y, float z, float vx, float vy, float vz);


}
 