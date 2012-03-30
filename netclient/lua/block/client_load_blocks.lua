package.path = "lua/?.lua;lua/block/?.lua;?.lua"

require("lua_library");

IS_CLIENT = 1;

require("block_template");
require("block_texture");
require("block_dat");

local ffi = require("ffi")
ffi.cdef[[
void LUA_save_cube_texture();

void set_cube_side_texture(int id, int side, int tex_id);
void set_cube_hud(int pos, int cube_id, int tex_id);

void LUA_set_block_properties(int id, int active, int solid, int occludes, int transparent);
void LUA_set_block_max_damage(int id, int max_damage);
void LUA_set_block_name(int id, char* name, int length);
]]


function set_block_name(id, name)
  print("block_name: " .. id .. " = " .. name);
  local str = ffi.new("char[64]");
  ffi.copy(str, name);
  ffi.C.LUA_set_block_name(id, str, string.len(name) );
end

--[[
t top
b bottom
n north
s south
w west
e east
]]

--- set block properties

for id, block in pairs(block_id_table) do
	p = block.properties
	ffi.C.LUA_set_block_properties(id, p.active, p.solid, p.occludes, p.transparent )
end

for id, block in pairs(block_id_table) do
	set_block_name(id, block.name)
end

for id, block in pairs(block_id_table) do
	ffi.C.LUA_set_block_max_damage(id, block.max_damage)
end

--- set block draw properties

for id, block in pairs(block_id_table) do
	ffi.C.set_cube_side_texture(id, 0, block.texture.t )
	ffi.C.set_cube_side_texture(id, 1, block.texture.b )
	ffi.C.set_cube_side_texture(id, 2, block.texture.n )
	ffi.C.set_cube_side_texture(id, 3, block.texture.s )
	ffi.C.set_cube_side_texture(id, 4, block.texture.w )
	ffi.C.set_cube_side_texture(id, 5, block.texture.e )
end

for id, block in pairs(block_id_table) do
	if(block.hud.pos ~= nil) then
		ffi.C.set_cube_hud(block.hud.pos, id, block.hud.tex);
	end
end


ffi.C.LUA_save_cube_texture();

