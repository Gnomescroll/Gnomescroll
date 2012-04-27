#pragma once

namespace t_item
{


int id_to_sprite(int id);

void set_item_name(int id, char* name, int length);
char* get_item_name(int id);
int get_item_id(char* name);

}