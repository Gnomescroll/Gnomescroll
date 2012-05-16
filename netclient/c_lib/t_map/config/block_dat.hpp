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
void set_cube_side_texture(int id, int side, int tex_id);
void set_cube_hud(int pos, int cube_id, int tex_id);

void LUA_set_block_properties(int id, int active, int solid, int occludes, int transparent);
void LUA_set_block_max_damage(int id, int max_damage);
void LUA_set_block_name(int id, char* name, int length);
*/

void iso_texture(int tex_id)
{
#ifdef DC_CLIENT
	set_cube_side_texture(_current_cube_id, 0, tex_id);
	set_cube_side_texture(_current_cube_id, 1, tex_id);
	set_cube_side_texture(_current_cube_id, 2, tex_id);
	set_cube_side_texture(_current_cube_id, 3, tex_id);
	set_cube_side_texture(_current_cube_id, 4, tex_id);
	set_cube_side_texture(_current_cube_id, 5, tex_id);
#endif
}


void iso_texture(int sheet_id, int ypos, int xpos)
{
#ifdef DC_CLIENT
	int tex_id = LUA_blit_item_texture(sheet_id, xpos, ypos);
	//set cube side textures
	set_cube_side_texture(_current_cube_id, 0, tex_id);
	set_cube_side_texture(_current_cube_id, 1, tex_id);
	set_cube_side_texture(_current_cube_id, 2, tex_id);
	set_cube_side_texture(_current_cube_id, 3, tex_id);
	set_cube_side_texture(_current_cube_id, 4, tex_id);
	set_cube_side_texture(_current_cube_id, 5, tex_id);
#endif
}

void side_texture(int side, int tex_id)
{
#ifdef DC_CLIENT 
    set_cube_side_texture(_current_cube_id, side, tex_id);
#endif
}

void side_texture(int side, int sheet_id, int xpos, int ypos)
{
#ifdef DC_CLIENT
    int tex_id = LUA_blit_item_texture(sheet_id, xpos, ypos);
    set_cube_side_texture(_current_cube_id, side, tex_id);
#endif
}

void hud_def(int hudy, int hudx, int tex_id)
{
#ifdef DC_CLIENT
    set_cube_hud(8*hudy+ hudx, _current_cube_id, int tex_id);
#endif
}

void hud_def(int hudy,int hudx, int tex_id, int sheet_id, int xpos, int ypos)
{
#ifdef DC_CLIENT
    int tex_id = LUA_blit_item_texture(sheet_id, xpos, ypos);
    set_cube_hud(8*hudy+ hudx, _current_cube_id, int tex_id);
#endif
}


int texture_alias(const char* spritesheet) 
{ 
#if DC_CLIENT
    return LUA_load_cube_texture_sheet((char*) spritesheet); 
#else
    return 0;
#endif
}

int sprite_alias(int spritesheet, int ypos, int xpos)
{
#if DC_CLIENT
    return LUA_blit_cube_texture(spritesheet, xpos, ypos); 
#else
    return 0;
#endif
}

void end_block_dat()
{
#if DC_CLIENT
    LUA_save_cube_texture();
#endif
}

/*
--[[
t top
b bottom
n north
s south
w west
e east
]]
*/

void load_block_dat()
{


    int t0 = texture_alias("media/sprites/t00.png");
    int t1 = texture_alias("media/sprites/t01.png");

    const int T = 0;
    const int B = 1;
    const int N = 2;
    const int S = 3;
    const int W = 4;
    const int E = 5;

    int error_block =  sprite_alias(t0,1,1);

    cube_def(255, ErrorBlock, "error_block");
    //iso_texture(error_block);
    side_texture(T, error_block);
    side_texture(B, error_block);
    side_texture(N, error_block);
    side_texture(S, error_block);
    side_texture(W, error_block);
    side_texture(E, error_block);
/*
    side_texture(T, error_block);
    side_texture(T, t0, 0,0);
*/

	end_block_dat();

}

/*

--register texture sheets
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