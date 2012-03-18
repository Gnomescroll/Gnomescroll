

--- LUA_PATH = "./:./media/lua/"




print("map_load_tiles: loading tiles") ;

--- texture stuff

texture_aliases = {}

function texture_alias(spritesheet, xpos, ypos)
  return register_texture(spritesheet, xpos, ypos)
end



--[[
t top
b bottom
n north
s south
w west
e east
]]

function iso_texture(spritesheet, xpos, ypos)
	local id;
	if (type(spritesheet) == "number") then
		id= spritesheet
	else
		if(not xpos or not ypos) then error("iso_texture: xpos or ypos not set!") end
		id = register_texture(spritesheet, xpos, ypos)
	end
	o = {};
	o.t = id;
	o.b = id;
	o.n = id;
	o.s = id;
	o.w = id;
	o.e = id;
	return o
end

--- convenience method

function NewSolidBlock(id, name)
  o = BlockNew(id, name)
  o.properties = SolidBlockProperty;
  return o
end

error_block = texture_alias("t00",0,0)
stone_block = texture_alias("t00",0,2)

Block:new1(0, "empty", EmptyBlockProperty, iso_texture(error_block), NoHud);


--- Classic Blocks ---

b = NewSolidBlock(1, "error_block"); -- id, name
b.texture = iso_texture(error_block);
b.hud = hud(63, "error.png");

b = NewSolidBlock(2, "lava");
b.texture = iso_texture(stone_block);
b.hud = hud(0, "stone.png");

b = NewSolidBlock(3, "terminal_blue");
b.texture = iso_texture("t00",0,3);
b.hud = hud(1, "blueore.png");

b = NewSolidBlock(4, "terminal_green");
b.texture = iso_texture("t00",0,3);
b.hud = hud(1, "blueore.png");

b = NewSolidBlock(5, "solar_panel");
b.texture = iso_texture("t00",1,0);
b.hud = hud(1, "blueore.png");

print("map_load_tiles: done");


b = NewSolidBlock(14, "soft_rock");
b.texture = iso_texture("t00",1,1);
b.hud = hud(1, "blueore.png");



for i, block in pairs(block_id_table) do
  o = {}
end