
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

function NewSolidBlock(id, name)
  o = BlockNew(id, name)
  o.properties = SolidBlockProperty;
  return o
end

--register texture sheets
t00 = register_spritesheet("t00.png")
t01 = register_spritesheet("t01.png")

--alias texture
error_block = texture_alias(t00,1,1)
black_blow = texture_alias(t00,4,2)
stone_block = texture_alias(t00,1,3)





Block:new1(0, "empty", EmptyBlockProperty, iso_texture(error_block), NoHud);


--- Classic Blocks ---

b = NewSolidBlock(255, "error_block"); -- id, name
b.texture = iso_texture(error_block);
--b.hud = hud(0, error_block);

b = NewSolidBlock(1, "lava");
b.texture = iso_texture(t00,2,1);
b.hud = hud(0+0, b.texture.n);

b = NewSolidBlock(2, "sand");
b.texture = iso_texture(t00,1,4);
b.hud = hud(0+1, b.texture.n);

b = NewSolidBlock(3, "sand_brick");
b.texture = iso_texture(t00,3,3);
b.hud = hud(0+2, b.texture.n);

b = NewSolidBlock(4, "grey_brick");
b.texture = iso_texture(t00,4,3);
b.hud = hud(0+3, b.texture.n);

b = NewSolidBlock(5, "terminal_blue");
b.texture = iso_texture(t00,3,1);
b.hud = hud(8+0, b.texture.n);

b = NewSolidBlock(6, "terminal_green");
b.texture = iso_texture(t00,4,1);
b.hud = hud(8+1, b.texture.n);

b = NewSolidBlock(7, "solar_panel");
b.texture = iso_texture(t00,1,2);
b.hud = hud(8+2, b.texture.n);

b = NewSolidBlock(8, "battery");
b.texture = iso_texture(t00,3,2);
b.texture.t = register_texture(t00,2,2);
b.texture.b = register_texture(t00,4,2);
b.hud = hud(8+3, b.texture.n);
b.max_damage = 32;


-- cell blocks --


--dust
b = NewSolidBlock(16, "light_dust");
b.texture = iso_texture(t01,1,1);
b.hud = hud(16+0, b.texture.n);
b.max_damage = 1;

b = NewSolidBlock(17, "dark_dust");
b.texture = iso_texture(t01,2,1);
b.hud = hud(16+1, b.texture.n);
b.max_damage = 2;

--rock
b = NewSolidBlock(18, "soft_rock");
b.texture = iso_texture(t01,3,1);
b.hud = hud(16+2, b.texture.n);
b.max_damage = 6;

b = NewSolidBlock(19, "hard_rock");
b.texture = iso_texture(t01,4,1);
b.hud = hud(16+3, b.texture.n);
b.max_damage = 12;

b = NewSolidBlock(20, "infested_rock");
b.texture = iso_texture(t01,5,1);
b.hud = hud(16+4, b.texture.n);
b.max_damage = 10;

-- other

b = NewSolidBlock(21, "extruder");
b.texture = iso_texture(t01,1,2);
b.hud = hud(24+0, b.texture.n);

b = NewSolidBlock(22, "extruder_medical");
b.texture = iso_texture(t01,3,1);
b.hud = hud(24+1, b.texture.n);

b = NewSolidBlock(23, "holy_stone");
b.texture = iso_texture(t01,6,5);
b.hud = hud(24+2, b.texture.n);
b.max_damage = 20;

b = NewSolidBlock(24, "holy_stone_artifact");
b.texture = iso_texture(t01,1,5);
b.hud = hud(24+3, b.texture.n);

-- brick

b = NewSolidBlock(25, "alien_brick_1");
b.texture = iso_texture(t01,6,1);
b.hud = hud(40+0, b.texture.n);

b = NewSolidBlock(26, "alien_brick_2");
b.texture = iso_texture(t01,6,2);
b.hud = hud(40, b.texture.n);

-- box

b = NewSolidBlock(27, "box_0");
b.texture = iso_texture(t01,3,6);
b.hud = hud(32+0, b.texture.n);

b = NewSolidBlock(28, "box_1");
b.texture = iso_texture(t01,4,6);
b.hud = hud(32+1, b.texture.n);

b = NewSolidBlock(29, "box_2");
b.texture = iso_texture(t01,5,6);
b.hud = hud(32+2, b.texture.n);

b = NewSolidBlock(30, "box_3");
b.texture = iso_texture(t01,6,6);
b.hud = hud(32+3, b.texture.n);

-- row 5

b = NewSolidBlock(31, "t1");
b.texture = iso_texture(t01,1,7);
b.hud = hud(40+0, b.texture.n);

b = NewSolidBlock(32, "t2");
b.texture = iso_texture(t01,2,7);
b.hud = hud(40+1, b.texture.n);

b = NewSolidBlock(33, "t3");
b.texture = iso_texture(t01,3,7);
b.hud = hud(40+2, b.texture.n);

b = NewSolidBlock(34, "t4");
b.texture = iso_texture(t01,4,7);
b.hud = hud(40+3, b.texture.n);

b = NewSolidBlock(35, "t5");
b.texture = iso_texture(t01,5,7);
b.hud = hud(40+4, b.texture.n);

b = NewSolidBlock(36, "t6");
b.texture = iso_texture(t01,6,7);
b.hud = hud(40+5, b.texture.n);


-- row 6
b = NewSolidBlock(37, "at1");
b.texture = iso_texture(t01,7,1);
b.hud = hud(48+0, b.texture.n);

b = NewSolidBlock(38, "at2");
b.texture = iso_texture(t01,7,2);
b.hud = hud(48+1, b.texture.n);

b = NewSolidBlock(39, "at3");
b.texture = iso_texture(t01,7,3);
b.hud = hud(48+2, b.texture.n);

b = NewSolidBlock(40, "at4");
b.texture = iso_texture(t01,7,4);
b.hud = hud(48+3, b.texture.n);

b = NewSolidBlock(41, "at5");
b.texture = iso_texture(t01,7,5);
b.hud = hud(48+4, b.texture.n);

b = NewSolidBlock(42, "at6");
b.texture = iso_texture(t01,7,6);
b.hud = hud(48+5, b.texture.n);

b = NewSolidBlock(43, "at7");
b.texture = iso_texture(t01,7,7);
b.hud = hud(48+6, b.texture.n);

-- row 7

b = NewSolidBlock(44, "bt1");
b.texture = iso_texture(t01,1,8);
b.hud = hud(56+0, b.texture.n);

b = NewSolidBlock(45, "bt2");
b.texture = iso_texture(t01,2,8);
b.hud = hud(56+1, b.texture.n);

b = NewSolidBlock(46, "bt3");
b.texture = iso_texture(t01,3,8);
b.hud = hud(56+2, b.texture.n);

b = NewSolidBlock(47, "bt4");
b.texture = iso_texture(t01,4,8);
b.hud = hud(56+3, b.texture.n);

b = NewSolidBlock(48, "bt5");
b.texture = iso_texture(t01,5,8);
b.hud = hud(56+4, b.texture.n);

b = NewSolidBlock(49, "bt6");
b.texture = iso_texture(t01,6,8);
b.hud = hud(56+5, b.texture.n);

b = NewSolidBlock(50, "bt7");
b.texture = iso_texture(t01,7,8);
b.hud = hud(56+6, b.texture.n);



--b = NewSolidBlock(51, "mars0");
--b.texture = iso_texture(t01,1,9);
--b.hud = hud(56-16+7, b.texture.n);

b = NewSolidBlock(52, "mars1");
b.texture = iso_texture(t01,2,9);
b.hud = hud(56-8+7, b.texture.n);

b = NewSolidBlock(53, "mars2");
b.texture = iso_texture(t01,3,9);
b.hud = hud(56+7, b.texture.n);


b = NewSolidBlock(54, "srs1");
b.texture = iso_texture(t01,4,10);
b.texture.t = register_texture(t01,3,10);
b.texture.b = register_texture(t01,5,10);
b.hud = hud(56-16+7, b.texture.n);

print("map_load_tiles: done");

--[[
for i, block in pairs(block_id_table) do
  o = {}
end
]]
