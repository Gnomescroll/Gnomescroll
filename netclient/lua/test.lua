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


--[[
    PerlinOctave3D* erosion3D;
    PerlinOctave2D* erosion2D;

    PerlinOctave2D* height2D;
    PerlinOctave2D* ridge2D;

    PerlinOctave2D* roughness2D;
]]

--cdata = ffi.new(ct [,nelem] [,init...])
--cdata = ctype([nelem,] [init...])
--ffi.copy(dst, str)

print("LuaJit: run_lua_test finished");

package.path = "media/lua/?.lua;?.lua"

erosion3D_persistance   = 0.50
erosion2D_persistance   = 0.50
height2D_persistance    = 0.50
ridge2D_persistance     = 0.50
roughness2D_persistance = 0.50

erosion3D_seed   = "test1"
erosion2D_seed   = "test2"
height2D_seed    = "test3"
ridge2D_seed     = "test4"
roughness2D_seed = "test5"


--z = ffi.new("char *")
--c = ffi.cast("uint8_t *", x)
--ffi.copy(erosion3D_seed, erosion3D_seed)

seed = ffi.new("unsigned char[128]")

ffi.copy(seed, erosion3D_seed)
ffi.C.LUA_set_noisemap_param(0, erosion3D_persistance, seed)

ffi.copy(seed, erosion2D_seed)
ffi.C.LUA_set_noisemap_param(1, erosion2D_persistance, seed)

ffi.copy(seed, height2D_seed)
ffi.C.LUA_set_noisemap_param(2, height2D_persistance, seed)

ffi.copy(seed, ridge2D_seed)
ffi.C.LUA_set_noisemap_param(3, ridge2D_persistance, seed)

ffi.copy(seed, roughness2D_seed)
ffi.C.LUA_set_noisemap_param(4, roughness2D_persistance, seed)

cache = ffi.C.LUA_get_map_lerp_array();

