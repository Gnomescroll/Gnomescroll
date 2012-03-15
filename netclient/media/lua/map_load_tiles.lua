package.path = "media/lua/?.lua;?.lua"

print "map_load_tiles: starting";


--[[ Setup FFI Interface ]]

prefix = "./media/blocks/";

local ffi = require("ffi")
ffi.cdef[[
void set_cube_side_texture(int id, int side, int tex_id);
void set_cube_hud(int pos, int cube_id, int tex_id);

void load_cube_texture_sprite_sheet(char*, int pos);
void load_hud_texture(char*, int pos);
]]
---local barreturn = ffi.C.barfunc(253)

--[[ Block Loader Functions ]]

require("block_loader");

--[[ Load Block Dat ]]

