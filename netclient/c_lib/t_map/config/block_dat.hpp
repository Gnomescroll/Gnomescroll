#pragma once


namespace t_map 
{


void texture_alias(cost char* texture_sheet)
{

}
	
void load_block_dat()
{

    int i0 = texture_alias("media/sprites/i00.png");
    int i1 = texture_alias("media/sprites/i01.png");

    item_def(0, IG_ERROR, "error_item");
    sprite_def(i0, 4,1);

    item_def(1, IG_PLACER, "regolith");
    sprite_def(i0, 1,3);
    s.placer_block_type_id = t_map::dat_get_cube_id("regolith");
    s.max_stack_size = 50;

    item_def(2, IG_RESOURCE, "copper_ore");
    sprite_def(i1, 1,3);
    s.max_stack_size = 50;

    item_def(3, IG_RESOURCE, "copper_bar");
    sprite_def(i1, 1,2);
    s.max_stack_size = 50;

    item_def(4, IG_RESOURCE, "quartz_crystal");
    sprite_def(i0, 1,5);

    item_def(5, IG_HITSCAN_WEAPON, "laser_rifle");
    sprite_def(i0, 3,7);
    s.hitscan_fire_cooldown = 30;
    s.hitscan_damage = 5;
    s.hitscan_max_ammo = 30;
    s.hitscan_bullet_effect_enum = 0;
    s.max_durability = 400;
    s.max_energy = 50;
    s.max_stack_size = 1;

    item_def(6, IG_MELEE_WEAPON, "copper_shovel");
    sprite_def(i1, 1,1);
    s.melee_fire_cooldown = 250;
    s.melee_damage = 2;
    s.max_durability = 400;
    s.max_stack_size = 1;

    item_def(7, IG_MINING_LASER, "mining_laser");
    sprite_def(i0, 3,5);
    s.mining_fire_cooldown = 200;
    s.mining_damage = 1;
    s.mining_block_damage = 3;
    s.max_durability = 200;
    s.max_energy = 50;
    s.max_stack_size = 1;

    item_def(8, IG_GRENADE_LAUNCHER, "grenade_launcher");
    sprite_def(i0, 1,7);
    s.max_durability = 200;
    s.max_energy = 100;
    s.max_stack_size = 1;

    item_def(9, IG_RESOURCE, "food_rock-0");
    sprite_def(i1, 1,4);
    s.max_stack_size = 8;
    s.nanite_food = true;

    item_def(10, IG_RESOURCE, "blue_cystal");
    sprite_def(i0, 1,4);
    s.max_stack_size = 8;

    item_def(11, IG_NANITE_COIN, "nanite_coin");
    sprite_def(i1, 2, 7);
    s.max_stack_size = 99;

    item_def(12, IG_DEBUG, "location_pointer");
    sprite_def(i0, 4,2);

    item_def(13, IG_DEBUG, "block_placer");
    sprite_def(i0, 4,5);

    item_def(14, IG_UNKNOWN, "unknown");
    sprite_def(i0, 5,1);

    end_item_dat();

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


}