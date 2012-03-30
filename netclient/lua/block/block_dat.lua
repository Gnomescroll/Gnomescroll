
--print("map_load_tiles: loading tiles") ;


--[[
t top
b bottom
n north
s south
w west
e east
]]

--register texture sheets
if (IS_CLIENT == 1) then 
	t00 = register_spritesheet("t00")
	t01 = register_spritesheet("t01")
else
	t00 = nil;
	t01 = nil;
end

--short hand functions

function iso_texture(spritesheet, xpos, ypos)
	if(IS_CLIENT == 1) then return end;
	local id;
	if (type(spritesheet) == "number" and xpos == nil and ypos == nil) then
		id = spritesheet
	else
		if(not xpos or not ypos) then error("iso_texture: xpos or ypos not set!") end
		id = register_texture(spritesheet, xpos, ypos)
	end
	return  { t = id, b = id, n = id, s = id, w = id, e = id };
end

--- convenience method

if (IS_CLIENT == 1) then 
	error_block = texture_alias(t00,0,0)
	stone_block = texture_alias(t00,0,2)
end

function NewSolidBlock(id, name)
  o = BlockNew(id, name)
  o.properties = SolidBlockProperty;
  return o
end



Block:new1(0, "empty", EmptyBlockProperty, iso_texture(error_block), NoHud);


--- Classic Blocks ---

b = NewSolidBlock(255, "error_block"); -- id, name
b.texture = iso_texture(error_block);
b.hud = hud(63, "error.png");


b = NewSolidBlock(1, "soft_rock");
b.texture = iso_texture(t00,0,1);
b.hud = hud(1, "blueore.png");

b = NewSolidBlock(2, "lava");
b.texture = iso_texture(stone_block);
b.hud = hud(0, "stone.png");

b = NewSolidBlock(3, "terminal_blue");
b.texture = iso_texture(t00,0,3);
b.hud = hud(1, "blueore.png");

b = NewSolidBlock(4, "terminal_green");
b.texture = iso_texture(t00,0,3);
b.hud = hud(1, "blueore.png");

b = NewSolidBlock(5, "solar_panel");
b.texture = iso_texture(t00,1,0);
b.hud = hud(1, "blueore.png");

b = NewSolidBlock(6, "holy_stone");
b.texture = iso_texture(t01,5,4);
b.hud = hud(1, "blueore.png");

print("map_load_tiles: done");

--[[
for i, block in pairs(block_id_table) do
  o = {}
end
]]