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

--[[
local erosion3D   = ffi.new("float*")
local erosion2D   = ffi.new("float*")
local height2D    = ffi.new("float*")
local ridge2D     = ffi.new("float*")
local roughness2D = ffi.new("float*")
]]

erosion3D   = ffi.C.LUA_get_noisemap_map_cache(0)
erosion2D   = ffi.C.LUA_get_noisemap_map_cache(1)
height2D    = ffi.C.LUA_get_noisemap_map_cache(2)
ridge2D     = ffi.C.LUA_get_noisemap_map_cache(3)
roughness2D = ffi.C.LUA_get_noisemap_map_cache(4)

cache = ffi.C.LUA_get_map_lerp_array();


XMAX = 128;
XYMAX = 128*128;

hmin = 64;

hrange = 4.0;   --half of range (can perturb this with another map)

_hmin = -1.0;
_hmax = 1.0;
_hmix = 0.01; --0.125; height mix

rmix = 0.8;	--range mix


--[[
local e3 = ffi.new("float")
local e2 = ffi.new("float")
local ri2 = ffi.new("float")
local h2 = ffi.new("float")
local r2 = ffi.new("float")
]]

for k=0,15  do
for i=0,127 do 
for j=0,127 do 

	local v = 0.0

	--print( i , j, k);

	x = 4.0*i
	y = 4.0*j
	z = 8.0*k

    local index2 = j*XMAX + i;
    local index3 = (k*XMAX*XMAX) + (j*XMAX) + i;


    e3 = erosion3D[index3];
    e2 = erosion2D[index2];
    ri2 = ridge2D[index2];
    h2 = height2D[index2];
    r2 = roughness2D[index2];

    --e3 = .5

    --if(ri2 < 0) then ri2 = r1*-1 end;
    --ri2 *= .20;
	--ri2 *= 40;

    ri2 = math.abs(ri2);
    ri2 = 40*0.20*math.floor(ri2 * 5);

    if( z < hmin + ri2) then v = v-0.25 end;

    v = v + 0.40*e3*e3;

    if(v < -1) then v = -1 end
    if(v >  1) then v = 1 end

    if(r2 < 0.125) then r2 = 0.0125 end; 

    tmp1 = _hmix*(z - (hmin + r2*h2*hrange) );

    if(tmp1 < _hmin) then tmp1 = _hmin end;
    if(tmp1 > _hmax) then tmp1 = _hmax end;

    v = v + tmp1;



    cache[index3] = v

--[[

    //r2 = 5*r2 -2;
    //if(r2 > 1) r2 = 1.0;
    //if(r2 < 0.50) r2 = 0.50;

    if(r2 < 0.125) r2 = 0.0125; 
    float tmp1 = _hmix*(z - (hmin + r2*h2*hrange) );

    if(tmp1 < _hmin) tmp1 = _hmin;
    if(tmp1 > _hmax) tmp1 = _hmax;
]]

end
end
end


ffi.C.LUA_generate_map();
