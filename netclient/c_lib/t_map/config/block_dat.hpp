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
    EmptyBlock,
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

        case EmptyBlock:
        p.active = false;
        p.solid = false;
        p.occludes = false;
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
    if(xpos <= 0 || ypos <= 0)
    {
        printf("Error: iso_texture index on block %i is less than zero! \n", _current_cube_id);
        abort();
    }
    xpos--;
    ypos--;

    printf("Blit 1: %i %i %i \n", sheet_id, xpos, ypos);
	int tex_id = LUA_blit_cube_texture(sheet_id, xpos, ypos);
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

void side_texture(int side, int sheet_id, int ypos, int xpos)
{
#ifdef DC_CLIENT
    if(xpos <= 0 || ypos <= 0)
    {
        printf("Block Dat Error: side_texture index on block %i is less than zero! \n", _current_cube_id);
        abort();
    }
    xpos--;
    ypos--;

    printf("Blit 2: %i %i %i \n", sheet_id, xpos, ypos);
    int tex_id = LUA_blit_cube_texture(sheet_id, xpos, ypos);
    set_cube_side_texture(_current_cube_id, side, tex_id);
#endif
}

void hud_def(int hudy, int hudx, int tex_id)
{
#ifdef DC_CLIENT

    if(hudx <= 0 || hudy <= 0)
    {
        printf("Block Dat Error: hud_def1 index on block %i less than zero! \n", _current_cube_id);
        abort();
    }
    hudy--;
    hudx--;

    set_cube_hud(8*hudy+ hudx, _current_cube_id, tex_id);
#endif
}

void hud_def(int hudy,int hudx, int sheet_id, int ypos, int xpos)
{
#ifdef DC_CLIENT
    if(hudx <= 0 || hudy <= 0)
    {
        printf("Block Dat Error: hud_def2, hudx,hudy on block %i less than zero! \n", _current_cube_id);
        abort();
    }
    hudy--;
    hudx--;

    if(xpos <= 0 || ypos <= 0)
    {
        printf("Block Dat Error: hud_def2, xpos,ypos on block %i less than zero! \n", _current_cube_id);
        abort();
    }
    xpos--;
    ypos--;

    printf("Blit 2: %i %i %i \n", sheet_id, xpos, ypos);
    int tex_id = LUA_blit_cube_texture(sheet_id, xpos, ypos);
    set_cube_hud(8*hudy+ hudx, _current_cube_id, tex_id);
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

int sprite_alias(int sheet_id, int ypos, int xpos)
{
#if DC_CLIENT
    printf("Blit 3: %i %i %i \n", sheet_id, xpos, ypos);
    return LUA_blit_cube_texture(sheet_id, xpos, ypos); 
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

/*
    ErrorBlock,
    EmptyBlock,
    SolidBlock,
    ItemContainerBlock
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

    hud_def(1,1, error_block);
/*
    side_texture(T, error_block);
    side_texture(T, t0, 0,0);
*/

    cube_def(0, EmptyBlock, "empty_block");
    iso_texture(error_block);


    cube_def(1, SolidBlock, "terminal_blue");
    iso_texture(t0,1,3);
    hud_def(1,2, t0,1,1);

    cube_def(2, SolidBlock, "terminal_green");
    iso_texture(t0,1,4);
    hud_def(1,3, t0,1,2);


    cube_def(3, SolidBlock, "battery");
    iso_texture(t0,2,3);
    side_texture(T, t0, 2,2);
    side_texture(B, t0, 2,4);
    hud_def(1,4, t0,1,3);


    cube_def(6, ItemContainerBlock, "crate_2");
    iso_texture(t1,2,6);
    side_texture(T, t1, 1,7);
    side_texture(B, t1, 1,7);
    side_texture(B, t1, 1,7);
    hud_def(2,1, t1,1,3);

    cube_def(7, ItemContainerBlock, "crate_3");
    iso_texture(t1,3,4);
    side_texture(T, t1, 2,5);
    side_texture(B, t1, 3,6);
    side_texture(N, t1, 3,5);
    hud_def(2,2,t1,1,3);

    cube_def(16, SolidBlock, "methane_1");
    iso_texture(t1,2,2);
    hud_def(3,1, t1,2,2);

    cube_def(17, SolidBlock, "methane_2");
    iso_texture(t1,3,2);
    hud_def(3,2, t1,3,2);

    cube_def(18, SolidBlock, "methane_3");
    iso_texture(t1,4,3);
    hud_def(3,3, t1,4,3);

    cube_def(19, SolidBlock, "methane_4");
    iso_texture(t1,5,3);
    hud_def(3,4, t1,5,3);

    cube_def(20, SolidBlock, "methane_5");
    iso_texture(t1,4,1);
    hud_def(3,5, t1,4,1);


    cube_def(54, SolidBlock, "regolith");
    iso_texture(t1,10,4);
    side_texture(T, t1,10,3);
    side_texture(B, t1,10,5);
    hud_def(4,1, t1,10,4);

    cube_def(55, SolidBlock, "carbon");
    iso_texture(t1,10,6);
    hud_def(4,2, t1,10,6);


/*

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