--- LUA_PATH = "./:./media/lua/"




print("map_load_tiles: loading tiles");



--- convenience method ---
function NewSolidBlock(id, name)
  o = BlockNew(id, name)
  o.properties = SolidBlockProperty;
  return o
end


Block:new1(0, "empty", EmptyBlockProperty, ErrorTexture, NoHud);

b = NewSolidBlock(1, "error_block"); -- id, name
b.texture = iso_texture("error.png");
b.hud = hud(63, "error.png");

b = NewSolidBlock(2, "stone");
b.texture = iso_texture("stone.png");
b.hud = hud(0, "stone.png");

b = NewSolidBlock(3, "blue_ore");
b.texture = iso_texture("blueore.png");
b.hud = hud(1, "blueore.png");

print("map_load_tiles: done");


for i, block in pairs(block_id_table) do
  o = {}
end