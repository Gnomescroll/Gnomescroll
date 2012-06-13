-- Test FFI
local ffi = require("ffi")

ffi.cdef[[
   void LUA_set_noisemap_param(int noise_map, float persistance, unsigned char* seed_string);
   float* LUA_get_noisemap_map_cache(int noise_map);
   float* LUA_get_map_lerp_array();
   void LUA_generate_map();
]]


--local barreturn = ffi.C.barfunc(253)

---io.write(barreturn)
---io.write('\n')

print("LuaJit: run_lua_test finished");

package.path = "media/lua/?.lua;?.lua"
