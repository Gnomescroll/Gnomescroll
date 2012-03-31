
--print("map_load_tiles: loading tiles") ;


--[[
t top
b bottom
n north
s south
w west
e east
]]


if (IS_CLIENT == 0) then 
	function texture_alias()
		return nil
	end

	function register_spritesheet()
		return nil
	end
	function register_texture()
		return nil
	end
end

--- convenience method

function iso_texture(spritesheet, xpos, ypos)
	if(IS_CLIENT == 0) then return {t = nil, b = nil, n = nil, s = nil, w = nil, e = nil} end;
	local id;
	if (type(spritesheet) == "number" and xpos == nil and ypos == nil) then
		id = spritesheet
	else
		if(not xpos or not ypos) then error("iso_texture: xpos or ypos not set!") end
		id = register_texture(spritesheet, xpos, ypos)
	end
	return  { t = id, b = id, n = id, s = id, w = id, e = id };
end

function NewSolidBlock(id, name)
  o = BlockNew(id, name)
  o.properties = SolidBlockProperty;
  return o
end

--register texture sheets
t00 = register_spritesheet("t00")
t01 = register_spritesheet("t01")

--alias texture
error_block = texture_alias(t00,1,1)
black_blow = texture_alias(t00,4,2)
stone_block = texture_alias(t00,1,3)





Block:new1(0, "empty", EmptyBlockProperty, iso_texture(error_block), NoHud);


--- Classic Blocks ---

b = NewSolidBlock(255, "error_block"); -- id, name
b.texture = iso_texture(error_block);
--b.hud = hud(0, error_block);

b = NewSolidBlock(1, "lava");
b.texture = iso_texture(t00,2,1);
b.hud = hud(0+0, b.texture.n);

b = NewSolidBlock(2, "sand");
b.texture = iso_texture(t00,1,4);
b.hud = hud(0+1, b.texture.n);

b = NewSolidBlock(3, "sand_brick");
b.texture = iso_texture(t00,3,3);
b.hud = hud(0+2, b.texture.n);

--[[
b = NewSolidBlock(4, "red_brick");
b.texture = iso_texture(t00,2,3);
b.hud = hud(0+3, b.texture.n);
]]

b = NewSolidBlock(5, "grey_brick");
b.texture = iso_texture(t00,4,3);
b.hud = hud(0+3, b.texture.n);

--[[
b = NewSolidBlock(6, "rough_stone");
b.texture = iso_texture(t00,1,3);
b.hud = hud(0+5, b.texture.n);
]]

--[[
b = NewSolidBlock(7, "rough_stone_blue_ore");
b.texture = iso_texture(t00,2,4);
b.hud = hud(0+6, b.texture.n);

b = NewSolidBlock(8, "wood_panel");
b.texture = iso_texture(t00,3,4);
b.hud = hud(0+7, b.texture.n);
]]

b = NewSolidBlock(9, "terminal_blue");
b.texture = iso_texture(t00,1,4);
b.hud = hud(8+0, b.texture.n);

b = NewSolidBlock(10, "terminal_green");
b.texture = iso_texture(t00,1,4);
b.hud = hud(0+1, b.texture.n);

b = NewSolidBlock(11, "solar_panel");
b.texture = iso_texture(t00,1,2);
b.hud = hud(0+2, b.texture.n);

b = NewSolidBlock(12, "battery");
b.texture = iso_texture(t00,3,2);
b.texture.t = register_texture(t00,2,2);
b.texture.b = register_texture(t00,4,2);
b.hud = hud(8+3, b.texture.n);
b.max_damage = 32;


-- cell blocks --


--dust
b = NewSolidBlock(16, "light_dust");
b.texture = iso_texture(t01,1,1);
b.hud = hud(16+0, b.texture.n);

b = NewSolidBlock(17, "dark_dust");
b.texture = iso_texture(t01,1,2);
b.hud = hud(16+1, b.texture.n);

--rock
b = NewSolidBlock(18, "soft_rock");
b.texture = iso_texture(t01,1,3);
b.hud = hud(16+2, b.texture.n);

b = NewSolidBlock(19, "hard_rock");
b.texture = iso_texture(t01,1,4);
b.hud = hud(16+3, b.texture.n);

b = NewSolidBlock(20, "infested_rock");
b.texture = iso_texture(t01,1,5);
b.hud = hud(16+4, b.texture.n);

-- other

b = NewSolidBlock(21, "extruder");
b.texture = iso_texture(t01,2,2);
b.hud = hud(24+0, b.texture.n);

b = NewSolidBlock(22, "extruder_medical");
b.texture = iso_texture(t01,2,3);
b.hud = hud(24+1, b.texture.n);

b = NewSolidBlock(23, "holy_stone");
b.texture = iso_texture(t01,5,6);
b.hud = hud(24+2, b.texture.n);

b = NewSolidBlock(24, "holy_stone_artifact");
b.texture = iso_texture(t01,5,1);
b.hud = hud(24+3, b.texture.n);

-- brick

b = NewSolidBlock(25, "alien_brick_1");
b.texture = iso_texture(t01,6,1);
b.hud = hud(40+0, b.texture.n);

b = NewSolidBlock(26, "alien_brick_2");
b.texture = iso_texture(t01,6,2);
b.hud = hud(40, b.texture.n);

-- box

b = NewSolidBlock(27, "box_0");
b.texture = iso_texture(t01,6,3);
b.hud = hud(32+0, b.texture.n);

b = NewSolidBlock(28, "box_1");
b.texture = iso_texture(t01,6,3);
b.hud = hud(32+1, b.texture.n);

b = NewSolidBlock(29, "box_2");
b.texture = iso_texture(t01,6,3);
b.hud = hud(32+2, b.texture.n);

b = NewSolidBlock(30, "box_3");
b.texture = iso_texture(t01,6,3);
b.hud = hud(32+3, b.texture.n);

--end

print("map_load_tiles: done");

--[[
for i, block in pairs(block_id_table) do
  o = {}
end
]]