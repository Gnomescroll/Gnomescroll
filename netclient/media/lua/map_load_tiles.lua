
print("map_load_tiles: loading tiles");

--[[
function Tile:new (o)
  o = o or {}
  setmetatable(o, self)
  self.__index = self
  return o
end
]]


BlockProperties = {
active = 0,
solid = 0,
occludes = 0,
transparent = 0
}

--- falls through if property is not set
function BlockProperties:new (o)
  o = o or {}
  setmetatable(o, self)
  self.__index = self
  return o
end

EmptyBlockProperties = Tile:new({
active = 0,
solid = 0,
occludes = 0,
transparent = 0
})

DefaultBlockProperties = Tile:new({
active = 1,
solid = 1,
occludes = 1,
transparent = 0
})

TransparentBlockProperties = Tile:new({
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

---default valuessssss
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

BlockHudDefault = BlockHud.new()

function default_hud()
---default value
BlockTexture =
{
  t = nil,
  b = nil,
  n = nil,
  s = nil,
  w = nil,
  e = nil
}

function iso_texture(tex) =
{
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
}


---holds everything else
Block = 
{
  name = nil,
  properties = nil,
  texture = nil,
  hud = nil
}

--- register ids


block_id_table = {};

function Block:new(id, name, properties, texture, hud)
  o = {name = name, properties = properties, texture = texture, hud = hud}
  setmetatable(o, self)
  self.__index = self
  --- table set
  if( block_id_table[id] != nil)
    print "Error: id taken"
  end
  block_id_table[id] = o;
  return o
end


function Block:iso_texture(tex)
  self.texture = 
  {
    t = tex,
    b = tex,
    n = tex,
    s = tex,
    w = tex,
    e = tex
  }
end

--[[
Set 
]]

id      1
name    test
hud     63 error.png
top     error.png
bottom  error.png
north   error.png
south   error.png
west    error.png
east    error.png


print("map_load_tiles: done");

--- usage: Tile:new()

--[[
function Tile.new (u)
  Account.balance = Account.balance - v
end

function Tile.withdraw (v)
  Account.balance = Account.balance - v
end
]]
    
