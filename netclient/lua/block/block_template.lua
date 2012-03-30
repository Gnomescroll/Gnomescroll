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

---default values
BlockHud =
{
pos = nil,
tex = 0,
}

function BlockHud:new ()
  o = {}
  setmetatable(o, self)
  self.__index = self
  return o
end

BlockHudDefault = BlockHud:new()

---function default_hud()
if( IS_CLIENT ~= 1 ) then
  function hud( pos, img)
    return nil
  end
end

NoHud = BlockHudDefault;

---holds everything else

default_max_damage = 32;

Block = 
{
  id = nil,
  name = nil,
  properties = nil,
  max_damage = default_max_damage, --default max damage
  texture = nil,
  hud = BlockHudDefault
}

--- register ids

block_id_table = {};  ---block id table

function Block:new(id, name)
  o = { max_damage = default_max_damage }
  o.name = name;
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
  o = {id=id, name = name, properties = properties, texture = texture, hud = hud, max_damage = default_max_damage}
  setmetatable(o, self)
  self.__index = self
  --- table set
  if( block_id_table[id] ~= nil) then print("Error: block id ", id, "taken") end
  block_id_table[id] = o;
  return o
end
