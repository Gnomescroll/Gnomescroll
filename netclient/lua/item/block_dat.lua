
--print("map_load_tiles: loading tiles") ;


--[[
t top
b bottom
n north
s south
w west
e east
]]


if (IS_CLIENT == 0) then 
    function texture_alias()
        return nil
    end

    function register_spritesheet()
        return nil
    end
    function register_texture()
        return nil
    end
end

--- convenience method

function iso_texture(spritesheet, xpos, ypos)
    if(IS_CLIENT == 0) then return {t = nil, b = nil, n = nil, s = nil, w = nil, e = nil} end;
    local id;
    if (type(spritesheet) == "number" and xpos == nil and ypos == nil) then
        id = spritesheet
    else
        if(not xpos or not ypos) then error("iso_texture: xpos or ypos not set!") end
        id = register_texture(spritesheet, xpos, ypos)
    end
    return  { t = id, b = id, n = id, s = id, w = id, e = id };
end

--function NewSolidBlock(id, name)
  --o = BlockNew(id, name)
  --o.properties = SolidBlockProperty;
  --return o
--end

--register texture sheets
i00 = register_spritesheet("i00.png")

--alias texture
--error_block = texture_alias(t00,1,1)
--black_blow = texture_alias(t00,4,2)
--stone_block = texture_alias(t00,1,3)




Block:new1(0, "empty", EmptyBlockProperty, iso_texture(error_block), NoHud);
b = NewSolidBlock(53, "mars2");
b.texture = iso_texture(t01,3,9);
b.hud = hud(56+7, b.texture.n);


print("map_load_tiles: done");

--[[
for i, block in pairs(block_id_table) do
  o = {}
end
]]
