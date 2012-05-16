#pragma once


namespace t_map 
{

int _current_cube_id = 0;

int texture_alias(const char* spritesheet);
void cube_def(int id, int type, const char* name);
int sprite_def(int spritesheet, int xpos, int ypos);


enum CubeType
{
	ErrorBlock,
	SolidBlock,
	ItemContainerBlock
};


/*
struct cubeProperties 
{
    bool active;
    bool solid;
    bool occludes;
    bool transparent;
    bool item_drop; //does block drop items
    bool item_container;  //inventory and crafting bench blocks
    bool reserved7;
    bool reserved8;

    unsigned char max_damage;
    unsigned char color_type; //flat, discrete and perlin
    //bool solid;
    //bool gravity;
};
*/
void cube_def(int id, int type, const char* name)
{ 
    _current_cube_id = id;
    set_cube_name(id, (char*) name);


    struct cubeProperties p;
    p.active = true;
    p.solid = true;
    p.occludes = true;
    p.transparent = false;
    p.item_drop = false;
    p.item_container = false;

    p.max_damage = 32;
    p.color_type = 0;

    switch(type)
    {
    	case ErrorBlock:
    	break;

    	case SolidBlock:
    	break;

    	case ItemContainerBlock:
    	p.item_container = true;
    	break;

    	default:
    	GS_ABORT();
    }


    //extern struct cubeProperties* cube_list;
    cube_list[id] = p;
}

/*
    void LUA_set_block_name(int id, char* name, int length) GNOMESCROLL_API;
    
    void LUA_set_block_properties(int id, int active, int solid, int occludes, int transparent) GNOMESCROLL_API;
    void LUA_set_block_max_damage(int id, int max_damage) GNOMESCROLL_API;
    void LUA_set_block_color_type(int id, int color_type) GNOMESCROLL_API;
*/

void iso_texture(int tex_id)
{

	//LUA_blit_item_texture(int sheet_id, int source_x, int source_y)
}


void iso_texture(int sheet_id, int xpos, int ypos)
{
	int tex = LUA_blit_item_texture(sheet_id, xpos, ypos);
	//set cube side textures
	iso_texture(tex);
}

#if DC_CLIENT
//int _current_cube_id = 0;
int texture_alias(const char* spritesheet) 
{ 
    return LUA_load_cube_texture_sheet((char*) spritesheet); 
}

int sprite_def(int spritesheet, int xpos, int ypos)
{
    return LUA_blit_cube_texture(spritesheet, xpos, ypos); 
}
#endif

#if DC_SERVER
int texture_alias(const char* spritesheet) {  return 0; }
int sprite_def(int spritesheet, int xpos, int ypos) {return 0;}
#endif

void end_block_dat()
{
    #if DC_CLIENT
    LUA_save_cube_texture();
    #endif
}

void load_block_dat()
{

    int t0 = texture_alias("media/sprites/t00.png");
    int t1 = texture_alias("media/sprites/t01.png");

    int error_block = texture_alias(t0,1,1);


    cube_def(255, ErrorBlock, "error_block");
    iso_texture(error_block);

	end_block_dat();

}

/*
t00 = register_spritesheet("t00.png")
t01 = register_spritesheet("t01.png")

--alias texture
error_block = texture_alias(t00,1,1)

--Block:new1(0, "empty", EmptyBlockProperty, iso_texture(error_block), NoHud);

b = NewSolidBlock(255, "error_block"); -- id, name
b.texture = iso_texture(error_block);
--b.hud = hud(0, error_block);


--- Classic Blocks ---

b = NewSolidBlock(1, "terminal_blue");
b.texture = iso_texture(t00,3,1);
b.hud = hud(8+0, b.texture.n);

b = NewSolidBlock(2, "terminal_green");
b.texture = iso_texture(t00,4,1);
b.hud = hud(8+1, b.texture.n);

b = NewSolidBlock(3, "solar_panel");
b.texture = iso_texture(t00,1,2);
b.hud = hud(8+2, b.texture.n);

b = NewSolidBlock(4, "battery");
b.texture = iso_texture(t00,3,2);
b.texture.t = register_texture(t00,2,2);
b.texture.b = register_texture(t00,4,2);
b.hud = hud(8+3, b.texture.n);
b.max_damage = 32;


-- cell blocks --

b = NewSolidBlock(5, "crate_1");
b.texture = iso_texture(t01,4,1);
b.texture.t = register_texture(t01,5,1);
b.texture.b = register_texture(t01,5,1);
b.texture.n = register_texture(t01,3,1);
b.hud = hud(24+0, b.texture.n);
b.max_damage = 32;

b = NewSolidBlock(6, "crate_2");
b.texture = iso_texture(t01,6,2);
b.texture.t = register_texture(t01,7,1);
b.texture.b = register_texture(t01,7,1);
b.texture.n = register_texture(t01,7,2);
b.hud = hud(24+1, b.texture.n);
b.max_damage = 32;

b = NewSolidBlock(7, "crate_3");
b.texture = iso_texture(t01,4,3);
b.texture.t = register_texture(t01,5,2);
b.texture.b = register_texture(t01,6,3);
b.texture.n = register_texture(t01,5,3);
b.hud = hud(24+2, b.texture.n);
b.max_damage = 32;

--dust
b = NewSolidBlock(16, "methane_1");
b.texture = iso_texture(t01,2,2);
b.hud = hud(16+0, b.texture.n);
b.max_damage = 1;

b = NewSolidBlock(17, "methane_2");
b.texture = iso_texture(t01,2,3);
b.hud = hud(16+1, b.texture.n);
b.max_damage = 2;

b = NewSolidBlock(18, "methane_3");
b.texture = iso_texture(t01,2,4);
b.hud = hud(16+2, b.texture.n);
b.max_damage = 6;

b = NewSolidBlock(19, "methane_4");
b.texture = iso_texture(t01,2,5);
b.hud = hud(16+3, b.texture.n);
b.max_damage = 12;

b = NewSolidBlock(20, "methane_5");
b.texture = iso_texture(t01,1,4);
b.hud = hud(16+4, b.texture.n);
b.max_damage = 10;

b = NewSolidBlock(54, "regolith");
b.texture = iso_texture(t01,4,10);
b.texture.t = register_texture(t01,3,10);
b.texture.b = register_texture(t01,5,10);
b.hud = hud(56-16+7, b.texture.n);

b = NewSolidBlock(55, "carbon");
b.texture = iso_texture(t01,6,10);
b.hud = hud(24+8+0, b.texture.n);
*/


}