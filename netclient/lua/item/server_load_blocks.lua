--package.path = "lua/?.lua;lua/block/?.lua;?.lua"

--require("lua_library");

--IS_CLIENT = 0;

--require("block_template");
--require("block_dat");

----[[ Setup FFI Interface ]]

--local ffi = require("ffi")
--ffi.cdef[[
--void LUA_set_block_properties(int id, int active, int solid, int occludes, int transparent);
--void LUA_set_block_max_damage(int id, int max_damage);

--void LUA_set_block_name(int id, char* name, int length);
--]]


--function set_block_name(id, name)
  ----print("block_name: " .. id .. " = " .. name);
  --local str = ffi.new("char[64]");
  --ffi.copy(str, name);
  --ffi.C.LUA_set_block_name(id, str, string.len(name) );
--end

----- set block properties

--for id, block in pairs(block_id_table) do
    --p = block.properties
    --ffi.C.LUA_set_block_properties(id, p.active, p.solid, p.occludes, p.transparent )
--end

--for id, block in pairs(block_id_table) do
    --ffi.C.LUA_set_block_max_damage(id, block.max_damage)
--end

--for id, block in pairs(block_id_table) do
    --set_block_name(id, block.name)
--end

