
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


BlockTexture =
{
hud     0 stone.png
t = stone.png
b = stone.png
n = stone.png
s = stone.png
w = stone.png
e = stone.png
}

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
    
