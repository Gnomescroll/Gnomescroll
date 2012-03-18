package.path = "media/lua/?.lua;?.lua"

print "map_load_tiles: starting";

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
---local barreturn = ffi.C.barfunc(253)

--- void load_cube_texture_sprite_sheet(char*, int pos);
--- void load_hud_texture(char*, int pos);

--[[ Block Loader Functions ]]

prefix = "media/sprites/";
function load_texture_sheet(filename)
	local str = ffi.new("char[128]");
    ffi.copy(str, prefix..filename);
	return ffi.C.LUA_load_cube_texture_sheet(str);
end

tex_id = load_texture_sheet("tiles_00.png");


ffi.C.LUA_blit_cube_texture(tex_id, 0,0, 0);
ffi.C.LUA_blit_cube_texture(tex_id, 0,1, 1);
ffi.C.LUA_blit_cube_texture(tex_id, 0,2, 2);
ffi.C.LUA_blit_cube_texture(tex_id, 0,3, 3);

ffi.C.LUA_blit_cube_texture(tex_id, 0,0, 16+0);
ffi.C.LUA_blit_cube_texture(tex_id, 0,1, 32+1);
ffi.C.LUA_blit_cube_texture(tex_id, 0,2, 48+2);
ffi.C.LUA_blit_cube_texture(tex_id, 0,3, 64+3);

ffi.C.LUA_save_cube_texture();

require("block_loader");


for id, block in pairs(block_id_table) do
	print( id, "=", block);
end

print("tex")

for id, tex_sheet in pairs(texture_id_table) do
	local sheet_id = tex_sheet.texture_sheet_id;
	local sheet_name = tex_sheet.texture_sheet_name;
	local xpos = tex_sheet.xpos;
	local ypos = tex_sheet.ypos;
	print( id, "=", tex);
end



--[[ Load Block Dat ]]

