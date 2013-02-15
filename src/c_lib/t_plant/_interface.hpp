#pragma once

/*

void love_ai_character_create();
void love_ai_character_destroy();
void love_ai_character_update();
void love_ai_tool_create();
void love_ai_tool_destroy();
void love_ai_tool_update();

...is more stuctured then this:

void create_character();
void destroy_character();
void update_character();
void create_tool();
void destroy_tool();
void update_tool();

...In C, where we don't have namespaces.

namespace love_ai
{
void love_ai_character_create();
}

Is redundant and verbose

*/

namespace t_plant
{

void init();
void teardown();
void tick();
void create_plant(int x, int y, int z, int type);

}   // t_plant

