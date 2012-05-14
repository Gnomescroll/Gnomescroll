#pragma once


namespace t_map 
{

void load_block_dat()
{

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

}


}