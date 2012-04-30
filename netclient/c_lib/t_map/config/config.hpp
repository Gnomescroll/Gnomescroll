#pragma once


#ifdef DC_CLIENT
#include <c_lib/SDL/texture_sheet_loader.hpp>
#endif

namespace t_map
{
int test = 91;

int texture_alias(const char* spritesheet);
void cube_def(int id, int type, const char* name);
int sprite_def(int spritesheet, int xpos, int ypos);

void load_item_dat()
{

 	int test = 10;
 	printf("test= %i \n", test);

	int t00 = texture_alias("media/sprites/i00.png");

	cube_def(0, 0 , "TEST_RESOURCE1");
	sprite_def(t00, 0,0);

	cube_def(1, 0 , "TEST_RESOURCE2");
	sprite_def(t00, 1,0);

	cube_def(2, 0 , "TEST_RESOURCE3");
	sprite_def(t00, 0,2);

#ifdef DC_CLIENT
	LUA_save_item_texture();
#endif
}

}

namespace t_map
{

#ifdef DC_CLIENT
int _current_cube_id = 0;
int texture_alias(const char* spritesheet) { return LUA_load_cube_texture_sheet((char*) spritesheet); }
void cube_def(int id, int type, const char* name) { _current_cube_id = id; set_cube_name(id, (char*) name); }
int sprite_def(int spritesheet, int xpos, int ypos) { return LUA_blit_cube_texture(spritesheet, xpos, ypos); }
#else
int texture_alias(const char* spritesheet) { return 0; }
void cube_def(int id, int type, const char* name) {}
int sprite_def(int spritesheet, int xpos, int ypos) {return 0;}
#endif
}