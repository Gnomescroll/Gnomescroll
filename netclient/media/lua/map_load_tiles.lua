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

void LUA_blit_cube_texture;
void LUA_save_cube_texture();

void load_cube_texture_sprite_sheet(char*, int pos);
void load_hud_texture(char*, int pos);
]]

ffi.C.LUA_save_cube_texture();

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

for id=0, 255, 1 do
	---print( id)
	ffi.C.set_cube_side_texture(id, 0, 0 )
	ffi.C.set_cube_side_texture(id, 1, 0 )
	ffi.C.set_cube_side_texture(id, 2, 0 )
	ffi.C.set_cube_side_texture(id, 3, 0 )
	ffi.C.set_cube_side_texture(id, 4, 0 )
	ffi.C.set_cube_side_texture(id, 5, 0 )

	ffi.C.LUA_blit_cube_texture(0, 0,0, id);
end


for id, block in pairs(block_id_table) do
	ffi.C.set_cube_side_texture(id, 0, block.texture.t )
	ffi.C.set_cube_side_texture(id, 1, block.texture.b )
	ffi.C.set_cube_side_texture(id, 2, block.texture.n )
	ffi.C.set_cube_side_texture(id, 3, block.texture.s )
	ffi.C.set_cube_side_texture(id, 4, block.texture.w )
	ffi.C.set_cube_side_texture(id, 5, block.texture.e )
end

for id, tex_sheet in pairs(texture_id_table) do
	local sheet_id = tex_sheet.texture_sheet_id;
	local sheet_name = tex_sheet.texture_sheet_name;
	local xpos = tex_sheet.xpos;
	local ypos = tex_sheet.ypos;
	print( id, "=", tex);
end