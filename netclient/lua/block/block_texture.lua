

--[[
1> Block Properties
2> Block Hud Information
3< Block Texture Information
]]

bl_ffi = require("ffi")
bl_ffi.cdef[[
int LUA_load_cube_texture_sheet(char* filename);
void LUA_blit_cube_texture(int sheet_id, int source_x, int source_y, int dest_index);

void load_cube_texture_sprite_sheet(char*, int pos);
]]

prefix = "media/sprites/";

function load_texture_sheet(filename)
  print("load_texture_sheet: ", filename);
  local str = bl_ffi.new("char[128]");
  bl_ffi.copy(str, prefix .. filename ..".png");
  return bl_ffi.C.LUA_load_cube_texture_sheet(str);
end


-- Spritesheet loader
spritesheet_name_to_id = {};
spritesheet_id_to_name = {};

function register_spritesheet(spritesheet)
  if(spritesheet_name_to_id[spritesheet] ) then 
    return  spritesheet_name_to_id[spritesheet]
  end

  local id = load_texture_sheet(spritesheet);
  spritesheet_name_to_id[spritesheet] = id;
  spritesheet_id_to_name[id] = spritesheet;

  print("textured sheet loaded: " .. spritesheet .. " id= " .. id )
  return id
end

-- texture loader

texture_id_counter = 0;
texture_id_table = {};

texture_look_up_table = {}


---register a texture on a spritesheet
function register_texture(spritesheet, xpos, ypos)
  --local sindex = register_spritesheet(spritesheet);
  sindex = spritesheet
  local index = string.format("%s_%i_%i", spritesheet, xpos, ypos)

  if(texture_look_up_table[index]) then
    print("register texture: index found: " .. index .. " id= " .. texture_look_up_table[index])
    return texture_look_up_table[index]
  end

  local id = texture_id_counter;
  print("registered texture: " .. index .. " id=" .. id);

  bl_ffi.C.LUA_blit_cube_texture(sindex, xpos, ypos, id)

  texture_look_up_table[index] = id;
  texture_id_counter = texture_id_counter + 1;
  return id;
end


--- hud stuff

function hud( pos, tex, xpos, ypos)
  if(xpos == nil or ypos == nil) then
    return { pos=pos, tex=tex }
  end
  return { pos=pos, tex=register_texture(tex,xpos,ypos) };
end

--- texture stuff

function texture_alias(spritesheet, xpos, ypos)
  return register_texture(spritesheet, xpos, ypos)
end