

package.path = "lua/?.lua;lua/block/?.lua;?.lua"

require("lua_library");

print("LuaJit: run_lua_test finished");

if( options_table == nil) then
    print "WTF"
end

print(table.val_to_str(options_table))




options = {}


-- server name --
options.server_name = "server_one"

-- server --
options.ip_address = "0.0.0.0"
options.port = 0

-- game settings --
options.game_mode = 'ctf'
options.team_kills = false
options.victory_points = 3
options.team_name_one = "Green Team"
options.team_name_two = "Red Team"

-- map --
options.map = 'natural_terrain'
options.seed = 800751

-- logger --
options.logger = true

ffi = require("ffi")
ffi.cdef[[
    void LUA_set_int_option(int option_id, int value);
    void LUA_set_uint_option(int option_id, unsigned int value);
    void LUA_set_bool_option(int option_id, int value);
    void LUA_set_float_option(int option_id, float value);
    void LUA_set_string_option(int option_id, char* value);
]]

for key,value in pairs(options) do 
    if(options_table[key] ~= nil) then

        local id = options_table[key].id
        if( options_table[key].type == "uint") then
            ffi.C.LUA_set_uint_option(id, options[key]);
        elseif( options_table[key].type == "int") then
            ffi.C.LUA_set_int_option(id, options[key]);
        elseif( options_table[key].type == "bool") then
            ffi.C.LUA_set_bool_option(id, options[key]);
        elseif( options_table[key].type == "float") then
            ffi.C.LUA_set_float_option(id, options[key]);
        elseif( options_table[key].type == "string") then
            local str = ffi.new("char[64]");
            ffi.copy(str, options[key]);
            ffi.C.LUA_set_string_option(id, str);
        else
            print("Set Options Error: type error"); 
        end
    else
        print("Set Options Error: option " .. key .. " does not exist ");
    end
end
