

--[[
1> Block Properties
2> Block Hud Information
3< Block Texture Information
]]

bl_ffi = require("ffi")

--bl_ffi.load("c_lib")

bl_ffi.cdef[[
int LUA_load_cube_texture_sheet(char* filename);
int LUA_blit_cube_texture(int sheet_id, int source_x, int source_y);

void load_cube_texture_sprite_sheet(char*, int pos);
]]

prefix = "media/sprites/";

local str = bl_ffi.new("char[128]");
function register_spritesheet(filename)
  bl_ffi.copy(str, prefix .. filename);
  id =  bl_ffi.C.LUA_load_cube_texture_sheet(str);
  --print("registered spritesheet: " .. filename .. " id=" .. id);
  return id
end

function register_texture(spritesheet, xpos, ypos)
  assert(xpos ~= nil)
  assert(ypos ~= nil)

  xpos = xpos -1
  ypos = ypos -1

  assert(xpos >= 0);
  assert(ypos >= 0);

  if(spritesheet == nil) then
    error("WTF")
  end

  id = bl_ffi.C.LUA_blit_cube_texture(spritesheet, xpos, ypos)
  --print( "registered texture: " .. id .. " id=" .. id )
  return id
end

function texture_alias(spritesheet, xpos, ypos)
  --print( "register texture alias: ".. spritesheet .. " " .. xpos .. " " .. ypos );
  return register_texture(spritesheet, xpos, ypos)
end

--- hud stuff

function hud( pos, tex, xpos, ypos)
  if(xpos == nil or ypos == nil) then
    return { pos=pos, tex=tex }
  end
  return { pos=pos, tex=register_texture(tex,xpos,ypos) };
end