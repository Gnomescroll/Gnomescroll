package.path = "media/lua/?.lua;?.lua"

require("lua_library");

require("block_loader");
require("block_dat");

--[[ Setup FFI Interface ]]


local ffi = require("ffi")
ffi.cdef[[
void set_cube_side_texture(int id, int side, int tex_id);
void set_cube_hud(int pos, int cube_id, int tex_id);

int LUA_load_cube_texture_sheet(char* filename);
void LUA_blit_cube_texture(int sheet_id, int source_x, int source_y, int dest_index);

void LUA_set_block_properties(int id, int active, int solid, int occludes, int transparent);
void LUA_set_block_max_damage(int id, int max_damage);

void LUA_save_cube_texture();
]]


--[[
prefix = "media/sprites/";
function load_texture_sheet(filename)
	local str = ffi.new("char[128]");
    ffi.copy(str, prefix..filename);
	return ffi.C.LUA_load_cube_texture_sheet(str);
end
]]

--[[
t top
b bottom
n north
s south
w west
e east
]]


--[[
for id, block in pairs(block_id_table) do
	print( id, "=", to_string(block.texture) );
end
]]

--[[
for id=255-16, 255, 1 do
	---print( id)
	ffi.C.LUA_blit_cube_texture(0, 0,0, id);
end
]]

--[[
for id=0, 255, 1 do
	ffi.C.set_cube_side_texture(id, 0, 0 )
	ffi.C.set_cube_side_texture(id, 1, 0 )
	ffi.C.set_cube_side_texture(id, 2, 0 )
	ffi.C.set_cube_side_texture(id, 3, 0 )
	ffi.C.set_cube_side_texture(id, 4, 0 )
	ffi.C.set_cube_side_texture(id, 5, 0 )
end
]]


for id, block in pairs(block_id_table) do
	--print( id, "=", to_string( block.texture) )
	ffi.C.set_cube_side_texture(id, 0, block.texture.t )
	ffi.C.set_cube_side_texture(id, 1, block.texture.b )
	ffi.C.set_cube_side_texture(id, 2, block.texture.n )
	ffi.C.set_cube_side_texture(id, 3, block.texture.s )
	ffi.C.set_cube_side_texture(id, 4, block.texture.w )
	ffi.C.set_cube_side_texture(id, 5, block.texture.e )
end


--- set block properties

for id, block in pairs(block_id_table) do
	p = block.properties
	--print( id, "=", to_string(p) );

	ffi.C.LUA_set_block_properties(id, p.active, p.solid, p.occludes, p.transparent )
end

--[[
for id=1, 255, 1 do
	ffi.C.LUA_set_block_properties(id, 1,1,1, 0)
end
]]

---
for id, tex_sheet in pairs(texture_id_table) do
	local sheet_id = tex_sheet.texture_sheet_id;
	local sheet_name = tex_sheet.texture_sheet_name;
	local xpos = tex_sheet.xpos;
	local ypos = tex_sheet.ypos;
	--print( id, "=", tex);
end



ffi.C.LUA_save_cube_texture();