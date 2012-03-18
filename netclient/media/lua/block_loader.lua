

--[[
1> Block Properties
2> Block Hud Information
3< Block Texture Information
]]

print("block_dat: starting");

BlockProperty = {
active = 0,
solid = 0,
occludes = 0,
transparent = 0
}

--- falls through if property is not set
function BlockProperty:new (o)
  o = o or {}
  setmetatable(o, self)
  self.__index = self
  return o
end

EmptyBlockProperty = BlockProperty:new({
active = 0,
solid = 0,
occludes = 0,
transparent = 0
})

SolidBlockProperty = BlockProperty:new({
active = 1,
solid = 1,
occludes = 1,
transparent = 0
})

TransparentBlockProperty =  BlockProperty:new({
active = 1,
solid = 1,
occludes = 0,
transparent = 1
})

--[[
t top
b bottom
n north
s south
w west
e east
]]

---default values
BlockHud =
{
hud_pos = nil,
hud_img = nil,
}

function BlockHud:new ()
  o = {}
  setmetatable(o, self)
  self.__index = self
  return o
end

BlockHudDefault = BlockHud:new()

---function default_hud()

function hud( pos, img)
  return
  {
    hud_pos = pos,
    hud_img = img
  } 
end

NoHud = nil;

-- default value --

texture_id_index = 0
texture_id_table = { [0]="error.png" } --store texture name/id pairs

texture_name_to_id = { ["error.png"]=0}

-- return texture id --
function register_texture(tex)
  if(texture_name_to_id[tex] ) then 
    return  texture_name_to_id[id] 
  end
  texture_id_index = texture_id_index + 1;
  return texture_id_index
end

BlockTexture =
{
  t = nil,
  b = nil,
  n = nil,
  s = nil,
  w = nil,
  e = nil
}

function iso_texture(tex)
  local tex = register_texture(tex)
  o = 
  {
    t = tex,
    b = tex,
    n = tex,
    s = tex,
    w = tex,
    e = tex
  }
  return o;
end

ErrorTexture = iso_texture("error.png")

---holds everything else
Block = 
{
  id = nil,
  name = nil,
  properties = nil,
  texture = nil,
  hud = nil
}

--- register ids


block_id_table = {};  ---block id table

function Block:new(id, name)
  o = {}
  setmetatable(o, self)
  self.__index = self
  --- table set
  if( block_id_table[id] ~= nil) then print("Error: block id ", id, "taken") end
  block_id_table[id] = o;
  return o
end

function BlockNew(id, name)
  return Block:new(id, name)
end

function Block:new1(id, name, properties, texture, hud)
  o = {id=id, name = name, properties = properties, texture = texture, hud = hud}
  setmetatable(o, self)
  self.__index = self
  --- table set
  if( block_id_table[id] ~= nil) then print("Error: block id ", id, "taken") end
  block_id_table[id] = o;
  return o
end

print("block_dat: finished");



--[[

void set_cube_side_texture(int id, int side, int tex_id)

cdef extern from "./hud/cube_selector.hpp" namespace "HudCubeSelector":
    cdef cppclass CubeSelector:
        void load_cube_property(int pos, int cube_id, int tex_id)
]]